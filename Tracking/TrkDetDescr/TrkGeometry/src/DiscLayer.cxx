/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  Layer(laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(nullptr)
{
    DiscSurface::associateLayer(*this);
}

Trk::DiscLayer::DiscLayer(Trk::DiscSurface* disc,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  DiscSurface(*disc),
  Layer(laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(nullptr)
{
    DiscSurface::associateLayer(*this);    
}

Trk::DiscLayer::DiscLayer(Amg::Transform3D* transform,
                          Trk::DiscBounds* dbounds,
                          Trk::SurfaceArray* surfaceArray,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          Trk::IApproachDescriptor* ades,
                          int laytyp) :
  DiscSurface(transform, dbounds),
  Layer(surfaceArray, thickness, olap, laytyp),
  m_approachDescriptor(ades)
{
    DiscSurface::associateLayer(*this);
    if (!ades && surfaceArray) buildApproachDescriptor();
    // register the layer
    if (ades) m_approachDescriptor->registerLayer(*this);
    
}
                
Trk::DiscLayer::DiscLayer(Amg::Transform3D* transform,
                          Trk::DiscBounds* dbounds,
                          Trk::SurfaceArray* surfaceArray,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          Trk::IApproachDescriptor* ades,
                          int laytyp) :
  DiscSurface(transform, dbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp),
  m_approachDescriptor(ades)
{
    DiscSurface::associateLayer(*this);
    if (!ades && surfaceArray) buildApproachDescriptor();
    // register the layer
    if (ades) m_approachDescriptor->registerLayer(*this);
}

Trk::DiscLayer::DiscLayer(const Trk::DiscLayer& dlay):
  DiscSurface(dlay),
  Layer(dlay),
  m_approachDescriptor(nullptr)
{
    DiscSurface::associateLayer(*this);
    if (m_surfaceArray) buildApproachDescriptor();
}

Trk::DiscLayer::DiscLayer(const Trk::DiscLayer& dlay, const Amg::Transform3D& transf):
  DiscSurface(dlay,transf),
  Layer(dlay),
  m_approachDescriptor(nullptr)
{
    DiscSurface::associateLayer(*this);
    if (m_surfaceArray) buildApproachDescriptor();    
}

Trk::DiscLayer& Trk::DiscLayer::operator=(const DiscLayer& dlay)
{
  if (this!=&dlay){
        // call the assignments of the base classes
        Trk::DiscSurface::operator=(dlay);
        Trk::Layer::operator=(dlay);
        m_approachDescriptor.reset();
        if (m_surfaceArray) {
          buildApproachDescriptor();
        }
        DiscSurface::associateLayer(*this);
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
    if (!Trk::Layer::m_layerMaterialProperties.get())
      return 0.;
    //const Amg::Vector3D& parmPos = parm.position(); 
    if (Trk::DiscSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPreFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePreFactor();  
}

double Trk::DiscLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                Trk::PropDirection dir) const 
{
   if (!Trk::Layer::m_layerMaterialProperties.get())
     return 0.;
   //const Amg::Vector3D& parmPos = parm.position(); 
   if (Trk::DiscSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
   return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}

void Trk::DiscLayer::moveLayer(Amg::Transform3D& shift)  {
       
       Amg::Transform3D transf = shift * (*m_transform);
       m_transform=std::make_unique<Amg::Transform3D>(transf);
       m_center.store(std::make_unique<Amg::Vector3D>(m_transform->translation()));
       m_normal.store(std::make_unique<Amg::Vector3D>(m_transform->rotation().col(2)));
       // rebuild that - deletes the current one
       if (m_approachDescriptor &&  m_approachDescriptor->rebuild()) {
         buildApproachDescriptor();
       }
}

void Trk::DiscLayer::resizeLayer(const VolumeBounds& bounds, double envelope)  {
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
        if (Trk::Layer::m_layerMaterialProperties.get() ){
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

    if (m_approachDescriptor &&  m_approachDescriptor->rebuild()){
        // rebuild the approach descriptor - delete the current approach descriptor
        buildApproachDescriptor();
    }
    
}

/** Surface seen on approach - if not defined differently, it is the surfaceRepresentation() */
const Trk::Surface& Trk::DiscLayer::approachSurface(const Amg::Vector3D& pos,
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
            //!< @TODO -> optimise by breaking the loop if possible
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
const Trk::Surface& Trk::DiscLayer::surfaceOnApproach(const Amg::Vector3D& pos,
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
void Trk::DiscLayer::buildApproachDescriptor(){

    // create the surface container   
    auto aSurfaces = std::make_unique<Trk::ApproachSurfaces>();
    // get the center
    const auto halfThickness=0.5*thickness()*normal();
    const Amg::Vector3D aspPosition(center()+halfThickness);
    const Amg::Vector3D asnPosition(center()-halfThickness);
    // create the new surfaces
    const Trk::DiscBounds* db = dynamic_cast<const Trk::DiscBounds*>(m_bounds.get());
    if (db){ 
        // create new surfaces
        Amg::Transform3D* asnTransform = new Amg::Transform3D(Amg::Translation3D(asnPosition)); 
        Amg::Transform3D* aspTransform = new Amg::Transform3D(Amg::Translation3D(aspPosition));   
        aSurfaces->push_back( new Trk::DiscSurface(aspTransform, db->clone()) );
        aSurfaces->push_back( new Trk::DiscSurface(asnTransform, db->clone()) );
        // set the layer and make TGOwn
        for (auto& sIter : (*aSurfaces)){
            sIter->associateLayer(*this);
            sIter->setOwner(Trk::TGOwn);
        }
    }
    //The approach descriptor takes owneship
    m_approachDescriptor = std::make_unique<Trk::ApproachDescriptor>(std::move(aSurfaces));
}

void Trk::DiscLayer::resizeAndRepositionLayer(const VolumeBounds& vBounds, const Amg::Vector3D& vCenter, double envelope)  {
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
        Trk::DiscSurface::m_transform=std::make_unique<Amg::Transform3D> (Amg::Translation3D(nDiscCenter));
        // delete derived and the cache
        Trk::DiscSurface::m_center.store(std::make_unique<Amg::Vector3D>(nDiscCenter));
        Trk::DiscSurface::m_normal.store(nullptr);
    }
    // rebuild the approaching layer 
    if (m_approachDescriptor &&  m_approachDescriptor->rebuild()) 
        buildApproachDescriptor();    
}
