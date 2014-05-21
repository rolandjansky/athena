/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthPseudoJetGetter.cxx

#include "JetSimTools/TruthPseudoJetGetter.h"
#include "xAODTruth/TruthParticleContainer.h"

//**********************************************************************

TruthPseudoJetGetter::TruthPseudoJetGetter(const std::string &name)
: PseudoJetGetter(name),
  m_selector("JetTruthParticleSelectorTool") {
  declareProperty("TruthSelector", m_selector);
}

//**********************************************************************

StatusCode TruthPseudoJetGetter::initialize() {
  
  StatusCode sc= m_selector.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve TruthSelector.");
    return sc;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int TruthPseudoJetGetter::
appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const {
  if ( ! evtStore()->contains<xAOD::TruthParticleContainer>(m_incoll) ) {
    ATH_MSG_ERROR("Unable to find truth particle container: " << m_incoll);
    return 1;
  }
  ATH_MSG_DEBUG("Found TruthParticleContainer "<< m_incoll);
  const xAOD::TruthParticleContainer* truthParts = 0;
  evtStore()->retrieve(truthParts,m_incoll);
  std::vector<const xAOD::TruthParticle*> filteredParts(SG::VIEW_ELEMENTS);
  filteredParts.reserve(truthParts->size());
  const JetTruthParticleSelectorTool& selector = *m_selector;
  selector.setupEvent();
  int count = 0;
  for ( const xAOD::TruthParticle* truthp: *truthParts ) {
    if ( selector(truthp) ) {
      filteredParts.push_back(truthp);
    }
    ++count;
  }
  ATH_MSG_DEBUG( " Truth Filtering done.  outsize="<<filteredParts.size() << " / "<< truthParts->size()  );
  return append(filteredParts, psjs, pli);
}
