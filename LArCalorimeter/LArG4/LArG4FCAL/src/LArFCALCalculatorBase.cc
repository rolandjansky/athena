/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// Base class describing common behaviour in FCal G4 calculator.
//
//----------------------------------------------------------------------------

#include "LArFCALCalculatorBase.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArG4BirksLaw.h"
#include "StoreGate/StoreGate.h"
//#include "LArG4RunControl/LArG4FCALOptions.h"
// Geant4 includes
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "globals.hh"

#include "LArReadoutGeometry/LArDetectorManager.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/FCALTube.h"
#include "AthenaKernel/Units.h"
namespace Units = Athena::Units;

//
// constructor
//
LArFCALCalculatorBase::LArFCALCalculatorBase(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalculatorSvcImp(name, pSvcLocator)
  , m_doHV(false)
  , m_ChannelMap(nullptr)
  , m_posModule(nullptr)
  , m_negModule(nullptr)
  , m_FCalSampling(0)
  , m_birksLaw(nullptr)
{
  declareProperty("FCALHVEnable",m_doHV);
  declareProperty("FCALSampling",m_FCalSampling);
  //m_FCalSampling.verifier().setUpper(3); //Would need to make m_FCalSampling an IntegerProperty for this to work. Overkill?
}

StatusCode LArFCALCalculatorBase::initialize()
{
  StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
  if (detStore->retrieve(m_ChannelMap)==StatusCode::FAILURE) {
  }

  if (m_BirksLaw) {
    const double Birks_LAr_density = 1.396;
    m_birksLaw = new LArG4BirksLaw(Birks_LAr_density,m_Birksk);
  }
  if(m_doHV) {

    const LArDetectorManager *manager=nullptr;
    if (detStore->retrieve(manager)!=StatusCode::SUCCESS) {
      throw std::runtime_error("Cannot locate FCAL Manager");
    }
    else {
      const FCALDetectorManager* fcalManager=manager->getFcalManager();
      m_posModule = fcalManager->getFCAL(FCALModule::Module(m_FCalSampling),FCALModule::POS);
      m_negModule = fcalManager->getFCAL(FCALModule::Module(m_FCalSampling),FCALModule::NEG);
    }
  }
  return StatusCode::SUCCESS;
}

//
// destructor
//
StatusCode LArFCALCalculatorBase::finalize()
{
  if(m_birksLaw) delete m_birksLaw;
  return StatusCode::SUCCESS;
}

G4bool LArFCALCalculatorBase::Process(const G4Step* a_step, std::vector<LArHitData>& hdata) const
{
  // Given a G4Step, determine the cell identifier.

  // 29-Mar-2002 WGS: this method now returns a boolean.  If it's
  // true, the hit is valid; if it's false, there was some problem
  // with the hit and it should be ignored.

  // make sure hdata is reset
  hdata.resize(1);
  // First, get the energy.
  hdata[0].energy = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();

  G4double stepLengthCm = a_step->GetStepLength() / Units::cm;
  if(m_birksLaw)  hdata[0].energy = (*m_birksLaw)(hdata[0].energy, stepLengthCm, 10);

  // Find out how long it took the energy to get here.
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4double timeOfFlight = (pre_step_point->GetGlobalTime() +
                           post_step_point->GetGlobalTime()) * 0.5;
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;


  // Determine if the hit was in-time.
  hdata[0].time = (timeOfFlight - p.mag()/CLHEP::c_light)/Units::ns;

  // zSide is negative if z<0.
  G4int zSide = 2;
  if (p.z() < 0.) zSide = -zSide;


  // Get the physical volume associated with this G4Step.
  G4VPhysicalVolume* thisPV = a_step->GetPreStepPoint()->GetPhysicalVolume();   //a_step->GetTrack()->GetVolume();
  G4int sampling = m_FCalSampling;
  FCAL_ChannelMap::tubemap_const_iterator t= m_ChannelMap->getTubeByCopyNumber(sampling, thisPV->GetCopyNo());
  FCAL_ChannelMap::tileName_t tileName = (*t).second.get_tileName();

  unsigned int phiIndex =  tileName & 0x0000FFFF;
  unsigned int etaIndex = (tileName & 0xFFFF0000) >> 16;

  // Apply high voltage here:
  {
    const FCALModule *fcalMod = zSide<0 ? m_negModule : m_posModule;
    if (fcalMod) { // Then apply high voltage:

      bool tubeFound(false);

      const FCALTile *tile = fcalMod->getTile(phiIndex, etaIndex);
      for (unsigned int i=0;i<tile->getNumTubes();i++) {
        FCALTubeConstLink tube=tile->getTube(i);
        if (tube->getXLocal() == (*t).second.x() && tube->getYLocal()==(*t).second.y()) {
          FCALHVLineConstLink line =tube->getHVLine();
          if (line) {
            double voltage = line->voltage();
            //double current = line->current();
            bool   hvOn    = line->hvOn();
            if (!hvOn) hdata[0].energy=0.0;
            hdata[0].energy *= pow((voltage)/2000.0,0.6);
            tubeFound=true;
            break;
          }
        }
      }
      if (!tubeFound)   throw std::runtime_error("FCAL Tube not found (for HV calculation)");

    }

  }
  hdata[0].id.clear();

  // Append the values to the empty identifier.
  hdata[0].id  << 4          // LArCalorimeter
               << 3          // LArFCAL
               << zSide      // EndCap
               << sampling   // FCal Module #
               << etaIndex
               << phiIndex;

  return true;
}
