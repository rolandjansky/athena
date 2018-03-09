/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Classes holding look-up tables for TileGeoG4SD
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
//************************************************************

#ifndef TILEGEOG4SD_TILEGEOG4LOOKUP_H
#define TILEGEOG4SD_TILEGEOG4LOOKUP_H

#include <vector>
#include <string>

class TileSimHit;

class TileGeoG4Cell {
public:
  int detector;            // 1-Barrel, 2-Extended Barrel, 3-ITC,
  // 4-gap/crack, 5-Mod0, 6-Prototype

  int cellNum;             // from -16 (A-16) to 16 (A16)

  int tower;               // from -16 (A-16) to 16 (A16)

  int sample;              // 1 - A, Proto1
  // 2 - B, BC, C, Proto2
  // 3 - D, Proto3
  // 4 - gap/crack, Proto4

  double sinTh;    //added by Mike
  double r;        //added by Mike

  int firstRow;
  int lastRow;
  std::vector<int> nrOfTilesInRow;
  int nrOfPMT;
  std::vector<int> holes;

  double zMin, zMax, zMin2, zMax2;  //added by Sergey

  // Two Vectors containing pointers to TileSimHit objects
  // corresponding to the PMTs of current cell
  // One vector contains information concerning Up PMT, another - Down PMT
  // Index in the vector is module CopyNumber-1.
  //
  // If there is only one PMT for the cell (GAP/CRACK scintillators) just UP vector are filled in

  std::vector<TileSimHit*> moduleToHitUp;
  std::vector<TileSimHit*> moduleToHitDown;
  std::vector<int> nCellHit;

  //Same as previous pair, but corresponding to negative side of extended sections
  std::vector<TileSimHit*> moduleToHitUpNegative;
  std::vector<TileSimHit*> moduleToHitDownNegative;
  std::vector<int> nCellHitNegative;

  TileGeoG4Cell();
    
};

class TileGeoG4Sample {
public:
  int firstRow;
  int lastRow;
  std::vector<TileGeoG4Cell*> cells;

  TileGeoG4Sample();
};

class TileGeoG4Section {
public:
  std::vector<TileGeoG4Sample*> samples;

  int nrOfModules;
  int nrOfPeriods;
  int nrOfScintillators;

  int nrOfPhysicalSections;
  int nrOfPhysicalSections_negative;

  TileGeoG4Section(const int verboseLevel);
  ~TileGeoG4Section();

  // Builds correspondence between scintillators and cells, i.e. fills m_ScinToCell
  void ScinToCell(bool gap_crack, int rowShift);

  // Returns the pointer to the cell corresponding to nScin-th scintillator
  TileGeoG4Cell* ScinToCell(int nScin);

  // Adds NULL in m_ModuleToHit vector for each cell of current section
  void AddModuleToCell(bool negative = 0);

  //Prints scin-cell correspondance information
  void PrintScinToCell(std::string section_name);

private:
  //for each scintillator in the module contains pointer to the corresponding cell
  std::vector<TileGeoG4Cell*> m_ScinToCell;
  int m_verboseLevel;

};

class TileRow {
public:
  std::vector<double> attLen[3]; // 3*11 values of lambda for barrel, ext bar, ITC
  std::vector<double> OpticalRatio[3]; // 3*11 I0 valies for barrel, ext bar, ITC

  TileRow(std::string& dataFile, std::string& ratioFile);
};

#endif // TILEGEOG4SD_TILEGEOG4LOOKUP_H
