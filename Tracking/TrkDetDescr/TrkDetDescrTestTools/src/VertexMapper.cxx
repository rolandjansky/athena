/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexMapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTestTools/VertexMapper.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"


// constructor
Trk::VertexMapper::VertexMapper(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_trackingGeometry(nullptr),
  m_trackingGeometryName("AtlasTrackingGeometry")
{
    declareInterface<IVertexMapper>(this);

    // The TrackingGeometry
    declareProperty("TrackingGeometry"                      , m_trackingGeometryName);


}

// destructor
Trk::VertexMapper::~VertexMapper()
{

}

// Athena standard methods
// initialize
StatusCode Trk::VertexMapper::initialize()
{


    return StatusCode::SUCCESS;
}     

// finalize
StatusCode Trk::VertexMapper::finalize()
{
    ATH_MSG_INFO(  "finalize() successful" );
    return StatusCode::SUCCESS;
}


StatusCode Trk::VertexMapper::updateTrackingGeometry() const {
    // retrieve the TrackingGeometry from the detector store 
    if (detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName).isFailure()){
        ATH_MSG_ERROR("Could not retrieve the tracking geometry. Bailing out.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

/** Record the vertex into the local frame of the closest module  */
Trk::MappedVertex Trk::VertexMapper::mapToLocal(const Amg::Vector3D& vertex) const {
    
    // first find the volume where you are in
    const Trk::TrackingVolume* mVolume = trackingGeometry().lowestTrackingVolume(vertex);
    // we have a volume, so go on
    if (mVolume){
        // from the volume, get all the layers 
        const Trk::LayerArray* mLayerArray = mVolume->confinedLayers();
        // and check if we have confined layers
        if (mLayerArray){
            // extract all the layers with sensitive surfaces
          Trk::BinnedArraySpan<Trk::Layer const * const> mLayerObjects = mLayerArray->arrayObjects();
          std::vector<const Trk::Layer*> mSensitiveLayers;
            // loop over for the extraction - and check if they have a sub surface array
            for (const auto & mLayer : mLayerObjects){
                if (mLayer->surfaceArray()){
                    // remember this layer
                    mSensitiveLayers.push_back(mLayer);
                }
            }
            // now find out the closest - if you have layers to test
            if (!mSensitiveLayers.empty()){
                // prepare the book keepint
                double        mDistance    = 10e10;
                const Layer*  mLayer       = nullptr;
                Amg::Vector3D mPosition(0.,0.,0.);
                Amg::Vector3D mDirection(0.,0.,0.);
                // loop over the test layers 
                for (auto& sLayer : mSensitiveLayers) {
                    // find out the optimal association : 
                    // - for cylinders we want to radially hit the layer
                    // - for disks we want to move along z
                    mDirection = Amg::Vector3D(
                                   (sLayer->surfaceRepresentation().type() ==
                                    Trk::SurfaceType::Cylinder)
                                     ? Amg::Vector3D(vertex.x(), vertex.y(), 0.)
                                     : Amg::Vector3D(0., 0., 1.))
                                   .unit();
                    // now intersect the layer
                    Intersection sIntersection = sLayer->surfaceRepresentation().straightLineIntersection(vertex,mDirection,false,false);
                    // check if valid
                    if (sIntersection.valid){
                        // record the new closest distance and remember the layer
                        double currentDistance = fabs(sIntersection.pathLength);
                        if ( currentDistance < mDistance ){
                            // assign the new closest distance
                            mDistance = currentDistance;
                            mLayer    = sLayer;
                            mPosition = sIntersection.position;
                        }
                    }
                } // loop over layers done
                
                // continue if you have found a good layer
                if (mLayer) {
                    // let's find the assoicated surfaces using the overlap descriptor
                    std::vector<SurfaceIntersection> testSurfaces;
                    // get the main target surface
                    const Surface* mSurface = mLayer->subSurface(mPosition);
                    // we have more than one test surface 
                    if (mSurface && mLayer->overlapDescriptor()){
                        // get the reachable surfaces, the target surface will be added 
                        mLayer->overlapDescriptor()->reachableSurfaces(testSurfaces, *mSurface, vertex, mDirection);
                        // let's loop over the provided surfaces, intersect them and take the closest
                        mDistance = 10e10;
                        for (auto& tSurface : testSurfaces){
                             // intersect
                             Intersection tsfInter = tSurface.object->straightLineIntersection(vertex,mDirection,false,false);
                             // check if the intersection is valid and the maxPathLength has not been exceeded
                             if (tsfInter.valid && tsfInter.pathLength < mDistance ){
                                 // change the mDistance and remember the surface
                                 mDistance = tsfInter.pathLength;
                                 mSurface  = tSurface.object;
                             } 
                        }
                    }
                    // now collect all the other surfaces from the compatible surface call 
                    if (mSurface){
                        // we have a surface, do the final transformation into the surface frame
                        Amg::Vector3D mLocPosition = mSurface->transform().inverse()*vertex;
                        // everything is set -> return 
                        return Trk::MappedVertex(mLocPosition,*mSurface,*mLayer,*mVolume);
                    }
                }
            }
        }
    }
    // did not succeed return an invalid solution
    return Trk::MappedVertex();
}

