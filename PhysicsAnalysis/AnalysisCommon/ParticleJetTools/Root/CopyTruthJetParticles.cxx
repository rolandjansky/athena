/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthJetParticles.h"
#include "xAODTruth/TruthVertex.h"
#include "TruthUtils/PIDHelpers.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteHandle.h"
#include "AsgMessaging/Check.h"

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
  , m_classif("",this)
{
  declareProperty("IncludeBSMNonInteracting",  m_includeBSMNonInt=false, "Include noninteracting BSM particles (excluding neutrinos) in the output collection");
  declareProperty("IncludeNeutrinos",  m_includeNu=false, "Include neutrinos in the output collection");
  declareProperty("IncludeMuons",      m_includeMu=false, "Include muons in the output collection");
  declareProperty("IncludePromptLeptons",  m_includePromptLeptons=true,  "Include leptons from prompt decays (i.e. not from hadron decays) in the output collection");
  declareProperty("IncludePromptPhotons",  m_includePromptPhotons=true,  "Include photons from Higgs and other decays that produce isolated photons");
  declareProperty("ChargedParticlesOnly", m_chargedOnly=false, "Include only charged particles in the output collection");

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

StatusCode CopyTruthJetParticles::initialize() {
  ATH_CHECK(m_classif.retrieve());

  ATH_CHECK(m_truthEventKey.initialize());
  ATH_CHECK(m_outTruthPartKey.initialize());

  // Ensure consistency in the photon dressing treatment
  if (m_photonCone>0 && !m_dressingName.empty()){
    ATH_MSG_ERROR("Requested both dR and decoration based photon dressing. This is unphysical; please choose one.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}



bool CopyTruthJetParticles::classifyJetInput(const xAOD::TruthParticle* tp, int barcodeOffset,
                                             std::vector<const xAOD::TruthParticle*>& promptLeptons,
                                             std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap) const {

  // Check if this thing is a candidate to be in a truth jet
  //  First block is largely copied from isGenStable, which works on HepMC only
  if (tp->barcode()>=barcodeOffset) return false; // Particle is from G4
  int pdgid = tp->pdgId();
  if (pdgid==21 && tp->e()==0) return false; // Work around for an old generator bug

  // -- changed for dark jet clustering -- //
  //if ( tp->status() %1000 !=1 ) return false; // Stable!
  if ( tp->status()%1000!=1 && !m_includeDark ) return false; // dark hadrons will not be status 1
  // ----------------------------------- //
  
  // Easy classifiers by PDG ID
  if(MCUtils::PID::isNeutrino(pdgid)) {
    if (!m_includeNu) return false;
  } else {
    if (!m_includeBSMNonInt && MC::isNonInteracting(pdgid)) return false;
  }
  if (!m_includeMu && abs(pdgid)==13) return false;

  // Already built a list of prompt leptons, just use it here
  if (!m_includePromptLeptons && std::find(promptLeptons.begin(),promptLeptons.end(),tp)!=promptLeptons.end()){
    ATH_MSG_VERBOSE("Veto prompt lepton (" << pdgid << ") with pt " << tp->pt() << " origin " << getPartOrigin( tp, originMap ));
    return false;
  }

  // Extra catch.  If we aren't supposed to include prompt leptons, we aren't supposed to include prompt neutrinos
  if (!m_includePromptLeptons && MCUtils::PID::isNeutrino(pdgid) && isPrompt(tp,originMap)){
    return false;
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

int CopyTruthJetParticles::setBarCodeFromMetaDataCheck() const{ 

  ATH_MSG_DEBUG(" in call once barcode offset is"<<m_barcodeOffset);

  // if  m_barcodeFromMetadata  is  set to  zero,  no search for barcode offset is performed in metadata 
  if(m_barcodeFromMetadata  == 0 )  ATH_MSG_INFO( "No barcode offset is searched for in metadata, its value is set to:  "<<m_barcodeOffset);

  // if  m_barcodeFromMetadata  is  set to  1, the check is performed  and a warning is set out
  // explicitly set out when nothing is foun
 if(m_barcodeFromMetadata>0){
    // Usage of metadata is only possible in Athena (not supported by dual-use tools yet)...
#ifndef XAOD_STANDALONE
    bool found = false;
    // retrieve the value for the current sample from metadata
    
    int barcodeOffset_tmp(0);
    ATH_MSG_INFO("Look for barcode offset in  metadata ... ");
    try {
      StatusCode sc= AthAnalysisHelper::retrieveMetadata("/Simulation/Parameters","SimBarcodeOffset",barcodeOffset_tmp) ;
      found = sc.isSuccess();
    } catch(std::exception &e) {
      //      ATH_MSG_DEBUG(" Could not retrieve barcode offset in metadata  : "<< e.what());
      ATH_MSG_DEBUG(" Could not retrieve barcode offset in metadata  : "<< e.what());
    }
    
    if(found){
      // barcode offset is found in the metadata  and it is set
      m_barcodeOffset = barcodeOffset_tmp;
      ATH_MSG_INFO(" Barcode offset retrieved from metadata. Its value is :  "<< m_barcodeOffset);
    } else {    

      if(m_barcodeFromMetadata==1) {
        // Required that it come from metadata -- fail if this didn't work
        ATH_MSG_ERROR("Can not retrieve metadata info  /Simulation/Parameters SimBarcodeOffset");
        throw runtime_error("Tried to retrieve metadata info and failed, and was not supposed to fail");
      }
      // m_barcodeFromMetadata == 2
      // Requested it from the metadata, with a fall-back
      ATH_MSG_DEBUG("NOT Retrieved from metadata, use default "<<m_barcodeOffset);
    }
#else // standalone :
    if (m_barcodeFromMetadata==1) {
      // Required it from the metadata -- fail if not in athena
      ATH_MSG_ERROR("Can't retrieve automatically the truth barcode offset outside Athena. Please set the CopyTruthJetParticles::BarCodeOffset for this specific sample");
      throw runtime_error("Tried to retrieve metadata info and failed, and was not supposed to fail");
    } else {
      // Requested it from the metadata -- fall back is the only option
      ATH_MSG_WARNING("Can't retrieve automatically the truth barcode offset outside Athena.  Falling back to offset property: " << m_barcodeOffset);
    }
#endif
  }  
  return 0;
}

int CopyTruthJetParticles::execute() const {

  // retrieve barcode Offset for this event from metadata.
  // We'd need a cleaner solution where this offset is set only at 
  // each new file, but this requires some tool interface which does 
  // not exist in RootCore yet. 
  // So we use the less disruptive solution in Athena for now...

  // the function used below is 
   // std::call_once(metaDataFlag,basicMetaDataCheck(), this);
  //  std::call_once(metaDataFlag,this->basicMetaDataCheck());
  // the syntax is explained in http://stackoverflow.com/questions/23197333/why-is-this-pointer-needed-when-calling-stdcall-once
  // this syntax requires the call_once function to receive the object the function is called upon
  //": these are all non-static member functions , they act on objects 
  // they need the "this" pointer which always point to the object the function is working on
  //http://www.learncpp.com/cpp-tutorial/812-static-member-functions/ 

  //  m_barcodeOffset = m_barcodeOffset;
    ATH_MSG_DEBUG(" barcode offset before the check  is"<<m_barcodeOffset);

  //  std::call_once(metaDataFlag,&CopyTruthJetParticles::basicMetaDataCheck,this,barcodeOffset);
  // this call happens only once and it modifies m_barcodeOffset
  // Note that catching the return value of this is rather complicated, so it throws rather than returning errors
  std::call_once(metaDataFlag,&CopyTruthJetParticles::setBarCodeFromMetaDataCheck, this);

  std::vector<const xAOD::TruthParticle*> promptLeptons;
  promptLeptons.reserve(10);
  
  /// we recopy the CopyTruthParticles::execute() below, passing the barcodeOffset to the classify function.
  //  we can not change m_barcodeOffset param since this is a const method.

  // Retrieve the xAOD truth objects
  auto truthEvents = SG::makeHandle(m_truthEventKey);
  if ( !truthEvents.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve truth event container " << m_truthEventKey.key());
    return 1;
  }
  
  // Classify particles for tagging and add to the TruthParticleContainer
  std::unique_ptr<ConstDataVector<xAOD::TruthParticleContainer> > ptruth(new ConstDataVector<xAOD::TruthParticleContainer>(SG::VIEW_ELEMENTS));
  std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin> originMap;
  originMap.clear();
  size_t numCopied = 0;
  const xAOD::TruthEvent* hsevt = truthEvents->front();
  if(!hsevt) {
    ATH_MSG_ERROR("Null pointer received for first truth event!");
    return 1;
  }

  for (unsigned int ip = 0; ip < hsevt->nTruthParticles(); ++ip) {
    const xAOD::TruthParticle* tp = hsevt->truthParticle(ip);
    if(tp == NULL) continue;
    if (tp->pt() < m_ptmin)
        continue;
    // Cannot use the truth helper functions; they're written for HepMC
    // Last two switches only apply if the thing is a lepton and not a tau
    int pdgid = tp->pdgId();
    if ((abs(pdgid)==11 || abs(pdgid)==13) && tp->hasProdVtx()){
      // If this is a prompt, generator stable lepton, then we can use it
      if(tp->status()==1 && tp->barcode()<m_barcodeOffset && isPrompt(tp,originMap)){
        promptLeptons.push_back(tp);
      }
    }
  }

  for (size_t itp(0); itp<hsevt->nTruthParticles(); ++itp) {
    const xAOD::TruthParticle* tp = hsevt->truthParticle(itp);
    if(tp == NULL) continue;
    if (tp->pt() < m_ptmin)
        continue;

      // Modification: The barcode offset is updated by the call to setBarCodeFromMetaDataCheck
    if (classifyJetInput(tp, m_barcodeOffset, promptLeptons, originMap)) { 
      ptruth->push_back(tp);
      numCopied += 1;
    }
  }

  ATH_MSG_DEBUG("Copied " << numCopied << " truth particles into " << m_outputname << " TruthParticle container");

  // record
  auto truthParticles_out = SG::makeHandle(m_outTruthPartKey);
  ATH_MSG_DEBUG("Recorded truth particle collection " << m_outTruthPartKey.key());
  // notify
  if (!truthParticles_out.put(std::move(ptruth))) {
    ATH_MSG_ERROR("Unable to write new TruthParticleContainer to event store: " 
                  << m_outTruthPartKey.key());
  } else {
    ATH_MSG_DEBUG("Created new TruthParticleContainer in event store: " 
                  << m_outTruthPartKey.key());
  }

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

