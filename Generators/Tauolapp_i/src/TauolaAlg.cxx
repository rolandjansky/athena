/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Tauolapp_i/TauolaAlg.h"

// Tauola header files
#include "Tauola/Log.h"
#include "Tauola/Tauola.h"
#include "Tauola/TauolaHepMCEvent.h"
using namespace Tauolapp;


// Constructor
TauolaAlg::TauolaAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  //Key to HepMC record
  declareProperty("McEventKey", m_key="GEN_EVENT");

  //TAUOLA configurables
  //TAUOLA decay mode of particles with same/opposite charge as "decay_particle"
  declareProperty("decay_mode_same", m_decay_mode_same=1);
  declareProperty("decay_mode_opposite", m_decay_mode_opp=2);
  declareProperty("decay_particle",m_decay_particle=15);
}


StatusCode TauolaAlg::initialize(){

  // Get the Storegate collection
  /// @todo Can't thi be removed?
  evtStore().setName( "StoreGateSvc");
  StatusCode sc = evtStore().retrieve();
  if ( !sc.isSuccess() ) {
    ATH_MSG_ERROR ("Could not locate StoreGateSvc");
    return sc;
  }

  // Setup and intialise Tauola Interface
  Tauola::setSameParticleDecayMode(m_decay_mode_same);
  Tauola::setOppositeParticleDecayMode(m_decay_mode_opp);
  // etc.... see Tauola.h for the full list of configurables
  // Note: some need to be set before (or after) calling Tauola::initialise();

  // Tauola::setHiggsScalarPseudoscalarMixingAngle(atof(argv[5]));
  // Tauola::setHiggsScalarPseudoscalarPDG(25);

  Tauola::initialise();
  // Tauola::setEtaKpi(0,0,0); // switches to decay eta K0_S and pi0 1/0 on/off.
  // Tauola::spin_correlation.setAll(false);

  return StatusCode::SUCCESS;
}


StatusCode TauolaAlg::execute() {

  // Load HepMC info
  const McEventCollection* mcCollptr_const;
  if ( evtStore()->retrieve(mcCollptr_const, m_key).isFailure() ) {
    ATH_MSG_ERROR ("Could not retrieve McEventCollection");
    return StatusCode::FAILURE;
  }

  // Const_cast to make an event possible to update
  McEventCollection* mcCollptr =  const_cast<McEventCollection*>(mcCollptr_const);

  // Loop over all events in McEventCollection
  McEventCollection::iterator itr;
  for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {

    // Convert event record to format readable by tauola interface
    TauolaHepMCEvent * t_event = new TauolaHepMCEvent(*itr);

    // t_event->getEvent()->print();

    // remove tau decays first
    t_event->undecayTaus();
    // decay taus
    t_event->decayTaus();
  }
  return StatusCode::SUCCESS;
}
