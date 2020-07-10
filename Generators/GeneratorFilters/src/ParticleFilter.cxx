/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Allows the user to search for particles with specified kinematics.
// It will pass if there is an particle with pT and eta or E in the specified range
#include "GeneratorFilters/ParticleFilter.h"

ParticleFilter::ParticleFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptcut",m_Ptmin = 10000.);
  declareProperty("Etacut",m_EtaRange = 10.0);
  declareProperty("Energycut",m_EnergyRange = 100000000.0);
  declareProperty("PDG",m_PDGID = 11);
  declareProperty("StatusReq",m_StatusReq=1);
  declareProperty("MinParts",m_MinParts=1);
  declareProperty("Exclusive",m_Exclusive=false);
}


StatusCode ParticleFilter::filterInitialize() {
  ATH_MSG_INFO("Ptcut=" << m_Ptmin);
  ATH_MSG_INFO("Etacut=" << m_EtaRange);
  ATH_MSG_INFO("Energycut=" << m_EnergyRange);
  ATH_MSG_INFO("PDG=" << m_PDGID);
  ATH_MSG_INFO("StatusReq=" << m_StatusReq);
  ATH_MSG_INFO("MinParts=" << m_MinParts);
  ATH_MSG_INFO("Exclusive=" << m_Exclusive);
  return StatusCode::SUCCESS;
}


StatusCode ParticleFilter::filterEvent() {
  int nParts = 0;
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto pitr: *genEvt) {
      if (std::abs(pitr->pdg_id()) != m_PDGID || !(m_StatusReq == -1 || pitr->status() == m_StatusReq)) continue;
      if (pitr->momentum().perp() >= m_Ptmin && std::abs(pitr->momentum().pseudoRapidity()) <= m_EtaRange && pitr->momentum().e() <= m_EnergyRange) {
      if ((!m_Exclusive)&&(m_MinParts == 1))  return StatusCode::SUCCESS; // Found at least one particle and we have an inclusive requirement
              
            // Count only particles not decaying to themselves
            bool notSelfDecay = true;
            if (pitr->end_vertex()) {
              for (auto  child: *(pitr->end_vertex())) {
                if ( child->pdg_id() == pitr->pdg_id() && HepMC::barcode(child)!=HepMC::barcode(pitr) && HepMC::barcode(child) < 100000) {
                  notSelfDecay = false;
                  break;
                }
              }
            }
            if (notSelfDecay) nParts++;
          }
    }
  }
   if (m_Exclusive)
     {setFilterPassed(nParts == m_MinParts);}
   else
     {setFilterPassed(nParts >= m_MinParts);}
  return StatusCode::SUCCESS;
}
