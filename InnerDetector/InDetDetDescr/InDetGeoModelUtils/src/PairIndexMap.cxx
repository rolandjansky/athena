/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/PairIndexMap.h"

namespace InDetDD {
  void
  PairIndexMap::add(int first, int second, int value) {
    m_map[std::make_pair(first, second)] = value;
  }

  int
  PairIndexMap::find(int first, int second) const {
    MapType::const_iterator iter = m_map.find(std::make_pair(first, second));
    if (iter == m_map.end()) return -1;

    return iter->second;
  }
}
