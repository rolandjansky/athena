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

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TileGeoG4Calib/TileCalibDddbManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include <sstream>


TileCalibDddbManager::TileCalibDddbManager(IRDBAccessSvc* access,
                                           std::string    version_tag,
                                           std::string    version_node,
                                           IMessageSvc* msgSvc) : 
  currentTile(0),
  currentSection(0),
  currentCell(0),
  currentPlateCell(0),
  currentGirderCell(0),
  currentCellInd(-1),
  currentPlateCellInd(-1),
  currentGirderCellInd(-1),
  mTag(version_tag),
  mNode(version_node)
{

  m_log = new MsgStream (msgSvc, "TileCalibDddbManager");

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

TileCalibDddbManager::~TileCalibDddbManager()
{
  delete m_log;
}



// --------------------------- T I L E -------------------------

int TileCalibDddbManager::GetNumModules(int ind)  const
{
  if((*tile)[ind])
    return (*tile)[ind]->getInt("NMODUL");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetNumModules() - Unable to retrieve number of modules. return -999"
             << endreq;
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
    (*m_log) << MSG::ERROR
             << "GetSwitchPlateToCell() - Unable to retrieve swithes. return TRUE"
             << endreq;
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
    (*m_log) << MSG::ERROR
             << "SetCurrentSection() - Unable to find the section " << section
             << endreq;
  else
    currentSection = (*tile_section)[ind];
}

int TileCalibDddbManager::GetCurrentSection() const
{
  if(currentSection)
    return currentSection->getInt("SECT");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetCurrentSection() - Current Section not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectPeriods() const
{
  if(currentSection)
    return currentSection->getInt("PERIODS");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetNumSectPeriods() - Current Section not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectSamples() const
{
  if(currentSection)
    return currentSection->getInt("SAMPLES");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetNumSectSamples() - Current Section not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectCells() const
{
  if(currentSection)
    return currentSection->getInt("CELLS");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetNumSectCells() - Current Section not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectPlateCells() const
{
  if(currentSection) {
    if(currentSection->isFieldNull("PLATECELLS")) {
      (*m_log) << MSG::DEBUG
               << "GetNumSectPlateCells() - PLATECELLS in section " << currentSection->getInt("SECT")
               << " is NULL, returning -999"
               << endreq;
      return -999;
    } else {
      return currentSection->getInt("PLATECELLS");
    }
  }
  else
  {
    (*m_log) << MSG::ERROR
             <<"GetNumSectPlateCells() - Current Section not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetNumSectGirderCells() const
{
  if(currentSection){
    if(currentSection->isFieldNull("GIRDERCELLS")) {
      (*m_log) << MSG::DEBUG
               << "GetNumSectGirderCells() - GIRDERCELLS in section " << currentSection->getInt("SECT")
               << " is NULL, returning -999"
               << endreq;
      return -999;
    } else {
      return currentSection->getInt("GIRDERCELLS");
    }
  }
  else
  {
    (*m_log) << MSG::ERROR
             << "GetNumSectGirderCells() - Current Section not set, returning -999"
             << endreq;
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
    (*m_log) << MSG::ERROR
             <<"GetSampleZBound() - Current Section not set, returning -9999.9"
             << endreq;
    return -9999.9;
  }
}



// ------------------ G I R D E R   C E L L------------------

//Set methods

void TileCalibDddbManager::SetCurrentGirderCell(unsigned int index)
{
  if(index >=n_tile_gcell )
    (*m_log) << MSG::ERROR
             << "SetCurrentGirderCell() - The requested index = " << index << " out of range"
             << endreq;
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
    (*m_log) << MSG::ERROR
             << "GetGirderCellDetector() - Current GirderCell not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetGirderCellTower() const
{
  if(currentGirderCell)
    return currentGirderCell->getInt("TOWER");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetGirderCellTower() - Current GirderCell not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetGirderCellSample() const
{
  if(currentGirderCell)
    return currentGirderCell->getInt("SAMPLE");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetGirderCellSample() - Current GirderCell not set, returning -999"
             << endreq;
    return -999;
  }
}

double TileCalibDddbManager::GetGirderCellEta() const
{
  if(currentGirderCell)
    return currentGirderCell->getDouble("ETA");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetGirderCellEta() - Current GirderCell not set, returning -999.9"
             << endreq;
    return -999.9;
  }
}

double TileCalibDddbManager::GetGirderCellDeta() const
{
  if(currentGirderCell)
    return currentGirderCell->getDouble("DETA");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetGirderCellDeta() - Current GirderCell not set, returning -999.9"
             << endreq;
    return -999.9;
  }
}

double TileCalibDddbManager::GetGirderCellXBound() const
{
  if(currentGirderCell)
    return currentGirderCell->getDouble("XBOUND");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetGirderCellXBound() - Current GirderCell not set, returning -9999.9"
             << endreq;
    return -9999.9;
  }
}





// -----------------  P L A T E   C E L L  ----------------

//Set methods

void TileCalibDddbManager::SetCurrentPlateCell(unsigned int index)
{
  if(index >=n_tile_pcell )
    (*m_log) << MSG::ERROR
             << "SetCurrentPlateCell() - The requested index = " << index << " out of range"
             << endreq;
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
    (*m_log) << MSG::ERROR
             << "GetPlateCellDetector() - Current PlateCell not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetPlateCellTower() const
{
  if(currentPlateCell)
    return currentPlateCell->getInt("TOWER");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetPlateCellTower() - Current PlateCell not set, returning -999"
             << endreq;
    return -999;
  }
}

int TileCalibDddbManager::GetPlateCellSample() const
{
  if(currentPlateCell)
    return currentPlateCell->getInt("SAMPLE");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetPlateCellSample() - Current PlateCell not set, returning -999"
             << endreq;
    return -999;
  }
}

double TileCalibDddbManager::GetPlateCellEta() const
{
  if(currentPlateCell)
    return currentPlateCell->getDouble("ETA");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetPlateCellEta() - Current PlateCell not set, returning -999.9"
             << endreq;
    return -999.9;
  }
}

double TileCalibDddbManager::GetPlateCellDeta() const
{
  if(currentPlateCell)
    return currentPlateCell->getDouble("DETA");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetPlateCellDeta() - Current PlateCell not set, returning -999.9"
             << endreq;
    return -999.9;
  }
}

double TileCalibDddbManager::GetPlateCellXBound() const
{
  if(currentPlateCell)
    return currentPlateCell->getDouble("XBOUND");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetPlateCellXBound() - Current PlateCell not set, returning -9999.9"
             << endreq;
    return -9999.9;
  }
}

double TileCalibDddbManager::GetPlateCellZBound() const
{
  if(currentPlateCell) {
    if(currentPlateCell->isFieldNull("ZBOUND")) {
      (*m_log) << MSG::DEBUG
               << "GetPlateCellZBound() - ZBOUND"
               << " in DETECTOR "<<currentPlateCell->getInt("DETECTOR")
               << " SAMPLE "<<currentPlateCell->getInt("SAMPLE")
               << " TOWER "<<currentPlateCell->getInt("TOWER")
               << " is NULL, returning -9999.9"
               << endreq;
      return -9999.9;
    } else {
      return currentPlateCell->getDouble("ZBOUND");
    }
  }
  else
  {
    (*m_log) << MSG::ERROR
             << "GetPlateCellZBound() - Current PlateCell not set, returning -9999.9"
             << endreq;
    return -9999.9;
  }
}

int TileCalibDddbManager::GetPlateCellNeighborPeriod() const
{
  if(currentPlateCell) {
    if(currentPlateCell->isFieldNull("NEIGHPERIOD")) {
      (*m_log) << MSG::DEBUG
               << "GetPlateCellNeighborPeriod() - NEIGHPERIOD "
               << " in DETECTOR "<<currentPlateCell->getInt("DETECTOR")
               << " SAMPLE "<<currentPlateCell->getInt("SAMPLE")
               << " TOWER "<<currentPlateCell->getInt("TOWER")
               << " is NULL, returning -999"
               << endreq;
      return -999;
    } else {
      return currentPlateCell->getInt("NEIGHPERIOD");
    }
  }
  else
  {
    (*m_log) << MSG::ERROR
             << "GetPlateCellNeighborPeriod() - Current PlateCell not set, returning -999"
             << endreq;
    return -999;
  }
}





// -------------------- C E L L  ---------------------


// Set methods

void TileCalibDddbManager::SetCurrentCell(unsigned int index)
{
  if(index >=n_tile_cell )
    (*m_log) << MSG::ERROR
             << "SetCurrentCell() - The requested index = " << index << " out of range"
             << endreq;
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
      (*m_log) << MSG::DEBUG
               << "GetNumOfPeriodsInCell() - PERIODS_"<<indx
               << " in DETECTOR "<<currentCell->getInt("DETECTOR")
               << " SAMPLE "<<currentCell->getInt("SAMPLE")
               << " is NULL, returning -999"
               << endreq;
      return -999;
    } else {
      return currentCell->getInt("PERIODS", period_set);
    }
  }
  else
  {
    (*m_log) << MSG::ERROR
             << "GetNumOfPeriodsInCell() - Current Cell not set, returning -999"
             << endreq;
    return -999;
  } 
}

int TileCalibDddbManager::GetCellSample() const
{
  if(currentCell)
    return currentCell->getInt("SAMPLE");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetCellSample() - Current Cell not set, returning -999"
             << endreq;
    return -999;
  } 
}

int TileCalibDddbManager::GetCellDetector() const
{
  if(currentCell)
    return currentCell->getInt("DETECTOR");
  else
  {
    (*m_log) << MSG::ERROR
             << "GetCellDetector() - Current Cell not set, returning -999"
             << endreq;
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




