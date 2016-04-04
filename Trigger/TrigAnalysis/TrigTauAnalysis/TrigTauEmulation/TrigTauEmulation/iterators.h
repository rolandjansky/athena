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
    iterator it;
    P func;
    T value;
    void progress() {
        bool cond;
        while (it && !(std::tie(cond, value) = func(*it), cond)) {
            ++it;
        }
    }
  public:
    select_iterator(const iterator& it, P&& func) : it(it), func(func), value() {
        progress();
    }
    ~select_iterator() {
    }
    inline T operator*() {
        return value;
    }
    inline T operator->() {
        return value;
    }
    inline void operator++() {
        ++it;
        progress();
    }
    inline bool operator!=(const select_iterator& b) {
        return it != b.it;
    }
    inline bool operator==(const select_iterator& b) {
        return it == b.it;
    }
    inline operator bool() {
        return it;
    }
    inline void insert(const T& a) {
        it.insert(a);
    }
    inline T erase() {
        T retval = std::move(value);
        it.erase();
        this->progress();
        return retval;
    }
};

template <class T, class S, class P>
struct select_container {
    S& value;
    P func;
    select_container(S& value, P&& func) : value(value), func(func) {
    }
    select_iterator<T, P&, decltype(value.begin())> begin() {
        return {value.begin(), func};
    }
    select_iterator<T, P&, decltype(value.end())> end() {
        return {value.end(), func};
    }
};

}
}

#endif // ITERATORS_H
