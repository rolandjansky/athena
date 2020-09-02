/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/ApproachDescriptor.h"
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
  Layer(laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(nullptr)
{
  CylinderSurface::associateLayer(*this);
}

Trk::CylinderLayer::CylinderLayer(Trk::CylinderSurface* cyl,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(*cyl),
  Layer(laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(nullptr)
{
    CylinderSurface::associateLayer(*this);
}
        
Trk::CylinderLayer::CylinderLayer(Amg::Transform3D* transform,
                                  Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  Trk::IApproachDescriptor* ades,
                                  int laytyp) :
  CylinderSurface(transform, cbounds),
  Layer(surfaceArray, thickness, olap, laytyp),
  m_approachDescriptor(ades)
{
    CylinderSurface::associateLayer(*this);
    if (!ades && surfaceArray) buildApproachDescriptor();
    // register the layer
    if (ades) m_approachDescriptor->registerLayer(*this);
    
}
                
Trk::CylinderLayer::CylinderLayer(Amg::Transform3D* transform,
                                  Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  Trk::IApproachDescriptor* ades,
                                  int laytyp) :
  CylinderSurface(transform, cbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(ades)
{
    CylinderSurface::associateLayer(*this);
    if (!ades && surfaceArray) buildApproachDescriptor();
    // register the layer
    if (ades) m_approachDescriptor->registerLayer(*this);
    
}

Trk::CylinderLayer::CylinderLayer(Trk::CylinderBounds* cbounds,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  int laytyp) :
  CylinderSurface(cbounds),
  Layer(laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(nullptr)
{
    CylinderSurface::associateLayer(*this);
}
        
Trk::CylinderLayer::CylinderLayer(Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  Trk::IApproachDescriptor* ades,
                                  int laytyp) :
  CylinderSurface(cbounds),
  Layer(surfaceArray, thickness, olap, laytyp),
  m_approachDescriptor(ades)
{
    CylinderSurface::associateLayer(*this);
    if (!ades && surfaceArray) buildApproachDescriptor();
    // register the layer
    if (ades) m_approachDescriptor->registerLayer(*this);  
}
                
Trk::CylinderLayer::CylinderLayer(Trk::CylinderBounds* cbounds,
                                  Trk::SurfaceArray* surfaceArray,
                                  const Trk::LayerMaterialProperties& laymatprop,
                                  double thickness,
                                  Trk::OverlapDescriptor* olap,
                                  Trk::IApproachDescriptor* ades,
                                  int laytyp) :
  CylinderSurface(cbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(ades)
{
    CylinderSurface::associateLayer(*this);
    if (!ades && surfaceArray) buildApproachDescriptor();
    // register the layer
    if (ades) m_approachDescriptor->registerLayer(*this);    
}

Trk::CylinderLayer::CylinderLayer(const Trk::CylinderLayer& clay):
  CylinderSurface(clay),
  Layer(clay),
  m_approachDescriptor(nullptr)
{
    CylinderSurface::associateLayer(*this);
    m_approachDescriptor.reset();
    if (m_surfaceArray) buildApproachDescriptor(); //!< TODO use clone when exists
}

Trk::CylinderLayer::CylinderLayer(const Trk::CylinderLayer& clay, const Amg::Transform3D& transf):
  CylinderSurface(clay,transf),
  Layer(clay),
  m_approachDescriptor(nullptr)
{
    if (m_surfaceArray) buildApproachDescriptor();
}

Trk::CylinderLayer& Trk::CylinderLayer::operator=(const CylinderLayer& clay)
{
  if (this!=&clay){
    // call the assignments of the base classes
    Trk::CylinderSurface::operator=(clay);
    Trk::Layer::operator=(clay);
    if (m_surfaceArray) buildApproachDescriptor(); 
    CylinderSurface::associateLayer(*this); 
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
    if (!Trk::Layer::m_layerMaterialProperties.get())
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
   if (!Trk::Layer::m_layerMaterialProperties.get())
      return 0;
    const Amg::Vector3D& parmPos = parm.position();
    Amg::Vector3D pastStep(parmPos + dir*parm.momentum().normalized());
    if (pastStep.perp() > parm.position().perp())
      return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}


void Trk::CylinderLayer::moveLayer(Amg::Transform3D& shift) {
       Amg::Transform3D transf = shift * (*m_transform);
       Trk::CylinderSurface::m_transform= std::make_unique<Amg::Transform3D>(transf);
       m_center.store(std::make_unique<Amg::Vector3D>(m_transform->translation()));
       m_normal.store(std::make_unique<Amg::Vector3D>(m_transform->rotation().col(2)));

       if (m_approachDescriptor &&  m_approachDescriptor->rebuild()){
           // build the new approach descriptor - deletes the current one
           buildApproachDescriptor();
       }
}

void Trk::CylinderLayer::resizeLayer(const VolumeBounds& bounds, double envelope) {
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
        if (Trk::Layer::m_layerMaterialProperties.get() ){
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
    
    if (m_approachDescriptor &&  m_approachDescriptor->rebuild()){
        // build the approach descriptor - delete the current approach descriptor
        buildApproachDescriptor();
    }

    
}

/** Surface seen on approach - if not defined differently, it is the surfaceRepresentation() */
const Trk::Surface& Trk::CylinderLayer::approachSurface(const Amg::Vector3D& pos,
                                                        const Amg::Vector3D& dir,
                                                        const Trk::BoundaryCheck& bcheck) const
{
    if (m_approachDescriptor){
        // get the test surfaces from the approach Descriptor
        const Trk::ApproachSurfaces* surfacesOnApproach = m_approachDescriptor->approachSurfaces(pos,dir);
        if (surfacesOnApproach){
            // test the intersections and go 
            std::vector<Trk::Intersection> sfIntersections;
            const Trk::Surface* aSurface = nullptr;
            double aPathLength           = 10e10;
            // get the surfaces
            for (auto& sfIter : (*surfacesOnApproach)){
                // get the intersection with the surface
                Trk::Intersection sIntersection = sfIter->straightLineIntersection(pos, dir, true, bcheck); 
                // validation
                if (sIntersection.valid && sIntersection.pathLength < aPathLength){
                    aPathLength = sIntersection.pathLength;
                    aSurface    = sfIter;
                } 
            } 
            if (aSurface) return (*aSurface);
        } 
    }
    return surfaceRepresentation();
}

/** Surface seen on approach - if not defined differently, it is the surfaceRepresentation() */
const Trk::Surface& Trk::CylinderLayer::surfaceOnApproach(const Amg::Vector3D& pos,
                                                          const Amg::Vector3D& mom,
                                                          Trk::PropDirection pDir,
                                                          const Trk::BoundaryCheck& bcheck,
                                                          bool resolveSubSurfaces,
                                                          const Trk::ICompatibilityEstimator*) const
{ 
    // resolve the surfaces
    if (m_approachDescriptor && resolveSubSurfaces){
        // resolve based on straight line intersection
        return approachSurface(pos,double(pDir)*mom.unit(),bcheck);
    }
    return surfaceRepresentation();
}



/** build approach surfaces */
void Trk::CylinderLayer::buildApproachDescriptor(){
    // delete it
    // delete the surfaces    
    auto  aSurfaces = std::make_unique<Trk::ApproachSurfaces>();
    // create new surfaces
    Amg::Transform3D* asTransform = m_transform ? new Amg::Transform3D(*m_transform) : nullptr;
    // create the new surfaces
    aSurfaces->push_back(new Trk::CylinderSurface(asTransform, m_bounds->r()-0.5*thickness(), m_bounds->halflengthZ() ));
    aSurfaces->push_back(new Trk::CylinderSurface(asTransform, m_bounds->r()+0.5*thickness(), m_bounds->halflengthZ() ));
    // set the layer and make TGOwn
    for (auto& sIter : (*aSurfaces)){
        sIter->associateLayer(*this);
        sIter->setOwner(Trk::TGOwn);
    }
    m_approachDescriptor = std::make_unique<Trk::ApproachDescriptor>(std::move(aSurfaces));
    
}

void Trk::CylinderLayer::resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& vCenter, double envelope) {
    // resize first of all
    resizeLayer(vBounds,envelope);
    // now reposition to the potentially center if necessary, do not change layers with no transform
    if ( Trk::CylinderSurface::m_transform || center().isApprox(vCenter) ) return;
    
    Trk::CylinderSurface::m_transform=std::make_unique<Amg::Transform3D>(vCenter);
    // delete derived and the cache
    Trk::CylinderSurface::m_center.store(std::make_unique<Amg::Vector3D>(vCenter));
    Trk::CylinderSurface::m_normal.store(nullptr);
    // rebuild approaching layers if needed
    if (m_approachDescriptor &&  m_approachDescriptor->rebuild()) 
        buildApproachDescriptor();
}

