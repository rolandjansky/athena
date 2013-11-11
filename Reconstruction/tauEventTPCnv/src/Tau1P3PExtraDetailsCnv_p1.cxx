/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PExtraDetailsCnv_p1.cxx
///
/// Implementations of persistent Tau1P3PExtraDetails to transient
/// Tau1P3PExtraDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-07.
///
/// $Id: Tau1P3PExtraDetailsCnv_p1.cxx,v 1.3 2009-01-20 17:19:01 tburgess Exp $

//Trick to access private members in tau common details
#define private public
#define protected public
#include "tauEvent/Tau1P3PExtraDetails.h"
#undef private
#undef protected

#include "tauEventTPCnv/Tau1P3PExtraDetailsCnv_p1.h"

Tau1P3PExtraDetailsCnv_p1::Tau1P3PExtraDetailsCnv_p1() :
    T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::Tau1P3PExtraDetails,
    Tau1P3PExtraDetails_p1 >()
{
}

Tau1P3PExtraDetailsCnv_p1::~Tau1P3PExtraDetailsCnv_p1()
{
}

void Tau1P3PExtraDetailsCnv_p1::persToTrans(
    const Tau1P3PExtraDetails_p1 *pers,
    Analysis::Tau1P3PExtraDetails *trans,
    MsgStream &msg )
{
    m_cellCnv.resetForCnv( pers->m_linkNames );
    trans->m_sumPTTracks = pers->m_sumPTTracks;
    const unsigned int tracks = pers->m_tracks;
    if( tracks == 0 ) return;
    const unsigned int samplings = pers->m_closestPhiTrkCell.size()/tracks;
    trans->allocTracksAndSamplings( tracks, samplings );
    unsigned int ind = 0;
    for( unsigned int i = 0; i < tracks; ++i ) {
	for( unsigned int j = 0; j < samplings; ++j ) {
	    m_cellCnv.persToTrans( 
		pers->m_closestEtaTrkVertCell[ind], 
		trans->m_closestEtaTrkVertCell[i][j], msg );
	    m_cellCnv.persToTrans( 
		pers->m_closestEtaTrkCell[ind], 
		trans->m_closestEtaTrkCell[i][j], msg );
	    m_cellCnv.persToTrans( 
		pers->m_closestPhiTrkVertCell[ind], 
		trans->m_closestPhiTrkVertCell[i][j], msg );
	    m_cellCnv.persToTrans( 
		pers->m_closestPhiTrkCell[ind], 
		trans->m_closestPhiTrkCell[i][j], msg );
	    trans->m_etaTrackCaloSamp[i][j] = pers->m_etaTrackCaloSamp[ind];
	    trans->m_phiTrackCaloSamp[i][j] = pers->m_phiTrackCaloSamp[ind];
	    ++ind;
	}
    }
}

void Tau1P3PExtraDetailsCnv_p1 :: transToPers
   (const Analysis::Tau1P3PExtraDetails */*trans*/,
    Tau1P3PExtraDetails_p1 */*pers*/,
    MsgStream &msg )
{
    msg << MSG::WARNING << "Tau1P3PExtraDetailsCnv_p1::transToPers called "
	"this should not happen, as this method is deprecated" << endreq;
    return;
}
