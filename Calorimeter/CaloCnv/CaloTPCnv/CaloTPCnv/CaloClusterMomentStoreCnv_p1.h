/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOATHENAPOOL_CALOCLUSTERMOMENTSTORECNV_P1_H
#define CALOATHENAPOOL_CALOCLUSTERMOMENTSTORECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
class CaloClusterMomentStore;
class CaloClusterMomentStore_p1;

//typedef T_AthenaPoolTPCnvBase<CaloClusterMomentStore,CaloClusterMomentStore_p1> CaloClusterMomentStoreCnvBasep1_t;

class CaloClusterMomentStoreCnv_p1// : public  CaloClusterMomentStoreCnvBasep1_t 
{
public:                                     
  CaloClusterMomentStoreCnv_p1();

  void transToPers(const CaloClusterMomentStore*, CaloClusterMomentStore_p1*/*, MsgStream&*/) ;
  void persToTrans(const CaloClusterMomentStore_p1*, CaloClusterMomentStore*/*, MsgStream&*/) ;
  
  //CaloClusterMomentStore* createTrans(const CaloClusterMomentStore_p1* /*,MsgStream&*/) ; 
  //CaloClusterMomentStore_p1* createPers(const CaloClusterMomentStore* /*,MsgStream&*/) ;
};


#endif

