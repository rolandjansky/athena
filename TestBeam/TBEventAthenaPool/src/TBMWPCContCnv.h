/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBMWPCCONTCNV_P1_H
#define TBEVENTATHENAPOOL_TBMWPCCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBMWPCCont_p1.h"
#include "TBEvent/TBMWPCCont.h"
#include "TBTPCnv/TBMWPCContCnv_p1.h"

// the latest persistent representation type of TBMWPCCont:
typedef  TBMWPCCont_p1  TBMWPCCont_PERS;
typedef  T_AthenaPoolCustomCnv<TBMWPCCont, TBMWPCCont_PERS >   TBMWPCContCnvBase;

class TBMWPCContCnv : public TBMWPCContCnvBase {
friend class CnvFactory<TBMWPCContCnv >;
protected:
  TBMWPCContCnv (ISvcLocator* svcloc) : TBMWPCContCnvBase(svcloc) {}
  virtual TBMWPCCont_PERS*   createPersistent (TBMWPCCont* transCont);
  virtual TBMWPCCont*        createTransient ();

  TBMWPCContCnv_p1  m_TPConverter;
};
#endif
