/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tau ID input variable calculator tool
 *
 * Author: Lorenz Hauswald
 */

#include "TauDiscriminant/TauIDVarCalculator.h"
#include "xAODTracking/VertexContainer.h"  

const float TauIDVarCalculator::LOW_NUMBER = -1111.;

TauIDVarCalculator::TauIDVarCalculator(const std::string& name):
  TauDiscriToolBase(name),
  m_vertexContainerKey("PrimaryVertices"),
  m_nVtx(1)
{
  declareProperty("vertexContainerKey", m_vertexContainerKey);
}

StatusCode TauIDVarCalculator::eventInitialize()
{
  if(!inTrigger()){
    m_nVtx = int(LOW_NUMBER);
    const xAOD::VertexContainer* vertexContainer = nullptr;
    if( evtStore()->retrieve( vertexContainer, m_vertexContainerKey ).isFailure() ){
      ATH_MSG_ERROR("VertexContainer with key " << m_vertexContainerKey << " could not be retrieved.");
      return StatusCode::FAILURE;
    }
    if(vertexContainer){
      m_nVtx = 0;
      for( auto vertex : *vertexContainer ){
	if(!vertex) continue;
	int nTrackParticles = vertex->nTrackParticles();
	if( (nTrackParticles >= 4 && vertex->vertexType() == xAOD::VxType::PriVtx) ||
	    (nTrackParticles >= 2 && vertex->vertexType() == xAOD::VxType::PileUp)){
	  m_nVtx++;
	}
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TauIDVarCalculator::initialize()
{

  return StatusCode::SUCCESS;
}

StatusCode TauIDVarCalculator::execute(xAOD::TauJet& tau)
{
  //define accessors:
  static SG::AuxElement::Accessor<int> acc_numTrack("NUMTRACK");
  acc_numTrack(tau) = tau.nTracks();

  if(!inTrigger()){
    static SG::AuxElement::Accessor<int> acc_nVertex("NUMVERTICES");
    acc_nVertex(tau) = m_nVtx >= 0 ? m_nVtx : 0.;
  }
  
  if(inTrigger()){
    //for old trigger BDT:
    static SG::AuxElement::Accessor<int> acc_numWideTrk("NUMWIDETRACK");
    acc_numWideTrk(tau) = tau.nWideTracks();
  }
  
  //don't calculate EleBDT variables if run from TrigTauDiscriminant:
  if(inTrigger()) return StatusCode::SUCCESS;
  
  //everything below is just for EleBDT!
  static SG::AuxElement::Accessor<float> acc_absEtaLead("ABS_ETA_LEAD_TRACK");
  static SG::AuxElement::Accessor<float> acc_absDeltaEta("TAU_ABSDELTAETA");
  static SG::AuxElement::Accessor<float> acc_absDeltaPhi("TAU_ABSDELTAPHI");
  static SG::AuxElement::ConstAccessor<float> acc_sumEMCellEtOverLeadTrkPt("sumEMCellEtOverLeadTrkPt");
  static SG::AuxElement::ConstAccessor<float> acc_etHadAtEMScale("etHadAtEMScale");
  static SG::AuxElement::ConstAccessor<float> acc_etEMAtEMScale("etEMAtEMScale");
  static SG::AuxElement::Accessor<float> acc_EMFractionAtEMScaleMOVEE3("EMFRACTIONATEMSCALE_MOVEE3");
  static SG::AuxElement::Accessor<float> acc_seedTrkSecMaxStripEtOverPt("TAU_SEEDTRK_SECMAXSTRIPETOVERPT");
  static SG::AuxElement::ConstAccessor<float> acc_secMaxStripEt("secMaxStripEt");
  static SG::AuxElement::ConstAccessor<float> acc_centFrac("centFrac");
  static SG::AuxElement::ConstAccessor<float> acc_etOverPtLeadTrk("etOverPtLeadTrk");
  static SG::AuxElement::Accessor<float> acc_corrftrk("CORRFTRK");
  static SG::AuxElement::ConstAccessor<float> acc_hadLeakEt("hadLeakEt");
  static SG::AuxElement::Accessor<float> acc_newhadLeakEt("HADLEAKET");
  static SG::AuxElement::Accessor<float> acc_trtNhtOverNlt("TAU_TRT_NHT_OVER_NLT");
  static SG::AuxElement::Accessor<float> acc_centFracCorrected("CORRCENTFRAC");
  
  if(tau.nTracks() > 0){
    acc_absEtaLead(tau) = fabs( tau.track(0)->eta() );
    acc_absDeltaEta(tau) = fabs( tau.track(0)->eta() - tau.eta() );
    acc_absDeltaPhi(tau) = fabs( tau.track(0)->phi() - tau.phi() );
    //EMFRACTIONATEMSCALE_MOVEE3:
    float etEMScale1 = acc_etEMAtEMScale(tau);
    float etEMScale2 = acc_etHadAtEMScale(tau);
    float tau_sumETCellsLAr = acc_sumEMCellEtOverLeadTrkPt(tau) * tau.track(0)->pt();
    float tau_E3 = tau_sumETCellsLAr - etEMScale1;
    float tau_seedCalo_etHadAtEMScale_noE3 = etEMScale2 - tau_E3;
    float tau_seedCalo_etEMAtEMScale_yesE3 = etEMScale1 + tau_E3;
    acc_EMFractionAtEMScaleMOVEE3(tau) = tau_seedCalo_etEMAtEMScale_yesE3 / (tau_seedCalo_etEMAtEMScale_yesE3 + tau_seedCalo_etHadAtEMScale_noE3);
    //TAU_SEEDTRK_SECMAXSTRIPETOVERPT:
    acc_seedTrkSecMaxStripEtOverPt(tau) = (tau.track(0)->pt() != 0) ? acc_secMaxStripEt(tau) / tau.track(0)->pt() : LOW_NUMBER;
    //TRT_NHT_OVER_NLT:
    uint8_t numberOfTRTHighThresholdHits;
    tau.track(0)->summaryValue(numberOfTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits);
    uint8_t numberOfTRTHits;
    tau.track(0)->summaryValue(numberOfTRTHits, xAOD::numberOfTRTHits);
    uint8_t numberOfTRTHighThresholdOutliers;
    tau.track(0)->summaryValue(numberOfTRTHighThresholdOutliers, xAOD::numberOfTRTHighThresholdOutliers);
    uint8_t numberOfTRTOutliers;
    tau.track(0)->summaryValue(numberOfTRTOutliers, xAOD::numberOfTRTOutliers);
    acc_trtNhtOverNlt(tau) = (numberOfTRTHits + numberOfTRTOutliers) > 0 ?
      float( numberOfTRTHighThresholdHits + numberOfTRTHighThresholdOutliers) / float(numberOfTRTHits + numberOfTRTOutliers) : LOW_NUMBER;
    acc_newhadLeakEt(tau) = acc_hadLeakEt(tau);
  }else{
    acc_absEtaLead(tau) = LOW_NUMBER;
    acc_absDeltaEta(tau) = LOW_NUMBER;
    acc_absDeltaPhi(tau) = LOW_NUMBER;
    acc_newhadLeakEt(tau) = LOW_NUMBER;
    acc_EMFractionAtEMScaleMOVEE3(tau) = LOW_NUMBER;
    acc_seedTrkSecMaxStripEtOverPt(tau) = LOW_NUMBER;
    acc_trtNhtOverNlt(tau) = LOW_NUMBER;
  }
  //CORRFTRK
  float correction = m_nVtx != int(LOW_NUMBER) ? 0.003 * m_nVtx : 0.;
  float etOverpTLeadTrk = acc_etOverPtLeadTrk(tau);
  float ptLeadTrkOverEt = etOverpTLeadTrk > 0 ? 1. / etOverpTLeadTrk : LOW_NUMBER;
  acc_corrftrk(tau) = ptLeadTrkOverEt != -1111. ? ptLeadTrkOverEt + correction : ptLeadTrkOverEt;
  
  acc_centFracCorrected(tau) = tau.pt() < 80*1000. ? acc_centFrac(tau) + correction : acc_centFrac(tau);
  
  return StatusCode::SUCCESS;
}

StatusCode TauIDVarCalculator::finalize()
{
  return StatusCode::SUCCESS;
}
