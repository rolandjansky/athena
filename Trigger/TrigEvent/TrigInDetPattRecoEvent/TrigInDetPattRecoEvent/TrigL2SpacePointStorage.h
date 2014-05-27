/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SPACEPOINT_STORAGE_H__
#define __TRIG_L2_SPACEPOINT_STORAGE_H__

#include <vector>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include <cstddef>


class TrigL2SpacePointStorage {
 public:

  TrigL2SpacePointStorage(unsigned int max_layer) : m_max_silicon_layer_num(max_layer+1) {
    m_layeredStorage.resize(m_max_silicon_layer_num, std::vector<const TrigSiSpacePointBase*>(0));
  }

  ~TrigL2SpacePointStorage() {
    for(unsigned int i=0;i<m_max_silicon_layer_num;i++) {
      m_layeredStorage[i].clear();
    }
  }

  const std::vector<const TrigSiSpacePointBase*>& operator[] (unsigned int idx) const {
    if(idx<m_max_silicon_layer_num) {
      return m_layeredStorage[idx];
    }
    else {
      return m_layeredStorage[m_max_silicon_layer_num];
    }
  }

  inline void addSpacePoint(const TrigSiSpacePointBase* pSP) {
    m_layeredStorage[pSP->layer()].push_back(pSP);
  }

 protected:

  unsigned int m_max_silicon_layer_num;
  std::vector<std::vector<const TrigSiSpacePointBase*> > m_layeredStorage;
 private:
  TrigL2SpacePointStorage() {};
  TrigL2SpacePointStorage(const TrigL2SpacePointStorage&) {};
};

namespace FTF {//fast track finder

class SpacePointStorageFiller {
 public:
  SpacePointStorageFiller(TrigL2SpacePointStorage& s) : m_storage(s) {};
    SpacePointStorageFiller(const SpacePointStorageFiller& sf) : m_storage(sf.m_storage) {};
      void operator()(const TrigSiSpacePointBase& sp) {
	m_storage.addSpacePoint(&sp);
      }
 protected:
    TrigL2SpacePointStorage& m_storage;
};

}//namespace

#endif
