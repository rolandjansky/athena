/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelMultipleConnection1D.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////


#ifndef INDETREADOUTGEOMETRY_PIXELMULTIPLECONNECTION1D_H
#define INDETREADOUTGEOMETRY_PIXELMULTIPLECONNECTION1D_H

#include <vector>

namespace InDetDD {

    /** @class PixelMultipleConnection1D

         Class used to handle connection of multiple diodes to the same readout cell

         @author: David Calvet
         @modifed: Grant Gorfine, Andreas Salzburger
      */

    class PixelMultipleConnection1D {
  
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      public:

        PixelMultipleConnection1D();

      /** Constructor with parameters:
          lower diode index for which the connection scheme is given
          vector containing, for each diode, the readout cell number to which
          the corresponding diode is connected */
        PixelMultipleConnection1D(const int lowerIndex, const std::vector<int> &connections);


        void set(const int lowerIndex, const std::vector<int> &connections);
 
  
        ///////////////////////////////////////////////////////////////////
        // Const methods:
        ///////////////////////////////////////////////////////////////////

        /** Check if the readout cell is in the range; Also for index of gangedCell. */
        bool outsideReadout(const int index) const;

        /** Check if the diode is in the range: */
        bool outsideDiode(const int cell) const;

        /** Return the number of diodes that are connected to this readout cell, Must check outsideReadout first. */
        int numberOfConnectedCells(const int index) const;

        /** Return the diode index corresponding the this readout. 
            number 0 will return the primary cell (ie diode index = readout cell index)
            Must check outsideReadout first.*/
        int connectedCell(const int index, unsigned int number) const;

        /** Return readout cell id of this diode, Must check outsideDiode first. */
        int readoutOfDiode(const int index) const;

        /** If the diode is gnaged return the other diode. If the diode is not ganged return the same diode.
            Must check outsideReadout first. */
        int gangedCell(const int index) const;
  
        ///////////////////////////////////////////////////////////////////
        // Private data:
        ///////////////////////////////////////////////////////////////////
      private:
        int m_lowerDiode;              //!< lower diode index to which it applies
        int m_upperDiode;              //!< upper diode index to which it applies
        int m_lowerReadout;            //!< lower cell to which it applies
        int m_upperReadout;            //!< upper cell to which it applies
        std::vector< std::vector<int> > m_readoutToDiode;  //!< connection table readout -> diodes
        std::vector<int> m_diodeToReadout;            //!< connection table diode -> readout
        std::vector<int> m_diodeGanged;              //!< connection table diode -> other diode.

        ///////////////////////////////////////////////////////////////////
        // Private methods:
        ///////////////////////////////////////////////////////////////////
        void debugPrintout();

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline bool PixelMultipleConnection1D::outsideReadout(const int readout) const
{
  return (readout<m_lowerReadout || readout>m_upperReadout);
}


inline bool PixelMultipleConnection1D::outsideDiode(const int diode) const
{
  return (diode<m_lowerDiode || diode>m_upperDiode);
}


inline int PixelMultipleConnection1D::numberOfConnectedCells(const int index) const
{
  return m_readoutToDiode[index - m_lowerReadout].size();
}

inline int  PixelMultipleConnection1D::connectedCell(const int index, unsigned int number) const
{
  return m_readoutToDiode[index - m_lowerReadout][number];
}

inline int PixelMultipleConnection1D::readoutOfDiode(const int index) const
{
  return m_diodeToReadout[index - m_lowerDiode];
}

inline int PixelMultipleConnection1D::gangedCell(const int index) const
{
  return m_diodeGanged[index - m_lowerReadout];
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_PIXELMULTIPLECONNECTION1D_H
