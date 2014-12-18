/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedPlaneLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_SUBTRACTEDPLANELAYER_H
#define TRKGEOMETRY_SUBTRACTEDPLANELAYER_H

class MsgStream;

#include "TrkGeometry/PlaneLayer.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
#include "TrkEventPrimitives/PropDirection.h"


namespace Trk {

  class LayerMaterialProperties;

  /** 
   @class SubtractedPlaneLayer
   
   Class to describe a planar detector layer for tracking, with subtraction 
       
   @author Sarka.Todorova@cern.ch 
   */

  class SubtractedPlaneLayer : virtual public SubtractedPlaneSurface, public Layer {
      
      public:
        /**Default Constructor*/
        SubtractedPlaneLayer(){}
        
        /**Constructor with SubtractedPlaneSurface 
           and MaterialProperties  */
        SubtractedPlaneLayer(const SubtractedPlaneSurface* subtrPlaneSurf,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));
                           
       /**Copy constructor of SubtractedPlaneLayer*/
        SubtractedPlaneLayer(const SubtractedPlaneLayer& pla);

       /**Copy constructor with shift*/
        SubtractedPlaneLayer(const SubtractedPlaneLayer& pla, const Amg::Transform3D& tr);
        
        /**Assignment operator for PlaneLayers */
        SubtractedPlaneLayer& operator=(const SubtractedPlaneLayer&);
                                          
        /**Destructor*/
        virtual ~SubtractedPlaneLayer(){}   
    
        /** Transforms the layer into a Surface representation for extrapolation */
        const SubtractedPlaneSurface& surfaceRepresentation() const override;            

        /** getting the MaterialProperties back - for pre-update*/ 
        double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                       Trk::PropDirection dir) const override;

        /** getting the MaterialProperties back - for post-update*/ 
        double  postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                         Trk::PropDirection dir) const override;

        /** move the Layer */
         void moveLayer( Amg::Transform3D& shift ) const override;
        
    private:
       /** Resize the layer to the tracking volume - not implemented */ 
       void resizeLayer(const VolumeBounds&, double) const override {}      
       
       /** Resize the layer to the tracking volume - not implemented */ 
       virtual void resizeAndRepositionLayer(const VolumeBounds&, const Amg::Vector3D&, double) const override {}
          

                                                                   
  };

} // end of namespace

#endif // TRKGEOMETY_SUBTRACTEDPLANELAYER_H

