/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////
//
// Manager for Tile Calibration DB
//
// author: Gia Khoriauli  <gia@mail.cern.ch> 
//
// May, 2005
//
////////////////////////////////////////////////////////////////////////

#ifndef TILEGEOG4CALIB_TILECALIBDDDBMANAGER_H
#define TILEGEOG4CALIB_TILECALIBDDDBMANAGER_H

#include <string>

#include "GaudiKernel/ServiceHandle.h"
class IRDBAccessSvc;
class IRDBRecordset;
class IRDBRecord;

class TileCalibDddbManager {
public:

  TileCalibDddbManager(ServiceHandle<IRDBAccessSvc> &access, std::string version_tag, std::string version_node,
                       const int verboseLevel);
  // Default destructor is fine

  enum TileCalibSections {
    TILE_BARREL = 1,
    TILE_EBARREL = 2,
    TILE_PLUG1 = 3,
    TILE_PLUG2 = 4,
    TILE_PLUG3 = 5,
    TILE_PLUG4 = 6
  };

  // ---------------- T I L E ---------------
  int GetNumModules(int index) const;
  bool GetSwitchPlateToCell() const;
  // -------------- S E C T I O N --------------
  void SetCurrentSection(unsigned int section);

  int GetCurrentSection() const;
  int GetNumSectPeriods() const;
  int GetNumSectSamples() const;
  int GetNumSectCells() const;
  int GetNumSectPlateCells() const;
  int GetNumSectGirderCells() const;

  double GetSampleZBound(int index) const;

  // ---------- G I R D E R   C E L L -----------
  void SetCurrentGirderCell(unsigned int item);

  int SetFirstDetGirderCell(int detector);
  int SetNextDetGirderCell();

  int GetGirderCellDetector() const;
  int GetGirderCellNum() const;
  int GetGirderCellTower() const;
  int GetGirderCellSample() const;
  double GetGirderCellEta() const;
  double GetGirderCellDeta() const;
  double GetGirderCellXBound() const;

  // ------------ P L A T E  C E L L ----------
  void SetCurrentPlateCell(unsigned int section);

  int SetFirstDetPlateCell(int detector);
  int SetFirstDetPlateCellInSample(int detector, int sample);
  int SetNextDetPlateCell();
  int SetNextDetPlateCellInSample();

  int GetPlateCellDetector() const;
  int GetPlateCellNum() const;
  int GetPlateCellTower() const;
  int GetPlateCellSample() const;
  double GetPlateCellEta() const;
  double GetPlateCellDeta() const;
  double GetPlateCellXBound() const;
  double GetPlateCellZBound() const;
  int GetPlateCellNeighborPeriod() const;

  // ----------------- C E L L -------------------

  //  void SetCurrentCell(int detector, double sample, double tower);
  void SetCurrentCell(unsigned int index);

  // Find first Cell for given Tile Section (detector)
  // 1 - success; 0 - doesn't exist any Cells for given Section
  int SetFirstDetCell(int detector);

  // Find first Cell for given Tile Section and Sample
  // 1 - success; 0 - doesn't exist any such Cell
  int SetFirstDetCellInSample(int detector, int sample);

  // Move to the next Cell with the same Tile Section field
  // 1 - success; 0 - the current is last one, it remains unchanged
  int SetNextDetCell();

  // Move to the next Cell with the same Tile Section and Sample fields
  // 1 - success; 0 - the current is last one, it remains unchanged
  int SetNextDetCellInSample();

  int GetNumOfPeriodsInCell(int index) const;
  int GetCellSample() const;
  int GetCellDetector() const;

  //Get IRDBRecordsets sizes
  int GetTileIRDBRS_size() const;
  int GetTileCalibSectionsIRDBRS_size() const;
  int GetTileCalibCellsIRDBRS_size() const;
  int GetTileCalibPlateCellsIRDBRS_size() const;
  int GetTileCalibGirderCellsIRDBRS_size() const;

private:

  const IRDBRecordset* m_tile;
  const IRDBRecordset* m_tileSection;
  const IRDBRecordset* m_tileCell;
  const IRDBRecordset* m_tilePlateCell;
  const IRDBRecordset* m_tileGirderCell;
  const IRDBRecordset* m_switches;

  unsigned int m_nTile;
  unsigned int m_nTileSect;
  unsigned int m_nTileCell;
  unsigned int m_nTilePCell;
  unsigned int m_nTileGCell;

  const IRDBRecord* m_currentTile;
  const IRDBRecord* m_currentSection;
  const IRDBRecord* m_currentCell;
  const IRDBRecord* m_currentPlateCell;
  const IRDBRecord* m_currentGirderCell;

  int m_currentCellInd;
  int m_currentPlateCellInd;
  int m_currentGirderCellInd;

  std::string m_tag;
  std::string m_node;

  int m_verboseLevel;
};

#endif // TILEGEOG4CALIB_TILECALIBDDDBMANAGER_H
