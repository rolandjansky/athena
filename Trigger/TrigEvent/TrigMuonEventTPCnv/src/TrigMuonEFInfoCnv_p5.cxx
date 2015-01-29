/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p5.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p5.h"


void TrigMuonEFInfoCnv_p5::persToTrans(const TrigMuonEFInfo_p5* persObj, TrigMuonEFInfo* transObj, MsgStream &log) {
    // std::cout << "TrigMuonEFInfoCnv_p5::persToTrans called " << std::endl;

    transObj->m_roi               = persObj->m_allTheInts[0];
    transObj->m_nSegments         = persObj->m_allTheInts[1];
    transObj->m_nMdtHits          = persObj->m_allTheInts[2];
    transObj->m_nRpcHits          = persObj->m_allTheInts[3];
    transObj->m_nTgcHits          = persObj->m_allTheInts[4];
    transObj->m_nCscHits          = persObj->m_allTheInts[5];
    
	transObj->m_etaPreviousLevel  = persObj->m_etaPreviousLevel;
	transObj->m_phiPreviousLevel  = persObj->m_phiPreviousLevel;

    // 
    // // legacy to be compatible - probably not needed !
    // delete transObj->m_spectrometerTrack;
    // transObj->m_spectrometerTrack = createTransFromPStore( &m_trackCnv, persObj->m_spectrometerTrack, log);
    // delete transObj->m_extrapolatedTrack;
    // transObj->m_extrapolatedTrack = createTransFromPStore( &m_trackCnv, persObj->m_extrapolatedTrack, log);
    // delete transObj->m_combinedTrack;
    // transObj->m_combinedTrack     = createTransFromPStore( &m_cbTrackCnv, persObj->m_combinedTrack, log);

	// track container
	delete transObj->m_trackContainer;
	transObj->m_trackContainer = createTransFromPStore( &m_trackContainerCnv, persObj->m_trackContainer, log);

}


void TrigMuonEFInfoCnv_p5::transToPers(const TrigMuonEFInfo* transObj, TrigMuonEFInfo_p5* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFInfoCnv_p3::transToPers called " << std::endl;

	persObj->m_allTheInts[0]    = transObj->m_roi;
	persObj->m_allTheInts[1]    = transObj->m_nSegments;
	persObj->m_allTheInts[2]    = transObj->m_nMdtHits;
	persObj->m_allTheInts[3]    = transObj->m_nRpcHits;
	persObj->m_allTheInts[4]    = transObj->m_nTgcHits;
	persObj->m_allTheInts[5]    = transObj->m_nCscHits;
	
	persObj->m_etaPreviousLevel  = transObj->m_etaPreviousLevel;
	persObj->m_phiPreviousLevel  = transObj->m_phiPreviousLevel;

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
		persObj->m_trackContainer    = toPersistent( &m_trackContainerCnv, transObj->m_trackContainer, log);
    // }

}
