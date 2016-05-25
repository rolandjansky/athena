/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTIONCNV_P1_H
#define INDETSIMDATACOLLECTIONCNV_P1_H

// INDetSimDataCollectionCnv_p1, T/P separation of InDetSimData
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetIdentifier/PixelID.h"

class InDetSimDataCollectionCnv_p1 : public T_AthenaPoolTPCnvBase<InDetSimDataCollection, InDetSimDataCollection_p1>
{
 public:
  InDetSimDataCollectionCnv_p1() {}
  
  virtual void	persToTrans(const InDetSimDataCollection_p1* persCont,
			    InDetSimDataCollection* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const InDetSimDataCollection* transCont,
			    InDetSimDataCollection_p1* persCont,
			    MsgStream &log) ;

private:
  const PixelID *m_pixId = nullptr;
  bool m_isInitialized = false;
  StatusCode initialize(MsgStream &log);
  
};

#endif
