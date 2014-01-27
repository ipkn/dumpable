// Copyright (c) 2014 ipkn.
// Licensed under the MIT license.

#pragma once

#include "dptr.h"
#include <vector>
#include <cassert>

namespace dumpable
{
    namespace detail
    {
        template <typename T>
        T find_power_of_2_greater_than(T n)
        {
            n += (n==0);
            n--;
            n|=n>>1;
            n|=n>>2;
            n|=n>>4;
            n|=n>>8;
            n|=n>>16;
            if (sizeof(T) > 4)
                n|=n>>32;
            n++;
            if (n < 8)
                return 8;
            return n;
        }
    }
    template <typename T>
    class dvector : protected dptr<T>
    {
        public:
            typedef T value_type;
            typedef size_t size_type;
            typedef std::ptrdiff_t difference_type;
            typedef T& reference;
            typedef const T& const_reference;
        protected:
            void assign(const T* begin, size_type size)
            {
                if (dumpable::detail::dptr_alloc)
                {
                    isPooled_ = true;
                    size_ = size;
                    void* buf = dptr<T>::alloc_internal(size * sizeof(T));
                    std::copy(begin, begin+size, (T*)buf);
                }
                else
                {
                    isPooled_ = false;
                    size_ = size;
                    size_type capacity = detail::find_power_of_2_greater_than(size);
                    dptr<T>::operator =(new T[capacity]);
                    std::copy(begin, begin+size, (T*)*this);
                }
            }

            void uninitialized_resize(size_type newSize)
            {
                assert(!dumpable::detail::dptr_alloc);
                size_type oldCapacity = detail::find_power_of_2_greater_than(size());
                if (isPooled_)
                    oldCapacity = size_;

                size_type newCapacity = detail::find_power_of_2_greater_than(newSize);
                if (oldCapacity != newCapacity)
                {
                    T* newBuffer = new T[newCapacity];
                    T* oldBuffer = (T*)*this;
                    for(size_type i = 0; i < std::min(size_, newCapacity); i ++)
                    {
                        new (newBuffer+i) T(std::move(*(oldBuffer+i)));
                        (oldBuffer+i)->~T();
                    }
                    if (!isPooled_)
                        delete[](oldBuffer);
                    dptr<T>::operator =(newBuffer);
                    isPooled_ = false;
                }
                size_ = newSize;
            }
        public:
            dvector() : size_(0), isPooled_(false) {}
            dvector(const std::vector<T>& v)
            { 
                assign(v.data(), v.size());
            }

            dvector(const dvector<T>& v)
            { 
                assign(v.data(), v.size());
            }

            template <typename Iter>
            dvector(Iter first, Iter last)
            {
                std::vector<T> v(first, last);
                assign(v.data(), v.size());
            }

            dvector(dvector<T>&& v) noexcept
                : dptr<T>(std::move(v)), size_(v.size_), isPooled_(v.isPooled_)
            {
                v.size_ = 0;
                v.isPooled_ = false;
            }

            ~dvector()
            {
                clear();
            }
        public:
            void clear()
            {
                T* begin = (T*)*this;
                if (!isPooled_ && begin)
                {
                    delete[] begin;
                }
                dptr<T>::operator =(nullptr);
                size_ = 0;
                isPooled_ = false;
            }

            typedef T* iterator;
            typedef const T* const_iterator;

            T* data() const { return (T*)*this; }
            iterator begin() const { return (T*)*this; }
            iterator end() const { return begin() + size_; }
            const_iterator cbegin() const { return begin(); }
            const_iterator cend() const { return end(); }
            size_type size() const { return size_; }
            bool empty() const { return !size_; }

            T& operator[](int index) { return *(begin() + index); }
            const T& operator[](int index) const { return *(begin() + index); }
            T& at(int index) { return *(begin() + index); }
            const T& at(int index) const { return *(begin() + index); }

            const T& front() const { return *begin(); }
            const T& back() const { return *(end()-1); }
            T& front() { return *begin(); }
            T& back() { return *(end()-1); }

            void reserve()
            {
                // do nothing
            }

            void resize(size_type newSize)
            {
                size_type oldSize = size();
                uninitialized_resize(newSize);
                T* buffer = (T*)*this;
                for(size_type i = oldSize; i < newSize; i ++)
                {
                    new (buffer+i) T();
                }
            }


            void push_back(const T& value)
            {
                uninitialized_resize(size()+1);
                T* buffer = (T*)*this;
                new (buffer+size_-1) T(value);
            }

            void push_back(T&& value)
            {
                uninitialized_resize(size()+1);
                T* buffer = (T*)*this;
                new (buffer+size_-1) T(std::move(value));
            }

            dvector<T>& operator = (const std::vector<T>& v)
            {
                clear();
                assign(v.data(), v.size());
                return *this;
            }
            dvector<T>& operator = (const dvector<T>& v)
            {
                if (&v == this)
                    return *this;
                clear();
                assign(v.data(), v.size());
                return *this;
            }

            dvector<T>& operator = (dvector<T>&& v) noexcept
            {
                if (this == &v)
                    return *this;
                size_ = v.size_;
                isPooled_ = v.isPooled_;

                dptr<T>::operator =(std::move(v));

                v.size_ = 0;
                v.isPooled_ = false;

                return *this;
            }


        private:
            size_type size_;
            char isPooled_;
    };
}
