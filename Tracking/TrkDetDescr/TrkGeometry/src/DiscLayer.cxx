/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetDescrUtils/BinUtility.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"

Trk::DiscLayer::DiscLayer(Amg::Transform3D* transform,
                          Trk::DiscBounds* dbounds,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  DiscSurface(transform, dbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::DiscLayer::DiscLayer(Trk::DiscSurface* disc,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  DiscSurface(*disc),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::DiscLayer::DiscLayer(Amg::Transform3D* transform,
                          Trk::DiscBounds* dbounds,
                          Trk::SurfaceArray* surfaceArray,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  DiscSurface(transform, dbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}
                
Trk::DiscLayer::DiscLayer(Amg::Transform3D* transform,
                          Trk::DiscBounds* dbounds,
                          Trk::SurfaceArray* surfaceArray,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  DiscSurface(transform, dbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::DiscLayer::DiscLayer(const Trk::DiscLayer& dlay):
  DiscSurface(dlay),
  Layer(dlay)
{}

Trk::DiscLayer::DiscLayer(const Trk::DiscLayer& dlay, const Amg::Transform3D& transf):
  DiscSurface(dlay,transf),
  Layer(dlay)
{}

Trk::DiscLayer& Trk::DiscLayer::operator=(const DiscLayer& dlay)
{
  if (this!=&dlay){
        // call the assignments of the base classes
        Trk::DiscSurface::operator=(dlay);
        Trk::Layer::operator=(dlay);
   }
  return(*this);
}
    
const Trk::DiscSurface& Trk::DiscLayer::surfaceRepresentation() const
{
  return (*this);
}

double Trk::DiscLayer::preUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                               Trk::PropDirection dir) const
{
    if (!Trk::Layer::m_layerMaterialProperties)
      return 0.;
    //const Amg::Vector3D& parmPos = parm.position(); 
    if (Trk::DiscSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPreFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePreFactor();  
}

double Trk::DiscLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                    Trk::PropDirection dir) const 
{
   if (!Trk::Layer::m_layerMaterialProperties)
     return 0.;
   //const Amg::Vector3D& parmPos = parm.position(); 
   if (Trk::DiscSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
   return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}

double Trk::DiscLayer::pathCorrection(const Trk::TrackParameters& parm) const {
        // normal and direction of track
        const Amg::Vector3D& normal   = surfaceRepresentation().normal();
        const Amg::Vector3D trackdir = parm.momentum().normalized();
        // projection
        double invCorr = fabs(normal.dot(trackdir));
        return (1./invCorr);
}

void Trk::DiscLayer::moveLayer(Amg::Transform3D& shift) const {
       Amg::Transform3D transf = shift * (*m_transform);
       delete m_transform;
       m_transform = new Amg::Transform3D(transf);
       delete m_center;
       m_center = new Amg::Vector3D(m_transform->translation());
       delete m_normal; 
       m_normal = new Amg::Vector3D(m_transform->rotation().col(2));
}

void Trk::DiscLayer::resizeLayer(const VolumeBounds& bounds, double envelope) const {
    // only do this if the volume bounds a CylinderVolumeBounds
    const Trk::CylinderVolumeBounds* cvb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&bounds);
    if (cvb){
        // get the dimensions
        double rInner = cvb->innerRadius();
        double rOuter = cvb->outerRadius();
        // (0) first, resize the layer itself
        Trk::DiscBounds* rDiscBounds = new Trk::DiscBounds(rInner+envelope,rOuter-envelope);
        Trk::DiscSurface::m_bounds = Trk::SharedObject<const Trk::SurfaceBounds>(rDiscBounds);
        // (1) resize the material properties by updating the BinUtility, assuming r/phi binning
        if (Trk::Layer::m_layerMaterialProperties ){
            const BinUtility* layerMaterialBU = Trk::Layer::m_layerMaterialProperties->binUtility();
            if (layerMaterialBU && layerMaterialBU->binningValue(0) == Trk::binR ){
                size_t binsR = layerMaterialBU->max(0)+1;
                // create a new binning with the new dimensions
                Trk::BinUtility* rBinUtility = 
                    new Trk::BinUtility(binsR,rInner+envelope,rOuter-envelope, Trk::open, Trk::binR);
                // copy the second dimension over if exist
                if (layerMaterialBU->dimensions()>1){
                    (*rBinUtility) += Trk::BinUtility(layerMaterialBU->max(1)+1, -M_PI, M_PI, Trk::closed, Trk::binPhi);
                }
                Trk::Layer::m_layerMaterialProperties->updateBinning(rBinUtility);
            }
        }
    }
}

void Trk::DiscLayer::resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& vCenter, double envelope) const {
    // resize first of all
    resizeLayer(vBounds,envelope);
    // now reposition to the potentially center if necessary, do not change layers with no transform
    const Trk::CylinderVolumeBounds* cvb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&vBounds);
    if (cvb){
        // get the halflength
        double hLengthZ = cvb->halflengthZ();
        Amg::Vector3D nDiscCenter = center().z() < 0. ?  Amg::Vector3D( vCenter - Amg::Vector3D(0.,0.,hLengthZ-0.5*thickness()) ) :
                                                         Amg::Vector3D( vCenter + Amg::Vector3D(0.,0.,hLengthZ-0.5*thickness()) );
        if (center().isApprox(nDiscCenter)) return;
        // else set to the new volume center
        delete Trk::DiscSurface::m_transform;
        Trk::DiscSurface::m_transform = new Amg::Transform3D;
        (*Trk::DiscSurface::m_transform) = Amg::Translation3D(nDiscCenter);
        // delete derived and the cache
        delete Trk::DiscSurface::m_center; Trk::DiscSurface::m_center = new Amg::Vector3D(nDiscCenter);
        delete Trk::DiscSurface::m_normal; Trk::DiscSurface::m_normal = 0;
    }    
}
