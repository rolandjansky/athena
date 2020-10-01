/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDetectorFactorySR1_h
#define PixelDetectorFactorySR1_h 


#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
// readout includes:
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "CxxUtils/checker_macros.h"

class PixelSwitches;
class PixelGeometryManager;
class PixelGeoModelAthenaComps;

class PixelDetectorFactorySR1 : public InDetDD::DetectorFactoryBase {

 public:
  
  // Constructor:
  PixelDetectorFactorySR1(PixelGeoModelAthenaComps * athenaComps,
			  const PixelSwitches & switches);
    
  // Destructor:
  ~PixelDetectorFactorySR1();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const InDetDD::PixelDetectorManager * getDetectorManager() const;
  

 private:  
  
  // Illegal operations:
  const PixelDetectorFactorySR1 & operator=(const PixelDetectorFactorySR1 &right);
  PixelDetectorFactorySR1(const PixelDetectorFactorySR1 &right);
  // private data
  InDetDD::PixelDetectorManager     *m_detectorManager;
  PixelGeometryManager * m_geometryManager;

};  

// Class PixelDetectorFactory 
#endif


