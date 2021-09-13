/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARLATOMEHEADERCONTAINERCNV_P1_H
#define LARTPCNV_LARLATOMEHEADERCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArLATOMEHeaderContainer_p1.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"

class MsgStream;

class LArLATOMEHeaderContainerCnv_p1  : public T_AthenaPoolTPCnvConstBase<LArLATOMEHeaderContainer, LArLATOMEHeaderContainer_p1>
{
public:

  LArLATOMEHeaderContainerCnv_p1() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void          persToTrans(const LArLATOMEHeaderContainer_p1* pers, LArLATOMEHeaderContainer* trans, MsgStream &log) const override;
  virtual void          transToPers(const LArLATOMEHeaderContainer* trans, LArLATOMEHeaderContainer_p1* pers, MsgStream &log) const override;
};

#endif
