/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBTAILCATCHERCNV_P1_H
#define TBEVENTATHENAPOOL_TBTAILCATCHERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBTailCatcher_p1.h"
#include "TBEvent/TBTailCatcher.h"
#include "TBTPCnv/TBTailCatcherCnv_p1.h"

// the latest persistent representation type of TBTailCatcher:
typedef  TBTailCatcher_p1  TBTailCatcher_PERS;
typedef  T_AthenaPoolCustomCnv<TBTailCatcher, TBTailCatcher_PERS >   TBTailCatcherCnvBase;

class TBTailCatcherCnv : public TBTailCatcherCnvBase {
friend class CnvFactory<TBTailCatcherCnv >;
protected:
  TBTailCatcherCnv (ISvcLocator* svcloc) : TBTailCatcherCnvBase(svcloc) {}
  virtual TBTailCatcher_PERS*   createPersistent (TBTailCatcher* transCont);
  virtual TBTailCatcher*        createTransient ();

  TBTailCatcherCnv_p1  m_TPConverter;
};
#endif


