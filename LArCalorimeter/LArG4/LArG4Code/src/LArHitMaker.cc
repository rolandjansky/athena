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

  m_fakeStep          = new G4Step();
  m_fakePreStepPoint  = m_fakeStep->GetPreStepPoint();
  m_fakePostStepPoint = m_fakeStep->GetPostStepPoint();
  m_touchableHandle   = new G4TouchableHistory();
  m_pNavigator        = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  m_naviSetup         = false;
  m_baseName          = "LAr";
}


LArHitMaker::~LArHitMaker()
{
  // Cleanup pointers.
  delete m_fakeStep;
  delete m_pNavigator;
}


void LArHitMaker::make(const EnergySpot& a_spot)
{
  // ------------------- 
  // Locate the spot.  Use m_naviSetup to test whether we've
  // initialized the G4Navigator _after_ all the G4 geometry
  // construction routines have executed.
  // -------------------
#ifdef DEBUG_FAST
            G4cout << "LArHitMaker::make -- Begin Class."
                   << G4endl;
#endif

  if (!m_naviSetup)
    {
      m_pNavigator->
    	LocateGlobalPointAndUpdateTouchable(a_spot.GetPosition(),
					    m_touchableHandle(),
					    false);
      m_naviSetup = true;
    }
  else
    {
      m_pNavigator->
	LocateGlobalPointAndUpdateTouchable(a_spot.GetPosition(),
					    m_touchableHandle() );
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
  m_fakePreStepPoint->SetTouchableHandle(m_touchableHandle);
  m_fakePreStepPoint->SetPosition(p);
  m_fakePreStepPoint->SetGlobalTime(t);
  

  // WGS: Most of the calculators in LArG4 expect a PostStepPoint as
  // well.  For now, try setting this to be the same as the
  // PreStepPoint.

  // AS move post step point a little bit to make Calculator happy
  
  m_fakePostStepPoint->SetTouchableHandle(m_touchableHandle);
  m_fakePostStepPoint->SetPosition(p);
  m_fakePostStepPoint->SetGlobalTime(t);

  // set total energy deposit:
  m_fakeStep->SetTotalEnergyDeposit(a_spot.GetEnergy());
  //set very short step length
  m_fakeStep->SetStepLength(1e-10);
  //set pre and post step point
  m_fakeStep->SetPreStepPoint(m_fakePreStepPoint);
  m_fakeStep->SetPostStepPoint(m_fakePostStepPoint);
  m_fakeStep->SetTrack(0);

  //--------------------------------------
  // Produce Hits
  //--------------------------------------

  // First, find out in which physical volume our hit is located.
  G4VPhysicalVolume* pCurrentVolume =
    m_fakeStep->GetPreStepPoint()->GetPhysicalVolume();

  // If the volume is valid...
  if ( pCurrentVolume != 0 ) {

    // for the time being create hits only in LAr volumes
    if ( (pCurrentVolume->GetName()).substr(0,m_baseName.length()) == m_baseName ) 
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
	    pSensitive->Hit(m_fakeStep);
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
	       << m_baseName
	       << G4endl;
      }
#endif
  }
}
