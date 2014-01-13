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
        protected:
            void* alloc_internal(size_t size)
            {
                void* ret;
                std::ptrdiff_t offset;
                std::tie(ret, offset) = detail::dptr_alloc(this, size);
                diff_ = offset;
                return ret;
            }
        public:
            dptr() : diff_(0) {}
            dptr(const dptr<T>& rhs) : diff_((char*)&*rhs - (char*)this) {}
            T& operator* () const
            {
                if (diff_ == 0)
                    return *(T*)nullptr;
                return *(T*)((char*)this + diff_);
            }
            T* operator-> () const
            {
                if (diff_ == 0)
                    return (T*)nullptr;
                return (T*)((char*)this + diff_);
            }
            operator T* () const
            {
                if (diff_ == 0)
                    return (T*)nullptr;
                return (T*)((char*)this + diff_);
            }
            dptr& operator = (T* x)
            {
                if (x == nullptr)
                    diff_ = 0;
                else if (detail::dptr_alloc)
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
    };
}
