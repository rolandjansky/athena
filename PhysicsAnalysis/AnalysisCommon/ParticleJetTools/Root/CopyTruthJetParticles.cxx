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
  declareProperty("IncludeNeutrinos",  m_includeNu=false, "Include neutrinos in the output collection");
  declareProperty("IncludeMuons",      m_includeMu=false, "Include muons in the output collection");
  declareProperty("IncludeWZLeptons",  m_includeWZ=true,  "Include leptons from W/Z decays in the output collection");
  declareProperty("IncludeTauLeptons", m_includeTau=true, "Include leptons from tau decays in the output collection");
  declareProperty("MaxAbsEta", m_maxAbsEta);
  declareProperty("BarCodeOffset", m_barcodeOffset);
  declareProperty("BarCodeFromMetadata", m_barcodeFromMetadata);

  declareProperty("MCTruthClassifier", m_classif);

  declareProperty("FSRPhotonCone", m_photonCone);
}

bool CopyTruthJetParticles::classifyJetInput(const xAOD::TruthParticle* tp, int barcodeOffset,
                                             std::vector<const xAOD::TruthParticle*>& WZleptons,
                                             std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap) const {

  // Check if this thing is a candidate to be in a truth jet
  //  First block is largely copied from isGenStable, which works on HepMC only
  if (tp->barcode()>=barcodeOffset) return false; // Particle is from G4
  int pdgid = tp->pdgId();
  if (pdgid==21 && tp->e()==0) return false; // Work around for an old generator bug
  if ( tp->status() %1000 !=1 ) return false; // Stable!
  
  // Easy classifiers by PDG ID
  if (!m_includeNu && MC::isNonInteracting(pdgid)) return false;
  if (!m_includeMu && abs(pdgid)==13) return false;

  // Cannot use the truth helper functions; they're written for HepMC
  // Last two switches only apply if the thing is a lepton and not a tau
  if (MC::PID::isLepton(pdgid) && abs(pdgid)!=15 && tp->hasProdVtx()){
    bool isFromWZ = fromWZ( tp, originMap );
    if(isFromWZ && (abs(pdgid)==11 || abs(pdgid)==13)) WZleptons.push_back(tp);
    if (!m_includeWZ && isFromWZ) {
      ATH_MSG_VERBOSE("Veto lepton (" << pdgid << ") from W/Z");
      return false;
    }
    if (!m_includeTau && fromTau( tp, originMap )) {
      ATH_MSG_VERBOSE("Veto lepton (" << pdgid << ") from tau");
      return false;
    }
  }

  if(!m_includeWZ && abs(pdgid)==22 && ( fromWZ( tp, originMap ) || getPartOrigin( tp, originMap )==FSRPhot ) ) {
    // Only exclude photons within deltaR of leptons (if m_photonCone<0, exclude all photons)
    if(m_photonCone>0) {
      for(const auto& lep : WZleptons) {
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

  if(fabs(tp->eta())>m_maxAbsEta) return false;
  // Made it! 
  return true;
}

bool CopyTruthJetParticles::fromWZ( const xAOD::TruthParticle* tp,
				    std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap ) const
{
  ParticleOrigin orig = getPartOrigin(tp, originMap);
  if(orig==WBoson || orig==ZBoson || orig==DiBoson) return true;
  return false;
}

bool CopyTruthJetParticles::fromTau( const xAOD::TruthParticle* tp,
				     std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap ) const
{
  ParticleOrigin orig = getPartOrigin(tp, originMap);
  if(orig==TauLep) return true;
  return false;
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

int CopyTruthJetParticles::execute() const {

  //*******************************
  // retrieve barcode Offset for this event from metadata.
  // We'd need a cleaner solution where this offset is set only at 
  // each new file, but this requires some tool interface which does 
  // not exist in RootCore yet. 
  // So we use the less disruptive solution in Athena for now...
  int barcodeOffset = m_barcodeOffset;
  if(m_barcodeFromMetadata>0){
    bool found = false;
    // retrieve the value for the current sample from metadata
#ifndef XAOD_STANDALONE
    // Usage of metadata is only possible in Athena (not supported by dual-use tools yet)...
    int barcodeOffset_tmp(0);
    ATH_MSG_DEBUG(" Look for barcode offset in  metadata ... ");
    try {
      StatusCode sc= AthAnalysisHelper::retrieveMetadata("/Simulation/Parameters","SimBarcodeOffset",barcodeOffset_tmp) ;
      found = sc.isSuccess();
    } catch(std::exception &e) {
      ATH_MSG_DEBUG(" Could not retrieve barcode offset in metadata  : "<< e.what());
    }

    if(found){
      barcodeOffset = barcodeOffset_tmp;
      ATH_MSG_DEBUG(" Retrieved from metadata :  "<< barcodeOffset);
    } else {
      if(m_barcodeFromMetadata==1) {
        ATH_MSG_ERROR( "Can not retrieve metadata info  /Simulation/Parameters SimBarcodeOffset ");
        return 1;
      }
      // m_barcodeFromMetadata == 2
      ATH_MSG_DEBUG(" NOT Retrieved from metadata, use default   "<< barcodeOffset << " _ "<< m_barcodeOffset);
    }
#else // standalone :
    ATH_MSG_ERROR(" Can't retrieve automatically the truth barcode offset outside Athena. Please set the CopyTruthJetParticles::BarCodeOffset for this specific sample");
    return 1;
#endif
  }

  std::vector<const xAOD::TruthParticle*> WZleptons;
  WZleptons.reserve(10);

  /// we recopy the CopyTruthParticles::execute() below, passing the barcodeOffset to the classify function.
  //  we can not change m_barcodeOffset param since this is a const method.

  // Retrieve the xAOD truth objects
  const xAOD::TruthEventContainer* xTruthEventContainer = NULL;
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
    if (tp->pt() < m_ptmin)
        continue;

    if (classifyJetInput(tp, barcodeOffset, WZleptons, originMap)) { // Modification w.r.t CopyTruthParticles : pass the barcodeoffset argument
      ipc->push_back(tp);
      numCopied += 1;
    }
  }

  ATH_MSG_DEBUG("Copied " << numCopied << " truth particles into " << m_outputname << " TruthParticle container");

  return 0;
}

