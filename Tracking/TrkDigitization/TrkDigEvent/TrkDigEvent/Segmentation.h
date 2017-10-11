//////////////////////////////////////////////////////////////////
// Segmentation.h, ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDIGEVENT_SEGMENTATION_H
#define TRKDIGEVENT_SEGMENTATION_H

// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"
// Trk includes
#include "TrkDigEvent/DigitizationCell.h"
// STL
#include <memory>
#include <vector>

namespace Trk {
        
    class Surface;
    class SurfaceBounds;
    
    /** @class Segmentation Base class 
    
        This helper class allows to define an arbitrary readout 
        segmentation for the geoemtric digitization, provided a shape of the module,
        it creates the segmentation surfaces and hands them to the digitization module
    
        Since the segmentation description might be identical for many elements while
        the lorentz angle may change, lorentzAngle and readoutDirection are provided
        and th the segmenation class only creates the surfaces for the module, but
        hosts the binning information.
    
        @author Andreas.Salzburger -at- cern.ch, Noemi.Calace -at- cern.ch        
    
    */
    class Segmentation {
        public:
            /** Virtual Destructor */
            virtual ~Segmentation() {}
            
            /** Create the segmentation surfaces in X */
            virtual void createSegmenationSurfaces(std::vector< std::shared_ptr< const Surface> >& boundarySurfaces,
                                                   std::vector< std::shared_ptr< const Surface> >& segmentationSurfacesX,
                                                   std::vector< std::shared_ptr< const Surface> >& segmentationSurfacesY,
                                                   double halfThickness,
                                                   int readoutDirection, 
                                                   double lorentzAngle) const = 0;

            /** Get the digitization cell fropm a 3D position - ignores the shift */
            virtual const DigitizationCell cell(const Amg::Vector3D& position) const = 0;
            
            /** Get the digitization cell fropm a 2D position  */
            virtual const DigitizationCell cell(const Amg::Vector2D& position) const = 0;
            
            /** calculate the cell Position from the Id */
            virtual const Amg::Vector2D cellPosition(const DigitizationCell& dCell) const = 0;
            
            /** Fill the associated digitsation cell from the start and end position in 3D, correct for lorentz effect if needed */
            virtual const DigitizationStep digitizationStep(const Amg::Vector3D& start, 
                                                            const Amg::Vector3D& end,
                                                            double halfThickness,
                                                            int readoutDirection, 
                                                            double lorentzAngle) const =0;
                                                                                     
            /** return the surface bounds by reference */
            virtual const SurfaceBounds& moduleBounds() const = 0;
                                                                                                 

    };

}

#endif 
   
