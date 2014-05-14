/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetDescrUtils/BinUtility.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

Trk::CylinderLayer::CylinderLayer(Amg::Transform3D* transform,
                                  Trk::CylinderBounds* cbounds,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(transform, cbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::CylinderLayer::CylinderLayer(Trk::CylinderSurface* cyl,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(*cyl),
  Layer(laymatprop, thickness, olap, laytyp)
{}
        
Trk::CylinderLayer::CylinderLayer(Amg::Transform3D* transform,
                                  Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(transform, cbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}
                
Trk::CylinderLayer::CylinderLayer(Amg::Transform3D* transform,
                                  Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(transform, cbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::CylinderLayer::CylinderLayer(Trk::CylinderBounds* cbounds,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(cbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}
        
Trk::CylinderLayer::CylinderLayer(Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(cbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}
                
Trk::CylinderLayer::CylinderLayer(Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(cbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::CylinderLayer::CylinderLayer(const Trk::CylinderLayer& clay):
  CylinderSurface(clay),
  Layer(clay)
{}

Trk::CylinderLayer::CylinderLayer(const Trk::CylinderLayer& clay, const Amg::Transform3D& transf):
  CylinderSurface(clay,transf),
  Layer(clay)
{}

Trk::CylinderLayer& Trk::CylinderLayer::operator=(const CylinderLayer& clay)
{
  if (this!=&clay){
   // call the assignments of the base classes
   Trk::CylinderSurface::operator=(clay);
   Trk::Layer::operator=(clay);
   }
  return(*this);
}

    
const Trk::CylinderSurface& Trk::CylinderLayer::surfaceRepresentation() const
{
  return (*this);
}

double Trk::CylinderLayer::preUpdateMaterialFactor(const Trk::TrackParameters& parm,
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

double Trk::CylinderLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
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

double Trk::CylinderLayer::pathCorrection(const Trk::TrackParameters& parm) const
{
    
  // pathCorrection is only supported for Cylinders with sym axis == z-axis
  if ( fabs(rotSymmetryAxis().z()) > 0.99 &&  !TRKGEOMETRY_COMPLEXPATHCORRECTION){
     // the fastest version
     return 1./sin(parm.parameters()[Trk::theta]);

  } if ( fabs(rotSymmetryAxis().z()) > 0.99) {
   // the center of the Cylinder 
   
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

   // create the global direction
   Amg::Vector3D dir(parm.momentum().normalized());
   // the cos of the enclosed angle is the dot product - the longitudinal correction
   double lambda = acos(fabs(dir.dot( rotSymmetryAxis() )));
   // return the correction 
   return (1./sin(lambda));
   
}

void Trk::CylinderLayer::moveLayer(Amg::Transform3D& shift) const {
       Amg::Transform3D transf = shift * (*m_transform);
       delete m_transform;
       m_transform = new Amg::Transform3D(transf);
       delete m_center;
       m_center = new Amg::Vector3D(m_transform->translation());
       delete m_normal;
       m_normal = new Amg::Vector3D(m_transform->rotation().col(2));
}

void Trk::CylinderLayer::resizeLayer(const VolumeBounds& bounds, double envelope) const {
    // only do this if the volume bounds a CylinderVolumeBounds
    const Trk::CylinderVolumeBounds* cvb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&bounds);
    if (cvb){
        // get the dimensions
        double hLengthZ = cvb->halflengthZ();
        double r        = surfaceRepresentation().bounds().r();
        // (0) first, resize the layer itself
        Trk::CylinderBounds* rCylinderBounds = new Trk::CylinderBounds(r,hLengthZ-envelope);
        Trk::CylinderSurface::m_bounds = Trk::SharedObject<const Trk::CylinderBounds>(rCylinderBounds);
        // (1) resize the material properties by updating the BinUtility, assuming rphi/z binning
        if (Trk::Layer::m_layerMaterialProperties ){
            const BinUtility* layerMaterialBU = Trk::Layer::m_layerMaterialProperties->binUtility();
            if (layerMaterialBU && layerMaterialBU->dimensions() > 1 ){
                size_t binsRPhi = layerMaterialBU->max(0)+1;
                size_t binsZ    = layerMaterialBU->max(1)+1;
                // create a new binning with the new dimensions
                Trk::BinUtility* rBinUtility = 
                    new Trk::BinUtility(binsRPhi,-r*M_PI,r*M_PI, Trk::closed, Trk::binRPhi);
                    (*rBinUtility) += Trk::BinUtility(binsZ, -hLengthZ+envelope, hLengthZ-envelope, Trk::open, Trk::binZ);
                Trk::Layer::m_layerMaterialProperties->updateBinning(rBinUtility);
            }
        }
    }
}

void Trk::CylinderLayer::resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& vCenter, double envelope) const {
    // resize first of all
    resizeLayer(vBounds,envelope);
    // now reposition to the potentially center if necessary, do not change layers with no transform
    if ( Trk::CylinderSurface::m_transform || center().isApprox(vCenter) ) return;
    // else set to the new volume center
    delete Trk::CylinderSurface::m_transform;
    Trk::CylinderSurface::m_transform = new Amg::Transform3D;
    (*Trk::CylinderSurface::m_transform) = Amg::Translation3D(vCenter);
    // delete derived and the cache
    delete Trk::CylinderSurface::m_center; Trk::CylinderSurface::m_center = new Amg::Vector3D(vCenter);
    delete Trk::CylinderSurface::m_normal; Trk::CylinderSurface::m_normal = 0;
    

}

