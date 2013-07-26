/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBTDCCNV_P1_H
#define TBEVENTATHENAPOOL_TBTDCCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBTDC_p1.h"
#include "TBEvent/TBTDC.h"
#include "TBTPCnv/TBTDCCnv_p1.h"

// the latest persistent representation type of TBTDC:
typedef  TBTDC_p1  TBTDC_PERS;
typedef  T_AthenaPoolCustomCnv<TBTDC, TBTDC_PERS >   TBTDCCnvBase;

class TBTDCCnv : public TBTDCCnvBase {
friend class CnvFactory<TBTDCCnv >;
protected:
  TBTDCCnv (ISvcLocator* svcloc) : TBTDCCnvBase(svcloc) {}
  virtual TBTDC_PERS*   createPersistent (TBTDC* transCont);
  virtual TBTDC*        createTransient ();

  TBTDCCnv_p1  m_TPConverter;
};
#endif

