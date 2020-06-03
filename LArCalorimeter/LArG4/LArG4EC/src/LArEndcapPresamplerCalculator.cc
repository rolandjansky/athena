/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArEndcapPresamplerCalculator
// 27-Dec-2002 Bill Seligman

// This singleton class provides detector-description information and
// calculations for the Geant4 simulation.

// 2-July-2003 Mikhail Leltchouk: local coordinates for determination
// of etaBin, phiBin at any Endcap Presamplerposition.

// 17-Aug-2004 WGS: Use a "common geometry" routine for both the
// standard calculator and the calibration hits.

#include "LArEndcapPresamplerCalculator.h"
#include "LArG4EC/IECPresamplerGeometry.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "LArG4Code/LArG4BirksLaw.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "globals.hh"
#include "AthenaKernel/Units.h"
// 03-Jan-2002 WGS: For 'copysign'.
#include <cmath>
#include <climits>

namespace Units = Athena::Units;

LArEndcapPresamplerCalculator::LArEndcapPresamplerCalculator(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalculatorSvcImp(name, pSvcLocator)
  , m_geometry("EMECPresamplerGeometry", name) // LArG4::EC::PresamplerGeometry
  , m_birksLaw(nullptr)
{
  declareProperty("GeometryCalculator", m_geometry);
}

StatusCode LArEndcapPresamplerCalculator::initialize()
{

  if(m_BirksLaw)
    {
      const double Birks_LAr_density = 1.396;
      m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,m_Birksk);
      ATH_MSG_INFO(" LArEndcapPresamplerCalculator: Birks' law ON ");
      ATH_MSG_INFO(" LArEndcapPresamplerCalculator:   parameter k    " << m_birksLaw->k());
    }
  else
    {
      ATH_MSG_INFO(" LArEndcapPresamplerCalculator: Birks' law OFF");
    }

  // Get the geometry routine.
  ATH_CHECK(m_geometry.retrieve());

  return StatusCode::SUCCESS;
}


StatusCode LArEndcapPresamplerCalculator::finalize()
{
  if (m_birksLaw) delete m_birksLaw;
  return StatusCode::SUCCESS;
}

G4bool LArEndcapPresamplerCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata) const
{
  // make sure hdata is reset
  hdata.resize(1);
  // Given a G4Step, find the identifier in the LAr EMEC associated
  // with that point.

  // 29-Mar-2002 WGS: this method now returns a boolean.  If it's
  // true, the hit is valid; if it's false, there was some problem
  // with the hit and it should be ignored.

  double energy = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();
  if (m_birksLaw) {
       G4double wholeStepLengthCm = a_step->GetStepLength() / Units::cm;
       G4double efield = 10.;  // 10 kV/cm simple approximation of electric field
       energy = (*m_birksLaw)(energy, wholeStepLengthCm,efield);
  }


  // First, get the energy.
  hdata[0].energy = energy;

  // Find out how long it took the energy to get here.
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4double timeOfFlight = (pre_step_point->GetGlobalTime() +
                           post_step_point->GetGlobalTime()) * 0.5;
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;

  // Determine if the hit was in-time.
  hdata[0].time = timeOfFlight/Units::ns - p.mag()/CLHEP::c_light/Units::ns;

  // Use the geometry routine to determine the identifier.
  hdata[0].id = m_geometry->CalculateIdentifier( a_step );

  if ( hdata[0].id == LArG4Identifier() )
    return false;
  else
    return true;
}
