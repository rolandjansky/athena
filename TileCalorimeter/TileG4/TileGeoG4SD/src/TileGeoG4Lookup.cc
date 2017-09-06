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

#include "G4ios.hh"

#include "TileGeoG4SD/TileGeoG4Lookup.hh"
#include <fstream>
#include <string>

TileRow::TileRow(std::string& datafile, std::string& ratiofile) {
  std::ifstream dataInFile, ratioInFile;
  double placeholder;

  dataInFile.open(datafile.c_str());

  for (int i = 0; i < 11; ++i) {
    for (int j = 0; j < 3; ++j) {
      dataInFile >> placeholder;
      attLen[j].push_back(placeholder);
    }
  }
  dataInFile.close();

  ratioInFile.open(ratiofile.c_str());

  for (int i = 0; i < 11; ++i) {
    for (int j = 0; j < 3; ++j) {
      ratioInFile >> placeholder;
      OpticalRatio[j].push_back(placeholder);
    }
  }
  ratioInFile.close();
}

TileGeoG4Cell::TileGeoG4Cell()
  : detector(0),
    cellNum(0),
    tower(0),
    sample(0),
    sinTh(0.),
    r(0.),
    firstRow(0),
    lastRow(0),
    nrOfPMT(0),
    zMin(0.),
    zMax(0.),
    zMin2(0.),
    zMax2(0.)    //added by Sergey
{
}

TileGeoG4Sample::TileGeoG4Sample()
  : firstRow(0),
    lastRow(0)
{
}

TileGeoG4Section::TileGeoG4Section(const int verboseLevel)
  : nrOfModules(0),
    nrOfPeriods(0),
    nrOfScintillators(0),
    nrOfPhysicalSections(0),
    nrOfPhysicalSections_negative(0),
    m_verboseLevel(verboseLevel)
{
}

TileGeoG4Section::~TileGeoG4Section() {
}

void TileGeoG4Section::ScinToCell(bool gap_crack, int rowShift) {

  int l_nRow = nrOfScintillators;             //Number of scintillators in the period
  int l_nPeriod = nrOfPeriods;                //Number of periods in the module
  int l_indCurrentSample;
  int l_rowShiftInSample;                     //Index of current row according to the current sample

  int* l_indCurrentCell = new int[l_nRow];    //Index of current cell for each row
  int* l_currentBoundary = new int[l_nRow];   //Current boundary for the row
  int row, per;

  m_ScinToCell.clear();

  // Initialize arrays. All current cells have index - 0.
  // Current boundaries are number of tiles for cells with the index 0
  l_indCurrentSample = 0;

  if (!gap_crack) for (row = 1; row <= l_nRow; row++) {
      if (row > (samples[l_indCurrentSample])->lastRow - rowShift) l_indCurrentSample++;
      l_rowShiftInSample = row - ( (samples[l_indCurrentSample])->firstRow - rowShift);
      l_indCurrentCell[row - 1] = 0;
      l_currentBoundary[row - 1] = ( (samples[l_indCurrentSample])->cells[0])->nrOfTilesInRow[l_rowShiftInSample];
    }

  //Loop through all periods and scintillators - fill m_ScinToCell
  //with the pointers to cells the current scintillator belongs to
  for (per = 0; per < l_nPeriod; per++) {
    l_indCurrentSample = 0;
    for (row = 1; row <= l_nRow; row++) {
      if (gap_crack) {
        m_ScinToCell.push_back( (samples[0])->cells[row - 1]);
      } else {
        if (row > (samples[l_indCurrentSample])->lastRow - rowShift) l_indCurrentSample++;
        l_rowShiftInSample = row - ( (samples[l_indCurrentSample])->firstRow - rowShift);

        if (per == l_currentBoundary[row - 1]) {
          //Boundary reached - move to the next cell and next boundary
          //Check forced by special period in ITC2

          if (++l_indCurrentCell[row - 1] < static_cast<int>( (samples[l_indCurrentSample])->cells.size())) {
            l_currentBoundary[row - 1] += ((samples[l_indCurrentSample])->cells[l_indCurrentCell[row - 1]])->nrOfTilesInRow[l_rowShiftInSample];
          }
        }

        if (l_indCurrentCell[row - 1] < static_cast<int>( (samples[l_indCurrentSample])->cells.size())) {
          m_ScinToCell.push_back( (samples[l_indCurrentSample])->cells[l_indCurrentCell[row - 1]]);
        } else {
          m_ScinToCell.push_back(0); // Caused by absence of 6-th scintillator in row 2 of ITC2
        }
      } //if(gap_crack)
    } //for(row)
  } //for(per)

  delete[] l_indCurrentCell;
  delete[] l_currentBoundary;

}

TileGeoG4Cell* TileGeoG4Section::ScinToCell(int nScin) {
  if (nScin < 0 || nScin >= static_cast<int>(m_ScinToCell.size())) {
    G4cout << "ERROR: ScinToCell(): Bad index to retrieve pointer to the Cell --> " << nScin << G4endl;
    return 0;
  }
  else
    return m_ScinToCell[nScin];
}

void TileGeoG4Section::AddModuleToCell(bool negative) {
  size_t l_nSamp, l_nCell;
  TileGeoG4Cell* current_cell;
  TileGeoG4Sample* current_sample;

  //Iterate through all samples of the section
  for (l_nSamp = 0; l_nSamp < samples.size(); l_nSamp++) {
    current_sample = samples[l_nSamp];

    //Iterate through all cells of the current sample
    for (l_nCell = 0; l_nCell < current_sample->cells.size(); l_nCell++) {
      current_cell = current_sample->cells[l_nCell];

      switch (current_cell->nrOfPMT) {
      case 1: {
        // We are in GAP/CRACK Cell - use Up vectors only
        if (negative) {
          current_cell->moduleToHitDownNegative.push_back(0);
          current_cell->nCellHitNegative.push_back(0);
        } else {
          current_cell->moduleToHitDown.push_back(0);
          current_cell->nCellHit.push_back(0);
        }
        break;
      }
      case 2: {
        // Standart cell and E5(E4')
        if (negative) {
          current_cell->moduleToHitUpNegative.push_back(0);
          current_cell->moduleToHitDownNegative.push_back(0);
          current_cell->nCellHitNegative.push_back(0);
        } else {
          current_cell->moduleToHitUp.push_back(0);
          current_cell->moduleToHitDown.push_back(0);
          current_cell->nCellHit.push_back(0);
        }
        break;
      }
      default: {
        // Unknown cell type
        G4cout << "ERROR: AddModuleToCell(): Unexpected number of PMTs in cell --> " << current_cell->nrOfPMT
               << G4endl;
        return;
      }
      }
    }
  }

  if (negative) nrOfPhysicalSections_negative++;
  else nrOfPhysicalSections++;
}

void TileGeoG4Section::PrintScinToCell(std::string section_name) {
  if (m_verboseLevel <= 5) return;
  int l_nRow = nrOfScintillators;
  int i = 0;

  G4cout << G4endl;
  G4cout << "***********************************************************" << G4endl;
  G4cout << "* Printing Scintillator-to-Cell corespondence for section *" << G4endl;
  G4cout << "       " << section_name.c_str() << "           " << G4endl;
  G4cout << "***********************************************************" << G4endl;

  for (size_t j = 0; j < m_ScinToCell.size(); j++) {
    if (m_ScinToCell[j]) G4cout << m_ScinToCell[j]->tower << "," << m_ScinToCell[j]->sample << " ";
    if (++i == l_nRow) {
      i = 0;
    }
  }
  G4cout << G4endl;

  G4cout << "***********************************************************" << G4endl;
  G4cout << G4endl;

}
