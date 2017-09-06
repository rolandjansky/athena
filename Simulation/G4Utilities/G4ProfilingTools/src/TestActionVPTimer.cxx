/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//								//
//		    TestActionVPTimer				//
//	Code for text output (into the athena.out files)	//
//	of information about the time spent simulating		//
//	various pieces of the detector and particles.		//
//								//
//	Written by      Kevin Sapp				//
//	                University of Pittsburgh		//
//	                kcs34@pitt.edu				//
//	Last update	10.13.09				//
//								//
//////////////////////////////////////////////////////////////////


#include "TestActionVPTimer.h"

#include <G4Track.hh>
#include <G4Electron.hh>
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4TouchableHistory.hh>
#include <G4ParticleDefinition.hh>

#include "boost/io/ios_state.hpp"
#include <algorithm>
#include <iomanip>
#include <ios>

using std::map;		using std::max;		using std::string;
using std::setw;	using std::ios;		using std::make_pair;
using std::pair;	using std::find;	using std::vector;

// #define _myDebug

namespace G4UA{


  TestActionVPTimer::TestActionVPTimer(const Config& config):m_config(config),m_report(),
							     m_runTimer(0), m_eventTimer(0),
							     m_eventTime(0.){

    // create event & run timers
    m_runTimer = new G4Timer();
    m_runTimer->Start();
    m_runTimer->Stop();
    m_eventTimer = new G4Timer();
    m_eventTimer->Start();
    m_eventTimer->Stop();
    
    // create step timer
    v_timer = new G4Timer();
    v_timer->Start();
    v_timer->Stop();

  }

  void TestActionVPTimer::BeginOfEventAction(const G4Event*){
    m_report.nev++;
    m_eventTimer->Start();
  }
  
  void TestActionVPTimer::EndOfEventAction(const G4Event*){
    // this function also stops the timer. it will be restarted at BoE
    m_eventTime += TimerSum(m_eventTimer);

  }
  
  void TestActionVPTimer::BeginOfRunAction(const G4Run*){;
    m_runTimer->Start();
  }
  
  void TestActionVPTimer::EndOfRunAction(const G4Run*){
    // this also stops the timer

    m_report.runTime += TimerSum(m_runTimer);

  }
  
  void TestActionVPTimer::UserSteppingAction(const G4Step* aStep){

    // HERE IS WHERE WE BEGIN OUR CLOCKING -- ONLY IF 
    // TIMERS ARE NOT VALID
    
    if (!v_timer->IsValid()) {
      
      // Collect the total time before processing anything else
      // this stops the timer
      double vtime = TimerSum(v_timer);
      
      // CHECKIN' OUT THA NAVIGATA
      VolumeTreeNavigator currentTree( aStep );
      
      // Set depth cuts here
      currentTree.SetDepthCutSimple(m_config.dCALO, m_config.dBeam, m_config.dIDET, m_config.dMUON);
      if ( !m_config.dDetail.empty() ) {
        currentTree.SetDepthCutDetail( m_config.dDetail.c_str() );
      }
      
      // Store time generated in current volume, remove deepest entry in v_history, then
      // repeat for the resulting VolTree (one level higher)
      G4ParticleDefinition* PDef = currentTree.GetTrack()->GetDefinition();
      while ( true ) {
        VolTree VHistory = currentTree.Extract();
        m_report.time_index[VHistory].tTotal += vtime;
	//std::cout<<"filling "<<m_report.time_index[VHistory].tTotal<<std::endl;
        if (PDef->GetParticleName() == "neutron")	{ m_report.time_index[VHistory].tNeutron += vtime; }
        else if (PDef->GetParticleSubType() == "e")	{ m_report.time_index[VHistory].tElectron += vtime; }
        else if (PDef->GetParticleSubType() == "pi")	{ m_report.time_index[VHistory].tPion += vtime; }
        else if (PDef->GetParticleType() == "gamma")	{ m_report.time_index[VHistory].tPhoton += vtime; }
        else if (PDef->GetParticleType() == "baryon")	{ m_report.time_index[VHistory].tBaryon += vtime; }
        else if (PDef->GetParticleType() == "lepton")	{ m_report.time_index[VHistory].tLepton += vtime; }
        else if (PDef->GetParticleType() == "meson")	{ m_report.time_index[VHistory].tMeson += vtime; }
        else						{ m_report.time_index[VHistory].tOther += vtime; }
        //ATH_MSG_DEBUG("Time stored in "<<VHistory.back().first->GetName());
        if ( !currentTree.Ascend() )  break;
      }
    }
    
    // Restart timer
    v_timer->Start();
    
    return;

  }

  
} // namespace G4UA 
