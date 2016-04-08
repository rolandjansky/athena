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

TestActionTimer::TestActionTimer(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),
    m_runTimer(0), m_eventTimer(0),
    m_runTime(0.), m_eventTime(0.),
    m_nev(0), m_histSvc("THistSvc",name)
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
  m_timeName[ePos]   = "Pos";
  m_timeName[eGam]   = "Gam";
  m_timeName[eNeut]  = "Neut";

  G4cout<< "TestActionTimer::Constructor: Labels "
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

void TestActionTimer::BeginOfEvent(const G4Event*)
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

void TestActionTimer::EndOfEvent(const G4Event*)
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

void TestActionTimer::BeginOfRun(const G4Run*)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
         << "##                                     ##" << G4endl
         << "##    TestActionTimer - BeginOfRun     ##" << G4endl
         << "##                                     ##" << G4endl
         << "#########################################" << G4endl;
#endif

  if ( m_histSvc.retrieve().isFailure() ){
    ATH_MSG_ERROR( "Failed getting the hist svc!" );
  }

  m_runTime=0.;
  m_runTimer->Start();

  return;
}

void TestActionTimer::EndOfRun(const G4Run*)
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
  for (int i(eElec); i < eMax; ++i) sumPTime += m_time[i];

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
    for (int i(eElec);i < eMax; i++) {
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

    TH1D * r_geo = new TH1D("GeoResults","GeoResults",eOther-eEMB+1,-0.5,float(eOther-eEMB+1)-0.5);
    TH1D * r_part = new TH1D("ParticleResults","ParticleResults",eMax-eElec+1,-0.5,float(eMax-eElec+1)-0.5);

    m_runTime -= DeadTime;
    for (int i(0); i <= eOther; ++i){
      r_geo->Fill( i , m_time[i] / m_runTime*100. );
      r_geo->GetXaxis()->SetBinLabel( i+1 , m_timeName[i].c_str() );
    }
    for (int i(eElec); i < eMax; ++i){
      r_part->Fill( i-eElec , m_time[i] / m_runTime*100. );
      r_part->GetXaxis()->SetBinLabel( i-eElec+1 , m_timeName[i].c_str() );
    }
    r_part->Fill( eMax-eElec , OtherPart / m_runTime*100. ); // Other particles don't get their own timer
    r_part->GetXaxis()->SetBinLabel( eMax-eElec+1 , "Others" );
    r_geo->SetXTitle("Detector Region");
    r_part->SetXTitle("Particle Type");
    r_geo->SetYTitle("Percent of Event Time");
    r_part->SetYTitle("Percent of Event Time");

    if (m_histSvc){
      if ( m_histSvc->regHist("/simTime/GeoResults",r_geo).isFailure() ||
           m_histSvc->regHist("/simTime/ParticleResults",r_part).isFailure() ){
        ATH_MSG_ERROR("Could not register histograms!");
      }
    }

  } else {

    G4cout << "******* TestActionTimer: Nothing to report, no events timed! *********" << G4endl;

  }

  return;
}

void TestActionTimer::Step(const G4Step* aStep)
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
  int preIndex = ClassifyVolume( thePrePVname );
  int postIndex=-1;
  if (thePrePVname != thePostPVname){
    timer = m_timer[preIndex];
    if (!timer->IsValid()){ m_time[preIndex] += TimerSum(timer); } else { VPanic(); }

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
    m_time[eElec] += TimerSum(m_timer[eElec]);
  }
  if (track->GetDefinition() == G4Positron::PositronDefinition() ){
    if ( m_timer[ePos]->IsValid() ){
      m_timer[ePos]->Start();
    }
  } else if (!m_timer[ePos]->IsValid()){
    m_time[ePos] += TimerSum(m_timer[ePos]);
  }
  if (track->GetDefinition() == G4Gamma::GammaDefinition() &&
      m_timer[eGam]->IsValid()){
    m_timer[eGam]->Start();
  } else if ( !m_timer[eGam]->IsValid() ){
    m_time[eGam] += TimerSum(m_timer[eGam]);
  }
  if (track->GetDefinition() == G4Neutron::NeutronDefinition() &&
      m_timer[eNeut]->IsValid()){
    m_timer[eNeut]->Start();
  } else if ( !m_timer[eNeut]->IsValid() ){
    m_time[eNeut] += TimerSum(m_timer[eNeut]);
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

StatusCode TestActionTimer::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
