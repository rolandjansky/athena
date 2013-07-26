/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Modified pool converter which complies with the standards of the
// transient/persistent scheme.
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBEVENTATHENAPOOL_TBTRACKCNV_P1_H
#define TBEVENTATHENAPOOL_TBTRACKCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TBTPCnv/TBTrack_p1.h"
#include "TBEvent/TBTrack.h"
#include "TBTPCnv/TBTrackCnv_p1.h"

// the latest persistent representation type of TBTrack:
typedef  TBTrack_p1  TBTrack_PERS;
typedef  T_AthenaPoolCustomCnv<TBTrack, TBTrack_PERS >   TBTrackCnvBase;

class TBTrackCnv : public TBTrackCnvBase {
friend class CnvFactory<TBTrackCnv >;
protected:
  TBTrackCnv (ISvcLocator* svcloc) : TBTrackCnvBase(svcloc) {}
  virtual TBTrack_PERS*   createPersistent (TBTrack* transCont);
  virtual TBTrack*        createTransient ();

  TBTrackCnv_p1  m_TPConverter;
};
#endif



