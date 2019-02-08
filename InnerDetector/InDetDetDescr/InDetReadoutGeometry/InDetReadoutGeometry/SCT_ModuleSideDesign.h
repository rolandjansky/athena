/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ModuleSideDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef INDETREADOUTGEOMETRY_SCT_MODULESIDEDESIGN_H
#define INDETREADOUTGEOMETRY_SCT_MODULESIDEDESIGN_H

// Base class
#include "InDetReadoutGeometry/SiDetectorDesign.h"

// Data member class
#include "InDetReadoutGeometry/SCT_ReadoutScheme.h"

// Other includes
#include "InDetIdentifier/SCT_ID.h"

namespace InDetDD {
/** @class SCT_ModuleSideDesign

   Base class for the SCT module side design,
   extended by the Forward and Barrel module design

    @author Alessandro Fornaini, Grant Gorfine
 */

class SCT_ModuleSideDesign: public SiDetectorDesign {
public:
    /** Constructor with parameters:
       local axis corresponding to eta direction
       local axis corresponding to phi direction
       local axis corresponding to depth direction
       thickness of silicon sensor
       number of crystals within module side
       number of diodes within crystal
       number of cells within module side
       index of diode connected to cell with index 0 */
    SCT_ModuleSideDesign(const double thickness,
                         const bool phiSymmetric,
                         const bool etaSymmetric,
                         const bool depthSymmetric,
                         const int crystals,
                         const int diodes,
                         const int cells,
                         const int shift,
                         const bool swapStripReadout,
                         InDetDD::CarrierType carrierType,
                         int readoutSide);

    /** Constructor with parameters with access to enum Axis:
       local axis corresponding to eta direction
       local axis corresponding to phi direction
       local axis corresponding to depth direction
       thickness of silicon sensor
       number of crystals within module side
       number of diodes within crystal
       number of cells within module side
       index of diode connected to cell with index 0 */
    SCT_ModuleSideDesign(const double thickness,
                         const bool phiSymmetric,
                         const bool etaSymmetric,
                         const bool depthSymmetric,
                         const int crystals,
                         const int diodes,
                         const int cells,
                         const int shift,
                         const bool swapStripReadout,
                         InDetDD::CarrierType carrierType,
                         int readoutSide,
                         const SiDetectorDesign::Axis stripDirection,
                         const SiDetectorDesign::Axis thicknessDirection);

    /** Destructor: */
    virtual ~SCT_ModuleSideDesign() = default;

    /** readout id -> id of connected diodes
        Not particularly useful for the SCT but implemented to keep a uniform interface.
           */
    virtual int numberOfConnectedCells(const SiReadoutCellId &readoutId) const;
    virtual SiCellId connectedCell(const SiReadoutCellId &readoutId, int number) const;

    /** If cell is ganged return the other cell, otherwise return an invalid id.
        Not particularly useful for the SCT but implemented to keep a uniform interface.
        For SCT always returns an invalid id. */
    virtual SiCellId gangedCell(const SiCellId &cellId) const;

    /** diode id -> readout id */
    virtual SiReadoutCellId readoutIdOfCell(const SiCellId &cellId) const;

    /** position -> id */
    virtual SiReadoutCellId readoutIdOfPosition(const SiLocalPosition &localPos) const;

    /** id -> position */
    virtual SiLocalPosition localPositionOfCell(const SiCellId &cellId) const = 0;
    virtual SiLocalPosition localPositionOfCluster(const SiCellId &cellId,
                                                   int cluserSize) const = 0;

    /** Check if cell is in range. Returns the original cellId if it is in range,
      otherwise it returns an invalid id. */
    virtual SiCellId cellIdInRange(const SiCellId &cellId) const;

    /** Get the neighbouring diodes of a given diode:
        Cell for which the neighbours must be found
         List of cells which are neighbours of the given one */
    virtual void neighboursOfCell(const SiCellId &cellId,
                                  std::vector<SiCellId> &neighbours) const;

    /** check if the position is in active area */
    virtual bool inActiveArea(const SiLocalPosition &chargePos, bool checkBondGap =
                                  true) const = 0;

    /** give distance to the nearest diode in units of pitch, from 0.0 to 0.5,
       this method should be fast as it is called for every surface charge
       in the SCT_SurfaceChargesGenerator */
    virtual double scaledDistanceToNearestDiode(const SiLocalPosition &chargePos) const =
        0;

    /** give the strip pitch (dependence on position needed for forward)  */
    virtual double stripPitch(const SiLocalPosition &chargePos) const = 0;

    /** give the strip pitch (For Forward returns pitch at center) */
    virtual double stripPitch() const = 0;

    /** give the ends of strips */
    virtual std::pair<SiLocalPosition, SiLocalPosition>
    endsOfStrip(const SiLocalPosition &position) const = 0;

    /** gives position of strip center
       ALTERNATIVE/PREFERED way is to use localPositionOfCell(const SiCellId & cellId) or
       rawLocalPositionOfCell method in SiDetectorElement.
       DEPRECATED */
    virtual SiLocalPosition positionFromStrip(const int stripNumber) const;

    /** give length of dead area */
    virtual double deadAreaLength() const = 0;

    /** give upper boundary of dead area */
    virtual double deadAreaUpperBoundary() const = 0;

    /** give lower boundary of dead area */
    virtual double deadAreaLowerBoundary() const = 0;

    /** number of crystals within module side: */
    int crystals() const;

    /** number of strips within crystal: */
    int diodes() const;
    virtual int diodesInRow(const int row) const; // Introduced for upgrade sensors which have several rows

    /** number of readout stips within module side: */
    int cells() const;

    /** number of edge strips before first readout strip. */
    int shift() const;

    /** Return true if hit local direction is the same as readout direction. */
    virtual bool swapHitPhiReadoutDirection() const;
    virtual bool swapHitEtaReadoutDirection() const;

    virtual int row(int stripId1Dim) const;    // For SCT, is 0; otherwise takes a 1-dim strip ID and returns its row
    virtual int strip(int stripId1Dim) const;  // For SCT, returns stripId1Dim; otherwise returns strip within row.
    virtual int strip1Dim(int strip, int row) const;   // For SCT, returns strip. Else inverse of above two.
private:
    SCT_ModuleSideDesign();

    // Copy constructor:
    SCT_ModuleSideDesign(const SCT_ModuleSideDesign &design);

    // Assignment operator:
    SCT_ModuleSideDesign &operator = (const SCT_ModuleSideDesign &design);

protected:
    SCT_ReadoutScheme m_scheme; // !< connection between diodes and readout cells

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    bool m_swapStripReadout;    // !< Flag to indicate if readout direction is opposite
                                // !< to hit local phi direction
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline int SCT_ModuleSideDesign::crystals() const {
    return m_scheme.crystals();
}

inline int SCT_ModuleSideDesign::diodes() const {
    return m_scheme.diodes();
}

inline int SCT_ModuleSideDesign::diodesInRow(const int /* row */) const {
    return m_scheme.diodes();
}

inline int SCT_ModuleSideDesign::cells() const {
    return m_scheme.cells();
}

inline int SCT_ModuleSideDesign::shift() const {
    return m_scheme.shift();
}

inline int SCT_ModuleSideDesign::numberOfConnectedCells(const SiReadoutCellId &readoutId)
const {
    return m_scheme.numberOfConnectedCells(readoutId);
}

inline SiCellId SCT_ModuleSideDesign::connectedCell(const SiReadoutCellId &readoutId,
                                                    int number) const {
    return m_scheme.connectedCell(readoutId, number);
}

inline SiCellId SCT_ModuleSideDesign::gangedCell(const SiCellId &) const {
    // No ganged cells in SCT so always return invalid id.
    return SiCellId();
}

inline SiReadoutCellId SCT_ModuleSideDesign::readoutIdOfCell(const SiCellId &cellId) const
{
    return m_scheme.readoutIdOfCell(cellId);
}

inline SiReadoutCellId SCT_ModuleSideDesign::readoutIdOfPosition(
    const SiLocalPosition &localPos) const {
    // Get the cellId then convert it to a readoutId.
    // NEED to think about active area and bond gap check
    return readoutIdOfCell(cellIdOfPosition(localPos));
}

inline bool SCT_ModuleSideDesign::swapHitPhiReadoutDirection() const {
    return m_swapStripReadout;
}

inline bool SCT_ModuleSideDesign::swapHitEtaReadoutDirection() const {
    return false;
}

inline int SCT_ModuleSideDesign::row(int /*stripId1Dim not used */) const {
    return -1; // Use -1 to realise this is an SCT module without rows
}

inline int SCT_ModuleSideDesign::strip(int stripId1Dim) const {
    return stripId1Dim;
}

inline int SCT_ModuleSideDesign::strip1Dim(int strip, int /*row not used */) const {
    return strip;
}
} // namespace InDetDD
#endif // INDETREADOUTGEOMETRY_SCT_MODULESIDEDESIGN_H
