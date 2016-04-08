/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetailsCnv_p1.cxx
///
/// Implementation of transient<->persistent converter
/// for Analysis::TauJet v3
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by Lukasz Janyst in May 2009
///
/// $Id: TauJetCnv_p3.cxx,v 1.5 2009-03-02 17:13:27 binet Exp $


#include "tauEvent/TauJet.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p2.h"
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"
#include "tauEventTPCnv/TauJetCnv_p3.h"

//Pre-allocate converters
static P4ImplEEtaPhiMCnv_p2 momCnv;
static ParticleBaseCnv_p1 partBaseCnv;
static ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > clusterCnv;
static ElementLinkCnv_p1<ElementLink<JetCollection> > jetCnv;
static ElementLinkVectorCnv_p1<
    ElementLinkVector<Analysis::TauDetailsContainer> > detailsCnv;
static ElementLinkVectorCnv_p1<
    ElementLinkVector<Rec::TrackParticleContainer> > tracksCnv;
static ElementLinkVectorCnv_p1<
    ElementLinkVector<Rec::TrackParticleContainer> > conversionTracksCnv;

static void setBit( unsigned char &field, unsigned num, bool val )
{
    if ( val )
	field |= (1 << num);
    else
	field &= ~(1 << num);
}

static bool getBit( unsigned char field, unsigned num )
{
    return (field & (1 << num)) ? true : false;
}

void TauJetCnv_p3::persToTrans( const TauJet_p3 *pers,
				  Analysis::TauJet *trans,
				  MsgStream &msg )
{
    momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(), msg );
    partBaseCnv.persToTrans( &pers->m_particleBase, &trans->particleBase(),
			     msg );
    clusterCnv.persToTrans( &pers->m_cluster, &trans->clusterLink(), msg );
    clusterCnv.persToTrans( &pers->m_cellCluster, &trans->cellClusterLink(), msg );
    jetCnv.persToTrans( &pers->m_jet, &trans->jetLink(), msg );
    tracksCnv.persToTrans( &pers->m_tracks, &trans->trackLinkVector(), msg );
    detailsCnv.persToTrans( &pers->m_tauDetails, &trans->tauDetailLinkVector(), msg );
    trans->setNumberOfTracks (pers->m_numberOfTracks);
    trans->setROIWord (pers->m_roiWord);

    if( getBit( pers->m_flags, 0 ) ) {
        std::vector<std::pair<TauJetParameters::TauID, double> > params; 
        params.reserve(pers->m_params.size());
        for (const auto& p : pers->m_params) {
          params.emplace_back( static_cast<TauJetParameters::TauID>( p.first),
                               p.second);
                                                                       
        }

        Analysis::TauPID* tauID = new Analysis::TauPID (std::move(params),
                                                        pers->m_isTauFlags,
                                                        pers->m_vetoFlags);
        trans->setTauID (tauID);
    } else {
	trans->setTauID( 0 );
    }

    if( getBit( pers->m_flags, 2 ) )
	trans->setAuthor( TauJetParameters::tauRec );
    if( getBit( pers->m_flags, 3 ) )
	trans->setAuthor( TauJetParameters::tau1P3P );
    conversionTracksCnv.persToTrans( 
	&pers->m_conversionTracks, 
	&trans->conversionTrackLinkVector(), msg );
}

void TauJetCnv_p3::transToPers( const Analysis::TauJet *trans,
				  TauJet_p3 *pers,
				  MsgStream &msg )
{
    momCnv.transToPers( &trans->momentumBase(), &pers->m_momentum, msg );
    partBaseCnv.transToPers( &trans->particleBase(), &pers->m_particleBase,
			     msg );
    const ElementLink<CaloClusterContainer> clusterLink = trans->clusterLink();
    clusterCnv.transToPers( &clusterLink, &pers->m_cluster, msg );
    const ElementLink<CaloClusterContainer> cellClusterLink = trans->cellClusterLink();
    clusterCnv.transToPers( &cellClusterLink, &pers->m_cellCluster, msg );
    const ElementLink<JetCollection> jetLink = trans->jetLink();
    jetCnv.transToPers( &jetLink, &pers->m_jet, msg );
    tracksCnv.transToPers( &trans->trackLinkVector(), &pers->m_tracks, msg );
    detailsCnv.transToPers( &trans->tauDetailLinkVector(), &pers->m_tauDetails, msg );
    pers->m_numberOfTracks = trans->numberOfTracks();
    pers->m_roiWord = trans->ROIWord();
    pers->m_params.clear();


    if( trans->tauID() ) {
	setBit( pers->m_flags, 0, true );

        pers->m_vetoFlags = trans->tauID()->vetoFlags().to_ulong();
        pers->m_isTauFlags = trans->tauID()->isTauFlags().to_ulong();

        pers->m_params.reserve (trans->tauID()->params().size());
        for (const auto& p : trans->tauID()->params()) {
            pers->m_params.emplace_back (static_cast<int>( p.first ), p.second);
        }
    } else {
	setBit( pers->m_flags, 0, false );
    }
    if( trans->hasAuthor( TauJetParameters::tauRec ) )
	setBit( pers->m_flags, 2, true );
    if( trans->hasAuthor( TauJetParameters::tau1P3P ) )
	setBit( pers->m_flags, 3, true );
    conversionTracksCnv.transToPers(
        &trans->conversionTrackLinkVector(), 
	&pers->m_conversionTracks, msg );
}
