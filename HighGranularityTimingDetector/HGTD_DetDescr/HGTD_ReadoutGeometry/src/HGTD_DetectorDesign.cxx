/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_DetectorDesign.h"

#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "TrkSurfaces/RectangleBounds.h"

namespace HGTDGeo {

// Constructor with parameters:
HGTD_DetectorDesign::HGTD_DetectorDesign(const double thickness,
                                         const int circuitsPerColumn,
                                         const int circuitsPerRow,
                                         const int cellColumnsPerCircuit,
                                         const int cellRowsPerCircuit,
                                         const int diodeColumnsPerCircuit,
                                         const int diodeRowsPerCircuit,
                                         const PixelDiodeMatrix * matrix,
                                         InDetDD::CarrierType carrierType,
                                         int readoutSide):
    DetectorDesign(thickness, 
                   true, true, true, // phi,eta,depth axes symmetric
                   carrierType,
                   readoutSide),
    m_diodeMap(matrix),
    m_readoutScheme(circuitsPerColumn,circuitsPerRow,
                    cellColumnsPerCircuit,cellRowsPerCircuit,
                    diodeColumnsPerCircuit,diodeRowsPerCircuit),
    m_bounds(0) {
}

// Destructor:
HGTD_DetectorDesign::~HGTD_DetectorDesign() {
    delete m_bounds;
}

// Returns distance to nearest detector edge 
// +ve = inside
// -ve = outside
void
HGTD_DetectorDesign::distanceToDetectorEdge(const SiLocalPosition & localPosition,
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
double HGTD_DetectorDesign::length() const
{
    return m_diodeMap.length();
}

// Methods to calculate average width of a module
double HGTD_DetectorDesign::width() const
{
    return m_diodeMap.width();
}

// Methods to calculate minimum width of a module
double HGTD_DetectorDesign::minWidth() const
{
    return width();
}

// Methods to calculate maximum width of a module
double HGTD_DetectorDesign::maxWidth() const
{
    return width();
}

// Pitch in phi direction
double 
HGTD_DetectorDesign::phiPitch() const
{
    // Average pitch. 
    return width() / rows();
}

// Pitch in phi direction
double 
HGTD_DetectorDesign::phiPitch(const SiLocalPosition &) const
{
    // Cheat since we know its constant.
    return phiPitch();
}

// Pitch in eta direction
double 
HGTD_DetectorDesign::etaPitch() const
{
    // Average pitch
    return length() / columns();
}

const Trk::SurfaceBounds & 
HGTD_DetectorDesign::bounds() const
{
    // We create on demand as width and length are 0 when PixeModuleDesign first gets
    // created.
    if (!m_bounds) m_bounds = new Trk::RectangleBounds(0.5*width(), 0.5*length());
    return *m_bounds;
}

SiDiodesParameters HGTD_DetectorDesign::parameters(const SiCellId & cellId) const
{
    return m_diodeMap.parameters(cellId);
}

SiLocalPosition HGTD_DetectorDesign::localPositionOfCell(const SiCellId & cellId) const
{
    return m_diodeMap.parameters(cellId).centre(); 
}

int HGTD_DetectorDesign::numberOfConnectedCells(const SiReadoutCellId & readoutId) const
{
    return m_readoutScheme.numberOfConnectedCells(readoutId);
}

SiCellId 
HGTD_DetectorDesign::connectedCell(const SiReadoutCellId & readoutId, int number) const
{
    return m_readoutScheme.connectedCell(readoutId, number);
}

SiCellId 
HGTD_DetectorDesign::gangedCell(const SiCellId & cellId) const
{
    return m_readoutScheme.gangedCell(cellId);  
}

SiReadoutCellId
HGTD_DetectorDesign::readoutIdOfCell(const SiCellId & cellId) const
{
    return m_readoutScheme.readoutIdOfCell(cellId);
}

SiReadoutCellId 
HGTD_DetectorDesign::readoutIdOfPosition(const SiLocalPosition & localPos) const
{
    return m_readoutScheme.readoutIdOfCell(m_diodeMap.cellIdOfPosition(localPos));
}

SiCellId HGTD_DetectorDesign::cellIdOfPosition(const SiLocalPosition & localPosition) const
{
    return m_diodeMap.cellIdOfPosition(localPosition);
}

SiCellId 
HGTD_DetectorDesign::cellIdInRange(const SiCellId & cellId) const
{
    return m_diodeMap.cellIdInRange(cellId);
}

} // namespace HGTDGeo
