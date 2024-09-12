#include <iostream>
#include <concepts>
#include <mutex>
#include <thread>

template <typename T>
class allocator {
public:
    allocator() = default;

    T* allocate(size_t element_count) {
        return reinterpret_cast<T*>(new uint8_t[element_count * sizeof(T)]);
    }

    void deallocate(T* ptr) {
        delete[] reinterpret_cast<uint8_t*>(ptr);
    }

    void construct(T* ptr) {
        new (ptr) T();
    }

    void destroy(T* ptr) {
        ptr->~T();
    }
};

 template <typename T, typename Alloc = allocator<T>>
     class stack {
     private:
         T* buf;
         int _top;
         size_t capacity;
         Alloc allocator;
     public:
         stack() {
            buf = allocator.allocate(10);
            capacity = 0;
            _top = -1;
         }
         stack(size_t size) {
             buf = allocator.allocate(size);
             capacity = size;
             _top = -1;
         }
         ~stack() {
             allocator.deallocate(buf);
         }
         stack(const stack<T>& other) {
             capacity = other.capacity;
             _top = other._top;
             buf = allocator.allocate(other.capacity);
            for(int i = 0; i < capacity; ++i) {
                 buf[i] = other.buf[i];
            }
         }
         stack(const stack<T>&& other) {
             capacity = other.capacity;
             _top = other._top;
             buf = other.buf;
            other.buf = nullptr;
        }
     public:
         T top() {
            allocator.destroy(buf + _top);
             return buf[_top--];
         }
         void push(T value) {
             if(isFull()) {
                 T* buf_copy = allocator.allocate(capacity*2);
                 for(int i = 0; i < capacity; ++i) {
                     buf_copy[i] = buf[i];
                 }
                 capacity*=2;
                 allocator.deallocate(buf);
                 buf = buf_copy;
             }
             allocator.construct(buf + (++_top));
             buf[_top] = value;
         }
         size_t size() {
             return _top + 1;
         }
         bool isEmpty() {
             return _top == -1;
         }
         bool isFull() {
             return _top + 1 == capacity;
         }
     };

     template <typename T, typename MutexType = std::mutex>
     class ThreadSafeStack {
     private:
         MutexType m_;
          stack<T> stack_;
     public:
         ThreadSafeStack() {}
     public:
         void push(T value) {
             std::unique_lock<MutexType> lock(m_);
             stack_.push(value);
         }
         T top() {
             std::unique_lock<MutexType> lock(m_);
             return stack_.top();
         }
         size_t size() {
             std::unique_lock<MutexType> lock(m_);
             return stack_.size();
         }
         bool isEmpty() {
             std::unique_lock<MutexType> lock(m_);
             return stack_.isEmpty();
         }
         bool isFull() {
             std::unique_lock<MutexType> lock(m_);
             return stack_.isFull();
         }
     };
 
int main() {
    ThreadSafeStack<int> t;
    std::thread t1 {
        [&] () {
            for(int i = 0; i < 7; ++i) {
                t.push(i+1);
            }
        }
    };
    std::thread t2 {
        [&] () {
            for(int i = 0; i < 7; ++i) {
                std::cout << t.top() << std::endl;
            }
        }
    };
    if(t1.joinable())
    t1.join();
    if(t2.joinable())
    t2.join();
}