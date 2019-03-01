/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
// Class TileGeoG4CalibSD.
// Calibration Sensitive Detector for TileCal
//
// Author: Gia Khoriauli   <gia@mail.cern.ch>
//
// December, 2005
// Major updates: July, 2013 (Sergey);
//
// DESCRIPTION OF SD METHODS
//
////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TileGeoG4CalibSD.h"
#include "TileGeoG4DMLookupBuilder.h"
#include "TileGeoG4DMLookup.h"
#include "TileEscapedEnergyProcessing.h"
#include "TileG4Interfaces/ITileCalculator.h"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"
#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileSimEvent/TileHitVector.h"

#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4TransportationManager.hh"
#include "G4Trd.hh"
#include "G4ios.hh"

#include "G4SubtractionSolid.hh"
#include "G4BooleanSolid.hh"

#include <iostream>
#include <string>

//static const double tanPi64=0.049126849769467254105343321271314;
static const double M_INV_PI32 = 32. / M_PI;

#ifdef HITSINFO    //added by Sergey
std::ofstream htout("hits.txt");
#endif

//------------------------------------------------------
//      PUBLIC  METHODS
//------------------------------------------------------

//FIND THE TILECAL SUB-SECTION AND ITS MODULE WHERE THE CURRENT
//STEP IS. DETERMINE SOME OF THE IDENTIFIER VALUES, WHICH ARE
//RELATED TO A SUB-SECTION AND A MODULE.
//
//OTHER IDENTIFIER VALUES WILL BE DETERMINED BY IDENITIFIER
//CALCULATORS DUE TO THE PARTICULAR VOLUME WHERE THIS STEP IS LOCATED
bool TileGeoG4CalibSD::FindTileCalibSection() {
  //RESET IDENTIFIER FIELDS
  ResetCellIDFields();
  ResetDMCellIDFields();
  ResetSectSideFlags();

  //reset DM sub detector
  m_dm_subDet = 5;

  //Global Hit Position: x,y,z,phi - added by Sergey
  G4ThreeVector prestepPos = m_aStep->GetPreStepPoint()->GetPosition();
  m_xGlobal = prestepPos.x();
  m_yGlobal = prestepPos.y();
  m_zGlobal = prestepPos.z();
  m_phiGlobal = prestepPos.phi();

  G4VPhysicalVolume* physSection;
  G4LogicalVolume* logVol;
  G4String namePhysSection;
  G4String nameLogiVol;

  //DETERMINE TOUCHABLEHISTORY AND PHYSICAL VOLUME FOR THIS STEP
  //THEY WILL BE USED BY THIS SECTION FINDER AND CALCULATOR METHODS
  m_stepTouchable = (G4TouchableHistory*) (m_aStep->GetPreStepPoint()->GetTouchable());
  m_stepPhysVol = m_stepTouchable->GetVolume();

  //DETERMINE A NAME OF THE STEP VOLUME
  m_nameVol = m_stepPhysVol->GetName();

  //Find level for mother volume of Detector setup
  G4int level = m_stepTouchable->GetHistoryDepth();

  //Debugging of volumes hierarchy
  if (verboseLevel >= 10) {
    for (int i = 0; i < level; i++) {
      physSection = m_stepTouchable->GetVolume(i);
      logVol = physSection->GetLogicalVolume();
      G4cout << " Level--> " << i
             << ", LogVol--> " << (logVol->GetName())
             << ", PhysVol--> " << (physSection->GetName()) << G4endl;
    }
  }

  //Find envelope of TileCal. For Tile or TileCTB configuration setup
  m_detector = 1;
  m_dm_sample = 1;
  m_module = 63;
  if (level>0) {
    nameLogiVol = m_stepTouchable->GetVolume(level - 1)->GetLogicalVolume()->GetName();
    while (nameLogiVol.find("Tile") == G4String::npos && level > 1) {
      level--;
      nameLogiVol = m_stepTouchable->GetVolume(level - 1)->GetLogicalVolume()->GetName();
    }
    if (nameLogiVol.find("Tile") == G4String::npos) {
      m_dm_sample=2;
      if (nameLogiVol.find("CALO::CALO") != G4String::npos) m_detector=2;
      else m_detector=3;
    }
  } else {
    nameLogiVol = m_nameVol;
    m_dm_sample = 3;
  }
  if (verboseLevel >= 10) {
    G4cout << nameLogiVol << " volume level --->" << level
           << ((m_dm_sample!=1)?" NOT A Tile volume":"") << G4endl;
    G4cout << "Global coords: ("
           << m_xGlobal << ", "
           << m_yGlobal << ", "
           << m_zGlobal << ") phi "
           << m_phiGlobal << " Edep "
           << m_aStep->GetTotalEnergyDeposit()
           << G4endl;
  }

  //NEW FEATURE (SINCE RELEASE 11.1.0) - ATLAS TILECAL HAS ONE MORE
  //ENVELOPES FOR CENTRAL BARREL (INCLUDES ALSO FINGERS) AND FOR
  //BOTH EXTENDED BARRELS (THEY INCLUDE FINGER, ITC AND GAP/CRACK
  //ENVELOPES ALSO)
  //SO, IF THERE IS NOT ANY TB SETUP OF TILECAL THEN MOVE DOWN TO
  //THE NEXT LEVEL OF ENVELOPES
  if (!m_tileTB) level--;

  //Check for Hits location wheather it is in a true Tile volume
  //or inside of Tile envelopes
  if (level > 1) {
    --level;  // Move to the level of sections Barrel, EBarrelPos, EBarrelNeg, Gap etc...

    physSection = m_stepTouchable->GetVolume(level - 1);
    namePhysSection = physSection->GetName();

    //determine a side
    m_side = 1;
    if ( (namePhysSection.find("Barrel") != G4String::npos) && (namePhysSection.find("EBarrel") == G4String::npos)) {
    } else {
      if (namePhysSection.find("Neg") != G4String::npos) {
        m_isNegative = true;
        m_side = -1;
        m_dm_subDet = -abs(m_dm_subDet);
      }
    }

    //Retrieve Tile calibration sections
    if (namePhysSection.find("EBarrel") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);
      m_isExtended = true;
      m_detector = 2;
      m_dm_region = 1;
    } else if (namePhysSection.find("Barrel") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
      m_detector = 1;
      m_dm_region = 0;
    } else if (namePhysSection.find("ITC") != G4String::npos) {
      m_isExtended = true;
      m_detector = 3;
      m_dm_region = 2;
      if (level > 2) {
        G4String namePlug = m_stepTouchable->GetVolume(level - 3)->GetName();
        if (namePlug.find("Plug1") != G4String::npos) {
          m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG1);
        } else {
          m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG2);
        }
      } else {
        m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG2);
      }
    } else if (namePhysSection.find("Gap") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG3);
      m_detector = 4;
      m_dm_region = 2;
    } else if (namePhysSection.find("Crack") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG4);
      m_detector = 4;
      m_dm_region = 2;
    } else if (namePhysSection.find("EFinger") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);
      m_isExtended = true;
      m_detector = 2;
      m_dm_region = 1;
    } else if (namePhysSection.find("Finger") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
      m_detector = 1;
      m_dm_region = 0;
    } else if (namePhysSection.find("ESaddle") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);
      m_isExtended = true;
      m_detector = 2;
      m_dm_region = 2;
    } else if (namePhysSection.find("Saddle") != G4String::npos) {
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
      m_detector = 1;
      m_dm_region = 0;
    } else {
      G4cout << "ERROR: FindTileCalibSection : wrong name for tile section --> " << namePhysSection << G4endl;
      return false;
    }

    //determine a module
    if (level > 1) {
      m_dm_sample = 0;
      if (namePhysSection.find("EBarrel") != G4String::npos || namePhysSection.find("ITC") != G4String::npos) {
        if (level > 2) {  //not IrUp,IrDw, IrBox, Iron1, ...
          m_module = m_stepTouchable->GetVolume(level - 2)->GetCopyNo() - 1;
          m_dm_nphi = m_module;
        }
      } else {
        m_module = m_stepTouchable->GetVolume(level - 2)->GetCopyNo() - 1;
        m_dm_nphi = m_module;
      }
    } else {
      return false;
    }

  } else {  //(level!>1)
    if (verboseLevel >= 10)
      G4cout << "FindTileCalibSection : current Step " << "is in Tile envelope "
             << " / Add Step energy to the Default Hit" << G4endl;
    return false;
  }

  //REAL SECTION IS FOUND
  return true;
}

void TileGeoG4CalibSD::CellIDCalculator() {
  int nPeriod, nSamp = 0;
  double zLocal;

  if ( (m_nameVol).find("Period") != G4String::npos) nPeriod = m_stepPhysVol->GetCopyNo();
  else
    // Move up by one level to retrieve TilePeriod replica number
    nPeriod = m_stepTouchable->GetVolume(1)->GetCopyNo();

  // calculate z coordinate of the hit against the center of TilePeriod
  // or z coordinate of the TileWrapper's center against the center of TilePeriod
  if (m_nameVol.find("Wrapper") != G4String::npos) {
    G4ThreeVector WrappCenter = m_stepPhysVol->GetObjectTranslation();
    zLocal = WrappCenter[2];
  } else {
    G4ThreeVector prestepPos = m_aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector localCoord = m_stepTouchable->GetHistory()->GetTopTransform().TransformPoint(prestepPos);
    zLocal = localCoord.z();
  }

  // by using of 'zLocal' calculate Sample of the step corresponding Cell
  switch (m_detector) {

  case 1:  // Barrel
    {
      if (zLocal < m_cSection->sample_ZBound[0]) {//-48.0

        nSamp = 0;
      } else if (zLocal < m_cSection->sample_ZBound[1]) {//-9.0

        nSamp = 1;
      } else if (zLocal < m_cSection->sample_ZBound[2]) {//36.0

        nSamp = 2;
      } else if (zLocal < m_cSection->sample_ZBound[3]) { //74.0

        nSamp = 3;
      } else {
        nSamp = 4;
      }
      break;
    }

  case 2:  // Ext.Barrel
    {
      if (zLocal < m_cSection->sample_ZBound[0]) {//-48.0

        nSamp = 0;
      } else if (zLocal < m_cSection->sample_ZBound[1]) {//6.0

        nSamp = 1;
      } else if (zLocal < m_cSection->sample_ZBound[3]) {//74.0

        nSamp = 2;
      } else {
        nSamp = 4;
      }
      break;
    }

  case 3:  // ITC
    {
      if (zLocal < m_cSection->sample_ZBound[3]) {//17.0

        nSamp = 0;
      } else {
        nSamp = 4;
      }
      break;
    }

  case 4:  // Gap/Crack
    {
      if (zLocal < m_cSection->sample_ZBound[0]) { //1.4 - Gap,  -5.8 - Crack

        nSamp = 0;
      } else {
        nSamp = 1;
      }
      break;
    }
  }

  if (nSamp == 4) {
    //last sample to Girder cell
    if (m_tileTB) DefaultHitIDCalculatorTB(2,m_detector-1,0,m_module);
    else DefaultHitIDCalculator(); //girder cell calculation - default id calculator
    return;
  }
  //retrieve Cell
  m_cell = m_cSection->GetCell(nPeriod, nSamp);

  if (!m_cell) {
    G4cout << "ERROR: CellIDCalculator: zero pointer to the current cell in "
           << m_nameVol <<" " << nPeriod << " " << nSamp << G4endl;
    m_defaultHit = true;
    return;
  }

  m_calibHitType = 0; //Cell Inactive CalibHit

  CellNumCorrectToHit(nPeriod, nSamp);  //added by Sergey: verification of Hit zGlobal wrt Cell zMin,zMax

  //determine remined ID fields
  m_sample = m_cell->sample;
  m_tower = m_cell->tower;
  if (m_tower < 0) m_tower *= -1;
  m_sample--;
  m_tower--;
  //determine the side of Barrel cell
  if ( (m_detector == 1) && (m_cellNum < 0)) m_side = -1; // _cellNum is calculated in CellNumCorrectToHit()

  //force _detector to be 3. That's because ITC & Gap/Crack
  //have the same Detector=3 value in a Cell ID
  if (m_detector == 4) m_detector = 3;

  //just a debugging of CaloCell ID fields
  DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
  if (IDcalc==0 || IDcalc==1) {
    if (m_cellNum < 0)
      m_det_side = -abs(m_detector);
    else
      m_det_side = m_detector;
    if (doHitsTXT) this->HitsInfoPrint(false,1);  // 1-CellIDCalculator
    if (doHitsNTup) m_ntuple->storeHit(m_det_side,m_module,m_tower,m_sample,m_cellNum,
                                       1,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
  }
#endif

  //reetrieve CaloCell ID
  m_id = m_caloCell_ID->cell_id(m_subCalo, m_detector, m_side, m_module, m_tower, m_sample);
}

void TileGeoG4CalibSD::ScintIDCalculator(TileHitData& hitData) {
  m_cell = hitData.cell;  //retrieve Cell from TileGeoG4SDCalc
  if (!m_cell) {
    G4cout << "ERROR: ScintIDCalculator: zero pointer to the current cell!" << G4endl;
    m_defaultHit = true;
    return;
  }

  m_calibHitType = 1;  //Cell Active CalibHit

  //determine remined ID fields
  m_detector = hitData.nDetector;
  m_sample = hitData.nSample - 1;
  m_tower = hitData.nTower - 1;
  m_module = hitData.nModule - 1;
  m_side = hitData.nSide;
  m_cellNum = m_cell->cellNum;

  //just a debugging of CaloCell ID fields
  this->DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
  if (IDcalc==0 || IDcalc==2) {
    if (m_cellNum < 0)
      m_det_side = -abs(m_detector);
    else
      m_det_side = m_detector;
    if (doHitsTXT) this->HitsInfoPrint(false,2);  // 2-ScintIDCalculator
    if (doHitsNTup) m_ntuple->storeHit(m_det_side,m_module,m_tower,m_sample,m_cellNum,
                                       2,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
  }
#endif

  //reetrieve CaloCell & Tile IDs
  m_id = m_caloCell_ID->cell_id(m_subCalo, m_detector, m_side, m_module, m_tower, m_sample);
}

void TileGeoG4CalibSD::PlateCellIDCalculator() {
  double xLocal, yLocal, zLocal;

  G4ThreeVector prestepPos = m_aStep->GetPreStepPoint()->GetPosition();
  if ( (m_nameVol).find("Ir") == G4String::npos) {
    G4ThreeVector PlateCenter = m_stepPhysVol->GetObjectTranslation();
    G4ThreeVector localCoord = m_stepTouchable->GetHistory()->GetTopTransform().TransformPoint(prestepPos);
    xLocal = PlateCenter[0] + localCoord.x();
    yLocal = PlateCenter[1] + localCoord.y();
    zLocal = PlateCenter[2] + localCoord.z();
  } else {
    xLocal = prestepPos.x();    //global x
    yLocal = prestepPos.y();    //global y
    zLocal = prestepPos.phi();  //global phi
    if (yLocal < 0.) {
      m_module = static_cast<int>(trunc(zLocal * M_INV_PI32 + 64.));
      m_dm_nphi = m_module;
    } else {
      G4cout << "ERROR: PlateCellIDCalculator: Ir* _module calculation error in " << m_nameVol << G4endl;
      m_defaultHit = true;
      return;
    }

  }

  m_pCell = 0;
  if ( ( (m_nameVol).find("Front") != G4String::npos) || ( (m_nameVol).find("Ir") != G4String::npos)) {
    if ( (m_nameVol).find("Plate") != G4String::npos) {
      m_pCell = m_cSection->GetTilePlateCell(xLocal, zLocal, 1);   // return FrontPlate cell
    } else {
      if ( (m_nameVol.find("IrUp") != G4String::npos) || (m_nameVol.find("IrDw") != G4String::npos)) {
        m_pCell = m_cSection->GetTilePlateCell(0, 0, 4);
      } else {
        m_pCell = m_cSection->GetTilePlateCell(0, 0, 3);
      }
    }

    if (!m_pCell) {
      G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current plate cell in " << m_nameVol << G4endl;
      m_defaultHit = true;
      return;
    }

    int period = m_pCell->neighbor;
    m_cell = m_cSection->GetCell(period, 0);  // return adjucent cell
  } else if (m_detector == 3 && (m_nameVol).find("EndPlateSh") != G4String::npos) {
    // foil in front of C10 or special D4 is always part of the cell
    m_cell = m_cSection->GetCell(0, 0);   // the only cell in PLUG1 or PLUG2

    m_calibHitType = 0;  //Plate Cell Inactive CalibHit

    //determine remined ID fields
    m_sample = m_cell->sample;
    m_tower = m_cell->tower;
    if (m_tower < 0) m_tower *= -1;
    m_sample--;
    m_tower--;

    //just a debugging of CaloCell ID fields
    DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==3) {
      m_cellNum = m_cell->cellNum;
      if (m_cellNum < 0)
        m_det_side = -abs(m_detector);
      else
        m_det_side = m_detector;
      if (doHitsTXT) HitsInfoPrint(false,3);  // 3-PlateCellIDCalculator
      if (doHitsNTup) m_ntuple->storeHit(m_det_side,m_module,m_tower,m_sample,m_cellNum,
                                         3,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
    }
#endif

    //retrieve CaloCell ID
    m_id = m_caloCell_ID->cell_id(m_subCalo, m_detector, m_side, m_module, m_tower, m_sample);
    return;
  } else {
    //TileEndPlate
    if ( (m_nameVol).find("Plate") != G4String::npos) {
      m_pCell = m_cSection->GetTilePlateCell(xLocal, zLocal, 2);   // return EndPlate cell
    } else {
      //SaddleModule
      if ((m_isNegative) && (m_dm_region == 0)) {
        m_isNegative = false;
        m_pCell = m_cSection->GetTilePlateCell(0, 0, 6);
      } else {
        m_pCell = m_cSection->GetTilePlateCell(0, 0, 5);
      }
    }

    if (!m_pCell) {
      G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current plate cell in " << m_nameVol << G4endl;
      m_defaultHit = true;
      return;
    }

    int sample = m_pCell->sample;

    //the last PlateCell of Barrel EndPlates or Extended outer Endplate
    //is DM cell as long as it's between Girder and Finger volumes.
    //So, it should not be asigned to the real Cell
    if ( (sample != (int) m_cSection->samples.size() - 1) || (m_pCell->detector == 4)) {
      int period = m_pCell->neighbor;
      m_cell = m_cSection->GetCell(period, sample - 1);    // return adjucent cell
    } else {
      //Hit is in the last PlateCell, which shouldn't be
      //assigned to the real Cell (+-D3 or +-D6)
      //Assign it to the section Finger cell.
      GirderCellIDCalculator();
      return;
    }
  }

  //AT THIS POINT WE HAVE DETERMINED THE BOTH PLATE CELL AND ITS
  //NEIGHBOR CELL OF THE CURRENT STEP. FURTHER AddToCell - FLAG
  //DEFINES WE NEED TO STORE THIS PLATE HIT IN THE NEIGHBOR REAL
  //CELL OR STORE IT SEPERATELY AS A DM PLATE CALIBHIT.
  //
  //FLAG POINTS PLATE HIT TO BE STORED IN THE UDJUCENT REAL CELL
  if ((m_addToCell) && ( (m_nameVol).find("Plate") != G4String::npos)) {
    //check Cell validity
    if (!m_cell) {
      G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current cell in " << m_nameVol << G4endl;
      m_defaultHit = true;
      return;
    }

    m_calibHitType = 0; //Plate Cell Inactive CalibHit

    //determine remined ID fields
    m_sample = m_cell->sample;
    m_tower = m_cell->tower;
    if(m_tower < 0) m_tower *= -1;
    m_sample--;
    m_tower--;

    //determine the side for Barrel cell
    if ((m_detector == 1) && (m_cell->tower < 0)) m_side = -1;

    //just a debugging of CaloCell ID fields
    DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==3) {
      m_cellNum = m_cell->cellNum;
      if (m_cellNum < 0)
        m_det_side = -abs(m_detector);
      else
        m_det_side = m_detector;
      if (doHitsTXT) HitsInfoPrint(false,3);  // 3-PlateCellIDCalculator
      if (doHitsNTup) m_ntuple->storeHit(m_det_side,m_module,m_tower,m_sample,m_cellNum,
                                         3,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
    }
#endif

    //retrieve CaloCell ID
    m_id = m_caloCell_ID->cell_id(m_subCalo, m_detector, m_side, m_module, m_tower, m_sample);
  }

  //FLAG POINTS THAT PLATE HITS SHOULD BE STORED AS DM CALIBHITS
  else {
    m_calibHitType = 2; //Plate DM CalibHit
    m_dm_type = 1;//one of the DM Identifier fields

    //check Plate Cell validity
    if(!m_pCell) {
      G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current plate cell in " << m_nameVol << G4endl;
      m_defaultHit = true;
      return;
    }

    //determine remined ID fields
    m_dm_sample = m_pCell->sample;
    //All EndPlate hits have DM sampling = 1
    //All FrontPlate hits have DM sampling = 0
    //force DM sampling of EndPlate hit to be 1
    if(m_dm_sample != 0 ) m_dm_sample = 1;
    //hit tower must not contain side of eta but
    //only its tower absolute value
    m_dm_neta = m_pCell->tower;
    if(m_dm_neta < 0 ) m_dm_neta *= -1;
    m_dm_neta--;
    //Fix #1, 11.03.2006. dm_neta of Barrel or Extended EndPlate Hit needs one more decreament.
    // Correspondence between cell neta and hit neta for EndPlates
    //   Barrel case:
    //        endplate DM hit neta range --   [0 1,2,3] //from DM hit Identifier description
    //        endplate cell neta range   -- [1,2,3,4,5] //from Tile EndPlates DM readout description (DDDB)
    //
    //   Extended case:
    //      endplate DM hit neta range --   [0 1,2]
    //      endplate cell neta range   -- [1,2,3,4]
    //
    // Note: in both cases EndPlate cells with neta=1 go in the adjucent Girder cells
    if( (m_dm_sample == 1) && ((m_pCell->detector == 1) || (m_pCell->detector == 2)) ) m_dm_neta--;

    //Fix #2, 11.03.2006. Due to DM hit Identifier description all ITC
    //(dm_region=2) Front/End Plate Hits must have dm_sample=1
    if((m_pCell->detector == 3) || (m_pCell->detector == 4)) m_dm_sample = 1;

    //determine the side for PlateCell
    if((m_dm_region == 0) && (m_pCell->tower < 0)) {
      m_side = -1;
      m_dm_subDet = -abs(m_dm_subDet);
    }

    //end plate of ext barrel at higher Z -> region = 2
    if((m_dm_region == 1) && (((m_nameVol).find("EndPlate2") != G4String::npos))) {
      m_dm_region = 2;
    }

    //just a debugging of CaloDM ID fields
    //second argument is 1 -> tells to the method
    //that Plate DM cell ID is being debugged
    DebugDMCellIDFields(1);

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==3) {
      if (doHitsTXT) HitsInfoPrint(true,3);  // 3-PlateCellIDCalculator
      if (doHitsNTup) m_ntuple->storeHit(m_dm_subDet,m_dm_nphi,m_dm_neta,m_dm_sample,m_dm_region,
                                         3,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
    }
#endif

    //retrieve CaloDM ID
    m_id = DM_ID_Maker(m_dm_subDet, m_dm_type, m_dm_sample, m_dm_region, m_dm_neta, m_dm_nphi);
  }
}

void TileGeoG4CalibSD::GirderCellIDCalculator() {
  if (m_nameVol.find("Girder") != G4String::npos) {
    G4ThreeVector prestepPos = m_aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector localCoord = m_stepTouchable->GetHistory()->GetTopTransform().TransformPoint(prestepPos);

    double xLocal = localCoord.x();
    m_gCell = m_cSection->GetTileGirderCell(xLocal);
  } else if (!m_isExtended) {
    //Barrel Finger or Barrel EndPlate's last cell
    if ( (m_nameVol.find("EndPlate") != G4String::npos) || (m_nameVol.find("EPHole") != G4String::npos)) {
      // EndPlate and EPHole fix
      if (m_nameVol.find("1") != G4String::npos) {
        // EndPlate1, EPHole1 : Neg side
        m_side = -1;
        m_dm_subDet = -abs(m_dm_subDet);
      }
    }
    int nGirderCell = static_cast<int>( (m_cSection->girderCells).size());
    if (m_side > 0)
      //Finger on the positive side
      m_gCell = m_cSection->girderCells[nGirderCell - 1];
    else
      //Finger on the negative side
      m_gCell = m_cSection->girderCells[nGirderCell - 2];
  } else {
    //Extended Finger or Extended EndPlate's last cell
    int nGirderCell = static_cast<int>( (m_cSection->girderCells).size());
    m_gCell = m_cSection->girderCells[nGirderCell - 1];
  }

  m_calibHitType = 3; //Girder DM CalibHit
  m_dm_type = 1;

  if (!m_gCell) {
    G4cout << "ERROR: GirderCellIDCalculator: zero pointer to the current girder cell in " << m_nameVol << G4endl;
    m_defaultHit = true;
    return;
  }

  //determine remined ID fields
  m_dm_sample = m_gCell->sample;
  m_dm_neta = m_gCell->tower;
  if (m_dm_neta < 0) m_dm_neta *= -1;
  m_dm_neta--;
  //determine the side for GirderCell
  if ( (m_dm_region == 0) && (m_gCell->tower < 0)) {
    m_side = -1;
    m_dm_subDet = -abs(m_dm_subDet);
  }

  //just a debugging of CaloDM ID fields
  //second argument is 2 -> tells to the method
  //that Girder || Finger DM cell is being debugged
  DebugDMCellIDFields(2);

#ifdef HITSINFO    //added by Sergey
  if (IDcalc==0 || IDcalc==4) {
    if (doHitsTXT) HitsInfoPrint(true,4);  // 4-GirderCellIDCalculator
    if (doHitsNTup) m_ntuple->storeHit(m_dm_subDet,m_dm_nphi,m_dm_neta,m_dm_sample,m_dm_region,
                                       4,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
  }
#endif

  //retrieve CaloDM ID
  m_id = DM_ID_Maker(m_dm_subDet, m_dm_type, m_dm_sample, m_dm_region, m_dm_neta, m_dm_nphi);
}

void TileGeoG4CalibSD::DefaultHitIDCalculator() {
  const double zBarrelMaxPos = m_lookupDM->zBarrMaxPos;
  const double zBarrelMaxNeg = m_lookupDM->zBarrMaxNeg;
  const double dzITC = m_lookupDM->zLegngthITC;
  const double dzEBarrelModule = m_lookupDM->dzExtBarrMod;
  const double zBarrelModuleMax = m_lookupDM->dzBarrMod / 2.0;

  const double zEBarrelModuleMaxPos = zBarrelMaxPos + dzITC + dzEBarrelModule;
  const double zEBarrelModuleMaxNeg = zBarrelMaxNeg - dzITC - dzEBarrelModule;

  const double drFrontPlate = m_lookupDM->dRFront;
  const double dzEndPlate = m_lookupDM->dZEnd;
  const double dzEndPlateSh = m_lookupDM->dZEndSh;

  const double dzBarrelPeriod = m_lookupDM->dzBarrPeriod; //18.25762
  const double dzEBarrelPeriod = m_lookupDM->dzExtBarrPeriod; //18.25358

  const double rBarrelMin = m_lookupDM->rBMin;
  const double rBarrelMax = m_lookupDM->rBMax;
  const double rGirderMin = m_lookupDM->rGirdMin;
  //ITC region parameters
  const double rPlug1Min = m_lookupDM->rP1Min;
  const double rPlug2Min = m_lookupDM->rP2Min;
  const double rGapMax = m_lookupDM->rGapMax;
  const double rGapMin = m_lookupDM->rGapMin;
  const double rCrackMax = m_lookupDM->rCrMax;
  const double rCrackMin = m_lookupDM->rCrMin;

  double rBarrelCenter = (rBarrelMin + drFrontPlate + rGirderMin) / 2;

  double zBarrelActiveMax = zBarrelModuleMax;
  double zEBarrelActiveMinPos = zBarrelMaxPos + dzITC;
  double zEBarrelActiveMaxPos = zBarrelMaxPos + dzITC + dzEBarrelModule;
  double zEBarrelActiveMinNeg = zBarrelMaxNeg - dzITC;
  double zEBarrelActiveMaxNeg = zBarrelMaxNeg - dzITC - dzEBarrelModule;

  if (!m_addToCell) {
    zBarrelActiveMax -= dzEndPlate;
    zEBarrelActiveMinPos += dzEndPlateSh;
    zEBarrelActiveMaxPos -= dzEndPlate;
    zEBarrelActiveMinNeg -= dzEndPlateSh;
    zEBarrelActiveMaxNeg += dzEndPlate;

  }

  double drAddGirder = 40; //4 CLHEP::cm to girder

  bool isDMHit = true;

  if (!m_addToGirder) drAddGirder = 0;

  double rGlobal = hypot(m_xGlobal, m_yGlobal); // radius

  if (m_yGlobal >= 0.) {
    m_module = static_cast<int>(trunc(m_phiGlobal * M_INV_PI32));
  } else {
    m_module = static_cast<int>(trunc(m_phiGlobal * M_INV_PI32 + 64.));
    if (m_module > 63) m_module = 63; //-0 phi fix
  }

  m_dm_nphi = m_module;

  if (m_zGlobal >= 0.) {
    m_side = 1;
    m_dm_subDet = 5;
  } else {
    m_side = -1;
    m_dm_subDet = -5;
  }
  m_dm_type = 1;

  bool isZinActiveRegion = false;
  if (fabs(m_zGlobal) <= zBarrelActiveMax) {
    //barrel active zone
    isZinActiveRegion = true;
  } else if ( ( (m_zGlobal >= zEBarrelActiveMaxNeg) && (m_zGlobal <= zEBarrelActiveMinNeg))
              || ( (m_zGlobal >= zEBarrelActiveMinPos) && (m_zGlobal <= zEBarrelActiveMaxPos))) {
    //ext.barrel active zone
    isZinActiveRegion = true;
  }

  if ( (m_zGlobal < zBarrelMaxPos) && (m_zGlobal > zBarrelMaxNeg)) {
    //barrel section
    m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
    m_dm_region = 0;
    m_detector = 1;

    if ( (rGlobal >= rGirderMin) || ( (rGlobal >= rGirderMin - drAddGirder) && (isZinActiveRegion))) {
      //girder-finger
      m_dm_sample = 2;
      int nGirderCell = static_cast<int>( (m_cSection->girderCells).size());

      if (fabs(m_zGlobal) >= zBarrelModuleMax - dzEndPlate) {
        //finger
        if (m_zGlobal > 0) {
          m_gCell = m_cSection->girderCells[nGirderCell - 1];
        } else {
          m_gCell = m_cSection->girderCells[nGirderCell - 2];
        }
      } else {
        m_gCell = m_cSection->GetTileGirderCell(m_zGlobal);
      }

      m_dm_neta = m_gCell->tower;
      if (m_dm_neta < 0) m_dm_neta *= -1;
      m_dm_neta--;
    } else if (isZinActiveRegion) {
      if (rGlobal < (rBarrelMin + drFrontPlate) && (!m_addToCell)) {
        //frontPlate if not AddToCell
        m_dm_sample = 0;
        m_pCell = m_cSection->GetTilePlateCell(m_zGlobal, 0, 1);
        m_dm_neta = m_pCell->tower;
        if (m_dm_neta < 0) m_dm_neta *= -1;
        m_dm_neta--;
      } else {
        //barrel
        isDMHit = false;
        int nSamp, nPeriod;
        double dzInBarrel = 0;

        if (rGlobal < m_cSection->sample_ZBound[0] + rBarrelCenter) {
          nSamp = 0;
        } else if (rGlobal < m_cSection->sample_ZBound[1] + rBarrelCenter) {
          nSamp = 1;
        } else if (rGlobal < m_cSection->sample_ZBound[2] + rBarrelCenter) {
          nSamp = 2;
        } else {
          nSamp = 3;
        }

        dzInBarrel = m_zGlobal - ( (-1) * zBarrelModuleMax + dzEndPlate);
        if (dzInBarrel > 2 * (zBarrelModuleMax - dzEndPlate)) {
          dzInBarrel = 2 * (zBarrelModuleMax - dzEndPlate); //last period is shorter
        }
        if (dzInBarrel < 0) {
          dzInBarrel = 0;
        }

        nPeriod = static_cast<int>(trunc(dzInBarrel / dzBarrelPeriod));
        m_cell = m_cSection->GetCell(nPeriod, nSamp);

        CellNumCorrectToHit(nPeriod, nSamp);  //added by Sergey: verification of Hit zGlobal wrt Cell zMin,zMax

        m_sample = m_cell->sample;
        m_tower = m_cell->tower;
        if (m_tower < 0) m_tower *= -1;
        m_sample--;
        m_tower--;
        //determine the side of Barrel cell
        if (m_cellNum < 0) { // _cellNum is calculated in CellNumCorrectToHit()
          m_side = -1;
        } else {
          m_side = 1;
        }
      }
    } else {
      //DM added to endplate
      m_dm_sample = 1;
      double rPlate = rGlobal - (rBarrelMin + rBarrelMax) / 2.0;
      double zPlate = m_zGlobal;
      if (zPlate > zBarrelModuleMax) zPlate = zBarrelModuleMax;
      if (zPlate < (-1) * zBarrelModuleMax) zPlate = (-1) * zBarrelModuleMax;
      m_pCell = m_cSection->GetTilePlateCell(zPlate, rPlate, 2);
      m_dm_neta = m_pCell->tower;
      if (m_dm_neta < 0) m_dm_neta *= -1;
      m_dm_neta -= 2;
      if (m_dm_neta < 0) m_dm_neta = 0; //possible dm_neta=-1
    }
  } else if ( (m_zGlobal < zBarrelMaxPos + dzITC) && (m_zGlobal > zBarrelMaxNeg - dzITC)) {
    //ITC section
    m_detector = 3;
    m_dm_region = 2;
    if (rGlobal < rGirderMin - drAddGirder) {
      //modules
      isDMHit = false;
      int nSamp = 0;
      double rCenter = 0;
      if (rGlobal > rPlug1Min) {
        //Plug1
        m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG1);
        nSamp = 0;
      } else if (rGlobal > rPlug2Min) {
        //Plug2
        m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG2);
        nSamp = 0;
      } else if (rGlobal > rGapMin) {
        //Gap
        m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG3);
        rCenter = (rGapMin + rGapMax) / 2.0;
        if (rGlobal - rCenter < m_cSection->sample_ZBound[0]) {
          nSamp = 0;
        } else {
          nSamp = 1;
        }
      } else {
        //Crack
        m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG4);
        rCenter = (rCrackMin + rCrackMax) / 2.0;
        if (rGlobal - rCenter < m_cSection->sample_ZBound[0]) {
          nSamp = 0;
        } else {
          nSamp = 1;
        }

      }
      m_cell = m_cSection->GetCell(0, nSamp);

      m_sample = m_cell->sample;
      m_tower = m_cell->tower;
      m_sample--;
      m_tower--;
      m_cellNum = m_cell->cellNum;    //added by Sergey
    } else {
      //girder
      m_dm_sample = 2;
      m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG1);
      m_gCell = m_cSection->GetTileGirderCell(0);
      m_dm_neta = m_gCell->tower;
      if (m_dm_neta < 0) m_dm_neta *= -1;
      m_dm_neta--;
    }

  } else {
    //ext.barrel region
    m_detector = 2;
    m_dm_region = 1;

    m_cSection = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);

    double zEBarrelModuleCenter;
    if (m_zGlobal > 0) {
      zEBarrelModuleCenter = zBarrelMaxPos + dzITC + dzEBarrelModule / 2.0;
    } else {
      zEBarrelModuleCenter = fabs(zBarrelMaxNeg) + dzITC + dzEBarrelModule / 2.0;
    }

    if ( (rGlobal >= rGirderMin) || ( (rGlobal >= rGirderMin - drAddGirder) && (isZinActiveRegion))) {
      //girder-finger
      m_dm_sample = 2;

      int nGirderCell = static_cast<int>( (m_cSection->girderCells).size());
      if ( (m_zGlobal >= zEBarrelModuleMaxPos - dzEndPlate) || (m_zGlobal <= zEBarrelModuleMaxNeg + dzEndPlate)) {
        m_gCell = m_cSection->girderCells[nGirderCell - 1];
      } else {
        m_gCell = m_cSection->GetTileGirderCell(fabs(m_zGlobal) - zEBarrelModuleCenter);
      }

      m_dm_neta = m_gCell->tower;
      if (m_dm_neta < 0) m_dm_neta *= -1;
      m_dm_neta--;
    } else if (isZinActiveRegion) {
      if (rGlobal < (rBarrelMin + drFrontPlate) && (!m_addToCell)) {
        //frontPlate if not AddToCell
        m_dm_sample = 0;
        m_pCell = m_cSection->GetTilePlateCell(fabs(m_zGlobal) - zEBarrelModuleCenter, 0, 1);
        m_dm_neta = m_pCell->tower;
        if (m_dm_neta < 0) m_dm_neta *= -1;
        m_dm_neta--;
      } else {
        //ext.barrel active cells
        isDMHit = false;
        int nSamp, nPeriod;
        double dzInExtBarrel;

        if (rGlobal < m_cSection->sample_ZBound[0] + rBarrelCenter) {
          nSamp = 0;
        } else if (rGlobal < m_cSection->sample_ZBound[1] + rBarrelCenter) {
          nSamp = 1;
        } else {
          nSamp = 2;
        }

        dzInExtBarrel = fabs(m_zGlobal) - (zEBarrelModuleCenter - dzEBarrelModule / 2.0 + dzEndPlateSh);
        if (dzInExtBarrel >= dzEBarrelModule - dzEndPlateSh - dzEndPlate) {
          dzInExtBarrel = dzEBarrelModule - dzEndPlateSh - dzEndPlate - dzEBarrelPeriod / 2.0; //center of last period
        }
        if (dzInExtBarrel < 0) {
          dzInExtBarrel = 0;
        }

        nPeriod = static_cast<int>(trunc(dzInExtBarrel / dzEBarrelPeriod));
        m_cell = m_cSection->GetCell(nPeriod, nSamp);

        m_sample = m_cell->sample;
        m_tower = m_cell->tower;
        m_sample--;
        m_tower--;
        m_cellNum = m_cell->cellNum;    //added by Sergey
      }

    } else {
      //endplate
      m_dm_sample = 1;
      double rPlate = rGlobal - (rBarrelMin + rBarrelMax) / 2.0;
      double zPlate = fabs(m_zGlobal) - zEBarrelModuleCenter;
      if (zPlate > dzEBarrelModule / 2.0) zPlate = dzEBarrelModule / 2.0;
      m_pCell = m_cSection->GetTilePlateCell(zPlate, rPlate, 2);
      m_dm_neta = m_pCell->tower;
      m_dm_neta -= 2;
      if (m_dm_neta < 0) m_dm_neta = 0;

      if ( (m_zGlobal > zEBarrelActiveMaxPos) || (m_zGlobal < zEBarrelActiveMaxNeg)) {
        m_dm_region = 2;
      }
    }

  }

  if (verboseLevel >= 10) G4cout << "Default Hit Calculator: processed" << G4endl;

  if (isDMHit) {  //dm hits
    if (m_dm_sample == 2) DebugDMCellIDFields(2);
    else DebugDMCellIDFields(1);

    m_calibHitType = 5; //Default DM CalibHit

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==6) {
      if (doHitsTXT) HitsInfoPrint(true,6);  // 6-DefaultHitIDCalculator-DM hit
      if (doHitsNTup) m_ntuple->storeHit(m_dm_subDet,m_dm_nphi,m_dm_neta,m_dm_sample,m_dm_region,
                                         6,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
    }
#endif

    m_id = DM_ID_Maker(m_dm_subDet, m_dm_type, m_dm_sample, m_dm_region, m_dm_neta, m_dm_nphi);

  } else {  //Inactive Cell hits
    DebugCellIDFields();

    m_calibHitType = 0; //Default Cell Inactive CalibHit

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==5) {
      if (m_cellNum < 0)
        m_det_side = -abs(m_detector);
      else
        m_det_side = m_detector;
      if (doHitsTXT) HitsInfoPrint(false,5);  // 5-DefaultHitIDCalculator-Cell hit
      if (doHitsNTup) m_ntuple->storeHit(m_det_side,m_module,m_tower,m_sample,m_cellNum,
                                         5,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
    }
#endif

    m_id = m_caloCell_ID->cell_id(m_subCalo, m_detector, m_side, m_module, m_tower, m_sample);
  }

}

void TileGeoG4CalibSD::DefaultHitIDCalculatorTB(int sample, int region, int eta, int phi)
{
  m_calibHitType = 5;

  m_dm_subDet = 5;
  m_dm_type = 1;
  m_dm_sample = std::max(0,std::min(3,sample));
  m_dm_region = std::max(0,std::min(2,region));
  m_dm_neta = eta;
  m_dm_nphi = phi;

  if (verboseLevel >= 10) G4cout << "Default Hit Calculator for TB processed" << G4endl;

  DebugDMCellIDFields(-1);

#ifdef HITSINFO    //added by Sergey
  if (IDcalc==0 || IDcalc==6) {
    if (doHitsTXT) HitsInfoPrint(true,6);  // 6-DefaultHitIDCalculator-DM hit
    if (doHitsNTup) m_ntuple->storeHit(m_dm_subDet,m_dm_nphi,m_dm_neta,m_dm_sample,m_dm_region,
        6,m_nEvent,m_xGlobal,m_yGlobal,m_zGlobal,m_E_tot);
  }
#endif

  m_id = DM_ID_Maker(m_dm_subDet, m_dm_type, m_dm_sample, m_dm_region, m_dm_neta, m_dm_nphi);
}

//-----------------------------------------------------------------------------------
//      PROTECTED  METHODS
//        To avoid an users potential harm intervention in the SD funcionality
//-----------------------------------------------------------------------------------
//DM Hit ID MAKER
Identifier TileGeoG4CalibSD::DM_ID_Maker(int subDet, int nType, int nSample, int nRegion, int nTower, int nModule) {
  Identifier caloDMID;
  try {
    caloDMID = m_caloDM_ID->zone_id(subDet, nType, nSample, nRegion, nTower, nModule, true);
  } catch (const CaloID_Exception & CalEx) {
    DebugDMCellIDFields(nSample);
    G4cout << "ERROR: M_ID_Maker() - WRONG ID -> " << CalEx.message() << G4endl;
  }

  return caloDMID;
}

//***SIMPLE ENERGY COUNTER****
void TileGeoG4CalibSD::EnergiesSimpleCounter() {

  m_E_em += m_result.energy[CaloG4::SimulationEnergies::kEm];
  m_E_nonem += m_result.energy[CaloG4::SimulationEnergies::kNonEm];
  m_E_invisible += m_result.energy[CaloG4::SimulationEnergies::kInvisible0];
  m_E_escaped += m_result.energy[CaloG4::SimulationEnergies::kEscaped];

  // calculate E_tot for current event only
  m_E_tot = m_result.energy[CaloG4::SimulationEnergies::kEm] + m_result.energy[CaloG4::SimulationEnergies::kNonEm]
    + m_result.energy[CaloG4::SimulationEnergies::kInvisible0]
    + m_result.energy[CaloG4::SimulationEnergies::kEscaped];

  if (verboseLevel >= 10)
    G4cout << " Hit Total energy " << m_E_tot
           << " EM "      << m_result.energy[CaloG4::SimulationEnergies::kEm]
           << " NonEM "   << m_result.energy[CaloG4::SimulationEnergies::kNonEm]
           << " Invis "   << m_result.energy[CaloG4::SimulationEnergies::kInvisible0]
           << " Escaped " << m_result.energy[CaloG4::SimulationEnergies::kEscaped]
           << G4endl;
}

void TileGeoG4CalibSD::DebugCellIDFields() {
  if (verboseLevel >= 10) {
    G4cout << "---- CELL ----" << G4endl;
    G4cout << "Detector >> " << m_cell->detector<< G4endl;
    G4cout << "Numm     >> " << m_cell->cellNum << G4endl;
    G4cout << "Tower    >> " << m_cell->tower << G4endl;
    G4cout << "Sample   >> " << m_cell->sample << G4endl;

    G4cout << "---- HIT ----" << G4endl;
    G4cout << "Detector >> " << m_detector << G4endl;
    G4cout << "Side     >> " << m_side << G4endl;
    G4cout << "Module   >> " << m_module << G4endl;
    G4cout << "Tower    >> " << m_tower << G4endl;
    G4cout << "Sample   >> " << m_sample << G4endl;
    G4cout << "Energy   >> " << m_E_tot << G4endl;
    G4cout << "---- END ----" << G4endl;
  }
}

void TileGeoG4CalibSD::DebugDMCellIDFields(int sample) {
  int detector, tower;
  if (sample < 0) {
    detector = sample;
    tower = 0;
  } else if (sample == 2) {
    // Girders || Fingers
    detector = m_gCell->detector;
    tower = m_gCell->tower;
  } else {
    // End || Front plates
    detector = m_pCell->detector;
    tower = m_pCell->tower;
  }

  if (verboseLevel >= 10) {
    G4cout << "---- DM CELL ----" << G4endl;
    G4cout << "Detector >> " << detector << G4endl;
    G4cout << "Tower    >> " << tower << G4endl;

    G4cout << "---- DM HIT ----" << G4endl;
    G4cout << "SubDet   >> " << m_dm_subDet << G4endl;
    G4cout << "Type     >> " << m_dm_type << G4endl;
    G4cout << "Sample   >> " << m_dm_sample << G4endl;
    G4cout << "Region   >> " << m_dm_region << G4endl;
    G4cout << "NEta     >> " << m_dm_neta << G4endl;
    G4cout << "NPhi     >> " << m_dm_nphi << G4endl;
    G4cout << "Energy   >> " << m_E_tot << G4endl;
    G4cout << "---- DM END ----" << G4endl;
  }
}

void TileGeoG4CalibSD::DebugEnergies() {
  m_E_tot = m_E_em + m_E_nonem + m_E_invisible + m_E_escaped + m_tile_eep->GetEnergy5();

  if (verboseLevel >= 5) {
    G4cout << " EndOfEvent" << G4endl;
    G4cout <<" ---------------------------------------------------------- "<< G4endl;
    G4cout <<" TOTAL ENERGY OF EVENT         = "<<m_E_tot << G4endl;
    G4cout <<" ------------------------------------ "<< G4endl;
    G4cout <<" EM COMPONENT OF ENERGY        = "<<m_E_em << G4endl;
    G4cout <<" ------------------------------------ "<< G4endl;
    G4cout <<" NON_EM COMPONENT OF ENERGY    = "<<m_E_nonem << G4endl;
    G4cout <<" ------------------------------------ "<< G4endl;
    G4cout <<" INVISIBLE COMPONENT OF ENERGY = "<<m_E_invisible << G4endl;
    G4cout <<" ------------------------------------ "<< G4endl;
    G4cout <<" ESCAPED COMPONENT OF ENERGY   = "<<m_E_escaped << G4endl;
    G4cout <<" --------------------------------------------------------- "<< G4endl;
    G4cout <<" energy5 (missed energy)       = "<<m_tile_eep->GetEnergy5()<< G4endl;
    G4cout <<" --------------------------------------------------------- "<< G4endl;
  }
}

// added by Sergey: verification of Hit zGlobal wrt Cell zMin,zMax
void TileGeoG4CalibSD::CellNumCorrectToHit(int nPeriod, int nSamp) {
  m_cellNum = m_cell->cellNum;
  if (m_tileTB) return;
  if (m_detector == 1) {
    double zMin = m_cell->zMin;
    double zMax = m_cell->zMax;
    if (nSamp == 2) {
      zMin = m_cell->zMin2;
      zMax = m_cell->zMax2;
    }
    int nPeriods = m_cSection->nrOfPeriods;
    bool Cell_ID_cor = false;
    if (m_zGlobal < zMin && nPeriod > 0) {
      if (verboseLevel >= 10) {
        std::streamsize prec = G4cout.precision();
        G4cout << "Det=" << m_detector << " nSamp=" << nSamp << " >> WRONG CELL:" << m_cellNum;
        G4cout << " Period=" << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
        G4cout << zMin << ", " << zMax << ", " << m_zGlobal << G4endl;
        G4cout.precision(prec);
      }
      nPeriod--;
      m_cell = m_cSection->GetCell(nPeriod, nSamp);
      Cell_ID_cor = true;
    } else if (m_zGlobal > zMax && nPeriod < nPeriods - 1) {
      if (verboseLevel >= 10) {
        std::streamsize prec = G4cout.precision();
        G4cout << "Det=" << m_detector << " nSamp=" << nSamp << " >> WRONG CELL:" << m_cellNum;
        G4cout << " Period=" << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
        G4cout << zMin << ", " << zMax << ", " << m_zGlobal << G4endl;
        G4cout.precision(prec);
      }
      nPeriod++;
      m_cell = m_cSection->GetCell(nPeriod, nSamp);
      Cell_ID_cor = true;
    }
    if (Cell_ID_cor) {
      zMin = m_cell->zMin;
      zMax = m_cell->zMax;
      if (nSamp == 2) {
        zMin = m_cell->zMin2;
        zMax = m_cell->zMax2;
      }
      if (m_zGlobal < zMin || m_zGlobal > zMax || m_cell->cellNum == m_cellNum) {
        std::streamsize prec = G4cout.precision();
        G4cout << "WARNING:  HIT IS OUTSIDE THE CELL:" << m_cellNum << " Period=";
        G4cout << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
        G4cout << zMin << ", " << zMax << ", " << m_zGlobal << G4endl;
        G4cout.precision(prec);
      } else {
        m_cellNum = m_cell->cellNum;
        if (verboseLevel >= 10) {
          std::streamsize prec = G4cout.precision();
          G4cout << "             CORRECTED CELL:" << m_cellNum << " Period=";
          G4cout << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
          G4cout << zMin << ", " << zMax << ", " << m_zGlobal << G4endl;
          G4cout.precision(prec);
        }
      }
    }
  }
}

#ifdef HITSINFO    //added by Sergey
void TileGeoG4CalibSD::HitsInfoPrint(bool isDMHit=true, int Calc=0)
// Calc: 0-All IDCalculators, 1-CellIDCalculator, 2-ScintIDCalculator, 3-PlateCellIDCalculator,
//       4-GirderCellIDCalculator, 5-DefaultHitIDCalculator-Cell hit, 6-DefaultHitIDCalculator-DM hit
{
  if(isDMHit) { //DM hit
    htout << " 1111111 " << m_dm_subDet << " " << m_dm_nphi << " " << m_dm_neta << " " << m_dm_sample;
    htout << " " << m_dm_region << " " << Calc << " " << m_nEvent << std::endl;
  } else { //Cell Hit
    htout << " 1111111 " << m_det_side << " " << m_module << " " << m_tower << " " << m_sample;
    htout << " " << m_cellNum << " " << Calc << " " << m_nEvent << std::endl;
  }
  htout << " 2222222 " << m_xGlobal << " " << m_yGlobal << " " << m_zGlobal << " " << m_E_tot << std::endl;
}
#endif

