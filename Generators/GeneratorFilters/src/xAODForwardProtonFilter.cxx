/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// GeneratorFilters/ForwardProtonFilter
//
// Allows the user to search for forward intact protons originating from
// elastic and diffractive interactions. Event passes filters if there forward
// proton with xi and pt in given range. If double tag option is enables, such
// protons are requested on both sides. single_tagA/single_tagC require protons
// on a given side, regardless what's happenning on the other
//
// Authors:
// Rafal Staszewski   Jul 2011

#include "GeneratorFilters/xAODForwardProtonFilter.h"

xAODForwardProtonFilter::xAODForwardProtonFilter(const std::string &name, ISvcLocator *pSvcLocator)
    : GenFilter(name, pSvcLocator)
{
}

StatusCode xAODForwardProtonFilter::filterEvent()
{
  bool accepted_A = false;
  bool accepted_C = false;

  // Retrieve TruthMuons container
  const xAOD::TruthEventContainer *xTruthEventContainer;
  if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
  {
    ATH_MSG_ERROR("No TruthEvents collection with name "
                  << "TruthEvents"
                  << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  xAOD::TruthEventContainer::const_iterator itr;
  for (itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr)
  {
    const xAOD::TruthEvent *genEvt = (*itr);
    unsigned int nPart = genEvt->nTruthParticles();
    for (unsigned int iPart = 0; iPart < nPart; ++iPart)
    {
      const xAOD::TruthParticle *pitr = genEvt->truthParticle(iPart);
      // We're only interested in stable (status == 1) particles
      if (pitr->status() != 1)
        continue;
      // We are specifically looking for protons
      const long pid = pitr->pdgId();
      if (pid != 2212)
        continue;
      const double E = pitr->e();
      const double pz = pitr->pz();
      const double pt = pitr->pt();
      const double xi = (m_E0 - E) / m_E0;

      if (m_xiMin <= xi && xi <= m_xiMax && m_ptMin <= pt && pt <= m_ptMax)
      {
        accepted_C = (pz > 0);
        accepted_A = (pz < 0);
      }
      if (accepted_A)
        ATH_MSG_DEBUG("Found a proton with xi=" << xi << " on side C");
      if (accepted_C)
        ATH_MSG_DEBUG("Found a proton with xi=" << xi << " on side A");
      if (m_DoubleTag && (accepted_A && accepted_C))
        return StatusCode::SUCCESS;

      if (!m_DoubleTag)
        continue;
      // if Single tag is not requested, do or
      if (!m_Single_tagA && !m_Single_tagC && (accepted_A || accepted_C))
        return StatusCode::SUCCESS;

      // if single tag request - check for presence on particular side
      if (m_Single_tagA && accepted_A)
        return StatusCode::SUCCESS;
      if (m_Single_tagC && accepted_C)
        return StatusCode::SUCCESS;
    }
  }

  // If we get here the event doesn't pass the criteria
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
