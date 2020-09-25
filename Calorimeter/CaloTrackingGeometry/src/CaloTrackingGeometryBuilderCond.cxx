/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloTrackingGeometryBuilderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Calo
#include "CaloTrackingGeometry/CaloTrackingGeometryBuilderCond.h"
#include "CaloTrackingGeometry/ICaloSurfaceHelper.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeBuilder.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeCreator.h"
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkDetDescrInterfaces/IDynamicLayerCreator.h"
//#include "TrkDetDescrInterfaces/IMaterialEffectsOnTrackProvider.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/BinnedMaterial.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/AlignableTrackingVolume.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <memory>
// CLHEP
//#include "CLHEP/Geometry/Transform3D.h"

//using HepGeom::Transform3D;
//using HepGeom::Translate3D;
//using HepGeom::RotateZ3D;
//using HepGeom::Vector3D;
//using CLHEP::Hep3Vector;
//using CLHEP::HepRotation;
//using CLHEP::mm;
//using CLHEP::radian;

// constructor
Calo::CaloTrackingGeometryBuilderCond::CaloTrackingGeometryBuilderCond(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_trackingVolumeArrayCreator("Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"),
  m_trackingVolumeHelper("Trk::TrackingVolumeHelper/TrackingVolumeHelper"),
  m_trackingVolumeCreator("Trk::CylinderVolumeCreator/TrackingVolumeCreator"),
  m_lArVolumeBuilder("LAr::LArVolumeBuilder/LArVolumeBuilder"),
  m_tileVolumeBuilder("Tile::TileVolumeBuilder/TileVolumeBuilder"),
  m_caloMaterial(nullptr),
  m_caloEnvelope(25*Gaudi::Units::mm),
  m_enclosingEnvelopeSvc("AtlasGeometry_EnvelopeDefSvc", n),
  m_caloDefaultRadius(4250.),
  m_caloDefaultHalflengthZ(6500.),
  m_indexStaticLayers(true),
  m_recordLayerIndexCaloSampleMap(false),
  m_layerIndexCaloSampleMapName("LayerIndexCaloSampleMap"),
  m_buildMBTS(true),
  //m_mbstSurfaceShape(2),
  m_entryVolume("Calo::Container::EntryVolume"),
  m_exitVolume("Calo::Container"),
  m_mbtsNegLayers(nullptr),
  m_mbtsPosLayers(nullptr)
  //m_caloSurfaceHelper("CaloSurfaceHelper/CaloSurfaceHelper")
{
  declareInterface<Trk::IGeometryBuilderCond>(this);
  // declare the properties via Python
  declareProperty("LArVolumeBuilder",                 m_lArVolumeBuilder);
  declareProperty("TileVolumeBuilder",                m_tileVolumeBuilder);
  declareProperty("TrackingVolumeArrayCreator",       m_trackingVolumeArrayCreator);
  declareProperty("TrackingVolumeHelper",             m_trackingVolumeHelper);
  declareProperty("TrackingVolumeCreator",            m_trackingVolumeCreator);
  declareProperty("GapLayerEnvelope",                 m_caloEnvelope);
  // envelope definition service
  declareProperty("EnvelopeDefinitionSvc",            m_enclosingEnvelopeSvc );
  // empty calorimeter to be built
  declareProperty("CalorimeterRadius",                m_caloDefaultRadius);
  declareProperty("CalorimeterHalflengthZ",           m_caloDefaultHalflengthZ);
  // unique layer handling
  declareProperty("IndexStaticLayers",                m_indexStaticLayers);
  // for energy deposition
  declareProperty("RecordLayerIndexCaloSampleMap",    m_recordLayerIndexCaloSampleMap);
  declareProperty("LayerIndexCaloSampleMapName",      m_layerIndexCaloSampleMapName);
  // MBTS like detectors 
  declareProperty("BuildMBTS",                        m_buildMBTS);
  //declareProperty("MBTS_SurfaceShape",                m_mbstSurfaceShape);
  //declareProperty("MBTS_Radii",                       m_mbtsRadii);
  //declareProperty("MBTS_RadiusGap",                   m_mbtsRadiusGap);
  //declareProperty("MBTS_PhiSegments",                 m_mbtsPhiSegments);
  //declareProperty("MBTS_PhiGap",                      m_mbtsPhiGap);
  //declareProperty("MBTS_PositionZ",                   m_mbtsPositionZ);
  //declareProperty("MBTS_StaggeringZ",                 m_mbtsStaggeringZ);
  // calo entry volume & exit volume
  declareProperty("EntryVolumeName",                  m_entryVolume);
  declareProperty("ExitVolumeName",                   m_exitVolume);
  
}

// destructor
Calo::CaloTrackingGeometryBuilderCond::~CaloTrackingGeometryBuilderCond()
{
  delete m_caloMaterial;
}

// Athena standard methods
// initialize
StatusCode Calo::CaloTrackingGeometryBuilderCond::initialize()
{
  
    // retrieve envelope definition service --------------------------------------------------
    if ( m_enclosingEnvelopeSvc.retrieve().isFailure() ){
      ATH_MSG_FATAL( "Could not retrieve EnvelopeDefinition service. Abort.");
      return StatusCode::FAILURE;
    }
    
    // Retrieve the tracking volume array creator   -------------------------------------------------    
    if (m_trackingVolumeArrayCreator.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeArrayCreator );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeArrayCreator );

    // Retrieve the tracking volume helper   -------------------------------------------------    
    if (m_trackingVolumeHelper.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeHelper );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeHelper );

    // Retrieve the second volume creator
    if (m_buildMBTS && m_trackingVolumeCreator.retrieve().isFailure()){
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeCreator );
        return StatusCode::FAILURE;
    } else
        ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeCreator );

    // Retrieve the volume builders

    // Retrieve the tracking volume array creator   -------------------------------------------------    
    if (m_lArVolumeBuilder.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_lArVolumeBuilder );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved tool " << m_lArVolumeBuilder );

    // Retrieve the tracking volume helper   -------------------------------------------------    
    if (m_tileVolumeBuilder.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_tileVolumeBuilder );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved tool " << m_tileVolumeBuilder );

    // Retrieve the calo surface helper (to load MBTS) -------------------------------------------------    
#if 0
    if (m_caloSurfaceHelper.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_caloSurfaceHelper );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved tool " << m_caloSurfaceHelper );
#endif

    // Dummy MaterialProerties
    m_caloMaterial = new Trk::Material;

    ATH_MSG_INFO( "initialize() succesful" );

    return StatusCode::SUCCESS;
}

// finalize
StatusCode Calo::CaloTrackingGeometryBuilderCond::finalize()
{
  // empty the material garbage 
  std::map<const Trk::Material*, bool>::iterator garbageIter  = m_materialGarbage.begin();
  std::map<const Trk::Material*, bool>::iterator garbageEnd   = m_materialGarbage.end();
  
  for ( ; garbageIter != garbageEnd; ++garbageIter ){
    delete (garbageIter->first);
  }

  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

std::pair<EventIDRange, const Trk::TrackingGeometry*> Calo::CaloTrackingGeometryBuilderCond::trackingGeometry(const EventContext&, std::pair<EventIDRange, const Trk::TrackingVolume*> tVolPair) const
{

  ATH_MSG_VERBOSE( "Starting to build CaloTrackingGeometry ..." );   
  
  // the return TG
  const Trk::TrackingGeometry* caloTrackingGeometry = nullptr; 
  const Trk::TrackingVolume*            calorimeter = nullptr;                     

  // the key dimensions
  RZPairVector keyDim; 

  // the enclosed input volume (ID)
  double enclosedInnerSectorHalflength = 0.;
  double enclosedInnerSectorRadius = 0.;
   
   // dummy objects
   const Trk::LayerArray* dummyLayers = nullptr;
   const Trk::TrackingVolumeArray* dummyVolumes = nullptr;
  
  //TODO/FIXME: just passing on range, no Calo IOV range used
  EventIDRange range;
  const Trk::TrackingVolume* innerVol = nullptr;
  if(tVolPair.second != nullptr){
    range = tVolPair.first;
    innerVol = tVolPair.second;
  }
  if (innerVol) {  
    ATH_MSG_VERBOSE( "Got Inner Detector Volume: " << innerVol->volumeName() ); 
    innerVol->screenDump(msg(MSG::VERBOSE)); 

    // retrieve dimensions
    const Trk::CylinderVolumeBounds* innerDetectorBounds 
      = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(innerVol->volumeBounds()));
    if (!innerDetectorBounds) std::abort();
    
    enclosedInnerSectorHalflength = innerDetectorBounds->halflengthZ();
    enclosedInnerSectorRadius = innerDetectorBounds->outerRadius();

    keyDim.push_back(RZPair(enclosedInnerSectorRadius, enclosedInnerSectorHalflength));
  }   
  // get the dimensions from the envelope service
  const RZPairVector& envelopeDefsIn = m_enclosingEnvelopeSvc->getCaloRZBoundary();

  // find the max,max pair
  unsigned int ii=0;
  for (unsigned int i=0; i<envelopeDefsIn.size(); i++) { 
    if (envelopeDefsIn[i].second>envelopeDefsIn[ii].second) ii=i;
    else if (envelopeDefsIn[i].second==envelopeDefsIn[ii].second && 
	     envelopeDefsIn[i].first>envelopeDefsIn[ii].first) ii=i;
  }  

  // find the sense of rotation
  int irot=1;
  unsigned int inext=ii+1;
  if (inext==envelopeDefsIn.size()) inext=0;
  if (envelopeDefsIn[inext].second!=envelopeDefsIn[ii].second) {irot=-1; inext = ii>0 ? ii-1:envelopeDefsIn.size()-1 ;}
  
  // fill starting with upper low edge, end with upper high edge
  RZPairVector envelopeDefs;
  if (irot>0) {
    for (unsigned int i=inext; i<envelopeDefsIn.size(); i++) envelopeDefs.push_back(envelopeDefsIn[i]); 
    if (inext>0) for (unsigned int i=0; i<=inext-1; i++) envelopeDefs.push_back(envelopeDefsIn[i]); 
  } else {
    int i=inext;  while (i>=0) {envelopeDefs.push_back(envelopeDefsIn[i]); i=i-1;};
    inext = envelopeDefsIn.size()-1; while (inext>=ii) {envelopeDefs.push_back(envelopeDefsIn[inext]); inext=inext-1;};
  } 


  double envEnclosingVolumeHalfZ = m_caloDefaultHalflengthZ; 

  ATH_MSG_VERBOSE( "Calo envelope definition retrieved:" );  
  RZPairVector msCutouts; 
  for (unsigned int i=0; i<envelopeDefs.size(); i++) {
    ATH_MSG_VERBOSE( "Rz pair:"<< i<<":"<< envelopeDefs[i].first<<","<<envelopeDefs[i].second );   
    if (fabs(envelopeDefs[i].second) < envEnclosingVolumeHalfZ) envEnclosingVolumeHalfZ = fabs(envelopeDefs[i].second);
    // ID dimensions : pick 1100 < R < 1200
    if ( envelopeDefs[i].first > 1100. && envelopeDefs[i].first < 1200. && envelopeDefs[i].second > 0. ) {
      // check that radial size of ID envelope fits
      if ( enclosedInnerSectorRadius != envelopeDefs[i].first ) ATH_MSG_INFO( "Enclosed ID volume radius does not match ID envelope, adjusting Calo." );
      envEnclosingVolumeHalfZ = envelopeDefs[i].second; 
      if (!innerVol) {
	enclosedInnerSectorRadius = envelopeDefs[i].first;
	enclosedInnerSectorHalflength = envelopeDefs[i].second;
      }
    }
    // collect outer cutouts, process those with |z| < 6785.mm ( this cut should be synchronized with MS default size,
    //  MS builder would crash for a longer input volume ) 
    if ( envelopeDefs[i].second > 0. && envelopeDefs[i].second < 6785. &&
	 ( envelopeDefs[i].first > 1200. || envelopeDefs[i].second > envEnclosingVolumeHalfZ ) ) {
      if ( msCutouts.empty() ) msCutouts.push_back( envelopeDefs[i] );
      else {
	RZPairVector::iterator envIter = msCutouts.begin();
        while (envIter!= msCutouts.end() && (*envIter).second < envelopeDefs[i].second ) envIter++;
        while (envIter!= msCutouts.end() && (*envIter).second == envelopeDefs[i].second && (*envIter).first  > envelopeDefs[i].first ) envIter++;
	msCutouts.insert(envIter, envelopeDefs[i]);
      }
    }
    // end outer (MS) cutouts
  }
  for (unsigned int i=0; i<msCutouts.size(); i++) ATH_MSG_VERBOSE( "MS cutouts to be processed by Calo:"<< i<<":"<< msCutouts[i].first<<","<<msCutouts[i].second );
  // first member of msCutouts redefines the default central cylinder dimension     
  if (!msCutouts.empty()) {
    m_caloDefaultRadius = msCutouts[0].first;
    m_caloDefaultHalflengthZ = msCutouts[0].second;
    ATH_MSG_VERBOSE(" Calo central cylinder dimensions adjusted using EnvelopeSvc:"<<m_caloDefaultRadius<<","<< m_caloDefaultHalflengthZ );
  }

  // create dummy ID if not built already

  if (!innerVol) {
   Trk::CylinderVolumeBounds* idBounds = new Trk::CylinderVolumeBounds(enclosedInnerSectorRadius, enclosedInnerSectorHalflength);

   Amg::Transform3D* idTr = new Amg::Transform3D(Trk::s_idTransform);

   innerVol = new Trk::TrackingVolume(idTr, idBounds, *m_caloMaterial,
				      dummyLayers, dummyVolumes,
				      "Calo::GapVolumes::DummyID");

   keyDim.push_back(RZPair(enclosedInnerSectorRadius, enclosedInnerSectorHalflength));
  }
 
  // BEAM PIPE
  //std::cout <<"envelope svc : number of BeamPipe Volumes:"<< m_enclosingEnvelopeSvc->getBeamPipeNumVols()<< std::endl;  
  const RZPairVector& bpDefs = m_enclosingEnvelopeSvc->getBeamPipeRZBoundary();
  
  ATH_MSG_VERBOSE( "BeamPipe envelope definition retrieved:" );   
  m_bpCutouts.clear();
  for (unsigned int i=0; i<bpDefs.size(); i++) {
    ATH_MSG_VERBOSE( "Rz pair:"<< i<<":"<< bpDefs[i].first<<","<<bpDefs[i].second );   
    // beam pipe within calo : pick 10. < R < 200;  envEnclosingVolumeHalfZ   =< z  <= msCutouts.back().second;
    if ( bpDefs[i].first > 10. && bpDefs[i].first < 200. && bpDefs[i].second>=envEnclosingVolumeHalfZ  && bpDefs[i].second <= msCutouts.back().second ) {
      m_bpCutouts.push_back( bpDefs[i] );
    }
    if (i>0 && i<4) keyDim.push_back(bpDefs[i]);
  }
 
  // no beam pipe within ID
  //if (m_bpCutouts[0].second == envEnclosingVolumeHalfZ && m_bpCutouts[0].first > 0 )  m_bpCutouts[0].first=0.;
  // last not needed
  if (m_bpCutouts.size()>1 && m_bpCutouts.back().second==m_bpCutouts[m_bpCutouts.size()-2].second) m_bpCutouts.erase(m_bpCutouts.end()-1);
  // end beam pipe envelope within Calo
  for (unsigned int i=0; i<m_bpCutouts.size(); i++) ATH_MSG_VERBOSE( "Beam pipe dimensions to be used by Calo:"<< i<<":"<< m_bpCutouts[i].first<<","<<m_bpCutouts[i].second );

  keyDim.push_back(RZPair(m_caloDefaultRadius,m_caloDefaultHalflengthZ)); 

   // get the dimensions from the envelope service
   //std::cout <<"envelope svc : number of MS Volumes:"<< m_enclosingEnvelopeSvc->getMuonNumVols()<< std::endl;  
   //RZPairVector& msDefs = m_enclosingEnvelopeSvc->getMuonRZValues(0);

   //ATH_MSG_VERBOSE( "MS envelope definition retrieved:" );   
   //for (unsigned int i=0; i<msDefs.size(); i++) ATH_MSG_VERBOSE( "Rz pair:"<< i<<":"<< msDefs[i].first<<","<<msDefs[i].second );   


   //////////// ---------------- BUILD FROM GEOMODEL ---------------------------- /////////////////////////////////

    
  // PART 1 : Liquid Argon Volumes ===========================================================================================
  // get the Tracking Volumes from the LAr Builder 
  const std::vector<const Trk::TrackingVolume*>* lArVolumes = m_lArVolumeBuilder->trackingVolumes();

  ATH_MSG_INFO( lArVolumes->size() << " volumes retrieved from " << m_lArVolumeBuilder.name() );   
  if (msgLvl(MSG::VERBOSE)){
    ATH_MSG_VERBOSE( "--------------- detailed output ---------------------------------------------------------- " );
    std::vector<const Trk::TrackingVolume*>::const_iterator lArVolIter    = lArVolumes->begin();
    std::vector<const Trk::TrackingVolume*>::const_iterator lArVolIterEnd = lArVolumes->end();
    for ( ; lArVolIter != lArVolIterEnd; ++lArVolIter)
     if (*lArVolIter) (*lArVolIter)->screenDump(msg(MSG::VERBOSE)) ;
  }           
  
  // LAr Barrel part  
  const Trk::TrackingVolume* solenoid                   = (*lArVolumes)[0];
  const Trk::TrackingVolume* solenoidlArPresamplerGap   = (*lArVolumes)[1];
  const Trk::TrackingVolume* lArBarrelPresampler        = (*lArVolumes)[2];
  const Trk::TrackingVolume* lArBarrel                  = (*lArVolumes)[3];

  // LAr Positive Endcap part
  const Trk::TrackingVolume* lArPositiveMBTS            = (*lArVolumes)[4];
  const Trk::TrackingVolume* lArPositiveEndcap          = (*lArVolumes)[5];
  const Trk::TrackingVolume* lArPositiveHec             = (*lArVolumes)[6];
  const Trk::TrackingVolume* lArPositiveFcal            = (*lArVolumes)[7];
  const Trk::TrackingVolume* lArPositiveHecFcalCover    = (*lArVolumes)[8];
  // LAr Negative Endcap part
  const Trk::TrackingVolume* lArNegativeMBTS            = (*lArVolumes)[9];
  const Trk::TrackingVolume* lArNegativeEndcap          = (*lArVolumes)[10];
  const Trk::TrackingVolume* lArNegativeHec             = (*lArVolumes)[11];
  const Trk::TrackingVolume* lArNegativeFcal            = (*lArVolumes)[12];
  const Trk::TrackingVolume* lArNegativeHecFcalCover    = (*lArVolumes)[13];
  const Trk::TrackingVolume* lArPosECPresampler         = (*lArVolumes)[14];
  const Trk::TrackingVolume* lArNegECPresampler         = (*lArVolumes)[15];


  // PART 2 : Tile Volumes ===========================================================================================
  // get the Tracking Volumes from the Tile Builder 
  const std::vector<const Trk::TrackingVolume*>* tileVolumes = m_tileVolumeBuilder->trackingVolumes();

  ATH_MSG_INFO( tileVolumes->size() << " volumes retrieved from " << m_tileVolumeBuilder.name() );   
  if (msgLvl(MSG::INFO)){
    ATH_MSG_INFO( "--------------- detailed output ---------------------------------------------------------- " );
    std::vector<const Trk::TrackingVolume*>::const_iterator tileVolIter = tileVolumes->begin();
    std::vector<const Trk::TrackingVolume*>::const_iterator tileVolIterEnd = tileVolumes->end();
    for ( ; tileVolIter != tileVolIterEnd;  (*tileVolIter)->screenDump(msg(MSG::VERBOSE)), ++tileVolIter)
      ;
  }
    
  // Tile Barrel part
  const Trk::TrackingVolume* tileCombined                  = (*tileVolumes)[0];
  // Tile Positive Extended Part
  const Trk::TrackingVolume* tilePositiveExtendedBarrel  = (*tileVolumes)[1];

  const Trk::CylinderVolumeBounds* ebBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(tilePositiveExtendedBarrel->volumeBounds()));

  double rTileMin = ebBounds ? ebBounds->innerRadius() : 2288.;
  double zTileMin = ebBounds ? tilePositiveExtendedBarrel->center().z()-ebBounds->halflengthZ() : 3559.5;

  keyDim.push_back(RZPair(rTileMin,zTileMin));
  for (unsigned int i=0; i<keyDim.size(); i++) {
    ATH_MSG_VERBOSE( "key dimensions:"<< i<<":"<< keyDim[i].first<<","<<keyDim[i].second );   
  }
  
  // The Bounds needed for the Gap Volume creation
  const Trk::CylinderVolumeBounds* solenoidBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(solenoid->volumeBounds()));
  if (!solenoidBounds) std::abort();
  const Trk::CylinderVolumeBounds* lArBarrelBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArBarrel->volumeBounds()));
  if (!lArBarrelBounds) std::abort();
  const Trk::CylinderVolumeBounds* lArPositiveEndcapBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArPositiveEndcap->volumeBounds()));
  if (!lArPositiveEndcapBounds) std::abort();
  const Trk::CylinderVolumeBounds* lArNegativeEndcapBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArNegativeEndcap->volumeBounds()));
  if (!lArNegativeEndcapBounds) std::abort();
  const Trk::CylinderVolumeBounds* lArPositiveHecBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArPositiveHec->volumeBounds()));
  if (!lArPositiveHecBounds) std::abort();
  const Trk::CylinderVolumeBounds* lArPositiveFcalBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArPositiveFcal->volumeBounds()));
  if (!lArPositiveFcalBounds) std::abort();
  const Trk::CylinderVolumeBounds* lArNegativeFcalBounds 
        = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArNegativeFcal->volumeBounds()));
  if (!lArNegativeFcalBounds) std::abort();
 
  const Trk::CylinderVolumeBounds* tileCombinedBounds 
       = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(tileCombined->volumeBounds()));
  if (!tileCombinedBounds) std::abort();
        
   // Create the gap volumes ======================================================================
   const Trk::TrackingVolume* lArTileCentralSectorGap    = nullptr;

   //const Trk::TrackingVolume* tilePositiveSectorInnerGap = 0;
   const Trk::TrackingVolume* lArPositiveSectorInnerGap  = nullptr;
   //const Trk::TrackingVolume* lArTilePositiveSectorGap   = 0;
   const Trk::TrackingVolume* lArPositiveSectorOuterGap  = nullptr;
   const Trk::TrackingVolume* lArPositiveSectorOuterGap0  = nullptr;
   const Trk::TrackingVolume* lArCentralPositiveGap      = nullptr;

   //const Trk::TrackingVolume* tileNegativeSectorInnerGap = 0;
   const Trk::TrackingVolume* lArNegativeSectorInnerGap  = nullptr;
   //const Trk::TrackingVolume* lArTileNegativeSectorGap   = 0;
   const Trk::TrackingVolume* lArNegativeSectorOuterGap  = nullptr;
   const Trk::TrackingVolume* lArNegativeSectorOuterGap0  = nullptr;
   const Trk::TrackingVolume* lArCentralNegativeGap      = nullptr;

   const Trk::TrackingVolume* trtSolenoidGap         = nullptr;  

   double caloPositiveOuterBoundary    =  tileCombinedBounds->halflengthZ();

   double lArPositiveOuterBoundary     = lArPositiveFcal->center().z();
          lArPositiveOuterBoundary    += lArPositiveFcalBounds->halflengthZ();

   double lArNegativeOuterBoundary     = lArNegativeFcal->center().z();
          lArNegativeOuterBoundary    -= lArNegativeFcalBounds->halflengthZ();

   Trk::Material* mAr = new Trk::Material(140.036, 856.32, 39.948, 18., 0.0014);
   Trk::Material* mAl = new Trk::Material(88.93, 388.62, 26.98, 13., 0.0027);
   //Trk::Material* mFe = new Trk::Material(17.58, 169.68, 55.85, 26., 0.0079);
   Trk::Material* mScint = new Trk::Material(424.35, 707.43, 11.16, 5.61, 0.001);  // from G4 definition
   const Trk::Material* crackMaterial = new Trk::Material(424.35, 707.43, 11.16, 5.61, 0.001);  // Scintillator/Glue (G4 def.)

   throwIntoGarbage(mAr);
   throwIntoGarbage(mAl);
   throwIntoGarbage(mScint);
   throwIntoGarbage(crackMaterial);

   // No. 1
   // building dense volume here
   Trk::Material tilePositiveSectorInnerGapMaterial = Trk::Material(58., 565., 30.7, 14.6, 0.0025);

   // calo sensitive volume outer radius/z extent defined (for example) here
   float caloVolsOuterRadius = tileCombinedBounds->outerRadius(); 
   float caloVolsExtendZ = caloPositiveOuterBoundary; 

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //  LAr InnerGap sector + beam pipe
  
   Trk::Material lArSectorInnerGapMaterial = Trk::Material(361., 1370., 14.5, 7., 0.0009);  // ???

   // create beam pipe volumes for InnerGap/MBTS and return their outer radius 
   float rInnerGapBP=0.;
   std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> innerGapBP = createBeamPipeVolumes(keyDim[0].second, 
							//lArPositiveEndcap->center().z()-lArPositiveEndcapBounds->halflengthZ(),
												       keyDim.back().second,
												       "InnerGap", rInnerGapBP);

   double z = 0.5*(keyDim.back().second+keyDim[0].second);

   if (lArPositiveMBTS && lArNegativeMBTS) {
     // Disc
     const Trk::CylinderVolumeBounds* mbtsBounds 
       = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArPositiveMBTS->volumeBounds()));
#if 0
     if (mbtsBounds && m_caloSurfaceHelper) {     // pass ownership
       Amg::Transform3D* mbtsNeg = new Amg::Transform3D(Amg::Translation3D(lArNegativeMBTS->center()));
       Amg::Transform3D* mbtsPos = new Amg::Transform3D(Amg::Translation3D(lArPositiveMBTS->center()));
       m_caloSurfaceHelper->LoadMBTSSurfaces(std::pair<const Trk::Surface*,const Trk::Surface*>
					     (new Trk::DiscSurface(mbtsNeg,mbtsBounds->innerRadius(),mbtsBounds->outerRadius()),
					      new Trk::DiscSurface(mbtsPos,mbtsBounds->innerRadius(),mbtsBounds->outerRadius())));
     }
#endif
     if (mbtsBounds && m_buildMBTS) {
       float rmin = mbtsBounds->innerRadius(); 
       float rmax = mbtsBounds->outerRadius(); 
       //float d = mbtsBounds->halflengthZ();
       Trk::DiscBounds* dibo = new Trk::DiscBounds(rmin,rmax);
       // MBTS positions
       Amg::Transform3D* mbtsNegZpos = new Amg::Transform3D(Amg::Translation3D(lArNegativeMBTS->center()));
       Amg::Transform3D* mbtsPosZpos = new Amg::Transform3D(Amg::Translation3D(lArPositiveMBTS->center()));
       // create the two Layers ( TODO: add trd surface subarray )
       Trk::DiscLayer* mbtsNegLayer = new Trk::DiscLayer(mbtsNegZpos,dibo,
                                                         //mbtsNegLayerSurfArray,
                                                         Trk::HomogeneousLayerMaterial(Trk::MaterialProperties(*m_caloMaterial,1.),1.),
                                                         1.);
       Trk::DiscLayer* mbtsPosLayer = new Trk::DiscLayer(mbtsPosZpos,dibo->clone(),
                                                         //mbtsPosLayerSurfArray,
                                                         Trk::HomogeneousLayerMaterial(Trk::MaterialProperties(*m_caloMaterial,1.),1.),
                                                         1.*Gaudi::Units::mm);

       m_mbtsNegLayers=new std::vector<const Trk::Layer*>;
       m_mbtsPosLayers=new std::vector<const Trk::Layer*>;
       m_mbtsNegLayers->push_back(mbtsNegLayer);
       m_mbtsPosLayers->push_back(mbtsPosLayer);       
     }
   }
      
   // building inner gap
   Trk::CylinderVolumeBounds* lArG1Bounds =
     new Trk::CylinderVolumeBounds( rInnerGapBP,
				    //lArPositiveEndcapBounds->outerRadius(),
				    keyDim[0].first,
				    0.5*(keyDim.back().second-keyDim[0].second));
   
   Amg::Transform3D* lArG1P =
     new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,z)));
   lArPositiveSectorInnerGap  = new Trk::TrackingVolume(lArG1P,
							lArG1Bounds,
							lArSectorInnerGapMaterial, 
							m_mbtsPosLayers, 
							"Calo::GapVolumes::LAr::PositiveSectorInnerGap");

   Amg::Transform3D* lArG1N =
     new  Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-z)));
   lArNegativeSectorInnerGap  = new Trk::TrackingVolume(lArG1N,
							lArG1Bounds->clone(),
							lArSectorInnerGapMaterial, 
							m_mbtsNegLayers,
							"Calo::GapVolumes::LAr::NegativeSectorInnerGap");

   // glue InnerGap with beam pipe volumes
   const Trk::TrackingVolume* positiveInnerGap = nullptr;
   if (innerGapBP.first) {
     std::vector<const Trk::TrackingVolume*> volsInnerGap;
     volsInnerGap.push_back(innerGapBP.first);
     volsInnerGap.push_back(lArPositiveSectorInnerGap);
     positiveInnerGap = m_trackingVolumeCreator->createContainerTrackingVolume(volsInnerGap,
									       *m_caloMaterial,
									       "Calo::Container::PositiveInnerGap"); 
   } else positiveInnerGap = lArPositiveSectorInnerGap;

   const Trk::TrackingVolume* negativeInnerGap = nullptr;
   if (innerGapBP.second) {
     std::vector<const Trk::TrackingVolume*> volsInnerGap;
     volsInnerGap.push_back(innerGapBP.second);
     volsInnerGap.push_back(lArNegativeSectorInnerGap);
     negativeInnerGap = m_trackingVolumeCreator->createContainerTrackingVolume(volsInnerGap,
									       *m_caloMaterial,
									       "Calo::Container::NegativeInnerGap"); 
   } else negativeInnerGap = lArNegativeSectorInnerGap;

   // glue MBTS volumes with ID
   std::vector<const Trk::TrackingVolume*> inBufferVolumes;
   inBufferVolumes.push_back(negativeInnerGap);     
   inBufferVolumes.push_back(innerVol);     
   inBufferVolumes.push_back(positiveInnerGap);     
   
   const Trk::TrackingVolume* inDetEnclosed = m_trackingVolumeCreator->createContainerTrackingVolume(inBufferVolumes,
												     *m_caloMaterial,
                                                                           "Calo::Container::EnclosedInnerDetector");


   ATH_MSG_DEBUG( " Inner detector enclosed (MBTS volumes)" );   

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // LAr endcap sector including the beam pipe 

   // create beam pipe volumes for Endcap and return their outer radius 
   float rEndcapBP=0.;
   std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> endcapBP = createBeamPipeVolumes( 
							  keyDim.back().second,
							  lArPositiveEndcap->center().z()+lArPositiveEndcapBounds->halflengthZ(),
							  "Endcap", rEndcapBP);

   // build lAr vessel around EC Presampler
   const Trk::CylinderVolumeBounds* ecpBounds 
      = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(lArPosECPresampler->volumeBounds()));
   if (!ecpBounds) std::abort();
    
   float ecpHz = ecpBounds->halflengthZ();
   float ecpRmin = ecpBounds->innerRadius();
   float ecpRmax = ecpBounds->outerRadius();

   Amg::Transform3D* ecpPos=new Amg::Transform3D(lArPosECPresampler->transform());
   Amg::Transform3D* ecpNeg=new Amg::Transform3D(lArNegECPresampler->transform());

   Trk::CylinderVolumeBounds* ecpUpBounds = new Trk::CylinderVolumeBounds(ecpRmax, keyDim.back().first, ecpHz);
   Trk::CylinderVolumeBounds* ecpDownBounds = new Trk::CylinderVolumeBounds(rEndcapBP, ecpRmin, ecpHz);

   const Trk::TrackingVolume* ecPresamplerCoverPos = new Trk::TrackingVolume(ecpPos,ecpUpBounds, *mAl,
									     dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::PositiveECPresamplerCover");
   const Trk::TrackingVolume* ecPresamplerCoverNeg = new Trk::TrackingVolume(ecpNeg,ecpUpBounds->clone(), *mAl,
									     dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::NegativeECPresamplerCover");
   const Trk::TrackingVolume* ecPresamplerInnerPos = new Trk::TrackingVolume(new Amg::Transform3D(*ecpPos),ecpDownBounds, *mAl,
									     dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::PositiveECPresamplerInner");
   const Trk::TrackingVolume* ecPresamplerInnerNeg = new Trk::TrackingVolume(new Amg::Transform3D(*ecpNeg),ecpDownBounds->clone(),
									     *mAl, dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::NegativeECPresamplerInner");

   // glue EC presampler radially
   std::vector<const Trk::TrackingVolume*> volsECP;
   volsECP.push_back(ecPresamplerInnerPos);
   volsECP.push_back(lArPosECPresampler);
   volsECP.push_back(ecPresamplerCoverPos);
   const Trk::TrackingVolume* positiveECP = m_trackingVolumeCreator->createContainerTrackingVolume(volsECP,
									       *m_caloMaterial,
									       "Calo::Container::PositiveECPresamplerR"); 
   std::vector<const Trk::TrackingVolume*> volsECN;
   volsECN.push_back(ecPresamplerInnerNeg);
   volsECN.push_back(lArNegECPresampler);
   volsECN.push_back(ecPresamplerCoverNeg);
   const Trk::TrackingVolume* negativeECP = m_trackingVolumeCreator->createContainerTrackingVolume(volsECN,
									       *m_caloMaterial,
									       "Calo::Container::NegativeECPresamplerR"); 

   // add surrounding buffers
   z = lArPosECPresampler->center().z()-ecpHz;
   float z1 = lArPosECPresampler->center().z()+ecpHz;
   float z2 = lArPositiveEndcap->center().z()-lArPositiveEndcapBounds->halflengthZ();
   Amg::Transform3D* ecIPos = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0., 0.5*(z+keyDim.back().second))));
   Amg::Transform3D* ecINeg = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-0.5*(z+keyDim.back().second))));
   Trk::CylinderVolumeBounds* ecpIBounds = new Trk::CylinderVolumeBounds(rEndcapBP, keyDim.back().first,0.5*(z-keyDim.back().second));
   Amg::Transform3D* ecOPos = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0., 0.5*(z1+z2))));
   Amg::Transform3D* ecONeg = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-0.5*(z1+z2))));
   Trk::CylinderVolumeBounds* ecpOBounds = new Trk::CylinderVolumeBounds(rEndcapBP, keyDim.back().first,0.5*(z2-z1));

   const Trk::TrackingVolume* ecPresamplerInPos = new Trk::TrackingVolume(ecIPos,ecpIBounds, *mAr,
									     dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::PositiveECPresamplerIn");
   const Trk::TrackingVolume* ecPresamplerInNeg = new Trk::TrackingVolume(ecINeg,ecpIBounds->clone(), *mAr,
									     dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::NegativeECPresamplerIn");
   const Trk::TrackingVolume* ecPresamplerOutPos = new Trk::TrackingVolume(ecOPos,ecpOBounds, *mAr,
									     dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::PositiveECPresamplerOut");
   const Trk::TrackingVolume* ecPresamplerOutNeg = new Trk::TrackingVolume(ecONeg,ecpOBounds->clone(),
									     *mAr, dummyLayers, dummyVolumes,
									     "Calo::GapVolumes::LAr::NegativeECPresamplerOut");

   // glue EC presampler in z
   std::vector<const Trk::TrackingVolume*> volECP;
   volECP.push_back(ecPresamplerInPos);
   volECP.push_back(positiveECP);
   volECP.push_back(ecPresamplerOutPos);
   const Trk::TrackingVolume* positiveEP = m_trackingVolumeCreator->createContainerTrackingVolume(volECP,
									       *m_caloMaterial,
									       "Calo::Container::PositiveECPresampler"); 
   std::vector<const Trk::TrackingVolume*> volECN;
   volECN.push_back(ecPresamplerOutNeg);
   volECN.push_back(negativeECP);
   volECN.push_back(ecPresamplerInNeg);
   const Trk::TrackingVolume* negativeEP = m_trackingVolumeCreator->createContainerTrackingVolume(volECN,
									       *m_caloMaterial,
									       "Calo::Container::NegativeECPresampler");

   // build lAr vessel around EMEC
   ecpHz = lArPositiveEndcapBounds->halflengthZ();
   ecpRmin = lArPositiveEndcapBounds->innerRadius();
   ecpRmax = lArPositiveEndcapBounds->outerRadius();

   Amg::Transform3D* ecPos=new Amg::Transform3D(lArPositiveEndcap->transform());
   Amg::Transform3D* ecNeg=new Amg::Transform3D(lArNegativeEndcap->transform());

   Trk::CylinderVolumeBounds* ecUpBounds = new Trk::CylinderVolumeBounds(ecpRmax, keyDim.back().first, ecpHz);
   Trk::CylinderVolumeBounds* ecDownBounds = new Trk::CylinderVolumeBounds(rEndcapBP, ecpRmin, ecpHz);

   const Trk::TrackingVolume* ecCoverPos = new Trk::TrackingVolume(ecPos,ecUpBounds, *mAr,
								   dummyLayers, dummyVolumes,
								   "Calo::GapVolumes::LAr::PositiveEndcapCover");
   const Trk::TrackingVolume* ecCoverNeg = new Trk::TrackingVolume(ecNeg,ecUpBounds->clone(), *mAr,
								   dummyLayers, dummyVolumes,
								   "Calo::GapVolumes::LAr::NegativeEndcapCover");
   const Trk::TrackingVolume* ecInnerPos = new Trk::TrackingVolume(new Amg::Transform3D(*ecPos),ecDownBounds, *mAl,
								   dummyLayers, dummyVolumes,
								   "Calo::GapVolumes::LAr::PositiveEndcapInner");
   const Trk::TrackingVolume* ecInnerNeg = new Trk::TrackingVolume(new Amg::Transform3D(*ecNeg),ecDownBounds->clone(),
								   *mAl, dummyLayers, dummyVolumes,
								   "Calo::GapVolumes::LAr::NegativeEndcapInner");

   // glue EMEC radially
   std::vector<const Trk::TrackingVolume*> volsEC;
   volsEC.push_back(ecInnerPos);
   volsEC.push_back(lArPositiveEndcap);
   volsEC.push_back(ecCoverPos);
   const Trk::TrackingVolume* positiveEC = m_trackingVolumeCreator->createContainerTrackingVolume(volsEC,
									       *m_caloMaterial,
									       "Calo::Container::PositiveEndcapR"); 
   std::vector<const Trk::TrackingVolume*> volsEN;
   volsEN.push_back(ecInnerNeg);
   volsEN.push_back(lArNegativeEndcap);
   volsEN.push_back(ecCoverNeg);
   const Trk::TrackingVolume* negativeEC = m_trackingVolumeCreator->createContainerTrackingVolume(volsEN,
									       *m_caloMaterial,
									       "Calo::Container::NegativeEndcapR"); 

   // glue presampler with EMEC
   std::vector<const Trk::TrackingVolume*> volEEP;
   volEEP.push_back(positiveEP);
   volEEP.push_back(positiveEC);
   const Trk::TrackingVolume* positiveEEP = m_trackingVolumeCreator->createContainerTrackingVolume(volEEP,
									       *m_caloMaterial,
									       "Calo::Container::PositiveEMEC"); 
   std::vector<const Trk::TrackingVolume*> volEEN;
   volEEN.push_back(negativeEC);
   volEEN.push_back(negativeEP);
   const Trk::TrackingVolume* negativeEEP = m_trackingVolumeCreator->createContainerTrackingVolume(volEEN,
									       *m_caloMaterial,
									       "Calo::Container::NegativeEMEC");

   // glue EMEC sector with beam pipe volumes

   std::vector<const Trk::TrackingVolume*> volsEndcapPos;
   if (endcapBP.first)  volsEndcapPos.push_back(endcapBP.first);
   volsEndcapPos.push_back(positiveEEP);

   std::unique_ptr<const Trk::TrackingVolume> positiveEndcap
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsEndcapPos,
                                                             *m_caloMaterial,
                                                             "Calo::Container::PositiveEndcap")); 

   std::vector<const Trk::TrackingVolume*> volsEndcapNeg;
   if (endcapBP.second)  volsEndcapNeg.push_back(endcapBP.second);
   volsEndcapNeg.push_back(negativeEEP);

   std::unique_ptr<const Trk::TrackingVolume> negativeEndcap
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsEndcapNeg,
                                                             *m_caloMaterial,
                                                             "Calo::Container::NegativeEndcap")); 

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Hec sector + beam pipe + lAr cover

   // create beam pipe volumes for Hec and return their outer radius 
   float rHecBP=0.;
   std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> hecBP = createBeamPipeVolumes( 
							  lArPositiveHec->center().z()-lArPositiveHecBounds->halflengthZ(),
							  lArPositiveHec->center().z()+lArPositiveHecBounds->halflengthZ(),
												       "Hec", rHecBP);
   // HecInnerGap (new gap volume )
   Trk::Material lArHecInnerGapMaterial = Trk::Material(390., 1223., 18.,9., 0.0014);

   // create the Bounds
   Trk::CylinderVolumeBounds* lArHecInnerGapBounds = new Trk::CylinderVolumeBounds(rHecBP,
										   lArPositiveHecBounds->innerRadius(),
										   lArPositiveHecBounds->halflengthZ() );

   Amg::Transform3D* lArHecPos = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,lArPositiveHec->center().z())));
   Amg::Transform3D* lArHecNeg = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,lArNegativeHec->center().z())));

   const Trk::TrackingVolume* lArPositiveHecInnerGap = new Trk::TrackingVolume(lArHecPos,lArHecInnerGapBounds ,
									       *mAl,
									       dummyLayers, dummyVolumes,
									       "Calo::GapVolumes::LAr::PositiveHecInnerGap");

   const Trk::TrackingVolume* lArNegativeHecInnerGap = new Trk::TrackingVolume(lArHecNeg,lArHecInnerGapBounds->clone() ,
									       *mAl,
									       dummyLayers, dummyVolumes,
									       "Calo::GapVolumes::LAr::NegativeHecInnerGap");
   // create the Bounds
   Trk::CylinderVolumeBounds* lArHecOuterGapBounds = new Trk::CylinderVolumeBounds(lArPositiveHecBounds->outerRadius(),
										   keyDim.back().first,
										   lArPositiveHecBounds->halflengthZ() );

   const Trk::TrackingVolume* lArPositiveHecOuterGap = new Trk::TrackingVolume(new Amg::Transform3D(*lArHecPos),
									       lArHecOuterGapBounds ,
									       *mAr,
									       dummyLayers, dummyVolumes,
									       "Calo::GapVolumes::LAr::PositiveHecOuterGap");

   const Trk::TrackingVolume* lArNegativeHecOuterGap = new Trk::TrackingVolume(new Amg::Transform3D(*lArHecNeg),
									       lArHecOuterGapBounds->clone() ,
									       *mAr,
									       dummyLayers, dummyVolumes,
									       "Calo::GapVolumes::LAr::NegativeHecOuterGap");

   // glue Hec sector with beam pipe volumes

   std::vector<const Trk::TrackingVolume*> volsHecPos;
   if (hecBP.first)  volsHecPos.push_back(hecBP.first);
   volsHecPos.push_back(lArPositiveHecInnerGap);
   volsHecPos.push_back(lArPositiveHec);
   volsHecPos.push_back(lArPositiveHecOuterGap);

   std::unique_ptr<const Trk::TrackingVolume> positiveHec
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsHecPos,
                                                             *m_caloMaterial,
                                                             "Calo::Container::PositiveHec"));

   std::vector<const Trk::TrackingVolume*> volsHecNeg;
   if (hecBP.second)  volsHecNeg.push_back(hecBP.second);
   volsHecNeg.push_back(lArNegativeHecInnerGap);
   volsHecNeg.push_back(lArNegativeHec);
   volsHecNeg.push_back(lArNegativeHecOuterGap);

   std::unique_ptr<const Trk::TrackingVolume> negativeHec
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsHecNeg,
                                                             *m_caloMaterial,
                                                             "Calo::Container::NegativeHec"));

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Fcal sector + beam pipe + lAr cover

   // create beam pipe volumes for Fcal and return their outer radius 
   float rFcalBP=0.;
   std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> fcalBP = createBeamPipeVolumes( 
							  lArPositiveFcal->center().z()-lArPositiveFcalBounds->halflengthZ(),
							  lArPositiveFcal->center().z()+lArPositiveFcalBounds->halflengthZ(),
												       "Fcal", rFcalBP);
   // FcalInnerGap (new gap volume )
   //Trk::Material lArFcalInnerGapMaterial = Trk::Material(390., 1223., 40.,18., 0.0014);

   
   if ( rFcalBP > lArPositiveFcalBounds->innerRadius()) {
     ATH_MSG_ERROR("PROBLEM : beam pipe collide with Fcal:"<< rFcalBP <<">" << lArPositiveFcalBounds->innerRadius()<<", abort" );  
     // create dummy infinite IOV Range 
     EventIDRange range;
     return std::pair<EventIDRange, const Trk::TrackingGeometry*>(range,0);
     
   }

   // create the Bounds
   Trk::CylinderVolumeBounds* lArFcalInnerGapBounds = new Trk::CylinderVolumeBounds(rFcalBP,
										    lArPositiveFcalBounds->innerRadius(),
										    lArPositiveFcalBounds->halflengthZ() );

   Amg::Transform3D* lArFcalPos = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,lArPositiveFcal->center().z())));
   Amg::Transform3D* lArFcalNeg = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,lArNegativeFcal->center().z())));

   const Trk::TrackingVolume* lArPositiveFcalInnerGap = new Trk::TrackingVolume(lArFcalPos,lArFcalInnerGapBounds ,
										*mAl,
										dummyLayers, dummyVolumes,
										"Calo::GapVolumes::LAr::PositiveFcalInnerGap");

   const Trk::TrackingVolume* lArNegativeFcalInnerGap = new Trk::TrackingVolume(lArFcalNeg,lArFcalInnerGapBounds->clone() ,
										*mAl,
										dummyLayers, dummyVolumes,
										"Calo::GapVolumes::LAr::NegativeFcalInnerGap");


   // create the Bounds
   Trk::CylinderVolumeBounds* lArFcalOuterGapBounds = new Trk::CylinderVolumeBounds(lArPositiveHecBounds->outerRadius(),
										    keyDim.back().first,
										    lArPositiveFcalBounds->halflengthZ() );

   const Trk::TrackingVolume* lArPositiveFcalOuterGap = new Trk::TrackingVolume(new Amg::Transform3D(*lArFcalPos),
										lArFcalOuterGapBounds ,
										*mAr,
										dummyLayers, dummyVolumes,
										"Calo::GapVolumes::LAr::PositiveFcalOuterGap");

   const Trk::TrackingVolume* lArNegativeFcalOuterGap = new Trk::TrackingVolume(new Amg::Transform3D(*lArFcalNeg),
										lArFcalOuterGapBounds->clone() ,
										*mAr,
										dummyLayers, dummyVolumes,
										"Calo::GapVolumes::LAr::NegativeFcalOuterGap");

   // glue Fcal sector with beam pipe volumes

   std::vector<const Trk::TrackingVolume*> volsFcalPos;
   if (fcalBP.first)  volsFcalPos.push_back(fcalBP.first);
   volsFcalPos.push_back(lArPositiveFcalInnerGap);
   volsFcalPos.push_back(lArPositiveFcal);
   volsFcalPos.push_back(lArPositiveHecFcalCover);
   volsFcalPos.push_back(lArPositiveFcalOuterGap);

   std::unique_ptr<const Trk::TrackingVolume> positiveFcal
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsFcalPos,
                                                             *m_caloMaterial,
                                                             "Calo::Container::PositiveFcal"));

   std::vector<const Trk::TrackingVolume*> volsFcalNeg;
   if (fcalBP.second)  volsFcalNeg.push_back(fcalBP.second);
   volsFcalNeg.push_back(lArNegativeFcalInnerGap);
   volsFcalNeg.push_back(lArNegativeFcal);
   volsFcalNeg.push_back(lArNegativeHecFcalCover);
   volsFcalNeg.push_back(lArNegativeFcalOuterGap);

   std::unique_ptr<const Trk::TrackingVolume> negativeFcal
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsFcalNeg,
                                                             *m_caloMaterial,
                                                             "Calo::Container::NegativeFcal"));

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Outer endcap sector + beam pipe

   // create beam pipe volumes for Outer sector and return their outer radius 
   float rOutBP=0.;
   std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> outBP = createBeamPipeVolumes( lArPositiveOuterBoundary, 
												   caloPositiveOuterBoundary,
												   "Outer", rOutBP);

   Trk::Material lArSectorOuterGapMat = Trk::Material(20.5, 723., 64., 28., 0.0084);
   
   Trk::CylinderVolumeBounds* lArSectorOuterG0Bounds = new Trk::CylinderVolumeBounds( rOutBP, 430.,
										      //lArPositiveEndcapBounds->outerRadius(),
							   0.5*(caloPositiveOuterBoundary-lArPositiveOuterBoundary));

   Trk::CylinderVolumeBounds* lArSectorOuterG1Bounds = new Trk::CylinderVolumeBounds( 430., keyDim.back().first,
										      //lArPositiveEndcapBounds->outerRadius(),
							   0.5*(caloPositiveOuterBoundary-lArPositiveOuterBoundary));
   
   z = 0.5*(caloPositiveOuterBoundary+lArPositiveOuterBoundary);

   Amg::Transform3D* lArSecOutG1P = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,z)));
   Amg::Transform3D* lArSecOutG0P = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,z)));

   lArPositiveSectorOuterGap0   =  new Trk::TrackingVolume(lArSecOutG0P, lArSectorOuterG0Bounds,
							  lArSectorOuterGapMat, 
							  dummyLayers, dummyVolumes, 
							  "Calo::GapVolumes::LAr::PositiveSectorOuterGap0");

   lArPositiveSectorOuterGap   =  new Trk::TrackingVolume(lArSecOutG1P, lArSectorOuterG1Bounds,
							  *mAr, 
							  dummyLayers, dummyVolumes, 
							  "Calo::GapVolumes::LAr::PositiveSectorOuterGap");

   Amg::Transform3D* lArSecOutG1N = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-z)));
   Amg::Transform3D* lArSecOutG0N = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-z)));

   lArNegativeSectorOuterGap0   =  new Trk::TrackingVolume(lArSecOutG0N, lArSectorOuterG0Bounds->clone(),
							  lArSectorOuterGapMat, 
							  dummyLayers, dummyVolumes, 
							  "Calo::GapVolumes::LAr::NegativeSectorOuterGap0");

   lArNegativeSectorOuterGap   =  new Trk::TrackingVolume(lArSecOutG1N, lArSectorOuterG1Bounds->clone(),
							  *mAr, 
							  dummyLayers, dummyVolumes, 
							  "Calo::GapVolumes::LAr::NegativeSectorOuterGap");

   // glue OuterGap with beam pipe volumes
   std::vector<const Trk::TrackingVolume*> volsOuterGapP;
   if (outBP.first) volsOuterGapP.push_back(outBP.first);
   volsOuterGapP.push_back(lArPositiveSectorOuterGap0);
   volsOuterGapP.push_back(lArPositiveSectorOuterGap);
   std::unique_ptr<const Trk::TrackingVolume> positiveOuterGap
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsOuterGapP,
                                                             *m_caloMaterial,
                                                             "Calo::Container::PositiveOuterGap"));

   std::vector<const Trk::TrackingVolume*> volsOuterGapN;
   if (outBP.second) volsOuterGapN.push_back(outBP.second);
   volsOuterGapN.push_back(lArNegativeSectorOuterGap0);
   volsOuterGapN.push_back(lArNegativeSectorOuterGap);
   std::unique_ptr<const Trk::TrackingVolume> negativeOuterGap
     (m_trackingVolumeCreator->createContainerTrackingVolume(volsOuterGapN,
                                                             *m_caloMaterial,
                                                             "Calo::Container::NegativeOuterGap"));

   ATH_MSG_DEBUG( "Endcap volumes ready" );

   // ++ (ii) lArNegativeSector 
   const Trk::TrackingVolume* lArNegativeSector           = nullptr;             
   // +++ has 4 sub volumes in Z
   std::vector<const Trk::TrackingVolume*> lArNegativeSectorVolumes;
   // +++ (A) -> Outer sector 
   lArNegativeSectorVolumes.push_back(negativeOuterGap.release());
   // +++ (B) -> Fcal sector
   lArNegativeSectorVolumes.push_back(negativeFcal.release());
   // +++ (C) -> Hec sector
   lArNegativeSectorVolumes.push_back(negativeHec.release());
   // +++ (D) -> Endcap sector
   lArNegativeSectorVolumes.push_back(negativeEndcap.release());
   // +++ all exists : create super volume (ii)
   lArNegativeSector = m_trackingVolumeCreator->createContainerTrackingVolume( lArNegativeSectorVolumes,
									       *m_caloMaterial,
									       "Calo::LAr::Container::NegativeSector");

   // ++ (ii) lArPositiveSector 
   const Trk::TrackingVolume* lArPositiveSector           = nullptr;             
   // +++ has 4 sub volumes in Z
   std::vector<const Trk::TrackingVolume*> lArPositiveSectorVolumes;
   // +++ (A) -> Endcap sector
   lArPositiveSectorVolumes.push_back(positiveEndcap.release());	 
   // +++ (B) -> Hec sector
   lArPositiveSectorVolumes.push_back(positiveHec.release());
   // +++ (C) -> Fcal sector
   lArPositiveSectorVolumes.push_back(positiveFcal.release());
   // +++ (D) -> OuterGap
   lArPositiveSectorVolumes.push_back(positiveOuterGap.release());
   // +++ all exists : create super volume (ii)
   lArPositiveSector = m_trackingVolumeCreator->createContainerTrackingVolume( lArPositiveSectorVolumes,
									       *m_caloMaterial,
									       "Calo::LAr::Container::PositiveSector");
   
   ////////////////////////////////////////////////////////////////////////////////////// 
   // central LAr barrel
   //
   // solenoid gap 
   Trk::CylinderVolumeBounds* trtSolGapBounds = new Trk::CylinderVolumeBounds(  keyDim[0].first,
										solenoidBounds->innerRadius(),
										solenoidBounds->halflengthZ());

   // solenoid gap is not completely empty
   Trk::Material solGapMat(535.,2871.,18.6,9.1,0.00038);
   
   
   trtSolenoidGap = new Trk::TrackingVolume(nullptr,
					    trtSolGapBounds,
					    solGapMat,
					    dummyLayers, dummyVolumes, 
					    "Calo::GapVolumes::SolenoidGap");

   Trk::CylinderVolumeBounds* lArTileCentralG1Bounds = new Trk::CylinderVolumeBounds( lArBarrelBounds->outerRadius(),
										      tileCombinedBounds->innerRadius(),
										      lArBarrelBounds->halflengthZ());
                                                 
   

   lArTileCentralSectorGap      = new Trk::TrackingVolume(nullptr, lArTileCentralG1Bounds,
							  *mAr, 
							  dummyLayers, dummyVolumes, 
							  "Calo::GapVolumes::LArTileCentralSectorGap");


   const Trk::TrackingVolume* lArCentralBarrelSector    = nullptr;
   // ++ has 6 sub volumes in R
   std::vector<const Trk::TrackingVolume*> lArCentralBarrelSectorVolumes;
   // ++++ (a) -> solenoid gap 
   lArCentralBarrelSectorVolumes.push_back(trtSolenoidGap);
   // ++++ (b) -> solenoid (exists already)
   lArCentralBarrelSectorVolumes.push_back(solenoid);
   // ++++ (c) -> solenoidlArPresamplerGap (exists already)
   lArCentralBarrelSectorVolumes.push_back(solenoidlArPresamplerGap);
   // ++++ (d) -> lArBarrelPresampler (exists already)
   lArCentralBarrelSectorVolumes.push_back(lArBarrelPresampler);
   // ++++ (e) -> lArBarrel (exists already)
   lArCentralBarrelSectorVolumes.push_back(lArBarrel);
   // ++++ (f) -> lArTile gap 
   lArCentralBarrelSectorVolumes.push_back(lArTileCentralSectorGap);
   // ++++ all sub volumes exist: build the super volume
   lArCentralBarrelSector =  m_trackingVolumeCreator->createContainerTrackingVolume(
										    lArCentralBarrelSectorVolumes,
										    *m_caloMaterial,
										    "Calo::Containers::LAr::CentralBarrelSector",
										    false, true);
   /////////////////////////////////////////////////////////////////////////////////////
   // side buffers   
   // the Tile Crack volume (TileGap3, enum 17) inserted here
   // binned material for Crack : steering in binEta
   // TODO turn into 2D binned array
   std::vector<const Trk::IdentifiedMaterial*> matCrack;
   // layer material can be adjusted here
   int baseID = Trk::GeometrySignature(Trk::Calo)*1000 + 17;
   matCrack.push_back(new std::pair<const Trk::Material*,int>(mScint,baseID));
   matCrack.push_back(new std::pair<const Trk::Material*,int>(m_caloMaterial,-1));
   matCrack.push_back(new std::pair<const Trk::Material*,int>(mAl,-1));
   //
   Trk::BinUtility* bun = new Trk::BinUtility(3,-1.8,-1.2,Trk::open,Trk::binEta);
   Trk::BinUtility* bup = new Trk::BinUtility(3, 1.2,1.8,Trk::open,Trk::binEta);
   // array of indices
   std::vector<std::vector<size_t> > indexP;  
   std::vector<std::vector<size_t> > indexN;  
   // binned material for LAr : layer depth per eta bin
   std::vector< Trk::BinUtility*> layDN(bun->bins());
   std::vector< Trk::BinUtility*> layUP(bup->bins());
   double crackZ1 = 3532.;
   double crackZ2 = 3540.;
   // construct bin utilities 
   std::vector<float> steps;
   for (unsigned int i=0; i< bup->bins(); i++) {
     steps.clear();
     std::vector<size_t> indx; indx.clear();
     steps.push_back(crackZ1);
     indx.push_back( i<2 ? 0 : 1);
     steps.push_back(crackZ2);
     indx.push_back(2);
     steps.push_back(keyDim.back().second);
     Trk::BinUtility* zBU = new Trk::BinUtility(steps, Trk::open, Trk::binZ);
     layUP[i] = zBU;
     indexP.push_back(indx);
   } 

   const Trk::BinnedMaterial* crackBinPos = new Trk::BinnedMaterial(crackMaterial,bup,layUP,indexP,matCrack);
   
   Amg::Transform3D* align=nullptr;
   
   Trk::CylinderVolumeBounds* crackBoundsPos = new Trk::CylinderVolumeBounds( keyDim[0].first,
									      tileCombinedBounds->innerRadius(),
									      0.5*(keyDim.back().second-crackZ1));
   z = 0.5*(keyDim.back().second+crackZ1);
   Amg::Transform3D* crackPosTransform = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,z)));
   
   const Trk::AlignableTrackingVolume* crackPos = new Trk::AlignableTrackingVolume(crackPosTransform,align,
										   crackBoundsPos,
										   crackBinPos,
										   17,
										   "Calo::Detectors::Tile::CrackPos");

   for (unsigned int i=0; i< bun->bins(); i++) {
     steps.clear();
     std::vector<size_t> indx; indx.clear();
     steps.push_back(-keyDim.back().second);
     indx.push_back(2);
     steps.push_back(-crackZ2);
     indx.push_back( i>0 ? 0 : 1);
     steps.push_back(-crackZ1);
     Trk::BinUtility* zBU = new Trk::BinUtility(steps, Trk::open, Trk::binZ);
     layDN[i] = zBU;
     indexN.push_back(indx);
   } 

   const Trk::BinnedMaterial* crackBinNeg = new Trk::BinnedMaterial(crackMaterial,bun,layDN,indexN,matCrack);
   
   align=nullptr;
   
   Amg::Transform3D* crackNegTransform = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-z)));
   
   const Trk::AlignableTrackingVolume* crackNeg = new Trk::AlignableTrackingVolume(crackNegTransform,align,
										   crackBoundsPos->clone(),
										   crackBinNeg,
										   17,
										   "Calo::Detectors::Tile::CrackNeg");
    ////
  
   Trk::CylinderVolumeBounds* lArCentralG1Bounds = new Trk::CylinderVolumeBounds( keyDim[0].first,
										  tileCombinedBounds->innerRadius(),
										  0.5*(crackZ1-lArBarrelBounds->halflengthZ()));

   z = 0.5*(crackZ1+lArBarrelBounds->halflengthZ());
   Amg::Transform3D* lArCentralG1P = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,z)));

   
   lArCentralPositiveGap        = new Trk::TrackingVolume(lArCentralG1P, lArCentralG1Bounds,
							  *mAr, 
							  dummyLayers, dummyVolumes, 
							  "Calo::GapVolumes::LArCentralPositiveGap");

   Amg::Transform3D* lArCentralG1N = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-z)));
   
   lArCentralNegativeGap        = new Trk::TrackingVolume(lArCentralG1N, lArCentralG1Bounds->clone(),
							  *mAr, 
							  dummyLayers, dummyVolumes, 
							  "Calo::GapVolumes::LArCentralNegativeGap");

   // glue laterally
   const Trk::TrackingVolume* lArCentralSector    = nullptr;
   // ++ has 5 sub volumes in z
   std::vector<const Trk::TrackingVolume*> lArCentralSectorVolumes;
   // ++++ (a) -> negative crack
   lArCentralSectorVolumes.push_back(crackNeg);
   // ++++ (b) -> negative gap 
   lArCentralSectorVolumes.push_back(lArCentralNegativeGap);
   // ++++ (c) -> central barrel 
   lArCentralSectorVolumes.push_back(lArCentralBarrelSector);
   // ++++ (d) -> positive gap 
   lArCentralSectorVolumes.push_back(lArCentralPositiveGap);
   // ++++ (e) -> positive crack 
   lArCentralSectorVolumes.push_back(crackPos);
   // ++++ all sub volumes exist: build the super volume
   lArCentralSector =  m_trackingVolumeCreator->createContainerTrackingVolume(
									     lArCentralSectorVolumes,
									     *m_caloMaterial,
									     "Calo::Containers::LAr::CentralSector");
  
   // glue with ID sector
   const Trk::TrackingVolume* caloCentralSector    = nullptr;
   // ++ has 2 sub volumes in R
   std::vector<const Trk::TrackingVolume*> caloCentralSectorVolumes;
   // ++++ (a) -> ID sector
   caloCentralSectorVolumes.push_back(inDetEnclosed);
   // ++++ (b) -> LAr sector 
   caloCentralSectorVolumes.push_back(lArCentralSector);
   // ++++ all sub volumes exist: build the super volume
   caloCentralSector =  m_trackingVolumeCreator->createContainerTrackingVolume(
									  caloCentralSectorVolumes,
									  *m_caloMaterial,
									  "Calo::Containers::CaloCentralSector");

   //std::cout <<"Combined LAr sector ready"<< std::endl;
   //////////////////////////////////////////////////////////////////////


   // glue laterally with endcaps
   const Trk::TrackingVolume* lArCombined    = nullptr;
   // ++ has 3 sub volumes in z
   std::vector<const Trk::TrackingVolume*> lArCombinedVolumes;
   // ++++ (a) -> negative endcap 
   lArCombinedVolumes.push_back(lArNegativeSector);
   // ++++ (b) -> barrel 
   lArCombinedVolumes.push_back(caloCentralSector);
   // ++++ (a) -> positive endcap 
   lArCombinedVolumes.push_back(lArPositiveSector);
   // ++++ all sub volumes exist: build the super volume
   lArCombined =  m_trackingVolumeCreator->createContainerTrackingVolume(
									 lArCombinedVolumes,
									 *m_caloMaterial,
									 "Calo::Containers::LAr::Combined");
 
   //std::cout <<"Combined LAr ready " << std::endl; 

   // glue with LAr sector
   const Trk::TrackingVolume* caloCombined    = nullptr;
   // ++ has 2 sub volumes in R
   std::vector<const Trk::TrackingVolume*> caloVolumes;
   // ++++ (a) -> LAr sector
   caloVolumes.push_back(lArCombined);
   // ++++ (b) -> Tile sector 
   caloVolumes.push_back(tileCombined);
   // ++++ all sub volumes exist: build the super volume
   caloCombined =  m_trackingVolumeCreator->createContainerTrackingVolume(
									 caloVolumes,
									 *m_caloMaterial,
									 "Calo::Containers::CombinedCalo");

   //std::cout <<"Combined Calo ready"<< std::endl;

   ///////////////////////////////////////////////////////////////////////////////////////////////

   // build the radial buffer volume to synchronize the radial envelope dimensions 
   const Trk::TrackingVolume* centralBuffer = nullptr;
   const Trk::TrackingVolume* ecPosBuffer = nullptr;
   const Trk::TrackingVolume* ecNegBuffer = nullptr;
    
   if ( caloVolsOuterRadius >  m_caloDefaultRadius ) {
     ATH_MSG_VERBOSE( "Calo volumes exceeds envelope radius: adjusting envelope (de-synchronizing...)" );
     m_caloDefaultRadius = caloVolsOuterRadius;
   }

   if ( caloVolsOuterRadius <  m_caloDefaultRadius ) {
     ATH_MSG_VERBOSE( "Build radial buffer to synchronize the boundaries in between R "<< caloVolsOuterRadius<<" and "<< m_caloDefaultRadius );
     
     Trk::CylinderVolumeBounds* centralSynBounds = new Trk::CylinderVolumeBounds(caloVolsOuterRadius,
										 m_caloDefaultRadius,
										 caloVolsExtendZ);
     centralBuffer = new Trk::TrackingVolume(nullptr, centralSynBounds,
					     *m_caloMaterial,
					     dummyLayers, dummyVolumes,
					     "Calo::GapVolumes::EnvelopeBuffer");
   }

   if ( caloVolsExtendZ <  m_caloDefaultHalflengthZ ) {
     ATH_MSG_VERBOSE( "Build longitudinal buffers to synchronize the boundaries in between Z "<< caloVolsExtendZ<<" and "<< m_caloDefaultHalflengthZ );
     
     Trk::CylinderVolumeBounds* endcapSynBounds = new Trk::CylinderVolumeBounds(0., m_caloDefaultRadius,
										0.5*(m_caloDefaultHalflengthZ-caloVolsExtendZ));
     // endcap buffers not empty 
     Trk::Material ecBuffMat(53./0.38, 355./0.38, 20., 10., 0.0053*pow(0.38,3));
     
     float zPos = 0.5*(m_caloDefaultHalflengthZ+caloVolsExtendZ);
     ecPosBuffer = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,zPos))),
					   endcapSynBounds,
					   *mAr,
					   dummyLayers, dummyVolumes,
					   "Calo::GapVolumes::PosECBuffer");

     ecNegBuffer = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-zPos))),
					   endcapSynBounds->clone(),
					   *mAr,
					   dummyLayers, dummyVolumes,
					   "Calo::GapVolumes::NegECBuffer");
   }
   
   ATH_MSG_VERBOSE( "All gap volumes for the Calorimeter created. Starting to build Volume Hiearchy." );   
	
   /////////////////////////////////////////////////////////////////////////////////////////////////
   
   ATH_MSG_VERBOSE( "Resolve MS cutouts:" );   
   
   // resolve number of additional cutout volumes
   int nCutVol = 0;
   std::vector<float> zCutStep;
   std::vector<float> rCutStep;
   if (msCutouts.size()>1) {
     zCutStep.push_back(msCutouts[0].second); 
     rCutStep.push_back(msCutouts[0].first); 
     for (unsigned int i=1; i<msCutouts.size(); i++) {
       if (msCutouts[i].second == zCutStep.back()) rCutStep.push_back(msCutouts[i].first);
       else zCutStep.push_back(msCutouts[i].second);
     }
     nCutVol = zCutStep.size()-1;
   }
   if (nCutVol>0) ATH_MSG_VERBOSE( nCutVol << " MS cutout volume(s) required " );

   std::vector<const Trk::TrackingVolume*> forwardCutoutVols;
   std::vector<const Trk::TrackingVolume*> backwardCutoutVols;

   // cutout region not empty 
   std::vector<Trk::Material> cutOutMat;
   cutOutMat.emplace_back(19.9, 213., 50., 23., 0.0065);     // 3.5 Fe + 1 Ar : verify
   cutOutMat.push_back(*m_caloMaterial);
   //cutOutMat.push_back(Trk::Material(18.74, 200.9, 52.36, 24.09, 0.0069));
   
   if (nCutVol>0) {
     // loop over zCutStep, rCutStep[0] gives the fixed outer radius, rCutStep[i] the cutout radius for ith volume
     float rout = rCutStep[0];
     float zlow = zCutStep[0];
     for (unsigned int i=1; i<zCutStep.size(); i++) {
       float zup  = zCutStep[i];
       float rcut = rCutStep[i];
       std::stringstream ss;
       ss << i;
       // get beam pipe volumes
       float rCutOutBP = 0.;
       std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> cutOutBP = createBeamPipeVolumes(zlow,zup,  
												       "CutOuts"+ss.str(), rCutOutBP);

       // build inner part ( -> Calo )
       unsigned int mindex = i>1 ? 1 : i;
       
       Trk::CylinderVolumeBounds* cutInBounds = new Trk::CylinderVolumeBounds( rCutOutBP,rcut, 0.5*(zup-zlow));
       const Trk::TrackingVolume* caloInPos = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,0.5*(zup+zlow)))),
								      cutInBounds,
								      cutOutMat[mindex],
								      dummyLayers, dummyVolumes,
								      "Calo::GapVolumes::CaloPositiveCutIn"+ss.str());
       const Trk::TrackingVolume* caloInNeg = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-0.5*(zup+zlow)))),
								      cutInBounds->clone(),
								      cutOutMat[mindex],
								      dummyLayers, dummyVolumes,
								      "Calo::GapVolumes::CaloNegativeCutIn"+ss.str());
       // build cutout ( -> MS ) : geometry signature needs to be resolved later : follow naming convention 
       Trk::CylinderVolumeBounds* cutOutBounds = new Trk::CylinderVolumeBounds( rcut, rout, 0.5*(zup-zlow));
       const Trk::TrackingVolume* caloOutPos = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,0.5*(zup+zlow)))),
								       cutOutBounds,
								       *m_caloMaterial,
								       dummyLayers, dummyVolumes,
								       "Muon::GapVolumes::CaloPositiveCutOut"+ss.str());
       const Trk::TrackingVolume* caloOutNeg = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-0.5*(zup+zlow)))),
								       cutOutBounds->clone(),
								       *m_caloMaterial,
								       dummyLayers, dummyVolumes,
								       "Muon::GapVolumes::CaloNegativeCutOut"+ss.str());

       // sign
       caloOutPos->sign(Trk::MS);
       caloOutNeg->sign(Trk::MS);
   
       // glue radially and save for final gluing
       std::vector<const Trk::TrackingVolume*> cvPos;
       cvPos.push_back(cutOutBP.first);
       cvPos.push_back(caloInPos);
       cvPos.push_back(caloOutPos);
       const Trk::TrackingVolume* cutOutPos = m_trackingVolumeCreator->createContainerTrackingVolume(cvPos,
												*m_caloMaterial,
										"Calo::GapVolumes::CaloPositiveCutoutVolume"+ss.str());
       forwardCutoutVols.push_back(cutOutPos);      
       //
       std::vector<const Trk::TrackingVolume*> cvNeg;
       cvNeg.push_back(cutOutBP.second);
       cvNeg.push_back(caloInNeg);
       cvNeg.push_back(caloOutNeg);
       const Trk::TrackingVolume* cutOutNeg = m_trackingVolumeCreator->createContainerTrackingVolume(cvNeg,
												*m_caloMaterial,
										"Calo::GapVolumes::CaloNegativeCutoutVolume"+ss.str());
       backwardCutoutVols.insert(backwardCutoutVols.begin(),cutOutNeg);      
  
       zlow = zup;
     }
   }

  /////////////////

 
  if (!centralBuffer) {   //(XX)
    // all volumes exist : create the super volume at top level
    calorimeter = caloCombined;

  } else {                //(X)

    std::vector<const Trk::TrackingVolume*> envRVols;
    envRVols.push_back(caloCombined);
    envRVols.push_back(centralBuffer);
    
    // add cutouts - final step
    if ( !forwardCutoutVols.empty() || ecNegBuffer ) {
      
      const Trk::TrackingVolume* caloRVolume = m_trackingVolumeCreator->createContainerTrackingVolume(envRVols,
												      *m_caloMaterial,
												      "Calo::Containers::CombinedRCalo");
      std::vector<const Trk::TrackingVolume*> envZVols;
      for (unsigned int i=0; i<backwardCutoutVols.size(); i++) envZVols.push_back(backwardCutoutVols[i]);   
      if (ecNegBuffer) envZVols.push_back(ecNegBuffer);
      envZVols.push_back(caloRVolume);  
      if (ecPosBuffer) envZVols.push_back(ecPosBuffer);
      for (unsigned int i=0; i<forwardCutoutVols.size(); i++) envZVols.push_back(forwardCutoutVols[i]);
      
      calorimeter = m_trackingVolumeCreator->createContainerTrackingVolume(envZVols,
									   *m_caloMaterial,
									   m_exitVolume);
      
      
      
    } else { // if no cutouts or endcap buffers, this is the top calo volume  
      
      calorimeter = m_trackingVolumeCreator->createContainerTrackingVolume(envRVols,
									   *m_caloMaterial,
									   m_exitVolume);
    }
    
  }

  ATH_MSG_VERBOSE( "TrackingVolume 'Calorimeter' built successfully. Wrap it in TrackingGeometry." );

  delete lArVolumes; lArVolumes = nullptr;
  delete tileVolumes; tileVolumes = nullptr;
  
  caloTrackingGeometry = new Trk::TrackingGeometry(calorimeter);
  
  if (msgLvl(MSG::VERBOSE) && caloTrackingGeometry) 
    caloTrackingGeometry->printVolumeHierarchy(msg(MSG::VERBOSE));   
  
  if (m_indexStaticLayers && caloTrackingGeometry)
    caloTrackingGeometry->indexStaticLayers( geometrySignature() );   
  
//     /** enumeration of samplings (i.e.layers) separately for various sub calorimeters */
//     enum CaloSample {
//       PreSamplerB=0, EMB1, EMB2, EMB3,       // LAr barrel
//       PreSamplerE, EME1, EME2, EME3,         // LAr EM endcap
//       HEC0, HEC1, HEC2, HEC3,                // Hadronic end cap cal.
//       TileBar0, TileBar1, TileBar2,          // Tile barrel
//       TileGap1, TileGap2, TileGap3,          // Tile gap (ITC & scint)
//       TileExt0, TileExt1, TileExt2,          // Tile extended barrel
//       FCAL0, FCAL1, FCAL2,                   // Forward EM endcap
//       Unknown
//     };
  // create dummy infinite IOV Range 
  // return what you have ...
  return std::make_pair(range, caloTrackingGeometry);
}

void Calo::CaloTrackingGeometryBuilderCond::registerInLayerIndexCaloSampleMap(
                                             Trk::LayerIndexSampleMap& licsMap,
                                             std::vector<CaloCell_ID::CaloSample> ccid,
                                             const Trk::TrackingVolume& vol,
                                             int side) const
{

  ATH_MSG_VERBOSE( "[+] Registering layers of TrackingVolume '" << vol.volumeName() << "' in LayerIndex/CaloCell_ID map" );   

  // get the confined Layers from the TrackingVolume
  const Trk::LayerArray* confinedLayers = vol.confinedLayers();
  if (!confinedLayers) return;
  
  const std::vector<const Trk::Layer*>& layerObjects = confinedLayers->arrayObjects();
  std::vector<const Trk::Layer*>::const_iterator layerObjIter = layerObjects.begin();
  std::vector<const Trk::Layer*>::const_iterator layerObjEnd  = layerObjects.end();
  
  // now pick out the material layers (and skip the navigation ones)
  std::vector<const Trk::Layer*> materialLayers;
  for ( ; layerObjIter != layerObjEnd; ++layerObjIter)
     if ((*layerObjIter)->layerMaterialProperties()) materialLayers.push_back((*layerObjIter));
  
  unsigned int matLaySize = materialLayers.size();
  unsigned int ccidSize   = ccid.size();
        
  ATH_MSG_VERBOSE( "[+] Found " << matLaySize << " material layers ( " << ccidSize << " CaloSample ids )" );  
  if ( matLaySize != ccidSize ) return;
  
  // everything's fine for side > 0
  if (side > 0){
     // match 1-to-1 
     std::vector<const Trk::Layer*>::iterator layerIt             = materialLayers.begin();
     std::vector<const Trk::Layer*>::iterator layerItEnd          = materialLayers.end();
     std::vector<CaloCell_ID::CaloSample>::iterator ccidIt    = ccid.begin();
     std::vector<CaloCell_ID::CaloSample>::iterator ccidItEnd = ccid.end();
          
     for ( ; layerIt != layerItEnd  && ccidIt != ccidItEnd; ++layerIt, ++ccidIt)
         licsMap.insert(std::make_pair((*layerIt)->layerIndex(),int(*ccidIt)));
       
  } else {  
    // the order needs to be reversed, because TG has z-ordering positive defined
    
     std::vector<CaloCell_ID::CaloSample>::iterator ccidIt    = ccid.begin();
     std::vector<CaloCell_ID::CaloSample>::iterator ccidItEnd = ccid.end();
          
     for ( ;ccidIt != ccidItEnd; ++ccidIt, --matLaySize)
         licsMap.insert(std::make_pair((materialLayers[matLaySize-1])->layerIndex(),int(*ccidIt)));    
  
  }
  
}

std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> Calo::CaloTrackingGeometryBuilderCond::createBeamPipeVolumes(float zmin, float zmax, 
															  const std::string& name,float& outerRadius ) const
{
  outerRadius = 0.;

  // dummy objects
  const Trk::LayerArray* dummyLayers = nullptr;
  const Trk::TrackingVolumeArray* dummyVolumes = nullptr;

  // beam pipe thickness along the z distance
  if (m_bpCutouts.empty()) {
    return std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> (0,0);
  }

  RZPairVector dim; 
  dim.push_back(RZPair(m_bpCutouts[0].first,zmin));
  float rOut = m_bpCutouts[0].first;
  for (unsigned int i=0; i<m_bpCutouts.size(); i++) {
    if (m_bpCutouts[i].second<=dim[0].second) dim[0].first=m_bpCutouts[i].first;
    else if ( m_bpCutouts[i].second<=zmax ) dim.push_back(m_bpCutouts[i]);
    if ( m_bpCutouts[i].second<=zmax ) rOut = m_bpCutouts[i].first;
  }
  
  if (dim.back().second < zmax) dim.push_back(RZPair(rOut,zmax));

  if (dim.size()==2) {   // simple case
    
    outerRadius = dim[0].first;
    
    Trk::CylinderVolumeBounds* bpBounds = new Trk::CylinderVolumeBounds( 0., outerRadius,
									 0.5*(zmax-zmin));
    
    Amg::Transform3D* bpPos = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,0.5*(zmin+zmax))));
    
    const Trk::TrackingVolume* bpVolPos = new Trk::TrackingVolume(bpPos, bpBounds,
								  *m_caloMaterial, 
								  dummyLayers, dummyVolumes, 
								  "BeamPipe::Positive"+name);

    Amg::Transform3D* bpNeg = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-0.5*(zmin+zmax))));
    
    const Trk::TrackingVolume* bpVolNeg = new Trk::TrackingVolume(bpNeg, bpBounds->clone(),
								  *m_caloMaterial, 
								  dummyLayers, dummyVolumes, 
								  "BeamPipe::Negative"+name);

    // geometry signature 
    bpVolPos->sign(Trk::BeamPipe);
    bpVolNeg->sign(Trk::BeamPipe);
    
    return std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> (bpVolPos,bpVolNeg); 

  }

  // cutout included

  outerRadius = dim[0].first;
  
  // find rMax
  for (unsigned int i=1; i<dim.size(); i++) if (dim[i].first > outerRadius) outerRadius = dim[i].first;
  
  // loop over z sections, prepare volumes for gluing
  std::vector<const Trk::TrackingVolume*> posVols;
  
  for (unsigned int i=0; i<dim.size()-1; i++) {
    
    if (dim[i].second == dim[i+1].second) continue;
    
    // beam pipe volume 
    Trk::CylinderVolumeBounds* bpBounds = new Trk::CylinderVolumeBounds( 0., dim[i].first,
									 0.5*(dim[i+1].second - dim[i].second));
    
    Amg::Transform3D* bpPos = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,0.5*(dim[i+1].second + dim[i].second))));
    
    const Trk::TrackingVolume* bpVolPos = new Trk::TrackingVolume(bpPos, bpBounds,
								  *m_caloMaterial, 
								  dummyLayers, dummyVolumes, 
								  "BeamPipe::Positive"+name);
    bpVolPos->sign(Trk::BeamPipe);
    
    const Trk::TrackingVolume* bpVolGap = nullptr;
    if ( dim[i].first < outerRadius ) {

      Trk::CylinderVolumeBounds* bpGB = new Trk::CylinderVolumeBounds( dim[i].first, outerRadius,
								       0.5*(dim[i+1].second - dim[i].second));

      Amg::Transform3D* bpPB = new Amg::Transform3D(*bpPos);

      bpVolGap = new Trk::TrackingVolume(bpPB, bpGB,
					 *m_caloMaterial, 
					 dummyLayers, dummyVolumes, 
					 "Calo::GapVolumes::Positive"+name); 
    }
    
    const Trk::TrackingVolume* bpSector = bpVolPos;
    
    if (bpVolGap) {
      std::vector<const Trk::TrackingVolume*> gapVols;
      gapVols.push_back(bpVolPos);
      gapVols.push_back(bpVolGap); 
      bpSector = m_trackingVolumeCreator->createContainerTrackingVolume(gapVols,
									*m_caloMaterial,
									"Calo::Container::PositiveBPSector"+name);
    }   
    posVols.push_back(bpSector);
  }

  //std::cout <<" processing complex beam pipe sector +: gluing in Z:"<< std::endl;
  //for (unsigned int i=0; i<posVols.size(); i++) std::cout <<i<<","<<posVols[i]->volumeName()<<","<<posVols[i]->center().z()<< std::endl;
  
  const Trk::TrackingVolume* bpPosSector = m_trackingVolumeCreator->createContainerTrackingVolume(posVols,
												  *m_caloMaterial,
												  "Calo::Container::PositiveBP"+name);

  //const Trk::CylinderVolumeBounds* testBounds 
  //    = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(bpPosSector->volumeBounds()));
  
  //if (testBounds) std::cout <<"positive BP sector created at z="<<bpPosSector->center().z()<<" and halfZ="<< testBounds->halflengthZ()<< std::endl;


  // loop over z sections, prepare volumes for gluing
  std::vector<const Trk::TrackingVolume*> negVols;
  
  for (unsigned int i=0; i<dim.size()-1; i++) {
    
    float zmax2 = -1.* (dim[i].second);
    float zmin2 = -1.* (dim[i+1].second);

    if (zmin2 == zmax2) continue;
    
    // beam pipe volume 
    Trk::CylinderVolumeBounds* bpBounds = new Trk::CylinderVolumeBounds( 0., dim[i].first,
									 0.5*(zmax2-zmin2));
    
    Amg::Transform3D* bpNeg = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,0.5*(zmin2+zmax2))));
    
    const Trk::TrackingVolume* bpVolNeg = new Trk::TrackingVolume(bpNeg, bpBounds,
								  *m_caloMaterial, 
								  dummyLayers, dummyVolumes, 
								  "BeamPipe::Negative"+name);
    bpVolNeg->sign(Trk::BeamPipe);
    
    const Trk::TrackingVolume* bpVolGap = dim[i].first < outerRadius ?
      new Trk::TrackingVolume(new Amg::Transform3D(*bpNeg),
			      new Trk::CylinderVolumeBounds( dim[i].first, outerRadius,
							     0.5*(zmax2 - zmin2)),
			      *m_caloMaterial, 
			      dummyLayers, dummyVolumes, 
			      "Calo::GapVolumes::Negative"+name)  : nullptr ;
    
    const Trk::TrackingVolume* bpSector = bpVolNeg;
    
    if (bpVolGap) {
      std::vector<const Trk::TrackingVolume*> gapVols;
      gapVols.push_back(bpVolNeg);
      gapVols.push_back(bpVolGap); 
      bpSector = m_trackingVolumeCreator->createContainerTrackingVolume(gapVols,
									*m_caloMaterial,
									"Calo::Container::NegativeBPSector"+name);
    }   

    if (negVols.empty()) negVols.push_back(bpSector);
    else negVols.insert(negVols.begin(),bpSector);
  }
  
  //std::cout <<" processing complex beam pipe sector -: gluing in Z:"<< std::endl;
  //for (unsigned int i=0; i<negVols.size(); i++) std::cout <<i<<","<<negVols[i]->volumeName()<<","<<negVols[i]->center().z()<< std::endl;

  const Trk::TrackingVolume* bpNegSector = m_trackingVolumeCreator->createContainerTrackingVolume(negVols,
												  *m_caloMaterial,
												  "Calo::Container::NegativeBP"+name);
  
  //testBounds 
  //    = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(bpNegSector->volumeBounds()));
  
  //if (testBounds) std::cout <<"negative BP sector created at z="<<bpNegSector->center().z()<<" and halfZ="<< testBounds->halflengthZ()<< std::endl;
  
  return std::pair<const Trk::TrackingVolume*,const Trk::TrackingVolume*> (bpPosSector,bpNegSector);

}



