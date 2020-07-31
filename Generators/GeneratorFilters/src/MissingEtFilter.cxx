/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MissingEtFilter.h"
#include "TruthUtils/HepMCHelpers.h"
#include "TruthUtils/PIDHelpers.h"


MissingEtFilter::MissingEtFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("METCut",m_METmin = 10000.);
  // Normally we'd include them, but this is unstable if using EvtGen
  declareProperty("UseNeutrinosFromHadrons",m_useHadronicNu = false);
}


StatusCode MissingEtFilter::filterEvent() {
  double sumx(0), sumy(0);
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto pitr: *genEvt) {
      if (!MC::isGenStable(pitr)) continue;
      if (!MC::isNonInteracting(pitr)) continue;
      if(!m_useHadronicNu && MC::PID::isNeutrino(pitr->pdg_id()) && !(fromWZ(pitr) || fromTau(pitr)) ) continue;
      ATH_MSG_VERBOSE("Found noninteracting particle: ID = " << pitr->pdg_id() << " PX = " << pitr->momentum().px() << " PY = "<< pitr->momentum().py());
      sumx += pitr->momentum().px();
      sumy += pitr->momentum().py();
    }
  }

  // Now see what the total missing Et is and compare to minimum
  double met = std::sqrt(sumx*sumx + sumy*sumy);
  ATH_MSG_DEBUG("Totals for event: EX = " << sumx << ", EY = "<< sumy << ", ET = " << met);
  setFilterPassed(met >= m_METmin);
  return StatusCode::SUCCESS;
}

bool MissingEtFilter::fromWZ( HepMC::ConstGenParticlePtr part ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function
  //  it's used in place of code in MCTruthClassifier as long as this package is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Hit a hadron -> return false
  // Hit a parton -> return true
  //   This catch is important - we *cannot* look explicitly for the W or Z, because some
  //    generators do not include the W or Z in the truth record (like Sherpa)
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!part->production_vertex()) return false;
#ifdef HEPMC3
  for (auto iter: part->production_vertex()->particles_in()){
    int parent_pdgid = iter->pdg_id();
    if (MC::PID::isW(parent_pdgid) || MC::PID::isZ(parent_pdgid)) return true;
    if (MC::PID::isHadron( parent_pdgid ) ) return false;
    if ( std::abs( parent_pdgid ) < 9 ) return true;
    if ( parent_pdgid == part->pdg_id() ) return fromWZ( iter );
  }
#else
  for (HepMC::GenVertex::particles_in_const_iterator iter=part->production_vertex()->particles_in_const_begin(); 
       iter!=part->production_vertex()->particles_in_const_end();++iter){
    int parent_pdgid = (*iter)->pdg_id();
    if (MC::PID::isW(parent_pdgid) || MC::PID::isZ(parent_pdgid)) return true;
    if (MC::PID::isHadron( parent_pdgid ) ) return false;
    if ( std::abs( parent_pdgid ) < 9 ) return true;
    if ( parent_pdgid == part->pdg_id() ) return fromWZ( *iter );
  }
#endif  
  return false;
}

bool MissingEtFilter::fromTau( HepMC::ConstGenParticlePtr part ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function
  //  it's used in place of code in MCTruthClassifier as long as this package is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Find a tau -> return true
  // Find a hadron or parton -> return false
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!part->production_vertex()) return false;
#ifdef HEPMC3
  for (auto iter: part->production_vertex()->particles_in()){
    int parent_pdgid = iter->pdg_id();
    if ( std::abs( parent_pdgid ) == 15  && fromWZ(iter)) return true;
    if (MC::PID::isHadron( parent_pdgid ) || std::abs( parent_pdgid ) < 9 ) return false;
    if ( parent_pdgid == part->pdg_id() ) return fromTau( iter );
  }
#else
  for (HepMC::GenVertex::particles_in_const_iterator iter=part->production_vertex()->particles_in_const_begin(); 
       iter!=part->production_vertex()->particles_in_const_end();++iter){
    int parent_pdgid = (*iter)->pdg_id();
    if ( std::abs( parent_pdgid ) == 15  && fromWZ(*iter)) return true;
    if (MC::PID::isHadron( parent_pdgid ) || std::abs( parent_pdgid ) < 9 ) return false;
    if ( parent_pdgid == part->pdg_id() ) return fromTau( *iter );
  }
#endif
  return false;
}
