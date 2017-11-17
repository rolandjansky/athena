/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthJetParticles.h"
#include "xAODTruth/TruthVertex.h"
#include "TruthUtils/PIDHelpers.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "AsgTools/Check.h"

#ifndef XAOD_STANDALONE
// Usage of metadata is for now only possible in Athena...
//#include "CoralBase/AttributeListException.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

// For std::find in comesFrom()
#include <algorithm>

using namespace std;
using namespace MCTruthPartClassifier;

CopyTruthJetParticles::CopyTruthJetParticles(const std::string& name)
  : CopyTruthParticles(name) ,
    m_maxAbsEta(5)
  , m_barcodeOffset(200000)
  , m_barcodeFromMetadata(2)
  , m_photonCone(0.1)
  , m_classif("")
{
  declareProperty("IncludeNonInteracting",  m_includeNonInt=false, "Include noninteracting particles (including neutrinos) in the output collection");
  declareProperty("IncludeNeutrinos",  m_includeNu=false, "Include neutrinos in the output collection (implied if using NonInt)");
  declareProperty("IncludeMuons",      m_includeMu=false, "Include muons in the output collection");
  declareProperty("IncludePromptLeptons",  m_includePromptLeptons=true,  "Include leptons (including neutrinos) from prompt decays (i.e. not from hadron decays) in the output collection");
  declareProperty("IncludePromptPhotons",  m_includePromptPhotons=true,  "Include photons from Higgs and other decays that produce isolated photons");
  //  declareProperty("IncludeTauLeptons", m_includeTau=true, "Include leptons from tau decays in the output collection");

  // -- added for dark jet clustering -- //
  declareProperty("IncludeSMParts", m_includeSM=true, "Include SM particles in the output collection");
  declareProperty("IncludeDarkHads", m_includeDark=false, "Include dark hadrons in the output collection");
  // ----------------------------------- //
  
  declareProperty("MaxAbsEta", m_maxAbsEta);
  declareProperty("BarCodeOffset", m_barcodeOffset);
  declareProperty("BarCodeFromMetadata", m_barcodeFromMetadata);

  declareProperty("MCTruthClassifier", m_classif);

  declareProperty("FSRPhotonCone", m_photonCone);

  declareProperty("VetoPDG_IDs", m_vetoPDG_IDs, "List of PDG IDs (python list) to veto.  Will ignore these and all children of these.");

  declareProperty("DressingDecorationName", m_dressingName="", "Name of the dressed photon decoration (if one should be used)");

}

bool CopyTruthJetParticles::classifyJetInput(const xAOD::TruthParticle* tp,
                                             std::vector<const xAOD::TruthParticle*>& promptLeptons,
                                             std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap) const {

  bool makenoise = tp->pdgId()==1000039;

  // Check if this thing is a candidate to be in a truth jet
  //  First block is largely copied from isGenStable, which works on HepMC only
  if (tp->barcode()>=m_barcodeOffset) return false; // Particle is from G4
  int pdgid = tp->pdgId();
  if (pdgid==21 && tp->e()==0) return false; // Work around for an old generator bug

  // -- changed for dark jet clustering -- //
  //if ( tp->status() %1000 !=1 ) return false; // Stable!
  if ( tp->status()%1000!=1 && !m_includeDark ) return false; // dark hadrons will not be status 1
  // ----------------------------------- //
  
  // Easy classifiers by PDG ID
  if (!m_includeNu && MCUtils::PID::isNeutrino(pdgid)) return false;
  else if (!m_includeNonInt && MC::isNonInteracting(pdgid)) return false;
  if (!m_includeMu && abs(pdgid)==13) return false;

  // Cannot use the truth helper functions; they're written for HepMC
  // Last two switches only apply if the thing is a lepton and not a tau
  if (MC::PID::isLepton(pdgid) && abs(pdgid)!=15 && tp->hasProdVtx()){
    bool isPromptLepton = isPrompt( tp, originMap );
    if(isPromptLepton && (abs(pdgid)==11 || abs(pdgid)==13)) promptLeptons.push_back(tp);
    if (!m_includePromptLeptons && isPromptLepton) {
      ATH_MSG_VERBOSE("Veto prompt lepton (" << pdgid << ") with pt " << tp->pt() << " origin " << getPartOrigin( tp, originMap ));
      return false;
    }
    // if (!m_includeTau && fromTau( tp, originMap )) {
    //   ATH_MSG_VERBOSE("Veto lepton (" << pdgid << ") from tau");
    //   return false;
    // }
  }

  // -- added for dark jet clustering -- //
  // new classifiers to account for dark particles
  // for dark jets: ignore SM particles; include only "stable" dark hadrons
  if (!m_includeSM && ((abs(tp->pdgId()) < 4.9e6) || (abs(tp->pdgId()) >= 5e6))) return false;
  if (m_includeDark) {
    if (abs(tp->pdgId()) <= 4900101) return false; // ignore Xd, qd, gd
    if (tp->hasDecayVtx() && (abs(tp->child()->pdgId()) >= 4.9e6)) return false; // ignore "non-stable" dark hadrons (decaying to dark sector) -- "stable" if decaying to SM 
  }
  // for SM jets: ignore dark particles - probably unnecessary bc of status requirement above
  if (!m_includeDark && (abs(tp->pdgId()) >= 4.9e6) && (abs(tp->pdgId()) < 5e6)) return false;
  // ----------------------------------- //

  if(!m_includePromptLeptons && abs(pdgid)==22 && ( isPrompt( tp, originMap ) || getPartOrigin( tp, originMap )==FSRPhot ) ) {
    // Only exclude photons within deltaR of leptons (if m_photonCone<0, exclude all photons)
    if(m_photonCone>0) {
      for(const auto& lep : promptLeptons) {
          double deltaR = tp->p4().DeltaR(lep->p4());
          // if photon within deltaR of lepton, remove along with lepton
          if( deltaR < m_photonCone ) {
            ATH_MSG_VERBOSE("Veto photon with pt " << tp->pt() << " with dR=" << deltaR
                       << " near to a " << lep->pdgId() << " with pt " << lep->pt());
            return false;
          }
      }
    }
  }

  // Exclude prompt photons, particularly those from Higgs decays to start
  if (!m_includePromptPhotons && MC::PID::isPhoton(pdgid) && tp->hasProdVtx()){
    ParticleOrigin orig = getPartOrigin(tp, originMap);
    if (orig==Higgs || orig==HiggsMSSM) return false;
  }

  // If we want to remove photons via the dressing decoration
  if (!m_dressingName.empty()){
    // Accessor for the dressing decoration above
    const static SG::AuxElement::Accessor<char> dressAcc(m_dressingName);
    if (pdgid==22 && dressAcc(*tp)) return false;
  } // End of removal via dressing decoration

  // Pseudo-rapidity cut
  if(fabs(tp->eta())>m_maxAbsEta) return false;

  // Vetoes of specific origins.  Not fast, but if no particles are specified should not execute
  if (m_vetoPDG_IDs.size()>0){
    std::vector<int> used_vertices;
    for (int anID : m_vetoPDG_IDs){
      used_vertices.clear();
      if (comesFrom(tp,anID,used_vertices)) return false;
    }
  }

  // Made it! 
  return true;
}

bool CopyTruthJetParticles::isPrompt( const xAOD::TruthParticle* tp,
                                      std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap ) const
{
  ParticleOrigin orig = getPartOrigin(tp, originMap);
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


MCTruthPartClassifier::ParticleOrigin CopyTruthJetParticles::getPartOrigin(const xAOD::TruthParticle* tp,
                                                                           std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap) const
{
  if(originMap.find(tp)==originMap.end()) {
    std::pair<ParticleType, ParticleOrigin> classification = m_classif->particleTruthClassifier( tp );
    originMap[tp] = classification.second;
  }
  return originMap[tp];
}


StatusCode CopyTruthJetParticles::initialize() {
  //*******************************
  // retrieve barcode Offset for this event from metadata.
  // We'd need a cleaner solution to set this offset only at
  // each new file, but this setting should be common to all
  // datasets within an MC campaign.
  // So we use the less disruptive solution in Athena for now...
  if(m_barcodeFromMetadata>0){
    // retrieve the value for the current sample from metadata
#ifndef XAOD_STANDALONE
    // Usage of metadata is only possible in Athena (not supported by dual-use tools yet)...
    bool found = false;
    int barcodeOffset_tmp(0);
    ATH_MSG_DEBUG(" Look for barcode offset in  metadata ... ");
    try {
      StatusCode sc= AthAnalysisHelper::retrieveMetadata("/Simulation/Parameters","SimBarcodeOffset",barcodeOffset_tmp) ;
      found = sc.isSuccess();
    } catch(std::exception &e) {
      ATH_MSG_DEBUG(" Could not retrieve barcode offset in metadata  : "<< e.what());
    }

    if(found){
      // Found this in the metadata -- hurray!
      ATH_MSG_DEBUG("Retrieved from metadata: "<<barcodeOffset_tmp<<" will override "<<m_barcodeOffset);
      m_barcodeOffset = barcodeOffset_tmp;
    } else {
      if(m_barcodeFromMetadata==1) {
        // Required that it come from metadata -- fail if this didn't work
        ATH_MSG_ERROR("Can not retrieve metadata info  /Simulation/Parameters SimBarcodeOffset");
        return StatusCode::FAILURE;
      }
      // m_barcodeFromMetadata == 2
      // Requested it from the metadata, with a fall-back
      ATH_MSG_DEBUG("NOT Retrieved from metadata, use default "<<m_barcodeOffset);
    }
#else // standalone :
    if (m_barcodeFromMetadata==1) {
      // Required it from the metadata -- fail if not in athena
      ATH_MSG_ERROR("Can't retrieve automatically the truth barcode offset outside Athena. Please set the CopyTruthJetParticles::BarCodeOffset for this specific sample");
      return StatusCode::FAILURE;
    } else {
      // Requested it from the metadata -- fall back is the only option
      ATH_MSG_WARNING("Can't retrieve automatically the truth barcode offset outside Athena.  Falling back to offset property: " << m_barcodeOffset);
    }
#endif
  }

  // Ensure consistency in the photon dressing treatment
  if (m_photonCone>0 && !m_dressingName.empty()){
    ATH_MSG_ERROR("Requested both dR and decoration based photon dressing. This is unphysical; please choose one.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


int CopyTruthJetParticles::execute() const {

  ATH_MSG_DEBUG("Executing " << name());

  std::vector<const xAOD::TruthParticle*> promptLeptons;
  promptLeptons.reserve(10);

  /// we recopy the CopyTruthParticles::execute() below

  // Retrieve the xAOD truth objects
  const xAOD::TruthEventContainer* xTruthEventContainer(nullptr);
  ASG_CHECK( evtStore()->retrieve( xTruthEventContainer, "TruthEvents"));

  // Make a new TruthParticleContainer and link it to StoreGate
  if (evtStore()->contains<xAOD::TruthParticleContainer>(m_outputname))
    ATH_MSG_ERROR("Tag input TruthParticleContainer " << m_outputname << " already exists");

  ConstDataVector<xAOD::TruthParticleContainer> *ipc = new ConstDataVector<xAOD::TruthParticleContainer>(SG::VIEW_ELEMENTS);
  if (evtStore()->record(ipc, m_outputname).isFailure())
    ATH_MSG_ERROR("Failed to record a new TruthParticleContainer " << m_outputname);

  // Classify particles for tagging and add to the TruthParticleContainer
  const xAOD::TruthEvent* evt = *xTruthEventContainer->begin();
  std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin> originMap;
  originMap.clear();
  size_t numCopied = 0;
  for (unsigned int ip = 0; ip < evt->nTruthParticles(); ++ip) {
    const xAOD::TruthParticle* tp = evt->truthParticle(ip);
    if(tp == NULL) continue;
    if (tp->pt() < m_ptmin)
        continue;

    if (classifyJetInput(tp, promptLeptons, originMap)) { // Modification w.r.t CopyTruthParticles : pass the barcodeoffset argument
      ipc->push_back(tp);
      numCopied += 1;
    }
  }

  ATH_MSG_DEBUG("Copied " << numCopied << " truth particles into " << m_outputname << " TruthParticle container");

  return 0;
}


bool CopyTruthJetParticles::comesFrom( const xAOD::TruthParticle* tp, const int pdgID, std::vector<int>& used_vertices ) const {
  // If it's not a particle, then it doesn't come from something...
  if (!tp) return false;
  // If it doesn't have a production vertex or has no parents, it doesn't come from much of anything
  if (!tp->prodVtx() || tp->nParents()==0) return false;
  // If we have seen it before, then skip this production vertex
  if (std::find(used_vertices.begin(),used_vertices.end(), tp->prodVtx()->barcode())!=used_vertices.end()) return false;
  // Add the production vertex to our used list
  used_vertices.push_back( tp->prodVtx()->barcode() );
  // Loop over the parents
  for (size_t par=0;par<tp->nParents();++par){
    // Check for null pointers in case of skimming
    if (!tp->parent(par)) continue;
    // Check for a match
    if (tp->parent(par)->absPdgId()==pdgID) return true;
    // Recurse on this parent
    if (comesFrom(tp->parent(par), pdgID, used_vertices)) return true;
  }
  // No hits -- all done with the checks!
  return false;
}

