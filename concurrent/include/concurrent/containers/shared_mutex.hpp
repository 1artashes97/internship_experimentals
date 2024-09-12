#pragma once
#include <thread>
#include <iostream>
#include <mutex>
#include <atomic>
#include <shared_mutex>

namespace il {
    class shared_mutex {
    private:
        std::atomic<int> count_{0};
    public:
        shared_mutex (shared_mutex&& other) = delete;
        shared_mutex&& operator=(shared_mutex&& other) = delete;
        shared_mutex(const shared_mutex& other) = delete;
        shared_mutex& operator=(const shared_mutex& other) = delete;
        shared_mutex() = default;
    public: // Exclusive Locking
        void lock() {
          int count_to_compare = 0;
            while(!count_.compare_exchange_weak(count_to_compare, -1)) {
                count_to_compare = 0;
            }
        }
        // bool try_lock() {
           
        // }
        void unlock() {
            count_.store(0);
        }
    public: // Shared Locking
        void lock_shared() {
            while(true) {
                int count_of_shared_thread = count_.load();
                if(count_of_shared_thread >= 0) {
                    if(count_.compare_exchange_weak(count_of_shared_thread, count_of_shared_thread + 1)) {
                        return;
                    }
                }
            }
        }
        // bool try_lock_shared() { //...
        // }
        void unlock_shared() {
            count_.store(count_ - 1);
        }
    };
}

                              // -- Testing -- //
// class ThreadSafeCounter {
//  public:
//   ThreadSafeCounter() = default;
 
//   // Multiple threads/readers can read the counter's value at the same time.
//   unsigned int get() const {
//     std::shared_lock lock(mutex_);
//     return value_;
//   }
 
//   // Only one thread/writer can increment/write the counter's value.
//   unsigned int increment() {
//     std::unique_lock lock(mutex_);
//     return ++value_;
//   }
 
//   // Only one thread/writer can reset/write the counter's value.
//   void reset() {
//     std::unique_lock lock(mutex_);
//     value_ = 0;
//   }
 
//  private:
//   mutable il::shared_mutex mutex_;
//   unsigned int value_ = 0;
// };
 
// int main() {
//   ThreadSafeCounter counter;
 
//   auto increment_and_print = [&counter]() {
//     for (int i = 0; i < 3; i++) {
//       std::cout << std::this_thread::get_id() << ' ' << counter.increment() << '\n';
 
//       // Note: Writing to std::cout actually needs to be synchronized as well
//       // by another std::mutex. This has been omitted to keep the example small.
//     }
//   };
 
//   std::thread thread1(increment_and_print);
//   std::thread thread2(increment_and_print);
 
//   thread1.join();
//   thread2.join();
// }