/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDetectorFactoryDC2_h
#define PixelDetectorFactoryDC2_h 


#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
// readout includes:
#include "InDetReadoutGeometry/PixelDetectorManager.h"

class PixelSwitches;
class PixelGeoModelAthenaComps;

class PixelDetectorFactoryDC2 : public InDetDD::DetectorFactoryBase {

 public:
  
  // Constructor:
  PixelDetectorFactoryDC2(const PixelGeoModelAthenaComps * athenaComps,
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
  
 
};

// Class PixelDetectorFactory 
#endif


