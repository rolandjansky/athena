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

#include "LArG4EC/LArEndcapPresamplerCalculator.h"
#include "LArG4EC/PresamplerGeometry.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "LArG4Code/LArG4BirksLaw.h"
#include "LArG4RunControl/LArG4EMECOptions.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "LArG4RunControl/LArG4EMECOptions.h"  
#include "LArG4RunControl/LArG4GlobalOptions.h"  
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "globals.hh"
// 03-Jan-2002 WGS: For 'copysign'.
#include <cmath>
#include <climits>

// Standard implementation of a singleton pattern.

LArEndcapPresamplerCalculator* LArEndcapPresamplerCalculator::GetCalculator()
{
  static LArEndcapPresamplerCalculator instance;
  return &instance;
}


LArEndcapPresamplerCalculator::LArEndcapPresamplerCalculator() :
  birksLaw(NULL)
{
  // Constructor initializes the geometry.

  // Make sure we don't have any undefined values.
  //m_identifier = LArG4Identifier();

  //m_time = 0.;
  //m_energy = 0.;
  m_isInTime = false;

  StoreGateSvc* detStore;
  LArG4EMECOptions   *emecOptions;
  LArG4GlobalOptions *globalOptions;
  StatusCode status;
  ISvcLocator* svcLocator = Gaudi::svcLocator(); 
  status = svcLocator->service("DetectorStore", detStore);
  
  if(status.isSuccess()){
    status = detStore->retrieve(emecOptions, "LArG4EMECOptions");
    if(status.isFailure()){
      throw std::runtime_error("LArWheelEnergyCalculator: cannot retrieve \
LArG4EMECOptions");
    }
    status = detStore->retrieve(globalOptions, "LArG4GlobalOptions");
    if(status.isFailure()){
      throw std::runtime_error("LArWheelEnergyCalculator: cannot retrieve \
LArG4GlobalOptions");
    }
  } else {
    throw std::runtime_error("LArWheelEnergyCalculator: cannot initialize \
StoreGate interface");
  }
  


  // Get the values from the detector parameters routine.
  m_OOTcut = globalOptions->OutOfTimeCut();

  if(emecOptions->EMECBirksLaw()){
          const double Birks_LAr_density = 1.396;
          const double Birks_k = emecOptions->EMECBirksk();
          birksLaw = new LArG4BirksLaw(Birks_LAr_density,Birks_k);
  }


   if(birksLaw) {
     std::cout << " LArEndcapPresamplerCalculator: Birks' law ON " << std::endl;
     std::cout << " LArEndcapPresamplerCalculator:   parameter k    " << birksLaw->k() << std::endl;
   }
   else
     std::cout << " LArEndcapPresamplerCalculator: Birks' law OFF" << std::endl;


  // Get the geometry routine.
  m_geometry = LArG4::EC::PresamplerGeometry::GetInstance();
}


LArEndcapPresamplerCalculator::~LArEndcapPresamplerCalculator() {
  if (birksLaw) delete birksLaw;
}

G4bool LArEndcapPresamplerCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata)
{
  // make sure hdata is reset
  hdata.resize(1);
  // Given a G4Step, find the identifier in the LAr EMEC associated
  // with that point.

  // 29-Mar-2002 WGS: this method now returns a boolean.  If it's
  // true, the hit is valid; if it's false, there was some problem
  // with the hit and it should be ignored.

  double energy = a_step->GetTotalEnergyDeposit();
  if (birksLaw) {
       G4double wholeStepLengthCm = a_step->GetStepLength() / CLHEP::cm;
       G4double efield = 10.;  // 10 kV/cm simple approximation of electric field
       energy = (*birksLaw)(energy, wholeStepLengthCm,efield);
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
  hdata[0].time = timeOfFlight/CLHEP::ns - p.mag()/CLHEP::c_light/CLHEP::ns;
  if (hdata[0].time > m_OOTcut)
    m_isInTime = false;
  else
    m_isInTime = true;

  // Use the geometry routine to determine the identifier.
  hdata[0].id = m_geometry->CalculateIdentifier( a_step );

  if ( hdata[0].id == LArG4Identifier() )
    return false;
  else
    return true;
}
