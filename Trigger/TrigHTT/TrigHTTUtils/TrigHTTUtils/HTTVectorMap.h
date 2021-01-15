/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HTTVECTORMAP
#define HTTVECTORMAP

/*
 * HTTVectorMap.h: This file defines a fast-lookup fast-iterate fixed-size map class.
 *
 * Declarations in this file:
 *      class HTTVectorMap
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-05-29
 */


#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <utility>
#include <iostream>

using std::vector;


/*
 * HTTVectorMap:
 *
 * Implements an ordered map as a fixed-size pair of vectors. This
 * enables fast lookup and iteration (good cache locality). However, this class
 * does not allow elements to be dynamically added/deleted.
 *
 * Will sort the input data. Key must implement the < operator.
 *
 * Accepts most of the std::map read functions, which will return
 * iterators to the data vector.
 */
template<class Key, class Value>
class HTTVectorMap
{
public:

    // --- Constructors --- //
    HTTVectorMap() {}

    HTTVectorMap(vector<Key> && keys, vector<Value> && data)
    {
        assign(std::move(keys), std::move(data));
    }
    HTTVectorMap(vector<Key> const & keys, vector<Value> const & data)
    {
        assign(keys, data);
    }

    void assign(vector<Key> && keys, vector<Value> && data)
    {
        if (keys.size() != data.size())
            throw std::invalid_argument("VectorMap: key-data size mismatch");

        m_keys = std::move(keys);
        m_data = std::move(data);

        sort();
    }

    void assign(vector<Key> const & keys, vector<Value> const & data)
    {
        if (keys.size() != data.size())
            throw std::invalid_argument("VectorMap: key-data size mismatch");

        m_keys = keys;
        m_data = data;

        sort();
    }


    // --- Data --- //
    inline vector<Key>   const & getKeys() const { return m_keys; }
    inline vector<Value> const & getData() const { return m_data; }

    // key access
    inline size_t getKeyIndex(Key key) const // will return size() if key is not found
    {
        auto lower_key = std::lower_bound(m_keys.begin(), m_keys.end(), key);
        if (*lower_key != key) return m_keys.size();
        return std::distance(m_keys.begin(), lower_key);
    }

    inline Key const & getKey(typename vector<Value>::iterator it) const
    {
        size_t i = std::distance(m_data.begin(), it);
        return m_keys[i];
    }
    inline Key const & getKey(typename vector<Value>::const_iterator it) const
    {
        size_t i = std::distance(m_data.begin(), it);
        return m_keys[i];
    }

    // std::map functions
    inline typename vector<Value>::iterator begin() noexcept { return m_data.begin(); }
    inline typename vector<Value>::const_iterator begin() const noexcept { return m_data.begin(); }
    inline typename vector<Value>::const_iterator cbegin() const noexcept { return m_data.cbegin(); }
    inline typename vector<Value>::iterator end() noexcept { return m_data.end(); }
    inline typename vector<Value>::const_iterator end() const noexcept { return m_data.end(); }
    inline typename vector<Value>::const_iterator cend() const noexcept { return m_data.cend(); }

    inline typename vector<Value>::iterator find(Key const & key)
    {
        auto lower_key = std::lower_bound(m_keys.begin(), m_keys.end(), key);
        if (*lower_key != key) return m_data.end();
        auto dist = std::distance(m_keys.begin(), lower_key);
        return m_data.begin() + dist;
    }
    inline typename vector<Value>::const_iterator find(Key const & key) const
    {
        auto lower_key = std::lower_bound(m_keys.begin(), m_keys.end(), key);
        if (*lower_key != key) return m_data.end();
        auto dist = std::distance(m_keys.begin(), lower_key);
        return m_data.begin() + dist;
    }
    inline typename vector<Value>::iterator lower_bound(Key const & key)
    {
        auto lower_key = std::lower_bound(m_keys.begin(), m_keys.end(), key);
        auto dist = std::distance(m_keys.begin(), lower_key);
        return m_data.begin() + dist;
    }
    inline typename vector<Value>::const_iterator lower_bound(Key const & key) const
    {
        auto lower_key = std::lower_bound(m_keys.begin(), m_keys.end(), key);
        auto dist = std::distance(m_keys.begin(), lower_key);
        return m_data.begin() + dist;
    }
    inline typename vector<Value>::iterator upper_bound(Key const & key)
    {
        auto upper_key = std::upper_bound(m_keys.begin(), m_keys.end(), key);
        auto dist = std::distance(m_keys.begin(), upper_key);
        return m_data.begin() + dist;
    }
    inline typename vector<Value>::const_iterator upper_bound(Key const & key) const
    {
        auto upper_key = std::upper_bound(m_keys.begin(), m_keys.end(), key);
        auto dist = std::distance(m_keys.begin(), upper_key);
        return m_data.begin() + dist;
    }
    inline std::pair<typename vector<Value>::iterator,typename vector<Value>::iterator> equal_range(Key const & key)
    {
        return std::make_pair(lower_bound(key), upper_bound(key));
    }
    inline std::pair<typename vector<Value>::const_iterator,typename vector<Value>::const_iterator>
        equal_range(Key const & key) const
    {
        return std::make_pair(lower_bound(key), upper_bound(key));
    }

    // std::vector functions
    inline Value & operator[](size_t i) { return m_data[i]; } // warning: no bounds check!
    inline Value const & operator[](size_t i) const { return m_data[i]; } // warning: no bounds check!
    inline Value * data() noexcept { return m_data.data(); }
    inline const Value * data() const noexcept { return m_data.data(); }

    inline size_t size() const { return m_keys.size(); }
    inline bool empty() const noexcept { return m_keys.empty(); }

private:
    vector<Key> m_keys;
    vector<Value> m_data;

    void sort()
    {
        if (!std::is_sorted(m_keys.begin(), m_keys.end()))
        {
            size_t size = m_keys.size();

            // initialize original index locations
            vector<size_t> idx(size);
            std::iota(idx.begin(), idx.end(), 0);

            // sort indexes based on comparing values in keys
            std::sort(idx.begin(), idx.end(),
                    [&](size_t i1, size_t i2) {return m_keys[i1] < m_keys[i2]; });

            // permute both vectors in-place
            vector<bool> done(size);
            for (size_t i = 0; i < size; ++i)
            {
                if (done[i]) continue;

                // move value in iSource to iTarget
                size_t iTarget = i;
                size_t iSource = idx[i];
                while (i != iSource)
                {
                    std::swap(m_keys[iTarget], m_keys[iSource]);
                    std::swap(m_data[iTarget], m_data[iSource]);
                    done[iTarget] = true;
                    iTarget = iSource;
                    iSource = idx[iSource];
                }
                done[iTarget] = true;
            }
        }
    }
};


#endif //HTTVECTORMAP
