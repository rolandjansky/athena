/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHECWheelCalculator.h"
#include "LArG4HEC/IHECGeometry.h"

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
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/Units.h"

#include "globals.hh"
#include <cmath>

namespace Units = Athena::Units;

#undef DEBUG_HITS


LArHECWheelCalculator::LArHECWheelCalculator(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalculatorSvcImp(name, pSvcLocator)
  , m_Geometry("HECGeometry",name) //FIXME LArG4::HEC::HECGeometry
  , m_DetectorManager(nullptr)
  , m_birksLaw(nullptr)
  , m_doHV(false)
{
  declareProperty("GeometryCalculator",m_Geometry);
  declareProperty("HECHVEnable",m_doHV);
}


LArHECWheelCalculator::~LArHECWheelCalculator()
{
  if(m_birksLaw) delete m_birksLaw;
}

StatusCode LArHECWheelCalculator::initialize()
{
  ATH_MSG_DEBUG("Use the LArHECWheelCalculator for the HEC");

  ATH_CHECK(m_Geometry.retrieve());

  if (m_BirksLaw)
    {
      const double Birks_LAr_density = 1.396;
      m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,m_Birksk);
    }

  if(m_doHV)
    {
      const LArDetectorManager *manager=nullptr;
      StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
      ATH_CHECK(detStore->retrieve(manager));
      m_DetectorManager=manager->getHecManager();
    }

  return StatusCode::SUCCESS;
}

G4bool LArHECWheelCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata) const
{

  // make sure hdata is reset
  hdata.resize(1);
  // First, get the energy.
  hdata[0].energy = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();


  // apply BirksLaw if we want to:
  G4double stepLengthCm = a_step->GetStepLength() / Units::cm;
  if(hdata[0].energy <= 0. || stepLengthCm <= 0.)  return false;
  if(m_birksLaw)  hdata[0].energy = (*m_birksLaw)(hdata[0].energy, stepLengthCm, 10.0 /*KeV/cm*/);

  // Find out how long it took the energy to get here.
  G4double timeOfFlight        = 0.5* (  a_step->GetPreStepPoint()->GetGlobalTime()
                                         + a_step->GetPostStepPoint()->GetGlobalTime() );
  G4ThreeVector point          = 0.5* (  a_step->GetPreStepPoint()->GetPosition()
                                         + a_step->GetPostStepPoint()->GetPosition() );

  hdata[0].time = (timeOfFlight - point.mag()/CLHEP::c_light)/Units::ns;

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

StatusCode LArHECWheelCalculator::finalize() {
  if(m_birksLaw) delete m_birksLaw;
  return StatusCode::SUCCESS;
}
