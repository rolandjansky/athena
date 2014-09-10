/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDiodeMap.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_PIXELDIODEMAP_H
#define INDETREADOUTGEOMETRY_PIXELDIODEMAP_H

// STL classes
#include <vector>
#include <list>

// Data member classes
#include "InDetReadoutGeometry/PixelDiodeMatrix.h"

// Input/output classes
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiDiodesParameters.h"

namespace InDetDD {

class SiCellId;
class PixelDiodeMatrix;

     /** @class PixelDiodeMap
     
        Class used to describe the diode segmentation of a pixel module
        - to get the position and size of a PixelDiode object
        - to get a PixelDiode object containing a space point
        - to get a PixelDiode object from a PixelModulePosition

        @author: David Calvet
        - modified Davide Costanzo. Replace PixelModulePosition with SiCellId
        - modified: Grant Gorfine, Andreas Salzburger
        */

    class PixelDiodeMap {
      
        public:
        
          /** Constructor from Diode matrix description */
          PixelDiodeMap(const PixelDiodeMatrix * diodeMatrix);
          
          /** Copy constructor: */
          PixelDiodeMap(const PixelDiodeMap &map);
          
          /** Destructor */
          ~PixelDiodeMap();
          
          /** Assignment operator */
          PixelDiodeMap &operator=(const PixelDiodeMap &map);
          
          ///////////////////////////////////////////////////////////////////
          // Const methods:
          ///////////////////////////////////////////////////////////////////
        
          /** Get diodes parameters (position and size): */
          SiDiodesParameters parameters(const SiCellId & diodeId) const;
        
          /** cell id for a given local position */
          SiCellId cellIdOfPosition(const Amg::Vector2D & localPosition) const;
        
          /** Check if cellId is within range */
          SiCellId cellIdInRange(const SiCellId & cellId) const;
        
          /** Get the neighbouring PixelDiodes of a given PixelDiode:
             Cell for which the neighbours must be found
             List of cells which are neighbours of the given one */
          void neighboursOfCell(const SiCellId & cellId,
        			std::vector<SiCellId> &neighbours) const;
        
          /** Compute the intersection length of two diodes:
              return: the intersection length when the two diodes are projected on one
              of the two axis, or 0 in case of no intersection or problem
              nput: the two diodes for which the intersection length is computed */
          double intersectionLength(const SiCellId &diode1, const SiCellId &diode2) const;
        
          // Global sensor size:
          double leftColumn() const;
          double rightColumn() const;
          double leftRow() const;
          double rightRow() const;
          
          double width() const;
          double length() const;
        
          // Number of cells in each direction.
          int phiDiodes() const;
          int etaDiodes() const;
        
          // Total number of diodes:
          int diodes() const;
        
          ///////////////////////////////////////////////////////////////////
          // Non-const methods:
          ///////////////////////////////////////////////////////////////////
          /** Set general layout flag. This can be set to true to allow handling
              of more complex layouts where cells are not lined up with each other 
              such as bricking. Probably never will be needed.  */
          void setGeneralLayout() {m_generalLayout = true;}
        
          ///////////////////////////////////////////////////////////////////
          // Private methods:
          ///////////////////////////////////////////////////////////////////
        private:
        
          /** Compute the intersection length along one direction:
               return 0 if no intersection
               x1,x2 are the centers
               dx1,dx2 are the full lengths */
          double intersectionLength1D(const double x1,const double dx1,
        			      const double x2,const double dx2) const;
          
          /** Slower method. Called by neighboursOfCell if m_generalLayout set. */
          void neighboursOfCellGeneral(const SiCellId & cellId,
        			                   std::vector<SiCellId> &neighbours) const;
            
          /** Slower method. Called by intersectionLength if m_generalLayout set. */
          double intersectionLengthGeneral(const SiCellId &diode1,
        				   const SiCellId &diode2) const;
            
        
          ///////////////////////////////////////////////////////////////////
          // Private data:
          ///////////////////////////////////////////////////////////////////
        private:
          const PixelDiodeMatrix * m_matrix;    //!< diode matrix
          bool m_generalLayout;                 //!< Flag set to allow for dealing wth more general layouts.
    };
    
    ///////////////////////////////////////////////////////////////////
    // Inline methods:
    ///////////////////////////////////////////////////////////////////
    
    inline double PixelDiodeMap::length() const
    {
      return m_matrix->etaWidth();
    }
    
    inline double PixelDiodeMap::width() const
    {
      return m_matrix->phiWidth();  
    }
    
    inline double PixelDiodeMap::leftColumn() const
    {
      return -0.5*length();
    }
    
    inline double PixelDiodeMap::rightColumn() const
    {
      return 0.5*length();  
    }
    
    inline double PixelDiodeMap::leftRow() const
    {
      return -0.5*width();
    }
    
    inline double PixelDiodeMap::rightRow() const
    {
      return 0.5*width();
    }
    
    inline int PixelDiodeMap::diodes() const
    {
      return phiDiodes() * etaDiodes();
    }
    
    inline int PixelDiodeMap::phiDiodes() const
    {
      return m_matrix->phiCells();
    }
    
    inline int PixelDiodeMap::etaDiodes() const
    {
      return m_matrix->etaCells();
    }
    
    inline SiCellId PixelDiodeMap::cellIdInRange(const SiCellId & cellId) const
    {
      if (!cellId.isValid() || 
          cellId.phiIndex() <0 || cellId.phiIndex() >= phiDiodes() ||
          cellId.etaIndex() <0 || cellId.etaIndex() >= etaDiodes()) return SiCellId(); // Invalid
      return cellId; 
    }

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_PIXELDIODEMAP_H
