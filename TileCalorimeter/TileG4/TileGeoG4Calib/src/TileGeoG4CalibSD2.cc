/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TileGeoG4SD/TileGeoG4SDCalc.hh"
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
static const double M_PI32 = M_PI/32;

#ifdef HITSINFO    //added by Sergey
std::ofstream htout("hits.txt");
#endif


//------------------------------------------------------
//      PUBLIC  METHODS
//------------------------------------------------------

void TileGeoG4CalibSD::ClassifyEnergy() {
    _result = m_simEn->Classify(aStep);
}



//FIND THE TILECAL SUB-SECTION AND ITS MODULE WHERE THE CURRENT
//STEP IS. DETERMINE SOME OF THE IDENTIFIER VALUES, WHICH ARE
//RELATED TO A SUB-SECTION AND A MODULE.
//
//OTHER IDENTIFIER VALUES WILL BE DETERMINED BY IDENITIFIER
//CALCULATORS DUE TO THE PARTICULAR VOLUME WHERE THIS STEP IS LOCATED
bool TileGeoG4CalibSD::FindTileCalibSection()
{
    //RESET IDENTIFIER FIELDS
    ResetCellIDFields();
    ResetDMCellIDFields();
    ResetSectSideFlags();

    //reset DM sub detector
    dm_subDet = 5;

    //Global Hit Position: x,y,z,phi - added by Sergey
    G4ThreeVector prestepPos = aStep->GetPreStepPoint()->GetPosition();
    xGlobal   = prestepPos.x();
    yGlobal   = prestepPos.y();
    zGlobal   = prestepPos.z();
    phiGlobal = prestepPos.phi();

    G4VPhysicalVolume* physSection     ;
    G4LogicalVolume*   logVol;
    G4String           namePhysSection ;
    G4String           nameLogiVol     ;

    //DETERMINE TOUCHABLEHISTORY AND PHYSICAL VOLUME FOR THIS STEP
    //THEY WILL BE USED BY THIS SECTION FINDER AND CALCULATOR METHODS
    _StepTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    _StepPhysVol   = _StepTouchable->GetVolume();

    //DETERMINE A NAME OF THE STEP VOLUME
    G4String nameVol = _StepPhysVol->GetName();

    //Find level for mother volume of Detector setup
    G4int level = _StepTouchable->GetHistoryDepth();

    //Debugging of volumes hierarchy
    for (int i=0; i<level; i++) {
        physSection = _StepTouchable->GetVolume(i);
        logVol      = physSection->GetLogicalVolume();
        if (verboseLevel>10) G4cout << " Level--> "  << i
            << ", LogVol--> "   << (logVol->GetName()).c_str()
            << ", PhysVol--> "  << (physSection->GetName()).c_str() << G4endl;
    }

    //Find envelope of TileCal. For Tile or TileCTB configuration setup
    nameLogiVol = _StepTouchable->GetVolume(level-1)->GetLogicalVolume()->GetName();
    while(nameLogiVol.find("Tile")==G4String::npos ) {
        level--;
        nameLogiVol = _StepTouchable->GetVolume(level-1)->GetLogicalVolume()->GetName();
    }
    if (verboseLevel>10) G4cout << nameLogiVol <<" volume level --->" << level << G4endl;

    //NEW FEATURE (SINCE RELEASE 11.1.0) - ATLAS TILECAL HAS ONE MORE
    //ENVELOPES FOR CENTRAL BARREL (INCLUDES ALSO FINGERS) AND FOR
    //BOTH EXTENDED BARRELS (THEY INCLUDE FINGER, ITC AND GAP/CRACK
    //ENVELOPES ALSO)
    //SO, IF THERE IS NOT ANY TB SETUP OF TILECAL THEN MOVE DOWN TO
    //THE NEXT LEVEL OF ENVELOPES
    if(!m_options.tileTB) level--;

    //Check for Hits location wheather it is in a true Tile volume
    //or inside of Tile envelopes
    if (level>1) {
        --level;  // Move to the level of sections Barrel, EBarrelPos, EBarrelNeg, Gap etc...

        physSection = _StepTouchable->GetVolume(level-1);
        namePhysSection = physSection->GetName();

        //determine a side
        _side = 1;
        if((namePhysSection.find("Barrel") != G4String::npos) &&
           (namePhysSection.find("EBarrel") == G4String::npos) )
            { }
        else {
            if (namePhysSection.find("Neg") != G4String::npos) {
                _is_negative = true;
                _side     = -1;
                dm_subDet = -abs(dm_subDet);
            }
        }

        //Retrieve Tile calibration sections
        if (namePhysSection.find("EBarrel") != G4String::npos) {
            _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);
            _is_extended = true;
            _detector = 2;
            dm_region = 1;
        } else if (namePhysSection.find("Barrel")  != G4String::npos) {
            _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
            _detector = 1;
            dm_region = 0;
        } else if (namePhysSection.find("ITC") != G4String::npos) {
            _is_extended = true;
            _detector = 3;
            dm_region = 2;
            if (level>2) {
                G4String namePlug = _StepTouchable->GetVolume(level-3)->GetName();
                if (namePlug.find("Plug1") != G4String::npos) {
                    _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG1);
                } else {
                    _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG2);
                }
            } else {
                _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG2);
            }
        } else if (namePhysSection.find("Gap") != G4String::npos) {
            _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG3);
            _detector = 4;
            dm_region = 2;
        } else if (namePhysSection.find("Crack") != G4String::npos) {
            _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG4);
            _detector = 4;
            dm_region = 2;
        } else if (namePhysSection.find("EFinger") != G4String::npos) {
            _c_section   = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);
            _is_extended = true;
            _detector = 2;
            dm_region = 1;
        } else if (namePhysSection.find("Finger") != G4String::npos) {
            _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
            _detector = 1;
            dm_region = 0;
        } else if (namePhysSection.find("ESaddle") != G4String::npos) {
            _c_section   = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);
            _is_extended = true;
            _detector = 2;
            dm_region = 2;
        } else if (namePhysSection.find("Saddle") != G4String::npos) {
            _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
            _detector = 1;
            dm_region = 0;
        } else {
            G4cout << "ERROR: FindTileCalibSection : wrong name for "
                     << "tile section --> " << namePhysSection.c_str() << G4endl;
            return false;
        }

        //determine a module
        if (level>1) {
            if(namePhysSection.find("EBarrel") != G4String::npos  ||
               namePhysSection.find("ITC")     != G4String::npos) {
                if (level>2) {  //not IrUp,IrDw, IrBox, Iron1, ...
                    _module = _StepTouchable->GetVolume(level-2)->GetCopyNo() - 1;
                    dm_nphi = _module;
                }
            } else {
                _module = _StepTouchable->GetVolume(level-2)->GetCopyNo() - 1;
                dm_nphi = _module;
            }
        } else {
            return false;
        }

    } else {  //(level!>1)
        if (verboseLevel>10) G4cout << "FindTileCalibSection : current Step "
            << "is in Tile envelope " << " / Add Step energy to the Default Hit"<< G4endl;
        return false;
    }

    //REAL SECTION IS FOUND
    return true;
}



Identifier TileGeoG4CalibSD::CellIDCalculator()
{
    G4ThreeVector WrappCenter, prestepPos, localCoord ;
    int           nPeriod, nSamp=0;
    double        zLocal;

    G4String nameVol = _StepPhysVol->GetName();

    if((nameVol).find("Period") != G4String::npos)
        nPeriod = _StepPhysVol->GetCopyNo();
    else
        // Move up by one level to retrieve TilePeriod replica number
        nPeriod = _StepTouchable->GetVolume(1)->GetCopyNo();

    // calculate z coordinate of the hit against the center of TilePeriod
    // or z coordinate of the TileWrapper's center against the center of TilePeriod
    if(nameVol.find("Wrapper") != G4String::npos) {
        WrappCenter = _StepPhysVol->GetObjectTranslation();
        zLocal      = WrappCenter[2] ;
    }
    else {
        prestepPos = aStep->GetPreStepPoint()->GetPosition();
        localCoord = _StepTouchable->GetHistory()->GetTopTransform().TransformPoint(prestepPos);
        zLocal     = localCoord.z();
    }


    // by using of 'zLocal' calculate Sample of the step corresponding Cell
    switch(_detector) {

    case 1 :  // Barrel
    {
        if(zLocal      < _c_section->sample_ZBound[0] ) //-48.0
            { nSamp=0; }
        else if(zLocal < _c_section->sample_ZBound[1] ) //-9.0
            { nSamp=1; }
        else if(zLocal < _c_section->sample_ZBound[2] ) //36.0
            { nSamp=2; }
        else if(zLocal < _c_section->sample_ZBound[3] ) //74.0
            { nSamp=3; }
        else
            { nSamp=4; }
        break;
    }

    case 2:  // Ext.Barrel
    {
        if(zLocal      < _c_section->sample_ZBound[0] ) //-48.0
            { nSamp=0; }
        else if(zLocal < _c_section->sample_ZBound[1] ) //6.0
            { nSamp=1; }
        else if(zLocal < _c_section->sample_ZBound[3] ) //74.0
            { nSamp=2; }
        else
            { nSamp=4;}
        break;
    }

    case 3:  // ITC
    {
        if(zLocal < _c_section->sample_ZBound[3] ) //17.0
            { nSamp = 0 ; }
        else
            { nSamp=4; }
        break;
    }

    case 4:  // Gap/Crack
    {
        if(zLocal < _c_section->sample_ZBound[0] )  //1.4 - Gap,  -5.8 - Crack
            { nSamp=0; }
        else
            { nSamp=1; }
        break;
    }
    }

    if (nSamp==4) {
        //last sample to Girder cell
        _id=DefaultHitIDCalculator(); //girder cell calculation - default id calculator
        return _id;
    }
    //retrieve Cell
    _cell = _c_section->GetCell(nPeriod, nSamp);

    if(!_cell) {
        G4cout << "ERROR: CellIDCalculator: zero pointer to the current cell"<< G4endl;
        G4cout << "ERROR: " << nameVol<<" "<<nPeriod<<" "<<nSamp<< G4endl;
        _DefaultHit = true;

        //return default hit DM Identifier
        //_id = m_caloDM_ID->zone_id(5, 2, 0, 0, 0, 0);
        _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
        return _id;
    }

    _calibhit_type = 0; //Cell Inactive CalibHit

    CellNumCorrectToHit(nPeriod,nSamp);  //added by Sergey: verification of Hit zGlobal wrt Cell zMin,zMax

    //determine remined ID fields
    _sample = _cell->sample;
    _tower  = _cell->tower ;
    if(_tower < 0) _tower *= -1;
    _sample-- ;
    _tower--  ;
    //determine the side of Barrel cell
    if ((_detector == 1)&&(_cellNum < 0)) _side = -1; // _cellNum is calculated in CellNumCorrectToHit()

    //force _detector to be 3. That's because ITC & Gap/Crack
    //have the same Detector=3 value in a Cell ID
    if (_detector == 4) _detector=3;

    //just a debugging of CaloCell ID fields
    DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==1) {
        if (_cellNum < 0)
            det_side = -abs(_detector);
        else 
            det_side = _detector;
        if (doHitsTXT) HitsInfoPrint(false,1);  // 1-CellIDCalculator
        if (doHitsNTup) m_ntuple->storeHit(det_side,_module,_tower,_sample,_cellNum,
                                           1,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
    }
#endif

    //reetrieve CaloCell ID
    _id = m_caloCell_ID->cell_id(_subCalo, _detector, _side, _module, _tower, _sample);
    return _id;
}



Identifier TileGeoG4CalibSD::ScintIDCalculator()
{
    _cell = m_calc->_cell;  //retrieve Cell from TileGeoG4SDCalc
    if(!_cell) {
        G4cout << "ERROR: ScintIDCalculator: zero pointer to the current cell!" << G4endl;
        _DefaultHit = true;

        //return default hit DM Identifier
        //_id = m_caloDM_ID->zone_id(5, 2, 0, 0, 0, 0);
        _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
        return _id;
    }

    _calibhit_type = 1;  //Cell Active CalibHit

    //determine remined ID fields
    _detector = m_calc->nDetector;
    _sample   = m_calc->nSample - 1;
    _tower    = m_calc->nTower  - 1;
    _module   = m_calc->nModule - 1;
    _side     = m_calc->nSide;
    _cellNum  = _cell->cellNum;

    //just a debugging of CaloCell ID fields
    DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==2) {
        if (_cellNum < 0)
            det_side = -abs(_detector);
        else 
            det_side = _detector;
        if (doHitsTXT) HitsInfoPrint(false,2);  // 2-ScintIDCalculator
        if (doHitsNTup) m_ntuple->storeHit(det_side,_module,_tower,_sample,_cellNum,
                                           2,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
    }
#endif

    //reetrieve CaloCell & Tile IDs
    _id = m_caloCell_ID->cell_id(_subCalo,_detector,_side,_module,_tower,_sample);
    _id_pmt_down = m_calc->pmtID_down;
    _id_pmt_up   = m_calc->pmtID_up;

    return _id;
}



Identifier TileGeoG4CalibSD::PlateCellIDCalculator()
{
    G4String nameVol   ;
    int      period, sample  ;
    G4ThreeVector PlateCenter, prestepPos, localCoord ;
    double xLocal, yLocal, zLocal;

    PlateCenter = _StepPhysVol->GetObjectTranslation();
    prestepPos  = aStep->GetPreStepPoint()->GetPosition();
    localCoord  = _StepTouchable->GetHistory()->GetTopTransform().TransformPoint(prestepPos);

    nameVol = _StepPhysVol->GetName();
    if ((nameVol).find("Ir") == G4String::npos) {
        xLocal = PlateCenter[0] + localCoord.x();
        yLocal = PlateCenter[1] + localCoord.y();
        zLocal = PlateCenter[2] + localCoord.z();
    }
    else {
        xLocal = prestepPos.x();    //global x
        yLocal = prestepPos.y();    //global y
        zLocal = prestepPos.phi();  //global phi
        if (yLocal<0) {
            _module=static_cast<int>(trunc(zLocal/M_PI32+64));
            dm_nphi = _module ;
        }
        else {
            G4cout << "ERROR: PlateCellIDCalculator: Ir* _module calculation error"<< G4endl;
            _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
            return _id;
        }

    }

    _p_cell = 0;
    if( ((nameVol).find("Front") != G4String::npos) || ((nameVol).find("Ir") != G4String::npos)) {
        if ((nameVol).find("Plate") != G4String::npos) {
            _p_cell = _c_section->GetTilePlateCell(xLocal, zLocal, 1);   // return FrontPlate cell
        }
        else {
            if( (nameVol.find("IrUp") != G4String::npos)  ||
                (nameVol.find("IrDw") != G4String::npos)) {
                _p_cell = _c_section->GetTilePlateCell(0, 0, 4);
            }
            else {
                _p_cell = _c_section->GetTilePlateCell(0, 0, 3);
            }
        }

        if(!_p_cell) {
            G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current plate cell"<< G4cout ;
            G4cout << "ERROR: " << nameVol<< G4cout ;
            _DefaultHit = true;

            //return default hit DM Identifier
            //_id = m_caloDM_ID->zone_id(5, 2, 0, 0, 0, 0);
            _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
            return _id;
        }

        period = _p_cell->neighbor;
        _cell = _c_section->GetCell(period, 0);  // return adjucent cell
    }
    else if (_detector==3 && (nameVol).find("EndPlateSh") != G4String::npos) {
        // foil in front of C10 or special D4 is always part of the cell
        _cell   = _c_section->GetCell(0, 0);   // the only cell in PLUG1 or PLUG2

        _calibhit_type = 0;  //Plate Cell Inactive CalibHit

        //determine remined ID fields
        _sample = _cell->sample ;
        _tower  = _cell->tower  ;
        if(_tower < 0) _tower *= -1 ;
        _sample-- ;
        _tower--  ;

        //just a debugging of CaloCell ID fields
        DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==3) {
        _cellNum = _cell->cellNum;
        if (_cellNum < 0)
            det_side = -abs(_detector);
        else 
            det_side = _detector;
        if (doHitsTXT) HitsInfoPrint(false,3);  // 3-PlateCellIDCalculator
        if (doHitsNTup) m_ntuple->storeHit(det_side,_module,_tower,_sample,_cellNum,
                                           3,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
    }
#endif

        //retrieve CaloCell ID
        _id = m_caloCell_ID->cell_id(_subCalo, _detector, _side, _module, _tower, _sample);
        return _id;
    }
    else {
        //TileEndPlate
        if ((nameVol).find("Plate") != G4String::npos) {
            _p_cell = _c_section->GetTilePlateCell(xLocal, zLocal, 2);   // return EndPlate cell
        }
        else {
            //SaddleModule
            if (( _is_negative )&&(dm_region == 0)) {
                _is_negative=false;
                _p_cell = _c_section->GetTilePlateCell(0, 0, 6);
            }
            else {
                _p_cell = _c_section->GetTilePlateCell(0, 0, 5);
            }
        }

        if(!_p_cell) {
            G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current plate cell"<< G4endl;
            G4cout <<nameVol<< G4endl;
            _DefaultHit = true;

            //return default hit DM Identifier
            //_id = m_caloDM_ID->zone_id(5, 2, 0, 0, 0, 0);
            _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
            return _id;
        }

        sample  = _p_cell->sample ;

        //the last PlateCell of Barrel EndPlates or Extended outer Endplate
        //is DM cell as long as it's between Girder and Finger volumes.
        //So, it should not be asigned to the real Cell
        if((sample != (int)_c_section->samples.size()-1) || (_p_cell->detector == 4)) {
            period  = _p_cell->neighbor;
            _cell   = _c_section->GetCell(period, sample-1) ;    // return adjucent cell
        }
        else {
            //Hit is in the last PlateCell, which shouldn't be
            //assigned to the real Cell (+-D3 or +-D6)
            //Assign it to the section Finger cell.
            _id = GirderCellIDCalculator() ;
            return _id ;
        }
    }


    //AT THIS POINT WE HAVE DETERMINED THE BOTH PLATE CELL AND ITS
    //NEIGHBOR CELL OF THE CURRENT STEP. FURTHER AddToCell - FLAG
    //DEFINES WE NEED TO STORE THIS PLATE HIT IN THE NEIGHBOR REAL
    //CELL OR STORE IT SEPERATELY AS A DM PLATE CALIBHIT.
    //
    //FLAG POINTS PLATE HIT TO BE STORED IN THE UDJUCENT REAL CELL
    if((AddToCell)&&((nameVol).find("Plate") != G4String::npos)) {
        //check Cell validity
        if(!_cell) {
            G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current cell"<< G4endl;
            G4cout <<nameVol<< G4endl;
            _DefaultHit = true;

            //return default hit DM Identifier
            //_id = m_caloDM_ID->zone_id(5, 2, 0, 0, 0, 0);
            _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
            return _id;
        }

        _calibhit_type = 0 ; //Plate Cell Inactive CalibHit

        //determine remined ID fields
        _sample = _cell->sample ;
        _tower  = _cell->tower  ;
        if(_tower < 0) _tower *= -1 ;
        _sample-- ;
        _tower--  ;

        //determine the side for Barrel cell
        if ((_detector == 1) && (_cell->tower < 0)) _side = -1;

        //just a debugging of CaloCell ID fields
        DebugCellIDFields();

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==3) {
        _cellNum = _cell->cellNum;
        if (_cellNum < 0)
            det_side = -abs(_detector);
        else 
            det_side = _detector;
        if (doHitsTXT) HitsInfoPrint(false,3);  // 3-PlateCellIDCalculator
        if (doHitsNTup) m_ntuple->storeHit(det_side,_module,_tower,_sample,_cellNum,
                                           3,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
    }
#endif

        //retrieve CaloCell ID
        _id = m_caloCell_ID->cell_id(_subCalo, _detector, _side, _module, _tower, _sample);
        return _id;
    }

    //FLAG POINTS THAT PLATE HITS SHOULD BE STORED AS DM CALIBHITS
    else {
        _calibhit_type = 2; //Plate DM CalibHit
        dm_type = 1;        //one of the DM Identifier fields

        //check Plate Cell validity
        if(!_p_cell) {
            G4cout << "ERROR: PlateCellIDCalculator: zero pointer to the current plate cell"<< G4endl;
            G4cout <<nameVol<< G4endl;
            _DefaultHit = true;

            //return default hit DM Identifier
            //_id = m_caloDM_ID->zone_id(5, 2, 0, 0, 0, 0);
            _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
            return _id;
        }

        //determine remined ID fields
        dm_sample = _p_cell->sample ;
        //All EndPlate hits have DM sampling = 1
        //All FrontPlate hits have DM sampling = 0
        //force DM sampling of EndPlate hit to be 1
        if(dm_sample != 0 ) dm_sample = 1 ;
        //hit tower must not contain side of eta but
        //only its tower absolute value
        dm_neta   = _p_cell->tower  ;
        if(dm_neta  <  0 )  dm_neta *= -1 ;
        dm_neta--  ;
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
        if( (dm_sample == 1) && ((_p_cell->detector == 1) || (_p_cell->detector == 2)) )        dm_neta--  ;

        //Fix #2, 11.03.2006. Due to DM hit Identifier description all ITC
        //(dm_region=2) Front/End Plate Hits must have dm_sample=1
        if((_p_cell->detector == 3) || (_p_cell->detector == 4))        dm_sample = 1 ;

        //determine the side for PlateCell
        if((dm_region == 0) && (_p_cell->tower < 0)) {
            _side     = -1;
            dm_subDet = -abs(dm_subDet);
        }

        //end plate of ext barrel at higher Z -> region = 2
        if((dm_region == 1) && (((nameVol).find("EndPlate2") != G4String::npos))) {
            dm_region = 2;
        }

        //just a debugging of CaloDM ID fields
        //second argument is 1 -> tells to the method
        //that Plate DM cell ID is being debugged
        DebugDMCellIDFields(1);

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==3) {
        if (doHitsTXT) HitsInfoPrint(true,3);  // 3-PlateCellIDCalculator
        if (doHitsNTup) m_ntuple->storeHit(dm_subDet,dm_nphi,dm_neta,dm_sample,dm_region,
                                           3,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
    }
#endif

        //retrieve CaloDM ID
        _id = DM_ID_Maker(dm_subDet, dm_type, dm_sample, dm_region, dm_neta, dm_nphi);
        return _id;
    }
}



Identifier TileGeoG4CalibSD::GirderCellIDCalculator()
{
    G4ThreeVector prestepPos, localCoord ;
    int           nGirderCell;
    double        xLocal;

    G4String nameVol = _StepPhysVol->GetName();

    if(nameVol.find("Girder") != G4String::npos) {
        prestepPos = aStep->GetPreStepPoint()->GetPosition();
        localCoord = _StepTouchable->GetHistory()->GetTopTransform().TransformPoint(prestepPos);

        xLocal  = localCoord.x();
        _g_cell = _c_section->GetTileGirderCell(xLocal);
    }
    else if (!_is_extended) {
        //Barrel Finger or Barrel EndPlate's last cell
        if((nameVol.find("EndPlate") != G4String::npos)||(nameVol.find("EPHole") != G4String::npos)) {
            // EndPlate and EPHole fix
            if(nameVol.find("1") != G4String::npos) {
                // EndPlate1, EPHole1 : Neg side
                _side     = -1;
                dm_subDet = -abs(dm_subDet);
            }
        }
        nGirderCell = static_cast<int>((_c_section->girderCells).size());
        if(_side > 0)
            //Finger on the positive side
            _g_cell = _c_section->girderCells[nGirderCell-1];
        else
            //Finger on the negative side
            _g_cell = _c_section->girderCells[nGirderCell-2];
    }
    else {
        //Extended Finger or Extended EndPlate's last cell
        nGirderCell = static_cast<int>((_c_section->girderCells).size());
        _g_cell = _c_section->girderCells[nGirderCell-1];
    }

    _calibhit_type = 3; //Girder DM CalibHit
    dm_type = 1;

    if(!_g_cell) {
        G4cout << "ERROR: GirderCellIDCalculator: zero pointer to the current girder cell"<< G4cout ;
        G4cout <<nameVol<< G4endl;
        _DefaultHit = true;

        //return default hit DM Identifier
        //_id = m_caloDM_ID->zone_id(5, 2, 0, 0, 0, 0);
        _id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0);
        return _id;
    }

    //determine remined ID fields
    dm_sample = _g_cell->sample ;
    dm_neta   = _g_cell->tower  ;
    if(dm_neta < 0) dm_neta *= -1 ;
    dm_neta--  ;
    //determine the side for GirderCell
    if((dm_region == 0) && (_g_cell->tower < 0)) {
        _side     = -1;
        dm_subDet = -abs(dm_subDet);
    }

    //just a debugging of CaloDM ID fields
    //second argument is 2 -> tells to the method
    //that Girder || Finger DM cell is being debugged
    DebugDMCellIDFields(2);

#ifdef HITSINFO    //added by Sergey
    if (IDcalc==0 || IDcalc==4) {
        if (doHitsTXT) HitsInfoPrint(true,4);  // 4-GirderCellIDCalculator
        if (doHitsNTup) m_ntuple->storeHit(dm_subDet,dm_nphi,dm_neta,dm_sample,dm_region,
                                           4,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
    }
#endif

    //reetrieve CaloDM ID
    _id = DM_ID_Maker(dm_subDet, dm_type, dm_sample, dm_region, dm_neta, dm_nphi);
    return _id;
}



Identifier TileGeoG4CalibSD::DefaultHitIDCalculator()
{
    const double zBarrelMaxPos = m_lookupDM->zBarrMaxPos;
    const double zBarrelMaxNeg = m_lookupDM->zBarrMaxNeg;
    const double dzITC = m_lookupDM->zLegngthITC;
    const double dzEBarrelModule=m_lookupDM->dzExtBarrMod;
    const double zBarrelModuleMax = m_lookupDM->dzBarrMod/2.0;

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

    double rBarrelCenter = (rBarrelMin + drFrontPlate + rGirderMin)/2;

    double zBarrelActiveMax = zBarrelModuleMax;
    double zEBarrelActiveMinPos = zBarrelMaxPos + dzITC;
    double zEBarrelActiveMaxPos = zBarrelMaxPos + dzITC + dzEBarrelModule;
    double zEBarrelActiveMinNeg = zBarrelMaxNeg - dzITC;
    double zEBarrelActiveMaxNeg = zBarrelMaxNeg - dzITC - dzEBarrelModule;

    if(!AddToCell) {
        zBarrelActiveMax -= dzEndPlate;
        zEBarrelActiveMinPos += dzEndPlateSh;
        zEBarrelActiveMaxPos -= dzEndPlate;
        zEBarrelActiveMinNeg -= dzEndPlateSh;
        zEBarrelActiveMaxNeg += dzEndPlate;

    }

    double drAddGirder = 40; //4 CLHEP::cm to girder

    bool isDMHit=true;

    if (!AddToGirder) drAddGirder=0;

    double rGlobal = hypot(xGlobal,yGlobal); // radius

    if(yGlobal>=0) {
        _module=static_cast<int>(trunc(phiGlobal/M_PI32));
    } else {
        _module=static_cast<int>(trunc(phiGlobal/M_PI32+64));
        if (_module>63) _module=63; //-0 phi fix
    }

    dm_nphi = _module;

    if (zGlobal>=0) {
        _side     = 1;
        dm_subDet = 5;
    } else {
        _side     = -1;
        dm_subDet = -5;
    }
    dm_type=1;

    bool isZinActiveRegion = false;
    if (fabs(zGlobal)<=zBarrelActiveMax) {
        //barrel active zone
        isZinActiveRegion = true;
    } else if(((zGlobal>=zEBarrelActiveMaxNeg)&&(zGlobal<=zEBarrelActiveMinNeg)) ||
              ((zGlobal>=zEBarrelActiveMinPos)&&(zGlobal<=zEBarrelActiveMaxPos))) {
        //ext.barrel active zone
        isZinActiveRegion = true;
    }


    if ((zGlobal<zBarrelMaxPos)&&(zGlobal>zBarrelMaxNeg)) {
        //barrel section
        _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_BARREL);
        dm_region=0;
        _detector = 1;


        if((rGlobal >= rGirderMin)||((rGlobal >= rGirderMin - drAddGirder)&&(isZinActiveRegion))) {
            //girder-finger
            dm_sample=2;
            int nGirderCell = static_cast<int>((_c_section->girderCells).size());

            if(fabs(zGlobal)>=zBarrelModuleMax-dzEndPlate) {
                //finger
                if(zGlobal>0) {
                    _g_cell = _c_section->girderCells[nGirderCell-1];
                }
                else {
                    _g_cell = _c_section->girderCells[nGirderCell-2];
                }
            }
            else {
                _g_cell = _c_section->GetTileGirderCell(zGlobal);
            }

            dm_neta   = _g_cell->tower  ;
            if(dm_neta < 0) dm_neta *= -1 ;
            dm_neta--  ;
        }
        else if(isZinActiveRegion) {
            if(rGlobal<(rBarrelMin+drFrontPlate)&&(!AddToCell)) {
                //frontPlate if not AddToCell
                dm_sample=0;
                _p_cell = _c_section->GetTilePlateCell(zGlobal, 0, 1);
                dm_neta   = _p_cell->tower;
                if(dm_neta  <  0 )  dm_neta *= -1 ;
                dm_neta--  ;
            }
            else {
                //barrel
                isDMHit=false;
                int nSamp,nPeriod;
                double dzInBarrel=0;

                if (rGlobal< _c_section->sample_ZBound[0] + rBarrelCenter) {
                    nSamp=0;
                }
                else if (rGlobal< _c_section->sample_ZBound[1] + rBarrelCenter) {
                    nSamp=1;
                }
                else if (rGlobal< _c_section->sample_ZBound[2] + rBarrelCenter) {
                    nSamp=2;
                }
                else {
                    nSamp=3;
                }

                dzInBarrel=zGlobal-((-1)*zBarrelModuleMax+dzEndPlate);
                if(dzInBarrel>2*(zBarrelModuleMax-dzEndPlate)) {
                    dzInBarrel=2*(zBarrelModuleMax-dzEndPlate); //last period is shorter
                }
                if(dzInBarrel<0) {
                    dzInBarrel=0;
                }

                nPeriod=static_cast<int>(trunc(dzInBarrel/dzBarrelPeriod));
                _cell = _c_section->GetCell(nPeriod, nSamp);

                CellNumCorrectToHit(nPeriod, nSamp);  //added by Sergey: verification of Hit zGlobal wrt Cell zMin,zMax

                _sample = _cell->sample;
                _tower  = _cell->tower ;
                if(_tower < 0) _tower *= -1;
                _sample-- ;
                _tower--  ;
                //determine the side of Barrel cell
                if (_cellNum < 0) { // _cellNum is calculated in CellNumCorrectToHit()
                    _side = -1;
                } else {
                    _side = 1;
                }
            }
        }
        else {
            //DM added to endplate
            dm_sample=1;
            double rPlate=rGlobal-(rBarrelMin+rBarrelMax)/2.0;
            double zPlate=zGlobal;
            if (zPlate>zBarrelModuleMax) zPlate=zBarrelModuleMax;
            if (zPlate<(-1)*zBarrelModuleMax) zPlate=(-1)*zBarrelModuleMax;
            _p_cell = _c_section->GetTilePlateCell(zPlate, rPlate, 2);
            dm_neta   = _p_cell->tower ;
            if(dm_neta  <  0 )  dm_neta *= -1;
            dm_neta-=2;
            if (dm_neta<0) dm_neta=0; //possible dm_neta=-1
        }
    }
    else if ((zGlobal<zBarrelMaxPos+dzITC)&&(zGlobal>zBarrelMaxNeg-dzITC)) {
        //ITC section
        _detector = 3;
        dm_region = 2;
        if(rGlobal < rGirderMin - drAddGirder) {
            //modules
            isDMHit=false;
            int nSamp=0;
            double rCenter=0;
            if(rGlobal>rPlug1Min) {
                //Plug1
                _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG1);
                nSamp=0;
            }
            else if(rGlobal>rPlug2Min) {
                //Plug2
                _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG2);
                nSamp=0;
            }
            else if(rGlobal>rGapMin) {
                //Gap
                _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG3);
                rCenter=(rGapMin+rGapMax)/2.0;
                if (rGlobal-rCenter<_c_section->sample_ZBound[0]) {
                    nSamp=0;
                }
                else {
                    nSamp=1;
                }
            }
            else {
                //Crack
                _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG4);
                rCenter=(rCrackMin+rCrackMax)/2.0;
                if (rGlobal-rCenter<_c_section->sample_ZBound[0]) {
                    nSamp=0;
                }
                else {
                    nSamp=1;
                }

            }
            _cell = _c_section->GetCell(0, nSamp);

            _sample = _cell->sample;
            _tower  = _cell->tower ;
            _sample-- ;
            _tower--  ;
            _cellNum = _cell->cellNum;    //added by Sergey
        }
        else {
            //girder
            dm_sample=2;
            _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_PLUG1);
            _g_cell = _c_section->GetTileGirderCell(0);
            dm_neta   = _g_cell->tower;
            if(dm_neta  <  0 )  dm_neta *= -1;
            dm_neta--;
        }

    }
    else {
        //ext.barrel region
        _detector    = 2;
        dm_region    = 1;

        _c_section = m_lookupDM->GetSection(TileCalibDddbManager::TILE_EBARREL);

        double zEBarrelModuleCenter;
        if(zGlobal>0) {
            zEBarrelModuleCenter=zBarrelMaxPos+dzITC+dzEBarrelModule/2.0;
        }
        else {
            zEBarrelModuleCenter=fabs(zBarrelMaxNeg)+dzITC+dzEBarrelModule/2.0;
        }


        if((rGlobal>=rGirderMin)||((rGlobal>=rGirderMin - drAddGirder)&&(isZinActiveRegion))) {
            //girder-finger
            dm_sample=2;

            int nGirderCell = static_cast<int>((_c_section->girderCells).size());
            if((zGlobal>=zEBarrelModuleMaxPos-dzEndPlate)||(zGlobal<=zEBarrelModuleMaxNeg+dzEndPlate)) {
                _g_cell = _c_section->girderCells[nGirderCell-1];
            }
            else {
                _g_cell = _c_section->GetTileGirderCell(fabs(zGlobal)-zEBarrelModuleCenter);
            }

            dm_neta   = _g_cell->tower;
            if(dm_neta  <  0 )  dm_neta *= -1;
            dm_neta--;
        }
        else if(isZinActiveRegion) {
            if(rGlobal<(rBarrelMin+drFrontPlate)&&(!AddToCell)) {
                //frontPlate if not AddToCell
                dm_sample=0;
                _p_cell = _c_section->GetTilePlateCell(fabs(zGlobal)-zEBarrelModuleCenter, 0, 1);
                dm_neta   = _p_cell->tower;
                if(dm_neta  <  0 )  dm_neta *= -1;
                dm_neta--  ;
            }
            else {
                //ext.barrel active cells
                isDMHit=false;
                int nSamp,nPeriod;
                double dzInExtBarrel;

                if (rGlobal< _c_section->sample_ZBound[0] + rBarrelCenter) {
                    nSamp=0;
                }
                else if (rGlobal< _c_section->sample_ZBound[1] + rBarrelCenter) {
                    nSamp=1;
                }
                else {
                    nSamp=2;
                }

                dzInExtBarrel=fabs(zGlobal)-(zEBarrelModuleCenter-dzEBarrelModule/2.0+dzEndPlateSh);
                if(dzInExtBarrel>=dzEBarrelModule-dzEndPlateSh-dzEndPlate) {
                    dzInExtBarrel=dzEBarrelModule-dzEndPlateSh-dzEndPlate-dzEBarrelPeriod/2.0; //center of last period
                }
                if(dzInExtBarrel<0) {
                    dzInExtBarrel=0;
                }

                nPeriod=static_cast<int>(trunc(dzInExtBarrel/dzEBarrelPeriod));
                _cell = _c_section->GetCell(nPeriod, nSamp);

                _sample = _cell->sample;
                _tower  = _cell->tower ;
                _sample-- ;
                _tower--  ;
                _cellNum = _cell->cellNum;    //added by Sergey
            }

        }
        else {
            //endplate
            dm_sample=1;
            double rPlate=rGlobal-(rBarrelMin+rBarrelMax)/2.0;
            double zPlate=fabs(zGlobal)-zEBarrelModuleCenter;
            if(zPlate>dzEBarrelModule/2.0) zPlate=dzEBarrelModule/2.0;
            _p_cell = _c_section->GetTilePlateCell(zPlate, rPlate, 2);
            dm_neta   = _p_cell->tower ;
            dm_neta-=2;
            if (dm_neta<0) dm_neta=0;

            if((zGlobal>zEBarrelActiveMaxPos)||(zGlobal<zEBarrelActiveMaxNeg)) {
                dm_region    = 2;
            }
        }

    }


    if (verboseLevel>5) 
        G4cout << "Default Hit Calculator: processed" << G4endl;

    if(isDMHit) {  //dm hits
        if(dm_sample==2) DebugDMCellIDFields(2);
        else             DebugDMCellIDFields(1);

        _calibhit_type = 5 ; //Default DM CalibHit

#ifdef HITSINFO    //added by Sergey
        if (IDcalc==0 || IDcalc==6) {
            if (doHitsTXT) HitsInfoPrint(true,6);  // 6-DefaultHitIDCalculator-DM hit
            if (doHitsNTup) m_ntuple->storeHit(dm_subDet,dm_nphi,dm_neta,dm_sample,dm_region,
                                               6,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
        }
#endif

        _id = DM_ID_Maker(dm_subDet, dm_type, dm_sample, dm_region, dm_neta, dm_nphi);
        return _id;

    } else {  //Inactive Cell hits
        DebugCellIDFields();

        _calibhit_type = 0 ; //Default Cell Inactive CalibHit

#ifdef HITSINFO    //added by Sergey
        if (IDcalc==0 || IDcalc==5) {
            if (_cellNum < 0)
                det_side = -abs(_detector);
            else 
                det_side = _detector;
            if (doHitsTXT) HitsInfoPrint(false,5);  // 5-DefaultHitIDCalculator-Cell hit
            if (doHitsNTup) m_ntuple->storeHit(det_side,_module,_tower,_sample,_cellNum,
                                               5,nEvent,xGlobal,yGlobal,zGlobal,E_tot);
        }
#endif

        _id = m_caloCell_ID->cell_id(_subCalo, _detector, _side, _module, _tower, _sample);
        return _id;
    }

}



//-----------------------------------------------------------------------------------
//      PROTECTED  METHODS
//        To avoid an users potential harm intervention in the SD funcionality
//-----------------------------------------------------------------------------------
//DM Hit ID MAKER
Identifier TileGeoG4CalibSD::DM_ID_Maker(int subDet, int nType, int nSample, int nRegion, int nTower, int nModule)
{
    G4bool doo_checks = m_caloDM_ID->do_checks();
    Identifier caloDMID;
    try {
        m_caloDM_ID->set_do_checks(true);
        caloDMID = m_caloDM_ID->zone_id( subDet, nType, nSample, nRegion, nTower, nModule );
    }
    catch(CaloID_Exception CalEx){
        DebugDMCellIDFields(nSample);
        G4cout << "ERROR: M_ID_Maker() - WRONG ID -> " <<CalEx.message()<< G4endl;
    }

    m_caloDM_ID->set_do_checks(doo_checks);
    return caloDMID;
}



bool TileGeoG4CalibSD::AreClassifiedEnergiesAllZero() {
    //check any kind of classified energies. if they all are zero
    //then nothing to do and go to the next Step
    if( (_result.energy[CaloG4::SimulationEnergies::kEm]         == 0. ) &&
        (_result.energy[CaloG4::SimulationEnergies::kNonEm]      == 0. ) &&
        (_result.energy[CaloG4::SimulationEnergies::kInvisible0] == 0. ) &&
        (_result.energy[CaloG4::SimulationEnergies::kEscaped]    == 0. ) )
        //All of them are zero
        return true;
    else
        //they aren't all zero
        return false;
}



void TileGeoG4CalibSD::SetEscapedEnergy(double escapedEnergy)
{
    _result.energy[CaloG4::SimulationEnergies::kEm]          = 0. ;
    _result.energy[CaloG4::SimulationEnergies::kNonEm]       = 0. ;
    _result.energy[CaloG4::SimulationEnergies::kInvisible0]  = 0. ;
    _result.energy[CaloG4::SimulationEnergies::kEscaped]     = escapedEnergy ;
}



//***SIMPLE ENERGY COUNTER****
void TileGeoG4CalibSD::EnergiesSimpleCounter() {

    E_em        += _result.energy[CaloG4::SimulationEnergies::kEm]  ;
    E_nonem     += _result.energy[CaloG4::SimulationEnergies::kNonEm]  ;
    E_invisible += _result.energy[CaloG4::SimulationEnergies::kInvisible0] ;
    E_escaped   += _result.energy[CaloG4::SimulationEnergies::kEscaped]       ;

    // calculate E_tot for current event only
    E_tot = _result.energy[CaloG4::SimulationEnergies::kEm] +
            _result.energy[CaloG4::SimulationEnergies::kNonEm]  +
            _result.energy[CaloG4::SimulationEnergies::kInvisible0] +
            _result.energy[CaloG4::SimulationEnergies::kEscaped] ;
}



void TileGeoG4CalibSD::DebugCellIDFields()
{
    if (verboseLevel>5) {
        G4cout << "---- CELL INFO ----"            << G4endl;
        G4cout << "Detector >> " << _cell->detector<< G4endl;
        G4cout << "Numm     >> " << _cell->cellNum << G4endl;
        G4cout << "Tower    >> " << _cell->tower   << G4endl;
        G4cout << "Sample   >> " << _cell->sample  << G4endl;

        G4cout << "---- HIT INFO ----"        << G4endl;
        G4cout << "Detector >> " << _detector << G4endl;
        G4cout << "Side     >> " << _side     << G4endl;
        G4cout << "Module   >> " << _module   << G4endl;
        G4cout << "Tower    >> " << _tower    << G4endl;
        G4cout << "Sample   >> " << _sample   << G4endl;
        G4cout << "Energy   >> " << E_tot     << G4endl;
        G4cout << "---- END ----"             << G4endl;
    }
}



void TileGeoG4CalibSD::DebugDMCellIDFields(int sample)
{
    int detector, tower;
    if(sample==2) {
        // Girders || Fingers
        detector = _g_cell->detector ;
        tower    = _g_cell->tower    ;
    }
    else {
        // End || Front plates
        detector = _p_cell->detector ;
        tower    = _p_cell->tower    ;
    }

    if (verboseLevel>5) {
        G4cout << "---- DM CELL INFO ----"      << G4endl;
        G4cout << "Detector >> " << detector << G4endl;
        G4cout << "Tower    >> " << tower    << G4endl;

        G4cout << "---- DM HIT INFO ----"        << G4endl;
        G4cout << "SubDet   >> " << dm_subDet << G4endl;
        G4cout << "Type     >> " << dm_type   << G4endl;
        G4cout << "Sample   >> " << dm_sample << G4endl;
        G4cout << "Region   >> " << dm_region << G4endl;
        G4cout << "NEta     >> " << dm_neta   << G4endl;
        G4cout << "NPhi     >> " << dm_nphi   << G4endl;
        G4cout << "Energy   >> " << E_tot     << G4endl;
        G4cout << "---- END INFO ----"        << G4endl;
    }
}



void TileGeoG4CalibSD::DebugEnergies()
{
    E_tot = E_em + E_nonem + E_invisible + E_escaped + m_tile_eep->GetEnergy5();

    if (verboseLevel>5) {
        G4cout <<" EndOfEvent" << G4endl;
        G4cout <<" ---------------------------------------------------------- "<< G4endl;
        G4cout <<" TOTAL ENERGY OF EVENT         = "<<E_tot<<" ("<<E0+E1+E2+E3<<")"<< G4endl;
        G4cout <<" ------------------------------------ "<< G4endl;
        G4cout <<" EM COMPONENT OF ENERGY        = "<<E_em<<" ("<<E0<<") "<< G4endl;
        G4cout <<" ------------------------------------ "<< G4endl;
        G4cout <<" NON_EM COMPONENT OF ENERGY    = "<<E_nonem<<" ("<<E1<<")"<< G4endl;
        G4cout <<" ------------------------------------ "<< G4endl;
        G4cout <<" INVISIBLE COMPONENT OF ENERGY = "<<E_invisible<<" ("<<E2<<")"<< G4endl;
        G4cout <<" ------------------------------------ "<< G4endl;
        G4cout <<" ESCAPED COMPONENT OF ENERGY   = "<<E_escaped<<" ("<<E3<<")"<< G4endl;
        G4cout <<" --------------------------------------------------------- "<< G4endl;
        G4cout <<" energy5 (missed energy)       = "<<m_tile_eep->GetEnergy5()<< G4endl;
        G4cout <<" --------------------------------------------------------- "<< G4endl;
    }
}



// added by Sergey: verification of Hit zGlobal wrt Cell zMin,zMax
void TileGeoG4CalibSD::CellNumCorrectToHit(int nPeriod, int nSamp)
{
    _cellNum = _cell->cellNum;
    if (_detector == 1) {
        double _zMin = _cell->zMin;
        double _zMax = _cell->zMax;
        if (nSamp == 2) {
            _zMin = _cell->zMin2;
            _zMax = _cell->zMax2;
        }
        int _nPeriods = _c_section->nrOfPeriods;
        bool Cell_ID_cor = false;
        if (zGlobal < _zMin && nPeriod > 0) {
            if (verboseLevel>5) {
                std::streamsize prec = G4cout.precision();
                G4cout << "Det=" << _detector << " nSamp=" << nSamp << " >> WRONG CELL:" << _cellNum;
                G4cout << " Period=" << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
                G4cout << _zMin << ", " << _zMax << ", " << zGlobal << G4endl;
                G4cout.precision(prec);
            }
            nPeriod--;
            _cell = _c_section->GetCell(nPeriod, nSamp);
            Cell_ID_cor = true;
        }
        else if (zGlobal > _zMax && nPeriod < _nPeriods-1) {
            if (verboseLevel>5) {
                std::streamsize prec = G4cout.precision();
                G4cout << "Det=" << _detector << " nSamp=" << nSamp << " >> WRONG CELL:" << _cellNum;
                G4cout << " Period=" << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
                G4cout << _zMin << ", " << _zMax << ", " << zGlobal << G4endl;
                G4cout.precision(prec);
            }
            nPeriod++;
            _cell = _c_section->GetCell(nPeriod, nSamp);
            Cell_ID_cor = true;
        }
        if(Cell_ID_cor) {
            _zMin = _cell->zMin;
            _zMax = _cell->zMax;
            if (nSamp == 2) {
                _zMin = _cell->zMin2;
                _zMax = _cell->zMax2;
            }
            if ( zGlobal < _zMin || zGlobal > _zMax || _cell->cellNum == _cellNum ) {
                std::streamsize prec = G4cout.precision();
                G4cout << "WARNING:  HIT IS OUTSIDE THE CELL:" << _cellNum << " Period=";
                G4cout << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
                G4cout << _zMin<< ", " << _zMax << ", " << zGlobal << G4endl;
                G4cout.precision(prec);
            }
            else {
                _cellNum = _cell->cellNum;
                if (verboseLevel>5) {
                    std::streamsize prec = G4cout.precision();
                    G4cout << "             CORRECTED CELL:" << _cellNum << " Period=";
                    G4cout << nPeriod << " zMin,zMax,zGlobal: " << std::setprecision(16);
                    G4cout << _zMin << ", " << _zMax << ", " << zGlobal << G4endl;
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
        htout << " 1111111 " << dm_subDet << " " << dm_nphi << " " << dm_neta << " " << dm_sample;
        htout << " " << dm_region << " " << Calc << " " << nEvent << std::endl;
    } else { //Cell Hit
        htout << " 1111111 " << det_side << " " << _module << " " << _tower << " " << _sample;
        htout << " " << _cellNum << " " << Calc << " " << nEvent << std::endl;
    }
    htout << " 2222222 " << xGlobal << " " << yGlobal << " " << zGlobal << " " << E_tot << std::endl;
}
#endif

