/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHECLocalCalculator.h"

#include "LArG4HEC/LocalGeometry.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4AffineTransform.hh"
#include "G4TouchableHistory.hh"

#include "LArG4Code/LArG4BirksLaw.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/Units.h"

#include "globals.hh"
#include <cmath>

namespace Units = Athena::Units;

#undef DEBUG_HITS



LArHECLocalCalculator::LArHECLocalCalculator(const std::string& name,  ISvcLocator *pSvcLocator)
  : LArCalculatorSvcImp(name, pSvcLocator)
  , m_Geometry("LocalHECGeometry", name)
  , m_birksLaw(nullptr)
  , m_isX(false)
{
  declareProperty("GeometryCalculator", m_Geometry);
  declareProperty("IsX", m_isX);
}

StatusCode LArHECLocalCalculator::initialize()
{
   ISvcLocator *svcLocator = Gaudi::svcLocator();
   StoreGateSvc* detStore(nullptr);
   ATH_CHECK(svcLocator->service("DetectorStore", detStore));
   ATH_MSG_DEBUG("Constructing LArHECLocalCalculator");

   ATH_CHECK(m_Geometry.retrieve());

   if (m_BirksLaw)
     {
       const double Birks_LAr_density = 1.396;
       m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,m_Birksk);
     }

   return StatusCode::SUCCESS;
}

StatusCode LArHECLocalCalculator::finalize()
{
  if(m_birksLaw) delete m_birksLaw;
  return StatusCode::SUCCESS;
}

G4bool LArHECLocalCalculator::Process(const G4Step* a_step, int depthadd, double deadzone, std::vector<LArHitData>& hdata) const
{

  // make sure vector is clear
  hdata.clear();
  // First, get the energy.
  hdata[0].energy = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();

  // apply BirksLaw if we want to:
  G4double stepLengthCm = a_step->GetStepLength() / Units::cm;
  if(hdata[0].energy <= 0. || stepLengthCm <= 0.)  return false;
  if(m_birksLaw)  hdata[0].energy = (*m_birksLaw)(hdata[0].energy, stepLengthCm, 10.0 /*KeV/cm*/);


  // Find out how long it took the energy to get here.
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4double timeOfFlight = (pre_step_point->GetGlobalTime() +
                           post_step_point->GetGlobalTime()) * 0.5;
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;

  hdata[0].time = (timeOfFlight - p.mag()/CLHEP::c_light)/Units::ns;

  // Calculate the identifier.
  hdata[0].id = m_Geometry->CalculateIdentifier( a_step, LArG4::HEC::kLocActive, depthadd, deadzone);
//  std::cout<<"LArHECLocalCalculator::Process "<<depthadd<<std::endl;
  return true;
}
