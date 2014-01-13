#pragma once

#include <cstddef>
#include <functional>

namespace dumpable
{
    namespace detail
    {
        std::function<std::pair<void*, std::ptrdiff_t>(void* self, size_t size)> dptr_alloc;
    }

    template <typename T>
    class dptr
    {
        private:
            std::ptrdiff_t diff_;
        public:
            dptr() : diff_(-(intptr_t)(char*)this) {}
            dptr(const dptr<T>& rhs) : diff_((char*)&*rhs - (char*)this) {}
            T& operator* () const
            {
                return *(T*)((char*)this + diff_);
            }
            T* operator-> () const
            {
                return (T*)((char*)this + diff_);
            }
            operator T* () const
            {
                return (T*)((char*)this + diff_);
            }
            dptr& operator = (T* x)
            {
                if (x && detail::dptr_alloc)
                {
                    void* y;
                    std::ptrdiff_t offset;
                    std::tie(y, offset) = detail::dptr_alloc(this, sizeof(T));
                    *(T*)y = *x;
                    diff_ = offset;
                }
                else
                    diff_ = (char*)x - (char*)this;
                return *this;
            }
            dptr& operator = (const dptr<T>& dptr_x)
            {
                T* x = &*dptr_x;
                return (*this = x);
            }
    };
}
