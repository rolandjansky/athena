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

TestActionEHist::TestActionEHist(string s) : 
     FADS::ActionsBase(s), FADS::UserAction(s), FADS::TrackingAction(),
	 _pds(0),  world(0), fFirstStep(0),  dCALO(2), dBeam(2), dIDET(2), dMUON(2),
	 dDetail(""), maxhists(1000), maxdirs(1000), p_tag("")
{}

void TestActionEHist::BeginOfRunAction(const G4Run* /*aRun*/)
{
#ifdef _myDebug
  G4cout << "#########################################" << G4endl
	 << "##                                     ##" << G4endl
	 << "##    TestActionEHist - BeginOfRun     ##" << G4endl
	 << "##                                     ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  // get jobOptions properties
  fName = theProperties["ROOTFileName"];
  if (fName.empty()) { 
      ATH_MSG_WARNING("No output file name specified, using default.root!");
      fName = "default.root";
  }
 
  char * endptr=0;
  if ( !theProperties["CaloDepth"].empty() ){
    dCALO = strtol(theProperties["CaloDepth"].c_str(),&endptr,0);
    if (endptr[0] != '\0') throw std::invalid_argument("Could not convert string to int: " + std::string(theProperties["CaloDepth"]));
  }
  if ( !theProperties["BeamPipeDepth"].empty() ){
    dBeam = strtol(theProperties["BeamPipeDepth"].c_str(),&endptr,0);
    if (endptr[0] != '\0') throw std::invalid_argument("Could not convert string to int: " + std::string(theProperties["BeamPipeDepth"]));
  }
  if ( !theProperties["InDetDepth"].empty() ){
    dIDET = strtol(theProperties["InDetDepth"].c_str(),&endptr,0);
    if (endptr[0] != '\0') throw std::invalid_argument("Could not convert string to int: " + std::string(theProperties["InDetDepth"]));
  }
  if ( !theProperties["MuonDepth"].empty() ){
    dMUON = strtol(theProperties["MuonDepth"].c_str(),&endptr,0);
    if (endptr[0] != '\0') throw std::invalid_argument("Could not convert string to int: " + std::string(theProperties["MuonDepth"]));
  }
  if ( !theProperties["MaxHists"].empty() ){
    maxhists = strtol(theProperties["MaxHists"].c_str(),&endptr,0);
    if (endptr[0] != '\0') throw std::invalid_argument("Could not convert string to int: " + std::string(theProperties["MaxHists"]));
  }
  if ( !theProperties["DetailDepth"].empty() ) dDetail = theProperties["DetailDepth"];
  maxdirs = maxhists;
  ATH_MSG_INFO("Retrieved job properties successfully");
    

  // initialize histogramming file (DON'T USE GAUDI) & directories
  world = new TFile(fName.c_str(), "RECREATE");
  ATH_MSG_INFO(fName<<" initialized, in directory "<<gDirectory->GetPath());

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

  world->Write();
  world->Close();
  delete world;
  ATH_MSG_INFO(fName<<" saved & closed.");

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
  TH1F* hExists = (TH1F*)gDirectory->FindObjectAny((part_tag+"_"+vol_tag+"_hist").c_str());
  if (!hLeft && !hExists) { return; }
  else if (!hExists) {
    hExists = new TH1F((part_tag+"_"+vol_tag+"_hist").c_str(),
		       (part_tag+" KE in "+vol_tag).c_str(),
		       nbins,0,nbins*binsize);
    hLeft--;
    ATH_MSG_DEBUG("Histogram "<<gDirectory->GetPath()<<"/"<<hExists->GetName()<<" created");
    if (!hLeft) ATH_MSG_INFO("Last histogram reached");
  }

  if (xfill >= 0 && weight >= 0) { hExists->Fill(xfill,weight); }
  else if (xfill >= 0) { hExists->Fill(xfill); }
  else { return; }
  ATH_MSG_DEBUG("KE added to "<<gDirectory->GetPath());
  return;
}


// Call or create directory & cd into it; returns directory-change status

bool TestActionEHist::BuildDirs(string vol_tag, string dirTitle, int& dLeft)
{
  bool enter = false;
  TDirectory* dExists = (TDirectory*)gDirectory->FindObjectAny(vol_tag.c_str());
  if (!dLeft && !dExists) { return enter; }
  if (!dExists) {
    dExists = new TDirectoryFile(vol_tag.c_str(),
				 dirTitle.c_str());
    dLeft--;
    ATH_MSG_DEBUG("Directory "<<gDirectory->GetPath()<<vol_tag<<" created");
    if (!dLeft) ATH_MSG_INFO("Last directory created");
  }

  if (dExists) enter = (bool)dExists->cd();
  if (enter) ATH_MSG_DEBUG("Current directory: "<<gDirectory->GetPath());
  return enter;
}
