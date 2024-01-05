#ifndef LRU_H
#define LRU_H


#include <unordered_map>
#include <list>
#include <cassert>
#include <iostream>
#include <format>


namespace alias {
    template <typename key_ty, typename value_ty>
    using unmap = std::unordered_map<key_ty, value_ty>;
} // namespace alias


namespace caches {

    template <typename key_ty, typename ty, size_t cache_size>
    class lru_cache {
    public:
        using key_type    = key_ty; // cache_key_type
        using mapped_type = ty; // cache_mapped_type
        using reference   = mapped_type&; // cache_reference
        using value_type  = std::pair<const key_ty, ty>; // cache_value_type

        using iterator = typename std::list<value_type>::iterator;

    private:
        using hash_key_type    = key_type;
        using hash_mapped_type = typename std::list<value_type>::iterator;
        using hash_value_type  = std::pair<const hash_key_type, hash_mapped_type>;

    public:
        lru_cache() : cache_(0), hash_(0) {}

        lru_cache (const lru_cache&) = delete;
        lru_cache& operator= (const lru_cache&) = delete;


        // PRECOND: key doesn't exist in the list of pages
        // INVARIANT: pages_.size() <= size_
        hash_mapped_type insert (const key_type& new_key,
                                 mapped_type& new_mapped) // should I copy new_key and new_mapped??
        {
            // check PRECOND
            assert(hash_.find(new_key) == hash_.end());

            if (cache_.size() == cache_size) { // cache is full
                // remove lowest priority key before
                hash_.erase(hash_.find( cache_.back().first ));
                cache_.pop_back();
            }

            const auto cache_loc = cache_.insert(
                    cache_.begin(), {new_key, new_mapped}
            );
            hash_.insert({new_key, cache_loc});

            return cache_loc;
        }

        bool full() const {
            return cache_.size() == cache_size;
        }

        // check does key in cache
        bool check (const key_type& key) const {
            const auto it = hash_.find(key);
            return it != hash_.end();
        }

        // promote key as highest priority element
        // PRECOND: key exists in the cache
        void promote (const key_type& cached_key) {
            auto hash_loc = hash_.find(cached_key);

            hash_mapped_type cache_loc = cache_.insert(
                    cache_.begin(), *(hash_loc->second)
            );
            cache_.erase(hash_loc->second);

            hash_loc->second = cache_loc;
        }

        reference cached (const key_type& cached_key) {
            auto cache_loc = hash_.find(cached_key)->second;
            promote(cached_key);
            return cache_loc->second;
        }

        void print() const {
            using std::cout;
            using std::format;

            cout << "{ ";
            for (const auto& [key, value] : cache_) {
                cout << format("[{}, {}], ", key, value);
            }
            cout << "}\n";
        }

    private:
        std::list<value_type> cache_;
        alias::unmap<
            hash_key_type,
            hash_mapped_type
        > hash_;
    };

} // namespace caches


#endif // LRU_H
