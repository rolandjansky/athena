/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header file
#include "GenFilterTool.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

// Tool handle interface
#include "MCTruthClassifier/IMCTruthClassifier.h"

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

  GenFilterTool::GenFilterTool(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p),
    m_classif("MCTruthClassifier/DFCommonTruthClassifier")
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
  }


  GenFilterTool::~GenFilterTool(){}


  bool GenFilterTool::isPrompt( const xAOD::TruthParticle* tp ) const
  {
    ParticleOrigin orig = getPartOrigin(tp);
    ATH_MSG_VERBOSE("Particle has origin " << orig);

    switch(orig) {
    case PhotonConv:
    case DalitzDec:
    case ElMagProc:
    case Mu:
    case TauLep:
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

    float genFiltHT(0.), genFiltMET(0.);
    ATH_CHECK( getGenFiltVars(truthPC, genFiltHT, genFiltMET) );

    ATH_MSG_DEBUG("Computed generator filter quantities: HT " << genFiltHT/1e3 << ", MET " << genFiltMET/1e3 );

    dec_genFiltHT(*eventInfo) = genFiltHT;
    dec_genFiltMET(*eventInfo) = genFiltMET;

    return StatusCode::SUCCESS;
  }

  StatusCode GenFilterTool::getGenFiltVars(const xAOD::TruthParticleContainer* tpc, float& genFiltHT, float& genFiltMET) const {
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

    return StatusCode::SUCCESS;
  }


} /// namespace
