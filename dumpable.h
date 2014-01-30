// Copyright (c) 2014 ipkn.
// Licensed under the MIT license.

#pragma once

#include <iostream> 
#include <cstddef>

#include "dumpableconf.h"
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
        return (T*)buffer;
    }

    template <typename T>
    T* from_dumped_buffer(void* buffer)
    {
        return (T*)buffer;
    }

    template <typename T>
    void write(const T& data, std::ostream& os)
    {
        T x;
        dpool local_pool(&x, sizeof(T));
        dumpable::detail::dptr_alloc = [&local_pool](void* self, dumpable::size_t size)->std::pair<void*, dumpable::ptrdiff_t>{
                return local_pool.alloc(self, size);
            };
        x = data;
        os.write((const char*)&x, sizeof(x));
        local_pool.write(os);
        dumpable::detail::dptr_alloc = nullptr;
    }
}
