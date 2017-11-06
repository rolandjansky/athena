/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class building look-up tables for TileGeoG4SD
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
//************************************************************

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelUtilities/GeoModelExperiment.h"

#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"

#include "CaloDetDescr/CaloDetDescrElement.h"

#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileCellDim.h"    //added by Sergey

#include "G4ios.hh"

TileGeoG4LookupBuilder::TileGeoG4LookupBuilder(StoreGateSvc* pDetStore, const int verboseLevel)
  : m_tileID(0),
    m_dbManager(0),
    m_cellMap(0),
    m_sectionMap(0),
    m_isE5(false),
    m_verboseLevel(verboseLevel) {

  const DataHandle<GeoModelExperiment> theExpt;
  StatusCode sc = pDetStore->retrieve(theExpt, "ATLAS");
  if (sc.isFailure()) {
    G4cout << "ERROR: Unable to retrieve GeoModelExperiment from DetectorStore" << G4endl;
    abort();
  }

  m_theManager = dynamic_cast<const TileDetDescrManager*>(theExpt->getManager("Tile"));
  if (m_theManager) m_dbManager = m_theManager->getDbManager();
  else {
    G4cout << "ERROR: Cannot get TileDetDescrManager" << G4endl;
    abort();
  }

  // retrieve TileID helper from det store
  sc = pDetStore->retrieve(m_tileID);
  if (sc.isFailure()) {
    G4cout << "ERROR: Unable to retrieve TileID helper from DetectorStore" << G4endl;
    abort();
  }
    }

TileGeoG4LookupBuilder::~TileGeoG4LookupBuilder() {
  delete m_cellMap;
  delete m_sectionMap;
}

void TileGeoG4LookupBuilder::BuildLookup(bool is_tb) {
  // initializations
  m_cellMap = new TileGeoG4CellMap();
  m_sectionMap = new TileGeoG4SectionMap();
  m_isE5 = (m_dbManager->SetCurrentSection(10 + TileDddbManager::TILE_PLUG4, false));
  // Building
  CreateGeoG4Cells();
  CreateGeoG4Sections(is_tb);

  // clean up
  m_cellMap->clear();

  /*
  // Debug printings
  TileGeoG4SectionMap::const_iterator first = m_sectionMap->begin();
  TileGeoG4SectionMap::const_iterator last = m_sectionMap->end();
  for(;first!=last;first++)
  first->second->PrintScinToCell("");
  // ---------------- Debug printings
  */
}

TileGeoG4Section* TileGeoG4LookupBuilder::GetSection(TileDddbManager::TileSections key) const {
  if (m_sectionMap && m_sectionMap->find(key) != m_sectionMap->end()) return m_sectionMap->operator[](key);
  else return 0;
}

void TileGeoG4LookupBuilder::ResetCells(TileHitVector* tileHitsCollection) {
  TileGeoG4SectionMap::iterator first = m_sectionMap->begin();
  TileGeoG4SectionMap::iterator last = m_sectionMap->end();

  TileGeoG4Sample* sample;
  TileGeoG4Cell* cell;
  size_t l_nSamp, l_nCell, j;

  for (; first != last; first++) {
    for (l_nSamp = 0; l_nSamp < first->second->samples.size(); l_nSamp++) {
      sample = first->second->samples[l_nSamp];
      for (l_nCell = 0; l_nCell < sample->cells.size(); l_nCell++) {
        cell = sample->cells[l_nCell];
        for (j = 0; j < cell->moduleToHitUp.size(); j++) {
          if (cell->moduleToHitUp[j]) {
            tileHitsCollection->Insert(TileHit(cell->moduleToHitUp[j]));
            delete cell->moduleToHitUp[j];
            cell->moduleToHitUp[j] = 0;
          }
        }
        for (j = 0; j < cell->moduleToHitDown.size(); j++) {
          if (cell->moduleToHitDown[j]) {
            tileHitsCollection->Insert(TileHit(cell->moduleToHitDown[j]));
            delete cell->moduleToHitDown[j];
            cell->moduleToHitDown[j] = 0;
          }
        }
        for (j = 0; j < cell->moduleToHitUpNegative.size(); j++) {
          if (cell->moduleToHitUpNegative[j]) {
            tileHitsCollection->Insert(TileHit(cell->moduleToHitUpNegative[j]));
            delete cell->moduleToHitUpNegative[j];
            cell->moduleToHitUpNegative[j] = 0;
          }
        }
        for (j = 0; j < cell->moduleToHitDownNegative.size(); j++) {
          if (cell->moduleToHitDownNegative[j]) {
            tileHitsCollection->Insert(TileHit(cell->moduleToHitDownNegative[j]));
            delete cell->moduleToHitDownNegative[j];
            cell->moduleToHitDownNegative[j] = 0;
          }
        }
        
        //reset CalibHit counter vectors (added by G.Kh)
        for (j = 0; j < cell->nCellHit.size(); j++)
          cell->nCellHit[j] = 0;
        for (j = 0; j < cell->nCellHitNegative.size(); j++)
          cell->nCellHitNegative[j] = 0;
      }
    }
  }
}

void TileGeoG4LookupBuilder::CreateGeoG4Cells() {
  if (!m_dbManager || !m_cellMap) {
    G4cout << "ERROR: CreateGeoG4Cells() - Initialization failed" << "  DdManager = " << m_dbManager << "  CellMap = "
           << m_cellMap << G4endl;
    abort();
  }

  TileGeoG4Cell *cell;
  std::string nameCell;
  std::string cellPrefix;
  int counter, i;

  char* buff = new char[5];

  int nCounter = m_dbManager->GetNumTicl();
  if (m_dbManager->GetNumberOfEnv() == 1) {
    G4cout << "WARNING: CreateGeoG4Cells() - nCells from DB " << nCounter << G4endl;
    nCounter = 45;
    G4cout << "WARNING: CreateGeoG4Cells() - Changing nCells for barrel-only configuration to " << nCounter << G4endl;
  }

  for (counter = 0; counter < nCounter; counter++) {
    m_dbManager->SetCurrentTicl(counter);

    //Create cell name depending of the detector and sample
    switch (static_cast<int>(m_dbManager->TICLdetector())) {
    case 1:  // Central Barrel
      {
        if (static_cast<int>(m_dbManager->TICLsample()) == 1) {
          cellPrefix = "A";
          if (m_dbManager->TICLncell() < 0)
            cellPrefix += "neg";
          else
            cellPrefix += "pos";
        } else if (static_cast<int>(m_dbManager->TICLsample()) == 2) {
          cellPrefix = "BC";
          if (m_dbManager->TICLncell() < 0)
            cellPrefix += "neg";
          else
            cellPrefix += "pos";
        } else if (static_cast<int>(m_dbManager->TICLsample()) == 3) {
          cellPrefix = "D";
          if (m_dbManager->TICLncell() < 0)
            cellPrefix += "neg";
          else if (m_dbManager->TICLncell() > 0)
            cellPrefix += "pos";
        }
        break;
      }
    case 2:  // Extended Barrel
      {
        if (static_cast<int>(m_dbManager->TICLsample()) == 1)
          cellPrefix = "A";
        else if (static_cast<int>(m_dbManager->TICLsample()) == 2)
          cellPrefix = "B";
        else if (static_cast<int>(m_dbManager->TICLsample()) == 3)
          cellPrefix = "D";
        break;
      }
    case 3:  // ITC
      {
        if (static_cast<int>(m_dbManager->TICLsample()) == 2)
          cellPrefix = "C";
        else if (static_cast<int>(m_dbManager->TICLsample()) == 3)
          cellPrefix = "D";
        break;
      }
    case 4:  // Gap/Crack
      {
        cellPrefix = "E";
        break;
      }
    default: {
      G4cout << "ERROR: CreateGeoG4Cells() - Unexpected detector ---> " << m_dbManager->TICLdetector() << G4endl;
      abort();
    }
    }

    sprintf(buff, "%i", abs(static_cast<int>(m_dbManager->TICLncell())));
    nameCell = cellPrefix + std::string(buff);

    if (m_cellMap->find(nameCell) != m_cellMap->end()) {
      G4cout << "ERROR CreateGeoG4Cells() - Attempt to recreate GeoG4Cell with name ---> " << nameCell << G4endl;
      abort();
    }

    // Create and fill in the GeoG4Cell
    cell = new TileGeoG4Cell();
    cell->detector = m_dbManager->TICLdetector();
    cell->cellNum = static_cast<int>(m_dbManager->TICLncell());
    cell->tower = static_cast<int>(m_dbManager->TICLtower());
    cell->sample = static_cast<int>(m_dbManager->TICLsample());
    cell->firstRow = static_cast<int>(m_dbManager->TICLfirstrow());
    cell->lastRow = static_cast<int>(m_dbManager->TICLlastrow());

    int mtower = std::min(abs(cell->tower), 16);
    if (cell->tower < 0)
      mtower *= -1;
    Identifier cell_id = m_tileID->cell_id(std::min(cell->detector, 3), (int)copysign(1.1, mtower), 0, abs(mtower) - 1, cell->sample - 1);
    CaloDetDescrElement * caloDDE = m_theManager->get_cell_element(cell_id);
    cell->sinTh = caloDDE->sinTh();
    cell->r = caloDDE->r();
    if (m_verboseLevel > 5)
      G4cout << "  Cell " << nameCell
             << ": cell_id=" << m_tileID->to_string(cell_id, -2)
             << "  r=" << cell->r
             << "  sinTh=" << cell->sinTh << G4endl;

    //added by Sergey
    TileCellDim* cellDim = m_theManager->get_cell_dim(cell_id);
    // Zmin and Zmax - position of first tile row in a cell along Z axis
    cell->zMin = cellDim->getZMin(0);
    cell->zMax = cellDim->getZMax(0);
    if (cell->detector > 1) {
      // ID for negative side
      cell_id = m_tileID->cell_id(std::min(cell->detector, 3), -(int)copysign(1.1, mtower), 0, abs(mtower) - 1, cell->sample - 1);
      // (*m_log)<< MSG::DEBUG  << " cell_id2="<<m_tileID->to_string(cell_id,-2)<<endreq;
      cellDim = m_theManager->get_cell_dim(cell_id);
      // Zmin2 and Zmax2 in ext.barrel - cell Z coordinates of EBC (negative ext.barrel)
      cell->zMin2 = cellDim->getZMin(0);
      cell->zMax2 = cellDim->getZMax(0);
    } else {
      // Zmin2 and Zmax2 in barrel - Z position of last tile row in a cell (needed for BC cells)
      cell->zMin2 = cellDim->getZMin(cell->lastRow - cell->firstRow);
      cell->zMax2 = cellDim->getZMax(cell->lastRow - cell->firstRow);
    }

    if (cell->tower > 16) { // correction for E4'
      double eta = -1.66;
      double theta = 2. * atan(exp( -eta));
      double z = (cell->zMin2 + cell->zMax2) / 2.;
      cell->sinTh = sin(theta);
      cell->r = z * tan(theta);
      if (m_verboseLevel > 5)
        G4cout << "  Cell " << nameCell << " corrected values: "
               << "  r=" << cell->r
               << "  sinTh=" << cell->sinTh
               << "  z=" << z
               << "  eta=" << eta << G4endl;
    }

    // (*m_log)<< MSG::DEBUG <<" Zmin="<<_cell->zMin<<"  Zmax="<<_cell->zMax
    // <<"  Zmin2="<<_cell->zMin2<<"  Zmax2="<<_cell->zMax2<<endreq;

    for (i = 0; i <= (cell->lastRow - cell->firstRow); i++)
      cell->nrOfTilesInRow.push_back(static_cast<int>(m_dbManager->TICLntilesrow(i)));

    cell->nrOfPMT = static_cast<int>(m_dbManager->TICLnpmt());

    for (i = 0; i < cell->nrOfPMT; i++)
      cell->holes.push_back(static_cast<int>(m_dbManager->TICLholes(i)));

    // Put it there...
    m_cellMap->operator[](nameCell) = cell;
  }
  delete[] buff;
}

void TileGeoG4LookupBuilder::CreateGeoG4Sections(bool is_tb) {
  if (!m_dbManager || !m_sectionMap) {
    G4cout << "ERROR: CreateGeoG4Sections() - Initialization failed" << "  DbManager = " << m_dbManager
           << "  SectionMap = " << m_sectionMap << G4endl;
    abort();
  }

  TileGeoG4Section* section;
  TileGeoG4Sample* sample;
  TileGeoG4Cell* cell;

  TileDddbManager::TileSections key;
  int j, counter;

  int sampleNum;
  int rowShift;

  std::vector<TileGeoG4Cell*> sectionCells;
  std::string cellPrefix;
  std::string cellName;

  char* buff = new char[5];

  int nSections = (is_tb) ? 4 : m_dbManager->GetNumTilb();
  if (m_dbManager->GetNumberOfEnv() == 1)
    nSections = 1;
  nSections = std::min(6, nSections);

  int nModules = (is_tb) ? 3 : 64;
  for (int ii = m_dbManager->GetNumberOfEnv() - 1; ii >= 0; --ii) {
    m_dbManager->SetCurrentEnvByIndex(ii);
    nModules = std::max(nModules, m_dbManager->GetEnvNModules());
  }

  for (counter = 1; counter <= nSections; counter++) {
    key = TileDddbManager::TileSections(counter);

    switch (key) {
    case TileDddbManager::TILE_BARREL: {
      // Collect all cells for this section
      // SAMPLE A
      cellPrefix = "Aneg";
      for (j = 10; j > 0; j--) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }
      cellPrefix = "Apos";
      for (j = 1; j < 11; j++) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }

      //SAMPLE BC
      cellPrefix = "BCneg";
      for (j = 9; j > 0; j--) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }
      cellPrefix = "BCpos";
      for (j = 1; j < 10; j++) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }

      //SAMPLE D
      cellPrefix = "Dneg";
      for (j = 3; j > 0; j--) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }
      cellName = "D0";
      sectionCells.push_back(m_cellMap->operator[](cellName));
      cellPrefix = "Dpos";
      for (j = 1; j < 4; j++) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }
      if (m_verboseLevel>5)
        G4cout << " TILE_BARREL: samples A, BC, D are filled" << G4endl;
      break;
    }
    case TileDddbManager::TILE_EBARREL: {
      // Collect all cells for this section
      // SAMPLE A
      cellPrefix = "A";
      for (j = 12; j < 17; j++) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }

      // SAMPLE B
      cellPrefix = "B";
      for (j = 11; j < 16; j++) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }

      // SAMPLE D
      cellPrefix = "D";
      for (j = 5; j < 7; j++) {
        sprintf(buff, "%i", j);
        cellName = cellPrefix + std::string(buff);
        sectionCells.push_back(m_cellMap->operator[](cellName));
      }
      if (m_verboseLevel>5)
        G4cout << " TILE_EBARREL: samples A, B, D are filled" << G4endl;
      break;
    }
    case TileDddbManager::TILE_PLUG1: {
      // Collect all cells for this section
      // Sample D
      cellName = "D4";
      sectionCells.push_back(m_cellMap->operator[](cellName));

      // filling PMT array for D4 Cells
      MakeTileGeoG4npmtD4();
      if (m_verboseLevel>5)
        G4cout << " TILE_PLUG1: cell D4 is filled" << G4endl;
      break;
    }
    case TileDddbManager::TILE_PLUG2: {
      // Collect all cells for this section
      // Sample C
      cellName = "C10";
      sectionCells.push_back(m_cellMap->operator[](cellName));

      // filling PMT array for C10 Cells
      MakeTileGeoG4npmtC10();
      if (m_verboseLevel>5)
        G4cout << " TILE_PLUG2: cell C10 is filled" << G4endl;
      break;
    }
    case TileDddbManager::TILE_PLUG3: {
      // Collect all cells for this section
      // Sample E
      cellName = "E2";
      sectionCells.push_back(m_cellMap->operator[](cellName));
      cellName = "E1";
      sectionCells.push_back(m_cellMap->operator[](cellName));
      if (m_verboseLevel>5)
        G4cout << " TILE_PLUG3: cells E2, E1 are filled" << G4endl;
      break;
    }
    case TileDddbManager::TILE_PLUG4: {
      // Collect all cells for this section
      // Sample E
      cellName = "E4";
      sectionCells.push_back(m_cellMap->operator[](cellName));
      cellName = "E3";
      sectionCells.push_back(m_cellMap->operator[](cellName));
      if (m_verboseLevel>5)
        G4cout << " TILE_PLUG4: cells E4, E3 are filled" << G4endl;

      // filling PMT array for E4' Cells
      MakeTileGeoG4npmtE5();
      if (m_isE5) {
        cellName = "E5";  //E4' cells
        sectionCells.push_back(m_cellMap->operator[](cellName));
        if (m_verboseLevel>5)
          G4cout << " TILE_PLUG4: special cells E5(E4') are filled" << G4endl;
      }

      break;
    }
    default: { // Don't do anything
      if (m_verboseLevel > 5)
        G4cout << " counter=" << counter << "  key=" << key << "  Default: Don't do anything" << G4endl;
      continue;
    }
    }

    if (m_isE5 && key == 6)
      m_dbManager->SetCurrentSection(key + 10);
    else
      m_dbManager->SetCurrentSection(key);
    section = new TileGeoG4Section(m_verboseLevel);

    section->nrOfModules = nModules;
    section->nrOfPeriods = m_dbManager->TILBnperiod();
    section->nrOfScintillators = m_dbManager->TILBnscin();

    if (m_verboseLevel > 5)
      G4cout << " counter=" << counter
             << "  key=" << key
             << "  nrOfModules=" << section->nrOfModules
             << "  nrOfPeriods=" << section->nrOfPeriods
             << "  nrOfScintillators=" << section->nrOfScintillators << G4endl;

    // iterate through all section cells and group them into samples
    cell = sectionCells[0];
    sampleNum = cell->sample;
    sample = new TileGeoG4Sample();
    sample->firstRow = cell->firstRow;
    sample->lastRow = cell->lastRow;
    sample->cells.push_back(cell);

    rowShift = cell->firstRow - 1;// ITC1 starts with row=4, Gap starts with row=3

    for (j = 1; j < static_cast<int>(sectionCells.size()); j++) {
      cell = sectionCells[j];

      if (sampleNum != cell->sample) {
        //Start new sample
        section->samples.push_back(sample);
        sampleNum = cell->sample;
        //Protection against messages from Coverity tool
        sample = NULL;
        sample = new TileGeoG4Sample();
        sample->firstRow = cell->firstRow;
        sample->lastRow = cell->lastRow;
      }

      sample->cells.push_back(cell);
    }
    section->samples.push_back(sample);

    if ((key == TileDddbManager::TILE_PLUG3) || (key == TileDddbManager::TILE_PLUG4)) {
      section->ScinToCell(true, rowShift);
      section->PrintScinToCell(((key == TileDddbManager::TILE_PLUG3) ? "PLUG3" : "PLUG4"));  //debugging of ScinToCell
    } else {
      section->ScinToCell(false, rowShift);
    }

    // Initialize ModuleToHit values
    for (j = 0; j < section->nrOfModules; j++) {
      section->AddModuleToCell(0);
      if (key != TileDddbManager::TILE_BARREL) {
        section->AddModuleToCell(1);
      }
    }

    m_sectionMap->operator[](key) = section;
    sectionCells.clear();

  }
  delete[] buff;
}

// Filling number of PMT in array for C10 Cells
void TileGeoG4LookupBuilder::MakeTileGeoG4npmtC10() {
  // we put number +1 for special C10 cells in EBC (negative)
  // and -1 for special C10 cells in EBA (poisitive)
  int nPMT = 1;

  // side == 0 is negative and side ==1 is positive
  for (int side = 0; side < 2; ++side) {

    int moduleNumber = 0;
    for (; moduleNumber < 38; ++moduleNumber)
      m_npmtC10[side][moduleNumber] = 2;
    for (; moduleNumber < 42; ++moduleNumber)
      m_npmtC10[side][moduleNumber] = nPMT;
    for (; moduleNumber < 54; ++moduleNumber)
      m_npmtC10[side][moduleNumber] = 2;
    for (; moduleNumber < 58; ++moduleNumber)
      m_npmtC10[side][moduleNumber] = nPMT;
    for (; moduleNumber < 64; ++moduleNumber)
      m_npmtC10[side][moduleNumber] = 2;

    nPMT *= -1;
  }
}

// Filling number of PMT array for D4 Cells
void TileGeoG4LookupBuilder::MakeTileGeoG4npmtD4() {
  // side == 0 is negative and side ==1 is positive
  for (int side = 0; side < 2; ++side) {
    for (int moduleNumber = 0; moduleNumber < 64; ++moduleNumber) {
      m_npmtD4[side][moduleNumber] = 2;
    }
  }
  m_npmtD4[1][14] = 0; // EBA15
  m_npmtD4[0][17] = 0; // EBC18
}

// Filling number of PMT array for E5(E4') Cells
void TileGeoG4LookupBuilder::MakeTileGeoG4npmtE5() {
  // side == 0 is negative and side ==1 is positive
  for (int side = 0; side < 2; ++side) {
    for (int moduleNumber = 0; moduleNumber < 64; ++moduleNumber) {
      m_npmtE5[side][moduleNumber] = 0;
    }
  }
  if (m_isE5) {
    m_npmtE5[0][31] = 2; // EBC32
    m_npmtE5[0][32] = 2; // EBC33
  }
}

