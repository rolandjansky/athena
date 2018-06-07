/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDetectorFactory_h
#define PixelDetectorFactory_h 


#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
// readout includes:
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"

class PixelSwitches;
class PixelGeometryManager;
class PixelGeoModelAthenaComps;

class PixelDetectorFactory : public InDetDD::DetectorFactoryBase {

 public:
  
  // Constructor:
  PixelDetectorFactory(const PixelGeoModelAthenaComps * athenaComps,
		       const PixelSwitches & switches);
  
  // Destructor:
  ~PixelDetectorFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const InDetDD::PixelDetectorManager * getDetectorManager() const;

  // Determine which alignment folders are loaded to decide if we register old or new folders
  virtual InDetDD::AlignFolderType getAlignFolderType() const;

 private:  
  
  // Illegal operations:
  const PixelDetectorFactory & operator=(const PixelDetectorFactory &right);
  PixelDetectorFactory(const PixelDetectorFactory &right);
  // private data
  InDetDD::PixelDetectorManager     *m_detectorManager;
  PixelGeometryManager * m_geometryManager;

  void doChecks();
  
};

// Class PixelDetectorFactory 
#endif


