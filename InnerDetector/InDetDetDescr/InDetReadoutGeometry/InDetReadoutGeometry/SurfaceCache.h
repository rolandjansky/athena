/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SurfaceCache.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef InDetReadoutGeometry_SurfaceCache_h
#define InDetReadoutGeometry_SurfaceCache_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSurfaces/SurfaceBounds.h"

namespace InDetDD {

    /** @class SurfaceCache
    
       This class is for internal use in InDetReadoutGeometry.
       It holds all the cached values related to the element surface.
       
       It's a simple class to still alllow the 
       Trk::Surface::transform() { m_associatedDetectorElement->transform() }
       method to work, hence, it basically copies the Trk::Surface structure.
       
       The surface cache class alows also to store 0 pointers,
       indicating the the holder of the cache has a commonly shared object,
       e.g. the straw bounds of all straws in one detector element.
       
       @author: Grant Gorfine, Andreas Salzburger 
    */
         
    class SurfaceCache
    {
    
    public:
      SurfaceCache(const Amg::Transform3D* transform,
    		         const Amg::Vector3D* center,
    		         const Amg::Vector3D* normal,
    		         const Trk::SurfaceBounds* bounds);
    
      ~SurfaceCache();
      
      ///delete copy c'tor
      SurfaceCache(const SurfaceCache &) = delete;
      
      ///delete assignment
      SurfaceCache & operator = (const SurfaceCache &) = delete;
    
      const Amg::Transform3D* transform() const {return (m_transform);}
      
      const Amg::Vector3D* center() const {return (m_center);}
      
      const Amg::Vector3D*  normal() const {return (m_normal);}
      
      const Trk::SurfaceBounds* bounds() const {return (m_bounds);}
      
    
    private:
      const Amg::Transform3D*       m_transform;
      const Amg::Vector3D*          m_center;
      const Amg::Vector3D*          m_normal;
      const Trk::SurfaceBounds*     m_bounds;
    
    };
      
    inline SurfaceCache::SurfaceCache(const Amg::Transform3D* transform,
    			                      const Amg::Vector3D* center,
    			                      const Amg::Vector3D* normal,
    			                      const Trk::SurfaceBounds* bounds)
      : m_transform(transform),
        m_center(center),
        m_normal(normal),
        m_bounds(bounds)
    {}
    
    
    inline SurfaceCache::~SurfaceCache()
    {
      delete m_transform;
      delete m_center;
      delete m_normal;
      delete m_bounds;
    }

} // end namespace

  
#endif // InDetReadoutGeometry_SurfaceCache_h
