/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTIONCNV_P2_H
#define INDETSIMDATACOLLECTIONCNV_P2_H

// INDetSimDataCollectionCnv_p2, T/P separation of InDetSimData
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class InDetSimDataCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<InDetSimDataCollection, InDetSimDataCollection_p2>
{
 public:
  InDetSimDataCollectionCnv_p2() {};
  
  virtual void	persToTrans(const InDetSimDataCollection_p2* persCont,
			    InDetSimDataCollection* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const InDetSimDataCollection* transCont,
			    InDetSimDataCollection_p2* persCont,
			    MsgStream &log) ;
  
};

#endif
