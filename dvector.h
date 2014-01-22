#pragma once

#include "dptr.h"
#include <vector>

namespace dumpable
{
    template <typename T>
    class dvector : protected dptr<T>
    {
        protected:
            void assign(const T* begin, size_t size)
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
                    dptr<T>::operator =(new T[size]);
                    std::copy(begin, begin+size, (T*)*this);
                }
            }
        public:
            dvector() : isPooled_(false), size_(0) {}
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
                : size_(v.size_), isPooled_(v.isPooled_), dptr<T>(std::move(v))
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

            T* begin() const { return (T*)*this; }
            T* data() const { return (T*)*this; }
            T* end() const { return begin() + size_; }
            size_t size() const { return size_; }
            bool empty() const { return !size_; }

            T& operator[](int index) const { return *(begin() + index); }
            T& front() const { return *begin(); }
            T& back() const { return *(end()-1); }

            dvector<T>& operator = (const std::vector<T>& v)
            {
                clear();
                assign(v.data(), v.size());
                return *this;
            }
            dvector<T>& operator = (const dvector<T>& v)
            {
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
            size_t size_;
            char isPooled_;
    };
}
