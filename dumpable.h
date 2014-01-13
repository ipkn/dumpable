#pragma once

#include <iostream> 
#include <cstddef>

#include "dptr.h"
#include "dpool.h"
#include "dvector.h"

namespace dumpable
{
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
