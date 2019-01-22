/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CavernInfraDetectorFactory01_h
#define CavernInfraDetectorFactory01_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "CavernInfraGeoModel/CavernInfraDetectorManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <string>

class StoreGateSvc;

class CavernInfraDetectorFactory01 : public GeoVDetectorFactory  
{
 
 public:
  
  // Constructor:
  CavernInfraDetectorFactory01(StoreGateSvc *pDetStore,
			  IRDBAccessSvc *pAccess);
  
  // Destructor:
  ~CavernInfraDetectorFactory01();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const CavernInfraDetectorManager * getDetectorManager() const;
  
  // Set version Tag and Node
  void setTagNode(std::string tag, std::string node);

 
 private:  
  // Illegal operations:
  const CavernInfraDetectorFactory01 & operator=(const CavernInfraDetectorFactory01 &right);
  CavernInfraDetectorFactory01(const CavernInfraDetectorFactory01 &right);
  
  // The manager:
  CavernInfraDetectorManager       *m_detectorManager;
  
  StoreGateSvc             *m_detectorStore;
  IRDBAccessSvc            *m_access;
  std::string              m_versionTag;
  std::string              m_versionNode;
};

// Class CavernInfraDetectorFactory01 
#endif


