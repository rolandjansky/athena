/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauJetBDTEvaluator.h"

TauJetBDTEvaluator::TauJetBDTEvaluator(const std::string& name)
  : TauRecToolBase(name)
  , m_myBdt(0)
{
  declareProperty("weightsFile", m_weightsFile="");
  declareProperty("minNTracks", m_minNTracks=0);
  declareProperty("maxNTracks", m_maxNTracks=999);
  declareProperty("minAbsTrackEta", m_minAbsTrackEta=-1);
  declareProperty("maxAbsTrackEta", m_maxAbsTrackEta=-1);
  declareProperty("outputVarName", m_outputVarName="BDTJetScore");
  declareProperty("GradiantBoost", m_isGrad=true, "Gradiant if true, else AdaBoost Classification");
  declareProperty("defaultValue", m_dummyValue=-1111, "if no weightsFile, then set all taus to this value nTrack/eta ignored");
}

//________________________________________
StatusCode TauJetBDTEvaluator::initialize(){

  //check if weightsFile is empty, if so, assume we apply dummyValue to all taus
  if(m_weightsFile.length()==0){
    ATH_MSG_INFO("No weights file set, will set all " << m_outputVarName << " to " << m_dummyValue );
    return StatusCode::SUCCESS;
  }

  //configure m_myBdt object if weights exists
  std::string full_path=find_file(m_weightsFile);
  m_myBdt = new tauRecTools::TRTBDT(full_path.c_str());
  if(m_myBdt->bdt==0) {
    ATH_MSG_FATAL("Couldn't configure BDT");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

SG::AuxElement::ConstAccessor<float> acc_absTrackEta("ABS_ETA_LEAD_TRACK");

//________________________________________
StatusCode TauJetBDTEvaluator::execute(xAOD::TauJet& xTau){
  //init output variable accessor
  static SG::AuxElement::Accessor<float> outputVar(m_outputVarName);

  if(m_myBdt==0) {
    (outputVar)(xTau) = m_dummyValue;
    return StatusCode::SUCCESS;
  }

  int nTracks = 0;
  xTau.detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
  if(nTracks<m_minNTracks) return StatusCode::SUCCESS;
  if(nTracks>m_maxNTracks) return StatusCode::SUCCESS;

  if( !inTrigger() ){
    float absTrackEta = acc_absTrackEta(xTau);
    if(m_minAbsTrackEta>=0. && absTrackEta < m_minAbsTrackEta) 
      return StatusCode::SUCCESS;
    if(m_maxAbsTrackEta>=0. && absTrackEta >= m_maxAbsTrackEta)
      return StatusCode::SUCCESS; 
  }

  m_myBdt->updateVariables(xTau);
  float response = (m_isGrad ? m_myBdt->GetGradBoostMVA() : m_myBdt->GetClassification() );   
  (outputVar)(xTau) = response;
  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauJetBDTEvaluator::finalize(){ 
  if(m_myBdt) delete m_myBdt; 
  return StatusCode::SUCCESS;
}
