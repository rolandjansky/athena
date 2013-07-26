/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBSCINTILLATORCONTCNV_P1_H
#define TBEVENTATHENAPOOL_TBSCINTILLATORCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBScintillatorCont_p1.h"
#include "TBEvent/TBScintillatorCont.h"
#include "TBTPCnv/TBScintillatorContCnv_p1.h"

// the latest persistent representation type of TBScintillatorCont:
typedef  TBScintillatorCont_p1  TBScintillatorCont_PERS;
typedef  T_AthenaPoolCustomCnv<TBScintillatorCont, TBScintillatorCont_PERS >   TBScintillatorContCnvBase;

class TBScintillatorContCnv : public TBScintillatorContCnvBase {
friend class CnvFactory<TBScintillatorContCnv >;
protected:
  TBScintillatorContCnv (ISvcLocator* svcloc) : TBScintillatorContCnvBase(svcloc) {}
  virtual TBScintillatorCont_PERS*   createPersistent (TBScintillatorCont* transCont);
  virtual TBScintillatorCont*        createTransient ();

  TBScintillatorContCnv_p1  m_TPConverter;
};
#endif

