/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBADCRAWCONTCNV_P1_H
#define TBEVENTATHENAPOOL_TBADCRAWCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBADCRawCont_p1.h"
#include "TBEvent/TBADCRawCont.h"
#include "TBTPCnv/TBADCRawContCnv_p1.h"

// the latest persistent representation type of TBADCRawCont:
typedef  TBADCRawCont_p1  TBADCRawCont_PERS;
typedef  T_AthenaPoolCustomCnv<TBADCRawCont, TBADCRawCont_PERS >   TBADCRawContCnvBase;

class TBADCRawContCnv : public TBADCRawContCnvBase {
friend class CnvFactory<TBADCRawContCnv >;
protected:
  TBADCRawContCnv (ISvcLocator* svcloc) : TBADCRawContCnvBase(svcloc) {}
  virtual TBADCRawCont_PERS*   createPersistent (TBADCRawCont* transCont);
  virtual TBADCRawCont*        createTransient ();

  TBADCRawContCnv_p1  m_TPConverter;
};
#endif

