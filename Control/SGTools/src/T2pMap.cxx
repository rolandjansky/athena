/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/T2pMap.h"
using namespace SG;
std::vector<DataProxy*> T2pMap::pacReport() const {
  std::vector<DataProxy*> notAccessed;
  if (m_pac) {
    t2p::const_iterator i(m_t2p.begin()), e(m_t2p.end());
    while (i != e) {
      if (!m_pac->accessed(i->first)) notAccessed.push_back(i->second);
      ++i;
    }
  }
  return notAccessed;
}
