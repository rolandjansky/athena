/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GeneratorFilters/FourTauLepLepHadHadFilter.h"
#include "GaudiKernel/AlgFactory.h"

FourTauLepLepHadHadFilter::FourTauLepLepHadHadFilter( const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter( name,pSvcLocator )
{
}


StatusCode FourTauLepLepHadHadFilter::filterInitialize() {
  return StatusCode::SUCCESS;
}


StatusCode FourTauLepLepHadHadFilter::filterFinalize() {
  return StatusCode::SUCCESS;
}


StatusCode FourTauLepLepHadHadFilter::filterEvent() {
  int nTau = 0;
  int nLepTau = 0;
  int nHadTau = 0;
  bool isOK = false;

  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    //Loop over all particles in event
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      //Find taus with not status code 1 = "stable final state"
      if (abs((*pitr)->pdg_id()) == 15 && (*pitr)->status() != 1) {
        HepMC::GenVertex::particle_iterator mother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);  
        
        //Check that one of the tau's parents is the 'a' = 36
        bool isTauParentOK = false;
        for (; mother != endMother; ++mother) {
          if ( (*mother)->pdg_id() == 36 ) isTauParentOK = true;        
        }
        if (!isTauParentOK) continue;

        nTau+=1;
        
        HepMC::GenVertex::particle_iterator child = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        HepMC::GenParticle *tempTau; 
        tempTau = (*pitr);

        //Check throuth the tau's children to see how it decays
        int tauType = 0; // Default, tau decays hadronically
        do {
          tauType = 0;
          for (; child != endChild; ++child) {
            if ( (*child)->production_vertex() != tempTau->end_vertex() ) continue;
            else if ( abs( (*child)->pdg_id() ) == 12 ) tauType = 1; //Tau decays to electron
            else if ( abs( (*child)->pdg_id() ) == 14 ) tauType = 2; //Tau decays to muon
            else if ( abs( (*child)->pdg_id() ) == 15 ) {            //Tau decays to another tau
              tauType = 3; 
              tempTau = (*child);
            }
          }
          if (tauType == 3 ) {
            ATH_MSG_INFO("TAU DECAYED TO ANOTHER TAU");          
            child = tempTau->end_vertex()->particles_begin(HepMC::children);
            endChild = tempTau->end_vertex()->particles_end(HepMC::children);
          }
          ATH_MSG_INFO("Tau Type = " << tauType);
        } while (tauType == 3);

        if (tauType == 0) nHadTau+=1;
        else if ((tauType == 1) || (tauType == 2)) nLepTau+=1;
      }
    }
  }
  
  ATH_MSG_INFO(" nTau    = " << nTau);
  ATH_MSG_INFO(" nLepTau = " << nLepTau);
  ATH_MSG_INFO(" nHadTau = " << nHadTau);
 
  if (nLepTau == 2 && nHadTau == 2) isOK = true;

  setFilterPassed(isOK);
  return StatusCode::SUCCESS;
}
