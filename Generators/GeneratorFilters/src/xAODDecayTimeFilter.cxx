/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODDecayTimeFilter.h"
#include <limits>       // std::numeric_limits


xAODDecayTimeFilter::xAODDecayTimeFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("LifetimeLow",m_lifetimeLow = std::numeric_limits<float>::lowest(), "proper decay time value in ps");
  declareProperty("LifetimeHigh",m_lifetimeHigh = std::numeric_limits<float>::max(), "proper decay time value in ps");
  declareProperty("PDGs",m_particleID);
}


StatusCode xAODDecayTimeFilter::filterInitialize() {
  ATH_MSG_INFO("lifetimeLow=" << m_lifetimeLow);
  ATH_MSG_INFO("lifetimeHigh=" << m_lifetimeHigh);
  for(int pdg : m_particleID){
      ATH_MSG_INFO("PDG codes=" << pdg);
  }
  return StatusCode::SUCCESS;
}

static double calcmag(const HepMC::FourVector& vect){
    return std::sqrt(vect.x() * vect.x() + vect.y() * vect.y() + vect.z() * vect.z());
}

double xAODDecayTimeFilter::tau(const xAOD::TruthParticle* ptr) const {
    auto startpos = ptr->prodVtx();
    auto endpos = ptr->decayVtx();
    HepMC::FourVector diff(endpos->x() - startpos->x(), endpos->y() - startpos->y(), endpos->z() - startpos->z(), endpos->t() - startpos->t());
    double mag = calcmag(diff);
    double length = mag;
    HepMC::FourVector p(ptr->px(),ptr->py(),ptr->pz(),ptr->e());// = ptr->momentum ();
    return (1000./299.792458) * (length * ptr->m() / calcmag(p));
}

StatusCode xAODDecayTimeFilter::filterEvent() {

      // Retrieve full TruthEventContainer container
    const xAOD::TruthEventContainer *xTruthEventContainer=NULL;
    if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name " << "TruthEvents" << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

  int nPassPDG = 0;
  bool passed = true;
  for ( xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr) {
       unsigned int nPart = (*itr)->nTruthParticles();
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
            const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);
            for (int pdg : m_particleID){
                if(pdg == part->pdgId()){
                    nPassPDG++;
                    double calctau = tau(part);
                    passed &= calctau < m_lifetimeHigh && calctau > m_lifetimeLow;
                }
            }
        }
  }
  
  setFilterPassed((nPassPDG > 0) & passed);
  return StatusCode::SUCCESS;
}

