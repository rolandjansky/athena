/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MagFieldElements/BFieldCache.h"
#include <iostream>

// set the field values at each corner (rescale for current scale factor)
void
BFieldCache::printField() const
{
  // print out field values
  std::cout << "Field at corner i, for each component j (Bz, Br, Bphi)"
            << '\n';

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << i << "," << j << ": " << m_field[j][i] << ", ";
    }
    std::cout << '\n';
  }
}

