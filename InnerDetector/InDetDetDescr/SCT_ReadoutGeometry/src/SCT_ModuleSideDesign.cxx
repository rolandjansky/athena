/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"
#include "Identifier/Identifier.h"
#include "ReadoutGeometryBase/SiIntersect.h"

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
        neighbours.emplace_back(stripM);
    }
    if (stripP < m_scheme.diodes() + m_scheme.shift()) {
        neighbours.emplace_back(stripP);
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
        return {}; // Invalid
    }
    return cellId;
}

  void SCT_ModuleSideDesign::setMother(SCT_ModuleSideDesign * mother){
    if(m_motherDesign){
      const std::string errMsg=std::string("SCT_ModuleSideDesign already has a mother set!");
	throw std::runtime_error(errMsg);
    } 
    m_motherDesign = mother;

  }

/**
 * @brief Get the strip and row number of the cell.
 * 
 * Can be used as `auto [strip, row] = getStripRow(cellId);` 
 * For SCT sensors, return 0 as there should only be one row; derived versions for ITk strip implement specializations
 * 
 * @param cellId The SiCellId
 * @return std::pair<int,int> A pair of ints representing the strip ID and row ID. In this case, [0,0].
 */
std::pair<int,int> SCT_ModuleSideDesign::getStripRow(SiCellId /*id*/) const {
   return {0,0};
 }

  SiIntersect SCT_ModuleSideDesign::inDetector(const SiLocalPosition &localPosition, double phiTol, double etaTol, bool forceStringent) const {
    
    //if we are not doing a stringent check, we should first see if there is
    //a motherDesign, and if so, do the check based on that instead
    if(!forceStringent){
      
      const SCT_ModuleSideDesign * mother = getMother();
      
      //Stringent check on mother, to skip checking for "grandmother"
      if(mother) return mother->inDetector(localPosition,phiTol,etaTol, true); 
    
    }

    double etaDist = 0;
    double phiDist = 0;

    distanceToDetectorEdge(localPosition, etaDist, phiDist);

    SiIntersect state;

    if (phiDist < -phiTol || etaDist < -etaTol) {
        state.setOut();
        return state;
    }

    if (phiDist > phiTol && etaDist > etaTol) {
        state.setIn();
        return state;
    }

    // Near boundary.
    state.setNearBoundary();
    return state;
}

SiIntersect SCT_ModuleSideDesign::inDetector(const SiLocalPosition &localPosition, double phiTol, double etaTol) const {
  //default check is not "stringent" - i.e it will use a mother if one exists
  return inDetector(localPosition,phiTol,etaTol, false);
}

} // namespace InDetDD
