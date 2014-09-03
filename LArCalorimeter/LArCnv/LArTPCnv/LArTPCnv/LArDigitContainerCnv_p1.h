/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARDIGITCONTAINERCNV_P1_H
#define LARTPCNV_LARDIGITCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArDigitContainer_p1.h"

class MsgStream;
class LArDigitContainer;
//class LArDigitContainer_p1;

class LArDigitContainerCnv_p1  : public T_AthenaPoolTPCnvBase<LArDigitContainer, LArDigitContainer_p1>
{
public:

  LArDigitContainerCnv_p1() {}

  virtual void          persToTrans(const LArDigitContainer_p1* pers, LArDigitContainer* trans, MsgStream &log);
  virtual void          transToPers(const LArDigitContainer* trans, LArDigitContainer_p1* pers, MsgStream &log);
};

#endif
