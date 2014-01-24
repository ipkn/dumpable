// Copyright (c) 2014 ipkn.
// Licensed under the MIT license.

#pragma once

#if defined(_MSC_VER) && !defined(noexcept)
#define noexcept throw()
#endif

#include <iostream> 
#include <cstddef>

#include "dptr.h"
#include "dpool.h"
#include "dvector.h"
#include "dstring.h"
#include "dmap.h"

namespace dumpable
{
    template <typename T>
    const T* from_dumped_buffer(const void* buffer)
    {
        std::ptrdiff_t offsetOfData = *(std::ptrdiff_t*)buffer;
        return (T*)((char*)buffer+offsetOfData+sizeof(offsetOfData));
    }

    template <typename T>
    T* from_dumped_buffer(void* buffer)
    {
        std::ptrdiff_t offsetOfData = *(std::ptrdiff_t*)buffer;
        return (T*)((char*)buffer+offsetOfData+sizeof(offsetOfData));
    }

    template <typename T>
    void write(const T& data, std::ostream& os)
    {
        T x;
        dpool local_pool(&x);
        dumpable::detail::dptr_alloc = [&local_pool](void* self, size_t size)->std::pair<void*, std::ptrdiff_t>{
                return local_pool.alloc(self, size);
            };
        x = data;
        local_pool.write(os);
        os.write((const char*)&x, sizeof(x));
        dumpable::detail::dptr_alloc = nullptr;
    }
}
