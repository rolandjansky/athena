/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DetectorFactory_h
#define LUCID_DetectorFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "GeoModelUtilities/GeoExtendedMaterial.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"

#include "LUCID_GeoModel/LUCID_DetectorManager.h"
#include "LUCID_GeoModel/LUCID_Constants.h"

class StoreGateSvc;
class AbsMaterialManager;
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
  void calcTubeParams();

  void addVJcone   (GeoFullPhysVol*);
  void addCooling  (GeoFullPhysVol*);
  void addVessel   (GeoFullPhysVol*);
  void addVesselGas(GeoPhysVol*);
  void addBulkHeads(GeoPhysVol*);
  void addTubes    (GeoPhysVol*);
  
 private:  

  const LUCID_DetectorFactory& operator=(const LUCID_DetectorFactory &right);
  LUCID_DetectorFactory(const LUCID_DetectorFactory &right);
  
  LUCID_DetectorManager*    m_detectorManager;   
  const AbsMaterialManager* m_materialManager;
  StoreGateSvc*             m_detectorStore;
  IRDBAccessSvc*            m_access;
  LUCID_RDBAccess*          m_lp;

  const GeoMaterial*        m_air;
  const GeoMaterial*        m_alu;
  const GeoMaterial*        m_cop;
  GeoExtendedMaterial*      m_gas;
  GeoExtendedMaterial*      m_quartz;
  GeoOpticalSurface*        m_optSur;
  
  double m_tubeTheta   [nLayers];
  double m_tubePosition[nLayers][nPmtTubesPerLayer][2];
};

#endif
