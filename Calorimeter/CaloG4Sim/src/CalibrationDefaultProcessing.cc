/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CalibrationDefaultProcessing
// 04-Mar-2004 William Seligman

// The calibration studies rely on every volume in the simulation
// being made into a sensitive detector.  There is a practical
// problem: What if we're still in the middle of developing code, and
// not every volume has been made sensitive yet?  What if we've
// overlooked a volume?

// This class provides a "default behavior" for all energy deposits
// that are not made in a volume that's been made sensitive for
// calibration studies.

// Since we need to examine the particles at every G4Step, this class
// makes use of a UserAction interface class.

#include "CaloG4Sim/CalibrationDefaultProcessing.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "G4VSensitiveDetector.hh"
#include "G4SDManager.hh"
#include "G4Run.hh"
#include "G4Step.hh"


namespace CaloG4 {

  // Register this class with the UserAction class managers.
  CalibrationDefaultProcessing::CalibrationDefaultProcessing(const std::string& type, const std::string& name, const IInterface* parent) : 
    UserActionBase(type,name,parent),m_defaultSD(0), m_SDname("Default::Dead::Uninstrumented::Calibration::Region")
      
  {

    declareProperty("SDName",m_SDname);

  }


  CalibrationDefaultProcessing::~CalibrationDefaultProcessing() {;}


  void CalibrationDefaultProcessing::BeginOfEvent(const G4Event*){

    // retrieve the SD from G4SDManager
    // done here instead of in initialize to leave more flexibility to the rest of the G4 init

     G4SDManager* SDman = G4SDManager::GetSDMpointer();

     m_defaultSD = G4SDManager::GetSDMpointer()-> FindSensitiveDetector(m_SDname);

     if(!m_defaultSD) ATH_MSG_ERROR("No valid SD name specified. The job will continue, but you should check your configuration");

  }

  void CalibrationDefaultProcessing::Step(const G4Step* a_step)
  {

    
    // Do we have a sensitive detector?
    if ( m_defaultSD != 0 )
      {
	// We only want to perform the default processing if no other
	// calibration processing has occurred for this step.

	if ( ! CaloG4::SimulationEnergies::StepWasProcessed() )
	  {
	    // We haven't performed any calibration processing for this
	    // step (probably there is no sensitive detector for the
	    // volume).  Use the default sensitive detector to process
	    // this step.  Note that we have to "cast away" const-ness for
	    // the G4Step*, due to how G4VSensitiveDetector::Hit() is
	    // defined.
	    m_defaultSD->Hit( const_cast<G4Step*>(a_step) );
	  }

	// In any case, clear the flag for the next step.
	CaloG4::SimulationEnergies::ResetStepProcessed();
      }
    else
      {
	static G4bool warningPrinted = false;
	if ( ! warningPrinted )
	  {
	    warningPrinted = true;
	    G4cout << "CaloG4::CalibrationDefaultProcessing::SteppingAction - "
		   << G4endl
		   << "   A default calibration sensitive detector was not defined."
		   << G4endl
		   << "   Not all calibration energies will be recorded."
		   << G4endl;
	  }
      }
  }

//  void CalibrationDefaultProcessing::SetDefaultSD( G4VSensitiveDetector* a_sd )
//  {
//    if ( m_defaultSD != 0 )
//      {
//	G4cout << "CaloG4::CalibrationDefaultProcessing::SetDefaultSD - "
//	       << G4endl
//	       << "   The default calibration sensitive detector '"
//	       << m_defaultSD->GetName()
//	       << "'" << G4endl
//	       << "   is being replace by sensitive detector '"
//	       << a_sd->GetName()
//	       << "'" << G4endl;
//      }
//
//    m_defaultSD = a_sd;
//  }
//
//


  StatusCode CalibrationDefaultProcessing::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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


} // namespace CaloG4
