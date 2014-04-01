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

#ifndef TileCalibDddbManager_H
#define TileCalibDddbManager_H

#include <string>

class IRDBAccessSvc;
class IRDBRecordset;
class IRDBRecord;
class IMessageSvc;
class MsgStream;

class TileCalibDddbManager
{
 public:

  TileCalibDddbManager(IRDBAccessSvc* access,
		       std::string    version_tag,
		       std::string    version_node,
                       IMessageSvc* m_msgSvc);

  ~TileCalibDddbManager();
    

  enum TileCalibSections
  {
    TILE_BARREL=1,
    TILE_EBARREL=2,
    TILE_PLUG1=3,
    TILE_PLUG2=4,
    TILE_PLUG3=5,
    TILE_PLUG4=6
  };


  // ---------------- T I L E ---------------
  int GetNumModules(int index) const;
  bool GetSwitchPlateToCell()	const;
  // -------------- S E C T I O N --------------
  void SetCurrentSection(unsigned int section);

  int GetCurrentSection()     const;
  int GetNumSectPeriods()     const;
  int GetNumSectSamples()     const;
  int GetNumSectCells()       const;
  int GetNumSectPlateCells()  const;
  int GetNumSectGirderCells() const;

  double GetSampleZBound(int index) const ;


  // ---------- G I R D E R   C E L L -----------
  void SetCurrentGirderCell(unsigned int item);

  int SetFirstDetGirderCell(int detector);
  int SetNextDetGirderCell();
  
  int GetGirderCellDetector() const;
  int GetGirderCellNum()      const;
  int GetGirderCellTower()    const;
  int GetGirderCellSample()   const;
  double GetGirderCellEta()   const;
  double GetGirderCellDeta()  const;
  double GetGirderCellXBound()const;


  // ------------ P L A T E  C E L L ----------
  void SetCurrentPlateCell(unsigned int section);

  int SetFirstDetPlateCell(int detector);
  int SetFirstDetPlateCellInSample(int detector, int sample);
  int SetNextDetPlateCell();
  int SetNextDetPlateCellInSample();

  int GetPlateCellDetector() const;
  int GetPlateCellNum()      const;
  int GetPlateCellTower()    const;
  int GetPlateCellSample()   const;
  double GetPlateCellEta()   const;
  double GetPlateCellDeta()  const;
  double GetPlateCellXBound()const;
  double GetPlateCellZBound()const;
  int GetPlateCellNeighborPeriod()const;



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
  int GetTileIRDBRS_size()		   const ;
  int GetTileCalibSectionsIRDBRS_size()    const ;
  int GetTileCalibCellsIRDBRS_size() 	   const ;
  int GetTileCalibPlateCellsIRDBRS_size()  const ;
  int GetTileCalibGirderCellsIRDBRS_size() const ;


 private:

  const IRDBRecordset* tile;
  const IRDBRecordset* tile_section;
  const IRDBRecordset* tile_cell;
  const IRDBRecordset* tile_plateCell;
  const IRDBRecordset* tile_girderCell;
  const IRDBRecordset* switches;
  
  unsigned int n_tile;
  unsigned int n_tile_sect;
  unsigned int n_tile_cell;
  unsigned int n_tile_pcell;
  unsigned int n_tile_gcell;

  const IRDBRecord* currentTile;
  const IRDBRecord* currentSection;
  const IRDBRecord* currentCell;
  const IRDBRecord* currentPlateCell;
  const IRDBRecord* currentGirderCell;

  int currentCellInd;
  int currentPlateCellInd;
  int currentGirderCellInd;

  std::string mTag;
  std::string mNode;

  MsgStream* m_log;
};

#endif
