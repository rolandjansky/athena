/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_DETECTORFACTORY_H 
#define SCT_GEOMODEL_SCT_DETECTORFACTORY_H 
 
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "CxxUtils/checker_macros.h"

#include <memory>

class GeoPhysVol;
class SCT_DataBase;
class SCT_GeometryManager;
class SCT_GeoModelAthenaComps;
class SCT_MaterialManager;
class SCT_Options;

class SCT_DetectorFactory : public InDetDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  SCT_DetectorFactory(const SCT_GeoModelAthenaComps * athenaComps, 
		      const SCT_Options & options) ATLAS_CTORDTOR_NOT_THREAD_SAFE; // Thread unsafe InDetDD::DetectorFactoryBase::rdbAccessSvc() method is used.

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

  InDetDD::SCT_DetectorManager *m_detectorManager;
  std::unique_ptr<SCT_GeometryManager> m_geometryManager;
  std::unique_ptr<SCT_DataBase> m_db;
  std::unique_ptr<SCT_MaterialManager> m_materials;
  bool m_useDynamicAlignFolders;

}; 
 
#endif 
 
