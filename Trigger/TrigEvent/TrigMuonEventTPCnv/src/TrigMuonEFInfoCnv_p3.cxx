/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p3.h"


void TrigMuonEFInfoCnv_p3::persToTrans(const TrigMuonEFInfo_p3* persObj,
		TrigMuonEFInfo* transObj,
		MsgStream &log)
{
	log << MSG::DEBUG << "TrigMuonEFInfoCnv_p3::persToTrans called " << endreq;

  std::unique_ptr<TrigMuonEFInfoTrackContainer> tracks
    (createTransFromPStore( &m_trackContainerCnv, persObj->m_trackContainer, log));
  std::unique_ptr<TrigMuonEFTrack> spectrometerTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log));
  std::unique_ptr<TrigMuonEFTrack> extrapolatedTrack
    (createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log));
  std::unique_ptr<TrigMuonEFCbTrack> combinedTrack
    (createTransFromPStore( &m_cbTrackCnv, persObj->m_combinedTrack, log));

  *transObj = TrigMuonEFInfo (persObj->m_roi,
                              std::move (tracks),
                              std::move (spectrometerTrack),
                              std::move (extrapolatedTrack),
                              std::move (combinedTrack));
  transObj->setNSegments (persObj->m_nSegments);
  transObj->setNMdtHits (persObj->m_nMdtHits);
  transObj->setNRpcHits (persObj->m_nRpcHits);
  transObj->setNTgcHits (persObj->m_nTgcHits);
  transObj->setNCscHits (persObj->m_nCscHits);

  transObj->setEtaPreviousLevel (persObj->m_etaPreviousLevel);
  transObj->setPhiPreviousLevel (persObj->m_phiPreviousLevel);
}


void TrigMuonEFInfoCnv_p3::transToPers(const TrigMuonEFInfo* /*transObj*/,
		TrigMuonEFInfo_p3* /*persObj*/,
		MsgStream &log)
{
	log << MSG::DEBUG << "TrigMuonEFInfoCnv_p3::transToPers called " << endreq;

//	persObj->m_roi               = transObj->m_roi;
//	persObj->m_nSegments         = transObj->m_nSegments;
//	persObj->m_nMdtHits          = transObj->m_nMdtHits;
//	persObj->m_nRpcHits          = transObj->m_nRpcHits;
//	persObj->m_nTgcHits          = transObj->m_nTgcHits;
//	persObj->m_nCscHits          = transObj->m_nCscHits;
//	persObj->m_etaPreviousLevel  = transObj->m_etaPreviousLevel;
//	persObj->m_phiPreviousLevel  = transObj->m_phiPreviousLevel;
//
//	// check for legacy
//	if (transObj->hasLegacyTrack()) {
//		log << MSG::DEBUG << "TrigMuonEFInfoCnv_p3::transToPers: has legacy track " << endreq;
//		// has a legacy track.
//		// this situation occurs if old BS files are read.
//		// make a tmp track container to safeguard const correctness of transObj.
//		TrigMuonEFInfoTrackContainer * tmpTrkCont = new TrigMuonEFInfoTrackContainer();
//		TrigMuonEFInfoTrack * tmpTrack = new TrigMuonEFInfoTrack();
//		TrigMuonEFTrack * tmpSpecTrack = new TrigMuonEFTrack( *(transObj->legacySpectrometerTrack() ));
//		tmpTrack->setSpectrometerTrack( tmpSpecTrack );
//		TrigMuonEFTrack * tmpExtrTrack = new TrigMuonEFTrack( *(transObj->legacyExtrapolatedTrack() ));
//		tmpTrack->setExtrapolatedTrack( tmpExtrTrack );
//		TrigMuonEFCbTrack * tmpCbTrack = new TrigMuonEFCbTrack( *(transObj->legacyCombinedTrack() ));
//		tmpTrack->setCombinedTrack( tmpCbTrack );
//		tmpTrkCont->push_back( tmpTrack );
//
//		// persistify the tmp track container
//		persObj->m_trackContainer    = toPersistent( &m_trackContainerCnv, tmpTrkCont, log);
//
//		// clean up
//		delete tmpTrkCont;
//	}
//	else {
//		persObj->m_trackContainer    = toPersistent( &m_trackContainerCnv, transObj->m_trackContainer, log);
//	}

}
