/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBTDCRAWCONTCNV_P1_H
#define TBEVENTATHENAPOOL_TBTDCRAWCONTCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBTDCRawCont_p1.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBTPCnv/TBTDCRawContCnv_p1.h"

// the latest persistent representation type of TBTDCRawCont:
typedef  TBTDCRawCont_p1  TBTDCRawCont_PERS;
typedef  T_AthenaPoolCustomCnv<TBTDCRawCont, TBTDCRawCont_PERS >   TBTDCRawContCnvBase;

class TBTDCRawContCnv : public TBTDCRawContCnvBase {
friend class CnvFactory<TBTDCRawContCnv >;
protected:
  TBTDCRawContCnv (ISvcLocator* svcloc) : TBTDCRawContCnvBase(svcloc) {}
  virtual TBTDCRawCont_PERS*   createPersistent (TBTDCRawCont* transCont);
  virtual TBTDCRawCont*        createTransient ();

  TBTDCRawContCnv_p1  m_TPConverter;
};
#endif

