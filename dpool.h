#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <cstddef>
#include <map>

namespace dumpable
{
    class dpool
    {
        public:
            dpool(void* startAddress)
                : poolSize_(0)
            {
                poolOffsets_.insert(std::make_pair(startAddress, 0));
            }

            void write(std::ostream& os)
            {
                os.write((char*)&poolSize_, sizeof(poolSize_));
                for(auto it = pool_.begin(); it != pool_.end(); ++it)
                {
                    os.write(&(*it)[0], it->size());
                }
            }

            std::pair<void*, std::ptrdiff_t> alloc(void* self, size_t size)
            {
                if (!size)
                    return std::make_pair(nullptr, 0);
                pool_.push_front(std::vector<char>(size));
                poolSize_ += size;
                void* allocatedAddress = &pool_.front()[0];
                poolOffsets_.insert(std::make_pair(allocatedAddress, -poolSize_));
                auto it = poolOffsets_.lower_bound(self);
                --it;
                std::ptrdiff_t diff = (char*)self - (char*)it->first;
                return std::make_pair(allocatedAddress, -poolSize_+it->second-diff);
            }
        private:
            std::deque<std::vector<char>> pool_;
            std::ptrdiff_t poolSize_;
            std::map<void*, std::ptrdiff_t> poolOffsets_;
    };
}
