/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetPseudojetCopier.cxx

#include "JetRec/JetPseudojetCopier.h"
#include "JetEDM/PseudoJetMap.h"

using std::string;
using fastjet::PseudoJet;
using jet::PseudoJetMap;
using xAOD::Jet;
using xAOD::JetContainer;

//**********************************************************************

namespace {

  /// Some helpers to sort according to p_T
  SG::AuxElement::Accessor<float> ptAcc("JetConstitScaleMomentum_pt");
  struct ConstitPtComp {
    bool operator()(const xAOD::Jet* j1, const xAOD::Jet * j2) {
      return ptAcc(*j1) > ptAcc(*j2) ;
    }
  };

  // Helpers to check equality according to constituents content.
  bool hasConstituent(const xAOD::Jet* j, const xAOD::IParticle* p){
    int i = 0; int nConst = j->numConstituents();
    while(i<nConst ){ 
      if (j->rawConstituent(i)==p) return true;
      i++; 
    }
    return false;
  }

  int differentJets(const xAOD::Jet* j1, const xAOD::Jet * j2, bool fullComp =true){
    if ( j1->numConstituents() != j2->numConstituents() ) return 1;
    if ( j1->numConstituents() == 0 ) return 2;
    if ( fullComp ) {
      for(size_t i=0;i<j1->numConstituents();i++){
        if(!hasConstituent(j2,j1->rawConstituent(i))) return 100+i;
      }
      return 0;        
    }
    // Assume same input and same jet finding alg has been used :
    if (j1->rawConstituent(0) == j2->rawConstituent(0)) return 0;
    return 200;
  }
  
}


//**********************************************************************

JetPseudojetCopier::JetPseudojetCopier(const std::string& name)
  : asg::AsgTool(name), m_hpjr("",this)  {
  declareProperty("DestinationContainer", m_dstname);
  declareProperty("Label", m_label ="JetPseudojetMap");
  declareProperty("JetPseudojetRetriever", m_hpjr);
}

//**********************************************************************

StatusCode JetPseudojetCopier::initialize() {
  if ( ! m_hpjr.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Unable to retrieve jet pseudojet retriever.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetPseudojetCopier::
copy(const JetContainer& srcjets, const JetContainer& dstjets, string pjmapname) const {

  if ( ! m_hpjr.retrieve().isSuccess() ) {
    ATH_MSG_WARNING("Unable to retrieve jet pseudojet retriever.");
    return 1;
  }

  // Check map used to store pseudojets.
  PseudoJetMap* ppjmap;
  if ( ! evtStore()->contains<PseudoJetMap>(pjmapname) ) {
    if ( ! evtStore()->record(new PseudoJetMap, pjmapname).isSuccess() ) {
      ATH_MSG_ERROR("Unable to create pseudojet map.");
    }
  }
  if ( ! evtStore()->retrieve(ppjmap, pjmapname).isSuccess() ) {
    ATH_MSG_WARNING("Unable to retrieve pseudojet map.");
    return 1;
  }
  PseudoJetMap& pjmap = *ppjmap;
  
  ATH_MSG_DEBUG("Copying...     source size=" << srcjets.size());
  ATH_MSG_DEBUG("          destination size=" << dstjets.size());

  // build sorted list
  // (sorting by constit scale pt -> almost 1-to-1 matching
  // between source and target containers. This renders the matching loops
  // below faster).
  std::list<const Jet*> sortedTarget(dstjets.begin(), dstjets.end());
  sortedTarget.sort(ConstitPtComp());
  std::list<const Jet*> sortedSource(srcjets.begin(), srcjets.end());
  sortedSource.sort(ConstitPtComp());

  size_t sourceIndex=0;
  // Loop over source jets 
  for ( auto sourceIt = sortedSource.begin(); sourceIt!=sortedSource.end(); ++sourceIt ) {
    const Jet* pjetin = *sourceIt;
    if ( pjetin == nullptr ) {
      ATH_MSG_WARNING("Skipping missing input jet.");
      continue;
    }
    ATH_MSG_DEBUG("   Checking jet " << sourceIndex << "   pt=" << ptAcc(*pjetin) );
    // find a matching jet in the target container
    //first remove null pointers
    sortedTarget.remove_if([](const Jet * pJetOut){return pJetOut == nullptr;});
    //
    for ( auto targetIt=sortedTarget.begin(); targetIt!=sortedTarget.end(); ++targetIt ) {
      const Jet* pjetout = *targetIt;
      int jetdiff = differentJets(pjetin, pjetout);
      bool isSame = jetdiff == 0;
      ATH_MSG_DEBUG("            vs     pt="<<ptAcc(*pjetout) << " jetdiff= " << jetdiff );    
      if ( isSame ){ // found a jet 
        ATH_MSG_DEBUG(" -------> identical to pt="<< ptAcc(*pjetout));
        const PseudoJet* ppj = m_hpjr->pseudojet(*pjetin);
        ATH_MSG_VERBOSE("Recording pseudojet " << long(ppj) << " for jet " << long(pjetout)
                        << " in map " << pjmapname);
        pjmap[pjetout] = ppj;
        // remove target from list to reduce next search length.
        sortedTarget.erase(targetIt); //the returned iterator is unused, as the loop is terminated
        break;
      }
      ATH_MSG_VERBOSE("No match found for destination jet " << long(pjetout));
    } // end target loop

    auto targetIt = sortedTarget.begin(); // re-start from start
    ATH_MSG_DEBUG("   Checking jet " << sourceIndex << "   done" );
    if ( targetIt == sortedTarget.end() ) break; // no more target left.   
    ++sourceIndex;
  }

  return 0;
}

//**********************************************************************

int JetPseudojetCopier::
copy(const JetContainer& srcjets, string dstname, string pjmapname) const {
  const xAOD::JetContainer* pdstjets;
  if ( ! evtStore()->retrieve(pdstjets, dstname).isSuccess() ) {
    ATH_MSG_WARNING("Unable to retrieve destination jet container.");
    return 1;
  }
  return copy(srcjets, *pdstjets, pjmapname);
}

//**********************************************************************

int JetPseudojetCopier::process(const JetContainer& srcjets) const {
  return copy(srcjets, m_dstname, m_label);
}

//**********************************************************************
