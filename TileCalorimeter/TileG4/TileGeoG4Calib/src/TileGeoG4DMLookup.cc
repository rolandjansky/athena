/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription classes for TileCal
//
// Author: Gia Khoriauli <gia.khoriauli@cern.ch>
// Junary 21th, 2004.
//
//************************************************************

#include "TileGeoG4DMLookup.h"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"

#include "G4ios.hh"

TileGeoG4CalibCell::TileGeoG4CalibCell()
  : detector(0),
    sample(0),
    nrOfPeriodsInCell() {
}

TileGeoG4PlateCell::TileGeoG4PlateCell()
  : detector(0),
    tower(0),
    sample(0),
    neighbor(0),
    eta(0),
    dEta(0),
    xBound(0),
    zBound(0) {
}

TileGeoG4GirderCell::TileGeoG4GirderCell()
  : detector(0),
    tower(0),
    sample(0),
    eta(0),
    dEta(0),
    xBound(0) {
}

TileGeoG4CalibSection::TileGeoG4CalibSection(const int verboseLevel)
  : section(0),
    nrOfPeriods(0),
    nrOfSamples(0),
    nrOfCells(0),
    nrOfPlateCells(0),
    nrOfGirderCells(0),
    sample_ZBound(),
    m_verboseLevel(verboseLevel) {
    }

void TileGeoG4CalibSection::DMToCell(bool gap_crack, TileGeoG4Section* tile_section) {
  int l_nSample = nrOfSamples;                    //Number of samples in the module
  int l_nPeriod = nrOfPeriods;                    //Number of periods in the module 

  int* l_indCurrentCell = new int[l_nSample];    //Index of current cell for each sample
  int* l_currentBoundary = new int[l_nSample];    //Current boundary for each sample
  int samp, per;
  bool passed = false;

  m_DMToCell.clear();

  // Initialize arrays. All current cells have index - 0.
  // Current boundaries are number of periods for cells with the index 0
  if (!gap_crack)                                                               // if we aren't into gap or crack   //g

    for (samp = 0; samp < l_nSample; samp++) {
      if (l_nSample == 4 && samp >= 2) passed = true;                                          // forced by Barrel section
      if (!passed) {            // if we are in the Barrel we DIDN'T PASSE the B subsample of bounded B & C cells yet!

        l_indCurrentCell[samp] = 0;
        l_currentBoundary[samp] = ( (samples[samp])->cells[0])->nrOfPeriodsInCell[0]; // A-Barrel or A,C,D-Ext. Barrel or ITC
      } else {
        if (samples[samp - 1]->cells[1]->nrOfPeriodsInCell[1] != 0) {                 //we ARE in the C subsample in Barrel

          l_indCurrentCell[samp] = 0;
          l_currentBoundary[samp] = ( (samples[samp - 1])->cells[0])->nrOfPeriodsInCell[1];   // C-Barrel
        } else {
          l_indCurrentCell[samp] = 0;
          l_currentBoundary[samp] = ( (samples[samp - 1])->cells[0])->nrOfPeriodsInCell[0];  // D-Barrel
        }
      }

      //Loop through all periods and samples - fill m_DMToCell
      //with the pointers to cells the current period of sample belongs to
      for (per = 0; per < l_nPeriod; per++) {

        if (per == l_currentBoundary[samp]) {
          //Boundary reached - move to the next cell and next boundary

          if (!passed) {                      //If we are in Barrel, then we didn't passed B-subsample of BC sample yet.

            if (++l_indCurrentCell[samp] < static_cast<int>( (samples[samp])->cells.size())) {
              l_currentBoundary[samp] += ( (samples[samp])->cells[l_indCurrentCell[samp]])->nrOfPeriodsInCell[0];
            }  // A,B-Barrel or A,b,C,D - Ext. Barrel or ITC
          } else {                                                                                     // passed==true
            if (samples[samp - 1]->cells[1]->nrOfPeriodsInCell[1] != 0) {                  // C-cell of Barrel

              if (++l_indCurrentCell[samp] < static_cast<int>( (samples[samp - 1])->cells.size())) {
                l_currentBoundary[samp] += ( (samples[samp - 1])->cells[l_indCurrentCell[samp]])->nrOfPeriodsInCell[1];
              }  // C subsample == 2 sample
            } else {                                                                              // D-cells of Barrel

              if (++l_indCurrentCell[samp] < static_cast<int>( (samples[samp - 1])->cells.size())) {
                l_currentBoundary[samp] += ( (samples[samp - 1])->cells[l_indCurrentCell[samp]])->nrOfPeriodsInCell[0];
              }    // D sample == 3 sample   Barrel
            }
          }
        }

        if (!passed) {                                                    // didn't passed B-subsample of BC sample yet.

          if (l_indCurrentCell[samp] < static_cast<int>( (samples[samp])->cells.size())) {
            m_DMToCell.push_back( (tile_section->samples[samp])->cells[l_indCurrentCell[samp]]);
          }     // A,B - Barrel or A,B,C,D - Ext. Barrel or ITC
        } else {                                     	                                                 // passed==true
          if (l_indCurrentCell[samp] < static_cast<int>( (samples[samp - 1])->cells.size())) {
            m_DMToCell.push_back( (tile_section->samples[samp - 1])->cells[l_indCurrentCell[samp]]);
          }    // C,D - Barrel
        }

      }    //for(per)
    }    //for(samp)

  else {//Gap/Crack

    m_DMToCell.push_back(tile_section->samples[0]->cells[0]);
    m_DMToCell.push_back(tile_section->samples[0]->cells[1]);
  }
  
  // check lookup table
  if (m_verboseLevel > 10) {
    if (!gap_crack) {
      for (samp = 0; samp < l_nSample; samp++) {
        for (per = 0; per < l_nPeriod; per++) {
          G4cout << "samp=" << samp
                 << "   per=" << per
                 << " :    cellNum = " << m_DMToCell[l_nPeriod * samp + per]->cellNum
                 << " ,  detector = " << m_DMToCell[l_nPeriod * samp + per]->detector
                 << " ,  sample = " << m_DMToCell[l_nPeriod * samp + per]->sample << G4endl;
        }
      }
    } else {
      for(int i =0; i < static_cast<int>(m_DMToCell.size()); i++) {
        G4cout << "samp = " << i
               << " :    cellNum = " << m_DMToCell[i]->cellNum
               << " ,  detector = " << m_DMToCell[i]->detector
               << " ,  sample = " << m_DMToCell[i]->sample << G4endl;
      }
    }
  }

  delete[] l_indCurrentCell;
  delete[] l_currentBoundary;
}

TileGeoG4Cell* TileGeoG4CalibSection::GetCell(int nPeriod, int nSample) {
  if (nPeriod < 0 || nPeriod > nrOfPeriods - 1 || nSample < 0 || nSample > nrOfSamples - 1) {
    G4cout << "ERROR: GetCell(): Bad index to retrieve pointer to the Cell." << G4endl;
    G4cout << "ERROR: The passed period and sample are these :  nPeriod = " << nPeriod
           << " ,  nSample = " << nSample << G4endl;
    G4cout << "ERROR: Since for this section there are :  number_periods = " << nrOfPeriods
           << " ,  number_samples = " << nrOfSamples << G4endl;

    return 0;
  } else {
    return m_DMToCell[nrOfPeriods*(nSample)+nPeriod];
  }
}

TileGeoG4PlateCell* TileGeoG4CalibSection::GetTilePlateCell(double xHit, double zHit, int plate) {
  int sample = 0;
  int cell_ind = 0;
  
  switch (plate) {
  case 1:  //FrontPlate
    {
      sample = 0;
      for (int counter = 0; counter < static_cast<int>(samples[sample]->plateCells.size()); counter++) {
        if (xHit > (samples[sample]->plateCells[counter])->xBound) {
          cell_ind++;
        } else {
          break;
        }
      }
      break;
    }
  case 2:  //EndPlate
    {
      for (unsigned int samp = 1; samp != samples.size(); ++samp) {
        if (zHit < (samples[samp]->plateCells[0])->zBound) {
          sample = samp;
          break;
        }
      }

      for (int counter = 0; counter < static_cast<int>(samples[sample]->plateCells.size()); counter++) {
        if (xHit > (samples[sample]->plateCells[counter])->xBound) {
          cell_ind++;
        } else {
          break;
        }
      }
      break;
    }
  case 3:  //Iron1,Iron2,Iron3,IrBox
    {
      sample = 0;
      cell_ind = 0;
      break;
    }
  case 4:  //IrUp, IrDw
    {
      sample = 0;
      cell_ind = static_cast<int>(samples[sample]->plateCells.size()) - 1;
      break;
    }
  case 5:  //Saddle except neg. barrel
    {
      sample = samples.size() - 2;
      cell_ind = samples[sample]->plateCells.size() - 1;
      break;
    }
  case 6:  //Saddle neg. barrel
    {
      sample = samples.size() - 2;
      cell_ind = 0;
      break;
    }
  } //switch

  return (samples[sample]->plateCells[cell_ind]);
}

TileGeoG4GirderCell* TileGeoG4CalibSection::GetTileGirderCell(double xHit) {
  int counter;
  int cell_ind = 0;

  for (counter = 0; counter < static_cast<int>(girderCells.size()); counter++) {
    if (xHit > girderCells[counter]->xBound) {
      ++cell_ind;
    } else {
      break;
    }
  }

  return (girderCells[cell_ind]);
}

void TileGeoG4CalibSection::AddModuleToCell(bool negative) {
  int l_nSamp, l_nCell;
  TileGeoG4PlateCell* current_plateCell;
  TileGeoG4GirderCell* current_girderCell;
  TileGeoG4CalibSample* current_sample;

  //Iterate through all samples of the section
  for (l_nSamp = 0; l_nSamp < static_cast<int>(samples.size()); l_nSamp++) {
    current_sample = samples[l_nSamp];

    //Iterate through all cells of the current sample
    for (l_nCell = 0; l_nCell < static_cast<int>(current_sample->plateCells.size()); l_nCell++) {
      current_plateCell = current_sample->plateCells[l_nCell];
      if (negative) {
        current_plateCell->m_nPlateCellHit_negative.push_back(0);
      } else {
        current_plateCell->m_nPlateCellHit.push_back(0);
      }
    }
  }

  for (l_nCell = 0; l_nCell < static_cast<int>(girderCells.size()); l_nCell++) {
    current_girderCell = girderCells[l_nCell];
    if (negative) {
      current_girderCell->m_nGirderCellHit_negative.push_back(0);
    } else {
      current_girderCell->m_nGirderCellHit.push_back(0);
    }
  }

}

