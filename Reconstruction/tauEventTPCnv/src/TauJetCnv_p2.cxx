/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetCnv_p2.cxx
// author: Ilija Vukotic
// date:   April 2008
//-----------------------------------------------------------------------------

#include "tauEvent/TauJet.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p2.h"
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"
#include "tauEventTPCnv/TauJetCnv_p2.h"

static P4ImplEEtaPhiMCnv_p2                                                       momCnv;
static ParticleBaseCnv_p1                                                         partBaseCnv;
static ElementLinkCnv_p1<ElementLink<CaloClusterContainer> >                      clusterCnv;
static ElementLinkCnv_p1<ElementLink<JetCollection> >                             jetCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Analysis::TauDetailsContainer> > detailsCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> >   tracksCnv;

static void setBit( unsigned char &field, unsigned num, bool val )
{
        if( val )
            field |= (1 << num);
        else
            field &= ~(1 << num);
}

static bool getBit( unsigned char field, unsigned num )
{
    return (field & (1 << num)) ? true : false;
}

void TauJetCnv_p2 :: persToTrans( const TauJet_p2    *pers,
                                  Analysis :: TauJet *trans,
                                  MsgStream          &msg )
{
    momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(), msg );
    partBaseCnv.persToTrans( &pers->m_particleBase, &trans->particleBase(),
			     msg );
    clusterCnv.persToTrans( &pers->m_cluster, &trans->clusterLink(), msg );
    clusterCnv.persToTrans( &pers->m_cellCluster, &trans->cellClusterLink(), msg );
    jetCnv.persToTrans( &pers->m_jet, &trans->jetLink(), msg );
    tracksCnv.persToTrans( &pers->m_tracks, &trans->trackLinkVector(),  msg );
    detailsCnv.persToTrans( &pers->m_tauDetails, &trans->tauDetailLinkVector(),  msg );
    trans->setNumberOfTracks (pers->m_numberOfTracks);
    trans->setROIWord (pers->m_roiWord);

    if( getBit( pers->m_flags, 0 ) )
    {
        std::vector<std::pair<TauJetParameters::TauID, double> > params; 
        params.reserve(pers->m_params.size());
        for (const auto& p : pers->m_params) {
          params.emplace_back( static_cast<TauJetParameters::TauID>( p.first),
                               p.second);
                                                                       
        }

        Analysis::TauPID* tauID = new Analysis::TauPID (std::move(params),
                                                        std::bitset<32>(),
                                                        pers->m_vetoFlags);
        trans->setTauID (tauID);
    }
    else
        trans->setTauID( 0 );

    if( getBit( pers->m_flags, 2 ) )
        trans->setAuthor( TauJetParameters :: tauRec );
    if( getBit( pers->m_flags, 3 ) )
        trans->setAuthor( TauJetParameters :: tau1P3P );
}

void TauJetCnv_p2 :: transToPers( const Analysis :: TauJet *trans,
                                  TauJet_p2                *pers,
                                  MsgStream                &msg )
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
	
    if( trans->tauID() )
    {
        setBit( pers->m_flags, 0, true );

        pers->m_vetoFlags = trans->tauID()->vetoFlags().to_ulong();

        pers->m_params.reserve (trans->tauID()->params().size());
        for (const auto& p : trans->tauID()->params()) {
            pers->m_params.emplace_back (static_cast<int>( p.first ), p.second);
        }
    }
    else
        setBit( pers->m_flags, 0, false );

    if( trans->hasAuthor( TauJetParameters :: tauRec ) )
        setBit( pers->m_flags, 2, true );
    if( trans->hasAuthor( TauJetParameters :: tau1P3P ) )
        setBit( pers->m_flags, 3, true );
}
