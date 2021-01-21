/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#include "GeneratorFilters/MultiLeptonWithParentFilter.h"


MultiLeptonWithParentFilter::MultiLeptonWithParentFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("PDGOrigin",       m_PDGOrigin              );
  declareProperty("PDGIntermediate", m_PDGIntermediate        );
  declareProperty("MinPt",           m_minPt          = 5000. );
  declareProperty("MaxEta",          m_maxEta         = 10.0  );
  declareProperty("NLeptonsMin",     m_NLeptonsMin    = 2     );
  declareProperty("NLeptonsMax",     m_NLeptonsMax    = 4     );
  declareProperty("IncludeLepTaus",  m_incLepTau      = true  );
  declareProperty("IncludeHadTaus",  m_incHadTau      = true  );
  declareProperty("VetoHadTaus",     m_vetoHadTau     = false );
}


StatusCode MultiLeptonWithParentFilter::filterInitialize()
{
  // Treat negative values as undefined
  if (m_NLeptonsMin < 0) {
    m_NLeptonsMin = 0;
  }
  if (m_NLeptonsMax < 0) {
    m_NLeptonsMax = INT_MAX;
  }

  // Validate options
  if (m_PDGOrigin.empty()) {
    ATH_MSG_ERROR("PDGOrigin[] not set ");
    return StatusCode::FAILURE;
  }

  if (m_NLeptonsMax < m_NLeptonsMin) {
    ATH_MSG_ERROR("Max leptons should not be smaller than min leptons");
    return StatusCode::FAILURE;
  }

  // Include leptonic tau decays
  if (m_incLepTau || m_incHadTau) {
    if (std::find(m_PDGIntermediate.begin(), m_PDGIntermediate.end(), 15) == m_PDGIntermediate.end()) {
      m_PDGIntermediate.push_back(15);
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode MultiLeptonWithParentFilter::filterEvent()
{
  int numLeptons = 0;

  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;

    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      // Electrons and muons
      if ((*pitr)->status() == 1 && (abs((*pitr)->pdg_id()) == 11 || abs((*pitr)->pdg_id()) == 13)) {
        if ((*pitr)->momentum().perp() >= m_minPt && fabs((*pitr)->momentum().pseudoRapidity()) <= m_maxEta) {
          ATH_MSG_DEBUG("Found lepton with PDG ID = " << (*pitr)->pdg_id()
                        << ", status = " <<  (*pitr)->status()
                        << ", pt = "     <<  (*pitr)->momentum().perp()
                        << ", eta = "    <<  (*pitr)->momentum().pseudoRapidity());

          // Child
          if (hasValidParent(*pitr)) {
            ATH_MSG_DEBUG("Lepton accepted");
            numLeptons++;
          }
        }
        continue;
      }

      if (!m_incHadTau && !m_vetoHadTau)
        continue;

      // Hadronic taus
      if ((*pitr)->status() == 2 && abs((*pitr)->pdg_id()) == 15) {
        if ((*pitr)->momentum().perp() >= m_minPt && fabs((*pitr)->momentum().pseudoRapidity()) <= m_maxEta) {
          // Check if hadronic
          HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
          HepMC::GenVertex::particle_iterator thisChild = firstChild;

          bool leptonic = false;
          for (; thisChild != endChild; ++thisChild) {
            if (abs((*thisChild)->pdg_id()) == 11 || abs((*thisChild)->pdg_id()) == 13) {
              leptonic = true;
              break;
            }
          }

          if (leptonic)
            continue;

          ATH_MSG_DEBUG("Found lepton with PDG ID = " << (*pitr)->pdg_id()
                        << ", status = " <<  (*pitr)->status()
                        << ", pt = "     <<  (*pitr)->momentum().perp()
                        << ", eta = "    <<  (*pitr)->momentum().pseudoRapidity());

          // Child
          if (hasValidParent(*pitr)) {
            if (m_vetoHadTau) {
              ATH_MSG_DEBUG("Rejecting event due to presence of a hadronic tau.");
              setFilterPassed(false);
              return StatusCode::SUCCESS;
            }

            numLeptons++;
          }
        }
        continue;
      }
    }
  }

  ATH_MSG_DEBUG("Found " << numLeptons << "Leptons");
  setFilterPassed(numLeptons >= m_NLeptonsMin && numLeptons <= m_NLeptonsMax);

  return StatusCode::SUCCESS;
}


bool MultiLeptonWithParentFilter::hasValidParent(HepMC::GenParticle *particle)
{
  ATH_MSG_DEBUG("Validating parents for particle with PDG ID = " << particle->pdg_id());

  HepMC::GenVertex::particle_iterator firstParent = particle->production_vertex()->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = particle->production_vertex()->particles_end(HepMC::parents);
  HepMC::GenVertex::particle_iterator thisParent = firstParent;

  bool valid = false;
  for (; thisParent != endParent; ++thisParent) {
    ATH_MSG_DEBUG("Has parent with PDG ID = " << (*thisParent)->pdg_id());

    if (abs((*thisParent)->pdg_id()) == abs(particle->pdg_id())) {
      return hasValidParent(*thisParent);
    }

    for (size_t i = 0; i < m_PDGOrigin.size(); i++) {
      if (abs((*thisParent)->pdg_id()) == m_PDGOrigin[i]) {
        ATH_MSG_DEBUG("Found valid origin with PDG ID = " << (*thisParent)->pdg_id());
        return true;
      }
    }

    for (size_t i = 0; i < m_PDGIntermediate.size(); i++) {
      if (abs((*thisParent)->pdg_id()) == m_PDGIntermediate[i]) {
        valid = valid || hasValidParent(*thisParent);
      }
    }
  }

  return valid;
}
