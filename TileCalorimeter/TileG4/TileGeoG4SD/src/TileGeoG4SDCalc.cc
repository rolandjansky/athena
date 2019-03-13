/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
// Filename : TileGeoG4SDCalc.cc
// Author   : Sergey Karpov <Sergey.Karpov@cern.ch>
// Created  : July, 2013
//
// DESCRIPTION:
//   Sensitive Detector initialisation for TileCal G4 simulations
//   of both ordinary Hits and CalibHits
//
// HISTORY:
//   Nov 2013 - Work with U-shape was added (Sasha Solodkov)
//
//************************************************************

//class header
#include "TileGeoG4SDCalc.hh"
//package headers
#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"
//Athena headers
#include "CxxUtils/make_unique.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileGeoModel/TileDetectorTool.h"
//Gaudi headers
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
//CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"
//Geant4 headers
#include "G4Geantino.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SubtractionSolid.hh"
#include "G4TouchableHistory.hh"
#include "G4Trd.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
//STL headers
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>


namespace
{
  thread_local std::unique_ptr<TileGeoG4LookupBuilder> s_lookup(nullptr);
}

static const double tanPi64 = 0.049126849769467254105343321271314; //FIXME!!!!!

TileGeoG4SDCalc::TileGeoG4SDCalc(const std::string& name, ISvcLocator *pSvcLocator)
  : AthService(name, pSvcLocator)
  , m_detStore("DetectorStore",name)
  , m_geoModSvc("GeoModelSvc",name)
  , m_keepHitTime(false)
{
  declareProperty( "DetectorStore", m_detStore );
  declareProperty( "GeoModelSvc", m_geoModSvc );
  declareProperty( "DeltaTHit" , m_options.deltaTHit );
  declareProperty( "TimeCut" , m_options.timeCut );
  declareProperty( "TileTB" , m_options.tileTB );
  declareProperty( "Ushape" , m_options.uShape );
  declareProperty( "DoBirk" , m_options.doBirk );
  declareProperty( "DoTileRow" , m_options.doTileRow );
  declareProperty( "DoTOFCorrection" , m_options.doTOFCorrection );

  declareProperty("PlateToCell", m_options.plateToCell);
  declareProperty("DoCalibHitParticleID", m_options.doCalibHitParticleID);
  declareProperty("RDBAccessSvcName", m_options.rDBAccessSvcName);
  declareProperty("GeoModelSvcName", m_options.geoModelSvcName);

  declareProperty("VerboseLevel", m_options.verboseLevel);
}

TileGeoG4SDCalc::~TileGeoG4SDCalc() {
}

StatusCode TileGeoG4SDCalc::initialize() {

  ATH_CHECK(m_detStore.retrieve());
  ATH_MSG_DEBUG("DetectorStore Svc initialized.");

  // Retrieve TileID helper from DetectorStore
  ATH_CHECK(m_detStore->retrieve(m_tileID));
  ATH_MSG_VERBOSE("TileID helper retrieved");

  // Retrieve TileTBID helper from DetectorStore
  ATH_CHECK(m_detStore->retrieve(m_tileTBID));
  ATH_MSG_VERBOSE("TileTBID helper retrieved");

  ATH_CHECK(m_geoModSvc.retrieve());
  ATH_MSG_VERBOSE("GeoModelSvc initialized.");

  if (msgLvl(MSG::VERBOSE))   { m_options.verboseLevel = std::max(10,m_options.verboseLevel); }
  else if(msgLvl(MSG::DEBUG)) { m_options.verboseLevel = std::max( 5,m_options.verboseLevel); }

  const int uShapeFromGM = this->getUshapeFromGM();

  // Unpack TileG4SimOptions from DetectorStore

  // Determine time window for hit to be recorded
  double deltaT = m_options.deltaTHit[m_options.deltaTHit.size() - 1] * CLHEP::ns;

  // Determine variable time window for hit to be recorded
  for (int i = m_options.deltaTHit.size() - 1; i > -1; --i) {
    m_options.deltaTHit[i] *= CLHEP::ns;
  }
  // protection agaist wrong vector size
  if (m_options.deltaTHit.size() % 3 != 1 || deltaT <= 0.0) {
    m_options.deltaTHit.resize(1);
    m_options.deltaTHit[0] = deltaT;
  }

  // Determine maximum time for hit to be recorded
  m_options.timeCut *= CLHEP::ns;

  // Include or not tilecal optical model
  if (m_options.uShape == -1) {
    m_options.uShape = uShapeFromGM; // U-shape not set, take value from GeoModel
    ATH_MSG_INFO("Using U-shape flag from GeoModel = " << uShapeFromGM <<
                 ( (m_options.uShape > 0) ? "  Scintillator width is equal to width of master plate "
                   : "  Scintillator width is smaller than width of master plate "));
  } else if (m_options.uShape == uShapeFromGM) {
    ATH_MSG_INFO("Using U-shape = " << m_options.uShape <<
                 ((m_options.uShape > 0) ? "  Scintillator width is equal to width of master plate "
                  : "  Scintillator width is smaller than width of master plate "));
  } else {
    if (uShapeFromGM > 0 && m_options.uShape > 0) { // both are > 0, so it's fine to use value from SimOptions
      ATH_MSG_INFO("Using U-shape from Simulation flags = " << m_options.uShape <<
                   "  Scintillator width is equal to width of master plate ");
    } else if (uShapeFromGM == 0 && m_options.uShape < -1) { // special case - negative value below -1
      ATH_MSG_INFO("Using U-shape from Simulation flags = " << m_options.uShape <<
                   "  Scintillator width is smaller than width of master plate ");
    } else {
      ATH_MSG_ERROR("U-shape flag in GeoModel = " << uShapeFromGM);
      ATH_MSG_ERROR("U-shape flag in Simulation = " << m_options.uShape);
      ATH_MSG_ERROR("Please, use correct GeoModel tag or make sure that you set both " <<
                    "Ushape parameter for TileGeoG4SD and GeoModelSvc.DetectorTools[ \"TileDetectorTool\" ].Ushape to the same value");
      ATH_MSG_FATAL( "Inconsistent U-shape parameters for TileCal" );
      return StatusCode::FAILURE;
    }
  }
  if (m_options.uShape < -2) {
    ATH_MSG_WARNING("Changing U-shape from " << m_options.uShape << " to -2");
    m_options.uShape = -2;
  }
  if (m_options.uShape > 1 && m_options.uShape < 10) {
    ATH_MSG_WARNING("Changing U-shape from " << m_options.uShape << " to 1");
    m_options.uShape = 1;
  }

  if (deltaT > 0.0)
    m_options.timeCut = ((int) (m_options.timeCut / deltaT) + 0.5) * deltaT;
  if (m_options.deltaTHit.size() > 1) {
    G4cout << "Using deltaTHit = ";
    unsigned int i = 0;
    double delta = m_options.deltaTHit[i++];
    while (i < m_options.deltaTHit.size()) {
      G4cout << delta / CLHEP::ns << " ns for window [";
      G4cout << m_options.deltaTHit[i++] / CLHEP::ns << ",";
      G4cout << m_options.deltaTHit[i++] / CLHEP::ns << "] ";
      delta = m_options.deltaTHit[i++];
    }
    G4cout << "and " << delta / CLHEP::ns << " ns outside this window" << G4endl;
  } else {
    ATH_MSG_INFO("Using deltaTHit = " << deltaT / CLHEP::ns << " ns. ");
  }
  ATH_MSG_INFO("Using timeCut = " << m_options.timeCut / CLHEP::ns << " ns. ");
  ATH_MSG_INFO("Using doBirk = " << (m_options.doBirk ? "true" : "false"));
  ATH_MSG_INFO("Using doTOFCorr = " << (m_options.doTOFCorrection ? "true" : "false"));
  ATH_MSG_INFO("Using doTileRow = " << (m_options.doTileRow ? "true" : "false"));
  ATH_MSG_INFO("Using doCalibHitParticleID = " << (m_options.doCalibHitParticleID ? "true" : "false"));

  if (deltaT > 0.0) {
    m_keepHitTime = true;
  } else {
    ATH_MSG_WARNING("deltaT is not set, ignore hit time in ProcessHits()");
  }

  m_tileSizeDeltaT = 100000 * CLHEP::ns; // used for doTileRow

  if (m_options.timeCut > m_tileSizeDeltaT - deltaT) {
    m_options.timeCut = m_tileSizeDeltaT - deltaT;
    ATH_MSG_WARNING("Reducing timeCut to " << m_options.timeCut / CLHEP::ns << " ns. ");
  } else if ( ! m_options.doTOFCorrection && m_options.timeCut < 1000*CLHEP::ns ) {
    // assuming that if TOF correction is disabled, then we are running cosmic simulation
    // and should not use too restrictive time cut
    m_options.timeCut = m_tileSizeDeltaT - deltaT;
    ATH_MSG_WARNING("TOF correction is disabled, settting time cut to "
                    << m_options.timeCut / CLHEP::ns << " ns. ");
  }

  m_lateHitTime = m_tileSizeDeltaT - deltaT;
  ATH_MSG_INFO("All hits with time above " << m_options.timeCut / CLHEP::ns << " ns will be stored with time = "
               << m_lateHitTime / CLHEP::ns << " ns.");

  // Read attenuation lengths from file and store them in tilerow
  // Variables to divide deposited energy in scintillators for Up and Down PMTs
  if (m_options.uShape == -2) {
    static const std::string attFileName = "TileAttenuation.dat";
    static const std::string ratioFileName = "TileOpticalRatio.dat";
    std::string attFile = PathResolver::find_file(attFileName, "DATAPATH");
    std::string ratioFile = PathResolver::find_file(ratioFileName, "DATAPATH");
    m_row = CxxUtils::make_unique<TileRow>(attFile, ratioFile); //holds attenuation lengths for tiles
    ATH_MSG_INFO("Using Optical Ratio = " << m_row->OpticalRatio[0].at(0));
  }

  return StatusCode::SUCCESS;
}

StatusCode TileGeoG4SDCalc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ITileCalculator::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ITileCalculator*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly available : try out a base class
  return AthService::queryInterface(riid, ppvInterface);
}

int TileGeoG4SDCalc::getUshapeFromGM() const {
  const TileDetectorTool* tileDetectorTool =
    dynamic_cast<const TileDetectorTool *>(m_geoModSvc->getTool("TileDetectorTool"));
  return (tileDetectorTool) ? tileDetectorTool->uShape() : 0;
}

TileGeoG4LookupBuilder* TileGeoG4SDCalc::GetLookupBuilder() const
{
  // Setup the lookup table if necessary
  if(!s_lookup) {
    s_lookup = std::make_unique<TileGeoG4LookupBuilder>(&*m_detStore, m_options.verboseLevel);
    s_lookup->BuildLookup(m_options.tileTB);
  }
  return s_lookup.get();
}

const TileSDOptions* TileGeoG4SDCalc::GetOptions() const
{
  return &m_options;
}

G4bool TileGeoG4SDCalc::FindTileScinSection(const G4Step* aStep, TileHitData& hitData) const
{
  // Get the lookup table
  auto lookup = GetLookupBuilder();

  // Determine touchablehistory for the step
  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

  // Find level of TileCal envelope
  G4int level = theTouchable->GetHistoryDepth();

  // Debugging of volumes hierarchy
  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "FindTileScinSection" << endmsg;
    for (int i = 0; i < level; i++) {
      G4VPhysicalVolume* pv = theTouchable->GetVolume(i);
      G4LogicalVolume* lv = pv->GetLogicalVolume();
      msg(MSG::VERBOSE) << " Level--> " << i
                        << ", LogVol--> " << lv->GetName()
                        << ", PhysVol--> " << pv->GetName() << endmsg;
    }
  }

  // Find envelope of TileCal
  G4String nameLogiVol = theTouchable->GetVolume(level - 1)->GetLogicalVolume()->GetName();
  static const char * const tileVolumeString = "Tile";
  while (nameLogiVol.find(tileVolumeString) == G4String::npos) {
    level--;
    nameLogiVol = theTouchable->GetVolume(level - 1)->GetLogicalVolume()->GetName();
  }
  ATH_MSG_VERBOSE("Tile volume level ---> " << level);

  level--;  // Move to the level of sections Barrel, EBarrelPos, EBarrelNeg, Gap etc...

  // Check whether we have multiple tree tops in DD
  // If this is the case, move one level down
  nameLogiVol = theTouchable->GetVolume(level - 1)->GetLogicalVolume()->GetName();
  static const char * const cenBarrelVolumeString = "CentralBarrel";
  static const char * const endcapPosVolumeString = "EndcapPos";
  static const char * const endcapNegVolumeString = "EndcapNeg";
  if (nameLogiVol.find(cenBarrelVolumeString) != G4String::npos ||
      nameLogiVol.find(endcapPosVolumeString) != G4String::npos ||
      nameLogiVol.find(endcapNegVolumeString) != G4String::npos ) {
    level--;
  }

  // Determine physical volume for the step
  const G4VPhysicalVolume* physSection = theTouchable->GetVolume(level - 1);
  const G4String namePhysSection = physSection->GetName();

  // Check current TileSectionDescription
  static const char * const ebarrelVolumeString = "EBarrel";
  static const char * const barrelVolumeString = "Barrel";
  static const char * const itcVolumeString = "ITC";
  static const char * const gapVolumeString = "Gap";
  static const char * const crackVolumeString = "Crack";
  int sect = 0;
  if (namePhysSection.find(ebarrelVolumeString) != G4String::npos) {
    hitData.section = lookup->GetSection(TileDddbManager::TILE_EBARREL);
    hitData.nDetector = 2;
    sect = 2;
  } else if (namePhysSection.find(barrelVolumeString) != G4String::npos) {
    hitData.section = lookup->GetSection(TileDddbManager::TILE_BARREL);
    hitData.nDetector = 1;
    sect = 1;
  } else if (namePhysSection.find(itcVolumeString) != G4String::npos) {
    hitData.nDetector = 3;
    static const char * const plug1VolumeString = "Plug1";
    const G4String namePlug = theTouchable->GetVolume(level - 3)->GetName();
    if (namePlug.find(plug1VolumeString) != G4String::npos) {
      hitData.section = lookup->GetSection(TileDddbManager::TILE_PLUG1);
      sect = 3;
    } else {
      hitData.section = lookup->GetSection(TileDddbManager::TILE_PLUG2);
      sect = 4;
    }
  } else if (namePhysSection.find(gapVolumeString) != G4String::npos) {
    hitData.section = lookup->GetSection(TileDddbManager::TILE_PLUG3);
    hitData.nDetector = 3;
    sect = 5;
  } else if (namePhysSection.find(crackVolumeString) != G4String::npos) {
    hitData.section = lookup->GetSection(TileDddbManager::TILE_PLUG4);
    hitData.nDetector = 3;
    sect = 6;
  } else {
    ATH_MSG_FATAL("Wrong name for tile scin section --> " << namePhysSection.c_str());
    return false;
  }

  // Check the copy number of the module (1-64)
  // hitData.nModule = theTouchable->GetVolume(level-2)->GetCopyNo();
  hitData.nModule = theTouchable->GetReplicaNumber(level - 2);
  if (theTouchable->GetVolume(level - 2)->IsReplicated())
    hitData.nModule++;
  if (hitData.nModule > hitData.section->nrOfModules) {
    ATH_MSG_FATAL("Wrong module index -->" << hitData.nModule);
    return false;
  }

  // Check the side
  hitData.nSide = 1;
  hitData.isNegative = false;
  static const char * const eBarrelVolumeString = "EBarrel";
  if ((namePhysSection.find(barrelVolumeString) != G4String::npos) &&
      (namePhysSection.find(eBarrelVolumeString) == G4String::npos) ) {}
  else {
    static const char * const negVolumeString = "Neg";
    if (namePhysSection.find(negVolumeString) != G4String::npos) {
      hitData.nSide = -1;
      hitData.isNegative = true;
    }
  }

  // Get the number of scintillator inside module 0 - ...
  const G4VPhysicalVolume* physVol = theTouchable->GetVolume();
  hitData.tileSize = physVol->GetCopyNo();
  hitData.tilePeriod = theTouchable->GetReplicaNumber(2);

  // 1. Move to period volume check if it's replicated and take the replica number
  //    If the period is not replicated that means we are in the second Absorber Child
  //    or in Gap/Crack
  // 2. Level of the period = 2 (Always)
  // G4VPhysicalVolume* physPeriod = theTouchable->GetVolume(2);
  // if (physPeriod ->IsReplicated())
  // tileperiod = theTouchable->GetReplicaNumber(2);
  // else
  // tileperiod = _section->nrOfPeriods - 1;

  ATH_MSG_VERBOSE("Section number: " << sect*hitData.nSide);
  ATH_MSG_VERBOSE("Module number:  " << hitData.nModule);
  ATH_MSG_VERBOSE("Period number:  " << hitData.tilePeriod);
  ATH_MSG_VERBOSE("Scin Copy No:   " << hitData.tileSize);

  int nScin = hitData.tilePeriod * hitData.section->nrOfScintillators + hitData.tileSize;
  hitData.cell = hitData.section->ScinToCell(nScin);
  if (!hitData.cell) {
    ATH_MSG_ERROR("Zero pointer to current cell, nScin=" << nScin);
    hitData.section->PrintScinToCell("ERROR in _cell");
    return false;
  }

  hitData.nrOfPMT = hitData.cell->nrOfPMT;
  ATH_MSG_VERBOSE("Number of PMTs: " << hitData.nrOfPMT);

  // Attach special D4 cell to cell D5 in ext.barrel
  if (sect == 3) {
    if ( lookup->TileGeoG4npmtD4(std::max(0, hitData.nSide), hitData.nModule - 1) == 0 ) {
      hitData.section = lookup->GetSection(TileDddbManager::TILE_EBARREL);
      hitData.nDetector = 2;  //ext.barrel
      hitData.tileSize += 9;
      nScin = hitData.tileSize; //last tile row in first period
      hitData.cell = hitData.section->ScinToCell(nScin);
      const char nm[5] = {'X', 'A', 'B', 'D', 'E'};
      ATH_MSG_VERBOSE("Disabling special D4, all energy goes to cell "
                      << nm[hitData.cell->sample] << hitData.cell->cellNum);
    }
  } else if (sect == 4) {
    // Change number of PMTs in Special C10 Cells
    hitData.nrOfPMT = lookup->TileGeoG4npmtC10(std::max(0, hitData.nSide), hitData.nModule - 1);
    if (hitData.nrOfPMT != 2) {
      ATH_MSG_VERBOSE("Changing number of PMTs in special C10 to " << hitData.nrOfPMT);
    }
  } else if (sect == 6 && hitData.cell->tower > 16) {
    // Check number of PMTs in Special E4' Cells (side C, modules 32,33)
    hitData.nrOfPMT = lookup->TileGeoG4npmtE5(std::max(0, hitData.nSide), hitData.nModule - 1);
    if (hitData.nrOfPMT != hitData.cell->nrOfPMT) {
      ATH_MSG_WARNING("Changing number of PMTs in special E4' from " << hitData.cell->nrOfPMT << " to " << hitData.nrOfPMT);
    }
  }

  // Determine tower and sample
  hitData.nTower = hitData.cell->tower;
  if (hitData.nTower < 0) {
    hitData.nTower *= -1;
  }
  hitData.nSample = hitData.cell->sample;

  // If that's central barrel then determine the side
  if ((hitData.nDetector == 1) && (hitData.cell->cellNum < 0)) {
    hitData.nSide = -1;
  }

  return true;
}

G4bool TileGeoG4SDCalc::MakePmtEdepTime(const G4Step* aStep, TileHitData& hitData, double& deltaTime) const
{
  if (hitData.nrOfPMT == 0) {
    return false;
  }

  const bool isE5 = (hitData.nTower > 16);  //special E5(E4') cells in EBC (should be in negative side only)

  // Take into account Birk's saturation law in organic scintillators.
  const G4double edep = (m_options.doBirk) ? this->BirkLaw(aStep) : aStep->GetTotalEnergyDeposit() * aStep->GetTrack()->GetWeight();

  double deltaT_up = 0;
  double deltaT_down = 0;
  const double ref_ind_tile = 1.59;
  const double ref_ind_fiber = 1.59;  //refraction indexes of tiles and fibers

  const G4TouchableHistory* theTouchable = (G4TouchableHistory*) (aStep->GetPreStepPoint()->GetTouchable());
  const G4VPhysicalVolume* physVol = theTouchable->GetVolume();
  const G4LogicalVolume* logiVol = physVol->GetLogicalVolume();

  G4Trd* scinSolid = dynamic_cast<G4Trd*>(logiVol->GetSolid());
  G4SubtractionSolid* booleanScin = dynamic_cast<G4SubtractionSolid*>(logiVol->GetSolid());

  // Local coordinates of PreStepPoint in scintillator
  G4ThreeVector prestepPos = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector localCoord = theTouchable->GetHistory()->GetTopTransform().TransformPoint(prestepPos);
  G4double yLocal = localCoord.y();
  G4double zLocal = localCoord.z();
  if (hitData.nDetector == 1 || hitData.nSide > 0)
    yLocal *= -1;  // LBA,LBC,EBA have yLocal inverse to the EBC one

  if (scinSolid || booleanScin) {
    if (booleanScin) {
      scinSolid = nullptr;
      // The first step inside boolean solid
      G4VSolid* solid1 = booleanScin->GetConstituentSolid(0);

      // G4SubtractionSolid* booleanScin1 = dynamic_cast<G4SubtractionSolid*>(solid1);
      // if(booleanScin1) {
      // // The second step inside boolean solid
      // G4VSolid* solid2 = booleanScin1->GetConstituentSolid(0);
      // scinSolid = dynamic_cast<G4Trd*>(solid2);
      // }

      scinSolid = dynamic_cast<G4Trd*>(solid1);
    }

    if (!scinSolid) {  //----    error ----
      ATH_MSG_FATAL("Scintillator solid is not G4TRD!");
      return false;
    }

    // Divide edep for Up and Down
    if (hitData.nrOfPMT == 2) {
      // Scintillator dimensions
      G4double dy1Scin = scinSolid->GetYHalfLength1();
      G4double dy2Scin = scinSolid->GetYHalfLength2();
      G4double dzScin = scinSolid->GetZHalfLength();

      // half width in y at local point
      G4double halfYLocal = dy1Scin + (dy2Scin - dy1Scin) * (zLocal + dzScin) / (dzScin * 2);

      // distances to edges in y (dy1 - to negative edge)
      G4double dy1Local = yLocal + halfYLocal;
      G4double dy2Local = halfYLocal - yLocal;

      if (isE5) { //special E5(E4') cells in EBC (negative)
        if (yLocal >= 0.) {
          hitData.edep_up = edep;
          hitData.edep_down = 0.;
        } else {
          hitData.edep_up = 0.;
          hitData.edep_down = edep;
        }
      } else {
        switch (m_options.uShape) {

        case 1:  // using the best U-shape known up to now (see #define in header file)
          hitData.edep_up = edep * Tile_1D_profile(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
          hitData.edep_down = edep * Tile_1D_profile(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
          break;

        case 0:  // the same as default case below (i.e. no ushape at all)
          hitData.edep_up = edep * (0.3 + 0.2 * dy1Local / halfYLocal);
          hitData.edep_down = edep - hitData.edep_up;
          break;

        case 14:
          hitData.edep_up = edep * Tile_1D_profileRescaled(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
          hitData.edep_down = edep * Tile_1D_profileRescaled(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
          break;

        case 20:
        case 21:
        case 22:
        case 23:
          hitData.edep_up = edep * Tile_1D_profileRescaled_zeroBins(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide,
                                                                    m_options.uShape - 20);
          hitData.edep_down = edep * Tile_1D_profileRescaled_zeroBins(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide,
                                                                      m_options.uShape - 20);
          break;

        case 13:
          hitData.edep_up = edep * Tile_1D_profileFunc(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
          hitData.edep_down = edep * Tile_1D_profileFunc(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
          break;

        case 12:
          hitData.edep_up = edep * Tile_1D_profileSym(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
          hitData.edep_down = edep * Tile_1D_profileSym(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
          break;

        case 11:
          hitData.edep_up = edep * Tile_1D_profileAsym(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
          hitData.edep_down = edep * Tile_1D_profileAsym(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
          break;

        case - 2: {
          int ind = hitData.nDetector - 1;
          if (1 == hitData.nDetector && 2 == hitData.nSample && 9 == hitData.nTower) { // read info about B-9 from ITC vector
            ind = 2;                                          // because barrel vector contains info for
          }                                                     // BC cells only (average over 6 rows)
          const double Izero = m_row->OpticalRatio[ind].at(hitData.tileSize);
          const double AttenuationLength = m_row->attLen[ind].at(hitData.tileSize);
          ATH_MSG_VERBOSE("Izero = " << Izero);
          ATH_MSG_VERBOSE("Tile Row = " << hitData.tileSize);
          ATH_MSG_VERBOSE("Attenuation Length = " << AttenuationLength);
          hitData.edep_up = edep * exp( -dy2Local / AttenuationLength) / Izero;
          hitData.edep_down = edep * exp( -dy1Local / AttenuationLength) / Izero;
          break;
        }

        default:
          hitData.edep_up = edep * (0.3 + 0.2 * dy1Local / halfYLocal);
          hitData.edep_down = edep - hitData.edep_up;
        }

      }

      deltaT_up = ref_ind_tile * (dy2Local - (hitData.cell->r * tanPi64)) / CLHEP::c_light;
      deltaT_down = ref_ind_tile * (dy1Local - (hitData.cell->r * tanPi64)) / CLHEP::c_light;

    }
  } else {
    ATH_MSG_FATAL("Scintillator solid is not G4TRD! or it is not Boolean Solid!");
    return false;
  }

  G4double totalTime = aStep->GetPostStepPoint()->GetGlobalTime();  //added by Mike

  if (m_keepHitTime) {
    const G4ThreeVector position = aStep->GetPostStepPoint()->GetPosition();  //position of the hit
    const double cosTh_hit = position.cosTheta();
    const double magn_hit = position.mag();
    const double r_hit = position.perp();  //hit radius r = sqrt(x*x + y*y)
    double correction(0.0);

    if (m_options.doTOFCorrection) {
      correction = (hitData.cell->r * (1.0 * ref_ind_fiber - 2.0 / hitData.cell->sinTh)
                    + magn_hit * (1.0 - ref_ind_fiber * sqrt(1.0 - cosTh_hit * cosTh_hit)))
        / CLHEP::c_light;
      //now it is time with correction due to fiber length.
      //additional term "-1/_cell->sinTh" is distance to cell's center (TOF correction)
    } else {
      correction = (hitData.cell->r - r_hit) * ref_ind_fiber / CLHEP::c_light;
    }
    totalTime += correction;

    if (m_options.verboseLevel >= 10) {
      G4cout << " deltaT_up: " << deltaT_up << " deltaT_down: " << deltaT_down << G4endl;
      G4cout << "Global time " << totalTime - correction << G4endl;

      if (m_options.doTOFCorrection) {
        G4cout << " distance to ATLAS center " << hitData.cell->r / hitData.cell->sinTh / CLHEP::c_light << " ns" << G4endl;
        G4cout << " correction " << correction << G4endl;
        G4cout << " after TOF correction " << totalTime << G4endl;
      } else {
        G4cout << " distance to cell center " << (hitData.cell->r - r_hit) << G4endl;
        G4cout << " correction " << correction << G4endl;
        G4cout << " after cell time correction " << totalTime << G4endl;
      }

      G4cout << " coord of hit: magn, r, cosTh, Ylocal "
             << magn_hit
             << ", " << r_hit
             << ", " << cosTh_hit
             << ", " << yLocal << G4endl;

      G4cout << " and cell:    magn, r, cosTh, Ycell/2 "
             << hitData.cell->r / hitData.cell->sinTh
             << ", " << hitData.cell->r << ", "
             << sqrt(1 - hitData.cell->sinTh * hitData.cell->sinTh) << ", " << hitData.cell->r * tanPi64 << G4endl;

      if (totalTime != 0.0) {
        G4cout << "---- CELL INFO ----" << G4endl;
        G4cout << "Detector >> " << hitData.cell->detector << G4endl;
        G4cout << "CellNum  >> " << hitData.cell->cellNum << G4endl;
        G4cout << "Tower    >> " << hitData.cell->tower << G4endl;
        G4cout << "Sample   >> " << hitData.cell->sample << G4endl;
        G4cout << "---- CELL INFO ----" << G4endl;

        G4cout << "---- HIT INFO ----" << G4endl;
        G4cout << "Detector >> " << hitData.nDetector << G4endl;
        G4cout << "Side     >> " << hitData.nSide << G4endl;
        if (isE5) {
          G4cout << "Module-1 >> " << hitData.nModule - 1 << ((yLocal >= 0.0) ? " Up" : " Down") << G4endl;
          G4cout << "Phi ind  >> " << (hitData.nModule - 32)*2 + ((yLocal >= 0.0) ? 1 : 0);
          G4cout << "  E5 Y-coord global " << position.y() << " local " << yLocal << G4endl;
          G4cout << "Eta ind  >> 2" << G4endl;
        } else {
          G4cout << "Module-1 >> " << hitData.nModule - 1 << G4endl;
          G4cout << "Tower-1  >> " << hitData.nTower - 1 << G4endl;
          G4cout << "Sample-1 >> " << hitData.nSample - 1 << G4endl;
        }
        G4cout << "Energy   >> " << edep << " (" << hitData.edep_up << "," << hitData.edep_down << ")" << G4endl;
        G4cout << "TotalTime>> " << totalTime << G4endl;
        G4cout << "---- HIT INFO ----" << G4endl;
      }
    }
  }

  // check that hit time is within limits
  // if not - put it to maximum allowed time
  if (totalTime + deltaT_up > m_options.timeCut || totalTime + deltaT_down > m_options.timeCut) {
    totalTime = m_lateHitTime;
    deltaT_up = deltaT_down = 0.0;
    ATH_MSG_VERBOSE(" hit time set to " << totalTime);
  }
  // calculate unique deltaT bin width for both up and down PMT, ignoring additional deltas
  deltaTime = this->deltaT(totalTime);
  const double scin_Time = totalTime + (hitData.tileSize + 1) * m_tileSizeDeltaT;
  hitData.scin_Time_up = hitData.scin_Time_down = scin_Time;
  hitData.totalTimeUp = hitData.totalTimeDown = totalTime;

  if (isE5) { //special E5(E4') cells in EBC (negative)
    if (hitData.edep_up != 0.) {
      hitData.pmtID_up = m_tileTBID->channel_id(hitData.nSide, (hitData.nModule - 32) * 2 + 1, 2);
      hitData.pmtID_down = m_invalid_id;
      hitData.scin_Time_down = 0.;
    } else {
      hitData.pmtID_up = m_invalid_id;
      hitData.pmtID_down = m_tileTBID->channel_id(hitData.nSide, (hitData.nModule - 32) * 2, 2);
      hitData.scin_Time_up = 0.;
    }
  } else {
    hitData.pmtID_up = m_tileID->pmt_id(hitData.nDetector, hitData.nSide, hitData.nModule - 1, hitData.nTower - 1, hitData.nSample - 1, 1);
    hitData.pmtID_down = m_tileID->pmt_id(hitData.nDetector, hitData.nSide, hitData.nModule - 1, hitData.nTower - 1, hitData.nSample - 1, 0);
    if (hitData.nrOfPMT == 2) {
      hitData.scin_Time_up += deltaT_up;
      hitData.scin_Time_down += deltaT_down;
      hitData.totalTimeUp += deltaT_up;
      hitData.totalTimeDown += deltaT_down;
    } else if (hitData.nrOfPMT == 1) {  //Gap/Crack has only one PMT. So, It's not needed to divide
      hitData.pmtID_up = m_invalid_id;
      hitData.scin_Time_up = 0.;
      hitData.edep_up = 0.;
      hitData.edep_down = edep;
    } else {
      hitData.pmtID_down = m_invalid_id;
      hitData.scin_Time_down = 0.;
      hitData.edep_down = 0.;
      hitData.edep_up = edep;
    }
  }

  ATH_MSG_VERBOSE("MakePmtEdepTime: right pmtID_up,pmtID_down," << "edep_up,edep_down,scin_Time_up,scin_Time_down:\t"
                  << hitData.pmtID_up << "\t"
                  << hitData.pmtID_down << "\t"
                  << hitData.edep_up << "\t"
                  << hitData.edep_down << "\t"
                  << hitData.scin_Time_up << "\t"
                  << hitData.scin_Time_down);

  return true;
}

TileMicroHit TileGeoG4SDCalc::GetTileMicroHit(const G4Step* aStep, TileHitData& hitData) const
{
  TileMicroHit microHit;
  microHit.pmt_up = m_invalid_id;
  microHit.pmt_down = m_invalid_id;
  microHit.e_up = 0.0;
  microHit.e_down = 0.0;
  microHit.time_up = 0.0;
  microHit.time_down = 0.0;
  //microHit.period  = 0;               microHit.tilerow   = 0; // prepared for future use

  if (aStep->GetTotalEnergyDeposit() == 0. && aStep->GetTrack()->GetDefinition() != G4Geantino::GeantinoDefinition()) {
    ATH_MSG_DEBUG("GetTileMicroHit: Edep=0");
    return microHit;
  }

  //Search for the tilecal sub-section, its module and some identifiers
  if (!this->FindTileScinSection(aStep, hitData)) {
    ATH_MSG_DEBUG("GetTileMicroHit: FindTileScinSection(aStep) is false!");
    return microHit;
  }

  double deltaTime(0);

  //calculation of pmtID, edep and scin_Time with aStep
  if (!this->MakePmtEdepTime(aStep, hitData, deltaTime)) {
    ATH_MSG_DEBUG("MakePmtEdepTime: wrong pmtID_up,pmtID_down,edep_up,"
                    << "edep_down,scin_Time_up,scin_Time_down:\t"
                    << hitData.pmtID_up << "\t"
                    << hitData.pmtID_down << "\t"
                    << hitData.edep_up << "\t"
                    << hitData.edep_down << "\t"
                    << hitData.scin_Time_up << "\t"
                    << hitData.scin_Time_down);
    return microHit;
  }
  microHit.pmt_up = hitData.pmtID_up;
  microHit.pmt_down = hitData.pmtID_down;
  microHit.e_up = hitData.edep_up;
  microHit.e_down = hitData.edep_down;
  microHit.time_up = hitData.totalTimeUp;
  microHit.time_down = hitData.totalTimeDown;
  //microHit.period  = tileperiod;      microHit.tilerow   = tilesize; // prepared for future use
  return microHit;
}

G4bool TileGeoG4SDCalc::ManageScintHit(TileHitData& hitData, double deltaTime) const
{
  //Having hitData.cell and hitData.nModule (number of current module) we need to
  //determine if a Hit object already exists for this cell and module.
  //If YES: we just add energy to the existing hit object
  //If No: we create a new object, fill it and place in the collection

  bool newTileHitUp = true;
  bool newTileHitDown = true;

  // Debugging of special E5(E4') cells
  //if (hitData.nTower>16) G4cout <<" Cells E5: hitData.nModule="<<hitData.nModule
  //                       <<"  edep_up="<<hitData.edep_up<<"  edep_down="<<hitData.edep_down
  //                       <<"  pmtID_up="<<hitData.pmtID_up<<"  pmtID_down="<<hitData.pmtID_down<<G4endl;

  if (hitData.isNegative) {
    if (hitData.nrOfPMT == 2) {
      if (hitData.cell->moduleToHitUpNegative[hitData.nModule - 1]) { newTileHitUp = false; }
      if (hitData.cell->moduleToHitDownNegative[hitData.nModule - 1]) { newTileHitDown = false; }
    } else if (hitData.nrOfPMT == 1) {
      if (hitData.cell->moduleToHitDownNegative[hitData.nModule - 1]) { newTileHitDown = false; }
    } else if (hitData.nrOfPMT == -1) {
      if (hitData.cell->moduleToHitUpNegative[hitData.nModule - 1]) { newTileHitUp = false; }
    } else {
      ATH_MSG_FATAL("ManageScintHit: Unexpected number of PMTs in a Cell -->" << hitData.nrOfPMT);
      return false;
    }
  } else { //positive
    if (hitData.nrOfPMT == 2) {
      if (hitData.cell->moduleToHitUp[hitData.nModule - 1]) { newTileHitUp = false; }
      if (hitData.cell->moduleToHitDown[hitData.nModule - 1]) { newTileHitDown = false; }
    } else if (hitData.nrOfPMT == 1) {
      if (hitData.cell->moduleToHitDown[hitData.nModule - 1]) { newTileHitDown = false; }
    } else if (hitData.nrOfPMT == -1) {
      if (hitData.cell->moduleToHitUp[hitData.nModule - 1]) { newTileHitUp = false; }
    } else {
      ATH_MSG_FATAL("ManageScintHit: Unexpected number of PMTs in a Cell -->" << hitData.nrOfPMT);
      return false;
    }
  }

  if (hitData.edep_up != 0.) {
    if (newTileHitUp) { this->CreateScintHit(1, hitData, deltaTime); }
    else { this->UpdateScintHit(1, hitData, deltaTime); }
  }

  if (hitData.edep_down != 0.) {
    if (newTileHitDown) { this->CreateScintHit(0, hitData, deltaTime); }
    else { this->UpdateScintHit(0, hitData, deltaTime); }
  }

  return true;
}

void TileGeoG4SDCalc::CreateScintHit(int pmt, TileHitData& hitData, double deltaTime) const
{
  if (pmt == 1) { //Upper PMT of Cell
    std::unique_ptr<TileSimHit> aHit = std::make_unique<TileSimHit>(hitData.pmtID_up, hitData.edep_up, hitData.totalTimeUp, deltaTime);
    if (m_options.doTileRow) {
      aHit->add(hitData.edep_up, hitData.scin_Time_up, deltaTime);
    }
    if (hitData.isNegative) {
      hitData.cell->moduleToHitUpNegative[hitData.nModule - 1] = aHit.release();
    }
    else {
      hitData.cell->moduleToHitUp[hitData.nModule - 1] = aHit.release();
    }
  } else { //Down PMT of Cell
    std::unique_ptr<TileSimHit> aHit = std::make_unique<TileSimHit>(hitData.pmtID_down, hitData.edep_down, hitData.totalTimeDown, deltaTime);
    if (m_options.doTileRow) {
      aHit->add(hitData.edep_down, hitData.scin_Time_down, deltaTime);
    }
    if (hitData.isNegative) {
      hitData.cell->moduleToHitDownNegative[hitData.nModule - 1] = aHit.release();
    }
    else {
      hitData.cell->moduleToHitDown[hitData.nModule - 1] = aHit.release();
    }
  }
}

void TileGeoG4SDCalc::UpdateScintHit(int pmt, TileHitData& hitData, double deltaTime) const
{
  TileSimHit* aHit(nullptr);

  if (pmt == 1) { //Upper PMT of Cell
    if (hitData.isNegative) {
      aHit = hitData.cell->moduleToHitUpNegative[hitData.nModule - 1];
    }
    else {
      aHit = hitData.cell->moduleToHitUp[hitData.nModule - 1];
    }
    aHit->add(hitData.edep_up, hitData.totalTimeUp, deltaTime);
    if (m_options.doTileRow) {
      aHit->add(hitData.edep_up, hitData.scin_Time_up, deltaTime);
    }
  } else { // Down PMT of Cell
    if (hitData.isNegative) {
      aHit = hitData.cell->moduleToHitDownNegative[hitData.nModule - 1];
    }
    else {
      aHit = hitData.cell->moduleToHitDown[hitData.nModule - 1];
    }
    aHit->add(hitData.edep_down, hitData.totalTimeDown, deltaTime);
    if (m_options.doTileRow) {
      aHit->add(hitData.edep_down, hitData.scin_Time_down, deltaTime);
    }
  }
}

G4double TileGeoG4SDCalc::BirkLaw(const G4Step* aStep) const
{
  // *** apply BIRK's saturation law to energy deposition ***
  // *** only organic scintillators implemented in this version MODEL=1
  //
  // Note : the material is assumed ideal, which means that impurities
  //        and aging effects are not taken into account
  //
  // algorithm : edep = destep / (1. + RKB*dedx + C*(dedx)**2)
  //
  // the basic units of the coefficient are g/(MeV*cm**2)
  // and de/dx is obtained in MeV/(g/cm**2)
  //
  // exp. values from NIM 80 (1970) 239-244 :
  //
  // RKB = 0.013  g/(MeV*cm**2)  and  C = 9.6e-6  g**2/((MeV**2)(cm**4))

  const G4String myMaterial = "Scintillator";
  const G4double birk1 = 0.0130 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2);
  const G4double birk2 = 9.6e-6 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2) * CLHEP::g / (CLHEP::MeV * CLHEP::cm2);
  G4double response = 0.;

  const G4double destep = aStep->GetTotalEnergyDeposit() * aStep->GetTrack()->GetWeight();
  //  doesn't work with shower parameterisation
  //  G4Material* material = aStep->GetTrack()->GetMaterial();
  //  G4double charge      = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
  const G4Material* material = aStep->GetPreStepPoint()->GetMaterial();
  const G4double charge = aStep->GetPreStepPoint()->GetCharge();

  // --- no saturation law for neutral particles ---
  // ---  and materials other than scintillator  ---
  if ( (charge != 0.) && (material->GetName() == myMaterial)) {
    G4double rkb = birk1;
    // --- correction for particles with more than 1 charge unit ---
    // --- based on alpha particle data (only apply for MODEL=1) ---
    if (fabs(charge) > 1.0)
      rkb *= 7.2 / 12.6;

    if (aStep->GetStepLength() != 0) {
      G4double dedx = destep / (aStep->GetStepLength()) / (material->GetDensity());
      response = destep / (1. + rkb * dedx + birk2 * dedx * dedx);
    }
    else {
      ATH_MSG_DEBUG("BirkLaw() - Current Step in scintillator has zero length." << "Ignore Birk Law for this Step");
      response = destep;
    }

    // if (-2==verboseLevel) {
    // G4cout << " Destep: " << destep/CLHEP::keV << " keV"
    // << " response after Birk: "  << response/CLHEP::keV << " keV" << G4endl;
    // }
    return response;
  }
  else {
    return destep;
  }
}

void TileGeoG4SDCalc::pmtEdepFromFCS_StepInfo(TileHitData& hitData, double ene, double yLocal, double halfYLocal, double zLocal, int uShape) const
{
  /// Code moved here from TileFastCaloSim/src/TileFCSmStepToTileHitVec.cxx
  switch (uShape) {
  case 1:
    hitData.edep_down = ene * this->Tile_1D_profile(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
    hitData.edep_up = ene * this->Tile_1D_profile(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
    break;

  case 14:
    hitData.edep_down = ene * this->Tile_1D_profileRescaled(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
    hitData.edep_up = ene * this->Tile_1D_profileRescaled(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
    break;

  case 13:
    hitData.edep_down = ene * this->Tile_1D_profileFunc(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
    hitData.edep_up = ene * this->Tile_1D_profileFunc(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
    break;

  case 12:
    hitData.edep_down = ene * this->Tile_1D_profileSym(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
    hitData.edep_up = ene * this->Tile_1D_profileSym(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
    break;

  case 11:
    hitData.edep_down = ene * this->Tile_1D_profileAsym(hitData.tileSize, yLocal, zLocal, 0, hitData.nDetector, hitData.nSide);
    hitData.edep_up = ene * this->Tile_1D_profileAsym(hitData.tileSize, yLocal, zLocal, 1, hitData.nDetector, hitData.nSide);
    break;

  default:
    hitData.edep_down = ene * ( 0.5 - 0.2 * yLocal/halfYLocal ); // should get 0.7 for yLocal = -halfYLocal
    hitData.edep_up = ene - hitData.edep_down;
  }
  return;
}

G4double TileGeoG4SDCalc::Tile_1D_profileAsym(int row, G4double x, G4double y, int PMT, int nDetector, int) {

  const double xlow = -0.0505; //dPhi low [rad]
  const double xup = 0.0505; //dPhi up [rad]
  const double range = (xup - xlow); //dPhi range
  const int size = 101;

  const G4double LB_A_TilePMT1[size] = { 1.07847, 0.624887, 0.654322, 0.691356, 0.703911, 0.693473, 0.680759, 0.676753,
                                         0.673369, 0.671851, 0.667918, 0.664623, 0.657569, 0.650744, 0.64948, 0.644052,
                                         0.638802, 0.634101, 0.630304, 0.621589, 0.623515, 0.618158, 0.61351, 0.609921,
                                         0.601714, 0.598364, 0.596654, 0.590037, 0.586428, 0.587488, 0.580534, 0.576024,
                                         0.576518, 0.570691, 0.572638, 0.57099, 0.566914, 0.561889, 0.561088, 0.557058,
                                         0.555053, 0.554754, 0.549734, 0.551518, 0.550784, 0.55031, 0.545551, 0.543381,
                                         0.530987, 0.513756, 0.48914, 0.470533, 0.461956, 0.462795, 0.461336, 0.458725,
                                         0.456819, 0.453019, 0.449656, 0.445899, 0.44309, 0.437865, 0.43525, 0.433208,
                                         0.428769, 0.428097, 0.420879, 0.421734, 0.41942, 0.415252, 0.414095, 0.411777,
                                         0.412378, 0.40767, 0.403453, 0.403986, 0.402261, 0.39878, 0.397702, 0.394806,
                                         0.392875, 0.395037, 0.39044, 0.390027, 0.388541, 0.387086, 0.386066, 0.383944,
                                         0.378411, 0.376616, 0.374179, 0.36925, 0.365689, 0.360154, 0.353057, 0.343255,
                                         0.331438, 0.307508, 0.275036, 0.258838, 0.368788 };

  const G4double LB_A_TilePMT2[size] = { 0.369783, 0.211972, 0.230947, 0.268859, 0.303837, 0.329078, 0.343237, 0.356303,
                                         0.363648, 0.367898, 0.373731, 0.378617, 0.380875, 0.383722, 0.386158, 0.389404,
                                         0.389971, 0.39164, 0.39418, 0.394549, 0.400512, 0.401163, 0.40209, 0.404069,
                                         0.405405, 0.406696, 0.409069, 0.411492, 0.411167, 0.418688, 0.418122, 0.415808,
                                         0.423246, 0.422552, 0.428291, 0.43252, 0.432099, 0.435391, 0.437928, 0.442073,
                                         0.442692, 0.449074, 0.449808, 0.45495, 0.458974, 0.463795, 0.464748, 0.469373,
                                         0.472605, 0.472922, 0.475554, 0.488757, 0.51082, 0.535798, 0.550519, 0.555443,
                                         0.557948, 0.558681, 0.559304, 0.560111, 0.560842, 0.561764, 0.564526, 0.567482,
                                         0.567264, 0.570459, 0.570672, 0.579118, 0.580413, 0.581684, 0.584309, 0.585863,
                                         0.595071, 0.596317, 0.595865, 0.601986, 0.606319, 0.610213, 0.61214, 0.615528,
                                         0.62043, 0.629095, 0.629321, 0.63812, 0.642166, 0.649751, 0.652894, 0.657102,
                                         0.657477, 0.666378, 0.669087, 0.67338, 0.677633, 0.683487, 0.683797, 0.689397,
                                         0.697545, 0.705333, 0.694233, 0.711603, 1.09091 };

  const G4double LB_BC_TilePMT1[size] = { 0.86232, 0.630408, 0.590843, 0.634737, 0.662197, 0.665007, 0.663989, 0.658233,
                                          0.653759, 0.652386, 0.648085, 0.648778, 0.645884, 0.641951, 0.637984,
                                          0.633653, 0.631213, 0.623483, 0.619489, 0.616728, 0.610455, 0.607257, 0.60162,
                                          0.598128, 0.596961, 0.588204, 0.588383, 0.581502, 0.577612, 0.577281,
                                          0.574111, 0.568601, 0.568942, 0.564751, 0.560802, 0.556877, 0.555466,
                                          0.550301, 0.550674, 0.549077, 0.543492, 0.54391, 0.543219, 0.539936, 0.537337,
                                          0.536966, 0.533898, 0.530139, 0.523018, 0.506361, 0.493297, 0.48197, 0.475256,
                                          0.471104, 0.472014, 0.468475, 0.465216, 0.462577, 0.459217, 0.457729,
                                          0.452528, 0.44843, 0.446773, 0.445092, 0.438734, 0.439273, 0.433635, 0.43346,
                                          0.429848, 0.429465, 0.42705, 0.426386, 0.422939, 0.421249, 0.417686, 0.417491,
                                          0.413383, 0.409932, 0.408009, 0.409207, 0.405517, 0.403717, 0.40342, 0.400112,
                                          0.399128, 0.395675, 0.393946, 0.391296, 0.388926, 0.385429, 0.381328,
                                          0.374845, 0.369267, 0.362154, 0.356343, 0.347328, 0.335477, 0.312162,
                                          0.285875, 0.291842, 0.397965 };

  const G4double LB_BC_TilePMT2[size] = { 0.263365, 0.214906, 0.219706, 0.262368, 0.305389, 0.331228, 0.348258,
                                          0.357851, 0.363826, 0.371329, 0.375669, 0.381844, 0.388628, 0.390827, 0.39324,
                                          0.396217, 0.40038, 0.399504, 0.40259, 0.4063, 0.407204, 0.410329, 0.409706,
                                          0.413856, 0.417444, 0.416128, 0.419332, 0.420385, 0.421561, 0.426402,
                                          0.427505, 0.429366, 0.433463, 0.43537, 0.438077, 0.43977, 0.441857, 0.44533,
                                          0.448792, 0.452066, 0.452791, 0.456907, 0.46185, 0.462705, 0.467372, 0.471284,
                                          0.47412, 0.4766, 0.47927, 0.478628, 0.48459, 0.496074, 0.511888, 0.524671,
                                          0.537065, 0.539915, 0.541911, 0.544955, 0.547102, 0.550736, 0.55038, 0.550081,
                                          0.554584, 0.55731, 0.55678, 0.562188, 0.563492, 0.567719, 0.569925, 0.572916,
                                          0.57681, 0.583971, 0.585097, 0.587382, 0.588469, 0.592708, 0.595646, 0.597952,
                                          0.603183, 0.610227, 0.612409, 0.617876, 0.623884, 0.626186, 0.63187, 0.635704,
                                          0.637357, 0.644649, 0.648575, 0.653345, 0.656098, 0.656642, 0.659195,
                                          0.660505, 0.664374, 0.66838, 0.675159, 0.672449, 0.681388, 0.763523, 1.15146 };

  const G4double LB_D_TilePMT1[size] = { 0, 0.654633, 0.643319, 0.702355, 0.72481, 0.729073, 0.725006, 0.722084,
                                         0.711588, 0.695388, 0.681839, 0.666268, 0.650862, 0.650125, 0.6472, 0.645389,
                                         0.647846, 0.64311, 0.641117, 0.63621, 0.635085, 0.626846, 0.621915, 0.617182,
                                         0.61213, 0.608226, 0.600898, 0.601514, 0.596864, 0.595888, 0.590334, 0.588135,
                                         0.582723, 0.573513, 0.566818, 0.563592, 0.561062, 0.555623, 0.553896, 0.550581,
                                         0.545828, 0.544675, 0.543014, 0.541125, 0.535408, 0.531537, 0.529289, 0.524695,
                                         0.517724, 0.508576, 0.49755, 0.48825, 0.480206, 0.476198, 0.470921, 0.468682,
                                         0.465822, 0.46104, 0.459963, 0.454502, 0.455241, 0.448025, 0.444876, 0.442404,
                                         0.439997, 0.436057, 0.434554, 0.430467, 0.43136, 0.425417, 0.424322, 0.422026,
                                         0.41588, 0.415965, 0.417683, 0.410721, 0.410801, 0.407133, 0.405118, 0.401957,
                                         0.400107, 0.395836, 0.39232, 0.3902, 0.386518, 0.382322, 0.375693, 0.371528,
                                         0.367903, 0.362157, 0.356937, 0.354351, 0.350887, 0.347506, 0.343497, 0.340126,
                                         0.332798, 0.318248, 0.291111, 0.292694, 0 };

  const G4double LB_D_TilePMT2[size] = { 0, 0.240556, 0.247205, 0.288321, 0.318026, 0.332626, 0.342986, 0.346285,
                                         0.352131, 0.354338, 0.35835, 0.360696, 0.362431, 0.367665, 0.374911, 0.379612,
                                         0.387946, 0.389888, 0.392624, 0.39514, 0.400225, 0.402654, 0.404647, 0.408398,
                                         0.412028, 0.412812, 0.414475, 0.419953, 0.421844, 0.424728, 0.425346, 0.429907,
                                         0.432968, 0.433622, 0.435276, 0.437076, 0.442814, 0.444171, 0.448392, 0.452367,
                                         0.452403, 0.456163, 0.459214, 0.466386, 0.466005, 0.469268, 0.474378, 0.478349,
                                         0.479885, 0.48364, 0.491987, 0.502334, 0.515772, 0.523225, 0.53028, 0.534131,
                                         0.536624, 0.541967, 0.546554, 0.549513, 0.55471, 0.553494, 0.55466, 0.558194,
                                         0.561207, 0.566447, 0.570257, 0.57575, 0.580979, 0.586047, 0.5912, 0.598926,
                                         0.598164, 0.602506, 0.610672, 0.609642, 0.615161, 0.618231, 0.623776, 0.630601,
                                         0.633729, 0.639211, 0.640506, 0.644938, 0.652022, 0.656081, 0.650194, 0.655026,
                                         0.658116, 0.662616, 0.673495, 0.683727, 0.700347, 0.716608, 0.724529, 0.736056,
                                         0.738087, 0.73451, 0.715925, 0.766569, 0 };

  const G4double EB_A_TilePMT1[size] = { 0, 0.560603, 0.619651, 0.659854, 0.676481, 0.680032, 0.677058, 0.676412,
                                         0.672087, 0.664434, 0.668327, 0.664378, 0.657041, 0.652984, 0.652358, 0.643966,
                                         0.637226, 0.637363, 0.629543, 0.626034, 0.624046, 0.611643, 0.611461, 0.608826,
                                         0.603121, 0.59707, 0.593578, 0.595048, 0.585366, 0.582533, 0.582907, 0.579518,
                                         0.578838, 0.570625, 0.569588, 0.569577, 0.562259, 0.559874, 0.554432, 0.560379,
                                         0.552937, 0.551042, 0.551915, 0.545489, 0.548241, 0.545509, 0.547181, 0.539352,
                                         0.53119, 0.509364, 0.49399, 0.47872, 0.465228, 0.465742, 0.462053, 0.459088,
                                         0.457982, 0.454907, 0.44405, 0.446072, 0.442728, 0.43975, 0.435672, 0.434726,
                                         0.43167, 0.428066, 0.427944, 0.424454, 0.421825, 0.415072, 0.417075, 0.412803,
                                         0.410633, 0.409595, 0.408562, 0.404673, 0.401011, 0.396696, 0.399913, 0.399069,
                                         0.392831, 0.392426, 0.388547, 0.390691, 0.39008, 0.38702, 0.385888, 0.381104,
                                         0.378728, 0.376933, 0.375037, 0.368767, 0.360503, 0.357983, 0.351359, 0.341704,
                                         0.329379, 0.310274, 0.289243, 0.268271, 0 };

  const G4double EB_A_TilePMT2[size] = { 0, 0.21694, 0.242906, 0.275586, 0.304135, 0.323365, 0.342423, 0.352209,
                                         0.357771, 0.367013, 0.372513, 0.378209, 0.38138, 0.384683, 0.387705, 0.388076,
                                         0.391914, 0.394855, 0.39723, 0.396872, 0.40362, 0.402388, 0.403161, 0.406887,
                                         0.408535, 0.40841, 0.40854, 0.412865, 0.416917, 0.417055, 0.420675, 0.423514,
                                         0.427849, 0.42687, 0.428571, 0.434523, 0.432222, 0.43654, 0.434952, 0.445519,
                                         0.445489, 0.449176, 0.453659, 0.455497, 0.461945, 0.461, 0.468555, 0.469576,
                                         0.474463, 0.47097, 0.480846, 0.491982, 0.507858, 0.531261, 0.543683, 0.550579,
                                         0.559665, 0.560484, 0.557976, 0.560176, 0.561398, 0.563651, 0.567057, 0.571179,
                                         0.572559, 0.57438, 0.580518, 0.581535, 0.581077, 0.585395, 0.589911, 0.593485,
                                         0.59407, 0.601767, 0.601422, 0.604539, 0.611576, 0.611665, 0.620252, 0.621478,
                                         0.625375, 0.631929, 0.631463, 0.639957, 0.644484, 0.652969, 0.658912, 0.664537,
                                         0.669256, 0.671258, 0.674165, 0.677353, 0.676356, 0.6825, 0.685042, 0.691857,
                                         0.689214, 0.698341, 0.688971, 0.682114, 0 };

  const G4double EB_BC_TilePMT1[size] = { 0, 0.595711, 0.626683, 0.651093, 0.657756, 0.65784, 0.653441, 0.651593,
                                          0.647285, 0.647727, 0.648761, 0.638875, 0.642782, 0.637978, 0.637605,
                                          0.631973, 0.625425, 0.619483, 0.619032, 0.614681, 0.607552, 0.603089,
                                          0.602434, 0.597399, 0.594446, 0.591552, 0.58259, 0.584124, 0.583589, 0.575463,
                                          0.571072, 0.569737, 0.56721, 0.563699, 0.557143, 0.555808, 0.548374, 0.551926,
                                          0.546198, 0.54663, 0.54311, 0.541829, 0.537086, 0.538926, 0.535719, 0.535655,
                                          0.533574, 0.529485, 0.522678, 0.507939, 0.495639, 0.485725, 0.477104,
                                          0.473411, 0.470701, 0.46998, 0.465921, 0.461188, 0.45927, 0.458668, 0.450597,
                                          0.450795, 0.44634, 0.44406, 0.440094, 0.438551, 0.435364, 0.433022, 0.428568,
                                          0.426903, 0.427127, 0.42307, 0.423439, 0.420076, 0.420954, 0.414844, 0.413976,
                                          0.412244, 0.412012, 0.406857, 0.406315, 0.405247, 0.402429, 0.400104,
                                          0.397631, 0.396347, 0.395964, 0.391771, 0.386712, 0.387696, 0.381802, 0.37871,
                                          0.370024, 0.366258, 0.359666, 0.351669, 0.340592, 0.325894, 0.301985,
                                          0.287608, 0 };

  const G4double EB_BC_TilePMT2[size] = { 0, 0.234557, 0.253225, 0.287584, 0.317295, 0.336244, 0.347798, 0.360506,
                                          0.366209, 0.374348, 0.380955, 0.384056, 0.390675, 0.394904, 0.398971,
                                          0.400065, 0.400888, 0.405113, 0.409522, 0.409947, 0.409004, 0.412831,
                                          0.417415, 0.418316, 0.419392, 0.423538, 0.423445, 0.426935, 0.428687,
                                          0.432169, 0.431132, 0.436884, 0.438562, 0.440886, 0.435626, 0.441257,
                                          0.441405, 0.447197, 0.450252, 0.452931, 0.453692, 0.456198, 0.459396,
                                          0.467566, 0.46758, 0.471555, 0.475446, 0.477092, 0.479544, 0.479303, 0.484977,
                                          0.497332, 0.509307, 0.523603, 0.532799, 0.5419, 0.544148, 0.542617, 0.545835,
                                          0.553468, 0.548405, 0.554382, 0.552854, 0.555936, 0.559865, 0.560262,
                                          0.563152, 0.566296, 0.566655, 0.573965, 0.575008, 0.580608, 0.583815, 0.58716,
                                          0.591758, 0.592724, 0.596683, 0.603353, 0.606138, 0.607797, 0.614827,
                                          0.617119, 0.622799, 0.624958, 0.628558, 0.633716, 0.642116, 0.642129,
                                          0.646415, 0.657243, 0.658014, 0.658316, 0.660188, 0.662146, 0.664552,
                                          0.667394, 0.671796, 0.678429, 0.66726, 0.68212, 0 };

  const G4double EB_D_TilePMT1[size] = { 1.33955, 0.599738, 0.610622, 0.672334, 0.702253, 0.715142, 0.712311, 0.714369,
                                         0.705503, 0.693468, 0.682178, 0.67153, 0.660052, 0.652514, 0.648368, 0.644583,
                                         0.639319, 0.639816, 0.63707, 0.636003, 0.625064, 0.623169, 0.615539, 0.61004,
                                         0.612989, 0.604285, 0.60211, 0.599982, 0.596287, 0.591812, 0.584206, 0.5781,
                                         0.57989, 0.569279, 0.565374, 0.560215, 0.55897, 0.556069, 0.549574, 0.548953,
                                         0.546773, 0.546131, 0.539646, 0.536223, 0.532459, 0.537577, 0.53022, 0.523327,
                                         0.517909, 0.513224, 0.494817, 0.490156, 0.482894, 0.476794, 0.471798, 0.470219,
                                         0.462529, 0.462844, 0.457363, 0.451507, 0.452108, 0.445656, 0.442162, 0.445021,
                                         0.437892, 0.436602, 0.434234, 0.429348, 0.428972, 0.427012, 0.424738, 0.420361,
                                         0.420803, 0.415714, 0.414458, 0.414393, 0.409867, 0.408234, 0.403759, 0.400254,
                                         0.398556, 0.401519, 0.39726, 0.394622, 0.389495, 0.389013, 0.38498, 0.377572,
                                         0.374728, 0.368278, 0.365855, 0.359556, 0.354557, 0.350851, 0.347773, 0.339743,
                                         0.333385, 0.318545, 0.29119, 0.296315, 0.498716 };

  const G4double EB_D_TilePMT2[size] = { 0.373204, 0.221209, 0.230915, 0.274249, 0.303416, 0.32417, 0.335391, 0.344998,
                                         0.35059, 0.356622, 0.357415, 0.365026, 0.368763, 0.373137, 0.379687, 0.381546,
                                         0.385385, 0.393049, 0.395879, 0.401467, 0.396728, 0.404174, 0.406102, 0.407186,
                                         0.414094, 0.412114, 0.417019, 0.41944, 0.422394, 0.423684, 0.424564, 0.427065,
                                         0.432004, 0.432527, 0.435079, 0.438066, 0.441576, 0.444315, 0.445437, 0.448891,
                                         0.453375, 0.459781, 0.459532, 0.462102, 0.464153, 0.475958, 0.471565, 0.473915,
                                         0.478647, 0.48852, 0.488427, 0.502302, 0.512157, 0.522277, 0.532683, 0.537865,
                                         0.537102, 0.54662, 0.546176, 0.546524, 0.555012, 0.551581, 0.55309, 0.564904,
                                         0.560271, 0.564667, 0.569602, 0.571621, 0.57912, 0.587331, 0.592966, 0.595552,
                                         0.599529, 0.600029, 0.605157, 0.610721, 0.615769, 0.620144, 0.623294, 0.625904,
                                         0.630684, 0.643899, 0.648446, 0.651391, 0.653568, 0.661572, 0.660372, 0.664029,
                                         0.66856, 0.672253, 0.685263, 0.690875, 0.700889, 0.714002, 0.721774, 0.729436,
                                         0.735752, 0.733404, 0.702704, 0.749629, 1.37763 };

  const double R[11] = { 2350., 2450., 2550., 2680., 2810., 2940., 3090., 3240., 3390., 3580., 3770. };

  const double sizerange = (double) size / range;
  const double size2 = (double) size / 2.;

  if (row < 0 || row >= 11) return 0.0;
  const double phi = atan(x / (y + R[row]));
  const int index = int(phi * sizerange + size2);
  if (index < 0 || index >= size) return 0.0;

  G4double amplitude = 0.0;

  if (nDetector != 2) { //Long Barrel or ITC (D4 and C10 cells)
    if (row < 3) { //A layer
      if (PMT) amplitude = LB_A_TilePMT2[index];
      else amplitude = LB_A_TilePMT1[index];
    } else if (row < 9) { //BC layer

      if (PMT) amplitude = LB_BC_TilePMT2[index];
      else amplitude = LB_BC_TilePMT1[index];
    } else { //D layer
      if (PMT) amplitude = LB_D_TilePMT2[index];
      else amplitude = LB_D_TilePMT1[index];
    }
  } else { //Extended barrel
    if (row < 3) { //A layer

      if (PMT) amplitude = EB_A_TilePMT2[index];
      else amplitude = EB_A_TilePMT1[index];
    } else if (row < 7) { //BC layer

      if (PMT) amplitude = EB_BC_TilePMT2[index];
      else amplitude = EB_BC_TilePMT1[index];
    } else { //D layer
      if (PMT) amplitude = EB_D_TilePMT2[index];
      else amplitude = EB_D_TilePMT1[index];
    }
  }

  return amplitude;
}

G4double TileGeoG4SDCalc::Tile_1D_profileSym(int row, G4double x, G4double y, int PMT, int nDetector, int nSide) {
  if (PMT) x *= -1.;

  const double xlow = -0.0495; //dPhi low [rad]
  const double xup = 0.0495; //dPhi up [rad]
  const double range = (xup - xlow); //dPhi range
  const int size = 99;

  const G4double LB_A_TilePMT[size] = { 0.798686, 0.768521, 0.738355, 0.731987, 0.716384, 0.690746, 0.690873, 0.687998,
                                        0.685936, 0.674505, 0.66563, 0.663982, 0.66087, 0.648268, 0.651073, 0.645228,
                                        0.640571, 0.632062, 0.627751, 0.623445, 0.615024, 0.611814, 0.604863, 0.605901,
                                        0.604366, 0.592773, 0.589675, 0.586044, 0.589639, 0.578932, 0.580875, 0.576878,
                                        0.577626, 0.576742, 0.571654, 0.569071, 0.564131, 0.566316, 0.558204, 0.561785,
                                        0.553382, 0.556666, 0.556231, 0.554735, 0.549192, 0.551893, 0.539479, 0.523665,
                                        0.501803, 0.475175, 0.470236, 0.466348, 0.465686, 0.466619, 0.45751, 0.458857,
                                        0.454826, 0.451077, 0.445747, 0.444979, 0.438131, 0.44113, 0.433266, 0.432629,
                                        0.430004, 0.4285, 0.424442, 0.419523, 0.418014, 0.412848, 0.41681, 0.409015,
                                        0.406221, 0.405717, 0.407713, 0.403795, 0.399283, 0.399162, 0.396586, 0.397223,
                                        0.396387, 0.394364, 0.394237, 0.390961, 0.391298, 0.385717, 0.385569, 0.384413,
                                        0.378738, 0.37634, 0.376318, 0.37067, 0.365174, 0.353848, 0.350047, 0.333913,
                                        0.305535, 0.287443, 0.269351 };

  const G4double LB_BC_TilePMT[size] = { 0.840167, 0.782127, 0.724088, 0.709418, 0.692402, 0.681197, 0.674417, 0.669156,
                                         0.664817, 0.662487, 0.661527, 0.64666, 0.639625, 0.639506, 0.634604, 0.633138,
                                         0.629965, 0.622392, 0.611544, 0.612276, 0.609295, 0.599487, 0.600218, 0.595549,
                                         0.59195, 0.585927, 0.588129, 0.582869, 0.581914, 0.57832, 0.566242, 0.566531,
                                         0.566806, 0.562764, 0.559538, 0.554847, 0.555314, 0.552864, 0.550949, 0.544862,
                                         0.547192, 0.546473, 0.543543, 0.539912, 0.540675, 0.532026, 0.529865, 0.511731,
                                         0.506149, 0.486568, 0.483676, 0.470489, 0.47578, 0.469457, 0.472109, 0.466663,
                                         0.46344, 0.461445, 0.45731, 0.450961, 0.451755, 0.447995, 0.444988, 0.441016,
                                         0.438419, 0.437138, 0.434441, 0.430241, 0.425238, 0.430013, 0.427375, 0.423364,
                                         0.423652, 0.417144, 0.417338, 0.414379, 0.413594, 0.407825, 0.409682, 0.406504,
                                         0.401463, 0.403732, 0.403367, 0.400835, 0.396702, 0.394123, 0.38889, 0.387777,
                                         0.390709, 0.384238, 0.378497, 0.37449, 0.368628, 0.362096, 0.354118, 0.341351,
                                         0.318618, 0.311057, 0.303497 };

  const G4double LB_D_TilePMT[size] = { 0.802733, 0.785879, 0.769025, 0.765545, 0.746614, 0.745132, 0.731779, 0.712144,
                                        0.700598, 0.686112, 0.667298, 0.652277, 0.652762, 0.648828, 0.653288, 0.648424,
                                        0.650436, 0.636156, 0.633884, 0.638805, 0.62662, 0.621023, 0.620164, 0.610749,
                                        0.605561, 0.598573, 0.60371, 0.599895, 0.592676, 0.592155, 0.5901, 0.570895,
                                        0.578341, 0.560666, 0.561856, 0.559764, 0.553194, 0.553639, 0.55254, 0.552112,
                                        0.553155, 0.546152, 0.539487, 0.54007, 0.532449, 0.531113, 0.52396, 0.518933,
                                        0.502346, 0.491149, 0.482804, 0.481136, 0.475212, 0.474474, 0.470108, 0.468168,
                                        0.461723, 0.460716, 0.461189, 0.454745, 0.450145, 0.446845, 0.440988, 0.439178,
                                        0.435478, 0.427095, 0.436321, 0.423331, 0.430042, 0.425786, 0.419749, 0.420005,
                                        0.420089, 0.41039, 0.410272, 0.409265, 0.409405, 0.404271, 0.401242, 0.403326,
                                        0.393439, 0.390084, 0.392204, 0.385405, 0.383721, 0.375292, 0.370913, 0.362406,
                                        0.363791, 0.362892, 0.359247, 0.354473, 0.353538, 0.35122, 0.345041, 0.340597,
                                        0.326134, 0.309465, 0.292797 };

  const G4double EBA_A_TilePMT[size] = { 0.792139, 0.77059, 0.749041, 0.732513, 0.711677, 0.707389, 0.700371, 0.68343,
                                         0.685048, 0.677941, 0.682767, 0.67369, 0.673043, 0.654482, 0.653841, 0.657083,
                                         0.639164, 0.642225, 0.645526, 0.629104, 0.625799, 0.613133, 0.614215, 0.604257,
                                         0.606819, 0.605589, 0.596293, 0.593044, 0.59487, 0.589281, 0.585647, 0.582881,
                                         0.577452, 0.583211, 0.567939, 0.569267, 0.572586, 0.563879, 0.564535, 0.560308,
                                         0.549291, 0.553397, 0.553705, 0.549888, 0.559446, 0.543409, 0.541302, 0.520682,
                                         0.501171, 0.482078, 0.471889, 0.464921, 0.467078, 0.461777, 0.467299, 0.454695,
                                         0.456425, 0.450839, 0.442844, 0.447983, 0.445826, 0.438496, 0.439654, 0.436557,
                                         0.426225, 0.435487, 0.427657, 0.424049, 0.424304, 0.421019, 0.420052, 0.413659,
                                         0.412293, 0.412259, 0.409828, 0.40315, 0.40625, 0.403081, 0.402396, 0.401966,
                                         0.406725, 0.397069, 0.393866, 0.399991, 0.390659, 0.386557, 0.388035, 0.387676,
                                         0.389129, 0.377124, 0.371488, 0.36622, 0.368689, 0.359087, 0.353053, 0.33639,
                                         0.318455, 0.296346, 0.274237 };

  const G4double EBA_BC_TilePMT[size] = { 0.765216, 0.742243, 0.719269, 0.703627, 0.69057, 0.674805, 0.670267, 0.668947,
                                          0.659931, 0.660287, 0.660122, 0.651824, 0.645504, 0.643503, 0.64, 0.631806,
                                          0.635015, 0.625643, 0.612319, 0.617659, 0.614942, 0.604726, 0.597412,
                                          0.598118, 0.584243, 0.58876, 0.585558, 0.58119, 0.577408, 0.570998, 0.570063,
                                          0.572889, 0.564722, 0.564901, 0.557248, 0.560575, 0.55793, 0.552173, 0.549076,
                                          0.544946, 0.544614, 0.540688, 0.539512, 0.543909, 0.535156, 0.53569, 0.5294,
                                          0.518111, 0.502598, 0.490091, 0.480693, 0.476542, 0.474851, 0.47228, 0.467972,
                                          0.470615, 0.463034, 0.458858, 0.456306, 0.45142, 0.451959, 0.448925, 0.449493,
                                          0.446992, 0.437707, 0.440169, 0.435363, 0.436332, 0.430786, 0.427524,
                                          0.427315, 0.425062, 0.423207, 0.421341, 0.415979, 0.418567, 0.413195,
                                          0.413133, 0.415054, 0.413901, 0.403158, 0.409199, 0.408162, 0.404106,
                                          0.402345, 0.398181, 0.395647, 0.393395, 0.392197, 0.387046, 0.378265,
                                          0.378248, 0.371159, 0.364677, 0.36052, 0.349385, 0.330943, 0.309066, 0.287188 };

  const G4double EBA_D_TilePMT[size] = { 0.853293, 0.810343, 0.767392, 0.77117, 0.763166, 0.73885, 0.722454, 0.712704,
                                         0.708289, 0.689475, 0.686132, 0.670626, 0.667155, 0.666158, 0.659128, 0.654614,
                                         0.644521, 0.647043, 0.641137, 0.636367, 0.620649, 0.626678, 0.618869, 0.607526,
                                         0.610932, 0.608854, 0.599103, 0.590668, 0.591601, 0.586672, 0.585799, 0.575764,
                                         0.574419, 0.563303, 0.561401, 0.564687, 0.554664, 0.551717, 0.546353, 0.545954,
                                         0.548395, 0.536797, 0.538616, 0.539388, 0.540761, 0.531249, 0.519834, 0.518415,
                                         0.50723, 0.498085, 0.487973, 0.482006, 0.471324, 0.471949, 0.474745, 0.468196,
                                         0.459489, 0.453945, 0.45721, 0.450447, 0.445233, 0.444708, 0.44009, 0.445018,
                                         0.436783, 0.432379, 0.435067, 0.429461, 0.430214, 0.424484, 0.423189, 0.419808,
                                         0.419174, 0.421762, 0.414918, 0.408691, 0.413379, 0.410125, 0.402343, 0.40589,
                                         0.402537, 0.402383, 0.394286, 0.395828, 0.39328, 0.38851, 0.380202, 0.37388,
                                         0.37259, 0.365574, 0.364808, 0.359212, 0.358402, 0.354387, 0.357725, 0.346101,
                                         0.325074, 0.325213, 0.325351 };

  const G4double EBC_A_TilePMT[size] = { 0.769736, 0.749093, 0.728449, 0.716066, 0.699899, 0.708187, 0.697357, 0.680695,
                                         0.680675, 0.671975, 0.682554, 0.672812, 0.665897, 0.652005, 0.650243, 0.654694,
                                         0.638575, 0.633551, 0.642522, 0.628344, 0.62259, 0.612959, 0.613509, 0.60803,
                                         0.60583, 0.605215, 0.597612, 0.589549, 0.590772, 0.592063, 0.589844, 0.585001,
                                         0.576459, 0.575985, 0.568724, 0.573167, 0.568943, 0.5659, 0.565044, 0.558883,
                                         0.551943, 0.54804, 0.555435, 0.550334, 0.557079, 0.542061, 0.535118, 0.515509,
                                         0.504208, 0.488921, 0.476949, 0.467806, 0.470398, 0.463569, 0.467382, 0.455878,
                                         0.453585, 0.447729, 0.442591, 0.444654, 0.443681, 0.439549, 0.438222, 0.435064,
                                         0.427641, 0.430104, 0.424952, 0.427298, 0.424509, 0.42268, 0.417724, 0.411476,
                                         0.416881, 0.412738, 0.407063, 0.406198, 0.400905, 0.400696, 0.40112, 0.403838,
                                         0.405643, 0.395856, 0.393633, 0.397733, 0.390308, 0.383923, 0.387838, 0.385103,
                                         0.38734, 0.375695, 0.370908, 0.36757, 0.363425, 0.358407, 0.339909, 0.329684,
                                         0.323696, 0.318239, 0.312782 };

  const G4double EBC_BC_TilePMT[size] = { 0.766452, 0.739171, 0.711889, 0.694101, 0.687686, 0.681347, 0.673205,
                                          0.666842, 0.662504, 0.661129, 0.659795, 0.651596, 0.644405, 0.637626,
                                          0.638357, 0.632446, 0.631024, 0.625421, 0.612974, 0.617425, 0.611536,
                                          0.606997, 0.600927, 0.600145, 0.586905, 0.583682, 0.588516, 0.580818,
                                          0.575231, 0.576225, 0.571197, 0.573376, 0.566909, 0.566433, 0.554706,
                                          0.557508, 0.557972, 0.551275, 0.549902, 0.544808, 0.544808, 0.541376,
                                          0.539367, 0.542216, 0.540656, 0.532499, 0.523579, 0.513936, 0.505591,
                                          0.492701, 0.485185, 0.476893, 0.473009, 0.472623, 0.470919, 0.468597,
                                          0.462497, 0.458355, 0.456945, 0.451173, 0.450211, 0.448106, 0.449418,
                                          0.444328, 0.437449, 0.442052, 0.434787, 0.438935, 0.4334, 0.430409, 0.42627,
                                          0.425691, 0.424935, 0.418823, 0.415494, 0.419133, 0.415964, 0.418201,
                                          0.413325, 0.414371, 0.404069, 0.410065, 0.409172, 0.405099, 0.403047,
                                          0.395686, 0.39453, 0.394525, 0.392017, 0.388433, 0.380338, 0.376997, 0.373855,
                                          0.365939, 0.356601, 0.343823, 0.336635, 0.326647, 0.316659 };

  const G4double EBC_D_TilePMT[size] = { 0.820525, 0.793705, 0.766885, 0.774946, 0.768826, 0.750764, 0.73799, 0.714447,
                                         0.705622, 0.685429, 0.67571, 0.662537, 0.661018, 0.661392, 0.653707, 0.656853,
                                         0.647954, 0.647615, 0.643691, 0.634402, 0.618658, 0.625813, 0.614812, 0.609879,
                                         0.606417, 0.6063, 0.59889, 0.595173, 0.587621, 0.5865, 0.586086, 0.576425,
                                         0.578589, 0.565547, 0.56289, 0.564154, 0.554402, 0.553343, 0.54999, 0.546815,
                                         0.546518, 0.54017, 0.537399, 0.538152, 0.538575, 0.530318, 0.520146, 0.520792,
                                         0.507983, 0.49819, 0.489932, 0.487399, 0.468589, 0.470491, 0.4717, 0.466595,
                                         0.459, 0.454569, 0.455474, 0.450239, 0.447, 0.442605, 0.441629, 0.442854,
                                         0.436902, 0.431356, 0.435549, 0.428119, 0.428026, 0.420955, 0.417468, 0.417023,
                                         0.414598, 0.415931, 0.410411, 0.40769, 0.403839, 0.407625, 0.39751, 0.401394,
                                         0.40169, 0.396019, 0.394027, 0.392475, 0.382778, 0.3845, 0.379875, 0.371261,
                                         0.372296, 0.364693, 0.3645, 0.35507, 0.357514, 0.352602, 0.350255, 0.341489,
                                         0.323783, 0.31584, 0.307897 };

  const double R[11] = { 2350., 2450., 2550., 2680., 2810., 2940., 3090., 3240., 3390., 3580., 3770. };

  const double sizerange = (double) size / range;
  const double size2 = (double) size / 2.;

  if (row < 0 || row >= 11) return 0.0;
  const double phi = atan(x / (y + R[row]));
  const int index = int(phi * sizerange + size2);
  if (index < 0 || index >= size) return 0.0;

  G4double amplitude = 0.0;

  if (nDetector != 2) { //Long Barrel or ITC (D4 and C10 cells)
    if (row < 3) { //A layer

      amplitude = LB_A_TilePMT[index];
    } else if (row < 9) { //BC layer

      amplitude = LB_BC_TilePMT[index];
    } else { //D layer
      amplitude = LB_D_TilePMT[index];
    }
  } else { //Extended barrel
    if (nSide > 0) {
      if (row < 3) { //A layer

        amplitude = EBA_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBA_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBA_D_TilePMT[index];
      }
    } else {
      if (row < 3) { //A layer

        amplitude = EBC_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBC_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBC_D_TilePMT[index];
      }
    }
  }

  return amplitude;
}

G4double TileGeoG4SDCalc::Tile_1D_profileFunc(int row, G4double x, G4double y, int PMT, int nDetector, int nSide) {
  if (PMT) x *= -1.;

  const double xlow = -0.0495; //dPhi low [rad]
  const double xup = 0.0495; //dPhi up [rad]
  const double range = (xup - xlow); //dPhi range
  const int size = 99;

  const G4double LB_A_TilePMT[size] = { 0.783186, 0.753606, 0.724026, 0.717782, 0.702482, 0.677341, 0.677465, 0.674647,
                                        0.672624, 0.661415, 0.652712, 0.651097, 0.648045, 0.635687, 0.638438, 0.632707,
                                        0.62814, 0.619796, 0.615568, 0.611346, 0.603089, 0.599941, 0.593125, 0.594143,
                                        0.592637, 0.58127, 0.578232, 0.574671, 0.578196, 0.567697, 0.569602, 0.565683,
                                        0.566416, 0.56555, 0.56056, 0.558028, 0.553183, 0.555326, 0.547372, 0.550883,
                                        0.542643, 0.545863, 0.545437, 0.54397, 0.538534, 0.541183, 0.52901, 0.513503,
                                        0.492065, 0.465954, 0.461111, 0.457298, 0.456649, 0.457564, 0.448631, 0.449952,
                                        0.445999, 0.442323, 0.437096, 0.436343, 0.429628, 0.432569, 0.424858, 0.424233,
                                        0.42166, 0.420185, 0.416206, 0.411381, 0.409902, 0.404836, 0.408721, 0.401077,
                                        0.398338, 0.397844, 0.399801, 0.395959, 0.391534, 0.391416, 0.38889, 0.389515,
                                        0.388695, 0.386711, 0.386586, 0.383374, 0.383704, 0.378231, 0.378087, 0.376953,
                                        0.371388, 0.369037, 0.369015, 0.363477, 0.358088, 0.346981, 0.343253, 0.327433,
                                        0.299605, 0.281865, 0.264124 };

  const G4double LB_BC_TilePMT[size] = { 0.823731, 0.766827, 0.709923, 0.69554, 0.678857, 0.667871, 0.661224, 0.656066,
                                         0.651812, 0.649528, 0.648586, 0.63401, 0.627113, 0.626996, 0.62219, 0.620752,
                                         0.617642, 0.610217, 0.599581, 0.600298, 0.597376, 0.58776, 0.588477, 0.583899,
                                         0.580371, 0.574466, 0.576624, 0.571467, 0.570531, 0.567007, 0.555165, 0.555449,
                                         0.555718, 0.551755, 0.548593, 0.543993, 0.544451, 0.542049, 0.540172, 0.534203,
                                         0.536488, 0.535783, 0.53291, 0.52935, 0.530098, 0.521618, 0.519499, 0.501721,
                                         0.496248, 0.47705, 0.474215, 0.461285, 0.466473, 0.460274, 0.462874, 0.457534,
                                         0.454374, 0.452418, 0.448364, 0.442139, 0.442918, 0.439231, 0.436283, 0.432389,
                                         0.429842, 0.428586, 0.425942, 0.421825, 0.41692, 0.421601, 0.419015, 0.415082,
                                         0.415364, 0.408984, 0.409174, 0.406273, 0.405503, 0.399847, 0.401667, 0.398552,
                                         0.393609, 0.395834, 0.395476, 0.392994, 0.388942, 0.386413, 0.381282, 0.380191,
                                         0.383066, 0.376721, 0.371093, 0.367164, 0.361417, 0.355013, 0.347191, 0.334673,
                                         0.312385, 0.304972, 0.29756 };

  const G4double LB_D_TilePMT[size] = { 0.781008, 0.76461, 0.748212, 0.744827, 0.726408, 0.724966, 0.711975, 0.692871,
                                        0.681638, 0.667543, 0.649238, 0.634624, 0.635096, 0.631268, 0.635608, 0.630875,
                                        0.632833, 0.618939, 0.616728, 0.621517, 0.609662, 0.604216, 0.60338, 0.594219,
                                        0.589172, 0.582373, 0.587371, 0.583659, 0.576636, 0.576129, 0.57413, 0.555444,
                                        0.562689, 0.545492, 0.54665, 0.544614, 0.538222, 0.538656, 0.537586, 0.537169,
                                        0.538184, 0.531371, 0.524887, 0.525454, 0.518039, 0.516739, 0.50978, 0.504889,
                                        0.488751, 0.477857, 0.469737, 0.468115, 0.462351, 0.461633, 0.457385, 0.455498,
                                        0.449227, 0.448248, 0.448707, 0.442438, 0.437962, 0.434752, 0.429053, 0.427293,
                                        0.423693, 0.415536, 0.424512, 0.411874, 0.418404, 0.414263, 0.408389, 0.408638,
                                        0.40872, 0.399283, 0.399168, 0.398189, 0.398325, 0.39333, 0.390383, 0.392411,
                                        0.382791, 0.379526, 0.381589, 0.374975, 0.373336, 0.365135, 0.360875, 0.352598,
                                        0.353945, 0.353071, 0.349524, 0.344879, 0.34397, 0.341715, 0.335703, 0.331379,
                                        0.317308, 0.30109, 0.284872 };

  const G4double EBA_A_TilePMT[size] = { 0.771303, 0.750321, 0.729339, 0.713245, 0.692957, 0.688782, 0.681949, 0.665453,
                                         0.667029, 0.660109, 0.664808, 0.65597, 0.655339, 0.637267, 0.636643, 0.6398,
                                         0.622351, 0.625332, 0.628546, 0.612557, 0.609339, 0.597006, 0.598059, 0.588363,
                                         0.590858, 0.58966, 0.580609, 0.577445, 0.579223, 0.573781, 0.570242, 0.567549,
                                         0.562263, 0.567871, 0.553, 0.554293, 0.557525, 0.549047, 0.549685, 0.54557,
                                         0.534843, 0.538841, 0.539141, 0.535424, 0.544731, 0.529115, 0.527064, 0.506986,
                                         0.487988, 0.469398, 0.459476, 0.452692, 0.454792, 0.449631, 0.455007, 0.442735,
                                         0.444419, 0.438981, 0.431196, 0.436199, 0.434099, 0.426962, 0.428089, 0.425074,
                                         0.415013, 0.424032, 0.416408, 0.412895, 0.413144, 0.409945, 0.409003, 0.402779,
                                         0.401448, 0.401415, 0.399048, 0.392546, 0.395564, 0.392479, 0.391812, 0.391393,
                                         0.396027, 0.386624, 0.383506, 0.389469, 0.380383, 0.376389, 0.377828, 0.377479,
                                         0.378893, 0.367205, 0.361717, 0.356587, 0.358992, 0.349642, 0.343766, 0.327542,
                                         0.310078, 0.288551, 0.267023 };

  const G4double EBA_BC_TilePMT[size] = { 0.750306, 0.72778, 0.705254, 0.689917, 0.677114, 0.661656, 0.657207, 0.655912,
                                          0.647072, 0.647421, 0.647259, 0.639123, 0.632926, 0.630965, 0.627529,
                                          0.619495, 0.622642, 0.613453, 0.600388, 0.605624, 0.60296, 0.592943, 0.585772,
                                          0.586464, 0.572859, 0.577288, 0.574148, 0.569866, 0.566157, 0.559872,
                                          0.558955, 0.561727, 0.553718, 0.553894, 0.54639, 0.549653, 0.547059, 0.541414,
                                          0.538377, 0.534328, 0.534002, 0.530153, 0.529, 0.533311, 0.524728, 0.525253,
                                          0.519084, 0.508016, 0.492805, 0.480541, 0.471326, 0.467257, 0.465599,
                                          0.463078, 0.458853, 0.461445, 0.454012, 0.449918, 0.447415, 0.442624,
                                          0.443152, 0.440178, 0.440735, 0.438282, 0.429178, 0.431592, 0.42688, 0.42783,
                                          0.422392, 0.419194, 0.418989, 0.416779, 0.414961, 0.413132, 0.407874,
                                          0.410411, 0.405144, 0.405083, 0.406967, 0.405836, 0.395302, 0.401226,
                                          0.400209, 0.396232, 0.394505, 0.390423, 0.387938, 0.38573, 0.384555, 0.379505,
                                          0.370895, 0.370878, 0.363927, 0.357571, 0.353495, 0.342577, 0.324495,
                                          0.303044, 0.281592 };

  const G4double EBA_D_TilePMT[size] = { 0.824321, 0.782829, 0.741337, 0.744986, 0.737255, 0.713763, 0.697924, 0.688505,
                                         0.684241, 0.666065, 0.662835, 0.647856, 0.644503, 0.64354, 0.636749, 0.632388,
                                         0.622638, 0.625074, 0.619369, 0.614761, 0.599576, 0.6054, 0.597856, 0.586898,
                                         0.590189, 0.588182, 0.578762, 0.570613, 0.571515, 0.566753, 0.565909, 0.556215,
                                         0.554916, 0.544178, 0.54234, 0.545515, 0.535831, 0.532985, 0.527803, 0.527417,
                                         0.529776, 0.518571, 0.520328, 0.521074, 0.5224, 0.513212, 0.502184, 0.500813,
                                         0.490008, 0.481174, 0.471405, 0.46564, 0.455321, 0.455924, 0.458626, 0.4523,
                                         0.443888, 0.438532, 0.441686, 0.435153, 0.430116, 0.429609, 0.425147, 0.429909,
                                         0.421953, 0.417698, 0.420295, 0.41488, 0.415606, 0.410072, 0.408821, 0.405554,
                                         0.404942, 0.407442, 0.40083, 0.394815, 0.399344, 0.3962, 0.388682, 0.392109,
                                         0.38887, 0.388721, 0.380899, 0.382388, 0.379927, 0.375319, 0.367293, 0.361185,
                                         0.35994, 0.353161, 0.352422, 0.347016, 0.346233, 0.342354, 0.345579, 0.33435,
                                         0.314037, 0.314171, 0.314304 };

  const G4double EBC_A_TilePMT[size] = { 0.75139, 0.731238, 0.711087, 0.698999, 0.683217, 0.691307, 0.680736, 0.664471,
                                         0.664451, 0.655958, 0.666285, 0.656776, 0.650026, 0.636464, 0.634745, 0.639089,
                                         0.623354, 0.61845, 0.627208, 0.613367, 0.607751, 0.598349, 0.598886, 0.593538,
                                         0.591391, 0.59079, 0.583368, 0.575498, 0.576691, 0.577951, 0.575785, 0.571057,
                                         0.562719, 0.562257, 0.555168, 0.559506, 0.555382, 0.552412, 0.551577, 0.545562,
                                         0.538788, 0.534978, 0.542196, 0.537217, 0.543801, 0.529141, 0.522364, 0.503222,
                                         0.49219, 0.477268, 0.465581, 0.456656, 0.459186, 0.45252, 0.456242, 0.445012,
                                         0.442774, 0.437058, 0.432042, 0.434056, 0.433106, 0.429073, 0.427777, 0.424694,
                                         0.417448, 0.419852, 0.414824, 0.417114, 0.414391, 0.412605, 0.407768, 0.401669,
                                         0.406945, 0.4029, 0.397361, 0.396516, 0.39135, 0.391146, 0.391559, 0.394212,
                                         0.395974, 0.386421, 0.384251, 0.388253, 0.381005, 0.374772, 0.378594, 0.375924,
                                         0.378108, 0.36674, 0.362067, 0.358809, 0.354763, 0.349865, 0.331808, 0.321826,
                                         0.315981, 0.310654, 0.305327 };

  const G4double EBC_BC_TilePMT[size] = { 0.750846, 0.72412, 0.697394, 0.679968, 0.673684, 0.667474, 0.659498, 0.653264,
                                          0.649015, 0.647668, 0.646361, 0.638329, 0.631284, 0.624643, 0.62536, 0.619569,
                                          0.618176, 0.612686, 0.600493, 0.604854, 0.599084, 0.594638, 0.588691,
                                          0.587926, 0.574955, 0.571798, 0.576533, 0.568992, 0.563519, 0.564492,
                                          0.559566, 0.561701, 0.555366, 0.5549, 0.543412, 0.546156, 0.546611, 0.540051,
                                          0.538705, 0.533715, 0.533715, 0.530353, 0.528384, 0.531176, 0.529648,
                                          0.521657, 0.512918, 0.503472, 0.495296, 0.482669, 0.475306, 0.467183,
                                          0.463378, 0.463, 0.461331, 0.459055, 0.45308, 0.449022, 0.447641, 0.441986,
                                          0.441044, 0.438982, 0.440267, 0.435281, 0.428542, 0.433051, 0.425934,
                                          0.429997, 0.424576, 0.421645, 0.41759, 0.417023, 0.416282, 0.410295, 0.407034,
                                          0.410599, 0.407495, 0.409686, 0.404909, 0.405934, 0.395842, 0.401716,
                                          0.400841, 0.396851, 0.394841, 0.387629, 0.386497, 0.386492, 0.384035,
                                          0.380524, 0.372594, 0.36932, 0.366243, 0.358488, 0.34934, 0.336823, 0.329781,
                                          0.319996, 0.310212 };

  const G4double EBC_D_TilePMT[size] = { 0.795431, 0.769431, 0.743432, 0.751246, 0.745313, 0.727804, 0.71542, 0.692597,
                                         0.684043, 0.664467, 0.655045, 0.642275, 0.640802, 0.641165, 0.633715, 0.636765,
                                         0.628138, 0.627809, 0.624005, 0.615, 0.599738, 0.606674, 0.596009, 0.591227,
                                         0.587871, 0.587758, 0.580574, 0.576971, 0.56965, 0.568563, 0.568162, 0.558797,
                                         0.560894, 0.548251, 0.545675, 0.546901, 0.537446, 0.536421, 0.53317, 0.530092,
                                         0.529804, 0.523651, 0.520964, 0.521694, 0.522104, 0.514099, 0.504239, 0.504865,
                                         0.492447, 0.482954, 0.474949, 0.472493, 0.454258, 0.456102, 0.457274, 0.452325,
                                         0.444963, 0.440667, 0.441544, 0.43647, 0.433329, 0.429069, 0.428122, 0.42931,
                                         0.42354, 0.418164, 0.422228, 0.415026, 0.414935, 0.408081, 0.404701, 0.404269,
                                         0.401918, 0.403211, 0.397859, 0.395222, 0.391488, 0.395159, 0.385353, 0.389118,
                                         0.389405, 0.383908, 0.381977, 0.380472, 0.371072, 0.372741, 0.368258, 0.359907,
                                         0.36091, 0.353539, 0.353353, 0.344211, 0.34658, 0.341819, 0.339543, 0.331046,
                                         0.313881, 0.306181, 0.298481 };

  const double R[11] = { 2350., 2450., 2550., 2680., 2810., 2940., 3090., 3240., 3390., 3580., 3770. };

  const double sizerange = (double) size / range;
  const double size2 = (double) size / 2.;

  if (row < 0 || row >= 11) return 0.0;
  const double phi = atan(x / (y + R[row]));
  const int index = int(phi * sizerange + size2);
  if (index < 0 || index >= size) return 0.0;

  G4double amplitude = 0.0;

  if (nDetector != 2) { //Long Barrel or ITC (D4 and C10 cells)
    if (row < 3) { //A layer

      amplitude = LB_A_TilePMT[index];
    } else if (row < 9) { //BC layer

      amplitude = LB_BC_TilePMT[index];
    } else { //D layer
      amplitude = LB_D_TilePMT[index];
    }
  } else { //Extended barrel
    if (nSide > 0) {
      if (row < 3) { //A layer

        amplitude = EBA_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBA_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBA_D_TilePMT[index];
      }
    } else {
      if (row < 3) { //A layer

        amplitude = EBC_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBC_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBC_D_TilePMT[index];
      }
    }
  }

  return amplitude;
}

G4double TileGeoG4SDCalc::Tile_1D_profileRescaled(int row, G4double x, G4double y, int PMT, int nDetector, int nSide) {
  if (PMT) x *= -1.;

  const double xlow = -0.0495; //dPhi low [rad]
  const double xup = 0.0495; //dPhi up [rad]
  const double range = (xup - xlow); //dPhi range
  const int size = 99;

  const G4double LB_A_TilePMT[size] = { 0.797741, 0.767611, 0.737482, 0.731121, 0.715537, 0.689929, 0.690055, 0.687185,
                                        0.685124, 0.673707, 0.664842, 0.663197, 0.660089, 0.647501, 0.650303, 0.644465,
                                        0.639813, 0.631315, 0.627008, 0.622707, 0.614297, 0.61109, 0.604147, 0.605184,
                                        0.603651, 0.592072, 0.588977, 0.585351, 0.588941, 0.578247, 0.580187, 0.576195,
                                        0.576942, 0.57606, 0.570978, 0.568398, 0.563464, 0.565646, 0.557544, 0.56112,
                                        0.552727, 0.556008, 0.555573, 0.554079, 0.548542, 0.551241, 0.538841, 0.523046,
                                        0.501209, 0.474613, 0.46968, 0.465796, 0.465135, 0.466067, 0.456968, 0.458314,
                                        0.454288, 0.450544, 0.445219, 0.444452, 0.437612, 0.440608, 0.432754, 0.432117,
                                        0.429496, 0.427993, 0.42394, 0.419026, 0.41752, 0.412359, 0.416317, 0.408531,
                                        0.40574, 0.405237, 0.407231, 0.403318, 0.398811, 0.39869, 0.396117, 0.396753,
                                        0.395918, 0.393898, 0.39377, 0.390499, 0.390835, 0.38526, 0.385113, 0.383958,
                                        0.37829, 0.375895, 0.375872, 0.370231, 0.364742, 0.353429, 0.349633, 0.333518,
                                        0.305173, 0.287103, 0.269032 };

  const G4double LB_BC_TilePMT[size] = { 0.83904, 0.781078, 0.723117, 0.708466, 0.691473, 0.680283, 0.673512, 0.668259,
                                         0.663925, 0.661599, 0.66064, 0.645793, 0.638767, 0.638648, 0.633753, 0.632288,
                                         0.62912, 0.621557, 0.610724, 0.611454, 0.608478, 0.598683, 0.599413, 0.59475,
                                         0.591156, 0.585141, 0.58734, 0.582087, 0.581133, 0.577544, 0.565483, 0.565771,
                                         0.566045, 0.562009, 0.558788, 0.554103, 0.554569, 0.552122, 0.55021, 0.544131,
                                         0.546458, 0.545739, 0.542813, 0.539188, 0.539949, 0.531312, 0.529154, 0.511045,
                                         0.50547, 0.485915, 0.483028, 0.469858, 0.475142, 0.468828, 0.471476, 0.466037,
                                         0.462818, 0.460826, 0.456697, 0.450356, 0.451149, 0.447394, 0.444391, 0.440424,
                                         0.437831, 0.436551, 0.433858, 0.429664, 0.424668, 0.429436, 0.426802, 0.422796,
                                         0.423084, 0.416585, 0.416778, 0.413823, 0.413039, 0.407278, 0.409132, 0.405958,
                                         0.400924, 0.40319, 0.402826, 0.400298, 0.39617, 0.393594, 0.388368, 0.387257,
                                         0.390185, 0.383722, 0.377989, 0.373987, 0.368134, 0.36161, 0.353643, 0.340893,
                                         0.31819, 0.31064, 0.30309 };

  const G4double LB_D_TilePMT[size] = { 0.795522, 0.77882, 0.762117, 0.758668, 0.739907, 0.738439, 0.725206, 0.705747,
                                        0.694305, 0.679948, 0.661304, 0.646417, 0.646898, 0.643, 0.64742, 0.642599,
                                        0.644594, 0.630441, 0.62819, 0.633067, 0.620992, 0.615444, 0.614593, 0.605262,
                                        0.600121, 0.593196, 0.598287, 0.594506, 0.587352, 0.586836, 0.584799, 0.565766,
                                        0.573146, 0.555629, 0.556809, 0.554736, 0.548224, 0.548666, 0.547577, 0.547152,
                                        0.548186, 0.541246, 0.534641, 0.535219, 0.527666, 0.526342, 0.519253, 0.514271,
                                        0.497834, 0.486737, 0.478467, 0.476815, 0.470943, 0.470212, 0.465885, 0.463963,
                                        0.457575, 0.456578, 0.457046, 0.45066, 0.446101, 0.442831, 0.437027, 0.435233,
                                        0.431566, 0.423259, 0.432401, 0.419528, 0.426179, 0.421962, 0.415978, 0.416232,
                                        0.416315, 0.406703, 0.406586, 0.405589, 0.405727, 0.400639, 0.397638, 0.399703,
                                        0.389905, 0.38658, 0.388681, 0.381943, 0.380274, 0.371921, 0.367581, 0.359151,
                                        0.360523, 0.359632, 0.35602, 0.351288, 0.350362, 0.348065, 0.341942, 0.337538,
                                        0.323205, 0.306686, 0.290166 };

  const G4double EBA_A_TilePMT[size] = { 0.785637, 0.764265, 0.742893, 0.7265, 0.705835, 0.701582, 0.694623, 0.67782,
                                         0.679425, 0.672377, 0.677163, 0.66816, 0.667518, 0.64911, 0.648474, 0.65169,
                                         0.633917, 0.636953, 0.640227, 0.623941, 0.620663, 0.608101, 0.609174, 0.599297,
                                         0.601839, 0.600618, 0.591399, 0.588176, 0.589987, 0.584444, 0.580839, 0.578097,
                                         0.572712, 0.578424, 0.563277, 0.564594, 0.567886, 0.559251, 0.559901, 0.555709,
                                         0.544783, 0.548855, 0.54916, 0.545375, 0.554854, 0.538948, 0.536859, 0.516408,
                                         0.497057, 0.478121, 0.468015, 0.461105, 0.463244, 0.457987, 0.463463, 0.450963,
                                         0.452678, 0.447139, 0.439209, 0.444305, 0.442167, 0.434897, 0.436045, 0.432974,
                                         0.422726, 0.431913, 0.424147, 0.420568, 0.420822, 0.417563, 0.416604, 0.410264,
                                         0.408909, 0.408875, 0.406464, 0.399841, 0.402915, 0.399772, 0.399093, 0.398666,
                                         0.403387, 0.393809, 0.390633, 0.396707, 0.387452, 0.383384, 0.38485, 0.384494,
                                         0.385934, 0.374029, 0.368439, 0.363214, 0.365663, 0.35614, 0.350155, 0.333629,
                                         0.315841, 0.293913, 0.271986 };

  const G4double EBA_BC_TilePMT[size] = { 0.76425, 0.741305, 0.718361, 0.702739, 0.689698, 0.673952, 0.66942, 0.668102,
                                          0.659098, 0.659453, 0.659288, 0.651001, 0.644689, 0.642691, 0.639191,
                                          0.631008, 0.634213, 0.624853, 0.611546, 0.616879, 0.614165, 0.603962,
                                          0.596658, 0.597363, 0.583505, 0.588016, 0.584818, 0.580456, 0.576679,
                                          0.570277, 0.569343, 0.572166, 0.564008, 0.564188, 0.556544, 0.559867,
                                          0.557225, 0.551476, 0.548382, 0.544258, 0.543926, 0.540005, 0.538831,
                                          0.543222, 0.53448, 0.535014, 0.528731, 0.517457, 0.501963, 0.489472, 0.480086,
                                          0.47594, 0.474252, 0.471684, 0.467381, 0.47002, 0.462449, 0.458279, 0.45573,
                                          0.45085, 0.451388, 0.448358, 0.448926, 0.446427, 0.437154, 0.439613, 0.434813,
                                          0.435781, 0.430242, 0.426984, 0.426776, 0.424525, 0.422673, 0.420809,
                                          0.415454, 0.418039, 0.412673, 0.412611, 0.41453, 0.413378, 0.402649, 0.408683,
                                          0.407647, 0.403596, 0.401837, 0.397678, 0.395147, 0.392898, 0.391702,
                                          0.386557, 0.377788, 0.37777, 0.37069, 0.364217, 0.360065, 0.348944, 0.330525,
                                          0.308675, 0.286825 };

  const G4double EBA_D_TilePMT[size] = { 0.83964, 0.797377, 0.755114, 0.758831, 0.750956, 0.727028, 0.710895, 0.7013,
                                         0.696957, 0.678444, 0.675153, 0.659896, 0.656481, 0.6555, 0.648582, 0.64414,
                                         0.634209, 0.63669, 0.630879, 0.626185, 0.610719, 0.616651, 0.608967, 0.597805,
                                         0.601157, 0.599113, 0.589518, 0.581218, 0.582136, 0.577285, 0.576426, 0.566552,
                                         0.565228, 0.554291, 0.552419, 0.555652, 0.545789, 0.54289, 0.537612, 0.537218,
                                         0.539621, 0.528208, 0.529998, 0.530758, 0.532108, 0.522749, 0.511517, 0.51012,
                                         0.499114, 0.490116, 0.480166, 0.474294, 0.463783, 0.464397, 0.467149, 0.460705,
                                         0.452137, 0.446681, 0.449894, 0.44324, 0.438109, 0.437593, 0.433048, 0.437898,
                                         0.429795, 0.425461, 0.428106, 0.42259, 0.42333, 0.417693, 0.416418, 0.413091,
                                         0.412467, 0.415014, 0.408279, 0.402152, 0.406765, 0.403563, 0.395906, 0.399396,
                                         0.396097, 0.395945, 0.387978, 0.389495, 0.386988, 0.382293, 0.374119, 0.367898,
                                         0.366629, 0.359724, 0.358971, 0.353465, 0.352667, 0.348716, 0.352001, 0.340563,
                                         0.319873, 0.320009, 0.320145 };

  const G4double EBC_A_TilePMT[size] = { 0.765353, 0.744828, 0.724302, 0.711989, 0.695914, 0.704155, 0.693387, 0.676819,
                                         0.676799, 0.668149, 0.678667, 0.668981, 0.662106, 0.648292, 0.646541, 0.650966,
                                         0.634939, 0.629944, 0.638864, 0.624766, 0.619045, 0.609469, 0.610016, 0.604568,
                                         0.602381, 0.60177, 0.594209, 0.586193, 0.587409, 0.588692, 0.586485, 0.58167,
                                         0.573177, 0.572706, 0.565486, 0.569903, 0.565703, 0.562678, 0.561827, 0.555701,
                                         0.5488, 0.54492, 0.552272, 0.5472, 0.553907, 0.538975, 0.532071, 0.512574,
                                         0.501337, 0.486138, 0.474234, 0.465142, 0.467719, 0.46093, 0.464721, 0.453282,
                                         0.451003, 0.44518, 0.440071, 0.442123, 0.441155, 0.437047, 0.435727, 0.432586,
                                         0.425206, 0.427655, 0.422533, 0.424865, 0.422092, 0.420273, 0.415346, 0.409133,
                                         0.414508, 0.410388, 0.404746, 0.403885, 0.398622, 0.398415, 0.398836, 0.401538,
                                         0.403333, 0.393603, 0.391392, 0.395468, 0.388086, 0.381737, 0.38563, 0.38291,
                                         0.385134, 0.373556, 0.368796, 0.365477, 0.361356, 0.356366, 0.337974, 0.327807,
                                         0.321853, 0.316427, 0.311002 };

  const G4double EBC_BC_TilePMT[size] = { 0.7648, 0.737577, 0.710355, 0.692604, 0.686204, 0.679879, 0.671754, 0.665405,
                                          0.661076, 0.659704, 0.658373, 0.650191, 0.643016, 0.636251, 0.636981,
                                          0.631083, 0.629664, 0.624073, 0.611652, 0.616094, 0.610218, 0.605689,
                                          0.599631, 0.598852, 0.58564, 0.582424, 0.587247, 0.579566, 0.573991, 0.574983,
                                          0.569965, 0.57214, 0.565687, 0.565212, 0.553511, 0.556306, 0.55677, 0.550087,
                                          0.548716, 0.543633, 0.543634, 0.540209, 0.538204, 0.541047, 0.539491,
                                          0.531351, 0.52245, 0.512828, 0.504501, 0.491639, 0.484139, 0.475865, 0.47199,
                                          0.471604, 0.469904, 0.467587, 0.4615, 0.457367, 0.45596, 0.4502, 0.44924,
                                          0.44714, 0.448449, 0.44337, 0.436506, 0.441099, 0.433849, 0.437988, 0.432466,
                                          0.429481, 0.425351, 0.424773, 0.424019, 0.41792, 0.414599, 0.41823, 0.415068,
                                          0.4173, 0.412434, 0.413478, 0.403198, 0.409181, 0.40829, 0.404226, 0.402178,
                                          0.394833, 0.393679, 0.393674, 0.391172, 0.387596, 0.379518, 0.376184,
                                          0.373049, 0.365151, 0.355832, 0.343082, 0.33591, 0.325943, 0.315977 };

  const G4double EBC_D_TilePMT[size] = { 0.810214, 0.783731, 0.757247, 0.765208, 0.759164, 0.741329, 0.728715, 0.705468,
                                         0.696755, 0.676816, 0.667219, 0.654211, 0.652711, 0.65308, 0.645492, 0.648598,
                                         0.639812, 0.639477, 0.635602, 0.62643, 0.610883, 0.617949, 0.607085, 0.602215,
                                         0.598796, 0.598681, 0.591363, 0.587693, 0.580236, 0.579129, 0.578721, 0.569181,
                                         0.571318, 0.55844, 0.555816, 0.557064, 0.547434, 0.546389, 0.543078, 0.539943,
                                         0.53965, 0.533382, 0.530646, 0.531389, 0.531807, 0.523653, 0.513609, 0.514248,
                                         0.501599, 0.491929, 0.483775, 0.481274, 0.4627, 0.464578, 0.465772, 0.460731,
                                         0.453232, 0.448857, 0.44975, 0.444581, 0.441382, 0.437043, 0.436079, 0.437289,
                                         0.431411, 0.425935, 0.430075, 0.422739, 0.422647, 0.415665, 0.412222, 0.411782,
                                         0.409388, 0.410704, 0.405253, 0.402566, 0.398764, 0.402502, 0.392514, 0.396349,
                                         0.396642, 0.391042, 0.389076, 0.387543, 0.377968, 0.379668, 0.375101, 0.366595,
                                         0.367618, 0.360109, 0.359919, 0.350608, 0.353021, 0.348171, 0.345853, 0.337198,
                                         0.319714, 0.311871, 0.304028 };

  const double R[11] = { 2350., 2450., 2550., 2680., 2810., 2940., 3090., 3240., 3390., 3580., 3770. };

  const double sizerange = (double) size / range;
  const double size2 = (double) size / 2.;

  if (row < 0 || row >= 11) return 0.0;
  const double phi = atan(x / (y + R[row]));
  const int index = int(phi * sizerange + size2);
  if (index < 0 || index >= size) return 0.0;

  G4double amplitude = 0.0;

  if (nDetector != 2) { //Long Barrel or ITC (D4 and C10 cells)
    if (row < 3) { //A layer

      amplitude = LB_A_TilePMT[index];
    } else if (row < 9) { //BC layer

      amplitude = LB_BC_TilePMT[index];
    } else { //D layer
      amplitude = LB_D_TilePMT[index];
    }
  } else { //Extended barrel
    if (nSide > 0) {
      if (row < 3) { //A layer

        amplitude = EBA_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBA_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBA_D_TilePMT[index];
      }
    } else {
      if (row < 3) { //A layer

        amplitude = EBC_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBC_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBC_D_TilePMT[index];
      }
    }
  }

  return amplitude;
}

G4double TileGeoG4SDCalc::Tile_1D_profileRescaled_zeroBins(int row, G4double x, G4double y, int PMT, int nDetector,
                                                           int nSide, int nZeroBins) {
  if (PMT) x *= -1.;

  const double xlow = -0.0495; //dPhi low [rad]
  const double xup = 0.0495; //dPhi up [rad]
  const double range = (xup - xlow); //dPhi range
  const int size = 99;

  const G4double LB_A_TilePMT[size] = { 0.797741, 0.767611, 0.737482, 0.731121, 0.715537, 0.689929, 0.690055, 0.687185,
                                        0.685124, 0.673707, 0.664842, 0.663197, 0.660089, 0.647501, 0.650303, 0.644465,
                                        0.639813, 0.631315, 0.627008, 0.622707, 0.614297, 0.61109, 0.604147, 0.605184,
                                        0.603651, 0.592072, 0.588977, 0.585351, 0.588941, 0.578247, 0.580187, 0.576195,
                                        0.576942, 0.57606, 0.570978, 0.568398, 0.563464, 0.565646, 0.557544, 0.56112,
                                        0.552727, 0.556008, 0.555573, 0.554079, 0.548542, 0.551241, 0.538841, 0.523046,
                                        0.501209, 0.474613, 0.46968, 0.465796, 0.465135, 0.466067, 0.456968, 0.458314,
                                        0.454288, 0.450544, 0.445219, 0.444452, 0.437612, 0.440608, 0.432754, 0.432117,
                                        0.429496, 0.427993, 0.42394, 0.419026, 0.41752, 0.412359, 0.416317, 0.408531,
                                        0.40574, 0.405237, 0.407231, 0.403318, 0.398811, 0.39869, 0.396117, 0.396753,
                                        0.395918, 0.393898, 0.39377, 0.390499, 0.390835, 0.38526, 0.385113, 0.383958,
                                        0.37829, 0.375895, 0.375872, 0.370231, 0.364742, 0.353429, 0.349633, 0.333518,
                                        0.305173, 0.287103, 0.269032 };

  const G4double LB_BC_TilePMT[size] = { 0.83904, 0.781078, 0.723117, 0.708466, 0.691473, 0.680283, 0.673512, 0.668259,
                                         0.663925, 0.661599, 0.66064, 0.645793, 0.638767, 0.638648, 0.633753, 0.632288,
                                         0.62912, 0.621557, 0.610724, 0.611454, 0.608478, 0.598683, 0.599413, 0.59475,
                                         0.591156, 0.585141, 0.58734, 0.582087, 0.581133, 0.577544, 0.565483, 0.565771,
                                         0.566045, 0.562009, 0.558788, 0.554103, 0.554569, 0.552122, 0.55021, 0.544131,
                                         0.546458, 0.545739, 0.542813, 0.539188, 0.539949, 0.531312, 0.529154, 0.511045,
                                         0.50547, 0.485915, 0.483028, 0.469858, 0.475142, 0.468828, 0.471476, 0.466037,
                                         0.462818, 0.460826, 0.456697, 0.450356, 0.451149, 0.447394, 0.444391, 0.440424,
                                         0.437831, 0.436551, 0.433858, 0.429664, 0.424668, 0.429436, 0.426802, 0.422796,
                                         0.423084, 0.416585, 0.416778, 0.413823, 0.413039, 0.407278, 0.409132, 0.405958,
                                         0.400924, 0.40319, 0.402826, 0.400298, 0.39617, 0.393594, 0.388368, 0.387257,
                                         0.390185, 0.383722, 0.377989, 0.373987, 0.368134, 0.36161, 0.353643, 0.340893,
                                         0.31819, 0.31064, 0.30309 };

  const G4double LB_D_TilePMT[size] = { 0.795522, 0.77882, 0.762117, 0.758668, 0.739907, 0.738439, 0.725206, 0.705747,
                                        0.694305, 0.679948, 0.661304, 0.646417, 0.646898, 0.643, 0.64742, 0.642599,
                                        0.644594, 0.630441, 0.62819, 0.633067, 0.620992, 0.615444, 0.614593, 0.605262,
                                        0.600121, 0.593196, 0.598287, 0.594506, 0.587352, 0.586836, 0.584799, 0.565766,
                                        0.573146, 0.555629, 0.556809, 0.554736, 0.548224, 0.548666, 0.547577, 0.547152,
                                        0.548186, 0.541246, 0.534641, 0.535219, 0.527666, 0.526342, 0.519253, 0.514271,
                                        0.497834, 0.486737, 0.478467, 0.476815, 0.470943, 0.470212, 0.465885, 0.463963,
                                        0.457575, 0.456578, 0.457046, 0.45066, 0.446101, 0.442831, 0.437027, 0.435233,
                                        0.431566, 0.423259, 0.432401, 0.419528, 0.426179, 0.421962, 0.415978, 0.416232,
                                        0.416315, 0.406703, 0.406586, 0.405589, 0.405727, 0.400639, 0.397638, 0.399703,
                                        0.389905, 0.38658, 0.388681, 0.381943, 0.380274, 0.371921, 0.367581, 0.359151,
                                        0.360523, 0.359632, 0.35602, 0.351288, 0.350362, 0.348065, 0.341942, 0.337538,
                                        0.323205, 0.306686, 0.290166 };

  const G4double EBA_A_TilePMT[size] = { 0.785637, 0.764265, 0.742893, 0.7265, 0.705835, 0.701582, 0.694623, 0.67782,
                                         0.679425, 0.672377, 0.677163, 0.66816, 0.667518, 0.64911, 0.648474, 0.65169,
                                         0.633917, 0.636953, 0.640227, 0.623941, 0.620663, 0.608101, 0.609174, 0.599297,
                                         0.601839, 0.600618, 0.591399, 0.588176, 0.589987, 0.584444, 0.580839, 0.578097,
                                         0.572712, 0.578424, 0.563277, 0.564594, 0.567886, 0.559251, 0.559901, 0.555709,
                                         0.544783, 0.548855, 0.54916, 0.545375, 0.554854, 0.538948, 0.536859, 0.516408,
                                         0.497057, 0.478121, 0.468015, 0.461105, 0.463244, 0.457987, 0.463463, 0.450963,
                                         0.452678, 0.447139, 0.439209, 0.444305, 0.442167, 0.434897, 0.436045, 0.432974,
                                         0.422726, 0.431913, 0.424147, 0.420568, 0.420822, 0.417563, 0.416604, 0.410264,
                                         0.408909, 0.408875, 0.406464, 0.399841, 0.402915, 0.399772, 0.399093, 0.398666,
                                         0.403387, 0.393809, 0.390633, 0.396707, 0.387452, 0.383384, 0.38485, 0.384494,
                                         0.385934, 0.374029, 0.368439, 0.363214, 0.365663, 0.35614, 0.350155, 0.333629,
                                         0.315841, 0.293913, 0.271986 };

  const G4double EBA_BC_TilePMT[size] = { 0.76425, 0.741305, 0.718361, 0.702739, 0.689698, 0.673952, 0.66942, 0.668102,
                                          0.659098, 0.659453, 0.659288, 0.651001, 0.644689, 0.642691, 0.639191,
                                          0.631008, 0.634213, 0.624853, 0.611546, 0.616879, 0.614165, 0.603962,
                                          0.596658, 0.597363, 0.583505, 0.588016, 0.584818, 0.580456, 0.576679,
                                          0.570277, 0.569343, 0.572166, 0.564008, 0.564188, 0.556544, 0.559867,
                                          0.557225, 0.551476, 0.548382, 0.544258, 0.543926, 0.540005, 0.538831,
                                          0.543222, 0.53448, 0.535014, 0.528731, 0.517457, 0.501963, 0.489472, 0.480086,
                                          0.47594, 0.474252, 0.471684, 0.467381, 0.47002, 0.462449, 0.458279, 0.45573,
                                          0.45085, 0.451388, 0.448358, 0.448926, 0.446427, 0.437154, 0.439613, 0.434813,
                                          0.435781, 0.430242, 0.426984, 0.426776, 0.424525, 0.422673, 0.420809,
                                          0.415454, 0.418039, 0.412673, 0.412611, 0.41453, 0.413378, 0.402649, 0.408683,
                                          0.407647, 0.403596, 0.401837, 0.397678, 0.395147, 0.392898, 0.391702,
                                          0.386557, 0.377788, 0.37777, 0.37069, 0.364217, 0.360065, 0.348944, 0.330525,
                                          0.308675, 0.286825 };

  const G4double EBA_D_TilePMT[size] = { 0.83964, 0.797377, 0.755114, 0.758831, 0.750956, 0.727028, 0.710895, 0.7013,
                                         0.696957, 0.678444, 0.675153, 0.659896, 0.656481, 0.6555, 0.648582, 0.64414,
                                         0.634209, 0.63669, 0.630879, 0.626185, 0.610719, 0.616651, 0.608967, 0.597805,
                                         0.601157, 0.599113, 0.589518, 0.581218, 0.582136, 0.577285, 0.576426, 0.566552,
                                         0.565228, 0.554291, 0.552419, 0.555652, 0.545789, 0.54289, 0.537612, 0.537218,
                                         0.539621, 0.528208, 0.529998, 0.530758, 0.532108, 0.522749, 0.511517, 0.51012,
                                         0.499114, 0.490116, 0.480166, 0.474294, 0.463783, 0.464397, 0.467149, 0.460705,
                                         0.452137, 0.446681, 0.449894, 0.44324, 0.438109, 0.437593, 0.433048, 0.437898,
                                         0.429795, 0.425461, 0.428106, 0.42259, 0.42333, 0.417693, 0.416418, 0.413091,
                                         0.412467, 0.415014, 0.408279, 0.402152, 0.406765, 0.403563, 0.395906, 0.399396,
                                         0.396097, 0.395945, 0.387978, 0.389495, 0.386988, 0.382293, 0.374119, 0.367898,
                                         0.366629, 0.359724, 0.358971, 0.353465, 0.352667, 0.348716, 0.352001, 0.340563,
                                         0.319873, 0.320009, 0.320145 };

  const G4double EBC_A_TilePMT[size] = { 0.765353, 0.744828, 0.724302, 0.711989, 0.695914, 0.704155, 0.693387, 0.676819,
                                         0.676799, 0.668149, 0.678667, 0.668981, 0.662106, 0.648292, 0.646541, 0.650966,
                                         0.634939, 0.629944, 0.638864, 0.624766, 0.619045, 0.609469, 0.610016, 0.604568,
                                         0.602381, 0.60177, 0.594209, 0.586193, 0.587409, 0.588692, 0.586485, 0.58167,
                                         0.573177, 0.572706, 0.565486, 0.569903, 0.565703, 0.562678, 0.561827, 0.555701,
                                         0.5488, 0.54492, 0.552272, 0.5472, 0.553907, 0.538975, 0.532071, 0.512574,
                                         0.501337, 0.486138, 0.474234, 0.465142, 0.467719, 0.46093, 0.464721, 0.453282,
                                         0.451003, 0.44518, 0.440071, 0.442123, 0.441155, 0.437047, 0.435727, 0.432586,
                                         0.425206, 0.427655, 0.422533, 0.424865, 0.422092, 0.420273, 0.415346, 0.409133,
                                         0.414508, 0.410388, 0.404746, 0.403885, 0.398622, 0.398415, 0.398836, 0.401538,
                                         0.403333, 0.393603, 0.391392, 0.395468, 0.388086, 0.381737, 0.38563, 0.38291,
                                         0.385134, 0.373556, 0.368796, 0.365477, 0.361356, 0.356366, 0.337974, 0.327807,
                                         0.321853, 0.316427, 0.311002 };

  const G4double EBC_BC_TilePMT[size] = { 0.7648, 0.737577, 0.710355, 0.692604, 0.686204, 0.679879, 0.671754, 0.665405,
                                          0.661076, 0.659704, 0.658373, 0.650191, 0.643016, 0.636251, 0.636981,
                                          0.631083, 0.629664, 0.624073, 0.611652, 0.616094, 0.610218, 0.605689,
                                          0.599631, 0.598852, 0.58564, 0.582424, 0.587247, 0.579566, 0.573991, 0.574983,
                                          0.569965, 0.57214, 0.565687, 0.565212, 0.553511, 0.556306, 0.55677, 0.550087,
                                          0.548716, 0.543633, 0.543634, 0.540209, 0.538204, 0.541047, 0.539491,
                                          0.531351, 0.52245, 0.512828, 0.504501, 0.491639, 0.484139, 0.475865, 0.47199,
                                          0.471604, 0.469904, 0.467587, 0.4615, 0.457367, 0.45596, 0.4502, 0.44924,
                                          0.44714, 0.448449, 0.44337, 0.436506, 0.441099, 0.433849, 0.437988, 0.432466,
                                          0.429481, 0.425351, 0.424773, 0.424019, 0.41792, 0.414599, 0.41823, 0.415068,
                                          0.4173, 0.412434, 0.413478, 0.403198, 0.409181, 0.40829, 0.404226, 0.402178,
                                          0.394833, 0.393679, 0.393674, 0.391172, 0.387596, 0.379518, 0.376184,
                                          0.373049, 0.365151, 0.355832, 0.343082, 0.33591, 0.325943, 0.315977 };

  const G4double EBC_D_TilePMT[size] = { 0.810214, 0.783731, 0.757247, 0.765208, 0.759164, 0.741329, 0.728715, 0.705468,
                                         0.696755, 0.676816, 0.667219, 0.654211, 0.652711, 0.65308, 0.645492, 0.648598,
                                         0.639812, 0.639477, 0.635602, 0.62643, 0.610883, 0.617949, 0.607085, 0.602215,
                                         0.598796, 0.598681, 0.591363, 0.587693, 0.580236, 0.579129, 0.578721, 0.569181,
                                         0.571318, 0.55844, 0.555816, 0.557064, 0.547434, 0.546389, 0.543078, 0.539943,
                                         0.53965, 0.533382, 0.530646, 0.531389, 0.531807, 0.523653, 0.513609, 0.514248,
                                         0.501599, 0.491929, 0.483775, 0.481274, 0.4627, 0.464578, 0.465772, 0.460731,
                                         0.453232, 0.448857, 0.44975, 0.444581, 0.441382, 0.437043, 0.436079, 0.437289,
                                         0.431411, 0.425935, 0.430075, 0.422739, 0.422647, 0.415665, 0.412222, 0.411782,
                                         0.409388, 0.410704, 0.405253, 0.402566, 0.398764, 0.402502, 0.392514, 0.396349,
                                         0.396642, 0.391042, 0.389076, 0.387543, 0.377968, 0.379668, 0.375101, 0.366595,
                                         0.367618, 0.360109, 0.359919, 0.350608, 0.353021, 0.348171, 0.345853, 0.337198,
                                         0.319714, 0.311871, 0.304028 };

  const double R[11] = { 2350., 2450., 2550., 2680., 2810., 2940., 3090., 3240., 3390., 3580., 3770. };

  const double sizerange = (double) size / range;
  const double size2 = (double) size / 2.;

  if (row < 0 || row >= 11) return 0.0;
  const double phi = atan(x / (y + R[row]));
  const int index = int(phi * sizerange + size2);
  if (index < nZeroBins || index >= size - nZeroBins) return 0.0;

  G4double amplitude = 0.0;

  if (nDetector != 2) { //Long Barrel or ITC (D4 and C10 cells)
    if (row < 3) { //A layer

      amplitude = LB_A_TilePMT[index];
    } else if (row < 9) { //BC layer

      amplitude = LB_BC_TilePMT[index];
    } else { //D layer
      amplitude = LB_D_TilePMT[index];
    }
  } else { //Extended barrel
    if (nSide > 0) {
      if (row < 3) { //A layer

        amplitude = EBA_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBA_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBA_D_TilePMT[index];
      }
    } else {
      if (row < 3) { //A layer

        amplitude = EBC_A_TilePMT[index];
      } else if (row < 7) { //BC layer

        amplitude = EBC_BC_TilePMT[index];
      } else { //D layer
        amplitude = EBC_D_TilePMT[index];
      }
    }
  }

  return amplitude;
}

