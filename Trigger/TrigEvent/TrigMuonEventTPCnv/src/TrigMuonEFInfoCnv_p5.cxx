/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p5.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p5.h"


void TrigMuonEFInfoCnv_p5::persToTrans(const TrigMuonEFInfo_p5* persObj, TrigMuonEFInfo* transObj, MsgStream &log) {
    // std::cout << "TrigMuonEFInfoCnv_p5::persToTrans called " << std::endl;

  // track container
  std::unique_ptr<TrigMuonEFInfoTrackContainer> tracks
    (createTransFromPStore( &m_trackContainerCnv, persObj->m_trackContainer, log));

  *transObj = TrigMuonEFInfo (persObj->m_allTheInts[0],
                              std::move (tracks));
  transObj->setNSegments (persObj->m_allTheInts[1]);
  transObj->setNMdtHits (persObj->m_allTheInts[2]);
  transObj->setNRpcHits (persObj->m_allTheInts[3]);
  transObj->setNTgcHits (persObj->m_allTheInts[4]);
  transObj->setNCscHits (persObj->m_allTheInts[5]);

  transObj->setEtaPreviousLevel (persObj->m_etaPreviousLevel);
  transObj->setPhiPreviousLevel (persObj->m_phiPreviousLevel);
}


void TrigMuonEFInfoCnv_p5::transToPers(const TrigMuonEFInfo* transObj, TrigMuonEFInfo_p5* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFInfoCnv_p3::transToPers called " << std::endl;

        persObj->m_allTheInts[0]    = transObj->RoINum();
	persObj->m_allTheInts[1]    = transObj->NSegments();
	persObj->m_allTheInts[2]    = transObj->NMdtHits();
	persObj->m_allTheInts[3]    = transObj->NRpcHits();
	persObj->m_allTheInts[4]    = transObj->NTgcHits();
	persObj->m_allTheInts[5]    = transObj->NCscHits();
	
	persObj->m_etaPreviousLevel  = transObj->EtaPreviousLevel();
	persObj->m_phiPreviousLevel  = transObj->PhiPreviousLevel();

	// check for legacy
    // if (transObj->hasLegacyTrack()) {
    //         std::cout << "TrigMuonEFInfoCnv_p3::transToPers: has legacy track " << std::endl;
    //  // has a legacy track.this situation occurs if old BS files are read.
    //  // make a tmp track container to safeguard const correctness of transObj.
    //  TrigMuonEFInfoTrackContainer * tmpTrkCont   = new TrigMuonEFInfoTrackContainer();
    //  TrigMuonEFInfoTrack          * tmpTrack     = new TrigMuonEFInfoTrack();
    //  TrigMuonEFTrack              * tmpSpecTrack = new TrigMuonEFTrack(   *(transObj->legacySpectrometerTrack()) );
    //  TrigMuonEFTrack              * tmpExtrTrack = new TrigMuonEFTrack(   *(transObj->legacyExtrapolatedTrack()) );
    //  TrigMuonEFCbTrack            * tmpCbTrack   = new TrigMuonEFCbTrack( *(transObj->legacyCombinedTrack())     );
    //  
    //  tmpTrack->setSpectrometerTrack( tmpSpecTrack );
    //  tmpTrack->setExtrapolatedTrack( tmpExtrTrack );
    //  tmpTrack->setCombinedTrack( tmpCbTrack );
    //  
    //  tmpTrkCont->push_back( tmpTrack );
    // 
    //  // persistify the tmp track container
    //  persObj->m_trackContainer    = toPersistent( &m_trackContainerCnv, tmpTrkCont, log);
    // 
    //  // clean up
    //  delete tmpTrkCont;
    // }
    // else {
        persObj->m_trackContainer    = toPersistent( &m_trackContainerCnv, transObj->TrackContainer(), log);
    // }

}
