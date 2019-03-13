/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SurfaceCache.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetReadoutGeometry/TRT_Conditions.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "InDetIdentifier/TRT_ID.h"

#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"

#include <vector>

namespace InDetDD {


    TRT_BaseElement::TRT_BaseElement(const GeoVFullPhysVol *volume, const Identifier & id, const TRT_ID * idHelper, const TRT_Conditions * conditions) :
        Trk::TrkDetElementBase(volume),
        m_id(id),
        m_idHelper(idHelper),
        m_conditions(conditions),
        m_strawSurfaces(nullptr),
        m_strawSurfacesCache(nullptr),
        m_surfaceCache{},
        m_surface{},
        m_surfaces{},
        m_mutex{}
    {
        m_idHash = m_idHelper->straw_layer_hash(id);  
        m_conditions->ref();
    }

    Identifier TRT_BaseElement::identify() const
    {
        return m_id;
    }

    IdentifierHash TRT_BaseElement::identifyHash() const
    {
        return m_idHash;
    }

    // [0] GeoModel / CLHEP Access
    const HepGeom::Transform3D TRT_BaseElement::getAbsoluteTransform(int straw) const 
    {
        return Amg::EigenTransformToCLHEP(strawTransform(straw));
    }

    // [A] description of the current detector element : Amg first, then CLHEP converted methods
    const Trk::Surface& TRT_BaseElement::surface() const
    {
        return elementSurface();
    }
    
    const Trk::SurfaceBounds& TRT_BaseElement::bounds() const
    {
        if (not m_surfaceCache) createSurfaceCache();
        return *(m_surfaceCache->bounds());
    }
    
    const Amg::Transform3D& TRT_BaseElement::transform() const
    {	
        if (not m_surfaceCache) createSurfaceCache();
        return *(m_surfaceCache->transform());
    }    


    const Amg::Vector3D& TRT_BaseElement::center() const
    {  
        if (not m_surfaceCache) createSurfaceCache();
        return *(m_surfaceCache->center());
    }
    
    const Amg::Vector3D& TRT_BaseElement::normal() const
    {
        if (not m_surfaceCache) createSurfaceCache();
        return *(m_surfaceCache->normal());
    }
    
    // [B] Description of the individual straws
    const Trk::Surface&  TRT_BaseElement::surface(const Identifier& id) const
    {
        int straw = m_idHelper->straw(id);
        // Create vector of all straws.
        if (!m_strawSurfaces) createStrawSurfaces();
        Trk::Surface * surfacePtr = (*m_strawSurfaces)[straw];
        if (!surfacePtr) {
            createSurfaceCache(id);
            surfacePtr = (*m_strawSurfaces)[straw];
        }
        return *surfacePtr;
    }


    const std::vector<const Trk::Surface*>& TRT_BaseElement::surfaces() const 
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        if (!m_surfaces.size()){
            m_surfaces.reserve(nStraws());
            for (unsigned is = 0; is<nStraws(); ++is)
                m_surfaces.push_back(&strawSurface(is));
        }
        return m_surfaces;
    }


    const Trk::SurfaceBounds& TRT_BaseElement::bounds(const Identifier&) const 
    {
        return strawBounds();
    }

    const Amg::Transform3D& TRT_BaseElement::transform(const Identifier & id) const 
    {
        int straw = m_idHelper->straw(id);
        if (!m_strawSurfacesCache) createStrawSurfacesCache();
        SurfaceCache* sCachePtr = (*m_strawSurfacesCache)[straw];
        if (!sCachePtr) {
            createSurfaceCache(id);
            sCachePtr = (*m_strawSurfacesCache)[straw];
        }
        // forward the transform of the cache
        return *(sCachePtr->transform());
    }       

    const Amg::Transform3D& TRT_BaseElement::strawTransform(unsigned int straw) const 
    {
      if (!m_strawSurfacesCache) createStrawSurfacesCache();
        SurfaceCache* sCachePtr = (*m_strawSurfacesCache)[straw];
        if (!sCachePtr) {
            Identifier id =  m_idHelper->straw_id(identify(), straw);
            createSurfaceCache(id);
            sCachePtr = (*m_strawSurfacesCache)[straw];
        }
        // forward the transform of the cache
        return *(sCachePtr->transform());
    }


    const Amg::Vector3D& TRT_BaseElement::normal(const Identifier&) const
    {
       // Not sure if the normal of the straw is ever used.
       // nor is there a well defined normal.
       // This wont be corrected for alignments.
       // Just return the element normal
       return normal();
    }

    const Amg::Vector3D& TRT_BaseElement::center(const Identifier & id) const 
    {
        int straw = m_idHelper->straw(id);
        if (!m_strawSurfacesCache) createStrawSurfacesCache();
        SurfaceCache* sCachePtr = (*m_strawSurfacesCache)[straw];
        if (!sCachePtr) {
            createSurfaceCache(id);
            sCachePtr = (*m_strawSurfacesCache)[straw];
        }
        // forward the transform of the cache
        return *(sCachePtr->center());
    }
    
    const Trk::StraightLineSurface& TRT_BaseElement::strawSurface(int straw) const
    {
        // Create vector of all straws.
        if (!m_strawSurfaces) createStrawSurfaces();
        Trk::StraightLineSurface* surfacePtr = (*m_strawSurfaces)[straw];
        if (!surfacePtr) {
            // get the straw identifier to the given straw number and element identifier
            Identifier id =  m_idHelper->straw_id(identify(), straw);
            createSurfaceCache(id);
            surfacePtr = (*m_strawSurfaces)[straw];
        }
        return *surfacePtr;
    }

    const Amg::Transform3D& TRT_BaseElement::strawTransform(int straw) const 
    {
       if (!m_strawSurfacesCache) createStrawSurfacesCache();
       SurfaceCache* sCachePtr = (*m_strawSurfacesCache)[straw];
       if (!sCachePtr) {
           Identifier id =  m_idHelper->straw_id(identify(), straw);
           createSurfaceCache(id);
           sCachePtr = (*m_strawSurfacesCache)[straw];
       }
       // forward the transform of the cache
       return *(sCachePtr->transform());
    }
    
    const Amg::Vector3D& TRT_BaseElement::strawCenter(int straw) const 
    {
        if (!m_strawSurfacesCache) createStrawSurfacesCache();
        SurfaceCache* sCachePtr = (*m_strawSurfacesCache)[straw];
        if (!sCachePtr) {
            Identifier id =  m_idHelper->straw_id(identify(), straw);
            createSurfaceCache(id);
            sCachePtr = (*m_strawSurfacesCache)[straw];
        }
        // forward the transform of the cache
        return *(sCachePtr->center());
    }
    
    Amg::Vector3D TRT_BaseElement::strawAxis(int straw) const
    {
        return ( strawTransform(straw).linear()*Amg::Vector3D::UnitZ()*strawDirection() );
    }

    /// ----- can be removed after full Amg migration ----------------------- (start)
    // [A] in CLHEP
    
    const HepGeom::Transform3D TRT_BaseElement::transformCLHEP() const
    {	
        return Amg::EigenTransformToCLHEP(*(m_surfaceCache->transform()));
    }

    const HepGeom::Point3D<double> TRT_BaseElement::centerCLHEP() const
    {
        
        const Amg::Vector3D& cCenter = center();
        return HepGeom::Point3D<double>(cCenter.x(),cCenter.y(),cCenter.z());
    }
    
    const HepGeom::Vector3D<double> TRT_BaseElement::normalCLHEP() const
    {
        const Amg::Vector3D& cNormal = normal();
        return HepGeom::Vector3D<double>(cNormal.x(),cNormal.y(),cNormal.z());
    }

    // [B] in CLHEP
    
    const HepGeom::Transform3D TRT_BaseElement::transformCLHEP(const Identifier & id) const 
    {
        return Amg::EigenTransformToCLHEP(transform(id));
    }
    
    const HepGeom::Point3D<double> TRT_BaseElement::centerCLHEP(const Identifier & id) const 
    {
        const Amg::Vector3D cCenter = center(id);
        return HepGeom::Point3D<double>(cCenter.x(),cCenter.y(),cCenter.z());
    }
    
    const HepGeom::Vector3D<double> TRT_BaseElement::normalCLHEP(const Identifier&) const
    {
        // Not sure if the normal of the straw is ever used.
        // nor is there a well defined normal.
        // This wont be corrected for alignments.
        // Just return the element normal
        return normalCLHEP();
    }

    /// ----- can be removed after full Amg migration -----------------------  (end)


    void TRT_BaseElement::createSurfaceCache(Identifier id) const
    {

      // should not happen, but worth the protection
      if (!m_strawSurfacesCache) createStrawSurfacesCache();
      if (!m_strawSurfaces) createStrawSurfaces();

      int straw =  m_idHelper->straw(id);

      // get the StrawTransform from GeoModel 
      HepGeom::Transform3D cStrawTransform = calculateStrawTransform(straw);

      // convert neccessary parts to Amg
      Amg::Transform3D* sTransform = new Amg::Transform3D(Amg::CLHEPTransformToEigen(cStrawTransform));
      Amg::Vector3D*    sCenter    = new Amg::Vector3D(sTransform->translation());

      // create the surface cache & fill it
      SurfaceCache* sSurfaceCache     = new SurfaceCache(sTransform, sCenter, 0, 0);
      (*m_strawSurfacesCache)[straw]  = sSurfaceCache;
      
      // creaete the surface only if needed (the links are still intact)
      if (!(*m_strawSurfaces)[straw]){
          Trk::StraightLineSurface* sSurface = new Trk::StraightLineSurface(*this, id);
          (*m_strawSurfaces)[straw] = sSurface;
          
      }
    }


    void TRT_BaseElement::invalidate() const
    {
        // Invalidate the caches
        // Call and barrel or endcap specific invalidation
        invalidateOther();
        // Its enough to delete and zero the caches.
        deleteCache();
    }

    void TRT_BaseElement::deleteCache() const
    {
        // invalidates the cache, surface object can still live

        // for all straws
        if (m_strawSurfacesCache) {
            for (size_t i=0; i<m_strawSurfacesCache.load()->size(); i++) {
                delete (*m_strawSurfacesCache)[i];
                (*m_strawSurfacesCache)[i] = 0;
            }
        }
        delete m_strawSurfacesCache;
        m_strawSurfacesCache = 0;
    }


    void TRT_BaseElement::updateAllCaches() const
    {
        // delete the caches first
        deleteCache();
        // Strawlayer caches
        if (not m_surfaceCache) createSurfaceCache();
        // Loop over all straws and request items that get cached.
        for (unsigned int iStraw=0; iStraw < nStraws(); iStraw++) {
            Identifier strawId = m_idHelper->straw_id(identify(),iStraw);
            createSurfaceCache(strawId);
        }

    }

    const TRT_Conditions* TRT_BaseElement::conditions() const
    {
        return m_conditions;
    }



    TRT_BaseElement::~TRT_BaseElement()
    {
        // delete the information about the straws
        if (m_strawSurfaces) {
            for (size_t i=0; i<m_strawSurfaces.load()->size(); i++) {
                delete (*m_strawSurfaces)[i];
            }
        }
        delete m_strawSurfaces;
        deleteCache();
        
        m_conditions->unref();

    }

    void TRT_BaseElement::createStrawSurfaces() const {
        if (m_strawSurfaces!=nullptr) return;
        std::vector<Trk::StraightLineSurface*>* expected{nullptr};
        std::vector<Trk::StraightLineSurface*>* desired{new std::vector<Trk::StraightLineSurface*>(nStraws(), 0)};
        if (not m_strawSurfaces.compare_exchange_strong(expected, desired)) {
            // This happens if more than one threads try to set m_strawSurfaces.
            delete desired;
            desired = nullptr;
        }
    }

    void TRT_BaseElement::createStrawSurfacesCache() const {
        if (m_strawSurfacesCache!=nullptr) return;
        std::vector<SurfaceCache*>* expected{nullptr};
        std::vector<SurfaceCache*>* desired{new std::vector<SurfaceCache*>(nStraws(), 0)};
        if (not m_strawSurfacesCache.compare_exchange_strong(expected, desired)) {
            // This happens if more than one threads try to set m_strawSurfaces.
            delete desired;
            desired = nullptr;
        }
    }
}
