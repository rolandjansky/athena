///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcDigitsCnv_p1.h 
// Transient/Persistent converter for ZdcDigits class
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef ZDCTPCNV_ZDCDIGITSCNV_P1_H
#define ZDCTPCNV_ZDCDIGITSCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ZdcEventTPCnv includes
#include "ZdcEventTPCnv/ZdcDigits_p1.h"

// ZdcEvent includes
#include "ZdcEvent/ZdcDigits.h"

class MsgStream;

class ZdcDigitsCnv_p1 : public T_AthenaPoolTPCnvBase<ZdcDigits, ZdcDigits_p1> {

public:

  /** Default constructor: 
   */
  ZdcDigitsCnv_p1() {}

  /** Method creating the transient representation ZdcDigits
   *  from its persistent representation ZdcDigits_p1
   */
  virtual void persToTrans(const ZdcDigits_p1* persObj, ZdcDigits* transObj, MsgStream &log);

  /** Method creating the persistent representation ZdcDigits_p1
   *  from its transient representation ZdcDigits
   */
  virtual void transToPers(const ZdcDigits* transObj, ZdcDigits_p1* persObj, MsgStream &log);

};

#endif //> ZDCTPCNV_ZDCDIGITSCNV_P1_H
