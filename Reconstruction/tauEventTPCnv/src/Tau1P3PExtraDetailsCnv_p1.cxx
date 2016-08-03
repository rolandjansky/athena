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

#include "tauEvent/Tau1P3PExtraDetails.h"
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
    trans->setSumPTTracks (pers->m_sumPTTracks);
    const unsigned int tracks = pers->m_tracks;
    if( tracks == 0 ) return;
    const unsigned int samplings = pers->m_closestPhiTrkCell.size()/tracks;
    trans->allocTracksAndSamplings( tracks, samplings );
    unsigned int ind = 0;
    for( unsigned int i = 0; i < tracks; ++i ) {
	for( unsigned int j = 0; j < samplings; ++j ) {
            ElementLink<CaloCellContainer> el;
	    m_cellCnv.persToTrans( 
		pers->m_closestEtaTrkVertCell[ind], 
		el, msg );
            trans->setClosestEtaTrkVertCellLink (el, i, j);
	    m_cellCnv.persToTrans( 
		pers->m_closestEtaTrkCell[ind], 
		el, msg );
            trans->setClosestEtaTrkCellLink (el, i, j);
	    m_cellCnv.persToTrans( 
		pers->m_closestPhiTrkVertCell[ind], 
		el, msg );
            trans->setClosestPhiTrkVertCellLink (el, i, j);
	    m_cellCnv.persToTrans( 
		pers->m_closestPhiTrkCell[ind], 
		el, msg );
            trans->setClosestPhiTrkCellLink (el, i, j);
	    trans->setEtaTrackCaloSamp (i, j, pers->m_etaTrackCaloSamp[ind]);
	    trans->setPhiTrackCaloSamp (i, j, pers->m_phiTrackCaloSamp[ind]);
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
	"this should not happen, as this method is deprecated" << endmsg;
    return;
}
