/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolume.cxx, (b) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Gaudi Kernel
#include "GaudiKernel/MsgStream.h"
// Trk
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/NavigationLayer.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/SubtractedPlaneLayer.h"
#include "TrkGeometry/SubtractedCylinderLayer.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
#include "TrkGeometry/CylinderLayerAttemptsCalculator.h"
#include "TrkGeometry/DiscLayerAttemptsCalculator.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/BoundaryCylinderSurface.h"
#include "TrkVolumes/BoundaryDiscSurface.h"
#include "TrkVolumes/BoundaryPlaneSurface.h"
#include "TrkVolumes/BoundarySubtractedCylinderSurface.h"
#include "TrkVolumes/BoundarySubtractedPlaneSurface.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/SubtractedVolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/DistanceSolution.h"
#include "TrkGeometrySurfaces/SubtractedCylinderSurface.h"
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/NavBinnedArray1D.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"

Trk::TrackingVolume::TrackingVolume() :
  Volume(),
  Material(),
  m_motherVolume(nullptr),
  m_boundarySurfaces(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name("undefined"),
  m_colorCode(20),
  m_redoNavigation(false)
{
}

// constructor: 1 a)
ATLAS_NOT_THREAD_SAFE Trk::TrackingVolume::TrackingVolume (Amg::Transform3D* htrans,
                                                           VolumeBounds* volbounds,
                                                           const LayerArray* subLayers,
                                                           const TrackingVolumeArray* subVolumes,
                                                           const std::string& volumeName) :
  Volume(htrans, volbounds),
  Material(),
  m_motherVolume(nullptr),
  m_boundarySurfaces(nullptr),
  m_confinedLayers(subLayers),
  m_confinedVolumes(subVolumes),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
  createLayerAttemptsCalculator();
  interlinkLayers();
}   

// constructor: 2 a)
ATLAS_NOT_THREAD_SAFE Trk::TrackingVolume::TrackingVolume (const Volume& volume,
                                                           const Material& matprop,
                                                           const LayerArray* subLayers,
                                                           const TrackingVolumeArray* subVolumes,
                                                           const std::string& volumeName) :
  Volume(volume),
  Material(matprop),
  m_motherVolume(nullptr),
  m_boundarySurfaces(nullptr),
  m_confinedLayers(subLayers),
  m_confinedVolumes(subVolumes),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
  createLayerAttemptsCalculator();
  interlinkLayers();
}


// constructor: 3 a)
ATLAS_NOT_THREAD_SAFE Trk::TrackingVolume::TrackingVolume (Amg::Transform3D* htrans,
                                                           VolumeBounds* volbounds,
                                                           const Material& matprop,
                                                           const LayerArray* subLayers,
                                                           const TrackingVolumeArray* subVolumes,
                                                           const std::string& volumeName) :
  Volume(htrans, volbounds),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(subLayers),
  m_confinedVolumes(subVolumes),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
  createLayerAttemptsCalculator();
  interlinkLayers();
}


// 1 b)
Trk::TrackingVolume::TrackingVolume(Amg::Transform3D* htrans,
                                    VolumeBounds* volbounds,
                                    const Material& matprop,
                                    const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
                                    const std::string& volumeName) :
  Volume(htrans, volbounds),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(detachedSubVolumes),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}

// 2 b)
Trk::TrackingVolume::TrackingVolume(const Volume& volume,
                                    const Material& matprop,
                                    const std::vector<const DetachedTrackingVolume*>* detachedSubVolumes,
                                    const std::string& volumeName) :
  Volume(volume),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(detachedSubVolumes),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}

// 1 d)
Trk::TrackingVolume::TrackingVolume(Amg::Transform3D* htrans,
                                    VolumeBounds* volbounds,
                                    const Material& matprop,
                                    const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                                    const std::string& volumeName) :
  Volume(htrans, volbounds),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(unorderedSubVolumes),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}

// 2 d)
Trk::TrackingVolume::TrackingVolume(const Volume& volume,
                                    const Material& matprop,
                                    const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                                    const std::string& volumeName) :
  Volume(volume),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(unorderedSubVolumes),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}

// 1 c)
Trk::TrackingVolume::TrackingVolume(Amg::Transform3D* htrans,
                                    VolumeBounds* volbounds,
                                    const Material& matprop,
                                    const std::vector<const Layer*>* layers,
                                    const std::string& volumeName) :
  Volume(htrans, volbounds),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(layers),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}

// 2 c)
Trk::TrackingVolume::TrackingVolume(const Volume& volume,
                                    const Material& matprop,
                                    const std::vector<const Layer*>* layers,
                                    const std::string& volumeName) :
  Volume(volume),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(layers),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}

// 1 d)
Trk::TrackingVolume::TrackingVolume(Amg::Transform3D* htrans,
                                    VolumeBounds* volbounds,
                                    const std::vector<const Layer*>* layers,
                                    const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                                    const Material& matprop,
                                    const std::string& volumeName) :
  Volume(htrans, volbounds),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(unorderedSubVolumes),
  m_confinedArbitraryLayers(layers),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}

// 2 d)
Trk::TrackingVolume::TrackingVolume(const Volume& volume,
                                    const std::vector<const Layer*>* layers,
                                    const std::vector<const TrackingVolume*>* unorderedSubVolumes,
                                    const Material& matprop,
                                    const std::string& volumeName) :
  Volume(volume),
  Material(matprop),
  m_motherVolume(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(unorderedSubVolumes),
  m_confinedArbitraryLayers(layers),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(nullptr),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(volumeName),
  m_colorCode(20),
  m_redoNavigation(false)
{
  createBoundarySurfaces();
}


Trk::TrackingVolume::TrackingVolume(const Trk::TrackingVolume& trVol,
                    Amg::Transform3D& transform):
  Volume(trVol,transform),
  Material(trVol),
  m_motherVolume(trVol.m_motherVolume),
  m_boundarySurfaces(nullptr),
  m_confinedLayers(nullptr),
  m_confinedVolumes(nullptr),
  m_confinedDetachedVolumes(nullptr),
  m_confinedDenseVolumes(nullptr),
  m_confinedArbitraryLayers(nullptr),
  m_outsideGlueVolumes(nullptr),
  m_sensitiveVolume(trVol.m_sensitiveVolume),
  m_layerAttemptsCalculator(nullptr),
  m_geometrySignature(Trk::Unsigned),
  m_geometryType(Trk::NumberOfGeometryTypes),
  m_name(trVol.m_name),
  m_colorCode(trVol.m_colorCode),
  m_redoNavigation(trVol.m_redoNavigation)
{
  //createBoundarySurfaces
  m_boundarySurfaces = new std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >;
  m_boundarySurfaces->reserve(trVol.boundarySurfaces().size());
  const Trk::TrackingVolume* in  = nullptr;
  const Trk::TrackingVolume* out = nullptr;
  for (unsigned int ib = 0; ib < trVol.boundarySurfaces().size() ; ib++) {
    in = trVol.boundarySurfaces()[ib].get()->insideVolume() == &trVol ? this : nullptr;
    out = in == nullptr ? this : nullptr;
    const Trk::CylinderSurface* cyl = dynamic_cast<const Trk::CylinderSurface*> (trVol.boundarySurfaces()[ib].get());
    const Trk::DiscSurface*     dis = dynamic_cast<const Trk::DiscSurface*> (trVol.boundarySurfaces()[ib].get());
    const Trk::PlaneSurface*    pla = dynamic_cast<const Trk::PlaneSurface*> (trVol.boundarySurfaces()[ib].get());
    const Trk::SubtractedCylinderSurface* scyl = dynamic_cast<const Trk::SubtractedCylinderSurface*> (trVol.boundarySurfaces()[ib].get());
    const Trk::SubtractedPlaneSurface*    spla = dynamic_cast<const Trk::SubtractedPlaneSurface*> (trVol.boundarySurfaces()[ib].get());
    if (scyl) m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::TrackingVolume> >
                 (new Trk::BoundarySubtractedCylinderSurface<Trk::TrackingVolume>(in,out,*scyl,transform)));
    else if (spla)  m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::TrackingVolume> >
                 (new Trk::BoundarySubtractedPlaneSurface<Trk::TrackingVolume>(in,out,*spla,transform)));
    else if (cyl)  m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::TrackingVolume> >
                 (new Trk::BoundaryCylinderSurface<Trk::TrackingVolume>(in,out,*cyl,transform)));
    else if (dis)  m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::TrackingVolume> >
                 (new Trk::BoundaryDiscSurface<Trk::TrackingVolume>(in,out,*dis,transform)));
    else if (pla)  m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::TrackingVolume> >
                 (new Trk::BoundaryPlaneSurface<Trk::TrackingVolume>(in,out,*pla,transform)));
  }
  //createLayerAttemptsCalculator();
  //interlinkLayers();  

  // confined layers
  const Trk::BinnedArray< Trk::Layer >* confinedLayers = trVol.confinedLayers();
  if (confinedLayers){
    const std::vector<const Trk::Layer*>& layers = confinedLayers->arrayObjects();
    std::vector<Trk::SharedObject<const Trk::Layer> > layerOrder;
    layerOrder.reserve(layers.size());
    for (unsigned int i=0; i < layers.size(); i++) {
      const Trk::PlaneLayer* lay = dynamic_cast<const Trk::PlaneLayer*> (layers[i]); 
      if (lay) {
        Trk::PlaneLayer* newlay = new Trk::PlaneLayer(*lay, transform);
        layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(newlay));
      }
    }
    const Trk::NavBinnedArray1D<Trk::Layer>* confLays = dynamic_cast<const Trk::NavBinnedArray1D<Trk::Layer>*> (confinedLayers);
    if (confLays)
        m_confinedLayers = new Trk::NavBinnedArray1D<Trk::Layer>(*confLays,
                std::vector<Trk::SharedObject<const Trk::Layer> >(layerOrder), transform);
  }

  // confined 'unordered' layers
  const std::vector<const Trk::Layer* >* confinedArbitraryLayers = trVol.confinedArbitraryLayers();
  if (confinedArbitraryLayers){
    // clone & apply the transform 
    std::vector<const Trk::Layer*>  uLayers;
    uLayers.reserve(confinedArbitraryLayers->size());
    for (unsigned int i=0; i < confinedArbitraryLayers->size(); i++) {
      const Trk::SubtractedPlaneLayer* slayer     = dynamic_cast<const Trk::SubtractedPlaneLayer*>((*confinedArbitraryLayers)[i]) ; 
      const Trk::SubtractedCylinderLayer* sclayer = dynamic_cast<const Trk::SubtractedCylinderLayer*>((*confinedArbitraryLayers)[i]) ;
      const Trk::PlaneLayer*                layer = dynamic_cast<const Trk::PlaneLayer*>((*confinedArbitraryLayers)[i]) ; 
      const Trk::CylinderLayer*            clayer = dynamic_cast<const Trk::CylinderLayer*>((*confinedArbitraryLayers)[i]) ; 
      
      if (slayer) {
        const Trk::SubtractedPlaneLayer* lay = new Trk::SubtractedPlaneLayer(*slayer, transform);
        uLayers.push_back(lay); 
      } else if (layer) {
        const Trk::PlaneLayer* lay = new Trk::PlaneLayer(*layer,transform);
        uLayers.push_back(lay);
      } else if (sclayer) {
        const Trk::SubtractedCylinderLayer* lay = new Trk::SubtractedCylinderLayer(*sclayer, transform);
        uLayers.push_back(lay);
      } else if (clayer) {
        const Trk::CylinderLayer* lay = new Trk::CylinderLayer(*clayer, transform);
        uLayers.push_back(lay);
      }
    }
    m_confinedArbitraryLayers = new std::vector<const Trk::Layer*>(uLayers);
  }
  
  // confined volumes
  const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = trVol.confinedVolumes();
  if (confinedVolumes){
    // retrieve array objects and apply the transform 
    const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes->arrayObjects();
    std::vector<Trk::SharedObject<const Trk::TrackingVolume> > volOrder;
    volOrder.reserve(volumes.size()); 
    for (unsigned int i=0; i < volumes.size(); i++) {
      const Trk::TrackingVolume* vol = new Trk::TrackingVolume(*(volumes[i]),transform);
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>(vol) );
    }
    const Trk::NavBinnedArray1D<Trk::TrackingVolume>* confVols = dynamic_cast<const Trk::NavBinnedArray1D<Trk::TrackingVolume>*> (confinedVolumes);
    if (confVols)
        m_confinedVolumes =  new Trk::NavBinnedArray1D<Trk::TrackingVolume>(*confVols,
                std::vector<Trk::SharedObject<const Trk::TrackingVolume> >(volOrder), transform);
  }
  
  // confined unordered volumes
  const std::vector<const Trk::TrackingVolume* >* confinedDenseVolumes = trVol.confinedDenseVolumes();
  if (confinedDenseVolumes){
    std::vector<const Trk::TrackingVolume*> newVol;
    newVol.reserve( confinedDenseVolumes->size() );
    // retrieve array objects and apply the transform 
    for (unsigned int i=0; i < confinedDenseVolumes->size(); i++) {
      const Trk::TrackingVolume* vol = new Trk::TrackingVolume( *((*confinedDenseVolumes)[i]), transform );
      newVol.push_back(vol); 
    }
    m_confinedDenseVolumes = new std::vector<const Trk::TrackingVolume*>(newVol);
  }  
}

Trk::TrackingVolume::~TrackingVolume()
{
   delete m_boundarySurfaces;   
   delete m_confinedLayers;
   delete m_confinedVolumes;
   delete m_confinedDetachedVolumes;
   if (m_confinedDenseVolumes) {
     for (size_t i =0; i < m_confinedDenseVolumes->size(); i++)
       delete (*m_confinedDenseVolumes)[i];
     delete m_confinedDenseVolumes;
   }
   if (m_confinedArbitraryLayers) {
     for (size_t i =0; i < m_confinedArbitraryLayers->size(); i++)
       delete (*m_confinedArbitraryLayers)[i];
     delete m_confinedArbitraryLayers;
   }
   delete m_outsideGlueVolumes;
   delete m_sensitiveVolume;
   delete m_layerAttemptsCalculator;
}

void Trk::TrackingVolume::clear() 
{
  if (m_confinedVolumes) { delete m_confinedVolumes; m_confinedVolumes=nullptr; }
  if (m_confinedLayers)  { delete m_confinedLayers; m_confinedLayers=nullptr; }
  if (m_confinedDenseVolumes) {
    for (size_t i =0; i < m_confinedDenseVolumes->size(); i++)
      delete (*m_confinedDenseVolumes)[i];
    delete m_confinedDenseVolumes;
    m_confinedDenseVolumes = nullptr;
  }
  if (m_confinedArbitraryLayers) {
    for (size_t i =0; i < m_confinedArbitraryLayers->size(); i++)
      delete (*m_confinedArbitraryLayers)[i];
    delete m_confinedArbitraryLayers;
    m_confinedArbitraryLayers = nullptr; 
  }
}


const Trk::Layer*   Trk::TrackingVolume::associatedLayer(const Amg::Vector3D& gp) const
{  
  // confined layers
  if (m_confinedLayers)
     return (m_confinedLayers->object(gp));
  // confined arbitrary   
  if (m_confinedArbitraryLayers) {
    for (unsigned int i=0; i < m_confinedArbitraryLayers->size(); i++) 
      if ( (*m_confinedArbitraryLayers)[i]->isOnLayer(gp) ) return (*m_confinedArbitraryLayers)[i]; 
   }
  return nullptr;
}
      
const Trk::Layer*   Trk::TrackingVolume::nextLayer(const Amg::Vector3D& gp,
                                                   const Amg::Vector3D& mom,
                                                   bool associatedResult,
                                                   bool skipNavLayer) const
{
  const Trk::Layer* nextLayer = nullptr;
  if (m_confinedLayers)
      nextLayer = (m_confinedLayers->nextObject(gp, mom, associatedResult));
  // forward it in this case
  if (!skipNavLayer) return nextLayer;
  // if only material or layers          
  if (nextLayer && ( nextLayer->layerMaterialProperties() || nextLayer->surfaceArray() ) ) return nextLayer;
  // try to get the next layer that has either material or sub surfaces
  while (nextLayer && ( !(nextLayer->layerMaterialProperties()) && !(nextLayer->surfaceArray()) ) )
         nextLayer = (m_confinedLayers->nextObject(gp, mom, associatedResult));    
  return nextLayer;
}

Trk::LayerIntersection<Amg::Vector3D> 
                   Trk::TrackingVolume::closestMaterialLayer(const Amg::Vector3D& gp,
                                                        const Amg::Vector3D& dir, 
                                                        PropDirection pDir,
                                                        const BoundaryCheck& bchk) const
{
  
  // the layer candidates to check
  std::vector<const Layer*> layerCandidates;

  // ---------------- BOUNDARY LAYER SECTION (only for mapping) ----------
  if (pDir == mappingMode){
      for (auto& bSurface : boundarySurfaces()){
          if (bSurface->surfaceRepresentation().materialLayer())
              layerCandidates.push_back(bSurface->surfaceRepresentation().materialLayer());
      }
  }
  // ---------------- CONFINED LAYER SECTION --------------
  if (m_confinedLayers){
       // the associated layer
       const Trk::Layer* assocLayer       = associatedLayer(gp);
       const Trk::Layer* previousMatLayer = nullptr;
       const Trk::Layer* nextMatLayer     = nullptr;
       // if the associated layer is a navigation layer - get the previous and next from this one
       const Trk::NavigationLayer* navLayer = dynamic_cast<const Trk::NavigationLayer*>(assocLayer);     
       if (navLayer) {
           // get previous / next
           previousMatLayer   = navLayer->previousLayer();
           nextMatLayer       = navLayer->nextLayer();
           if (previousMatLayer) 
               layerCandidates.push_back(previousMatLayer);
           if (nextMatLayer) 
               layerCandidates.push_back(nextMatLayer);
       } else
           layerCandidates.push_back(assocLayer);
    }
    // --- solve for the layer candidates ---------------------
    //
    Trk::Intersection laySurfIntersection(Amg::Vector3D(0.,0.,0.), 10e10, false);    
    // layer candidates found - continue
    if (!layerCandidates.empty()){
        const Layer*        cLayer     = nullptr;
        // iterate through and chose
        for ( auto& lcIter : layerCandidates ){
            // only the forceFwd solution is possible here
            bool forceDir = ( pDir == alongMomentum || pDir == oppositeMomentum);
            double dirScalor = ( pDir == oppositeMomentum)  ? -1. : 1.;
            // get the intersection soltuion
            Trk::Intersection sfI = (*lcIter).surfaceRepresentation().straightLineIntersection(gp, dirScalor*dir, forceDir, bchk);
            if (sfI.valid && (sfI.pathLength*sfI.pathLength) < (laySurfIntersection.pathLength*laySurfIntersection.pathLength) ){
                laySurfIntersection  = sfI;
                cLayer = lcIter;
            }
        }
        // now return the pair: in case of a valid intersection, or if no mapping mode is chosen
        if (cLayer) 
          return Trk::LayerIntersection< Amg::Vector3D > (laySurfIntersection, cLayer, &(cLayer->surfaceRepresentation()), nullptr ,pDir);
     }
     // mapping mode chosen, but no valid intersection yet
     const Trk::TrackingVolume* nVolume = nextVolume(gp, dir, pDir);
     
     // forward the next Volume solution or a 0 solution
     return ( nVolume && nVolume != this ) ? nVolume->closestMaterialLayer(gp,dir,pDir,bchk) : 
        Trk::LayerIntersection< Amg::Vector3D >(laySurfIntersection, nullptr, nullptr, nullptr, pDir);
}
     
                                                                
const Trk::TrackingVolume* Trk::TrackingVolume::associatedSubVolume(const Amg::Vector3D& gp) const
{
  if (m_confinedVolumes) 
     return (m_confinedVolumes->object(gp));

  if (m_confinedDetachedVolumes) {
    for (size_t i=0; i < m_confinedDetachedVolumes->size(); i++) {
      if ( (*m_confinedDetachedVolumes)[i]->trackingVolume()->inside(gp,0.001) ) return ((*m_confinedDetachedVolumes)[i])->trackingVolume(); 
    }
  }

  if (m_confinedDenseVolumes) {
    for (size_t  i=0; i < m_confinedDenseVolumes->size(); i++) 
      if ( (*m_confinedDenseVolumes)[i]->inside(gp,0.001) ) return (*m_confinedDenseVolumes)[i]; 
  }

  return this;
}
      
const Trk::TrackingVolume* Trk::TrackingVolume::nextVolume(const Amg::Vector3D& gp, const Amg::Vector3D& dir, Trk::PropDirection pDir) const {
    // get the boundary surfaces & intersect them
    const Trk::TrackingVolume* nVolume = nullptr;
    // fix the direction once
    bool forceDir = ( pDir == Trk::alongMomentum || pDir == Trk::oppositeMomentum);
    double dirScalor = ( pDir == Trk::oppositeMomentum)  ? -1. : 1.;
    Amg::Vector3D cDir = dirScalor*dir;
    double pathLength = 10e10;
    // now loop through the and find the closest 
    const auto &bSurfaces = boundarySurfaces();
    for (auto& bSurfIter : bSurfaces ){
        // get the intersection soltuion
        Trk::Intersection sfI = (*bSurfIter).surfaceRepresentation().straightLineIntersection(gp, cDir, forceDir, true);
        if (sfI.valid && (sfI.pathLength*sfI.pathLength) < (pathLength*pathLength) ){
            // assign the next Volume
            Trk::PropDirection attachedDir = sfI.pathLength > 0. ? Trk::alongMomentum : Trk::oppositeMomentum;
            pathLength = sfI.pathLength;
            nVolume    = (*bSurfIter).attachedVolume(gp, cDir, attachedDir );
        }
    }
    return nVolume;
}
  
      
const Trk::TrackingVolume* Trk::TrackingVolume::nextSubVolume(const Amg::Vector3D& gp, const Amg::Vector3D& mom) const
{
  if (m_confinedVolumes)
     return (m_confinedVolumes->nextObject(gp, mom));
  return this;
}

std::vector<const Trk::DetachedTrackingVolume*>* Trk::TrackingVolume::assocDetachedSubVolumes(const Amg::Vector3D& gp, double tol) const
{
  std::vector<const Trk::DetachedTrackingVolume*>* currVols = new std::vector<const Trk::DetachedTrackingVolume*>;

  const std::vector<const Trk::DetachedTrackingVolume*>* detVols = confinedDetachedVolumes();
  if (detVols) {
    for (unsigned int i=0; i < detVols->size(); i++) { 
      if ( (*detVols)[i]->trackingVolume()->inside(gp,tol) ) currVols->push_back((*detVols)[i]); 
    }   
  }
  return currVols;
}


void Trk::TrackingVolume::indexContainedStaticLayers ATLAS_NOT_THREAD_SAFE(GeometrySignature geoSig, int& offset) const
{

  // the offset gets increased internally
  // the static layers first ------------------------------------------------------------------
  if (m_confinedLayers){
    const std::vector<const Trk::Layer*>& layers = m_confinedLayers->arrayObjects();
    for ( auto& layerIter : layers){
      // only index the material layers & only those that have not yet been singed
      if ( layerIter && layerIter->layerIndex().value() < 0 ){
	    // sign only those with material properties - rest goes to 0
	    Trk::LayerIndex layIndex = layerIter->layerMaterialProperties() ?
	    Trk::LayerIndex(int(geoSig)*TRKDETDESCR_GEOMETRYSIGNATUREWEIGHT+(++offset)) :  Trk::LayerIndex(0);
	    // now register the index      
	    layerIter->registerLayerIndex(layIndex);          
      }
    }
  }
  
  // the boundary surface layer
  auto& bSurfaces = boundarySurfaces();
  for (auto& bsIter : bSurfaces ){
    const Trk::Layer* mLayer = bsIter->surfaceRepresentation().materialLayer();
    if (mLayer && mLayer->layerIndex().value() < 0.){
      Trk::LayerIndex layIndex = Trk::LayerIndex(int(geoSig)*TRKDETDESCR_GEOMETRYSIGNATUREWEIGHT+(++offset));
      mLayer->registerLayerIndex(layIndex);
    }
  }
  
  // step down the hierarchy to the contained volumes and index those ------------------------
  if (confinedVolumes()){
    const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes()->arrayObjects();
    for (auto& volumesIter :  volumes){
      if (volumesIter) 
	volumesIter->indexContainedStaticLayers(geoSig, offset);
    }
  }
}

void Trk::TrackingVolume::indexContainedMaterialLayers ATLAS_NOT_THREAD_SAFE (GeometrySignature geoSig, int& offset) const {
  
  // the offset gets increased internally
  // the static layers first and check if they have surfaces with material layers that need index
  if (m_confinedLayers){
    const std::vector<const Trk::Layer*>& layers = m_confinedLayers->arrayObjects();
    for ( auto& layerIter : layers){
      // only index the material layers & only those that have not yet been singed
      if ( layerIter ){
	    const Trk::SurfaceArray* surfArray = layerIter->surfaceArray();
	    if (surfArray) {
	      const std::vector<const Trk::Surface*>& layerSurfaces = surfArray->arrayObjects();
	      // loop over the surfaces - there can be 0 entries
	      for (auto& laySurfIter : layerSurfaces) {
	            const Trk::Layer* materialLayer = laySurfIter ? laySurfIter->materialLayer() : nullptr;
	            if ( materialLayer && materialLayer->layerIndex().value() < 0 ){
	              // sign only those with material properties - rest goes to 0
	              Trk::LayerIndex layIndex = materialLayer->layerMaterialProperties() ?
	    	      Trk::LayerIndex(int(geoSig)*TRKDETDESCR_GEOMETRYSIGNATUREWEIGHT+(++offset)) :  Trk::LayerIndex(0);
	              // now register the index      
	              materialLayer->registerLayerIndex(layIndex);          
	            }
	          }
	        }
         }
      }
  }
  
  // step down the hierarchy to the contained volumes and index those ------------------------
  if (confinedVolumes()){
    const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes()->arrayObjects();
    for (auto& volumesIter :  volumes){
      if (volumesIter) 
	volumesIter->indexContainedMaterialLayers(geoSig, offset);
    }
  }
}


void Trk::TrackingVolume::addMaterial(const Trk::Material& mprop, float fact) 
{
  // assume the scaling factor refers to the volume scaling
  float flin = pow(fact,0.33); 
  //average X0
  double invX0 = X0>0. ? 1./X0 : 0.;
  double sum_invX0 = invX0 + flin/mprop.X0;
  X0 = 1./sum_invX0;
  //average L0
  double invL0 = L0>0. ? 1./L0 : 0.;
  double sum_invL0 = invL0 + flin/mprop.L0;
  L0 = 1./sum_invL0;
  //add density
  float rho1 = rho;
  rho += fact*mprop.rho; 
  // averageZ
  float n1 = Z>0. ? rho1/Z : 0.;  
  float n2 = fact*mprop.rho/mprop.Z;
  Z = rho/(n1+n2);   
  // averageA
  n1 = A>0. ? rho1/A : 0.;  
  n2 = fact*mprop.rho/mprop.A;
  A = rho/(n1+n2);   
  // zOverAtimesRho 
  zOaTr = Z/A*rho;  
  // mean energy loss (linear scaling)
  dEdX += flin*mprop.dEdX; 
}

void Trk::TrackingVolume::addMaterial ATLAS_NOT_THREAD_SAFE(const Material &mat,
                                                                  float fact) const{
  const_cast<Trk::TrackingVolume*>(this)->addMaterial(mat,fact);
}
 
void Trk::TrackingVolume::propagateMaterialProperties ATLAS_NOT_THREAD_SAFE(const Trk::Material& mprop){
  
  X0            = mprop.X0;
  L0            = mprop.L0;
  Z             = mprop.Z;
  A             = mprop.A;
  rho           = mprop.rho;  
  zOaTr         = mprop.zOaTr;
  dEdX          = mprop.dEdX;  

  // only do the loop over confined static objects
  const Trk::BinnedArray< Trk::TrackingVolume >* confVolumes = confinedVolumes();
   if (confVolumes){
    const std::vector<const Trk::TrackingVolume*>& volumes = confVolumes->arrayObjects();
    for (auto& volumesIter : volumes ){
        if (volumesIter) volumesIter->propagateMaterialProperties(mprop);
    }
  }

}

void Trk::TrackingVolume::sign ATLAS_NOT_THREAD_SAFE (Trk::GeometrySignature geosign, Trk::GeometryType geotype) const
{

  // never overwrite what is already signed, that's a crime
  if (m_geometrySignature == Trk::Unsigned) const_cast<Trk::GeometrySignature&> (m_geometrySignature) = geosign;
  const_cast<Trk::GeometryType&> (m_geometryType) = geotype;

  // confined volumes
  const Trk::BinnedArray< Trk::TrackingVolume >* confVolumes = confinedVolumes();
  if (confVolumes){
    const std::vector<const Trk::TrackingVolume*>& volumes = confVolumes->arrayObjects();
    for ( auto& volumesIter : volumes )
        if (volumesIter) volumesIter->sign(geosign, geotype);
  }
  // same procedure for the detached volumes
  const std::vector<const Trk::DetachedTrackingVolume* >* confDetachedVolumes = confinedDetachedVolumes();
  if (confDetachedVolumes)
     for (auto& volumesIter : (*confDetachedVolumes) )
       if (volumesIter) volumesIter->sign(geosign, geotype);
  // confined dense volumes
  const std::vector<const Trk::TrackingVolume* >* confDenseVolumes = confinedDenseVolumes();
  if (confDenseVolumes)
     for (auto& volumesIter : (*confDenseVolumes) )
       if (volumesIter) volumesIter->sign(geosign, geotype);
}

const std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > >&  Trk::TrackingVolume::boundarySurfaces() const
{ return (*m_boundarySurfaces); }

const Trk::BoundarySurface<Trk::TrackingVolume>* Trk::TrackingVolume::boundarySurface(const ObjectAccessor::value_type& oa) const
{
  return (m_boundarySurfaces->operator[](oa)).get();
}

void Trk::TrackingVolume::createBoundarySurfaces ATLAS_NOT_THREAD_SAFE ()
{
  // prepare the BoundarySurfaces
  m_boundarySurfaces = new std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > >;
  // transform Surfaces To BoundarySurfaces
  const std::vector<const Trk::Surface*>* surfaces = Trk::Volume::volumeBounds().decomposeToSurfaces(transform());
  std::vector<const Trk::Surface*>::const_iterator surfIter = surfaces->begin();

  // counter to flip the inner/outer position for Cylinders
  unsigned int sfCounter = 0;
  unsigned int sfNumber  = surfaces->size();

  // memory optimisation
  m_boundarySurfaces->reserve(sfNumber+1);
  
  // identify Subtracted/CombinedVolumes
  const Trk::SubtractedVolumeBounds* subtrVol = dynamic_cast<const Trk::SubtractedVolumeBounds*> 
                                                                    (&( Trk::Volume::volumeBounds()));
  const Trk::CombinedVolumeBounds*   combVol  = dynamic_cast<const Trk::CombinedVolumeBounds*> 
                                                                    (&( Trk::Volume::volumeBounds()));
  bool subtr = (subtrVol) ? 1 : 0;
  bool comb  = (combVol)  ? 1 : 0;

  if (!subtr && !comb) {

    const Trk::SimplePolygonBrepVolumeBounds*   spbVol  = dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*> 
                                                                    (&( Trk::Volume::volumeBounds()));  

    for ( ; surfIter != surfaces->end(); ++surfIter){
      sfCounter++;

      Trk::TrackingVolume* in  = this;
      Trk::TrackingVolume* out = nullptr;
   
      // ST update: subtracted surfaces may appear in 'simple' volumes (SimplePolygonBrep...)
      const Trk::SubtractedPlaneSurface*      spsf = dynamic_cast<const Trk::SubtractedPlaneSurface*>(*surfIter);
      const Trk::PlaneSurface*      psf = dynamic_cast<const Trk::PlaneSurface*>(*surfIter);
      if (spsf) { 
        if (spbVol && sfCounter==1 ) {in = nullptr; out = this;}
	            m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> >
				      (new Trk::BoundarySubtractedPlaneSurface<Trk::TrackingVolume>(in, out, *spsf)));    
	    delete spsf; continue;
      }
      if (psf) {
        m_boundarySurfaces->push_back(
          Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume>>(
            new Trk::BoundaryPlaneSurface<Trk::TrackingVolume>(in, out, *psf)));
        delete psf;
        continue;
      }

      const Trk::DiscSurface*       dsf = dynamic_cast<const Trk::DiscSurface*>(*surfIter);
      if (dsf) {
          m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> >
              (new Trk::BoundaryDiscSurface<Trk::TrackingVolume>(in, out, *dsf)));
        delete dsf; continue;
      }

      const Trk::SubtractedCylinderSurface*  scsf = dynamic_cast<const Trk::SubtractedCylinderSurface*>(*surfIter);
      const Trk::CylinderSurface*   csf = dynamic_cast<const Trk::CylinderSurface*>(*surfIter);
      if (scsf) {
          Trk::TrackingVolume* inner = (sfCounter == 4 && sfNumber > 3) ? nullptr : this;
          Trk::TrackingVolume* outer = (inner) ? nullptr : this;
          m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::TrackingVolume> >
            (new Trk::BoundarySubtractedCylinderSurface<Trk::TrackingVolume>(inner, outer, *scsf)));
          delete scsf; continue;
      }
      if (csf) {
        Trk::TrackingVolume* inner =
          (sfCounter == 4 && sfNumber > 3) ? nullptr : this;
        Trk::TrackingVolume* outer = (inner) ? nullptr : this;
        m_boundarySurfaces->push_back(
          Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume>>(
            new Trk::BoundaryCylinderSurface<Trk::TrackingVolume>(
              inner, outer, *csf)));
        delete csf;
        continue;
      }
    }

  } else {

    const std::vector<bool> bOrient = subtrVol ? subtrVol->boundsOrientation() : combVol->boundsOrientation() ;
     
    for ( ; surfIter != surfaces->end(); ++surfIter){
      
      Trk::TrackingVolume* in  = bOrient[sfCounter] ? this : nullptr;
      Trk::TrackingVolume* out = bOrient[sfCounter] ? nullptr : this;
      sfCounter++;
   
      const Trk::SubtractedPlaneSurface*      psf = dynamic_cast<const Trk::SubtractedPlaneSurface*>(*surfIter);
      if (psf) {
          m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> >
              (new Trk::BoundarySubtractedPlaneSurface<Trk::TrackingVolume>(in, out, *psf)));
          delete psf; continue;
      }        
      
      const Trk::SubtractedCylinderSurface*   csf = dynamic_cast<const Trk::SubtractedCylinderSurface*>(*surfIter);
      if (csf) {
          m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::TrackingVolume> >
              (new Trk::BoundarySubtractedCylinderSurface<Trk::TrackingVolume>(in, out, *csf)));
          delete csf; continue;
      }
    }
  }
  
  delete surfaces;
}


void Trk::TrackingVolume::createLayerAttemptsCalculator()
{
  // check whether there's a static array
  if (m_confinedLayers){
     // BinUtility
     const Trk::BinUtility* binUtility = m_confinedLayers->binUtility();
     if (binUtility){
         if (binUtility->binningValue() == Trk::binR)
             m_layerAttemptsCalculator = new Trk::CylinderLayerAttemptsCalculator(1,5);
         if (binUtility->binningValue() == Trk::binZ)
             m_layerAttemptsCalculator = new Trk::DiscLayerAttemptsCalculator(1,5);
     }
   }
}

void Trk::TrackingVolume::interlinkLayers ATLAS_NOT_THREAD_SAFE() {
  if (m_confinedLayers){
    const std::vector<const Trk::Layer*>& layers = m_confinedLayers->arrayObjects();
    // forward loop
    const Trk::Layer* lastLayer = nullptr;
    std::vector<const Trk::Layer*>::const_iterator layerIter = layers.begin();
    for ( ; layerIter != layers.end(); ++layerIter)
      { 
	if (*layerIter) {
	  // register the layers
	  (**layerIter).setBinUtility(m_confinedLayers->binUtility() ? m_confinedLayers->binUtility() : nullptr); 
	  (**layerIter).setPreviousLayer(lastLayer);
	  // register the volume
	  (**layerIter).encloseTrackingVolume(*this); 
	}
	lastLayer = (*layerIter);
      }
    // backward loop
    lastLayer = nullptr;
    layerIter = layers.end();
    --layerIter;
    for ( ; ; --layerIter)
      { 
	if (*layerIter)
	  (**layerIter).setNextLayer(lastLayer);
  lastLayer = (*layerIter);
  if (layerIter == layers.begin()) break;
      }
  }
}

const Trk::LayerArray* Trk::TrackingVolume::checkoutConfinedLayers() const
{ 
    const Trk::LayerArray* checkoutLayers = m_confinedLayers;
    return checkoutLayers;
}

void Trk::TrackingVolume::registerOutsideGlueVolumes (Trk::GlueVolumesDescriptor* gvd)
{ 
  delete m_outsideGlueVolumes;
  m_outsideGlueVolumes = gvd;
}

void Trk::TrackingVolume::registerOutsideGlueVolumes ATLAS_NOT_THREAD_SAFE (Trk::GlueVolumesDescriptor* gvd) const {
  const_cast<Trk::TrackingVolume*>(this)->registerOutsideGlueVolumes(gvd);
}

const Trk::GlueVolumesDescriptor& Trk::TrackingVolume::glueVolumesDescriptor() {
  if (!m_outsideGlueVolumes)
    m_outsideGlueVolumes = new Trk::GlueVolumesDescriptor;
  return (*m_outsideGlueVolumes);
}

const Trk::GlueVolumesDescriptor& Trk::TrackingVolume::glueVolumesDescriptor ATLAS_NOT_THREAD_SAFE () const{
    return (const_cast<Trk::TrackingVolume*>(this))->glueVolumesDescriptor();
}


void Trk::TrackingVolume::moveVolume ATLAS_NOT_THREAD_SAFE (Amg::Transform3D& shift) const
{
  if (m_transform) {
    Amg::Transform3D transf = shift * (*m_transform);
    const_cast<Trk::TrackingVolume*>(this)->m_transform = std::make_unique<Amg::Transform3D>(transf);
  } else {
    const_cast<Trk::TrackingVolume*>(this)->m_transform = std::make_unique<Amg::Transform3D>(shift);
  }
  const_cast<Trk::TrackingVolume*>(this)->m_center.store(std::make_unique<Amg::Vector3D>(m_transform->translation())); 
}


const Trk::TrackingVolume* Trk::TrackingVolume::cloneTV ATLAS_NOT_THREAD_SAFE (Amg::Transform3D& transform) const
{
  // clone the mother volume
  const Trk::Volume* vol = Trk::Volume::clone();
  
  // clone 'ordered layers
  Trk::LayerArray*  layerArray = nullptr;
  // confined layers
  const Trk::BinnedArray< Trk::Layer >* confLayers = confinedLayers();
  if (confLayers){
    // retrieve array objects and apply the transform 
    const std::vector<const Trk::Layer*>& layers = confLayers->arrayObjects();
    std::vector<Trk::SharedObject<const Trk::Layer> > layerOrder;
    for (unsigned int i=0; i < layers.size(); i++) {
      const Trk::PlaneLayer* lay = dynamic_cast<const Trk::PlaneLayer*> (layers[i]); 
      if (lay) {
        Trk::PlaneLayer* newlay = new Trk::PlaneLayer(*lay, transform);
        layerOrder.push_back( Trk::SharedObject<const Trk::Layer>(newlay)); 
      }
    }
    // recreate LayerArray  
    const Trk::NavBinnedArray1D<Trk::Layer>* confLaysNav = dynamic_cast<const Trk::NavBinnedArray1D<Trk::Layer>*> (confLayers);
    if (confLaysNav)
        layerArray =  new Trk::NavBinnedArray1D<Trk::Layer>(
                *confLaysNav,
                std::vector<Trk::SharedObject<const Trk::Layer> >(layerOrder),
                transform);
  }

  // clone 'unordered' layers
  std::vector<const Trk::Layer*>*  unorderedLayers = nullptr;
  const std::vector<const Trk::Layer* >* confArbLayers = confinedArbitraryLayers();
  if (confArbLayers){
    // clone & apply the transform 
    std::vector<const Trk::Layer*>  uLayers;
    for (unsigned int i=0; i < confArbLayers->size(); i++) {
      const Trk::SubtractedPlaneLayer* slayer     = dynamic_cast<const Trk::SubtractedPlaneLayer*>((*confArbLayers)[i]) ; 
      const Trk::SubtractedCylinderLayer* sclayer = dynamic_cast<const Trk::SubtractedCylinderLayer*>((*confArbLayers)[i]) ;
      const Trk::PlaneLayer*                layer = dynamic_cast<const Trk::PlaneLayer*>((*confArbLayers)[i]) ; 
      const Trk::CylinderLayer*            clayer = dynamic_cast<const Trk::CylinderLayer*>((*confArbLayers)[i]) ; 
      
      if (slayer) {
        const Trk::SubtractedPlaneLayer* lay = new Trk::SubtractedPlaneLayer(*slayer);
        lay->moveLayer( transform);
        uLayers.push_back(lay); 
      } else if (layer) {
        const Trk::PlaneLayer* lay = new Trk::PlaneLayer(*layer);
        lay->moveLayer( transform );
        uLayers.push_back(lay);
      } else if (sclayer) {
        const Trk::SubtractedCylinderLayer* lay = new Trk::SubtractedCylinderLayer(*sclayer);
        lay->moveLayer( transform );
        uLayers.push_back(lay);
      } else if (clayer) {
        const Trk::CylinderLayer* lay = new Trk::CylinderLayer(*clayer);
        lay->moveLayer( transform );
        uLayers.push_back(lay);
      }
    }
    unorderedLayers = new std::vector<const Trk::Layer*>(uLayers);
  }
  
  // cloning confined volumes
  Trk::TrackingVolumeArray*  volumeArray = nullptr;
  const Trk::BinnedArray< Trk::TrackingVolume >* confVolumes = confinedVolumes();
  if (confVolumes) {
    // retrieve array objects and apply the transform 
    const std::vector<const Trk::TrackingVolume*>& volumes = confVolumes->arrayObjects();
    std::vector<Trk::SharedObject<const TrackingVolume> > volOrder;
    for (unsigned int i=0; i < volumes.size(); i++) {
      const Trk::TrackingVolume* vol = volumes[i]->cloneTV( transform );
      volOrder.push_back(Trk::SharedObject<const TrackingVolume>(vol)); 
    }
    // recreate TrackingVolumeArray
    const Trk::NavBinnedArray1D<Trk::TrackingVolume>* confVolsNav = dynamic_cast<const Trk::NavBinnedArray1D<Trk::TrackingVolume>*> (confVolumes);
    if (confVolsNav)
        volumeArray =  new Trk::NavBinnedArray1D<Trk::TrackingVolume>(
                *confVolsNav,
                std::vector<Trk::SharedObject<const TrackingVolume> >(volOrder),
                transform );
  }
  
  // cloning confined unordered volumes
  const std::vector<const Trk::TrackingVolume* >* confDenseVolumes = confinedDenseVolumes();
  std::vector<const Trk::TrackingVolume*>* newDenseVol=nullptr; 
  if (confDenseVolumes) {
    std::vector<const Trk::TrackingVolume*> newVol; 
    // retrieve array objects and apply the transform 
    for (unsigned int i=0; i < confDenseVolumes->size(); i++) {
      const Trk::TrackingVolume* vol = (*confDenseVolumes)[i]->cloneTV( transform );
      newVol.push_back(vol); 
    }
    newDenseVol = new std::vector<const Trk::TrackingVolume*>(newVol);
  }
  
  // create the Tracking Volume
  Trk::TrackingVolume* newTrkVol = nullptr;
  if (confArbLayers || confDenseVolumes) {
    if (confArbLayers && confDenseVolumes ) { 
      newTrkVol = new Trk::TrackingVolume(*vol, 
                                          unorderedLayers,
                                          newDenseVol,
                                          *this,
                                          volumeName());
    } else if (confArbLayers) { 
      newTrkVol = new Trk::TrackingVolume(*vol,
                                          *this, 
                                          unorderedLayers,
                                          volumeName());
    } else {
      newTrkVol = new Trk::TrackingVolume(*vol,
                                          *this, 
                                          newDenseVol,
                                          volumeName());
    }
    delete layerArray;
  } else {
    newTrkVol = new Trk::TrackingVolume(*vol,
                                        *this, 
                                        layerArray,volumeArray,
                                        volumeName());
  }
  delete vol;
  // finally, position the mother volume
  newTrkVol -> moveVolume( transform ); 
  // create boundary surfaces
  delete newTrkVol->m_boundarySurfaces;
  newTrkVol -> createBoundarySurfaces();
  
  delete volumeArray;
  return newTrkVol;
}

const Trk::Layer* Trk::TrackingVolume::closest(const Amg::Vector3D& pos,
                                               const Amg::Vector3D& dir,
                                               const Trk::Layer& first,
                                               const Trk::Layer& second) const
{
   // use the distance solution for assigning the layer            
   Trk::DistanceSolution distSolToPrevious 
        = first.surfaceRepresentation().straightLineDistanceEstimate(pos, dir);
   Trk::DistanceSolution distSolToNext
        = second.surfaceRepresentation().straightLineDistanceEstimate(pos, dir);
   // find out which one
   return (distSolToPrevious.absClosest() < distSolToNext.absClosest() ? &first : &second); 
}                                               

                                
void  Trk::TrackingVolume::moveTV ATLAS_NOT_THREAD_SAFE (Amg::Transform3D& transform) const {

  moveVolume( transform ); 

  // confined 'ordered' layers
  const Trk::BinnedArray< Trk::Layer >* confLayers = confinedLayers();
  if (confLayers)
    for (auto& clayIter : confLayers->arrayObjects() )
        clayIter->moveLayer( transform );

  // confined 'unordered' layers
  const std::vector<const Trk::Layer* >* confArbLayers = confinedArbitraryLayers();
  if (confArbLayers)
      for (auto& calayIter : (*confArbLayers) )
          calayIter->moveLayer( transform );

  // confined volumes
  const Trk::BinnedArray< Trk::TrackingVolume >* confVolumes = confinedVolumes();
  if (confVolumes)
    // retrieve array objects and apply the transform 
    for (auto& cVolumesIter : confVolumes->arrayObjects())
        cVolumesIter->moveTV( transform );

  // confined unordered volumes
  const std::vector<const Trk::TrackingVolume* >* confDenseVolumes = confinedDenseVolumes();
  if (confDenseVolumes)
      // retrieve array objects and apply the transform 
      for (auto& cVolumesIter : (*confDenseVolumes) )
          cVolumesIter->moveTV( transform );
}


void Trk::TrackingVolume::synchronizeLayers ATLAS_NOT_THREAD_SAFE (MsgStream& msgstream, double envelope) const {

  // case a : Layers exist
  const Trk::BinnedArray< Trk::Layer >* confLayers = confinedLayers();
  if (confLayers){
    for (auto& clayIter : confLayers->arrayObjects())
        if (clayIter){
            if (clayIter->surfaceRepresentation().type() == Trk::Surface::Cylinder && !(center().isApprox(clayIter->surfaceRepresentation().center())) )
                clayIter->resizeAndRepositionLayer(volumeBounds(),center(),envelope);
            else 
                clayIter->resizeLayer(volumeBounds(),envelope);
        }  else
            msgstream << MSG::WARNING << "  ---> found 0 pointer to layer in Volume [ "<<volumeName()<<" ], indicates problem." << endmsg;
  }
  // case b : container volume -> step down
  const Trk::BinnedArray< Trk::TrackingVolume >* confVolumes = confinedVolumes();
  if (confVolumes){
    for (auto& cVolumesIter : confVolumes->arrayObjects())
        cVolumesIter->synchronizeLayers(msgstream, envelope);
  } 
}

void Trk::TrackingVolume::compactify ATLAS_NOT_THREAD_SAFE (size_t& cSurfaces, size_t& tSurfaces) const {
        
  // confined 'ordered' layers
  const Trk::BinnedArray< Trk::Layer >* confLayers = confinedLayers();
  if (confLayers){
    const std::vector<const Trk::Layer*>& layers = confLayers->arrayObjects();
    for (auto& clayIter : layers ) {
      if (&(*clayIter)!=nullptr) 
        clayIter->compactify(cSurfaces,tSurfaces);
      else 
        std::cout<<"WARNING: Attempt to compactify nullptr layer in volume : "<<volumeName()<<std::endl;
    }
  }
  // confined 'unordered' layers
  const std::vector<const Trk::Layer* >* confArbLayers = confinedArbitraryLayers();
  if (confArbLayers) {
      for (auto& calayIter : (*confArbLayers ) ) {
        if (&(*calayIter)!=nullptr) 
          calayIter->compactify(cSurfaces,tSurfaces);
        else 
          std::cout<<"WARNING: Attempt to compactify nullptr layer."<<std::endl;
      }
  }
  // confined volumes
  const Trk::BinnedArray< Trk::TrackingVolume >* confVolumes = confinedVolumes();
  if (confVolumes){
    const std::vector<const Trk::TrackingVolume*>& volumes = confVolumes->arrayObjects();
    for (auto& cVolumesIter : volumes) cVolumesIter->compactify(cSurfaces,tSurfaces);
  } 
  // confined unordered volumes
  const std::vector<const Trk::TrackingVolume* >* confDenseVolumes = confinedDenseVolumes();
  if (confDenseVolumes)
      for (auto& cVolumesIter : (*confDenseVolumes) ) cVolumesIter->compactify(cSurfaces,tSurfaces);
  // detached volumes if any 
  if (m_confinedDetachedVolumes)
      for (auto& cdVolumesIter : (*m_confinedDetachedVolumes) ) cdVolumesIter->compactify(cSurfaces,tSurfaces);
}

void  Trk::TrackingVolume::screenDump(MsgStream& msg) const
{
  msg << "[[ Trk::TrackingVolume ]] called: " << volumeName() << std::endl;
  msg << '\t' << '\t' << "# position (x,y,z) : " << center().x() << ", " << center().y() << ", " << center().z() << std::endl;
  msg << '\t' << '\t' << "# bounds           : " << volumeBounds()     << endmsg;
}

