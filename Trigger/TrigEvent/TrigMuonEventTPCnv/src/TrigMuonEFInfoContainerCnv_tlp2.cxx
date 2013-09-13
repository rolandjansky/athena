/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp2.h"

TrigMuonEFInfoContainerCnv_tlp2::TrigMuonEFInfoContainerCnv_tlp2()
{
	// Add all converters defined in this top level converter:
	// never change the order of adding converters!
	addMainTPConverter();

	addTPConverter( &m_P4IPtCotThPhiMCnv);
	addTPConverter( &m_MuonEFInfoTrackContainerCnv);
	addTPConverter( &m_MuonEFInfoTrackCnv);
	
	addTPConverterForReading( &m_MuonEFCbTrackCnv);
	
	addTPConverter( &m_MuonEFInfoCnv);
	addTPConverter( &m_MuonEFTrackCnv);

	addTPConverter( &m_MuonEFCbTrackCnv_p7);

}

void TrigMuonEFInfoContainerCnv_tlp2::setPStorage( TrigMuonEFInfoContainer_tlp2 *storage )
{
	setMainCnvPStorage( &storage->m_TrigMuonEFInfoContainer_p2 );

	m_MuonEFInfoCnv.   setPStorage(&storage->m_MuonEFInfo_p5);
	m_MuonEFTrackCnv.  setPStorage(&storage->m_MuonEFTrack_p6);
	m_MuonEFCbTrackCnv.setPStorage(&storage->m_MuonEFCbTrack_p6);
	m_MuonEFCbTrackCnv_p7.setPStorage(&storage->m_MuonEFCbTrack_p7);
	
	m_P4IPtCotThPhiMCnv.setPStorage(&storage->m_P4IPtCotThPhiM);
	m_MuonEFInfoTrackContainerCnv. setPStorage( &storage->m_MuonEFInfoTrackContainer);
	m_MuonEFInfoTrackCnv.          setPStorage( &storage->m_MuonEFInfoTrack);


}
