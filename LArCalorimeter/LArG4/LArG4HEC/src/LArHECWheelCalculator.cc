/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HEC/LArHECWheelCalculator.h"
#include "LArG4HEC/HECGeometry.h"

#include "LArG4RunControl/LArG4GlobalOptions.h"
#include "LArG4RunControl/LArG4HECOptions.h"
#include "LArG4RunControl/LArG4HECOptions.h"

 
#include "LArReadoutGeometry/LArDetectorManager.h"
#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVSubgap.h"


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
#include "AthenaKernel/getMessageSvc.h"

#include "globals.hh"
#include <cmath>

#undef DEBUG_HITS

// Standard implementation of a singleton pattern.

LArHECWheelCalculator* LArHECWheelCalculator::m_instance = 0;

LArHECWheelCalculator* LArHECWheelCalculator::GetCalculator()
{
  if (m_instance == 0) 
    {
      m_instance = new LArHECWheelCalculator();
    }
  return m_instance;
}


LArHECWheelCalculator::~LArHECWheelCalculator() {
  delete m_birksLaw;
}

LArHECWheelCalculator::LArHECWheelCalculator()
  :m_msgSvc(0),
   //m_identifier(),m_time(0),m_energy(0),
   m_isInTime(false), m_birksLaw(NULL)
{
   StoreGateSvc* detStore;
   LArG4GlobalOptions *globalOptions=NULL;
   LArG4HECOptions    *hecOptions=NULL;
   
   ISvcLocator *svcLocator = Gaudi::svcLocator();
   StatusCode status = svcLocator->service("DetectorStore", detStore);
 
   if(status.isSuccess()){
     status = detStore->retrieve(globalOptions, "LArG4GlobalOptions");
     if(status.isFailure()){
       throw std::runtime_error("LArHECWheelCalculator: cannot retrieve LArG4GlobalOptions");
     }
     status = detStore->retrieve(hecOptions, "LArG4HECOptions");
     if(status.isFailure()){
       // throw std::runtime_error("LArHECWheelCalculator: cannot retrieve LArG4HECOptions");
     }
   } else {
     throw std::runtime_error("LArHECWheelCalculator: cannot initialize StoreGate interface");
   }

   MsgStream log(Athena::getMessageSvc(),"LArHECWheelCalculator" );
   log << MSG::INFO << "Use the LArHECWheelCalculator for the HEC" << endreq;

   m_OOTcut = globalOptions->OutOfTimeCut();

   m_Geometry = LArG4::HEC::HECGeometry::GetInstance();
   m_DetectorManager=NULL;

   if (hecOptions) {
     if (hecOptions->HECBirksLaw()) {
       const double Birks_LAr_density = 1.396;
       const double Birks_k = hecOptions->HECBirksk();
       m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,Birks_k);

       if (hecOptions->HECHVEnable()) {
	 const LArDetectorManager *manager=NULL;
	 if (detStore->retrieve(manager)!=StatusCode::SUCCESS) {
	   throw std::runtime_error("Cannot locate HEC Manager");
	 }
	 else {
	   m_DetectorManager=manager->getHecManager();
	 }
       }
     }
   }

}

G4bool LArHECWheelCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata)
{

  // make sure hdata is reset
  hdata.resize(1);
  // First, get the energy.
  hdata[0].energy = a_step->GetTotalEnergyDeposit();
  

  // apply BirksLaw if we want to:
  G4double stepLengthCm = a_step->GetStepLength() / CLHEP::cm;
  if(hdata[0].energy <= 0. || stepLengthCm <= 0.)  return false;
  if(m_birksLaw)  hdata[0].energy = (*m_birksLaw)(hdata[0].energy, stepLengthCm, 10.0 /*KeV/cm*/);

  // Find out how long it took the energy to get here.
  G4double timeOfFlight        = 0.5* (  a_step->GetPreStepPoint()->GetGlobalTime()
				       + a_step->GetPostStepPoint()->GetGlobalTime() );
  G4ThreeVector point          = 0.5* (  a_step->GetPreStepPoint()->GetPosition()
				       + a_step->GetPostStepPoint()->GetPosition() );

  hdata[0].time = timeOfFlight/CLHEP::ns - point.mag()/CLHEP::c_light/CLHEP::ns;

  if (hdata[0].time > m_OOTcut)
    m_isInTime = false;
  else
    m_isInTime = true;

  // Calculate the identifier.
  int subgapIndex=0;
  hdata[0].id = m_Geometry->CalculateIdentifier( a_step, LArG4::HEC::kWheelActive,&subgapIndex);

  //  int detector = m_identifier[0];
  //  int subdet   = m_identifier[1];
  int zSide    = hdata[0].id[2];
  int sampling = hdata[0].id[3];
  int region   = hdata[0].id[4];
  int eta      = hdata[0].id[5];
  int phi      = hdata[0].id[6];

  

  if (m_DetectorManager) {
    const HECDetectorRegion *hecRegion=m_DetectorManager->getDetectorRegion(zSide<0? 0: 1, sampling, region);
    HECCellConstLink cell=hecRegion->getHECCell(eta, phi);
    HECHVSubgapConstLink subgap = cell->getSubgap(subgapIndex);
    hdata[0].energy *= (pow(subgap->voltage()/1800.0,0.6));
  }

  return true;
}

