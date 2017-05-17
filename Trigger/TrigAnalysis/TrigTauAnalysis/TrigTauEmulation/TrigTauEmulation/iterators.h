/**
Copyright 2014-2015 Bernard van Gastel, bvgastel@bitpowder.com.
This file is part of Bit Powder Libraries.

Bit Powder Libraries is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Bit Powder Libraries is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Bit Powder Libraries.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ITERATORS_H
#define ITERATORS_H

#include <iterator>
#include <tuple>

namespace bitpowder {
namespace lib {

template <class T, class P, class iterator>
class select_iterator : public std::iterator<std::forward_iterator_tag, T, unsigned int> {
    iterator m_it;
    P m_func;
    T m_value;
    void progress() {
        bool cond;
        while (m_it && !(std::tie(cond, m_value) = m_func(*m_it), cond)) {
            ++m_it;
        }
    }
  public:
    select_iterator(const iterator& it, P&& func) : m_it(it), m_func(func), m_value() {
        progress();
    }
    ~select_iterator() {
    }
    inline T operator*() {
        return m_value;
    }
    inline T operator->() {
        return m_value;
    }
    inline void operator++() {
        ++m_it;
        progress();
    }
    inline bool operator!=(const select_iterator& b) {
        return m_it != b.m_it;
    }
    inline bool operator==(const select_iterator& b) {
        return m_it == b.m_it;
    }
    inline operator bool() {
        return m_it;
    }
    inline void insert(const T& a) {
        m_it.insert(a);
    }
    inline T erase() {
        T retval = std::move(m_value);
        m_it.erase();
        this->progress();
        return retval;
    }
};

template <class T, class S, class P>
struct select_container {
    S& m_value;
    P m_func;
    select_container(S& value, P&& func) : m_value(value), m_func(func) {
    }
    select_iterator<T, P&, decltype(m_value.begin())> begin() {
        return {m_value.begin(), m_func};
    }
    select_iterator<T, P&, decltype(m_value.end())> end() {
        return {m_value.end(), m_func};
    }
};

}
}

#endif // ITERATORS_H
