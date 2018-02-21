/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardRegionGeoModelManager_h
#define ForwardRegionGeoModelManager_h 1
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

class ForwardRegionGeoModelManager : public GeoVDetectorManager  {

 public:
  // Constructor
  ForwardRegionGeoModelManager();

  // Destructor
  ~ForwardRegionGeoModelManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  
  // Access to raw geometry:
  virtual PVConstLink getTreeTop(unsigned int i) const;


  // Add a Tree top:
  void addTreeTop(PVLink);


  private:  


  const ForwardRegionGeoModelManager & operator=(const ForwardRegionGeoModelManager &right);
  ForwardRegionGeoModelManager(const ForwardRegionGeoModelManager &right);
  std::vector<PVLink> m_volume;
};

// Class ForwardRegionGeoModelManager

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ForwardRegionGeoModelManager, 157152126, 1)
#endif



#endif


