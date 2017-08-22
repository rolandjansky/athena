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

#include "TestActionEHist.h"

#include "G4Track.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4ParticleDefinition.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VProcess.hh"
#include "TFile.h"
#include "TH1F.h"

#include <cstdlib>

using std::string;	using std::stringstream;
using std::vector;

// #define _myDebug

namespace G4UA{
  
  TestActionEHist::TestActionEHist(const Config& config):m_config(config),
							 m_world(0),
							 m_firstStep(false), 
							 m_p_tag(""){
  }
  
  
  void TestActionEHist::PreUserTrackingAction(const G4Track* aTrack){
    if (aTrack) {
      
      // Set Particle label and empty trajectory
      G4ParticleDefinition* pDef = aTrack->GetDefinition();
      G4String pName    = pDef->GetParticleName();
      G4String pSubType = pDef->GetParticleSubType();
      if (pName == "neutron" || pName == "proton" ) { m_p_tag = pName; }
      else if (pSubType =="e" || pSubType == "pi" ) { m_p_tag = pSubType; }
      else { m_p_tag = pDef->GetParticleType(); }
      if (!m_trajectory.empty()) m_trajectory.clear();
    }
    return;
  }
  
  void TestActionEHist::PostUserTrackingAction(const G4Track* aTrack){
    if (aTrack) {
      m_trajectory.clear();
    }
    return;
  }
  
  void TestActionEHist::BeginOfRunAction(const G4Run*){
    // initialize histogramming file (DON'T USE GAUDI) & directories
    m_world = new TFile(m_config.name.c_str(), "RECREATE");
    G4cout<<m_config.name<<" initialized, in directory "<<gDirectory->GetPath()<<G4endl;
    
  }
  
  void TestActionEHist::EndOfRunAction(const G4Run*){

    m_world->Write();
    m_world->Close();
    delete m_world;
    G4cout<<m_config.name<<" saved & closed."<<G4endl;
    
    return;
  }
  
  void TestActionEHist::UserSteppingAction(const G4Step* aStep){
    
    if (aStep) {
      
      // Create tree structure for current step
      VolumeTreeNavigator currentTree( aStep );
      
      // check for processes which don't need to be histogrammed
      //if ( currentTree.KillProcesses( 2, "nKiller", "G4FastSimulationManagerProcess" ) )  return;
      
      // Stores a list of the names of leaf-node volumes which the track has entered
      // and checks it for earlier occurences of the same volume
      m_firstStep = true;
      if (currentTree.GetStepNumber() > 1) {
	for ( vector<string>::const_iterator it = m_trajectory.begin(); it != m_trajectory.end(); it++ ) {
	  if ( *it == stringify( currentTree.GetVolume()->GetName() ) ) { m_firstStep = false;  break; }
	}
      }
      
      // For particles' first step in the current volume
      if (m_firstStep) {
	// push_back trajectory element
	m_trajectory.push_back( stringify( currentTree.GetVolume()->GetName() ) );
	
	// set depth cut (MUST implement Simple if Detail is used)
	currentTree.SetDepthCutSimple(m_config.dCALO, m_config.dBeam, m_config.dIDET, m_config.dMUON);
	// Detailed depth cut format: /Atlas::Atlas/[level 1]/[level 2]/.../[last directory level]
	if ( !m_config.dDetail.empty() ) {
	  currentTree.SetDepthCutDetail( m_config.dDetail.c_str() );
	}
	
//	for( auto histelement : currentTree.GetHistory())
//	    std::cout<<histelement.first->GetName()<< std::endl;
//	std::cout<<"end dump tree "<<std::endl;
//	std::cout<<"before ascending "<<currentTree.GetVolume()->GetName()<<std::endl;
//
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
	  string p_name = m_p_tag + ( currentTree.GetStepNumber() != 1 ? "_entred" : "_madein" );
	  string title = thNoDau+" daughters, "+stringify(currentTree.GetCurrentDepth())+" from base";
	  
	  // Build and fill histogram at bottom of tree, then build dir and cd to it
	  if ( currentTree.GetStepNumber() == 1 || currentTree.GetCurrentDepth() == currentTree.GetFullDepth() ) {
            BuildHists(v_name, p_name, m_config.maxhists, currentTree.GetPreStepPoint()->GetKineticEnergy());
	  }
	  if ( !currentTree.Descend() )  break;
	  if ( !BuildDirs(v_name, title, m_config.maxdirs) ) break;
	}
	
	// Return to world to get ready for next step... FINALLY
	m_world->cd();
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
      //G4cout<<"Histogram "<<gDirectory->GetPath()<<"/"<<hExists->GetName()<<" created"<<G4endl;
      if (!hLeft) G4cout<<"Last histogram reached"<<G4endl;
    }
    
    if (xfill >= 0 && weight >= 0) { hExists->Fill(xfill,weight); }
    else if (xfill >= 0) { hExists->Fill(xfill); }
    else { return; }
    
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

      if (!dLeft) G4cout<<"Last directory created"<<G4endl;
  }

    if (dExists) enter = (bool)dExists->cd();
    //    if (enter) ATH_MSG_DEBUG("Current directory: "<<gDirectory->GetPath());
    return enter;
  }



} // namespace G4UA 
