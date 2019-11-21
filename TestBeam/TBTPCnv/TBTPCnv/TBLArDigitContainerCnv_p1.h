/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBLArDigitContainer
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBLARDIGITCONTAINERCNV_P1_H
#define TBTPCNV_TBLARDIGITCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBLArDigitContainer_p1.h"

class MsgStream;
class TBLArDigitContainer;
//class LArDigitContainer_p1;

class TBLArDigitContainerCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBLArDigitContainer, TBLArDigitContainer_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBLArDigitContainerCnv_p1() {}

  virtual void          persToTrans(const TBLArDigitContainer_p1* pers, TBLArDigitContainer* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBLArDigitContainer* trans, TBLArDigitContainer_p1* pers, MsgStream &log) const override;
};

#endif
