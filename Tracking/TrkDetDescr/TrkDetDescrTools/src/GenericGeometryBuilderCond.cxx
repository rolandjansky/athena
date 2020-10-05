/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericGeometryBuilderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"
// Trk include
#include "TrkDetDescrTools/GenericGeometryBuilderCond.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/Material.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// STD
#include <map>
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"


// constructor
Trk::GenericGeometryBuilderCond::GenericGeometryBuilderCond(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_enclosingEnvelopeSvc("AtlasEnvelopeDefSvc", n),      
  m_enclosingEnvelope(0),         
  m_trackingVolumeCreator(""),
  m_geometrySignature(0),
  m_geometryName("Undefined"),
  m_geometryColorCode(8),
  m_barrelFraction(0.75),         
  m_barrelLayers(10),         
  m_endcapLayers(20),       
  m_extendedEndcap(false),            
  m_extendedEndcapLayers(5)
{
    // declare the interface
    declareInterface<Trk::IGeometryBuilderCond>(this);
    // Tools & Services
    declareProperty("EnvelopeDefinitionSvc",        m_enclosingEnvelopeSvc);      
    declareProperty("Envelope",                     m_enclosingEnvelope);         
    declareProperty("TrackingVolumeCreator",        m_trackingVolumeCreator);  
    // configuration                                
    declareProperty("GeometrySignature",            m_geometrySignature);
    declareProperty("GeometryName",                 m_geometryName);
    declareProperty("GeometryColorCode",            m_geometryColorCode);
    // Barrel Layer configuration                   
    declareProperty("BarrelFraction",               m_barrelFraction);              
    declareProperty("BarrelLayers",                 m_barrelLayers);              
    // Endcap Layer configuration                   
    declareProperty("EndcapLayers",                 m_endcapLayers);             
    // Extended endcap Layer configuration 
    declareProperty("ExtendedEndcap",               m_extendedEndcap);            
    declareProperty("ExtendedEndcapLayers",         m_extendedEndcapLayers);      
}

// destructor
Trk::GenericGeometryBuilderCond::~GenericGeometryBuilderCond()
{}

// Athena standard methods - initialize
StatusCode Trk::GenericGeometryBuilderCond::initialize()
{
    // Retrieve the volume array creator  ----------------------------------------------------
    if (m_enclosingEnvelopeSvc.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve EnvelopeSvc " << m_enclosingEnvelopeSvc );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO( "Retrieved " << m_enclosingEnvelopeSvc );

    // Retrieve the tracking volume creator  --------------------------------------------------    
    if (m_trackingVolumeCreator.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_trackingVolumeCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeCreator );

    ATH_MSG_INFO( " initialize() successful" );
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::GenericGeometryBuilderCond::finalize()
{
    ATH_MSG_INFO( "finalize() successful." );
    return StatusCode::SUCCESS;
}


std::pair<EventIDRange, const Trk::TrackingGeometry*> Trk::GenericGeometryBuilderCond::trackingGeometry(const EventContext& /*ctx*/, std::pair< EventIDRange, const Trk::TrackingVolume*> innerVolPair) const
{

    ATH_MSG_VERBOSE("Starting to build TrackingGeometry for GeometrySignature : " << m_geometrySignature );

    // the geometry to be constructed
    const Trk::TrackingGeometry* tGeometry = 0;
    
    double innerVolumeRadius             = 0.;
    double innerVolumeHalfZ              = 0.;
    double enclosingVolumeRadius         = 0.;
    double enclosingVolumeHalfZ          = 0.;
    double enclosingExtendedVolumeRadius = 0.;
    double enclosingExtendedVolumeHalfZ  = 0.;

    // vacuum
    Trk::Material vacuum;
    
    const std::map<int,AtlasDetDescr::AtlasRegion> trk2DetDesc{
      {Trk::ID,AtlasDetDescr::fAtlasID},
      {Trk::Calo,AtlasDetDescr::fAtlasCalo},
      {Trk::MS,AtlasDetDescr::fAtlasMS}
    };

    const auto addId=trk2DetDesc.find(m_geometrySignature);
    if (addId==trk2DetDesc.end()) {
      ATH_MSG_WARNING("No geometry signature found, return 0.");
      //dummy infinite range
      EventIDRange range;
      return std::make_pair(range,tGeometry);
    }

    const RZPairVector& envelopeDefs=m_enclosingEnvelopeSvc->getRZBoundary(addId->second);

    // ------------------------------- overall dimensions ----------------------------------------------
    // get the maximum extend in R
    for ( auto& rzIter : envelopeDefs){
        if ( rzIter.first > enclosingVolumeRadius ) {
            // maximal r-extend
            enclosingVolumeRadius = rzIter.first;
            enclosingVolumeHalfZ  = fabs(rzIter.second);            
        }
    }
    ATH_MSG_VERBOSE("       -> Overall dimensions estimated as (r,hZ) = " << enclosingVolumeRadius << ", " << enclosingVolumeHalfZ);
    
    // we need to build extended barrels ---------------------------------------------------------------
    if ( m_extendedEndcap && envelopeDefs.size() > 4){
        ATH_MSG_VERBOSE("       -> configured to build extended Barrel, checking for RZpair direction.");
        // find out the direction of the rz points point
        Amg::Vector3D p1(envelopeDefs[0].second,envelopeDefs[0].first, 0.);
        Amg::Vector3D p2(envelopeDefs[1].second,envelopeDefs[1].first, 0.);
        int clockwise = p1.cross(p2).z() < 0. ? 1 : -1;
        ATH_MSG_VERBOSE("       -> determined " << ( clockwise>0 ? "clockwise" : "anti-clockwise") << " direction from (z1,r1) = " 
            << envelopeDefs[0].second << ", " << envelopeDefs[0].first << " and " << envelopeDefs[1].second << ", " << envelopeDefs[1].first );
        // now parse for the extended barrel  
        size_t irz = 0;  
        for ( auto& rzIter : envelopeDefs){
            if (irz > 1 && rzIter.second > 0 && rzIter.first >= enclosingVolumeRadius ) {
                // maximal r-extend
                enclosingExtendedVolumeRadius = envelopeDefs[irz-clockwise*2].first;
                enclosingExtendedVolumeHalfZ  = envelopeDefs[irz-clockwise*2].second;            
            }
            ++irz;
        }
        ATH_MSG_VERBOSE("       -> Extended dimensions estimated as (r,hZ) = " << enclosingExtendedVolumeRadius << ", " << enclosingExtendedVolumeHalfZ);
            
    } else {
        ATH_MSG_WARNING("Could not parse dimensions for extended Barrel, ignoring it.");
        m_extendedEndcap = false;
    }
    // --------------------------------------------------------------------------------------------------

    //generic detector has infinite alignment validity range or if exists of input volume
    EventIDRange range;

    // get the inner radius and half length if a volume is provided
    const Trk::CylinderVolumeBounds* cvb = 0;
    const Trk::TrackingVolume* innerVol = innerVolPair.second;
    if (innerVol){
        range = innerVolPair.first;
        cvb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(innerVol->volumeBounds()));
        if (cvb){
            innerVolumeRadius = cvb->outerRadius();
            innerVolumeHalfZ  = cvb->halflengthZ();
            ATH_MSG_VERBOSE("       -> inner cylindrical volume found with (r,hZ) = " << innerVolumeRadius << ", " << innerVolumeHalfZ);
        } 
    }
    if (!cvb){
       // if no suitable inner volume is given, divide into the configured barrel fraction
       innerVolumeHalfZ = m_barrelFraction*0.5*enclosingVolumeHalfZ;
       ATH_MSG_VERBOSE("       -> no (suitable) inner volume found, setting barrel/endcap fraction as configured.");
       
    }

    // now create the barrel-type volume wrapping around the  inner volume
    const Trk::TrackingVolume* cSector = m_trackingVolumeCreator->createGapTrackingVolume(vacuum,
                                                                                         innerVolumeRadius, enclosingVolumeRadius,
                                                                                         -innerVolumeHalfZ, innerVolumeHalfZ,
                                                                                         m_barrelLayers,
                                                                                         true,
                                                                                         m_geometryName+"::Generic::Barrel");
    cSector->registerColorCode(m_geometryColorCode);
    // wrap the inner volume into a centralSector 
    if (cvb){ 
        auto centralVolumes = std::vector<const Trk::TrackingVolume*>{innerVol, cSector};
        // override barrelVolume if enclosing is needed -> memory ownership shifted to container
        cSector = m_trackingVolumeCreator->createContainerTrackingVolume(centralVolumes,
                                                                         vacuum,
                                                                         m_geometryName+"::Containers::Central");
    }
    // create the two endcaps
    const Trk::TrackingVolume* nSector = m_trackingVolumeCreator->createGapTrackingVolume(vacuum,
                                                                                         0., enclosingVolumeRadius,
                                                                                         -enclosingVolumeHalfZ, -innerVolumeHalfZ,
                                                                                         m_endcapLayers,
                                                                                         false,
                                                                                         m_geometryName+"::Generic::NegativeEndcap");
    nSector->registerColorCode(m_geometryColorCode);
    // create the two endcaps
    const Trk::TrackingVolume* pSector = m_trackingVolumeCreator->createGapTrackingVolume(vacuum,
                                                                                         0., enclosingVolumeRadius,
                                                                                         innerVolumeHalfZ, enclosingVolumeHalfZ,
                                                                                         m_endcapLayers,
                                                                                         false,
                                                                                         m_geometryName+"::Generic::PositiveEndcap");
    pSector->registerColorCode(m_geometryColorCode);
    
    auto allVolumes = std::vector<const Trk::TrackingVolume*>();
    // if extended barrel is to be built
    auto extendedVolumes = std::vector<const Trk::TrackingVolume*>();
    if ( m_extendedEndcap){
        auto names = std::vector<std::string>{ "Negative", "Positive"};
        for ( size_t it = 0; it < 2; ++it){ 
          // which side we are
          double minZ = !it ? -enclosingExtendedVolumeHalfZ : enclosingVolumeHalfZ;
          double maxZ = !it ? -enclosingVolumeHalfZ : enclosingExtendedVolumeHalfZ;  
          // create the extended volume (bottom part)
          const Trk::TrackingVolume* exVolume = m_trackingVolumeCreator->createGapTrackingVolume(
                                                                                vacuum,
                                                                                0., enclosingExtendedVolumeRadius,
                                                                                minZ, maxZ,
                                                                                m_extendedEndcapLayers,
                                                                                false,
                                                                                m_geometryName+"::Generic::Extended"+names[it]);
          exVolume->registerColorCode(m_geometryColorCode);
          // create the extended volume (ring part)
          const Trk::TrackingVolume* exrVolume = m_trackingVolumeCreator->createGapTrackingVolume(
                                                                                vacuum,
                                                                                enclosingExtendedVolumeRadius, enclosingVolumeRadius,
                                                                                minZ, maxZ,
                                                                                m_extendedEndcapLayers,
                                                                                false,
                                                                                m_geometryName+"::Generic::RingExtended"+names[it]);
          // sign it with one higher volume id 
          exrVolume->sign( Trk::GeometrySignature(int(geometrySignature())+1) );
          // pack it into a container
          auto exVolumes = std::vector<const Trk::TrackingVolume*>{exVolume, exrVolume};
          const Trk::TrackingVolume* exSector = m_trackingVolumeCreator->createContainerTrackingVolume(exVolumes,
                                                                                                       vacuum,
                                                                                                       m_geometryName+"::Generic::"+names[it]+"ExtendedEndcap");
          // for the overall geometry                                                                     
          extendedVolumes.push_back(exSector);                                                                    
       }
    }
    // and now create the triple
    allVolumes = extendedVolumes.size() ?  std::vector<const Trk::TrackingVolume*>{extendedVolumes[0],nSector,cSector,pSector,extendedVolumes[1]} : 
                                           std::vector<const Trk::TrackingVolume*>{nSector,cSector,pSector};
    const Trk::TrackingVolume* tVolume = m_trackingVolumeCreator->createContainerTrackingVolume(allVolumes,
                                                                                                vacuum,
                                                                                                m_geometryName+"::Container");                                                                                      
    // now create the TrackingGeometry from the highest volume
    if (tVolume) {
      tGeometry = new Trk::TrackingGeometry(tVolume);
      tGeometry->indexStaticLayers(geometrySignature());   
    }


    return std::make_pair(range, tGeometry);

}

