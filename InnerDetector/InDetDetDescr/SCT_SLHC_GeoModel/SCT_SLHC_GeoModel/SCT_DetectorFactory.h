/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_DETECTORFACTORY_H 
#define SCT_SLHC_GeoModel_SCT_DETECTORFACTORY_H 
 
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

class GeoPhysVol;

namespace InDetDD {
  class Zone;
}

namespace InDetDDSLHC {

class SCT_GeometryManager;
class SCT_Options;
class SCT_GeoModelAthenaComps;
class SCT_Barrel;
class SCT_Forward;

class SCT_DetectorFactory : public InDetDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  SCT_DetectorFactory(const SCT_GeoModelAthenaComps * athenaComps, 
		      const SCT_Options & options); 

  // Destructor
  virtual ~SCT_DetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const InDetDD::SCT_DetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  SCT_DetectorFactory(const SCT_DetectorFactory &right); 
  const SCT_DetectorFactory & operator=(const SCT_DetectorFactory &right); 

  InDetDD::Zone * makeZone(InDetDD::Zone * zone, const SCT_Barrel & barrel, const SCT_Forward & forward);
  InDetDD::SCT_DetectorManager *m_detectorManager;
  SCT_GeometryManager *m_geometryManager;

  void doChecks();

}; 
} 
#endif 
 
