/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauJetBDTEvaluator.h"

TauJetBDTEvaluator::TauJetBDTEvaluator(const std::string& name)
  : TauRecToolBase(name)
  , m_mvaBDT(nullptr) {
  declareProperty("weightsFile", m_weightsFile="");
  declareProperty("minNTracks", m_minNTracks=0);
  declareProperty("maxNTracks", m_maxNTracks=999);
  declareProperty("minAbsTrackEta", m_minAbsTrackEta=-1);
  declareProperty("maxAbsTrackEta", m_maxAbsTrackEta=-1);
  declareProperty("outputVarName", m_outputVarName="BDTJetScore");
  declareProperty("defaultValue", m_dummyValue=-1111, "if no weightsFile, then set all taus to this value nTrack/eta ignored");
}

//________________________________________
StatusCode TauJetBDTEvaluator::initialize(){

  //check if weightsFile is empty, if so, assume we apply dummyValue to all taus
  if(m_weightsFile.length()==0){
    ATH_MSG_INFO("No weights file set, will set all " << m_outputVarName << " to " << m_dummyValue );
    return StatusCode::SUCCESS;
  }

  //configure m_mvaBDT object if weights exists
  std::string full_path=find_file(m_weightsFile);
  m_mvaBDT = std::make_unique<tauRecTools::BDTHelper>();
  ATH_CHECK(m_mvaBDT->initialize(full_path.c_str()));
  
  return StatusCode::SUCCESS;
}

const SG::AuxElement::ConstAccessor<float> acc_absTrackEta("ABS_ETA_LEAD_TRACK");

//________________________________________
StatusCode TauJetBDTEvaluator::execute(xAOD::TauJet& xTau) const {
  //init output variable accessor
  SG::AuxElement::Accessor<float> outputVar(m_outputVarName);

  if(m_mvaBDT==nullptr) {
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

  // Calculate BDT score, will be -999 when tau lacks variables
  float score = m_mvaBDT->getGradBoostMVA(xTau);
  (outputVar)(xTau) = score;
  
  return StatusCode::SUCCESS;
}
