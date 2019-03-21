/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetJvtEfficiency/JetJvtEfficiency.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"
#include "xAODJet/JetContainer.h"
#include "TFile.h"

namespace CP {

static SG::AuxElement::Decorator<char>  isPUDec("isJvtPU");
//static SG::AuxElement::ConstAccessor<char>  isHSAcc("isJvtHS");

JetJvtEfficiency::JetJvtEfficiency( const std::string& name): asg::AsgTool( name ),
  m_appliedSystEnum(NONE),
  m_sfDec(nullptr),
  m_isHSDec(nullptr),
  m_isHSAcc(nullptr),
  h_JvtHist(nullptr),
  h_EffHist(nullptr),
  m_jvtCut(0),
  m_jvtCutBorder(0)
  {
    declareProperty( "WorkingPoint", m_wp = "Medium"                             );
    declareProperty( "SFFile",m_file="JetJvtEfficiency/Moriond2018/JvtSFFile_EMTopoJets.root");
    declareProperty( "ScaleFactorDecorationName", m_sf_decoration_name = "JvtSF"  );
    declareProperty( "JetJvtMomentName",   m_jetJvtMomentName   = "Jvt"           );
    declareProperty( "JetfJvtMomentName",   m_jetfJvtMomentName   = "passFJVT"    );
    declareProperty("OverlapDecorator", m_ORdec = ""                        );
    declareProperty( "JetEtaName",   m_jetEtaName   = "DetectorEta"               );
    declareProperty( "MaxPtForJvt",   m_maxPtForJvt   = 120e3                      );
    declareProperty( "DoTruthReq",   m_doTruthRequirement   = true );
    declareProperty( "TruthLabel",   m_isHS_decoration_name  = "isJvtHS"          );
    applySystematicVariation(CP::SystematicSet()).ignore();
}

StatusCode JetJvtEfficiency::initialize(){
  m_sfDec.reset(new SG::AuxElement::Decorator< float>(m_sf_decoration_name));
  m_isHSDec.reset(new SG::AuxElement::Decorator<char>(m_isHS_decoration_name));
  m_isHSAcc.reset(new SG::AuxElement::ConstAccessor<char>(m_isHS_decoration_name));
  m_dofJVT = (m_file.find("fJvt") != std::string::npos);
  m_doOR = (!m_ORdec.empty());
  if (!m_doTruthRequirement) ATH_MSG_WARNING ( "No truth requirement will be performed, which is not recommended.");

  bool ispflow = (m_file.find("EMPFlow") != std::string::npos);
  m_jvtCutBorder = -2.;
  if (m_wp=="Loose" && !ispflow) m_jvtCut = 0.11;
  else if (m_wp=="Medium" && ispflow) m_jvtCut = 0.2;
  else if (m_wp=="Tight" && ispflow) m_jvtCut = 0.5;
  else if (m_wp=="Medium"){
    m_jvtCut = 0.59;
    m_jvtCutBorder = 0.11;
  }
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

  if (m_wp=="Loose") h_JvtHist.reset( dynamic_cast<TH2*>(infile->Get("JvtLoose")) );
  else if (m_wp=="Medium") h_JvtHist.reset( dynamic_cast<TH2*>(infile->Get("JvtDefault")) );
  else if (m_wp=="Tight") h_JvtHist.reset( dynamic_cast<TH2*>(infile->Get("JvtTight")) );

  h_JvtHist->SetDirectory(0);

  if (m_wp=="Loose") h_EffHist.reset( dynamic_cast<TH2*>(infile->Get("EffLoose")) );
  else if (m_wp=="Medium") h_EffHist.reset( dynamic_cast<TH2*>(infile->Get("EffDefault")) );
  else if (m_wp=="Tight") h_EffHist.reset( dynamic_cast<TH2*>(infile->Get("EffTight")) );

  h_EffHist->SetDirectory(0);

  if(h_JvtHist.get()==nullptr || h_EffHist.get()==nullptr) {
    ATH_MSG_ERROR("Failed to retrieve histograms.");
    return StatusCode::FAILURE;
  }

  if (m_dofJVT) {
    if (!addAffectingSystematic(fJvtEfficiencyUp,true)
      || !addAffectingSystematic(fJvtEfficiencyDown,true)) {
      ATH_MSG_ERROR("failed to set up fJvt systematics");
      return StatusCode::FAILURE;
    }
  } else {
    if (!addAffectingSystematic(JvtEfficiencyUp,true)
      || !addAffectingSystematic(JvtEfficiencyDown,true)) {
      ATH_MSG_ERROR("failed to set up Jvt systematics");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode JetJvtEfficiency::finalize(){
  return StatusCode::SUCCESS;
}

CorrectionCode JetJvtEfficiency::getEfficiencyScaleFactor( const xAOD::Jet& jet,float& sf ){
    if (!isInRange(jet)) {
      sf = 1;
      return CorrectionCode::OutOfValidityRange;
    }
    if (m_doTruthRequirement) {
        if(!m_isHSAcc->isAvailable(jet)) {
            ATH_MSG_ERROR("Truth tagging required but decoration not available. Please call JetJvtEfficiency::tagTruth(...) first.");
            return CorrectionCode::Error;
        } else {
            if (!(*m_isHSAcc)(jet)) {
                sf = 1;
                return CorrectionCode::Ok;
            }
        }
    }
    int jetbin = h_JvtHist->FindBin(jet.pt(),fabs(jet.getAttribute<float>(m_jetEtaName)));
    float baseFactor = h_JvtHist->GetBinContent(jetbin);
    float errorTerm  = h_JvtHist->GetBinError(jetbin);
    if (!m_dofJVT && m_appliedSystEnum==JVT_EFFICIENCY_UP) baseFactor += errorTerm;
    else if (!m_dofJVT && m_appliedSystEnum==JVT_EFFICIENCY_DOWN) baseFactor -= errorTerm;
    else if (m_dofJVT && m_appliedSystEnum==FJVT_EFFICIENCY_UP) baseFactor += errorTerm;
    else if (m_dofJVT && m_appliedSystEnum==FJVT_EFFICIENCY_DOWN) baseFactor -= errorTerm;
    sf = baseFactor;
    return CorrectionCode::Ok;
}

CorrectionCode JetJvtEfficiency::getInefficiencyScaleFactor( const xAOD::Jet& jet,float& sf ){
    if (!isInRange(jet)) {
      sf = 1;
      return CorrectionCode::OutOfValidityRange;
    }
    if (m_doTruthRequirement) {
        if(!m_isHSAcc->isAvailable(jet)) {
            ATH_MSG_ERROR("Truth tagging required but decoration not available. Please call JetJvtEfficiency::tagTruth(...) first.");
            return CorrectionCode::Error;
        } else {
            if(!(*m_isHSAcc)(jet)) {
                sf = 1;
                return CorrectionCode::Ok;
            }
        }
    }
    int jetbin = h_JvtHist->FindBin(jet.pt(),fabs(jet.getAttribute<float>(m_jetEtaName)));
    float baseFactor = h_JvtHist->GetBinContent(jetbin);
    float effFactor = h_EffHist->GetBinContent(jetbin);
    float errorTerm  = h_JvtHist->GetBinError(jetbin);
    float errorEffTerm  = h_EffHist->GetBinError(jetbin);
    if (!m_dofJVT && m_appliedSystEnum==JVT_EFFICIENCY_UP) baseFactor += errorTerm;
    else if (!m_dofJVT && m_appliedSystEnum==JVT_EFFICIENCY_DOWN) baseFactor -= errorTerm;
    else if (m_dofJVT && m_appliedSystEnum==FJVT_EFFICIENCY_UP) baseFactor += errorTerm;
    else if (m_dofJVT && m_appliedSystEnum==FJVT_EFFICIENCY_DOWN) baseFactor -= errorTerm;
    if (!m_dofJVT && m_appliedSystEnum==JVT_EFFICIENCY_UP) effFactor += errorEffTerm;
    else if (!m_dofJVT && m_appliedSystEnum==JVT_EFFICIENCY_DOWN) effFactor -= errorEffTerm;
    else if (m_dofJVT && m_appliedSystEnum==FJVT_EFFICIENCY_UP) effFactor += errorEffTerm;
    else if (m_dofJVT && m_appliedSystEnum==FJVT_EFFICIENCY_DOWN) effFactor -= errorEffTerm;
    sf = (1-baseFactor*effFactor)/(1-effFactor);
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

CorrectionCode JetJvtEfficiency::applyAllEfficiencyScaleFactor(const xAOD::IParticleContainer *jets,float& sf) {
  sf = 1;
  const xAOD::JetContainer *truthJets = nullptr;
  if( evtStore()->retrieve(truthJets, "AntiKt4TruthJets").isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve AntiKt4TruthJets container");
  }
  if(!truthJets || tagTruth(jets,truthJets).isFailure()) {
    ATH_MSG_WARNING("Unable to match truthJets to jets in tagTruth() method");
  }
  for(const auto& ipart : *jets) {
    if (ipart->type()!=xAOD::Type::Jet) {
      ATH_MSG_WARNING("Input is not a jet");
      return CP::CorrectionCode::Error;
    }
    const xAOD::Jet *jet = static_cast<const xAOD::Jet*>(ipart);
    float current_sf = 0;
    CorrectionCode result = (m_dofJVT?jet->getAttribute<char>(m_jetfJvtMomentName):passesJvtCut(*jet))?this->getEfficiencyScaleFactor(*jet,current_sf):this->getInefficiencyScaleFactor(*jet,current_sf);
    if (result == CP::CorrectionCode::Error) {
      ATH_MSG_WARNING("Inexplicably failed JVT calibration" );
      return result;
    }
    (*m_sfDec)(*jet) = current_sf;
    sf *= current_sf;
  }
  return CorrectionCode::Ok;
}



bool JetJvtEfficiency::passesJvtCut(const xAOD::Jet& jet) {
  if (!isInRange(jet)) return true;
  if (fabs(jet.getAttribute<float>(m_jetEtaName))>2.4 && fabs(jet.getAttribute<float>(m_jetEtaName))<2.5) return jet.getAttribute<float>(m_jetJvtMomentName)>m_jvtCutBorder;
  return jet.getAttribute<float>(m_jetJvtMomentName)>m_jvtCut;
}

bool JetJvtEfficiency::isInRange(const xAOD::Jet& jet) {
  if (m_doOR && !jet.getAttribute<char>(m_ORdec)) return false;
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
  else if (!m_dofJVT && systVar == JvtEfficiencyUp) m_appliedSystEnum = JVT_EFFICIENCY_UP;
  else if (!m_dofJVT && systVar == JvtEfficiencyDown) m_appliedSystEnum = JVT_EFFICIENCY_DOWN;
  else if (m_dofJVT && systVar == fJvtEfficiencyUp) m_appliedSystEnum = FJVT_EFFICIENCY_UP;
  else if (m_dofJVT && systVar == fJvtEfficiencyDown) m_appliedSystEnum = FJVT_EFFICIENCY_DOWN;
  else m_appliedSystEnum = NONE;

  ATH_MSG_DEBUG("applied systematic is " << m_appliedSystEnum);
  return SystematicCode::Ok;
}

StatusCode JetJvtEfficiency::tagTruth(const xAOD::IParticleContainer *jets,const xAOD::IParticleContainer *truthJets) {
    for(const auto& jet : *jets) {
      bool ishs = false;
      bool ispu = true;
      for(const auto& tjet : *truthJets) {
        if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
        if (tjet->p4().DeltaR(jet->p4())<0.6) ispu = false;
      }
      (*m_isHSDec)(*jet)=ishs;
      isPUDec(*jet)=ispu;
    }
    return StatusCode::SUCCESS;
  }

} /* namespace CP */
