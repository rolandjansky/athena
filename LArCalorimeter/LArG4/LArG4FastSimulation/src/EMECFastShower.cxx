/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/EMECFastShower.h"
#include "LArG4FastSimulation/EndcapFastSimDedicatedSD.h"
#include "G4FastSimulation/FastSimModelProxy.h"
#include "globals.hh"

#include "LArG4FastSimSvc/ILArG4FastSimSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include <stdexcept>

#include "G4VSolid.hh"
#include "G4ios.hh"
#include "geomdefs.hh"

static FastSimModelProxy<EMECFastShower> plugin("EMECFastShower");

EMECFastShower::EMECFastShower(G4String name):
  LArFastShower(name)
{
  std::cout << "Initializing EMECFastShower"
	    << " - package version " << PACKAGE_VERSION << std::endl;

  m_FastSimDedicatedSD = new EndcapFastSimDedicatedSD();
}

ILArG4FastSimSvc* EMECFastShower::fastSimSvc()
{
	  if ( 0 == m_fastSimSvc ) {
	    ISvcLocator* svcLocator = Gaudi::svcLocator();
	    StatusCode sc = svcLocator->service("LArG4EMECFastSimSvc", m_fastSimSvc);
	    if (sc.isFailure()) {
	      throw std::runtime_error("EMECFastShower: cannot retrieve LArG4EMECFastSimSvc");
	    }
	  }

	  return m_fastSimSvc;
}

G4bool EMECFastShower::ForcedAccept(const G4FastTrack & fastTrack)
{
  G4ThreeVector InitialPositionShower = fastTrack.GetPrimaryTrack()->GetPosition();
  if ( !fastSimSvc()->ContainHigh() &&
	  ( InitialPositionShower.eta()>2.9 ||
		InitialPositionShower.eta()<-2.9 ) ) return true;

  if ( !fastSimSvc()->ContainCrack() &&
	  ( ( InitialPositionShower.eta()>2.2 &&
		  InitialPositionShower.eta()<2.8 ) ||
		( InitialPositionShower.eta()>-2.8 &&
		  InitialPositionShower.eta()<-2.2 ) ) ) return true;

  if ( !fastSimSvc()->ContainLow() &&
	  ( InitialPositionShower.eta()<1.8 ||
		InitialPositionShower.eta()>-1.8 ) ) return true;

  return false;
}

G4bool EMECFastShower::ForcedDeny  (const G4FastTrack &)
{
	return false;
}
