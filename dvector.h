#pragma once

#include <vector>

namespace dumpable
{
    template <typename T>
    class dvector : private dptr<T>
    {
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
            assign(v.begin(), v.size());
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
        }

        T* begin() const { return (T*)*this; }
        T* data() const { return (T*)*this; }
        T* end() const { return begin() + size_; }
        size_t size() const { return size_; }
        bool empty() const { return !size_; }

        T& operator[](size_t index) const { return *(begin() + index); }
        T& front() const { return *begin(); }
        T& back() const { return *(end()-1); }
        dvector<T>& operator = (const std::vector<T>& v)
        {
            clear();
            assign(v.data(), v.size());
        }
        dvector<T>& operator = (const dvector<T>& v)
        {
            clear();
            assign(v.data(), v.size());
        }

    private:
        size_t size_;
        char isPooled_;
    };
}
