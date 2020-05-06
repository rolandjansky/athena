/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTIONCNV_P4_H
#define INDETSIMDATACOLLECTIONCNV_P4_H

// INDetSimDataCollectionCnv_p4, T/P separation of InDetSimData
// author D.Costanzo <davide.costanzo@cern.ch>,O.Arnaez <olivier.arnaez@cern.ch>

#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p4.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class InDetSimDataCollectionCnv_p4 : public T_AthenaPoolTPCnvBase<InDetSimDataCollection, InDetSimDataCollection_p4>
{
 public:
  InDetSimDataCollectionCnv_p4() {};

  virtual void	persToTrans(const InDetSimDataCollection_p4* persCont,
                            InDetSimDataCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const InDetSimDataCollection* transCont,
                            InDetSimDataCollection_p4* persCont,
                            MsgStream &log) ;

};

#endif
