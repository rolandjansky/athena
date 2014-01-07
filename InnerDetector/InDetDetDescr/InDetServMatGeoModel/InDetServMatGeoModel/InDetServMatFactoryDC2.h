/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC2_H
#define INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC2_H


#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "InDetServMatGeoModel/InDetServMatManager.h"

class StoreGateSvc;
class IRDBAccessSvc;

class InDetServMatFactoryDC2 : public GeoVDetectorFactory  {

 public:
  
  // Constructor:
  InDetServMatFactoryDC2(StoreGateSvc  *pDetStore,
			 ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~InDetServMatFactoryDC2();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  // manager
  virtual const InDetDD::InDetServMatManager* getDetectorManager () const;

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:  
  
  // Illegal operations:
  const InDetServMatFactoryDC2 & operator=(const InDetServMatFactoryDC2 &right);
  InDetServMatFactoryDC2(const InDetServMatFactoryDC2 &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
  InDetDD::InDetServMatManager   *m_manager;
  mutable Athena::MsgStreamMember m_msg;
};

#endif //  INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC2_H


