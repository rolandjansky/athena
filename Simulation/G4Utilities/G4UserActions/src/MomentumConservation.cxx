/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/MomentumConservation.h"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
//#include "G4FourVector.hh"
#include <iostream>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace G4UA
{

  void MomentumConservation::EndOfEventAction(const G4Event* anEvent)
  {

    // Energy conservation:

    // Get energy of primaries
    double eprim = 0;
    //G4FourVector p4prim;
    for (int iv = 0; iv < anEvent->GetNumberOfPrimaryVertex(); ++iv) {
      const G4PrimaryVertex* v = anEvent->GetPrimaryVertex(iv);
      for (int ip = 0; ip < v->GetNumberOfParticle(); ++ip) {
        const G4PrimaryParticle* p = v->GetPrimary(ip);
        /// @todo Does G4PrimaryParticle really have no Get(Total/Kinetic)Energy methods?
        /// @todo Do we need to add up the total energy at the start?
        eprim += p->GetMomentum().mag(); //< Just KE
      }
    }

    // Get energy from neutrinos (which aren't stacked)
    double eneut = 0;
    //vector<G4Track*> tracks = theTruthManager->GetSecondaries();

    // Energy conservation check
    const double efinal = _sum_edep + _sum_eesc + eneut;
    ATH_MSG_INFO( "Energy conservation check: "
          << "initial = " << eprim/CLHEP::GeV << " GeV, "
          << "final = " << efinal/CLHEP::GeV << " GeV" );


    // Momentum conservation:
    /// @todo Everything! Use truth strategy as for neutrinos

    // Reset for next event
    _sum_edep = 0;
    _sum_eesc = 0;

  }

  void MomentumConservation::UserSteppingAction(const G4Step* aStep){

    if (aStep->GetPostStepPoint()->GetPhysicalVolume() != 0) {
      const double edep = aStep->GetTotalEnergyDeposit();
      _sum_edep += edep;
    } else {
      /// @todo Should this be GetKineticEnergy?
      const double eesc = aStep->GetPostStepPoint()->GetTotalEnergy();
      _sum_eesc += eesc;
    }

  }

} // namespace G4UA
