/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrapezoidSegmentation.h, ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDIGEVENT_TRAPEZOIDSEGMENTATION_H
#define TRKDIGEVENT_TRAPEZOIDSEGMENTATION_H

// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"
// Trk includes
#include "TrkDigEvent/Segmentation.h"
#include "TrkDigEvent/DigitizationCell.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkDetDescrUtils/BinUtility.h"
// STL
#include <memory>

namespace Trk {
                  
    /** @class Segmentation Base class 
    
        Segementation class for generic pixel, strixels and strip segmentations on a trapezoidal shape
    
        Conventions: 
          - 3D positions are within the 3D frame of the module 
          - 2D positions are corrected to the readout surface 
            (the lorentzShift is not taken into account fro trapezoidal elements)
    
        @author Andreas.Salzburger -at- cern.ch, Noemi.Calace -at- cern.ch    
    
    */
    class TrapezoidSegmentation : public Segmentation {
        public:
            /** Constructor for all same-size pixels or strips (in case numCellsY is set to 1) */
            TrapezoidSegmentation(std::shared_ptr<const Trk::TrapezoidBounds>,
                                    size_t numCellsX, size_t numCellsY=1); 
                                    
            /** TODO contructor from BinUtilities for more complex readouts */                        
            
            /** Virtual Destructor */
            virtual ~TrapezoidSegmentation();
            
            
            /** Create the segmentation surfaces in X */
            void createSegmenationSurfaces(std::vector< std::shared_ptr< const Surface> >& boundarySurfaces,
                                           std::vector< std::shared_ptr< const Surface> >& segmentationSurfacesX,
                                           std::vector< std::shared_ptr< const Surface> >& segmentationSurfacesY,
                                           double halfThickness,
                                           int readoutDirection=1., 
                                           double lorentzAngle=0.) const override;
            
            /** Get the digitization cell from a 3D position - ignores the shift */
            const DigitizationCell cell(const Amg::Vector3D& position) const override;

            /** Get the digitization cell from a position */
            const DigitizationCell cell(const Amg::Vector2D& position) const override;
            
            /** calculate the cell Position from the Id */
            const Amg::Vector2D cellPosition(const DigitizationCell& cId) const override;
            
            /** Fill the associated digitisation cell from this start and end position, correct for lorentz effect if needed */
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
	    
	    /** Return the local pitch X  */
	    double PitchX(const Amg::Vector2D &localPos) const;
	    
	    /** Return the local sinStereo  */
	    double sinStereoLocal(const Amg::Vector2D &localPos) const;
	    
	    /** Return the projected x value on the y=0 */
	    double projectLocX(const Amg::Vector2D &localPos) const;
	    
	    /** Return the radius correponding to the given module */
	    double radius() const;
                                                                                                          
            std::shared_ptr<const TrapezoidBounds>         m_activeBounds;
            BinUtility*                                    m_binUtility;
            size_t                                         m_binsX;
            size_t                                         m_binsY;
        
    };
    
    inline const SurfaceBounds& TrapezoidSegmentation::moduleBounds() const { return (*(m_activeBounds.get())); }
    
    inline size_t TrapezoidSegmentation::numCellsX() const { return m_binsX; }
        
    inline size_t TrapezoidSegmentation::numCellsY() const { return m_binsY; }
    
    template <class T> const DigitizationCell TrapezoidSegmentation::cellT(const T& position) const
    {
        if (m_binsX == 1) 
            return DigitizationCell(0, m_binUtility->bin(position,0));
        else if (m_binsY == 1 )
            return DigitizationCell(m_binUtility->bin(position,0), 0);
        return DigitizationCell(m_binUtility->bin(position,0), m_binUtility->bin(position,1));
    }
    
    inline const DigitizationCell TrapezoidSegmentation::cell(const Amg::Vector3D& position) const { 
      Amg::Vector3D CorrPosition = position;
      CorrPosition.x() = projectLocX(Amg::Vector2D(CorrPosition.x(), CorrPosition.y()));
      return cellT<Amg::Vector3D>(CorrPosition); 
    }

    inline const DigitizationCell TrapezoidSegmentation::cell(const Amg::Vector2D& position) const { 
      using Trk::locX;
      using Trk::locY;
      Amg::Vector2D CorrPosition = position;
      CorrPosition[Trk::locX] = projectLocX(Amg::Vector2D(CorrPosition[Trk::locX], CorrPosition[Trk::locY]));
      return cellT<Amg::Vector2D>(CorrPosition);  
    }
    
    double TrapezoidSegmentation::radius() const
    {
      return m_activeBounds->halflengthY()/(m_activeBounds->maxHalflengthX() - m_activeBounds->minHalflengthX())*(m_activeBounds->maxHalflengthX() + m_activeBounds->minHalflengthX());
    }
    
    double TrapezoidSegmentation::sinStereoLocal(const Amg::Vector2D &localPos) const {
      using Trk::locX;
      using Trk::locY;
      double oneOverRadius = 1./radius();
      double x = localPos[Trk::locX];
      double y = localPos[Trk::locY];
      return -x*oneOverRadius / sqrt( (1+y*oneOverRadius)*(1+y*oneOverRadius) + x*oneOverRadius*x*oneOverRadius ); 
    }
    
    double TrapezoidSegmentation::PitchX(const Amg::Vector2D &localPos) const {
      using Trk::locY;
      double tanPhi = (m_activeBounds->maxHalflengthX()-m_activeBounds->minHalflengthX())/m_activeBounds->halflengthY();
      double lengthXatHit = (radius()+localPos[Trk::locY])*tanPhi;
      return lengthXatHit/float(m_binsX);
    }
    
    double TrapezoidSegmentation::projectLocX(const Amg::Vector2D& localPos) const {
      return -radius()*tan(asin(sinStereoLocal(localPos)));
    }

  
}

#endif // end of TRKDIGEVENT_TRAPEZOIDSEGMENTATION_H
