/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Layer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_LAYER_H
#define TRKGEOMETRY_LAYER_H

class MsgStream;

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

namespace Trk {
  
  class Surface;
  class MaterialProperties;
  class BinUtility;
  class Volume;
  class VolumeBounds;
  class TrackingVolume;
  class DetachedTrackingVolume;
  class ICompatibilityEstimator;


  typedef ObjectIntersection<Surface> SurfaceIntersection;
  typedef BinnedArray<Surface>        SurfaceArray;

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
          int ltype=int(passive));
                
    /**Constructor with pointer to SurfaceArray (passing ownership) */
    Layer(SurfaceArray* surfaceArray,
          double thickness = 0.,
          OverlapDescriptor* od = 0,
          int ltype=int(passive));
                
    /**Constructor with MaterialProperties and pointer SurfaceArray (passing ownership) */
    Layer(SurfaceArray* surfaceArray,
          const LayerMaterialProperties& laymatprop,
          double thickness = 0.,
          OverlapDescriptor* od = 0,
          int ltype=int(passive)); 
        
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
            
    /** templated on Layer method */
    template <class T> bool onLayer(const T& parameters, const BoundaryCheck& bcheck  = BoundaryCheck(true)) const;        
            
    /** isOnLayer() method, using isOnSurface() with Layer specific tolerance */
    virtual bool isOnLayer(const Amg::Vector3D& gp, const BoundaryCheck& bcheck = BoundaryCheck(true)) const;
        
    /** getting the LayerMaterialProperties including full/pre/post update */
    const LayerMaterialProperties* layerMaterialProperties() const;

    /** getting the MaterialProperties back - for full update*/ 
    const MaterialProperties* fullUpdateMaterialProperties(const TrackParameters& par) const;
        
    /** getting the MaterialProperties back - for pre-update*/ 
    virtual double preUpdateMaterialFactor(const TrackParameters&, PropDirection) const { return 1.; }

    /** getting the MaterialProperties back - for pre-update*/ 
    virtual double postUpdateMaterialFactor(const TrackParameters&,
                                            PropDirection) const { return 0.; }
                                                                      
    /** gettint hte overlap descriptor */         
    const OverlapDescriptor* overlapDescriptor() const;
    
    /** getting the next/previous Layer if registered - unit for direction vector required */          
    const Layer* nextLayer(const Amg::Vector3D& gp, const Amg::Vector3D& udir) const;    
    
    /** getting what's stored to be the previous Layer, boolean to skip navigation layers */
    const Layer* previousLayer(bool skipNavLayer=false) const;                                   

    /** getting what's stored to be the next Layer, boolean to skip navigation layers*/
    const Layer* nextLayer(bool skipNavLayer=false) const;                                   

    /** Surface seen on approach - if not defined differently, it is the surfaceRepresentation() */
    virtual const Surface& surfaceOnApproach(const Amg::Vector3D& pos,
                                             const Amg::Vector3D& dir,
                                             PropDirection pdir,
                                             const BoundaryCheck& bcheck,
                                             bool resolveSubSurfaces = 0,
                                             const ICompatibilityEstimator* ice = 0) const;

    /** get compatible surfaces starting from charged parameters */
    virtual size_t compatibleSurfaces(std::vector<SurfaceIntersection>& cSurfaces,
			                          const TrackParameters& pars,
			                          PropDirection pdir,
			                          const BoundaryCheck& bcheck,
                                      bool materialSurfacesOnly = true,
			                          const Surface* startSurface = 0,
			                          const Surface* endSurface = 0,
			                          const ICompatibilityEstimator* ice = 0) const;

    /** get compatible surfaces starting from neutral parameters */
    virtual size_t compatibleSurfaces(std::vector<SurfaceIntersection>& cSurfaces,
    			                      const NeutralParameters& pars,
    			                      PropDirection pdir,
    			                      const BoundaryCheck& bcheck,
                                      bool materialSurfacesOnly = true,
    			                      const Surface* startSurface = 0,
    			                      const Surface* endSurface = 0,
    			                      const ICompatibilityEstimator* ice = 0) const;

    /** Has sub-structure method: 
        - sub-structure depending on : 
          (a) only when required to resolve sub surfaces for sensitive hits 
          (b) also material is ordered with sub structure */
    virtual bool hasSubStructure(bool resolveSensitive=false) const;

    /** get the confining TrackingVolume */
    const TrackingVolume* enclosingTrackingVolume() const;

    /** get the confining DetachedTrackingVolume */
    const DetachedTrackingVolume* enclosingDetachedTrackingVolume() const;

    /** get the layerIndex */
    const LayerIndex&  layerIndex() const;

    /** get the Layer codinging */
    int layerType() const;

    /** set the Layer codinging */
    void setLayerType(int identifier) const;

    /** boolean method to check if the layer needs a LayerMaterialProperties */
    bool needsMaterialProperties() const;
    
    /** assignMaterialPropeties - \todo make method private */
    void assignMaterialProperties(const LayerMaterialProperties&, double scale=1.0) const;    

    /** move the Layer */
    virtual void moveLayer( Amg::Transform3D& ) const {};

    /**register Volume associated to the layer */
    void registerRepresentingVolume(const Volume *theVol) const;
    
    /** get the Volume associated to the layer */
    const Volume* representingVolume() const;

    /** set the reference measure */
    void setRef(double ) const;

    /** get the reference measure */
    double getRef() const;

  private:
    /** get compatible surfaces starting from charged parameters */
    template <class T>  size_t getCompatibleSurfaces(std::vector<SurfaceIntersection>& cSurfaces,
                                                     const T& pars,
                                                     PropDirection pdir,
                                                     const BoundaryCheck& bcheck,
                                                     bool materialSurfacesOnly = true,
                                                     const Surface* startSurface = 0,
                                                     const Surface* endSurface = 0,
                                                     const ICompatibilityEstimator* ice = 0) const;

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
    virtual void resizeLayer(const VolumeBounds&, double) const {}

    /** resize and reposition layer : dedicated for entry layers */
    virtual void resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& vCenter, double envelope=1.) const = 0;

    SurfaceArray*                                   m_surfaceArray;              //!< SurfaceArray on this layer Surface
    mutable SharedObject<LayerMaterialProperties>   m_layerMaterialProperties;   //!< MaterialPoperties of this layer Surface
    double                                          m_layerThickness;            //!< thickness of the Layer

    OverlapDescriptor*                              m_overlapDescriptor;         //!< descriptor for overlap/next surface
      
    // These are stored by pointers and never deleted as they belong to the Volume
    mutable const Layer*                            m_previousLayer;             //!< the previous Layer according to BinGenUtils 
    mutable const Layer*                            m_nextLayer;                 //!< next Layer according to BinGenUtils 
    mutable const BinUtility*                       m_binUtility;                //!< BinUtility for next/previous decission
                                                    
    mutable const TrackingVolume*                   m_enclosingTrackingVolume;   //!< Enclosing TrackingVolume
                                                    
    mutable const DetachedTrackingVolume*           m_enclosingDetachedTrackingVolume; //!< Enclosing DetachedTrackingVolume   
                                                    
    mutable LayerIndex                              m_index;                     //!< LayerIndex
                                                    
    mutable int                                     m_layerType;                 //!< active passive layer
                                                    
    mutable const Volume*                           m_representingVolume;        //!< Representing Volume
                                                    
    mutable double                                  m_ref;                       //!< reference measure for local coordinate convertors
  };

  
  inline const SurfaceArray* Layer::surfaceArray() const 
    { return m_surfaceArray; }

  inline double Layer::thickness() const
    { return m_layerThickness; }
  
  template <class T> bool Layer::onLayer(const T& pars, const BoundaryCheck& bcheck) const {
      // simple check first .. compare surfaces if parameters are AtaSurface 
      if ( pars.type() == AtaSurface ) {
         // surface based association 
         if ( &pars.associatedSurface() ==  &surfaceRepresentation() )
              return (bcheck ?  surfaceRepresentation().insideBoundsCheck(pars.localPosition(),bcheck) : true);
         // layer based association
         if ( (pars.associatedSurface().associatedLayer() == this) && !bcheck )
             return true;
      }
      return isOnLayer(pars.position(),bcheck);
  }

  /** returns all Compatible surfaces with given BoundaryCheck */
  template <class T> size_t Layer::getCompatibleSurfaces(std::vector<SurfaceIntersection>& cSurfaces,
                                                         const T& pars,
                                                         PropDirection pDir,
                                                         const BoundaryCheck& bcheck,
                                                         bool materialSurfacesOnly,
                                                         const Surface* startSurface,
                                                         const Surface* endSurface,
                                                         const ICompatibilityEstimator*) const
  {
    
      bool robustMode = false;
      
      if (robustMode) {
        // Temp code to return all surfaces
        // position and momentum/dir 
        const Amg::Vector3D& pos = pars.position();
        const Amg::Vector3D  dir = (pDir == oppositeMomentum) ? Amg::Vector3D(-1.*pars.momentum().unit()) : pars.momentum().unit() ;
        
        // check if you need to force the momentum direction
        bool fDirection = ( pDir == anyDirection ? false : true );
        
        // check if you have to stop at the endSurface
        double maxPathLength = 10e10;
        if (endSurface){
          // intersect the end surface
          Intersection endInter = endSurface->straightLineIntersection(pos,dir,fDirection,bcheck);
          // non-valid intersection with the end surface provided at this layer indicates wrong direction or faulty setup
          // -> do not return compatible surfaces since they may lead you on a wrong navigation path 
          if (endInter.valid && endInter.pathLength > 0.)
            maxPathLength = endInter.pathLength;
          else return 0;
        }
        const std::vector<const Trk::Surface*>& AllSurfaces = m_surfaceArray->arrayObjects();
        // get a reference surface
        cSurfaces.clear();
        for (unsigned int myloop = 0; myloop < AllSurfaces.size(); myloop++) {
          if (AllSurfaces.at(myloop)) {
            if (AllSurfaces.at(myloop) == endSurface || AllSurfaces.at(myloop) == startSurface) continue;
            //std::cout << AllSurfaces.at(myloop) << "  " << pos << "  " << dir << "  " << fDirection << "  " << bcheck << std::endl;
            Intersection myInter = (AllSurfaces.at(myloop))->straightLineIntersection(pos,dir,fDirection,bcheck);
            //std::cout << "Intersection Info " << myInter.valid << ",  " << myInter.pathLength << ",  " << maxPathLength << std::endl;
            // allow only if it is in the maximal path length
            if (myInter.valid && myInter.pathLength < maxPathLength)
              cSurfaces.push_back(SurfaceIntersection(myInter,AllSurfaces.at(myloop),pDir));
          }
        }
        // now sort it
        std::sort(cSurfaces.begin(),cSurfaces.end());
        return cSurfaces.size();
      }
      
      // fast exit - nothing to do
      if (!m_surfaceArray || !m_overlapDescriptor) return 0;

      // position and momentum/dir 
      const Amg::Vector3D& pos = pars.position();
      const Amg::Vector3D  dir = (pDir == oppositeMomentum) ? Amg::Vector3D(-1.*pars.momentum().unit()) : pars.momentum().unit() ;

      // check if you need to force the momentum direction
      bool fDirection = ( pDir == anyDirection ? false : true );
      
      // check if you have to stop at the endSurface
      double maxPathLength = 10e10;
      if (endSurface){
          // intersect the end surface
          Intersection endInter = endSurface->straightLineIntersection(pos,dir,fDirection,bcheck);
          // non-valid intersection with the end surface provided at this layer indicates wrong direction or faulty setup
          // -> do not return compatible surfaces since they may lead you on a wrong navigation path 
          if (endInter.valid && endInter.pathLength > 0.)
              maxPathLength = endInter.pathLength;
          else return 0;
      }
      
      // clear the vector, just in case
      cSurfaces.clear();

      // create a new for for the moment because there are two different modes:
      // - the layer does the intersection already
      // - you do the intersection  
      // !< TODO harmonise this
      std::vector<SurfaceIntersection> testSurfaces;
      
      // get the main target surface
      const Surface* tSurface = subSurface(pos);
      bool acceptSurfaces = (tSurface) ?
        m_overlapDescriptor->reachableSurfaces(testSurfaces, *tSurface, pos, dir) : m_overlapDescriptor->reachableSurfaces(testSurfaces, pos, dir);
      // boolean said you can directly take the surfaces from the reachable surfaces
      if (acceptSurfaces) {
          // no start nor end surface is given - accept totally if not configured to only collect material surfaces
          if (!startSurface && !endSurface && !materialSurfacesOnly)
              cSurfaces = testSurfaces;
          else { // endSurface was given - check for maxPathLength && endSurface
              for (auto& tSurface : testSurfaces){
                  // exclude the startSurface and endSurface from this loop 
                  if (tSurface.object == endSurface || tSurface.object == startSurface) continue;
                  // accept if in path range
                  if (tSurface.intersection.pathLength < maxPathLength && (!materialSurfacesOnly || tSurface.object->materialLayer()) ) 
                      cSurfaces.push_back(tSurface);
              }
          }
      } else if (testSurfaces.size()) {
          for (auto& tSurface : testSurfaces){
              // exclude the endSurface
              if (tSurface.object == endSurface || tSurface.object == startSurface) continue;
              // minimize the computational cost
              Intersection tsfInter = tSurface.object->straightLineIntersection(pos,dir,fDirection,false);
              // check if the intersection is valid and the maxPathLength has not been exceeded
              if (tsfInter.valid && tsfInter.pathLength < maxPathLength ){
                  // resulting propDirection
                  PropDirection rDir = fDirection ? pDir : ( tsfInter.pathLength > 0 ? alongMomentum : oppositeMomentum );
                  // and the surfaces & direction to push back - take only material surfaces if configured to do so
                  if (!materialSurfacesOnly || tSurface.object->materialLayer())
                      cSurfaces.push_back(SurfaceIntersection(tsfInter,tSurface.object,rDir));
              } 
          }
      }
      
      // the layer surface itself is a testSurface - if there's material
      const Surface* layerSurface = &surfaceRepresentation();
      if (layerMaterialProperties() &&  layerSurface != startSurface && layerSurface != endSurface ){
          // self intersection
          Intersection lInter = surfaceRepresentation().straightLineIntersection(pos,dir,fDirection,bcheck);
          // allow only if it is in the maximal path length 
          if (lInter.valid && lInter.pathLength < maxPathLength)
              cSurfaces.push_back(SurfaceIntersection(lInter,layerSurface,pDir));
      }
      // now sort it 
      std::sort(cSurfaces.begin(),cSurfaces.end());
      // return
      return cSurfaces.size();                                                                 
  }

  inline const LayerMaterialProperties* Layer::layerMaterialProperties() const{ return m_layerMaterialProperties.getPtr(); }

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



