/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITCONTAINERCNV_P1_H
#define LUCID_DIGITCONTAINERCNV_P1_H

#include "LUCID_DigitContainer_p1.h"
#include "LUCID_RawEvent/LUCID_DigitContainer.h"
#include "LUCID_EventTPCnv/LUCID_DigitCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_DigitContainerCnv_p1: public T_AthenaPoolTPCnvConstBase<LUCID_DigitContainer, LUCID_DigitContainer_p1> {
  
 public:
  using base_class::persToTrans;
  using base_class::transToPers;

  LUCID_DigitContainerCnv_p1() {}
  
  virtual void persToTrans(const LUCID_DigitContainer_p1* persObj, LUCID_DigitContainer*   transObj, MsgStream& log)const override;
  virtual void transToPers(const LUCID_DigitContainer*   transObj, LUCID_DigitContainer_p1* persObj, MsgStream& log)const override;
};

template<> class T_TPCnv<LUCID_DigitContainer, LUCID_DigitContainer_p1>: public LUCID_DigitContainerCnv_p1 {
  
 public:
};

#endif 
