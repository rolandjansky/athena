/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4CalibSD.
// Calibration Sensitive Detector for TileCal
//
// Author: Gia Khoriauli   <gia@mail.cern.ch>
// May, 2005.
//
// Major updates: December, 2005;
//                July, 2013 (Sergey);
//
//************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TileGeoG4CalibSD.h"
#include "TileGeoG4DMLookupBuilder.h"
#include "TileEscapedEnergyProcessing.h"
#include "TileGeoG4SD/TileGeoG4SDCalc.hh"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"
#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileSimEvent/TileHitVector.h"
#include "PathResolver/PathResolver.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloIdentifier/TileID.h"

#include "CaloG4Sim/SimulationEnergies.h"
#include "CaloG4Sim/EscapedEnergyRegistry.h"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "MCTruth/EventInformation.h"

#include "MCTruth/VTrackInformation.h"
#include "CxxUtils/make_unique.h"

#include <iostream>
#include <string>


//CONSTRUCTOR
TileGeoG4CalibSD::TileGeoG4CalibSD(const G4String& name, const std::vector<std::string>& outputCollectionNames, ServiceHandle<StoreGateSvc> &detStore, const TileSDOptions &opts)
    : G4VSensitiveDetector(name)
    , tileActiveCellCalibHits(outputCollectionNames[1])
    , tileInactiveCellCalibHits(outputCollectionNames[2])
    , tileDeadMaterialCalibHits(outputCollectionNames[3])
    , tileHits(outputCollectionNames[0])
    , m_options(opts)
    , m_rdbSvc(opts.rDBAccessSvcName,name)
    , m_geoModSvc(opts.geoModelSvcName,name)
#ifdef HITSINFO    //added by Sergey
    , m_ntuple("TileCalibHitNtuple/TileCalibHitNtuple")
    , m_ntupleCnt("TileCalibHitCntNtup/TileCalibHitCntNtup")
#endif
    , m_calc(0)
    , m_lookupDM(0)
    , m_simEn(0)
    , m_tile_eep(0)
    , E_tot(0.0)
    , E_em(0.0)
    , E_nonem(0.0)
    , E_invisible(0.0)
    , E_escaped(0.0)
    , E0(0.0)
    , E1(0.0)
    , E2(0.0)
    , E3(0.0)
    , _StepTouchable(0)
    , _StepPhysVol(0)
    , _c_section(0)
    , _cell(0)
    , _p_cell(0)
    , _g_cell(0)
    , _result()
    , _id()
    , _id_pmt_up()
    , _id_pmt_down()
    , _calibhit_type(0)
    , _subCalo(0)
    , _detector(0)
    , _side(0)
    , _module(0)
    , _tower(0)
    , _sample(0)
    , dm_subDet(0)
    , dm_type(0)
    , dm_sample(0)
    , dm_region(0)
    , dm_neta(0)
    , dm_nphi(0)
    , xGlobal(0.0)
    , yGlobal(0.0)
    , zGlobal(0.0)
    , phiGlobal(0.0)
    , _cellNum(0)
#ifdef HITSINFO    //added by Sergey
    , nEvent(1)
    , det_side(0)
#endif
    , _DefaultHit(false)
    , _doBirkFlag(false)
    , _is_extended(false)
    , _is_negative(false)
{

    m_simEn    = new CaloG4::SimulationEnergies();
    m_tile_eep = new TileEscapedEnergyProcessing(verboseLevel);
    m_tile_eep->SetEscapedFlag(false);
    m_tile_eep->SetEnergy5(0.);
    m_tile_eep->SetEscapedEnergy(0.);

    // @TODO : Watch out for this!!  Is it a singleton or something??
    CaloG4::EscapedEnergyRegistry* registry = CaloG4::EscapedEnergyRegistry::GetInstance();
    registry->AddAndAdoptProcessing( "Tile", m_tile_eep );

#ifdef HITSINFO    //added by Sergey
    if (doHitsNTup) {
      if(m_ntuple.retrieve().isFailure()){
        G4cout << " Can't retrieve " << m_ntuple << G4endl;
        abort();
      }
    }
    if(m_ntupleCnt.retrieve().isFailure()){
      G4cout <<" Can't retrieve " << m_ntupleCnt << G4endl;
      abort();
    }
#endif

  // Retrieve CaloCell_ID helper from detector store
  if (detStore->retrieve(m_caloCell_ID).isFailure()){
    G4cout << "FATAL: Failed to retrieve calo cell ID helper" << G4endl;
    abort();
  }

  // Retrieve TileID helper from det store
  if (detStore->retrieve(m_caloDM_ID).isFailure()){
    G4cout << "FATAL: Failed to retrieve DM ID helper" << G4endl;
    abort();
  }

//**************************************************************************************************
    //Sensitive Detector initialisation for TileCal G4 simulations (Sergey)
    // Owns this now 
    m_calc = new TileGeoG4SDCalc(m_options);
//**************************************************************************************************

    // Grab the service handles
    if (m_rdbSvc.retrieve().isFailure()){
      G4cout << "FATAL: Could not retrieve the RDB Svc" << G4endl;
      abort();
    }
    if (m_geoModSvc.retrieve().isFailure()){
      G4cout << "FATAL: Could not retrieve the geo model svc" << G4endl;
      abort();
    }

    //BUILD TILECAL ORDINARY AND CALIBRATION LOOK-UP TABLES
    m_lookup   = m_calc->m_lookup;
    m_lookupDM = new TileGeoG4DMLookupBuilder(m_lookup, m_rdbSvc, m_geoModSvc, detStore, verboseLevel);



    if(m_options.tileTB) {
        m_lookup->BuildLookup(true);
        m_lookupDM->BuildLookup(true);
    } else {
        m_lookup->BuildLookup();
        m_lookupDM->BuildLookup();
    }
    if (verboseLevel>5) G4cout << "Lookup built for Tile" << G4endl;

    _plateToCell=m_lookupDM->GetPlateToCell();
    G4cout << "Using plateToCell (from DB) = " << (_plateToCell ? "true" : "false") << G4endl;

    // current settings for AddToCell and AddToGirder are controlled just by one flag
    AddToCell = _plateToCell;
    AddToGirder = !_plateToCell;

    m_event_info = 0;
    aStep = 0;
}



//DESTRUCTOR
TileGeoG4CalibSD::~TileGeoG4CalibSD()
{
    delete m_simEn;
    delete m_tile_eep;
    delete m_lookup;
    delete m_lookupDM;
}



//-----------------------------------------------------
// Initialize - CALLED AT THE BEGGINING OF EACH EVENT
//-----------------------------------------------------
void TileGeoG4CalibSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
    if (verboseLevel>5) G4cout << "Initializing SD" << G4endl;

#ifdef ATHENAHIVE
    // Temporary fix for Hive until isValid is fixed
    tileActiveCellCalibHits = CxxUtils::make_unique<CaloCalibrationHitContainer>(tileActiveCellCalibHits.name());
    tileInactiveCellCalibHits = CxxUtils::make_unique<CaloCalibrationHitContainer>(tileInactiveCellCalibHits.name());
    tileDeadMaterialCalibHits = CxxUtils::make_unique<CaloCalibrationHitContainer>(tileDeadMaterialCalibHits.name());
    tileHits = CxxUtils::make_unique<TileHitVector>(tileHits.name());
#else
    if (!tileActiveCellCalibHits.isValid()) tileActiveCellCalibHits = CxxUtils::make_unique<CaloCalibrationHitContainer>(tileActiveCellCalibHits.name());
    if (!tileInactiveCellCalibHits.isValid()) tileInactiveCellCalibHits = CxxUtils::make_unique<CaloCalibrationHitContainer>(tileInactiveCellCalibHits.name());
    if (!tileDeadMaterialCalibHits.isValid()) tileDeadMaterialCalibHits = CxxUtils::make_unique<CaloCalibrationHitContainer>(tileDeadMaterialCalibHits.name());
    if (!tileHits.isValid()) tileHits = CxxUtils::make_unique<TileHitVector>(tileHits.name());
#endif

    //TILECAL IDENTIFIER NUMBER - ALWAYS FIXED
    _subCalo  = 3;
    m_event_info = 0;

}



//-----------------------------------------------------------------------------
//  ProcessHits - CALLED AT EACH STEP INSIDE SENSITIVE VOLUMES
//-----------------------------------------------------------------------------
G4bool TileGeoG4CalibSD::ProcessHits(G4Step* step, G4TouchableHistory* /*ROhist*/)
{
    if (m_calc->GetOptions().doCalibHitParticleID && !m_event_info) 
      m_event_info = dynamic_cast<EventInformation*>(G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());

    if (verboseLevel>10) G4cout << "Process Hits" << G4endl;
    aStep = step;

    //FORCE DEFAULT HIT FLAG TO BE FALSE
    _DefaultHit = false;

    //Reset CalibHit Type
    _calibhit_type = -999;

    //CLASSIFY ENERGY LOSS IN THIS STEP.
    //CONSIDER SPECIAL CASE WHEN THE ESCAPING
    //OF A TILECAL SECONDARY PARTICLE HAS BEEN OCCURED
    if( !(m_tile_eep->GetEscapedFlag()) ) {
        ClassifyEnergy();
        m_simEn->SetStepProcessed();
        _doBirkFlag = true;
    } else {
        if (verboseLevel>5) G4cout << "Escaped energy processing"<< G4endl;
        SetEscapedEnergy(m_tile_eep->GetEscapedEnergy());
        m_simEn->SetStepProcessed();
        m_tile_eep->SetEscapedFlag(false);
        _doBirkFlag = false;
    }

    //THIS METHOD WILL CHECK WHETER ARE ALL CLASSIFIED ENERGIES
    //ZERO OR NOT. IF THEY ARE THEN RETURN FROM ProcessHits
    if(AreClassifiedEnergiesAllZero()) {
        if (verboseLevel>10) G4cout << "all Energies at this step = 0" << G4endl;
        return false;
    }

    //COUNT CLASSIFIED ENERGIES
    EnergiesSimpleCounter();

    //STRING KEY INCLUDED IN STEP VOLUME NAME DEFINES
    //THE CALCULATOR, WHICH SHOULD BE USED TO CALCULATE
    //CALIBRATION HIT IDENTIFIER
    //
    //ALSO, IT DEFINES WHERE THE HIT SHOULD BE STORED (IF IT IS A NEW)
    //OR UPDATED, IN CELL OR DEAD MATERIAL CALIBHIT VECTOR OR IN A DEFAULT HIT
    G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    G4VPhysicalVolume*  physVol      = theTouchable->GetVolume();
    G4String            nameVol      = physVol->GetLogicalVolume()->GetName();



    //------------- H I T   I D E N T I F I E R   C A L C U L A T I O N -----------
    //FIRST OF ALL FIND A TILECAL SECTION WHERE THE STEP IS.
    //IF THE SECTION CAN'T BE FOUND (NOT EXPECTED!) OR THE STEP IS
    //SOMEWHERE IN DEFAULT REGION, THEN THE STEP ENERGIES WILL GO
    //IN DEFAULT HIT IN BOTH CASES.
    //THIS MEANS THAT IDENTIFIER CAN'T BE DETERMINED -> RETURN DEFAULT ID
    bool is_identifiable = FindTileCalibSection();
    if (!is_identifiable) {
        if (verboseLevel>10) G4cout
            << "FindTileCalibSection: The Section can't be found, RETURN DEFAULT ID" << G4endl;
        _DefaultHit = true;
    }

    //SENSITIVE MATERIAL CELL (SCINTILLATOR)
    else if (nameVol.find("Scintillator") != G4String::npos) {
        bool is_scin = m_calc->FindTileScinSection(aStep);
        if (is_scin) {
            //BESIDE A CALIBRATION HIT THERE ORDINARY
            //HITS SHOULD BE ALSO CREATED OR UPDATED
            if  (_doBirkFlag) {
                m_calc->MakePmtEdepTime(aStep);  //calculation of pmtID, edep and scin_Time with aStep
                m_calc->ManageScintHit();  //create or update ordinary hit object in the collection
            }
            ScintIDCalculator();
        } else {
            if (verboseLevel>10) G4cout
                                    << "ProcessHits: FindTileScinSection returns false" << G4endl;
            _DefaultHit = true;
        }
    }

    //CELL PASSIVE MATERIALS
    else if((nameVol.find("Period")  != G4String::npos)  ||
            (nameVol.find("Glue")    != G4String::npos)  ||
            (nameVol.find("Wrapper") != G4String::npos)) {
        CellIDCalculator();
    }

    //FRONT AND END PLATES
    else if((nameVol.find("Plate") != G4String::npos) ||
            (nameVol.find("IrUp")  != G4String::npos) ||
            (nameVol.find("IrDw")  != G4String::npos) ||
            (nameVol.find("IrBox") != G4String::npos) ||
            (nameVol.find("Iron1") != G4String::npos) ||
            (nameVol.find("Iron2") != G4String::npos) ||
            (nameVol.find("Iron3") != G4String::npos) ||
            (nameVol.find("Iron4") != G4String::npos) /* ||
            (nameVol.find("SaddleModule") != G4String::npos) */ ) {
        PlateCellIDCalculator();
    }

    //GIRDER AND FINGER MATERIALS
    else if((nameVol.find("Girder") != G4String::npos) ||
            (nameVol.find("Finger") != G4String::npos) ||
            (nameVol.find("EPHole") != G4String::npos) ||
            (nameVol.find("LArCables") != G4String::npos)  ||
            (nameVol.find("LArService") != G4String::npos) ||
            (nameVol.find("ExtBarrelSaddleSupport") != G4String::npos) ) {
        GirderCellIDCalculator();
    }

    //HIT IS IN THE VOLUME, WHICH  CAN'T BE IDENTIFIED
    //FLAG IT TO BE PUT IN A DEFAULT CALIBHIT
    else {
        _DefaultHit = true;
    }



//-----------------------------------------------------
// --- M A N A G E   C A L I B R A T I O N   H I T ---
//-----------------------------------------------------

    // --------------------------------------------------------------------------
    // G.P.: now comes new hit processing (see LArCalibHitMerger.cxx for details)
    // --------------------------------------------------------------------------
    int m_primary_id = 0;
    if(m_calc->GetOptions().doCalibHitParticleID) {

      if (m_event_info && m_event_info->GetCurrentPrimary()) 
        m_primary_id = m_event_info->GetCurrentPrimary()->barcode();
      else
        throw std::runtime_error("CalibrationSensitiveDetector: Unable to retrieve barcode!");
      
      /*
      else {

	// normal steps should have a G4Track:
	if (step->GetTrack()) {
	  const VTrackInformation* trackInfo=dynamic_cast<const VTrackInformation*>(step->GetTrack()->GetUserInformation());
	  
	  if (trackInfo && trackInfo->GetISFParticle()) {	
	    const ISF::ISFParticle* isfp = trackInfo->GetISFParticle();
	    m_primary_id = isfp->barcode();	
	  }
	  else
	    throw std::runtime_error("CalibrationSensitiveDetector: Unable to retrieve barcode!");
	}
	
	// escaped energy have fake steps with no G4Track, so get particle from m_event_info (retrieving it first if not done already)
	else {
	  if (NULL==m_event_info)  
	    m_event_info = dynamic_cast<EventInformation*>(G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());

	  if (m_event_info && m_event_info->GetCurrentISFPrimary())
	    m_primary_id = m_event_info->GetCurrentISFPrimary()->barcode();
	  else
	    throw std::runtime_error("CalibrationSensitiveDetector: Unable to retrieve barcode!");	    
	}            
      } 
      */
    }

    Identifier m_id = _id;
    if(_DefaultHit) {
        if(m_options.tileTB)
            m_id = this->DM_ID_Maker(5, 2, 0, 0, 0, 0); //Default hit
        else
            m_id = DefaultHitIDCalculator();
    }
    if(!m_id.is_valid()) {
        G4cout << "ERROR: Wrong identifier in ProcessHits()!" << G4endl;
        return true;
    }

    CaloCalibrationHit* hit = new CaloCalibrationHit(m_id,
                                                     _result.energy[CaloG4::SimulationEnergies::kEm],
                                                     _result.energy[CaloG4::SimulationEnergies::kNonEm],
                                                     _result.energy[CaloG4::SimulationEnergies::kInvisible0],
                                                     _result.energy[CaloG4::SimulationEnergies::kEscaped],
                                                     m_primary_id);

    G4double visibleEnergy   = hit->energy(0) + hit->energy(1) + hit->energy(3);
    G4double invisibleEnergy = hit->energy(2);

    if(fabs(invisibleEnergy) < 0.001*CLHEP::eV && visibleEnergy < 0.001*CLHEP::eV ) {
        // Get rid of the hit if we are not going to use it.
        delete hit;
        return true;
    }

    // If we haven't had a hit in this cell before, add the current hit
    // to the hit collection.

    // If we've had a hit in this cell before, then add this to the
    // previously-existing hit.

    // Look for the hit in the m_calibrationHits (this is a binary search).
    m_calibrationHits_ptr_t bookmark;
    if (_calibhit_type==1) { // Cell Active CalibHit - Scintillator
        bookmark = m_activeCalibrationHits.lower_bound(hit);
        if(bookmark == m_activeCalibrationHits.end() || !(*bookmark)->Equals(hit)) {
            // We haven't had a hit in this readout cell before.  Add it to our set.
            if(m_activeCalibrationHits.empty() || bookmark == m_activeCalibrationHits.begin()) {
                // Insert the hit before the first entry in the map.
                m_activeCalibrationHits.insert(hit);
            } else {
                m_activeCalibrationHits.insert(--bookmark, hit);
            }
        } else {
            // Update the existing hit.
            (*bookmark)->Add(hit);
            // We don't need the hit anymore.  Remember that we've adopted
            // the hit, so we're responsible for deleting it if needed.
            delete hit;
        }
    } else if (_calibhit_type==0) { // Cell Inactive CalibHit - Iron, etc. in cell
        bookmark = m_inactiveCalibrationHits.lower_bound(hit);
        if(bookmark == m_inactiveCalibrationHits.end() || !(*bookmark)->Equals(hit)) {
            if(m_inactiveCalibrationHits.empty() || bookmark == m_inactiveCalibrationHits.begin()) {
                m_inactiveCalibrationHits.insert(hit);
            } else {
                m_inactiveCalibrationHits.insert(--bookmark, hit);
            }
        } else {
            (*bookmark)->Add(hit);
            delete hit;
        }
    } else if (_calibhit_type != -999) { // DM CalibHit
        bookmark = m_deadCalibrationHits.lower_bound(hit);
        if(bookmark == m_deadCalibrationHits.end() || !(*bookmark)->Equals(hit)) {
            if(m_deadCalibrationHits.empty() || bookmark == m_deadCalibrationHits.begin()) {
                m_deadCalibrationHits.insert(hit);
            } else {
                m_deadCalibrationHits.insert(--bookmark, hit);
            }
        } else {
            (*bookmark)->Add(hit);
            delete hit;
        }
    } else {
        delete hit;
    }

    //STEP IS PROCESSED SUCCESFULY
    return true;
}



//-----------------------------------------------------
//    EndOfEvent - CALLED AT THE END OF EACH EVENT
//-----------------------------------------------------
void TileGeoG4CalibSD::EndOfAthenaEvent()
{
    if (verboseLevel>10) G4cout << "Store Hits" << G4endl;

    //CREATE CALIBHITS FROME THEIR VECTORS AND
    //STORE THEM IN THE RESPECTIEVE CONTAINERS
    m_calibrationHits_ptr_t it;

    // Cell Active Material Container
    tileActiveCellCalibHits->reserve(m_activeCalibrationHits.size());
    for( auto &it : m_activeCalibrationHits ) {
        tileActiveCellCalibHits->push_back(std::move(it));
    }

    // Cell Inactive Material Container
    tileInactiveCellCalibHits->reserve(m_inactiveCalibrationHits.size());
    for( auto &it : m_inactiveCalibrationHits ) {
        tileInactiveCellCalibHits->push_back(std::move(it));
    }

    // Tile Dead Material Container
    tileDeadMaterialCalibHits->reserve(m_deadCalibrationHits.size());
    for( auto &it : m_deadCalibrationHits ) {
        tileDeadMaterialCalibHits->push_back(std::move(it));
    }

    // copy ordinary hits to tileHits vector and reset all pointers
    m_lookup->ResetCells(&*tileHits);

#ifdef HITSINFO  // added by Sergey
    m_ntupleCnt->StoreCNT(&*tileActiveCellCalibHits,&*tileInactiveCellCalibHits,&*tileDeadMaterialCalibHits);
#endif

    //DEBUG CALIBHITS ENERGIES CALCULATED BY SIMPLE CALCULATOR
    //AND DEBUG THE SAME ENERIES DIRECTLE FROM CALIBHITS. THEY
    //MUST BE THE SAME IF EVERITHING WENT ON WELL
    //
    //NOTE: ATHENA DEBUGGING LEVEL SHOULD BE SET 'INFO' OR HIGHER
    DebugEnergies();

    m_activeCalibrationHits.clear();
    m_inactiveCalibrationHits.clear();
    m_deadCalibrationHits.clear();

    //RESET CELL, GIRDER CELL AND
    //PLATE CELL HITS COUNTER VECTORS
    m_lookupDM->ResetCells();

#ifdef HITSINFO  //INCREMENT OF EVENT COUNTER -- added by Sergey
    if (doHitsNTup || doHitsTXT) nEvent++;
#endif

}

