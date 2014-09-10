/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiReadoutCellId.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SIREADOUTCELLID_H
#define INDETREADOUTGEOMETRY_SIREADOUTCELLID_H

// Base class
#include "InDetReadoutGeometry/SiCellId.h"

namespace InDetDD {

   /** @class SiReadoutCellId 

   Identifier for the strip or pixel readout cell.
  
   SiReadoutCellId behaves identically to its base class. Its only
   introduced to provide some type safety and distiguish between
   diodes (base class) and readout cells (this class). This is since
   more than one diode can be connected to a readout cell. 

   The following conversion is legal

        SiReadoutCellId -> SiCellId   OK   

   However the oppoisite is not allowed 

        SiCellId -> SiReadoutCellId   NOT ALLOWED

   since a SiCellId might not have direct readout out (eg for an
   edge strip or ganged pixel).
   
   @author Grant Gorfine
   */

  class SiReadoutCellId : public SiCellId {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
     public:
  
      // Allow construction from base class
      //  explicit SiReadoutCellId(const SiCellIdentifier &);

      SiReadoutCellId(); // Set in invalid state.
      SiReadoutCellId(int strip);
      SiReadoutCellId(int phiIndex, int etaIndex);

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline SiReadoutCellId::SiReadoutCellId() 
  : SiCellId() 
{}

inline SiReadoutCellId::SiReadoutCellId(int phiIndex, int etaIndex)
  : SiCellId(phiIndex, etaIndex)
{}

inline SiReadoutCellId::SiReadoutCellId(int strip)
  : SiCellId(strip)
{}


} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SIREADOUTCELLID_H
