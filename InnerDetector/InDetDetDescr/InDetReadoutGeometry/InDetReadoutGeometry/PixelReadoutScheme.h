/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelReadoutScheme.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_PIXELREADOUTSCHEME_H
#define INDETREADOUTGEOMETRY_PIXELREADOUTSCHEME_H

// Data member classes
#include "InDetReadoutGeometry/PixelMultipleConnection1D.h"

// Input and output classes
#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "InDetReadoutGeometry/SiCellId.h"

namespace InDetDD {

  /** @class PixelReadoutScheme

      Class used to describe the connection scheme of a diode matrix
      to a set of readout circuits
      - to get the readout cell connected to a diode
      - to get the list of diodes connected to a readout cell

      @author David Calvet
      - modified on 5/10/04 Davide Costanzo. Replace PixelModulePosition with SiCellId
      - modified: Grant Gorfine, Andreas Salzburger
    */

  class PixelReadoutScheme {
  
     ///////////////////////////////////////////////////////////////////
     // Public methods:
     ///////////////////////////////////////////////////////////////////
    public:

      ///////////////////////////////////////////////////////////////////
      // Constructors/destructor:
      ///////////////////////////////////////////////////////////////////
  
      /** Constructor with parameters:
       
         number of circuits in one circuit column
         number of circuits in one circuit row
         number of cell columns per circuit
         number of cell rows per circuit
         number of diode columns connected to one circuit
         number of diode rows connected to one circuit */
         PixelReadoutScheme(const int circuitsPerColumn,const int circuitsPerRow,
                            const int cellColumnsPerCircuit,
                            const int cellRowsPerCircuit,
                            const int diodeColumnsPerCircuit,
                            const int diodeRowsPerCircuit);
  
        /** Destructor: */
         ~PixelReadoutScheme();
  
       ///////////////////////////////////////////////////////////////////
       // Const methods:
       ///////////////////////////////////////////////////////////////////

        /** total number of circuits: */
        int numberOfCircuits() const;

        /** number of cell columns per circuit: */
        int columnsPerCircuit() const;

        /** number of cell rows per circuit:*/
        int rowsPerCircuit() const;

        /** number of cell columns per module:*/
        int columns() const;

        /** number of cell rows per module:*/
        int rows() const;

        /** number of cells connected to this readout.*/
        int numberOfConnectedCells(const SiReadoutCellId & readoutId) const;

        /** Cell ids of cell connected to this readout.*/
        SiCellId connectedCell(const SiReadoutCellId & readoutId, unsigned int number) const;

        /** Readout id of this diode.*/
        SiReadoutCellId readoutIdOfCell(const SiCellId & cellId) const;

        /** If cell is ganged return the other cell, otherwise return an invalid id.*/
       SiCellId gangedCell(const SiCellId & cellId) const;

     ///////////////////////////////////////////////////////////////////
     // Non-const methods:
     ///////////////////////////////////////////////////////////////////

      /** Add a new multiple connection for rows:
          lower diode row for which the connection scheme is given
          vector containing, for each diode, the readout cell row number 
           to which the corresponding diode is connected */
      void addMultipleRowConnection(const int lowerRow, const std::vector<int> &connections);

     ///////////////////////////////////////////////////////////////////
     // Private methods:
     ///////////////////////////////////////////////////////////////////
    private:

      PixelReadoutScheme();
  
     ///////////////////////////////////////////////////////////////////
     // Private data:
     ///////////////////////////////////////////////////////////////////
   private:
     PixelMultipleConnection1D m_rowConnections; //!< multiple connections for rows
     int m_numberOfCircuits; //!< total number of circuits
     int m_columnsPerCircuit; //!< number of cell columns per circuit
     int m_rowsPerCircuit; //!< number of cell rows per circuit
     int m_columns; //!< Number of columns
     int m_rows; //!< Number of rows

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline PixelReadoutScheme::~PixelReadoutScheme()
{}

inline int PixelReadoutScheme::numberOfCircuits() const
{
  return m_numberOfCircuits;
}

inline int PixelReadoutScheme::columnsPerCircuit() const
{
  return m_columnsPerCircuit;
}

inline int PixelReadoutScheme::rowsPerCircuit() const 
{
  return m_rowsPerCircuit;
}

inline int PixelReadoutScheme::columns() const
{
  return m_columns;
}

inline int PixelReadoutScheme::rows() const 
{
  return m_rows;
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_PIXELREADOUTSCHEME_H
