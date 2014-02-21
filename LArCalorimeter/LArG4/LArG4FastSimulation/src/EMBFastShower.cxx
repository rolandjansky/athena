/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/EMBFastShower.h"
#include "LArG4FastSimulation/BarrelFastSimDedicatedSD.h"
#include "G4FastSimulation/FastSimModelProxy.h"
#include "globals.hh"

#include "LArG4FastSimSvc/ILArG4FastSimSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include <stdexcept>

#include "G4VSolid.hh"
#include "G4ios.hh"
#include "geomdefs.hh"

static FastSimModelProxy<EMBFastShower> plugin("EMBFastShower");

EMBFastShower::EMBFastShower(G4String name):
  LArFastShower(name)
{
  std::cout << "Initializing EMBFastShower"
	    << " - package version " << PACKAGE_VERSION << std::endl;

  m_FastSimDedicatedSD = new BarrelFastSimDedicatedSD();
}

ILArG4FastSimSvc* EMBFastShower::fastSimSvc()
{
	  if ( 0 == m_fastSimSvc ) {
	    ISvcLocator* svcLocator = Gaudi::svcLocator();
	    StatusCode sc = svcLocator->service("LArG4EMBFastSimSvc", m_fastSimSvc);
	    if (sc.isFailure()) {
	      throw std::runtime_error("EMBFastShower: cannot retrieve LArG4EMBFastSimSvc");
	    }
	  }

	  return m_fastSimSvc;
}

G4bool EMBFastShower::ForcedAccept(const G4FastTrack & fastTrack)
{
  G4ThreeVector InitialPositionShower = fastTrack.GetPrimaryTrack()->GetPosition();

  if ( !fastSimSvc()->ContainHigh() &&
      ( InitialPositionShower.eta()>1.1 ||
        InitialPositionShower.eta()<-1.1 ) ) return true;

  if ( !fastSimSvc()->ContainCrack() &&
      ( ( InitialPositionShower.eta()>0.5 &&
          InitialPositionShower.eta()<1.1 ) ||
        ( InitialPositionShower.eta()<-0.5 && 
          InitialPositionShower.eta()>-1.1 ) ) ) return true;

  if ( !fastSimSvc()->ContainLow() &&
      ( InitialPositionShower.eta()<0.3 ||
        InitialPositionShower.eta()>-0.3 ) ) return true;
  return false;
}

G4bool EMBFastShower::ForcedDeny  (const G4FastTrack &)
{
  return false;
}
