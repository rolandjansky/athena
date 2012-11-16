/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DetectorManager_h
#define LUCID_DetectorManager_h 1

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include <vector>

class LUCID_DetectorManager: public GeoVDetectorManager {
  
 public:

  LUCID_DetectorManager();
  ~LUCID_DetectorManager();

  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink  getTreeTop    (unsigned int i) const;
  void                 addTreeTop    (PVLink);
  
 private:  
  
  const LUCID_DetectorManager& operator=(const LUCID_DetectorManager &right);
  
  LUCID_DetectorManager(const LUCID_DetectorManager &right);
  
  std::vector<PVLink> m_volume;  
}; 

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LUCID_DetectorManager, 115078776, 1)
#endif

#endif


