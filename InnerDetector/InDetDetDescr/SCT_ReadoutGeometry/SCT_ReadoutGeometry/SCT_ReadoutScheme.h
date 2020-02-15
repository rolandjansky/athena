/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ReadoutScheme.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SCT_READOUTSCHEME_H
#define INDETREADOUTGEOMETRY_SCT_READOUTSCHEME_H

#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"

#include <list>

namespace InDetDD {

    /** @class SCT_ReadoutScheme
    
        Definition of the readout scheme in the SCT detector describing,
        number of sides, cells, crystals per module.
    
        @author Alessandro Fornaini, Grant Gorfine
    */
    
    class SCT_ReadoutScheme {
    
        ///////////////////////////////////////////////////////////////////
        // Public methods:
        ///////////////////////////////////////////////////////////////////
      public:
      
        // Copy constructor:
        SCT_ReadoutScheme(const SCT_ReadoutScheme &scheme);
      
        // Constructor with parameters:
        //   number of crystals within module side
        //   number of diodes within crystal
        //   number of cells within module side
        //   index of diode connected to cell with index 0
        SCT_ReadoutScheme(const int crystals,const int diodes,
      		    const int cells,const int shift);
      
        // Destructor:
        virtual ~SCT_ReadoutScheme() = default;
      
        // Assignment operator:
        SCT_ReadoutScheme &operator=(const SCT_ReadoutScheme &scheme);

        void setDiodes(int numDiodes);
        void setCells(int numReadoutCells);
      
        ///////////////////////////////////////////////////////////////////
        // Const methods:
        ///////////////////////////////////////////////////////////////////
      
        // number of crystals within module side:
        int crystals() const;
      
        // number of diodes within crystal:
        int diodes() const;
      
        // number of cells within module side:
        int cells() const;
      
        // index of diode connected to cell with index 0:
        int shift() const;
      
        // readout id -> id of connected diodes
        int numberOfConnectedCells(const SiReadoutCellId & readoutId) const;
        SiCellId connectedCell(const SiReadoutCellId & readoutId, int number) const;
       
        // diode id -> readout id
        SiReadoutCellId readoutIdOfCell(const SiCellId & cellId) const;
      
      
      
        ///////////////////////////////////////////////////////////////////
        // Non-const methods:
        ///////////////////////////////////////////////////////////////////
      
        ///////////////////////////////////////////////////////////////////
        // Private methods:
        ///////////////////////////////////////////////////////////////////
      private:
      
        SCT_ReadoutScheme();
      
        ///////////////////////////////////////////////////////////////////
        // Private data:
        ///////////////////////////////////////////////////////////////////
      private:
        int m_crystals; // number of crystals within module side
        int m_diodes; // number of diodes within crystal
        int m_cells; // number of cells within module side
        int m_shift; // index of diode connected to cell with index 0
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline void SCT_ReadoutScheme::setDiodes(int numDiodes) {
  m_diodes = numDiodes;
}

inline void SCT_ReadoutScheme::setCells(int numReadoutCells) {
  m_cells = numReadoutCells;
}

inline int SCT_ReadoutScheme::crystals() const
{
  return m_crystals;
}

inline int SCT_ReadoutScheme::diodes() const
{
  return m_diodes;
}

inline int SCT_ReadoutScheme::cells() const
{
  return m_cells;
}

inline int SCT_ReadoutScheme::shift() const
{
  return m_shift;
}

inline int SCT_ReadoutScheme::numberOfConnectedCells(const SiReadoutCellId & readoutId) const
{
  // Assume an in range readout id is used. No checks are made.
  // Always 1 to 1 correspondence in SCT 
  return (readoutId.isValid()) ? 1 : 0;
}


inline SiCellId SCT_ReadoutScheme::connectedCell(const SiReadoutCellId & readoutId, int) const
{
  // Assume a valid readout id is used.
  // Always 1 to 1 correspondence in SCT 
  return readoutId; // Allowed to convert a SiReadoutCellId to a SiCellId.
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SCT_READOUTSCHEME_H
