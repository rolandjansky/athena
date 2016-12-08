/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetJvtEfficiency/JetJvtEfficiency.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"

namespace CP {

JetJvtEfficiency::JetJvtEfficiency( const std::string& name): asg::AsgTool( name ),
  m_appliedSystEnum(NONE),
  h_JvtHist(nullptr),
  h_EffHist(nullptr),
  m_jvtCut(0),
  m_rand(0)
  {
    declareProperty( "WorkingPoint", m_wp = "Medium"                             );
    declareProperty( "SFFile",m_file="JetJvtEfficiency/Moriond2016_v2/JvtSFFile.root");
    //declareProperty( "SFFile",m_file="JetJvtEfficiency/JvtSFFile.root");
    declareProperty( "ScaleFactorDecorationName", m_sf_decoration_name = "JvtSF"  );
    declareProperty( "RandomDropDecorationName", m_drop_decoration_name = "drop"  );
    declareProperty( "JetJvtMomentName",   m_jetJvtMomentName   = "Jvt"           );
    declareProperty( "JetEtaName",   m_jetEtaName   = "DetectorEta"               );
    declareProperty( "MaxPtForJvt",   m_maxPtForJvt   = 60e3                      );
    declareProperty( "TruthLabel",   m_truthLabel   = ""                      );
    applySystematicVariation(CP::SystematicSet()).ignore();
}

StatusCode JetJvtEfficiency::initialize(){
  m_sfDec = new SG::AuxElement::Decorator< float>(m_sf_decoration_name);
  m_dropDec = new SG::AuxElement::Decorator<char>(m_drop_decoration_name);
  m_doTruthRequirement = m_truthLabel!="";

  if (m_wp=="Loose") m_jvtCut = 0.11;
  else if (m_wp=="Medium") m_jvtCut = 0.59;
  else if (m_wp=="Tight") m_jvtCut = 0.91;
  else if (m_wp=="None") {
    m_jvtCut = -2;
    m_maxPtForJvt = 0;
    m_wp = "Medium";
  }
  else {
    ATH_MSG_ERROR("Invalid jvt working point name");
    return StatusCode::FAILURE;
  }

  if (m_file.empty()) return StatusCode::SUCCESS;

  std::string filename = PathResolverFindCalibFile(m_file);
  if (filename.empty()){
    ATH_MSG_WARNING ( "Could NOT resolve file name " << m_file);
  }  else{
    ATH_MSG_INFO(" Path found = "<<filename);
  }

  TFile *infile = TFile::Open(filename.c_str());

  if (m_wp=="Loose") h_JvtHist = dynamic_cast<TH2*>(infile->Get("JvtLoose")) ;
  else if (m_wp=="Medium") h_JvtHist = dynamic_cast<TH2*>(infile->Get("JvtDefault")) ;
  else if (m_wp=="Tight") h_JvtHist = dynamic_cast<TH2*>(infile->Get("JvtTight")) ;

  h_JvtHist->SetDirectory(0);

  if (m_wp=="Loose") h_EffHist = dynamic_cast<TH2*>(infile->Get("EffLoose"));
  else if (m_wp=="Medium") h_EffHist = dynamic_cast<TH2*>(infile->Get("EffDefault"));
  else if (m_wp=="Tight") h_EffHist = dynamic_cast<TH2*>(infile->Get("EffTight"));

  h_EffHist->SetDirectory(0);

  if (!addAffectingSystematic(JvtEfficiencyUp,true) || !addAffectingSystematic(JvtEfficiencyDown,true)) {
    ATH_MSG_ERROR("failed to set up Jvt systematics");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode JetJvtEfficiency::finalize(){
  delete h_JvtHist;
  delete h_EffHist;
  return StatusCode::SUCCESS;
}

CorrectionCode JetJvtEfficiency::getEfficiencyScaleFactor( const xAOD::Jet& jet,float& sf ){
    if (!isInRange(jet)) {
      sf = 1;
      return CorrectionCode::OutOfValidityRange;
    }
    int jetbin = h_JvtHist->FindBin(jet.pt(),fabs(jet.getAttribute<float>(m_jetEtaName)));
    float baseFactor = h_JvtHist->GetBinContent(jetbin);
    float errorTerm  = h_JvtHist->GetBinError(jetbin);
    if (m_appliedSystEnum==JVT_EFFICIENCY_UP) baseFactor += errorTerm;
    else if (m_appliedSystEnum==JVT_EFFICIENCY_DOWN) baseFactor -= errorTerm;
    if (m_doTruthRequirement && !jet.auxdata<int>(m_truthLabel)) sf = 1;
    else sf = baseFactor;
    return CorrectionCode::Ok;
}

CorrectionCode JetJvtEfficiency::getInefficiencyScaleFactor( const xAOD::Jet& jet,float& sf ){
    if (!isInRange(jet)) {
      sf = 1;
      return CorrectionCode::OutOfValidityRange;
    }
    int jetbin = h_JvtHist->FindBin(jet.pt(),fabs(jet.getAttribute<float>(m_jetEtaName)));
    float baseFactor = h_JvtHist->GetBinContent(jetbin);
    float effFactor = h_EffHist->GetBinContent(jetbin);
    float errorTerm  = h_JvtHist->GetBinError(jetbin);
    float errorEffTerm  = h_EffHist->GetBinError(jetbin);
    if (m_appliedSystEnum==JVT_EFFICIENCY_UP) baseFactor += errorTerm;
    else if (m_appliedSystEnum==JVT_EFFICIENCY_DOWN) baseFactor -= errorTerm;
    if (m_appliedSystEnum==JVT_EFFICIENCY_UP) effFactor += errorEffTerm;
    else if (m_appliedSystEnum==JVT_EFFICIENCY_DOWN) effFactor -= errorEffTerm;
    if (m_doTruthRequirement && !jet.auxdata<int>(m_truthLabel)) sf = 1;
    else sf = (1-baseFactor*effFactor)/(1-effFactor);
    return CorrectionCode::Ok;
}

CorrectionCode JetJvtEfficiency::applyEfficiencyScaleFactor(const xAOD::Jet& jet) {
    float sf = 0;
    CorrectionCode result = this->getEfficiencyScaleFactor(jet,sf);
    (*m_sfDec)(jet) = sf;
    return result;
}

CorrectionCode JetJvtEfficiency::applyInefficiencyScaleFactor(const xAOD::Jet& jet) {
    float sf = 0;
    CorrectionCode result = this->getInefficiencyScaleFactor(jet,sf);
    (*m_sfDec)(jet) = sf;
    return result;
}

CorrectionCode JetJvtEfficiency::applyAllEfficiencyScaleFactor(const xAOD::IParticleContainer *jets,float& sf,bool doJvtSelection) {
  sf = 1;
  for(const auto& ipart : *jets) {
    if (ipart->type()!=xAOD::Type::Jet) {
      ATH_MSG_WARNING("Input is not a jet");
      return CP::CorrectionCode::Error;
    }
    const xAOD::Jet *jet = static_cast<const xAOD::Jet*>(ipart);
    float current_sf = 0;
    CorrectionCode result = (!doJvtSelection || !m_doTruthRequirement || passesJvtCut(*jet))?this->getEfficiencyScaleFactor(*jet,current_sf):this->getInefficiencyScaleFactor(*jet,current_sf);
    if (result == CP::CorrectionCode::Error) {
      ATH_MSG_WARNING("Inexplicably failed JVT calibration" );
      return result;
    }
    (*m_sfDec)(*jet) = current_sf;
    sf *= current_sf;
  }
  return CorrectionCode::Ok;
}

CorrectionCode JetJvtEfficiency::applyRandomDropping( const xAOD::Jet& jet ) {
  float sf = 0;
  CorrectionCode result = this->getEfficiencyScaleFactor(jet,sf);
  if (result == CP::CorrectionCode::Error) {
    ATH_MSG_WARNING("Inexplicably failed JVT calibration" );
    return result;
  }
  if (!isInRange(jet) || jet.getAttribute<float>(m_jetJvtMomentName)<m_jvtCut || (m_doTruthRequirement && !jet.auxdata<int>(m_truthLabel))) (*m_dropDec)(jet) = 0;
  else (*m_dropDec)(jet) = m_rand.Rndm()>sf?1:0;
  return result;
}

CorrectionCode JetJvtEfficiency::applyAllRandomDropping(const xAOD::IParticleContainer *jets) {
  for(const auto& ipart : *jets) {
    if (ipart->type()!=xAOD::Type::Jet) {
      ATH_MSG_WARNING("Input is not a jet");
      return CP::CorrectionCode::Error;
    }
    const xAOD::Jet *jet = static_cast<const xAOD::Jet*>(ipart);
    CorrectionCode result = this->applyRandomDropping(*jet);
    if (result == CP::CorrectionCode::Error) {
      ATH_MSG_WARNING("Inexplicably failed JVT calibration" );
      return result;
    }
  }
  return CorrectionCode::Ok;
}

bool JetJvtEfficiency::passesJvtCut(const xAOD::Jet& jet) {
  if (jet.isAvailable<char>(m_drop_decoration_name) && jet.auxdata<char>(m_drop_decoration_name)) return false;
  if (!isInRange(jet)) return true;
  return jet.getAttribute<float>(m_jetJvtMomentName)>m_jvtCut;
}

bool JetJvtEfficiency::isInRange(const xAOD::Jet& jet) {
  if (fabs(jet.getAttribute<float>(m_jetEtaName))<h_JvtHist->GetYaxis()->GetBinLowEdge(1)) return false;
  if (fabs(jet.getAttribute<float>(m_jetEtaName))>h_JvtHist->GetYaxis()->GetBinUpEdge(h_JvtHist->GetNbinsY())) return false;
  if (jet.pt()<h_JvtHist->GetXaxis()->GetBinLowEdge(1)) return false;
  if (jet.pt()>h_JvtHist->GetXaxis()->GetBinUpEdge(h_JvtHist->GetNbinsX())) return false;
  if (jet.pt()>m_maxPtForJvt) return false;
  return true;
}

SystematicCode JetJvtEfficiency::sysApplySystematicVariation(const CP::SystematicSet& systSet){
  m_appliedSystEnum = NONE;
  if (systSet.size()==0) {
    ATH_MSG_DEBUG("No affecting systematics received.");
    return CP::SystematicCode::Ok;
  } else if (systSet.size()>1) {
    ATH_MSG_WARNING("Tool does not support multiple systematics, returning unsupported" );
    return CP::SystematicCode::Unsupported;
  }
  SystematicVariation systVar = *systSet.begin();
  if (systVar == SystematicVariation("")) m_appliedSystEnum = NONE;
  else if (systVar == JvtEfficiencyUp) m_appliedSystEnum = JVT_EFFICIENCY_UP;
  else if (systVar == JvtEfficiencyDown) m_appliedSystEnum = JVT_EFFICIENCY_DOWN;
  else {
    ATH_MSG_WARNING("unsupported systematic applied");
    return SystematicCode::Unsupported;
  }

  ATH_MSG_DEBUG("applied systematic is " << m_appliedSystEnum);
  return SystematicCode::Ok;
}

  void JetJvtEfficiency::setRandomSeed(int seed){
    m_rand.SetSeed(seed);
  }

} /* namespace CP */
