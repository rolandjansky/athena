/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//								//
//			TestActionEHist.cxx			//
//	Code for ROOT output (filename selected in code)	//
//	of histograms of the initial kinetic energy		//
//	(from truth) of specified particles, by volume.		//
//								//
//	Written by 	Kevin Sapp				//
//			University of Pittsburgh		//
//			kcs34@pitt.edu				//
//	Last update	07.09.09				//
//								//
//////////////////////////////////////////////////////////////////

#include "LArG4FastSimulation/TestActionEHist.h"

#include <G4Track.hh>
#include <G4Electron.hh>
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4ParticleDefinition.hh>
#include <G4TouchableHistory.hh>
#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4VProcess.hh>

#include <cstdlib>

using std::string;	using std::stringstream;
using std::vector;

// #define _myDebug

static TestActionEHist ts2("TestActionEHist");

TestActionEHist::TestActionEHist(string s) : UserAction(s), TrackingAction(),
	 _pds(0),  world(0), fFirstStep(0),  dCALO(2), dBeam(2), dIDET(2), dMUON(2),
	 dDetail(""), maxhists(1000), maxdirs(1000), p_tag("")
{ 
  // test GAUDI messaging service
  _msgSvc = Athena::getMessageSvc();
  MsgStream log(_msgSvc,"TestActionEHist");
  log<< MSG::INFO <<"Athena message service initialized (TestActionEHist)"<< endmsg;
}

void TestActionEHist::BeginOfRunAction(const G4Run* /*aRun*/)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionEHist - BeginOfRun     ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  MsgStream log(_msgSvc,"TestActionEHist");

  // get jobOptions properties
  fName = theProperties["ROOTFileName"];
  if (fName.empty()) { 
      log<< MSG::WARNING <<"No output file name specified, using default.root!"<< endmsg;
      fName = "default.root";
  }
  
  if ( !theProperties["CaloDepth"].empty() )     dCALO = atoi(theProperties["CaloDepth"].c_str());
  if ( !theProperties["BeamPipeDepth"].empty() ) dBeam = atoi(theProperties["BeamPipeDepth"].c_str());
  if ( !theProperties["InDetDepth"].empty() )    dIDET = atoi(theProperties["InDetDepth"].c_str());
  if ( !theProperties["MuonDepth"].empty() )     dMUON = atoi(theProperties["MuonDepth"].c_str());
  if ( !theProperties["DetailDepth"].empty() )   dDetail = theProperties["DetailDepth"];
  if ( !theProperties["MaxHists"].empty() )      maxhists = atoi(theProperties["MaxHists"].c_str());
  maxdirs = maxhists;
  log<< MSG::INFO <<"Retrieved job properties successfully"<< endmsg;
    

  // initialize histogramming file (DON'T USE GAUDI) & directories
  world = new TFile(fName.c_str(), "RECREATE");
  log<< MSG::INFO <<fName<<" initialized, in directory "<<gDirectory->GetPath()<< endmsg;

  return;
}


void TestActionEHist::EndOfRunAction(const G4Run* /*aRun*/)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionEHist - EndOfRun       ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  MsgStream log(_msgSvc,"TestActionEHist");
  world->Write();
  world->Close();
  delete world;
  log<< MSG::INFO <<fName<<" saved & closed."<< endmsg;

  return;
}


void TestActionEHist::PreUserTrackingAction(const G4Track* aTrack)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##   TestActionEHist - BeginOfTrack    ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
if (aTrack) {

  // Set Particle label and empty trajectory
  G4ParticleDefinition* pDef = aTrack->GetDefinition();
  G4String pName    = pDef->GetParticleName();
  G4String pSubType = pDef->GetParticleSubType();
  if (pName == "neutron" || pName == "proton" ) { p_tag = pName; }
  else if (pSubType =="e" || pSubType == "pi" ) { p_tag = pSubType; }
  else { p_tag = pDef->GetParticleType(); }
  if (!trajectory.empty()) trajectory.clear();
}
  return;
}


void TestActionEHist::PostUserTrackingAction(const G4Track* aTrack)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionEHist - EndOfTrack     ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
if (aTrack) {
  trajectory.clear();
}
  return;
}


void TestActionEHist::SteppingAction(const G4Step* aStep)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionEHist - Stepping       ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif
if (aStep) {

  MsgStream log(_msgSvc,"TestActionEHist");

  // Create tree structure for current step
  VolumeTreeNavigator currentTree( aStep );

  // check for processes which don't need to be histogrammed
  //if ( currentTree.KillProcesses( 2, "nKiller", "G4FastSimulationManagerProcess" ) )  return;

  // Stores a list of the names of leaf-node volumes which the track has entered
  // and checks it for earlier occurences of the same volume
  fFirstStep = true;
  if (currentTree.GetStepNumber() > 1) {
    for ( vector<string>::const_iterator it = trajectory.begin(); it != trajectory.end(); it++ ) {
      if ( *it == stringify( currentTree.GetVolume()->GetName() ) ) { fFirstStep = false;  break; }
    }
  }

  // For particles' first step in the current volume
  if (fFirstStep) {
    // push_back trajectory element
    trajectory.push_back( stringify( currentTree.GetVolume()->GetName() ) );

    // set depth cut (MUST implement Simple if Detail is used)
    currentTree.SetDepthCutSimple(dCALO, dBeam, dIDET, dMUON);
    // Detailed depth cut format: /Atlas::Atlas/[level 1]/[level 2]/.../[last directory level]
    if ( !dDetail.empty() ) {
        currentTree.SetDepthCutDetail( dDetail.c_str() );
    }

    // go to ATLAS::ATLAS
    currentTree.Ascend( currentTree.GetCurrentDepth() );

    while ( true ) {
        // collect naming info for histogram
	G4VPhysicalVolume* pv = currentTree.GetVolume();
	string thPV = cleanstr(pv->GetName());
	string thNoDau = stringify(pv->GetLogicalVolume()->GetNoDaughters());
        int thRep = currentTree.GetCopyNumber();

	// construct keyname & directory title
	string v_name = (thRep == 16969 ? thPV : thPV+"_"+stringify(thRep));
	string p_name = p_tag + ( currentTree.GetStepNumber() != 1 ? "_entred" : "_madein" );
	string title = thNoDau+" daughters, "+stringify(currentTree.GetCurrentDepth())+" from base";

	// Build and fill histogram at bottom of tree, then build dir and cd to it
	if ( currentTree.GetStepNumber() == 1 || currentTree.GetCurrentDepth() == currentTree.GetFullDepth() ) {
            BuildHists(v_name, p_name, maxhists, currentTree.GetPreStepPoint()->GetKineticEnergy());
        }
        if ( !currentTree.Descend() )  break;
	if ( !BuildDirs(v_name, title, maxdirs) ) break;
    }

    // Return to world to get ready for next step... FINALLY
    world->cd();
  }
}
  return;
}


// Call or create histogram & fill it; returns fill status

void TestActionEHist::BuildHists(string vol_tag, string part_tag, int& hLeft, double xfill,
				 double weight, const int nbins, const int binsize)
{
  MsgStream log(_msgSvc, "TestActionEHist::BuildHists");
  TH1F* hExists = (TH1F*)gDirectory->FindObjectAny((part_tag+"_"+vol_tag+"_hist").c_str());
  if (!hLeft && !hExists) { return; }
  else if (!hExists) {
    hExists = new TH1F((part_tag+"_"+vol_tag+"_hist").c_str(),
		       (part_tag+" KE in "+vol_tag).c_str(),
		       nbins,0,nbins*binsize);
    hLeft--;
    log<< MSG::DEBUG <<"Histogram "<<gDirectory->GetPath()<<"/"<<hExists->GetName()<<" created"<< endmsg;
    if (!hLeft) log<< MSG::INFO <<"Last histogram reached"<< endmsg;
  }

  if (xfill >= 0 && weight >= 0) { hExists->Fill(xfill,weight); }
  else if (xfill >= 0) { hExists->Fill(xfill); }
  else { return; }
  log<< MSG::DEBUG <<"KE added to "<<gDirectory->GetPath()<< endmsg;
  return;
}


// Call or create directory & cd into it; returns directory-change status

bool TestActionEHist::BuildDirs(string vol_tag, string dirTitle, int& dLeft)
{
  MsgStream log(_msgSvc, "TestActionEHist::BuildDirs");
  bool enter = false;
  TDirectory* dExists = (TDirectory*)gDirectory->FindObjectAny(vol_tag.c_str());
  if (!dLeft && !dExists) { return enter; }
  if (!dExists) {
    dExists = new TDirectoryFile(vol_tag.c_str(),
				 dirTitle.c_str());
    dLeft--;
    log<< MSG::DEBUG <<"Directory "<<gDirectory->GetPath()<<vol_tag<<" created"<< endmsg;
    if (!dLeft) log<< MSG::INFO <<"Last directory created"<< endmsg;
  }

  if (dExists) enter = (bool)dExists->cd();
  if (enter) log<< MSG::DEBUG <<"Current directory: "<<gDirectory->GetPath()<< endmsg;
  return enter;
}
