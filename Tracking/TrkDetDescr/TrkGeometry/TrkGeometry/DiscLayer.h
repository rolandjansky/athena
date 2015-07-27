/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_DISCLAYER_H
#define TRKGEOMETRY_DISCLAYER_H

class MsgStream;

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkEventPrimitives/PropDirection.h"
// STL sorting
#include <algorithm>

namespace Trk {

  class DiscBounds;
  class VolumeBounds;
  class LayerMaterialProperties;
  class OverlapDescriptor;
  class IApproachDescriptor;
  
  /**
   @class DiscLayer
   
   Class to describe a disc-like detector layer for tracking, 
   it inhertis from both, Layer base class
   and DiscSurface class
       
   @author Andreas.Salzburger@cern.ch 
   */

  class DiscLayer : virtual public DiscSurface, public Layer {

      friend class TrackingVolume;
      
      public:
        /**Default Constructor*/
        DiscLayer(){}
        
        /**Constructor with DiscSurface components and MaterialProperties */
        DiscLayer(Amg::Transform3D* transform,
                  DiscBounds* dbounds,
                  const LayerMaterialProperties& laymatprop,
                  double thickness = 0.,
                  OverlapDescriptor* od = 0,
                  int laytyp=int(Trk::active));

        /**Constructor with DiscSurface and MaterialProperties */
        DiscLayer(DiscSurface* disc,
                  const LayerMaterialProperties& laymatprop,
                  double thickness = 0.,
                  OverlapDescriptor* od = 0,
                  int laytyp=int(Trk::active));
                  
        /**Constructor with DiscSurface components and pointer to SurfaceArray (passing ownership) */
        DiscLayer(Amg::Transform3D* transform,
                  DiscBounds* dbounds,
                  SurfaceArray* surfaceArray,
                  double isontolerance = 0.,
                  OverlapDescriptor* od = 0,
                  IApproachDescriptor* ad = 0,
                  int laytyp=int(Trk::active));
                
        /**Constructor with DiscSurface components, 
           MaterialProperties and pointer SurfaceArray (passing ownership) */
        DiscLayer(Amg::Transform3D* transform,
                  DiscBounds* dbounds,
                  SurfaceArray* surfaceArray,
                  const LayerMaterialProperties& laymatprop,
                  double thickness = 0.,
                  OverlapDescriptor* od = 0,
                  IApproachDescriptor* ad = 0,
                  int laytyp=int(Trk::active)); 
 
        /**Copy constructor of DiscLayer*/
        DiscLayer(const DiscLayer& cla);

        /**Copy constructor with shift*/
        DiscLayer(const DiscLayer& cla, const Amg::Transform3D& tr);
        
        /**Assignment operator for DiscLayers */
        DiscLayer& operator=(const DiscLayer&);
              
        /**Destructor*/
        virtual ~DiscLayer(){}  
                
        /** Transforms the layer into a Surface representation for extrapolation */
        const DiscSurface& surfaceRepresentation() const override;

        /** Surface seen on approach - if not defined differently, it is the surfaceRepresentation() */
        const Surface& surfaceOnApproach(const Amg::Vector3D& pos,
                                         const Amg::Vector3D& mom, 
                                         PropDirection pdir,
                                         const BoundaryCheck& bcheck,
                                         bool resolveSubSurfaces = 0,
                                         const ICompatibilityEstimator* ice = 0) const override;
      
        /** getting the MaterialProperties back - for pre-update*/ 
        double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                       Trk::PropDirection dir) const override;

        /** getting the MaterialProperties back - for post-update*/ 
        double  postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                         Trk::PropDirection dir) const override;

       /** move the Layer */
        void moveLayer( Amg::Transform3D& shift ) const override;
     
     private:   
       /** Resize the layer to the tracking volume - only works for CylinderVolumeBouns */ 
       void resizeLayer(const VolumeBounds& vBounds, double envelope) const override;        
           
       /** Resize the layer to the tracking volume - not implemented */ 
       void resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& cCenter, double envelop) const override;

       /** build approach surfaces */
       void buildApproachDescriptor() const;
    
       /** Surface seen on approach - if not defined differently, it is the surfaceRepresentation() */
       const Surface& approachSurface(const Amg::Vector3D& pos,
                                      const Amg::Vector3D& dir,
                                      const BoundaryCheck& bcheck) const;    
     protected:
       mutable IApproachDescriptor*  m_approachDescriptor;      //!< surface for approaching
    
  };

  /** @class DiscLayerSorterZ 
	simple helper function to allow sorting of DiscLayers in z
  */
  class DiscLayerSorterZ : public std::binary_function<const DiscLayer*, const DiscLayer*, bool> {
	public:       
	 /** Default Constructor */
	 DiscLayerSorterZ()
	    {}
	       
	    bool operator() (const DiscLayer* one, const DiscLayer* two) const 
	    {return ( one->center().z() <  two->center().z() ); }          
    };

} // end of namespace

#endif // TRKGEOMETY_DISCLAYER_H

