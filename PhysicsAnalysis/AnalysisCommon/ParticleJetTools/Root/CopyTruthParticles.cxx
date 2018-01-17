/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthParticles.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "TruthUtils/PIDHelpers.h"
#include "AsgTools/Check.h"

using namespace std;


CopyTruthParticles::CopyTruthParticles(const std::string& name)
  : AsgTool(name)
{
  declareProperty("OutputName", m_outputname="TagInputs", "Name of the resulting IParticle collection");
  declareProperty("PtMin", m_ptmin=0, "Minimum pT of particles to be accepted for tagging (in MeV)");
}


int CopyTruthParticles::execute() const {
  // Retrieve the xAOD truth objects
  const xAOD::TruthEventContainer* xTruthEventContainer = NULL;
  ASG_CHECK( evtStore()->retrieve( xTruthEventContainer, "TruthEvents"), 1);

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
    if (!tp || tp->pt() < m_ptmin)
        continue;

    if (classify(tp)) {
      ipc->push_back(tp);
      numCopied += 1;
    }
  }

  ATH_MSG_DEBUG("Copied " << numCopied << " truth particles into " << m_outputname << " TruthParticle container");

  return 0;
}
