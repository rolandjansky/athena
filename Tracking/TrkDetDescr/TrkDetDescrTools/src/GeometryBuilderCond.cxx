/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometryBuilderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTools/GeometryBuilderCond.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeBuilder.h"
#include "TrkDetDescrInterfaces/ILayerBuilderCond.h"
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
//Athena
#include "AthenaKernel/IOVInfiniteRange.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"


// constructor
Trk::GeometryBuilderCond::GeometryBuilderCond(const std::string& t, const std::string& n, const IInterface* p)
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
  m_inDetGeometryBuilderCond("", this),
  m_caloGeometry{},
  m_caloGeometryBuilderCond("", this),
  m_hgtdGeometry{},
  m_hgtdGeometryBuilderCond("", this),
  m_muonGeometry{},
  m_muonGeometryBuilderCond("", this),
  m_compactify(true),
  m_synchronizeLayers(true)
{
    declareInterface<IGeometryBuilderCond>(this);
    // by hand declarations
    declareProperty("CreateWorldManually",                  m_createWorld);
    declareProperty("NavigationLevel",                      m_navigationLevel);
    // (1) dimension & material
    declareProperty("WorldDimension",                       m_worldDimension);
    declareProperty("WorldMaterialProperties",              m_worldMaterialProperties);
    // tool declarations ----------------------------------------------------------------
    declareProperty("TrackingVolumeArrayCreator",           m_trackingVolumeArrayCreator);
    declareProperty("TrackingVolumeHelper",                 m_trackingVolumeHelper);
    declareProperty("InDetTrackingGeometryBuilder",         m_inDetGeometryBuilderCond);
    declareProperty("HGTD_TrackingGeometryBuilder",         m_hgtdGeometryBuilderCond);
    declareProperty("CaloTrackingGeometryBuilder",          m_caloGeometryBuilderCond);
    declareProperty("MuonTrackingGeometryBuilder",          m_muonGeometryBuilderCond);
    // optimize layer dimension & memory usage -------------------------------
    declareProperty("Compactify",                           m_compactify );
    declareProperty("SynchronizeLayers",                    m_synchronizeLayers );
}

// destructor
Trk::GeometryBuilderCond::~GeometryBuilderCond()
{}


// Athena standard methods
// initialize
StatusCode Trk::GeometryBuilderCond::initialize()
{

    // Retrieve the volume array creator  ----------------------------------------------------
    ATH_CHECK(m_trackingVolumeArrayCreator.retrieve()); 

    // Retrieve the tracking volume helper  --------------------------------------------------    
    ATH_CHECK (m_trackingVolumeHelper.retrieve()); 
    // Geometries =============================================================================
    // (I) Inner Detector ---------------------------------------------------------------------
    if (!m_inDetGeometryBuilderCond.empty()) {
        ATH_CHECK(m_inDetGeometryBuilderCond.retrieve());
    }
    // (H) High Granularity Timing Detector ----------------------------------------------------
    if(!m_hgtdGeometryBuilderCond.empty()) {
        if (m_hgtdGeometryBuilderCond.retrieve().isFailure()) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_hgtdGeometryBuilderCond );
            return StatusCode::FAILURE;
        } else
            ATH_MSG_INFO( "Retrieved tool " << m_hgtdGeometryBuilderCond );
    }    
    // (C) Calorimeter --------------------------------------------------------------------------
    if (!m_caloGeometryBuilderCond.empty()) {
        ATH_CHECK (m_caloGeometryBuilderCond.retrieve());
    }        
    // (M) Muon System -------------------------------------------------------------------------
    if (!m_muonGeometryBuilderCond.empty()) {
        ATH_CHECK(m_muonGeometryBuilderCond.retrieve());
    }

    // if no world dimensions are declared, take default ones
    if (m_worldDimension.empty())
        m_worldDimension = std::vector<double>{0.*Gaudi::Units::meter, 10.*Gaudi::Units::meter, 15.*Gaudi::Units::meter};

    // if no world materials are declared, take default ones - set vacuum 
    if (m_worldMaterialProperties.size() < 5) 
        m_worldMaterialProperties = std::vector<double>{10.e10,10.e10,0., 0., 0.};

    m_worldMaterial = Trk::Material(m_worldMaterialProperties[0],
				    m_worldMaterialProperties[1],
				    m_worldMaterialProperties[2],
				    m_worldMaterialProperties[3],
				    m_worldMaterialProperties[4]);

    ATH_MSG_DEBUG( " initialize() successful" );

    return StatusCode::SUCCESS;
}

std::pair<EventIDRange, Trk::TrackingGeometry*>
Trk::GeometryBuilderCond::trackingGeometry(const EventContext& ctx,
                                           std::pair<EventIDRange, 
                                           const Trk::TrackingVolume*> /*tVolPair*/) const
{

    // the geometry to be constructed
    std::pair<EventIDRange, Trk::TrackingGeometry*> tGeometry;
    if ( m_inDetGeometryBuilderCond.empty() && m_hgtdGeometryBuilderCond.empty() && m_caloGeometryBuilderCond.empty() && m_muonGeometryBuilderCond.empty() ) {

        ATH_MSG_VERBOSE( "Configured to only create world TrackingVolume." );

        Trk::VolumeBounds* worldBounds = new Trk::CylinderVolumeBounds(m_worldDimension[0],
                                                                       m_worldDimension[1],
                                                                       m_worldDimension[2]);

        Trk::TrackingVolume* worldVolume = new Trk::TrackingVolume(nullptr,
                                                                   worldBounds,
                                                                   m_worldMaterial,
                                                                   nullptr,
                                                                   nullptr,
                                                                   "EmptyWorldVolume");
        //dummy infinite IOV range
        EventIDRange range=IOVInfiniteRange::infiniteMixed();

        // create a new geometry
        tGeometry = std::make_pair(range, new Trk::TrackingGeometry(worldVolume));
    } else
        tGeometry = atlasTrackingGeometry(ctx);
    // sign it before you return anything
    tGeometry.second->sign(geometrySignature());
    return tGeometry;

}


std::pair<EventIDRange, Trk::TrackingGeometry*> 
Trk::GeometryBuilderCond::atlasTrackingGeometry(const EventContext& ctx) const
{
    // the return geometry
    std::pair<EventIDRange, Trk::TrackingGeometry*> atlasTrackingGeometry;
    //Set IOV range covering 0 - inf
    EventIDRange range=IOVInfiniteRange::infiniteMixed();

    // A ------------- INNER DETECTOR SECTION --------------------------------------------------------------------------------
    // get the Inner Detector and/or HGTD and/or Calorimeter trackingGeometry
    std::pair<EventIDRange, Trk::TrackingGeometry*> inDetTrackingGeometry;
    std::pair<EventIDRange, Trk::TrackingGeometry*> hgtdTrackingGeometry;
    std::pair<EventIDRange, Trk::TrackingGeometry*> caloTrackingGeometry ;

    // the volumes to be given to higher level tracking geometry builders
    const Trk::TrackingVolume* inDetVolume    = nullptr;
    const Trk::TrackingVolume* hgtdVolume     = nullptr;
    const Trk::TrackingVolume* caloVolume     = nullptr;

    // mark the highest volume
    const Trk::TrackingVolume* highestVolume  = nullptr;

#ifdef TRKDETDESCR_MEMUSAGE       
    m_memoryLogger.refresh(getpid());
    ATH_MSG_INFO( "[ memory usage ] Start of TrackingGeometry building: "  );    
    ATH_MSG_INFO( m_memoryLogger );                     
#endif  

    // ========================== INNER DETECTOR PART =================================================
    if (!m_inDetGeometryBuilderCond.empty()) {
        // debug output
        ATH_MSG_VERBOSE( "ID Tracking Geometry is going to be built." );
        // build the geometry
        inDetTrackingGeometry = m_inDetGeometryBuilderCond->trackingGeometry(ctx, std::pair<EventIDRange, const Trk::TrackingVolume*>(range, nullptr));
        // check
        if (inDetTrackingGeometry.second) {
            // sign it
            inDetTrackingGeometry.second->sign(m_inDetGeometryBuilderCond->geometrySignature());
            // check whether the world has to be created or not
            if (m_createWorld || m_hgtdGeometry || m_caloGeometry || m_muonGeometry) {
                // checkout the highest InDet volume
                inDetVolume = inDetTrackingGeometry.second->checkoutHighestTrackingVolume();
                // assign it as the highest volume
                highestVolume = inDetVolume;
                range = inDetTrackingGeometry.first;
                // cleanup
                delete inDetTrackingGeometry.second;
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
    if (!m_hgtdGeometryBuilderCond.empty()) {
        if (inDetVolume)
            ATH_MSG_VERBOSE( "HGTD Tracking Geometry is going to be built with enclosed ID." );
        else 
            ATH_MSG_VERBOSE( "HGTD Tracking Geometry is going to be built stand-alone." );
        // get the InnerDetector TrackingGeometry
        hgtdTrackingGeometry = m_hgtdGeometryBuilderCond->trackingGeometry(ctx, std::make_pair(inDetTrackingGeometry.first, inDetVolume));
        // if you have to create world or there is a Calo/Muon geometry builder ...
        if (hgtdTrackingGeometry.second) {
            // sign it
            hgtdTrackingGeometry.second->sign(m_hgtdGeometryBuilderCond->geometrySignature());
            if (m_createWorld || m_caloGeometry || m_muonGeometry){
                // check out the highest Calo volume
                hgtdVolume = hgtdTrackingGeometry.second->checkoutHighestTrackingVolume();
                // assign it as the highest volume (overwrite ID)
                highestVolume = hgtdVolume;
                range = hgtdTrackingGeometry.first;
                // cleanup
                delete hgtdTrackingGeometry.second;
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
    if (!m_caloGeometryBuilderCond.empty()) {
        std::string enclosed = "stand-alone.";
        if (inDetVolume and hgtdVolume)
            enclosed = "with encloded ID/HGTD.";
        else if (inDetVolume or hgtdVolume)
            enclosed = (inDetVolume) ? "with encloded ID." : "with encloded HGTD.";                  
        ATH_MSG_VERBOSE( "Calorimeter Tracking Geometry is going to be built "<< enclosed );

        // get the InnerDetector TrackingGeometry or the HGTD tracking geometry
        if (inDetVolume and not hgtdVolume)
          caloTrackingGeometry = m_caloGeometryBuilderCond->trackingGeometry(ctx, std::make_pair(inDetTrackingGeometry.first, inDetVolume));
        else 
          caloTrackingGeometry = m_caloGeometryBuilderCond->trackingGeometry(ctx, std::make_pair(hgtdTrackingGeometry.first, hgtdVolume));
        // if you have to create world or there is a Muon geometry builder ...
        if (caloTrackingGeometry.second) {
            // sign it
            caloTrackingGeometry.second->sign(m_caloGeometryBuilderCond->geometrySignature());
            if (m_createWorld || m_muonGeometry){
                // check out the highest Calo volume
                caloVolume = caloTrackingGeometry.second->checkoutHighestTrackingVolume();
                // assign it as the highest volume (overwrite ID)
                highestVolume = caloVolume;
                range = caloTrackingGeometry.first;
                // cleanup
                delete caloTrackingGeometry.second;
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
    if (!m_muonGeometryBuilderCond.empty()) {
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
            atlasTrackingGeometry = m_muonGeometryBuilderCond->trackingGeometry(ctx, std::make_pair(inDetTrackingGeometry.first, inDetVolume));
        else if (hgtdVolume and not caloVolume)
            atlasTrackingGeometry = m_muonGeometryBuilderCond->trackingGeometry(ctx, std::make_pair(hgtdTrackingGeometry.first, hgtdVolume));        
        else
            atlasTrackingGeometry = m_muonGeometryBuilderCond->trackingGeometry(ctx, std::make_pair(caloTrackingGeometry.first, caloVolume));

        // sign it
        if (atlasTrackingGeometry.second)
            atlasTrackingGeometry.second->sign(m_muonGeometryBuilderCond->geometrySignature());

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
                               nullptr,
                               nullptr,
                               "AtlasInnerNegativeSector");

        // the AtlasInnerPositiveSector
        Amg::Transform3D* atlasInnerPositiveSectorTransf = new Amg::Transform3D;
                        (*atlasInnerPositiveSectorTransf) = Amg::Translation3D(0.,0.,innerCylinderSectorPositionZ);
        Trk::TrackingVolume* atlasInnerPositiveSector = new Trk::TrackingVolume(
                               atlasInnerPositiveSectorTransf,
                               innerCylinderSectorBounds->clone(),
                               m_worldMaterial,
                               nullptr,
                               nullptr,
                               "AtlasInnerPositiveSector");

        ATH_MSG_VERBOSE( "Inner Negative/Positive Sectors built successfully." );

        // create the subvolume Array
        auto atlasInnerSectorVolumes = std::vector<const Trk::TrackingVolume*>{atlasInnerNegativeSector,highestVolume,atlasInnerPositiveSector}; 

        ATH_MSG_VERBOSE( "Create the Atlas Inner Sector volumes. " );
        Trk::BinnedArray<const Trk::TrackingVolume>* atlasInnerSectorVolumeArray = m_trackingVolumeArrayCreator ?
                m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(atlasInnerSectorVolumes) : nullptr;


        // Atlas inner Sector bounds
        Trk::CylinderVolumeBounds* innerSectorBounds =
                new Trk::CylinderVolumeBounds(0., innerVolumeOuterR, m_worldDimension[2]);
        // build the Tracking volumes
        Trk::TrackingVolume* atlasInnerSector = new Trk::TrackingVolume(nullptr,
                                                                        innerSectorBounds,
                                                                        m_worldMaterial,
                                                                        nullptr,
                                                                        atlasInnerSectorVolumeArray,
                                                                        "AtlasInnerSector");

        // Atlas outer Sector
        Trk::CylinderVolumeBounds* outerSectorBounds =
                new Trk::CylinderVolumeBounds(innerVolumeOuterR, m_worldDimension[1], m_worldDimension[2]);
        Trk::TrackingVolume* atlasOuterSector = new Trk::TrackingVolume(nullptr,
                                                                        outerSectorBounds,
                                                                        m_worldMaterial,
                                                                        nullptr,
                                                                        nullptr,
                                                                        "AtlasOuterSector");

        ATH_MSG_VERBOSE( "Atlas Inner/Outer Sectors built successfully." );

        // create the array of Inner and Outer sector
        auto atlasVolumes =  std::vector<const Trk::TrackingVolume*>{atlasInnerSector, atlasOuterSector};

        Trk::BinnedArray<const Trk::TrackingVolume>* atlasVolumeArray = m_trackingVolumeArrayCreator ?
                m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(atlasVolumes) : nullptr;

        // create the Atlas volume bounds
        Trk::CylinderVolumeBounds* atlasBounds = new Trk::CylinderVolumeBounds(0., m_worldDimension[1], m_worldDimension[2]);

        // create the Atlas TrackingVolume
        Trk::TrackingVolume* atlasVolume = new Trk::TrackingVolume(nullptr,
                                                                   atlasBounds,
                                                                   m_worldMaterial,
                                                                   nullptr,
                                                                   atlasVolumeArray,
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
        atlasTrackingGeometry.second = new Trk::TrackingGeometry(atlasVolume);

        atlasTrackingGeometry.first = range;
        
        // detailed information about this tracking geometry
        ATH_MSG_VERBOSE( "Atlas TrackingGeometry built with following parameters : ");
        //ATH_MSG_VERBOSE( " - TrackingVolume containers            : " << atlasTrackingGeometry->numberOfContainerVolumes() );
        //ATH_MSG_VERBOSE( " - TrackingVolume at navigation level   : " << atlasTrackingGeometry->numberOfContainerVolumes() );
        //ATH_MSG_VERBOSE( " - Contained static layers              : " << atlasTrackingGeometry->boundaryLayers().size());        
        ATH_MSG_VERBOSE( " - Unique material layers on boundaries : " << atlasTrackingGeometry.second->boundaryLayers().size());        

#ifdef TRKDETDESCR_MEMUSAGE            
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] After Outer Sector TrackingGeometry building: "  );
        ATH_MSG_INFO( m_memoryLogger );
#endif

    }

    if (atlasTrackingGeometry.second) {
        if (m_navigationLevel < 3)
            atlasTrackingGeometry.second->registerNavigationLevel( Trk::NavigationLevel(m_navigationLevel));
    }
    else ATH_MSG_WARNING( "atlasTrackingGeometry() ... atlasTrackingGeometry = 0, could not call registerNavigationLevel and propagateMagneticFieldProperties" );

#ifdef TRKDETDESCR_MEMUSAGE            
    m_memoryLogger.refresh(getpid());
    ATH_MSG_INFO( "[ memory usage ] End of TrackingGeometry building: "  );    
    ATH_MSG_INFO( m_memoryLogger );                     
#endif

    // synchronize the layers
    if (atlasTrackingGeometry.second) {
    if (m_synchronizeLayers) atlasTrackingGeometry.second->synchronizeLayers(msg());

    // compactify if configured to do so
    if (m_compactify) atlasTrackingGeometry.second->compactify(msg());
    }
    return atlasTrackingGeometry;
} 
