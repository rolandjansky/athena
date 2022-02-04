/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTools/TrackingVolumeHelper.h"
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/MaterialLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include <memory>
#include <stdexcept>


// constructor
Trk::TrackingVolumeHelper::TrackingVolumeHelper(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  TrackingVolumeManipulator(),
  m_layerArrayCreator("Trk::LayerArrayCreator/LayerArrayCreator"),
  m_trackingVolumeArrayCreator("Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"),
  m_barrelLayerBinsZ(1),
  m_barrelLayerBinsPhi(1),
  m_endcapLayerBinsR(1),
  m_endcapLayerBinsPhi(1)
{
    declareInterface<ITrackingVolumeHelper>(this);
    // the tools
    declareProperty("LayerArrayCreator",                m_layerArrayCreator);
    declareProperty("TrackingVolumeArrayCreator",       m_trackingVolumeArrayCreator);
    // Material binning
    declareProperty("BarrelLayerBinsZ"                 , m_barrelLayerBinsZ);
    declareProperty("BarrelLayerBinsPhi"               , m_barrelLayerBinsPhi);
    declareProperty("EndcapLayerBinsR"                 , m_endcapLayerBinsR);
    declareProperty("EndcapLayerBinsPhi"               , m_endcapLayerBinsPhi);
}

// destructor
Trk::TrackingVolumeHelper::~TrackingVolumeHelper()
{}


// the interface methods
StatusCode Trk::TrackingVolumeHelper::initialize()
{

    ATH_MSG_DEBUG( "initialize() " );    

    // Retrieve the layer array creator  ----------------------------------------------------
    if (m_layerArrayCreator.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerArrayCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_DEBUG( "Retrieved tool " << m_layerArrayCreator );

    // Retrieve the volume array creator  ----------------------------------------------------
    if (m_trackingVolumeArrayCreator.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeArrayCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_DEBUG( "Retrieved tool " << m_trackingVolumeArrayCreator );

    return StatusCode::SUCCESS;
}    


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::glueTrackingVolumes(const Trk::TrackingVolume& firstVol,
                                                    Trk::BoundarySurfaceFace firstFace,
                                                    const Trk::TrackingVolume& secondVol,
                                                    Trk::BoundarySurfaceFace secondFace,
                                                    bool buildBoundaryLayer) const
{
    Trk::TrackingVolumeManipulator::glueVolumes( firstVol, firstFace, secondVol, secondFace );
    
    // ----------------------------------------------------------------------------------------
    // create a MaterialLayer as a boundary
    if (buildBoundaryLayer){
        const auto& bSurfacesFirst  =  firstVol.boundarySurfaces();
        const auto& bSurfacesSecond =  secondVol.boundarySurfaces();
        // get the boundary surfaces
        const Trk::Surface& firstFaceSurface  = bSurfacesFirst[firstFace]->surfaceRepresentation(); 
        const Trk::Surface& secondFaceSurface  = bSurfacesSecond[secondFace]->surfaceRepresentation(); 
        // dynamic_cast to the right type
        std::unique_ptr<Trk::LayerMaterialProperties> lmps( layerMaterialProperties(firstFaceSurface) );
        // LayerMaterialProperties will be cloned in MaterialLayer

        // set the layer to the two surfaces
        if (lmps){
            Trk::Layer* mLayer = new Trk::MaterialLayer(firstFaceSurface, *lmps);
            ATH_MSG_VERBOSE( "Set MaterialLayer to the BoundarySurface of first volume." );
            (const_cast<Trk::Surface&>(firstFaceSurface)).setMaterialLayer(*mLayer);
            ATH_MSG_VERBOSE("Set MaterialLayer to the BoundarySurface of second volume.");
            (const_cast<Trk::Surface&>(secondFaceSurface)).setMaterialLayer(*mLayer);
        }
    }
}

/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::glueTrackingVolumes(const Trk::TrackingVolume& firstVol,
                                                    Trk::BoundarySurfaceFace firstFace,
                                                    const std::vector<const Trk::TrackingVolume*>& secondVolumes,
                                                    Trk::BoundarySurfaceFace secondFace,
                                                    bool buildBoundaryLayer,
                                                    bool boundaryFaceExchange) const
{

    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE( "Glue Volume '" << firstVol.volumeName() << "' to " << secondVolumes.size() << " volume(s): " );
        for (const auto & volIter : secondVolumes)
            ATH_MSG_VERBOSE( "              -> " << (volIter)->volumeName() );
    }
    // prepare the material layer if needed
    Trk::Layer* mLayer = nullptr;
    // ----------------------------------------------------------------------------------------
    // create a MaterialLayer as a boundary
    if (buildBoundaryLayer){
        // the first face surface 
        const Trk::Surface& firstFaceSurface = firstVol.boundarySurfaces()[firstFace]->surfaceRepresentation();
        std::unique_ptr<Trk::LayerMaterialProperties> lmps( layerMaterialProperties(firstFaceSurface) );
        // LayerMaterialProperties are cloned by MaterialLayer

        // the material layer is ready - it can be assigned
        mLayer = new Trk::MaterialLayer(firstFaceSurface, *lmps);
        ATH_MSG_VERBOSE( "Set MaterialLayer to the BoundarySurface of first volume (may be shared with second volume)." );
        (const_cast<Trk::Surface&>(firstFaceSurface)).setMaterialLayer(*mLayer);
    }  
    // if only one volume was given in the vector call the standard one-to-one glueing
    // 1-to-1 case
    if (secondVolumes.size() == 1) {
        // self call for one-on-one
        glueTrackingVolumes(firstVol, firstFace, *(secondVolumes[0]), secondFace);
    } else {
        // create the navigation bin array
        Trk::BinnedArray<const Trk::TrackingVolume>* navArray = nullptr;
        // create the Array - either r-binned or z-binned
        if (firstFace == Trk::negativeFaceXY || firstFace == Trk::positiveFaceXY )
            navArray = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(secondVolumes, true);
        else
            navArray = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(secondVolumes, true);
        
        // set the volume array to the first boundary surface - this must always happen
        if (firstFace != Trk::tubeInnerCover)
            setOutsideTrackingVolumeArray( firstVol, firstFace, navArray );
        else
            setInsideTrackingVolumeArray( firstVol, firstFace, navArray );
        // the navigation arrays are completed now - check if the boundary face should be exchanged
        // [1] the boundary face exchange ----------------------------------------------------------------------------------------
        if (boundaryFaceExchange){
            // creating only one boundary surface
            ATH_MSG_VERBOSE("Creating a joint boundary surface for 1-to-n glueing case.");
            // get the dimension of boundary surface of the first volume
            const Trk::SharedObject< const BoundarySurface<TrackingVolume> > bSurface = firstVol.boundarySurfaces()[firstFace];
            // replace the boundary surface
            for ( const auto & volIter: secondVolumes )             
                setBoundarySurface(*volIter, bSurface, secondFace);
        } else {
         // [2] the traditional way, keeping two boundary surfaces    
         // now set the face to the volume array -------------------------------------------------------------------------------
            for ( const auto & volIter: secondVolumes ) {
                // the secondGlueFace
                Trk::BoundarySurfaceFace secondGlueFace = secondFace;
                if (secondFace == Trk::tubeOuterCover) {
                    //check for cylinder case
                    const Trk::CylinderVolumeBounds* currentVolBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(&((volIter)->volumeBounds()));
                    // protection : there may be a cylinder within the tube vector
                    if (currentVolBounds && currentVolBounds->innerRadius() < 10e-3)
                        secondGlueFace = Trk::cylinderCover;
                    setOutsideTrackingVolume(*volIter, secondGlueFace, (&(firstVol)));
                } // for all surfaces except the tunbeInnerCover outside of the surface is identical to outside of the volume
                else if (secondGlueFace != Trk::tubeInnerCover)
                    setOutsideTrackingVolume(*volIter, secondGlueFace, (&(firstVol)));
                else
                    setInsideTrackingVolume(*volIter, secondGlueFace, (&(firstVol)));
                // if existing, set the material Layer
                // get the second face surface and set the new MaterialLayer
                const Trk::Surface& secondFaceSurface = volIter->boundarySurfaces()[secondFace]->surfaceRepresentation();
                (const_cast<Trk::Surface&>(secondFaceSurface)).setMaterialLayer(mLayer);
            }
        }
    } // 1-to-n case    
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::glueTrackingVolumes(const std::vector<const Trk::TrackingVolume*>& firstVolumes,
                                                    Trk::BoundarySurfaceFace firstFace,
                                                    const std::vector<const Trk::TrackingVolume*>& secondVolumes,
                                                    Trk::BoundarySurfaceFace secondFace,
                                                    bool buildBoundaryLayer,
                                                    bool boundaryFaceExchange) const
{
    
    
    Trk::BinnedArray<const Trk::TrackingVolume>* navArrayOne = nullptr;
    Trk::BinnedArray<const Trk::TrackingVolume>* navArrayTwo = nullptr;

    std::unique_ptr<const Trk::Surface>     mLayerSurface;
    std::unique_ptr<Trk::Layer>       mLayer;

    ATH_MSG_VERBOSE("Glue configuration firstFace | secondFace = " << firstFace << " | " << secondFace );

    // create the Arrays - assuming cylindrical TrackingVolumes
    if (firstFace < 2 && secondFace < 2 ) {
        ATH_MSG_VERBOSE( "The glueing is done along z axis" );
        navArrayOne = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(firstVolumes, true);
        navArrayTwo = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(secondVolumes, true);
        // build a disc to separate the two
        if (buildBoundaryLayer || boundaryFaceExchange){
            double rmin = 10e10; double rmax = 0; double boundaryz = 0.; double centerzOne = 0.;
            for (const auto & volIter : firstVolumes ){
                const Trk::CylinderVolumeBounds* cb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(volIter->volumeBounds()));
                if (cb) {
                    takeSmaller(rmin,cb->innerRadius());
                    takeBigger(rmax,cb->outerRadius());
                    // get the z of the surface
                    boundaryz = volIter->boundarySurfaces()[firstFace]->surfaceRepresentation().center().z(); 
                }
                centerzOne = volIter->center().z();
            }
            if (buildBoundaryLayer){
              Amg::Transform3D mLayerTransform =
                Amg::Transform3D(Amg::Translation3D(0., 0., boundaryz));
              // layer surface
              mLayerSurface =
                std::make_unique<Trk::DiscSurface>(mLayerTransform, rmin, rmax);
              // create a MaterialLayer
              std::unique_ptr<const Trk::LayerMaterialProperties> lmps(
                layerMaterialProperties(*mLayerSurface));
              // MaterialLayer clones the LayerMaterialPropteries.

              if (lmps) {
                mLayer = std::make_unique<Trk::MaterialLayer>(
                  std::shared_ptr<const Trk::Surface>(std::move(mLayerSurface)),
                  *lmps);
                }
            }
            if (boundaryFaceExchange){
                // creating only one boundary surface
                ATH_MSG_VERBOSE("Creating a joint boundary surface for n-to-n glueing case.");
                // check if the seconf volumes have a bigger z value or a smaller one
                double centerzTwo = secondVolumes[secondVolumes.size()-1]->center().z();
                // thi sboundary surface is having a z-axix along the global z-axis
                Amg::Transform3D boundaryTransform =
                  Amg::Transform3D(Amg::Translation3D(0., 0., boundaryz));
                // disc surfaces
                Trk::DiscSurface dSurface(boundaryTransform, rmin, rmax);
                // swap if needed 
                if (centerzTwo < centerzOne){
                    Trk::BinnedArray<const Trk::TrackingVolume>* navArraySwap = navArrayOne;
                    navArrayTwo = navArrayOne;
                    navArrayOne = navArraySwap;
                }
                // create the new boudnary surface which spans over the entire volume border
                Trk::SharedObject< Trk::BinnedArray<const Trk::TrackingVolume> >  navArrayInside(navArrayOne);
                Trk::SharedObject< Trk::BinnedArray<const Trk::TrackingVolume> >  navArrayOutside(navArrayTwo);
                Trk::BoundaryDiscSurface<Trk::TrackingVolume>* boundarySurface = new Trk::BoundaryDiscSurface<Trk::TrackingVolume>(navArrayInside,navArrayOutside,dSurface);
                Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > sharedBoundarySurface(boundarySurface);
                // attach the material layer to the shared boundary if existing
                if (mLayer) {
                    ATH_MSG_VERBOSE( "Set MaterialLayer to the BoundarySurface of volume from second array." );
                    (const_cast<Trk::Surface&>(boundarySurface->surfaceRepresentation()))
                      .setMaterialLayer(*(mLayer.release()));
                }
                // set the boundary surface to the volumes of both sides
                for (const auto & volIter : firstVolumes){
                    ATH_MSG_VERBOSE("  -> first array : setting a newly created boundary surface to  " << volIter->volumeName());
                    setBoundarySurface(*volIter,sharedBoundarySurface,firstFace);
                }
                for (const auto & volIter : secondVolumes){
                    ATH_MSG_VERBOSE("  -> second array : setting a newly created boundary surface to  " << volIter->volumeName());
                    setBoundarySurface(*volIter,sharedBoundarySurface,secondFace);
                }
                // we are done here
                return;
            }
        }
    } else {
        ATH_MSG_VERBOSE( "The glueing is done along the radius." );
        navArrayOne = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(firstVolumes, true);
        navArrayTwo = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(secondVolumes, true);
        // check if the boundary layer was configured to be built
        if (buildBoundaryLayer || boundaryFaceExchange){
            // build a cylinder to separate the two
            double zmin = 10e10; double zmax = -10e10; double boundaryr = 0.; double volumerOne = 0.; double volumerTwo = 10e10;
            for (const auto & volIter : firstVolumes ){
                const Trk::CylinderVolumeBounds* cb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(volIter->volumeBounds()));
                if (cb) {
                    takeSmaller(zmin,volIter->center().z()-cb->halflengthZ());
                    takeBigger(zmax,volIter->center().z()+cb->halflengthZ());
                    // get the z of the surface
                    boundaryr = volIter->boundarySurfaces()[firstFace]->surfaceRepresentation().bounds().r(); 
                    // get the volume radius
                    volumerOne = cb->outerRadius();
                }
            }
            // check if boundary layer should be built
            if (buildBoundaryLayer){
              std::unique_ptr<Amg::Transform3D> mLayerTransform =
                ((zmin + zmax) * (zmin + zmax) < 10e-4)
                  ? nullptr
                  : std::make_unique < Amg::Transform3D>();

              if (mLayerTransform) (*mLayerTransform) = Amg::Translation3D(0.,0.,0.5*(zmin+zmax));
                mLayerSurface.reset( mLayerTransform ? new Trk::CylinderSurface(*mLayerTransform,boundaryr,0.5*(zmax-zmin))  :
                                     new Trk::CylinderSurface(boundaryr,0.5*(zmax-zmin)) );
                // create a MaterialLayer
                std::unique_ptr<const Trk::LayerMaterialProperties>  lmps( layerMaterialProperties(*mLayerSurface) );
                // LayerMaterialProperties will be cloned in MaterialLayer
                if (lmps) mLayer = std::make_unique<Trk::MaterialLayer>( 
                                                               std::shared_ptr<const Trk::Surface>(std::move(mLayerSurface)), 
                                                               *lmps );
            }
            // check if boundary face should be exchanged
            if (boundaryFaceExchange) {
                // creating only one boundary surface
                ATH_MSG_VERBOSE("Creating a joint boundary surface for n-to-n glueing case.");
                // the boundary transform can be 0 for cylinder surfaces
                std::unique_ptr<Amg::Transform3D> boundaryTransform =
                  ((zmin + zmax) * (zmin + zmax) < 10e-4)
                    ? nullptr
                    : std::make_unique<Amg::Transform3D>();

                if (boundaryTransform) (*boundaryTransform) = Amg::Translation3D(0.,0.,0.5*(zmin+zmax));
                // create the cylinder surface for the shared boundary
                Trk::CylinderSurface cSurface = boundaryTransform ? Trk::CylinderSurface(*boundaryTransform,boundaryr,0.5*(zmax-zmin)) :
                                                               Trk::CylinderSurface(boundaryr,0.5*(zmax-zmin));
                // get the volume outer radius of the sconf volumes 
                const Trk::CylinderVolumeBounds* cbTwo = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(secondVolumes[secondVolumes.size()-1]->volumeBounds()));
                if (cbTwo){
                    volumerTwo = cbTwo->outerRadius();
                }                                                                   
                // swap if needed 
                if (volumerTwo < volumerOne){
                    Trk::BinnedArray<const Trk::TrackingVolume>* navArraySwap = navArrayOne;
                    navArrayTwo = navArrayOne;
                    navArrayOne = navArraySwap;
                }
                // create the new boudnary surface which spans over the entire volume border
                Trk::SharedObject< Trk::BinnedArray<const Trk::TrackingVolume> >  navArrayInside(navArrayOne);
                Trk::SharedObject< Trk::BinnedArray<const Trk::TrackingVolume> >  navArrayOutside(navArrayTwo);
                Trk::BoundaryCylinderSurface<Trk::TrackingVolume>* boundarySurface = new Trk::BoundaryCylinderSurface<Trk::TrackingVolume>(navArrayInside,navArrayOutside,cSurface);
                Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > sharedBoundarySurface(boundarySurface);
                // attach the material layer to the shared boundary if existing
                if (mLayer) {
                  ATH_MSG_VERBOSE("Set MaterialLayer to the BoundarySurface of volume from second array.");
                  // assume that now the mlayer onwership goes over to the TrackingVolume
                  (const_cast<Trk::Surface&>(boundarySurface->surfaceRepresentation()))
                    .setMaterialLayer(*(mLayer.release()));
                }
                // set the boundary surface to the volumes of both sides
                for (const auto & volIter : firstVolumes){
                    ATH_MSG_VERBOSE("  -> first array : setting a newly created boundary surface to  " << volIter->volumeName());
                    setBoundarySurface(*volIter,sharedBoundarySurface,firstFace);
                }
                for (const auto & volIter : secondVolumes){
                    ATH_MSG_VERBOSE("  -> second array : setting a newly created boundary surface to  " << volIter->volumeName());
                    setBoundarySurface(*volIter,sharedBoundarySurface,secondFace);
                }
                // we are done here
                return;
        
            }
        } // build either boundary layer or exchange the face
    } // radial glueing


    // create the boundary faces - not creating a joint one
    ATH_MSG_VERBOSE("Leaving individual boundary surfaces for n-to-n glueing case.");

    // assign the navigation arrays
    Trk::SharedObject< Trk::BinnedArray< const Trk::TrackingVolume> > navArrayOneShared(navArrayOne);
    Trk::SharedObject< Trk::BinnedArray< const Trk::TrackingVolume> > navArrayTwoShared(navArrayTwo);

    Trk::Layer                       *mLayer_ptr=mLayer.get();
    // (a) to the first set of volumes
    for (const auto & tVolIter: firstVolumes) {
        // take care of the orientation of the normal vector
        if (firstFace != Trk::tubeInnerCover) {
            setOutsideTrackingVolumeArray(*tVolIter,firstFace,navArrayTwoShared);
            ATH_MSG_VERBOSE( "Set outsideTrackingVolumeArray at face " << firstFace << " to " << (*tVolIter).volumeName() );
        } else {
            setInsideTrackingVolumeArray(*tVolIter,firstFace,navArrayTwoShared);
            ATH_MSG_VERBOSE( "Set insideTrackingVolumeArray at face " << firstFace << " to " << (*tVolIter).volumeName() );
        }
        // set the boundary layer if it exists
        if (mLayer_ptr) {
            ATH_MSG_VERBOSE( "Set MaterialLayer to the BoundarySurface of volume from first array." );
            const Trk::Surface& firstFaceSurface = tVolIter->boundarySurfaces()[firstFace]->surfaceRepresentation();
            // assume that now the mlayer onwership goes over to the TrackingVolume
            mLayer.release();
            (const_cast<Trk::Surface&>(firstFaceSurface)).setMaterialLayer(*mLayer_ptr);
        }
                    
    }
    // (b) to the second set of volumes
    for (const auto & tVolIter : secondVolumes) {
        // take care of the orientation of the normal vector
        if (secondFace != Trk::tubeInnerCover) {
            ATH_MSG_VERBOSE( "Set outsideTrackingVolumeArray at face " << secondFace << " to " << (*tVolIter).volumeName() );
            setOutsideTrackingVolumeArray(*tVolIter,secondFace,navArrayOneShared);
        } else {
            ATH_MSG_VERBOSE( "Set insideTrackingVolumeArray at face " << secondFace << " to " << (*tVolIter).volumeName() );
            setInsideTrackingVolumeArray(*tVolIter,secondFace,navArrayOneShared);
        }
        if (mLayer_ptr) {
            ATH_MSG_VERBOSE( "Set MaterialLayer to the BoundarySurface of volume from second array." );
            const Trk::Surface& secondFaceSurface = tVolIter->boundarySurfaces()[secondFace]->surfaceRepresentation();
            // assume that now the mlayer onwership goes over to the TrackingVolume
            mLayer.release();
            (const_cast<Trk::Surface&>(secondFaceSurface)).setMaterialLayer(*mLayer_ptr);
        }
    }    
    // coverity will report a bug here for mLayer running out of scope, but the memory management is done later in the TrackingVolume
}

Trk::TrackingVolume* Trk::TrackingVolumeHelper::glueTrackingVolumeArrays(
                                                    const Trk::TrackingVolume& firstVol,
                                                    Trk::BoundarySurfaceFace firstFace,
                                                    const Trk::TrackingVolume& secondVol,
                                                    Trk::BoundarySurfaceFace secondFace, std::string name) const
{ 
    Trk::TrackingVolume* enclosingVolume = nullptr;

    const Trk::CylinderVolumeBounds* cyl1 = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(firstVol.volumeBounds()));
    const Trk::CylinderVolumeBounds* cyl2 = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(secondVol.volumeBounds()));

    if (!cyl1 || !cyl2) {
        ATH_MSG_ERROR( "TrackingVolumeHelper::glueTrackingVolumeArrays: input volumes not cylinders, return 0" );
        return enclosingVolume;
    }
    if (cyl1->halfPhiSector()!= M_PI || cyl2->halfPhiSector()!= M_PI ) {
        ATH_MSG_ERROR( "TrackingVolumeHelper::glueTrackingVolumeArrays: not coded for cylinder Phi sectors yet, return 0" );
        return enclosingVolume;
    }

    // if the swap is required
    Trk::BoundarySurfaceFace firstFaceCorr = firstFace;
    Trk::BoundarySurfaceFace secondFaceCorr = secondFace;


    // build volume envelope
    std::vector<const Trk::TrackingVolume*> vols;
    Trk::CylinderVolumeBounds* envBounds =  nullptr;
    Amg::Transform3D* envTransf = nullptr;
    Trk::BinnedArray<const Trk::TrackingVolume>*  subVols = nullptr;
    vols.push_back(&firstVol);
    vols.push_back(&secondVol);
    std::vector<const Trk::TrackingVolume*> envGlueNegXY;
    std::vector<const Trk::TrackingVolume*> envGluePosXY;
    std::vector<const Trk::TrackingVolume*> envGlueOuter;
    std::vector<const Trk::TrackingVolume*> envGlueInner;

    if (firstFace==Trk::positiveFaceXY) {
        envBounds =  new Trk::CylinderVolumeBounds(cyl1->innerRadius(),
                                                   cyl1->outerRadius(),
                                                   cyl1->halflengthZ()+cyl2->halflengthZ());
        envTransf   = new Amg::Transform3D;
        (*envTransf) = Amg::Translation3D(firstVol.center().x(), 
                                          firstVol.center().y(),
                                          firstVol.center().z()+cyl2->halflengthZ());
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(vols,false);
        envGlueNegXY.push_back(&firstVol);
        envGluePosXY.push_back(&secondVol);
        envGlueOuter = vols;
        envGlueInner = vols;
    } else if (firstFace==Trk::negativeFaceXY) {
        envBounds =  new Trk::CylinderVolumeBounds(cyl1->innerRadius(),
                                                   cyl1->outerRadius(),
                                                   cyl1->halflengthZ()+cyl2->halflengthZ());
        envTransf = new Amg::Transform3D;
        (*envTransf) = Amg::Translation3D(firstVol.center().x(), 
                                          firstVol.center().y(),
                                          firstVol.center().z()-cyl2->halflengthZ());
        envGlueNegXY.push_back(&secondVol);
        envGluePosXY.push_back(&firstVol);
        // revert vols
        vols.clear();
        vols.push_back(&secondVol);
        vols.push_back(&firstVol);
        // --- account for the swapping
        firstFaceCorr = secondFace;
        secondFaceCorr = firstFace;
        //
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(vols,false);
        envGlueOuter = vols;
        envGlueInner = vols;
    } else if (firstFace==Trk::tubeInnerCover) {
        if (secondFace==Trk::tubeOuterCover){
            envBounds =  new Trk::CylinderVolumeBounds(cyl2->innerRadius(),
                                                       cyl1->outerRadius(),
                                                       cyl1->halflengthZ());
        } else {
            envBounds =  new Trk::CylinderVolumeBounds(cyl1->outerRadius(),
                                                       cyl1->halflengthZ());
        }
        envTransf = firstVol.transform().isApprox(Amg::Transform3D::Identity()) ? nullptr : new Amg::Transform3D;
        if (envTransf)
           (*envTransf) = Amg::Translation3D(firstVol.center());
        // revert vols
        vols.clear();
        vols.push_back(&secondVol);
        vols.push_back(&firstVol);
        // account for the swapping
        firstFaceCorr = secondFace;
        secondFaceCorr = firstFace;
        //
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(vols,false);
        envGlueNegXY = vols;
        envGluePosXY = vols;
        envGlueOuter.push_back(&firstVol);
        envGlueInner.push_back(&secondVol);
    } else {
        envBounds =  new Trk::CylinderVolumeBounds(cyl1->innerRadius(),
                                                   cyl2->outerRadius(),
                                                   cyl1->halflengthZ());
        envTransf = firstVol.transform().isApprox(Amg::Transform3D::Identity()) ? nullptr : new Amg::Transform3D;
        if (envTransf)
           (*envTransf) = Amg::Translation3D(firstVol.center());
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(vols,false);
        envGlueNegXY = vols;
        envGluePosXY = vols;
        envGlueOuter.push_back(&secondVol);
        envGlueInner.push_back(&firstVol);
        // account for the swapping
        firstFaceCorr = secondFace;
        secondFaceCorr = firstFace;
    }

    // create the enveloping volume
    enclosingVolume  =  new Trk::TrackingVolume(envTransf,
                                                envBounds,
                                                firstVol,
                                                nullptr,subVols,
                                                name);

    // ENVELOPE GLUE DESCRIPTION -----------------------------------------------------------------
    // glue descriptors ---- they jump to the first one
    Trk::GlueVolumesDescriptor& glueDescr  = enclosingVolume->glueVolumesDescriptor();

    // for the outside volumes, could be done in a loop as well, but will only save 4 lines
    std::vector<const Trk::TrackingVolume*> glueNegXY;
    std::vector<const Trk::TrackingVolume*> gluePosXY;
    std::vector<const Trk::TrackingVolume*> glueInner;
    std::vector<const Trk::TrackingVolume*> glueOuter;
    fillGlueVolumes(vols,envGlueNegXY,Trk::negativeFaceXY,glueNegXY);
    fillGlueVolumes(vols,envGluePosXY,Trk::positiveFaceXY,gluePosXY);
    fillGlueVolumes(vols,envGlueInner,Trk::tubeInnerCover,glueInner);
    fillGlueVolumes(vols,envGlueOuter,Trk::tubeOuterCover,glueOuter);
    // set them to the envelopGlueDescriptor
    glueDescr.registerGlueVolumes(Trk::negativeFaceXY, glueNegXY);
    glueDescr.registerGlueVolumes(Trk::positiveFaceXY, gluePosXY);
    glueDescr.registerGlueVolumes(Trk::tubeInnerCover, glueInner);
    glueDescr.registerGlueVolumes(Trk::tubeOuterCover, glueOuter);
    glueDescr.registerGlueVolumes(Trk::cylinderCover,  glueOuter);

    // INTERNAL GLUEING ---------------------------------------------------------------------------
    glueTrackingVolumes(vols,firstFaceCorr,secondFaceCorr);

    return enclosingVolume;
}


/** private helper method to fill the glue volumes (or the volume itself in) */      
void Trk::TrackingVolumeHelper::fillGlueVolumes(const std::vector< const TrackingVolume*>& topLevelVolumes,
                                                const std::vector< const TrackingVolume*>& envelopeFaceVolumes,
                                                BoundarySurfaceFace glueFace, 
                                                std::vector<const Trk::TrackingVolume*>& glueVols) 
{
    // loop over the topLevel Volumes
    std::vector<const Trk::TrackingVolume*>::const_iterator refVolIter = topLevelVolumes.begin();
    for ( ; refVolIter != topLevelVolumes.end(); ++refVolIter ) {
        // loop over the faceVolumes
        for (unsigned int ienvFace=0; ienvFace< envelopeFaceVolumes.size(); ++ienvFace){
            // check whether this volume was assigned to on this face
            if (envelopeFaceVolumes[ienvFace]==(*refVolIter)) {
                // get the GlueVolumesDescriptor
                const Trk::GlueVolumesDescriptor& glueVolDescriptor = (*refVolIter)->glueVolumesDescriptor();
                // if the size of glue volumes is 0 -> the referenceVolume is at navigation level
                if ( (glueVolDescriptor.glueVolumes(glueFace)).empty()) {
                    glueVols.push_back(*refVolIter);
                } else {
                    // fill all the sub-volumes described by the glueVolumeDescriptor
                    for (unsigned int isubNavVol=0; isubNavVol<(glueVolDescriptor.glueVolumes(glueFace)).size(); ++isubNavVol)
                        glueVols.push_back( (glueVolDescriptor.glueVolumes(glueFace))[isubNavVol]  );
                }
            }
        }// loop over envelopeFaceVolumes
    } // loop over reference Volumes
}


/** Execute the glueing  - the input volumes are all on navigation level */
void Trk::TrackingVolumeHelper::glueTrackingVolumes(const std::vector<const Trk::TrackingVolume*>& glueVols,
                                                    BoundarySurfaceFace firstFace,
                                                    BoundarySurfaceFace secondFace) const
{

    if (glueVols.size()<2) {
        ATH_MSG_VERBOSE( "Nothing to do in glueVolumes() " );
        return;
    }


    ATH_MSG_VERBOSE( " glueTrackingVolumes() called with boundary faces " << int(firstFace) << " and " << int(secondFace) << "." );

    // the iterators through the volumes
    std::vector<const Trk::TrackingVolume*>::const_iterator firstVol  = glueVols.begin();
    std::vector<const Trk::TrackingVolume*>::const_iterator secondVol = firstVol;
    ++secondVol;
    for ( ; secondVol != glueVols.end(); ++firstVol, ++secondVol) {

        if (msgLvl(MSG::VERBOSE))
            ATH_MSG_VERBOSE( "Processing '" << (*firstVol)->volumeName() << "' and '" << (*secondVol)->volumeName() << "'." );

        // get the glue volume descriptors to see that we have all subvolumes
        const Trk::GlueVolumesDescriptor& glueDescr1 = (*firstVol)->glueVolumesDescriptor();
        const Trk::GlueVolumesDescriptor& glueDescr2 = (*secondVol)->glueVolumesDescriptor();

        // glue volumes at navigation level
        std::vector<const Trk::TrackingVolume*> glueVols1;
        std::vector<const Trk::TrackingVolume*> glueVols2;
        glueVols1 = glueDescr1.glueVolumes(firstFace);
        glueVols2 = glueDescr2.glueVolumes(secondFace);

        // trivial cases
        // (glue one to the other)
        if (glueVols1.empty() && glueVols2.empty()) {
            glueTrackingVolumes(**firstVol,firstFace,**secondVol,secondFace);
            continue;
            // (glue one to many)
        } else if (glueVols1.empty() && !glueVols2.empty()) {
            glueVols1.push_back(*firstVol);
            // (glue the other one to many)
        } else if (!glueVols1.empty() && glueVols2.empty()) {
            glueVols2.push_back(*secondVol);
        }

        // non-trivial case :: array against array
        // in Z : assume 2dim R/Phi
        if (firstFace==Trk::negativeFaceXY || firstFace==positiveFaceXY ) {
            // turn both vectors into R/Phi 2dim binnedArrays; assume equidistant binning in Phi
            Trk::BinnedArray<const Trk::TrackingVolume>*  gv1 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiR(glueVols1,true);
            Trk::BinnedArray<const Trk::TrackingVolume>*  gv2 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiR(glueVols2,true);
            SharedObject<Trk::BinnedArray<const Trk::TrackingVolume> > sgv1(gv1);
            SharedObject<Trk::BinnedArray<const Trk::TrackingVolume> > sgv2(gv2);

            // array vs. array in Z
            if (glueVols2.size()>1)
                for (unsigned int i=0; i<glueVols1.size(); i++) setOutsideTrackingVolumeArray( *(glueVols1[i]), firstFace, sgv2 );
            else
                for (unsigned int i=0; i<glueVols1.size(); i++) setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols2[0] );

            if (glueVols1.size()>1)
                for (unsigned int i=0; i<glueVols2.size(); i++) setOutsideTrackingVolumeArray( *(glueVols2[i]), secondFace, sgv1 );
            else
                for (unsigned int i=0; i<glueVols2.size(); i++) setOutsideTrackingVolume( *(glueVols2[i]), secondFace, glueVols1[0] );


        } else {
            // turn both vectors into Z/Phi 2dim binnedArrays; assume equidistant binning in Phi
            Trk::BinnedArray<const Trk::TrackingVolume>*  gv1 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiZ(glueVols1,true);
            Trk::BinnedArray<const Trk::TrackingVolume>*  gv2 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiZ(glueVols2,true);
            SharedObject<Trk::BinnedArray<const Trk::TrackingVolume> > sgv1(gv1);
            SharedObject<Trk::BinnedArray<const Trk::TrackingVolume> > sgv2(gv2);

            // the glue cases -----------------------------------------------------------------------------------
            // handle the tube with care !
            // first vol
            for (unsigned int i=0; i<glueVols1.size(); i++) {
                // set the array as the outside array of the firstVol
                if (firstFace != Trk::tubeInnerCover) {
                    if (glueVols2.size()>1)
                        setOutsideTrackingVolumeArray( *(glueVols1[i]), firstFace, sgv2 );
                    else
                        setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols2[0] );
                } else {
                    if (glueVols2.size()>1){
                        setInsideTrackingVolumeArray( *(glueVols1[i]), firstFace, sgv2 );
                        setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols1[i] );
                    } else {
                        setInsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols2[0] );
                        setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols1[i] );
                    }
                }
            }
            // second
            for (unsigned int i=0; i<glueVols2.size(); i++) {
                // set the array as the outside array of the secondVol
                if (secondFace != Trk::tubeInnerCover)
                    setOutsideTrackingVolumeArray( *(glueVols2[i]), secondFace, sgv1 );
                else {
                    setInsideTrackingVolumeArray( *(glueVols2[i]), secondFace, sgv1 );
                    setOutsideTrackingVolume( *(glueVols2[i]), secondFace, glueVols2[i] );
                }
            }
        }
    }                 
}

Trk::LayerMaterialProperties* Trk::TrackingVolumeHelper::layerMaterialProperties(const Trk::Surface& boundarySurface) const
{
  Trk::LayerMaterialProperties* layerMaterial = nullptr;
  if (boundarySurface.type() == Trk::SurfaceType::Cylinder){
        const Trk::CylinderBounds* cb = dynamic_cast<const Trk::CylinderBounds*>(&boundarySurface.bounds());
        if (!cb) throw std::logic_error("Not CylinderBounds");
        // --------------- material estimation ----------------------------------------------------------------
        // -- material with 1D binning
        double hz = cb->halflengthZ();
        double r  = cb->r();
        Trk::BinUtility layerBinUtilityZ(m_barrelLayerBinsZ, -hz, hz, Trk::open, Trk::binZ);
        if (m_barrelLayerBinsPhi==1){
            layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityZ);
        } else  { // -- material with 2D binning
            Trk::BinUtility layerBinUtilityRPhiZ(m_barrelLayerBinsPhi, -r*M_PI, r*M_PI, Trk::closed,Trk::binRPhi);
            layerBinUtilityRPhiZ += layerBinUtilityZ;                                                       
            layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityRPhiZ);
        }
        // --------------- material estimation ----------------------------------------------------------------
  }
  if (boundarySurface.type() == Trk::SurfaceType::Disc){
      // --------------- material estimation ----------------------------------------------------------------
      const Trk::DiscBounds* db = dynamic_cast<const Trk::DiscBounds*>(&boundarySurface.bounds());
      if (!db) throw std::logic_error("Not DiscBounds");
      double rMin = db->rMin();
      double rMax = db->rMax();
      Trk::BinUtility layerBinUtilityR(m_endcapLayerBinsR,rMin,rMax,Trk::open, Trk::binR);
      // -- material with 1D binning
      if (m_endcapLayerBinsPhi==1){
          layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityR);
      } else { // -- material with 2D binning
          Trk::BinUtility layerBinUtilityPhi(m_endcapLayerBinsPhi,-M_PI,M_PI,Trk::closed,Trk::binPhi);
          layerBinUtilityR += layerBinUtilityPhi;
          layerMaterial     = new Trk::BinnedLayerMaterial(layerBinUtilityR);
      }
      // --------------- material estimation ----------------------------------------------------------------
  }
  // return what you have
  return layerMaterial;    
}



/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setInsideTrackingVolume(const Trk::TrackingVolume& tvol,
                                                        Trk::BoundarySurfaceFace face,
                                                        const Trk::TrackingVolume* insidevol) const
{
    Trk::TrackingVolumeManipulator::setInsideVolume( tvol, face, insidevol );
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setInsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                             Trk::BoundarySurfaceFace face,
                                                             Trk::BinnedArray<const Trk::TrackingVolume>* insidevolarray) const
{
    Trk::TrackingVolumeManipulator::setInsideVolumeArray(tvol,face,insidevolarray);
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setInsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                             Trk::BoundarySurfaceFace face,
                                                             Trk::SharedObject<Trk::BinnedArray<const Trk::TrackingVolume> > insidevolarray) const
{
    Trk::TrackingVolumeManipulator::setInsideVolumeArray(tvol,face,insidevolarray);
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setOutsideTrackingVolume(const Trk::TrackingVolume& tvol,
                                                         Trk::BoundarySurfaceFace face,
                                                         const Trk::TrackingVolume* outsidevol) const
{ 
    ATH_MSG_VERBOSE( "     -> Glue '" << outsidevol->volumeName() << "' at face " << face << " to '" << tvol.volumeName() << "'.");
    Trk::TrackingVolumeManipulator::setOutsideVolume( tvol, face, outsidevol );
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setOutsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                              Trk::BoundarySurfaceFace face,
                                                              Trk::BinnedArray<const Trk::TrackingVolume>* outsidevolarray) const
{ 
    unsigned int numVols = outsidevolarray->arrayObjects().size() ;
    ATH_MSG_VERBOSE( "     -> Glue " << numVols << " volumes at face " << face << " to '" << tvol.volumeName() );
    Trk::TrackingVolumeManipulator::setOutsideVolumeArray( tvol, face, outsidevolarray );
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setOutsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                              Trk::BoundarySurfaceFace face,
                                                              Trk::SharedObject<Trk::BinnedArray<const Trk::TrackingVolume> > outsidevolarray) const
{ 
    unsigned int numVols = outsidevolarray.get()->arrayObjects().size() ;
    ATH_MSG_VERBOSE( "     -> Glue " << numVols << " volumes at face " << face << " to '" << tvol.volumeName() );
    Trk::TrackingVolumeManipulator::setOutsideVolumeArray( tvol, face, outsidevolarray );
}



