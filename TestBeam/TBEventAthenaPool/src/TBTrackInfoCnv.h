/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBTRACKINFOCNV_P1_H
#define TBEVENTATHENAPOOL_TBTRACKINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBTrackInfo_p1.h"
#include "TBEvent/TBTrackInfo.h"
#include "TBTPCnv/TBTrackInfoCnv_p1.h"

// the latest persistent representation type of TBTrackInfo:
typedef  TBTrackInfo_p1  TBTrackInfo_PERS;
typedef  T_AthenaPoolCustomCnv<TBTrackInfo, TBTrackInfo_PERS >   TBTrackInfoCnvBase;

class TBTrackInfoCnv : public TBTrackInfoCnvBase {
friend class CnvFactory<TBTrackInfoCnv >;
protected:
  TBTrackInfoCnv (ISvcLocator* svcloc) : TBTrackInfoCnvBase(svcloc) {}
  virtual TBTrackInfo_PERS*   createPersistent (TBTrackInfo* transCont);
  virtual TBTrackInfo*        createTransient ();

  TBTrackInfoCnv_p1  m_TPConverter;
};
#endif

