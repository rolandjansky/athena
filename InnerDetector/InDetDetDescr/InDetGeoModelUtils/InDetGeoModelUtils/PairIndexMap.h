/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_PairIndexMap_H
#define InDetGeoModelUtils_PairIndexMap_H


#include <map>
#include <utility>

namespace InDetDD {

  /// Class to store map between pair of two ints and an int.
  /// Used to help read in database tables which are a lookup via two ints.
  class PairIndexMap {
    
  public :
    void add(int first, int second, int value);
    int find(int first, int second) const;
    
  private:
    typedef std::map<std::pair<int,int>, int> MapType;
    MapType m_map;
    
  };
}

#endif // PixelPairIndexMap_H
