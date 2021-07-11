/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigComboHypoTool.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

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
  ATH_MSG_DEBUG("AcceptAll  = " << m_acceptAll );
  ATH_MSG_DEBUG("Variable   = " << m_varTag );
  ATH_MSG_DEBUG("UseCut min = " << m_useMin );
  ATH_MSG_DEBUG("UseCut max = " << m_useMax );
  ATH_MSG_DEBUG("varCut min = " << m_varMin );
  ATH_MSG_DEBUG("varCut max = " << m_varMax );
  ATH_MSG_DEBUG("LegA       = " << m_legA );
  ATH_MSG_DEBUG("LegB       = " << m_legB );

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }

  if (m_legA==""){
    ATH_MSG_ERROR("LegA not set!");
    return StatusCode::FAILURE;
  }
  if (m_legB==""){
    ATH_MSG_ERROR("LegB not set!");
    return StatusCode::FAILURE;
  }
  if ((!m_useMin) && (!m_useMax)){
    ATH_MSG_ERROR("Trying to configure the Tool without setting UseMin and UseMax!");
    return StatusCode::FAILURE;
  }

  fillVarMap();
  if (m_varMap.find(m_varTag) == m_varMap.end()){
    ATH_MSG_ERROR("The variable is not present in the comboHypoVars list");
    return StatusCode::FAILURE;
  }
  m_var = m_varMap[m_varTag];

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

bool TrigComboHypoTool::executeAlg(const std::vector<Combo::LegDecision>& combination) const {
  ATH_MSG_DEBUG("On combination executeAlg");
  auto varOfAccepted  = Monitored::Scalar( m_varTag+"OfAccepted"   , -1.0 );
  auto varOfProcessed = Monitored::Scalar( m_varTag+"OfProcessed"  , -1.0 );
  auto monitorIt      = Monitored::Group( m_monTool, varOfAccepted, varOfProcessed);

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
    if (legName == m_legA){
      if (legA_index != -1) {
        ATH_MSG_WARNING("More than one Decision Object supplied on " << legName 
          << "! E.g. from a 2muX leg or similar. Do not know which one to use, will take the last one!");
      }
      legA_index = i;
    }else  if (legName == m_legB){
      if (legB_index != -1) {
        ATH_MSG_WARNING("More than one Decision Object supplied on " << legName 
          << "! E.g. from a 2muX leg or similar. Do not know which one to use, will take the last one!");
      }
      legB_index = i;
    }
    ATH_MSG_DEBUG("\t Leg: "<< legName <<", full name:"<<combId.name());
  }

  if ( legA_index<0){
    ATH_MSG_ERROR("legA = "<< m_legA << " NOT FOUND!");
    return false;
  }
  if ( legB_index<0){
    ATH_MSG_ERROR("legB = "<< m_legB << " NOT FOUND!");
    return false;
  }

  float  eta1, phi1, pt1;
  float  eta2, phi2, pt2;
  
  auto EL= combination[legA_index].second;    

  if (m_isLegA_MET) {
    auto legA_pLink = TrigCompositeUtils::findLink<xAOD::TrigMissingETContainer>( *EL, featureString() ).link;
    if (!legA_pLink.isValid()){
      ATH_MSG_ERROR("link for "<<m_legA<<"is MET");
      ATH_MSG_ERROR("link for "<<m_legA<<" not valid");
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
      ATH_MSG_ERROR("link for "<<m_legA<<"is not MET");
      ATH_MSG_ERROR("link for "<<m_legA<<" not valid");
      return false;
    }
    eta1 = (*legA_pLink)->p4().Eta();
    phi1 = (*legA_pLink)->p4().Phi();
    pt1  = (*legA_pLink)->p4().Pt();

  }
  ATH_MSG_DEBUG("link for legA: "<<m_legA<<" is valid");

  EL = combination[legB_index].second;

  if (m_isLegB_MET) {
    auto legB_pLink = TrigCompositeUtils::findLink<xAOD::TrigMissingETContainer>( *EL, featureString() ).link;
    if (!legB_pLink.isValid()){
      ATH_MSG_ERROR("link for "<<m_legB<<" not valid");
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
      ATH_MSG_ERROR("link for "<<m_legB<<"is not MET");
      ATH_MSG_ERROR("link for "<<m_legB<<" not valid");
      return false;
    }
    eta2 = (*legB_pLink)->p4().Eta();
    phi2 = (*legB_pLink)->p4().Phi();
    pt2  = (*legB_pLink)->p4().Pt();
  }
  ATH_MSG_DEBUG("link for legB: "<<m_legB<<" is valid");

  // apply the cut
  bool  pass(true);
  float value(-9999.);

  switch(m_var){
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
      value    = std::sqrt(ex*ex + ey*ey);
      break;
    }
  default:
    ATH_MSG_ERROR("m_varTag =  "<<m_varTag<<" not present in the list of comboHypoVars");
    return false;
  }
  varOfProcessed = value;

  ATH_MSG_DEBUG("Found a combination with " << varOfProcessed);

  if (m_useMin && m_useMax){
    if (varOfProcessed < m_varMin || varOfProcessed > m_varMax){ 
      ATH_MSG_DEBUG("Combination failed var cut: "<< m_varTag <<"= "<< varOfProcessed << " not in [" << m_varMin << "," <<  m_varMax << "]");
      pass=false;
    }else{
      varOfAccepted = value;
      ATH_MSG_DEBUG( m_varTag <<"= "<< varOfAccepted << " is  within [" <<m_varMin<< "," << m_varMax << "] This selection passed! ");
    }
  }else if (m_useMin){
    if (varOfProcessed < m_varMin ){ 
      ATH_MSG_DEBUG("Combination failed var cut: "<< m_varTag <<"= "<< varOfProcessed << " not > " << m_varMin);
      pass=false;
    }else{
      varOfAccepted = value;
      ATH_MSG_DEBUG( m_varTag <<"= "<< varOfAccepted << " < " <<m_varMin << " This selection passed! ");
    }
  }else if (m_useMax){
    if (varOfProcessed > m_varMax){ 
      ATH_MSG_DEBUG("Combination failed var cut: "<< m_varTag <<"= "<< varOfProcessed << " not < " << m_varMax);
      pass=false;
    }else{
      varOfAccepted = value;
      ATH_MSG_DEBUG( m_varTag <<"= "<< varOfAccepted << " > " << m_varMax << " This selection passed! ");
    }
  }
  return pass;

}




