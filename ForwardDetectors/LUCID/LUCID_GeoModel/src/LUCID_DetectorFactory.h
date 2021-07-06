/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCIDGEOMODEL_LUCIDDETECTORFACTORY_H
#define LUCIDGEOMODEL_LUCIDDETECTORFACTORY_H

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "GeoModelUtilities/GeoExtendedMaterial.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"

#include "LUCID_GeoModel/LUCID_DetectorManager.h"
#include "LUCID_GeoModel/LUCID_Constants.h"

class StoreGateSvc;
class StoredMaterialManager;
class GeoShape;
class GeoPhysVol;
class GeoFullPhysVol;
class LUCID_RDBAccess;
class IRDBAccessSvc;

class LUCID_DetectorFactory : public GeoVDetectorFactory {

 public:
  
  LUCID_DetectorFactory(StoreGateSvc* pDetStore, IRDBAccessSvc* pAccess);
  ~LUCID_DetectorFactory();
  
  virtual void create(GeoPhysVol*);
  
  virtual const LUCID_DetectorManager* getDetectorManager() const;

  void buildMaterials();

  void addVJcone              (GeoFullPhysVol*);
  void addVJconeFrontRing     (GeoFullPhysVol*);
  void addVJconeBackRing      (GeoFullPhysVol*);
  void addLucidSupportCylinder(GeoFullPhysVol*);
  void addPmtSupportCylinder  (GeoFullPhysVol*);
  void addPmtTubes            (GeoFullPhysVol*);
  void addPmtSupport          (GeoFullPhysVol*);
  
 private:  

  const LUCID_DetectorFactory& operator=(const LUCID_DetectorFactory &right);
  LUCID_DetectorFactory(const LUCID_DetectorFactory &right);
  
  LUCID_DetectorManager*       m_detectorManager;   
  const StoredMaterialManager* m_materialManager;
  StoreGateSvc*                m_detectorStore;
  IRDBAccessSvc*               m_access;
  LUCID_RDBAccess*             m_lp;

  GeoExtendedMaterial*      m_quartz;
  GeoMaterial*              m_peek;
};

#endif
