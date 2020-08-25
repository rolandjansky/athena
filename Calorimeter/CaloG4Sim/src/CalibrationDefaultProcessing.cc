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

// For the event-level flag
#include "MCTruth/AtlasG4EventUserInfo.h"

#include "G4VSensitiveDetector.hh"
#include "G4SDManager.hh"
#include "G4Run.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

namespace G4UA
{

  namespace CaloG4
  {

    CalibrationDefaultProcessing::CalibrationDefaultProcessing(const Config& config)
      : AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),
                     "CalibrationDefaultProcessing"),
        m_config(config),
        m_defaultSD(0)
    {
    }

    void CalibrationDefaultProcessing::BeginOfEventAction(const G4Event*)
    {
      // retrieve the SD from G4SDManager
      // done here instead of in initialize to leave more flexibility to the rest of the G4 init
      //G4SDManager* SDman = G4SDManager::GetSDMpointer();
      m_defaultSD = G4SDManager::GetSDMpointer()->FindSensitiveDetector(m_config.SDName);

      if(!m_defaultSD) {
        ATH_MSG_ERROR("No valid SD name specified. The job will continue, " <<
                      "but you should check your configuration");
      }
    }

    void CalibrationDefaultProcessing::UserSteppingAction(const G4Step* a_step)
    {
      // Do we have a sensitive detector?
      if ( m_defaultSD != 0 ) {
        // We only want to perform the default processing if no other
        // calibration processing has occurred for this step.
        AtlasG4EventUserInfo* atlasG4EvtUserInfo = dynamic_cast<AtlasG4EventUserInfo*>(
            G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());
        auto* track = a_step->GetTrack();
        if ( atlasG4EvtUserInfo &&
             ( atlasG4EvtUserInfo->GetLastProcessedBarcode() != track->GetTrackID() ||
               atlasG4EvtUserInfo->GetLastProcessedStep() != track->GetCurrentStepNumber() ) )
        {
          // We haven't performed any calibration processing for this
          // step (probably there is no sensitive detector for the
          // volume).  Use the default sensitive detector to process
          // this step.  Note that we have to "cast away" const-ness for
          // the G4Step*, due to how G4VSensitiveDetector::Hit() is
          // defined.
          m_defaultSD->Hit( const_cast<G4Step*>(a_step) );

          // Update the step info
          atlasG4EvtUserInfo->SetLastProcessedBarcode( track->GetTrackID() );
          atlasG4EvtUserInfo->SetLastProcessedStep( track->GetCurrentStepNumber() );
        }
      }

      else {
        // FIXME - thread unsafe static!!!
        static G4bool warningPrinted = false;
        if ( ! warningPrinted ) {
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

  } // namespace CaloG4

} // namespace G4UA
