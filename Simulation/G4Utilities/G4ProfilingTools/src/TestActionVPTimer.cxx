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

TestActionVPTimer::TestActionVPTimer(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),
    m_runTimer(0), m_eventTimer(0),
    m_runTime(0.), m_eventTime(0.),
    dCALO(2), dBeam(2), dIDET(2), dMUON(2), dDetail(""),
    m_nev(0)
{

  declareProperty("CaloDepth",dCALO);
  declareProperty("BeamPipeDepth",dBeam);
  declareProperty("InDetDepth",dIDET);
  declareProperty("MuonDepth",dMUON);
  declareProperty("DetailDepth",dDetail);



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

#ifdef _myDebug
  G4cout << "TestActionVPTimer::Constructor done" << G4endl;
#endif
}

void TestActionVPTimer::BeginOfEvent(const G4Event* /*anEvent*/)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionVPTimer - BeginOfEvent   ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
  
  m_nev++;
  m_eventTimer->Start();

  return;
}


void TestActionVPTimer::EndOfEvent(const G4Event* /*anEvent*/)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionVPTimer - EndOfEvent     ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  m_eventTime += TimerSum(m_eventTimer);
 
  return;
}


void TestActionVPTimer::EndOfRun(const G4Run* /*aRun*/)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionVPTimer - EndOfRun       ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
  
  m_runTime += TimerSum(m_runTimer);

  VolTree topPV;
  topPV.push_back(v_time_index.begin()->first.front());
  volData atlasData = v_time_index.find(topPV)->second;
  G4double DeadTime  = m_runTime - atlasData.tTotal;

  boost::io::ios_base_all_saver coutsave (G4cout);
  G4cout.setf(ios::fixed,ios::floatfield);
  G4cout.precision(2);

  G4cout << "TestActionVPTimer::EndOfRunAction: Runtime " << m_runTime
	 << " Last Event " << m_eventTime/m_nev << " Dead Time " << DeadTime << G4endl;

  if (m_nev != 0){
      G4cout << "******* TestActionVPTimer: Beginning timer dump *******" << G4endl;

      G4cout << setw(12) << "Time" << setw(12) << "per Event" << setw(12)
	     << "per Run" << setw(6) << "Depth" << "  Location/Particle" << G4endl;
      G4cout << vPrFmt(m_runTime, m_nev, atlasData.tTotal, -999, "  Event Average") << G4endl;
      G4cout << vPrFmt(DeadTime, m_nev, atlasData.tTotal, -999, "  Dead/Initialization") << G4endl;

      ATH_MSG_INFO("\n******* Data by volume/select particles *******");
      ATH_MSG_INFO("Data printed in reverse tree order (mother following daughter)");

      TreeOut(topPV, atlasData.tTotal);
//      for (VolIt c = v_time_index.begin(); c != v_time_index.end(); c++) {
//          TimerPrint(*c, atlasData.tTotal, c->first.size());
//      }

      ATH_MSG_INFO( "\n******* Data by particle/category *******");

      G4cout << vPrFmt(atlasData.tElectron, m_nev, atlasData.tTotal, -999, "e+/-"   ) << G4endl;
      G4cout << vPrFmt(atlasData.tPhoton,   m_nev, atlasData.tTotal, -999, "gamma"  ) << G4endl;
      G4cout << vPrFmt(atlasData.tNeutron,  m_nev, atlasData.tTotal, -999, "neutron") << G4endl;
      G4cout << vPrFmt(atlasData.tPion,     m_nev, atlasData.tTotal, -999, "pi+/-/0") << G4endl;
      G4cout << vPrFmt(atlasData.tBaryon,   m_nev, atlasData.tTotal, -999, "baryon" ) << G4endl;
      G4cout << vPrFmt(atlasData.tLepton,   m_nev, atlasData.tTotal, -999, "lepton" ) << G4endl;
      G4cout << vPrFmt(atlasData.tMeson,    m_nev, atlasData.tTotal, -999, "meson"  ) << G4endl;
      G4cout << vPrFmt(atlasData.tOther,    m_nev, atlasData.tTotal, -999, "other"  ) << G4endl;

  } else { ATH_MSG_WARNING("******* No events timed! *******"); }
  return;
}


void TestActionVPTimer::Step(const G4Step* aStep)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionVPTimer - Stepping       ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  // HERE IS WHERE WE BEGIN OUR CLOCKING -- ONLY IF 
  // TIMERS ARE NOT VALID

  if (!v_timer->IsValid()) {

    // Collect the total time before processing anything else
    double vtime = TimerSum(v_timer);

    // CHECKIN' OUT THA NAVIGATA
    VolumeTreeNavigator currentTree( aStep );

    // Set depth cuts here
    currentTree.SetDepthCutSimple(dCALO, dBeam, dIDET, dMUON);
    if ( !dDetail.empty() ) {
        currentTree.SetDepthCutDetail( dDetail.c_str() );
   }

    // Store time generated in current volume, remove deepest entry in v_history, then
    // repeat for the resulting VolTree (one level higher)
    G4ParticleDefinition* PDef = currentTree.GetTrack()->GetDefinition();
    while ( true ) {
        VolTree VHistory = currentTree.Extract();
        v_time_index[VHistory].tTotal += vtime;
        if (PDef->GetParticleName() == "neutron")	{ v_time_index[VHistory].tNeutron += vtime; }
        else if (PDef->GetParticleSubType() == "e")	{ v_time_index[VHistory].tElectron += vtime; }
        else if (PDef->GetParticleSubType() == "pi")	{ v_time_index[VHistory].tPion += vtime; }
        else if (PDef->GetParticleType() == "gamma")	{ v_time_index[VHistory].tPhoton += vtime; }
        else if (PDef->GetParticleType() == "baryon")	{ v_time_index[VHistory].tBaryon += vtime; }
        else if (PDef->GetParticleType() == "lepton")	{ v_time_index[VHistory].tLepton += vtime; }
        else if (PDef->GetParticleType() == "meson")	{ v_time_index[VHistory].tMeson += vtime; }
        else						{ v_time_index[VHistory].tOther += vtime; }
        ATH_MSG_DEBUG("Time stored in "<<VHistory.back().first->GetName());
        if ( !currentTree.Ascend() )  break;
    }
  }

  // Restart timer
  v_timer->Start();

  return;
}


// printing function which sums the time of volumes below the current one in the tree by recursively calling itself,
// then starting a set of output statements at one particular leaf node and adding times as it moves upward

void TestActionVPTimer::TreeOut(VolTree id, const double tAtlas, int depth)
{
  VolIt v = v_time_index.find(id);
  if (v != v_time_index.end()) {
      depth++;
      for (VolIt c = v_time_index.begin(); c != v_time_index.end(); c++) {
	  VolTree check = c->first;
	  check.pop_back();
	  if (id == check)  TreeOut(c->first, tAtlas, depth);
      }
      TimerPrint(*v, tAtlas, depth);
  }
  return;
}


StatusCode TestActionVPTimer::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
