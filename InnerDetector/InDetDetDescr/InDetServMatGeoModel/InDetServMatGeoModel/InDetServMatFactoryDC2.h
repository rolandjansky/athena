/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC2_H
#define INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC2_H


#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"
//the following needed because the return type of getDetectorManager() is not 
//the same as the method return type as specified in the baseclass
#include "InDetServMatGeoModel/InDetServMatManager.h"

#include "GaudiKernel/ServiceHandle.h"

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

  MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }

 private:  
  
  // Illegal operations:
  const InDetServMatFactoryDC2 & operator=(const InDetServMatFactoryDC2 &right);
  InDetServMatFactoryDC2(const InDetServMatFactoryDC2 &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
  InDetDD::InDetServMatManager   *m_manager;
  Athena::MsgStreamMember m_msg;
};

#endif //  INDETSERVMATGEOMODEL_INDETSERVMATFACTORYDC2_H


