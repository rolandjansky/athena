/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBTrack
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBTRACKCNV_P1_H
#define TBTPCNV_TBTRACKCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBTrack_p1.h"

class MsgStream;
class TBTrack;


class TBTrackCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBTrack, TBTrack_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBTrackCnv_p1() {}

  virtual void          persToTrans(const TBTrack_p1* pers, TBTrack* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBTrack* trans, TBTrack_p1* pers, MsgStream &log) const override;
};



#endif
