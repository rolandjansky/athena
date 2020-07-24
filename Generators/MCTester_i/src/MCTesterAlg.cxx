/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorObjects/McEventCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/ISvcLocator.h"
 
#include "StoreGate/StoreGateSvc.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
 
#include "GaudiKernel/IPartPropSvc.h"
#include "MCTester_i/MCTesterAlg.h"
#include "GaudiKernel/MsgStream.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

/////////////////////////////////////////////////////////////////////////////
MCTesterAlg::MCTesterAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  //Key to HepMC record
  declareProperty("McEventKey", m_key="GEN_EVENT");
  declareProperty("McEventInfoKey", m_infokey="");

  //MC-TESTER SETUP configureables
  declareProperty("decay_particle", m_decay_particle=15);
  declareProperty("result_path", m_result_path="./mc-tester.root");
  declareProperty("nbins", m_nbins=100);
  declareProperty("bin_min", m_bin_min=0.0);
  declareProperty("bin_max", m_bin_max=2000.0);
  declareProperty("suppress_decay", m_suppress_decay="111");
  declareProperty("include_decays_to_self", m_include_decays_to_self=true);

  //New from MC-Tester 1.23
  declareProperty("mass_power",m_mass_power=1);
  declareProperty("mass_scale_on",m_mass_scale_on=false);

  //descrption of generation that goes on booklet title page
  declareProperty("generation_description_line1",m_description_l1="");
  declareProperty("generation_description_line2",m_description_l2="");
  declareProperty("generation_description_line3",m_description_l3="");

  //Others (for 4-momentum conservation check)
  declareProperty("collision_px", m_collision_px=0.0);
  declareProperty("collision_py", m_collision_py=0.0);
  declareProperty("collision_pz", m_collision_pz=0.0);
  declareProperty("collision_E", m_collision_E=14000000.0);
  declareProperty("collision_check_sum_margin", m_collision_check_sum_margin=1);

  m_decay_name = 0;
  
  m_events_total = 0;
  m_events_without_particle = 0;
  m_particles_of_interest = 0;
  m_non_decaying_particle = 0;
  m_deviation_from_centre_of_mass = 0;
  m_zero_lifetime_particles = 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode MCTesterAlg::initialize()
{
  ATH_MSG_DEBUG( "initialize()" );

  //Setup and intiatlise MC-Tester
  Setup::decay_particle=m_decay_particle;
  Setup::result1_path = const_cast<char*>(m_result_path.c_str());
  Setup::SetHistogramDefaults(m_nbins, m_bin_min, m_bin_max);

  Setup::mass_power = m_mass_power;
  Setup::mass_scale_on = m_mass_scale_on;

  //tokenise string of particle pdg_id to handle as stable
  char * temp_str = const_cast<char*>(m_suppress_decay.c_str());
  char * token_str = strtok (temp_str," ,");
  while (token_str != NULL) {
    Setup::SuppressDecay(atoi(token_str));
    token_str = strtok(NULL, " ,");
  }
  Setup::SuppressDecay(m_decay_particle);

  //replace all "_"'s with "\_" which latex handles correctly
  for(unsigned int i=m_description_l1.find("_",0); i < m_description_l1.size(); i = m_description_l1.find("_", i+2)){
    m_description_l1.insert(i,"\\");
  }
  for(unsigned int i=m_description_l2.find("_",0); i < m_description_l2.size(); i = m_description_l2.find("_", i+2)){
    m_description_l2.insert(i,"\\");
  }
  for(unsigned int i=m_description_l3.find("_",0); i < m_description_l3.size(); i = m_description_l3.find("_", i+2)){
    m_description_l3.insert(i,"\\");
  }
  Setup::gen1_desc_1 = const_cast<char*>(m_description_l1.c_str());
  Setup::gen1_desc_2 = const_cast<char*>(m_description_l2.c_str());
  Setup::gen1_desc_3 = const_cast<char*>(m_description_l3.c_str());

  MC_Initialize();

  //initialise extra variables
  m_events_total = 0;
  m_events_without_particle = 0;
  m_particles_of_interest = 0;
  m_non_decaying_particle = 0;
  m_zero_lifetime_particles = 0;
  m_deviation_from_centre_of_mass = 0;
  m_decay_name = HEPParticle::GetParticleName(m_decay_particle);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode MCTesterAlg::execute() 
{
  ATH_MSG_DEBUG( "execute()" );

  int run_number,evt_number;
  
  //Load event info
  const EventInfo * mcInfoptr;
  if ( evtStore()->retrieve(mcInfoptr, m_infokey).isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve EventInfo" );
    return StatusCode::FAILURE;
  } else{
    run_number=mcInfoptr->event_ID()->run_number();
    evt_number=mcInfoptr->event_ID()->event_number();

    ATH_MSG_DEBUG( "run: " << run_number << " event: " << evt_number );
  }

  //Load HepMC info
  const McEventCollection* mcCollptr;
  if ( evtStore()->retrieve(mcCollptr, m_key).isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve McEventCollection" );
    return StatusCode::FAILURE;
  } else {
    // Loop over all events in McEventCollection
    McEventCollection::const_iterator itr;

    for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
      // const_cast needed because HepMCEvent ctor below takes
      // a non-const reference (and we probably don't want to copy it).
      HepMC::GenEvent* evt=const_cast<HepMC::GenEvent*>(*itr);

      //make HepMCEvent
      HepMCEvent * temp_event = new HepMCEvent(*evt,m_include_decays_to_self);

      //Fix event number as it's not set in GenEvent
      temp_event->SetEventNumber(evt_number);

      ATH_CHECK(MC_Validate_checkDecay(temp_event));
      ATH_CHECK(MC_Validate_check4MomentumSum(temp_event));

      MC_Analyze(temp_event);
      delete temp_event;
    }    
  }  

  return StatusCode::SUCCESS;  
}
  
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode MCTesterAlg::finalize() 
{
  ATH_MSG_DEBUG( "finalize()" );

  ATH_MSG_INFO(m_events_total << " events processed for validation");
  ATH_MSG_INFO(m_events_without_particle << " events without a " << m_decay_name);
  ATH_MSG_INFO(m_particles_of_interest << " " << m_decay_name<< "s found in total");
  ATH_MSG_INFO(m_non_decaying_particle << " " << m_decay_name<< "s do not decay");
  ATH_MSG_INFO(m_zero_lifetime_particles << " " << m_decay_name<< "s with zero lifetime");
  ATH_MSG_INFO(m_deviation_from_centre_of_mass << " events failing sum of 4 momentum check " );

  MC_Finalize();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode MCTesterAlg::MC_Validate_checkDecay(HepMCEvent * e)
{
  m_events_total++;
  
  //Find particles of interest
  HEPParticleList plist;
  e->FindParticle(m_decay_particle,&plist);
  if (plist.empty()){
    m_events_without_particle++;
    return StatusCode::SUCCESS;
  }
  
  //Find daughters of particle
  HEPParticleListIterator particles(plist);

  for(HEPParticle *part=particles.first(); part!=0; part=particles.next() ) {
    // exclude the ones whos status is wrong.
    if(!part->IsHistoryEntry()){
      m_particles_of_interest++;

      //Check the lifetime
      if (part->GetTau() == 0) {
	      m_zero_lifetime_particles++;	
	      ATH_MSG_WARNING(m_decay_name << " lifetime is " << part->GetTau() << " in event " << 
                        e->GetEventNumber());
      }
      else
	      ATH_MSG_DEBUG(m_decay_name << " lifetime is " << part->GetTau());

        ATH_MSG_DEBUG(m_decay_name << "production vertex x,y,z,t: " << 
                      part->GetVx() << ", " << 
                      part->GetVy() << ", " << 
                      part->GetVz() << ", " << 
                      part->GetTau());

      //Check it has children (ie decays)
      HEPParticleList daughterlist;
      part->GetDaughterList(&daughterlist);
      if(daughterlist.empty()) {
	      m_non_decaying_particle++;
      	ATH_MSG_WARNING(m_decay_name << " does not decaying in event: " << e->GetEventNumber());
      }
    }
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode MCTesterAlg::MC_Validate_check4MomentumSum(HepMCEvent * e)
{
  std::vector<double> * sum = e->Sum4Momentum();

  if(std::abs(sum->at(0)-m_collision_px)>m_collision_check_sum_margin || 
     std::abs(sum->at(1)-m_collision_py)>m_collision_check_sum_margin ||
     std::abs(sum->at(2)-m_collision_pz)>m_collision_check_sum_margin ||
     std::abs(sum->at(3)-m_collision_E)>m_collision_check_sum_margin) {

    m_deviation_from_centre_of_mass++;
    ATH_MSG_WARNING("Event " << e->GetEventNumber() << 
                    " failed sum of 4 momentum check: ");
    ATH_MSG_WARNING("Final: (" << sum->at(0) << "," << sum->at(1) << 
                    "," << sum->at(2) << "," << sum->at(3)<<")");
    ATH_MSG_WARNING("Initial: (" << m_collision_px << "," << m_collision_py << 
                    "," << m_collision_pz <<","<< m_collision_E << ")");
  } else {
    ATH_MSG_DEBUG("Event " << e->GetEventNumber() << 
                  " passed sum of 4 momentum check: ");
    ATH_MSG_DEBUG("Final: (" << sum->at(0) << "," << sum->at(1) << 
                  "," << sum->at(2) << "," << sum->at(3)<<")");
    ATH_MSG_DEBUG("Initial: (" << m_collision_px << "," << m_collision_py << 
                  "," << m_collision_pz <<","<< m_collision_E << ")");
  }
  
  delete sum;

  return StatusCode::SUCCESS;
}

