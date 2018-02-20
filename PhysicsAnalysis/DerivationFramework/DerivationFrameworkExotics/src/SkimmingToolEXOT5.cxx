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
  declareProperty("DiJetDEtaCut",          m_DEta = 2.5);
  m_isMC = true;
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

  if (recoJets->size() > 1) std::partial_sort(recoJets->begin(), recoJets->begin()+2, recoJets->end(), DescendingPt());
  float mjj = 0;
  if (recoJets->size() > 1) {
    TLorentzVector jet1 = recoJets->at(0)->p4();
    TLorentzVector jet2 = recoJets->at(1)->p4();
    auto dijet = jet1 + jet2;
    mjj = dijet.M();
  }
    // Compute dijet max Mjj, max Deta
  float mjj_max = 0., deta_max=0.;
  std::vector<TLorentzVector> jet_tlv;
  TLorentzVector jet_tmp;
  int num_jet = 0;
  float tmpJetMass, tmpDEta;
  for (auto jet : *recoJets)
    if(jet->pt() > 40000){
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
       if (tmpJetMass > mjj_max) mjj_max = tmpJetMass;
       if (tmpDEta > deta_max) deta_max = tmpDEta;
     }

  if ((recoJets->size() > 0 && recoJets->at(0)->pt() > m_monoJetPt) || (recoJets->size() > 1 && recoJets->at(0)->pt() > m_leadingJetPt && recoJets->at(1)->pt() > m_subleadingJetPt && mjj > m_Mjj ) || (recoJets->size() > 1 && recoJets->at(0)->pt() > m_leadingJetPt && recoJets->at(1)->pt() > m_subleadingJetPt && mjj_max > m_Mjj && deta_max > m_DEta)) passRecoJetCuts = true;

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
    // Compute dijet max Mjj, max Deta
    float truth_mjj_max = 0., truth_deta_max=0.;
    std::vector<TLorentzVector> truth_jet_tlv;
    TLorentzVector truth_jet_tmp;
    int truth_num_jet = 0;
    float truth_tmpJetMass, truth_tmpDEta;
    for (auto jet : *truthJets){
      if(jet->pt() > 40000){
        truth_jet_tmp.SetPtEtaPhiE(jet->pt(),jet->eta(),jet->phi(),jet->e());
        truth_jet_tlv.push_back(truth_jet_tmp);
        truth_num_jet++;
      }
    }
    if (truth_num_jet >= 2)
        for (int i = 0; i < truth_num_jet; i++)
          for (int j = i + 1; j < truth_num_jet; j++){
            TLorentzVector truth_jet_sum = truth_jet_tlv.at(i) + truth_jet_tlv.at(j);
            truth_tmpDEta = fabs(truth_jet_tlv.at(i).Eta()-truth_jet_tlv.at(j).Eta());
            truth_tmpJetMass = (truth_jet_sum).M();
            if (truth_tmpJetMass > mjj_max) truth_mjj_max = truth_tmpJetMass;
            if (truth_tmpDEta > deta_max) truth_deta_max = truth_tmpDEta;
        }
    if ((truthJets->size() > 0 && truthJets->at(0)->pt() > m_monoJetPt) || (truthJets->size() > 1 && truthJets->at(0)->pt() > m_leadingJetPt && truthJets->at(1)->pt() > m_subleadingJetPt && truthMjj > m_Mjj) || (truthJets->size() > 1 && truthJets->at(0)->pt() > m_leadingJetPt && truthJets->at(1)->pt() > m_subleadingJetPt && truth_mjj_max > m_Mjj && truth_deta_max > m_DEta)) passTruthJetCuts = true;
  }

  bool acceptEvent = passUncalibMonojetCut || passRecoJetCuts || passTruthJetCuts;

  m_npass++;
  return acceptEvent;

}
