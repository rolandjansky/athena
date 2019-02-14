/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ModuleSideDesign.cxx
//   Implementation file for class SCT_ModuleSideDesign
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.2 03/7/2001 Alessandro Fornaini
// Modified: Grant Gorfine
///////////////////////////////////////////////////////////////////

#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "Identifier/Identifier.h"

namespace InDetDD {
// Constructor with parameters:
//   local axis corresponding to eta direction
//   local axis corresponding to phi direction
//   local axis corresponding to depth direction
//   thickness of silicon sensor
//   number of crystals within module side
//   number of diodes within crystal
//   number of cells within module side
//   index of diode connected to cell with index 0


SCT_ModuleSideDesign::SCT_ModuleSideDesign(const double thickness,
                                           const bool phiSymmetric,
                                           const bool etaSymmetric,
                                           const bool depthSymmetric,
                                           const int crystals,
                                           const int diodes,
                                           const int cells,
                                           const int shift,
                                           const bool swapStripReadout,
                                           InDetDD::CarrierType carrierType,
                                           int readoutSide):
    SiDetectorDesign(thickness, phiSymmetric, etaSymmetric, depthSymmetric, carrierType, readoutSide),
    m_scheme(crystals, diodes, cells, shift),
    m_swapStripReadout(swapStripReadout) {
}

SCT_ModuleSideDesign::SCT_ModuleSideDesign(const double thickness,
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
                                           const InDetDD::SiDetectorDesign::Axis stripDirection,
                                           const InDetDD::SiDetectorDesign::Axis thicknessDirection)
    :
    SiDetectorDesign(thickness, phiSymmetric, etaSymmetric, depthSymmetric, carrierType, readoutSide,
                     stripDirection, thicknessDirection),
    m_scheme(crystals, diodes, cells, shift),
    m_swapStripReadout(swapStripReadout) {
}

void SCT_ModuleSideDesign::neighboursOfCell(const SiCellId &cellId,
                                            std::vector<SiCellId> &neighbours) const {
    neighbours.clear();
    neighbours.reserve(2);

    if (!cellId.isValid()) {
        return;
    }

    int strip = cellId.strip();
    int stripM = strip - 1;
    int stripP = strip + 1;

    if (stripM >= m_scheme.shift()) {
        neighbours.push_back(stripM);
    }
    if (stripP < m_scheme.diodes() + m_scheme.shift()) {
        neighbours.push_back(stripP);
    }
}

// This method returns the position of the centre of a strip
// ALTERNATIVE/PREFERED way is to use localPositionOfCell(const SiCellId & cellId) or
// rawLocalPositionOfCell method in SiDetectorElement.
// DEPRECATED (but used in numerous places)
SiLocalPosition SCT_ModuleSideDesign::positionFromStrip(const int stripNumber) const {
    return localPositionOfCell(SiCellId(stripNumber));
}

/// Check if cell is in range. Returns the original cellId if it is in range, otherwise it
// returns an invalid id.
SiCellId SCT_ModuleSideDesign::cellIdInRange(const SiCellId &cellId) const {
    if (!cellId.isValid() ||
        cellId.strip() < 0 || cellId.strip() >= cells()) {
        return SiCellId(); // Invalid
    }
    return cellId;
}
} // namespace InDetDD
