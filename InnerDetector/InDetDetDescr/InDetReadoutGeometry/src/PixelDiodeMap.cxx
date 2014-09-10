/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDiodeMap.cxx
//   Implementation file for class PixelDiodeMap
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// Version 4.2 14/08/2001 David Calvet
// Modified: Grant Gorfine
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/PixelDiodeMap.h"
#include "InDetReadoutGeometry/PixelDiodeMatrix.h"
#include "InDetReadoutGeometry/SiCellId.h"

#include <cmath>
#include <algorithm>

namespace InDetDD {



// Implicit constructor:
PixelDiodeMap::PixelDiodeMap(const PixelDiodeMatrix * matrix) :
  m_matrix(matrix),
  m_generalLayout(false) 
{
  m_matrix->ref();
}

PixelDiodeMap::~PixelDiodeMap()
{
  m_matrix->unref();
}


  SiCellId PixelDiodeMap::cellIdOfPosition(const Amg::Vector2D & localPos) const
{
  using std::abs;
  using Trk::distPhi;
  using Trk::distEta;

  // Check that we are in the bounds of the top level matrix
  // NB. edge is included in bounds.

  double halfWidth = 0.5*width();
  double halfLength = 0.5*length();
  if ( (abs(localPos[distPhi]) > halfWidth) || 
       (abs(localPos[distEta]) > halfLength) ) {
    return SiCellId(); // Invalid Id.
  }

  // position relative to bottom left corner.
  Amg::Vector2D relativePos = localPos + Amg::Vector2D(halfWidth, halfLength);

  // The cellId returned will be added to this so we must start with 0,0.
  SiCellId cellId(0,0);

  const PixelDiodeMatrix * cell = m_matrix->cellIdOfPosition(relativePos, cellId);

  // return invalid Id if there was a problem (don't expect this to be the case).
  if (!cell) {
    return SiCellId(); // Invalid id.
  } 

  return cellId;
  
}

  
// Get diodes parameters (position and size):
SiDiodesParameters
PixelDiodeMap::parameters(const SiCellId & cellId) const
{

  // Check we are in range
  
  if (!cellId.isValid() || 
      (cellId.phiIndex() < 0) || 
      (cellId.phiIndex() >= m_matrix->phiCells()) ||
      (cellId.etaIndex() < 0) ||
      (cellId.etaIndex() >= m_matrix->etaCells())) {
    return SiDiodesParameters();
  }

  double halfWidth = 0.5*width();
  double halfLength = 0.5*length();
  
  //
  // Position is relative to left bottom corner.
  //
  Amg::Vector2D position(-halfWidth, -halfLength);
  const PixelDiodeMatrix * cell = m_matrix->positionOfCell(cellId, position);
  
  if (cell) {
    
    // get size
    Amg::Vector2D size(cell->phiWidth(), cell->etaWidth());

    // return parameters
    return SiDiodesParameters(position,size);
  }

  // return something in case of failure.
  return SiDiodesParameters();
}

void PixelDiodeMap::neighboursOfCell(const SiCellId & cellId,
				     std::vector<SiCellId> &neighbours) const
{
  neighbours.clear();

  if (!cellId.isValid()) return;

  // If non regular layout revert to slower method
  if (m_generalLayout) return neighboursOfCellGeneral(cellId, neighbours);

  neighbours.reserve(8);
  // neighbours easily determined from cell number
  // normally 8 neighbours 4 edge and 4 corners
  
  int phiIndex = cellId.phiIndex();
  int etaIndex = cellId.etaIndex();

  // M = minus
  // P = plus
  int phiM = phiIndex-1;
  int phiP = phiIndex+1;
  int etaM = etaIndex-1;
  int etaP = etaIndex+1;

  // -,0
  if (phiM >= 0)                                 neighbours.push_back(SiCellId(phiM,etaIndex));
  // -,-
  if (phiM >= 0 && etaM >= 0)                    neighbours.push_back(SiCellId(phiM,etaM));
  // 0,-
  if (etaM >= 0)                                 neighbours.push_back(SiCellId(phiIndex,etaM));
  // +,-
  if (phiP < phiDiodes() && etaM >= 0)           neighbours.push_back(SiCellId(phiP,etaM));
  // +,0
  if (phiP < phiDiodes())                        neighbours.push_back(SiCellId(phiP,etaIndex));
  // -,+
  if (phiM >= 0 && etaP < etaDiodes())           neighbours.push_back(SiCellId(phiM,etaP));
  // 0,+
  if (etaP < etaDiodes())                        neighbours.push_back(SiCellId(phiIndex,etaP));
  // +,+
  if (phiP < phiDiodes() && etaP < etaDiodes())  neighbours.push_back(SiCellId(phiP,etaP));
}

// Get the neighbouring PixelDiodes of a given PixelDiode:
// This will work for more complex layouts such as bricking. Probably never really needed but
// since the code was here I keep it available.
void PixelDiodeMap::neighboursOfCellGeneral(const SiCellId & cellId,
					    std::vector<SiCellId> &neighbours) const
{
  // extract the diode spatial parameters
  const SiDiodesParameters params=parameters(cellId);
  const SiLocalPosition diodeCenter=params.centre();
  const SiLocalPosition diodeSize=params.width();
  const double &centerColumn=diodeCenter.xColumn();
  const double &centerRow=diodeCenter.xRow();
  const double halfSizeColumn=diodeSize.xColumn()/2;
  const double halfSizeRow=diodeSize.xRow()/2;

  // parameter
  const double epsilon=0.01;

  // compute the points to check
  const double left1=centerColumn-halfSizeColumn*(1+epsilon);
  const double right1=centerColumn+halfSizeColumn*(1+epsilon);
  const double left2=centerColumn-halfSizeColumn*(1-epsilon);
  const double right2=centerColumn+halfSizeColumn*(1-epsilon);
  const double top1=centerRow+halfSizeRow*(1+epsilon);
  const double bot1=centerRow-halfSizeRow*(1+epsilon);
  const double top2=centerRow+halfSizeRow*(1-epsilon);
  const double bot2=centerRow-halfSizeRow*(1-epsilon);

  // build the list of positions to check
  std::vector<SiLocalPosition> positions;
  positions.reserve(12);
  SiLocalPosition position;
  position.xRow(bot1); position.xColumn(left2); positions.push_back(position);
  position.xRow(bot1); position.xColumn(left1); positions.push_back(position);
  position.xRow(bot2); position.xColumn(left1); positions.push_back(position);
  position.xRow(top2); position.xColumn(left1); positions.push_back(position);
  position.xRow(top1); position.xColumn(left1); positions.push_back(position);
  position.xRow(top1); position.xColumn(left2); positions.push_back(position);
  position.xRow(bot1); position.xColumn(right2); positions.push_back(position);
  position.xRow(bot1); position.xColumn(right1); positions.push_back(position);
  position.xRow(bot2); position.xColumn(right1); positions.push_back(position);
  position.xRow(top2); position.xColumn(right1); positions.push_back(position);
  position.xRow(top1); position.xColumn(right1); positions.push_back(position);
  position.xRow(top1); position.xColumn(right2); positions.push_back(position);

  // build the list of neighbours
  neighbours.reserve(8);

  // loop on all positions to check
  for(std::vector<SiLocalPosition>::const_iterator p_position=positions.begin() ;
      p_position!=positions.end() ; ++p_position) {

    // get the PixelDiode for this position
    SiCellId cellId_neighb = cellIdOfPosition(*p_position);

    if (cellId.isValid()) {
      // check if the diode is already in the list
      //bool found=false;
      std::vector<SiCellId>::const_iterator foundIter
	= std::find(neighbours.begin(), neighbours.end(), cellId_neighb );

      // If not found add this diode to the list
      if (foundIter ==  neighbours.end()) neighbours.push_back(cellId_neighb);
		    
    } 
  }
}


// Compute the intersection length of two diodes:
double PixelDiodeMap::intersectionLength(const SiCellId &diode1,
					 const SiCellId &diode2) const
{ 
  if(!diode1.isValid() || !diode2.isValid()) return 0;
  // If non regular layout revert to slower method
  if (m_generalLayout) return intersectionLengthGeneral(diode1, diode2);

  const SiLocalPosition size = parameters(diode1).width();

  int phiIndexDelta = std::abs(diode1.phiIndex() - diode2.phiIndex());
  int etaIndexDelta = std::abs(diode1.etaIndex() - diode2.etaIndex());

  // Intersection length is just the length or width of the diode depending on which neighbour.
  if (phiIndexDelta == 1 && etaIndexDelta == 0) return size.xEta();
  if (phiIndexDelta == 0 && etaIndexDelta == 1) return size.xPhi();
  // Will return 0 if it is a corner neighbour or if its not a neighbour or if they are oth the same diode.
  return 0;
}

// Compute the intersection length of two diodes:
// This will work for more complex layouts such as bricking. Probably never really needed but
// since the code was here I keep it available.
double PixelDiodeMap::intersectionLengthGeneral(const SiCellId &diode1,
						const SiCellId &diode2) const

{
  const SiDiodesParameters params1=parameters(diode1);
  const SiDiodesParameters params2=parameters(diode2);
  const SiLocalPosition center1=params1.centre();
  const SiLocalPosition center2=params2.centre();
  const SiLocalPosition size1=params1.width();
  const SiLocalPosition size2=params2.width();
  
  // compute intersection length on column direction
  const double intersectionColumn=intersectionLength1D(center1.xColumn(),
					  size1.xColumn(),
					  center2.xColumn(),
					  size2.xColumn());
  // compute intersection length on row direction
  const double intersectionRow=intersectionLength1D(center1.xRow(),
				       size1.xRow(),
				       center2.xRow(),
				       size2.xRow());

  // return the real intersection 
  // (if both directions intersect, there is a problem)
  if (intersectionColumn>0) {
    if (intersectionRow>0) return 0;
    return intersectionColumn;
  } else {
    return intersectionRow;
  }
}

// Compute the intersection length along one direction:
double PixelDiodeMap::intersectionLength1D(const double x1,const double dx1,
					   const double x2,const double dx2) const
{
  // compute distance between the two centers
  double distance=std::abs(x1-x2);

  // compute theoretical intersection
  double intersection=(dx1+dx2)/2-distance;

  // if intersection if negative, no intersection
  if (intersection<-1e-10) return intersection;
  else if (intersection<1e-10) return 0;
  else {
    // intersection cannot exceed size
    if (intersection>dx1) intersection=dx1;
    if (intersection>dx2) intersection=dx2;
    return intersection;
  }
}

} // namespace InDetDD

