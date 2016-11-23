/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
Muon Cluster Readout Element properties
 -----------------------------------------
 Copyright (C) 2013 by ATLAS Collaboration
***************************************************************************/

#ifndef MUONGEOMODEL_MUONCLUSTERREADOUTELEMENT_H
#define MUONGEOMODEL_MUONCLUSTERREADOUTELEMENT_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include <vector>
namespace Trk {
  class Surface;
  class SurfaceBounds;
}

namespace MuonGM {

  class MuonDetectorManager;
 

  class MuonClusterReadoutElement: public MuonReadoutElement {
  public:

    /** struct to hold information needed for TrkDetElementBase, takes owership of all pointers */ 
    struct SurfaceData {
      /** all surfaces on the muon strip detectors have the same normal vector */
      std::vector<Amg::Vector3D> m_layerNormals;

      /** the eta and phi bounds of the individual layers are not all identical in the muon system (sTGC staircase) */
      std::vector<Trk::SurfaceBounds*>   m_surfBounds;

      /** there are two transformation per layer as the eta/phi surfaces are rotated by 90 degrees wrt eachother: Total number = 2*number of layers */
      std::vector<Amg::Transform3D> m_layerTransforms;
      
      /** there are two surfaces per layer as the eta/phi surfaces are rotated by 90 degrees wrt eachother: Total number = 2*number of layers */
      std::vector<Trk::PlaneSurface*>   m_layerSurfaces;

      /** the eta and phi surfaces for a given layer share the same center: Total number = number of layers */
      std::vector<Amg::Vector3D>  m_layerCenters;
      
      SurfaceData() {}

      ~SurfaceData() {
	for (unsigned int i=0; i<m_surfBounds.size(); i++) delete m_surfBounds[i];
        for (unsigned int i=0; i<m_layerSurfaces.size(); i++) delete m_layerSurfaces[i];
      }
      
    };

    
    MuonClusterReadoutElement(GeoVFullPhysVol* pv, std::string stName,
			      int zi, int fi, bool is_mirrored,
			      MuonDetectorManager* mgr);
    
    ~MuonClusterReadoutElement();                  

    /** distance to readout. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    virtual double distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const = 0;

    /** strip number corresponding to local position. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    virtual int stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const = 0;

    /** strip position 
	If the strip number is outside the range of valid strips, the function will return false */
    virtual bool stripPosition( const Identifier& id, Amg::Vector2D& pos ) const = 0;

    /** space point position for a given pair of phi and eta identifiers 
	The LocalPosition is expressed in the reference frame of the phi surface.
	If one of the identifiers is outside the valid range, the function will return false */
    virtual bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const = 0;

    /** Global space point position for a given pair of phi and eta identifiers 
	If one of the identifiers is outside the valid range, the function will return false */
    virtual bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const = 0;

    /** number of layers in phi/eta projection */
    virtual int numberOfLayers( bool measuresPhi ) const = 0;

    /** number of strips per layer */
    virtual int numberOfStrips( const Identifier& layerId )   const = 0;
    virtual int numberOfStrips( int layer, bool measuresPhi ) const = 0;

    /** clear the cache of the readout elememt */
    void clearCache() const;
    
    /** fill the cache of the readout element, to be implemented by the concrete implementations */
    virtual void fillCache() const = 0;

    /** access to chamber surface (phi orientation), uses the first gas gap */ 
    const Trk::PlaneSurface&    surface() const;   
    const Trk::SurfaceBounds&   bounds() const;
    const Amg::Vector3D&  center() const;
    const Amg::Vector3D& normal() const;
    const Amg::Transform3D& transform() const;

    /** access to layer surfaces */
    const Trk::PlaneSurface&    surface(const Identifier& id) const;
    const Trk::SurfaceBounds&   bounds(const Identifier& id) const;
    const Amg::Vector3D&  center(const Identifier& id) const;
    const Amg::Vector3D& normal(const Identifier& id) const;
    const Amg::Transform3D& transform(const Identifier& id) const;

    /** access to layer surfaces */
    const Trk::PlaneSurface&    surface(int surfHash) const;
    const Trk::SurfaceBounds&   bounds(int boundHash) const;
    const Amg::Vector3D&  center(int layHash) const;
    const Amg::Vector3D& normal(int layHash) const;
    const Amg::Transform3D& transform(int surfHash) const;

    /** returns all the surfaces contained in this detector element */
    virtual const std::vector<const Trk::Surface*>& surfaces() const;

    /** returns the hash function to be used to look up the center and the normal of the tracking surface for a given identifier */
    virtual int  layerHash(const Identifier& id)   const = 0; 

    /** returns the hash function to be used to look up the surface and surface transform for a given identifier */
    virtual int  surfaceHash(const Identifier& id) const = 0; 

    /** returns the hash function to be used to look up the surface boundary for a given identifier */
    virtual int  boundaryHash(const Identifier& id) const = 0; 

    /** returns whether the given identifier measures phi or not */
    virtual bool measuresPhi(const Identifier& id) const = 0; 

  protected:
    
    void shiftSurface(const Identifier& id) const; 
    void restoreSurfaces() const;

    mutable SurfaceData* m_surfaceData;
    mutable SurfaceData* m_surfaceDataBackup;

    /** these are all surfaces represented by this detector element : it's for visualization without casting */
    mutable std::vector<const Trk::Surface*>  m_elementSurfaces;
  };

  inline const Trk::PlaneSurface&    MuonClusterReadoutElement::surface()   const { return surface(0); }
  inline const Amg::Transform3D& MuonClusterReadoutElement::transform() const { return transform(0); }
  inline const Amg::Vector3D&  MuonClusterReadoutElement::center()    const { return center(0); }
  inline const Amg::Vector3D& MuonClusterReadoutElement::normal()    const { return normal(0); }
  inline const Trk::SurfaceBounds&   MuonClusterReadoutElement::bounds()    const { return bounds(0); }


  inline const Trk::PlaneSurface&    MuonClusterReadoutElement::surface(const Identifier& id) const {   return surface(surfaceHash(id));  }
  inline const Amg::Transform3D& MuonClusterReadoutElement::transform(const Identifier& id) const { return transform( surfaceHash(id)); }
  inline const Amg::Vector3D&  MuonClusterReadoutElement::center(const Identifier& id) const { return center(layerHash(id) ); }
  inline const Amg::Vector3D& MuonClusterReadoutElement::normal(const Identifier& id) const { return normal(layerHash(id) ); }
  inline const Trk::SurfaceBounds&   MuonClusterReadoutElement::bounds(const Identifier& id) const { return bounds(boundaryHash(id)); }

  inline void MuonClusterReadoutElement::clearCache() const {
    // clear base cache
    MuonReadoutElement::clear();
    delete m_surfaceData; m_surfaceData = 0;
    delete m_surfaceDataBackup; m_surfaceDataBackup = 0;
  }


  inline const Trk::PlaneSurface&    MuonClusterReadoutElement::surface(int hash) const {   
    if( !m_surfaceData ){
      reLog()<<MSG::ERROR<<"requesting surface but cache is empty" << endmsg;
      throw;
      //fillCache();
    }
    if( hash == -1 || hash >= (int)m_surfaceData->m_layerSurfaces.size() ){
      reLog()<<MSG::WARNING<<this << " surface hash out of range: " << hash << " elements " << m_surfaceData->m_layerSurfaces.size() << endmsg;
      return *m_surfaceData->m_layerSurfaces.front(); 
    }
    return *m_surfaceData->m_layerSurfaces[hash]; 
  } 


  inline const Amg::Transform3D& MuonClusterReadoutElement::transform(int hash) const { 
    if( !m_surfaceData ){
      reLog()<<MSG::ERROR<<"requesting transform but cache is empty" << endmsg;
      throw;
      //fillCache();
    }
    if( hash == -1 || hash >= (int)m_surfaceData->m_layerTransforms.size() ){
      reLog()<<MSG::WARNING<< this << "transform hash out of range: " << hash << " elements " << m_surfaceData->m_layerTransforms.size() << endmsg;
      return m_surfaceData->m_layerTransforms.front(); 
    }
    return m_surfaceData->m_layerTransforms[hash]; 
  }

  inline const Amg::Vector3D&  MuonClusterReadoutElement::center(int hash) const { 
    if( !m_surfaceData ){
      reLog()<<MSG::ERROR<<"requesting center but cache is empty" << endmsg;
      throw;
      //fillCache();
    }
    if( hash == -1 || hash >= (int)m_surfaceData->m_layerCenters.size() ){
      reLog()<<MSG::WARNING<<"center hash out of range: " << hash << " elements " << m_surfaceData->m_layerCenters.size() << endmsg;
      return m_surfaceData->m_layerCenters.front(); 
    }
    return m_surfaceData->m_layerCenters[hash];
  } 

  inline const Amg::Vector3D& MuonClusterReadoutElement::normal(int hash) const { 
    if( !m_surfaceData ){
      reLog()<<MSG::ERROR<<"requesting normal but cache is empty" << endmsg;
      throw;
      //fillCache();
    }
    if( hash == -1 || hash >= (int)m_surfaceData->m_layerNormals.size() ){
      reLog()<<MSG::WARNING<<"normal hash out of range: " << hash << " elements " << m_surfaceData->m_layerNormals.size() << endmsg;
      return m_surfaceData->m_layerNormals.front(); 
    }
    return m_surfaceData->m_layerNormals[hash];
  } 
  
  inline const Trk::SurfaceBounds&   MuonClusterReadoutElement::bounds(int hash) const { 
    if( !m_surfaceData ){
      reLog()<<MSG::ERROR<<"requesting bounds but cache is empty" << endmsg;
      throw;
      //fillCache();
    }    
    if( hash == -1 || hash >= (int)m_surfaceData->m_surfBounds.size() ){
      reLog()<<MSG::WARNING<<"normal hash out of range: " << hash << " elements " << m_surfaceData->m_surfBounds.size() << endmsg;
      return *m_surfaceData->m_surfBounds.front(); 
    }
    return *m_surfaceData->m_surfBounds[hash]; 
  }  

  inline const std::vector<const Trk::Surface*>& MuonClusterReadoutElement::surfaces() const {
      // create when first time requested and when possible
      if (!m_elementSurfaces.size() && m_surfaceData)
          m_elementSurfaces.insert(m_elementSurfaces.begin(), m_surfaceData->m_layerSurfaces.begin(), m_surfaceData->m_layerSurfaces.end());
      // return the element surfaces      
      return m_elementSurfaces;
  }

}
#endif // MUONGEOMODEL_MUONCLUSTERREADOUTELEMENT_H
