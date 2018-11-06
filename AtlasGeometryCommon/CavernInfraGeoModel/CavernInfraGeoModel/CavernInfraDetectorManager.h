/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CavernInfraDetectorManager_h
#define CavernInfraDetectorManager_h 1

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include <vector>

class CavernInfraDetectorManager : public GeoVDetectorManager  
{
 public:

  // Constructor
  CavernInfraDetectorManager();

  // Destructor
  ~CavernInfraDetectorManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink getTreeTop(unsigned int i) const;
 
  void addTreeTop(PVLink);      // Add a Tree top:

 private:  

  const CavernInfraDetectorManager & operator=(const CavernInfraDetectorManager &right);
  CavernInfraDetectorManager(const CavernInfraDetectorManager &right);
  
  // Tree Tops
  std::vector<PVLink> m_treeTops;
};

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(CavernInfraDetectorManager, 1500000300, 1)
#endif

#endif


