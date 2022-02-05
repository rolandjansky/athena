/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_DETMANAGER_H
#define ZDC_DETMANAGER_H

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "AthenaBaseComps/AthMessaging.h"

class ZDC_DetManager : public GeoVDetectorManager, public AthMessaging
{
 public:
  
  ZDC_DetManager();
  ~ZDC_DetManager();
  
  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink  getTreeTop(unsigned int i) const;
  
  void addTreeTop(PVLink);
  
 private:
  std::vector<PVLink> m_volume;
};

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(ZDC_DetManager, 9886, 1)
#endif

#endif
