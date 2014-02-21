/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//								//
//			TestActionTimer				//
//	Code for text output (into the athena.out files)	//
//	of information about the time spent simulating		//
//	various pieces of the detector and particles.		//
//								//
//	Written by 	Zachary Marshall			//
//			Caltech					//
//			zmarshal@caltech.edu			//
//								//
//	Last update	08.07.08				//
//								//
//////////////////////////////////////////////////////////////////


#include "LArG4FastSimulation/TestActionTimer.h"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Step.hh"

#include "G4String.hh"
#include "G4Timer.hh"
#include "G4Track.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"

// #define _myDebug

static TestActionTimer ts1("TestActionTimer");


TestActionTimer::TestActionTimer(std::string s)
  : UserAction(s),
    m_runTimer(0), m_eventTimer(0),
    m_runTime(0.), m_eventTime(0.),
    m_nev(0)
{
  m_timeName.resize(eMax);

  m_timeName[eEMB]   = "EMB";
  m_timeName[eEMEC]  = "EMEC";
  m_timeName[eFC1]   = "FC1";
  m_timeName[eFC23]  = "FC23";
  m_timeName[eFCO]   = "FCO";
  m_timeName[eHEC]   = "HEC";
  m_timeName[eCry]   = "Cry";
  m_timeName[eLAr]   = "LAr";
  m_timeName[eHCB]   = "HCB";
  m_timeName[ePre]   = "Pre";
  m_timeName[eMu]    = "Mu";
  m_timeName[ePx]    = "Px";
  m_timeName[eTrt]    = "TRT";
  m_timeName[eSev]   = "IDServ";
  m_timeName[eSct]   = "SCT";
  m_timeName[eOther] = "Other";

  m_timeName[eElec]  = "Elec";
  m_timeName[eGam]   = "Gam";

  G4cout << "TestActionTimer::Constructor: Labels "
	 <<" Run Event ";
  for (int i(0); i < eMax; ++i) G4cout << m_timeName[i] << " ";
  G4cout << "Particle Dead" << G4endl;

  // init timers
  m_runTimer = new G4Timer();
  m_runTimer->Start();

  // create all timers, start and stop, init counters
  for (int i(0); i < eMax; ++i) {
    G4Timer* timer = new G4Timer();
    timer->Start();

    m_timer.push_back(timer);
    m_time.push_back(0.);
  }
    
  PPanic();
  VPanic();

#ifdef _myDebug
  G4cout << "TestActionTimer::Constructor done" << G4endl;
#endif
}

void TestActionTimer::BeginOfEventAction(const G4Event*)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionTimer - BeginOfEvent   ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
  
  m_nev++;

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

void TestActionTimer::EndOfEventAction(const G4Event*)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionTimer - EndOfEvent     ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  m_runTime += TimerSum(m_runTimer);
  m_eventTime = TimerSum(m_eventTimer);

  VPanic();
  PPanic();
  
  return;
}

void TestActionTimer::BeginOfRunAction(const G4Run*)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionTimer - BeginOfRun     ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  m_runTime=0.;
  m_runTimer->Start();

  return;
}

void TestActionTimer::EndOfRunAction(const G4Run*)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionTimer - EndOfRun       ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
  
  m_runTime += TimerSum(m_runTimer);

  VPanic();
  PPanic();

  G4double sumVTime = 0.;
  G4double sumPTime = 0.;
  for (int i(0); i <= eOther; ++i) sumVTime += m_time[i];
  for (int i(eElec); i <= eGam; ++i) sumPTime += m_time[i];

  G4double DeadTime  = m_runTime - sumVTime;
  G4double OtherPart = m_runTime - DeadTime - sumPTime;

  G4cout << "TestActionTimer::EndOfRunAction: Timer "
	 << m_runTime << " " << m_eventTime << " ";
  for (int i(0); i < eMax; ++i) {
    G4cout << m_time[i] << " ";
  }
  G4cout << OtherPart << " " << DeadTime << G4endl;

  if (m_nev>0){
    G4cout << "****** TestActionTimer: Beginning timer dump ******" << G4endl;
    G4cout << "Timer\tEvent\tFraction"<<G4endl;
    G4cout << "EvtAve\t"<<m_runTime/m_nev<<"\t100"<< G4endl;
    for (int i(0);i <= eOther; i++) {
      G4cout << m_timeName[i] << "\t" << m_time[i]/m_nev << "\t" << m_time[i]/m_runTime << G4endl;
    }
    G4cout << "Dead\t" << DeadTime/m_nev << "\t" << DeadTime/m_runTime << G4endl;
    for (int i(eElec);i <= eGam; i++) {
      G4cout << m_timeName[i] << "\t" << m_time[i]/m_nev << "\t" << m_time[i]/m_runTime << G4endl;
    }
    G4cout << "OtherPart\t" << OtherPart/m_nev << "\t" << OtherPart/m_runTime << G4endl;

    G4cout << "****** TestActionTimer: Major Subdet Times per Event *********" << G4endl;
    G4cout << "EMB\t" << (m_time[eEMB])/m_nev << G4endl;
    G4cout << "EMEC\t" << (m_time[eEMEC])/m_nev << G4endl;
    G4cout << "FCAL\t" << (m_time[eFC1]+m_time[eFC23]+m_time[eFCO])/m_nev << G4endl;
    G4cout << "HEC\t" << (m_time[eHEC])/m_nev << G4endl;
    G4cout << "Tile\t" << (m_time[eHCB])/m_nev << G4endl;
    G4cout << "Tracker\t" << (m_time[ePx]+m_time[eSct]+m_time[eTrt]+m_time[eSev])/m_nev << G4endl;
    G4cout << "Muons\t" << (m_time[eMu])/m_nev << G4endl;
    G4cout << "Oth.LAr\t" << (m_time[ePre]+m_time[eCry]+m_time[eLAr])/m_nev << G4endl;
    G4cout << "Other\t" << (m_time[eOther])/m_nev << G4endl;
    G4cout << "EvtAve\t" << (m_runTime)/m_nev << G4endl;
    G4cout << "******* TestActionTimer: Major Subdet Frac per Event *********" << G4endl;
    G4cout << "EMB\t" << (m_time[eEMB])/m_runTime*100. << G4endl;
    G4cout << "EMEC\t" << (m_time[eEMEC])/m_runTime*100. << G4endl;
    G4cout << "FCAL\t" << (m_time[eFC1]+m_time[eFC23]+m_time[eFCO])/m_runTime*100. << G4endl;
    G4cout << "HEC\t" << (m_time[eHEC])/m_runTime*100. << G4endl;
    G4cout << "Tile\t" << (m_time[eHCB])/m_runTime*100. << G4endl;
    G4cout << "Tracker\t" << (m_time[ePx]+m_time[eSct]+m_time[eTrt]+m_time[eSev])/m_runTime*100. << G4endl;
    G4cout << "Muons\t" << (m_time[eMu])/m_runTime*100. << G4endl;
    G4cout << "Oth.LAr\t" << (m_time[ePre]+m_time[eCry]+m_time[eLAr])/m_runTime*100. << G4endl;
    G4cout << "Other\t" << (m_time[eOther])/m_runTime*100. << G4endl;
    G4cout << "EvtAve\t" << (m_runTime)/m_nev << G4endl;
    G4cout << "******* TestActionTimer: Timer Information *********" << G4endl;
    G4cout << "Timers were run over " << m_nev << " events, and the averages were taken by" << G4endl;
    G4cout << "detector portion.  Detectors are determined by volume name.  Pieces include:" << G4endl;
    G4cout << "EMB, EMEC, FCAL, HEC, Tile - the sampling portions of the calorimeters." << G4endl;
    G4cout << "Pixel, SCT, TRT - the sensitive portions of the tracker only" << G4endl;
    G4cout << "InDetSvc - all remaining portions of the inner detector" << G4endl;
    G4cout << "Oth.LAr - all remaining portions of the LAr, including preshower, solenoid," << G4endl;
    G4cout << "\tcryostats, and services." << G4endl;
    G4cout << "Other - volumes not listed, including ATLAS::ATLAS and CALO::CALO" << G4endl;
    G4cout << "" << G4endl;
    G4cout << "Dead time includes overhead per event (file i/o, event loading, timing overhead)" << G4endl;
    G4cout << "" << G4endl;
    G4cout << "All times are user+system time on your processor" << G4endl;
    G4cout << "******* TestActionTimer: End of timer dump *********" << G4endl;

  } else {

    G4cout << "******* TestActionTimer: Nothing to report, no events timed! *********" << G4endl;

  }

  return;
}

void TestActionTimer::SteppingAction(const G4Step* aStep)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionTimer - Stepping       ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
  
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

  if (thePrePVname != thePostPVname){

    if( thePrePVname.length() >= 17 && 
             thePrePVname(13,4) == "EMEC" ){
      timer = m_timer[eEMEC];
      if (!timer->IsValid()){ m_time[eEMEC] += TimerSum(timer); } else { VPanic(); }
    }
    else if (thePrePVname.length() >= 16 && 
             thePrePVname(13,3) == "EMB" ){
      timer = m_timer[eEMB];
      if (!timer->IsValid()){ m_time[eEMB] += TimerSum(timer); } else { VPanic(); }
    }
    else if(thePrePVname.length() >= 25 && 
            thePrePVname(21,4) == "Cryo" ) {
      timer = m_timer[eCry];
      if (!timer->IsValid()){ m_time[eCry] += TimerSum(timer); } else { VPanic(); }
    }
    else if( thePrePVname.length() >= 26 && 
             thePrePVname(13,13) == "FCAL::Module1"){ 
      timer = m_timer[eFC1];
      if (!timer->IsValid()){ m_time[eFC1] += TimerSum(timer); } else { VPanic(); }
    }
    else if( thePrePVname.length() >= 25 && 
             thePrePVname(13,12) == "FCAL::Module" ){
      timer = m_timer[eFC23];
      if (!timer->IsValid()){ m_time[eFC23] += TimerSum(timer); } else { VPanic(); }
    }
    else if ( thePrePVname.length() >= 17 &&
         thePrePVname(13,4) == "FCAL" ){
      timer = m_timer[eFCO];
      if (!timer->IsValid()){ m_time[eFCO] += TimerSum(timer); } else { VPanic(); }
    }
    else if (thePrePVname.length() >= 16 && 
             thePrePVname(13,3) == "HEC" ){
      timer = m_timer[eHEC];
      if (!timer->IsValid()){ m_time[eHEC] += TimerSum(timer); } else { VPanic(); }
    }
    else if(thePrePVname.length() >= 31 && 
            thePrePVname(21,10) == "Presampler" ) {
      timer = m_timer[ePre];
      if (!timer->IsValid()){ m_time[ePre] += TimerSum(timer); } else { VPanic(); }
    }
    else if (thePrePVname.length() >= 3 && 
             thePrePVname(0,3) == "LAr" ){
      timer = m_timer[eLAr];
      if (!timer->IsValid()){ m_time[eLAr] += TimerSum(timer); } else { VPanic(); }
    }
    else if( ( (thePrePVname.length() >= 4 &&
                thePrePVname(0,4) == "Muon") ||
               thePrePVname(0,4) == "MUON" ) || 
             ( thePrePVname.length() >= 9 &&
               thePrePVname(0,9) == "DriftTube" ) ||
             thePrePVname.contains("MDT") ||
             ( thePrePVname.length() >= 12 &&
               thePrePVname(0,12) == "SensitiveGas" ) ||
             thePrePVname.contains("MDT") ||
             thePrePVname.contains("station") ){
      timer = m_timer[eMu];
      if (!timer->IsValid()){ m_time[eMu] += TimerSum(timer); } else { VPanic(); }
    }
    else if ((thePrePVname.length() >= 5 && 
              thePrePVname(0,5) == "Pixel") ||
             thePrePVname == "Outside Barrel Service"){
      timer = m_timer[ePx];
      if (!timer->IsValid()){ m_time[ePx] += TimerSum(timer); } else { VPanic(); }
    }
    else if (thePrePVname.length() >= 3 && 
             thePrePVname(0,3) == "SCT" ){
      timer = m_timer[eSct];
      if (!timer->IsValid()){ m_time[eSct] += TimerSum(timer); } else { VPanic(); }
    }
    else if ( ( thePrePVname.length() >= 3 &&
                thePrePVname(0,3) == "TRT" ) ||
                thePrePVname == "GasMANeg" ){
      timer = m_timer[eTrt];
      if (!timer->IsValid()){ m_time[eTrt] += TimerSum(timer); } else { VPanic(); }
    }
    else if (thePrePVname.length() >= 4 && 
             thePrePVname(0,4) == "Tile"){ 
      timer = m_timer[eHCB];
      if (!timer->IsValid()){ m_time[eHCB] += TimerSum(timer); } else { VPanic(); }
    }
    else if ( ( thePrePVname.length() >= 12 && 
                thePrePVname(0,12) == "InDetServMat" ) || 
              ( thePrePVname.length() >= 4 && 
                thePrePVname(0,4) == "IDET" ) ||
              ( thePrePVname.length() >= 8 && 
                thePrePVname(0,8) == "BeamPipe" ) || 
              ( thePrePVname.length() >= 7 &&
                thePrePVname(0,7) == "Section" ) ||
              ( thePrePVname.length() >= 3 &&
                ( thePrePVname(0,3) == "BLM" ||
                  thePrePVname(0,3) == "BCM" ) ) ){
      timer = m_timer[eSev];
      if (!timer->IsValid()){ m_time[eSev] += TimerSum(timer); } else { VPanic(); }
    } 
    else {
      timer = m_timer[eOther];
      if (!timer->IsValid()){ m_time[eOther] += TimerSum(timer); } else { VPanic(); }
    }
    
    // Now start the appropriate clock
    if( thePostPVname.length() >= 17 && 
             thePostPVname(13,4) == "EMEC" ){
      m_timer[eEMEC]->Start();
    }
    else if (thePostPVname.length() >= 16 && 
             thePostPVname(13,3) == "EMB" ){
      m_timer[eEMB]->Start();
    }
    else if(thePostPVname.length() >= 25 && 
            thePostPVname(21,4) == "Cryo" ) {
      m_timer[eCry]->Start();
    }
    else if( thePostPVname.length() >= 26 && 
             thePostPVname(13,13) == "FCAL::Module1"){
      m_timer[eFC1]->Start();
    }
    else if( thePostPVname.length() >= 25 && 
             thePostPVname(13,12) == "FCAL::Module" ){
      m_timer[eFC23]->Start();
    }
    else if (thePostPVname.length() >= 17 && 
             thePostPVname(13,4) == "FCAL" ){
      m_timer[eFCO]->Start();
    }
    else if( thePostPVname.length() >= 31 && 
             thePostPVname(21,10) == "Presampler" ) {
      m_timer[ePre]->Start();
    }
    else if (thePostPVname.length() >= 16 && 
             thePostPVname(13,3) == "HEC" ){
      m_timer[eHEC]->Start();
    }
    else if (thePostPVname.length() >= 3 && 
             thePostPVname(0,3) == "LAr" ){
      m_timer[eLAr]->Start();
    }
    else if( ( (thePostPVname.length() >= 4 &&
                thePostPVname(0,4) == "Muon") ||
               thePostPVname(0,4) == "MUON" ) ||
             ( thePostPVname.length() >= 9 &&
               thePostPVname(0,9) == "DriftTube" ) ||
             thePostPVname.contains("MDT") ||
             ( thePostPVname.length() >= 12 &&
               thePostPVname(0,12) == "SensitiveGas" ) ||
             thePostPVname.contains("MDT") ||
             thePostPVname.contains("station") ){
      m_timer[eMu]->Start();
    }
    else if ((thePostPVname.length() >= 5 &&
              thePostPVname(0,5) == "Pixel") ||
             thePostPVname == "Outside Barrel Service"){
      m_timer[ePx]->Start();
    }
    else if ( ( thePostPVname.length() >= 3 && 
                thePostPVname(0,3) == "TRT" ) || 
                thePostPVname == "GasMANeg" ){
      m_timer[eTrt]->Start();
    }
    else if (thePostPVname.length() >= 3 && 
             thePostPVname(0,3) == "SCT" ){
      m_timer[eSct]->Start();
    }
    else if (thePostPVname.length() >= 4 && 
             thePostPVname(0,4) == "Tile"){
      m_timer[eHCB]->Start();
    }
    else if ( ( thePostPVname.length() >= 12 &&
                thePostPVname(0,12) == "InDetServMat" ) ||
              ( thePostPVname.length() >= 4 &&
                thePostPVname(0,4) == "IDET" ) ||
              ( thePostPVname.length() >= 8 &&
                thePostPVname(0,8) == "BeamPipe" ) ||
              ( thePostPVname.length() >= 7 &&
                thePostPVname(0,7) == "Section" ) ||
              ( thePostPVname.length() >= 3 &&
                ( thePostPVname(0,3) == "BLM" ||
                  thePostPVname(0,3) == "BCM" ) ) ){
      m_timer[eSev]->Start();
    }
    else if (thePostPVname != "OutOfWorld"){
      m_timer[eOther]->Start();
    }
    
    // Otherwise it's going into "out of world"
  } else {
    
    if (thePrePVname.length() >= 17 && 
        thePrePVname(13,4) == "EMB" ){
      timer = m_timer[eEMB];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if(thePrePVname.length() >= 25 && 
            thePrePVname(21,4) == "Cryo" ) {
      timer = m_timer[eCry];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if( thePrePVname.length() >= 17 && 
             thePrePVname(13,4) == "EMEC" ){
      timer = m_timer[eEMEC];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if( thePrePVname.length() >= 26 && 
             thePrePVname(13,13) == "FCAL::Module1"){
      timer = m_timer[eFC1];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if( thePrePVname.length() >= 25 && 
             thePrePVname(13,12) == "FCAL::Module" ){
      timer = m_timer[eFC23];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if (thePrePVname.length() >= 17 && 
             thePrePVname(13,4) == "FCAL" ){
      timer = m_timer[eFCO];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if( thePrePVname.length() >= 31 && 
             thePrePVname(21,10) == "Presampler" ) {
      timer = m_timer[ePre];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if (thePrePVname.length() >= 16 && 
             thePrePVname(13,3) == "HEC" ){
      timer = m_timer[eHEC];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if (thePrePVname.length() >= 3 && 
             thePrePVname(0,3) == "LAr" ){
      timer = m_timer[eLAr];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if( ( (thePrePVname.length() >= 4 &&
                thePrePVname(0,4) == "Muon") ||
               thePrePVname(0,4) == "MUON" ) ||
             ( thePrePVname.length() >= 9 &&
               thePrePVname(0,9) == "DriftTube" ) ||
             thePrePVname.contains("MDT") ||
             ( thePrePVname.length() >= 12 &&
               thePrePVname(0,12) == "SensitiveGas" ) ||
             thePrePVname.contains("MDT") ||
             thePrePVname.contains("station") ){
      timer = m_timer[eMu];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if ((thePrePVname.length() >= 5 &&
              thePrePVname(0,5) == "Pixel") ||
             thePrePVname == "Outside Barrel Service"){
      timer = m_timer[ePx];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if (thePrePVname.length() >= 3 && 
             thePrePVname(0,3) == "SCT" ){
      timer = m_timer[eSct];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if ( ( thePrePVname.length() >= 3 &&
                thePrePVname(0,3) == "TRT" ) ||
                thePrePVname == "GasMANeg" ){
      timer = m_timer[eTrt];
      if (timer->IsValid()) {
        VPanic(); timer->Start();
      }
    }
    else if (thePrePVname.length() >= 4 && 
             thePrePVname(0,4) == "Tile"){
      timer = m_timer[eHCB];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else if ( ( thePrePVname.length() >= 12 &&
                thePrePVname(0,12) == "InDetServMat" ) ||
              ( thePrePVname.length() >= 4 &&
                thePrePVname(0,4) == "IDET" ) ||
              ( thePrePVname.length() >= 8 &&
                thePrePVname(0,8) == "BeamPipe" ) ||
              ( thePrePVname.length() >= 7 &&
                thePrePVname(0,7) == "Section" ) ||
              ( thePrePVname.length() >= 3 &&
                ( thePrePVname(0,3) == "BLM" ||
                  thePrePVname(0,3) == "BCM" ) ) ){
      timer = m_timer[eSev];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
    else {
      timer = m_timer[eOther];
      if (timer->IsValid()) {
	VPanic(); timer->Start();
      }
    }
  }
  
  // Now for the particle based timers
  
  if (track->GetDefinition() == G4Electron::ElectronDefinition() ||
      track->GetDefinition() == G4Positron::PositronDefinition() ){
    if ( m_timer[eElec]->IsValid() ){
      m_timer[eElec]->Start();
    }
  } else if (!m_timer[eElec]->IsValid()){
    m_time[eElec] += TimerSum(m_timer[eElec]);
  }

  if (track->GetDefinition() == G4Gamma::GammaDefinition() && 
      m_timer[eGam]->IsValid()){
    m_timer[eGam]->Start();
  } else if ( !m_timer[eGam]->IsValid() ){
    m_time[eGam] += TimerSum(m_timer[eGam]);
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
  for (int i(eElec); i <= eGam; ++i) {
    G4Timer* timer = m_timer[i];
#ifdef _myDebug
    G4cout << "TestActionTimer::PPanic stopping counter i:" << i << " " << timer << G4endl;
#endif
    if (!timer->IsValid()){
      m_time[i] += TimerSum(timer);
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
      m_time[i] += TimerSum(timer);
    }
  }

#ifdef _myDebug
  G4cout << "TestActionTimer::VPanic done" << G4endl;
#endif

  return;
}
