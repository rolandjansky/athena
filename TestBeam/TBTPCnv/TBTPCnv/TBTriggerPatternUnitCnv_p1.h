/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBTriggerPatternUnit
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBTRIGGERPATTERNUNITCNV_P1
#define TBTPCNV_TBTRIGGERPATTERNUNITCNV_P1

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBTriggerPatternUnit_p1.h"

class MsgStream;
class TBTriggerPatternUnit;


class TBTriggerPatternUnitCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBTriggerPatternUnit, TBTriggerPatternUnit_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBTriggerPatternUnitCnv_p1() {}

  virtual void          persToTrans(const TBTriggerPatternUnit_p1* pers, TBTriggerPatternUnit* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBTriggerPatternUnit* trans, TBTriggerPatternUnit_p1* pers, MsgStream &log) const override;
};



#endif
