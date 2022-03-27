/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialValidation.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
// Trk
#include "MaterialValidation.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkDetDescrInterfaces/IMaterialMapper.h"
#include "TrkNeutralParameters/NeutralParameters.h"

// test 
#include "TrkVolumes/CylinderVolumeBounds.h"

// Gaudi Units
#include "GaudiKernel/SystemOfUnits.h"


Trk::MaterialValidation::MaterialValidation(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name,pSvcLocator)    ,
  m_materialMapper("Trk::MaterialMapper/MappingMaterialMapper"),
  m_maxMaterialValidationEvents(25000),
  m_flatDist(nullptr),
  m_etaMin(-3.),
  m_etaMax(3.),
  m_runNativeNavigation(true),
  m_accTinX0(0)
{

    // ---------------------- The Material Mapping -------------------------- //
    // the toolhandle of the MaterialMapper to be used
    declareProperty("MaterialMapper"               , m_materialMapper);
    declareProperty("MaximumMappingEvents"         , m_maxMaterialValidationEvents);
    // ---------------------- Range setup ----------------------------------- //
    declareProperty("MinEta"                      , m_etaMin);
    declareProperty("MaxEta"                      , m_etaMax);
    // ---------------------- Native navigation ----------------------------- //
    declareProperty("NativeNavigation"            , m_runNativeNavigation);
}  

Trk::MaterialValidation::~MaterialValidation()
{
    delete m_flatDist;
}


StatusCode Trk::MaterialValidation::initialize()
{
    
    // Get the TrackingGeometry from StoreGate
#ifdef LEGACY_TRKGEOM
    if (!m_trackingGeometrySvc.empty()) {
       ATH_CHECK( m_trackingGeometrySvc.retrieve());
    }
#endif
    ATH_CHECK( m_trackingGeometryReadKey.initialize(!m_trackingGeometryReadKey.key().empty()) );
    
    if ( (m_materialMapper.retrieve()).isFailure() )
        ATH_MSG_WARNING("Could not retrieve MaterialMapper");

    // initialize the random generators
    m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));
    return StatusCode::SUCCESS;
}


StatusCode Trk::MaterialValidation::execute()
{
    ATH_MSG_VERBOSE( "MaterialValidation execute() start ================================================" );

    // create the random direction - flat in eta
    double eta   = m_etaMin + (m_etaMax-m_etaMin)*m_flatDist->shoot();
    double theta = 2.*atan(exp(-eta));
    double phi   = M_PI * ( 2*m_flatDist->shoot() - 1.);
    m_accTinX0 = 0;
    
    // get the position and riection from the random numbers
    Amg::Vector3D position(0.,0.,0.);
    Amg::Vector3D direction(sin(theta)*cos(phi),sin(theta)*sin(phi), cos(theta));
    
    ATH_MSG_DEBUG("[>] Start mapping event with phi | eta = " << phi << " | " << direction.eta()); 
     
    // find the start TrackingVolume
    const Trk::TrackingVolume* sVolume = trackingGeometry().lowestTrackingVolume(position);
    const Trk::TrackingVolume* nVolume = sVolume;
    while (nVolume ) {
        Trk::PositionAtBoundary paB = collectMaterialAndExit(*nVolume, position, direction);
        position =  paB.first;
        nVolume  =  paB.second;
    }     
    ATH_MSG_DEBUG("[<] Finishing event with collected path [X0] = " << m_accTinX0);
    return StatusCode::SUCCESS;
}

Trk::PositionAtBoundary Trk::MaterialValidation::collectMaterialAndExit(const Trk::TrackingVolume& tvol, 
                                                                        const Amg::Vector3D& position, 
                                                                        const Amg::Vector3D& direction)
{
    // get the entry layers -----------------------------------------------------------
    std::map<double, Trk::AssociatedMaterial>                      collectedMaterial;
    
    // all boundaries found --- proceed 
    Trk::PositionAtBoundary pab(position, 0);
    
    ATH_MSG_DEBUG("[>>] Entering Volume: " << tvol.volumeName() << "- at " << Amg::toString(position));
    
    Trk::NeutralCurvilinearParameters cvp(position,direction,0.);
    
    if (m_runNativeNavigation){
        // A : collect all hit layers 
        auto layerIntersections = tvol.materialLayersOrdered<Trk::NeutralCurvilinearParameters>(nullptr,nullptr,cvp,Trk::alongMomentum);
        // loop over the layers
        for (auto& lCandidate : layerIntersections ) {
            // get the layer
             const Trk::Layer*    layer   = lCandidate.object;
             double pathLength = lCandidate.intersection.pathLength;
             // get the associate material 
             if (layer->layerMaterialProperties()){
                 // take it from the global position
                 const Trk::MaterialProperties* mprop = layer->layerMaterialProperties()->fullMaterial(lCandidate.intersection.position);
                 if (mprop){
                     double stepLength = mprop->thickness()*fabs(layer->surfaceRepresentation().pathCorrection(lCandidate.intersection.position,direction));
                     collectedMaterial[pathLength] = Trk::AssociatedMaterial(lCandidate.intersection.position, mprop, stepLength, &tvol, layer);
                 }
             }
         }             
        // B : collect all boundary layers, start from last hit layer         
        Amg::Vector3D lastPosition = !collectedMaterial.empty() ? collectedMaterial.rbegin()->second.materialPosition() : (position + direction.unit());
        Trk::NeutralCurvilinearParameters lcp(lastPosition,direction,0.);
        // boundary surfaces
        auto boundaryIntersections = tvol.boundarySurfacesOrdered<Trk::NeutralCurvilinearParameters>(lcp,Trk::alongMomentum);
        if (!boundaryIntersections.empty()){
            // by definition is the first one
            lastPosition = boundaryIntersections.begin()->intersection.position;
            const Trk::BoundarySurface<Trk::TrackingVolume>* bSurfaceTV = boundaryIntersections.begin()->object;
            const Trk::Surface& bSurface = bSurfaceTV->surfaceRepresentation(); 
            // get the path lenght to it
            if (bSurface.materialLayer() && bSurface.materialLayer()->layerMaterialProperties()){
                const Trk::MaterialProperties* mprop = bSurface.materialLayer()->layerMaterialProperties()->fullMaterial(lastPosition);
                double pathLength = (lastPosition-position).mag();
                if (mprop){
                    double stepLength = mprop->thickness()*fabs(bSurface.pathCorrection(lastPosition,direction));
                    collectedMaterial[pathLength] = Trk::AssociatedMaterial(lastPosition, mprop, stepLength, &tvol, bSurface.materialLayer());
                } else
                    collectedMaterial[pathLength] = Trk::AssociatedMaterial(lastPosition, &tvol, bSurface.materialLayer());
            }
            // set the new volume 
            const Trk::TrackingVolume* naVolume = bSurfaceTV->attachedVolume(lastPosition, direction, Trk::alongMomentum);
            pab = Trk::PositionAtBoundary(lastPosition,naVolume);
        } else 
            pab = Trk::PositionAtBoundary(lastPosition,0);
    } else {
        std::map<double, std::pair<const Trk::Layer*, Amg::Vector3D> > intersectedLayers;
               
        // Process the contained layers if they exist
        const Trk::LayerArray* layerArray = tvol.confinedLayers();
        if (layerArray) {
           // display output
           Trk::BinnedArraySpan<Trk::Layer const * const> layers = layerArray->arrayObjects();
           auto layIter  = layers.begin();
           auto layIterE = layers.end();    
           for ( ; layIter != layIterE; ++layIter){
               if ( (*layIter)->layerMaterialProperties() ){
                     Trk::Intersection lsIntersection = (*layIter)->surfaceRepresentation().straightLineIntersection(position, direction, true, true);
                   if (lsIntersection.valid){
                     intersectedLayers[lsIntersection.pathLength] = std::pair<const Trk::Layer*, Amg::Vector3D>(*layIter,lsIntersection.position);
                     // get & record the material
                     //   - the position on the surface
                     Amg::Vector3D mposition = (*layIter)->surfaceRepresentation().transform().inverse()*lsIntersection.position;
                     const Trk::MaterialProperties* mprop = (*layIter)->layerMaterialProperties()->fullMaterial(mposition);
                     if (mprop) {
                         double stepLength = mprop->thickness()*fabs((*layIter)->surfaceRepresentation().pathCorrection(lsIntersection.position,direction));
                         collectedMaterial[lsIntersection.pathLength] = Trk::AssociatedMaterial(lsIntersection.position, mprop, stepLength, &tvol, (*layIter));
                     } else 
                         collectedMaterial[lsIntersection.pathLength] = Trk::AssociatedMaterial(lsIntersection.position, &tvol, (*layIter));
        
                     ATH_MSG_VERBOSE("[>>>>] record material hit at layer with index " << (*layIter)->layerIndex().value() << " - at " << Amg::toString(lsIntersection.position) );
                     if (mprop)
                         ATH_MSG_VERBOSE("[>>>>] MaterialProperties are " << (*mprop) );
                     else 
                         ATH_MSG_VERBOSE("[>>>>] No MaterialProperties found." );                     
                   }
               }
           }
        }
        
        // material for confined layers collected, now go to boundary
        
        // update the position to the last one
        Amg::Vector3D lastPosition = !intersectedLayers.empty() ? (*(--(intersectedLayers.end()))).second.second : position;
        
        std::map<double, Trk::VolumeExit > volumeExits;
        // now find the exit point
        const std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > >& bSurfaces = tvol.boundarySurfaces();
        auto bSurfIter  = bSurfaces.begin();
        auto bSurfIterE = bSurfaces.end();
        for ( ; bSurfIter != bSurfIterE; ++bSurfIter){
            // omit positions on the surface
            if (  !(*bSurfIter)->surfaceRepresentation().isOnSurface(lastPosition, true, 0.1, 0.1)  ){
                Trk::Intersection evIntersection = (*bSurfIter)->surfaceRepresentation().straightLineIntersection(lastPosition, direction, true, true);
                ATH_MSG_VERBOSE("[>>>>] boundary surface intersection / validity :" << Amg::toString(evIntersection.position) << " / " << evIntersection.valid);
	            ATH_MSG_VERBOSE("[>>>>] with path length = " << evIntersection.pathLength );
                if (evIntersection.valid){
                    // next attached Tracking Volume
                    const Trk::TrackingVolume* naVolume = (*bSurfIter)->attachedVolume(evIntersection.position, direction, Trk::alongMomentum);
                    // put it into the map
                    volumeExits[evIntersection.pathLength] = Trk::VolumeExit(naVolume, (&(*bSurfIter)->surfaceRepresentation()), evIntersection.position);
                    // volume exit
                    ATH_MSG_VERBOSE("[>>>>] found volume exit - at " << Amg::toString(evIntersection.position) );
              }
            } else
                ATH_MSG_VERBOSE("[>>>>] starting position is on surface ! " );
        }
        // prepare the boundary    
        if (!volumeExits.empty()){
            // get the first entry in the map: closest next volume
            VolumeExit closestVolumeExit = (*volumeExits.begin()).second;
            // check if the volume exit boundary has material attached
            const Trk::Surface* bSurface = closestVolumeExit.bSurface;
            if ( bSurface && bSurface->materialLayer() && bSurface->materialLayer()->layerMaterialProperties()){
                ATH_MSG_VERBOSE("[>>>>] The boundary surface has an associated layer, collect material from there");
                const Trk::MaterialProperties* mprop = bSurface->materialLayer()->layerMaterialProperties()->fullMaterial(closestVolumeExit.vExit);
                double pathToExit = (closestVolumeExit.vExit-lastPosition).mag();
                if (mprop){
                    double stepLength = mprop->thickness()*fabs(bSurface->pathCorrection(closestVolumeExit.vExit,direction));
                    collectedMaterial[pathToExit] = Trk::AssociatedMaterial(closestVolumeExit.vExit, mprop, stepLength, &tvol, bSurface->materialLayer());
                } else
                    collectedMaterial[pathToExit] = Trk::AssociatedMaterial(closestVolumeExit.vExit, &tvol, bSurface->materialLayer());
            }
            // 
            if (closestVolumeExit.nVolume != &tvol && closestVolumeExit.nVolume) {
                 ATH_MSG_VERBOSE("[>>>>] Next Volume: " << closestVolumeExit.nVolume->volumeName() << " - at " << Amg::toString(closestVolumeExit.vExit) );
                // return for further navigation
                pab = Trk::PositionAtBoundary(closestVolumeExit.vExit,closestVolumeExit.nVolume);
            }
        } else {
            ATH_MSG_VERBOSE( "[>>>>] No exit found from Volume '" <<  tvol.volumeName() << "' - starting radius = " << lastPosition.perp() );
            const Trk::CylinderVolumeBounds* cvb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(tvol.volumeBounds()));
            if (cvb) 
                ATH_MSG_VERBOSE( "[>>>>] Volume outer radius = " << cvb->outerRadius() );
        }
    
    }
    // finally collect the material
    ATH_MSG_DEBUG("[>>>] Collecting materials from "<< collectedMaterial.size() << " layers");
    // provide the material to the material mapper 
    auto cmIter  = collectedMaterial.begin();
    auto cmIterE = collectedMaterial.end();
    for ( ; cmIter != cmIterE; ++cmIter ){        
        m_materialMapper->recordMaterialHit(cmIter->second, cmIter->second.materialPosition());
        m_accTinX0 += cmIter->second.steplengthInX0();
        int layerIndex = cmIter->second.associatedLayer() ? cmIter->second.associatedLayer()->layerIndex().value() : 0;
        ATH_MSG_DEBUG("[>>>] Accumulate pathLength/X0 on layer with index " << layerIndex << " - t/X0 (total so far) =  " << cmIter->second.steplengthInX0() << " (" << m_accTinX0 << ")");
        if (layerIndex){
          std::string surfaceType =
            cmIter->second.associatedLayer()->surfaceRepresentation().type() ==
                Trk::SurfaceType::Cylinder
              ? "Cylinder at radius = "
              : "Disc at z-position = ";
          std::string layerType =
            cmIter->second.associatedLayer()->surfaceArray() ? "Active "
                                                             : "Passive ";
          double rz =
            cmIter->second.associatedLayer()->surfaceRepresentation().type() ==
                Trk::SurfaceType::Cylinder
              ? cmIter->second.associatedLayer()
                  ->surfaceRepresentation()
                  .bounds()
                  .r()
              : cmIter->second.associatedLayer()
                  ->surfaceRepresentation()
                  .center()
                  .z();
          ATH_MSG_DEBUG("      " << layerType << surfaceType << rz);
        }
        ATH_MSG_DEBUG("      Distance to origin is " << cmIter->second.materialPosition().mag() );
    }
    
    // return what you have
    return pab;
                                                                            
}
                                          


StatusCode Trk::MaterialValidation::finalize()
{
    ATH_MSG_INFO( "MaterialValidation finalize()" );
    return StatusCode::SUCCESS;
}

void Trk::MaterialValidation::throwFailedToGetTrackingGeometry() const {
   std::stringstream msg;
   msg << "Failed to get conditions data " << m_trackingGeometryReadKey.key() << ".";
   throw std::runtime_error(msg.str());
}


