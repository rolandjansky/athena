/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   TauJetCnv_p2.cxx
// author: Ilija Vukotic
// date:   April 2008
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "tauEvent/TauJet.h"
#undef private
#undef protected

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
    clusterCnv.persToTrans( &pers->m_cluster, &trans->m_cluster, msg );
    clusterCnv.persToTrans( &pers->m_cellCluster, &trans->m_cellCluster, msg );
    jetCnv.persToTrans( &pers->m_jet, &trans->m_jet, msg );
    tracksCnv.persToTrans( &pers->m_tracks, &trans->m_tracks,  msg );
    detailsCnv.persToTrans( &pers->m_tauDetails, &trans->m_tauDetails,  msg );
    trans->m_numberOfTracks = pers->m_numberOfTracks;
    trans->m_roiWord = pers->m_roiWord;

    if( getBit( pers->m_flags, 0 ) )
    {
        if (trans->tauID() == 0)
          trans->setTauID (new Analysis::TauPID);

        trans->m_tauID->m_vetoFlags = pers->m_vetoFlags;

        std :: vector<std :: pair<int, double> > :: const_iterator it;
        trans->m_tauID->m_params.clear();
        trans->m_tauID->m_params.reserve(pers->m_params.size());
        for( it = pers->m_params.begin(); it != pers->m_params.end(); ++it )
        {
            std :: pair<TauJetParameters :: TauID, double> param;
            param.first = static_cast<TauJetParameters :: TauID>( (*it).first );
            param.second = (*it).second;
            trans->m_tauID->m_params.push_back( param );
        }
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
    clusterCnv.transToPers( &trans->m_cluster, &pers->m_cluster, msg );
    clusterCnv.transToPers( &trans->m_cellCluster, &pers->m_cellCluster, msg );
    jetCnv.transToPers( &trans->m_jet, &pers->m_jet, msg );
    tracksCnv.transToPers( &trans->m_tracks, &pers->m_tracks, msg );
    detailsCnv.transToPers( &trans->m_tauDetails, &pers->m_tauDetails, msg );
    pers->m_numberOfTracks = trans->m_numberOfTracks;
    pers->m_roiWord = trans->m_roiWord;
    pers->m_params.clear();
	
    if( trans->m_tauID )
    {
        setBit( pers->m_flags, 0, true );

        pers->m_vetoFlags = trans->m_tauID->m_vetoFlags.to_ulong();

        pers->m_params.reserve (trans->m_tauID->m_params.size());
        std :: vector<std :: pair<TauJetParameters :: TauID, double> > :: const_iterator it;
        for( it = trans->m_tauID->m_params.begin(); it != trans->m_tauID->m_params.end(); ++it )
        {
            std :: pair<int, double> param;
            param.first = static_cast<int>( (*it).first );
            param.second = (*it).second;
            pers->m_params.push_back( param );
        }
    }
    else
        setBit( pers->m_flags, 0, false );

    if( trans->hasAuthor( TauJetParameters :: tauRec ) )
        setBit( pers->m_flags, 2, true );
    if( trans->hasAuthor( TauJetParameters :: tau1P3P ) )
        setBit( pers->m_flags, 3, true );
}
