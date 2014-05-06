/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHitMaker

// Created by F. Mazzucato,  September 2002
// Modified my E.Barberio  January 2003
// LArG4 compatibility introduced 30-Apr-2003 Bill Seligman

// 11-Aug-04 WGS: It appears that having more than one G4Navigator
// object in Geant4 may be a bad idea.  Don't create a separate
// G4Navigator for this class; use the one in G4TransportationManager.

#include "LArG4Code/LArHitMaker.h"
#include "LArG4Code/EnergySpot.h"

#include "G4TransportationManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4TouchableHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Navigator.hh"
#include "G4VPhysicalVolume.hh"

#include "globals.hh"
#include "G4ios.hh"

#undef DEBUG_FAST

LArHitMaker::LArHitMaker()
{
  // Initialize the fake G4Step that we'll pass to the sensitive
  // detector.

  fFakeStep          = new G4Step();
  fFakePreStepPoint  = fFakeStep->GetPreStepPoint();
  fFakePostStepPoint = fFakeStep->GetPostStepPoint();
  fTouchableHandle   = new G4TouchableHistory();
  fpNavigator        = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  fNaviSetup         = false;
  fBaseName          = "LAr";
}


LArHitMaker::~LArHitMaker()
{
  // Cleanup pointers.
  delete fFakeStep;
  delete fpNavigator;
}


void LArHitMaker::make(const EnergySpot& a_spot)
{
  // ------------------- 
  // Locate the spot.  Use fNaviSetup to test whether we've
  // initialized the G4Navigator _after_ all the G4 geometry
  // construction routines have executed.
  // -------------------
#ifdef DEBUG_FAST
            G4cout << "LArHitMaker::make -- Begin Class."
                   << G4endl;
#endif

  if (!fNaviSetup)
    {
      fpNavigator->
    	LocateGlobalPointAndUpdateTouchable(a_spot.GetPosition(),
					    fTouchableHandle(),
					    false);
      fNaviSetup = true;
    }
  else
    {
      fpNavigator->
	LocateGlobalPointAndUpdateTouchable(a_spot.GetPosition(),
					    fTouchableHandle() );
     }

  //--------------------------------------
  // Fill the attributes of the G4Step needed
  // by our sensitive detector:
  //-------------------------------------
#ifdef DEBUG_FAST
            G4cout << "LArHitMaker::make -- ready to fill step."
                   << G4endl;
#endif

  // Get the position and time from the energy spot.
  G4ThreeVector p = a_spot.GetPosition();
  G4double t = a_spot.GetTime();

  // set touchable volume at PreStepPoint:
  fFakePreStepPoint->SetTouchableHandle(fTouchableHandle);
  fFakePreStepPoint->SetPosition(p);
  fFakePreStepPoint->SetGlobalTime(t);
  

  // WGS: Most of the calculators in LArG4 expect a PostStepPoint as
  // well.  For now, try setting this to be the same as the
  // PreStepPoint.

  // AS move post step point a little bit to make Calculator happy
  
  fFakePostStepPoint->SetTouchableHandle(fTouchableHandle);
  fFakePostStepPoint->SetPosition(p);
  fFakePostStepPoint->SetGlobalTime(t);

  // set total energy deposit:
  fFakeStep->SetTotalEnergyDeposit(a_spot.GetEnergy());
  //set very short step length
  fFakeStep->SetStepLength(1e-10);
  //set pre and post step point
  fFakeStep->SetPreStepPoint(fFakePreStepPoint);
  fFakeStep->SetPostStepPoint(fFakePostStepPoint);
  fFakeStep->SetTrack(0);

  //--------------------------------------
  // Produce Hits
  //--------------------------------------

  // First, find out in which physical volume our hit is located.
  G4VPhysicalVolume* pCurrentVolume =
    fFakeStep->GetPreStepPoint()->GetPhysicalVolume();

  // If the volume is valid...
  if ( pCurrentVolume != 0 ) {

    // for the time being create hits only in LAr volumes
    if ( (pCurrentVolume->GetName()).substr(0,fBaseName.length()) == fBaseName ) 
      {

      // Is this volume associated with a sensitive detector?
      G4VSensitiveDetector* pSensitive;
      if (pCurrentVolume->GetLogicalVolume()->GetSensitiveDetector()){
      	pSensitive = pCurrentVolume->GetLogicalVolume()->GetSensitiveDetector();
	}
      else {
	return;
      }
	
	if ( pSensitive != 0 )
	  {
	    // Create the actual hit.
	    pSensitive->Hit(fFakeStep);
	  }
	
	else
	  {
	    // If we reach this point, then the EnergySpot was created
	    // outside of a sensitive region.

	  }
      } 
#ifdef DEBUG_FAST
    else 
      {
	G4cout << "LArHitMaker::make - will not make hit in "
	       << pCurrentVolume->GetName()
	       << " since volume name does not begin with "
	       << fBaseName
	       << G4endl;
      }
#endif
  }
}
