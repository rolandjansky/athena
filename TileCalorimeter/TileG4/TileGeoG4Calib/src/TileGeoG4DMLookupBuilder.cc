/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class building look-up tables for TileGeoG4CalibSD
//
// Author: Gia Khoriauli <gia.khoriauli@cern.ch>
//
// May, 2005
//
// DESCRIPTION:
//  The additional Look-up builder for Calibration Hit
//  recording. Must be called after the 'ordinary' Tile
//  Look-up builder has been already called
//
//************************************************************

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelUtilities/GeoModelExperiment.h"

//'ordinary' Look-up & its Builder
#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"

//Calibration Look-up & its Builder
#include "TileGeoG4DMLookupBuilder.h"
#include "TileGeoG4DMLookup.h"

//DB Manager
#include "TileDetDescr/TileDddbManager.h"
#include "TileCalibDddbManager.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "G4ios.hh"
#include "CLHEP/Units/SystemOfUnits.h"

/////////// C O N S T R U C T O R

TileGeoG4DMLookupBuilder::TileGeoG4DMLookupBuilder(TileGeoG4LookupBuilder* lookup_builder,
                                                   ServiceHandle<IRDBAccessSvc> &raccess,
                                                   ServiceHandle<IGeoModelSvc> &geo_svc,
                                                   const ServiceHandle<StoreGateSvc> &pDetStore, const int verboseLevel)
: rBMin(0),
  rBMax(0),
  zBarrMaxPos(0),
  zBarrMaxNeg(0),
  dzBarrMod(0),
  dzExtBarrMod(0),
  zLegngthITC(0),
  dzBarrPeriod(0),
  dzExtBarrPeriod(0),
  rGirdMin(0),
  dRFront(0),
  dZEnd(0),
  dZEndSh(0),
  rP1Min(0),
  rP2Min(0),
  rGapMax(0),
  rGapMin(0),
  rCrMax(0),
  rCrMin(0),
  m_dbManager(0),
  m_lookup_builder(lookup_builder),
  m_sectionMap(0),
  m_tdbManager(0),
  m_verboseLevel(verboseLevel),
  m_plateToCell(false)
{

  const DataHandle<GeoModelExperiment> theExpt;
  StatusCode sc = pDetStore->retrieve(theExpt, "ATLAS");
  if (sc.isFailure()) {
    G4cout << "ERROR: Unable to retrieve GeoModelExperiment from DetectorStore" << G4endl;
    abort();
  }

  m_theManager = dynamic_cast<const TileDetDescrManager*>(theExpt->getManager("Tile"));
  if (m_theManager)
    m_tdbManager = m_theManager->getDbManager();
  else {
    G4cout << "ERROR: Cannot get TileDetDescrManager" << G4endl;
    abort();
  }

  std::string versionTag =
    (geo_svc->tileVersionOverride()).empty() ? geo_svc->atlasVersion() : geo_svc->tileVersionOverride();
  std::string versionNode = (geo_svc->tileVersionOverride()).empty() ? "ATLAS" : "TileCal";

  //m_dbManager = new TileCalibDddbManager(raccess,"ATLAS-00","ATLAS");
  m_dbManager = new TileCalibDddbManager(raccess, versionTag, versionNode, verboseLevel);
}

TileGeoG4DMLookupBuilder::~TileGeoG4DMLookupBuilder() {
  delete m_dbManager;
}

///////////////// B U I L D E R

void TileGeoG4DMLookupBuilder::BuildLookup(bool is_tb, int plateToCell) {
  // initializations
  m_sectionMap = new TileGeoG4CalibSectionMap();
  
  // Building Section Look-up tables for Calibration Hits
  CreateGeoG4CalibSections(is_tb, plateToCell);
}

//////////////////// G E T   S E C T I O N

TileGeoG4CalibSection* TileGeoG4DMLookupBuilder::GetSection(TileCalibDddbManager::TileCalibSections key) const {
  if (m_sectionMap && m_sectionMap->find(key) != m_sectionMap->end())
    return m_sectionMap->operator[](key);
  else
    return 0;
}

//////// C E L L   R E S E T

void TileGeoG4DMLookupBuilder::ResetCells() {
  TileGeoG4CalibSectionMap::iterator first = m_sectionMap->begin();
  TileGeoG4CalibSectionMap::iterator last = m_sectionMap->end();

  TileGeoG4CalibSample* sample;
  TileGeoG4PlateCell* pl_cell;
  TileGeoG4GirderCell* gr_cell;

  unsigned int l_nSamp, l_nCell;
  unsigned int j;

  for (; first != last; first++) {
    //Reset PlateCells
    for (l_nSamp = 0; l_nSamp < first->second->samples.size(); l_nSamp++) {
      sample = first->second->samples[l_nSamp];
      for (l_nCell = 0; l_nCell < sample->plateCells.size(); l_nCell++) {
        pl_cell = sample->plateCells[l_nCell];
        for (j = 0; j < pl_cell->m_nPlateCellHit.size(); j++)
          pl_cell->m_nPlateCellHit[j] = 0;
        for (j = 0; j < pl_cell->m_nPlateCellHit_negative.size(); j++)
          pl_cell->m_nPlateCellHit_negative[j] = 0;
      }
    }
    //Reset GirderCells
    for (l_nCell = 0; l_nCell < first->second->girderCells.size(); l_nCell++) {
      gr_cell = first->second->girderCells[l_nCell];
      for (j = 0; j < gr_cell->m_nGirderCellHit.size(); j++)
        gr_cell->m_nGirderCellHit[j] = 0;
      for (j = 0; j < gr_cell->m_nGirderCellHit_negative.size(); j++)
        gr_cell->m_nGirderCellHit_negative[j] = 0;
    }
  }
}

bool TileGeoG4DMLookupBuilder::GetPlateToCell() {
  return m_plateToCell;
}

//////////////// C R E A T E    S E C T I O N S

void TileGeoG4DMLookupBuilder::CreateGeoG4CalibSections(bool is_tb, int plateToCell) {
  if (!m_dbManager || !m_sectionMap) {
    G4cout << "ERROR: CreateGeoG4CalibSections() - Initialization failed"
           << "  DbManager = " << m_dbManager
           << "  SectionMap = " << m_sectionMap << G4endl;
    abort();
  }
  
  //plateToCell parameter
  if (plateToCell == -1) {
    m_plateToCell = m_dbManager->GetSwitchPlateToCell();
    G4cout << "Using plateToCell flag from GeoModel" << G4endl;
  } else {
    m_plateToCell = (plateToCell>0);
    G4cout << "Using plateToCell flag from jobOptions" << G4endl;
  }

  //geometry
  m_tdbManager->SetCurrentEnvByType(1);
  rBMin = m_tdbManager->GetEnvRin() * CLHEP::cm;
  rBMax = m_tdbManager->GetEnvRout() * CLHEP::cm;
  zBarrMaxPos = (m_tdbManager->GetEnvZLength() / 2.0) * CLHEP::cm;
  zBarrMaxNeg = (( -1) * m_tdbManager->GetEnvZLength() / 2.0) * CLHEP::cm;

  for (int ii = m_tdbManager->GetNumberOfEnv() - 1; ii >= 0; --ii) {
    m_tdbManager->SetCurrentEnvByIndex(ii);
    int env_type = m_tdbManager->GetEnvType();
    if (env_type == 5) {
      zBarrMaxPos += m_tdbManager->GetEnvDZ() * CLHEP::cm;
      zLegngthITC = m_tdbManager->GetEnvZLength() * CLHEP::cm;
    } else if (env_type == 4) {
      zBarrMaxNeg += m_tdbManager->GetEnvDZ() * CLHEP::cm;
      zLegngthITC = m_tdbManager->GetEnvZLength() * CLHEP::cm;
    }
  }
  
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
  rGirdMin = m_tdbManager->TILBrmax() * CLHEP::cm;
  dRFront = m_tdbManager->TILBdrfront() * CLHEP::cm;
  dZEnd = m_tdbManager->TILBdzend() * CLHEP::cm;
  dzBarrMod = m_tdbManager->TILBdzmodul() * CLHEP::cm;
  dzBarrPeriod = (dzBarrMod - 2 * dZEnd + m_tdbManager->TILBdzmast() * CLHEP::cm) / m_tdbManager->TILBnperiod();
  
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
  dzExtBarrMod = m_tdbManager->TILBdzmodul() * CLHEP::cm;
  dZEndSh = m_tdbManager->TILBdzend1() * CLHEP::cm;
  dzExtBarrPeriod = (dzExtBarrMod - dZEnd - dZEndSh) / m_tdbManager->TILBnperiod();
  
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
  rP1Min = m_tdbManager->TILBrmin() * CLHEP::cm;
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
  rP2Min = m_tdbManager->TILBrmin() * CLHEP::cm;
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);
  rGapMin = m_tdbManager->TILBrmin() * CLHEP::cm;
  rGapMax = m_tdbManager->TILBrmax() * CLHEP::cm;
  m_tdbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
  rCrMin = m_tdbManager->TILBrmin() * CLHEP::cm;
  rCrMax = m_tdbManager->TILBrmax() * CLHEP::cm;
  //

  TileGeoG4CalibSection* calibSection;
  TileGeoG4CalibSample* calibSample;
  TileGeoG4CalibCell* calibCell;
  TileGeoG4PlateCell* plateCell;
  TileGeoG4GirderCell* girderCell;
  
  TileCalibDddbManager::TileCalibSections key;
  TileDddbManager::TileSections v_key;

  int j, counter, sampleNum;
  
  //Define number of Sections & Modules due to the selected detector geometry

  int nSections = (is_tb) ? 4 : 6;
  if (m_tdbManager->GetNumberOfEnv() == 1)
    nSections = 1;

  int nModules = (is_tb) ? 3 : 64;
  for (int ii = m_tdbManager->GetNumberOfEnv() - 1; ii >= 0; --ii) {
    m_tdbManager->SetCurrentEnvByIndex(ii);
    nModules = std::max(nModules, m_tdbManager->GetEnvNModules());
  }

  //Loop over the Tile sections
  for (counter = 1; counter <= nSections; counter++) {

    //Retrieve current Section from DB
    key = TileCalibDddbManager::TileCalibSections(counter);
    m_dbManager->SetCurrentSection(key);
    
    //Take a pointer to the "ordinary" Tile Section
    //Needed to retrieve Cells from "ordinary" Look-up
    v_key = TileDddbManager::TileSections(counter);
    TileGeoG4Section* tile_section = m_lookup_builder->GetSection(v_key);

    //Create & initialize Section
    calibSection = new TileGeoG4CalibSection(m_verboseLevel);
    calibSection->section = static_cast<int>(m_dbManager->GetCurrentSection());
    calibSection->nrOfPeriods = static_cast<int>(m_dbManager->GetNumSectPeriods());
    calibSection->nrOfSamples = static_cast<int>(m_dbManager->GetNumSectSamples());
    calibSection->nrOfCells = static_cast<int>(m_dbManager->GetNumSectCells());
    calibSection->nrOfPlateCells = static_cast<int>(m_dbManager->GetNumSectPlateCells());
    calibSection->nrOfGirderCells = static_cast<int>(m_dbManager->GetNumSectGirderCells());
    for (int i = 0; i < 4; i++) {
      if ((i == 3) && (m_plateToCell)) {
        calibSection->sample_ZBound[i] = 9999.9 * CLHEP::cm;
      } else {
        calibSection->sample_ZBound[i] = static_cast<double>(m_dbManager->GetSampleZBound(i)) * CLHEP::cm;
      }
    }

    //------------------------------------------ C E L L ------------------------------------------

    m_dbManager->SetFirstDetCell(calibSection->section);

    //Make first Cell of the current Section
    calibCell = new TileGeoG4CalibCell();
    calibCell->sample = static_cast<int>(m_dbManager->GetCellSample());
    calibCell->detector = static_cast<int>(m_dbManager->GetCellDetector());
    calibCell->nrOfPeriodsInCell[0] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(0));
    calibCell->nrOfPeriodsInCell[1] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(1));

    //Create Section first Sample & Store the first Cell of current Section
    calibSample = new TileGeoG4CalibSample();
    calibSample->cells.push_back(calibCell);

    sampleNum = calibCell->sample;

    //Loop over the other Section Cells in DB and store them into Samples
    while (m_dbManager->SetNextDetCell()) {

      //Create & initialize Cell
      calibCell = new TileGeoG4CalibCell();
      calibCell->sample = static_cast<int>(m_dbManager->GetCellSample());
      calibCell->detector = static_cast<int>(m_dbManager->GetCellDetector());
      calibCell->nrOfPeriodsInCell[0] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(0));
      calibCell->nrOfPeriodsInCell[1] = static_cast<int>(m_dbManager->GetNumOfPeriodsInCell(1));

      //Check the Cell is it from the next Sample or not
      if (sampleNum != calibCell->sample) {

        //Store the current Sample & Create the new one
        calibSection->samples.push_back(calibSample);
        calibSample = nullptr;
        calibSample = new TileGeoG4CalibSample();

        sampleNum = calibCell->sample;
      }
      
      //Store a Cell into Sample
      calibSample->cells.push_back(calibCell);

    }
    //Don't forget store last Sample
    calibSection->samples.push_back(calibSample);

    //check cells2sample structure
    if (m_verboseLevel >= 10) {
      for (int samp = 0; samp < static_cast<int>(calibSection->samples.size()); samp++) {
        for (int cell = 0; cell < static_cast<int>(calibSection->samples[samp]->cells.size()); cell++) {
          G4cout << "samp=" << samp << "  cell_ind=" << cell
                 << " :  nrOfPeriodsInCell[0]= " << calibSection->samples[samp]->cells[cell]->nrOfPeriodsInCell[0]
                 << " ,  nrOfPeriodsInCell[1] = " << calibSection->samples[samp]->cells[cell]->nrOfPeriodsInCell[1]
                 << " ,  detector = " << calibSection->samples[samp]->cells[cell]->detector
                 << " ,  sample = " << calibSection->samples[samp]->cells[cell]->sample
                 << G4endl;
        }
      }
    }
    
    //Build a corespondence vector
    //between Cell and Sample/Period
    if ((key != TileCalibDddbManager::TILE_PLUG3) && (key != TileCalibDddbManager::TILE_PLUG4))
      calibSection->DMToCell(false, tile_section);
    else {
      calibSection->DMToCell(true, tile_section);
    }

    //----------------------------------- P L A T E   C E L L ------------------------------------
    
    //Clean up Samples plateCell vectors. ...for any case
    for (int samp = 0; samp < static_cast<int>(calibSection->samples.size()); samp++) {
      for (int pcell = 0; pcell < static_cast<int>(calibSection->samples[samp]->plateCells.size()); pcell++)
        delete calibSection->samples[samp]->plateCells[pcell];
      calibSection->samples[samp]->plateCells.clear();
    }

    //Set current PlateCell
    if (m_dbManager->SetFirstDetPlateCell(calibSection->section)) {

      //Make first PlateCell of the current Section
      plateCell = new TileGeoG4PlateCell();
      plateCell->detector = static_cast<int>(m_dbManager->GetPlateCellDetector());
      plateCell->tower = static_cast<int>(m_dbManager->GetPlateCellTower());
      plateCell->sample = static_cast<int>(m_dbManager->GetPlateCellSample());
      plateCell->eta = static_cast<double>(m_dbManager->GetPlateCellEta());
      plateCell->dEta = static_cast<double>(m_dbManager->GetPlateCellDeta());
      plateCell->xBound = static_cast<double>(m_dbManager->GetPlateCellXBound()) * CLHEP::cm;
      plateCell->zBound = static_cast<double>(m_dbManager->GetPlateCellZBound()) * CLHEP::cm;
      plateCell->neighbor = static_cast<int>(m_dbManager->GetPlateCellNeighborPeriod());
      
      //Samples are already created
      calibSample = calibSection->samples[0];
      calibSample->plateCells.push_back(plateCell);
      
      sampleNum = plateCell->sample;
      
      //Sample iterator
      unsigned int n_samp = 0;
      
      // iterate over the PlateCells & store them into Samples
      while (m_dbManager->SetNextDetPlateCell()) {

        //Create & initialize PlateCell
        plateCell = new TileGeoG4PlateCell();
        plateCell->detector = static_cast<int>(m_dbManager->GetPlateCellDetector());
        plateCell->tower = static_cast<int>(m_dbManager->GetPlateCellTower());
        plateCell->sample = static_cast<int>(m_dbManager->GetPlateCellSample());
        plateCell->eta = static_cast<double>(m_dbManager->GetPlateCellEta());
        plateCell->dEta = static_cast<double>(m_dbManager->GetPlateCellDeta());
        plateCell->xBound = static_cast<double>(m_dbManager->GetPlateCellXBound()) * CLHEP::cm;
        plateCell->zBound = static_cast<double>(m_dbManager->GetPlateCellZBound()) * CLHEP::cm;
        plateCell->neighbor = static_cast<int>(m_dbManager->GetPlateCellNeighborPeriod());

        //Check the PlateCell is it from the next Sample or not
        if (sampleNum != plateCell->sample) {
          //Take the next Sample. If it is necessary create more Samples
          sampleNum = plateCell->sample;
          if (++n_samp < calibSection->samples.size()) {
            //The 'old' Sample should be used
            //Protection against messages from Coverity tool ?
            calibSample = nullptr;
            calibSample = calibSection->samples[n_samp];
          } else {
            //If new Sample was already created then store it in Section
            if (n_samp > calibSection->samples.size())
              calibSection->samples.push_back(calibSample);
            //The new Sample is necessary.
            //Protection against messages from Coverity tool ?
            calibSample = nullptr;
            calibSample = new TileGeoG4CalibSample();
          }
        }

        //Store a PlateCell into Sample
        calibSample->plateCells.push_back(plateCell);
      }
      
      //Don't forget store last Sample if it was created as a new Sample
      if (++n_samp > calibSection->samples.size())
        calibSection->samples.push_back(calibSample);
    }

    // check plateCells2sample structure
    if (m_verboseLevel >= 10) {
      for (int psamp = 0; psamp < static_cast<int>(calibSection->samples.size()); psamp++) {
        for (int pcell = 0; pcell < static_cast<int>(calibSection->samples[psamp]->plateCells.size()); pcell++) {
          G4cout << "psamp=" << psamp << "  cell_ind=" << pcell
                 << " :  detector   = " << calibSection->samples[psamp]->plateCells[pcell]->detector
                 << " ,  tower    = " << calibSection->samples[psamp]->plateCells[pcell]->tower
                 << " ,  sample = " << calibSection->samples[psamp]->plateCells[pcell]->sample
                 << G4endl;
        }
      }
    }

    //---------------------------------- G I R D E R   C E L L -------------------------------------
    //
    // comment: there are the Barrel & Extended Fingers jointed to Girders
    
    //Clean up Section girderCell vector. ...for any case
    for (int gcell = 0; gcell < static_cast<int>(calibSection->girderCells.size()); gcell++)
      delete calibSection->girderCells[gcell];
    calibSection->girderCells.clear();
    
    if (m_dbManager->SetFirstDetGirderCell(calibSection->section)) {

      do {

        //Create & initialize GirderCell
        girderCell = new TileGeoG4GirderCell();
        girderCell->detector = static_cast<int>(m_dbManager->GetGirderCellDetector());
        girderCell->tower = static_cast<int>(m_dbManager->GetGirderCellTower());
        girderCell->sample = static_cast<int>(m_dbManager->GetGirderCellSample());
        girderCell->eta = static_cast<double>(m_dbManager->GetGirderCellEta());
        girderCell->dEta = static_cast<double>(m_dbManager->GetGirderCellDeta());
        girderCell->xBound = static_cast<double>(m_dbManager->GetGirderCellXBound()) * CLHEP::cm;

        calibSection->girderCells.push_back(girderCell);

      } while (m_dbManager->SetNextDetGirderCell());

    }
    
    // check girder cells in the section
    if (m_verboseLevel >= 10) {
      for (int gcell = 0; gcell < static_cast<int>(calibSection->girderCells.size()); gcell++) {
        G4cout << "cell_ind=" << gcell
               << " :  detector   = " << calibSection->girderCells[gcell]->detector
               << " ,  tower    = " << calibSection->girderCells[gcell]->tower
               << " ,  sample = " << calibSection->girderCells[gcell]->sample
               << G4endl;
      }
    }

    //---------------------- M O D U L E S --------------------

    // Initialize ModuleToCell values
    for (j = 0; j < nModules; j++) {
      calibSection->AddModuleToCell(false);
      if (key != TileCalibDddbManager::TILE_BARREL) {
        calibSection->AddModuleToCell(true);
      }
    }

    //Store Section in map
    m_sectionMap->operator[](key) = calibSection;

  } //End of Section loop
}  //END

