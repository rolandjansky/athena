/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBTRIGGERPATTERNUNITCNV_P1_H
#define TBEVENTATHENAPOOL_TBTRIGGERPATTERNUNITCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBTriggerPatternUnit_p1.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBTPCnv/TBTriggerPatternUnitCnv_p1.h"

// the latest persistent representation type of TBTriggerPatternUnit:
typedef  TBTriggerPatternUnit_p1  TBTriggerPatternUnit_PERS;
typedef  T_AthenaPoolCustomCnv<TBTriggerPatternUnit, TBTriggerPatternUnit_PERS >   TBTriggerPatternUnitCnvBase;

class TBTriggerPatternUnitCnv : public TBTriggerPatternUnitCnvBase {
friend class CnvFactory<TBTriggerPatternUnitCnv >;
protected:
  TBTriggerPatternUnitCnv (ISvcLocator* svcloc) : TBTriggerPatternUnitCnvBase(svcloc) {}
  virtual TBTriggerPatternUnit_PERS*   createPersistent (TBTriggerPatternUnit* transCont);
  virtual TBTriggerPatternUnit*        createTransient ();

  TBTriggerPatternUnitCnv_p1  m_TPConverter;
};
#endif

