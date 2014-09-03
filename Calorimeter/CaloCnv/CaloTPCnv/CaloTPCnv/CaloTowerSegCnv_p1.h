/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOTOWERSEGCNV_P1_H
#define CALOATHENAPOOL_CALOTOWERSEGCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


//class MsgStream;
class CaloTowerSeg;
class CaloTowerSeg_p1;

class CaloTowerSegCnv_p1 { //: public T_AthenaPoolTPCnvBase<CaloTowerSeg,CaloTowerSeg_p1> {
 public: 
  void persToTrans(const CaloTowerSeg_p1*, CaloTowerSeg*) ;
  void transToPers(const CaloTowerSeg*, CaloTowerSeg_p1*) ;

};

#endif
