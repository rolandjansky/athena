/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GeneratorFilters/DecaysFinalStateFilter
//
// Picks events with a given number of quarks, leptons and neutrinos from
// decays of a list of specified resonances (e.g. W, Z, ...).
// Will work only if resonances are explicitly included in event record.
//
// Examples:
//
//   topAlg.DecaysFinalStateFilter.PDGAllowedParents = [ 23 ]
//   topAlg.DecaysFinalStateFilter.NQuarks = 2
//   topAlg.DecaysFinalStateFilter.NChargedLeptons = 2
//   -> picks semileptonic ZZ decays
//
//   topAlg.DecaysFinalStateFilter.PDGAllowedParents = [ 23, 24, -24 ]
//   topAlg.DecaysFinalStateFilter.NQuarks = 2
//   topAlg.DecaysFinalStateFilter.NChargedLeptons = 2
//   -> allows W(qq)Z(ll) and Z(qq)Z(ll)
//
// Authors:
// Kerim Suruliz Nov 2014
// Frank Siegert Nov 2014

#include "GeneratorFilters/DecaysFinalStateFilter.h"
//#include "GaudiKernel/MsgStream.h"
#include <cmath>


DecaysFinalStateFilter::DecaysFinalStateFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator) 
{
  declareProperty("PDGAllowedParents", m_PDGAllowedParents);

  declareProperty("NQuarks", m_NQuarks = -1);
  declareProperty("NChargedLeptons", m_NChargedLeptons = -1);
  declareProperty("NNeutrinos", m_NNeutrinos = -1);
  declareProperty("NPhotons", m_NPhotons = -1);

  declareProperty("MinNQuarks", m_MinNQuarks = 0);
  declareProperty("MinNChargedLeptons", m_MinNChargedLeptons = 0);
  declareProperty("MinNNeutrinos", m_MinNNeutrinos = 0);
  declareProperty("MinNPhotons", m_MinNPhotons = 0);
}


StatusCode DecaysFinalStateFilter::filterEvent() {
  int nChargedLeptons = 0;
  int nQuarks = 0;
  int nNeutrinos = 0;
  int nPhotons = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;


  for (auto part: *genEvt){
      // look only at the allowed parents (e.g. W, Z)
      bool allowedParent = false;
      for (size_t i=0; i<m_PDGAllowedParents.size(); ++i) {
        if ( part->pdg_id() == m_PDGAllowedParents[i]) allowedParent = true;
      }
      if (!allowedParent) continue;

      if (!part->end_vertex()) continue;

      for (auto opitr: *(part->end_vertex())) {
        int apid = std::abs(opitr->pdg_id());
        if (apid == 1 || apid == 2 || apid == 3 || apid == 4 || apid ==5) nQuarks++;
        if (apid == 11 || apid == 13 || apid == 15) nChargedLeptons++;
        if (apid == 12 || apid == 14 || apid == 16) nNeutrinos++;
        if (apid == 22) nPhotons++;
      }
    }
  }

  if (nQuarks < m_MinNQuarks || (m_NQuarks != -1 && nQuarks != m_NQuarks)) {
    setFilterPassed(false);
  }
  else if (nChargedLeptons < m_MinNChargedLeptons || (m_NChargedLeptons != -1 && nChargedLeptons != m_NChargedLeptons)) {
    setFilterPassed(false);
  } 
  else if (nNeutrinos < m_MinNNeutrinos || (m_NNeutrinos != -1 && nNeutrinos != m_NNeutrinos)) {
    setFilterPassed(false);
  } 
  else if (nPhotons < m_MinNPhotons || (m_NPhotons != -1 && nPhotons != m_NPhotons)) {
    setFilterPassed(false);
  } 
  else {
    setFilterPassed(true);
  }
  return StatusCode::SUCCESS;
}
