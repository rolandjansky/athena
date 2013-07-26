/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBLARDIGITCONTAINERCNV_H
#define TBEVENTATHENAPOOL_TBLARDIGITCONTAINERCNV_H

#include "TBTPCnv/TBLArDigitContainerCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
//#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBLArDigitContainer.h"

//class TBLArDigitContainer;
//class TBLArDigitContainer_p1;

typedef TBLArDigitContainer_p1 TBLArDigitContainerPERS;

typedef T_AthenaPoolCustomCnv<TBLArDigitContainer,TBLArDigitContainerPERS> TBLArDigitContainerCnvBase;

class TBLArDigitContainerCnv : public TBLArDigitContainerCnvBase 
{
  friend class CnvFactory<TBLArDigitContainerCnv>;
 protected:
  TBLArDigitContainerCnv(ISvcLocator*);
  virtual TBLArDigitContainer* createTransient();
  virtual TBLArDigitContainerPERS* createPersistent(TBLArDigitContainer*);
 private:
  TBLArDigitContainerCnv_p1 m_converter;
  pool::Guid   p0_guid;
  pool::Guid   p1_guid;

};

#endif

