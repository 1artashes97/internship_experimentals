#pragma once
#include <iostream>
#include <shared_mutex>
#include "shared_mutex.hpp"

namespace il {
    template <typename MutexType = shared_mutex>
    class shared_lock {
    private:
        MutexType* m_ = nullptr;
    public:
        shared_lock(MutexType& m) {
           m_ = m;
           assert(m_ != nullptr);
           m_->lock_shared(); 
        }
        ~shared_lock() {
            m_->unlock_shared();
        }
        shared_lock&& operator=(shared_lock&& other) {
            m_(other.m_);
            other.m_ = nullptr;
            return std::move(*this);
        }
        shared_lock(const shared_lock& other) = delete;
        shared_lock& operator=(const shared_lock& other) = delete;
        shared_lock(shared_lock&& other) = delete;
    public:
        void lock() {
            assert(nullptr != m_);
            m_->lock();
        }
        void unlock() {
            assert(nullptr != m_);
            m_->unlock();
        }
    };
}