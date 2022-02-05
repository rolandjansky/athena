/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

/**
@class LookUpTable
Generic lookup table.
Uses comparison function objects or the < operator to sort and look up
values in the table.
*/

template <class T, class C = std::less<T> > class LookUpTable {
public:
    typedef T val_type;
    typedef std::vector<val_type> vec_type;
    typedef std::pair<val_type, val_type> pair_type;
    //  typename Data::iterator DataIt;
    typedef typename vec_type::iterator vec_type_it;
    typedef typename vec_type::const_iterator vec_type_cit;

    LookUpTable() = default;
    ~LookUpTable() = default;

    bool empty() const { return m_data.empty(); }

    void setData(const vec_type& data);
    const vec_type& getData() const { return m_data; }

    vec_type_it begin() { return m_data.begin(); }
    vec_type_it end() { return m_data.end(); }
    vec_type_cit begin() const { return m_data.begin(); }
    vec_type_cit end() const { return m_data.end(); }

    vec_type_cit lowerBound(const val_type& val);
    pair_type find(const val_type& val) const;

    template <class CC> pair_type find(const val_type& val, const CC& c) const {
        vec_type_cit it = lower_bound(m_data.begin(), m_data.end(), val, c);
        if (it == m_data.end())
            return std::make_pair(*(it - 2), *(it - 1));
        else if (it == m_data.begin())
            return std::make_pair(*it, *(it + 1));
        else
            return std::make_pair(*(it - 1), *it);
    }

private:
    vec_type m_data;
};

template <class T, class C> inline void LookUpTable<T, C>::setData(const vec_type& data) {
    m_data.assign(data.begin(), data.end());
    sort(m_data.begin(), m_data.end(), C());
}

template <class T, class C> inline typename LookUpTable<T, C>::vec_type_cit LookUpTable<T, C>::lowerBound(const val_type& val) {
    vec_type_cit it = lower_bound(m_data.begin(), m_data.end(), val, C());
    if (it == m_data.end()) return m_data.end() - 1;
    return it;
}

template <class T, class C> inline typename LookUpTable<T, C>::pair_type LookUpTable<T, C>::find(const val_type& val) const {
    vec_type_cit it = lower_bound(m_data.begin(), m_data.end(), val, C());
    if (it == m_data.end())
        return std::make_pair(*(it - 2), *(it - 1));
    else if (it == m_data.begin())
        return std::make_pair(*it, *(it + 1));
    else
        return std::make_pair(*(it - 1), *it);
}

#endif
