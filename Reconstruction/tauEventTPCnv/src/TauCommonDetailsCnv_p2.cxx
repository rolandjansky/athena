/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails_p1.h
///
/// Implementation of transient<->persistent converter
/// for Analysis::TauCommonDetails
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Noel Dawe
/// @author Thomas Burgess
/// @author Ilija Vukotic

//trick to access private members in tau common details
#define private public
#define protected public
#include "tauEvent/TauCommonDetails.h"
#undef private
#undef protected

#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "EventCommonTPCnv/HepLorentzVector_p1.h"
#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "tauEventTPCnv/TauCommonDetailsCnv_p2.h"

//Pre allocate converters
static HepLorentzVectorCnv_p1 hepLorentzVectorCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > tracksCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > conversionTracksCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > seedCalo_wideTrkCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > seedTrk_wideTrkCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > otherTrkCnv;
static ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > clusterCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<CaloClusterContainer> > clusterVectCnv;

TauCommonDetailsCnv_p2::TauCommonDetailsCnv_p2()
    :  T_AthenaPoolTPPolyCnvBase<Analysis::TauDetails,
    Analysis::TauCommonDetails,
    TauCommonDetails_p2>(),
    m_recVertexCnv(0)
{}

TauCommonDetailsCnv_p2::~TauCommonDetailsCnv_p2(){}

void TauCommonDetailsCnv_p2::persToTrans(
        const TauCommonDetails_p2 *pers,
        Analysis::TauCommonDetails *trans,
        MsgStream &msg ) {

    std::vector<float>::const_iterator i=pers->m_allTheData.begin();
    int first = (int)((*i)/100000);
    int second = ((int)(*i)) % 100000;
    i++;
    trans->m_seedTrk_etChrgEM01Trk.resize( first );
    std::copy(i, i+first, trans->m_seedTrk_etChrgEM01Trk.begin());
    i+=first;
    trans->m_seedTrk_etResChrgEMTrk.resize( second );
    std::copy(i, i+second, trans->m_seedTrk_etResChrgEMTrk.begin());
    i+=second;

    trans->m_ipZ0SinThetaSigLeadTrk           = (*i++);
    trans->m_etOverPtLeadTrk                  = (*i++);
    trans->m_etOverPtLeadLooseTrk             = (*i++);
    trans->m_leadTrkPt                        = (*i++);
    trans->m_leadLooseTrkPt                   = (*i++);
    trans->m_ipSigLeadTrk                     = (*i++);
    trans->m_ipSigLeadLooseTrk                = (*i++);
    trans->m_chrgLooseTrk                     = (*i++);
    trans->m_massTrkSys                       = (*i++);
    trans->m_trkWidth2                        = (*i++);
    trans->m_trFlightPathSig                  = (*i++);
    trans->m_etEflow                          = (*i++);
    trans->m_mEflow                           = (*i++);
    trans->m_Ele_E237E277                     = (*i++);
    trans->m_Ele_PresamplerFraction           = (*i++);
    trans->m_Ele_ECALFirstFraction            = (*i++);
    trans->m_seedCalo_nIsolLooseTrk           = (*i++);
    trans->m_seedCalo_EMRadius                = (*i++);
    trans->m_seedCalo_hadRadius               = (*i++);
    trans->m_seedCalo_etEMAtEMScale           = (*i++);
    trans->m_seedCalo_etHadAtEMScale          = (*i++);
    trans->m_seedCalo_isolFrac                = (*i++);
    trans->m_seedCalo_centFrac                = (*i++);
    trans->m_seedCalo_stripWidth2             = (*i++);
    trans->m_seedCalo_etEMCalib               = (*i++);
    trans->m_seedCalo_etHadCalib              = (*i++);
    trans->m_seedCalo_eta                     = (*i++);
    trans->m_seedCalo_phi                     = (*i++);
    trans->m_seedCalo_trkAvgDist              = (*i++);
    trans->m_seedCalo_trkRmsDist              = (*i++);
    trans->m_seedTrk_EMRadius                 = (*i++);
    trans->m_seedTrk_isolFrac                 = (*i++);
    trans->m_seedTrk_etChrgHadOverSumTrkPt    = (*i++);
    trans->m_seedTrk_isolFracWide             = (*i++);
    trans->m_seedTrk_etHadAtEMScale           = (*i++);
    trans->m_seedTrk_etEMAtEMScale            = (*i++);
    trans->m_seedTrk_etEMCL                   = (*i++);
    trans->m_seedTrk_etChrgEM                 = (*i++);
    trans->m_seedTrk_etNeuEM                  = (*i++);
    trans->m_seedTrk_etResNeuEM               = (*i++);
    trans->m_seedTrk_hadLeakEt                = (*i++);
    trans->m_seedTrk_sumEMCellEtOverLeadTrkPt = (*i++);
    trans->m_seedTrk_secMaxStripEt            = (*i++);
    trans->m_seedTrk_stripWidth2              = (*i++);
    trans->m_seedTrk_etChrgHad                = (*i++);
    trans->m_seedTrk_etIsolEM                 = (*i++);
    trans->m_seedTrk_etIsolHad                = (*i++);

    union toConv {int i; float f;} m;
    m.f = (*i++); trans->m_seedCalo_nStrip       = m.i;
    m.f = (*i++); trans->m_seedTrk_nStrip        = m.i;
    m.f = (*i++); trans->m_seedTrk_nOtherCoreTrk = m.i;
    m.f = (*i++); trans->m_seedTrk_nIsolTrk      = m.i;

    if (i < pers->m_allTheData.end()) {
        trans->m_numCells                         = static_cast<unsigned int>((*i++));
        trans->m_numTopoClusters                  = static_cast<unsigned int>((*i++));
        trans->m_numEffTopoClusters               = (*i++);   
        trans->m_topoInvMass                      = (*i++);   
        trans->m_effTopoInvMass                   = (*i++);   
        trans->m_topoMeanDeltaR                   = (*i++);   
        trans->m_effTopoMeanDeltaR                = (*i++);   
    }

    if (i < pers->m_allTheData.end())
    {
        trans->m_EM_TES_scale                     = (*i++);
        trans->m_LC_TES_precalib                  = (*i++);
        trans->m_seedCalo_lead2ClusterEOverAllClusterE = (*i++);
        trans->m_seedCalo_lead3ClusterEOverAllClusterE = (*i++);
        trans->m_seedCalo_caloIso                 = (*i++);
        trans->m_seedCalo_caloIsoCorrected        = (*i++);
        trans->m_seedCalo_dRmax                   = (*i++);
    }
    else {
        trans->m_EM_TES_scale                     = 0;
        trans->m_LC_TES_precalib                  = 0;
        trans->m_seedCalo_lead2ClusterEOverAllClusterE = 0;
        trans->m_seedCalo_lead3ClusterEOverAllClusterE = 0;
        trans->m_seedCalo_caloIso                 = 0;
        trans->m_seedCalo_caloIsoCorrected        = 0;
        trans->m_seedCalo_dRmax                   = 0;
    }

    if (i < pers->m_allTheData.end())
    {
	trans->m_cellBasedEnergyRing1		  = (*i++);
	trans->m_cellBasedEnergyRing2		  = (*i++);
	trans->m_cellBasedEnergyRing3		  = (*i++);
	trans->m_cellBasedEnergyRing4		  = (*i++);
	trans->m_cellBasedEnergyRing5		  = (*i++);
	trans->m_cellBasedEnergyRing6		  = (*i++);
	trans->m_cellBasedEnergyRing7		  = (*i++);
        trans->m_TRT_NHT_OVER_NLT                 = (*i++);
    }
    else {
        trans->m_cellBasedEnergyRing1		  = 0;
        trans->m_cellBasedEnergyRing2		  = 0;
        trans->m_cellBasedEnergyRing3		  = 0;
        trans->m_cellBasedEnergyRing4		  = 0;
        trans->m_cellBasedEnergyRing5		  = 0;
        trans->m_cellBasedEnergyRing6		  = 0;
        trans->m_cellBasedEnergyRing7		  = 0;
        trans->m_TRT_NHT_OVER_NLT                 = 0;
    }
    if (i < pers->m_allTheData.end())
    {
        trans->m_TauJetVtxFraction                = (*i++);
    }
    else {
        trans->m_TauJetVtxFraction                = 0;
    }

    assert (i <= pers->m_allTheData.end());

    tracksCnv.persToTrans( &pers->m_looseTrk, &trans->m_looseTrk, msg );
    conversionTracksCnv.persToTrans( &pers->m_looseConvTrk, &trans->m_looseConvTrk,msg );
    seedCalo_wideTrkCnv.persToTrans( &pers->m_seedCalo_wideTrk, &trans->m_seedCalo_wideTrk,msg );
    seedTrk_wideTrkCnv.persToTrans( &pers->m_seedTrk_wideTrk, &trans->m_seedTrk_wideTrk,msg );
    otherTrkCnv.persToTrans( &pers->m_otherTrk, &trans->m_otherTrk,msg );
    clusterCnv.persToTrans( &pers->m_cellEM012Cluster, &trans->m_cellEM012Cluster, msg );
    hepLorentzVectorCnv.persToTrans( &pers->m_sumPi0Vec, &trans->m_sumPi0Vec,msg);
    trans->m_secVtx = createTransFromPStore( &m_recVertexCnv, pers->m_secVtx, msg);
    clusterVectCnv.persToTrans( &pers->m_pi0, &trans->m_pi0,msg );
}

void TauCommonDetailsCnv_p2::transToPers(
        const Analysis::TauCommonDetails *trans, 
        TauCommonDetails_p2 *pers, MsgStream &msg ) {

    float sizeToWrite=trans->m_seedTrk_etChrgEM01Trk.size() * 100000 + 
        trans->m_seedTrk_etResChrgEMTrk.size();

    // 49 floats + one sizeToWrite + 6 ints = 56
    // +2+5 new variables (floats) = 63 
    // +7 new variables = 70
    // +1 new variable = 71
    // +1 new variable = 72
    pers->m_allTheData.reserve(72 + trans->m_seedTrk_etChrgEM01Trk.size() + 
            trans->m_seedTrk_etResChrgEMTrk.size());

    pers->m_allTheData.push_back(sizeToWrite);

    std::copy(trans->m_seedTrk_etChrgEM01Trk.begin(), 
            trans->m_seedTrk_etChrgEM01Trk.end(), 
            std::back_inserter(pers->m_allTheData));
    std::copy(trans->m_seedTrk_etResChrgEMTrk.begin(), 
            trans->m_seedTrk_etResChrgEMTrk.end(), 
            std::back_inserter(pers->m_allTheData));

    pers->m_allTheData.push_back(trans->m_ipZ0SinThetaSigLeadTrk);
    pers->m_allTheData.push_back(trans->m_etOverPtLeadTrk);
    pers->m_allTheData.push_back(trans->m_etOverPtLeadLooseTrk);
    pers->m_allTheData.push_back(trans->m_leadTrkPt);
    pers->m_allTheData.push_back(trans->m_leadLooseTrkPt);
    pers->m_allTheData.push_back(trans->m_ipSigLeadTrk);
    pers->m_allTheData.push_back(trans->m_ipSigLeadLooseTrk);
    pers->m_allTheData.push_back(trans->m_chrgLooseTrk);
    pers->m_allTheData.push_back(trans->m_massTrkSys);
    pers->m_allTheData.push_back(trans->m_trkWidth2);
    pers->m_allTheData.push_back(trans->m_trFlightPathSig);
    pers->m_allTheData.push_back(trans->m_etEflow);
    pers->m_allTheData.push_back(trans->m_mEflow);
    pers->m_allTheData.push_back(trans->m_Ele_E237E277);
    pers->m_allTheData.push_back(trans->m_Ele_PresamplerFraction);
    pers->m_allTheData.push_back(trans->m_Ele_ECALFirstFraction);   
    pers->m_allTheData.push_back(trans->m_seedCalo_nIsolLooseTrk);
    pers->m_allTheData.push_back(trans->m_seedCalo_EMRadius);
    pers->m_allTheData.push_back(trans->m_seedCalo_hadRadius);
    pers->m_allTheData.push_back(trans->m_seedCalo_etEMAtEMScale);
    pers->m_allTheData.push_back(trans->m_seedCalo_etHadAtEMScale);
    pers->m_allTheData.push_back(trans->m_seedCalo_isolFrac);
    pers->m_allTheData.push_back(trans->m_seedCalo_centFrac);
    pers->m_allTheData.push_back(trans->m_seedCalo_stripWidth2);
    pers->m_allTheData.push_back(trans->m_seedCalo_etEMCalib);
    pers->m_allTheData.push_back(trans->m_seedCalo_etHadCalib);
    pers->m_allTheData.push_back(trans->m_seedCalo_eta);
    pers->m_allTheData.push_back(trans->m_seedCalo_phi);
    pers->m_allTheData.push_back(trans->m_seedCalo_trkAvgDist);
    pers->m_allTheData.push_back(trans->m_seedCalo_trkRmsDist);
    pers->m_allTheData.push_back(trans->m_seedTrk_EMRadius);
    pers->m_allTheData.push_back(trans->m_seedTrk_isolFrac);
    pers->m_allTheData.push_back(trans->m_seedTrk_etChrgHadOverSumTrkPt);
    pers->m_allTheData.push_back(trans->m_seedTrk_isolFracWide);
    pers->m_allTheData.push_back(trans->m_seedTrk_etHadAtEMScale);
    pers->m_allTheData.push_back(trans->m_seedTrk_etEMAtEMScale);
    pers->m_allTheData.push_back(trans->m_seedTrk_etEMCL);
    pers->m_allTheData.push_back(trans->m_seedTrk_etChrgEM);
    pers->m_allTheData.push_back(trans->m_seedTrk_etNeuEM);
    pers->m_allTheData.push_back(trans->m_seedTrk_etResNeuEM);
    pers->m_allTheData.push_back(trans->m_seedTrk_hadLeakEt);
    pers->m_allTheData.push_back(trans->m_seedTrk_sumEMCellEtOverLeadTrkPt);
    pers->m_allTheData.push_back(trans->m_seedTrk_secMaxStripEt);
    pers->m_allTheData.push_back(trans->m_seedTrk_stripWidth2);
    pers->m_allTheData.push_back(trans->m_seedTrk_etChrgHad);
    pers->m_allTheData.push_back(trans->m_seedTrk_etIsolEM);
    pers->m_allTheData.push_back(trans->m_seedTrk_etIsolHad);
    union toConv {int i; float f;} m;
    m.i= trans->m_seedCalo_nStrip; pers->m_allTheData.push_back(m.f);
    m.i= trans->m_seedTrk_nStrip; pers->m_allTheData.push_back(m.f);
    m.i= trans->m_seedTrk_nOtherCoreTrk; pers->m_allTheData.push_back(m.f);
    m.i= trans->m_seedTrk_nIsolTrk; pers->m_allTheData.push_back(m.f);

    pers->m_allTheData.push_back(trans->m_numCells);   
    pers->m_allTheData.push_back(trans->m_numTopoClusters);   
    pers->m_allTheData.push_back(trans->m_numEffTopoClusters);   
    pers->m_allTheData.push_back(trans->m_topoInvMass);   
    pers->m_allTheData.push_back(trans->m_effTopoInvMass);   
    pers->m_allTheData.push_back(trans->m_topoMeanDeltaR);   
    pers->m_allTheData.push_back(trans->m_effTopoMeanDeltaR);   

    pers->m_allTheData.push_back(trans->m_EM_TES_scale);
    pers->m_allTheData.push_back(trans->m_LC_TES_precalib);
    
    pers->m_allTheData.push_back(trans->m_seedCalo_lead2ClusterEOverAllClusterE);
    pers->m_allTheData.push_back(trans->m_seedCalo_lead3ClusterEOverAllClusterE);
    pers->m_allTheData.push_back(trans->m_seedCalo_caloIso);
    pers->m_allTheData.push_back(trans->m_seedCalo_caloIsoCorrected);
    pers->m_allTheData.push_back(trans->m_seedCalo_dRmax);
    pers->m_allTheData.push_back(trans->m_cellBasedEnergyRing1);
    pers->m_allTheData.push_back(trans->m_cellBasedEnergyRing2);
    pers->m_allTheData.push_back(trans->m_cellBasedEnergyRing3);
    pers->m_allTheData.push_back(trans->m_cellBasedEnergyRing4);
    pers->m_allTheData.push_back(trans->m_cellBasedEnergyRing5);
    pers->m_allTheData.push_back(trans->m_cellBasedEnergyRing6);
    pers->m_allTheData.push_back(trans->m_cellBasedEnergyRing7);
    pers->m_allTheData.push_back(trans->m_TRT_NHT_OVER_NLT);

    pers->m_allTheData.push_back(trans->m_TauJetVtxFraction);

    tracksCnv.transToPers( &trans->m_looseTrk, &pers->m_looseTrk, msg );
    conversionTracksCnv.transToPers( &trans->m_looseConvTrk, &pers->m_looseConvTrk, msg );    
    seedCalo_wideTrkCnv.transToPers( &trans->m_seedCalo_wideTrk, &pers->m_seedCalo_wideTrk, msg );
    seedTrk_wideTrkCnv.transToPers( &trans->m_seedTrk_wideTrk, &pers->m_seedTrk_wideTrk, msg );
    otherTrkCnv.transToPers( &trans->m_otherTrk, &pers->m_otherTrk, msg );
    clusterCnv.transToPers( &trans->m_cellEM012Cluster, &pers->m_cellEM012Cluster, msg );
    hepLorentzVectorCnv.transToPers( &trans->m_sumPi0Vec, &pers->m_sumPi0Vec, msg );
    pers->m_secVtx = toPersistent(	&m_recVertexCnv, trans->m_secVtx,msg);
    clusterVectCnv.transToPers( &trans->m_pi0,	&pers->m_pi0, msg );

}
