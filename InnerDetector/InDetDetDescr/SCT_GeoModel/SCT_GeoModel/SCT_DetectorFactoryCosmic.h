/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_DETECTORFACTORYCOSMIC_H 
#define SCT_GEOMODEL_SCT_DETECTORFACTORYCOSMIC_H 
 
#include "GeoModelKernel/GeoVDetectorFactory.h" 
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

class StoreGateSvc;
class ISvcLocator;
class GeoPhysVol;
class SCT_GeometryManager;
class SCT_Options;

class SCT_DetectorFactoryCosmic : public GeoVDetectorFactory  
{ 
  
 public: 
  // Constructor
  SCT_DetectorFactoryCosmic(ISvcLocator* svcLocator, StoreGateSvc *detStore, const SCT_Options & options); 

  // Destructor
  virtual ~SCT_DetectorFactoryCosmic(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const InDetDD::SCT_DetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  SCT_DetectorFactoryCosmic(const SCT_DetectorFactoryCosmic &right); 
  const SCT_DetectorFactoryCosmic & operator=(const SCT_DetectorFactoryCosmic &right); 

  InDetDD::SCT_DetectorManager *m_detectorManager;
  SCT_GeometryManager *m_geometryManager;


}; 
 
#endif 
 
