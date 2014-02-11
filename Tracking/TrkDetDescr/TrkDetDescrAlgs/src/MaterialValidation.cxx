/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialValidation.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
// Trk
#include "TrkDetDescrAlgs/MaterialValidation.h"
#include "TrkGeometry/EntryLayerProvider.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/IMaterialMapper.h"

// test 
#include "TrkVolumes/CylinderVolumeBounds.h"

// Gaudi Units
#include "GaudiKernel/SystemOfUnits.h"


Trk::MaterialValidation::MaterialValidation(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name,pSvcLocator)    ,
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc",name),
  m_trackingGeometry(0),
  m_materialMapper("Trk::MaterialMapper/MappingMaterialMapper"),
  m_materialMappingEvents(0),
  m_maxMaterialMappingEvents(25000),
  m_flatDist(0),
  m_etaMin(-3.),
  m_etaMax(3.)
{

    // ---------------------- The TrackingGeometrySvc ------------------------ //
    declareProperty("TrackingGeometrySvc"         , m_trackingGeometrySvc);
    // ---------------------- The Material Mapping -------------------------- //
    // the toolhandle of the MaterialMapper to be used
    declareProperty("MaterialMapper"               , m_materialMapper);
    declareProperty("MaximumMappingEvents"         , m_maxMaterialMappingEvents);
    // ---------------------- Range setup ----------------------------------- //
    declareProperty("MinEta"                      , m_etaMin);
    declareProperty("MaxEta"                      , m_etaMax);
}  

Trk::MaterialValidation::~MaterialValidation()
{
    delete m_flatDist;
}


StatusCode Trk::MaterialValidation::initialize()
{
    
    // Get the TrackingGeometry from StoreGate
    // initialize the TrackingGeometrySvc
    if (m_trackingGeometrySvc.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Cannot retrieve TrackingGeometrySvc. Abort job. " );
        return StatusCode::FAILURE;
    }
    
    if ( (m_materialMapper.retrieve()).isFailure() )
        ATH_MSG_WARNING("Could not retrieve MaterialMapper");

    // initialize the random generators
    m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));
    return StatusCode::SUCCESS;
}


StatusCode Trk::MaterialValidation::execute()
{
    ATH_MSG_VERBOSE( "MaterialValidation execute() start" );

    // ------------------------------- get the trackingGeometry at first place
    if (!m_trackingGeometry) {
        StatusCode retrieveCode = retrieveTrackingGeometry();
        if (retrieveCode.isFailure()){
            ATH_MSG_INFO( "Could not retrieve TrackingGeometry. Exiting." );
            return retrieveCode;
        }
    }

    // create the random direction - flat in eta
    double eta   = m_etaMin + (m_etaMax-m_etaMin)*m_flatDist->shoot();
    double theta = 2.*atan(exp(-eta));
    double phi   = M_PI * ( 2*m_flatDist->shoot() - 1.);
    
    // get the position and riection from the random numbers
    Amg::Vector3D position(0.,0.,0.);
    Amg::Vector3D direction(sin(theta)*cos(phi),sin(theta)*sin(phi), cos(theta));
     
    // find the start TrackingVolume
    const Trk::TrackingVolume* sVolume = m_trackingGeometry->lowestTrackingVolume(position);
    const Trk::TrackingVolume* nVolume = sVolume;
    while (nVolume ) {
        Trk::PositionAtBoundary paB = collectMaterialAndExit(*nVolume, position, direction);
        position =  paB.first;
        nVolume  =  paB.second;
    }
     
    // finalize the event 
    m_materialMapper->finalizeEvent(eta,phi);

    return StatusCode::SUCCESS;
}

Trk::PositionAtBoundary Trk::MaterialValidation::collectMaterialAndExit(const Trk::TrackingVolume& tvol, 
                                                                        const Amg::Vector3D& position, 
                                                                        const Amg::Vector3D& direction)
{
    // get the entry layers -----------------------------------------------------------
    std::map<double, std::pair<const Trk::Layer*, Amg::Vector3D> > intersectedLayers;
    std::map<double, Trk::AssociatedMaterial>                      collectedMaterial;
    
    ATH_MSG_VERBOSE("Entering Volume: " << tvol.volumeName() << "- at " << Amg::toString(position));
    
    // get theta from the direction
    double theta = direction.theta();
    
    // Process the entry layers if they exist
    const Trk::EntryLayerProvider* entryLayerProvider = tvol.entryLayerProvider();
    if (entryLayerProvider) {
      // display output
      const std::vector<const Trk::Layer*>& entryLayers = entryLayerProvider->layers();
      auto eLayIter  = entryLayers.begin();
      auto eLayIterE = entryLayers.end();    
      for ( ; eLayIter != eLayIterE; ++eLayIter){
          Trk::SurfaceIntersection esIntersection = (*eLayIter)->surfaceRepresentation().straightLineIntersection(position, direction, true, true);
          if (esIntersection.valid){
              // intersection valid : record it
              intersectedLayers[esIntersection.pathLength] = std::pair<const Trk::Layer*, Amg::Vector3D>(*eLayIter,esIntersection.intersection);
              // calculate the pathCorrection
              double stepLength = (*eLayIter)->surfaceRepresentation().type() == Trk::Surface::Cylinder ? fabs(1./sin(theta)) : fabs(1./cos(theta));
              // get & record the material
              const Trk::MaterialProperties* mprop = 0;
              if ((*eLayIter)->layerMaterialProperties()){
                  Amg::Vector3D mposition = (*eLayIter)->surfaceRepresentation().transform().inverse()*esIntersection.intersection;
                  mprop = (*eLayIter)->layerMaterialProperties()->fullMaterial(mposition);
              }
              // if you have material properties 
              if (mprop)
                  collectedMaterial[esIntersection.pathLength] = Trk::AssociatedMaterial(esIntersection.intersection, mprop, stepLength, &tvol, (*eLayIter));
              else 
                  collectedMaterial[esIntersection.pathLength] = Trk::AssociatedMaterial(esIntersection.intersection, &tvol, (*eLayIter));

              ATH_MSG_VERBOSE(" - record material hit at (entry) layer with index " << (*eLayIter)->layerIndex().value() << " - at " << Amg::toString(esIntersection.intersection) );
              if (mprop)
                  ATH_MSG_VERBOSE(" - MaterialProperties are " << (*mprop) );
              else 
                  ATH_MSG_VERBOSE(" - No MaterialProperties found." );
              
          }
       }
    }
    
    // Process the contained layers if they exist
    const Trk::LayerArray* layerArray = tvol.confinedLayers();
    if (layerArray) {
       // display output
       const std::vector<const Trk::Layer*>& layers = layerArray->arrayObjects();
       auto layIter  = layers.begin();
       auto layIterE = layers.end();    
       for ( ; layIter != layIterE; ++layIter){
           if ( (*layIter)->layerMaterialProperties() ){
                 Trk::SurfaceIntersection lsIntersection = (*layIter)->surfaceRepresentation().straightLineIntersection(position, direction, true, true);
               if (lsIntersection.valid){
                 intersectedLayers[lsIntersection.pathLength] = std::pair<const Trk::Layer*, Amg::Vector3D>(*layIter,lsIntersection.intersection);
                 // calculate the pathCorrection
                 double stepLength = (*layIter)->surfaceRepresentation().type() == Trk::Surface::Cylinder ? fabs(1./sin(theta)) : fabs(1./cos(theta));
                 // get & record the material
                 //   - the position on the surface
                 Amg::Vector3D mposition = (*layIter)->surfaceRepresentation().transform().inverse()*lsIntersection.intersection;
                 const Trk::MaterialProperties* mprop = (*layIter)->layerMaterialProperties()->fullMaterial(mposition);
                 if (mprop)
                     collectedMaterial[lsIntersection.pathLength] = Trk::AssociatedMaterial(lsIntersection.intersection, mprop, stepLength, &tvol, (*layIter));
                 else 
                     collectedMaterial[lsIntersection.pathLength] = Trk::AssociatedMaterial(lsIntersection.intersection, &tvol, (*layIter));

                 ATH_MSG_VERBOSE(" - record material hit at (conf.) layer with index " << (*layIter)->layerIndex().value() << " - at " << Amg::toString(lsIntersection.intersection) );
                 if (mprop)
                     ATH_MSG_VERBOSE(" - MaterialProperties are " << (*mprop) );
                 else 
                     ATH_MSG_VERBOSE(" - No MaterialProperties found." );                     
               }
           }
       }
    }
    
    // provide the material to the material mapper 
    auto cmIter  = collectedMaterial.begin();
    auto cmIterE = collectedMaterial.end();
    for ( ; cmIter != cmIterE; ++cmIter )
        m_materialMapper->recordMaterialHit(cmIter->second, cmIter->second.materialPosition());
    
    // update the position to the last one
    Amg::Vector3D lastPosition = intersectedLayers.size() ? (*(--(intersectedLayers.end()))).second.second : position;
    // exit the volume
    std::map<double, std::pair<const Trk::TrackingVolume*, Amg::Vector3D> > volumeExits;
    // now find the exit point
    const std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > >& bSurfaces = tvol.boundarySurfaces();
    auto bSurfIter  = bSurfaces.begin();
    auto bSurfIterE = bSurfaces.end();
    for ( ; bSurfIter != bSurfIterE; ++bSurfIter){
        // omit positions on the surface
        if (  !(*bSurfIter)->surfaceRepresentation().isOnSurface(lastPosition, true, 0.1, 0.1)  ){
            Trk::SurfaceIntersection evIntersection = (*bSurfIter)->surfaceRepresentation().straightLineIntersection(lastPosition, direction, true, true);
            ATH_MSG_DEBUG(" - boundary surface intersection / validity :" << Amg::toString(evIntersection.intersection) << " / " << evIntersection.valid);
	        ATH_MSG_DEBUG(" - with path length = " << evIntersection.pathLength );
            if (evIntersection.valid){
                // next attached Tracking Volume
                const Trk::TrackingVolume* naVolume = (*bSurfIter)->attachedVolume(evIntersection.intersection, direction, Trk::alongMomentum);
                // put it into the map
                volumeExits[evIntersection.pathLength] = std::pair<const Trk::TrackingVolume*, Amg::Vector3D>(naVolume,evIntersection.intersection);
                // volume exit
                ATH_MSG_DEBUG(" - found volume exit - at " << Amg::toString(evIntersection.intersection) );
          }
        } else {
            ATH_MSG_DEBUG(" - starting position is on surface ! " );
        }
    }
    
    if (volumeExits.size()){
        // get the first entry in the map: closest next volume
        const Trk::TrackingVolume* cnVolume = (*volumeExits.begin()).second.first;
        if (cnVolume != &tvol) {
            if (cnVolume) 
                ATH_MSG_VERBOSE("Next Volume: " << cnVolume->volumeName() << " - at " << Amg::toString((*volumeExits.begin()).second.second) );
            // record that
            m_materialMapper->record((*volumeExits.begin()).second.second);
            // return for further navigation
            return Trk::PositionAtBoundary((*volumeExits.begin()).second.second ,cnVolume);
        }
    } else{
        ATH_MSG_DEBUG( "No exit found from Volume '" <<  tvol.volumeName() << "' - starting radius = " << lastPosition.perp() );
        const Trk::CylinderVolumeBounds* cvb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(tvol.volumeBounds()));
        if (cvb) 
            ATH_MSG_DEBUG( "        Volume outer radius = " << cvb->outerRadius() );
    }
    
    return Trk::PositionAtBoundary(position, 0);
                                                                            
}
                                          


StatusCode Trk::MaterialValidation::finalize()
{
    ATH_MSG_INFO( "MaterialValidation finalize()" );
    return StatusCode::SUCCESS;
}

StatusCode Trk::MaterialValidation::retrieveTrackingGeometry()
{

    // Retrieve the TrackingGeometry from the DetectorStore
    if ((detStore()->retrieve(m_trackingGeometry, m_trackingGeometrySvc->trackingGeometryName())).isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve TrackingGeometry from DetectorStore!" );
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}
