#pragma once

#include <cstddef>

namespace dumpable
{
    template <typename T>
    class dptr
    {
        private:
            std::ptrdiff_t diff_;
        public:
            dptr() : diff_(-(intptr_t)(char*)this) {}
            dptr(const dptr<T>& rhs) : diff_((char*)&*rhs - (char*)this) {}
            T& operator* ()
            {
                return *(T*)((char*)this + diff_);
            }
            T* operator-> ()
            {
                return (T*)((char*)this + diff_);
            }
            operator T* ()
            {
                return (T*)((char*)this + diff_);
            }
            dptr& operator = (T* x)
            {
                diff_ = (char*)x - (char*)this;
                return *this;
            }
            dptr& operator = (const dptr<T>& x)
            {
                diff_ = (char*)&*x - (char*)this;
                return *this;
            }
    };
}
