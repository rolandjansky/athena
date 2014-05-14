/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/ConeLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkSurfaces/ConeBounds.h"
#include "TrkParameters/TrackParameters.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"


Trk::ConeLayer::ConeLayer(Amg::Transform3D* transform,
                          Trk::ConeBounds* cbounds,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  ConeSurface(transform, cbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::ConeLayer::ConeLayer(Trk::ConeSurface* cyl,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  ConeSurface(*cyl),
  Layer(laymatprop, thickness, olap, laytyp)
{}
        
Trk::ConeLayer::ConeLayer(Amg::Transform3D* transform,
			              Trk::ConeBounds* cbounds,
			              Trk::SurfaceArray* surfaceArray,
			              double thickness,
			              Trk::OverlapDescriptor* olap,
			              int laytyp) :
  ConeSurface(transform, cbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}
                
Trk::ConeLayer::ConeLayer(Amg::Transform3D* transform,
                          Trk::ConeBounds* cbounds,
                          Trk::SurfaceArray* surfaceArray,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  ConeSurface(transform, cbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::ConeLayer::ConeLayer(const Trk::ConeLayer& clay):
  ConeSurface(clay),
  Layer(clay)
{}

Trk::ConeLayer::ConeLayer(const Trk::ConeLayer& clay, const Amg::Transform3D& transf):
  ConeSurface(clay,transf),
  Layer(clay)
{}

Trk::ConeLayer& Trk::ConeLayer::operator=(const ConeLayer& clay)
{
  if (this!=&clay){
    // call the assignments of the base classes
    Trk::ConeSurface::operator=(clay);
    Trk::Layer::operator=(clay);
  }
  return(*this);
}

    
const Trk::ConeSurface& Trk::ConeLayer::surfaceRepresentation() const
{
  return (*this);
}

double Trk::ConeLayer::preUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                   Trk::PropDirection dir) const
{    
  if (!Trk::Layer::m_layerMaterialProperties)
    return 0.;
  // calculate the direction to the normal 
  const Amg::Vector3D& parmPos = parm.position();
  Amg::Vector3D pastStep(parmPos + dir*parm.momentum().normalized());
  if (pastStep.perp() > parm.position().perp())
    return Trk::Layer::m_layerMaterialProperties->alongPreFactor();
  return   Trk::Layer::m_layerMaterialProperties->oppositePreFactor();
}

double Trk::ConeLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                    Trk::PropDirection dir) const 
{
  if (!Trk::Layer::m_layerMaterialProperties)
    return 0;
  const Amg::Vector3D& parmPos = parm.position();
  Amg::Vector3D pastStep(parmPos + dir*parm.momentum().normalized());
  if (pastStep.perp() > parm.position().perp())
    return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
  return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}

double Trk::ConeLayer::pathCorrection(const Trk::TrackParameters& parm) const
{
    
  // pathCorrection is only supported for Cones with sym axis ==
  // z-axis... not sure what the complext correction should be...
  if ( fabs(rotSymmetryAxis().z()) > 0.99) //{  &&  !TRKGEOMETRY_COMPLEXPATHCORRECTION){
    // the fastest version
    return 1./sin( parm.parameters()[Trk::theta]+surfaceRepresentation().bounds().alpha() );
    /*
  } if ( fabs(rotSymmetryAxis().z()) > 0.99) {
    // the center of the Cone 
   
    const Amg::Vector3D& center = surfaceRepresentation().center();

    // create the global direction
    Amg::Vector3D dir(parm.momentum().normalized());
    double dirTfactor = 1./dir.perp(); 
  
    Amg::Vector3D transdir(dirTfactor*dir.x(), dirTfactor*dir.y(), 0.);

    // and the transverse correction - to be safe against small shifts
    Amg::Vector3D normal(parm.position().x() - center.x() ,parm.position().y() - center.y() ,0.);

    // the cos of the enclosed angle is the dot product - the longitudinal correction
    double lambda = acos(fabs(dir.dot( rotSymmetryAxis() )));
    double tau = acos(fabs(transdir.dot(normal.normalized())));

    // get the tangenti
    double tanlambda = tan(lambda);
    double tantau  = tan(tau);  
    return sqrt(1.+tantau*tantau + 1./(tanlambda*tanlambda));
  }
    */
    
  // create the global direction
  Amg::Vector3D dir(parm.momentum().normalized());
  // the cos of the enclosed angle is the dot product - the longitudinal correction
  double lambda = acos(fabs(dir.dot( rotSymmetryAxis() )));
  // return the correction 
  return (1./sin(lambda+surfaceRepresentation().bounds().alpha()));
   
}

void Trk::ConeLayer::moveLayer(Amg::Transform3D& shift) const {
  Amg::Transform3D transf = shift * (*m_transform);
  delete m_transform;
  m_transform = new Amg::Transform3D(transf);
  delete m_center;
  m_center = new Amg::Vector3D(m_transform->translation());
  delete m_normal;
  m_normal = new Amg::Vector3D(m_transform->rotation().col(2));
}
