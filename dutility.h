#pragma once

#include <memory>
#include "dptr.h"

namespace dumpable
{
    template <typename T>
    struct not_dump : public T
    {
        not_dump<T>& operator = (T&& x)
        {
            if (detail::dumpable_is_custom_alloc())
                T::operator=(T());
            else 
                T::operator=(std::move(x));
            return *this;
        }

        not_dump<T>& operator = (const T& x)
        {
            if (detail::dumpable_is_custom_alloc())
                T::operator=(T());
            else 
                T::operator=(x);
            return *this;
        }

        not_dump<T>& operator = (not_dump<T>&& x)
        {
            if (detail::dumpable_is_custom_alloc())
                T::operator=(T());
            else 
                T::operator=(std::move(x));
            return *this;
        }

        not_dump<T>& operator = (const not_dump<T>& x)
        {
            if (detail::dumpable_is_custom_alloc())
                T::operator=(T());
            else 
                T::operator=(x);
            return *this;
        }
    };
}
