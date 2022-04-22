/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  STD_ARRAY_HELPER
#define STD_ARRAY_HELPER
#include <array>
 /// Helper function to initialize in place the arrays
  template<class T, size_t N>  constexpr std::array<T, N> make_array(const T& def_val) {
    std::array<T,N> arr{};
    arr.fill(def_val);
    return arr;
}
#endif
