#pragma once

#include <map>
#include <algorithm>
#include "dvector.h"

namespace dumpable
{
    // implemented as sorted array
    template <typename K, typename V, typename Compare = std::less<K>>
    class dmap
    {
        public:
            dmap() {}
            dmap(const std::map<K, V, Compare>& rhs)
                : items_(rhs.begin(), rhs.end())
            {
            }

            dmap(const dmap<K, V, Compare>& rhs)
            {
                items_ = rhs.items_;
            }

            dmap(dmap<K, V, Compare>&& rhs)
            {
                items_ = std::move(rhs.items_);
            }

            void clear()
            {
                items_.clear();
            }
            size_t size() const { return items_.size(); }
            bool empty() const { return items_.empty(); }

            typedef K key_type;
            typedef V mapped_type;
            typedef std::pair<K, V> value_type;
            typedef Compare key_compare;
            class value_compare {
                    friend class dmap;
                    protected:
                        value_compare() {}
                    public:
                        bool operator()(const value_type& lhs, const value_type& rhs) const
                        {
                            return Compare()(lhs.first, rhs.first);
                        };
                };
            typedef value_type& reference;
            typedef const value_type& const_reference;
            typedef typename dvector<std::pair<K, V>>::iterator iterator;
            iterator begin() const { return items_.begin(); }
            iterator end() const { return items_.end(); }
            value_compare value_comp() const { return value_compare(); }

            dmap<K, V, Compare>& operator = (const dmap<K, V, Compare>& rhs)
            {
                items_ = rhs.items_;
                return *this;
            }

            dmap<K, V, Compare>& operator = (dmap<K, V, Compare>&& rhs) noexcept
            {
                items_ = std::move(rhs.items_);
                return *this;
            }

        protected:
            class find_comp {
                    friend class dmap;
                    protected:
                        find_comp() {}
                    public:
                        bool operator()(const value_type& lhs, const value_type& rhs) const
                        {
                            return Compare()(lhs.first, rhs.first);
                        };
                        bool operator()(const key_type& lhs, const value_type& rhs) const
                        {
                            return Compare()(lhs, rhs.first);
                        };
                        bool operator()(const value_type& lhs, const key_type& rhs) const
                        {
                            return Compare()(lhs.first, rhs);
                        };
                };
            
        public:
            iterator find(const K& key) const noexcept
            {
                iterator it = std::lower_bound(begin(), end(), key, find_comp());
                if (it != end() && !key_compare()(key, it->first))
                    return it;
                return end();
            }
            size_t count(const K& key) const noexcept
            {
                return find(key) == end() ? 0 : 1;
            }
        private:
            dvector<std::pair<K, V>> items_;
    };
}
