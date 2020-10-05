/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolume.h, (b) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_TRACKINGVOLUME_H
#define TRKGEOMETRY_TRACKINGVOLUME_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkVolumes/Volume.h"
#include "TrkVolumes/AbstractVolume.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundaryDiscSurface.h"
#include "TrkVolumes/BoundaryCylinderSurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/LayerAttemptsCalculator.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/ObjectAccessor.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkEventPrimitives/PropDirection.h"
//STL
#include <string>
// ATH_MSG macros
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "CxxUtils/checker_macros.h"
#ifndef TRKGEOMETRY_MAXLAYERATTEMPTS
#define TRKGEOMETRY_MAXLAYERATTEMPTS 100
#endif

class MsgStream;

namespace Trk {

  // classes 
  class Layer;
  class PlaneLayer;
  class TrackingVolume;
  class DetachedTrackingVolume;
  class GlueVolumesDescriptor;
  class VolumeBounds;

  typedef BinnedArray< Layer >            LayerArray;                         
  typedef BinnedArray< TrackingVolume >   TrackingVolumeArray;
  
  template <class T> using LayerIntersection            = FullIntersection< Layer, Surface, T >;
  template <class T> using BoundaryIntersection         = FullIntersection< BoundarySurface<TrackingVolume>, Surface, T >;
 
  /**
   @class TrackingVolume
     
   Full Volume description used in Tracking,
   it inherits from Volume to get the geometrical structure,
   such as from MaterialProperties and from MagneticFieldProperties   

       A TrackingVolume can provide the (layer) material information / internal navigation with in
       5 different ways:

           --- a) Static confinement of Layers
           --- b) detached sub volumes 
           --- b) unordered (arbitrarily oriented) layers
           --- d) unordered sub volumes 
           --- e) unordered layers AND unordered subvolumes
 
   Static configurations can have a BendingCorrector that allows for correction of the path
   due to the bending of the track

           --- dedicated entry/exit layers can be defined for holding 
               two perpenticular layers that are accessed in an entry/exit action

   In addition it is capable of holding a subarray of Layers and TrackingVolumes,
   Layers and TrackingVolumes are then owned by the TrackingVolume holding them.
  
   @author Andreas.Salzburger@cern.ch
   */

   class TrackingVolume : public Volume,
                          public Material {
                          
    /** The TrackingVolumeManipulator has to be a friend */
    friend class TrackingVolumeManipulator;                       
    
    /** Give the TrackingGeometry friend rights */  
    friend class TrackingGeometry;
    
    /** Give the DetachedTrackingVolume friend rights */  
    friend class DetachedTrackingVolume;
                                             
    public:
      /** Default Constructor */
      TrackingVolume();
      
      /** Constructor for a full equipped Tracking Volume 
      - explicitely  ======> 1 a) static confinement */
      TrackingVolume (Amg::Transform3D* htrans,
                      VolumeBounds* volbounds,
                      const LayerArray* subLayers=nullptr,
                      const TrackingVolumeArray* subVolumes=nullptr,
                      const std::string& volumeName="undefined");
      
      /** Constructor for a full equipped Tracking Volume
         - full by inheritance  ======> 2 a) static confinement */
      TrackingVolume (const Volume& volume,
                      const Material& matprop,
                      const LayerArray* subLayers=nullptr,
                      const TrackingVolumeArray* subVolumes=nullptr,
                      const std::string& volumeName="undefined");
                     
      /** Constructor for a full equipped Tracking Volume
        - mixed  ======> 3 a) static confinement */
      TrackingVolume (Amg::Transform3D* htrans,
                      VolumeBounds*   volbounds,
                      const Material& matprop,
                      const LayerArray* subLayers=nullptr,
                      const TrackingVolumeArray* subVolumes=nullptr,
                      const std::string& volumeName="undefined");


      /** Constructor for a full equipped Tracking Volume with detached subvolumes
        -  mixed =======> 1 b) detached volumes */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const Material& matprop,
                     const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Constructor for a full equipped Tracking Volume with detached subvolumes  
        -  mixed =======> 2 b) detached volumes */
      TrackingVolume(const Volume& volume,
                     const Material& matprop,
                     const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Constructor for a full equipped Tracking Volume with unordered subvolumes
        - mixed =======> 1 d) unordered volumes  */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const Material& matprop,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Constructor for a full equipped Tracking Volume with unordered subvolumes 
        - mixed =======> 2 d) unordered volumes  */
      TrackingVolume(const Volume& volume,
                     const Material& matprop,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;
      
      /** Constructor for a full equipped Tracking Volume with arbitrary layers
        -  mixed =======> 1 c) arbitrarily oriented layers */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const Material& matprop,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Constructor for a full equipped Tracking Volume with arbitrary layers
        -  mixed =======> 2 c) arbitrarily oriented layers */
      TrackingVolume(const Volume& volume,
                     const Material& matprop,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Constructor for a full equipped Tracking Volume with arbitrary layers AND subVolumes - 
        -  mixed =======> 1 e) unordered layers AND unordered subvolumes */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const Material& matprop,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** Constructor for a full equipped Tracking Volume with arbitrary layers AND subVolumes - 
        -  mixed =======> 2 e) unordered layers AND unordered subvolumes */
      TrackingVolume(const Volume& volume,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const Material& matprop,
                     const std::string& volumeName="undefined")
        ATLAS_CTORDTOR_NOT_THREAD_SAFE;

      /** copy constructor with shift */
      TrackingVolume(const TrackingVolume& trVol, Amg::Transform3D& transform);
      
      /** Destructor */
      ~TrackingVolume() override;
      
      /** Return the associated Layer */
      const Layer*   associatedLayer(const Amg::Vector3D& gp) const;
      
      /** Return the next Layer if existing, NULL if no next layer corresponds */
      const Layer*   nextLayer(const Amg::Vector3D& gp, 
                               const Amg::Vector3D& mom,
                               bool asres=true,
                               bool skipNavLayer=false) const;
      
      /** Return the entry Layer to a TrackingVolume depending on the entry point **/
      const Layer* entryLayer(const Amg::Vector3D &gp, const Amg::Vector3D& dir) const;

      /** Return the exit Layer from a TrackingVolume */
      const Layer* exitLayer(const Amg::Vector3D& gp, const Amg::Vector3D& dir) const;

      /** Return the material layers ordered based on straight line intersections:
          - startLayer and endLayer are included in the list 
      */
      template <class T> std::vector< LayerIntersection<T> >
                          materialLayersOrdered(const Layer* sLayer,
                                                const Layer* eLayer,
                                                const T& parameters, 
                                                PropDirection pDir = alongMomentum,
                                                const BoundaryCheck& bchk = true,
                                                bool resolveSubSurfaces = false) const;


      /** Return the closest layer with material description 
            - it assumes to be on lowest navigation level
            - does not step down to eventually confined TrackingVolumes
            - navigates to the next trackign volume IF PropDirection == mappingMode
        */
      LayerIntersection<Amg::Vector3D>
                            closestMaterialLayer(const Amg::Vector3D& gp, 
                                                 const Amg::Vector3D& dir, 
                                                 PropDirection pDir = alongMomentum,
                                                 const BoundaryCheck& bchk = true ) const;
            
      /** Return the associated sub Volume, returns THIS if no subVolume exists */
      const TrackingVolume* associatedSubVolume(const Amg::Vector3D& gp) const;
      
      /** Return the next volume along the navigation stream */
      const TrackingVolume* nextVolume(const Amg::Vector3D& gp, 
                                       const Amg::Vector3D& dir, 
                                       PropDirection pDir = alongMomentum) const;
      
      /** Return the next sub Volume if existing, returns THIS if no subVolume exists */
      const TrackingVolume* nextSubVolume(const Amg::Vector3D& gp, const Amg::Vector3D& dir) const;
      
      /** Return the associated detached subvolumes */
      std::vector<const DetachedTrackingVolume*>* assocDetachedSubVolumes(const Amg::Vector3D& gp, double tol) const;
   
      /** Layer attempts - as provided by the LayerAttemptCreator */
      unsigned int layerAttempts(BoundarySurfaceFace exitFace) const;

      /** Layer attempts - as provided by the LayerAttemptCreator */
      unsigned int maxLayerAttempts() const;

      /** Return the subLayer array - not the ownership*/
      const LayerArray* confinedLayers() const;

      /** Return the subLayer array - not the ownership*/
      const std::vector<const Layer*>* confinedArbitraryLayers() const;
      
      /** Return the subLayerarray including the ownership*/
      const LayerArray* checkoutConfinedLayers() const;

      /** Return the subLayer array */
      const TrackingVolumeArray* confinedVolumes() const;

      /** Return detached subVolumes - not the ownership */
      const std::vector<const DetachedTrackingVolume*>* confinedDetachedVolumes() const;

      /** Return unordered subVolumes - not the ownership */
      const std::vector<const TrackingVolume*>* confinedDenseVolumes() const;
      
      /** Returns the VolumeName - for debug reason, might be depreciated later */
      const std::string& volumeName() const;
      
      /** Method to return the BoundarySurfaces */
      const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >& boundarySurfaces() const;
      
      /** Returns the boundary surfaces ordered in probability to hit them based on straight line intersection */
      template <class T> std::vector< BoundaryIntersection<T> > 
                         boundarySurfacesOrdered(const T& parameters, 
                                                 PropDirection pDir = alongMomentum,
                                                 bool startOffBoundary = false) const;
      
      /** Get the BoundarySurface to the appointed Accessor state */
      const BoundarySurface<TrackingVolume>* boundarySurface(const ObjectAccessor::value_type& oa) const;
      
      /** show if you are on a boundary surface */
      template <class T> bool onVolumeBoundary(const T& pars ) const;
      
      /** Register the outside glue volumes -
          ordering is in the TrackingVolume Frame:
           - negativeFaceXY
           - (faces YZ, ZY, radial faces)
           - positiveFaceXY
      */

      void registerOutsideGlueVolumes (GlueVolumesDescriptor* gvd);            
      void registerOutsideGlueVolumes ATLAS_NOT_THREAD_SAFE (GlueVolumesDescriptor* gvd) const;            
      
      const GlueVolumesDescriptor& glueVolumesDescriptor();
      const GlueVolumesDescriptor& glueVolumesDescriptor ATLAS_NOT_THREAD_SAFE() const;

      /** the sensitive area */
      void registerSensitiveVolume(const AbstractVolume* svol);
      void registerSensitiveVolume ATLAS_NOT_THREAD_SAFE (const AbstractVolume* svol) const;
      
      /** return the sensitive volume */
      const AbstractVolume* sensitiveVolume() const;

      /** return the sensitive volume - gives ownership*/
      const AbstractVolume* checkoutSensitiveVolume() const;

      /** sign the volume - the geometry builder has to do that */
      void sign ATLAS_NOT_THREAD_SAFE(GeometrySignature signat, GeometryType gtype = Static) const;

      /** return the Signature */
      GeometrySignature geometrySignature() const;
      
      /** return the Signature */
      GeometryType geometryType() const;
                        
      /** Register the color code */
      void registerColorCode(unsigned int icolor);
      void registerColorCode ATLAS_NOT_THREAD_SAFE (unsigned int icolor) const;

      /** Get the color code */
      unsigned int colorCode() const; 

      /** force a navigation check */
      void forceNavigationCheck();
      void forceNavigationCheck ATLAS_NOT_THREAD_SAFE() const;
      
      /** Boolean, if true navigation needs to be redone when hitting this volume */
      bool redoNavigation() const; 

      /** Return the MotherVolume - if it exists */
      const TrackingVolume* getMotherVolume() const;

      /** Return the MotherVolume - if it exists */
      void setMotherVolume(const TrackingVolume* mvol);
      void setMotherVolume ATLAS_NOT_THREAD_SAFE (const TrackingVolume* mvol) const;

      /** move Volume */
      void moveVolume ATLAS_NOT_THREAD_SAFE ( Amg::Transform3D& shift ) const;

      /** add Material */
      void addMaterial( const Material& mat, float fact=1. );
      void addMaterial ATLAS_NOT_THREAD_SAFE (const Material& mat, float fact=1.) const ;

      virtual bool isAlignable () const;
      /** remove content */
      void clear();

      void screenDump(MsgStream& msg) const;

    protected:

      /** clone at new position */
      const TrackingVolume* cloneTV ATLAS_NOT_THREAD_SAFE(Amg::Transform3D& transform) const;
            
    private:          
      /** reIndex the static layers of the TrackingVolume */
      void indexContainedStaticLayers ATLAS_NOT_THREAD_SAFE(GeometrySignature geoSig, int& offset) const;

      /** reIndex the material layers of the TrackingVolume */
      void indexContainedMaterialLayers ATLAS_NOT_THREAD_SAFE (GeometrySignature geoSig, int& offset) const;
      
      /** propagate material properties to subvolumes */
      void propagateMaterialProperties ATLAS_NOT_THREAD_SAFE (const Material& mprop);
      void propagateMaterialProperties ATLAS_NOT_THREAD_SAFE (const Material& mprop) const;
      
      /** Create Boundary Surface */
      void createBoundarySurfaces ATLAS_NOT_THREAD_SAFE ();
      
      /** Create Layer Attempts Caluclator */
      void createLayerAttemptsCalculator();
      
      /** compactify the memory usage in the event by setting ownership to TackingGeometry 
        the referenced types are the number of registered surfaces & total surfaces
      */
      void compactify ATLAS_NOT_THREAD_SAFE (size_t& rSurfaces, size_t& tSurfaces) const;

      /** method to synchronize the layers with potentially updated volume bounds:
          - adapts the layer dimensions to the new volumebounds + envelope
          - adapts entry layer position where necessary to the new volumebounds 
      */
      void synchronizeLayers ATLAS_NOT_THREAD_SAFE (MsgStream& msgstream, double envelope = 1.) const;

      /** Register Next - Previous for Layers, set volumelink */
      void interlinkLayers ATLAS_NOT_THREAD_SAFE();

      /** Helper method - find closest of two layers */
      const Layer* closest(const Amg::Vector3D& pos,
                                const Amg::Vector3D& dir,
                                const Layer& first,
                                const Layer& second) const;
                                
      
      /** move the Tracking Volume*/
      void moveTV ATLAS_NOT_THREAD_SAFE(Amg::Transform3D& transform) const;
           
      /** Forbidden copy constructor */   
      TrackingVolume(const TrackingVolume&) = delete;

      /** Forbid assignment. */
      TrackingVolume &operator=(const TrackingVolume&) = delete;
       
      const TrackingVolume*        m_motherVolume;  //!< mother volume of this volume

      std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >* m_boundarySurfaces;  //!< boundary Surfaces
      //(a)
      const LayerArray*                                                     m_confinedLayers;   //!< Array of Layers inside the Volume
      const TrackingVolumeArray*                                            m_confinedVolumes;  //!< Array of Volumes inside the Volume
      //(b)                                                                 
      const std::vector<const DetachedTrackingVolume*>*                     m_confinedDetachedVolumes; //!< Detached subvolumes
      // additionally                                                            
      const std::vector<const TrackingVolume*>*                             m_confinedDenseVolumes;    //!< Unordered subvolumes
      //(b)                                                                      
      const std::vector<const Layer*>*                                      m_confinedArbitraryLayers; //!< Unordered Layers inside the Volume
                                                                            
      GlueVolumesDescriptor*                                                m_outsideGlueVolumes;      //!< Volumes to glue Volumes from the outside
                                                                            
      const AbstractVolume*                                                 m_sensitiveVolume;         //!< Sensitive volume
                                                                            
      LayerAttemptsCalculator*                                              m_layerAttemptsCalculator; //!< provided the number of layer attempts
                                                                            
      GeometrySignature            m_geometrySignature;             //!< The Signature done by the GeometryBuilder 
      GeometryType                 m_geometryType;                  //!< defines how the Extrapolator propagates through this
                                                                            
      std::string                  m_name;                          //!< Volume name for debug reasons
      unsigned int                 m_colorCode;                     //!< Color code for displaying 
      bool                         m_redoNavigation;                //!< Navigation boolean. If true navigation needs to be redone when entering this volume

    public:
      

  };

  inline const std::string& TrackingVolume::volumeName() const { return m_name; }
 
  inline unsigned int TrackingVolume::layerAttempts(BoundarySurfaceFace exitFace) const
  {
      if (m_layerAttemptsCalculator) return m_layerAttemptsCalculator->layerAttempts(exitFace);
      return TRKGEOMETRY_MAXLAYERATTEMPTS;
  }

  inline unsigned int TrackingVolume::maxLayerAttempts() const
  {
      if (m_layerAttemptsCalculator) return m_layerAttemptsCalculator->maxLayerAttempts();
      return TRKGEOMETRY_MAXLAYERATTEMPTS;
  }

  inline const LayerArray*  TrackingVolume::confinedLayers() const
  { return m_confinedLayers; }

  inline const std::vector<const Layer*>* TrackingVolume::confinedArbitraryLayers() const 
  { return m_confinedArbitraryLayers; }

  inline const TrackingVolumeArray*  TrackingVolume::confinedVolumes() const
  { return m_confinedVolumes; }

  inline const  std::vector<const DetachedTrackingVolume*>*  TrackingVolume::confinedDetachedVolumes() const 
  { return m_confinedDetachedVolumes; }

  inline const  std::vector<const TrackingVolume*>*  TrackingVolume::confinedDenseVolumes() const 
  { return m_confinedDenseVolumes; }

  inline void TrackingVolume::registerSensitiveVolume(const AbstractVolume* svol)
  { m_sensitiveVolume = svol; }
  inline void TrackingVolume::registerSensitiveVolume ATLAS_NOT_THREAD_SAFE (const AbstractVolume* svol) const{
    const_cast<TrackingVolume&>(*this).registerSensitiveVolume(svol);
  }

  inline const AbstractVolume* TrackingVolume::sensitiveVolume() const
  { return m_sensitiveVolume; }

  inline const AbstractVolume* TrackingVolume::checkoutSensitiveVolume() const{
      const AbstractVolume* returnVolume = m_sensitiveVolume;
      return returnVolume; 
  }
    
  template <class T> bool TrackingVolume::onVolumeBoundary(const T& pars) const {
      // get the associated Surface
      const Surface* pSurface = &pars.associatedSurface();
      auto& bSurfaces = boundarySurfaces();
      // fast loop pointer comparison of the surfaces 
      for (auto& bsIter : bSurfaces ){
          const BoundarySurface<TrackingVolume>* bSurface = bsIter.get();
          // pointer of the parameter surface is identical with one of the boundary surface pointers
          if (pSurface == &bSurface->surfaceRepresentation()) return true;
      }
      // slow loop - checking the onSurface (does pointer comparison as well)
      for (auto& bsIter : bSurfaces ){
          const BoundarySurface<TrackingVolume>* bSurface = bsIter.get();
          // pointer of the parameter surface is identical with one of the boundary surface pointers
          if (bSurface->onBoundary(pars)) return true;
      }
      // could not find an onSurface
      return false;
  }
     

  /** Return the material layers ordered based on straight line intersections 
      - start and end layer are always part of it
  */
  template <class T> std::vector< LayerIntersection<T> >
                       TrackingVolume::materialLayersOrdered(const Layer* sLayer,
                                                             const Layer* eLayer,
                                                             const T& pars, 
                                                             PropDirection pDir,
                                                             const BoundaryCheck& bchk,
                                                             bool resolveSubSurfaces) const
  {
      // get position and momentum from the parameters
      const Amg::Vector3D& gp = pars.position();
      const Amg::Vector3D& gm = pars.momentum();
      // the layer intersections 
      std::vector< LayerIntersection<T> > lIntersections;
      // assign the direction
      const Amg::Vector3D& dir = ( pDir == alongMomentum ? gm.unit() : Amg::Vector3D(-1*gm.unit()));
      // the confinedLayers
      if (m_confinedLayers){
          // cache the longest path length to avoid punch-through to the other side
          Trk::Intersection     sLayerIntersection(Amg::Vector3D(0.,0.,0),0.,true,0.);
          const Trk::Surface*   sLayerSurface  = nullptr;
          double validPathLength = 0.;
          // start layer given or not - test layer
          const Trk::Layer* tLayer = sLayer ? sLayer : associatedLayer(gp);
          if (tLayer){
              do {
                  // collect material or sensitive layers, always provide the final layer for the navigation stop
                  if (tLayer->layerMaterialProperties() || tLayer->surfaceArray() || tLayer == eLayer){
                      // get the approaching surface 
                      const Surface& tSurface = tLayer->surfaceOnApproach(gp,dir,pDir,bchk,resolveSubSurfaces);
                      // calculate the intersection with the layer
                      Trk::Intersection lIntersection = tSurface.straightLineIntersection(gp,dir,true,bchk);
                      // (a) if the current layer is NOT the start layer - intersection is ok
                      if (tLayer != sLayer && lIntersection.valid){
                          lIntersections.push_back(LayerIntersection<T>(lIntersection,tLayer,&tSurface,0,pDir));
                          validPathLength = lIntersection.pathLength;
                      } else if (tLayer == sLayer) {
                          // (b) the current layer is the start layer - we need to cache it and check with the path length
                          //     this avoids potential punch-through to other side of 
                          sLayerIntersection = lIntersection;
                          sLayerSurface      = &tSurface;
                      } else if (tLayer == eLayer) {
                          // (c) it is the end layer after all - provide it and break the loop
                          lIntersections.push_back(LayerIntersection<T>(lIntersection,tLayer,&tSurface,0,pDir));
                          break;
                      }
                  } 
                  // move to next one or break because you reached the end layer
                  tLayer = (tLayer == eLayer ) ? nullptr :  tLayer->nextLayer(gp,dir);
              } while (tLayer);
          }
          
          // final check for compatibility of the start layer in order to avoid punch-through
          if (sLayer && sLayerIntersection.valid && sLayerIntersection.pathLength < validPathLength)
              lIntersections.push_back(LayerIntersection<T>(sLayerIntersection,sLayer,sLayerSurface,0,pDir));
          
      }
      // and the arbitraray layers
      if (m_confinedArbitraryLayers){
          // loop over the layers and intersect them
          for (auto& layer : (*m_confinedArbitraryLayers)){
              // intersections
              Trk::Intersection lIntersection = layer->surfaceRepresentation().straightLineIntersection(gp,dir,true,bchk);
              if (lIntersection.valid)
                  lIntersections.push_back(LayerIntersection<T>(lIntersection,layer,&(layer->surfaceRepresentation()),0,pDir));
          }
      }

      // sort them accordingly to the path length
      std::sort(lIntersections.begin(),lIntersections.end());
      // and return
      return lIntersections;
  }

  /** Returns the boundary surfaces ordered in probability to hit them based on straight line intersection @todo change hard-coded default */
  template <class T > std::vector< BoundaryIntersection<T> > 
                      TrackingVolume::boundarySurfacesOrdered(const T& pars, 
                                                              PropDirection pDir,
                                                              bool) const
  {
      
      
      // assign the direction
      
      const Amg::Vector3D dir = ( pDir == alongMomentum ? pars.momentum().unit() : Amg::Vector3D(-1*pars.momentum().unit()));
      // loop over boundarySurfaces and calculate the intersection
      std::vector< BoundaryIntersection<T> > bIntersections;
      auto& bSurfaces = boundarySurfaces();
      for (auto& bsIter : bSurfaces ){
          const BoundarySurface<TrackingVolume>* bSurface = bsIter.get();
          Intersection bsIntersection   = bSurface->surfaceRepresentation().straightLineIntersection(pars.position(),dir,true,false);
          if (bsIntersection.valid)
              bIntersections.push_back(BoundaryIntersection<T>(bsIntersection,bSurface,&(bSurface->surfaceRepresentation()),0,pDir));
      }
      // and now sort to get the closest
      std::sort(bIntersections.begin(),bIntersections.end());
      // and return
      return bIntersections;
  }


    
  inline GeometrySignature TrackingVolume::geometrySignature() const
  { return m_geometrySignature; }  
    
  inline GeometryType TrackingVolume::geometryType() const
  { return m_geometryType; }      
    
  inline void TrackingVolume::registerColorCode(unsigned int icolor)
  { m_colorCode = icolor; }
  inline void TrackingVolume::registerColorCode(unsigned int icolor) const
  { const_cast<TrackingVolume&> (*this).registerColorCode(icolor);}


  inline unsigned int TrackingVolume::colorCode() const
  { return m_colorCode; }

  inline void TrackingVolume::forceNavigationCheck()
  { m_redoNavigation = true; }
  inline void TrackingVolume::forceNavigationCheck ATLAS_NOT_THREAD_SAFE() const
  { const_cast<TrackingVolume&>(*this).forceNavigationCheck(); }
      
  inline bool TrackingVolume::redoNavigation() const
  { return m_redoNavigation; }

  inline const TrackingVolume* TrackingVolume::getMotherVolume() const
  { return m_motherVolume; }

  inline void TrackingVolume::setMotherVolume(const TrackingVolume* mvol) 
  { m_motherVolume = mvol; }
  inline void TrackingVolume::setMotherVolume ATLAS_NOT_THREAD_SAFE (const TrackingVolume* mvol) const
  { const_cast<TrackingVolume&>(*this).setMotherVolume(mvol); }

  inline void TrackingVolume::propagateMaterialProperties ATLAS_NOT_THREAD_SAFE(const Material& mprop) const {
    const_cast<TrackingVolume *>(this)->propagateMaterialProperties(mprop);
  }

 inline bool TrackingVolume::isAlignable () const{return false;}
} // end of namespace

#endif // TRKGEOMETRY_TRACKINGVOLUME_H


