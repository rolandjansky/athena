/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlaneLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_PLANELAYER_H
#define TRKGEOMETRY_PLANELAYER_H

class MsgStream;

#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/PropDirection.h"
// STL sorting
#include <algorithm>

namespace Trk {

class RectangleBounds;
class TrapezoidBounds;
class DiamondBounds;
class EllipseBounds;
class LayerMaterialProperties;

  /** 
   @class PlaneLayer
   
   Class to describe a planar detector layer for tracking, 
   it inhertis from both, Layer base class and PlaneSurface class
       
   @author Andreas.Salzburger@cern.ch 
   */

  class PlaneLayer : virtual public PlaneSurface, public Layer {
      
      public:
        /**Default Constructor*/
        PlaneLayer(){}
        
        /**Constructor with PlaneSurface 
           components and MaterialProperties 
           - rectangle bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   RectangleBounds* rbounds,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                  int laytyp=int(Trk::active));
                   
        /**Constructor with PlaneSurface 
           components and MaterialProperties 
           - trapezoidal bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   TrapezoidBounds* tbounds,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));
                    
       /**Constructor with PlaneSurface 
           components and MaterialProperties 
           - diamond bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   DiamondBounds* tbounds,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));
                   
        /**Constructor with PlaneSurface 
           components and MaterialProperties 
           - ellipse bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   EllipseBounds* tbounds,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));

        /**Constructor with PlaneSurface 
           and MaterialProperties */

        PlaneLayer(Trk::PlaneSurface* plane,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));

        /**Constructor with PlaneSurface 
           components and MaterialProperties 
           - shared bounds */
	PlaneLayer(Amg::Transform3D* transform,
		   Trk::SharedObject<const Trk::SurfaceBounds>& tbounds,
		   const Trk::LayerMaterialProperties& laymatprop,
		   double thickness = 0.,
		   Trk::OverlapDescriptor* olap = 0,
		   int laytyp=int(Trk::active));
                           
        /**Constructor with PlaneSurface 
        components and pointer to SurfaceArray (passing ownership),
        - rectangle bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   RectangleBounds* rbounds,
                   SurfaceArray* surfaceArray,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));
         
         /**Constructor with PlaneSurface 
        components and pointer to SurfaceArray (passing ownership),
        - trapezoidal bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   TrapezoidBounds* tbounds,
                   SurfaceArray* surfaceArray,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));
         
         /**Constructor with PlaneSurface 
        components and pointer to SurfaceArray (passing ownership),
        - diamond bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   DiamondBounds* tbounds,
                   SurfaceArray* surfaceArray,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active));
                                  
        /**Constructor with PlaneSurface components, 
           MaterialProperties and pointer SurfaceArray (passing ownership) 
           - rectangle bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   RectangleBounds* rbounds,
                   SurfaceArray* surfaceArray,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active)); 
        
        /**Constructor with PlaneSurface components, 
           MaterialProperties and pointer SurfaceArray (passing ownership) 
           - trapezoidal bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   TrapezoidBounds* rbounds,
                   SurfaceArray* surfaceArray,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active)); 
        
        /**Constructor with PlaneSurface components, 
           MaterialProperties and pointer SurfaceArray (passing ownership) 
           - diamond bounds */
        PlaneLayer(Amg::Transform3D* transform,
                   DiamondBounds* rbounds,
                   SurfaceArray* surfaceArray,
                   const LayerMaterialProperties& laymatprop,
                   double thickness = 0.,
                   OverlapDescriptor* od = 0,
                   int laytyp=int(Trk::active)); 
                           
       /**Copy constructor of PlaneLayer*/
        PlaneLayer(const PlaneLayer& pla);

       /**Copy constructor with shift*/
        PlaneLayer(const PlaneLayer& pla, const Amg::Transform3D& tr);
        
        /**Assignment operator for PlaneLayers */
        PlaneLayer& operator=(const PlaneLayer&);
               
        /**Destructor*/
        virtual ~PlaneLayer(){}   
    
        /** Transforms the layer into a Surface representation for extrapolation */
        const PlaneSurface& surfaceRepresentation() const;            

        /** getting the MaterialProperties back - for pre-update*/ 
        double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                       Trk::PropDirection dir) const;

        /** getting the MaterialProperties back - for post-update*/ 
        double  postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                         Trk::PropDirection dir) const;

       /** move the Layer */
        void moveLayer( Amg::Transform3D& shift ) const;
    
     private:
       /** Resize the layer to the tracking volume - not implemented */ 
       void resizeLayer(const VolumeBounds&, double) const {}      
       
       /** Resize the layer to the tracking volume - not implemented */ 
       virtual void resizeAndRepositionLayer(const VolumeBounds&, const Amg::Vector3D&, double) const {}

                                                                   
  };

  /** @class PlaneLayerSorterX
     Functor for PlaneLayer X-Sorting */
     
  class PlaneLayerSorterX : public std::binary_function<const PlaneLayer*, const PlaneLayer*, bool> {
    public:       
       /** Default Constructor */
       PlaneLayerSorterX()
       {}
      
       bool operator() (const PlaneLayer* one, const PlaneLayer* two) const                
       { return ( one->center().x() <  two->center().x() ); }
       
  };
  
  /** @class PlaneLayerSorterY
     Functor for PlaneLayer Y-Sorting */
     
  class PlaneLayerSorterY : public std::binary_function<const PlaneLayer*, const PlaneLayer*, bool> {
    public:       
       /** Default Constructor */
       PlaneLayerSorterY()
       {}
      
       bool operator() (const PlaneLayer* one, const PlaneLayer* two) const                
       { return ( one->center().y() <  two->center().y() ); }
       
  };
  
  /** @class PlaneLayerSorterZ
     Functor for PlaneLayer Z-Sorting */
     
  class PlaneLayerSorterZ : public std::binary_function<const PlaneLayer*, const PlaneLayer*, bool> {
    public:       
       /** Default Constructor */
       PlaneLayerSorterZ()
       {}
      
       bool operator() (const PlaneLayer* one, const PlaneLayer* two) const                
       { return ( one->center().z() <  two->center().z() ); }
       
  };  


} // end of namespace

#endif // TRKGEOMETY_PLANELAYER_H

