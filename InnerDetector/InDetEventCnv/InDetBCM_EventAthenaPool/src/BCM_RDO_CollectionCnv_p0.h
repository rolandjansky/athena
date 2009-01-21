/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBCM_EVENTATHENAPOOL_BCM_RDO_COLLECTIONCNV_P0_H
#define INDETBCM_EVENTATHENAPOOL_BCM_RDO_COLLECTIONCNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"
#include "InDetBCM_EventAthenaPool/BCM_RDO_Collection_p0.h"
#include "BCM_RawDataCnv_p0.h"

class MsgStream;

typedef T_AthenaPoolTPCnvVector< BCM_RDO_Collection, BCM_RDO_Collection_p0, BCM_RawDataCnv_p0 > BCM_RDO_CollectionCnv_p0_basetype;

class BCM_RDO_CollectionCnv_p0 : public BCM_RDO_CollectionCnv_p0_basetype {

 public:
  BCM_RDO_CollectionCnv_p0() {}

  virtual void persToTrans(const BCM_RDO_Collection_p0* persColl, BCM_RDO_Collection* transColl, MsgStream &log);
  virtual void transToPers(const BCM_RDO_Collection* transColl, BCM_RDO_Collection_p0* persColl, MsgStream &log);
};

#endif
