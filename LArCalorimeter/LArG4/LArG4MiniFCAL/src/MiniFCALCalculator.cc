/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4MiniFCAL/MiniFCALCalculator.h"
#include "LArG4MiniFCAL/MiniFCALAssignIdentifier.h"

#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4AffineTransform.hh"
#include "G4TouchableHistory.hh"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "globals.hh"
#include <cmath>

#undef DEBUG_HITS

// Standard implementation of a singleton pattern.

MiniFCALCalculator* MiniFCALCalculator::m_instance = 0;

MiniFCALCalculator* MiniFCALCalculator::GetCalculator()
{
  if (m_instance == 0) 
    {
      m_instance = new MiniFCALCalculator();
    }
  return m_instance;
}


//MiniFCALCalculator::~MiniFCALCalculator() {}

MiniFCALCalculator::MiniFCALCalculator()
  :m_msgSvc(0), m_identifier(),m_time(0),m_energy(0),m_isInTime(false)
{
   StoreGateSvc* detStore;
   LArG4GlobalOptions *globalOptions=NULL;
   
   ISvcLocator *svcLocator = Gaudi::svcLocator();
   StatusCode status = svcLocator->service("DetectorStore", detStore);
 
   if(status.isSuccess()){
     status = detStore->retrieve(globalOptions, "LArG4GlobalOptions");
     if(status.isFailure()){
       throw std::runtime_error("MiniFCALCalculator: cannot retrieve LArG4GlobalOptions");
     }
   } 
   else {
     throw std::runtime_error("MiniFCALCalculator: cannot initialize StoreGate interface");
   }

   MsgStream log(Athena::getMessageSvc(),"MiniFCALCalculator" );
   log << MSG::INFO << "Use the MiniFCALCalculator for the MiniFCAL" << endreq;

   m_OOTcut = globalOptions->OutOfTimeCut();

   m_Geometry = LArG4::MiniFCAL::MiniFCALAssignIdentifier::GetInstance();

}


G4bool MiniFCALCalculator::Process(const G4Step* a_step)
{

  // First, get the energy.
  m_energy = a_step->GetTotalEnergyDeposit();

  if(m_energy <= 0.)  return false;

  // Find out how long it took the energy to get here.
  G4double timeOfFlight        = 0.5* (  a_step->GetPreStepPoint()->GetGlobalTime()
				       + a_step->GetPostStepPoint()->GetGlobalTime() );
  G4ThreeVector point          = 0.5* (  a_step->GetPreStepPoint()->GetPosition()
				       + a_step->GetPostStepPoint()->GetPosition() );

  m_time = timeOfFlight/CLHEP::ns - point.mag()/CLHEP::c_light/CLHEP::ns;

  if (m_time > m_OOTcut)
    m_isInTime = false;
  else
    m_isInTime = true;

  // Calculate the identifier.
  m_identifier = m_Geometry->CalculateIdentifier( a_step, LArG4::MiniFCAL::kActive);


  return true;
}

