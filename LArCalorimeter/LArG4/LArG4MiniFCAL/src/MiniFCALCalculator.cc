/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MiniFCALCalculator.h"
#include "LArG4MiniFCAL/MiniFCALAssignIdentifier.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4AffineTransform.hh"
#include "G4TouchableHistory.hh"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/Units.h"

#include "globals.hh"
#include <cmath>

namespace Units = Athena::Units;

#undef DEBUG_HITS

MiniFCALCalculator::MiniFCALCalculator(const std::string& name, ISvcLocator *pSvcLocator):LArCalculatorSvcImp(name, pSvcLocator)
{
}

StatusCode MiniFCALCalculator::initialize()
{

   ATH_MSG_INFO("Use the MiniFCALCalculator for the MiniFCAL");

   m_Geometry = LArG4::MiniFCAL::MiniFCALAssignIdentifier::GetInstance();
   return StatusCode::SUCCESS;
}


G4bool MiniFCALCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata) const
{
  // make sure hdata is reset
  hdata.resize(1);

  // First, get the energy.
  hdata[0].energy = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();

  if(hdata[0].energy <= 0.)  return false;

  // Find out how long it took the energy to get here.
  G4double timeOfFlight = 0.5* (  a_step->GetPreStepPoint()->GetGlobalTime()
                                + a_step->GetPostStepPoint()->GetGlobalTime() );
  G4ThreeVector point   = 0.5* (  a_step->GetPreStepPoint()->GetPosition()
                                + a_step->GetPostStepPoint()->GetPosition() );

  hdata[0].time = (timeOfFlight - point.mag()/CLHEP::c_light)/Units::ns;

  // Calculate the identifier.
  hdata[0].id = m_Geometry->CalculateIdentifier( a_step, LArG4::MiniFCAL::kActive);

  return true;
}
