#ifndef LRU_H
#define LRU_H


#include <unordered_map>
#include <list>
#include <iostream>
#include <format>


namespace alias {
    template <typename key_ty, typename value_ty>
    using unmap = std::unordered_map<key_ty, value_ty>;
} // namespace alias


namespace caches {

    template <typename key_ty, typename ty, size_t cache_size>
    class lru_cache {
    // INVARIANT: cache_.size() <= size_ && LRU is always at the top
    public:
        using key_type    = key_ty; // cache_key_type
        using mapped_type = ty; // cache_mapped_type
        using reference   = mapped_type&; // cache_reference
        using value_type  = std::pair<const key_ty, const ty>; // cache_value_type

        using iterator = typename std::list<value_type>::iterator;

    private:
        using hash_key_type    = key_type;
        using hash_mapped_type = typename std::list<value_type>::iterator;
        using hash_value_type  = std::pair<const hash_key_type, hash_mapped_type>;

    public:
        lru_cache() : cache_(0), hash_(0) {}

        lru_cache (const lru_cache&) = delete;
        lru_cache& operator= (const lru_cache&) = delete;


        template <typename pred_ty>
        mapped_type lookup_update (const key_type& key,
                                   pred_ty pred) // COND: pred_ty(key_type) --> mapped_type
        {
            auto hash_loc = hash_.find(key);

            // case: cache hit
            if (hash_loc != hash_.end()) {
                // promote
                iterator cache_loc = cache_.emplace(cache_.begin(),
                                                    key, hash_loc->second->second);
                cache_.erase(hash_loc->second);
                hash_loc->second = cache_loc;

                return hash_loc->second->second;
            } else { // case: cache miss
                if (full()) {
                    // remove the lowest priority page
                    hash_.erase(hash_.find( cache_.back().first ));
                    cache_.pop_back();
                }
                // insert new key at the the cache top
                iterator cache_loc = cache_.emplace(cache_.begin(),
                        key, pred(key));
                // check emplace status
                const auto [hast_loc_new, hash_emplace_status] =
                    hash_.emplace(key, cache_loc);
                if (!hash_emplace_status) {
                    throw std::runtime_error("key already exists");
                }

                return cache_loc->second;
            }
        }

        bool full() const {
            return cache_.size() == cache_size;
        }

        // check does key in cache
        bool check (const key_type& key) const {
            const auto it = hash_.find(key);
            return it != hash_.end();
        }

        // TODO: extract key
        // TODO: extract (mapped_type)value = pred(key)

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
