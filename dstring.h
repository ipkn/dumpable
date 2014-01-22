#pragma once

#include "dptr.h"
#include <string>
#include <cstring>
#include <iostream>

namespace dumpable
{
    template <typename T, typename Traits = std::char_traits<T>>
    class dbasic_string : protected dptr<T>
    {
        protected:
            void assign(const T* begin, size_t size)
            {
                if (dumpable::detail::dptr_alloc)
                {
                    isPooled_ = true;
                    size_ = size;
                    void* buf = dptr<T>::alloc_internal((size+1) * sizeof(T));
                    Traits::copy((T*)buf, begin, size+1);
                }
                else
                {
                    isPooled_ = false;
                    size_ = size;
                    dptr<T>::operator =(new T[size+1]);
                    Traits::copy((T*)*this, begin, size+1);
                }
            }
        public:
            dbasic_string() {}
            dbasic_string(const T* str)
            {
                size_t length = Traits::length(str);
                assign(str, length);
            }
            dbasic_string(const std::basic_string<T, Traits>& s)
            {
                assign(s.c_str(), s.size());
            }
            dbasic_string(const dbasic_string<T, Traits>& s)
            {
                assign(s.c_str(), s.size());
            }
            dbasic_string(dbasic_string<T, Traits>&& s) noexcept
                : size_(s.size_), isPooled_(s.isPooled_), dptr<T>(std::move(s))
                {
                    s.size_ = 0;
                    s.isPooled_ = false;
                }
            ~dbasic_string()
            {
                clear();
            }

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
            T* begin() const noexcept { return (T*)*this; }
            T* end() const noexcept { return begin() + size(); }

            const T* c_str() const noexcept
            {
                return data();
            }

            const T* data() const noexcept  
            { 
                if (empty())
                    return (T*)"\x00\x00\x00\x00";
                return (T*)*this; 
            }

            T& operator[](int index) const noexcept { return *(begin() + index); }
            size_t size() const noexcept { return size_; }
            bool empty() const noexcept { return !size_; }
            T& front() const noexcept { return *begin(); }
            T& back() const noexcept { return *(end()-1); }

            friend std::ostream& operator << (std::ostream& os, const dbasic_string<T, Traits>& str)
            {
                os << str.c_str(); 
            }

            friend bool operator == (const dbasic_string<T, Traits>& a, const dbasic_string<T, Traits>& b)
            {
                if (a.size() != b.size())
                    return false;
                if (a.c_str() == b.c_str())
                    return true;
                return !Traits::compare(a.c_str(), b.c_str(), a.size()+1);
            }

            friend bool operator != (const dbasic_string<T, Traits>& a, const dbasic_string<T, Traits>& b)
            {
                return !(a==b);
            }

            friend bool operator == (const dbasic_string<T, Traits>& a, const T* b)
            {
                size_t length = Traits::length(b);
                if (length != a.size())
                    return false;
                return !Traits::compare(b, a.c_str(), a.size()+1);
            }

            friend bool operator != (const dbasic_string<T, Traits>& a, const T* b)
            {
                return !(a==b);
            }

            friend bool operator == (const T* a, const dbasic_string<T, Traits>& b)
            {
                size_t length = Traits::length(a);
                if (length != b.size())
                    return false;
                return !Traits::compare(a, b.c_str(), b.size()+1);
            }

            friend bool operator != (const T* a, const dbasic_string<T, Traits>& b)
            {
                return !(a==b);
            }

        public:
            dbasic_string<T, Traits>& operator = (const T* str)
            {
                clear();
                size_t length = Traits::length(str);
                assign(str, length);
            }
            dbasic_string<T, Traits>& operator = (const std::basic_string<T, Traits>& s)
            {
                clear();
                assign(s.c_str(), s.size());
                return *this;
            }
            dbasic_string<T, Traits>& operator = (const dbasic_string<T, Traits>& s)
            {
                clear();
                assign(s.c_str(), s.size());
                return *this;
            }
            dbasic_string<T, Traits>& operator = (dbasic_string<T, Traits>&& s)
            {
                if (&s == this)
                    return *this;
                size_ = s.size_;
                isPooled_ = s.isPooled_;

                dptr<T>::operator =(std::move(s));

                s.size_ = 0;
                s.isPooled_ = false;

                return *this;
            }
        private:
            size_t size_;
            char isPooled_;

    };

    typedef dbasic_string<char> dstring;
    typedef dbasic_string<wchar_t> dwstring;
}
