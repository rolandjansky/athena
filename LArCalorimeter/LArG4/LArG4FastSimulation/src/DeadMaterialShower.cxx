/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/DeadMaterialShower.h"
#include "G4FastSimulation/FastSimModelProxy.h"
#include "globals.hh"

#include "G4VSolid.hh"
#include "G4ios.hh"
#include "geomdefs.hh"
#include "LArG4FastSimSvc/ILArG4FastSimSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include <stdexcept>

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"

static FastSimModelProxy<DeadMaterialShower> plugin("DeadMaterialShower");

DeadMaterialShower::DeadMaterialShower(G4String name)
  : FastSimModel(name)
  ,m_fastSimSvc(0)
{
  std::cout << "Initializing DeadMaterialShower"
	    << " - package version " << PACKAGE_VERSION << std::endl;
}

ILArG4FastSimSvc* DeadMaterialShower::fastSimSvc()
{
	  if ( 0 == m_fastSimSvc ) {
	    ISvcLocator* svcLocator = Gaudi::svcLocator();
	    StatusCode sc = svcLocator->service("DeadMaterialFastSimSvc", m_fastSimSvc);
	    if (sc.isFailure()) {
	      throw std::runtime_error("DeadMaterialShower: cannot retrieve DeadMaterialFastSimSvc");
	    }
	  }

	  return m_fastSimSvc;
}

G4bool DeadMaterialShower::IsApplicable(const G4ParticleDefinition& particleType)
{
	  // Apply parameterization only to Electrons, Positrons, and Photons.  ModelTrigger will not be called if this returns false
	  if ( &particleType == G4Electron::ElectronDefinition()				||
	       &particleType == G4Positron::PositronDefinition()				||
	       &particleType == G4PionPlus::PionPlusDefinition()                ||
	       &particleType == G4PionMinus::PionMinusDefinition()              ||
	       &particleType == G4Gamma::GammaDefinition()                      ||
	       &particleType == G4Neutron::NeutronDefinition()
	       )
		return true;
	  else
	  	return false;
}

G4bool DeadMaterialShower::ModelTrigger(const G4FastTrack& fastTrack)
{
	/*
	 * DeadMaterial needs three very special params, which are unique among FS services
	 * We can either introduce the three new params to LArG4FastSimSvc, or to use
	 * the existing ones. Previously we were using some already-existing params,
	 * let's leave it like this for now
	 */
	double lowEnergy  = fastSimSvc()->minEneToShowerLib(*(G4Electron::Definition()));
	double highEnergy = fastSimSvc()->maxEneToShowerLib(*(G4Electron::Definition()));
	double zcutoff    = fastSimSvc()->maxEneToShowerLib(*(G4Gamma::Definition()));
  if ( fabs( fastTrack.GetPrimaryTrack()->GetPosition().z() ) < zcutoff ){
    // In front of the cut - kill e+/- up to low energy
    if ( fastTrack.GetPrimaryTrack()->GetKineticEnergy() < lowEnergy &&
         (fastTrack.GetPrimaryTrack()->GetDefinition() == G4Electron::Definition() ||
          fastTrack.GetPrimaryTrack()->GetDefinition() == G4Positron::Definition() ) ) return true;
    return false;
  }

  // Behind the cut - kill !muons up to high energy
  if ( fastTrack.GetPrimaryTrack()->GetKineticEnergy() > highEnergy ||
       fastTrack.GetPrimaryTrack()->GetDefinition() == G4MuonPlus::Definition() ||
       fastTrack.GetPrimaryTrack()->GetDefinition() == G4MuonMinus::Definition() ) return false;
  return true;
}

void DeadMaterialShower::DoIt(const G4FastTrack&, G4FastStep& fastStep)
{
  fastStep.KillPrimaryTrack();
  fastStep.SetPrimaryTrackPathLength(0.0);
}

