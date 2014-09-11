/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/CopyPseudoJets.h"

#include "JetEDM/FastJetLink.h"
#include "xAODJet/Jet_PseudoJet.icc"

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

  bool identicalJets(const xAOD::Jet* j1, const xAOD::Jet * j2, bool fullComp=false){
    
    if(j1->numConstituents() != j2->numConstituents() ) return false;
    if(j1->numConstituents()==0) return false;
    
    if(fullComp) {
      for(size_t i=0;i<j1->numConstituents();i++){
        if(!hasConstituent(j2,j1->rawConstituent(i))) return false;
      }
      return true;        
    }

    // Assume same input and same jet finding alg has been used :
    return (j1->rawConstituent(0) == j2->rawConstituent(0)) ;
  }
  
}


//**********************************************************************

CopyPseudoJets::CopyPseudoJets(const std::string& n)
: asg::AsgTool(n)  {
  declareProperty("TargetContainer", m_targetContainerName);
}

//**********************************************************************

int CopyPseudoJets::modify(xAOD::JetContainer& jets) const {
    
  const xAOD::JetContainer * targetCont;
  if ( ! evtStore()->retrieve( targetCont, m_targetContainerName).isSuccess() ) {
    ATH_MSG_WARNING("Unable to retrieve inpu jet container.");
    return 1;
  }
  if(targetCont->empty() ) return 0;

  ATH_MSG_DEBUG(" Modifying...  source size="<< jets.size() << " "<< targetCont->size()   );
  

  // build sorted list
  // (sorting by constit scale pt -> almost 1-to-1 matching
  // between source and target containers. This renders the matching loops
  // below faster).
  std::list<const xAOD::Jet*> sortedTarget(targetCont->begin(), targetCont->end() );
  sortedTarget.sort( ConstitPtComp() );
  
  std::list<const xAOD::Jet*> sortedSource(jets.begin(), jets.end() );
  sortedSource.sort( ConstitPtComp() );

  ATH_MSG_DEBUG(" Modifying...  source size="<< sortedSource.size() << " "<< sortedTarget.size()   );

  
  auto targetIt = sortedTarget.begin();
  auto targetEnd = sortedTarget.end();
  auto sourceIt = sortedSource.begin();
  auto sourceEnd = sortedSource.end();

  size_t sourceIndex=0;
  // Loop over source jets 
  for( ; sourceIt != sourceEnd; ++sourceIt){
    ATH_MSG_DEBUG("   Checking jet "<< sourceIndex << "   pt="<<ptAcc(**sourceIt) );

    // find a matching jet in the target container
    for( ; targetIt != targetEnd; ++targetIt){
      bool isSame = identicalJets(*sourceIt, *targetIt);
      ATH_MSG_DEBUG("            vs     pt="<<ptAcc(**targetIt) << " isSame= "<<isSame );    

      if( isSame ){ // found a jet 
        ATH_MSG_DEBUG(" -------> identical to pt="<< ptAcc(**targetIt));
        // WARNING doing a const cast !!
        const_cast<xAOD::Jet*>(*targetIt)->setPseudoJet( (*sourceIt)->getPseudoJet() );
        // remove target from list to reduce next search length.
        sortedTarget.erase(targetIt); 
        break;
      }
    } // end target loop

    targetIt = sortedTarget.begin(); // re-start from start
    ATH_MSG_DEBUG("   Checking jet "<< sourceIndex << "   done" );
    if( targetIt == targetEnd) break; // no more target left.   
    sourceIndex++;
  }
  
  
  return 0;
}

//**********************************************************************

