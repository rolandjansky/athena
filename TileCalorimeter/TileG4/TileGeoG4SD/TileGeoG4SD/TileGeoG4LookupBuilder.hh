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

#ifndef TILEGEOG4SD_TILEGEOG4LOOKUPBUILDER_H
#define TILEGEOG4SD_TILEGEOG4LOOKUPBUILDER_H

#include <map>
#include <string>
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileSimEvent/TileHitVector.h"

class TileGeoG4Cell;
class TileGeoG4Section;
class StoreGateSvc;

class TileGeoG4LookupBuilder {
public:
  TileGeoG4LookupBuilder(StoreGateSvc* pDetStore, const int verboseLevel);
  ~TileGeoG4LookupBuilder();
  void BuildLookup(bool test_beam = false);
  TileGeoG4Section* GetSection(TileDddbManager::TileSections key) const;

  // Method invoked at the end of each event by SD
  // it copies all TileHits to TileHitVector and deletes them
  void ResetCells(TileHitVector* tileHitsCollection);
  // returns number of PMTs in C10 Cells
  int TileGeoG4npmtC10(int side, int module) {
    return m_npmtC10[side][module];
  }
  // returns number of PMTs in D4 Cells
  int TileGeoG4npmtD4(int side, int module) {
    return m_npmtD4[side][module];
  }
  // returns number of PMTs in E4' Cells
  int TileGeoG4npmtE5(int side, int module) {
    return m_npmtE5[side][module];
  }

private:
  typedef std::map<std::string, TileGeoG4Cell*, std::less<std::string> > TileGeoG4CellMap;
  typedef std::map<TileDddbManager::TileSections, TileGeoG4Section*, std::less<TileDddbManager::TileSections> > TileGeoG4SectionMap;

  void CreateGeoG4Cells();
  void CreateGeoG4Sections(bool is_tb);
  // Fill array of number of PMTs for C10 Cells
  void MakeTileGeoG4npmtC10();
  // Fill array of number of PMTs for D4 Cells
  void MakeTileGeoG4npmtD4();
  // Fill array of number of PMTs for E4' Cells
  void MakeTileGeoG4npmtE5();

  const TileDetDescrManager* m_theManager;
  const TileID* m_tileID;
  TileDddbManager* m_dbManager;
  TileGeoG4CellMap* m_cellMap;
  TileGeoG4SectionMap* m_sectionMap;
  bool m_isE5;
  int m_npmtC10[2][64]; //array of number of PMTs for C10 Cells
  int m_npmtD4[2][64];  //array of number of PMTs for D4 Cells
  int m_npmtE5[2][64];  //array of number of PMTs for E4' Cells

  int m_verboseLevel; //!< Level of verbosity, passed in from above
};

#endif // TILEGEOG4SD_TILEGEOG4LOOKUPBUILDER_H
