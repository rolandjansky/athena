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


TileCalibDddbManager::TileCalibDddbManager(ServiceHandle<IRDBAccessSvc> &access,
                                           std::string    version_tag,
                                           std::string    version_node,
                                           const int verboseLevel) :
  currentTile(0),
  currentSection(0),
  currentCell(0),
  currentPlateCell(0),
  currentGirderCell(0),
  currentCellInd(-1),
  currentPlateCellInd(-1),
  currentGirderCellInd(-1),
  mTag(version_tag),
  mNode(version_node),
  m_verboseLevel(verboseLevel)
{

  access->connect();

  tile = access->getRecordset("TILE",mTag,mNode);
  n_tile = tile->size();

  tile_section = access->getRecordset("TileCalibSections",mTag,mNode);
  n_tile_sect = tile_section->size();

  tile_cell = access->getRecordset("TileCalibCells",mTag,mNode);
  n_tile_cell = tile_cell->size();

  tile_plateCell = access->getRecordset("TileCalibPlateCells",mTag,mNode);
  n_tile_pcell = tile_plateCell->size();

  tile_girderCell = access->getRecordset("TileCalibGirderCells",mTag,mNode);
  n_tile_gcell = tile_girderCell->size();

  switches = access->getRecordset("TileSwitches",mTag,mNode);
  
  access->disconnect();

}



// --------------------------- T I L E -------------------------

int TileCalibDddbManager::GetNumModules(int ind)  const
{
  if((*tile)[ind])
    return (*tile)[ind]->getInt("NMODUL");
  else
  {
    G4cout << "GetNumModules() - Unable to retrieve number of modules. return -999"
             << G4endl;
    return -999;
  }
}

bool TileCalibDddbManager::GetSwitchPlateToCell()  const
{
  if ((*switches).size()!=0)
  {
    if (!(*switches)[0]->isFieldNull("ADDPLATESTOCELL"))
    {
      return (*switches)[0]->getInt("ADDPLATESTOCELL");
    }
    else
    {
      return true;
    }
  }
  else
  {
    G4cout << "GetSwitchPlateToCell() - Unable to retrieve swithes. return TRUE"
             << G4endl;
    return true;
  }
}


// ----------------------- S E C T I O N -------------------------

void TileCalibDddbManager::SetCurrentSection(unsigned int section)
{
  unsigned int ind = 0;

  while (((*tile_section)[ind]->getInt("SECT") != static_cast<int>(section)) && (++ind<n_tile_sect)) 
  {
  }

  if(ind >= n_tile_sect )
    G4cout   << "SetCurrentSection() - Unable to find the section " << section
             << G4endl;
  else
    currentSection = (*tile_section)[ind];
}

int TileCalibDddbManager::GetCurrentSection() const
{
  if(currentSection)
    return currentSection->getInt("SECT");
  else
  {
    G4cout   << "GetCurrentSection() - Current Section not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectPeriods() const
{
  if(currentSection)
    return currentSection->getInt("PERIODS");
  else
  {
    G4cout   << "GetNumSectPeriods() - Current Section not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectSamples() const
{
  if(currentSection)
    return currentSection->getInt("SAMPLES");
  else
  {
    G4cout   << "GetNumSectSamples() - Current Section not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectCells() const
{
  if(currentSection)
    return currentSection->getInt("CELLS");
  else
  {
    G4cout   << "GetNumSectCells() - Current Section not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectPlateCells() const
{
  if(currentSection) {
    if(currentSection->isFieldNull("PLATECELLS")) {
      if (m_verboseLevel>5) G4cout
               << "GetNumSectPlateCells() - PLATECELLS in section " << currentSection->getInt("SECT")
               << " is NULL, returning -999"
               << G4endl;
      return -999;
    } else {
      return currentSection->getInt("PLATECELLS");
    }
  }
  else
  {
    G4cout   <<"GetNumSectPlateCells() - Current Section not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectGirderCells() const
{
  if(currentSection){
    if(currentSection->isFieldNull("GIRDERCELLS")) {
      if (m_verboseLevel>5) G4cout
               << "GetNumSectGirderCells() - GIRDERCELLS in section " << currentSection->getInt("SECT")
               << " is NULL, returning -999"
               << G4endl;
      return -999;
    } else {
      return currentSection->getInt("GIRDERCELLS");
    }
  }
  else
  {
    G4cout   << "GetNumSectGirderCells() - Current Section not set, returning -999"
             << G4endl;
    return -999;
  }
}

double TileCalibDddbManager::GetSampleZBound(int ind) const 
{
  std::stringstream ss;
  std::string indx;

  ss<<ind;
  ss>>indx;

  if(currentSection) {
    if(currentSection->isFieldNull("SAMPZBOUND_"+indx)) {
      return 9999.9;
    } else {
      return currentSection->getDouble("SAMPZBOUND", ind);
    }
  }
  else
  {
    G4cout   <<"GetSampleZBound() - Current Section not set, returning -9999.9"
             << G4endl;
    return -9999.9;
  }
}



// ------------------ G I R D E R   C E L L------------------

//Set methods

void TileCalibDddbManager::SetCurrentGirderCell(unsigned int index)
{
  if(index >=n_tile_gcell )
    G4cout   << "SetCurrentGirderCell() - The requested index = " << index << " out of range"
             << G4endl;
  else
  {
    currentGirderCell = (*tile_girderCell)[index];
    currentGirderCellInd = index;
  }
}

int TileCalibDddbManager::SetFirstDetGirderCell(int detector)
{
   unsigned int ind = 0;

  // Looking for the first cell of given detector
  while (((*tile_girderCell)[ind]->getInt("DETECTOR") != detector) && (++ind < n_tile_gcell))
  {
  }

  if(ind >= n_tile_gcell)
    return 0;  // FAILURE
  else
  {
    currentGirderCell = (*tile_girderCell)[ind];
    currentGirderCellInd = ind;
    return 1;  // SUCCESS 
  }
}

int TileCalibDddbManager::SetNextDetGirderCell()
{
  if(currentGirderCellInd<0) return 0; // FAILURE

  int currentDetector = (*tile_girderCell)[currentGirderCellInd]->getInt("DETECTOR");

  if((++currentGirderCellInd == static_cast<int>(n_tile_gcell))||((*tile_girderCell)[currentGirderCellInd]->getInt("DETECTOR") != currentDetector))
  {
    currentGirderCellInd--;
    return 0;  // FAILURE
  }

  currentGirderCell = (*tile_girderCell)[currentGirderCellInd];
  return 1;
}


//Get methods

int TileCalibDddbManager::GetGirderCellDetector() const
{
  if(currentGirderCell)
    return currentGirderCell->getInt("DETECTOR");
  else
  {
    G4cout   << "GetGirderCellDetector() - Current GirderCell not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetGirderCellTower() const
{
  if(currentGirderCell)
    return currentGirderCell->getInt("TOWER");
  else
  {
    G4cout   << "GetGirderCellTower() - Current GirderCell not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetGirderCellSample() const
{
  if(currentGirderCell)
    return currentGirderCell->getInt("SAMPLE");
  else
  {
    G4cout   << "GetGirderCellSample() - Current GirderCell not set, returning -999"
             << G4endl;
    return -999;
  }
}

double TileCalibDddbManager::GetGirderCellEta() const
{
  if(currentGirderCell)
    return currentGirderCell->getDouble("ETA");
  else
  {
    G4cout   << "GetGirderCellEta() - Current GirderCell not set, returning -999.9"
             << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetGirderCellDeta() const
{
  if(currentGirderCell)
    return currentGirderCell->getDouble("DETA");
  else
  {
    G4cout   << "GetGirderCellDeta() - Current GirderCell not set, returning -999.9"
             << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetGirderCellXBound() const
{
  if(currentGirderCell)
    return currentGirderCell->getDouble("XBOUND");
  else
  {
    G4cout   << "GetGirderCellXBound() - Current GirderCell not set, returning -9999.9"
             << G4endl;
    return -9999.9;
  }
}





// -----------------  P L A T E   C E L L  ----------------

//Set methods

void TileCalibDddbManager::SetCurrentPlateCell(unsigned int index)
{
  if(index >=n_tile_pcell )
    G4cout   << "SetCurrentPlateCell() - The requested index = " << index << " out of range"
             << G4endl;
  else
  {
    currentPlateCell = (*tile_plateCell)[index];
    currentPlateCellInd = index;
  }
}

int TileCalibDddbManager::SetFirstDetPlateCell(int detector)
{
   unsigned int ind = 0;

  // Looking for the first cell of given detector
  while (((*tile_plateCell)[ind]->getInt("DETECTOR") != detector) && (++ind < n_tile_pcell))
  {
  }

  if(ind >= n_tile_pcell)
    return 0;  // FAILURE

  currentPlateCell = (*tile_plateCell)[ind];
  currentPlateCellInd = ind;
  return 1;  // SUCCESS 
}

int  TileCalibDddbManager::SetFirstDetPlateCellInSample(int detector, int sample)
{
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while (((*tile_plateCell)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < n_tile_pcell)) 
  {
  }

  if(detector_ind >= n_tile_pcell)
    return 0;  // FAILURE
  else
  {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while (((*tile_plateCell)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < n_tile_pcell)) 
    {
    }

    if(sample_ind >= n_tile_pcell)
      return 0;  // FAILURE

    currentCell = (*tile_plateCell)[sample_ind];
    currentPlateCellInd = sample_ind;
    return 1;  // SUCCESS
  }
}

int  TileCalibDddbManager::SetNextDetPlateCell()
{
  if(currentPlateCellInd<0) return 0; // FAILURE

  int currentDetector = (*tile_plateCell)[currentPlateCellInd]->getInt("DETECTOR");

  if((++currentPlateCellInd == static_cast<int>(n_tile_pcell))||((*tile_plateCell)[currentPlateCellInd]->getInt("DETECTOR") != currentDetector))
  {
    currentPlateCellInd--;
    return 0;  // FAILURE
  }

  currentPlateCell = (*tile_plateCell)[currentPlateCellInd];
  return 1;
}

int TileCalibDddbManager::SetNextDetPlateCellInSample()
{
  if(currentPlateCellInd<0) return 0; // FAILURE

  int currentDetector = (*tile_plateCell)[currentPlateCellInd]->getInt("DETECTOR");
  int currentSample = (*tile_plateCell)[currentPlateCellInd]->getInt("SAMPLE");

  if((++currentPlateCellInd == static_cast<int>(n_tile_pcell))||
     ((*tile_plateCell)[currentPlateCellInd]->getInt("DETECTOR") != currentDetector)||
     ((*tile_plateCell)[currentPlateCellInd]->getInt("SAMPLE") != currentSample))
  {
    currentPlateCellInd--;
    return 0;  // FAILURE
  }

  currentPlateCell = (*tile_plateCell)[currentPlateCellInd];
  return 1;

}


//Get methods

int TileCalibDddbManager::GetPlateCellDetector() const
{
  if(currentPlateCell)
    return currentPlateCell->getInt("DETECTOR");
  else
  {
    G4cout   << "GetPlateCellDetector() - Current PlateCell not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetPlateCellTower() const
{
  if(currentPlateCell)
    return currentPlateCell->getInt("TOWER");
  else
  {
    G4cout   << "GetPlateCellTower() - Current PlateCell not set, returning -999"
             << G4endl;
    return -999;
  }
}

int TileCalibDddbManager::GetPlateCellSample() const
{
  if(currentPlateCell)
    return currentPlateCell->getInt("SAMPLE");
  else
  {
    G4cout   << "GetPlateCellSample() - Current PlateCell not set, returning -999"
             << G4endl;
    return -999;
  }
}

double TileCalibDddbManager::GetPlateCellEta() const
{
  if(currentPlateCell)
    return currentPlateCell->getDouble("ETA");
  else
  {
    G4cout   << "GetPlateCellEta() - Current PlateCell not set, returning -999.9"
             << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetPlateCellDeta() const
{
  if(currentPlateCell)
    return currentPlateCell->getDouble("DETA");
  else
  {
    G4cout   << "GetPlateCellDeta() - Current PlateCell not set, returning -999.9"
             << G4endl;
    return -999.9;
  }
}

double TileCalibDddbManager::GetPlateCellXBound() const
{
  if(currentPlateCell)
    return currentPlateCell->getDouble("XBOUND");
  else
  {
    G4cout   << "GetPlateCellXBound() - Current PlateCell not set, returning -9999.9"
             << G4endl;
    return -9999.9;
  }
}

double TileCalibDddbManager::GetPlateCellZBound() const
{
  if(currentPlateCell) {
    if(currentPlateCell->isFieldNull("ZBOUND")) {
      if (m_verboseLevel>5) G4cout
               << "GetPlateCellZBound() - ZBOUND"
               << " in DETECTOR "<<currentPlateCell->getInt("DETECTOR")
               << " SAMPLE "<<currentPlateCell->getInt("SAMPLE")
               << " TOWER "<<currentPlateCell->getInt("TOWER")
               << " is NULL, returning -9999.9"
               << G4endl;
      return -9999.9;
    } else {
      return currentPlateCell->getDouble("ZBOUND");
    }
  }
  else
  {
    G4cout   << "GetPlateCellZBound() - Current PlateCell not set, returning -9999.9"
             << G4endl;
    return -9999.9;
  }
}

int TileCalibDddbManager::GetPlateCellNeighborPeriod() const
{
  if(currentPlateCell) {
    if(currentPlateCell->isFieldNull("NEIGHPERIOD")) {
      if (m_verboseLevel>5) G4cout
               << "GetPlateCellNeighborPeriod() - NEIGHPERIOD "
               << " in DETECTOR "<<currentPlateCell->getInt("DETECTOR")
               << " SAMPLE "<<currentPlateCell->getInt("SAMPLE")
               << " TOWER "<<currentPlateCell->getInt("TOWER")
               << " is NULL, returning -999"
               << G4endl;
      return -999;
    } else {
      return currentPlateCell->getInt("NEIGHPERIOD");
    }
  }
  else
  {
    G4cout   << "GetPlateCellNeighborPeriod() - Current PlateCell not set, returning -999"
             << G4endl;
    return -999;
  }
}





// -------------------- C E L L  ---------------------


// Set methods

void TileCalibDddbManager::SetCurrentCell(unsigned int index)
{
  if(index >=n_tile_cell )
    G4cout   << "SetCurrentCell() - The requested index = " << index << " out of range"
             << G4endl;
  else
  {
    currentCell = (*tile_cell)[index];
    currentCellInd = index;
  }
}

int TileCalibDddbManager::SetFirstDetCell(int detector)
{
  unsigned int ind = 0;

  // Looking for the first cell of given detector
  while (((*tile_cell)[ind]->getInt("DETECTOR") != detector) && (++ind < n_tile_cell))
  {
  }

  if(ind >= n_tile_cell)
    return 0;  // FAILURE

  currentCell = (*tile_cell)[ind];
  currentCellInd = ind;
  return 1;  // SUCCESS

}

int TileCalibDddbManager::SetFirstDetCellInSample(int detector, int sample)
{
  unsigned int detector_ind = 0;

  // Looking for the first cell of given detector
  while (((*tile_cell)[detector_ind]->getInt("DETECTOR") != detector) && (++detector_ind < n_tile_cell)) 
  {
  }

  if(detector_ind >= n_tile_cell)
    return 0;  // FAILURE
  else
  {
    // Looking for the first cell in given sample of given detector
    unsigned int sample_ind = detector_ind;

    while (((*tile_cell)[sample_ind]->getInt("SAMPLE") != sample) && (++sample_ind < n_tile_cell)) 
    {
    }

    if(sample_ind >= n_tile_cell)
      return 0;  // FAILURE

    currentCell = (*tile_cell)[sample_ind];
    currentCellInd = sample_ind;
    return 1;  // SUCCESS
  }
}

int TileCalibDddbManager::SetNextDetCell()
{
  if(currentCellInd<0) return 0; // FAILURE

  int currentDetector = (*tile_cell)[currentCellInd]->getInt("DETECTOR");

  if((++currentCellInd == static_cast<int>(n_tile_cell))||((*tile_cell)[currentCellInd]->getInt("DETECTOR") != currentDetector))
  {
    currentCellInd--;
    return 0;  // FAILURE
  }

  currentCell = (*tile_cell)[currentCellInd];
  return 1;
}

int TileCalibDddbManager::SetNextDetCellInSample()
{
  if(currentCellInd<0) return 0; // FAILURE

  int currentDetector = (*tile_cell)[currentCellInd]->getInt("DETECTOR");
  int currentSample = (*tile_cell)[currentCellInd]->getInt("SAMPLE");

  if((++currentCellInd == static_cast<int>(n_tile_cell))||
     ((*tile_cell)[currentCellInd]->getInt("DETECTOR") != currentDetector)||
     ((*tile_cell)[currentCellInd]->getInt("SAMPLE") != currentSample))
  {
    currentCellInd--;
    return 0;  // FAILURE
  }

  currentCell = (*tile_cell)[currentCellInd];
  return 1;
}


// Get methods

int TileCalibDddbManager::GetNumOfPeriodsInCell(int period_set) const
{
  std::stringstream ss;
  std::string indx;

  ss<<period_set;
  ss>>indx;

  if(currentCell) {
    if(currentCell->isFieldNull("PERIODS_"+indx)) {
      if (m_verboseLevel>5) G4cout
               << "GetNumOfPeriodsInCell() - PERIODS_"<<indx
               << " in DETECTOR "<<currentCell->getInt("DETECTOR")
               << " SAMPLE "<<currentCell->getInt("SAMPLE")
               << " is NULL, returning -999"
               << G4endl;
      return -999;
    } else {
      return currentCell->getInt("PERIODS", period_set);
    }
  }
  else
  {
    G4cout   << "GetNumOfPeriodsInCell() - Current Cell not set, returning -999"
             << G4endl;
    return -999;
  } 
}

int TileCalibDddbManager::GetCellSample() const
{
  if(currentCell)
    return currentCell->getInt("SAMPLE");
  else
  {
    G4cout   << "GetCellSample() - Current Cell not set, returning -999"
             << G4endl;
    return -999;
  } 
}

int TileCalibDddbManager::GetCellDetector() const
{
  if(currentCell)
    return currentCell->getInt("DETECTOR");
  else
  {
    G4cout   << "GetCellDetector() - Current Cell not set, returning -999"
             << G4endl;
    return -999;
  } 
}



//Get IRDBRecordsets sizes

int TileCalibDddbManager::GetTileIRDBRS_size()                 const  {
	return n_tile;
}

int TileCalibDddbManager::GetTileCalibSectionsIRDBRS_size()    const  {
	return n_tile_sect;
}

int TileCalibDddbManager::GetTileCalibCellsIRDBRS_size()       const  {
	return n_tile_cell;
}

int TileCalibDddbManager::GetTileCalibPlateCellsIRDBRS_size()  const  {
	return n_tile_pcell;
}

int TileCalibDddbManager::GetTileCalibGirderCellsIRDBRS_size() const  {
	return n_tile_gcell;
}




