/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tau ID input variable calculator tool
 *
 * Author: Lorenz Hauswald
 */

#include "tauRecTools/TauIDVarCalculator.h"
#include "tauRecTools/HelperFunctions.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include "TLorentzVector.h"

#define GeV 1000
const float TauIDVarCalculator::LOW_NUMBER = -1111.;

TauIDVarCalculator::TauIDVarCalculator(const std::string& name):
  TauRecToolBase(name),
  m_incShowerSubtr(true)
{
  declareProperty("IncShowerSubtr", m_incShowerSubtr);
}

StatusCode TauIDVarCalculator::initialize()
{  
  ATH_CHECK(m_tauVertexCorrection.retrieve()); 
  return StatusCode::SUCCESS;
}

StatusCode TauIDVarCalculator::execute(xAOD::TauJet& tau) const
{
  static const SG::AuxElement::Accessor<float> acc_absipSigLeadTrk("absipSigLeadTrk");
  float ipSigLeadTrk=0.;
  if(!tau.detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk))
    return StatusCode::FAILURE;
  acc_absipSigLeadTrk(tau) = std::abs(ipSigLeadTrk);
  
  if(inTrigger()) return StatusCode::SUCCESS;
  
  //everything below is just for EleBDT!
  static const SG::AuxElement::Accessor<float> acc_absEtaLead("ABS_ETA_LEAD_TRACK"); 
  static const SG::AuxElement::Accessor<float> acc_leadTrackProbHT("leadTrackProbHT");
  static const SG::AuxElement::Accessor<float> acc_absDeltaEta("TAU_ABSDELTAETA");
  static const SG::AuxElement::Accessor<float> acc_absDeltaPhi("TAU_ABSDELTAPHI");
  static const SG::AuxElement::ConstAccessor<float> acc_sumEMCellEtOverLeadTrkPt("sumEMCellEtOverLeadTrkPt");
  static const SG::AuxElement::ConstAccessor<float> acc_etHadAtEMScale("etHadAtEMScale");
  static const SG::AuxElement::ConstAccessor<float> acc_etEMAtEMScale("etEMAtEMScale");
  static const SG::AuxElement::Accessor<float> acc_EMFractionAtEMScaleMOVEE3("EMFRACTIONATEMSCALE_MOVEE3");
  static const SG::AuxElement::Accessor<float> acc_seedTrkSecMaxStripEtOverPt("TAU_SEEDTRK_SECMAXSTRIPETOVERPT");
  static const SG::AuxElement::ConstAccessor<float> acc_secMaxStripEt("secMaxStripEt");
  static const SG::AuxElement::ConstAccessor<float> acc_centFrac("centFrac");

  // Will: Fixed variables for R21
  static const SG::AuxElement::Accessor<float> acc_EMFracFixed("EMFracFixed");
  static const SG::AuxElement::Accessor<float> acc_hadLeakFracFixed("hadLeakFracFixed");
  static const SG::AuxElement::Accessor<float> acc_etHotShotDR1("etHotShotDR1"); // replace secMaxStripEt
  static const SG::AuxElement::Accessor<float> acc_etHotShotWin("etHotShotWin"); // replace secMaxStripEt
  static const SG::AuxElement::Accessor<float> acc_etHotShotDR1OverPtLeadTrk("etHotShotDR1OverPtLeadTrk"); // replace TAU_SEEDTRK_SECMAXSTRIPETOVERPT
  static const SG::AuxElement::Accessor<float> acc_etHotShotWinOverPtLeadTrk("etHotShotWinOverPtLeadTrk"); // replace TAU_SEEDTRK_SECMAXSTRIPETOVERPT


  // EMFracFixed and eHad1AtEMScaleFixed (for acc_hadLeakFracFixed)
  // --------------------------------------------------------------
  // ECAL Layers: 0, 1, 2. Don't include 3 because it is mismodelled!
  std::vector<CaloSampling::CaloSample> EMSamps = { 
        CaloSampling::PreSamplerB, CaloSampling::PreSamplerE, 
        CaloSampling::EMB1, CaloSampling::EME1, 
        CaloSampling::EMB2, CaloSampling::EME2 };
  // All HCAL Layers
  std::vector<CaloSampling::CaloSample> HadSamps = { 
        CaloSampling::HEC0, CaloSampling::HEC1, CaloSampling::HEC2, CaloSampling::HEC3, 
        CaloSampling::TileBar0, CaloSampling::TileBar1, CaloSampling::TileBar2,
        CaloSampling::TileGap1, CaloSampling::TileGap2, CaloSampling::TileGap3,
        CaloSampling::TileExt0, CaloSampling::TileExt1, CaloSampling::TileExt2};
  // First HCal Layer
  std::vector<CaloSampling::CaloSample> Had1Samps = { 
        CaloSampling::HEC0, CaloSampling::TileBar0, CaloSampling::TileGap1, CaloSampling::TileExt0};

  if (! tau.jetLink().isValid()) {
    ATH_MSG_ERROR("Tau jet link is invalid.");
    return StatusCode::FAILURE;
  }
  const xAOD::Jet *jetSeed = tau.jet();
  
  const xAOD::Vertex* jetVertex = m_tauVertexCorrection->getJetVertex(*jetSeed);
  
  const xAOD::Vertex* tauVertex = nullptr;
  if (tau.vertexLink().isValid()) tauVertex = tau.vertex();
  
  TLorentzVector tauAxis = m_tauVertexCorrection->getTauAxis(tau);

  std::vector<const xAOD::CaloCluster*> clusterList;
  ATH_CHECK(tauRecTools::GetJetClusterList(jetSeed, clusterList, m_incShowerSubtr));
  
  float eEMAtEMScaleFixed = 0.;
  float eHadAtEMScaleFixed = 0.;
  float eHad1AtEMScaleFixed = 0.;

  for (const xAOD::CaloCluster* cluster  : clusterList) {
    TLorentzVector clusterP4 = m_tauVertexCorrection->getVertexCorrectedP4(*cluster, tauVertex, jetVertex);
    
    if( tauAxis.DeltaR(clusterP4) > 0.2 ) continue;
    
    for( auto samp : EMSamps )
      eEMAtEMScaleFixed += cluster->eSample(samp);
    for( auto samp : HadSamps )
      eHadAtEMScaleFixed += cluster->eSample(samp);
    for( auto samp : Had1Samps )
      eHad1AtEMScaleFixed += cluster->eSample(samp);  
  }
  acc_EMFracFixed(tau) = ( eEMAtEMScaleFixed + eHadAtEMScaleFixed ) != 0. ? 
      eEMAtEMScaleFixed / ( eEMAtEMScaleFixed + eHadAtEMScaleFixed ) : LOW_NUMBER;
 
  if(tau.nTracks() > 0){
    const xAOD::TrackParticle* track = tau.track(0)->track();
    acc_absEtaLead(tau) = std::abs( track->eta() );
    acc_absDeltaEta(tau) = std::abs( track->eta() - tau.eta() );
    acc_absDeltaPhi(tau) = std::abs( track->p4().DeltaPhi(tau.p4()) );
    //EMFRACTIONATEMSCALE_MOVEE3:
    float etEMScale1 = acc_etEMAtEMScale(tau);
    float etEMScale2 = acc_etHadAtEMScale(tau);
    float tau_sumETCellsLAr = acc_sumEMCellEtOverLeadTrkPt(tau) * track->pt();
    float tau_E3 = tau_sumETCellsLAr - etEMScale1;
    float tau_seedCalo_etHadAtEMScale_noE3 = etEMScale2 - tau_E3;
    float tau_seedCalo_etEMAtEMScale_yesE3 = etEMScale1 + tau_E3;
    acc_EMFractionAtEMScaleMOVEE3(tau) = tau_seedCalo_etEMAtEMScale_yesE3 / (tau_seedCalo_etEMAtEMScale_yesE3 + tau_seedCalo_etHadAtEMScale_noE3);
    //TAU_SEEDTRK_SECMAXSTRIPETOVERPT:
    acc_seedTrkSecMaxStripEtOverPt(tau) = (track->pt() != 0.) ? acc_secMaxStripEt(tau) / track->pt() : LOW_NUMBER;

    float fTracksEProbabilityHT;
    track->summaryValue( fTracksEProbabilityHT, xAOD::eProbabilityHT);
    acc_leadTrackProbHT(tau) = fTracksEProbabilityHT;
    
    // hadLeakFracFixed
    acc_hadLeakFracFixed(tau) = (track->p4().P() != 0.) ? eHad1AtEMScaleFixed / track->p4().P() : LOW_NUMBER;

    // HOT SHOTS!!!!!
    // --------------
    // Get track position extrapolated to EM1
    const xAOD::TauTrack* tauTrack = tau.track(0);
    float etaCalo = -10.0;
    float phiCalo = -10.0;
    if( not tauTrack->detail(xAOD::TauJetParameters::CaloSamplingEtaEM, etaCalo))
        ATH_MSG_WARNING("Failed to retrieve extrapolated chargedPFO eta");
    if( not tauTrack->detail(xAOD::TauJetParameters::CaloSamplingPhiEM, phiCalo))
        ATH_MSG_WARNING("Failed to retrieve extrapolated chargedPFO phi");
    ATH_MSG_DEBUG("track EM " << ", eta: " << etaCalo << ", phi: " << phiCalo );
    
    // Get hottest shot in dR<0.1 and in 0.05 x 0.1 window
    float etHotShotDR1 = 0.;
    float etHotShotWin = 0.;
    for( auto shotLink : tau.shotPFOLinks() ){
        if( not shotLink.isValid() ){
            ATH_MSG_WARNING("Invalid shotLink");
            continue;
        }
        const xAOD::PFO* shot = *shotLink;
        float etShot = 0.;
        shot->attribute(xAOD::PFODetails::tauShots_pt3, etShot);
       
        // In dR < 0.1
        if(xAOD::P4Helpers::deltaR(*shot, etaCalo, phiCalo, false) and etShot > etHotShotDR1){
          etHotShotDR1 = etShot;
        }
        // In 0.012 x 0.1 window
        if(std::abs(shot->eta() - etaCalo) > 0.012 ) continue;
        if(std::abs(xAOD::P4Helpers::deltaPhi(shot->phi(), phiCalo)) > 0.1 ) continue;
        if(etShot > etHotShotWin) etHotShotWin = etShot;
    }
    acc_etHotShotDR1(tau) = etHotShotDR1;
    acc_etHotShotWin(tau) = etHotShotWin;
    acc_etHotShotDR1OverPtLeadTrk(tau) = (track->pt() != 0.) ? etHotShotDR1 / track->pt() : LOW_NUMBER;
    acc_etHotShotWinOverPtLeadTrk(tau) = (track->pt() != 0.) ? etHotShotWin / track->pt() : LOW_NUMBER;

  }
  else{
    acc_absEtaLead(tau) = LOW_NUMBER;
    acc_absDeltaEta(tau) = LOW_NUMBER;
    acc_absDeltaPhi(tau) = LOW_NUMBER;
    acc_EMFractionAtEMScaleMOVEE3(tau) = LOW_NUMBER;
    acc_seedTrkSecMaxStripEtOverPt(tau) = LOW_NUMBER;
    acc_hadLeakFracFixed(tau) = LOW_NUMBER;
    acc_etHotShotDR1(tau) = LOW_NUMBER; 
    acc_etHotShotWin(tau) = LOW_NUMBER;
    acc_etHotShotDR1OverPtLeadTrk(tau) = LOW_NUMBER; 
    acc_etHotShotWinOverPtLeadTrk(tau) = LOW_NUMBER; 
  }
 
  return StatusCode::SUCCESS;
}
