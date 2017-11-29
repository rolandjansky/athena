/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTIONCNV_P3_H
#define INDETSIMDATACOLLECTIONCNV_P3_H

// INDetSimDataCollectionCnv_p3, T/P separation of InDetSimData
// author D.Costanzo <davide.costanzo@cern.ch>,O.Arnaez <olivier.arnaez@cern.ch>

#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class InDetSimDataCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<InDetSimDataCollection, InDetSimDataCollection_p3>
{
 public:
  InDetSimDataCollectionCnv_p3() {};

  virtual void	persToTrans(const InDetSimDataCollection_p3* persCont,
                            InDetSimDataCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const InDetSimDataCollection* transCont,
                            InDetSimDataCollection_p3* persCont,
                            MsgStream &log) ;

};

#endif
