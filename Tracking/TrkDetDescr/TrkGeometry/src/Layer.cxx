/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Layer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkSurfaces/Surface.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVolumes/Volume.h"

Trk::Layer::Layer() :
  m_surfaceArray(nullptr),
  m_layerMaterialProperties(nullptr),
  m_layerThickness(0.),
  m_overlapDescriptor(nullptr),
  m_previousLayer(nullptr),
  m_nextLayer(nullptr),
  m_binUtility(nullptr),
  m_enclosingTrackingVolume(nullptr),
  m_enclosingDetachedTrackingVolume(nullptr),
  m_index(-1),
  m_layerType(Trk::active),
  m_representingVolume(nullptr),
  m_ref(0.)
{}

Trk::Layer::Layer(const Trk::LayerMaterialProperties& laymatprop,
                  double thickness,
                  Trk::OverlapDescriptor* olap,
                  int laytyp) :
  m_surfaceArray(nullptr),
  m_layerMaterialProperties(SharedObject<LayerMaterialProperties>(laymatprop.clone())),
  m_layerThickness(thickness),
  m_overlapDescriptor(olap),
  m_previousLayer(nullptr),
  m_nextLayer(nullptr),
  m_binUtility(nullptr),
  m_enclosingTrackingVolume(nullptr),
  m_enclosingDetachedTrackingVolume(nullptr),
  m_index(-1),
  m_layerType(laytyp),
  m_representingVolume(nullptr),
  m_ref(0.)
{}
        
Trk::Layer::Layer(Trk::SurfaceArray* surfaceArray,
                  double thickness,
                  Trk::OverlapDescriptor* olap,
                  int laytyp) :
  m_surfaceArray(surfaceArray),
  m_layerMaterialProperties(SharedObject<LayerMaterialProperties>(nullptr)),
  m_layerThickness(thickness),
  m_overlapDescriptor(olap),
  m_previousLayer(nullptr),
  m_nextLayer(nullptr),
  m_binUtility(nullptr),
  m_enclosingTrackingVolume(nullptr),
  m_enclosingDetachedTrackingVolume(nullptr),
  m_index(-1),
  m_layerType(laytyp),
  m_representingVolume(nullptr),
  m_ref(0.)
{}
                
Trk::Layer::Layer(Trk::SurfaceArray* surfaceArray,
                  const Trk::LayerMaterialProperties& laymatprop,
                  double thickness,
                  Trk::OverlapDescriptor* olap,
  int laytyp) :
  m_surfaceArray(surfaceArray),
  m_layerMaterialProperties(SharedObject<LayerMaterialProperties>(laymatprop.clone())),
  m_layerThickness(thickness),
  m_overlapDescriptor(olap),
  m_previousLayer(nullptr),
  m_nextLayer(nullptr),
  m_binUtility(nullptr),
  m_enclosingTrackingVolume(nullptr),
  m_enclosingDetachedTrackingVolume(nullptr),
  m_index(-1),
  m_layerType(laytyp),
  m_representingVolume(nullptr),
  m_ref(0.)
{}

Trk::Layer::Layer(const Trk::Layer& lay) :
  m_surfaceArray(lay.m_surfaceArray ? lay.m_surfaceArray->clone() : nullptr),
  m_layerMaterialProperties(lay.m_layerMaterialProperties),
  m_layerThickness(lay.m_layerThickness),
  m_overlapDescriptor(lay.m_overlapDescriptor ? lay.m_overlapDescriptor->clone() : nullptr),
  m_previousLayer(nullptr),
  m_nextLayer(nullptr),
  m_binUtility(nullptr),
  m_enclosingTrackingVolume(nullptr),
  m_enclosingDetachedTrackingVolume(nullptr),
  m_index(-1),
  m_layerType(lay.m_layerType),
  m_representingVolume(lay.m_representingVolume ?lay.m_representingVolume->clone() : nullptr),
  m_ref(lay.m_ref)
{}
  

Trk::Layer::~Layer()
{  
  delete m_surfaceArray;
  delete m_overlapDescriptor;
  delete m_representingVolume;
}


Trk::Layer& Trk::Layer::operator=(const Trk::Layer& lay)
{
  if (this != &lay){
    delete m_overlapDescriptor;
    delete m_surfaceArray;
    m_layerThickness                    = lay.m_layerThickness;
    m_enclosingTrackingVolume           = lay.m_enclosingTrackingVolume;
    m_enclosingDetachedTrackingVolume   = lay.m_enclosingDetachedTrackingVolume;
    m_overlapDescriptor                 = (lay.m_overlapDescriptor) ? lay.m_overlapDescriptor->clone() : nullptr;
    m_surfaceArray                      = (lay.m_surfaceArray) ? lay.m_surfaceArray->clone() : nullptr;
    m_layerMaterialProperties.reset(lay.m_layerMaterialProperties->clone());
    // just assign by pointer
    m_nextLayer               = lay.m_nextLayer;
    m_previousLayer           = lay.m_previousLayer;
    m_binUtility              = lay.m_binUtility;
    m_index                   = lay.m_index;
    m_layerType               = lay.m_layerType;
    m_representingVolume      = (lay.m_representingVolume) ? lay.m_representingVolume->clone() : nullptr;
    m_ref                     = lay.m_ref;
  }
  return (*this);
}

const Trk::Surface* Trk::Layer::subSurface(const Amg::Vector3D& gp) const
{
  if (m_surfaceArray) return m_surfaceArray->object(gp);
  return nullptr;
}                  

const Trk::Surface* Trk::Layer::subSurface(const Amg::Vector2D& lp) const
{
  if (m_surfaceArray) return m_surfaceArray->object(lp);
  return nullptr;
}

const Trk::Surface* Trk::Layer::subSurfaceReference(unsigned int idx) const
{
   // the reference surface
   const Trk::Surface* referenceSurface = nullptr;
   if (m_surfaceArray){    
      // get a reference surface
      const std::vector<const Trk::Surface*>& surfaces = m_surfaceArray->arrayObjects();
      // get a reference surface
      unsigned int rfSurfaces =  surfaces.size();
      if (idx && idx < rfSurfaces) return surfaces[idx];
      // get the first one which is non zero
      for (unsigned int rsf = 0; rsf < rfSurfaces; ++rsf){                                
           referenceSurface = surfaces[rsf];
           if (referenceSurface) break;
      }   
   }
   return referenceSurface;
}

bool Trk::Layer::isOnLayer(const Amg::Vector3D& gp, const BoundaryCheck& bchk) const 
{
  return (surfaceRepresentation()).isOnSurface(gp, bchk, 0.5*m_layerThickness);
}

const Trk::Layer* Trk::Layer::previousLayer(bool skipNavLayer) const 
{ 
  if (!skipNavLayer) return m_previousLayer; 
  const Trk::Layer* prevMatLayer = m_previousLayer;
  // get the previoys Material layer
  while (prevMatLayer && !prevMatLayer->layerMaterialProperties() && !prevMatLayer->surfaceArray() )
         prevMatLayer = prevMatLayer->previousLayer();
  return prevMatLayer;       

}

const Trk::Layer* Trk::Layer::nextLayer(bool skipNavLayer) const 
{ if (!skipNavLayer) return m_nextLayer; 
  const Trk::Layer* nextMatLayer = m_nextLayer;
  // get the next Material layer
  while (nextMatLayer && !nextMatLayer->layerMaterialProperties() && !nextMatLayer->surfaceArray() )
         nextMatLayer = nextMatLayer->nextLayer();
  return nextMatLayer;       
}
    

const Trk::Layer* Trk::Layer::nextLayer(const Amg::Vector3D& gp, const Amg::Vector3D& mom) const {
   // no binutility -> no chance to find out the direction
   if (!m_binUtility) return nullptr;
   return (m_binUtility->orderDirection(gp,mom) == Trk::next) ? m_nextLayer : m_previousLayer;
}

const Trk::MaterialProperties* Trk::Layer::fullUpdateMaterialProperties(const Trk::TrackParameters& parm) const {
    if (m_layerMaterialProperties.get()) return m_layerMaterialProperties->fullMaterial(parm.position());
    return nullptr;
  }

bool Trk::Layer::needsMaterialProperties() const {
  //!< @todo this is temporary    
  if (m_surfaceArray){
    const std::vector<const Trk::Surface*>& surfaces = m_surfaceArray->arrayObjects();
    for ( auto& sIter : surfaces ) {
      if (sIter && sIter->materialLayer() && (sIter->materialLayer())->layerMaterialProperties())
	     return false;	
    }
    return true;
  }
  return true;
}

void Trk::Layer::assignMaterialProperties( const LayerMaterialProperties& prop, double scale)
{
  m_layerMaterialProperties = Trk::SharedObject<LayerMaterialProperties>(prop.clone());  
  if (scale != 1.0) 
     (*(m_layerMaterialProperties.get())) *= scale;
}

void Trk::Layer::compactify(size_t& cSurfaces, size_t& tSurfaces) const {
    // set the layer surface representation
    ++tSurfaces;
    if (surfaceRepresentation().owner() == Trk::noOwn){
        surfaceRepresentation().setOwner(Trk::TGOwn);
        ++cSurfaces;
    }
    // set the subsurface representation, usually already owned by DetElement
    if (m_surfaceArray){
        const std::vector<const Trk::Surface*>& surfaces = m_surfaceArray->arrayObjects();
        for ( auto& sIter : surfaces ){
            if ( sIter && (*sIter).owner() == Trk::noOwn){
                (*sIter).setOwner(Trk::TGOwn);
                ++cSurfaces;
            }
            ++tSurfaces;
        }
    }
}

                                   
/** Surface seen on approach - if not defined differently, it is the surfaceRepresentation() */
const Trk::Surface& Trk::Layer::surfaceOnApproach(const Amg::Vector3D&,
                                                  const Amg::Vector3D&,
                                                  Trk::PropDirection,
                                                  const Trk::BoundaryCheck&,
                                                  bool,
                                                  const Trk::ICompatibilityEstimator*) const
{ return surfaceRepresentation(); }
                                                     

size_t Trk::Layer::compatibleSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces,
				      const Trk::TrackParameters& pars,
				      Trk::PropDirection pdir,
				      const Trk::BoundaryCheck& bcheck,
                      bool materialSurfacesOnly,
				      const Trk::Surface* startSurface,
				      const Trk::Surface* endSurface,
				      const Trk::ICompatibilityEstimator* ice) const
{
  return getCompatibleSurfaces(cSurfaces, pars, pdir, bcheck, materialSurfacesOnly, startSurface, endSurface, ice);
}

size_t Trk::Layer::compatibleSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces,
				      const Trk::NeutralParameters& pars,
				      Trk::PropDirection pdir,
				      const Trk::BoundaryCheck& bcheck,
                      bool materialSurfacesOnly,
				      const Trk::Surface* startSurface,
				      const Trk::Surface* endSurface,
				      const Trk::ICompatibilityEstimator* ice) const
{
  return getCompatibleSurfaces(cSurfaces, pars, pdir, bcheck, materialSurfacesOnly, startSurface, endSurface, ice);
}

inline bool Trk::Layer::hasSubStructure(bool resolveSensitive) const 
{
    return resolveSensitive && m_surfaceArray;
}
