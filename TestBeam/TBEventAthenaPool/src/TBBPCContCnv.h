/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBBPCCONTCNV_P1_H
#define TBEVENTATHENAPOOL_TBBPCCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBBPCCont_p1.h"
#include "TBEvent/TBBPCCont.h"
#include "TBTPCnv/TBBPCContCnv_p1.h"

// the latest persistent representation type of TBBPCCont:
typedef  TBBPCCont_p1  TBBPCCont_PERS;
typedef  T_AthenaPoolCustomCnv<TBBPCCont, TBBPCCont_PERS >   TBBPCContCnvBase;

class TBBPCContCnv : public TBBPCContCnvBase {
friend class CnvFactory<TBBPCContCnv >;
protected:
  TBBPCContCnv (ISvcLocator* svcloc) : TBBPCContCnvBase(svcloc) {}
  virtual TBBPCCont_PERS*   createPersistent (TBBPCCont* transCont);
  virtual TBBPCCont*        createTransient ();

  TBBPCContCnv_p1  m_TPConverter;
};
#endif

