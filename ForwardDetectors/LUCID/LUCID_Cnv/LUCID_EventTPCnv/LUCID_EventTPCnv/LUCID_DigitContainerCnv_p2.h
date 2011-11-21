/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITCONTAINERCNV_P2_H
#define LUCID_DIGITCONTAINERCNV_P2_H

#include "LUCID_DigitContainer_p2.h"
#include "LUCID_RawEvent/LUCID_DigitContainer.h"
#include "LUCID_EventTPCnv/LUCID_DigitCnv_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_DigitContainerCnv_p2: public T_AthenaPoolTPCnvBase<LUCID_DigitContainer, LUCID_DigitContainer_p2> {
  
 public:
  
  LUCID_DigitContainerCnv_p2() {}
  
  virtual void persToTrans(const LUCID_DigitContainer_p2* persObj, LUCID_DigitContainer*   transObj, MsgStream& log);
  virtual void transToPers(const LUCID_DigitContainer*   transObj, LUCID_DigitContainer_p2* persObj, MsgStream& log);
};

template<> class T_TPCnv<LUCID_DigitContainer, LUCID_DigitContainer_p2>: public LUCID_DigitContainerCnv_p2 {
  
 public:
};

#endif 
