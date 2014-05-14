/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/LayerAttemptsCalculator.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkEventPrimitives/PropDirection.h"
//STL
#include <string>
// ATH_MSG macros
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

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
  class EntryLayerProvider;
  class ObjectAccessor;
  class GlueVolumesDescriptor;
  class VolumeBounds;

  typedef BinnedArray< Layer >            LayerArray;                         
  typedef BinnedArray< TrackingVolume >   TrackingVolumeArray;
  
  /**
    @struct LayerIntersection 
    */

  struct LayerIntersection {

      SurfaceIntersection   sIntersection;
      mutable const Layer*  layer;
      
      LayerIntersection(const SurfaceIntersection& sInter,
                        const Layer*  sLayer):
        sIntersection(sInter),
        layer(sLayer){}
  };
         
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
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds* volbounds,
                     const LayerArray* subLayers=0,
                     const TrackingVolumeArray* subVolumes=0,
                     const std::string& volumeName="undefined");
      
      /** Constructor for a full equipped Tracking Volume
         - full by inheritance  ======> 2 a) static confinement */
      TrackingVolume(const Volume& volume,
                     const Material& matprop,
                     const LayerArray* subLayers=0,
                     const TrackingVolumeArray* subVolumes=0,
                     const std::string& volumeName="undefined");
                     
      /** Constructor for a full equipped Tracking Volume
        - mixed  ======> 3 a) static confinement */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const Material& matprop,
                     const LayerArray* subLayers=0,
                     const TrackingVolumeArray* subVolumes=0,
                     const std::string& volumeName="undefined");


      /** Constructor for a full equipped Tracking Volume with detached subvolumes
        -  mixed =======> 1 b) detached volumes */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const Material& matprop,
                     const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
                     const std::string& volumeName="undefined");                            

      /** Constructor for a full equipped Tracking Volume with detached subvolumes  
        -  mixed =======> 2 b) detached volumes */
      TrackingVolume(const Volume& volume,
                     const Material& matprop,
                     const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
                     const std::string& volumeName="undefined");                            

      /** Constructor for a full equipped Tracking Volume with unordered subvolumes
        - mixed =======> 1 d) unordered volumes  */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const Material& matprop,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const std::string& volumeName="undefined");                            

      /** Constructor for a full equipped Tracking Volume with unordered subvolumes 
        - mixed =======> 2 d) unordered volumes  */
      TrackingVolume(const Volume& volume,
                     const Material& matprop,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const std::string& volumeName="undefined");                            
      
      /** Constructor for a full equipped Tracking Volume with arbitrary layers
        -  mixed =======> 1 c) arbitrarily oriented layers */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const Material& matprop,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::string& volumeName="undefined");                     

      /** Constructor for a full equipped Tracking Volume with arbitrary layers
        -  mixed =======> 2 c) arbitrarily oriented layers */
      TrackingVolume(const Volume& volume,
                     const Material& matprop,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::string& volumeName="undefined");                     

      /** Constructor for a full equipped Tracking Volume with arbitrary layers AND subVolumes - 
        -  mixed =======> 1 e) unordered layers AND unordered subvolumes */
      TrackingVolume(Amg::Transform3D* htrans,
                     VolumeBounds*   volbounds,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const Material& matprop,
                     const std::string& volumeName="undefined");                     

      /** Constructor for a full equipped Tracking Volume with arbitrary layers AND subVolumes - 
        -  mixed =======> 2 e) unordered layers AND unordered subvolumes */
      TrackingVolume(const Volume& volume,
                     const std::vector<const Layer*>* arbitraryLayers,
                     const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                     const Material& matprop,
                     const std::string& volumeName="undefined");                     

      /** copy constructor with shift */
      TrackingVolume(const Trk::TrackingVolume& trVol, Amg::Transform3D& transform);
      
      /** Destructor */
      ~TrackingVolume();
      
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

      /** Return the closest layer with material description 
            - it assumes to be on lowest navigation level
            - does not step down to eventually confined TrackingVolumes
            - navigates to the next trackign volume IF PropDirection == Trk::mappingMode
        */
      const LayerIntersection closestMaterialLayer(const Amg::Vector3D& gp, 
                                                   const Amg::Vector3D& dir, 
                                                   PropDirection pDir = Trk::alongMomentum,
                                                   BoundaryCheck bchk = true ) const;
            
      /** Return the associated sub Volume, returns THIS if no subVolume exists */
      const TrackingVolume* associatedSubVolume(const Amg::Vector3D& gp) const;
      
      /** Return the next volume along the navigation stream */
      const TrackingVolume* nextVolume(const Amg::Vector3D& gp, 
                                       const Amg::Vector3D& dir, 
                                       PropDirection pDir = Trk::alongMomentum) const;
      
      /** Return the next sub Volume if existing, returns THIS if no subVolume exists */
      const TrackingVolume* nextSubVolume(const Amg::Vector3D& gp, const Amg::Vector3D& dir) const;

      /** Return the associated detached subvolumes */
      std::vector<const Trk::DetachedTrackingVolume*>* assocDetachedSubVolumes(const Amg::Vector3D& gp, double tol) const;

      /** Return the subLayer array - not the ownership*/
      const EntryLayerProvider* entryLayerProvider() const;   

      /** Return the subLayer array - not the ownership*/
      const EntryLayerProvider* checkoutEntryLayerProvider() const;   
   
      /** Layer attempts - as provided by the LayerAttemptCreator */
      unsigned int layerAttempts(BoundarySurfaceFace exitFace) const;

      /** Layer attempts - as provided by the LayerAttemptCreator */
      unsigned int maxLayerAttempts() const;

      /** Return the subLayer array - not the ownership*/
      const LayerArray* confinedLayers() const;

      /** Return the subLayer array - not the ownership*/
      const std::vector<const Trk::Layer*>* confinedArbitraryLayers() const;
      
      /** Return the subLayerarray including the ownership*/
      const LayerArray* checkoutConfinedLayers() const;

      /** Return the subLayer array */
      const TrackingVolumeArray* confinedVolumes() const;

      /** Return detached subVolumes - not the ownership */
      const std::vector<const Trk::DetachedTrackingVolume*>* confinedDetachedVolumes() const;

      /** Return unordered subVolumes - not the ownership */
      const std::vector<const Trk::TrackingVolume*>* confinedDenseVolumes() const;
      
      /** Returns the VolumeName - for debug reason, might be depreciated later */
      const std::string& volumeName() const;
      
      /** Method to return the BoundarySurfaces */
      const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >& boundarySurfaces() const;
                                            
      /** Get the BoundarySurface to the appointed Accessor state */
      const BoundarySurface<TrackingVolume>* boundarySurface(const ObjectAccessor& oa) const;

      /** Register (and give ownership) the entryLayer array */
      void registerEntryLayerProvider(const EntryLayerProvider* entryLayerProvider) const;

      /** Register the outside glue volumes -
          ordering is in the TrackingVolume Frame:
           - negativeFaceXY
           - (faces YZ, ZY, radial faces)
           - positiveFaceXY
      */

      void registerOutsideGlueVolumes(GlueVolumesDescriptor* gvd) const;            
      /** Register the outside glue volumes -
          ordering is in the TrackingVolume Frame:
           - negativeFaceXY
           - (faces YZ, ZY, radial faces)
           - positiveFaceXY
      */      
      const GlueVolumesDescriptor& glueVolumesDescriptor() const;

      /** the sensitive area */
      void registerSensitiveVolume(const AbstractVolume* svol) const;
      
      /** return the sensitive volume */
      const AbstractVolume* sensitiveVolume() const;

      /** return the sensitive volume - gives ownership*/
      const AbstractVolume* checkoutSensitiveVolume() const;

      /** sign the volume - the geometry builder has to do that */
      void sign(GeometrySignature signat) const;

      /** return the Signature */
      GeometrySignature geometrySignature() const;
                        
      /** Register the color code */
      void registerColorCode(unsigned int icolor) const;

      /** Get the color code */
      unsigned int colorCode() const; 

      /** force a navigation check */
      void forceNavigationCheck() const;
      
      /** Boolean, if true navigation needs to be redone when hitting this volume */
      bool redoNavigation() const; 

      /** Return the MotherVolume - if it exists */
      const Trk::TrackingVolume* getMotherVolume() const;

      /** Return the MotherVolume - if it exists */
      void setMotherVolume(const Trk::TrackingVolume* mvol) const;

      /** move Volume */
      void moveVolume( Amg::Transform3D& shift ) const;

      /** add Material */
      void addMaterial( const Material& mat, float fact=1. ) const;

      /** remove content */
      void clear() const;

      /** dump to screen */
      void screenDump(MsgStream& msg) const;

    protected:

      /** clone at new position */
      const TrackingVolume* cloneTV(Amg::Transform3D& transform) const;
            
    private:          
      /** reIndex the static layers of the TrackingVolume */
      void indexContainedStaticLayers(GeometrySignature geoSig, int& offset) const;
      
      /** propagate material properties to subvolumes */
      void propagateMaterialProperties(const Material& mprop) const;
      
      /** Create Boundary Surface */
      void createBoundarySurfaces();
      
      /** Create Layer Attempts Caluclator */
      void createLayerAttemptsCalculator();
      
      /** compactify the memory usage in the event by setting ownership to TackingGeometry 
        the referenced types are the number of registered surfaces & total surfaces
      */
      void compactify(size_t& rSurfaces, size_t& tSurfaces) const;

      /** method to synchronize the layers with potentially updated volume bounds:
          - adapts the layer dimensions to the new volumebounds + envelope
          - adapts entry layer position where necessary to the new volumebounds 
      */
      void synchronizeLayers(MsgStream& msgstream, double envelope = 1.) const;

      /** Register Next - Previous for Layers, set volumelink */
      void interlinkLayers();

      /** Helper method - find closest of two layers */
      const Trk::Layer* closest(const Amg::Vector3D& pos,
                                const Amg::Vector3D& dir,
                                const Trk::Layer& first,
                                const Trk::Layer& second) const;
                                
      
      /** move the Tracking Volume*/
      void moveTV(Amg::Transform3D& transform) const;
           
      /** Forbidden copy constructor */   
      TrackingVolume(const TrackingVolume&): Volume(), Material() {} 
       
      mutable const TrackingVolume*        m_motherVolume;  //!< mother volume of this volume

      std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >* m_boundarySurfaces;  //!< boundary Surfaces
      //(a)
      mutable const EntryLayerProvider*                                m_entryLayerProvider;      //!< Array of entry Layers
      mutable const LayerArray*                                        m_confinedLayers;   //!< Array of Layers inside the Volume
      mutable const TrackingVolumeArray*                               m_confinedVolumes;  //!< Array of Volumes inside the Volume
      //(b)
      mutable const std::vector<const Trk::DetachedTrackingVolume*>*   m_confinedDetachedVolumes; //!< Detached subvolumes
      // additionally
      mutable const std::vector<const Trk::TrackingVolume*>*           m_confinedDenseVolumes;    //!< Unordered subvolumes
      //(b)
      mutable const std::vector<const Trk::Layer*>*                    m_confinedArbitraryLayers; //!< Unordered Layers inside the Volume

      mutable GlueVolumesDescriptor*                                   m_outsideGlueVolumes;      //!< Volumes to glue Volumes from the outside

      mutable const AbstractVolume*                                    m_sensitiveVolume;         //!< Sensitive volume

      LayerAttemptsCalculator*                                         m_layerAttemptsCalculator; //!< provided the number of layer attempts

      mutable GeometrySignature            m_geometrySignature;        //!< The Signature done by the GeometryBuilder 
      
      std::string                          m_name;                     //!< Volume name for debug reasons
      mutable unsigned int                 m_colorCode;                //!< Color code for displaying (Root Color code)
      mutable bool                         m_redoNavigation;           //!< Navigation boolean. If true navigation needs to be redone when entering this volume

    public:
      /// Log a message using the Athena controlled logging system
      MsgStream& msg( MSG::Level lvl ) const { return m_msg.get() << lvl ; }
      /// Check whether the logging system is active at the provided verbosity level
      bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
      
    private:
      /// Private message stream member
      mutable Athena::MsgStreamMember m_msg;

  };

  inline const std::string& TrackingVolume::volumeName() const { return m_name; }

  inline const EntryLayerProvider*  TrackingVolume::entryLayerProvider() const { return m_entryLayerProvider; }
 
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

  inline const std::vector<const Trk::Layer*>* TrackingVolume::confinedArbitraryLayers() const 
  { return m_confinedArbitraryLayers; }

  inline const TrackingVolumeArray*  TrackingVolume::confinedVolumes() const
  { return m_confinedVolumes; }

  inline const  std::vector<const Trk::DetachedTrackingVolume*>*  TrackingVolume::confinedDetachedVolumes() const 
  { return m_confinedDetachedVolumes; }

  inline const  std::vector<const Trk::TrackingVolume*>*  TrackingVolume::confinedDenseVolumes() const 
  { return m_confinedDenseVolumes; }

  inline void TrackingVolume::registerSensitiveVolume(const AbstractVolume* svol) const
  { m_sensitiveVolume = svol; }

  inline const AbstractVolume* TrackingVolume::sensitiveVolume() const
  { return m_sensitiveVolume; }
   
  inline const AbstractVolume* TrackingVolume::checkoutSensitiveVolume() const
  {
      const AbstractVolume* returnVolume = m_sensitiveVolume;
      m_sensitiveVolume = 0;
      return returnVolume; 
  }
    
  inline GeometrySignature TrackingVolume::geometrySignature() const
  { return m_geometrySignature; }  
    
  inline void TrackingVolume::registerColorCode(unsigned int icolor) const
  { m_colorCode = icolor; }

  inline unsigned int TrackingVolume::colorCode() const
  { return m_colorCode; }

  inline void TrackingVolume::forceNavigationCheck() const
  { m_redoNavigation = true; }
      
  inline bool TrackingVolume::redoNavigation() const
  { return m_redoNavigation; }

  inline const TrackingVolume* TrackingVolume::getMotherVolume() const
  { return m_motherVolume; }

  inline void TrackingVolume::setMotherVolume(const TrackingVolume* mvol) const
  { m_motherVolume = mvol; }

} // end of namespace

#endif // TRKGEOMETRY_TRACKINGVOLUME_H


