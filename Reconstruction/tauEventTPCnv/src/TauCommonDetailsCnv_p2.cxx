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

#include "tauEvent/TauCommonDetails.h"
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
    int first = (int)((*i)*1e-5);
    int second = ((int)(*i)) % 100000;
    i++;
    trans->setSeedTrk_etChrgEM01Trk (i, i+first);
    i+=first;
    trans->setSeedTrk_etResChrgEMTrk (i, i+second);
    i+=second;

    trans->setIpZ0SinThetaSigLeadTrk          (*i++);
    trans->setEtOverPtLeadTrk                 (*i++);
    trans->setEtOverPtLeadLooseTrk            (*i++);
    trans->setLeadTrkPt                       (*i++);
    trans->setLeadLooseTrkPt                  (*i++);
    trans->setIpSigLeadTrk                    (*i++);
    trans->setIpSigLeadLooseTrk               (*i++);
    trans->setChrgLooseTrk                    (*i++);
    trans->setMassTrkSys                      (*i++);
    trans->setTrkWidth2                       (*i++);
    trans->setTrFlightPathSig                 (*i++);
    trans->setEtEflow                         (*i++);
    trans->setMEflow                          (*i++);
    trans->setEle_E237E277                    (*i++);
    trans->setEle_PresamplerFraction          (*i++);
    trans->setEle_ECALFirstFraction           (*i++);
    trans->setSeedCalo_nIsolLooseTrk          (*i++);
    trans->setSeedCalo_EMRadius               (*i++);
    trans->setSeedCalo_hadRadius              (*i++);
    trans->setSeedCalo_etEMAtEMScale          (*i++);
    trans->setSeedCalo_etHadAtEMScale         (*i++);
    trans->setSeedCalo_isolFrac               (*i++);
    trans->setSeedCalo_centFrac               (*i++);
    trans->setSeedCalo_stripWidth2            (*i++);
    trans->setSeedCalo_etEMCalib              (*i++);
    trans->setSeedCalo_etHadCalib             (*i++);
    trans->setSeedCalo_eta                    (*i++);
    trans->setSeedCalo_phi                    (*i++);
    trans->setSeedCalo_trkAvgDist             (*i++);
    trans->setSeedCalo_trkRmsDist             (*i++);
    trans->setSeedTrk_EMRadius                (*i++);
    trans->setSeedTrk_isolFrac                (*i++);
    trans->setSeedTrk_etChrgHadOverSumTrkPt   (*i++);
    trans->setSeedTrk_isolFracWide            (*i++);
    trans->setSeedTrk_etHadAtEMScale          (*i++);
    trans->setSeedTrk_etEMAtEMScale           (*i++);
    trans->setSeedTrk_etEMCL                  (*i++);
    trans->setSeedTrk_etChrgEM                (*i++);
    trans->setSeedTrk_etNeuEM                 (*i++);
    trans->setSeedTrk_etResNeuEM              (*i++);
    trans->setSeedTrk_hadLeakEt               (*i++);
    trans->setSeedTrk_sumEMCellEtOverLeadTrkPt(*i++);
    trans->setSeedTrk_secMaxStripEt           (*i++);
    trans->setSeedTrk_stripWidth2             (*i++);
    trans->setSeedTrk_etChrgHad               (*i++);
    trans->setSeedTrk_etIsolEM                (*i++);
    trans->setSeedTrk_etIsolHad               (*i++);

    union toConv {int i; float f;} m;
    m.f = (*i++); trans->setSeedCalo_nStrip       (m.i);
    m.f = (*i++); trans->setSeedTrk_nStrip        (m.i);
    m.f = (*i++); trans->setSeedTrk_nOtherCoreTrk (m.i);
    m.f = (*i++); trans->setSeedTrk_nIsolTrk      (m.i);

    if (i < pers->m_allTheData.end()) {
        trans->setNumCells                        (static_cast<unsigned int>((*i++)));
        trans->setNumTopoClusters                 (static_cast<unsigned int>((*i++)));
        trans->setNumEffTopoClusters              (*i++);   
        trans->setTopoInvMass                     (*i++);   
        trans->setEffTopoInvMass                  (*i++);   
        trans->setTopoMeanDeltaR                  (*i++);   
        trans->setEffTopoMeanDeltaR               (*i++);   
    }

    if (i < pers->m_allTheData.end())
    {
        trans->set_EM_TES_scale                   (*i++);
        trans->set_LC_TES_precalib                (*i++);
        trans->setSeedCalo_lead2ClusterEOverAllClusterE  (*i++);
        trans->setSeedCalo_lead3ClusterEOverAllClusterE (*i++);
        trans->setSeedCalo_caloIso                 (*i++);
        trans->setSeedCalo_caloIsoCorrected        (*i++);
        trans->setSeedCalo_dRmax                   (*i++);
    }
    else {
        trans->set_EM_TES_scale                    (0);
        trans->set_LC_TES_precalib                 (0);
        trans->setSeedCalo_lead2ClusterEOverAllClusterE (0);
        trans->setSeedCalo_lead3ClusterEOverAllClusterE (0);
        trans->setSeedCalo_caloIso                 (0);
        trans->setSeedCalo_caloIsoCorrected        (0);
        trans->setSeedCalo_dRmax                   (0);
    }

    if (i < pers->m_allTheData.end())
    {
	trans->setCellBasedEnergyRing1		  (*i++);
	trans->setCellBasedEnergyRing2		  (*i++);
	trans->setCellBasedEnergyRing3		  (*i++);
	trans->setCellBasedEnergyRing4		  (*i++);
	trans->setCellBasedEnergyRing5		  (*i++);
	trans->setCellBasedEnergyRing6		  (*i++);
	trans->setCellBasedEnergyRing7		  (*i++);
        trans->setTRT_NHT_OVER_NLT                (*i++);
    }
    else {
        trans->setCellBasedEnergyRing1		  (0);
        trans->setCellBasedEnergyRing2		  (0);
        trans->setCellBasedEnergyRing3		  (0);
        trans->setCellBasedEnergyRing4		  (0);
        trans->setCellBasedEnergyRing5		  (0);
        trans->setCellBasedEnergyRing6		  (0);
        trans->setCellBasedEnergyRing7		  (0);
        trans->setTRT_NHT_OVER_NLT                (0);
    }
    if (i < pers->m_allTheData.end())
    {
        trans->setTauJetVtxFraction                (*i++);
    }
    else {
        trans->setTauJetVtxFraction                (0);
    }

    assert (i <= pers->m_allTheData.end());

    tracksCnv.persToTrans( &pers->m_looseTrk, &trans->looseTrk(), msg );
    conversionTracksCnv.persToTrans( &pers->m_looseConvTrk, &trans->looseConvTrk(),msg );
    seedCalo_wideTrkCnv.persToTrans( &pers->m_seedCalo_wideTrk, &trans->seedCalo_wideTrk(),msg );
    seedTrk_wideTrkCnv.persToTrans( &pers->m_seedTrk_wideTrk, &trans->seedTrk_wideTrk(),msg );
    otherTrkCnv.persToTrans( &pers->m_otherTrk, &trans->otherTrk(),msg );
    clusterCnv.persToTrans( &pers->m_cellEM012Cluster, &trans->cellEM012ClusterLink(), msg );
    CLHEP::HepLorentzVector sumPi0Vec;
    hepLorentzVectorCnv.persToTrans( &pers->m_sumPi0Vec, &sumPi0Vec,msg);
    trans->setSumPi0Vec (sumPi0Vec);
    trans->setSecVtx (createTransFromPStore( &m_recVertexCnv, pers->m_secVtx, msg));
    clusterVectCnv.persToTrans( &pers->m_pi0, &trans->pi0LinkVec(),msg );
}

void TauCommonDetailsCnv_p2::transToPers(
        const Analysis::TauCommonDetails *trans, 
        TauCommonDetails_p2 *pers, MsgStream &msg ) {

    float sizeToWrite=trans->seedTrk_etChrgEM01TrkVector().size() * 100000 + 
      trans->seedTrk_etResChrgEMTrkVector().size();

    // 49 floats + one sizeToWrite + 6 ints = 56
    // +2+5 new variables (floats) = 63 
    // +7 new variables = 70
    // +1 new variable = 71
    // +1 new variable = 72
    pers->m_allTheData.reserve(72 + trans->seedTrk_etChrgEM01TrkVector().size() + 
                               trans->seedTrk_etResChrgEMTrkVector().size());

    pers->m_allTheData.push_back(sizeToWrite);

    std::copy(trans->seedTrk_etChrgEM01TrkVector().begin(), 
              trans->seedTrk_etChrgEM01TrkVector().end(), 
              std::back_inserter(pers->m_allTheData));
    std::copy(trans->seedTrk_etResChrgEMTrkVector().begin(), 
              trans->seedTrk_etResChrgEMTrkVector().end(), 
              std::back_inserter(pers->m_allTheData));

    pers->m_allTheData.push_back(trans->ipZ0SinThetaSigLeadTrk());
    pers->m_allTheData.push_back(trans->etOverPtLeadTrk());
    pers->m_allTheData.push_back(trans->etOverPtLeadLooseTrk());
    pers->m_allTheData.push_back(trans->leadTrkPt());
    pers->m_allTheData.push_back(trans->leadLooseTrkPt());
    pers->m_allTheData.push_back(trans->ipSigLeadTrk());
    pers->m_allTheData.push_back(trans->ipSigLeadLooseTrk());
    pers->m_allTheData.push_back(trans->chrgLooseTrk());
    pers->m_allTheData.push_back(trans->massTrkSys());
    pers->m_allTheData.push_back(trans->trkWidth2());
    pers->m_allTheData.push_back(trans->trFlightPathSig());
    pers->m_allTheData.push_back(trans->etEflow());
    pers->m_allTheData.push_back(trans->mEflow());
    pers->m_allTheData.push_back(trans->ele_E237E277());
    pers->m_allTheData.push_back(trans->ele_PresamplerFraction());
    pers->m_allTheData.push_back(trans->ele_ECALFirstFraction());   
    pers->m_allTheData.push_back(trans->seedCalo_nIsolLooseTrk());
    pers->m_allTheData.push_back(trans->seedCalo_EMRadius());
    pers->m_allTheData.push_back(trans->seedCalo_hadRadius());
    pers->m_allTheData.push_back(trans->seedCalo_etEMAtEMScale());
    pers->m_allTheData.push_back(trans->seedCalo_etHadAtEMScale());
    pers->m_allTheData.push_back(trans->seedCalo_isolFrac());
    pers->m_allTheData.push_back(trans->seedCalo_centFrac());
    pers->m_allTheData.push_back(trans->seedCalo_stripWidth2());
    pers->m_allTheData.push_back(trans->seedCalo_etEMCalib());
    pers->m_allTheData.push_back(trans->seedCalo_etHadCalib());
    pers->m_allTheData.push_back(trans->seedCalo_eta());
    pers->m_allTheData.push_back(trans->seedCalo_phi());
    pers->m_allTheData.push_back(trans->seedCalo_trkAvgDist());
    pers->m_allTheData.push_back(trans->seedCalo_trkRmsDist());
    pers->m_allTheData.push_back(trans->seedTrk_EMRadius());
    pers->m_allTheData.push_back(trans->seedTrk_isolFrac());
    pers->m_allTheData.push_back(trans->seedTrk_etChrgHadOverSumTrkPt());
    pers->m_allTheData.push_back(trans->seedTrk_isolFracWide());
    pers->m_allTheData.push_back(trans->seedTrk_etHadAtEMScale());
    pers->m_allTheData.push_back(trans->seedTrk_etEMAtEMScale());
    pers->m_allTheData.push_back(trans->seedTrk_etEMCL());
    pers->m_allTheData.push_back(trans->seedTrk_etChrgEM());
    pers->m_allTheData.push_back(trans->seedTrk_etNeuEM());
    pers->m_allTheData.push_back(trans->seedTrk_etResNeuEM());
    pers->m_allTheData.push_back(trans->seedTrk_hadLeakEt());
    pers->m_allTheData.push_back(trans->seedTrk_sumEMCellEtOverLeadTrkPt());
    pers->m_allTheData.push_back(trans->seedTrk_secMaxStripEt());
    pers->m_allTheData.push_back(trans->seedTrk_stripWidth2());
    pers->m_allTheData.push_back(trans->seedTrk_etChrgHad());
    pers->m_allTheData.push_back(trans->seedTrk_etIsolEM());
    pers->m_allTheData.push_back(trans->seedTrk_etIsolHad());
    union toConv {int i; float f;} m;
    m.i= trans->seedCalo_nStrip(); pers->m_allTheData.push_back(m.f);
    m.i= trans->seedTrk_nStrip(); pers->m_allTheData.push_back(m.f);
    m.i= trans->seedTrk_nOtherCoreTrk(); pers->m_allTheData.push_back(m.f);
    m.i= trans->seedTrk_nIsolTrk(); pers->m_allTheData.push_back(m.f);

    pers->m_allTheData.push_back(trans->numCells());   
    pers->m_allTheData.push_back(trans->numTopoClusters());   
    pers->m_allTheData.push_back(trans->numEffTopoClusters());   
    pers->m_allTheData.push_back(trans->topoInvMass());   
    pers->m_allTheData.push_back(trans->effTopoInvMass());   
    pers->m_allTheData.push_back(trans->topoMeanDeltaR());   
    pers->m_allTheData.push_back(trans->effTopoMeanDeltaR());   

    pers->m_allTheData.push_back(trans->EM_TES_scale());
    pers->m_allTheData.push_back(trans->LC_TES_precalib());
    
    pers->m_allTheData.push_back(trans->seedCalo_lead2ClusterEOverAllClusterE());
    pers->m_allTheData.push_back(trans->seedCalo_lead3ClusterEOverAllClusterE());
    pers->m_allTheData.push_back(trans->seedCalo_caloIso());
    pers->m_allTheData.push_back(trans->seedCalo_caloIsoCorrected());
    pers->m_allTheData.push_back(trans->seedCalo_dRmax());
    pers->m_allTheData.push_back(trans->cellBasedEnergyRing1());
    pers->m_allTheData.push_back(trans->cellBasedEnergyRing2());
    pers->m_allTheData.push_back(trans->cellBasedEnergyRing3());
    pers->m_allTheData.push_back(trans->cellBasedEnergyRing4());
    pers->m_allTheData.push_back(trans->cellBasedEnergyRing5());
    pers->m_allTheData.push_back(trans->cellBasedEnergyRing6());
    pers->m_allTheData.push_back(trans->cellBasedEnergyRing7());
    pers->m_allTheData.push_back(trans->TRT_NHT_OVER_NLT());

    pers->m_allTheData.push_back(trans->TauJetVtxFraction());

    tracksCnv.transToPers( &trans->looseTrk(), &pers->m_looseTrk, msg );
    conversionTracksCnv.transToPers( &trans->looseConvTrk(), &pers->m_looseConvTrk, msg );    
    seedCalo_wideTrkCnv.transToPers( &trans->seedCalo_wideTrk(), &pers->m_seedCalo_wideTrk, msg );
    seedTrk_wideTrkCnv.transToPers( &trans->seedTrk_wideTrk(), &pers->m_seedTrk_wideTrk, msg );
    otherTrkCnv.transToPers( &trans->otherTrk(), &pers->m_otherTrk, msg );
    clusterCnv.transToPers( &trans->cellEM012ClusterLink(), &pers->m_cellEM012Cluster, msg );
    hepLorentzVectorCnv.transToPers( &trans->sumPi0Vec(), &pers->m_sumPi0Vec, msg );
    pers->m_secVtx = toPersistent(	&m_recVertexCnv, trans->secVtx(),msg);
    clusterVectCnv.transToPers( &trans->pi0LinkVec(),	&pers->m_pi0, msg );

}
