/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthJetParticles.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "TruthUtils/PIDUtils.h"

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

namespace MC {

  /// TEMPORARY copy from TruthUtils/ in the ParticleJetTools-00-03-33-branch
  /// @brief Identify if the particle with given PDG ID would not interact with the detector, i.e. not a neutrino or WIMP
  /// @deprecated Use MCUtils functions instead, e.g. isStronglyInteracting || isEMInteracting
  inline bool isNonInteracting(int pid) {
    const int apid = abs(pid);
    if (apid == 12 || apid == 14 || apid == 16) return true; //< neutrinos
    if (apid == 1000022 || apid == 1000024 || apid == 5100022) return true; // SUSY & KK photon and Z partners
    if (apid == 39 || apid == 1000039 || apid == 5000039) return true; //< gravitons: standard, SUSY and KK
    if (apid == 9000001 || apid == 9000002 || apid == 9000003 || apid == 9000004 || apid == 9000005 || apid == 9000006) return true; //< exotic particles from monotop model
    return false;
  }

}

CopyTruthJetParticles::CopyTruthJetParticles(const std::string& name)
  : CopyTruthParticles(name) ,
    m_maxAbsEta(5)
  , m_barcodeOffset(200000)
  , m_barcodeFromMetadata(2)
{
  declareProperty("IncludeNeutrinos",  m_includeNu=false, "Include neutrinos in the output collection");
  declareProperty("IncludeMuons",      m_includeMu=false, "Include muons in the output collection");
  declareProperty("IncludeWZLeptons",  m_includeWZ=true,  "Include leptons from W/Z decays in the output collection");
  declareProperty("IncludeTauLeptons", m_includeTau=true, "Include leptons from tau decays in the output collection");
  declareProperty("MaxAbsEta", m_maxAbsEta);
  declareProperty("BarCodeOffset", m_barcodeOffset);
  declareProperty("BarCodeFromMetadata", m_barcodeFromMetadata);
}


namespace {

  class OriginHelper {
    /// This helper is here to avoid infinite loop in fromWZ()
    /// They implement the same selection as CopyTruthJetParticles::fromWZ CopyTruthJetParticles::fromTau
    /// It's supposed to be a fast cache compatible fix to be used 
    /// for the mc15 production. See  ATLASRECTS-2121.

  public:
    OriginHelper() {m_visited.reserve(4);}

    bool fromWZ( const xAOD::TruthParticle* tp ) {
      m_visited.clear();
      int res = fromWZ_recc(tp);
      if(res == -1 ){ 
        return true;
      }
      return bool(res);
    }

    int fromWZ_recc(const xAOD::TruthParticle* tp ) {
      // check tp was not already seen to avoid infinite loops
      for(const xAOD::TruthParticle* p : m_visited) if( p == tp ) return -1;
      // we've visited tp
      m_visited.push_back(tp);

      bool foundLoop =false;
      if (!tp->hasProdVtx()) return 0;
      for (size_t i=0;i<tp->prodVtx()->nIncomingParticles();++i){
        if (MC::PID::isHadron( tp->prodVtx()->incomingParticle(i)->pdgId() ) ) return 0;
        if ( abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) < 9 ) return 1;
        if ( tp->prodVtx()->incomingParticle(i)->pdgId() == tp->pdgId() ) {
          int ret= fromWZ_recc( tp->prodVtx()->incomingParticle(i) );        
          if( ret != -1) return ret;
          foundLoop = true;
        }
      }
      if ( foundLoop) return 1; // in this case we assume it's a W or Z decay. Is this correct ?
      return 0;
    }


    bool fromTau( const xAOD::TruthParticle* tp ) {
      if (!tp->hasProdVtx()) return false;
      for (size_t i=0;i<tp->prodVtx()->nIncomingParticles();++i){
        if ( abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) == 15 ) return true;
        if (MC::PID::isHadron( tp->prodVtx()->incomingParticle(i)->pdgId() ) ||
            abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) < 9 ) return false;
        if ( tp->prodVtx()->incomingParticle(i)->pdgId() == tp->pdgId() ) return fromWZ( tp->prodVtx()->incomingParticle(i) );
      }
      return false;
    
    }

  
    std::vector<const xAOD::TruthParticle*> m_visited;
  };

}


bool CopyTruthJetParticles::classifyJetInput(const xAOD::TruthParticle* tp, int barcodeOffset) const {

  // Check if this thing is a candidate to be in a truth jet
  //  First block is largely copied from isGenStable, which works on HepMC only
  if (tp->barcode()>=barcodeOffset) return false; // Particle is from G4
  if (tp->pdgId()==21 && tp->e()==0) return false; // Work around for an old generator bug
  if ( tp->status()!=1 ) return false; // Stable!
  
  // Easy classifiers by PDG ID
  if (!m_includeNu && MC::isNonInteracting(tp->pdgId())) return false;
  if (!m_includeMu && abs(tp->pdgId())==13) return false;

  OriginHelper ohelp; // now using this helper to avoid infinite loops.

  // Last two switches only apply if the thing is a lepton and not a tau
  if (MC::PID::isLepton(tp->pdgId()) && abs(tp->pdgId())!=15 && tp->hasProdVtx()){
    if (!m_includeWZ && ohelp.fromWZ( tp )) return false;
    if (!m_includeTau && ohelp.fromTau( tp )) return false;
  }

  if(fabs(tp->eta())>m_maxAbsEta) return false;
  // Made it! 
  return true;
}





bool CopyTruthJetParticles::fromWZ( const xAOD::TruthParticle* tp ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function 
  //  it's used in place of code in MCTruthClassifier as long as this packagage is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Hit a hadron -> return false
  // Hit a parton -> return true
  //   This catch is important - we *cannot* look explicitly for the W or Z, because some
  //    generators do not include the W or Z in the truth record (like Sherpa)
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!tp->hasProdVtx()) return false;
  for (size_t i=0;i<tp->prodVtx()->nIncomingParticles();++i){
    if (MC::PID::isHadron( tp->prodVtx()->incomingParticle(i)->pdgId() ) ) return false;
    if ( abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) < 9 ) return true;
    if ( tp->prodVtx()->incomingParticle(i)->pdgId() == tp->pdgId() ) {
      return fromWZ( tp->prodVtx()->incomingParticle(i) );
    }
  }
  return false;
}

bool CopyTruthJetParticles::fromTau( const xAOD::TruthParticle* tp ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function 
  //  it's used in place of code in MCTruthClassifier as long as this packagage is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Find a tau -> return true
  // Find a hadron or parton -> return false
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!tp->hasProdVtx()) return false;
  for (size_t i=0;i<tp->prodVtx()->nIncomingParticles();++i){
    if ( abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) == 15 ) return true;
    if (MC::PID::isHadron( tp->prodVtx()->incomingParticle(i)->pdgId() ) ||
        abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) < 9 ) return false;
    if ( tp->prodVtx()->incomingParticle(i)->pdgId() == tp->pdgId() ) return fromWZ( tp->prodVtx()->incomingParticle(i) );
  }
  return false;
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
  size_t numCopied = 0;
  for (unsigned int ip = 0; ip < evt->nTruthParticles(); ++ip) {
    const xAOD::TruthParticle* tp = evt->truthParticle(ip);
    if (tp->pt() < m_ptmin)
        continue;

    if (classifyJetInput(tp, barcodeOffset)) { // Modification w.r.t CopyTruthParticles : pass the barcodeoffset argument
      ipc->push_back(tp);
      numCopied += 1;
    }
  }

  ATH_MSG_DEBUG("Copied " << numCopied << " truth particles into " << m_outputname << " TruthParticle container");

  return 0;
}

