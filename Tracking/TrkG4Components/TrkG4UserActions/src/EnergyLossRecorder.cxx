/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossRecorder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkG4UserActions/EnergyLossRecorder.h"
#include <iostream>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/MsgStream.h"
#include "SimHelpers/ServiceAccessor.h"
#include "G4Step.hh"
//#include "G4Material.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

//#include "TDirectory.h"



#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{
  
  
  EnergyLossRecorder::EnergyLossRecorder(const Config& config):
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"EnergyLossRecorder"),
    m_config(config),
    m_outfile(0),
    m_eventTree(0),
    m_runTree(0),
    m_entries(0),
    m_layer(),
    m_energy(),
    m_eLoss(),
    m_tInX0(),
    m_radius(),
    m_posX(),
    m_posY(),
    m_posZ(),
    m_eta(),
    m_theta(),
    m_phi(),
    m_used(),
    m_thickX0(0.),
    m_verboseLevel(0),
    m_cylinderR(),
    m_cylinderZ(),
    m_useX0(true),
    m_onionMode(true),
    m_keepAlive(false),
    m_startLayer(0),
    m_noMaterialStep(0),
    m_events(0),
    m_totalRecordedLayers(0),
    m_avgRecordedLayers(0)
  {;
  }


  void EnergyLossRecorder::beginOfRun(const G4Run*){
    
    if (m_config.cylinders > TRKG4USERACTIONS_MAXLAYERS-1) {
      m_config.cylinders = TRKG4USERACTIONS_MAXLAYERS-1;
      ATH_MSG_WARNING(" too many cylinders, setting to " << m_config.cylinders
		      << " ... please check your jobOptions !" );
    }
    if(m_config.useX0prop!="")
      m_useX0 = TurnOffString(m_config.useX0prop);
    
    if(m_config.onionModeprop!="")
      m_onionMode = TurnOffString(m_config.onionModeprop);
    
    if(m_config.keepAliveprop!="")
      m_keepAlive = TurnOffString(m_config.keepAliveprop);
    
    
    ATH_MSG_INFO(" cylinders    = " << m_config.cylinders   );
    ATH_MSG_INFO(" minEta       = " << m_config.minEta      );
    ATH_MSG_INFO(" maxEta       = " << m_config.maxEta      );
    ATH_MSG_INFO(" momentum     = " << m_config.momentum    );
    ATH_MSG_INFO(" pdg          = " << m_config.pdg         );
    ATH_MSG_INFO(" usePt        = " << m_config.usePt       );
    ATH_MSG_INFO(" useX0        = " << m_useX0       );
    ATH_MSG_INFO(" onionMode    = " << m_onionMode   );
    ATH_MSG_INFO(" keepAlive    = " << m_keepAlive   );
    
    // only barrel (according to Tracking Geometry, except last layer)
    //double const s_cylInitR[TRKG4USERACTIONS_MAXLAYERS] = { 0,   39.0,  163.7,   545.5,  1170  ,  4250, 10000, 13000, 0, 0 };
    //double const s_cylInitZ[TRKG4USERACTIONS_MAXLAYERS] = { 0, 2748  ,  455.5,   795.7,   795.8,  3250, 15000, 18650, 0, 0 };
    
    // old values (according to Atlas Geometry)
    //  double const s_cylInitR[TRKG4USERACTIONS_MAXLAYERS] = { 0,   34.5,  250  ,   550  ,  1085  ,  4250, 13000, 0, 0, 0 };
    //  double const s_cylInitZ[TRKG4USERACTIONS_MAXLAYERS] = { 0, 2748  ,  680  ,  2820  ,  3120  ,  6500, 22500, 0, 0, 0 };
    
    ++m_config.cylinders; // increase by one to get effective layers
    
    for (size_t lay = 1; lay<m_config.cylinders; ++lay) {
      
      m_cylinderR[lay]=m_config.Rvec[lay];
      m_cylinderZ[lay]=m_config.Zvec[lay];
      ATH_MSG_INFO(" m_cylinderR[" << lay << "] = " << m_cylinderR[lay] <<
		   "\t ... m_cylinderZ[" << lay << "] = " << m_cylinderZ[lay]);
    }
    

    ATH_MSG_INFO(" BeginOfRunAction" );

    m_outfile = 0;
    m_eventTree = 0;
    
    //if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   trying to create ROOT output file ..." << std::endl;
    m_outfile   = new TFile("outfile.root","RECREATE","TFile with energy loss tree"); // output file
    //if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   BeginOfRunAction : m_outfile = " << m_outfile << std::endl;
    
    m_eventTree = new TTree("EventTreeG4","Event output of the EnergyLossRecorder Algorithm");
    //if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   BeginOfRunAction : m_eventTree = " << m_eventTree << std::endl;
    
    m_eventTree->Branch("Entries",    &m_entries, "entries/i");
    m_eventTree->Branch("Energy",      m_energy,  "energy[entries]/F");
    m_eventTree->Branch("EnergyLoss",  m_eLoss,   "eLoss[entries]/F");
    m_eventTree->Branch("tInX0",       m_tInX0,   "tinX0[entries]/F");
    m_eventTree->Branch("Radius",      m_radius,  "radius[entries]/F");
    m_eventTree->Branch("PosX",        m_posX,    "posX[entries]/F");
    m_eventTree->Branch("PosY",        m_posY,    "posY[entries]/F");
    m_eventTree->Branch("PosZ",        m_posZ,    "posZ[entries]/F");
    m_eventTree->Branch("Eta",         m_eta,     "eta[entries]/F");
    m_eventTree->Branch("Phi",         m_phi,     "phi[entries]/F");
    m_eventTree->Branch("Layer",       m_layer,   "layer[entries]/I");
//  m_eventTree->Branch("Theta",       m_theta,   "theta[entries]/F");

    m_runTree = new TTree("RunTreeG4","Run stats of the EnergyLossRecorder Algorithm");
    if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   BeginOfRunAction : m_runTree = " << m_runTree << std::endl;

    m_runTree->Branch("Layers",  &m_config.cylinders,       "layers/i");
    m_runTree->Branch("CylR",     m_cylinderR,       "cylR[layers]/F");
    m_runTree->Branch("CylZ",     m_cylinderZ,       "cylZ[layers]/F");
    m_runTree->Branch("Momentum",&m_config.momentum,        "momentum/F");
    m_runTree->Branch("UsePt",   &m_config.usePt,           "usePt/O");
    m_runTree->Branch("MinEta",  &m_config.minEta,          "minEta/F");
    m_runTree->Branch("MaxEta",  &m_config.maxEta,          "maxEta/F");
    m_runTree->Branch("PDG",     &m_config.pdg,             "pdg/I");
    m_runTree->Branch("Events",  &m_events,          "events/i");
    m_runTree->Branch("NoMaterialStep", &m_noMaterialStep, "noMatStep/i");
    m_runTree->Branch("AvgRecordedLayers", &m_avgRecordedLayers, "avgRecLayers/F");

  }
  
  void EnergyLossRecorder::endOfRun(const G4Run*){

    ATH_MSG_INFO("   EndOfRunAction : no material found in " << m_noMaterialStep << " steps");
    ATH_MSG_INFO("   EndOfRunAction : total events = " << m_events );
    ATH_MSG_INFO("   EndOfRunAction : total recorded layers = " << m_totalRecordedLayers);

    m_avgRecordedLayers = m_events ? (float)m_totalRecordedLayers / (float)m_events : 0;
    ATH_MSG_INFO("   EndOfRunAction : average recorded layers = " << m_avgRecordedLayers );
    
    m_runTree->Fill();
    
    //if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   EndOfRunAction : writing ROOT output file" << std::endl;
    m_outfile->Write();
    //if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   EndOfRunAction : closing ROOT output file" << std::endl;
    m_outfile->Close();
    //  delete m_eventTree;
    //  delete m_outfile;
  }
  
  void EnergyLossRecorder::beginOfEvent(const G4Event*){

    //if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   BeginOfEventAction " << m_events << std::endl;
    
    m_entries    = 0;
    m_startLayer = 0;
    m_thickX0    = 0.;
    //    m_pdg       = 0;
    //    m_energyIn  = 0.;
    for (size_t i=0; i<m_config.cylinders; ++i) {
      m_energy[i] = 0.;
      m_eLoss[i]  = 0.;
      m_radius[i] = 0.;
      m_posX[i]   = 0.;
      m_posY[i]   = 0.;
      m_posZ[i]   = 0.;
      m_eta[i]    = 0.;
      m_theta[i]  = 0.;
      m_phi[i]    = 0.;
      m_tInX0[i]  = 0.;
      m_layer[i]  = 0;
      m_used[i]   = false;
    }
  }
  
  void EnergyLossRecorder::endOfEvent(const G4Event*){

    //if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   EndOfEventAction" << std::endl;
    
    //if (m_config.keepAlive && m_config.verboseLevel>1) {
    //  std::cout << "[EnergyLossRecorder]   EndOfEventAction : particle died at [r/z] = [ "
    //            << m_radius[m_cylinders] << " / " << m_posZ[m_cylinders] << " ]" << std::endl;
    //  std::cout << "[EnergyLossRecorder]   EndOfEventAction : particle died at [eta/phi] = [ "
    //            << m_eta[m_cylinders] << " / " << m_phi[m_cylinders] << " ]" << std::endl;
    //}
    
    ++m_events;
    m_totalRecordedLayers += m_entries-1;
    //    // this is the last layer with layerID = m_cylinders
    //    m_layer[m_entries] = m_cylinders;
    //    ++m_entries;
    // finally: fill the tree!
    m_eventTree->Fill();
  }
  
  void EnergyLossRecorder::processStep(const G4Step* aStep){
    
    // kill secondary particles
    if (aStep->GetTrack()->GetParentID()) {
      //if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [xx] killing secondary" << std::endl;
      aStep->GetTrack()->SetTrackStatus(fStopAndKill);
      return;
    }
    // keep primary particles
    
    // record the starting parameters at the first step
    if (m_entries==0) {
      
//        m_pdg = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); // for ROOT run tree
      
      //if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [ 0] recording start parameters" << std::endl;
      
      //        m_pdg             = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
      //        m_energyIn        = aStep->GetPreStepPoint()->GetKineticEnergy();
      
      // get the current (pre) momentum in eta + theta + phi
      m_eta[m_entries]    = aStep->GetPreStepPoint()->GetMomentum().getEta();
      m_theta[m_entries]  = aStep->GetPreStepPoint()->GetMomentum().getTheta();
      m_phi[m_entries]    = aStep->GetPreStepPoint()->GetMomentum().getPhi();
      
      // get the current (pre) position and radius
      G4ThreeVector pos   = aStep->GetPreStepPoint()->GetPosition();
      m_posX[m_entries]   = pos[0];
      m_posY[m_entries]   = pos[1];
      m_posZ[m_entries]   = pos[2];
      m_radius[m_entries] = sqrt(pos[0]*pos[0] + pos[1]*pos[1]);   // r = sqrt(x^2+y^2)
      
      // get the current (pre) energy and eLoss
      m_energy[m_entries] = aStep->GetPreStepPoint()->GetKineticEnergy();
      m_eLoss[m_entries]  = 0.;
      m_tInX0[m_entries]  = 0.;
      
      m_layer[m_entries]  = 0;
      // increase the index - this "records" the current parameters from the PreStepPoint
      ++m_entries;
      
      m_used[0] = true;
      m_startLayer = 1; // start loop from here (this value will be gradually increased)

    } else {
      
      // get the new position and radius after the step
      G4ThreeVector posnew = aStep->GetPostStepPoint()->GetPosition();
      float radiusnew     = sqrt(posnew[0]*posnew[0] + posnew[1]*posnew[1]);
      
      size_t recorded = 0; // has something been recorded already for this particle?
      
      // loop over all cylinders (ascending)
      for (size_t lay = m_startLayer; lay < m_config.cylinders; ++lay) {
	//if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [ " << lay << "] investigating layer " << lay << std::endl;
	
	if (!m_used[lay] // this layer was not yet used
	    && m_cylinderR[lay] // there exists a (positive) radius for this layer index
	    && (radiusnew>m_cylinderR[lay] || abs(posnew[2])>m_cylinderZ[lay]) ) // this layer has been passed
	  {
	    // record stuff
	    if (!recorded) { // nothing was recorded yet - get all the values
	      
	      // get the current (pre) momentum in eta + theta + phi
	      m_eta[m_entries]    = aStep->GetPreStepPoint()->GetMomentum().getEta();
	      m_theta[m_entries]  = aStep->GetPreStepPoint()->GetMomentum().getTheta();
	      m_phi[m_entries]    = aStep->GetPreStepPoint()->GetMomentum().getPhi();
	      
	      // get the current (pre) position and radius
	      G4ThreeVector pos   = aStep->GetPreStepPoint()->GetPosition();
	      m_posX[m_entries]   = pos[0];
	      m_posY[m_entries]   = pos[1];
	      m_posZ[m_entries]   = pos[2];
	      m_radius[m_entries] = sqrt(pos[0]*pos[0] + pos[1]*pos[1]);   // r = sqrt(x^2+y^2)
	      //if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [+" << lay
	      //				      << "] recording layer at [r/z] = [ " << m_radius[m_entries] << " / " << m_posZ[m_entries] << " ]" << std::endl;
	      
	      // get the current (pre) energy and eLoss
	      m_energy[m_entries] = aStep->GetPreStepPoint()->GetKineticEnergy();
	      m_eLoss[m_entries]  = m_energy[0] - m_energy[m_entries];
	      m_tInX0[m_entries]  = m_thickX0;
	      
	      recorded = m_entries; // the current step is recorded for the first time, so this index will be the base for copying
	      
	    } else { // this step was already recorded in a previous layer - copy the values (faster)
	      
	      //if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [+" << lay
	      //			      << "] recording layer - copying from previous layer " << recorded << std::endl;
	      
	      m_eta[m_entries]    = m_eta[recorded];
	      m_theta[m_entries]  = m_theta[recorded];
	      m_phi[m_entries]    = m_phi[recorded];
	      
	      m_posX[m_entries]   = m_posX[recorded];
	      m_posY[m_entries]   = m_posY[recorded];
	      m_posZ[m_entries]   = m_posZ[recorded];
	      m_radius[m_entries] = m_radius[recorded];
	      
	      m_energy[m_entries] = m_energy[recorded];
	      m_eLoss[m_entries]  = m_eLoss[recorded];
	      m_tInX0[m_entries]  = m_tInX0[recorded];
	      
	    }
	    
	    m_layer[m_entries]  = lay; // write this layer index (for sanity check)
	    ++m_entries; // increase the index - this "records" the current parameters from the PreStepPoint
	    m_used[lay] = true; // done recording this layer, don't use it again for the same particle
	    //if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [ " << lay << "] " << m_entries-1 << " layers recorded so far" << std::endl;
	    
	    if (m_entries>=m_config.cylinders) {
	      if (!m_keepAlive) {
		// all layers have been recorded, kill the particle
		//if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [XX] all layers recorded, killing primary" << std::endl;
		aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	      } //else {
		// ... or keep it e.g. for debugging / recording of very last step in the geometry
		//if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [oo] all layers recorded, keeping primary" << std::endl;
	      // }
	    }
	    
	    // increase startLayer for future steps if all previous layers have been recorded too
	    if (m_onionMode) { // success in onion mode means that startLayer can be increased for future loops
	      ++m_startLayer;
	    } else { // check from current startLayer onwards: if next layer is now used, increase startLayer and repeat
	      for (lay=m_startLayer; m_used[lay]; ++lay, ++m_startLayer) {}
	    }
	    //if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [ " << lay << "] increasing starting layer for loops to " << m_startLayer << std::endl;
	    
	  }
	// onionMode: strictly hierarchical ordering, break loop if a layer was not passed
	else if (m_onionMode) {
	  //if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [-" << lay << "] layer NOT intersected, breaking loop" << std::endl;
	  //                m_startLayer = lay; // from now onwards start from this layer, as the ones before were already recorded
	  break;
	}
	//else {
	//  if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [-" << lay << "] layer NOT intersected, continuing" << std::endl;
	//}
      }
    }
    
    // keep track of particle until end of geometry
    if (m_keepAlive) {
      G4ThreeVector pos = aStep->GetPostStepPoint()->GetPosition();
      m_posZ[m_config.cylinders]   = pos[2];
      m_radius[m_config.cylinders] = sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
      m_eta[m_config.cylinders]    = aStep->GetPostStepPoint()->GetMomentum().getEta();
      m_phi[m_config.cylinders]    = aStep->GetPostStepPoint()->GetMomentum().getPhi();
      //if (m_verboseLevel>1) {
      //	std::cout << "[EnergyLossRecorder]   SteppingAction [ " << m_startLayer << "] tracking particle to [r/z] = [ "
      //		  << m_radius[m_cylinders] << " / " << m_posZ[m_cylinders] << " ]" << std::endl;
      //	std::cout << "[EnergyLossRecorder]   SteppingAction [ " << m_startLayer << "] tracking particle to [eta/phi] = [ "
      //		  << m_eta[m_cylinders] << " / " << m_phi[m_cylinders] << " ]" << std::endl;
      //}
    }
    
    // at the end of every step add t/X0 ... (= step length in units of X0)
    if (m_useX0) {
      G4Material *mat = aStep->GetPreStepPoint()->GetMaterial();
      if (mat) {
	double steplength = aStep->GetStepLength();
	double X0         = mat->GetRadlen();
	m_thickX0        += X0 ? (float)(steplength/X0) : 0.;
	//	if (m_verboseLevel>1) std::cout << "[EnergyLossRecorder]   SteppingAction [ " << m_startLayer << "] m_thickX0 = " << m_thickX0
	//					<< "     collected in ... " << mat->GetName() << std::endl;
      }
      else {
	//	if (m_verboseLevel) std::cout << "[EnergyLossRecorder]   SteppingAction [!" << m_startLayer << "] WARNING - no material found!" << std::endl;
	++m_noMaterialStep;
      }
    }
  }
  
  
} // namespace G4UA 
