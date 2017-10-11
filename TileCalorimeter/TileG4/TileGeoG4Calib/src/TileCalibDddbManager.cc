/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
//
// Manager of Tile Calibration DB
//
// author: Gia Khoriauli <gia@mail.cern.ch>
//
// May, 2005
//
///////////////////////////////////////////////////////////////

#include "TileCalibDddbManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "G4ios.hh"

#include <sstream>

TileCalibDddbManager::TileCalibDddbManager(ServiceHandle<IRDBAccessSvc> &access, std::string version_tag,
                                           std::string version_node, const int verboseLevel)
  : m_currentTile(0),
    m_currentSection(0),
    m_currentCell(0),
    m_currentPlateCell(0),
    m_currentGirderCell(0),
    m_currentCellInd( -1),
    m_currentPlateCellInd( -1),
    m_currentGirderCellInd( -1),
    m_tag(version_tag),
  m_node(version_node),
  m_verboseLevel(verboseLevel)
{

  access->connect();

  m_tile = access->getRecordset("TILE", m_tag, m_node);
  m_nTile = m_tile->size();

  m_tileSection = access->getRecordset("TileCalibSections", m_tag, m_node);
  m_nTileSect = m_tileSection->size();

  m_tileCell = access->getRecordset("TileCalibCells", m_tag, m_node);
  m_nTileCell = m_tileCell->size();

  m_tilePlateCell = access->getRecordset("TileCalibPlateCells", m_tag, m_node);
  m_nTilePCell = m_tilePlateCell->size();

  m_tileGirderCell = access->getRecordset("TileCalibGirderCells", m_tag, m_node);
  m_nTileGCell = m_tileGirderCell->size();

  m_switches = access->getRecordset("TileSwitches", m_tag, m_node);

  access->disconnect();

}

// --------------------------- T I L E -------------------------

int TileCalibDddbManager::GetNumModules(int ind) const {
  if ( (*m_tile)[ind])
    return (*m_tile)[ind]->getInt("NMODUL");
  else {
    G4cout << "GetNumModules() - Unable to retrieve number of modules. return -999" << G4endl;
    return -999;
  }
}

bool TileCalibDddbManager::GetSwitchPlateToCell() const {
  if ( (*m_switches).size() != 0) {
    if (! (*m_switches)[0]->isFieldNull("ADDPLATESTOCELL")) {
      return (*m_switches)[0]->getInt("ADDPLATESTOCELL");
    } else {
      return true;
    }
  } else {
    G4cout << "GetSwitchPlateToCell() - Unable to retrieve swithes. return TRUE" << G4endl;
    return true;
  }
}

// ----------------------- S E C T I O N -------------------------

void TileCalibDddbManager::SetCurrentSection(unsigned int section) {
  unsigned int ind = 0;

  while ( ( (*m_tileSection)[ind]->getInt("SECT") != static_cast<int>(section)) && (++ind < m_nTileSect)) {}

  if (ind >= m_nTileSect)
    G4cout << "SetCurrentSection() - Unable to find the section " << section << G4endl;
  else
    m_currentSection = (*m_tileSection)[ind];
}

int TileCalibDddbManager::GetCurrentSection() const {
  if (m_currentSection)
    return m_currentSection->getInt("SECT");
  else {
    G4cout << "GetCurrentSection() - Current Section not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectPeriods() const {
  if (m_currentSection)
    return m_currentSection->getInt("PERIODS");
  else {
    G4cout << "GetNumSectPeriods() - Current Section not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectSamples() const {
  if (m_currentSection)
    return m_currentSection->getInt("SAMPLES");
  else {
    G4cout << "GetNumSectSamples() - Current Section not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectCells() const {
  if (m_currentSection)
    return m_currentSection->getInt("CELLS");
  else {
    G4cout << "GetNumSectCells() - Current Section not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectPlateCells() const {
  if (m_currentSection) {
    if (m_currentSection->isFieldNull("PLATECELLS")) {
      if (m_verboseLevel > 5)
        G4cout << "GetNumSectPlateCells() - PLATECELLS in section " << m_currentSection->getInt("SECT")
               << " is NULL, returning -999" << G4endl;
      return -999;
    } else {
      return m_currentSection->getInt("PLATECELLS");
    }
  } else {
    G4cout << "GetNumSectPlateCells() - Current Section not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectGirderCells() const {
  if (m_currentSection) {
    if (m_currentSection->isFieldNull("GIRDERCELLS")) {
      if (m_verboseLevel > 5)
        G4cout << "GetNumSectGirderCells() - GIRDERCELLS in section " << m_currentSection->getInt("SECT")
               << " is NULL, returning -999" << G4endl;
      return -999;
    } else {
      return m_currentSection->getInt("GIRDERCELLS");
    }
  } else {
    G4cout << "GetNumSectGirderCells() - Current Section not set, returning -999" << G4endl;
    return -999;
  }
}

double TileCalibDddbManager::GetSampleZBound(int ind) const {
  std::stringstream ss;
  std::string indx;

  ss << ind;
  ss >> indx;

  if (m_currentSection) {
    if (m_currentSection->isFieldNull("SAMPZBOUND_" + indx)) {
      return 9999.9;
    } else {
      return m_currentSection->getDouble("SAMPZBOUND", ind);
    }
  } else {
    G4cout << "GetSampleZBound() - Current Section not set, returning -9999.9" << G4endl;
    return -9999.9;
  }
}

// ------------------ G I R D E R   C E L L------------------

//Set methods

void TileCalibDddbManager::SetCurrentGirderCell(unsigned int index) {
  if (index >= m_nTileGCell)
    G4cout << "SetCurrentGirderCell() - The requested index = " << index << " out of range" << G4endl;
  else {
    m_currentGirderCell = (*m_tileGirderCell)[index];
    m_currentGirderCellInd = index;
  }
}

int TileCalibDddbManager::SetFirstDetGirderCell(int detector) {
  unsigned int ind = 0;

  // Looking for the first cell of given detector
  while ( ( (*m_tileGirderCell)[ind]->getInt("DETECTOR") != detector) && (++ind < m_nTileGCell)) {}

  if (ind >= m_nTileGCell)
    return 0;  // FAILURE
  else {
    m_currentGirderCell = (*m_tileGirderCell)[ind];
    m_currentGirderCellInd = ind;
    return 1;  // SUCCESS
  }
}

int TileCalibDddbManager::SetNextDetGirderCell() {
  if (m_currentGirderCellInd < 0)
    return 0; // FAILURE

  int currentDetector = (*m_tileGirderCell)[m_currentGirderCellInd]->getInt("DETECTOR");

  if ( (++m_currentGirderCellInd == static_cast<int>(m_nTileGCell))
       || ( (*m_tileGirderCell)[m_currentGirderCellInd]->getInt("DETECTOR") != currentDetector)) {
    m_currentGirderCellInd--;
    return 0;  // FAILURE
  }

  m_currentGirderCell = (*m_tileGirderCell)[m_currentGirderCellInd];
  return 1;
}

//Get methods

int TileCalibDddbManager::GetGirderCellDetector() const {
  if (m_currentGirderCell)
    return m_currentGirderCell->getInt("DETECTOR");
  else {
    G4cout << "GetGirderCellDetector() - Current GirderCell not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetGirderCellTower() const {
  if (m_currentGirderCell)
    return m_currentGirderCell->getInt("TOWER");
  else {
    G4cout << "GetGirderCellTower() - Current GirderCell not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetGirderCellSample() const {
  if (m_currentGirderCell)
    return m_currentGirderCell->getInt("SAMPLE");
  else {
    G4cout << "GetGirderCellSample() - Current GirderCell not set, returning -999" << G4endl;
    return -999;
  }
}

double TileCalibDddbManager::GetGirderCellEta() const {
  if (m_currentGirderCell)
    return m_currentGirderCell->getDouble("ETA");
  else {
    G4cout << "GetGirderCellEta() - Current GirderCell not set, returning -999.9" << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetGirderCellDeta() const {
  if (m_currentGirderCell)
    return m_currentGirderCell->getDouble("DETA");
  else {
    G4cout << "GetGirderCellDeta() - Current GirderCell not set, returning -999.9" << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetGirderCellXBound() const {
  if (m_currentGirderCell)
    return m_currentGirderCell->getDouble("XBOUND");
  else {
    G4cout << "GetGirderCellXBound() - Current GirderCell not set, returning -9999.9" << G4endl;
    return -9999.9;
  }
}

// -----------------  P L A T E   C E L L  ----------------

//Set methods

void TileCalibDddbManager::SetCurrentPlateCell(unsigned int index) {
  if (index >= m_nTilePCell)
    G4cout << "SetCurrentPlateCell() - The requested index = " << index << " out of range" << G4endl;
  else {
    m_currentPlateCell = (*m_tilePlateCell)[index];
    m_currentPlateCellInd = index;
  }
}

int TileCalibDddbManager::SetFirstDetPlateCell(int detector) {
  unsigned int ind = 0;

  // Looking for the first cell of given detector
  while ( ( (*m_tilePlateCell)[ind]->getInt("DETECTOR") != detector) && (++ind < m_nTilePCell)) {}

  if (ind >= m_nTilePCell)
    return 0;  // FAILURE

  m_currentPlateCell = (*m_tilePlateCell)[ind];
  m_currentPlateCellInd = ind;
  return 1;  // SUCCESS
}

int TileCalibDddbManager::SetFirstDetPlateCellInSample(int detector, int sample) {
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while ( ( (*m_tilePlateCell)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < m_nTilePCell)) {}

  if (detector_ind >= m_nTilePCell)
    return 0;  // FAILURE
  else {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while ( ( (*m_tilePlateCell)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < m_nTilePCell)) {}

    if (sample_ind >= m_nTilePCell)
      return 0;  // FAILURE

    m_currentCell = (*m_tilePlateCell)[sample_ind];
    m_currentPlateCellInd = sample_ind;
    return 1;  // SUCCESS
  }
}

int TileCalibDddbManager::SetNextDetPlateCell() {
  if (m_currentPlateCellInd < 0)
    return 0; // FAILURE

  int currentDetector = (*m_tilePlateCell)[m_currentPlateCellInd]->getInt("DETECTOR");

  if ( (++m_currentPlateCellInd == static_cast<int>(m_nTilePCell))
       || ( (*m_tilePlateCell)[m_currentPlateCellInd]->getInt("DETECTOR") != currentDetector)) {
    m_currentPlateCellInd--;
    return 0;  // FAILURE
  }

  m_currentPlateCell = (*m_tilePlateCell)[m_currentPlateCellInd];
  return 1;
}

int TileCalibDddbManager::SetNextDetPlateCellInSample() {
  if (m_currentPlateCellInd < 0)
    return 0; // FAILURE

  int currentDetector = (*m_tilePlateCell)[m_currentPlateCellInd]->getInt("DETECTOR");
  int currentSample = (*m_tilePlateCell)[m_currentPlateCellInd]->getInt("SAMPLE");

  if ( (++m_currentPlateCellInd == static_cast<int>(m_nTilePCell))
       || ( (*m_tilePlateCell)[m_currentPlateCellInd]->getInt("DETECTOR") != currentDetector)
       || ( (*m_tilePlateCell)[m_currentPlateCellInd]->getInt("SAMPLE") != currentSample)) {
    m_currentPlateCellInd--;
    return 0;  // FAILURE
  }

  m_currentPlateCell = (*m_tilePlateCell)[m_currentPlateCellInd];
  return 1;

}

//Get methods

int TileCalibDddbManager::GetPlateCellDetector() const {
  if (m_currentPlateCell)
    return m_currentPlateCell->getInt("DETECTOR");
  else {
    G4cout << "GetPlateCellDetector() - Current PlateCell not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetPlateCellTower() const {
  if (m_currentPlateCell)
    return m_currentPlateCell->getInt("TOWER");
  else {
    G4cout << "GetPlateCellTower() - Current PlateCell not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetPlateCellSample() const {
  if (m_currentPlateCell)
    return m_currentPlateCell->getInt("SAMPLE");
  else {
    G4cout << "GetPlateCellSample() - Current PlateCell not set, returning -999" << G4endl;
    return -999;
  }
}

double TileCalibDddbManager::GetPlateCellEta() const {
  if (m_currentPlateCell)
    return m_currentPlateCell->getDouble("ETA");
  else {
    G4cout << "GetPlateCellEta() - Current PlateCell not set, returning -999.9" << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetPlateCellDeta() const {
  if (m_currentPlateCell)
    return m_currentPlateCell->getDouble("DETA");
  else {
    G4cout << "GetPlateCellDeta() - Current PlateCell not set, returning -999.9" << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetPlateCellXBound() const {
  if (m_currentPlateCell)
    return m_currentPlateCell->getDouble("XBOUND");
  else {
    G4cout << "GetPlateCellXBound() - Current PlateCell not set, returning -9999.9" << G4endl;
    return -9999.9;
  }
}

double TileCalibDddbManager::GetPlateCellZBound() const {
  if (m_currentPlateCell) {
    if (m_currentPlateCell->isFieldNull("ZBOUND")) {
      if (m_verboseLevel > 5)
        G4cout << "GetPlateCellZBound() - ZBOUND" << " in DETECTOR " << m_currentPlateCell->getInt("DETECTOR")
               << " SAMPLE " << m_currentPlateCell->getInt("SAMPLE") << " TOWER " << m_currentPlateCell->getInt("TOWER")
               << " is NULL, returning -9999.9" << G4endl;
      return -9999.9;
    } else {
      return m_currentPlateCell->getDouble("ZBOUND");
    }
  } else {
    G4cout << "GetPlateCellZBound() - Current PlateCell not set, returning -9999.9"
           << G4endl;
    return -9999.9;
  }
}

int TileCalibDddbManager::GetPlateCellNeighborPeriod() const {
  if (m_currentPlateCell) {
    if (m_currentPlateCell->isFieldNull("NEIGHPERIOD")) {
      if (m_verboseLevel > 5)
        G4cout << "GetPlateCellNeighborPeriod() - NEIGHPERIOD " << " in DETECTOR "
               << m_currentPlateCell->getInt("DETECTOR") << " SAMPLE " << m_currentPlateCell->getInt("SAMPLE")
               << " TOWER " << m_currentPlateCell->getInt("TOWER") << " is NULL, returning -999" << G4endl;
      return -999;
    } else {
      return m_currentPlateCell->getInt("NEIGHPERIOD");
    }
  } else {
    G4cout << "GetPlateCellNeighborPeriod() - Current PlateCell not set, returning -999"
           << G4endl;
    return -999;
  }
}

// -------------------- C E L L  ---------------------

// Set methods

void TileCalibDddbManager::SetCurrentCell(unsigned int index) {
  if (index >= m_nTileCell)
    G4cout << "SetCurrentCell() - The requested index = " << index << " out of range" << G4endl;
  else {
    m_currentCell = (*m_tileCell)[index];
    m_currentCellInd = index;
  }
}

int TileCalibDddbManager::SetFirstDetCell(int detector) {
  unsigned int ind = 0;

  // Looking for the first cell of given detector
  while ( ( (*m_tileCell)[ind]->getInt("DETECTOR") != detector) && (++ind < m_nTileCell)) {}

  if (ind >= m_nTileCell)
    return 0;  // FAILURE

  m_currentCell = (*m_tileCell)[ind];
  m_currentCellInd = ind;
  return 1;  // SUCCESS

}

int TileCalibDddbManager::SetFirstDetCellInSample(int detector, int sample) {
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while ( ( (*m_tileCell)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < m_nTileCell)) {}

  if (detector_ind >= m_nTileCell)
    return 0;  // FAILURE
  else {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while ( ( (*m_tileCell)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < m_nTileCell)) {}

    if (sample_ind >= m_nTileCell)
      return 0;  // FAILURE

    m_currentCell = (*m_tileCell)[sample_ind];
    m_currentCellInd = sample_ind;
    return 1;  // SUCCESS
  }
}

int TileCalibDddbManager::SetNextDetCell() {
  if (m_currentCellInd < 0)
    return 0; // FAILURE

  int currentDetector = (*m_tileCell)[m_currentCellInd]->getInt("DETECTOR");

  if ( (++m_currentCellInd == static_cast<int>(m_nTileCell)) || ( (*m_tileCell)[m_currentCellInd]->getInt("DETECTOR")
                                                                  != currentDetector)) {
    m_currentCellInd--;
    return 0;  // FAILURE
  }

  m_currentCell = (*m_tileCell)[m_currentCellInd];
  return 1;
}

int TileCalibDddbManager::SetNextDetCellInSample() {
  if (m_currentCellInd < 0)
    return 0; // FAILURE

  int currentDetector = (*m_tileCell)[m_currentCellInd]->getInt("DETECTOR");
  int currentSample = (*m_tileCell)[m_currentCellInd]->getInt("SAMPLE");

  if ( (++m_currentCellInd == static_cast<int>(m_nTileCell)) || ( (*m_tileCell)[m_currentCellInd]->getInt("DETECTOR")
                                                                  != currentDetector)
       || ( (*m_tileCell)[m_currentCellInd]->getInt("SAMPLE") != currentSample)) {
    m_currentCellInd--;
    return 0;  // FAILURE
  }

  m_currentCell = (*m_tileCell)[m_currentCellInd];
  return 1;
}

// Get methods

int TileCalibDddbManager::GetNumOfPeriodsInCell(int period_set) const {
  std::stringstream ss;
  std::string indx;

  ss << period_set;
  ss >> indx;

  if (m_currentCell) {
    if (m_currentCell->isFieldNull("PERIODS_" + indx)) {
      if (m_verboseLevel > 5)
        G4cout << "GetNumOfPeriodsInCell() - PERIODS_" << indx << " in DETECTOR " << m_currentCell->getInt("DETECTOR")
               << " SAMPLE " << m_currentCell->getInt("SAMPLE") << " is NULL, returning -999" << G4endl;
      return -999;
    } else {
      return m_currentCell->getInt("PERIODS", period_set);
    }
  } else {
    G4cout << "GetNumOfPeriodsInCell() - Current Cell not set, returning -999"
           << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetCellSample() const {
  if (m_currentCell)
    return m_currentCell->getInt("SAMPLE");
  else {
    G4cout << "GetCellSample() - Current Cell not set, returning -999" << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetCellDetector() const {
  if (m_currentCell)
    return m_currentCell->getInt("DETECTOR");
  else {
    G4cout << "GetCellDetector() - Current Cell not set, returning -999" << G4endl;
    return -999;
  }
}

//Get IRDBRecordsets sizes

int TileCalibDddbManager::GetTileIRDBRS_size() const {
  return m_nTile;
}

int TileCalibDddbManager::GetTileCalibSectionsIRDBRS_size() const {
  return m_nTileSect;
}

int TileCalibDddbManager::GetTileCalibCellsIRDBRS_size() const {
  return m_nTileCell;
}

int TileCalibDddbManager::GetTileCalibPlateCellsIRDBRS_size() const {
  return m_nTilePCell;
}

int TileCalibDddbManager::GetTileCalibGirderCellsIRDBRS_size() const {
  return m_nTileGCell;
}

