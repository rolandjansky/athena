/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


class TBTriggerPatternUnitCnv_p1  : public T_AthenaPoolTPCnvBase<TBTriggerPatternUnit, TBTriggerPatternUnit_p1>
{
public:

  TBTriggerPatternUnitCnv_p1() {}

  virtual void          persToTrans(const TBTriggerPatternUnit_p1* pers, TBTriggerPatternUnit* trans, MsgStream &log);
  virtual void          transToPers(const TBTriggerPatternUnit* trans, TBTriggerPatternUnit_p1* pers, MsgStream &log);
};



#endif
