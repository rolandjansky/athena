/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Look-up classes for Tile Calibration
//
// Author: Gia Khoriauli <Gia.Khoriauli@cern.ch>
// May, 2005
//
//************************************************************

#ifndef TILEGEOG4CALIB_TILEGEOG4DMLOOKUP_H
#define TILEGEOG4CALIB_TILEGEOG4DMLOOKUP_H

#include <vector>

class TileGeoG4Cell;
class TileGeoG4Section;

class TileGeoG4CalibCell {
public:

  int detector;
  int sample;
  int nrOfPeriodsInCell[2];

  TileGeoG4CalibCell();
};

class TileGeoG4PlateCell {
public:

  int detector;
  int tower;
  int sample;
  int neighbor;
  double eta;
  double dEta;
  double xBound;
  double zBound;

  std::vector<int> m_nPlateCellHit;
  std::vector<int> m_nPlateCellHit_negative;

  TileGeoG4PlateCell();
};

class TileGeoG4GirderCell {
public:

  int detector;
  int tower;
  int sample;
  double eta;
  double dEta;
  double xBound;

  std::vector<int> m_nGirderCellHit;
  std::vector<int> m_nGirderCellHit_negative;

  TileGeoG4GirderCell();
};

struct TileGeoG4CalibSample {
  std::vector<TileGeoG4CalibCell*> cells;
  std::vector<TileGeoG4PlateCell*> plateCells;
};

class TileGeoG4CalibSection {
public:

  TileGeoG4CalibSection(const int verboseLevel);
  ~TileGeoG4CalibSection();

  int section;
  int nrOfPeriods;
  int nrOfSamples;
  int nrOfCells;
  int nrOfPlateCells;
  int nrOfGirderCells;
  double sample_ZBound[4];

  std::vector<TileGeoG4CalibSample*> samples;
  std::vector<TileGeoG4GirderCell*> girderCells;

  //Builds correspondence between absorber materials and cells. fills m_DMToCell in
  void DMToCell(bool gap_crack, TileGeoG4Section* tile_section);

  //Returns the pointer to the cell corresponding to nPeriod-th period and sample
  TileGeoG4Cell* GetCell(int nPeriod, int nSample);

  //Returns the pointer to the Plate 'cell' corresponding to the Hits  x, z - coordinates
  //and the kind of Plate, Front or End.
  TileGeoG4PlateCell* GetTilePlateCell(double xHit, double zHit, int plate);

  //Returns the pointer to the Girder 'cell' corresponding to the Hits x - coordinate
  TileGeoG4GirderCell* GetTileGirderCell(double xHit);

  //Initializes all arayes of all cells of all Tile sections
  void AddModuleToCell(bool negative);

private:

  //for each unit of Absorber Material ("Period") in Tile modules
  //contains pointer to the corresponding cell
  std::vector<TileGeoG4Cell*> m_DMToCell;
  int m_verboseLevel;

};

#endif // TILEGEOG4CALIB_TILEGEOG4DMLOOKUP_H

