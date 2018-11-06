/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileCalibData.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/Exception.h"

template<class T>
TileCalibData<T>::TileCalibData() : m_calibDrawers(TileCalibUtils::MAX_DRAWERIDX, nullptr) {

}

template<class T>
TileCalibData<T>::~TileCalibData() {

  //=== Delete TileCalibDrawers (which do not own the blob)
  std::set<const T*> uniqueDrawers(m_calibDrawers.begin(), m_calibDrawers.end());
  for (const T* calibDrawer : uniqueDrawers) {
    delete calibDrawer;
  }

}

template<class T>
const T* TileCalibData<T>::getCalibDrawer(unsigned int drawerIdx) const {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {

    throw TileCalib::IndexOutOfRange("TileCalibData::getCalibDrawer()",
                                     drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  return m_calibDrawers[drawerIdx];
}

template<class T>
void TileCalibData<T>::setCalibDrawer(unsigned int drawerIdx, const T* calibDrawer) {

  if (drawerIdx >= TileCalibUtils::MAX_DRAWERIDX) {

    throw TileCalib::IndexOutOfRange("TileCalibData::setCalibDrawer()",
                                     drawerIdx, TileCalibUtils::MAX_DRAWERIDX);
  }

  m_calibDrawers[drawerIdx] = calibDrawer;
}


template class TileCalibData<TileCalibDrawerFlt>;
template class TileCalibData<TileCalibDrawerOfc>;
template class TileCalibData<TileCalibDrawerBch>;
