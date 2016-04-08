/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonExtraDetailsCnv_p1.cxx
/// 
/// Implementation of transient<->persistent converter 
/// for Analysis::TauCommonExtraDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// 
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonExtraDetailsCnv_p1.cxx,v 1.3 2009-01-20 17:19:01 tburgess Exp $

#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEventTPCnv/VectorUtils.h"
#include "tauEventTPCnv/TauCommonExtraDetailsCnv_p1.h"

TauCommonExtraDetailsCnv_p1::TauCommonExtraDetailsCnv_p1() 
    : T_AthenaPoolTPPolyCnvBase<Analysis::TauDetails, Analysis::TauCommonExtraDetails,
			    TauCommonExtraDetails_p1>()
{

}

TauCommonExtraDetailsCnv_p1::~TauCommonExtraDetailsCnv_p1()
{

}

void TauCommonExtraDetailsCnv_p1::persToTrans( 
    const TauCommonExtraDetails_p1 *pers,
    Analysis::TauCommonExtraDetails *trans,
    MsgStream &msg )
{
    trans->setSumPtLooseTrk (pers->m_sumPtLooseTrk);
    trans->setSumPtTrk (pers->m_sumPtTrk);   
    trans->setSeedCalo_nEMCell (pers->m_seedCalo_nEMCell);
    trans->setSeedCalo_stripEt (pers->m_seedCalo_stripEt);
    trans->setSeedCalo_EMCentFrac (pers->m_seedCalo_EMCentFrac);
    trans->setSeedCalo_sumCellEnergy (pers->m_seedCalo_sumCellEnergy);
    trans->setSeedCalo_sumEMCellEnergy (pers->m_seedCalo_sumEMCellEnergy);
    m_cellCnv.resetForCnv( pers->m_linkNames );
    const unsigned int tracks = pers->m_tracks;
    if ( tracks == 0 ) return;
    const unsigned short int samplings = 
	pers->m_closestPhiTrkCell.size()/tracks;
    trans->allocTracksAndSamplings( tracks, samplings );
    int ind = 0;
    for( unsigned short int i = 0; i < tracks; ++i ) {
        for( unsigned short int j = 0; j < samplings; ++j ) {
            m_cellCnv.persToTrans( 
		pers->m_closestEtaTrkVertCell[ind], 
		trans->closestEtaTrkVertCell()[i][j], msg );
            m_cellCnv.persToTrans( 
		pers->m_closestEtaTrkCell[ind], 
		trans->closestEtaTrkCell()[i][j], msg );
            m_cellCnv.persToTrans( 
		pers->m_closestPhiTrkVertCell[ind],
		trans->closestPhiTrkVertCell()[i][j], msg );
            m_cellCnv.persToTrans( 
		pers->m_closestPhiTrkCell[ind], 
		trans->closestPhiTrkCell()[i][j], msg );
            trans->etaTrkCaloSamp()[i][j] = 
		pers->m_etaTrkCaloSamp[ind];
            trans->phiTrkCaloSamp()[i][j] = 
		pers->m_phiTrkCaloSamp[ind];
               
            ++ind;
        }
    }
    const unsigned int looseTracks = pers->m_looseTracks;
    if ( looseTracks == 0 ) return;
    const unsigned short int looseSamplings = 
	pers->m_etaLooseTrkCaloSamp.size()/tracks;
    ind = 0; 
    for( unsigned short int i = 0; i < looseTracks; ++i ) {
        for( unsigned short int j = 0; j < looseSamplings; ++j ) {
            trans->etaLooseTrkCaloSamp()[i][j] = 
		pers->m_etaLooseTrkCaloSamp[ind];
            trans->phiLooseTrkCaloSamp()[i][j] = 
		pers->m_phiLooseTrkCaloSamp[ind];    
            ++ind;
     
        
        }
    }    
        
}

void TauCommonExtraDetailsCnv_p1::transToPers( 
    const Analysis::TauCommonExtraDetails *trans,
    TauCommonExtraDetails_p1 *pers,
    MsgStream &msg )
{
    pers->m_sumPtLooseTrk=trans->sumPtLooseTrk();
    pers->m_sumPtTrk=trans->sumPtTrk();   
    pers->m_seedCalo_nEMCell=trans->seedCalo_nEMCell();
    pers->m_seedCalo_stripEt=trans->seedCalo_stripEt();
    pers->m_seedCalo_EMCentFrac=trans->seedCalo_EMCentFrac();
    pers->m_seedCalo_sumCellEnergy=trans->seedCalo_sumCellEnergy();
    pers->m_seedCalo_sumEMCellEnergy=trans->seedCalo_sumEMCellEnergy();
    m_cellCnv.resetForCnv( pers->m_linkNames );
    const unsigned int tracks = trans->closestEtaTrkVertCell().size();
    const unsigned int looseTracks = trans->etaLooseTrkCaloSamp().size();
    pers->m_tracks = tracks;
    pers->m_looseTracks = looseTracks;
    if ( tracks == 0 ) return;
    const unsigned int colsize = trans->closestEtaTrkVertCell()[0].size();
    const unsigned int size = tracks*colsize;
    const unsigned int loosesize = looseTracks*colsize;
    pers->m_closestEtaTrkVertCell.resize( size );
    pers->m_closestEtaTrkCell.resize( size );
    pers->m_closestPhiTrkVertCell.resize( size );
    pers->m_closestPhiTrkCell.resize( size );
    pers->m_etaTrkCaloSamp.resize( size );
    pers->m_phiTrkCaloSamp.resize( size );
    pers->m_etaLooseTrkCaloSamp.resize( loosesize );
    pers->m_phiLooseTrkCaloSamp.resize( loosesize );
    
    int ind = 0;
    for( unsigned short int i = 0; i < tracks; ++i ) {
        for( unsigned short int j = 0; j < colsize; ++j ) {
            m_cellCnv.transToPers( 
                trans->closestEtaTrkVertCell()[i][j], 
		pers->m_closestEtaTrkVertCell[ind], msg );
            m_cellCnv.transToPers( 
		trans->closestEtaTrkCell()[i][j], 
		pers->m_closestEtaTrkCell[ind], msg );
            m_cellCnv.transToPers( 
		trans->closestPhiTrkVertCell()[i][j],
		pers->m_closestPhiTrkVertCell[ind], msg );
            m_cellCnv.transToPers( 
		trans->closestPhiTrkCell()[i][j], 
		pers->m_closestPhiTrkCell[ind], msg );
            pers->m_etaTrkCaloSamp[ind] = 
		trans->etaTrkCaloSamp()[i][j];
            pers->m_phiTrkCaloSamp[ind] = 
		trans->phiTrkCaloSamp()[i][j];
            pers->m_etaLooseTrkCaloSamp[ind] = 
		trans->etaLooseTrkCaloSamp()[i][j];
            pers->m_phiLooseTrkCaloSamp[ind] = 
		trans->phiLooseTrkCaloSamp()[i][j];
                
            ++ind;
        }
    }
    
    
    ind = 0;
    for( unsigned short int i = 0; i < looseTracks; ++i ) {
        for( unsigned short int j = 0; j < colsize; ++j ) {
            pers->m_etaLooseTrkCaloSamp[ind] = 
              trans->etaLooseTrkCaloSamp()[i][j];
            pers->m_phiLooseTrkCaloSamp[ind] = 
              trans->phiLooseTrkCaloSamp()[i][j];
                
            ++ind;
    
        } 
    }
    
}
