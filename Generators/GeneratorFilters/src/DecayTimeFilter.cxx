/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DecayTimeFilter.h"
#include <limits>       // std::numeric_limits


DecayTimeFilter::DecayTimeFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("LifetimeLow",m_lifetimeLow = std::numeric_limits<float>::lowest(), "proper decay time value in ps");
  declareProperty("LifetimeHigh",m_lifetimeHigh = std::numeric_limits<float>::max(), "proper decay time value in ps");
  declareProperty("PDGs",m_particleID);
}


StatusCode DecayTimeFilter::filterInitialize() {
  ATH_MSG_INFO("lifetimeLow=" << m_lifetimeLow);
  ATH_MSG_INFO("lifetimeHigh=" << m_lifetimeHigh);
  for(int pdg : m_particleID){
      ATH_MSG_INFO("PDG codes=" << pdg);
  }
  return StatusCode::SUCCESS;
}

double calcmag(const HepMC::FourVector& vect){
    return std::sqrt(vect.x() * vect.x() + vect.y() * vect.y() + vect.z() * vect.z());
}

double DecayTimeFilter::tau(HepMC::ConstGenParticlePtr ptr) const {
    auto startpos = ptr->production_vertex()->position ();
    auto endpos = ptr->end_vertex() ->position ();
    HepMC::FourVector diff(endpos.x() - startpos.x(), endpos.y() - startpos.y(), endpos.z() - startpos.z(), endpos.t() - startpos.t());
    double mag = calcmag(diff);
    double length = mag;
    HepMC::FourVector p = ptr->momentum ();
    return (1000./299.792458) * (length * ptr->generated_mass() / calcmag(p));
}

StatusCode DecayTimeFilter::filterEvent() {
  int nPassPDG = 0;
  bool passed = true;
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
     const HepMC::GenEvent* genEvt = *itr;
     for (auto part: *genEvt){
       for (int pdg : m_particleID){
           if(pdg == part->pdg_id()){
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
