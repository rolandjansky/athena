/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// RectangularSegmentation.h, ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDIGEVENT_RECTANGLESEGMENTATION_H
#define TRKDIGEVENT_RECTANGLESEGMENTATION_H

// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"
// Trk includes
#include "TrkDigEvent/Segmentation.h"
#include "TrkDigEvent/DigitizationCell.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkDetDescrUtils/BinUtility.h"
// STL
#include <memory>

namespace Trk {
                  
    /** @class Segmentation Base class 
    
        Segementation class for generic pixel, strixels and strip segmentations on a rectangle shape
    
        Conventions: 
          - 3D positions are within the 3D frame of the module 
          - 2D positions are corrected to the readout surface 
            (they need to be corrected by the lorentzShift for the parameter surface in the center of the surface)
    
        @author Andreas.Salzburger -at- cern.ch, Noemi.Calace -at- cern.ch    
    
    */
    class RectangularSegmentation : public Segmentation {
        public:
            /** Constructor for all same-size pixels or strips (in cas numCellsY is set to 1) */
            RectangularSegmentation(std::shared_ptr<const Trk::RectangleBounds>,
                                    size_t numCellsX, size_t numCellsY=1); 
	    
	    /** Constructor for ATLAS module type pixels or strips */
           RectangularSegmentation(std::shared_ptr<const Trk::RectangleBounds>,
                                    size_t numCellsX, double longY, size_t numCellsY, double numOfChips); 
                                    
            /** TODO contructor from BinUtilities for more complex readouts */                        
            
            /** Virtual Destructor */
            virtual ~RectangularSegmentation();
            
            
            /** Create the segmentation surfaces in X */
            void createSegmenationSurfaces(std::vector< std::shared_ptr< const Surface> >& boundarySurfaces,
                                           std::vector< std::shared_ptr< const Surface> >& segmentationSurfacesX,
                                           std::vector< std::shared_ptr< const Surface> >& segmentationSurfacesY,
                                           double halfThickness,
                                           int readoutDirection=1., 
                                           double lorentzAngle=0.) const override;
            
            /** Get the digitization cell fropm a 3D position - ignores the shift */
            const DigitizationCell cell(const Amg::Vector3D& position) const override;

            /** Get the digitization cell fropm a position */
            const DigitizationCell cell(const Amg::Vector2D& position) const override;
            
            /** calculate the cell Position from the Id */
            const Amg::Vector2D cellPosition(const DigitizationCell& cId) const override;
            
            /** Fill the associated digitsation cell from this start and end position, correct for lorentz effect if needed */
            const DigitizationStep digitizationStep(const Amg::Vector3D& start, 
                                                    const Amg::Vector3D& end,
                                                    double halfThickness,
                                                    int readoutDirection=1, 
                                                    double lorentzAngle=0.) const override;
                                                                             
                                                                             
            /** return the surface bounds by reference */
            const SurfaceBounds& moduleBounds() const override;
            
            /** Return the simple binning parameters */
            size_t numCellsX() const;

            /** Return the simple binning parameters */
            size_t numCellsY() const;                                                                          
            
        private:
            
            template <class T> const DigitizationCell cellT(const T& position) const;  
                                                                                                          
            std::shared_ptr<const RectangleBounds>         m_activeBounds;
            BinUtility*                                    m_binUtility;
            size_t                                         m_binsX;
            size_t                                         m_binsY;
        
    };
    
    inline const SurfaceBounds& RectangularSegmentation::moduleBounds() const { return (*(m_activeBounds.get())); }
    
    inline size_t RectangularSegmentation::numCellsX() const { return m_binsX; }

    inline size_t RectangularSegmentation::numCellsY() const { return m_binsY; }
    
    template <class T> const DigitizationCell RectangularSegmentation::cellT(const T& position) const
    {
        if (m_binsX == 1) 
            return DigitizationCell(0, m_binUtility->bin(position,0));
        else if (m_binsY == 1 )
            return DigitizationCell(m_binUtility->bin(position,0), 0);
        return DigitizationCell(m_binUtility->bin(position,0), m_binUtility->bin(position,1));
    }
    
    inline const DigitizationCell RectangularSegmentation::cell(const Amg::Vector3D& position) const { return cellT<Amg::Vector3D>(position); }

    inline const DigitizationCell RectangularSegmentation::cell(const Amg::Vector2D& position) const { return cellT<Amg::Vector2D>(position); }
 
}

#endif 
