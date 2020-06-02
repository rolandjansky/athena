/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// HGTD_DetectorDesign.h, (c) ATLAS Detector software
//////////////////////////////////////////////////////////

#ifndef HGTD_READOUTGEOMETRY_HGTD_DETECTORDESIGN_H
#define HGTD_READOUTGEOMETRY_HGTD_DETECTORDESIGN_H

// base class
#include "ReadoutGeometryBase/DetectorDesign.h"

// Data member classes
#include "PixelReadoutGeometry/PixelDiodeMap.h"
#include "PixelReadoutGeometry/PixelReadoutScheme.h"

// Other includes
#include "ReadoutGeometryBase/InDetDD_Defs.h"

namespace Trk{
    class RectangleBounds;
    class SurfaceBounds;
}

namespace InDetDD {
    class SiLocalPosition;
    class PixelDiodeMatrix;
}

namespace HGTDGeo {

/**

    @class HGTD_DetectorDesign

    TODO: fill in class description

*/

class HGTD_DetectorDesign: public DetectorDesign {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    ///////////////////////////////////////////////////////////////////
    // Constructors/destructor:
    ///////////////////////////////////////////////////////////////////

    /** Constructor
        @param thickness: Thickness of sensor
        @param circuitsPerColumn: Number of circuits in one circuit column
        @param circuitsPerRow: Number of circuits in one circuit row
        @param cellColumnsPerCircuit: Number of cell columns per circuit
        @param cellRowsPerCircuit: Number of cell rows per circuit
        @param diodeColumnsPerCircuit: Number of diode columns connected to one circuit
        @param diodeRowsPerCircuit: Number of diode rows connected to one circuit
        @param matrix: PixelDiodeMatrix, describing segmentation and conversion between cell id and position
        @param carrierType: Carrier type, either holes or electrons
        @param readoutSide: Readout side, +ve = positive Depth Side, -ve = negative depth side
    */
    HGTD_DetectorDesign(double thickness,
                        const int circuitsPerColumn,
                        const int circuitsPerRow,
                        const int cellColumnsPerCircuit,
                        const int cellRowsPerCircuit,
                        const int diodeColumnsPerCircuit,
                        const int diodeRowsPerCircuit,
                        const PixelDiodeMatrix * matrix,
                        InDetDD::CarrierType carrierType,
                        int readoutSide);

    // Destructor:
    virtual ~HGTD_DetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    // TODO: a number of other methods possible if using PixelDiodeMatrix and PixelReadoutScheme, see PixelModuleDesign class

    /** Returns distance to nearest detector active edge 
     +ve = inside
     -ve = outside */
    virtual void distanceToDetectorEdge(const SiLocalPosition & localPosition, 
                            double & etaDist, double & phiDist) const;

    /** Global sensor size: */
    double sensorLeftColumn() const;
    double sensorRightColumn() const;
    double sensorLeftRow() const;
    double sensorRightRow() const;

    /** Total number of diodes: */
    int numberOfDiodes() const;

    /** Total number of circuits:*/
    int numberOfCircuits() const;

    /** Number of cell columns per circuit:*/
    int columnsPerCircuit() const;

    /** Number of cell rows per circuit:*/
    int rowsPerCircuit() const;

    /** Number of cell columns per module:*/
    int columns() const;

    /** Number of cell rows per module:*/
    int rows() const;

    /** Method to calculate length of a module*/
    virtual double length() const;

    /** Method to calculate average width of a module*/
    virtual double width() const;

    /** Method to calculate minimum width of a module*/
    virtual double minWidth() const;
 
    /** Method to calculate maximum width of a module*/
    virtual double maxWidth() const;

    /** Pitch in phi direction*/
    virtual double phiPitch() const;

    /** Pitch in phi direction*/
    virtual double phiPitch(const SiLocalPosition & localPosition) const;

    /** Pitch in eta direction*/
    virtual double etaPitch() const;

    /** Return true if hit local direction is the same as readout direction.*/
    virtual bool swapHitPhiReadoutDirection() const;
    virtual bool swapHitEtaReadoutDirection() const;

    /** Element boundary*/
    virtual const Trk::SurfaceBounds & bounds() const;

    /** readout or diode id -> position, size */
    virtual SiDiodesParameters parameters(const SiCellId & cellId) const;
    virtual SiLocalPosition localPositionOfCell(const SiCellId & cellId) const;

    /** readout id -> id of connected diodes */
    virtual int numberOfConnectedCells(const SiReadoutCellId & readoutId) const;
    virtual SiCellId connectedCell(const SiReadoutCellId & readoutId, int number) const;
    
    /** If cell is ganged return the other cell, otherwise return an invalid id. */
    virtual SiCellId gangedCell(const SiCellId & cellId) const;

    /** diode id -> readout id */
    virtual SiReadoutCellId readoutIdOfCell(const SiCellId & cellId) const;
    
    /** position -> id */
    virtual SiReadoutCellId readoutIdOfPosition(const SiLocalPosition & localPos) const;
    virtual SiCellId cellIdOfPosition(const SiLocalPosition & localPos) const;

    /** Get the neighbouring diodes of a given diode:
          Cell for which the neighbours must be found
          List of cells which are neighbours of the given one */
    virtual void neighboursOfCell(const SiCellId & cellId, std::vector<SiCellId> &neighbours) const;

    /** Compute the intersection length of two diodes:
          return: the intersection length when the two diodes are projected on one
           of the two axis, or 0 in case of no intersection or problem
          input: the two diodes for which the intersection length is computed */
    double intersectionLength(const SiCellId &diode1, const SiCellId &diode2) const;

    /** Check if cell is in range. Returns the original cellId if it is in range, otherwise it returns an invalid id. */
    virtual SiCellId cellIdInRange(const SiCellId & cellId) const;

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    HGTD_DetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    // TODO: replace PixelDiodeMap and PixelReadoutScheme with HGTD equivalents?
    PixelDiodeMap m_diodeMap;
    PixelReadoutScheme m_readoutScheme;
    mutable Trk::RectangleBounds * m_bounds;

    // Disallow Copy and assignment;
    HGTD_DetectorDesign(const HGTD_DetectorDesign &design);
    HGTD_DetectorDesign &operator = (const HGTD_DetectorDesign &design);

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline double HGTD_DetectorDesign::sensorLeftColumn() const
{
    return m_diodeMap.leftColumn();
}

inline double HGTD_DetectorDesign::sensorRightColumn() const
{
    return m_diodeMap.rightColumn();
}

inline double HGTD_DetectorDesign::sensorLeftRow() const
{
    return m_diodeMap.leftRow();
}

inline double HGTD_DetectorDesign::sensorRightRow() const
{
    return m_diodeMap.rightRow();
}

inline int HGTD_DetectorDesign::numberOfDiodes() const
{
    return m_diodeMap.diodes();
}

inline int HGTD_DetectorDesign::numberOfCircuits() const
{
    return m_readoutScheme.numberOfCircuits();
}

inline int HGTD_DetectorDesign::columnsPerCircuit() const
{
    return m_readoutScheme.columnsPerCircuit();
}

inline int HGTD_DetectorDesign::rowsPerCircuit() const
{
    return m_readoutScheme.rowsPerCircuit();
}

inline int HGTD_DetectorDesign::columns() const
{
    return m_readoutScheme.columns();
}

inline int HGTD_DetectorDesign::rows() const
{
    return m_readoutScheme.rows();
}

inline bool HGTD_DetectorDesign::swapHitPhiReadoutDirection() const
{
    return true;
}

inline bool HGTD_DetectorDesign::swapHitEtaReadoutDirection() const
{
    return true;
}

inline void HGTD_DetectorDesign::neighboursOfCell(const SiCellId & cellId,
                                std::vector<SiCellId> &neighbours) const
{
    return m_diodeMap.neighboursOfCell(cellId, neighbours);
}

inline double HGTD_DetectorDesign::intersectionLength(const SiCellId &diode1,
                                const SiCellId &diode2) const
{
    return m_diodeMap.intersectionLength(diode1, diode2);
}

} // namespace HGTDGeo

#endif // HGTD_READOUTGEOMETRY_HGTD_DETECTORDESIGN_H
