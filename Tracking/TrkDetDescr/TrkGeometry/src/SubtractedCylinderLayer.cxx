/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedCylinderLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/SubtractedCylinderLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"


Trk::SubtractedCylinderLayer::SubtractedCylinderLayer(const Trk::SubtractedCylinderSurface* subCyl,
						      const Trk::LayerMaterialProperties& laymatprop,
						      double thickness,
						      Trk::OverlapDescriptor* olap,
						      int laytyp) :
  SubtractedCylinderSurface(*subCyl),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::SubtractedCylinderLayer::SubtractedCylinderLayer(const Trk::SubtractedCylinderLayer& clay)
  
= default;

Trk::SubtractedCylinderLayer::SubtractedCylinderLayer(const Trk::SubtractedCylinderLayer& clay, const Amg::Transform3D& transf):
  SubtractedCylinderSurface(clay, transf),
  Layer(clay)
{}

Trk::SubtractedCylinderLayer& Trk::SubtractedCylinderLayer::operator=(const SubtractedCylinderLayer& clay)
{
  if (this!=&clay){
    // call the assignments of the base classes
    Trk::SubtractedCylinderSurface::operator=(clay);
    Trk::Layer::operator=(clay);
  }
  return(*this);
}

    
const Trk::SubtractedCylinderSurface& Trk::SubtractedCylinderLayer::surfaceRepresentation() const
{
  return (*this);
}

double Trk::SubtractedCylinderLayer::preUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                   Trk::PropDirection dir) const
{    
    if (!Trk::Layer::m_layerMaterialProperties.get())
      return 0.;
    // calculate the direction to the normal 
    const Amg::Vector3D& parmPos = parm.position();
    Amg::Vector3D pastStep(parmPos + dir*parm.momentum().normalized());
    if (pastStep.perp() > parm.position().perp())
      return Trk::Layer::m_layerMaterialProperties->alongPreFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePreFactor();
}

double Trk::SubtractedCylinderLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                    Trk::PropDirection dir) const 
{
   if (!Trk::Layer::m_layerMaterialProperties.get())
      return 0;
    const Amg::Vector3D& parmPos = parm.position();
    Amg::Vector3D pastStep(parmPos + dir*parm.momentum().normalized());
    if (pastStep.perp() > parm.position().perp())
      return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}

void Trk::SubtractedCylinderLayer::moveLayer(Amg::Transform3D& shift)  {
      Amg::Transform3D transf = shift * (*m_transform);
      m_transform=std::make_unique<Amg::Transform3D>(transf);
      m_center.store(std::make_unique<Amg::Vector3D>(m_transform->translation()));
      m_normal.store(std::make_unique<Amg::Vector3D>(m_transform->rotation().col(2)));
}
