/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/FCAL2FastShower.h"
#include "LArG4FastSimulation/FCALFastSimDedicatedSD.h"
#include "G4FastSimulation/FastSimModelProxy.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "globals.hh"

#include "LArG4FastSimSvc/ILArG4FastSimSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include <stdexcept>

#include "G4VSolid.hh"
#include "G4ios.hh"
#include "geomdefs.hh"

static FastSimModelProxy<FCAL2FastShower> plugin("FCAL2FastShower");

FCAL2FastShower::FCAL2FastShower(G4String name):
  LArFastShower(name)
{
  std::cout << "Initializing " << GetName()
	    << " - package version " << PACKAGE_VERSION << std::endl;

  // there can be more than one FCal type FasShower class, but only one FCalFastSimDedicated SD!
  FADS::SensitiveDetectorCatalog *sdc=FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
  m_FastSimDedicatedSD = dynamic_cast<IFastSimDedicatedSD*>(sdc->GetSensitiveDetector("FCALFastSimDedicated"));
  if (m_FastSimDedicatedSD == 0) {
    m_FastSimDedicatedSD = new FCALFastSimDedicatedSD();
  }
}

ILArG4FastSimSvc* FCAL2FastShower::fastSimSvc()
{
	  if ( 0 == m_fastSimSvc ) {
	    ISvcLocator* svcLocator = Gaudi::svcLocator();
	    StatusCode sc = svcLocator->service("LArG4FCAL2FastSimSvc", m_fastSimSvc);
	    if (sc.isFailure()) {
	      throw std::runtime_error("FCAL2FastShower: cannot retrieve LArG4FCAL2FastSimSvc");
	    }
	  }

	  return m_fastSimSvc;
}

G4bool FCAL2FastShower::ForcedAccept(const G4FastTrack & fastTrack)
{
  G4ThreeVector InitialPositionShower = fastTrack.GetPrimaryTrack()->GetPosition();
  // Test for the desire to skip containment at the edges
  if ( !fastSimSvc()->ContainHigh() &&
	  ( InitialPositionShower.eta()>4.4 ||
		InitialPositionShower.eta()<-4.4 ) ) return true;

  if ( !fastSimSvc()->ContainLow() &&
	  ( InitialPositionShower.eta()<3.8 ||
		InitialPositionShower.eta()>-3.8 ) ) return true;

  return false;
}

G4bool FCAL2FastShower::ForcedDeny  (const G4FastTrack &)
{
	return false;
}
