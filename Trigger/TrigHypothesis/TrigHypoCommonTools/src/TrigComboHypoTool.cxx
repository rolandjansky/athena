/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigComboHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <Math/Vector4D.h>    // for LorentzVector
#include <Math/Vector4Dfwd.h> // PtEtaPhiM typedef
#include <Math/Vector2D.h>    // for DisplacementVector
#include <Math/Vector2Dfwd.h> // Polar2DVectorF typedef

#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include <vector>
#include <algorithm>
#include <cmath>

constexpr float invGeV = 1. / Gaudi::Units::GeV;

using namespace TrigCompositeUtils;

// Translate strings into enum values
const std::map<std::string, TrigComboHypoTool::ComboHypoVars> VarMap = {
  {"dR",   TrigComboHypoTool::ComboHypoVars::DR},
  {"invm", TrigComboHypoTool::ComboHypoVars::INVM},
  {"dphi", TrigComboHypoTool::ComboHypoVars::DPHI},
  {"mT",   TrigComboHypoTool::ComboHypoVars::MT},
  {"deta", TrigComboHypoTool::ComboHypoVars::DETA}
};

TrigComboHypoTool::TrigComboHypoTool(const std::string& type, 
				     const std::string& name, 
				     const IInterface*  parent)
  : ComboHypoToolBase(type, name, parent)
{}


bool TrigComboHypoTool::VarInfo::validate(std::string& errmsg) const {
  if (legA==0){
    errmsg = "legA ID not set!";
    return false;
  }
  if (legB==0){
    errmsg="legB ID not set!";
    return false;
  }
  if ((!useMin) && (!useMax)){
    errmsg="Trying to configure the Tool without setting at least one of UseMin or UseMax!";
    return false;
  }
  if (legA==legB && (legA_is_MET || legB_is_MET)) {
    errmsg = "Cannot specify the same MET leg for both sides!";
    return false;
  }
  return true;
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
    ATH_MSG_ERROR("Trying to configure the Tool with legA and legB vectors of different size!");
    return StatusCode::FAILURE;
  }
  if (m_useMin_vec.size() != m_useMax_vec.size()) {
    ATH_MSG_ERROR("Trying to configure the Tool with UseMin and UseMax vectors of different size!");
    return StatusCode::FAILURE;
  }
  if (m_legA_vec.size() != m_useMax_vec.size()) {
    ATH_MSG_ERROR("Trying to configure the Tool with legA/B and UseMax/Min vectors of different size!");
    return StatusCode::FAILURE;
  }
  if (m_varTag_vec.size() != m_useMax_vec.size()) {
    ATH_MSG_ERROR("Trying to configure the Tool with varTag and UseMax/Min(LegA/B) vectors of different size!");
    return StatusCode::FAILURE;
  }

  for (size_t i=0; i<m_varTag_vec.size(); ++i){
    VarInfo info;
    info.index = i;
    if(!m_monTool_vec.empty()) {
      info.monToolName = m_monTool_vec[i].name();
    }
    if (VarMap.find(m_varTag_vec[i]) == VarMap.end()){
      ATH_MSG_ERROR("The variable is not present in the ComboHypoVars list");
      return StatusCode::FAILURE;
    }
    info.varTag = (m_varTag_vec[i]);
    info.var = VarMap.at(m_varTag_vec[i]);
    //
    info.useMin = m_useMin_vec[i];
    if(info.useMin) {info.varMin=m_varMin_vec[i];}
    info.useMax = m_useMax_vec[i];
    if(info.useMax) {info.varMax=m_varMax_vec[i];}
    //
    info.legA = m_legA_vec[i];
    info.legA_is_MET = m_isLegA_MET_vec[i];
    info.legB = m_legB_vec[i];
    info.legB_is_MET = m_isLegB_MET_vec[i];
    info.legsAreEqual = info.legA==info.legB;

    std::string validmsg{""};
    if(!info.validate(validmsg)) {
      ATH_MSG_ERROR(validmsg);
      return StatusCode::FAILURE;
    }

    m_varInfo_vec.push_back(info);
  }
  ATH_MSG_DEBUG("Initialization completed successfully");
  
  return StatusCode::SUCCESS;
}


bool TrigComboHypoTool::executeAlg(const Combination& combination) const {
  //loop over all the hypos
  bool lastDecision(true);
  std::vector<float> values;
  values.reserve(m_varInfo_vec.size());

  for (auto varInfo = m_varInfo_vec.cbegin(); varInfo!=m_varInfo_vec.cend() && lastDecision; ++varInfo){
    lastDecision = executeAlgStep(combination, *varInfo, values);
  }

  // Monitoring of variables for only accepted events
  if(lastDecision && !m_monTool_vec.empty()) {
    for (auto varInfo = m_varInfo_vec.cbegin(); varInfo!=m_varInfo_vec.cend(); ++varInfo){
      float value = values[varInfo->index];
      auto varOfAccepted  = Monitored::Scalar(m_varTag_vec[varInfo->index]+"OfAccepted", value );//varInfo->monToolName+"OfAccepted", value );
      auto monitorIt      = Monitored::Group (m_monTool_vec[varInfo->index], varOfAccepted);
      ATH_MSG_DEBUG( varInfo->varTag << " = " << value << " is in range " << varInfo->rangeStr() << ".");
      ATH_MSG_DEBUG("m_varTag_vec = "<< m_varTag_vec<<", values = "<<values << ", valIndex = "<< varInfo->index <<", monToolName = " << varInfo->monToolName << ", monToolVec = "<< m_monTool_vec);
    }
  }
  return lastDecision;
}


bool TrigComboHypoTool::executeAlgStep(const Combination& combination, const VarInfo& varInfo, std::vector<float> &vals) const {
  ATH_MSG_DEBUG("Executing selection " << varInfo.index << " of " << m_varInfo_vec.size() << ": " << varInfo.rangeStr());

  std::pair<KineInfo,KineInfo> kinepair;
  if(!fillPairKinematics(kinepair, combination, varInfo)) {
    ATH_MSG_ERROR("Failed to extract kinematics of feature pair!");
    return false;
  }

  if(msgLvl(MSG::DEBUG)) {
    float eta_check, phi_check, pt_check;
    std::tie(eta_check,phi_check,pt_check) = kinepair.first;
    msg() << MSG::DEBUG << "Test filled legA kinematics: pt " << pt_check*invGeV << ", eta " << eta_check << ", phi " << phi_check << endmsg;

    std::tie(eta_check,phi_check,pt_check) = kinepair.second;
    msg() << MSG::DEBUG << "Test filled legB kinematics: pt " << pt_check*invGeV << ", eta " << eta_check << ", phi " << phi_check << endmsg;
  }

  // apply the cut
  float value = compute(kinepair,varInfo.var);
  if(!m_monTool_vec.empty()) {
    auto varOfProcessed = Monitored::Scalar(m_varTag_vec[varInfo.index]+"OfProcessed"  , value );
    auto monitorIt      = Monitored::Group (m_monTool_vec[varInfo.index], varOfProcessed);
  }
  vals.push_back(value);
  bool pass = varInfo.test(value);

  ATH_MSG_DEBUG("Found a combination with " << value);
  if(!pass) {
    ATH_MSG_DEBUG("Combination failed var cut: " << varInfo.varTag << " = " << value << " not in range " << varInfo.rangeStr());
  }
  return pass;
}


/// Test function to compare decision ID with the legs to be used in var computation
bool testLegId(const Combo::LegDecision& d, uint32_t targetleg) {
  auto combId = HLT::Identifier(d.first);
  if(!TrigCompositeUtils::isLegId(combId)) return false;
  return combId.numeric() == targetleg;
}


bool TrigComboHypoTool::fillLegDecisions_sameLeg(std::pair<Combo::LegDecision,Combo::LegDecision>& legpair, const Combination& combination, uint32_t leg) const {
  Combination leg_features;
  if(m_skipLegCheck) {
    // If there is only one leg, the decision IDs don't have a leg ID
    std::copy(combination.begin(),combination.end(),std::back_inserter(leg_features));
  } else {
    // Extract the features matching the legs
    // We take all of them, so as to be able to check if there is any ambiguity
    auto isMyLeg = [&leg](const Combo::LegDecision& d) { return testLegId(d,leg); };
    std::copy_if(combination.begin(),combination.end(),std::back_inserter(leg_features),isMyLeg);
  }

  if (leg_features.size()==2) {
    legpair.first = leg_features[0];
    legpair.second = leg_features[1];
  } else {
    ATH_MSG_ERROR(leg_features.size() << " Decision Objects supplied on leg " << leg
                  << ", must be 2 for same-leg topo selection!");
    return false;
  }

  return true;
}


bool TrigComboHypoTool::fillLegDecisions_diffLeg(std::pair<Combo::LegDecision,Combo::LegDecision>& legpair, const Combination& combination, uint32_t legA, uint32_t legB) const {
  // Extract the features matching the legs
  // We take all of them, so as to be able to check if there is any ambiguity
  auto isLegA = [&legA](const Combo::LegDecision& d) { return testLegId(d,legA); };
  auto isLegB = [&legB](const Combo::LegDecision& d) { return testLegId(d,legB); };
  Combination legA_features, legB_features;

  std::copy_if(combination.begin(),combination.end(),std::back_inserter(legA_features),isLegA);
  if(legA_features.size()!=1) {
    ATH_MSG_ERROR(legA_features.size() << " Decision Objects supplied on leg " << legA
                  << ", must be 1 for different-leg topo selection!");
    return false;
  }

  std::copy_if(combination.begin(),combination.end(),std::back_inserter(legB_features),isLegB);
  if (legB_features.size()!=1) {
    ATH_MSG_ERROR(legB_features.size() << " Decision Objects supplied on leg " << legB
                  << ", must be 1 for different-leg topo selection!");
    return false;
  }

  legpair.first = legA_features[0];
  legpair.second = legB_features[0];

  return true;
}


bool TrigComboHypoTool::fillPairKinematics(std::pair<KineInfo,KineInfo>& kinepair, const Combination& combination, const VarInfo& varInfo) const {
    ATH_MSG_DEBUG("Decision objects available = "<< combination);
    // Check that there are enough features
    size_t nFeatures(combination.size());
    if (nFeatures < 2){
      ATH_MSG_ERROR("Number of Decision Objects passed is less than 2! Sum over decision objects on all legs = " << combination.size() );
      return false;
    }
    std::pair<Combo::LegDecision,Combo::LegDecision> legpair;
    if (varInfo.legsAreEqual) {fillLegDecisions_sameLeg(legpair,combination,varInfo.legA);}
    else {fillLegDecisions_diffLeg(legpair,combination,varInfo.legA,varInfo.legB);}
    ATH_MSG_DEBUG("Fill leg A kinematics");
    if(!fillKineInfo(kinepair.first,legpair.first,varInfo.legA_is_MET)) {
      ATH_MSG_ERROR("Failed to extract requisite kinematic info from leg " << varInfo.legA << "!");
      return false;
    }
    ATH_MSG_DEBUG("Fill leg B kinematics");
    if(!fillKineInfo(kinepair.second,legpair.second,varInfo.legB_is_MET)) {
      ATH_MSG_ERROR("Failed to extract requisite kinematic info from leg " << varInfo.legB << "!");
      return false;
    }
    return true;
  }


bool TrigComboHypoTool::fillKineInfo(TrigComboHypoTool::KineInfo& kinematics, Combo::LegDecision decision, bool isMET) const {
  float eta, phi, pt;
  if (isMET) {
    auto pLink = TrigCompositeUtils::findLink<xAOD::TrigMissingETContainer>( *decision.second, featureString() ).link;
    if (!pLink.isValid()){
      ATH_MSG_ERROR("link for MET not valid");
      return false;
    }
    ROOT::Math::XYVectorF metv((*pLink)->ex(),(*pLink)->ey());
    eta = FLOATDEFAULT;
    phi = metv.phi();
    pt  = metv.r();
  } else {
    auto pLink = TrigCompositeUtils::findLink<xAOD::IParticleContainer>( *decision.second, featureString() ).link;
    if (!pLink.isValid()){
      ATH_MSG_ERROR("link for IParticle not valid");
      return false;
    }
    eta = (*pLink)->p4().Eta();
    phi = (*pLink)->p4().Phi();
    pt  = (*pLink)->p4().Pt();
  }
  ATH_MSG_DEBUG("Filled kinematics with pt " << pt*invGeV << ", eta " << eta << ", phi " << phi);
  kinematics = std::make_tuple(eta,phi,pt);
  return true;
}


float TrigComboHypoTool::compute(const std::pair<KineInfo,KineInfo>& kinepair, ComboHypoVars var) const {
  const auto& [legA_kine,legB_kine] = kinepair;
  const auto& [eta1,phi1,pt1] = legA_kine;
  const auto& [eta2,phi2,pt2] = legB_kine;

  ATH_MSG_DEBUG("Leg A has pt " << pt1*invGeV << ", eta " << eta1 << ", phi " << phi1);
  ATH_MSG_DEBUG("Leg B has pt " << pt2*invGeV << ", eta " << eta2 << ", phi " << phi2);

  float value(0);
  switch(var) {
    case ComboHypoVars::DR:
      {
        value = xAOD::P4Helpers::deltaR(eta1,phi1,eta2,phi2);
        break;
      }
    case ComboHypoVars::DPHI:
      {
        value = xAOD::P4Helpers::deltaPhi(phi1,phi2);
        break;
      }
    case ComboHypoVars::INVM:
      {
        ROOT::Math::PtEtaPhiMVector p1(pt1,eta1,phi1,0.), p2(pt2,eta2,phi2,0.);
        value = (p1+p2).M()*invGeV; // Convert to GeV
        break;
      }
    case ComboHypoVars::MT:
      {
        ROOT::Math::Polar2DVectorF p1(pt1,phi1), p2(pt2,phi2);
        value = sqrt((p1+p2).Mag2())*invGeV; // Convert to GeV
        break;
      }
    case ComboHypoVars::DETA:
      {
        value = std::fabs(eta2-eta1);
        break;
      }
    default:
      {
        ATH_MSG_ERROR("Undefined variable requested -- should never happen!");
      }
  }
  return value;
}
