/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDetectorFactoryDC2_h
#define PixelDetectorFactoryDC2_h 


#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
// readout includes:
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "CxxUtils/checker_macros.h"

class PixelSwitches;
class PixelGeoModelAthenaComps;

namespace PixelGeoDC2 {
  class PixelGeometryManager;
}

class PixelDetectorFactoryDC2 : public InDetDD::DetectorFactoryBase {

 public:
  
  // Constructor:
  PixelDetectorFactoryDC2(PixelGeoModelAthenaComps * athenaComps,
			  const PixelSwitches & switches);
  
  // Destructor:
  ~PixelDetectorFactoryDC2();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const InDetDD::PixelDetectorManager * getDetectorManager() const;
 

 private:  
  
  // Illegal operations:
  const PixelDetectorFactoryDC2 & operator=(const PixelDetectorFactoryDC2 &right);
  PixelDetectorFactoryDC2(const PixelDetectorFactoryDC2 &right);
  // private data
  InDetDD::PixelDetectorManager     *m_detectorManager;
  PixelGeoDC2::PixelGeometryManager* m_geometryManager;
  
 
};

// Class PixelDetectorFactory 
#endif


