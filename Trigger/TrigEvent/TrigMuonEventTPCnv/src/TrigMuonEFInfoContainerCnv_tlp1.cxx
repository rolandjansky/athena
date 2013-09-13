/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp1.h"

TrigMuonEFInfoContainerCnv_tlp1::TrigMuonEFInfoContainerCnv_tlp1()
{
	// Add all converters defined in this top level converter:
	// never change the order of adding converters!
	addMainTPConverter();

	addTPConverterForReading( &m_MuonEFInfoCnv);
	addTPConverterForReading( &m_MuonEFTrackCnv);
	addTPConverter( &m_P4IPtCotThPhiMCnv);
	addTPConverterForReading( &m_MuonEFInfoCnv_p2);
	addTPConverterForReading( &m_MuonEFCbTrackCnv);
	addTPConverterForReading( &m_MuonEFInfoCnv_p3);
	addTPConverterForReading( &m_MuonEFTrackCnv_p2);
	addTPConverterForReading( &m_MuonEFCbTrackCnv_p2);
	addTPConverter( &m_MuonEFInfoTrackContainerCnv);
	addTPConverter( &m_MuonEFInfoTrackCnv);
	addTPConverterForReading( &m_MuonEFTrackCnv_p3);
	addTPConverterForReading( &m_MuonEFCbTrackCnv_p3);
	addTPConverter( &m_MuonEFInfoCnv_p4);
	addTPConverterForReading( &m_MuonEFTrackCnv_p4);
	addTPConverterForReading( &m_MuonEFCbTrackCnv_p4);
	addTPConverter( &m_MuonEFTrackCnv_p5);
	addTPConverter( &m_MuonEFCbTrackCnv_p5);

}

void TrigMuonEFInfoContainerCnv_tlp1::setPStorage( TrigMuonEFInfoContainer_tlp1 *storage )
{
	setMainCnvPStorage( &storage->m_TrigMuonEFInfoContainers );

	m_MuonEFInfoCnv.    setPStorage(&storage->m_MuonEFInfo);
	m_MuonEFInfoCnv_p2. setPStorage(&storage->m_MuonEFInfo_p2);
	m_MuonEFInfoCnv_p3. setPStorage(&storage->m_MuonEFInfo_p3);
	m_MuonEFInfoCnv_p4. setPStorage(&storage->m_MuonEFInfo_p4);
	m_MuonEFTrackCnv.   setPStorage(&storage->m_MuonEFTrack);
	m_MuonEFTrackCnv_p2.setPStorage(&storage->m_MuonEFTrack_p2);
	m_MuonEFTrackCnv_p3.setPStorage(&storage->m_MuonEFTrack_p3);
	m_MuonEFTrackCnv_p4.setPStorage(&storage->m_MuonEFTrack_p4);
	m_MuonEFTrackCnv_p5.setPStorage(&storage->m_MuonEFTrack_p5);
	m_P4IPtCotThPhiMCnv.setPStorage(&storage->m_P4IPtCotThPhiM);
	m_MuonEFCbTrackCnv. setPStorage(&storage->m_MuonEFCbTrack);
	m_MuonEFCbTrackCnv_p2.         setPStorage(&storage->m_MuonEFCbTrack_p2);
	m_MuonEFCbTrackCnv_p3.         setPStorage(&storage->m_MuonEFCbTrack_p3);
	m_MuonEFCbTrackCnv_p4.         setPStorage(&storage->m_MuonEFCbTrack_p4);
	m_MuonEFCbTrackCnv_p5.         setPStorage(&storage->m_MuonEFCbTrack_p4);
	m_MuonEFInfoTrackContainerCnv. setPStorage( &storage->m_MuonEFInfoTrackContainer);
	m_MuonEFInfoTrackCnv.          setPStorage( &storage->m_MuonEFInfoTrack);

}

