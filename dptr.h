// Copyright (c) 2014 ipkn.
// Licensed under the MIT license.

#pragma once

#include "dumpableconf.h"

#include <cstddef>
#include <functional>
#include <tuple>

namespace dumpable
{
    namespace detail
    {
        std::function<std::pair<void*, dumpable::ptrdiff_t>(void* self, dumpable::size_t size)>& dptr_alloc()
        { 
            static std::function<std::pair<void*, dumpable::ptrdiff_t>(void* self, dumpable::size_t size)> allocFunc;
            return allocFunc;
        }
        bool dumpable_is_custom_alloc()
        {
            return !!dptr_alloc();
        }
    }

    template <typename T>
    class dptr
    {
        private:
            dumpable::ptrdiff_t diff_;
        protected:
            void* alloc_internal(dumpable::size_t size)
            {
                void* ret;
                dumpable::ptrdiff_t offset;
                std::tie(ret, offset) = detail::dptr_alloc()(this, size);
                diff_ = offset;
                return ret;
            }
        public:
            dptr() : diff_(0) {}
            dptr(const dptr<T>& rhs) 
            {
                diff_ = (char*)&*rhs - (char*)this; 
            }
            dptr(dptr<T>&& rhs) noexcept 
            {
                diff_ = (char*)&*rhs - (char*)this;
                rhs = nullptr;
            }
            T& operator* () const noexcept
            {
                if (diff_ == 0)
                    return *(T*)nullptr;
                return *(T*)((char*)this + diff_);
            }
            T* operator-> () const noexcept
            {
                if (diff_ == 0)
                    return (T*)nullptr;
                return (T*)((char*)this + diff_);
            }
            operator T* () const noexcept
            {
                if (diff_ == 0)
                    return (T*)nullptr;
                return (T*)((char*)this + diff_);
            }
            dptr& operator = (T* x)
            {
                if (x == nullptr)
                    diff_ = 0;
                else if (detail::dptr_alloc())
                {
                    void* ret = alloc_internal(sizeof(T));
                    *(T*)ret = *x;
                }
                else
                    diff_ = (char*)x - (char*)this;
                return *this;
            }
            dptr& operator = (const dptr<T>& dptr_x)
            {
                if (&dptr_x == this)
                    return *this;
                T* x = &*dptr_x;
                return (*this = x);
            }
            dptr& operator = (dptr<T>&& dptr_x) noexcept
            {
                if (&dptr_x == this)
                    return *this;
                T* x = &*dptr_x;
                dptr_x = nullptr;
                return (*this = x);
            }
    };
}
