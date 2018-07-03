/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// Based on DerivationFramework::SkimmingToolExample
// Author: Valentinos Christodoulou (valentinos.christodoulou@cern.ch)
///////////////////////////////////////////////////////////////////////

#include "DerivationFrameworkExotics/SkimmingToolEXOT5.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

struct DescendingPt:std::function<bool(const xAOD::IParticle*, const xAOD::IParticle*)> {
  bool operator()(const xAOD::IParticle* l, const xAOD::IParticle* r)  const {
    return l->pt() > r->pt();
  }
};

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

DerivationFramework::SkimmingToolEXOT5::SkimmingToolEXOT5(const std::string& t,
  const std::string& n,
  const IInterface* p) :
AthAlgTool(t, n, p),
m_jetCalibrationTool("JetCalibrationTool/EXOT5JESTool"),
m_ntot(0),
m_npass(0)
{

  declareInterface<DerivationFramework::ISkimmingTool>(this);

  declareProperty("JetContainer",          m_jetSGKey = "AntiKt4EMTopoJets");
  declareProperty("JetCalibToolName",      m_jetCalibToolName = "" ); //empty means use the default from ExtendedJetCommon

  declareProperty("UncalibMonoJetPtCut",   m_uncalibMonoJetPt = 100000.);
  declareProperty("MonoJetPtCut",          m_monoJetPt = 100000.);
  declareProperty("LeadingJetPtCut",       m_leadingJetPt = 40000.);
  declareProperty("SubleadingJetPtCut",    m_subleadingJetPt = 40000.);
  declareProperty("DiJetMassCut",          m_Mjj = 150000.);
  declareProperty("DiJetMassMaxCut",       m_Mjj_Max = 150000.);
  declareProperty("DiJetDEtaCut",          m_DEta = 2.5);
  declareProperty("VBFJetThresh",          m_VBFjet = 40000.);

  m_isMC = true;
  e_JetsDEta_Max = 0;
  e_DiJetMass_Max = 0;
  e_DiJetMass = 0;
}

DerivationFramework::SkimmingToolEXOT5::~SkimmingToolEXOT5() {
}

StatusCode DerivationFramework::SkimmingToolEXOT5::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");

  if(m_jetCalibToolName.empty()){
    std::string jetColl = m_jetSGKey.substr(0,m_jetSGKey.length()-4);
    m_jetCalibrationTool.setTypeAndName("DFJetCalib_"+jetColl);
  }
  else{
    m_jetCalibrationTool.setTypeAndName("JetCalibrationTool/"+m_jetCalibToolName);//EXOT5JESTool");
  }
  CHECK(m_jetCalibrationTool.retrieve());
  ATH_MSG_INFO("Retrieved tool: " << m_jetCalibrationTool);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SkimmingToolEXOT5::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " events, " << m_npass << " events passed filter ");

  return StatusCode::SUCCESS;

}

bool DerivationFramework::SkimmingToolEXOT5::eventPassesFilter() const
{

  m_ntot++;

  const xAOD::EventInfo* eventInfo = nullptr;
  ATH_CHECK(evtStore()->retrieve(eventInfo));
  m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  const xAOD::JetContainer* jets = nullptr;
  ATH_CHECK(evtStore()->retrieve(jets, m_jetSGKey));

  bool passUncalibMonojetCut = false;
  bool passRecoJetCuts = false;
  bool passTruthJetCuts = false;

  std::unique_ptr<xAOD::JetContainer> recoJets = make_unique<xAOD::JetContainer>();
  std::unique_ptr<xAOD::JetAuxContainer> recoJetsAux = make_unique<xAOD::JetAuxContainer>();
  recoJets->setStore(recoJetsAux.get());

  std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> jets_shallowCopy = xAOD::shallowCopyContainer(*jets);
  for (const auto &jet : *jets_shallowCopy.first) {
    if (jet->pt() > 100000.) passUncalibMonojetCut = true;
    if (m_jetCalibrationTool->applyCalibration(*jet) == CP::CorrectionCode::Error) {
      ATH_MSG_ERROR("Problem applying jet calibration");
      return StatusCode::FAILURE;
    }
    xAOD::Jet* newJet = new xAOD::Jet();
    newJet->makePrivateStore(*jet);
    recoJets->push_back(newJet);
  }
  delete jets_shallowCopy.first;
  delete jets_shallowCopy.second;

  // Compute the leading dijet mass
  computeMassjj(recoJets);
  // Compute dijet max Mjj, max Deta for VBF h>Inv
  computeMaxjj(recoJets);
  // Skim for MonoJet, MET+X, VBF h>Inv
  passRecoJetCuts = passSkimCuts(recoJets);

  if (m_isMC) {
    const xAOD::JetContainer* truthJetContainer = nullptr;
    ATH_CHECK(evtStore()->retrieve(truthJetContainer, "AntiKt4TruthJets"));

    std::unique_ptr<xAOD::JetContainer> truthJets = make_unique<xAOD::JetContainer>();
    std::unique_ptr<xAOD::JetAuxContainer> truthJetsAux = make_unique<xAOD::JetAuxContainer>();
    truthJets->setStore(truthJetsAux.get());

    for (const auto &truthJet : *truthJetContainer) {
      xAOD::Jet* newTruthJet = new xAOD::Jet();
      newTruthJet->makePrivateStore(*truthJet);
      truthJets->push_back(newTruthJet);
    }

  // Compute the leading dijet mass
  computeMassjj(truthJets);
  // Compute dijet max Mjj, max Deta for VBF h>Inv
  computeMaxjj(truthJets);
  // Skim for MonoJet, MET+X, VBF h>Inv
  passTruthJetCuts = passSkimCuts(truthJets);
  }

  bool acceptEvent = passUncalibMonojetCut || passRecoJetCuts || passTruthJetCuts;

  m_npass++;
  return acceptEvent;

}

void DerivationFramework::SkimmingToolEXOT5::computeMaxjj(const std::unique_ptr<xAOD::JetContainer> & jets) const
{
  e_JetsDEta_Max = 0;
  e_DiJetMass_Max = 0;
  std::vector<TLorentzVector> jet_tlv;
  TLorentzVector jet_tmp;
  int num_jet = 0;
  float tmpJetMass, tmpDEta;
  for (auto jet : *jets)
    if(jet->pt() > m_VBFjet){
      jet_tmp.SetPtEtaPhiE(jet->pt(),jet->eta(),jet->phi(),jet->e());
      jet_tlv.push_back(jet_tmp);
      num_jet++;
    }
  if (num_jet >= 2)
   for (int i = 0; i < num_jet; i++)
     for (int j = i + 1; j < num_jet; j++) {
       TLorentzVector jet_sum = jet_tlv.at(i) + jet_tlv.at(j);
       tmpDEta = fabs(jet_tlv.at(i).Eta()-jet_tlv.at(j).Eta());
       tmpJetMass = (jet_sum).M();
       if (tmpJetMass > e_DiJetMass_Max) e_DiJetMass_Max = tmpJetMass;
       if (tmpDEta > e_JetsDEta_Max) e_JetsDEta_Max = tmpDEta;
     }
}

void DerivationFramework::SkimmingToolEXOT5::computeMassjj(const std::unique_ptr<xAOD::JetContainer> & jets) const
{
  if (jets->size() > 1) std::partial_sort(jets->begin(), jets->begin()+2, jets->end(), DescendingPt());
  e_DiJetMass = 0;
  if (jets->size() > 1) {
    TLorentzVector jet1 = jets->at(0)->p4();
    TLorentzVector jet2 = jets->at(1)->p4();
    auto dijet = jet1 + jet2;
    e_DiJetMass = dijet.M();
  }
}

bool DerivationFramework::SkimmingToolEXOT5::passSkimCuts(const std::unique_ptr<xAOD::JetContainer> & jets) const
{
  bool passMonoJetSkim = (jets->size() > 0 && jets->at(0)->pt() > m_monoJetPt);
  bool passMETXSkim    = (jets->size() > 1 && jets->at(0)->pt() > m_leadingJetPt && jets->at(1)->pt() > m_subleadingJetPt && e_DiJetMass > m_Mjj);
  bool passVBFSkim     = (jets->size() > 1 && jets->at(0)->pt() > m_leadingJetPt && jets->at(1)->pt() > m_subleadingJetPt && e_DiJetMass_Max > m_Mjj_Max && e_JetsDEta_Max > m_DEta);
  return (passMonoJetSkim || passMETXSkim || passVBFSkim);
}

