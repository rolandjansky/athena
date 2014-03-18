/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for DetailedTrackTruth.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/DetailedTrackTruthCnv_p1.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p1.h"
#include "TrkTruthData/DetailedTrackTruth.h"

#include "TrkTruthTPCnv/SubDetHitStatisticsCnv_p0.h"
#include "TrkTruthTPCnv/TruthTrajectoryCnv_p1.h"

#include "TrackTruthCollectionAccessor.h"

#include <stdexcept>

namespace {
  SubDetHitStatisticsCnv_p0 subDetHitStatConverter;
  TruthTrajectoryCnv_p1 truthTrajConverter;
}


void DetailedTrackTruthCnv_p1::persToTrans( const Trk::DetailedTrackTruth_p1* pers,
					    DetailedTrackTruth* trans, 
					    MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"DetailedTrackTruthCnv_p1::persToTrans()"<<endreq;

  static bool s_firsttime = true;
  if(s_firsttime) {
    s_firsttime = false;
    msg<<MSG::WARNING<<"Reading DetailedTrackTruth in the old format, statsTruth() info will not be available."<<endreq;
  }
  
  subDetHitStatConverter.persToTrans(&pers->m_hitsCommon, &TrackTruthCollectionAccessor::statsCommon(trans), msg);
  subDetHitStatConverter.persToTrans(&pers->m_hitsTrack, &TrackTruthCollectionAccessor::statsTrack(trans), msg);
  truthTrajConverter.persToTrans(&pers->m_trajectory, &TrackTruthCollectionAccessor::trajectory(trans), msg);
  
  msg<<MSG::DEBUG<<"DetailedTrackTruthCnv_p1::persToTrans() DONE"<<endreq;
}

void DetailedTrackTruthCnv_p1::transToPers( const DetailedTrackTruth*,
					    Trk::DetailedTrackTruth_p1*, 
					    MsgStream& msg ) 
{
  const char *info = "DetailedTrackTruthCnv_p1::transToPers(): THIS OLD CONVERTER SHOLD NOT GET CALLED!";
  msg<<MSG::FATAL<<info<<endreq;
  throw std::runtime_error(info);
}
