/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p4.h"


void TrigMuonEFInfoCnv_p4::persToTrans(const TrigMuonEFInfo_p4* persObj,
		TrigMuonEFInfo* transObj,
		MsgStream &log)
{
	log << MSG::DEBUG << "TrigMuonEFInfoCnv_p4::persToTrans called " << endreq;
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


void TrigMuonEFInfoCnv_p4::transToPers(const TrigMuonEFInfo* transObj,
		TrigMuonEFInfo_p4* persObj,
		MsgStream &log)
{
	log << MSG::DEBUG << "TrigMuonEFInfoCnv_p3::transToPers called " << endreq;

	persObj->m_roi               = transObj->RoINum();
	persObj->m_nSegments         = transObj->NSegments();
	persObj->m_nMdtHits          = transObj->NMdtHits();
	persObj->m_nRpcHits          = transObj->NRpcHits();
	persObj->m_nTgcHits          = transObj->NTgcHits();
	persObj->m_nCscHits          = transObj->NCscHits();
	persObj->m_etaPreviousLevel  = transObj->EtaPreviousLevel();
	persObj->m_phiPreviousLevel  = transObj->PhiPreviousLevel();

	// check for legacy
	if (transObj->hasLegacyTrack()) {
		log << MSG::DEBUG << "TrigMuonEFInfoCnv_p3::transToPers: has legacy track " << endreq;
		// has a legacy track.
		// this situation occurs if old BS files are read.
		// make a tmp track container to safeguard const correctness of transObj.
		TrigMuonEFInfoTrackContainer * tmpTrkCont = new TrigMuonEFInfoTrackContainer();
		TrigMuonEFInfoTrack * tmpTrack = new TrigMuonEFInfoTrack();
		TrigMuonEFTrack * tmpSpecTrack = new TrigMuonEFTrack( *(transObj->legacySpectrometerTrack() ));
		tmpTrack->setSpectrometerTrack( tmpSpecTrack );
		TrigMuonEFTrack * tmpExtrTrack = new TrigMuonEFTrack( *(transObj->legacyExtrapolatedTrack() ));
		tmpTrack->setExtrapolatedTrack( tmpExtrTrack );
		TrigMuonEFCbTrack * tmpCbTrack = new TrigMuonEFCbTrack( *(transObj->legacyCombinedTrack() ));
		tmpTrack->setCombinedTrack( tmpCbTrack );
		tmpTrkCont->push_back( tmpTrack );

		// persistify the tmp track container
		persObj->m_trackContainer    = toPersistent( &m_trackContainerCnv, tmpTrkCont, log);

		// clean up
		delete tmpTrkCont;
		delete tmpTrack;
		delete tmpSpecTrack;
		delete tmpExtrTrack;
		delete tmpCbTrack;
	}
	else {
          persObj->m_trackContainer    = toPersistent( &m_trackContainerCnv, transObj->TrackContainer(), log);
	}

}
