/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Layer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_LAYER_H
#define TRKGEOMETRY_LAYER_H


#define TRKGEOMETRY_COMPLEXPATHCORRECTION 0

class MsgStream;

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  
  class Surface;
  class MaterialProperties;
  class BinUtility;
  class Volume;
  class VolumeBounds;
  class TrackingVolume;
  class DetachedTrackingVolume;

  typedef BinnedArray<Surface> SurfaceArray;

  /**
     @enum LayerType
   
     For readability     
  */
  enum LayerType { passive = 0,
                   active = 1 };

  /**
     @class Layer
  
     Base Class for a Detector Layer in the Tracking realm.
     An actual implemented Detector Layer inheriting from this base 
     class has to inherit from a specific type of Surface as well.
     In addition, a Layer can carry:
    
     - a SurfaceArray of Surfaces holding the actual detector elements or subSurfaces.
     - LayerMaterialProperties for Surface-based materialUpdates
     - an OverlapDescriptor (mainly used for blind extrapolation)
     - a pointer to the TrackingVolume (can only be set by such)     
     - an active/passive code :
     0      - activ
     1      - passive
     [....] - other

     @author Andreas.Salzburger@cern.ch
   
  */

  class Layer {

    /**Declare the TrackingGeometrySvc as friend, to be able to set material properties */
    friend class TrackingGeometrySvc;   
   
    /**Declare the IGeometryBuilder as a friend, to be able to change the volumelink */
    friend class IGeometryBuilder;

    /**Declare the TrackingVolume as a friend, to be able to register previous, 
       next and set the enclosing TrackingVolume*/
    friend class TrackingVolume;
    
    /** Declare the DetachedTrackingVolume as a friend to be able to register it */
    friend class DetachedTrackingVolume;
   
  public:                
    /**Default Constructor*/
    Layer();
        
    /**Constructor with MaterialProperties */
    Layer(const LayerMaterialProperties& laymatprop,
          double thickness = 0.,
          OverlapDescriptor* od = 0,
          int ltype=int(Trk::passive));
                
    /**Constructor with pointer to SurfaceArray (passing ownership) */
    Layer(SurfaceArray* surfaceArray,
          double thickness = 0.,
          OverlapDescriptor* od = 0,
          int ltype=int(Trk::passive));
                
    /**Constructor with MaterialProperties and pointer SurfaceArray (passing ownership) */
    Layer(SurfaceArray* surfaceArray,
          const LayerMaterialProperties& laymatprop,
          double thickness = 0.,
          OverlapDescriptor* od = 0,
          int ltype=int(Trk::passive)); 
        
    /**Copy Constructor for Layers */     
    Layer(const Layer& lay);
      
    /**Destructor*/
    virtual ~Layer();
        
    /** Assignment operator */
    Layer& operator=(const Layer& lay);
                     
    /** Return the entire SurfaceArray, returns 0 if no SurfaceArray*/
    const SurfaceArray* surfaceArray() const;
        
    /** If no subSurface array is defined or no subSurface can be found 
        to the given Amg::Vector3D, it would return 0
    */
    const Surface* subSurface(const Amg::Vector3D& gp) const;                  

    /** If no subSurface array is defined or no subSurface can be found 
        to the given Amg::Vector2D, it would return 0
    */
    const Surface* subSurface(const Amg::Vector2D& lp) const;
    
    /** Return a reference sub surface of the layer, usually the first one 
        in the array 
        - if an index is given (and the index is in range), this surface is returned    
    */
    const Surface* subSurfaceReference(unsigned int idx=0) const;
        
    /** Transforms the layer into a Surface representation for extrapolation */
    virtual const Surface& surfaceRepresentation() const = 0;
        
    /** Return the Thickness of the Layer */
    double thickness() const;
        
    /** Return the path correction when traversing the Layer */
    virtual double pathCorrection(const TrackParameters& par) const = 0;

    /** isOnLayer() method, using isOnSurface() with Layer specific tolerance */
    virtual bool isOnLayer(const Amg::Vector3D& gp) const;
        
    /** getting the LayerMaterialProperties including full/pre/post update */
    const LayerMaterialProperties* layerMaterialProperties() const;

    /** getting the MaterialProperties back - for full update*/ 
    const MaterialProperties* fullUpdateMaterialProperties(const TrackParameters& par) const;
        
    /** getting the MaterialProperties back - for pre-update*/ 
    virtual double preUpdateMaterialFactor(const TrackParameters& par,
                                           PropDirection dir) const = 0;

    /** getting the MaterialProperties back - for pre-update*/ 
    virtual double postUpdateMaterialFactor(const TrackParameters& par,
                                            PropDirection dir) const = 0;
                                                                      
    /** getting the next/overlapping Surface */
    const OverlapCell overlapSurface(const TrackParameters& tp,
                                     const Surface* sf = 0,
                                     PropDirection dir = Trk::anyDirection) const;
        
    /** gettint hte overlap descriptor */         
    const OverlapDescriptor* overlapDescriptor() const;
    
    /** getting the next/previous Layer if registered - unit for direction vector required */          
    const Layer* nextLayer(const Amg::Vector3D& gp, const Amg::Vector3D& udir) const;    
    
    /** getting what's stored to be the previous Layer, boolean to skip navigation layers */
    const Layer* previousLayer(bool skipNavLayer=false) const;                                   

    /** getting what's stored to be the next Layer, boolean to skip navigation layers*/
    const Layer* nextLayer(bool skipNavLayer=false) const;                                   

    /** get the confining TrackingVolume */
    const TrackingVolume* enclosingTrackingVolume() const;

    /** get the confining DetachedTrackingVolume */
    const DetachedTrackingVolume* enclosingDetachedTrackingVolume() const;

    /** get the layerIndex */
    const LayerIndex&  layerIndex() const;

    /** get the Layer codinging */
    int layerType() const;

    /** set the Layer codinging */
    void setLayerType(int id) const;

    /** assignMaterialPropeties - \todo make method private */
    void assignMaterialProperties(const LayerMaterialProperties&, double scale=1.0) const;    

    /** move the Layer */
    virtual void moveLayer( Amg::Transform3D& shift ) const = 0;

    /**register Volume associated to the layer */
    void registerRepresentingVolume(const Volume *theVol) const;
    
    /** get the Volume associated to the layer */
    const Volume* representingVolume() const;

    /** set the reference measure */
    void setRef(double ) const;

    /** get the reference measure */
    double getRef() const;

  private:

    void compactify(size_t& cSurfaces, size_t& tSurfaces) const;                    //!< propagate TrackingGeometry owner downstream
      
    void registerLayerIndex(const LayerIndex& lIdx) const;                          //!< register layer index for material map registration
    
    /** private method to set enclosing TrackingVolume, called by friend class only
        optionally, the layer can be resized to the dimensions of the TrackingVolume
        - Bounds of the Surface are resized
        - MaterialProperties dimensions are resized
        - SubSurface array boundaries are NOT resized
    */
    void encloseTrackingVolume(const TrackingVolume& tvol) const;
    
    void encloseDetachedTrackingVolume(const DetachedTrackingVolume& tvol) const;   //!< private method to set the enclosed detached TV

  protected:
    /** resize layer to the TrackingVolume dimensions - to be overloaded by the extended classes*/
    virtual void resizeLayer(const VolumeBounds&, double envelope=1.) const = 0;

    /** resize and reposition layer : dedicated for entry layers */
    virtual void resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& vCenter, double envelope=1.) const = 0;

    SurfaceArray*                              m_surfaceArray;              //!< SurfaceArray on this layer Surface
    mutable LayerMaterialProperties*           m_layerMaterialProperties;   //!< MaterialPoperties of this layer Surface
    double                                     m_layerThickness;            //!< thickness of the Layer

    OverlapDescriptor*                         m_overlapDescriptor;         //!< descriptor for overlap/next surface
      
    // These are stored by pointers and never deleted as they belong to the Volume
    mutable const Layer*                       m_previousLayer;             //!< the previous Layer according to BinGenUtils 
    mutable const Layer*                       m_nextLayer;                 //!< next Layer according to BinGenUtils 
    mutable const BinUtility*                  m_binUtility;                //!< BinUtility for next/previous decission
      
    mutable const TrackingVolume*              m_enclosingTrackingVolume;   //!< Enclosing TrackingVolume

    mutable const DetachedTrackingVolume*      m_enclosingDetachedTrackingVolume; //!< Enclosing DetachedTrackingVolume   

    mutable LayerIndex                         m_index;                     //!< LayerIndex

    mutable int                                m_layerType;                 //!< active passive layer

    mutable const Volume*                      m_representingVolume;        //!< Representing Trk::Volume

    mutable double                             m_ref;                       //!< reference measure for local coordinate convertors
  };

  
  inline const SurfaceArray* Layer::surfaceArray() const 
    { return m_surfaceArray; }

  inline double Layer::thickness() const
    { return m_layerThickness; }


  inline const LayerMaterialProperties* Layer::layerMaterialProperties() const{ return m_layerMaterialProperties; }

  inline const OverlapCell Layer::overlapSurface(const TrackParameters& tp,
                                                 const Surface* sf,
                                                 PropDirection dir) const
    {   if (m_overlapDescriptor) return m_overlapDescriptor->overlapSurface(tp, sf, dir);
        return Trk::OverlapCell(0,dir);
    }
    
  inline const OverlapDescriptor* Layer::overlapDescriptor() const 
    {
     return m_overlapDescriptor;
    }

  inline const TrackingVolume* Layer::enclosingTrackingVolume() const 
    { return m_enclosingTrackingVolume; }

  inline void Layer::encloseTrackingVolume(const TrackingVolume& tvol) const 
    { 
     m_enclosingTrackingVolume = &(tvol); 
    }

  inline const DetachedTrackingVolume* Layer::enclosingDetachedTrackingVolume() const 
    { return m_enclosingDetachedTrackingVolume; }

  inline void Layer::encloseDetachedTrackingVolume(const DetachedTrackingVolume& tvol) const 
    { m_enclosingDetachedTrackingVolume = &(tvol); }

  inline const LayerIndex& Layer::layerIndex() const
    { return m_index; }

  inline int Layer::layerType() const
    { return m_layerType; }

  inline void Layer::setLayerType( int id ) const
    { m_layerType = id; }

  inline const Volume* Layer::representingVolume() const
    { return m_representingVolume; }

  inline void Layer::registerRepresentingVolume(const Volume *theVol) const
    { m_representingVolume = theVol; }
  
   inline void Layer::registerLayerIndex(const LayerIndex& lIdx) const
    { m_index = lIdx; }  
    
  inline double Layer::getRef() const
    { return m_ref; }
  
  inline void Layer::setRef(double x) const
    { m_ref = x; }
  
} // end of namespace

#endif // TRKGEOMETRY_LAYER_H



