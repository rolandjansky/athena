/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrackCnv_p7
 *
 * @brief transient-persistent converter for TrigMuonEFCbTrack_p7
 *
 * @author Mark Owen <markowen@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P7_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACKCNV_P7_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p7.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

class MsgStream;

class TrigMuonEFCbTrackCnv_p7 : public T_AthenaPoolTPCnvBase<TrigMuonEFCbTrack, TrigMuonEFCbTrack_p7>
{

 public:

  TrigMuonEFCbTrackCnv_p7() :  m_TrigMuonEFTrackCnv(0) {}

  virtual void persToTrans(const TrigMuonEFCbTrack_p7* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p7* persObj, MsgStream &log);

 protected:
  ITPConverterFor<TrigMuonEFTrack>     *m_TrigMuonEFTrackCnv;

};


#endif
