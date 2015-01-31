/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauJetCnv_p5.cxx
///
/// Implementation of transient<->persistent converter
/// for Analysis::TauJet v5
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Noel Dawe
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by Lukasz Janyst in May 2009
///
/// $Id: TauJetCnv_p4.cxx,v 1.5 2009-03-02 17:13:27 binet Exp $


//trick to access private members in tau jet
#define private public
#define protected public
#include "tauEvent/TauJet.h"
#undef private
#undef protected

#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p2.h"
#include "ParticleEventTPCnv/ParticleBaseCnv_p2.h"
#include "tauEventTPCnv/TauJetCnv_p5.h"

#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"

//Pre-allocate converters
static HepLorentzVectorCnv_p1 hepLorentzVectorCnv;
static P4ImplEEtaPhiMCnv_p2 momCnv;
static ParticleBaseCnv_p2 partBaseCnv;
static ElementLinkCnv_p3<ElementLink<CaloClusterContainer> > clusterCnv;
static ElementLinkCnv_p3<ElementLink<JetCollection> > jetCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Analysis::TauDetailsContainer> > detailsCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > tracksCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > conversionTracksCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > seedCalo_tracksCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > seedTrk_tracksCnv;

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

void TauJetCnv_p5::persToTrans( const TauJet_p5 *pers,
        Analysis::TauJet *trans,
        MsgStream &msg )
{
    momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(), msg );
    partBaseCnv.persToTrans( &pers->m_particleBase, &trans->particleBase(), msg );
    clusterCnv.persToTrans( &pers->m_cluster, &trans->m_cluster, msg );
    clusterCnv.persToTrans( &pers->m_cellCluster, &trans->m_cellCluster, msg );
    jetCnv.persToTrans( &pers->m_jet, &trans->m_jet, msg );
    tracksCnv.persToTrans( &pers->m_tracks, &trans->m_tracks, msg );
    detailsCnv.persToTrans( &pers->m_tauDetails, &trans->m_tauDetails, msg );
    trans->m_numberOfTracks = pers->m_numberOfTracks;
    trans->m_roiWord = pers->m_roiWord;

    if( getBit( pers->m_flags, 0 ) ) {
        if (trans->tauID() == 0)
            trans->setTauID (new Analysis::TauPID);

        trans->m_tauID->m_vetoFlags = pers->m_vetoFlags;
        trans->m_tauID->m_isTauFlags = pers->m_isTauFlags;

        std::vector<std::pair<int, float> >::const_iterator it;
        trans->m_tauID->m_params.clear();
        trans->m_tauID->m_params.reserve(pers->m_params.size());
        for( it = pers->m_params.begin(); 
                it != pers->m_params.end(); ++it ) {
            std::pair<TauJetParameters::TauID, double> param;
            param.first = static_cast<TauJetParameters::TauID>( (*it).first );
            param.second = (*it).second;
            trans->m_tauID->m_params.push_back( param );
        }
    } else {
        trans->setTauID( 0 );
    }

    if( getBit( pers->m_flags, 2 ) )
        trans->setAuthor( TauJetParameters::tauRec );
    if( getBit( pers->m_flags, 3 ) )
        trans->setAuthor( TauJetParameters::tau1P3P );
    conversionTracksCnv.persToTrans( 
            &pers->m_conversionTracks, 
            &trans->m_conversionTracks, msg );
    seedCalo_tracksCnv.persToTrans( 
            &pers->m_seedCalo_tracks, 
            &trans->m_seedCalo_tracks, msg );
    seedTrk_tracksCnv.persToTrans( 
            &pers->m_seedTrk_tracks, 
            &trans->m_seedTrk_tracks, msg );

//     STILL NEED CONDITIONALS
    /*
     std::vector<std::pair<int, HepLorentzVector_p1> >::const_iterator it;
     trans->m_tauHLVStorage.clear();
     trans->m_tauHLVStorage.reserve(pers->m_tauHLVStorage.size());
     for( it = pers->m_tauHLVStorage.begin(); 
             it != pers->m_tauHLVStorage.end(); ++it )
         {
           CLHEP::HepLorentzVector dummy;
           hepLorentzVectorCnv.persToTrans(&((*it).second), &dummy,msg);
           std::pair<TauJetParameters::TauCalibType, CLHEP::HepLorentzVector> store;
           store.first = static_cast<TauJetParameters::TauCalibType>( (*it).first );
           store.second = dummy;
           trans->m_tauHLVStorage.push_back( store );
     }
     */
    CLHEP::HepLorentzVector hlv;
    hepLorentzVectorCnv.persToTrans( &pers->m_tauHLV_jetseed, &hlv,msg);
    trans->m_tauHLVStorage.push_back ( std::make_pair(TauJetParameters::JetSeed,hlv) );
    hepLorentzVectorCnv.persToTrans( &pers->m_tauHLV_detaxis, &hlv,msg);
    trans->m_tauHLVStorage.push_back ( std::make_pair(TauJetParameters::DetectorAxis,hlv) );
    hepLorentzVectorCnv.persToTrans( &pers->m_tauHLV_intaxis, &hlv,msg);
    trans->m_tauHLVStorage.push_back ( std::make_pair(TauJetParameters::IntermediateAxis,hlv) );
    hepLorentzVectorCnv.persToTrans( &pers->m_tauHLV_tesaxis, &hlv,msg);
    trans->m_tauHLVStorage.push_back ( std::make_pair(TauJetParameters::TauEnergyScale,hlv) );
    hepLorentzVectorCnv.persToTrans( &pers->m_tauHLV_etaaxis, &hlv,msg);
    trans->m_tauHLVStorage.push_back ( std::make_pair(TauJetParameters::TauEtaCalib,hlv) );
}

void TauJetCnv_p5::transToPers( const Analysis::TauJet *trans,
        TauJet_p5 *pers,
        MsgStream &msg )
{
    momCnv.transToPers( &trans->momentumBase(), &pers->m_momentum, msg );
    partBaseCnv.transToPers( &trans->particleBase(), &pers->m_particleBase, msg );
    clusterCnv.transToPers( &trans->m_cluster, &pers->m_cluster, msg );
    clusterCnv.transToPers( &trans->m_cellCluster, &pers->m_cellCluster, msg );
    jetCnv.transToPers( &trans->m_jet, &pers->m_jet, msg );
    tracksCnv.transToPers( &trans->m_tracks, &pers->m_tracks, msg );
    detailsCnv.transToPers( &trans->m_tauDetails, &pers->m_tauDetails, msg );
    pers->m_numberOfTracks = trans->m_numberOfTracks;
    pers->m_roiWord = trans->m_roiWord;
    pers->m_params.clear();

    if( trans->m_tauID ) {
        setBit( pers->m_flags, 0, true );

        pers->m_vetoFlags = trans->m_tauID->m_vetoFlags.to_ulong();
        pers->m_isTauFlags = trans->m_tauID->m_isTauFlags.to_ulong();

        pers->m_params.reserve (trans->m_tauID->m_params.size());
        std::vector<std::pair<
            TauJetParameters::TauID, double> >::const_iterator it;
        for( it = trans->m_tauID->m_params.begin(); 
                it != trans->m_tauID->m_params.end(); ++it ) {
            std::pair<int, float> param;
            param.first = static_cast<int>( (*it).first );
            param.second = (*it).second;
            pers->m_params.push_back( param );
        }
    } else {
        setBit( pers->m_flags, 0, false );
    }
    if( trans->hasAuthor( TauJetParameters::tauRec ) )
        setBit( pers->m_flags, 2, true );
    if( trans->hasAuthor( TauJetParameters::tau1P3P ) )
        setBit( pers->m_flags, 3, true );
    conversionTracksCnv.transToPers(
            &trans->m_conversionTracks, 
            &pers->m_conversionTracks, msg );
    seedCalo_tracksCnv.transToPers(
            &trans->m_seedCalo_tracks, 
            &pers->m_seedCalo_tracks, msg );
    seedTrk_tracksCnv.transToPers(
            &trans->m_seedTrk_tracks, 
            &pers->m_seedTrk_tracks, msg );


//       STILL NEED CONDITIONALS
    /*
        pers->m_tauHLVStorage.clear();
        pers->m_tauHLVStorage.reserve (trans->m_tauHLVStorage.size());
        std::vector<std::pair<TauJetParameters::TauCalibType, CLHEP::HepLorentzVector> >::const_iterator it;
        for( it = trans->m_tauHLVStorage.begin(); it != trans->m_tauHLVStorage.end(); ++it ) {
          HepLorentzVector_p1 dummy;
          hepLorentzVectorCnv.transToPers( &((*it).second), &dummy, msg);

          std::pair<int, HepLorentzVector_p1 > store;
          store.first = static_cast<int>( (*it).first );
          store.second = dummy;
          pers->m_tauHLVStorage.push_back( store );
        }
        */
    std::vector<std::pair<TauJetParameters::TauCalibType, CLHEP::HepLorentzVector> >::const_iterator it;
    for( it = trans->m_tauHLVStorage.begin(); it != trans->m_tauHLVStorage.end(); ++it ) {
        if ((*it).first==TauJetParameters::JetSeed)
            hepLorentzVectorCnv.transToPers( &((*it).second), &pers->m_tauHLV_jetseed, msg);
        else if ((*it).first==TauJetParameters::DetectorAxis)
            hepLorentzVectorCnv.transToPers( &((*it).second), &pers->m_tauHLV_detaxis, msg);
        else if ((*it).first==TauJetParameters::IntermediateAxis)
            hepLorentzVectorCnv.transToPers( &((*it).second), &pers->m_tauHLV_intaxis, msg);
        else if ((*it).first==TauJetParameters::TauEnergyScale)
            hepLorentzVectorCnv.transToPers( &((*it).second), &pers->m_tauHLV_tesaxis, msg);
        else if ((*it).first==TauJetParameters::TauEtaCalib)
            hepLorentzVectorCnv.transToPers( &((*it).second), &pers->m_tauHLV_etaaxis, msg);
    }

}
