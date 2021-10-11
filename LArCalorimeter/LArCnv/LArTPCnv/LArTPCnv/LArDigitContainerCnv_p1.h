/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARDIGITCONTAINERCNV_P1_H
#define LARTPCNV_LARDIGITCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArDigitContainer_p1.h"

class MsgStream;
class LArDigitContainer;
//class LArDigitContainer_p1;

class LArDigitContainerCnv_p1  : public T_AthenaPoolTPCnvConstBase<LArDigitContainer, LArDigitContainer_p1>
{
public:

  LArDigitContainerCnv_p1() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void          persToTrans(const LArDigitContainer_p1* pers, LArDigitContainer* trans, MsgStream &log) const override;
  virtual void          transToPers(const LArDigitContainer* trans, LArDigitContainer_p1* pers, MsgStream &log) const override;
};

#endif
