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
  m_ntot(0),
  m_npass(0)
{

  declareInterface<DerivationFramework::ISkimmingTool>(this);

  declareProperty("JetContainer",          m_jetSGKey = "AntiKt4EMTopoJets");

  declareProperty("UncalibMonoJetPtCut",   m_uncalibMonoJetPt = 100000.);
  declareProperty("MonoJetPtCut",          m_monoJetPt = 100000.);
  declareProperty("LeadingJetPtCut",       m_leadingJetPt = 40000.);
  declareProperty("SubleadingJetPtCut",    m_subleadingJetPt = 40000.);
  declareProperty("DiJetMassCut",          m_Mjj = 150000.);
}

DerivationFramework::SkimmingToolEXOT5::~SkimmingToolEXOT5() {
}

StatusCode DerivationFramework::SkimmingToolEXOT5::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");

  m_jetCalibrationTool.setTypeAndName("JetCalibrationTool/EXOT5JESTool");
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

  if (recoJets->size() > 1) std::partial_sort(recoJets->begin(), recoJets->begin()+2, recoJets->end(), DescendingPt());
  float mjj = 0;
  if (recoJets->size() > 1) {
    TLorentzVector jet1 = recoJets->at(0)->p4();
    TLorentzVector jet2 = recoJets->at(1)->p4();
    auto dijet = jet1 + jet2;
    mjj = dijet.M();
  }
  if ((recoJets->size() > 0 && recoJets->at(0)->pt() > m_monoJetPt) || (recoJets->size() > 1 && recoJets->at(0)->pt() > m_leadingJetPt && recoJets->at(1)->pt() > m_subleadingJetPt && mjj > m_Mjj)) passRecoJetCuts = true;

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

    if (truthJets->size() > 1) std::partial_sort(truthJets->begin(), truthJets->begin()+2, truthJets->end(), DescendingPt());
    float truthMjj = 0;
    if (truthJets->size() > 1) {
      TLorentzVector truthJet1 = truthJets->at(0)->p4();
      TLorentzVector truthJet2 = truthJets->at(1)->p4();
      auto truthDijet = truthJet1 + truthJet2;
      truthMjj = truthDijet.M();
    }
    if ((truthJets->size() > 0 && truthJets->at(0)->pt() > m_monoJetPt) || (truthJets->size() > 1 && truthJets->at(0)->pt() > m_leadingJetPt && truthJets->at(1)->pt() > m_subleadingJetPt && truthMjj > m_Mjj)) passTruthJetCuts = true;
  }

  bool acceptEvent = passUncalibMonojetCut || passRecoJetCuts || passTruthJetCuts;

  m_npass++;
  return acceptEvent;

}
