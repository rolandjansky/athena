/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "Tauolapp_i/TauolaPP.h"

// Tauola header files
#include "Tauola/Log.h"
#include "Tauola/Tauola.h"
#include "Tauola/TauolaHepMCEvent.h"

#include "Tauola/f_Variables.h"

// for proper seeding
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
//for Ranlux
#include "CLHEP/Random/Randomize.h"

using namespace Tauolapp;
using namespace CLHEP;

RanluxEngine theRanluxEngine(123456,1);

double RanluxGenerator()
{
//  std::cout<<" called Ranlux "<<std::endl;
  return RandFlat::shoot(&theRanluxEngine);
}


//Random number service
IAtRndmGenSvc* &TauolaPP::atRndmGenSvc()
{
  static IAtRndmGenSvc *p_AtRndmGenSvc = 0;
  return p_AtRndmGenSvc;
}

std::string &TauolaPP::tauolapp_stream()
{

  static std::string s_tauolapp_stream = "TAUOLAPP_INIT";
  return s_tauolapp_stream;
}


// Constructor
TauolaPP::TauolaPP(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  //Key to HepMC record
  declareProperty("McEventKey", m_key="GEN_EVENT");

  //TAUOLA configurables
  //TAUOLA decay mode of particles with same/opposite charge as "decay_particle"
  declareProperty("decay_mode_same", m_decay_mode_same=1);
  declareProperty("decay_mode_opposite", m_decay_mode_opp=2);
  declareProperty("decay_particle",m_decay_particle=15);
  declareProperty("tau_mass",m_tau_mass=1.77684);
  declareProperty("spin_correlation",m_spin_correlation=true);
  declareProperty("setRadiation",m_setRadiation=true);
  declareProperty("setRadiationCutOff",m_setRadiationCutOff=0.01); 
}


StatusCode TauolaPP::initialize(){

  // Get the Storegate collection
  /// @todo Can't thi be removed?
  evtStore().setName( "StoreGateSvc");
  StatusCode sc = evtStore().retrieve();
  if ( !sc.isSuccess() ) {
    ATH_MSG_ERROR ("Could not locate StoreGateSvc");
    return sc;
  }



  // Random number service
  StatusCode RndmStatus = service("AtRndmGenSvc", atRndmGenSvc(), true);
	 
  if(!RndmStatus.isSuccess() || atRndmGenSvc() == 0)
  {
     ATH_MSG_ERROR("Could not get Random number service!");
     return StatusCode::FAILURE;
  }

	 
  HepRandomEngine* engine = atRndmGenSvc()->GetEngine(tauolapp_stream());
  const long*   sip     =       engine->getSeeds();
  long  int     si1     =       sip[0];
  long  int     si2     =       sip[1];
	 

  atRndmGenSvc()->CreateStream(si1, si2, tauolapp_stream());
  tauolapp_stream() = "TAUOLAPP";



  // Setup and intialise Tauola Interface
  Tauola::setSameParticleDecayMode(m_decay_mode_same);
  Tauola::setOppositeParticleDecayMode(m_decay_mode_opp);
  // etc.... see Tauola.h for the full list of configurables
  // Note: some need to be set before (or after) calling Tauola::initialize();

  // Tauola::setHiggsScalarPseudoscalarMixingAngle(atof(argv[5]));
  // Tauola::setHiggsScalarPseudoscalarPDG(25);

  Tauola::initialize();
 
  Tauola::setEtaK0sPi(1,0,1); // switches to decay eta K0_S and pi0 1/0 on/off.
  Tauola::spin_correlation.setAll(m_spin_correlation);
  Tauola::setRadiation(m_setRadiation);
  Tauola::setRadiationCutOff(m_setRadiationCutOff);

  //call RanLux generator for ++ part of Tauola
  Tauola::setRandomGenerator(RanluxGenerator);

  //seeding tauola-fortran generator
  Tauola::setSeed((int) si1,0,0);

  //seeding tauola++ generator
  theRanluxEngine.setSeed(si2,1);

  //setting tau mass
  parmas_.amtau=m_tau_mass;

  //cout<<"tauola tau mass "<<Tauola::getTauMass()<<endl;

  return StatusCode::SUCCESS;
}


StatusCode TauolaPP::execute() {

  // Load HepMC info
  const McEventCollection* mcCollptr_const;
  if ( evtStore()->retrieve(mcCollptr_const, m_key).isFailure() ) {
    ATH_MSG_ERROR ("Could not retrieve McEventCollection");
    return StatusCode::FAILURE;
  }
	 
  HepRandomEngine* engine = atRndmGenSvc()->GetEngine(tauolapp_stream());
  const long*   sip     =       engine->getSeeds();
  long  int     si1     =       sip[0];
  long  int     si2     =       sip[1];
	 

  //seeding tauola-fortran generator
  Tauola::setSeed((int) si1,0,0);

  //seeding tauola++ generator
  theRanluxEngine.setSeed(si2,1);


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
