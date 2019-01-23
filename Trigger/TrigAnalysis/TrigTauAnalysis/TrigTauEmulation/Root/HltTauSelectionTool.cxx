/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#include "TrigTauEmulation/HltTauSelectionTool.h"
#include "TrigTauEmulation/Utils.h"
#include "TrigTauEmulation/MsgStream.h"

//Default constructor
HltTauSelectionTool::HltTauSelectionTool(const std::string& name) : SelectionTool(name)

{

  declareProperty("PreselPt", m_pt=25000., "Preselection Pt cut");
  declareProperty("CentFracStrategy", m_centfrac_strategy="pt_dependent", "CentFrac cut strategy");
  declareProperty("CentFracCut", m_centfrac=0.9, "CentFrac cut");
  declareProperty("UsePresel", m_use_presel=true, "Turn on/off preselection");
  declareProperty("UseCaloPresel", m_use_calo_presel=true, "Turn on/off preselection centfrac cut");

  declareProperty("UseFastTracking", m_use_fasttracking=true, "Turn on/off fast tracking");
  declareProperty("FastTrackSelectionTool", m_ftf_tool);
 
  declareProperty("IdLevel", m_id_level="medium", "loose/medium/tight");
  declareProperty("UseTauID", m_use_tauid=true, "Turn on/off the tau id selection");

  declareProperty("RecalculateBDTscore", m_recalculateBDTscore=false, "Recalculate tau BDT scores");
  declareProperty("TauDiscriminantToolName", m_TauDiscriminantToolName="TauIDTool");

  m_calopresel = new HltTauCaloPresel();
  m_tauid = new HltTauID();

}

// Copy constructor
HltTauSelectionTool::HltTauSelectionTool(const HltTauSelectionTool& other) : SelectionTool(other.name() + "_copy")
{
  m_pt = other.m_pt;
  m_centfrac_strategy = other.m_centfrac_strategy;
  m_centfrac = other.m_centfrac;
  m_use_presel = other.m_use_presel;
  m_use_calo_presel = other.m_use_calo_presel;
  m_use_fasttracking = other.m_use_fasttracking;
  m_ftf_tool = other.m_ftf_tool;
  m_id_level = other.m_id_level;
  m_use_tauid = other.m_use_tauid;
  m_recalculateBDTscore = other.m_recalculateBDTscore;
  m_TauDiscriminantToolName = other.m_TauDiscriminantToolName;

  m_calopresel = new HltTauCaloPresel();
  m_tauid = new HltTauID();

}


HltTauSelectionTool::~HltTauSelectionTool() {
  delete m_calopresel;
  delete m_tauid;
  // --> not needed anymore (central tool now) / will remove soooon
  // delete m_ftk;
}

StatusCode HltTauSelectionTool::initialize()

{
  // #ifdef ASGTOOL_STANDALONE
  //   if (m_recalculateBDTscore) {
  //     m_tauIDTool = new ToolHandle<TauDiscriminantTool>(m_TauDiscriminantToolName);
  //     m_tauIDTool->operator->()->initialize().ignore();
  //   }
  // #endif

  m_calopresel->SetUsePresel(m_use_presel);
  m_calopresel->SetUseCaloPresel(m_use_calo_presel);
  m_calopresel->SetPreselPt(m_pt);
  m_calopresel->SetPreselCentFracCut(m_centfrac);
  m_calopresel->SetPreselCentFracStrategy(m_centfrac_strategy);

  if(m_use_fasttracking){
    ATH_CHECK(m_ftf_tool.retrieve());
  }

  // --> not needed anymore (central tool now) / will remove soooon
  // m_ftk->SetUseFastTracking(m_use_fasttracking);
  // m_ftk->SetTrackD0(m_d0);
  // m_ftk->SetTrackZ0(m_z0);
  // m_ftk->SetDeltaZ0(m_delta_z0);
  // m_ftk->SetCoreConeDr(m_core);
  // m_ftk->SetIsoConeDr(m_iso);
  // m_ftk->SetNcore(m_ncore_bound);
  // m_ftk->SetNiso(m_niso_bound);

  m_tauid->SetIdLevel(m_id_level);
  m_tauid->SetUseTauID(m_use_tauid);

  m_accept.addCut("HltTau", "HltTau");

  return StatusCode::SUCCESS;
}

const asg::AcceptInfo& HltTauSelectionTool::getAcceptInfo() const
{
  return m_accept;
}


asg::AcceptData HltTauSelectionTool::accept(const DecoratedHltTau& hlttau) const {
  return accept(hlttau.getHltTau(), hlttau.getPreselTracksIso(), hlttau.getPreselTracksCore()); 
}

// accept based on EDM tau candidate and first step FTF tracks from the TDT
asg::AcceptData HltTauSelectionTool::accept(const xAOD::TauJet *hlttau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const {

  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("HltTau", false);

// #ifdef ASGTOOL_STANDALONE
//   if (m_recalculateBDTscore) {
//     MY_MSG_INFO("Recalculating BDT score for HLT tau " << hlttau);

//     m_tauIDTool->operator->()->applyDiscriminant(*(const_cast<xAOD::TauJet*>(hlttau)) ).ignore();
//     MY_MSG_INFO("tau BDT score = " << hlttau->discriminant(xAOD::TauJetParameters::BDTJetScore));
//   }
// #endif

  if (not m_calopresel->accept(hlttau)) {
    return acceptData;
  }

  if (m_use_fasttracking) {
    if (not m_ftf_tool->accept(hlttau, preselTracksIso, preselTracksCore)) {
      return acceptData;
    }
  }

  if (not m_tauid->accept(hlttau)) {
    return acceptData;
  }

  acceptData.setCutResult("HltTau", true);
  return acceptData;

}

asg::AcceptData HltTauSelectionTool::accept(const xAOD::TauJet * hlttau, const xAOD::TauJetContainer * presel_taus) const {

  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("HltTau", false);

  // #ifdef ASGTOOL_STANDALONE
  //   if (m_recalculateBDTscore) {
  //     MY_MSG_INFO("Recalculating BDT score for HLT tau " << hlttau);
  
  //     m_tauIDTool->operator->()->applyDiscriminant(*(const_cast<xAOD::TauJet*>(hlttau)) ).ignore();
  //     MY_MSG_INFO("tau BDT score = " << hlttau->discriminant(xAOD::TauJetParameters::BDTJetScore));
  //   }
  // #endif

  //MY_MSG_INFO("\t\tBefore calo cuts");
  if (not m_calopresel->accept(hlttau)) {
    //std::cout << "not accepting calo presel" << std::endl;
    return acceptData;
  }

  const xAOD::TauJet *presel_tau = NULL;
  for (const auto tau: *presel_taus) {
    double dR = Utils::DeltaR(hlttau->eta(), hlttau->phi(), tau->eta(), tau->phi());
    if (dR < 0.01) {
      presel_tau = tau;
      break;
    }
  }

  // MY_MSG_INFO("\t\tBefore preselection check");
  if (m_use_fasttracking and presel_tau == NULL) { 
    //std::cout << "no presel_tau present" << std::endl;
    return acceptData;
  }

  if (m_use_fasttracking)
    if (not m_ftf_tool->accept(hlttau)) {
      return acceptData;
    }

  // --> not needed anymore (central tool now) / will remove soooon
  // // MY_MSG_INFO("\t\tBefore FTk cuts");
  // if (not m_ftk->accept(presel_tau)) {
  //   //std::cout << "not accepting tracking cut" << std::endl;
  //   return acceptData;
  // } else {
  //   //std::cout << "accepting tracking cut" << std::endl;
  // }

  // MY_MSG_INFO("\t\tBefore BDT cuts");
  if (not m_tauid->accept(hlttau)) {
    //std::cout << "not accepting tau ID cut" << std::endl;
    return acceptData;
  } else {
    //std::cout << "accepting tau ID cut" << std::endl;
  }

  // MY_MSG_INFO("\t\tPass all cuts");
  acceptData.setCutResult("HltTau", true);
  return acceptData;

}
