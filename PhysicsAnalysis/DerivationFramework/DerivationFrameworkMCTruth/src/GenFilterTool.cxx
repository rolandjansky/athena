/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Class header file
#include "GenFilterTool.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

// Tool handle interface
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "PMGAnalysisInterfaces/IHFORSelectionTool.h"

namespace DerivationFramework {

  using namespace MCTruthPartClassifier;

  static bool isNonInteracting(int pid) {
    const int apid = abs(pid);
    if (apid == 12 || apid == 14 || apid == 16) return true; //< neutrinos
    if (apid == 1000022 || apid == 1000024 || apid == 5100022) return true; // SUSY & KK photon and Z partners
    if (apid == 39 || apid == 1000039 || apid == 5000039) return true; //< gravitons: standard, SUSY and KK
    if (apid == 9000001 || apid == 9000002 || apid == 9000003 || apid == 9000004 || apid == 9000005 || apid == 9000006) return true; //< exotic particles from monotop model
    return false;
  }

  static SG::AuxElement::Decorator<float> dec_genFiltHT("GenFiltHT");
  static SG::AuxElement::Decorator<float> dec_genFiltMET("GenFiltMET");
  static SG::AuxElement::Decorator<float> dec_genFiltPTZ("GenFiltPTZ");
  static SG::AuxElement::Decorator<float> dec_genFiltFatJ("GenFiltFatJ");
  static SG::AuxElement::Decorator<int> dec_HFOR("HFORDecision");

  GenFilterTool::GenFilterTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p)
    , m_classif("MCTruthClassifier/DFCommonTruthClassifier")
    , m_hforTool("HFORSelectionTool/HFORSelectionTool")
  {

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("MCCollectionName",m_mcName="TruthParticles");
    declareProperty("TruthJetCollectionName",m_truthJetsName="AntiKt4TruthWZJets");
    declareProperty("MinJetPt",m_MinJetPt = 35e3);
    declareProperty("MaxJetEta",m_MaxJetEta = 2.5);
    declareProperty("MinLeptonPt",m_MinLepPt = 25e3);
    declareProperty("MaxLeptonEta",m_MaxLepEta = 2.5);
    declareProperty("SimBarcodeOffset", m_SimBarcodeOffset = 200000);
    declareProperty("HFORSelectionTool",m_hforTool);
  }


  GenFilterTool::~GenFilterTool(){}


  bool GenFilterTool::isPrompt( const xAOD::TruthParticle* tp ) const
  {
    ParticleOrigin orig = getPartOrigin(tp);
    ATH_MSG_VERBOSE("Particle has origin " << orig);

    switch(orig) {
    case Unknown:
    case PhotonConv:
    case DalitzDec:
    case ElMagProc:
    case Mu:
    case LightMeson:
    case StrangeMeson:
    case CharmedMeson:
    case BottomMeson:
    case CCbarMeson:
    case JPsi:
    case BBbarMeson:
    case LightBaryon:
    case StrangeBaryon:
    case CharmedBaryon:
    case BottomBaryon:
    case PionDecay:
    case KaonDecay:
      return false;
    default:
      break;
    }
    return true;
  }

  MCTruthPartClassifier::ParticleOrigin GenFilterTool::getPartOrigin( const xAOD::TruthParticle* tp ) const
  {
    if(m_originMap.find(tp)==m_originMap.end()) {
      std::pair<ParticleType, ParticleOrigin> classification = m_classif->particleTruthClassifier( tp );
      m_originMap[tp] = classification.second;
    }
    return m_originMap[tp];
  }


  StatusCode GenFilterTool::addBranches() const{
    ATH_MSG_VERBOSE("GenFilterTool::addBranches()");

    const xAOD::EventInfo* eventInfo;
    if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
      ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
      return StatusCode::FAILURE;
    }

    const xAOD::TruthParticleContainer* truthPC = 0;
    if (evtStore()->retrieve(truthPC,m_mcName).isFailure()) {
      ATH_MSG_ERROR("WARNING could not retrieve TruthParticleContainer " <<m_mcName);
      return StatusCode::FAILURE;
    }

    m_originMap.clear();

    float genFiltHT(0.), genFiltMET(0.), genFiltPTZ(0.), genFiltFatJ(0.);
    ATH_CHECK( getGenFiltVars(truthPC, genFiltHT, genFiltMET, genFiltPTZ, genFiltFatJ) );

    ATH_MSG_DEBUG("Computed generator filter quantities: HT " << genFiltHT/1e3 << ", MET " << genFiltMET/1e3 << ", PTZ " << genFiltPTZ/1e3 << ", FatJ " << genFiltFatJ/1e3 );

    dec_genFiltHT(*eventInfo) = genFiltHT;
    dec_genFiltMET(*eventInfo) = genFiltMET;
    dec_genFiltPTZ(*eventInfo) = genFiltPTZ;
    dec_genFiltFatJ(*eventInfo) = genFiltFatJ;

    if (m_hforTool->getSampleType()!=HFORType::noType){
      dec_HFOR(*eventInfo) = static_cast< int >( m_hforTool->getDecisionType() );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode GenFilterTool::getGenFiltVars(const xAOD::TruthParticleContainer* tpc, float& genFiltHT, float& genFiltMET, float& genFiltPTZ, float& genFiltFatJ) const {
    // Get jet container out
    const xAOD::JetContainer* truthjets = 0;
    if ( evtStore()->retrieve( truthjets, m_truthJetsName).isFailure() || !truthjets ){
      ATH_MSG_ERROR( "No xAOD::JetContainer found in StoreGate with key " << m_truthJetsName );
      return StatusCode::FAILURE;
    }

    // Get HT
    genFiltHT = 0.;
    for (const auto& tj : *truthjets) {
      if ( tj->pt()>m_MinJetPt && fabs(tj->eta())<m_MaxJetEta ) {
        ATH_MSG_VERBOSE("Adding truth jet with pt " << tj->pt()
                        << ", eta " << tj->eta()
                        << ", phi " << tj->phi()
                        << ", nconst = " << tj->numConstituents());
        genFiltHT += tj->pt();
      }
    }

    // Get MET and add leptons to HT
    float MEx(0.), MEy(0.);
    for (const auto& tp : *tpc){
      int pdgid = tp->pdgId();
      if (tp->barcode() >= m_SimBarcodeOffset) continue; // Particle is from G4
      if (pdgid==21 && tp->e()==0) continue; // Work around for an old generator bug
      if ( tp->status() %1000 !=1 ) continue; // Stable!

      if ((abs(pdgid)==11 || abs(pdgid)==13) && tp->pt()>m_MinLepPt && fabs(tp->eta())<m_MaxLepEta) {
        if( isPrompt(tp) ) {
          ATH_MSG_VERBOSE("Adding prompt lepton with pt " << tp->pt()
                          << ", eta " << tp->eta()
                          << ", phi " << tp->phi()
                          << ", status " << tp->status()
                          << ", pdgId " << pdgid);
          genFiltHT += tp->pt();
        }
      }

      if (isNonInteracting(pdgid) && isPrompt(tp) ) {
        ATH_MSG_VERBOSE("Found prompt nonInteracting particle with pt " << tp->pt()
                        << ", eta " << tp->eta()
                        << ", phi " << tp->phi()
                        << ", status " << tp->status()
                        << ", pdgId " << pdgid);
        MEx += tp->px();
        MEy += tp->py();
      }
    }
    genFiltMET = sqrt(MEx*MEx+MEy*MEy);

    // Get PTZ
    float PtZ(.0);
    float MinPt_PTZ(5000.), MaxEta_PTZ(5.0), MinMass_PTZ(20000.), MaxMass_PTZ(14000000.);
    bool AllowElecMu_PTZ = false;
    bool AllowSameCharge_PTZ = false;
    for (const xAOD::TruthParticle* pitr1 : *tpc){
      int pdgId1 = pitr1->pdgId();
      if (pitr1->barcode() >= m_SimBarcodeOffset) continue;
      if (pitr1->status()!=1) continue;
      // Pick electrons or muons with Pt > MinPt_PTZ and |eta| < m_maxEta
      if (abs(pdgId1) == 11 || abs(pdgId1) == 13) {
        if (pitr1->pt() >= MinPt_PTZ && fabs(pitr1->eta()) <= MaxEta_PTZ){
          for (const xAOD::TruthParticle* pitr2 : *tpc){
            if (pitr2==pitr1) continue;
            if (pitr2->barcode() >= m_SimBarcodeOffset) continue;
            if (pitr2->status()!=1) continue;
            int pdgId2 = pitr2->pdgId();
            // Pick electrons or muons with Pt > MinPt_PTZ and |eta| < MaxEta_PTZ
            // If AllowSameCharge_PTZ is not true only pick those with opposite charge to the first particle
            // If AllowElecMu_PTZ is true allow also Z -> emu compinations (with charge requirements as above)
            if ((AllowSameCharge_PTZ  && (abs(pdgId2) == abs(pdgId1) || (AllowElecMu_PTZ && (abs(pdgId2) == 11 || abs(pdgId2) == 13) ) ) ) ||
                (!AllowSameCharge_PTZ && (pdgId2 == -1*pdgId1 || (AllowElecMu_PTZ && (pdgId2 == (pdgId1 < 0 ? 1 : -1) * 11 || (pdgId1 < 0 ? 1 : -1) * pdgId2 == 13) ) ) ) ) {
              if (pitr2->pt() >= MinPt_PTZ && fabs(pitr2->eta()) <= MaxEta_PTZ){
                double invMass = (pitr1->p4()+pitr2->p4()).M();
                double dilepPt = (pitr1->p4()+pitr2->p4()).Pt();
                // Only consider pair that fall in the mass window
                if (MinMass_PTZ < invMass && invMass < MaxMass_PTZ) {
                  if (dilepPt > PtZ) PtZ = dilepPt;
                }
              }
            }
          }
        }
      }
    }
    genFiltPTZ = PtZ;

   //Get FatJ
   // Get correct jet container
   const xAOD::JetContainer* truthjets10 = 0;
   if ( evtStore()->retrieve( truthjets10, "AntiKt10TruthJets").isFailure() || !truthjets10 ){
     ATH_MSG_ERROR( "No xAOD::JetContainer found in StoreGate with key AntiKt10TruthJets" );
     return StatusCode::FAILURE;
   }
   genFiltFatJ=0.;
   for (const auto& j : *truthjets10) {
     if (j->pt()>genFiltFatJ) genFiltFatJ=j->pt();
   }


    return StatusCode::SUCCESS;
  }


} /// namespace
