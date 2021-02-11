/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARSCDIGITCONTAINERCNV_P1_H
#define LARTPCNV_LARSCDIGITCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArSCDigitContainer_p1.h"
#include "LArRawEvent/LArSCDigitContainer.h"

class MsgStream;

class LArSCDigitContainerCnv_p1  : public T_AthenaPoolTPCnvBase<LArSCDigitContainer, LArSCDigitContainer_p1>
{
public:

  LArSCDigitContainerCnv_p1() {}

  virtual void          persToTrans(const LArSCDigitContainer_p1* pers, LArSCDigitContainer* trans, MsgStream &log);
  virtual void          transToPers(const LArSCDigitContainer* trans, LArSCDigitContainer_p1* pers, MsgStream &log);
};

#endif
