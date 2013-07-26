/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBPHASECNV_P1_H
#define TBEVENTATHENAPOOL_TBPHASECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBPhase_p1.h"
#include "TBEvent/TBPhase.h"
#include "TBTPCnv/TBPhaseCnv_p1.h"

// the latest persistent representation type of TBPhase:
typedef  TBPhase_p1  TBPhase_PERS;
typedef  T_AthenaPoolCustomCnv<TBPhase, TBPhase_PERS >   TBPhaseCnvBase;

class TBPhaseCnv : public TBPhaseCnvBase {
friend class CnvFactory<TBPhaseCnv >;
protected:
  TBPhaseCnv (ISvcLocator* svcloc) : TBPhaseCnvBase(svcloc) {}
  virtual TBPhase_PERS*   createPersistent (TBPhase* transCont);
  virtual TBPhase*        createTransient ();

  TBPhaseCnv_p1  m_TPConverter;
};
#endif
