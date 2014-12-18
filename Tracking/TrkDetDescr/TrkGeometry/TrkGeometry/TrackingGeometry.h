/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingGeometry.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_TRACKINGGEOMETRY_H
#define TRKGEOMETRY_TRACKINGGEOMETRY_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/TrackingVolume.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
// CLASS DEF
#include "CLIDSvc/CLASS_DEF.h"
// STL
#include <map>
// ATH_MSG macros
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"


class MsgStream;

namespace Trk {

 /** @enum NavigationLevel
     destinguishes an association TrackingGeometry with 
     one for global search */

  enum NavigationLevel
  {
      noNavigation = 0,
      globalSearch = 1,
      association  = 2
  };

  class TrackingVolume;
  class Layer;
  class MaterialProperties;

  /** 
    @class TrackingGeometry
    
    The TrackingGeometry class is the owner of the constructed TrackingVolumes.
    It enables both, a global search for an asociatedVolume
    (respectively, if existing, a global search of an associated Layer or the next
    associated Layer), such as a continous navigation by BoundarySurfaces between 
    the confined TrackingVolumes.
    
    @author Andreas.Salzburger@cern.ch
    
    */
  class TrackingGeometry {

    /** Give the GeometryBuilder friend rights */  
    friend class GeometryBuilder;
    friend class IGeometryBuilder;
  
    public :
      /** Constructor */
      TrackingGeometry(const TrackingVolume* highestVolume, NavigationLevel navlevel=globalSearch);
      
      /** Destructor */
      ~TrackingGeometry();
      
      /** return the world */
      const TrackingVolume* highestTrackingVolume() const;
      
      /** return the lowest tracking Volume */
      const TrackingVolume* lowestTrackingVolume(const Amg::Vector3D& gp) const;

      /** return the vector of lowest detached tracking Volume(->overlaps) */
      std::vector<const Trk::DetachedTrackingVolume*>* lowestDetachedTrackingVolumes(const Amg::Vector3D& gp) const;

      /** return the lowest static tracking Volume */
      const TrackingVolume* lowestStaticTrackingVolume(const Amg::Vector3D& gp) const;
      
      /** return the tracking Volume by name, 0 if it doesn't exist */
      const TrackingVolume* trackingVolume(const std::string& name) const;
            
      /** Forward the associated Layer information */
      const Layer* associatedLayer(const Amg::Vector3D& gp) const;
      
      /** Forward the next Layer information */
      const Layer* nextLayer(const Amg::Vector3D& gp, const Amg::Vector3D& mom, bool skipNavLayer=false) const;

      /** Closest Material Layer - used for the mapping option */
      template <class T> const LayerIntersection<Amg::Vector3D>  closestMaterialLayer(const T& pars,
                                                                                      PropDirection pDir = Trk::alongMomentum,
                                                                                      BoundaryCheck bchk = true) const;
      /** check position at volume boundary */
      bool atVolumeBoundary(const Amg::Vector3D& gp, const TrackingVolume* vol, double tol) const;
      
      /** check position at volume boundary + navigation link */
      bool atVolumeBoundary(const Amg::Vector3D& gp, const Amg::Vector3D& mom, const TrackingVolume* vol, 
			                const TrackingVolume*& nextVol, Trk::PropDirection dir, double tol) const;
      
      /** Return the unique BoundarySurfaces with MaterialInformation */
     const std::map<const Layer*,int>& boundaryLayers() const;
      
      /** Return the Navigation Level - only one TrackingGeometry can have full association 
         to GeoModel */
      NavigationLevel navigationLevel() const;

      /** Print the summary of volume Hierarchy of the TrackingGeometry */
      void printVolumeHierarchy(MsgStream& msgstream) const;
      
      /** indexLayers : method to re-set the index of the layers, depending on geometrySignature */
      void indexStaticLayers(GeometrySignature geosit, int offset = 0) const;

    private:
      /** Geometry Builder busineess:
          the geometry builder has to sign*/
      void sign(GeometrySignature geosit, GeometryType geotype = Static) const;
    
      /** Geometry Builder busineess:
          set all contained surfaces TG owned - this should save memory and avoid surface copying 
          - prints compactification statistics */
      void compactify(MsgStream& msgstream, const TrackingVolume* vol=0) const;
      
      /**  Geometry Builder busineess:
           synchronize all layers to enclosed volume dimensions */
      void synchronizeLayers(MsgStream& msgstream, const TrackingVolume* vol=0) const;
           
    
      /** private method the Navigation Level */
      void registerNavigationLevel(NavigationLevel navlevel) const;

      /** private method to register recursively the tracking volumes */
      void registerTrackingVolumes(const TrackingVolume& tvol, const TrackingVolume* mvol = 0, int lvl = 0);

      /**  private method to be called from GeometryBuilder: return the world with ownership */
      const TrackingVolume* checkoutHighestTrackingVolume() const;

      /** print VolumeInformation with Level */
      void printVolumeInformation(MsgStream& msgstream, const TrackingVolume& tvol, int lvl) const;
      
      /** The known world - and the beam */   
      mutable const TrackingVolume*                         m_world;
      mutable const PerigeeSurface*                         m_beam;
      
      /** The unique boundary Layers */
      mutable std::map<const Layer*,int>                    m_boundaryLayers;
      
      /** The Volumes in a map for later finding */
      std::map<const std::string, const TrackingVolume*>    m_trackingVolumes;

      /** The Navigation level for identification */
      mutable NavigationLevel                               m_navigationLevel;
      
    public:
      /// Log a message using the Athena controlled logging system
      MsgStream& msg( MSG::Level lvl ) const { return m_msg.get() << lvl ; }
      
      /// Check whether the logging system is active at the provided verbosity level
      bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
      
    private:
      /// Private message stream member
      mutable Athena::MsgStreamMember m_msg;

  };
  
  inline const TrackingVolume* TrackingGeometry::highestTrackingVolume() const
  { return (m_world); }

  inline NavigationLevel TrackingGeometry::navigationLevel() const
  { return m_navigationLevel; }

  inline void TrackingGeometry::registerNavigationLevel(NavigationLevel navLevel) const 
  { m_navigationLevel = navLevel; }

  inline void TrackingGeometry::sign(GeometrySignature geosit, GeometryType geotype) const
  { m_world->sign(geosit, geotype); }
  
  inline const TrackingVolume* TrackingGeometry::trackingVolume(const std::string& name) const
  {
      std::map<const std::string, const TrackingVolume*>::const_iterator sVol = m_trackingVolumes.begin();
      sVol = m_trackingVolumes.find(name);
      if (sVol != m_trackingVolumes.end()) return(sVol->second);
      return 0;
  }
  
  inline const Trk::Layer* TrackingGeometry::associatedLayer(const Amg::Vector3D& gp) const
  {
      const TrackingVolume* lowestVol = (lowestTrackingVolume(gp));
      return lowestVol->associatedLayer(gp);
  }
  
  inline const Trk::Layer* TrackingGeometry::nextLayer(const Amg::Vector3D& gp,
                                                     const Amg::Vector3D& mom,
                                                     bool skipNavLayer) const
  {
      const TrackingVolume* lowestVol = (lowestTrackingVolume(gp));
      return lowestVol->nextLayer(gp, mom, true, skipNavLayer);
  }

  inline const std::map<const Layer*,int>& TrackingGeometry::boundaryLayers() const 
  { return m_boundaryLayers; }

  
  template <class T> const LayerIntersection<Amg::Vector3D> TrackingGeometry::closestMaterialLayer(const T& pars,
                                                                                                   PropDirection pDir,
                                                                                                   BoundaryCheck bchk) const
  {
      const TrackingVolume* lowestVol = (lowestTrackingVolume(pars.position()));
      return ( lowestVol ) ? (lowestVol->closestMaterialLayer(pars.position(),pars.momentum().unit(), pDir, bchk)) : 
            Trk::LayerIntersection<Amg::Vector3D>(Trk::Intersection(pars.position(),10e10,false), 0, 0, 0 );
  }  

} // end of namespace

CLASS_DEF(Trk::TrackingGeometry, 167645219, 1)

#endif //TRKGEOMETRY_TRACKINGGEOMETRY_H
