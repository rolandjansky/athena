/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_CYLINDERLAYER_H
#define TRKGEOMETRY_CYLINDERLAYER_H

class MsgStream;

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"
// STL sorting
#include <algorithm>

namespace Trk {

class CylinderBounds;
class LayerMaterialProperties;
class OverlapDescriptor;

  /**
   @class CylinderLayer
   
   Class to describe a cylindrical detector layer for tracking, it inhertis from both, 
   Layer base class and CylinderSurface class
       
   @author Andreas.Salzburger@cern.ch
  */

  class CylinderLayer : public CylinderSurface, public Layer {
                   
      public:
        /**Default Constructor*/
        CylinderLayer(){}
        
        /**Constructor with CylinderSurface components and  MaterialProperties */
        CylinderLayer(Amg::Transform3D* transform,
                      CylinderBounds* cbounds,
                      const LayerMaterialProperties& laymatprop,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));

        /**Constructor with CylinderSurface and  MaterialProperties */
        CylinderLayer(CylinderSurface* cyl,
                      const LayerMaterialProperties& laymatprop,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));
                              
        /**Constructor with CylinderSurface components and pointer to SurfaceArray (passing ownership) */
        CylinderLayer(Amg::Transform3D* transform,
                      CylinderBounds* cbounds,
                      SurfaceArray* surfaceArray,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));
                
        /**Constructor with CylinderSurface components,
           MaterialProperties and pointer SurfaceArray (passing ownership) */
        CylinderLayer(Amg::Transform3D* transform,
                      CylinderBounds* cbounds,
                      SurfaceArray* surfaceArray,
                      const LayerMaterialProperties& laymatprop,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));
                      
        /**Concentric Layer: Constructor with CylinderSurface components and  MaterialProperties */
        CylinderLayer(CylinderBounds* cbounds,
                      const LayerMaterialProperties& laymatprop,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));

        /**Concentric Layer: Constructor with CylinderSurface components and pointer to SurfaceArray (passing ownership) */
        CylinderLayer(CylinderBounds* cbounds,
                      SurfaceArray* surfaceArray,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));
                
        /**Concentric Layer: Constructor with CylinderSurface components,
           MaterialProperties and pointer SurfaceArray (passing ownership) */
        CylinderLayer(CylinderBounds* cbounds,
                      SurfaceArray* surfaceArray,
                      const LayerMaterialProperties& laymatprop,
                      double thickness = 0.,
                      OverlapDescriptor* od = 0,
                      int laytyp=int(Trk::active));
                              
        /**Copy constructor of CylinderLayer*/
        CylinderLayer(const CylinderLayer& cla);

        /**Copy constructor with shift*/
        CylinderLayer(const CylinderLayer& cla, const Amg::Transform3D& tr);
        
        /**Assignment operator for CylinderLayers */
        CylinderLayer& operator=(const CylinderLayer&);
                      
        /**Destructor*/
        virtual ~CylinderLayer(){}  
                
        /** Transforms the layer into a Surface representation for extrapolation */
        const CylinderSurface& surfaceRepresentation() const;
        
        /** getting the MaterialProperties back - for pre-update*/ 
        double preUpdateMaterialFactor(const Trk::TrackParameters& par,
                                       Trk::PropDirection dir) const;

        /** getting the MaterialProperties back - for post-update*/ 
        double  postUpdateMaterialFactor(const Trk::TrackParameters& par,
                                         Trk::PropDirection dir) const;

       /** Return the path correction */
       double pathCorrection(const TrackParameters& par) const;

       /** move the Layer */
       void moveLayer( Amg::Transform3D& shift ) const;
     
     private:   
       /** Resize the layer to the tracking volume - only works for CylinderVolumeBouns */ 
       void resizeLayer(const VolumeBounds& vBounds, double envelope) const; 
       
       /** Resize the layer to the tracking volume */ 
       void resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& cCenter, double envelope) const;       
  };

  /** @class CylinderLayerSorterR
       Functor for CylinderLayer R-Sorting */
  
  class CylinderLayerSorterR : public std::binary_function<const CylinderLayer*, const CylinderLayer*, bool> {
     public:       
      /** Default Constructor */
      CylinderLayerSorterR(){}
      
      bool operator() (const CylinderLayer* one, const CylinderLayer* two) const 
      { return ( one->surfaceRepresentation().bounds().r() <  two->surfaceRepresentation().bounds().r() ); }
  };

 
} // end of namespace

#endif // TRKGEOMETY_CYLINDERLAYER_H

