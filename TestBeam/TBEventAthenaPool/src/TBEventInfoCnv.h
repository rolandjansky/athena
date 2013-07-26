/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBEVENTINFOCNV_P1_H
#define TBEVENTATHENAPOOL_TBEVENTINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBEventInfo_p1.h"
#include "TBEvent/TBEventInfo.h"
#include "TBTPCnv/TBEventInfoCnv_p1.h"

// the latest persistent representation type of TBEventInfo:
typedef  TBEventInfo_p1  TBEventInfo_PERS;
typedef  T_AthenaPoolCustomCnv<TBEventInfo, TBEventInfo_PERS >   TBEventInfoCnvBase;

class TBEventInfoCnv : public TBEventInfoCnvBase {
friend class CnvFactory<TBEventInfoCnv >;
protected:
  TBEventInfoCnv (ISvcLocator* svcloc) : TBEventInfoCnvBase(svcloc) {}
  virtual TBEventInfo_PERS*   createPersistent (TBEventInfo* transCont);
  virtual TBEventInfo*        createTransient ();

  TBEventInfoCnv_p1  m_TPConverter;
};
#endif

