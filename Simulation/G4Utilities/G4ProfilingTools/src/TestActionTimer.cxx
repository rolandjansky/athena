/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//                                                              //
//                       TestActionTimer                        //
//       Code for text output (into the athena.out files)       //
//       of information about the time spent simulating         //
//       various pieces of the detector and particles.          //
//                                                              //
//       Written by      Zachary Marshall                       //
//                       Caltech                                //
//                       zmarshal@caltech.edu                   //
//                                                              //
//       Last update    08.07.08                                //
//                                                              //
//////////////////////////////////////////////////////////////////


#include "TestActionTimer.h"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Step.hh"

#include "G4String.hh"
#include "G4Timer.hh"
#include "G4Track.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4Neutron.hh"

// For file output
#include "GaudiKernel/ITHistSvc.h"
#include "TH1D.h"

// #define _myDebug


namespace G4UA{


  TestActionTimer::TestActionTimer():m_report(),
				     m_runTimer(0),
				     m_eventTimer(0),
				     //m_runTime(0.),
				     m_eventTime(0.)//,
				     //m_histSvc("THistSvc",name)
  {
    m_report.timeName.resize(eMax);
    m_report.timeName[eEMB]   = "EMB";
    m_report.timeName[eEMEC]  = "EMEC";
    m_report.timeName[eFC1]   = "FC1";
    m_report.timeName[eFC23]  = "FC23";
    m_report.timeName[eFCO]   = "FCO";
    m_report.timeName[eHEC]   = "HEC";
    m_report.timeName[eCry]   = "Cry";
    m_report.timeName[eLAr]   = "LAr";
    m_report.timeName[eHCB]   = "HCB";
    m_report.timeName[ePre]   = "Pre";
    m_report.timeName[eMu]    = "Mu";
    m_report.timeName[ePx]    = "Px";
    m_report.timeName[eTrt]    = "TRT";
    m_report.timeName[eSev]   = "IDServ";
    m_report.timeName[eSct]   = "SCT";
    m_report.timeName[eOther] = "Other";
      
    m_report.timeName[eElec]  = "Elec";
    m_report.timeName[ePos]   = "Pos";
    m_report.timeName[eGam]   = "Gam";
    m_report.timeName[eNeut]  = "Neut";
    
    G4cout<< "TestActionTimer::Constructor: Labels "
	  <<" Run Event ";
    for (int i(0); i < eMax; ++i) G4cout << m_report.timeName[i] << " ";
    G4cout << "Particle Dead" << G4endl;
    
    // init timers
    
    m_runTimer = new G4Timer();
    m_runTimer->Start();
    
    // create all timers, start and stop, init counters
    for (int i(0); i < eMax; ++i) {
      G4Timer* timer = new G4Timer();
      timer->Start();
      
      m_timer.push_back(timer);
      m_report.time.push_back(0.);
    }
    
    PPanic();
    VPanic();
    
#ifdef _myDebug
    G4cout << "TestActionTimer::Constructor done" << G4endl;
#endif
    
  }

  void TestActionTimer::BeginOfEventAction(const G4Event*){

    
    m_report.nev++;
    
    if (m_eventTimer != 0) {
      delete m_eventTimer;
      m_eventTimer = 0;
    }
    m_eventTime = 0.;
    m_eventTimer = new G4Timer();
    m_eventTimer->Start();
    
    m_runTimer->Start();
    return;
  }
  
  void TestActionTimer::EndOfEventAction(const G4Event*){
 
    m_report.runTime += TimerSum(m_runTimer);
    m_eventTime = TimerSum(m_eventTimer);
    
    VPanic();
    PPanic();
    
    return;
    
  }
  
  void TestActionTimer::BeginOfRunAction(const G4Run*){

    
    m_report.runTime=0.;
    m_runTimer->Start();
    
    return;
  }
  
  void TestActionTimer::EndOfRunAction(const G4Run*){
    std::cerr<<"TestActionTimer::EndOfRunAction "<< m_report.runTime <<std::endl;
    m_report.runTime += TimerSum(m_runTimer);
    VPanic();
    PPanic();
  }
  
  void TestActionTimer::UserSteppingAction(const G4Step* aStep){
    
    // HERE IS WHERE WE BEGIN OUR CLOCKING
    
    // Get basic information about the event
    G4Track* track = aStep->GetTrack();
    
    G4String thePrePVname = track->GetVolume()->GetName();
    G4String thePostPVname;
    if (track->GetNextVolume() != 0) {
      thePostPVname = track->GetNextVolume()->GetName();
    } else {
      thePostPVname = "OutOfWorld";
    }
    
    // Check what volume we are in.  If the volume does not agree with the clock, kill the clock
    G4Timer* timer = 0;
    int preIndex = ClassifyVolume( thePrePVname );
    int postIndex=-1;
    if (thePrePVname != thePostPVname){
      timer = m_timer[preIndex];
      if (!timer->IsValid()){ m_report.time[preIndex] += TimerSum(timer); } else { VPanic(); }
      
      // Now start the appropriate clock
      postIndex = ClassifyVolume( thePostPVname );
      m_timer[postIndex]->Start();
      
      // Otherwise it's going into "out of world"
    } else {
      timer = m_timer[preIndex];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    
    // Now for the particle based timers
    if (track->GetDefinition() == G4Electron::ElectronDefinition() ){
      if ( m_timer[eElec]->IsValid() ){
	m_timer[eElec]->Start();
      }
    } else if (!m_timer[eElec]->IsValid()){
      m_report.time[eElec] += TimerSum(m_timer[eElec]);
    }
    if (track->GetDefinition() == G4Positron::PositronDefinition() ){
      if ( m_timer[ePos]->IsValid() ){
	m_timer[ePos]->Start();
      }
    } else if (!m_timer[ePos]->IsValid()){
      m_report.time[ePos] += TimerSum(m_timer[ePos]);
    }
    if (track->GetDefinition() == G4Gamma::GammaDefinition() &&
	m_timer[eGam]->IsValid()){
      m_timer[eGam]->Start();
    } else if ( !m_timer[eGam]->IsValid() ){
      m_report.time[eGam] += TimerSum(m_timer[eGam]);
    }
    if (track->GetDefinition() == G4Neutron::NeutronDefinition() &&
	m_timer[eNeut]->IsValid()){
      m_timer[eNeut]->Start();
    } else if ( !m_timer[eNeut]->IsValid() ){
      m_report.time[eNeut] += TimerSum(m_timer[eNeut]);
    }
    
    return;
    
    
    
  }
  
  
  double TestActionTimer::TimerSum(G4Timer* timer) const
  {
    if (timer == 0) return -999.;
    timer->Stop();
    return (timer->GetUserElapsed() + timer->GetSystemElapsed());
  }
  
  void TestActionTimer::PPanic()
  {
#ifdef _myDebug
    G4cout << "TestActionTimer::PPanic" << G4endl;
#endif
    
    // stop all particle counters and add time
    for (int i(eElec); i < eMax; ++i) {
      G4Timer* timer = m_timer[i];
#ifdef _myDebug
      G4cout << "TestActionTimer::PPanic stopping counter i:" << i << " " << timer << G4endl;
#endif
      if (!timer->IsValid()){
	m_report.time[i] += TimerSum(timer);
      }
    }
    
#ifdef _myDebug
    G4cout << "TestActionTimer::PPanic done" << G4endl;
#endif
    
    return;
  }
  
  void TestActionTimer::VPanic()
  {
#ifdef _myDebug
    G4cout << "TestActionTimer::VPanic" << G4endl;
#endif
    
    // stop all volume counters and add time
    for (int i(0); i <= eOther; ++i) {
      G4Timer* timer = m_timer[i];
#ifdef _myDebug
      G4cout << "TestActionTimer::VPanic stopping counter i:" << i  << " " << timer << G4endl;
#endif
      if (!timer->IsValid()){
	m_report.time[i] += TimerSum(timer);
      }
    }
    
#ifdef _myDebug
    G4cout << "TestActionTimer::VPanic done" << G4endl;
#endif
    
    return;
  }
  
  int TestActionTimer::ClassifyVolume( G4String& nom ) const {
    
    if( nom.length() >= 17 &&
	nom(13,4) == "EMEC" ){
      return eEMEC;
    }
    else if ( nom.length() >= 16 &&
	      nom(13,3) == "EMB" ){
      return eEMB;
    }
    else if( nom.length() >= 25 &&
	     nom(21,4) == "Cryo" ) {
      return eCry;
    }
    else if( nom.length() >= 26 &&
	     nom(13,13) == "FCAL::Module1"){
      return eFC1;
    }
    else if( nom.length() >= 25 &&
	     nom(13,12) == "FCAL::Module" ){
      return eFC23;
    }
    else if ( nom.length() >= 17 &&
	      nom(13,4) == "FCAL" ){
      return eFCO;
    }
    else if ( nom.length() >= 16 &&
	      nom(13,3) == "HEC" ){
      return eHEC;
    }
    else if( nom.length() >= 31 &&
	     nom(21,10) == "Presampler" ) {
      return ePre;
    }
    else if ( nom.length() >= 3 &&
	      nom(0,3) == "LAr" ){
      return eLAr;
    }
    else if( ( (nom.length() >= 4 &&
		nom(0,4) == "Muon") ||
	       nom(0,4) == "MUON" ) ||
	     ( nom.length() >= 9 &&
	       nom(0,9) == "DriftTube" ) ||
	     nom.contains("MDT") ||
	     ( nom.length() >= 12 &&
	       nom(0,12) == "SensitiveGas" ) ||
	     nom.contains("MDT") ||
	     nom.contains("station") ){
      return eMu;
    }
    else if ((nom.length() >= 5 &&
	      nom(0,5) == "Pixel") ||
	     nom == "Outside Barrel Service"){
      return ePx;
    }
    else if ( nom.length() >= 3 &&
	      nom(0,3) == "SCT" ){
      return eSct;
    }
    else if ( ( nom.length() >= 3 &&
		nom(0,3) == "TRT" ) ||
	      nom == "GasMANeg" ){
      return eTrt;
    }
    else if ( nom.length() >= 4 &&
	      nom(0,4) == "Tile"){
      return eHCB;
    }
    else if ( ( nom.length() >= 12 &&
		nom(0,12) == "InDetServMat" ) ||
	      ( nom.length() >= 4 &&
		nom(0,4) == "IDET" ) ||
	      ( nom.length() >= 8 &&
		nom(0,8) == "BeamPipe" ) ||
	      ( nom.length() >= 7 &&
		nom(0,7) == "Section" ) ||
	      ( nom.length() >= 3 &&
		( nom(0,3) == "BLM" ||
		  nom(0,3) == "BCM" ) ) ){
      return eSev;
    }
    return eOther;
  }
  
  
  
  
} // namespace G4UA 
