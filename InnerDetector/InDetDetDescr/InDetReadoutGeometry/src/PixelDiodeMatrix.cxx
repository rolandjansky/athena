/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/PixelDiodeMatrix.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include <cassert>

namespace InDetDD {

PixelDiodeMatrix::PixelDiodeMatrix(double phiWidth, double etaWidth)
  : m_phiWidth(phiWidth),
    m_etaWidth(etaWidth),
    m_phiCells(1),
    m_etaCells(1),
    m_direction(phiDir), // Irrelevant
    m_numCells(0),
    m_lowerCell(0),
    m_middleCells(0),
    m_upperCell(0),
    m_singleCell(true)
{}

PixelDiodeMatrix::PixelDiodeMatrix(Direction direction,  // phi or eta
				   const PixelDiodeMatrix * lowerCell,
				   const PixelDiodeMatrix * middleCells,
				   int numCells,
				   const PixelDiodeMatrix * upperCell)
  : m_phiWidth(0),
    m_etaWidth(0),
    m_phiCells(0),
    m_etaCells(0),
    m_direction(direction),
    m_numCells(numCells),
    m_lowerCell(lowerCell),
    m_middleCells(middleCells),
    m_upperCell(upperCell),
    m_singleCell(false)
{
  // middleCells must be non zero.
  assert(m_middleCells);

  // Increment ref counts
  if (m_lowerCell) m_lowerCell->ref();
  if (m_middleCells) m_middleCells->ref();
  if (m_upperCell) m_upperCell->ref();
 
  if (m_direction == phiDir) {
    
    // In eta direction widths must be all the same.
    if (m_middleCells){
      m_etaWidth = m_middleCells->etaWidth();
      m_etaCells = m_middleCells->etaCells();
    }
    // Check lower and upper are consistent
    // TODO.
    
    if (m_lowerCell) {
      m_phiWidth += m_lowerCell->phiWidth();
      m_phiCells += m_lowerCell->phiCells();
    }

    if (m_middleCells){
      m_phiWidth += m_numCells * m_middleCells->phiWidth();
      m_phiCells += m_numCells * m_middleCells->phiCells();
    }
  
    if (m_upperCell) {
      m_phiWidth += m_upperCell->phiWidth();
      m_phiCells += m_upperCell->phiCells();
    }

  } else { // eta Direction

    // In phi direction widths must be all the same.
    if (m_middleCells){
      m_phiWidth = m_middleCells->phiWidth();
      m_phiCells = m_middleCells->phiCells();
    }
    // Check lower and upper are consistent
    // TODO.
    
    if (m_lowerCell) {
      m_etaWidth += m_lowerCell->etaWidth();
      m_etaCells += m_lowerCell->etaCells();
    }

    if (m_middleCells){
      m_etaWidth += m_numCells * m_middleCells->etaWidth();
      m_etaCells += m_numCells * m_middleCells->etaCells();
    }
  
    if (m_upperCell) {
      m_etaWidth += m_upperCell->etaWidth();
      m_etaCells += m_upperCell->etaCells();
    }

  }

}

PixelDiodeMatrix:: ~PixelDiodeMatrix()
{
  // unref the point the pointers.
  if (m_lowerCell) m_lowerCell->unref();
  if (m_middleCells) m_middleCells->unref();
  if (m_upperCell) m_upperCell->unref();
}

const PixelDiodeMatrix * 
PixelDiodeMatrix::cellIdOfPosition(const Amg::Vector2D & relPosition, SiCellId & cellId) const

  /// Description.  
  /// Overview of algoritm:
  /// 
  ///  - Before calling this method on the top level matrix the 2D position is
  ///    calculated relative to the bottom left corner.
  ///  - The method cellIdOfPosition is always called with the position relative to
  ///    bottom left of the cell and the cell id of bottom left child cell. 
  ///    For the top level matrix this is the position (-halfWidth, -halfLength) and a starting 
  ///    cell id of (0,0).
  ///  - It first checks if it is in the lower cell (if it exists). 
  ///  - If not, it determines which cell by dividing the position relative to
  ///    the start of the cell array by the pitch in that direction. 
  ///  - If it is beyond the range of the middle cells it is taken as being in 
  ///    the upper cell.
  ///  - The cell number is added to the cellId that is passed to the method
  ///  - Once the cell is determined the function is called recursively until it
  ///    reaches a single cell. 
  ///

{
  using Trk::distPhi;
  using Trk::distEta;

  if (m_singleCell) {
    return this;
  }

  double relPosDir = 0; // Relative position along m_direction
  int startIndex = 0;
  double pitch = 0;
  int middleCells = 0;

  if (m_direction == phiDir) {

    relPosDir = relPosition[distPhi];
    pitch = m_middleCells->phiWidth();
    middleCells = m_middleCells->phiCells();

    if (m_lowerCell) {
      if (relPosDir < m_lowerCell->phiWidth()) {
	return m_lowerCell->cellIdOfPosition(relPosition, cellId);
      } else {
	relPosDir  -=  m_lowerCell->phiWidth();
	startIndex +=  m_lowerCell->phiCells();
      }
    }
  } else { // etaDir

    relPosDir = relPosition[distEta];
    pitch = m_middleCells->etaWidth();
    middleCells = m_middleCells->etaCells();

    if (m_lowerCell) {
      if (relPosDir < m_lowerCell->etaWidth()) {
	return m_lowerCell->cellIdOfPosition(relPosition, cellId);
      } else {
	relPosDir  -=  m_lowerCell->etaWidth();
	startIndex +=  m_lowerCell->etaCells();
      }
    }
  } 

  
  int index = static_cast<int>((relPosDir) / pitch);

  if (index < 0) index = 0; // Make sure its in range (in case of rounding errors)
  const PixelDiodeMatrix * nextCell = 0;

  if (m_upperCell && (index >= m_numCells)) { 
    // We are in the upper cell. 
    index = m_numCells;
    nextCell = m_upperCell;
  } else {
    // We are in the middle cells
    // Make sure its in range (in case of rounding errors)
    if (index >= m_numCells) index = m_numCells - 1;
    nextCell = m_middleCells;
  }
  

  relPosDir  -= index *  pitch;
  startIndex += index *  middleCells;

  int newPhiIndex = cellId.phiIndex();
  int newEtaIndex = cellId.etaIndex();
  const PixelDiodeMatrix * cell = 0;

  if (m_direction == phiDir) {
    if (nextCell->singleCell()) {
      newPhiIndex += startIndex;
      cell = nextCell;
    } else {
      Amg::Vector2D newRelPos(relPosDir, relPosition[distEta]);
      SiCellId relId(0,0);
      cell = nextCell->cellIdOfPosition(newRelPos, relId);
      newPhiIndex += startIndex + relId.phiIndex();
      newEtaIndex += relId.etaIndex();
    } 
  } else { 
    if (nextCell->singleCell()) {
      newEtaIndex += startIndex;
      cell = nextCell;
    } else {
      Amg::Vector2D newRelPos(relPosition[distPhi], relPosDir);
      SiCellId relId(0,0);
      cell = nextCell->cellIdOfPosition(newRelPos, relId);
      newPhiIndex += relId.phiIndex();
      newEtaIndex += startIndex + relId.etaIndex();
    }
  }

  cellId = SiCellId(newPhiIndex, newEtaIndex);
  return cell;
}

	
	   
const PixelDiodeMatrix * 
PixelDiodeMatrix::positionOfCell(const SiCellId & cellId, Amg::Vector2D & position) const

  /// Description.  
  /// Overview of algoritm:
  ///
  ///  - It starts with the position of the bottom left corner of the cell.
  ///    For the full matrix this is (-halfwidth,-halflength)
  ///  - It first checks if is in the lower cell (if it exists).
  ///  - If not it determines the bottom edge of the cell by multiplying the cell
  ///    number by the pitch.
  ///  - If it is beyond the range of the middle cells it is taken as being in 
  ///    the upper cell.
  ///  - This position is then passed recursively to the same method until it
  ///    reaches a single cell.
  ///  - At the end the 2D position is then transformed to the position
  ///    relative to the module center.

{
  using Trk::distPhi;
  using Trk::distEta;

  if (m_singleCell) {
    position[distPhi] += 0.5*m_phiWidth;
    position[distEta] += 0.5*m_etaWidth;
    return this;
  }

  int relIndex = 0; // Relative index along m_direction
  double pitch = 0;
  int middleCells = 0;
  double startPos = 0;

  if (m_direction == phiDir) {

    relIndex = cellId.phiIndex();
    pitch = m_middleCells->phiWidth();
    middleCells = m_middleCells->phiCells();
    
    if (m_lowerCell) {
      if (relIndex < m_lowerCell->phiCells()) {
	return m_lowerCell->positionOfCell(cellId, position);
      } else {
	relIndex -=  m_lowerCell->phiCells();
	startPos +=  m_lowerCell->phiWidth();
      }
    } 

  } else { // etaDir

    relIndex = cellId.etaIndex();
    pitch = m_middleCells->etaWidth();
    middleCells = m_middleCells->etaCells();

    if (m_lowerCell) {
      if (relIndex < m_lowerCell->etaCells()) {
	return m_lowerCell->positionOfCell(cellId, position);
      } else {
	relIndex -=  m_lowerCell->etaCells();
	startPos +=  m_lowerCell->etaWidth();
      }
    }
  } 

  int index = relIndex / middleCells;
  if (index > m_numCells) index = m_numCells;
  relIndex -= index * middleCells;
  startPos += index * pitch;

  const PixelDiodeMatrix * nextCell = 0;

  if (m_upperCell && (index == m_numCells)) { 
    // We are in the upper cell. 
    nextCell = m_upperCell;
  } else {
    // We are in the middle cells
    nextCell = m_middleCells;
  }

  const PixelDiodeMatrix * cell = 0;
  if (m_direction == phiDir) {
    SiCellId relId(relIndex,cellId.etaIndex());
    position[distPhi] += startPos;
    cell = nextCell->positionOfCell(relId, position);
  } else {
    SiCellId relId(cellId.phiIndex(),relIndex);
    position[distEta] += startPos;
    cell = nextCell->positionOfCell(relId, position);
  }    
  
  return cell;
}

} // end namespace
