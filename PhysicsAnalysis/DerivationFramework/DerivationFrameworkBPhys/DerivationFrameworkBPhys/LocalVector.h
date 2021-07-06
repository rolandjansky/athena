/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef LOCALVECTOR_H
#define LOCALVECTOR_H

#include <array>
#include <algorithm>
#include <iterator>

template <class A_Type, size_t SIZE>
class LocalVector
{
protected:
    std::array<A_Type, SIZE> m_memblock;
    size_t m_x;
public:
    LocalVector() : m_x(0) { }
    size_t size() const noexcept {
        return m_x;
    }
    constexpr size_t max_size() const noexcept {
        return SIZE;
    }
    typename std::array<A_Type, SIZE>::iterator begin() noexcept {
        return m_memblock.begin();
    }
    typename std::array<A_Type, SIZE>::iterator end() noexcept {
        auto it = m_memblock.begin();
        std::advance(it, m_x);
        return it;
    }
    typename std::array<A_Type, SIZE>::const_iterator begin() const noexcept {
        return m_memblock.cbegin();
    }
    typename std::array<A_Type, SIZE>::const_iterator end() const noexcept {
        auto it = m_memblock.cbegin();
        std::advance(it, m_x);
        return it;
    }
    A_Type& operator[]( size_t pos ) {
        return m_memblock[pos];
    }
    const A_Type& operator[]( size_t pos ) const {
        return m_memblock[pos];
    }
    bool contains(const A_Type& a) const {
        return std::find(begin(), end(), a) != end();
    }
    void push_back(const A_Type& a) {
        m_memblock[m_x++] = a;
    }
    void pop_back() {
        --m_x;
    }
    void clear() noexcept {
        m_x = 0;
    }
    A_Type& back(  ) {
        return m_memblock[m_x-1];
    }
    const A_Type& back(  )   const {
        return m_memblock[m_x-1];
    }
    A_Type& front(  ) {
        return m_memblock.front();
    }
    const A_Type& front(  )   const {
        return m_memblock.front();
    }
};
#endif
