// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Some common classes for general use in ATLAS

#include <utility> // for std::pair
#include <vector>
#include <map>
#include <set>
#include <string>

#include <stdint.h>

#include <boost/array.hpp>


template struct std::pair<unsigned int, unsigned int>;
template struct std::pair<std::string, unsigned int>;
template struct std::pair<std::string, std::vector<int> >;
template struct std::pair<std::string, std::vector<double> >;
template struct std::pair<std::pair<std::string, unsigned int>, std::pair<std::string, unsigned int> >;
template struct std::pair<unsigned int, std::vector<float> >;
template struct std::pair<unsigned int, std::vector<unsigned int> >;
