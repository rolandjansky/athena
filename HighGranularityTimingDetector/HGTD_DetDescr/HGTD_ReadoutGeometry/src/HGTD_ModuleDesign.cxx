/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_ModuleDesign.h"

#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "TrkSurfaces/RectangleBounds.h"

namespace InDetDD {

// Constructor with parameters:
HGTD_ModuleDesign::HGTD_ModuleDesign(const double thickness,
                                     const int circuitsPerColumn,
                                     const int circuitsPerRow,
                                     const int cellColumnsPerCircuit,
                                     const int cellRowsPerCircuit,
                                     const int diodeColumnsPerCircuit,
                                     const int diodeRowsPerCircuit,
                                     const PixelDiodeMatrix * matrix,
                                     InDetDD::CarrierType carrierType,
                                     int readoutSide,
                                     DetectorDesign::Axis yDirection,
                                     DetectorDesign::Axis depthDirection):
    DetectorDesign(thickness, 
                   true, true, true, // phi,eta,depth axes symmetric
                   carrierType,
                   readoutSide,
                   yDirection,
                   depthDirection),
    m_diodeMap(matrix),
    m_readoutScheme(circuitsPerColumn,circuitsPerRow,
                    cellColumnsPerCircuit,cellRowsPerCircuit,
                    diodeColumnsPerCircuit,diodeRowsPerCircuit),
    m_bounds(0) {
}

// Destructor:
HGTD_ModuleDesign::~HGTD_ModuleDesign() {
    delete m_bounds;
}

// Returns distance to nearest detector edge 
// +ve = inside
// -ve = outside
void
HGTD_ModuleDesign::distanceToDetectorEdge(const SiLocalPosition & localPosition,
                                          double & etaDist, double & phiDist) const
{ 
    // This assume element is centered at 0,0 
    // As the calculation is symmetric around 0,0 we only have to test it for one side.
    double xEta = abs(localPosition.xEta());
    double xPhi = abs(localPosition.xPhi());

    double xEtaEdge = 0.5*length();
    double xPhiEdge = 0.5*width();

    // Distance to top/bottom
    etaDist = xEtaEdge - xEta;
  
    // Distance to right/left edge
    phiDist = xPhiEdge - xPhi;
}

// Methods to calculate length of a module
double HGTD_ModuleDesign::length() const
{
    return m_diodeMap.length();
}

// Methods to calculate average width of a module
double HGTD_ModuleDesign::width() const
{
    return m_diodeMap.width();
}

// Methods to calculate minimum width of a module
double HGTD_ModuleDesign::minWidth() const
{
    return width();
}

// Methods to calculate maximum width of a module
double HGTD_ModuleDesign::maxWidth() const
{
    return width();
}

// Method to calculate eta width from a column range
double HGTD_ModuleDesign::widthFromColumnRange(const int colMin, const int colMax) const
{
    SiCellId idMin(0, colMin);
    SiCellId idMax(0, colMax);
    double minEta = parameters(idMin).xEtaMin();
    double maxEta = parameters(idMax).xEtaMax();

    return fabs(maxEta-minEta);
}

// Method to calculate phi width from a row range
double HGTD_ModuleDesign::widthFromRowRange(const int rowMin, const int rowMax) const
{
    SiCellId idMin(rowMin, 0);
    SiCellId idMax(rowMax, 0);
    double minPhi = parameters(idMin).xPhiMin();
    double maxPhi = parameters(idMax).xPhiMax();

    return fabs(maxPhi-minPhi);
}

// Pitch in phi direction
double
HGTD_ModuleDesign::phiPitch() const
{
    // Average pitch. 
    return width() / rows();
}

// Pitch in phi direction
double
HGTD_ModuleDesign::phiPitch(const SiLocalPosition &) const
{
    // Cheat since we know its constant.
    return phiPitch();
}

// Pitch in eta direction
double
HGTD_ModuleDesign::etaPitch() const
{
    // Average pitch
    return length() / columns();
}

const Trk::SurfaceBounds & 
HGTD_ModuleDesign::bounds() const
{
    // We create on demand as width and length are 0 when HGTD_ModuleDesign first gets
    // created.
    if (!m_bounds) m_bounds = new Trk::RectangleBounds(0.5*width(), 0.5*length());
    return *m_bounds;
}

SiDiodesParameters HGTD_ModuleDesign::parameters(const SiCellId & cellId) const
{
    return m_diodeMap.parameters(cellId);
}

SiLocalPosition HGTD_ModuleDesign::localPositionOfCell(const SiCellId & cellId) const
{
    return m_diodeMap.parameters(cellId).centre(); 
}

int HGTD_ModuleDesign::numberOfConnectedCells(const SiReadoutCellId & readoutId) const
{
    return m_readoutScheme.numberOfConnectedCells(readoutId);
}

SiCellId
HGTD_ModuleDesign::connectedCell(const SiReadoutCellId & readoutId, int number) const
{
    return m_readoutScheme.connectedCell(readoutId, number);
}

SiReadoutCellId
HGTD_ModuleDesign::readoutIdOfCell(const SiCellId & cellId) const
{
    return m_readoutScheme.readoutIdOfCell(cellId);
}

SiReadoutCellId
HGTD_ModuleDesign::readoutIdOfPosition(const SiLocalPosition & localPos) const
{
    return m_readoutScheme.readoutIdOfCell(m_diodeMap.cellIdOfPosition(localPos));
}

SiCellId HGTD_ModuleDesign::cellIdOfPosition(const SiLocalPosition & localPosition) const
{
    return m_diodeMap.cellIdOfPosition(localPosition);
}

SiCellId
HGTD_ModuleDesign::cellIdInRange(const SiCellId & cellId) const
{
    return m_diodeMap.cellIdInRange(cellId);
}

} // namespace InDetDD
