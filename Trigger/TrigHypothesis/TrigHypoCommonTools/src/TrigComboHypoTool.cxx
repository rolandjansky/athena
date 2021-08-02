/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigComboHypoTool.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

#include <vector>
#include <cmath>

using namespace TrigCompositeUtils;

TrigComboHypoTool::TrigComboHypoTool(const std::string& type, 
				     const std::string& name, 
				     const IInterface*  parent)
  : ComboHypoToolBase(type, name, parent)
{}

void   TrigComboHypoTool::fillVarMap(){
  m_varMap["dR"]   = comboHypoVars::DR;
  m_varMap["invm"] = comboHypoVars::INVM;
  m_varMap["dphi"] = comboHypoVars::DPHI;
  m_varMap["mT"]   = comboHypoVars::MT;
}

StatusCode TrigComboHypoTool::initialize()
{
  ATH_MSG_DEBUG("Variable   = " << m_varTag_vec );
  ATH_MSG_DEBUG("UseCut min = " << m_useMin_vec );
  ATH_MSG_DEBUG("UseCut max = " << m_useMax_vec );
  ATH_MSG_DEBUG("varCut min = " << m_varMin_vec );
  ATH_MSG_DEBUG("varCut max = " << m_varMax_vec );
  ATH_MSG_DEBUG("LegA       = " << m_legA_vec );
  ATH_MSG_DEBUG("LegB       = " << m_legB_vec );

  ATH_CHECK( m_monTool_vec.retrieve() );

  if (m_legA_vec.size() != m_legB_vec.size()) {
    ATH_MSG_ERROR("Trying to configure the Tool with legA and legB with different size!");
    return StatusCode::FAILURE;
  }
  if (m_useMin_vec.size() != m_useMax_vec.size()) {
    ATH_MSG_ERROR("Trying to configure the Tool with UseMin and UseMax with different size!");
    return StatusCode::FAILURE;
  }
  if (m_legA_vec.size() != m_useMax_vec.size()) {
    ATH_MSG_ERROR("Trying to configure the Tool with legA/B and UseMax/Min with different size!");
    return StatusCode::FAILURE;
  }
  if (m_varTag_vec.size() != m_useMax_vec.size()) {
    ATH_MSG_ERROR("Trying to configure the Tool with varTag and UseMax/Min(LegA/B) with different size!");
    return StatusCode::FAILURE;
  }
  fillVarMap();

  for (size_t i=0; i<m_legA_vec.size(); ++i){
    if (m_legA_vec[i]==""){
      ATH_MSG_ERROR("Array of legA IDs not set!");
      return StatusCode::FAILURE;
    }
    if (m_legB_vec[i]==""){
      ATH_MSG_ERROR("Array of legB IDs not set!");
      return StatusCode::FAILURE;
    }
    if ((!m_useMin_vec[i]) && (!m_useMax_vec[i])){
      ATH_MSG_ERROR("Trying to configure the Tool without setting UseMin and UseMax!");
      return StatusCode::FAILURE;
    }
    if (m_varMap.find(m_varTag_vec[i]) == m_varMap.end()){
      ATH_MSG_ERROR("The variable is not present in the comboHypoVars list");
      return StatusCode::FAILURE;
    }
    m_var_vec.push_back(m_varMap[m_varTag_vec[i]]);
  }
  ATH_MSG_DEBUG("Initialization completed successfully");
  
  return StatusCode::SUCCESS;
}

bool TrigComboHypoTool::executeAlg(const std::vector<Combo::LegDecision>& combination) const {
  //loop over all the hypos
  bool                 lastDecision(true);
  std::vector<float>   values;
  values.reserve(m_varTag_vec.size());

  for (size_t i=0; i<m_varTag_vec.size() && lastDecision; ++i){
    lastDecision = executeAlgStep(combination, i, values);
  }
  
  //now fill the monitoring histograms with the accepted values
  for (size_t i=0; i<m_varTag_vec.size() && lastDecision; ++i){
    std::string   monToolName=m_varTag_vec[i];
    if (m_varTag_vec.size()>1){
      monToolName = monToolName + m_legA_vec[i] + m_legB_vec[i];
    }
    auto varOfAccepted  = Monitored::Scalar(monToolName+"OfAccepted"   , -1.0 );
    auto monitorIt      = Monitored::Group (m_monTool_vec[i], varOfAccepted);
    varOfAccepted = values[i];
    ATH_MSG_DEBUG( m_varTag_vec[i] <<"= "<< varOfAccepted << " is  within [" <<m_varMin_vec[i]<< "," << m_varMax_vec[i] << "] This selection passed! ");
  }
  
  return lastDecision;
}

bool TrigComboHypoTool::executeAlgStep(const std::vector<Combo::LegDecision>& combination, size_t index, std::vector<float> &vals) const {
  ATH_MSG_DEBUG("On combination executeAlg");
  std::string   monToolName=m_varTag_vec[index];
  if (m_varTag_vec.size()>1){
    monToolName = monToolName + m_legA_vec[index] + m_legB_vec[index];
  }
  auto varOfProcessed = Monitored::Scalar(monToolName+"OfProcessed"  , -1.0 );
  auto monitorIt      = Monitored::Group (m_monTool_vec[index], varOfProcessed);

  //check that we found the two legs
  int nCombs(combination.size());
  if (nCombs < 2){
    ATH_MSG_ERROR("Number of Decision Objects passed is less than 2! Sum over decision objects on all legs = " << combination.size() );
    return false;
  }
  int           legA_index(-1), legB_index(-1);

  ATH_MSG_DEBUG("Decision objects available = "<< combination);
  for (int i=0; i<nCombs; ++i){
    auto combId = HLT::Identifier(combination[i].first);
    if (!TrigCompositeUtils::isLegId(combId))
      continue;
    std::string   legName = combId.name().substr(0,6);
    if (legName == m_legA_vec[index]){
      if (legA_index != -1) {
        ATH_MSG_WARNING("More than one Decision Object supplied on " << legName 
          << "! E.g. from a 2muX leg or similar. Do not know which one to use, will take the last one!");
      }
      legA_index = i;
    }else  if (legName == m_legB_vec[index]){
      if (legB_index != -1) {
        ATH_MSG_WARNING("More than one Decision Object supplied on " << legName 
          << "! E.g. from a 2muX leg or similar. Do not know which one to use, will take the last one!");
      }
      legB_index = i;
    }
    ATH_MSG_DEBUG("\t Leg: "<< legName <<", full name:"<<combId.name());
  }

  if ( legA_index<0){
    ATH_MSG_ERROR("legA = "<< m_legA_vec[index] << " NOT FOUND!");
    return false;
  }
  if ( legB_index<0){
    ATH_MSG_ERROR("legB = "<< m_legB_vec[index] << " NOT FOUND!");
    return false;
  }

  float  eta1, phi1, pt1;
  float  eta2, phi2, pt2;
  
  auto EL= combination[legA_index].second;    

  if (m_isLegA_MET_vec[index]) {
    auto legA_pLink = TrigCompositeUtils::findLink<xAOD::TrigMissingETContainer>( *EL, featureString() ).link;
    if (!legA_pLink.isValid()){
      ATH_MSG_ERROR("link for "<<m_legA_vec[index]<<"is MET");
      ATH_MSG_ERROR("link for "<<m_legA_vec[index]<<" not valid");
      return false;
    }
    float ex = (*legA_pLink)[0].ex()/1000.;//converting to GeV
    float ey = (*legA_pLink)[0].ey()/1000.;//converting to GeV
    eta1 = -9999.;
    phi1 = ex==0.0 && ey==0.0 ? 0.0: std::atan2(ey,ex);
    pt1  = std::sqrt(ex*ex+ey*ey);
  }else {
    auto legA_pLink = TrigCompositeUtils::findLink<xAOD::IParticleContainer>( *EL, featureString() ).link;
    if (!legA_pLink.isValid()){
      ATH_MSG_ERROR("link for "<<m_legA_vec[index]<<"is not MET");
      ATH_MSG_ERROR("link for "<<m_legA_vec[index]<<" not valid");
      return false;
    }
    eta1 = (*legA_pLink)->p4().Eta();
    phi1 = (*legA_pLink)->p4().Phi();
    pt1  = (*legA_pLink)->p4().Pt();

  }
  ATH_MSG_DEBUG("link for legA: "<<m_legA_vec[index]<<" is valid");

  EL = combination[legB_index].second;

  if (m_isLegB_MET_vec[index]) {
    auto legB_pLink = TrigCompositeUtils::findLink<xAOD::TrigMissingETContainer>( *EL, featureString() ).link;
    if (!legB_pLink.isValid()){
      ATH_MSG_ERROR("link for "<<m_legB_vec[index]<<" not valid");
      return false;
    }
    float ex = (*legB_pLink)[0].ex()/1000.;//converting to GeV
    float ey = (*legB_pLink)[0].ey()/1000.;//converting to GeV
    eta2 = -9999.;
    phi2 = ex==0.0 && ey==0.0 ? 0.0: std::atan2(ey,ex);
    pt2  = std::sqrt(ex*ex+ey*ey);
  }else {
    auto legB_pLink = TrigCompositeUtils::findLink<xAOD::IParticleContainer>( *EL, featureString() ).link;
    if (!legB_pLink.isValid()){
      ATH_MSG_ERROR("link for "<<m_legB_vec[index]<<"is not MET");
      ATH_MSG_ERROR("link for "<<m_legB_vec[index]<<" not valid");
      return false;
    }
    eta2 = (*legB_pLink)->p4().Eta();
    phi2 = (*legB_pLink)->p4().Phi();
    pt2  = (*legB_pLink)->p4().Pt();
  }
  ATH_MSG_DEBUG("link for legB: "<<m_legB_vec[index]<<" is valid");

  // apply the cut
  bool  pass(true);
  float value(-9999.);

  switch(m_var_vec[index]){
  case comboHypoVars::DR:
    {
      float dEta = eta2 - eta1;
      float dPhi = -remainder( -phi1 + phi2, 2*M_PI );
      value      = std::sqrt(dEta*dEta + dPhi*dPhi);
      break;
    }
  case comboHypoVars::INVM:
    {
      value = std::sqrt(2.*pt1*pt2*(std::cosh(eta1 - eta2) - std::cos(phi1 - phi2) ) )*1e-3; // Convert to GeV
      break;
    }
  case comboHypoVars::DPHI:
    {
      value = std::fabs(remainder( -phi1 + phi2, 2*M_PI ));
      break;
    }
  case comboHypoVars::MT:
    {
      float ex = pt1*std::cos(phi1) + pt2*std::cos(phi2);
      float ey = pt1*std::sin(phi1) + pt2*std::sin(phi2);
      value    = std::sqrt(ex*ex + ey*ey)/1000.;//converting to GeV   
      break;
    }
  default:
    ATH_MSG_ERROR("m_varTag =  "<<m_varTag_vec[index]<<" not present in the list of comboHypoVars");
    return false;
  }
  varOfProcessed = value;
  vals.push_back(value);

  ATH_MSG_DEBUG("Found a combination with " << varOfProcessed);

  if (m_useMin_vec[index] && m_useMax_vec[index]){
    if (varOfProcessed < m_varMin_vec[index] || varOfProcessed > m_varMax_vec[index]){ 
      ATH_MSG_DEBUG("Combination failed var cut: "<< m_varTag_vec[index] <<"= "<< varOfProcessed << " not in [" << m_varMin_vec[index] << "," <<  m_varMax_vec[index] << "]");
      pass=false;
    }
  }else if (m_useMin_vec[index]){
    if (varOfProcessed < m_varMin_vec[index] ){ 
      ATH_MSG_DEBUG("Combination failed var cut: "<< m_varTag_vec[index] <<"= "<< varOfProcessed << " not > " << m_varMin_vec[index]);
      pass=false;
    }
  }else if (m_useMax_vec[index]){
    if (varOfProcessed > m_varMax_vec[index]){ 
      ATH_MSG_DEBUG("Combination failed var cut: "<< m_varTag_vec[index] <<"= "<< varOfProcessed << " not < " << m_varMax_vec[index]);
      pass=false;
    }
  }
  return pass;

}




