// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFISOLATIONCNV_P1_H__
#define TRIGMUONEVENT_TRIGMUONEFISOLATIONCNV_P1_H__
/**
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TrigMuonEFIsolationCnv_p1
 *
 * @brief T/P Converter for TrigMuonEFIsolation_p1
 *
 * @author Mark Owen <markowen@cern.ch>
 */

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p1.h"

class TrigMuonEFIsolationCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMuonEFIsolation, TrigMuonEFIsolation_p1> {

 public:
  
  /// Default constructor
  TrigMuonEFIsolationCnv_p1() {}

  /// Convert from persistent to transient
  virtual void persToTrans(const TrigMuonEFIsolation_p1* persObj, TrigMuonEFIsolation* transObj, MsgStream &log);
  /// Convert from transient to persistent
  virtual void transToPers(const TrigMuonEFIsolation* transObj, TrigMuonEFIsolation_p1* persObj, MsgStream &log);


};//class TrigMuonEFIsolationCnv_p1

#endif //TRIGMUONEVENT_TRIGMUONEFISOLATIONCNV_P1_H__
