/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC3_H
#define INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC3_H


#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "InDetServMatGeoModel/InDetServMatManager.h"

class StoreGateSvc;
class IRDBAccessSvc;

class InDetServMatFactoryDC3: public InDetDD::DetectorFactoryBase  {

 public:
  
  // Constructor:
  InDetServMatFactoryDC3(InDetDD::AthenaComps * athenaComps);
  
  // Destructor:
  ~InDetServMatFactoryDC3();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  // manager
  virtual const InDetDD::InDetServMatManager* getDetectorManager () const;

 private:  
  
  // Illegal operations:
  const InDetServMatFactoryDC3 & operator=(const InDetServMatFactoryDC3 &right);
  InDetServMatFactoryDC3(const InDetServMatFactoryDC3 &right);

  // private data
  InDetDD::InDetServMatManager   *m_manager;
};

#endif //  INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC3_H


