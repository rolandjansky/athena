/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlaneLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/PlaneLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::RectangleBounds* rbounds,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, rbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::TrapezoidBounds* tbounds,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::DiamondBounds* tbounds,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::EllipseBounds* tbounds,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Trk::PlaneSurface* plane,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(*plane),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::SharedObject<const Trk::SurfaceBounds>& tbounds,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}
  
Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::RectangleBounds* rbounds,
                            Trk::SurfaceArray* surfaceArray,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, rbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::TrapezoidBounds* tbounds,
                            Trk::SurfaceArray* surfaceArray,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::DiamondBounds* tbounds,
                            Trk::SurfaceArray* surfaceArray,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}
              
Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::RectangleBounds* rbounds,
                            Trk::SurfaceArray* surfaceArray,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, rbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::TrapezoidBounds* tbounds,
                            Trk::SurfaceArray* surfaceArray,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(Amg::Transform3D* transform,
                            Trk::DiamondBounds* tbounds,
                            Trk::SurfaceArray* surfaceArray,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  PlaneSurface(transform, tbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::PlaneLayer::PlaneLayer(const Trk::PlaneLayer& play)
  
= default;

Trk::PlaneLayer::PlaneLayer(const Trk::PlaneLayer& play, const Amg::Transform3D& transf):
  PlaneSurface(play, transf),
  Layer(play)
{}

Trk::PlaneLayer& Trk::PlaneLayer::operator=(const PlaneLayer& play)
{
  if (this!=&play){
    // call the assignments of the base classes
    Trk::PlaneSurface::operator=(play);
    Trk::Layer::operator=(play);
    m_index = play.m_index;
  }
  return(*this);
}
        
const Trk::PlaneSurface& Trk::PlaneLayer::surfaceRepresentation() const
{
  return (*this);
}

double Trk::PlaneLayer::preUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                Trk::PropDirection dir) const
{
    if (!Trk::Layer::m_layerMaterialProperties.get())
      return 0.;
    if (Trk::PlaneSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPreFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePreFactor();  
}

double Trk::PlaneLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                 Trk::PropDirection dir) const 
{
    if (!Trk::Layer::m_layerMaterialProperties.get())
      return 0.;
    if (Trk::PlaneSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}

void Trk::PlaneLayer::moveLayer(Amg::Transform3D& shift)  {
       Amg::Transform3D transf = shift * (*m_transform);
       m_transform=std::make_unique<Amg::Transform3D>(transf);
       m_center.store(std::make_unique<Amg::Vector3D>(m_transform->translation()));
       m_normal.store(std::make_unique<Amg::Vector3D>(m_transform->rotation().col(2)));
}


