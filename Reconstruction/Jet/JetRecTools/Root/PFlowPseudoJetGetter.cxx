/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PFlowPseudoJetGetter.cxx

#include "JetRecTools/PFlowPseudoJetGetter.h"
#include "xAODPFlow/PFOContainer.h"
#include "AthContainers/ConstDataVector.h"

PFlowPseudoJetGetter::PFlowPseudoJetGetter(const std::string &name)
  : PseudoJetGetter(name) {
}

int PFlowPseudoJetGetter::appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const { 
  const xAOD::PFOContainer* pfos(0);
  ATH_MSG_DEBUG("Retrieving xAOD container " << m_incoll << ", ghost scale="
		<< m_ghostscale  <<  ", isGhost=" << bool(m_ghostscale));
  if( evtStore()->retrieve(pfos,m_incoll).isFailure() ) {
    ATH_MSG_ERROR("Unable to find input collection: " << m_incoll);
    return 1;
  }

  // Select only the PFOs we want to use for jet building
  ConstDataVector<xAOD::PFOContainer> filteredpfos(SG::VIEW_ELEMENTS);
  for(const auto& pfo : *pfos) {
    // Technically this also skips 0 energy, which has no effect other than
    // on memory size, but is used in some workflows for pileup rejection
    bool reject = (m_skipNegativeEnergy && pfo->e()<-1*FLT_MIN);
    // Although it may seem pointless, we need to keep charged PFOs with energy==0
    // because for MET TST with PFlow, there may be high pt charged PFOs that receive
    // a weight of 0 due to being in dense showers, but need to be present for
    // overlap removal, because they don't retain these weights when added to the TST
    if( fabs(pfo->charge())>FLT_MIN) {
      const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
      reject = !PVMatchedAcc(*pfo);
    }
    if(!reject) filteredpfos.push_back(pfo);
  }

  return append(filteredpfos, psjs, pli);
}
