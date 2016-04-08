/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HEC/LArHECLocalCalculator.h"

#include "LArG4HEC/LocalGeometry.h"

#include "LArG4RunControl/LArG4GlobalOptions.h"
#include "LArG4RunControl/LArG4HECOptions.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4AffineTransform.hh"
#include "G4TouchableHistory.hh"

#include "LArG4Code/LArG4BirksLaw.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

#include "globals.hh"
#include <cmath>

#undef DEBUG_HITS

// Standard implementation of a singleton pattern.

LArHECLocalCalculator* LArHECLocalCalculator::m_instance = 0;
G4bool LArHECLocalCalculator::m_isX = false;

LArHECLocalCalculator* LArHECLocalCalculator::GetCalculator()
{
  if (m_instance == 0) 
    {
      m_instance = new LArHECLocalCalculator();
    }
  return m_instance;
}


LArHECLocalCalculator::LArHECLocalCalculator()
   ://m_identifier(),m_time(0),m_energy(0),
      m_isInTime(false),m_birksLaw(NULL)
{
   StoreGateSvc* detStore;
   LArG4GlobalOptions *globalOptions=NULL;
   LArG4HECOptions    *hecOptions=NULL;
   
   ISvcLocator *svcLocator = Gaudi::svcLocator();
   StatusCode status = svcLocator->service("DetectorStore", detStore);
 
   if(status.isSuccess()){
       status = detStore->retrieve(globalOptions, "LArG4GlobalOptions");
       if(status.isFailure()){
          throw std::runtime_error("LArHECLocalCalculator: cannot retrieve LArG4GlobalOptions");
       }
       status = detStore->retrieve(hecOptions, "LArG4HECOptions");
       if(status.isFailure()){
          // throw std::runtime_error("LArHECLocalCalculator: cannot retrieve LArG4HECOptions");
       }
   } else {
          throw std::runtime_error("LArHECLocalCalculator: cannot initialize StoreGate interface");
   }
   status = svcLocator->service("MessageSvc", m_msgSvc);
   if(status.isFailure()) m_msgSvc = 0;
   if(m_msgSvc) {
     MsgStream log(m_msgSvc,"LArHECLocalCalculator");
     log << MSG::INFO << "Constructing Calculator " << endreq;
   }

   m_OOTcut = globalOptions->OutOfTimeCut();

   m_isX = false;

   m_Geometry = LArG4::HEC::LocalGeometry::GetInstance();
   m_Geometry->SetX(m_isX);

   if (hecOptions) {
      if (hecOptions->HECBirksLaw()) {
          const double Birks_LAr_density = 1.396;
          const double Birks_k = hecOptions->HECBirksk();
          m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,Birks_k);
      }
   }
}

LArHECLocalCalculator::~LArHECLocalCalculator()
{
  delete m_birksLaw;
}

void LArHECLocalCalculator::SetX(bool x){ 
   m_Geometry->SetX(x);
   m_isX = x; 
}

G4bool LArHECLocalCalculator::Process(const G4Step* a_step, int depthadd, double deadzone, std::vector<LArHitData>& hdata)
{

  // make sure vector is clear
  hdata.clear();
  // First, get the energy.
  hdata[0].energy = a_step->GetTotalEnergyDeposit();

  // apply BirksLaw if we want to:
  G4double stepLengthCm = a_step->GetStepLength() / CLHEP::cm;
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
					 
  hdata[0].time = timeOfFlight/CLHEP::ns - p.mag()/CLHEP::c_light/CLHEP::ns;
  if (hdata[0].time > m_OOTcut)
    m_isInTime = false;
  else
    m_isInTime = true;

  // Calculate the identifier.
  hdata[0].id = m_Geometry->CalculateIdentifier( a_step, LArG4::HEC::kLocActive, depthadd, deadzone);
//  std::cout<<"LArHECLocalCalculator::Process "<<depthadd<<std::endl;
  return true;
}

