/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometryBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTools/GeometryBuilder.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeBuilder.h"
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"

#ifdef TRKDETDESCR_MEMUSAGE                            
#include <unistd.h>
#endif

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// STD
#include <map>
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"


// constructor
Trk::GeometryBuilder::GeometryBuilder(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  TrackingVolumeManipulator(),
#ifdef TRKDETDESCR_MEMUSAGE      
  m_memoryLogger(),
#endif 
  m_createWorld(true),
  m_navigationLevel(2),
  m_worldDimension(),
  m_worldMaterialProperties(),
  m_trackingVolumeArrayCreator("Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"),
  m_trackingVolumeHelper("Trk::TrackingVolumeHelper/TrackingVolumeHelper"),
  m_inDetGeometryBuilder(""),
  m_hgtdGeometryBuilder(""),
  m_caloGeometryBuilder(""),
  m_muonGeometryBuilder("")  ,
  m_compactify(true),
  m_synchronizeLayers(true)
{
    declareInterface<IGeometryBuilder>(this);
    // by hand declarations
    declareProperty("CreateWorldManually",                  m_createWorld);
    declareProperty("NavigationLevel",                      m_navigationLevel);
    // (1) dimension & material
    declareProperty("WorldDimension",                       m_worldDimension);
    declareProperty("WorldMaterialProperties",              m_worldMaterialProperties);
    // tool declarations ----------------------------------------------------------------
    declareProperty("TrackingVolumeArrayCreator",           m_trackingVolumeArrayCreator);
    declareProperty("TrackingVolumeHelper",                 m_trackingVolumeHelper);
    declareProperty("InDetTrackingGeometryBuilder",         m_inDetGeometryBuilder);
    declareProperty("HGTD_TrackingGeometryBuilder",         m_hgtdGeometryBuilder);
    declareProperty("CaloTrackingGeometryBuilder",          m_caloGeometryBuilder);
    declareProperty("MuonTrackingGeometryBuilder",          m_muonGeometryBuilder);
    // optimize layer dimension & memory usage -------------------------------
    declareProperty("Compactify",                           m_compactify );
    declareProperty("SynchronizeLayers",                    m_synchronizeLayers );
}

// destructor
Trk::GeometryBuilder::~GeometryBuilder()
{}


// Athena standard methods
// initialize
StatusCode Trk::GeometryBuilder::initialize()
{

    // Retrieve the volume array creator  ----------------------------------------------------
    if (m_trackingVolumeArrayCreator.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_trackingVolumeArrayCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeArrayCreator );

    // Retrieve the tracking volume helper  --------------------------------------------------    
    if (m_trackingVolumeHelper.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_trackingVolumeHelper );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeHelper );

    // Geometries =============================================================================
    // (I) Inner Detector ---------------------------------------------------------------------
    if (!m_inDetGeometryBuilder.empty()) {
        if (m_inDetGeometryBuilder.retrieve().isFailure()) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_inDetGeometryBuilder );
            return StatusCode::FAILURE;
        } else
            ATH_MSG_INFO( "Retrieved tool " << m_inDetGeometryBuilder );
    }
    // (H) High Granularity Timing Detector ----------------------------------------------------
    if(!m_hgtdGeometryBuilder.empty()) {
        if (m_hgtdGeometryBuilder.retrieve().isFailure()) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_hgtdGeometryBuilder );
            return StatusCode::FAILURE;
        } else
            ATH_MSG_INFO( "Retrieved tool " << m_hgtdGeometryBuilder );
    }    
    // (C) Calorimeter --------------------------------------------------------------------------
    if (!m_caloGeometryBuilder.empty()) {
        if (m_caloGeometryBuilder.retrieve().isFailure()) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_caloGeometryBuilder );
            return StatusCode::FAILURE;
        } else
            ATH_MSG_INFO( "Retrieved tool " << m_caloGeometryBuilder );
    }
    // (M) Muon System -------------------------------------------------------------------------
    if (!m_muonGeometryBuilder.empty()) {
        if (m_muonGeometryBuilder.retrieve().isFailure()) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_muonGeometryBuilder );
            return StatusCode::FAILURE;
        } else
            ATH_MSG_INFO( "Retrieved tool " << m_muonGeometryBuilder );
    }

    // if no world dimensions are declared, take default ones
    if (!m_worldDimension.size())
        m_worldDimension = std::vector<double>{0.*Gaudi::Units::meter, 10.*Gaudi::Units::meter, 15.*Gaudi::Units::meter};

    // if no world materials are declared, take default ones - set vacuum 
    if (m_worldMaterialProperties.size() < 5) 
        m_worldMaterialProperties = std::vector<double>{10.e10,10.e10,0., 0., 0.};

    m_worldMaterial = Trk::Material(m_worldMaterialProperties[0],
				    m_worldMaterialProperties[1],
				    m_worldMaterialProperties[2],
				    m_worldMaterialProperties[3],
				    m_worldMaterialProperties[4]);

    ATH_MSG_INFO( " initialize() successful" );

    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::GeometryBuilder::finalize()
{
    ATH_MSG_INFO( "finalize() successful." );
    return StatusCode::SUCCESS;
}


const Trk::TrackingGeometry* Trk::GeometryBuilder::trackingGeometry(const Trk::TrackingVolume*) const
{

    // the geometry to be constructed
    const Trk::TrackingGeometry* tGeometry = 0;
    if ( m_inDetGeometryBuilder.empty() && m_hgtdGeometryBuilder.empty() && m_caloGeometryBuilder.empty() && m_muonGeometryBuilder.empty() ) {

        ATH_MSG_VERBOSE( "Configured to only create world TrackingVolume." );

        Trk::VolumeBounds* worldBounds = new Trk::CylinderVolumeBounds(m_worldDimension[0],
                                                                       m_worldDimension[1],
                                                                       m_worldDimension[2]);

        Trk::TrackingVolume* worldVolume = new Trk::TrackingVolume(0,
                                                                   worldBounds,
                                                                   m_worldMaterial,
                                                                   (const LayerArray*) 0,
                                                                   (const TrackingVolumeArray*) 0,
                                                                   "EmptyWorldVolume");

        // create a new geometry
        tGeometry = new Trk::TrackingGeometry(worldVolume);
    } else
        tGeometry = atlasTrackingGeometry();
    // sign it before you return anything
    tGeometry->sign(geometrySignature());
    return tGeometry;

}


const Trk::TrackingGeometry* Trk::GeometryBuilder::atlasTrackingGeometry() const
{
    // the return geometry
    const Trk::TrackingGeometry* atlasTrackingGeometry = 0;

    // A ------------- INNER DETECTOR SECTION --------------------------------------------------------------------------------
    // get the Inner Detector and/or HGTD and/or Calorimeter trackingGeometry
    const Trk::TrackingGeometry* inDetTrackingGeometry  = 0;
    const Trk::TrackingGeometry* hgtdTrackingGeometry   = 0;
    const Trk::TrackingGeometry* caloTrackingGeometry   = 0;

    // the volumes to be given to higher level tracking geometry builders
    const Trk::TrackingVolume* inDetVolume    = 0;
    const Trk::TrackingVolume* hgtdVolume     = 0;
    const Trk::TrackingVolume* caloVolume     = 0;

    // mark the highest volume
    const Trk::TrackingVolume* highestVolume  = 0;

#ifdef TRKDETDESCR_MEMUSAGE       
    m_memoryLogger.refresh(getpid());
    ATH_MSG_INFO( "[ memory usage ] Start of TrackingGeometry building: "  );    
    ATH_MSG_INFO( m_memoryLogger );                     
#endif  

    // ========================== INNER DETECTOR PART =================================================
    if (!m_inDetGeometryBuilder.empty()) {
        // debug output
        ATH_MSG_VERBOSE( "ID Tracking Geometry is going to be built." );
        // build the geometry
        inDetTrackingGeometry = m_inDetGeometryBuilder->trackingGeometry();
        // check
        if (inDetTrackingGeometry) {
            // sign it
            inDetTrackingGeometry->sign(m_inDetGeometryBuilder->geometrySignature());
            // check whether the world has to be created or not
            if (m_createWorld || m_hgtdGeometry || m_caloGeometry || m_muonGeometry) {
                // checkout the highest InDet volume
                inDetVolume = inDetTrackingGeometry->checkoutHighestTrackingVolume();
                // assign it as the highest volume
                highestVolume = inDetVolume;
                // cleanup
                delete inDetTrackingGeometry;
            } else // -> Take the exit and return ID stand alone
                atlasTrackingGeometry = inDetTrackingGeometry;
        }

#ifdef TRKDETDESCR_MEMUSAGE            
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] After InDet TrackingGeometry building: "  );
        ATH_MSG_INFO( m_memoryLogger );
#endif

    }
    
    // ========================== HGTD PART =================================================
    // if a HGTD Geometry Builder is present -> wrap it around the ID
    if (!m_hgtdGeometryBuilder.empty()) {
        if (inDetVolume)
            ATH_MSG_VERBOSE( "HGTD Tracking Geometry is going to be built with enclosed ID." );
        else 
            ATH_MSG_VERBOSE( "HGTD Tracking Geometry is going to be built stand-alone." );
        // get the InnerDetector TrackingGeometry
        hgtdTrackingGeometry = m_hgtdGeometryBuilder->trackingGeometry(inDetVolume);
        // if you have to create world or there is a Calo/Muon geometry builder ...
        if (hgtdTrackingGeometry) {
            // sign it
            hgtdTrackingGeometry->sign(m_hgtdGeometryBuilder->geometrySignature());
            if (m_createWorld || m_caloGeometry || m_muonGeometry){
                // check out the highest Calo volume
                hgtdVolume = hgtdTrackingGeometry->checkoutHighestTrackingVolume();
                // assign it as the highest volume (overwrite ID)
                highestVolume = hgtdVolume;
                // cleanup
                delete hgtdTrackingGeometry;
            } else // -> Take the exit and return HGTD back
                atlasTrackingGeometry = hgtdTrackingGeometry;
        }

#ifdef TRKDETDESCR_MEMUSAGE            
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] After Calo TrackingGeometry building: "  );
        ATH_MSG_INFO( m_memoryLogger );
#endif

    }

    // ========================== CALORIMETER PART =================================================
    // if a Calo Geometry Builder is present -> wrap it around the ID or HGTD
    if (!m_caloGeometryBuilder.empty()) {
        std::string enclosed = "stand-alone.";
        if (inDetVolume and hgtdVolume)
            enclosed = "with encloded ID/HGTD.";
        else if (inDetVolume or hgtdVolume)
            enclosed = (inDetVolume) ? "with encloded ID." : "with encloded HGTD.";                  
        ATH_MSG_VERBOSE( "Calorimeter Tracking Geometry is going to be built "<< enclosed );
        // get the InnerDetector TrackingGeometry
        if (inDetVolume and not hgtdVolume)
          caloTrackingGeometry = m_caloGeometryBuilder->trackingGeometry(inDetVolume);
        else 
          caloTrackingGeometry = m_caloGeometryBuilder->trackingGeometry(hgtdVolume);
        // if you have to create world or there is a Muon geometry builder ...
        if (caloTrackingGeometry) {
            // sign it
            caloTrackingGeometry->sign(m_caloGeometryBuilder->geometrySignature());
            if (m_createWorld || m_muonGeometry){
                // check out the highest Calo volume
                caloVolume = caloTrackingGeometry->checkoutHighestTrackingVolume();
                // assign it as the highest volume (overwrite ID)
                highestVolume = caloVolume;
                // cleanup
                delete caloTrackingGeometry;
            } else // -> Take the exit and return Calo back
                atlasTrackingGeometry = caloTrackingGeometry;
        }

#ifdef TRKDETDESCR_MEMUSAGE            
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] After Calo TrackingGeometry building: "  );
        ATH_MSG_INFO( m_memoryLogger );
#endif

    }

    // ========================== MUON SYSTEM PART =================================================
    // if Muon Geometry Builder is present -> wrap either ID or Calo
    if (!m_muonGeometryBuilder.empty()) {
        std::string enclosed = "stand-alone.";
        if (inDetVolume and hgtdVolume and caloVolume )
            enclosed = "with encloded ID/HGTD/Calo.";
        else if (inDetVolume or hgtdVolume or caloVolume) {
            if (inDetVolume) {
              if (hgtdVolume)
                enclosed = "with encloded ID/HGTD";
              else if (caloVolume)
                enclosed = "with encloded ID/Calo";
              else
                enclosed = "with encloded ID";
            } else if (hgtdVolume) {
              if (caloVolume)
                enclosed = "with encloded HGTD/Calo";
              else
                enclosed = "with encloded HGTD";
            } else {
              enclosed = "with encloded Calo";
            }
        }
        ATH_MSG_VERBOSE( "Muon System Tracking Geometry is going to be built "<< enclosed );
        // there's nothing outside the muons -- wrap the calo or the HGTD if one or both of them exist
        if (inDetVolume and not hgtdVolume and not caloVolume)
            atlasTrackingGeometry = m_muonGeometryBuilder->trackingGeometry( inDetVolume );
        if (hgtdVolume and not caloVolume)
            atlasTrackingGeometry = m_muonGeometryBuilder->trackingGeometry( hgtdVolume );
        else
            atlasTrackingGeometry = m_muonGeometryBuilder->trackingGeometry( caloVolume );

        // sign it
        if (atlasTrackingGeometry)
            atlasTrackingGeometry->sign(m_muonGeometryBuilder->geometrySignature());

#ifdef TRKDETDESCR_MEMUSAGE            
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] After Muon TrackingGeometry building: "  );
        ATH_MSG_INFO( m_memoryLogger );
#endif

        // ========================== WRAPPING SECTION FOR ID/CALO ====================================
    } else if (m_createWorld && highestVolume) {
        // wrapping and world creation has been switched on
        ATH_MSG_VERBOSE( "Enclosing world is going to be built for: " << highestVolume->volumeName() );
        // get the glue volumes
        const Trk::GlueVolumesDescriptor& innerGlueVolumes = highestVolume->glueVolumesDescriptor();
        // some screen output
        ATH_MSG_VERBOSE( "Retrieved with following glue volumes: " << innerGlueVolumes );
        // at negative face
        const std::vector<const Trk::TrackingVolume*>& innerNegativeFaceVolumes = innerGlueVolumes.glueVolumes(Trk::negativeFaceXY);
        // at cylinder cover
        const std::vector<const Trk::TrackingVolume*>& innerCentralFaceVolumes = innerGlueVolumes.glueVolumes(Trk::cylinderCover);
        // at positive face
        const std::vector<const Trk::TrackingVolume*>& innerPositiveFaceVolumes = innerGlueVolumes.glueVolumes(Trk::positiveFaceXY);

        // get the dimensions
        // cast them to CylinderVolumeBounds
        const Trk::CylinderVolumeBounds* innerVolumeBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(highestVolume->volumeBounds()));
        if (!innerVolumeBounds) ATH_MSG_WARNING( "atlasTrackingGeometry() ... dynamic cast to innerVolumeBounds failed!" );
        double innerVolumeOuterR      = innerVolumeBounds ? innerVolumeBounds->outerRadius() : 0;
        double innerVolumeHalflengthZ = innerVolumeBounds ? innerVolumeBounds->halflengthZ() : 0;
        // Hierarchy after enclosing
        //
        // AtlasWorldVolume:
        //    AtlasInnerCylinder
        //       AtlasInnerNegativeSector
        //       InnerEnclosedVolume (can be ID/Calo)
        //       AtlasOuterNegativeSector
        //    AtlasOuterTube
        // B -------------- BUILD WORLD AROUND for ID stand alone applications

        double innerCylinderSectorHalflengthZ = 0.5*(m_worldDimension[2] - innerVolumeHalflengthZ);
        Trk::CylinderVolumeBounds* innerCylinderSectorBounds =
                new Trk::CylinderVolumeBounds(0., innerVolumeOuterR, innerCylinderSectorHalflengthZ);

        double innerCylinderSectorPositionZ = fabs(m_worldDimension[2]-innerCylinderSectorHalflengthZ);

        // the AtlasInnerNegativeSector
        Amg::Transform3D* atlasInnerNegativeSectorTransf = new Amg::Transform3D;
                        (*atlasInnerNegativeSectorTransf) = Amg::Translation3D(0.,0.,-innerCylinderSectorPositionZ);
        Trk::TrackingVolume* atlasInnerNegativeSector = new Trk::TrackingVolume(
                               atlasInnerNegativeSectorTransf,
                               innerCylinderSectorBounds,
                               m_worldMaterial,
                               (const LayerArray*) 0,
                               (const TrackingVolumeArray*) 0,
                               "AtlasInnerNegativeSector");

        // the AtlasInnerPositiveSector
        Amg::Transform3D* atlasInnerPositiveSectorTransf = new Amg::Transform3D;
                        (*atlasInnerPositiveSectorTransf) = Amg::Translation3D(0.,0.,innerCylinderSectorPositionZ);
        Trk::TrackingVolume* atlasInnerPositiveSector = new Trk::TrackingVolume(
                               atlasInnerPositiveSectorTransf,
                               innerCylinderSectorBounds->clone(),
                               m_worldMaterial,
                               (const LayerArray*) 0,
                               (const TrackingVolumeArray*) 0,
                               "AtlasInnerPositiveSector");

        ATH_MSG_VERBOSE( "Inner Negative/Positive Sectors built successfully." );

        // create the subvolume Array
        auto atlasInnerSectorVolumes = std::vector<const Trk::TrackingVolume*>{atlasInnerNegativeSector,highestVolume,atlasInnerPositiveSector}; 

        ATH_MSG_VERBOSE( "Create the Atlas Inner Sector volumes. " );
        Trk::BinnedArray<Trk::TrackingVolume>* atlasInnerSectorVolumeArray = m_trackingVolumeArrayCreator ?
                m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(atlasInnerSectorVolumes) : 0;


        // Atlas inner Sector bounds
        Trk::CylinderVolumeBounds* innerSectorBounds =
                new Trk::CylinderVolumeBounds(0., innerVolumeOuterR, m_worldDimension[2]);
        // build the Tracking volumes
        Trk::TrackingVolume* atlasInnerSector = new Trk::TrackingVolume(0,
                                                                        innerSectorBounds,
                                                                        m_worldMaterial,
                                                                        0,atlasInnerSectorVolumeArray,
                                                                        "AtlasInnerSector");

        // Atlas outer Sector
        Trk::CylinderVolumeBounds* outerSectorBounds =
                new Trk::CylinderVolumeBounds(innerVolumeOuterR, m_worldDimension[1], m_worldDimension[2]);
        Trk::TrackingVolume* atlasOuterSector = new Trk::TrackingVolume(0,
                                                                        outerSectorBounds,
                                                                        m_worldMaterial,
                                                                        (const LayerArray*) 0,
                                                                        (const TrackingVolumeArray*) 0,
                                                                        "AtlasOuterSector");

        ATH_MSG_VERBOSE( "Atlas Inner/Outer Sectors built successfully." );

        // create the array of Inner and Outer sector
        auto atlasVolumes =  std::vector<const Trk::TrackingVolume*>{atlasInnerSector, atlasOuterSector};

        Trk::BinnedArray<Trk::TrackingVolume>* atlasVolumeArray = m_trackingVolumeArrayCreator ?
                m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(atlasVolumes) : 0;

        // create the Atlas volume bounds
        Trk::CylinderVolumeBounds* atlasBounds = new Trk::CylinderVolumeBounds(0., m_worldDimension[1], m_worldDimension[2]);

        // create the Atlas TrackingVolume
        Trk::TrackingVolume* atlasVolume = new Trk::TrackingVolume(0,
                                                                   atlasBounds,
                                                                   m_worldMaterial,
                                                                   0,atlasVolumeArray,
                                                                   "Atlas");

        ATH_MSG_VERBOSE( "Atlas Tracking World volume built successfully." );


        // glue the inner sector to be complete
        m_trackingVolumeHelper->glueTrackingVolumes( *atlasInnerNegativeSector, Trk::positiveFaceXY,
                                                      innerNegativeFaceVolumes, Trk::negativeFaceXY );

        m_trackingVolumeHelper->glueTrackingVolumes( *atlasInnerPositiveSector, Trk::negativeFaceXY,
                                                      innerPositiveFaceVolumes, Trk::positiveFaceXY );

        ATH_MSG_VERBOSE( "Atlas Inner Sector glued successfully together." );

        // iterators to the face volumes
        auto volIter    = innerCentralFaceVolumes.begin();
        auto volIterEnd = innerCentralFaceVolumes.end();

        // glue outer and inner sector together
        std::vector<const Trk::TrackingVolume*> atlasInnerOuterVolumes;
        atlasInnerOuterVolumes.push_back(atlasInnerNegativeSector);
        for ( ; volIter != volIterEnd; ++volIter)
            if (*volIter) atlasInnerOuterVolumes.push_back(*volIter);
        atlasInnerOuterVolumes.push_back(atlasInnerPositiveSector);

        m_trackingVolumeHelper->glueTrackingVolumes(*atlasOuterSector, Trk::tubeInnerCover,
                                                     atlasInnerOuterVolumes, Trk::tubeOuterCover);

        ATH_MSG_VERBOSE( "Atlas Inner/Outer Sector glued successfully together." );

        // job done -> create the TrackingGeometry
        atlasTrackingGeometry = new Trk::TrackingGeometry(atlasVolume);
        
        // detailed information about this tracking geometry
        ATH_MSG_VERBOSE( "Atlas TrackingGeometry built with following parameters : ");
        //ATH_MSG_VERBOSE( " - TrackingVolume containers            : " << atlasTrackingGeometry->numberOfContainerVolumes() );
        //ATH_MSG_VERBOSE( " - TrackingVolume at navigation level   : " << atlasTrackingGeometry->numberOfContainerVolumes() );
        //ATH_MSG_VERBOSE( " - Contained static layers              : " << atlasTrackingGeometry->boundaryLayers().size());        
        ATH_MSG_VERBOSE( " - Unique material layers on boundaries : " << atlasTrackingGeometry->boundaryLayers().size());        

#ifdef TRKDETDESCR_MEMUSAGE            
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] After Outer Sector TrackingGeometry building: "  );
        ATH_MSG_INFO( m_memoryLogger );
#endif

    }

    if (atlasTrackingGeometry) {
        if (m_navigationLevel < 3)
            atlasTrackingGeometry->registerNavigationLevel( Trk::NavigationLevel(m_navigationLevel));
    }
    else ATH_MSG_WARNING( "atlasTrackingGeometry() ... atlasTrackingGeometry = 0, could not call registerNavigationLevel and propagateMagneticFieldProperties" );

#ifdef TRKDETDESCR_MEMUSAGE            
    m_memoryLogger.refresh(getpid());
    ATH_MSG_INFO( "[ memory usage ] End of TrackingGeometry building: "  );    
    ATH_MSG_INFO( m_memoryLogger );                     
#endif

    // synchronize the layers
    if (atlasTrackingGeometry) {
    if (m_synchronizeLayers) atlasTrackingGeometry->synchronizeLayers(msg());

    // compactify if configured to do so
    if (m_compactify) atlasTrackingGeometry->compactify(msg());
    }
    return atlasTrackingGeometry;
} 
