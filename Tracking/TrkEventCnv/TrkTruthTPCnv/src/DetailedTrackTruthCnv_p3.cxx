/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for DetailedTrackTruth.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008
// Olivier.Arnaez@cern.ch, 2015

#include "TrkTruthTPCnv/DetailedTrackTruthCnv_p3.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p3.h"
#include "TrkTruthData/DetailedTrackTruth.h"

#include "TrkTruthTPCnv/SubDetHitStatisticsCnv_p0.h"
#include "TrkTruthTPCnv/TruthTrajectoryCnv_p2.h"

#include "TrackTruthCollectionAccessor.h"

namespace {
  SubDetHitStatisticsCnv_p0 subDetHitStatConverter;
  TruthTrajectoryCnv_p2 truthTrajConverter;
}


void DetailedTrackTruthCnv_p3::persToTrans( const Trk::DetailedTrackTruth_p3* pers,
                                            DetailedTrackTruth* trans,
                                            MsgStream& msg )
{
  msg<<MSG::DEBUG<<"DetailedTrackTruthCnv_p3::persToTrans()"<<endmsg;

  subDetHitStatConverter.persToTrans(&pers->m_hitsCommon, &TrackTruthCollectionAccessor::statsCommon(trans), msg);
  subDetHitStatConverter.persToTrans(&pers->m_hitsTrack, &TrackTruthCollectionAccessor::statsTrack(trans), msg);
  subDetHitStatConverter.persToTrans(&pers->m_hitsTruth, &TrackTruthCollectionAccessor::statsTruth(trans), msg);
  truthTrajConverter.persToTrans(&pers->m_trajectory, &TrackTruthCollectionAccessor::trajectory(trans), msg);

  msg<<MSG::DEBUG<<"DetailedTrackTruthCnv_p3::persToTrans() DONE"<<endmsg;
}

void DetailedTrackTruthCnv_p3::transToPers( const DetailedTrackTruth* trans,
                                            Trk::DetailedTrackTruth_p3* pers,
                                            MsgStream& msg )
{
  msg<<MSG::DEBUG<<"DetailedTrackTruthCnv_p3::transToPers()"<<endmsg;

  subDetHitStatConverter.transToPers(&trans->statsCommon(), &pers->m_hitsCommon, msg);
  subDetHitStatConverter.transToPers(&trans->statsTrack(),  &pers->m_hitsTrack,  msg);
  subDetHitStatConverter.transToPers(&trans->statsTruth(),  &pers->m_hitsTruth,  msg);
  truthTrajConverter.transToPers(&trans->trajectory(),      &pers->m_trajectory, msg);

  msg<<MSG::DEBUG<<"DetailedTrackTruthCnv_p3::transToPers() DONE"<<endmsg;
}
