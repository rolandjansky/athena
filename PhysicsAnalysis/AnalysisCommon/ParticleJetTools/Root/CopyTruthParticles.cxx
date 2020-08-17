/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthParticles.h"
#include <memory>
#include "TruthUtils/PIDHelpers.h"
#include "AsgTools/Check.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteHandle.h"

using namespace std;


CopyTruthParticles::CopyTruthParticles(const std::string& name)
  : AsgTool(name)
{
  declareProperty("PtMin", m_ptmin=0, "Minimum pT of particles to be accepted for tagging (in MeV)");
}

StatusCode CopyTruthParticles::initialize() {

  ATH_CHECK(m_truthEventKey.initialize());
  ATH_CHECK(m_outTruthPartKey.initialize());

  return StatusCode::SUCCESS;
}

int CopyTruthParticles::execute() const {
  // Retrieve the xAOD truth objects
  auto truthEvents = SG::makeHandle(m_truthEventKey);
  if ( !truthEvents.isValid() ) {
    ATH_MSG_ERROR("Failed to retrieve truth event container " << m_truthEventKey.key());
    return 1;
  }
  
  // Classify particles for tagging and add to the TruthParticleContainer
  std::unique_ptr<ConstDataVector<xAOD::TruthParticleContainer> > ptruth(new ConstDataVector<xAOD::TruthParticleContainer>(SG::VIEW_ELEMENTS));
  size_t numCopied = 0;
  const xAOD::TruthEvent* hsevt = truthEvents->front();
  if(!hsevt) {
    ATH_MSG_ERROR("Null pointer received for first truth event!");
    return 1;
  }
  for (size_t itp(0); itp<hsevt->nTruthParticles(); ++itp) {
    const xAOD::TruthParticle* tp = hsevt->truthParticle(itp);
    if (!tp || tp->pt() < m_ptmin)
    if (!tp) {
      // This might result from a thinned truth collection, so is not
      // an error state.
      ATH_MSG_VERBOSE("Null pointer received for truth particle to be copied");
      continue;
    }
    if (tp->pt() < m_ptmin)
        continue;

    if (classify(tp)) {
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
