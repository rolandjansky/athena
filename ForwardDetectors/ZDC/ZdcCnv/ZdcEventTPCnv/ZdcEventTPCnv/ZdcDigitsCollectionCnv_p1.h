///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcDigitsCollectionCnv_p1.h 
// Header file for class ZdcDigitsCollectionCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef ZDCTPCNV_ZDCDIGITSCOLLECTIONCNV_P1_H
#define ZDCTPCNV_ZDCDIGITSCOLLECTIONCNV_P1_H

// ZdcEventTPCnv includes
#include "ZdcEventTPCnv/ZdcDigitsCollection_p1.h"
#include "ZdcEventTPCnv/ZdcDigitsCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Zdc includes
#include "ZdcEvent/ZdcDigitsCollection.h"

class MsgStream;

class ZdcDigitsCollectionCnv_p1: public T_AthenaPoolTPCnvBase<ZdcDigitsCollection, ZdcDigitsCollection_p1> {
  
public:
  
  ZdcDigitsCollectionCnv_p1() {}
  
  virtual void persToTrans(const ZdcDigitsCollection_p1* persObj, ZdcDigitsCollection*   transObj, MsgStream& log);
  virtual void transToPers(const ZdcDigitsCollection*   transObj, ZdcDigitsCollection_p1* persObj, MsgStream& log);
};


template<>
class T_TPCnv<ZdcDigitsCollection, ZdcDigitsCollection_p1>
  : public ZdcDigitsCollectionCnv_p1
{
public:
};

#endif //> ZDCTPCNV_ZDCDIGITSCOLLECTIONCNV_P1_H

