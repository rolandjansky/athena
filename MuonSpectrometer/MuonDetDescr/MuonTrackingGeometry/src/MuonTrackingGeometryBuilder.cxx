/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MuonTrackingGeometryBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Muon
#include "MuonTrackingGeometry/MuonTrackingGeometryBuilder.h"
#include "MuonReadoutGeometry/GlobalUtilities.h" 
// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Units
#include "GaudiKernel/SystemOfUnits.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/BinnedArray2D.h"
#include "TrkDetDescrUtils/BinnedArray1D1D1D.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
#include "TrkVolumes/DoubleTrapezoidVolumeBounds.h"
#include "TrkVolumes/BevelledCylinderVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/SubtractedVolumeBounds.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkVolumes/PrismVolumeBounds.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
#include "TrkGeometry/Material.h"
#include<fstream>

// STD
#include <map>


// constructor
Muon::MuonTrackingGeometryBuilder::MuonTrackingGeometryBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_stationBuilder("Muon::MuonStationBuilder/MuonStationBuilder"),
  m_inertBuilder("Muon::MuonInertMaterialBuilder/MuonInertMaterialBuilder"),
  m_trackingVolumeArrayCreator("Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"),
  m_trackingVolumeHelper("Trk::TrackingVolumeHelper/TrackingVolumeHelper"),
  m_enclosingEnvelopeSvc("AtlasEnvelopeDefSvc", n),
  m_muonSimple(false),
  m_loadMSentry(false),
  m_muonActive(true),
  m_muonInert(true),
  m_innerBarrelRadius(4255.),
  //  m_outerBarrelRadius(13400.),
  m_outerBarrelRadius(13910.),
  //  m_barrelZ(6783.),
  m_barrelZ(6785.),
  m_innerEndcapZ(12900.),
  m_outerEndcapZ(26046.),
  m_bigWheel(15600.),
  m_outerWheel(21000.),
  m_ectZ(7920.),
  m_beamPipeRadius(70.),
  //m_innerShieldRadius(810.),
  m_innerShieldRadius(850.),
  m_outerShieldRadius(1500.),
  m_diskShieldZ(6915.),
  m_standaloneTrackingVolume(0),
  m_barrelEtaPartition(9),
  m_innerEndcapEtaPartition(3),
  m_outerEndcapEtaPartition(3),
  m_phiPartition(16),
  m_adjustStatic(true),
  m_static3d(true),
  m_blendInertMaterial(false),
  m_removeBlended(false),
  m_inertPerm(0),
  m_alignTolerance(0.),
  m_colorCode(0),
  m_activeAdjustLevel(2),
  m_inertAdjustLevel(1),
  m_frameNum(0),
  m_frameStat(0),
  m_entryVolume("MuonSpectrometerEntrance"),
  m_exitVolume("All::Container::CompleteDetector"),
  m_chronoStatSvc( "ChronoStatSvc", n )
{
  m_stationSpan = 0;
  m_inertSpan = 0;
  m_stations = 0;
  m_inertObjs = 0;
  m_blendMap.clear();
  m_blendVols.clear(); 

  declareInterface<Trk::IGeometryBuilder>(this);

  declareProperty("SimpleMuonGeometry",               m_muonSimple);  
  declareProperty("LoadMSEntry",                      m_loadMSentry);  
  declareProperty("BuildActiveMaterial",              m_muonActive);  
  declareProperty("BuildInertMaterial",               m_muonInert);
  declareProperty("InertMaterialBuilder",             m_inertBuilder);
  // the overall dimensions
  declareProperty("EnvelopeDefinitionSvc",            m_enclosingEnvelopeSvc );
  declareProperty("InnerBarrelRadius",                m_innerBarrelRadius);
  declareProperty("OuterBarrelRadius",                m_outerBarrelRadius);
  declareProperty("BarrelZ",                          m_barrelZ);
  declareProperty("InnerEndcapZ",                     m_innerEndcapZ);
  declareProperty("OuterEndcapZ",                     m_outerEndcapZ);
  declareProperty("EtaBarrelPartitions",              m_barrelEtaPartition);
  declareProperty("EtaInnerEndcapPartitions",         m_innerEndcapEtaPartition);
  declareProperty("EtaOuterEndcapPartitions",         m_outerEndcapEtaPartition);
  declareProperty("PhiPartitions",                    m_phiPartition);
  declareProperty("AdjustStatic",                     m_adjustStatic);
  declareProperty("StaticPartition3D",                m_static3d);
  declareProperty("ActiveAdjustLevel",                m_activeAdjustLevel);
  declareProperty("InertAdjustLevel",                 m_inertAdjustLevel);
  declareProperty("BlendInertMaterial",               m_blendInertMaterial);
  declareProperty("RemoveBlendedMaterialObjects",     m_removeBlended);
  declareProperty("AlignmentPositionTolerance",       m_alignTolerance);
  declareProperty("ColorCode",                        m_colorCode);
  // calo entry volume & exit volume
  declareProperty("EntryVolumeName",                   m_entryVolume);
  declareProperty("ExitVolumeName",                    m_exitVolume);  
}

// destructor
Muon::MuonTrackingGeometryBuilder::~MuonTrackingGeometryBuilder()
{}

// Athena standard methods
// initialize
StatusCode Muon::MuonTrackingGeometryBuilder::initialize()
{
 
  // Retrieve the tracking volume helper   -------------------------------------------------    
  if (m_trackingVolumeHelper.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeHelper );
      return StatusCode::FAILURE;
    } else
    ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeHelper );
  
  // Retrieve the tracking volume array creator   -------------------------------------------    
  if (m_trackingVolumeArrayCreator.retrieve().isFailure())
    {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeArrayCreator );
      return StatusCode::FAILURE;
    } else
    ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeArrayCreator );
  
  // Retrieve the station builder (if configured) -------------------------------------------    
  if (m_muonActive) { 
    
    if (m_stationBuilder.retrieve().isFailure())
      {
	ATH_MSG_ERROR( "Failed to retrieve tool " << m_stationBuilder <<" Creation of stations might fail." );
      } else
      ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeArrayCreator );
  } else {
    m_activeAdjustLevel = 0;                // no active material to consider
  }
  
  // Retrieve the inert material builder builder (if configured) -------------------------------------------    
  
  if (m_muonInert || m_blendInertMaterial) {
    
    if (m_inertBuilder.retrieve().isFailure())
      {
	ATH_MSG_ERROR( "Failed to retrieve tool " << m_inertBuilder <<"Creation of inert material objects might fail." );
      } else
      ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeArrayCreator );
  }
  
  if ( !m_muonInert ) m_inertAdjustLevel = 0;
  
  if (m_loadMSentry ) {
    // retrieve envelope definition service --------------------------------------------------
    if ( m_enclosingEnvelopeSvc.retrieve().isFailure() ){
      ATH_MSG_WARNING( "Could not retrieve EnvelopeDefinition service. Using default definition.");
      m_loadMSentry = false;
    }
  }

  if (m_chronoStatSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING( "Could not retrieve Tool " << m_chronoStatSvc << ". Exiting.");
  }
  
  
  ATH_MSG_INFO( name() <<" initialize() successful" );    
    
  return StatusCode::SUCCESS;
}


const Trk::TrackingGeometry* Muon::MuonTrackingGeometryBuilder::trackingGeometry(const Trk::TrackingVolume* tvol) const
{
  
  ATH_MSG_INFO( name() <<" building tracking geometry" );    
  m_chronoStatSvc->chronoStart("MS::build-up");
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  // check setup
  if (m_muonInert && m_blendInertMaterial) {
    if (!m_adjustStatic || !m_static3d) {
      ATH_MSG_INFO( name() <<" diluted inert material hardcoded for 3D volume frame, adjusting setup" );
      m_adjustStatic = true;
      m_static3d = true;
    }
  }
  // process muon material objects
  if (m_muonActive && m_stationBuilder && !m_stations) m_stations = m_stationBuilder->buildDetachedTrackingVolumes();
  if (m_muonInert && m_inertBuilder && !m_inertObjs) m_inertObjs = m_inertBuilder->buildDetachedTrackingVolumes(m_blendInertMaterial);
  if (m_inertObjs && m_blendInertMaterial && m_removeBlended) {
    while ( m_inertPerm<m_inertObjs->size() 
	    && (*m_inertObjs)[m_inertPerm]->name().substr((*m_inertObjs)[m_inertPerm]->name().size()-4,4)=="PERM") m_inertPerm++;
  }
  
  // find object's span with tolerance for the alignment 
  if (!m_stationSpan) m_stationSpan = findVolumesSpan(m_stations, 100.*m_alignTolerance, m_alignTolerance*Gaudi::Units::deg);
  if (!m_inertSpan)   m_inertSpan = findVolumesSpan(m_inertObjs,0.,0.);
 
  // 0) Preparation //////////////////////////////////////////////////////////////////////////////////////
  
  m_muonMaterial = Trk::Material(10e10,10e10,0.,0.,0.);      // default material properties
    
  // dummy substructures
  const Trk::LayerArray* dummyLayers = 0;
  const Trk::TrackingVolumeArray* dummyVolumes = 0;


  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  //   Envelope definition (cutouts)
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  RZPairVector envelopeDefs;
  if (m_enclosingEnvelopeSvc) {
    // get the dimensions from the envelope service
    RZPairVector& envelopeDefsIn = m_enclosingEnvelopeSvc->getMuonRZValues();

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
    if (irot>0) {
      for (unsigned int i=inext; i<envelopeDefsIn.size(); i++) envelopeDefs.push_back(envelopeDefsIn[i]); 
      if (inext>0) for (unsigned int i=0; i<=inext-1; i++) envelopeDefs.push_back(envelopeDefsIn[i]); 
    } else {
      int i=inext;  while (i>=0) {envelopeDefs.push_back(envelopeDefsIn[i]); i=i-1;};
      inext = envelopeDefsIn.size()-1; while (inext>=ii) {envelopeDefs.push_back(envelopeDefsIn[inext]); inext=inext-1;};
    } 

    // find maximal z,R extent
    float maxR = 0.; 
    for (unsigned int i=0; i<envelopeDefs.size(); i++) {
      if (envelopeDefs[i].first > maxR )  maxR = envelopeDefs[i].first; 
    } 
    
    m_outerBarrelRadius = maxR;
    m_outerEndcapZ      = envelopeDefs[0].second;

    ATH_MSG_VERBOSE( "Muon envelope definition retrieved: outer R,Z:"<<m_outerBarrelRadius<<","<< m_outerEndcapZ );  

    // construct inner and outer envelope
    
    for (unsigned int i=0; i<envelopeDefs.size(); i++) {
      ATH_MSG_VERBOSE( "Rz pair:"<< i<<":"<< envelopeDefs[i].first<<","<<envelopeDefs[i].second );   
    }

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  if (m_muonSimple) {             
    Trk::VolumeBounds* globalBounds = new Trk::CylinderVolumeBounds(m_outerBarrelRadius,
								    m_outerEndcapZ);
    Trk::TrackingVolume* topVolume = new Trk::TrackingVolume(0,
							     globalBounds,
							     m_muonMaterial,
							     dummyLayers,dummyVolumes,
							     "GlobalVolume");
    m_standaloneTrackingVolume = topVolume;
    return new Trk::TrackingGeometry(topVolume);
  }     
  
  ATH_MSG_INFO( name() <<"building barrel+innerEndcap+outerEndcap" );    
  
////////////////////////////////////////////////////////////////////////////////////////////////////////
// MuonSpectrometer contains:
//       - Barrel
//       - Endcaps inner/outer
  const Trk::TrackingVolume* muonBarrel = 0;
  Trk::CylinderVolumeBounds* barrelBounds = 0;
  const Trk::TrackingVolume* negativeMuonOuterWheel = 0;
  Trk::CylinderVolumeBounds* negativeOuterWheelBounds = 0;
  const Trk::TrackingVolume* negativeMuonBigWheel = 0;
  Trk::CylinderVolumeBounds* negativeBigWheelBounds = 0;
  const Trk::TrackingVolume* negativeMuonOuterBuffer = 0;
  Trk::CylinderVolumeBounds* negativeOuterBufferBounds = 0;
  const Trk::TrackingVolume* positiveMuonOuterWheel = 0;
  const Trk::TrackingVolume* negativeMuonSmallWheel = 0;
  const Trk::TrackingVolume* positiveMuonSmallWheel = 0;
  Trk::CylinderVolumeBounds* negativeSmallWheelBounds = 0;
  const Trk::TrackingVolume* negativeECT = 0;
  const Trk::TrackingVolume* positiveECT = 0;
  Trk::CylinderVolumeBounds* negativeECTBounds = 0;
  const Trk::TrackingVolume* positiveMuonBigWheel = 0;
  const Trk::TrackingVolume* positiveMuonOuterBuffer = 0;
// volumes needed to close the geometry
  const Trk::TrackingVolume* negBeamPipe = 0;
  const Trk::TrackingVolume* posBeamPipe = 0;
  Trk::CylinderVolumeBounds* negBeamPipeBounds = 0;
  Trk::CylinderVolumeBounds* posBeamPipeBounds = 0;
  const Trk::TrackingVolume* enclosed = 0;
  Trk::CylinderVolumeBounds* enclosedBounds = 0;
  const Trk::TrackingVolume* negDiskShield = 0;
  Trk::CylinderVolumeBounds* negDiskShieldBounds = 0;
  const Trk::TrackingVolume* posDiskShield = 0;
  Trk::CylinderVolumeBounds* posDiskShieldBounds = 0;
  const Trk::TrackingVolume* negInnerShield = 0;
  const Trk::TrackingVolume* posInnerShield = 0;
  Trk::CylinderVolumeBounds* negInnerShieldBounds = 0;
  Trk::CylinderVolumeBounds* posInnerShieldBounds = 0;
  const Trk::TrackingVolume* negOuterShield = 0;
  const Trk::TrackingVolume* posOuterShield = 0;
  Trk::CylinderVolumeBounds* negOuterShieldBounds = 0;
  Trk::CylinderVolumeBounds* posOuterShieldBounds = 0;
// if input, redefine dimensions to fit expected MS entry 
  if (tvol){
    bool msEntryDefined = false;
    if ( tvol->volumeName() == m_entryVolume ) msEntryDefined = true; 
    // get dimensions
    ATH_MSG_DEBUG(" msEntryDefined " << msEntryDefined);
    const Trk::CylinderVolumeBounds* enclosedDetectorBounds 
      = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(tvol->volumeBounds()));
    if (!enclosedDetectorBounds) {    
      ATH_MSG_ERROR(" dynamic cast of enclosed volume to the cylinder bounds failed, aborting MTG build-up ");
      return 0;
    }
    double enclosedDetectorHalfZ = enclosedDetectorBounds->halflengthZ();
    double enclosedDetectorOuterRadius = enclosedDetectorBounds->outerRadius();
    // get subvolumes at navigation level and check THEIR dimensions      
    const Trk::GlueVolumesDescriptor& enclosedDetGlueVolumes = tvol->glueVolumesDescriptor();
    std::vector<const Trk::TrackingVolume*> enclosedCentralFaceVolumes = enclosedDetGlueVolumes.glueVolumes(Trk::cylinderCover);
    std::vector<const Trk::TrackingVolume*> enclosedNegativeFaceVolumes = enclosedDetGlueVolumes.glueVolumes(Trk::negativeFaceXY);
    std::vector<const Trk::TrackingVolume*> enclosedPositiveFaceVolumes = enclosedDetGlueVolumes.glueVolumes(Trk::positiveFaceXY);
    if (enclosedCentralFaceVolumes.size()) {
      const Trk::CylinderVolumeBounds* cylR = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(enclosedCentralFaceVolumes[0]->volumeBounds()));
      if (cylR && cylR->outerRadius() != enclosedDetectorOuterRadius ) {
        enclosedDetectorOuterRadius = cylR->outerRadius();
        ATH_MSG_WARNING(name() << " enclosed volume envelope outer radius does not correspond to radius of glue volumes : adjusted " );
      }
    }
    if (enclosedNegativeFaceVolumes.size() && enclosedPositiveFaceVolumes.size()) {
      double negZ = -enclosedDetectorHalfZ;
      double posZ =  enclosedDetectorHalfZ;
      const Trk::CylinderVolumeBounds* cylN = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(enclosedNegativeFaceVolumes[0]->volumeBounds()));
      if (cylN) negZ = enclosedNegativeFaceVolumes[0]->center()[2]-cylN->halflengthZ();
      const Trk::CylinderVolumeBounds* cylP = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(enclosedPositiveFaceVolumes[0]->volumeBounds()));
      if (cylP) posZ = enclosedPositiveFaceVolumes[0]->center()[2]+cylP->halflengthZ();
      if ( fabs(negZ+enclosedDetectorHalfZ)>0.001 ||  fabs(posZ-enclosedDetectorHalfZ)>0.001 ) {
	ATH_MSG_WARNING( name() << " enclosed volume envelope z dimension does not correspond to that of glue volumes " );
        if ( fabs(negZ+posZ)<0.001) {
          enclosedDetectorHalfZ = posZ;
          ATH_MSG_WARNING( name() << " z adjusted " );
	} else {
	  ATH_MSG_ERROR( name() << "assymetric Z dimensions - cannot recover "<< negZ <<","<< posZ );
          return 0;
	}
     }
    }
    //
    

    // 
    ATH_MSG_INFO( name() <<" dimensions of enclosed detectors (halfZ,outerR):"
	<<  enclosedDetectorHalfZ<<","<< enclosedDetectorOuterRadius );    
    // check if input makes sense - gives warning if cuts into muon envelope
    // adjust radius
    const Trk::TrackingVolume* barrelR = 0;
    if ( enclosedDetectorOuterRadius > m_innerBarrelRadius ) {
      ATH_MSG_WARNING( name() <<" enclosed volume too wide, cuts into muon envelope, abandon :R:" <<enclosedDetectorOuterRadius );
      return 0;   
    } else { 
      m_innerBarrelRadius = enclosedDetectorOuterRadius;
      barrelR = tvol;      
    }
    // adjust z
    if ( enclosedDetectorHalfZ > m_barrelZ ) {
      ATH_MSG_WARNING( name() <<" enclosed volume too long, cuts into muon envelope, abandon :Z:"<< enclosedDetectorHalfZ );    
      return 0;
    } else {    
      if ( enclosedDetectorHalfZ < m_barrelZ ) {
	Trk::CylinderVolumeBounds* barrelZPBounds = new Trk::CylinderVolumeBounds(m_innerBarrelRadius,
										 0.5*(m_barrelZ - enclosedDetectorHalfZ) );
	Trk::CylinderVolumeBounds* barrelZMBounds = new Trk::CylinderVolumeBounds(m_innerBarrelRadius,
										 0.5*(m_barrelZ - enclosedDetectorHalfZ) );
	double zbShift =  0.5*(m_barrelZ + enclosedDetectorHalfZ);
	const Trk::TrackingVolume* barrelZPBuffer = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,zbShift))),
									    barrelZPBounds,
									    m_muonMaterial,
									    dummyLayers,dummyVolumes,
									    "BarrelRZPosBuffer");
	const Trk::TrackingVolume* barrelZMBuffer = new Trk::TrackingVolume(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-zbShift))),
									    barrelZMBounds,
									    m_muonMaterial,
									    dummyLayers,dummyVolumes,
									    "BarrelRZNegBuffer");
	
	ATH_MSG_INFO( name() << "glue barrel R  + barrel Z buffer" );
	const Trk::TrackingVolume* barrelZP = m_trackingVolumeHelper->glueTrackingVolumeArrays(*barrelR, Trk::positiveFaceXY,
											       *barrelZPBuffer,Trk::negativeFaceXY, 
											       "All::Gaps::BarrelZP");    
        // set name
	std::string nameEncl = msEntryDefined ? "All::Gaps::Barrel" : m_entryVolume ;
        ATH_MSG_DEBUG( " nameEncl " << nameEncl);
	enclosed = m_trackingVolumeHelper->glueTrackingVolumeArrays(*barrelZP, Trk::negativeFaceXY,
								    *barrelZMBuffer,Trk::positiveFaceXY, 
								    nameEncl);    
 
      } else enclosed = barrelR;
      // ST the following simplification forces MuonSpectrometerEntrance to coincide with Calo::Container
      //m_barrelZ = enclosedDetectorHalfZ;
      //enclosed = barrelR;      
    }
    
  } else {     // no input, create the enclosed volume
    if (m_loadMSentry && m_enclosingEnvelopeSvc ) {
      RZPairVector& envelopeDefs = m_enclosingEnvelopeSvc->getCaloRZValues();
      // to be implemented in detail - for the moment, take just maximal extent
      ATH_MSG_DEBUG( " m_loadMSentry " << m_loadMSentry << " m_enclosingEnvelopeSvc " << m_enclosingEnvelopeSvc );
      double rmax = 0.; double zmax = 0.;
      for (unsigned int i=0; i<envelopeDefs.size(); i++) { 
	if ( envelopeDefs[i].first>rmax ) rmax= envelopeDefs[i].first;
	if ( fabs(envelopeDefs[i].second)>zmax ) zmax= fabs(envelopeDefs[i].second);
      }
      if (envelopeDefs.size()) {
        if ( rmax>0. && rmax <= m_innerBarrelRadius && zmax>0. && zmax <= m_barrelZ ) {
	  enclosedBounds = new Trk::CylinderVolumeBounds(rmax,zmax);
	} else {
	  ATH_MSG_INFO( name() << " input MSEntrance size (R,Z:"<< rmax <<","<<zmax<<") clashes with MS material, switch to default values (R,Z:" << m_innerBarrelRadius <<","<< m_barrelZ << ")" );
	}
      }
    }
    
    if (!enclosedBounds) enclosedBounds = new Trk::CylinderVolumeBounds(m_innerBarrelRadius,
									m_barrelZ);
    enclosed = new Trk::TrackingVolume(0,
				       enclosedBounds,
				       m_muonMaterial,
				       dummyLayers,dummyVolumes,
				       m_entryVolume);
    enclosed->registerColorCode(0); 
    ATH_MSG_DEBUG( " register Barrel m_entryVolume " << m_entryVolume);
  }

  // construct inner and outer envelope
    
  for (unsigned int i=0; i<envelopeDefs.size(); i++) {
    //ATH_MSG_VERBOSE( "Rz pair:"<< i<<":"<< envelopeDefs[i].first<<","<<envelopeDefs[i].second );   
    if (m_msCutoutsIn.size() && m_msCutoutsIn.back().second== -m_outerEndcapZ) break;
    if ( !m_msCutoutsIn.size() || fabs(m_msCutoutsIn.back().second) > m_barrelZ
	 || fabs(envelopeDefs[i].second)> m_barrelZ ) m_msCutoutsIn.push_back(envelopeDefs[i]); 
    else if (m_msCutoutsIn.size() && m_msCutoutsIn.back().second==m_barrelZ && m_msCutoutsIn.back().first!=m_innerBarrelRadius) {
      m_msCutoutsIn.push_back(RZPair(m_innerBarrelRadius,m_barrelZ));
      m_msCutoutsIn.push_back(RZPair(m_innerBarrelRadius,-m_barrelZ));
      m_msCutoutsIn.push_back(RZPair(m_msCutoutsIn[m_msCutoutsIn.size()-3].first,-m_barrelZ));
    }
  }

  unsigned int il=1; 
  while (envelopeDefs[il-1].second!=-m_outerEndcapZ) il++;
  for ( ; il<envelopeDefs.size(); il++)  m_msCutoutsOut.push_back(envelopeDefs[il]); 

  for (unsigned int i=0; i<m_msCutoutsIn.size(); i++) {
    ATH_MSG_VERBOSE( "Rz pair for inner MS envelope:"<< i<<":"<< m_msCutoutsIn[i].first<<","<<m_msCutoutsIn[i].second );   
  }
  for (unsigned int i=0; i<m_msCutoutsOut.size(); i++) {
    ATH_MSG_VERBOSE( "Rz pair for outer MS envelope:"<< i<<":"<< m_msCutoutsOut[i].first<<","<<m_msCutoutsOut[i].second );   
  }

  if ( m_msCutoutsIn[5].second != m_innerEndcapZ) {
    m_innerEndcapZ = m_msCutoutsIn[5].second;
  }
  ATH_MSG_VERBOSE( "inner endcap Z set to:"<< m_innerEndcapZ );   
  
  // create central volume ("enclosed" + disk shields ) - this is to allow safe gluing with 3D MS binning
  getShieldParts();
  
  negDiskShieldBounds = new Trk::CylinderVolumeBounds(m_innerBarrelRadius,
						      0.5*(m_diskShieldZ - m_barrelZ) );
  Amg::Vector3D negDiskShieldPosition(0.,0.,-0.5*(m_diskShieldZ+m_barrelZ));
  Trk::Volume negDiskVol(new Amg::Transform3D(Amg::Translation3D(negDiskShieldPosition)),negDiskShieldBounds);
  negDiskShield = processShield(&negDiskVol,2,"Muons::Detectors::NegativeDiskShield");

  posDiskShieldBounds = new Trk::CylinderVolumeBounds(m_innerBarrelRadius,
						      0.5*(m_diskShieldZ - m_barrelZ) );
  Amg::Vector3D posDiskShieldPosition(0.,0., 0.5*(m_diskShieldZ+m_barrelZ));
  Trk::Volume posDiskVol(new Amg::Transform3D(Amg::Translation3D(posDiskShieldPosition)),posDiskShieldBounds);
  posDiskShield = processShield(&posDiskVol,2,"Muons::Detectors::PositiveDiskShield");
	   
  ATH_MSG_INFO( name() << "glue enclosed  + disk shields" );
  const Trk::TrackingVolume* centralP = m_trackingVolumeHelper->glueTrackingVolumeArrays(*enclosed, Trk::positiveFaceXY,
											 *posDiskShield,Trk::negativeFaceXY, 
											 "Muon::Container::CentralP");    
  const Trk::TrackingVolume* central  = m_trackingVolumeHelper->glueTrackingVolumeArrays(*centralP, Trk::negativeFaceXY,
											 *negDiskShield,Trk::positiveFaceXY, 
											 "Muon::Container::Central");    
  
// define basic volumes
  if (m_adjustStatic) { getZParts(); getHParts();}

// muon barrel
  barrelBounds = new Trk::CylinderVolumeBounds(m_innerBarrelRadius,
					       m_outerBarrelRadius,
					       m_diskShieldZ);
  Trk::Volume barrelVol(0,barrelBounds);
// process volume
// barrel
  if (m_adjustStatic && m_static3d) muonBarrel = processVolume( &barrelVol,0,"Muon::Detectors::Barrel"); 
  else if (m_adjustStatic) muonBarrel = processVolume( &barrelVol,-1,"Muon::Detectors::Barrel"); 
  else muonBarrel = processVolume( &barrelVol,m_barrelEtaPartition,m_phiPartition,"Muon::Detectors::Barrel"); 
// inner Endcap
// build as smallWheel+ECT
// small wheel
   double smallWheelZHalfSize = 0.5*(m_ectZ - m_diskShieldZ);
   negativeSmallWheelBounds = new Trk::CylinderVolumeBounds(m_innerShieldRadius,
							    m_outerBarrelRadius,
							    smallWheelZHalfSize);
   Amg::Vector3D negSmallWheelPosition(0.,0.,-m_ectZ+smallWheelZHalfSize);
   Amg::Transform3D* negSmallWheelTransf = new Amg::Transform3D(Amg::Translation3D(negSmallWheelPosition));
   Trk::Volume negSWVol(negSmallWheelTransf,negativeSmallWheelBounds);
   if (m_adjustStatic && m_static3d) negativeMuonSmallWheel = processVolume( &negSWVol,1,"Muon::Detectors::NegativeSmallWheel" ); 
   else if (m_adjustStatic) negativeMuonSmallWheel = processVolume( &negSWVol,-1,"Muon::Detectors::NegativeSmallWheel" ); 
   else negativeMuonSmallWheel = processVolume( &negSWVol,m_innerEndcapEtaPartition,m_phiPartition,
						"Muon::Detectors::NegativeSmallWheel" ); 
   //checkVolume(negativeMuonSmallWheel);
   //
   Amg::Vector3D posSmallWheelShift(0.,0.,2*(m_ectZ-smallWheelZHalfSize));
   Trk::Volume posSWVol(negSWVol,Amg::Transform3D(Amg::Translation3D(posSmallWheelShift)));
   if (m_adjustStatic && m_static3d) positiveMuonSmallWheel = processVolume( &posSWVol,1,"Muon::Detectors::PositiveSmallWheel" ); 
   else if (m_adjustStatic) positiveMuonSmallWheel = processVolume( &posSWVol,-1,"Muon::Detectors::PositiveSmallWheel" ); 
   else positiveMuonSmallWheel = processVolume( &posSWVol,m_innerEndcapEtaPartition,m_phiPartition,
						 "Muon::Detectors::PositiveSmallWheel" ); 
   //checkVolume(positiveMuonSmallWheel);
// ECT
   double ectZHalfSize = 0.5*(m_innerEndcapZ - m_ectZ);
   negativeECTBounds = new Trk::CylinderVolumeBounds(m_innerShieldRadius,
						     m_outerBarrelRadius,
						     ectZHalfSize);
   Amg::Vector3D negECTPosition(0.,0.,-m_ectZ-ectZHalfSize);
   Amg::Transform3D* negECTTransf = new Amg::Transform3D(Amg::Translation3D(negECTPosition));
   Trk::Volume negECTVol(negECTTransf,negativeECTBounds);
   if (m_adjustStatic && m_static3d) negativeECT = processVolume( &negECTVol,2,"Muon::Detectors::NegativeECT" ); 
   else if (m_adjustStatic) negativeECT = processVolume( &negECTVol,-1,"Muon::Detectors::NegativeECT" ); 
   else negativeECT = processVolume( &negECTVol,m_innerEndcapEtaPartition,m_phiPartition,
						"Muon::Detectors::NegativeECT" ); 
   //checkVolume(negativeECT);
   //
   Amg::Vector3D posECTShift(0.,0.,2*(m_ectZ+ectZHalfSize));
   Trk::Volume posECTVol(negECTVol,Amg::Transform3D(Amg::Translation3D(posECTShift)));
   if (m_adjustStatic && m_static3d) positiveECT = processVolume( &posECTVol,2,"Muon::Detectors::PositiveECT" ); 
   else if (m_adjustStatic) positiveECT = processVolume( &posECTVol,-1,"Muon::Detectors::PositiveECT" ); 
   else positiveECT = processVolume( &posECTVol,m_innerEndcapEtaPartition,m_phiPartition,
						"Muon::Detectors::PositiveECT" ); 
   //checkVolume(positiveECT);
   // glue
   const Trk::TrackingVolume* negativeMuonInnerEndcap =
     m_trackingVolumeHelper->glueTrackingVolumeArrays(*negativeECT, Trk::positiveFaceXY,
						      *negativeMuonSmallWheel, Trk::negativeFaceXY, 
						      "Muon::Container::NegInnerEndcap");  
   const Trk::TrackingVolume* positiveMuonInnerEndcap = 
     m_trackingVolumeHelper->glueTrackingVolumeArrays(*positiveMuonSmallWheel, Trk::positiveFaceXY,
						      *positiveECT, Trk::negativeFaceXY,
						      "Muon::Container::PosInnerEndcap");  
   
// inner shields   
   double innerEndcapZHalfSize = 0.5*(m_innerEndcapZ - m_diskShieldZ);
   negInnerShieldBounds = new Trk::CylinderVolumeBounds(m_beamPipeRadius,
							m_innerShieldRadius,
							innerEndcapZHalfSize);
   Amg::Vector3D negInnerEndcapPosition(0.,0.,-m_diskShieldZ-innerEndcapZHalfSize);
   Trk::Volume negisVol(new Amg::Transform3D(Amg::Translation3D(negInnerEndcapPosition)),negInnerShieldBounds);
   negInnerShield = processShield(&negisVol,1,"Muons::Detectors::NegativeInnerShield");

   posInnerShieldBounds = new Trk::CylinderVolumeBounds(m_beamPipeRadius,
							m_innerShieldRadius,
							innerEndcapZHalfSize);
   Amg::Vector3D posInnerEndcapPosition(0.,0.,m_diskShieldZ+innerEndcapZHalfSize);
   Trk::Volume posisVol(new Amg::Transform3D(Amg::Translation3D(posInnerEndcapPosition)),posInnerShieldBounds);
   posInnerShield = processShield(&posisVol,1,"Muons::Detectors::PositiveInnerShield");

// outer Endcap
// build as bigWheel+buffer+outerWheel
// outer wheel
   double outerWheelZHalfSize = 0.5*(m_outerEndcapZ - m_outerWheel);
   negativeOuterWheelBounds = new Trk::CylinderVolumeBounds(m_outerShieldRadius,
							    m_outerBarrelRadius,
							    outerWheelZHalfSize);
   Amg::Vector3D negOuterWheelPosition(0.,0.,-m_outerEndcapZ+outerWheelZHalfSize);
   Amg::Transform3D* negOuterWheelTransf = new Amg::Transform3D(Amg::Translation3D(negOuterWheelPosition));
   Trk::Volume negOWVol(negOuterWheelTransf,negativeOuterWheelBounds);
   if (m_adjustStatic && m_static3d) negativeMuonOuterWheel = processVolume( &negOWVol,3,"Muon::Detectors::NegativeOuterWheel" ); 
   else if (m_adjustStatic) negativeMuonOuterWheel = processVolume( &negOWVol,-1,"Muon::Detectors::NegativeOuterWheel" ); 
   else negativeMuonOuterWheel = processVolume( &negOWVol,m_outerEndcapEtaPartition,m_phiPartition,
						"Muon::Detectors::NegativeOuterWheel" ); 
   //
   Amg::Vector3D posOuterWheelShift(0.,0.,2*(m_outerEndcapZ-outerWheelZHalfSize));
   Trk::Volume posOWVol(negOWVol,Amg::Transform3D(Amg::Translation3D(posOuterWheelShift)));
   if (m_adjustStatic && m_static3d) positiveMuonOuterWheel = processVolume( &posOWVol,3,"Muon::Detectors::PositiveOuterWheel" ); 
   else if (m_adjustStatic) positiveMuonOuterWheel = processVolume( &posOWVol,-1,"Muon::Detectors::PositiveOuterWheel" ); 
   else positiveMuonOuterWheel = processVolume( &posOWVol,m_outerEndcapEtaPartition,m_phiPartition,
						 "Muon::Detectors::PositiveOuterWheel" ); 
// outer buffer
   double outerBufferZHalfSize = 0.5*(m_outerWheel - m_bigWheel);
   negativeOuterBufferBounds = new Trk::CylinderVolumeBounds(m_outerShieldRadius,
							     m_outerBarrelRadius,
							     outerBufferZHalfSize);
   Amg::Vector3D negOuterBufferPosition(0.,0.,-m_bigWheel-outerBufferZHalfSize);
   Amg::Transform3D* negOuterBufferTransf = new Amg::Transform3D(Amg::Translation3D(negOuterBufferPosition));
   Trk::Volume negBuffVol(negOuterBufferTransf,negativeOuterBufferBounds);
   if (m_adjustStatic && m_static3d) negativeMuonOuterBuffer = processVolume( &negBuffVol,3,"Muon::Detectors::NegativeOuterBuffer" ); 
   else if (m_adjustStatic) negativeMuonOuterBuffer = processVolume( &negBuffVol,-1,"Muon::Detectors::NegativeOuterBuffer" ); 
   else negativeMuonOuterBuffer = processVolume( &negBuffVol,m_outerEndcapEtaPartition,m_phiPartition,
						"Muon::Detectors::NegativeOuterBuffer" ); 
   //
   Amg::Vector3D posOuterBufferShift(0.,0.,2*(m_bigWheel+outerBufferZHalfSize));
   Trk::Volume posBuffVol(negBuffVol,Amg::Transform3D(Amg::Translation3D(posOuterBufferShift)));
   if (m_adjustStatic && m_static3d) positiveMuonOuterBuffer = processVolume( &posBuffVol,3,"Muon::Detectors::PositiveOuterBuffer" ); 
   else if (m_adjustStatic) positiveMuonOuterBuffer = processVolume( &posBuffVol,-1,"Muon::Detectors::PositiveOuterBuffer" ); 
   else positiveMuonOuterBuffer = processVolume( &posBuffVol,m_outerEndcapEtaPartition,m_phiPartition,
						"Muon::Detectors::PositiveOuterBuffer" ); 
// big wheel
   double bigWheelZHalfSize = 0.5*(m_bigWheel - m_innerEndcapZ);
   negativeBigWheelBounds = new Trk::CylinderVolumeBounds(m_outerShieldRadius,
							  m_outerBarrelRadius,
							  bigWheelZHalfSize);
   Amg::Vector3D negBigWheelPosition(0.,0.,-m_innerEndcapZ-bigWheelZHalfSize);
   Amg::Transform3D* negBigWheelTransf = new Amg::Transform3D(Amg::Translation3D(negBigWheelPosition));
   Trk::Volume negBWVol(negBigWheelTransf,negativeBigWheelBounds);
   if (m_adjustStatic && m_static3d) negativeMuonBigWheel = processVolume( &negBWVol,3,"Muon::Detectors::NegativeBigWheel" ); 
   else if (m_adjustStatic) negativeMuonBigWheel = processVolume( &negBWVol,-1,"Muon::Detectors::NegativeBigWheel" ); 
   else negativeMuonBigWheel = processVolume( &negBWVol,m_outerEndcapEtaPartition,m_phiPartition,
						"Muon::Detectors::NegativeBigWheel" ); 
   //
   Amg::Vector3D posBigWheelShift(0.,0.,2*(m_innerEndcapZ+bigWheelZHalfSize));
   Trk::Volume posBWVol(negBWVol,Amg::Transform3D(Amg::Translation3D(posBigWheelShift)));
   if (m_adjustStatic && m_static3d) positiveMuonBigWheel = processVolume( &posBWVol,3,"Muon::Detectors::PositiveBigWheel" ); 
   else if (m_adjustStatic) positiveMuonBigWheel = processVolume( &posBWVol,-1,"Muon::Detectors::PositiveBigWheel" ); 
   else positiveMuonBigWheel = processVolume( &posBWVol,m_outerEndcapEtaPartition,m_phiPartition,
						 "Muon::Detectors::PositiveBigWheel" ); 
// glue
   const Trk::TrackingVolume* negNavOEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*negativeMuonOuterWheel, Trk::positiveFaceXY,
											       *negativeMuonOuterBuffer, Trk::negativeFaceXY, 
											       "Muon::Container::NegOEndcap");  
   const Trk::TrackingVolume* posNavOEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*positiveMuonOuterBuffer, Trk::positiveFaceXY,
											       *positiveMuonOuterWheel, Trk::negativeFaceXY,
											       "Muon::Container::PosOEndcap");  
   const Trk::TrackingVolume* negativeMuonOuterEndcap =
     m_trackingVolumeHelper->glueTrackingVolumeArrays(*negNavOEndcap, Trk::positiveFaceXY,
						      *negativeMuonBigWheel, Trk::negativeFaceXY, 
						      "Muon::Container::NegOuterEndcap");  
   const Trk::TrackingVolume* positiveMuonOuterEndcap =
     m_trackingVolumeHelper->glueTrackingVolumeArrays(*positiveMuonBigWheel, Trk::positiveFaceXY, 
						      *posNavOEndcap, Trk::negativeFaceXY,
						      "Muon::Container::PosOuterEndcap");  

// outer shields   
   double outerEndcapZHalfSize = 0.5*(m_outerEndcapZ-m_innerEndcapZ);
   double outerEndcapPosition  = 0.5*(m_outerEndcapZ+m_innerEndcapZ);
   Amg::Vector3D negOuterShieldPosition(0.,0.,-outerEndcapPosition);
   negOuterShieldBounds = new Trk::CylinderVolumeBounds(m_beamPipeRadius,
							m_outerShieldRadius,
							outerEndcapZHalfSize);
   Trk::Volume negosVol(new Amg::Transform3D(Amg::Translation3D(negOuterShieldPosition)),negOuterShieldBounds);
   negOuterShield = processShield(&negosVol,0,"Muons::Detectors::NegativeOuterShield");

   posOuterShieldBounds = new Trk::CylinderVolumeBounds(m_beamPipeRadius,
							m_outerShieldRadius,
							outerEndcapZHalfSize);
   Amg::Vector3D posOuterShieldPosition(0.,0.,outerEndcapPosition);
   Trk::Volume pososVol(new Amg::Transform3D(Amg::Translation3D(posOuterShieldPosition)),posOuterShieldBounds);
   posOuterShield = processShield(&pososVol,0,"Muons::Detectors::PositiveOuterShield");

// beamPipe
   negBeamPipeBounds = new Trk::CylinderVolumeBounds(m_beamPipeRadius,
                                                  outerEndcapZHalfSize+innerEndcapZHalfSize);
   posBeamPipeBounds = new Trk::CylinderVolumeBounds(m_beamPipeRadius,
                                                  outerEndcapZHalfSize+innerEndcapZHalfSize);
   Amg::Vector3D posBeamPipePosition(0.,0., m_outerEndcapZ-innerEndcapZHalfSize-outerEndcapZHalfSize);
   Amg::Vector3D negBeamPipePosition(0.,0.,-m_outerEndcapZ+innerEndcapZHalfSize+outerEndcapZHalfSize);
   Trk::Volume negbpVol(new Amg::Transform3D(Amg::Translation3D(negBeamPipePosition)),negBeamPipeBounds);
   negBeamPipe = processVolume(&negbpVol,1,1,"Muons::Gaps::NegativeBeamPipe");
   Trk::Volume posbpVol(new Amg::Transform3D(Amg::Translation3D(posBeamPipePosition)),posBeamPipeBounds);
   posBeamPipe = processVolume(&posbpVol,1,1,"Muons::Gaps::PositiveBeamPipe");

   negBeamPipe->registerColorCode(0); 
   posBeamPipe->registerColorCode(0); 

   ATH_MSG_INFO( name() <<" volumes defined " );    
//
// glue volumes at navigation level, create enveloping volume 
// radially
// central + barrel
   ATH_MSG_INFO( name() << "glue barrel+enclosed volumes" );
   const Trk::TrackingVolume* barrel = m_trackingVolumeHelper->glueTrackingVolumeArrays(*muonBarrel,Trk::tubeInnerCover,
											*central, Trk::cylinderCover, 
                                                                                        "All::Container::Barrel");
   //checkVolume(barrel);
// shield+outerEndcap
   ATH_MSG_INFO( name() << "glue shield+outerEndcap" );
   const Trk::TrackingVolume* negOuterEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*negativeMuonOuterEndcap, Trk::tubeInnerCover,
												*negOuterShield, Trk::tubeOuterCover,
												"Muon::Container::NegativeOuterEndcap");
   //checkVolume(negOuterEndcap);
   const Trk::TrackingVolume* posOuterEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*positiveMuonOuterEndcap, Trk::tubeInnerCover,
												*posOuterShield,Trk::tubeOuterCover,
												"Muon::Container::PositiveOuterEndcap");
   //checkVolume(posOuterEndcap);
// shield+innerEndcap
   ATH_MSG_INFO( name() << "glue shield+innerEndcap" );
   const Trk::TrackingVolume* negInnerEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*negativeMuonInnerEndcap, Trk::tubeInnerCover,
												*negInnerShield, Trk::tubeOuterCover,
												"Muon::Container::NegativeInnerEndcap");
   //checkVolume(negInnerEndcap);
   const Trk::TrackingVolume* posInnerEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*positiveMuonInnerEndcap, Trk::tubeInnerCover,
												*posInnerShield,Trk::tubeOuterCover,
												"Muon::Container::PositiveInnerEndcap");
   //checkVolume(posInnerEndcap);
// inner+outerEndcap
   ATH_MSG_INFO( name() << "glue inner+outerEndcap" );
   const Trk::TrackingVolume* negNavEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*negOuterEndcap, Trk::positiveFaceXY,
											      *negInnerEndcap, Trk::negativeFaceXY, 
											      "Muon::Container::NegativeEndcap");  
   const Trk::TrackingVolume* posNavEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*posInnerEndcap, Trk::positiveFaceXY,
											      *posOuterEndcap, Trk::negativeFaceXY,
											      "Muon::Container::PositiveEndcap");  
   //checkVolume(negNavEndcap);
   //checkVolume(posNavEndcap);
// beam pipe + endcaps
   ATH_MSG_INFO( name() << "glue beamPipe+endcaps" );
   const Trk::TrackingVolume* negEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*negNavEndcap, Trk::tubeInnerCover,
											   *negBeamPipe, Trk::cylinderCover,
										           "All::Container::NegativeEndcap");  
   const Trk::TrackingVolume* posEndcap = m_trackingVolumeHelper->glueTrackingVolumeArrays(*posNavEndcap, Trk::tubeInnerCover,
											   *posBeamPipe, Trk::cylinderCover,
										           "All::Container::PositiveEndcap");  
   //checkVolume(negEndcap);
   //checkVolume(posEndcap);
// barrel + endcaps
   ATH_MSG_INFO( name() << "glue barrel+endcaps" );

   const Trk::TrackingVolume* negDet = m_trackingVolumeHelper->glueTrackingVolumeArrays(*negEndcap, Trk::positiveFaceXY,
                                                                                        *barrel, Trk::negativeFaceXY, 
									                "All::Container::NegDet");  
   const Trk::TrackingVolume* detector = m_trackingVolumeHelper->glueTrackingVolumeArrays(*posEndcap, Trk::negativeFaceXY, 
											  *negDet, Trk::positiveFaceXY,
											  m_exitVolume);
// blend material
   if (m_blendInertMaterial) blendMaterial();

// tracking geometry 
   Trk::TrackingGeometry* trackingGeometry = new Trk::TrackingGeometry(detector,Trk::globalSearch);

// clean-up
   if (m_stationSpan) {
     for (size_t i = 0; i < m_stationSpan->size(); i++)
       delete (*m_stationSpan)[i];
     delete m_stationSpan; m_stationSpan = 0;
   }
   if (m_inertSpan) {
     for (size_t i = 0; i < m_inertSpan->size(); i++)
       delete (*m_inertSpan)[i];
     delete m_inertSpan; m_inertSpan = 0;
   }
   
   for (size_t i = 0; i < m_spans.size(); i++) delete m_spans[i];
   
   for (std::map<const Trk::DetachedTrackingVolume*,std::vector<const Trk::TrackingVolume*>* >::iterator it = m_blendMap.begin();
	it != m_blendMap.end();
	++it)
     {
       delete it->second;
     }   

   m_chronoStatSvc->chronoStop("MS::build-up");

   ATH_MSG_INFO( name() <<" returning tracking geometry " );    
   ATH_MSG_INFO( name() <<" with "<< m_frameNum<<" subvolumes at navigation level" );    
   ATH_MSG_INFO( name() <<"( mean number of enclosed detached volumes:"<< float(m_frameStat)/m_frameNum<<")" );    
   return trackingGeometry;  
}

// finalize
StatusCode Muon::MuonTrackingGeometryBuilder::finalize()
{
    if (m_stations) {
      for (size_t i = 0; i < m_stations->size(); i++) {
	if ((*m_stations)[i]) delete (*m_stations)[i];
        else ATH_MSG_DEBUG( name() << " station pointer corrupted ! " ); 
      }
      delete m_stations; m_stations = 0;
    } 
    if (m_inertObjs) {
      unsigned int inLim = (m_blendInertMaterial && m_removeBlended) ? m_inertPerm : m_inertObjs->size();
      for (size_t i = 0; i < inLim; i++) {
	if ((*m_inertObjs)[i])	delete (*m_inertObjs)[i];
        else ATH_MSG_DEBUG( name() << " inert object pointer corrupted ! " ); 
      }
      delete m_inertObjs; m_inertObjs = 0;
    } 

    m_chronoStatSvc->chronoPrint("MS::build-up");

    ATH_MSG_INFO( name() <<" finalize() successful" );
    return StatusCode::SUCCESS;
}
const Muon::Span* Muon::MuonTrackingGeometryBuilder::findVolumeSpan(const Trk::VolumeBounds* volBounds, Amg::Transform3D transform, double zTol, double phiTol) const
{
  if (!volBounds) return 0;
  // volume shape
  const Trk::CuboidVolumeBounds* box = dynamic_cast<const Trk::CuboidVolumeBounds*> (volBounds);
  const Trk::TrapezoidVolumeBounds* trd = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (volBounds);
  const Trk::DoubleTrapezoidVolumeBounds* dtrd = dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*> (volBounds);
  const Trk::BevelledCylinderVolumeBounds* bcyl = dynamic_cast<const Trk::BevelledCylinderVolumeBounds*> (volBounds);
  const Trk::CylinderVolumeBounds* cyl = dynamic_cast<const Trk::CylinderVolumeBounds*> (volBounds);
  const Trk::SubtractedVolumeBounds* sub = dynamic_cast<const Trk::SubtractedVolumeBounds*> (volBounds);
  const Trk::CombinedVolumeBounds* comb = dynamic_cast<const Trk::CombinedVolumeBounds*> (volBounds);
  const Trk::SimplePolygonBrepVolumeBounds* spb = dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*> (volBounds);
  const Trk::PrismVolumeBounds* prism = dynamic_cast<const Trk::PrismVolumeBounds*> (volBounds);

  if(box)  ATH_MSG_VERBOSE(" findVolumeSpan box " );
  if(trd)  ATH_MSG_VERBOSE(" findVolumeSpan trd " );
  if(dtrd) ATH_MSG_VERBOSE( " findVolumeSpan dtrd " );
  if(bcyl) ATH_MSG_VERBOSE(" findVolumeSpan bcyl " );
  if(cyl)  ATH_MSG_VERBOSE(" findVolumeSpan cyl " );
  if(sub)  ATH_MSG_VERBOSE(" findVolumeSpan sub " );
  if(comb) ATH_MSG_VERBOSE(" findVolumeSpan comb " );
  if(spb)  ATH_MSG_VERBOSE(" findVolumeSpan spb " );
  if(prism) ATH_MSG_VERBOSE(" findVolumeSpan prism " );

  if (sub) return findVolumeSpan(&(sub->outer()->volumeBounds()),transform*sub->outer()->transform(),zTol,phiTol);

  if (comb) {
    const Muon::Span* s1 = findVolumeSpan(&(comb->first()->volumeBounds()),transform*comb->first()->transform(),zTol,phiTol);     
    const Muon::Span* s2 = findVolumeSpan(&(comb->second()->volumeBounds()),transform*comb->second()->transform(),zTol,phiTol);     

    ATH_MSG_VERBOSE( "Combined span1:"<<name()<< ","<<(*s1)[0]<<","<< (*s1)[1]<<","<<(*s1)[2]<<"," << (*s1)[3]<<","<< (*s1)[4]<<","<< (*s1)[5]);
    ATH_MSG_VERBOSE( "Combined span2:"<<name()<< ","<<(*s2)[0]<<","<< (*s2)[1]<<","<<(*s2)[2]<<"," << (*s2)[3]<<","<< (*s2)[4]<<","<< (*s2)[5]);

    Muon::Span scomb;
    scomb.reserve(6); 
    scomb.push_back(fmin((*s1)[0],(*s2)[0]));
    scomb.push_back(fmax((*s1)[1],(*s2)[1]));
    scomb.push_back(fmin((*s1)[2],(*s2)[2]));
    scomb.push_back(fmax((*s1)[3],(*s2)[3]));
    scomb.push_back(fmin((*s1)[4],(*s2)[4]));
    scomb.push_back(fmax((*s1)[5],(*s2)[5]));
    return new Muon::Span(scomb);
  }

  // loop over edges ...
  double minZ = m_outerEndcapZ ; double maxZ = - m_outerEndcapZ;
  double minPhi = 2*M_PI; double maxPhi = 0.;
  double minR = m_outerBarrelRadius; double maxR = 0.;
  std::vector<Amg::Vector3D> edges;
  edges.reserve(16);
  Muon::Span span;  
  span.reserve(6); 
  
  double cylZcorr = 0.; 
  if (box) {
    edges.push_back( Amg::Vector3D(box->halflengthX(),box->halflengthY(),box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-box->halflengthX(),box->halflengthY(),box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(box->halflengthX(),-box->halflengthY(),box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-box->halflengthX(),-box->halflengthY(),box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(box->halflengthX(),box->halflengthY(),-box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-box->halflengthX(),box->halflengthY(),-box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(box->halflengthX(),-box->halflengthY(),-box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-box->halflengthX(),-box->halflengthY(),-box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(0.,0.,-box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(0.,0., box->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-box->halflengthX(),0.,0.) );
    edges.push_back( Amg::Vector3D( box->halflengthX(),0.,0.) );
    edges.push_back( Amg::Vector3D(0.,-box->halflengthY(),0.) );
    edges.push_back( Amg::Vector3D(0., box->halflengthY(),0.) );
  }
  if (trd) {
//     std::cout <<  " Trapezoid minHalflengthX " << trd->minHalflengthX() << " maxHalflengthX() " << trd->maxHalflengthX() << " halflengthY() " << trd->halflengthY() << " halflengthZ " << trd->halflengthZ() << std::endl;
  
    edges.push_back( Amg::Vector3D(trd->maxHalflengthX(),trd->halflengthY(),trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-trd->maxHalflengthX(),trd->halflengthY(),trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(trd->minHalflengthX(),-trd->halflengthY(),trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-trd->minHalflengthX(),-trd->halflengthY(),trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(trd->maxHalflengthX(),trd->halflengthY(),-trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-trd->maxHalflengthX(),trd->halflengthY(),-trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(trd->minHalflengthX(),-trd->halflengthY(),-trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-trd->minHalflengthX(),-trd->halflengthY(),-trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(0.,0.,-trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(0.,0., trd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-0.5*(trd->minHalflengthX()-trd->maxHalflengthX()),0.,0.) );
    edges.push_back( Amg::Vector3D( 0.5*(trd->minHalflengthX()-trd->maxHalflengthX()),0.,0.) );
    edges.push_back( Amg::Vector3D(0.,-trd->halflengthY(),0.) );
    edges.push_back( Amg::Vector3D(0., trd->halflengthY(),0.) );
  }
  if (dtrd) {
    edges.push_back( Amg::Vector3D( dtrd->maxHalflengthX(),2*dtrd->halflengthY2(),dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-dtrd->maxHalflengthX(),2*dtrd->halflengthY2(),dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D( dtrd->medHalflengthX(),0.,dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-dtrd->medHalflengthX(),0.,dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D( dtrd->minHalflengthX(),-2*dtrd->halflengthY1(),dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-dtrd->minHalflengthX(),-2*dtrd->halflengthY1(),dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D( dtrd->maxHalflengthX(),2*dtrd->halflengthY2(),-dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-dtrd->maxHalflengthX(),2*dtrd->halflengthY2(),-dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D( dtrd->medHalflengthX(),0.,-dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-dtrd->medHalflengthX(),0.,-dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D( dtrd->minHalflengthX(),-2*dtrd->halflengthY1(),-dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(-dtrd->minHalflengthX(),-2*dtrd->halflengthY1(),-dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(0.,0.,-dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(0.,0., dtrd->halflengthZ()) );
    edges.push_back( Amg::Vector3D(0.,-2*dtrd->halflengthY1(),0.) );
    edges.push_back( Amg::Vector3D(0., 2*dtrd->halflengthY2(),0.) );
  }
  if (bcyl) {
    edges.push_back( Amg::Vector3D(0.,0.,bcyl->halflengthZ()));
    edges.push_back( Amg::Vector3D(0.,0.,-bcyl->halflengthZ()));
  }
  if (cyl) {
    edges.push_back( Amg::Vector3D(0.,0.,cyl->halflengthZ()));
    edges.push_back( Amg::Vector3D(0.,0.,-cyl->halflengthZ()));
  }
  if (spb) {
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif      
    const std::vector<std::pair<double, double> > vtcs = spb->xyVertices();
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif      
    for (unsigned int i=0;i<vtcs.size();i++) {
      edges.push_back( Amg::Vector3D(vtcs[i].first,vtcs[i].second, spb->halflengthZ()) );
      edges.push_back( Amg::Vector3D(vtcs[i].first,vtcs[i].second, -spb->halflengthZ()) );
    }
// center
    edges.push_back( Amg::Vector3D(0.,0.,spb->halflengthZ()));
    edges.push_back( Amg::Vector3D(0.,0.,-spb->halflengthZ()));

  }
  if (prism) {
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif  
    const std::vector<std::pair<double, double> > vtcs = prism->xyVertices();
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif      
    for (unsigned int i=0;i<vtcs.size();i++) {
      edges.push_back( Amg::Vector3D(vtcs[i].first,vtcs[i].second, prism->halflengthZ()) );
      edges.push_back( Amg::Vector3D(vtcs[i].first,vtcs[i].second, -prism->halflengthZ()) );
    }
    edges.push_back( Amg::Vector3D(0.,0.,prism->halflengthZ()));
    edges.push_back( Amg::Vector3D(0.,0.,-prism->halflengthZ()));

  }
  // apply transform and get span
  double minP0=M_PI; double maxP0 = 0.; double minP1=2*M_PI; double maxP1=M_PI;
  // determine phiStep for prism and spb
  double phiStep = 0.;
  for (unsigned int ie=0; ie < edges.size() ; ie++) {
    Amg::Vector3D gp = transform*edges[ie];
    double phi = gp.phi()+M_PI; 

    ATH_MSG_VERBOSE( " local edges:"<< ie<<" x "<< edges[ie].x() << " y " << edges[ie].y() << " z " << edges[ie].z() << " phi "<< edges[ie].phi());
    ATH_MSG_VERBOSE( " Global edges:"<< ie<<" x "<< gp.x() << " y " << gp.y() << " z " << gp.z() << " phi position + pi "<< phi);

    if(ie>0&&phiStep<0.001) {
       double phin = (transform*edges[ie-1]).phi() - M_PI;
       double cph = cos(phi)*cos(phin) + sin(phi)*sin(phin);
       phiStep = fabs(cph)<=1 ? acos(cph) : M_PI;     // TODO check this logic  
       ATH_MSG_VERBOSE( " "<< ie<<" phiStep  "<< phiStep );
    }
    double rad = gp.perp();
    if (cyl || bcyl) {
      double radius = 0.; double hz = 0.;
      Amg::Vector3D dir = (transform*Amg::Vector3D(0.,0.,1.));
      double thAx = dir.theta();
      if (cyl) { radius = cyl->outerRadius(); hz = cyl->halflengthZ();}
      if (bcyl) { radius = bcyl->outerRadius(); hz = bcyl->halflengthZ();}
      if ( gp[2]-radius*sin(thAx) <minZ ) minZ = gp[2]-radius*sin(thAx);
      if ( gp[2]+radius*sin(thAx) >maxZ ) maxZ = gp[2]+radius*sin(thAx);
      if ( rad-radius*fabs(cos(thAx)) < minR )  minR = rad>radius ? rad-radius*fabs(cos(thAx)): 0;
      if ( rad+radius*fabs(cos(thAx)) > maxR )  maxR = rad+radius*fabs(cos(thAx));
      // distance of cylinder axis and global axis
      if (dir.perp()>0.001) {
        // distance to minimal approach
        double dMA = fabs(dir[0]*gp[0]+dir[1]*gp[1])/dir.perp()/dir.perp();
        double dMD = sqrt (fmax(0.,gp.perp()*gp.perp()-dMA*dMA));
        if (dMA<2*hz && dMD-radius < minR ) minR = fmax(0.,dMD-radius); 
      } 
      double dph = rad>0.001 ? atan(radius/rad) : M_PI;
      if ( phi-dph <M_PI && phi-dph < minP0 ) minP0 = phi-dph;
      if ( phi+dph <M_PI && phi+dph > maxP0 ) maxP0 = phi+dph;
      if ( phi-dph >M_PI && phi-dph < minP1 ) minP1 = phi-dph;
      if ( phi+dph >M_PI && phi+dph > maxP1 ) maxP1 = phi+dph;      
    } else {
      if ( gp[2]<minZ ) minZ = gp[2];
      if ( gp[2]>maxZ ) maxZ = gp[2];
      if ( phi<M_PI && phi < minP0 ) minP0 = phi; 
      if ( phi<M_PI && phi > maxP0 ) maxP0 = phi; 
      if ( phi>M_PI && phi < minP1 ) minP1 = phi; 
      if ( phi>M_PI && phi > maxP1 ) maxP1 = phi; 
      //if ( phi < minPhi ) minPhi = phi; 
      //if ( phi > maxPhi ) maxPhi = phi; 
      if ( rad < minR ) minR = rad; 
      if ( rad > maxR ) maxR = rad;
    } 
  }
  if (maxPhi<minPhi) {
    if (maxP0>=minP0 && maxP1<minP1) { minPhi = minP0; maxPhi = maxP0; }
    else if ( maxP1>=minP1 && maxP0<minP0) { minPhi = minP1; maxPhi = maxP1; }
    else if ( maxP1 - minP0 < (maxP0 - minP1+2*M_PI) ) { minPhi = minP0; maxPhi = maxP1; }
    else { minPhi = minP1 ; maxPhi = maxP0; }  
    if (maxPhi<0.001 && minPhi>2*M_PI-0.001) {minPhi=0; maxPhi=2*M_PI;}
  }
//
// correct edges for spb or prism
//
  if(spb||prism) {
    if(minP0<phiStep&&2*M_PI-maxP1<phiStep) {
      minPhi = 0.;
      maxPhi = 2*M_PI;
    }
  }

  if ( box || trd || dtrd || spb ) {
    span.push_back( minZ - zTol );  
    span.push_back( maxZ + zTol );  
    span.push_back( minPhi - phiTol );  
    span.push_back( maxPhi + phiTol );  
    span.push_back( fmax(m_beamPipeRadius+0.001, minR - zTol) );  
    span.push_back( maxR + zTol );  
  } else if (bcyl || cyl ) {
    span.push_back( minZ - cylZcorr -zTol );
    span.push_back( maxZ + cylZcorr +zTol );
    span.push_back( minPhi - phiTol );
    span.push_back( maxPhi + phiTol );
    span.push_back( fmax(m_beamPipeRadius+0.001, minR - zTol) );  
    span.push_back( maxR + zTol );  
  } else {
    ATH_MSG_ERROR( name() <<" volume shape not recognized: ");
    for (int i=0; i<6; i++) span.push_back(0.);
  }
  const Muon::Span* newSpan=new Muon::Span(span);
  return newSpan;
}

const std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Muon::Span*> >* >* Muon::MuonTrackingGeometryBuilder::findVolumesSpan(const std::vector<const Trk::DetachedTrackingVolume*>*& objs, double zTol, double phiTol) const
{

  if (!objs || !objs->size()) return 0;
  std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >* spans =
   new std::vector<std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >* >(9);
  // split MS into 9 blocks to speed up the build-up of geometry
  for (unsigned int i=0;i<9;i++) (*spans)[i] = new std::vector<std::pair<const Trk::DetachedTrackingVolume*,const Span*> >; 
  for (unsigned int iobj=0; iobj<objs->size(); iobj++) {
    Amg::Transform3D  transform = (*objs)[iobj]->trackingVolume()->transform();
    const Muon::Span* span = findVolumeSpan(&((*objs)[iobj]->trackingVolume()->volumeBounds()), transform, zTol, phiTol);
    double x0 = (*objs)[iobj]->trackingVolume()->X0;
    double intX0 = fabs((*span)[0]-(*span)[1])/(x0+0.000000001);
    double l0 = (*objs)[iobj]->trackingVolume()->L0;
    ATH_MSG_DEBUG( "span:"<<(*objs)[iobj]->name()<< ","<<(*span)[0]<<","<< (*span)[1]<<","<<(*span)[2]<<","
     << (*span)[3]<<","<< (*span)[4]<<","<< (*span)[5] << " X0 " << x0 << " L0 "<< l0 << " intX0 for span0 span1 " << intX0 );

    int nspans = 0;
    // negative outer wheel
    if ( (*span)[0] < -m_bigWheel ) {
        (*spans)[0]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // negative big wheel
    if ( (*span)[0] < -m_innerEndcapZ && (*span)[1]>-m_bigWheel ) {
        (*spans)[1]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // neg.ect
    if ( (*span)[0] < -m_ectZ && (*span)[1]>-m_innerEndcapZ ) {
        (*spans)[2]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // neg.small wheel
    if ( (*span)[0] < -m_diskShieldZ && (*span)[1]>-m_ectZ ) {
        (*spans)[3]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // barrel  
    if ( (*span)[0] <  m_diskShieldZ && (*span)[1]> -m_diskShieldZ ) {
//	 && ((*span)[5]> m_innerBarrelRadius || (*span)[0]<-m_barrelZ || (*span)[1]>m_barrelZ)  ) {
        (*spans)[4]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // pos.small wheel
    if ( (*span)[0] < m_ectZ && (*span)[1]> m_diskShieldZ ) {
        (*spans)[5]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // pos.ect
    if ( (*span)[0] < m_innerEndcapZ && (*span)[1]> m_ectZ ) {
        (*spans)[6]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // positive big wheel
    if ( (*span)[0] < m_bigWheel && (*span)[1]> m_innerEndcapZ ) {
        (*spans)[7]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 
    // positive outer wheel
    if ( (*span)[1] >  m_bigWheel ) {
        (*spans)[8]->push_back(std::pair<const Trk::DetachedTrackingVolume*,const Span*>((*objs)[iobj],span));
        nspans++;
    } 

    if(nspans == 0) ATH_MSG_WARNING( " object not selected in span regions " << (*objs)[iobj]->name() );
    if(nspans > 1) ATH_MSG_VERBOSE( " object selected in " << nspans << " span regions " << (*objs)[iobj]->name() );

    m_spans.push_back(span);                 // keep track of things to delete
  }

  return spans;
}


const Trk::TrackingVolume* Muon::MuonTrackingGeometryBuilder::processVolume(const Trk::Volume* vol, int etaN , int phiN, std::string volumeName) const
{

  const Trk::TrackingVolume* tVol = 0;

  unsigned int colorCode = m_colorCode;

  std::vector<const Trk::DetachedTrackingVolume*> blendVols; 

  // partitions ? include protection against wrong setup
  if (etaN < 1 || phiN < 1) {
    ATH_MSG_ERROR( name() << "wrong partition setup" );
    etaN = 1;
    phiN = 1;
  }
  if ( etaN * phiN > 1 ) {  // partition
    const Trk::CylinderVolumeBounds* cyl=dynamic_cast<const Trk::CylinderVolumeBounds*> (&(vol->volumeBounds()));
    if (!cyl) {
      ATH_MSG_ERROR( " process volume: volume cylinder boundaries not retrieved, return 0 " );
      return 0; 
    }
    // subvolume boundaries
    Trk::CylinderVolumeBounds* subBds = 0;

    double phiSect = M_PI/phiN;
    double etaSect = (cyl->halflengthZ())/etaN;

    subBds = new Trk::CylinderVolumeBounds(cyl->innerRadius(), cyl->outerRadius(), phiSect,etaSect);
    const Trk::Volume* protVol= new Trk::Volume(0, subBds);     

    // create subvolumes & BinnedArray
    std::vector<Trk::TrackingVolumeOrderPosition> subVolumes;
    std::vector<const Trk::TrackingVolume*> sVols;                // for gluing
    std::vector<const Trk::TrackingVolume*> sVolsNeg;             // for gluing
    std::vector<const Trk::TrackingVolume*> sVolsPos;             // for gluing
    for (int eta = 0; eta < etaN; eta++) {
      if (colorCode>0) colorCode = 26 - colorCode;
      // reference point for the check of envelope
      double posZ = (vol->center())[2]+ etaSect * (2.*eta+1.-etaN) ;
      double posR = 0.5*(cyl->innerRadius()+cyl->outerRadius());
      int geoSignature = 4;
      // loop over inner cutouts
      for (unsigned int in=1; in<m_msCutoutsIn.size(); in++) {
        if (posZ>=m_msCutoutsIn[in].second && posZ<=m_msCutoutsIn[in-1].second) {
          if (posR < m_msCutoutsIn[in].first) geoSignature=2;
          break;
	}
      }
      if (geoSignature == 4) {
	// loop over outer cutouts
	for (unsigned int io=1; io<m_msCutoutsOut.size(); io++) {
	  if (posZ>=m_msCutoutsOut[io-1].second && posZ<=m_msCutoutsOut[io].second) {
	    if (posR > m_msCutoutsOut[io].first) geoSignature=5;
	    break;
	  }
	}
      }
      for (int phi = 0; phi < phiN; phi++) {
	if (colorCode>0) colorCode = 26 - colorCode;
        // define subvolume
        Amg::Transform3D  transf(Amg::AngleAxis3D(phiSect*(2*phi+1), Amg::Vector3D(0.,0.,1.))*Amg::Translation3D(Amg::Vector3D(0.,0.,posZ)));
        const Trk::Volume* subVol= new Trk::Volume(*protVol, transf);     
        // enclosed muon objects ?   
	std::string volName = volumeName +MuonGM::buildString(eta,2) +MuonGM::buildString(phi,2) ; 
        blendVols.clear();
        std::vector<const Trk::DetachedTrackingVolume*>* detVols= getDetachedObjects( subVol, blendVols);
        const Trk::TrackingVolume* sVol = new Trk::TrackingVolume( *subVol,
								   m_muonMaterial,
								   detVols,
								   volName );
        // statistics
        m_frameNum++ ; if (detVols) m_frameStat += detVols->size();  
        // prepare blending
        if (m_blendInertMaterial && blendVols.size()) {
          for (unsigned int id=0;id<blendVols.size();id++) {
	    if (!m_blendMap[blendVols[id]]) {
	      m_blendMap[blendVols[id]] = new std::vector<const Trk::TrackingVolume*>;
              m_blendVols.push_back(blendVols[id]);
	    }
	    m_blendMap[blendVols[id]]->push_back(sVol);
	  }
	}  
        //
        if (geoSignature == 2) sVol->sign(Trk::BeamPipe);
        if (geoSignature == 5) sVol->sign(Trk::Cavern);
        sVol->registerColorCode(colorCode); 
	// reference position 
	Amg::Vector3D gp(subBds->outerRadius(),0.,0.);
	subVolumes.push_back(Trk::TrackingVolumeOrderPosition(Trk::SharedObject<const Trk::TrackingVolume>(sVol, true),
                                                             Amg::Vector3D(transf*gp)));
        //glue subVolumes
        sVols.push_back(sVol); 
        if (eta==0)      sVolsNeg.push_back(sVol); 
        if (eta==etaN-1) sVolsPos.push_back(sVol); 
        // in phi        
        if ( phiN>1 && phi>0) {
          m_trackingVolumeHelper->glueTrackingVolumes(*sVol, Trk::tubeSectorNegativePhi,
						      *(sVols[eta*phiN+phi-1]), Trk::tubeSectorPositivePhi);
          if ( phi==phiN-1 )  m_trackingVolumeHelper->glueTrackingVolumes(*(sVols[eta*phiN]), Trk::tubeSectorNegativePhi,
                                                                          *sVol, Trk::tubeSectorPositivePhi);
	}
        // in eta
        if ( etaN>1 && eta>0) m_trackingVolumeHelper->glueTrackingVolumes(*sVol, Trk::negativeFaceXY,
						      *(sVols[(eta-1)*phiN+phi]), Trk::positiveFaceXY);        
        //
	delete subVol; //shouldn't need this any more
      }
    }

    //Trk::BinUtility2DPhiZ* volBinUtil=new Trk::BinUtility2DPhiZ(phiN,etaN,subBds->outerRadius(),cyl->halflengthZ(),M_PI, new Amg::Transform3D(vol->transform()));
    Trk::BinUtility buPhi( phiN, -M_PI, M_PI, Trk::closed, Trk::binPhi ); 
    const Trk::BinUtility   buZ( etaN, vol->transform().translation()[2]-cyl->halflengthZ(), 
                                        vol->transform().translation()[2]+cyl->halflengthZ(), Trk::open, Trk::binZ ); 
    buPhi += buZ; 

    Trk::BinUtility* volBinUtil=new Trk::BinUtility(buPhi);        // TODO verify ordering PhiZ vs. ZPhi 

    delete protVol;
    Trk::BinnedArray2D<Trk::TrackingVolume>* subVols=new Trk::BinnedArray2D<Trk::TrackingVolume>(subVolumes,volBinUtil);

    tVol = new Trk::TrackingVolume( *vol,
                                    m_muonMaterial,
				    0,subVols,
				    volumeName);
    // register glue volumes
    const Trk::GlueVolumesDescriptor& volGlueVolumes = tVol->glueVolumesDescriptor();
    volGlueVolumes.registerGlueVolumes(Trk::tubeInnerCover,sVols);
    volGlueVolumes.registerGlueVolumes(Trk::tubeOuterCover,sVols);
    volGlueVolumes.registerGlueVolumes(Trk::negativeFaceXY,sVolsNeg);
    volGlueVolumes.registerGlueVolumes(Trk::positiveFaceXY,sVolsPos);

  } else {
    // enclosed muon objects ? 
    blendVols.clear();
    std::vector<const Trk::DetachedTrackingVolume*>* muonObjs = getDetachedObjects( vol, blendVols);

    tVol = new Trk::TrackingVolume( *vol,
                                    m_muonMaterial,
				    muonObjs,
				    volumeName);
    // statistics
    m_frameNum++ ; if (muonObjs) m_frameStat += muonObjs->size();  
    // prepare blending
    if (m_blendInertMaterial && blendVols.size()) {
      for (unsigned int id=0;id<blendVols.size();id++) {
	if (!m_blendMap[blendVols[id]]) {
	  m_blendMap[blendVols[id]] = new std::vector<const Trk::TrackingVolume*>;
	  m_blendVols.push_back(blendVols[id]);
	}
	m_blendMap[blendVols[id]]->push_back(tVol);
      }
    }  
  }

  return tVol;
} 

const Trk::TrackingVolume* Muon::MuonTrackingGeometryBuilder::processVolume(const Trk::Volume* vol, int mode , std::string volumeName) const
{
  ATH_MSG_VERBOSE( name() << "processing volume in mode:"<< mode );

  // mode : -1 ( adjusted z/phi partition )
  //         0 ( -"- plus barrel H binning )            
  //         0 ( -"- plus inner endcap H binning )            
  //         0 ( -"- plus outer endcap H binning )            

  const Trk::TrackingVolume* tVol = 0;

  unsigned int colorCode = m_colorCode;

  std::vector<const Trk::DetachedTrackingVolume* > blendVols;

  //getPartitionFromMaterial(vol);

  // retrieve cylinder
  const Trk::CylinderVolumeBounds* cyl=dynamic_cast<const Trk::CylinderVolumeBounds*> (&(vol->volumeBounds()));
  if (!cyl) {
    ATH_MSG_ERROR( " process volume: volume cylinder boundaries not retrieved, return 0 " );
    return 0; 
  }
  // create vector of zSteps for this volume
  std::vector<float> zSteps;
  std::vector<int> zTypes;
  zSteps.clear(); zTypes.clear();
  double zPos = vol->center()[2];
  double hz = cyl->halflengthZ();
  double z1 = zPos-hz;  double z2 = zPos+hz ;
  zSteps.push_back(z1);
  for (unsigned int iz=0;iz<m_zPartitions.size();iz++) {
    if ( m_zPartitions[iz]==zSteps.front()) zTypes.push_back(m_zPartitionsType[iz]);
    if ( m_zPartitions[iz]> z1 && m_zPartitions[iz] < z2 ) {
      zSteps.push_back(m_zPartitions[iz]);
      if (!zTypes.size()) { 
	if (iz==0) zTypes.push_back(0);
        else zTypes.push_back(m_zPartitionsType[iz-1]);
      }
      zTypes.push_back(m_zPartitionsType[iz]);
      z1 = m_zPartitions[iz];
    }
  }
  zSteps.push_back(z2);

  for (unsigned int iz=0;iz<zSteps.size(); iz++) ATH_MSG_DEBUG( "z partition in volume:"<<volumeName<<":"<<iz<<":"<<zSteps[iz]); 

  // phi binning
  if (fabs(zPos)> m_barrelZ && cyl->outerRadius()<m_outerBarrelRadius) getPhiParts(0);
  else if (fabs(zPos)<= m_ectZ) getPhiParts(2);
  else if (fabs(zPos)<= m_innerEndcapZ) getPhiParts(3);
  else if (fabs(zPos)> m_outerWheel && cyl->outerRadius()> m_outerShieldRadius ) getPhiParts(1);
  else if (fabs(zPos)> m_innerEndcapZ && fabs(zPos)<m_bigWheel && cyl->outerRadius()> m_outerShieldRadius ) getPhiParts(1);
  else getPhiParts(0);

  // R/H binning ?
  unsigned int etaN = zSteps.size()-1;
  unsigned int phiN = m_adjustedPhi.size();
   
  int phiTypeMax = 0;                // count different partitions

  if ( mode > -1 ) {
    // create z,phi bin utilities
    //Trk::BinUtility1DZZ* zBinUtil = new Trk::BinUtility1DZZ(zSteps);
    //Trk::BinUtility1DF* pBinUtil = new Trk::BinUtility1DF(m_adjustedPhi);
    Trk::BinUtility* zBinUtil = new Trk::BinUtility(zSteps, Trk::open, Trk::binZ );
    Trk::BinUtility* pBinUtil = new Trk::BinUtility(m_adjustedPhi,  Trk::closed, Trk::binPhi ); 
    std::vector<std::vector<Trk::BinUtility*> >*  hBinUtil=new std::vector<std::vector<Trk::BinUtility*> >;
    for (unsigned iz=0;iz < zSteps.size()-1; iz++) {
      std::vector<Trk::BinUtility*> phBinUtil; 
      for (unsigned ip=0;ip < m_adjustedPhi.size(); ip++) {
        // retrieve reference phi
	float phiRef = 0.5*m_adjustedPhi[ip];
        if (ip<m_adjustedPhi.size()-1) phiRef += 0.5*m_adjustedPhi[ip+1] ;
	else phiRef += 0.5*m_adjustedPhi[0]+M_PI ;

        if (m_adjustedPhiType[ip]>phiTypeMax) phiTypeMax = m_adjustedPhiType[ip];
        for (std::pair<int,float> i: m_hPartitions[mode][zTypes[iz]][m_adjustedPhiType[ip]]) {
          ATH_MSG_VERBOSE( " mode " << mode << " phiRef " << phiRef << " zTypes[iz] " << zTypes[iz] << " m_adjustedPhiType[ip] " << m_adjustedPhiType[ip] << " hPartitions " << i.second );
        }
        phBinUtil.push_back(new Trk::BinUtility(phiRef,m_hPartitions[mode][zTypes[iz]][m_adjustedPhiType[ip]]));
      }
      hBinUtil->push_back(phBinUtil);
    }

    // create subvolumes & BinnedArray
    std::vector<Trk::TrackingVolumeOrderPosition>  subVolumesVect;
    std::vector<std::vector<std::vector<const Trk::TrackingVolume*> > > subVolumes;
    std::vector<std::vector<Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > > > hBins;
    std::vector<const Trk::TrackingVolume*> sVolsInn;             // for gluing
    std::vector<const Trk::TrackingVolume*> sVolsOut;             // for gluing
    std::vector<const Trk::TrackingVolume*> sVolsNeg;             // for gluing
    std::vector<const Trk::TrackingVolume*> sVolsPos;             // for gluing
    for (unsigned int eta = 0; eta < zSteps.size()-1; eta++) {
      if (colorCode>0) colorCode = 6 -colorCode;
      double posZ = 0.5*(zSteps[eta] + zSteps[eta+1]) ;
      double   hZ = 0.5*fabs(zSteps[eta+1] - zSteps[eta]) ;
      std::vector<std::vector<const Trk::TrackingVolume*> > phiSubs;
      std::vector<Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > >  phBins;
      std::vector<int> phiType(phiTypeMax+1,-1);        // indication of first phi/R partition built for a given type (for cloning)
      std::vector<std::vector<Trk::Volume*> > garbVol(phiTypeMax+1);
      unsigned int pCode = 1; 
      for (unsigned int phi = 0; phi < phiN; phi++) {
	pCode = (colorCode>0) ? 3-pCode : 0;
	double posPhi = 0.5*m_adjustedPhi[phi];
	double phiSect = 0.;
        if (phi<phiN-1) {
	  posPhi += 0.5*m_adjustedPhi[phi+1] ;
          phiSect = 0.5*fabs(m_adjustedPhi[phi+1]-m_adjustedPhi[phi]);
	} else {
	  posPhi += 0.5*m_adjustedPhi[0]+M_PI ;
          phiSect = 0.5*fabs(m_adjustedPhi[0]+2*M_PI-m_adjustedPhi[phi]);
	}
	std::vector<std::pair<int,float> > hSteps =  m_hPartitions[mode][zTypes[eta]][m_adjustedPhiType[phi]];
	std::vector<const Trk::TrackingVolume*> hSubs;
	std::vector<Trk::TrackingVolumeOrderPosition> hSubsTr;
        int phiP = phiType[m_adjustedPhiType[phi]]; 

        unsigned int hCode = 1; 
        for (unsigned int h = 0; h < hSteps.size()-1; h++) {
	  hCode = colorCode>0 ? 1 - hCode : 0; 
          // similar volume may exist already
	  Trk::Volume* subVol=0;
	  Amg::Transform3D* transf = new Amg::Transform3D(Amg::AngleAxis3D(posPhi,Amg::Vector3D(0.,0.,1.))*Amg::Translation3D(Amg::Vector3D(0.,0.,posZ)));
          //
          int volType = 0;     // cylinder 
          if ( hSteps[h].first == 1 && hSteps[h+1].first == 0 ) volType = 1;  
          if ( hSteps[h].first == 0 && hSteps[h+1].first == 1 ) volType = 2;  
          if ( hSteps[h].first == 1 && hSteps[h+1].first == 1 ) volType = 3;  
	  // define subvolume
          if (phiP>-1 ) {
            subVol = new Trk::Volume(*(phiSubs[phiP][h]),(*transf)*phiSubs[phiP][h]->transform().inverse());
	  } else if ( phiSect<0.5*M_PI) {
	    Trk::BevelledCylinderVolumeBounds* subBds = new Trk::BevelledCylinderVolumeBounds(hSteps[h].second,
							   hSteps[h+1].second,
							   phiSect, 
							   hZ,
							   volType);
	    subVol = new Trk::Volume(transf, subBds);
	  } else {
	    Trk::CylinderVolumeBounds* subBds = new Trk::CylinderVolumeBounds(hSteps[h].second,
						   hSteps[h+1].second,
						   phiSect, 
						   hZ);
	    subVol = new Trk::Volume(transf, subBds);
	  }
       
	  // enclosed muon objects ? also adjusts material properties in case of material blend  
	  std::string volName = volumeName +MuonGM::buildString(eta,2) +MuonGM::buildString(phi,2) +MuonGM::buildString(h,2) ; 
          blendVols.clear(); 
	  std::vector<const Trk::DetachedTrackingVolume*>* detVols= getDetachedObjects( subVol, blendVols);

	  const Trk::TrackingVolume* sVol = new Trk::TrackingVolume( *subVol,
								     m_muonMaterial,
								     detVols,
								     volName );                                                                  
     
	  // statistics
	  m_frameNum++ ; if (detVols) m_frameStat += detVols->size();  
	  // prepare blending
	  if (m_blendInertMaterial && blendVols.size()) {
	    for (unsigned int id=0;id<blendVols.size();id++) {
	      if (!m_blendMap[blendVols[id]]) {
		m_blendMap[blendVols[id]] = new std::vector<const Trk::TrackingVolume*>;
                m_blendVols.push_back(blendVols[id]);
	      }
	      m_blendMap[blendVols[id]]->push_back(sVol);
	    }
	  }  
	  // reference point for the check of envelope
	  double posR = 0.5*(hSteps[h].second+hSteps[h+1].second);
	  // loop over inner cutouts
	  for (unsigned int in=1; in<m_msCutoutsIn.size(); in++) {
	    if (posZ>=m_msCutoutsIn[in].second && posZ<=m_msCutoutsIn[in-1].second) {
	      if (posR < m_msCutoutsIn[in].first) sVol->sign(Trk::BeamPipe);
	      break;
	    }
	  }
	  // loop over outer cutouts
	  for (unsigned int io=1; io<m_msCutoutsOut.size(); io++) {
	    if (posZ>=m_msCutoutsOut[io-1].second && posZ<=m_msCutoutsOut[io].second) {
	      if (posR > m_msCutoutsOut[io].first) sVol->sign(Trk::Cavern);
	      break;
	    }
	  }
	  //
          sVol->registerColorCode(colorCode+pCode+hCode);
	  // reference position 
	  Amg::Vector3D gp(0.5*(hSteps[h].second+hSteps[h+1].second),0.,0.);
	  subVolumesVect.push_back(Trk::TrackingVolumeOrderPosition(Trk::SharedObject<const Trk::TrackingVolume>(sVol, false),
	                                                       Amg::Vector3D((*transf)*gp)));
	  hSubsTr.push_back(Trk::TrackingVolumeOrderPosition(Trk::SharedObject<const Trk::TrackingVolume>(sVol, true),
	                                                       Amg::Vector3D((*transf)*gp)));
	  hSubs.push_back(sVol);

          // cleanup 
          if (phiP>-1) {delete transf; delete subVol;}
          else garbVol[m_adjustedPhiType[phi]].push_back(subVol);       // don't delete before cloned

	  //glue subVolume
	  if (h==0)                sVolsInn.push_back(sVol); 
	  if (h==hSteps.size()-2)  sVolsOut.push_back(sVol); 
	  if (eta==0)      sVolsNeg.push_back(sVol); 
	  if (eta==etaN-1) sVolsPos.push_back(sVol); 
          // in R/H
          if (h>0) { // glue 'manually'
            if (volType == 1 || volType == 3 ) {  // plane surface
	      m_trackingVolumeHelper->setOutsideTrackingVolume(*sVol, Trk::tubeSectorInnerCover,hSubs[h-1]); 
	      m_trackingVolumeHelper->setOutsideTrackingVolume(*(hSubs[h-1]), Trk::tubeSectorOuterCover,sVol); 
            } else {  // cylinder surface
	      m_trackingVolumeHelper->setInsideTrackingVolume(*sVol, Trk::tubeSectorInnerCover,hSubs[h-1]); 
	      m_trackingVolumeHelper->setOutsideTrackingVolume(*(hSubs[h-1]), Trk::tubeSectorOuterCover,sVol);
	    } 
          }
	  // in phi        
	  if ( phiN>1 && phi>0) {
	    m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*sVol, Trk::tubeSectorNegativePhi,phBins[phi-1]);
	    if ( phi==phiN-1 )  m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*sVol, Trk::tubeSectorPositivePhi, phBins[0]);
	  }
	  // in eta
	  if ( etaN>1 && eta>0) m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*sVol, Trk::negativeFaceXY, hBins[eta-1][phi]);     
	  //
	}
        phiSubs.push_back(hSubs); 
	Trk::BinnedArray1D<Trk::TrackingVolume>* volBinArray = new Trk::BinnedArray1D<Trk::TrackingVolume>(hSubsTr,(*hBinUtil)[eta][phi]->clone());
        phBins.push_back(Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >(volBinArray));
        // save link to current partition for cloning 
        if (phiP<0) phiType[m_adjustedPhiType[phi]] = phi;    

        // finish phi gluing
        if (phiN>1 && phi>0) {
	  for (unsigned int j=0; j<phiSubs[phi-1].size(); j++) {
	    m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*(phiSubs[phi-1][j]), Trk::tubeSectorPositivePhi,phBins[phi]);
	  }
	}
        if (phiN>1 && phi==phiN-1) {
	  for (unsigned int j=0; j<phiSubs[0].size(); j++) {
	    m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*(phiSubs[0][j]), Trk::tubeSectorNegativePhi,phBins[phi]);
	  }
	}
        // finish eta gluing
        if ( etaN>1 && eta>0) {
	  for (unsigned int j=0; j<subVolumes[eta-1][phi].size(); j++) {
	    m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*(subVolumes[eta-1][phi][j]), Trk::positiveFaceXY,phBins[phi]);
	  }
	}
      }
      subVolumes.push_back(phiSubs);
      hBins.push_back(phBins);
      // get rid of the garbage
      for (unsigned int j=0;j<garbVol.size();j++)
	for (unsigned int jj=0;jj<garbVol[j].size();jj++) delete garbVol[j][jj];
    }

    //Trk::BinUtility3DZFH* volBinUtil=new Trk::BinUtility3DZFH(zBinUtil,pBinUtil,hBinUtil,new Amg::Transform3D(vol->transform()));
  
    Trk::BinnedArray1D1D1D<Trk::TrackingVolume>* subVols=new Trk::BinnedArray1D1D1D<Trk::TrackingVolume>(subVolumesVect,zBinUtil,pBinUtil,hBinUtil);

    tVol = new Trk::TrackingVolume( *vol,
                                    m_muonMaterial,
				    0,subVols,
				    volumeName);
    // register glue volumes
    const Trk::GlueVolumesDescriptor& volGlueVolumes = tVol->glueVolumesDescriptor();
    volGlueVolumes.registerGlueVolumes(Trk::tubeInnerCover,sVolsInn);
    volGlueVolumes.registerGlueVolumes(Trk::tubeOuterCover,sVolsOut);
    volGlueVolumes.registerGlueVolumes(Trk::negativeFaceXY,sVolsNeg);
    volGlueVolumes.registerGlueVolumes(Trk::positiveFaceXY,sVolsPos);
  
    return tVol;
  } 

  // proceed with 2D z/phi binning
  // partitions ? include protection against wrong setup
  if (phiN < 1) {
    ATH_MSG_ERROR( name() << "wrong partition setup" );
    phiN = 1;
  } else {
    ATH_MSG_VERBOSE( name() << "partition setup:(z,phi):"<<etaN<<","<<phiN );
  }

  if ( etaN * phiN > 1 ) {  // partition
    // subvolume boundaries
    Trk::CylinderVolumeBounds* subBds=0;

    // create subvolumes & BinnedArray
    std::vector<Trk::TrackingVolumeOrderPosition> subVolumes(etaN*phiN);
    std::vector<const Trk::TrackingVolume*> sVols(etaN*phiN);                // for gluing
    std::vector<const Trk::TrackingVolume*> sVolsNeg(phiN);             // for gluing
    std::vector<const Trk::TrackingVolume*> sVolsPos(phiN);             // for gluing
    for (unsigned int eta = 0; eta < zSteps.size()-1; eta++) {
      double posZ = 0.5*(zSteps[eta] + zSteps[eta+1]) ;
      double   hZ = 0.5*fabs(zSteps[eta+1] - zSteps[eta]) ;
      colorCode = 26 -colorCode;
      for (unsigned int phi = 0; phi < phiN; phi++) {
	colorCode = 26 -colorCode;
	double posPhi = 0.5*m_adjustedPhi[phi];
	double phiSect = 0.;
        if (phi<phiN-1) {
	  posPhi += 0.5*m_adjustedPhi[phi+1] ;
          phiSect = 0.5*fabs(m_adjustedPhi[phi+1]-m_adjustedPhi[phi]);
	} else {
	  posPhi += 0.5*m_adjustedPhi[0]+M_PI ;
          phiSect = 0.5*fabs(m_adjustedPhi[0]+2*M_PI-m_adjustedPhi[phi]);
	}
	// define subvolume
	subBds = new Trk::CylinderVolumeBounds(cyl->innerRadius(),
					       cyl->outerRadius(),
					       phiSect, 
					       hZ);
        Amg::Transform3D* transf = new Amg::Transform3D(Amg::AngleAxis3D(posPhi,Amg::Vector3D(0.,0.,1.))*Amg::Translation3D(Amg::Vector3D(0.,0.,posZ)));
        Trk::Volume subVol(transf, subBds);     
        // enclosed muon objects ?   
	std::string volName = volumeName +MuonGM::buildString(eta,2) +MuonGM::buildString(phi,2) ; 

	Trk::Material mat=m_muonMaterial;
        blendVols.clear();
        std::vector<const Trk::DetachedTrackingVolume*>* detVols= getDetachedObjects( &subVol, blendVols);
        const Trk::TrackingVolume* sVol = new Trk::TrackingVolume( subVol,
								   m_muonMaterial,
								   detVols,
								   volName );
        // statistics
        m_frameNum++ ; if (detVols) m_frameStat += detVols->size();  
        // prepare blending
        if (m_blendInertMaterial && blendVols.size()) {
          for (unsigned int id=0;id<blendVols.size();id++) {
	    if (!m_blendMap[blendVols[id]]) {
	      m_blendMap[blendVols[id]] = new std::vector<const Trk::TrackingVolume*>;
              m_blendVols.push_back(blendVols[id]);
	    }
	    m_blendMap[blendVols[id]]->push_back(sVol);
	  }
	}  
	// reference point for the check of envelope
	double posR = 0.5*(cyl->innerRadius()+cyl->outerRadius());
	// loop over inner cutouts
	for (unsigned int in=1; in<m_msCutoutsIn.size(); in++) {
	  if (posZ>=m_msCutoutsIn[in].second && posZ<=m_msCutoutsIn[in-1].second) {
	    if (posR < m_msCutoutsIn[in].first) sVol->sign(Trk::BeamPipe);
	    break;
	  }
	}
	// loop over outer cutouts
	for (unsigned int io=1; io<m_msCutoutsOut.size(); io++) {
	  if (posZ>=m_msCutoutsOut[io-1].second && posZ<=m_msCutoutsOut[io].second) {
	    if (posR > m_msCutoutsOut[io].first) sVol->sign(Trk::Cavern);
	    break;
	  }
	}
        //delete subVol;
        sVol->registerColorCode(colorCode); 
        // reference position 
	Amg::Vector3D gp(subBds->outerRadius(),0.,0.);
	//subVolumes.push_back(Trk::TrackingVolumeOrderPosition(Trk::SharedObject<const Trk::TrackingVolume>(sVol, true),
        //                                                     Amg::Vector3D((*transf)*gp)));
	subVolumes[phi*etaN+eta] = Trk::TrackingVolumeOrderPosition(Trk::SharedObject<const Trk::TrackingVolume>(sVol, false),
                                                             Amg::Vector3D((*transf)*gp));
        //glue subVolumes
        //sVols[phi*etaN+eta] = sVol; 
        sVols[phiN*eta+phi] = sVol; 
        if (eta==0)      sVolsNeg[phi]=sVol; 
        if (eta==etaN-1) sVolsPos[phi]=sVol; 
        // in phi        
        if ( phiN>1 && phi>0) {
          m_trackingVolumeHelper->glueTrackingVolumes(*sVol, Trk::tubeSectorNegativePhi,
						      *(sVols[eta*phiN+phi-1]), Trk::tubeSectorPositivePhi);
          if ( phi==phiN-1 )  m_trackingVolumeHelper->glueTrackingVolumes(*(sVols[eta*phiN]), Trk::tubeSectorNegativePhi,
                                                                          *sVol, Trk::tubeSectorPositivePhi);
	}
        // in eta
        if ( etaN>1 && eta>0) m_trackingVolumeHelper->glueTrackingVolumes(*sVol, Trk::negativeFaceXY,
						      *(sVols[(eta-1)*phiN+phi]), Trk::positiveFaceXY);        
        //
      }
    }

    //Trk::BinUtility2DZF* volBinUtil=new Trk::BinUtility2DZF(zSteps,m_adjustedPhi,new Amg::Transform3D(vol->transform()));
    Trk::BinUtility zBinUtil(zSteps, Trk::BinningOption::open, Trk::BinningValue::binZ );
    const Trk::BinUtility pBinUtil(m_adjustedPhi,  Trk::BinningOption::closed, Trk::BinningValue::binPhi ); 

    zBinUtil += pBinUtil; 

    Trk::BinUtility* volBinUtil=new Trk::BinUtility(zBinUtil);        // TODO verify ordering PhiZ vs. ZPhi 

    Trk::BinnedArray2D<Trk::TrackingVolume>* subVols=new Trk::BinnedArray2D<Trk::TrackingVolume>(subVolumes,volBinUtil);

    tVol = new Trk::TrackingVolume( *vol,
                                    m_muonMaterial,
				    0,subVols,
				    volumeName);
    // register glue volumes
    const Trk::GlueVolumesDescriptor& volGlueVolumes = tVol->glueVolumesDescriptor();
    volGlueVolumes.registerGlueVolumes(Trk::tubeInnerCover,sVols);
    volGlueVolumes.registerGlueVolumes(Trk::tubeOuterCover,sVols);
    volGlueVolumes.registerGlueVolumes(Trk::negativeFaceXY,sVolsNeg);
    volGlueVolumes.registerGlueVolumes(Trk::positiveFaceXY,sVolsPos);

  } else {
    // enclosed muon objects ? 
    blendVols.clear();
    std::vector<const Trk::DetachedTrackingVolume*>* muonObjs = getDetachedObjects( vol, blendVols);

    tVol = new Trk::TrackingVolume( *vol,
                                    m_muonMaterial,
				    muonObjs,
				    volumeName);
    // statistics
    m_frameNum++ ; if (muonObjs) m_frameStat += muonObjs->size();  
    // prepare blending
    if (m_blendInertMaterial && blendVols.size()) {
      for (unsigned int id=0;id<blendVols.size();id++) {
	if (!m_blendMap[blendVols[id]]) {
	  m_blendMap[blendVols[id]] = new std::vector<const Trk::TrackingVolume*>;
	  m_blendVols.push_back(blendVols[id]);
	}
	m_blendMap[blendVols[id]]->push_back(tVol);
      }
    }  
  }

  return tVol;
} 

const Trk::TrackingVolume* Muon::MuonTrackingGeometryBuilder::processShield(const Trk::Volume* vol, int type,std::string volumeName) const
{
  ATH_MSG_VERBOSE( name() << "processing shield volume "<< volumeName<<"  in mode:"<< type );

  const Trk::TrackingVolume* tVol = 0;

  unsigned int colorCode = m_colorCode;

  std::vector<const Trk::DetachedTrackingVolume*> blendVols;

  //getPartitionFromMaterial(vol);

  // retrieve cylinder
  const Trk::CylinderVolumeBounds* cyl=dynamic_cast<const Trk::CylinderVolumeBounds*> (&(vol->volumeBounds()));
  if (!cyl) {
    ATH_MSG_ERROR( " process volume: volume cylinder boundaries not retrieved, return 0 " );
    return 0; 
  }
  // create vector of zSteps for this volume
  std::vector<float> zSteps;
  zSteps.clear();
  double zPos = vol->center()[2];
  double hz = cyl->halflengthZ();
  double z1 = zPos-hz;  double z2 = zPos+hz ;
  zSteps.push_back(z1);
  for (unsigned int iz=0;iz<m_shieldZPart.size();iz++) {
    if ( m_shieldZPart[iz]> z1 && m_shieldZPart[iz] < z2 ) {
      zSteps.push_back(m_shieldZPart[iz]);
      z1 = m_shieldZPart[iz];
    }
  }
  zSteps.push_back(z2);

  // phi binning trivial
  m_adjustedPhi.clear();
  m_adjustedPhi.push_back(0.);

  unsigned int etaN = zSteps.size()-1;

  // create z,h bin utilities
  //Trk::BinUtility1DZZ* zBinUtil = new Trk::BinUtility1DZZ(zSteps);
  //Trk::BinUtility1DF* pBinUtil = new Trk::BinUtility1DF(m_adjustedPhi);
  Trk::BinUtility* zBinUtil = new Trk::BinUtility(zSteps, Trk::BinningOption::open, Trk::BinningValue::binZ );
  //Trk::BinUtility* pBinUtil = new Trk::BinUtility(m_adjustedPhi,  Trk::BinningOption::closed, Trk::BinningValue::binPhi );
  Trk::BinUtility* pBinUtil = new Trk::BinUtility( 1, -M_PI, M_PI, Trk::BinningOption::closed, Trk::BinningValue::binPhi ); 
  std::vector<std::vector<Trk::BinUtility*> >*  hBinUtil=new std::vector<std::vector<Trk::BinUtility*> >;
  float phiRef = 0.;
  for (unsigned iz=0;iz < zSteps.size()-1; iz++) {
    std::vector<Trk::BinUtility*> phBinUtil;
    phBinUtil.push_back(new Trk::BinUtility(phiRef,m_shieldHPart[type]) );
    hBinUtil->push_back(phBinUtil);
  }

  // subvolume boundaries
  Trk::CylinderVolumeBounds* subBds=0;

  // create subvolumes & BinnedArray
  std::vector<Trk::TrackingVolumeOrderPosition>  subVolumesVect;
  std::vector<std::vector<std::vector<const Trk::TrackingVolume*> > > subVolumes;
  std::vector<std::vector<Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > > > hBins;
  std::vector<const Trk::TrackingVolume*> sVolsInn;             // for gluing
  std::vector<const Trk::TrackingVolume*> sVolsOut;             // for gluing
  std::vector<const Trk::TrackingVolume*> sVolsNeg;             // for gluing
  std::vector<const Trk::TrackingVolume*> sVolsPos;             // for gluing
  for (unsigned int eta = 0; eta < zSteps.size()-1; eta++) {
    if (colorCode>0) colorCode = 26 -colorCode;
    double posZ = 0.5*(zSteps[eta] + zSteps[eta+1]) ;
    double   hZ = 0.5*fabs(zSteps[eta+1] - zSteps[eta]) ;
    std::vector<std::vector<const Trk::TrackingVolume*> > phiSubs;
    std::vector<Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > >  phBins;
    int phi = 0;
    double posPhi = 0.;
    double phiSect = M_PI;
    std::vector<std::pair<int,float> > hSteps =  m_shieldHPart[type];
    std::vector<const Trk::TrackingVolume*> hSubs;
    std::vector<Trk::TrackingVolumeOrderPosition> hSubsTr;
    unsigned int hCode = 1; 
    for (unsigned int h = 0; h < hSteps.size()-1; h++) {
      hCode = (colorCode>0) ? 1 - hCode : 0; 
      // define subvolume
      subBds = new Trk::CylinderVolumeBounds(hSteps[h].second,
					     hSteps[h+1].second,
					     phiSect, 
					     hZ);
      Amg::Transform3D* transf = new Amg::Transform3D(Amg::AngleAxis3D(posPhi,Amg::Vector3D(0.,0.,1.))*Amg::Translation3D(Amg::Vector3D(0.,0.,posZ)));
      Trk::Volume subVol(transf, subBds);
      
      // enclosed muon objects ? also adjusts material properties in case of material blend  
      std::string volName = volumeName +MuonGM::buildString(eta,2) +MuonGM::buildString(phi,2) +MuonGM::buildString(h,2) ; 
      blendVols.clear();
      std::vector<const Trk::DetachedTrackingVolume*>* detVols= getDetachedObjects( &subVol, blendVols);
      
      const Trk::TrackingVolume* sVol = new Trk::TrackingVolume( subVol,
								 m_muonMaterial,
								 detVols,
								 volName );
      
      // statistics
      m_frameNum++ ; if (detVols) m_frameStat += detVols->size();  
      // prepare blending
      if (m_blendInertMaterial && blendVols.size()) {
	for (unsigned int id=0;id<blendVols.size();id++) {
	  if (!m_blendMap[blendVols[id]]) {
	    m_blendMap[blendVols[id]] = new std::vector<const Trk::TrackingVolume*>;
	    m_blendVols.push_back(blendVols[id]);
	  }
	  m_blendMap[blendVols[id]]->push_back(sVol);
	}
      }  
      // reference point for the check of envelope
      double posR = 0.5*(hSteps[h].second+hSteps[h+1].second);
      // loop over inner cutouts
      for (unsigned int in=1; in<m_msCutoutsIn.size(); in++) {
	if (posZ>=m_msCutoutsIn[in].second && posZ<=m_msCutoutsIn[in-1].second) {
	  if (posR < m_msCutoutsIn[in].first) sVol->sign(Trk::BeamPipe);
	  break;
	}
      }
      //
      sVol->registerColorCode(colorCode+hCode);
      // reference position 
      Amg::Vector3D gp(subBds->mediumRadius(),0.,0.);
      subVolumesVect.push_back(Trk::TrackingVolumeOrderPosition(Trk::SharedObject<const Trk::TrackingVolume>(sVol, false),
								Amg::Vector3D((*transf)*gp)));
      hSubsTr.push_back(Trk::TrackingVolumeOrderPosition(Trk::SharedObject<const Trk::TrackingVolume>(sVol, true),
							 Amg::Vector3D((*transf)*gp)));
      hSubs.push_back(sVol);
      
      //glue subVolume
      if (h==0)                sVolsInn.push_back(sVol); 
      if (h==hSteps.size()-2)  sVolsOut.push_back(sVol); 
      if (eta==0)      sVolsNeg.push_back(sVol); 
      if (eta==etaN-1) sVolsPos.push_back(sVol); 
      // in R/H
      if (h>0) { // glue 'manually'
	m_trackingVolumeHelper->setInsideTrackingVolume(*sVol, Trk::tubeSectorInnerCover,hSubs[h-1]); 
	m_trackingVolumeHelper->setOutsideTrackingVolume(*(hSubs[h-1]), Trk::tubeSectorOuterCover,sVol);
      }
      // in eta
      if ( etaN>1 && eta>0) m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*sVol, Trk::negativeFaceXY, hBins[eta-1][phi]);     
    }	
    phiSubs.push_back(hSubs); 
    Trk::BinnedArray1D<Trk::TrackingVolume>* volBinArray = new Trk::BinnedArray1D<Trk::TrackingVolume>(hSubsTr,(*hBinUtil)[eta][phi]->clone());
    phBins.push_back(Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> >(volBinArray));

    // finish eta gluing
    if ( etaN>1 && eta>0) {
      for (unsigned int j=0; j<subVolumes[eta-1][phi].size(); j++) {
	m_trackingVolumeHelper->setOutsideTrackingVolumeArray(*(subVolumes[eta-1][phi][j]), Trk::positiveFaceXY,phBins[phi]);
      }
    }
    subVolumes.push_back(phiSubs);
    hBins.push_back(phBins);
  }
    
  //Trk::BinUtility3DZFH* volBinUtil=new Trk::BinUtility3DZFH(zBinUtil,pBinUtil,hBinUtil,new Amg::Transform3D(vol->transform()));

  Trk::BinnedArray1D1D1D<Trk::TrackingVolume>* subVols=new Trk::BinnedArray1D1D1D<Trk::TrackingVolume>(subVolumesVect,zBinUtil,pBinUtil,hBinUtil);
  
  tVol = new Trk::TrackingVolume( *vol,
				  m_muonMaterial,
				  0,subVols,
				  volumeName);
  // register glue volumes
  const Trk::GlueVolumesDescriptor& volGlueVolumes = tVol->glueVolumesDescriptor();
  volGlueVolumes.registerGlueVolumes(Trk::tubeInnerCover,sVolsInn);
  volGlueVolumes.registerGlueVolumes(Trk::tubeOuterCover,sVolsOut);
  volGlueVolumes.registerGlueVolumes(Trk::negativeFaceXY,sVolsNeg);
  volGlueVolumes.registerGlueVolumes(Trk::positiveFaceXY,sVolsPos);
  
  return tVol;
} 

std::vector<const Trk::DetachedTrackingVolume*>* Muon::MuonTrackingGeometryBuilder::getDetachedObjects(const Trk::Volume* vol, std::vector<const Trk::DetachedTrackingVolume*>& blendVols, int mode ) const
{
  // mode : 0 all, 1 active only, 2 inert only

  std::vector<const Trk::DetachedTrackingVolume*>* detTVs = 0;

  if (!m_stations && !m_inertObjs) return detTVs;
  
  // get min/max Z/Phi from volume (allways a cylinder/bevelled cylinder volume )  
  const Trk::CylinderVolumeBounds* cyl = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(vol->volumeBounds()));
  const Trk::BevelledCylinderVolumeBounds* bcyl = dynamic_cast<const Trk::BevelledCylinderVolumeBounds*> (&(vol->volumeBounds()));
   
  double rmed = 0.; double dphi = 0.; double hz = 0.; double rMin = 0.; double rMax = 0.; double rMaxc = 0.; int type = 0;
  if (cyl) {
    rmed = cyl->mediumRadius();
    dphi = cyl->halfPhiSector();
    hz = cyl->halflengthZ();
    rMin = cyl->innerRadius();
    rMax = cyl->outerRadius();
    rMaxc = rMax;
  } else if (bcyl) {
    rmed = bcyl->mediumRadius();
    dphi = bcyl->halfPhiSector();
    hz = bcyl->halflengthZ();
    rMin = bcyl->innerRadius();
    rMax = bcyl->outerRadius();    
    rMaxc = rMax;    
    type = bcyl->type();
    if (type>1) rMaxc *=1./cos(dphi);
  } else return 0;
 
  Amg::Vector3D center(rmed,0.,0.);
  center = vol->transform() * center;

  double zMin = center[2] - hz; 
  double zMax = center[2] + hz;
  double pMin = 0.;
  double pMax = +2*M_PI;
  bool phiLim = false;
  if (dphi < M_PI) { 
    pMin = center.phi() - dphi + M_PI; 
    pMax = center.phi() + dphi + M_PI;
    phiLim = true;
  } 

  ATH_MSG_VERBOSE( " zMin "<< zMin <<" zMax " <<zMax << " rMin "<< rMin <<" rMax "<<rMax << " rMaxc " << rMaxc << " phi limits "<< pMin <<" phiMax "<<pMax <<  " phiLim " << phiLim);

  // define detector region : can extend over several
  int gMin = (zMax <= -m_bigWheel) ? 0 : 1;
  if ( zMin >= m_bigWheel ) gMin = 8;
  else if ( zMin >= m_innerEndcapZ ) gMin = 7;
  else if ( zMin >= m_ectZ )         gMin = 6;
  else if ( zMin >= m_diskShieldZ )      gMin = 5;
  else if ( zMin >=-m_diskShieldZ )      gMin = 4;
  else if ( zMin >=-m_ectZ )         gMin = 3;
  else if ( zMin >=-m_innerEndcapZ ) gMin = 2;
  int gMax = (zMax >= m_bigWheel) ? 8 : 7;
  if ( zMax <= -m_bigWheel ) gMax = 0;
  else if ( zMax <= -m_innerEndcapZ ) gMax = 1;
  else if ( zMax <= -m_ectZ )         gMax = 2;
  else if ( zMax <= -m_diskShieldZ )      gMax = 3;
  else if ( zMax <= m_diskShieldZ )      gMax = 4;
  else if ( zMax <= m_ectZ )         gMax = 5;
  else if ( zMax <= m_innerEndcapZ ) gMax = 6;

  ATH_MSG_VERBOSE( " active volumes gMin "<< gMin<<" gMax "<<gMax);
   
  std::list<const Trk::DetachedTrackingVolume*> detached;
  // active, use corrected rMax
  if (mode <2 && m_stationSpan) {
    for (int gMode=gMin; gMode<=gMax; gMode++) {
      for (unsigned int i=0; i<(*m_stationSpan)[gMode]->size() ; i++) {
	const Muon::Span* s = (*((*m_stationSpan)[gMode]))[i].second;          // span
	const Trk::DetachedTrackingVolume* station = (*((*m_stationSpan)[gMode]))[i].first;   // station
	bool rLimit = !m_static3d || ( (*s)[4] <= rMaxc && (*s)[5] >= rMin );
// Check meanZ for BME stations
        bool meanZOK = false;
        if(station->name()=="BME1_Station"||station->name()=="BME2_Station") {
            if( ((*s)[0]+(*s)[1])/2.< zMax&&((*s)[0]+(*s)[1])/2.> zMin ) meanZOK = true;
            if( ((*s)[2]+(*s)[3])/2 <pMin&&phiLim) meanZOK = false;
            if( ((*s)[2]+(*s)[3])/2 <pMin&&phiLim) meanZOK = false;
        }
        if ( rLimit && (((*s)[0] < zMax && (*s)[1] > zMin)||meanZOK) ) {
          bool accepted = false;
          if (phiLim) {
              if (pMin>=0 && pMax<=2*M_PI) {
                if ( (*s)[2]<=(*s)[3] && (*s)[2] <= pMax && (*s)[3] >= pMin )  accepted = true;
                if ( (*s)[2]>(*s)[3] && ((*s)[2] <= pMax || (*s)[3] >= pMin) ) accepted = true;
              } else if (pMin < 0) {
                if ( (*s)[2]<=(*s)[3] && ((*s)[2] <= pMax || (*s)[3] >= pMin+2*M_PI) ) accepted = true;
                if ( (*s)[2]>(*s)[3]  ) accepted = true;
              } else if (pMax > 2*M_PI) {
                if ( (*s)[2]<=(*s)[3] && ((*s)[2] <= pMax-2*M_PI || (*s)[3] >= pMin) ) accepted = true;
                if ( (*s)[2]>(*s)[3]  ) accepted = true;
              }
          } else  accepted = true;
          if(meanZOK) accepted = true;
          if (accepted) {
              detached.push_back(station);
              ATH_MSG_VERBOSE(" active volume accepted by rLimit " <<  station->name() << " zMin " << zMin << " zMax " << zMax << " rMin " << rMin << " rMax " << rMax << " PhiMin " << pMin  << " PhiMax " << pMax);
          }
        } else {
//            ATH_MSG_VERBOSE( " active volume rejected by rLimit " <<  station->name() );
	} 
      }
    }
  }
  // passive 
  if (mode !=1 && m_inertSpan) {
    for (int gMode=gMin; gMode<=gMax; gMode++) {
      for (unsigned int i=0; i<(*m_inertSpan)[gMode]->size() ; i++) {
	const Muon::Span* s = (*((*m_inertSpan)[gMode]))[i].second;
	const Trk::DetachedTrackingVolume* inert = (*((*m_inertSpan)[gMode]))[i].first;
	//bool rail = ( (*m_inertObjs)[i]->name() == "Rail" ) ? true : false; 
	bool rLimit = (!m_static3d || ( (*s)[4] <= rMaxc && (*s)[5] >= rMin ) ); 
	if ( rLimit && (*s)[0] < zMax && (*s)[1] > zMin ) {
	  bool accepted = false;
	  if (phiLim) {
	    if (pMin>=0 && pMax<=2*M_PI) {
	      if ( (*s)[2]<=(*s)[3] && (*s)[2] <= pMax && (*s)[3] >= pMin )  accepted = true;
	      if ( (*s)[2]>(*s)[3] && ((*s)[2] <= pMax || (*s)[3] >= pMin) ) accepted = true;
	    } else if (pMin < 0) {
	      if ( (*s)[2]<=(*s)[3] && ((*s)[2] <= pMax || (*s)[3] >= pMin+2*M_PI) ) accepted = true;
	      if ( (*s)[2]>(*s)[3]  ) accepted = true;
	    } else if (pMax > 2*M_PI) {
	      if ( (*s)[2]<=(*s)[3] && ((*s)[2] <= pMax-2*M_PI || (*s)[3] >= pMin) ) accepted = true;
	      if ( (*s)[2]>(*s)[3]  ) accepted = true;
	    }
	  } else  accepted = true;
	  if (accepted) {
	    bool perm = inert->name().substr(inert->name().size()-4,4)=="PERM";
	    if ( !m_blendInertMaterial || !m_removeBlended || perm ) detached.push_back(inert);
	    if ( m_blendInertMaterial && !perm ) blendVols.push_back(inert);
            ATH_MSG_VERBOSE( " Inert volume accepted by rLimit " <<  inert->name() << " zMin " << zMin << " zMax " << zMax << " rMin " << rMin << " rMax " << rMax << " PhiMin " << pMin  << " PhiMax " << pMax);
	  }
	}
      } 
    }
  }
  if (!detached.empty()) detTVs = new std::vector<const Trk::DetachedTrackingVolume*>(detached.begin(), detached.end()); 
  return detTVs;
}

bool Muon::MuonTrackingGeometryBuilder::enclosed(const Trk::Volume* vol, const Muon::Span* s) const
{
  bool encl = false;
  double tol = 1.;
  double ptol = 0.11;     // 0.08 for BT, 0.11 feet
  
  // get min/max Z/Phi from volume (allways a cylinder/bevelled cylinder volume )  
  const Trk::CylinderVolumeBounds* cyl = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(vol->volumeBounds()));
  const Trk::BevelledCylinderVolumeBounds* bcyl = dynamic_cast<const Trk::BevelledCylinderVolumeBounds*> (&(vol->volumeBounds()));
   
  double rmed = 0.; double dphi = 0.; double hz = 0.; double rMin = 0.; double rMax = 0.; double rMaxc = 0.; int type = 0;
  if (cyl) {
    rmed = cyl->mediumRadius();
    dphi = cyl->halfPhiSector();
    hz = cyl->halflengthZ();
    rMin = cyl->innerRadius();
    rMax = cyl->outerRadius();
    rMaxc = rMax;
  } else if (bcyl) {
    rmed = bcyl->mediumRadius();
    dphi = bcyl->halfPhiSector();
    hz = bcyl->halflengthZ();
    rMin = bcyl->innerRadius();
    rMax = bcyl->outerRadius();    
    rMaxc = rMax;    
    type = bcyl->type();
    if (type>1) rMaxc *=1./cos(dphi);
  } else return 0;
 
  Amg::Vector3D center(rmed,0.,0.);
  center = vol->transform() * center;

  double zMin = center[2] - hz; 
  double zMax = center[2] + hz;
  double pMin = 0.;
  double pMax = +2*M_PI;
  bool phiLim = false;
  if (dphi < M_PI) { 
    pMin = center.phi() - dphi + M_PI; 
    pMax = center.phi() + dphi + M_PI;
    phiLim = true;
  } 
  //
  //const Muon::Span* s = findVolumeSpan(&(cs->volumeBounds()), cs->transform(), 0.,0.) ;
  //std::auto_ptr<const Muon::Span> s (findVolumeSpan(&(cs->volumeBounds()), cs->transform(), 0.,0.) );
  ATH_MSG_VERBOSE( "enclosing volume:z:"<< zMin<<","<<zMax<<":r:"<< rMin<<","<<rMax<<":phi:"<<pMin<<","<<pMax);
  //
  bool rLimit = (!m_static3d || ( (*s)[4] < rMax-tol && (*s)[5] > rMin+tol ) ); 
  if ( rLimit && (*s)[0] < zMax-tol && (*s)[1] > zMin+tol ) {
    if (phiLim) {
      if (pMin>=0 && pMax<=2*M_PI) {
	if ( (*s)[2]<=(*s)[3] && (*s)[2] < pMax+ptol && (*s)[3] > pMin-ptol )  return true;
	if ( (*s)[2]>(*s)[3] && ((*s)[2] < pMax-ptol || (*s)[3] > pMin+ptol) ) return true;
      } else if (pMin < 0) {
	if ( (*s)[2]<=(*s)[3] && ((*s)[2] < pMax+ptol || (*s)[3] > pMin-ptol+2*M_PI) ) return true;
	if ( (*s)[2]>(*s)[3]  ) return true;
      } else if (pMax > 2*M_PI) {
	if ( (*s)[2]<=(*s)[3] && ((*s)[2] < pMax+ptol-2*M_PI || (*s)[3] > pMin-ptol) ) return true;
	if ( (*s)[2]>(*s)[3]  ) return true;
      }
    } else {
      return true;
    }
  }
  return encl;
}

void Muon::MuonTrackingGeometryBuilder::checkVolume(const Trk::TrackingVolume* vol ) const
{

  std::cout << "MuonTrackingGeometryBuilder::checkVolume: " << vol->volumeName() << std::endl;

  const Trk::CylinderVolumeBounds* cyl = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(vol->volumeBounds()));
  if (!cyl) {
    std::cout << "MuonTrackingGeometryBuilder::checkVolume:not a cylinder, return" << std::endl;
    return;
  }
  std::cout << "cylinder dimensions:innerR,outerR,halfZ,halfPhi:" << cyl->innerRadius() << "," << cyl->outerRadius()
	    << cyl->halflengthZ() << "," << cyl->halfPhiSector() << std::endl;

  const Trk::GlueVolumesDescriptor& glueDescr = vol->glueVolumesDescriptor();
  std::cout << "glue volumes:neg,pos,inn,outer" << (glueDescr.glueVolumes(Trk::negativeFaceXY)).size()<<"," 
                                                << (glueDescr.glueVolumes(Trk::positiveFaceXY)).size()<<","
                                                << (glueDescr.glueVolumes(Trk::tubeInnerCover)).size()<<","
	                                        << (glueDescr.glueVolumes(Trk::tubeOuterCover)).size()<<std::endl;
}

void Muon::MuonTrackingGeometryBuilder::getZParts() const
{
  // activeAdjustLevel:  1: separate MDT stations
  //                        +(inertLevel=0) barrel Z partition
  //                     2: split TGC
  //                        +(inertLevel=0) barrel R partition
  //                     3: split TGC supports
  // inertAdjustLevel:   1: BT,ECT     
 
  // hardcode for the moment
  m_zPartitions.clear();
  m_zPartitionsType.clear();
  m_zPartitions.reserve(120);
  m_zPartitionsType.reserve(120);

  // outer endcap
  m_zPartitions.push_back(-m_outerEndcapZ);  m_zPartitionsType.push_back(1);   // EO
  m_zPartitions.push_back(-23001.);  m_zPartitionsType.push_back(1);   // oute envelope change
  //if (m_activeAdjustLevel>0) { m_zPartitions.push_back(-21630.);  m_zPartitionsType.push_back(1); }   // EOL
  m_zPartitions.push_back(-22030.);  m_zPartitionsType.push_back(1);   // EOL
  m_zPartitions.push_back(-m_outerWheel);    m_zPartitionsType.push_back(0);   // Octogon
  // m_zPartitions.push_back(-17990.);          m_zPartitionsType.push_back(0);   // buffer
  m_zPartitions.push_back(-18650.);          m_zPartitionsType.push_back(0);   // buffer
  m_zPartitions.push_back(-m_bigWheel);      m_zPartitionsType.push_back(1);   // TGC3 
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-15225.);  m_zPartitionsType.push_back(1);}    
  if (m_activeAdjustLevel>1) { m_zPartitions.push_back(-15172.);  m_zPartitionsType.push_back(1);}   // end supp 
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-15128.);  m_zPartitionsType.push_back(1);}   // supp 
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-15070.);  m_zPartitionsType.push_back(1);}    
  if (m_activeAdjustLevel>0) { m_zPartitions.push_back(-14940.);  m_zPartitionsType.push_back(1); }  //
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-14805.);  m_zPartitionsType.push_back(1);}    
  if (m_activeAdjustLevel>1) { m_zPartitions.push_back(-14733.);  m_zPartitionsType.push_back(1);}   // end supp.
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-14708.);  m_zPartitionsType.push_back(1);}   // supp.   
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-14650.);  m_zPartitionsType.push_back(1);}   //    
  if (m_activeAdjustLevel>0) { m_zPartitions.push_back(-14560.);  m_zPartitionsType.push_back(1); }   // EML 
  if (m_activeAdjustLevel>0) { m_zPartitions.push_back(-14080.);  m_zPartitionsType.push_back(1); }   // EMS
  if (m_activeAdjustLevel>0) { m_zPartitions.push_back(-13620.);  m_zPartitionsType.push_back(1); }   // TGC
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-13525.);  m_zPartitionsType.push_back(1); }   // TGC
  if (m_activeAdjustLevel>1) { m_zPartitions.push_back(-13448.5); m_zPartitionsType.push_back(1); }   // end supp.
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-13421.5); m_zPartitionsType.push_back(1); }   // supp.
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-13346);   m_zPartitionsType.push_back(1); }   // TGC

  // inner endcap
  m_zPartitions.push_back(-m_innerEndcapZ);     m_zPartitionsType.push_back(0);   // 
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-12790);  m_zPartitionsType.push_back(0); }   // ECT 
  if (m_inertAdjustLevel>1) { m_zPartitions.push_back(-12100.); m_zPartitionsType.push_back(0); }   // 
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-12000.); m_zPartitionsType.push_back(0); }   // 
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-11210.); m_zPartitionsType.push_back(1); }   // BT 
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-10480.); m_zPartitionsType.push_back(0); }   // 
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-9700.);  m_zPartitionsType.push_back(0); }   // 
  if (m_inertAdjustLevel>1) { m_zPartitions.push_back(-9300.);  m_zPartitionsType.push_back(0); }   // rib
  if (m_inertAdjustLevel>1) { m_zPartitions.push_back(-8800.);  m_zPartitionsType.push_back(0); }   // ect
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-8610.);  m_zPartitionsType.push_back(1); }   // BT
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-8000.);  m_zPartitionsType.push_back(1); }   // BT
  m_zPartitions.push_back(-m_ectZ);  m_zPartitionsType.push_back(0);              // ECT/small wheel
  if (m_activeAdjustLevel>0) { m_zPartitions.push_back(-7450.); m_zPartitionsType.push_back(0); }   // EIS 
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-7364.); m_zPartitionsType.push_back(0); }   // EIS 
  if (m_activeAdjustLevel>0 || m_inertAdjustLevel>0) {m_zPartitions.push_back(-7170.); m_zPartitionsType.push_back(0);}   // cone assembly,TGC 
  if (m_activeAdjustLevel>0) { m_zPartitions.push_back(-7030.); m_zPartitionsType.push_back(0); }   // TGC
  if (m_activeAdjustLevel>2) { m_zPartitions.push_back(-6978.); m_zPartitionsType.push_back(0); }   // TGC

  // barrel
  m_zPartitions.push_back(-m_diskShieldZ);   m_zPartitionsType.push_back(0);   // disk 
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-6829.);  m_zPartitionsType.push_back(0); }   // back disk 
  //if (m_inertAdjustLevel>1) { m_zPartitions.push_back(-6600.);  m_zPartitionsType.push_back(0); }   // 
  m_zPartitions.push_back(-6550.); m_zPartitionsType.push_back(0);          // outer envelope change
  if (m_activeAdjustLevel>0){ m_zPartitions.push_back(-6100.);  m_zPartitionsType.push_back(0); }  
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-5503.);  m_zPartitionsType.push_back(1); }   // BT 
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-4772.);  m_zPartitionsType.push_back(0); }   //  
  if (m_activeAdjustLevel>0){ m_zPartitions.push_back(-4300.);  m_zPartitionsType.push_back(0); }  //  
  m_zPartitions.push_back(-4000.); m_zPartitionsType.push_back(0);          // outer envelope change
  if (m_inertAdjustLevel>1) { m_zPartitions.push_back(-3700.);  m_zPartitionsType.push_back(0); }   //  
  if (m_inertAdjustLevel>1) { m_zPartitions.push_back(-3300.);  m_zPartitionsType.push_back(0); }   //  
  if (m_activeAdjustLevel>0){ m_zPartitions.push_back(-2600.);  m_zPartitionsType.push_back(0); }   //  
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-2078.);  m_zPartitionsType.push_back(1); }   // BT  
  if (m_inertAdjustLevel>0) { m_zPartitions.push_back(-1347.);  m_zPartitionsType.push_back(1); }   //  cryoring 
  if (m_activeAdjustLevel>0){ m_zPartitions.push_back(-800.);   m_zPartitionsType.push_back(1); }  //  cryoring 
  if (m_inertAdjustLevel>1) { m_zPartitions.push_back(-300.);   m_zPartitionsType.push_back(0); }   //   
  if (m_inertAdjustLevel+m_activeAdjustLevel<1) { m_zPartitions.push_back(-0.7*m_diskShieldZ);   m_zPartitionsType.push_back(0); }   //

  unsigned int zSiz = m_zPartitions.size();
  for (unsigned int i = 0; i<zSiz ; i++) {
    m_zPartitions.push_back(- m_zPartitions[zSiz-1-i]);  
    if (i<zSiz-1) m_zPartitionsType.push_back(m_zPartitionsType[zSiz-2-i]);  
  } 
  
  return;
}

void Muon::MuonTrackingGeometryBuilder::getPhiParts(int mode) const
{
  if (mode==0) {             // trivial
    m_adjustedPhi.clear();
    m_adjustedPhiType.clear();
    m_adjustedPhi.push_back(0.);
    m_adjustedPhiType.push_back(0);

  } else if (mode==1) {  
    int phiNum = 1;
    if ( m_activeAdjustLevel>0 ) phiNum = m_phiPartition;
    m_adjustedPhi.resize(phiNum);
    m_adjustedPhiType.resize(phiNum);
    m_adjustedPhi[0] = 0.;
    m_adjustedPhiType[0] = 0;
    int ic = 0;
    while (ic < phiNum-1 ) {
      ic++; 
      m_adjustedPhi[ic]= m_adjustedPhi[ic-1] + 2.*M_PI/phiNum ;
      m_adjustedPhiType[ic]= 0;
    }

  } else if (mode==2) {        // barrel(BT)
    // hardcode for the moment
    m_adjustedPhi.resize(16);
    m_adjustedPhiType.resize(16);
    
    double phiSect[2];
    phiSect[0] = ( M_PI/8 - 0.105 ); 
    phiSect[1] = 0.105 ; 
    
    m_adjustedPhi[0]= - phiSect[0];
    m_adjustedPhiType[0]= 0;
    int ic = 0; int is = 1;

    while (ic < 15 ) {
      ic++; is = 1 - is;
      m_adjustedPhi[ic]= m_adjustedPhi[ic-1] + 2*phiSect[is] ;
      m_adjustedPhiType[ic]= 1-is;
    }

  } else if (mode==3) {      // ECT(+BT)
    // hardcode for the moment
    m_adjustedPhi.resize(32);
    m_adjustedPhiType.resize(32);
    
    double phiSect[3];
    phiSect[0] = 0.126;
    phiSect[2] = 0.105;
    phiSect[1] = 0.5*( M_PI/8. - phiSect[0]-phiSect[2] ); 
    
    m_adjustedPhi[0]= - phiSect[0];
    m_adjustedPhiType[0]= 0;
    m_adjustedPhi[1]= m_adjustedPhi[0]+2*phiSect[0];
    m_adjustedPhiType[1]= 1;
    int ic = 1; int is = 0;

    while (ic < 31 ) {
      ic++; is = 2 - is;
      m_adjustedPhi[ic]= m_adjustedPhi[ic-1] + 2*phiSect[1] ;
      m_adjustedPhiType[ic]= is;
      ic++;
      m_adjustedPhi[ic]= m_adjustedPhi[ic-1] + 2*phiSect[is] ;
      m_adjustedPhiType[ic]= 1;
    }
    //for (unsigned int ic=0;ic<m_adjustedPhi.size();ic++) std::cout<<"adjustedPhi:"<<ic<<","<<m_adjustedPhi[ic]<<","<<m_adjustedPhiType[ic]<< std::endl;
  }

  return;
}

void Muon::MuonTrackingGeometryBuilder::getHParts() const
{
  // hardcode for the moment
  m_hPartitions.clear();              // barrel, inner endcap, outer endcap

  // 0: barrel 2x2
  // non BT sector
  std::vector<std::pair<int,float> >  barrelZ0F0;
  barrelZ0F0.push_back( std::pair<int,float>(0,m_innerBarrelRadius) );
  if (m_activeAdjustLevel>0) {
    barrelZ0F0.push_back( std::pair<int,float>(0,4450.) );                // for DiskShieldingBackDisk
    barrelZ0F0.push_back( std::pair<int,float>(0,6500.) );                // BI/BM
    barrelZ0F0.push_back( std::pair<int,float>(0,8900.) );                // BM/BO
    barrelZ0F0.push_back( std::pair<int,float>(0,13000.) );               // outer envelope
  }
  barrelZ0F0.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  std::vector<std::pair<int,float> >  barrelZ0F1;
  barrelZ0F1.push_back( std::pair<int,float>(0,m_innerBarrelRadius) );
  if (m_inertAdjustLevel>0) {
    barrelZ0F1.push_back( std::pair<int,float>(1,4500.) );
    barrelZ0F1.push_back( std::pair<int,float>(1,5900.) );
  } else if (m_activeAdjustLevel>0) barrelZ0F1.push_back( std::pair<int,float>(0,4450.) );                
  if (m_activeAdjustLevel>0) barrelZ0F1.push_back( std::pair<int,float>(0,6500.) );
  if (m_inertAdjustLevel>0)  barrelZ0F1.push_back( std::pair<int,float>(1,8900.) );
  else if (m_activeAdjustLevel>0)  barrelZ0F1.push_back( std::pair<int,float>(0,8900.) );
  if (m_inertAdjustLevel>0)  barrelZ0F1.push_back( std::pair<int,float>(1,10100.) );
  barrelZ0F1.push_back( std::pair<int,float>(0,13000.) );               // outer envelope
  barrelZ0F1.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  // BT sector
  std::vector<std::pair<int,float> >  barrelZ1F0;
  barrelZ1F0.push_back( std::pair<int,float>(0,m_innerBarrelRadius) );
  if (m_activeAdjustLevel+m_inertAdjustLevel>0) barrelZ1F0.push_back( std::pair<int,float>(0,4450.) );                
  if (m_inertAdjustLevel>0) {
    barrelZ1F0.push_back( std::pair<int,float>(1,5800.) );
    barrelZ1F0.push_back( std::pair<int,float>(1,6500.) );
  } else if (m_activeAdjustLevel>0) barrelZ1F0.push_back( std::pair<int,float>(0,6500.) );
  if (m_inertAdjustLevel>0) {
    barrelZ1F0.push_back( std::pair<int,float>(1,6750.) );
    barrelZ1F0.push_back( std::pair<int,float>(1,8400.) );
  }
  if (m_activeAdjustLevel>0) barrelZ1F0.push_back( std::pair<int,float>(0,8770.) );  // adapted for cryoring (from 8900)
  if (m_inertAdjustLevel>0) barrelZ1F0.push_back( std::pair<int,float>(1,9850.) );   // adapted for cryoring (from 9600)
  barrelZ1F0.push_back( std::pair<int,float>(0,13000.) );               // outer envelope
  barrelZ1F0.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  std::vector<std::pair<int,float> >  barrelZ1F1;
  barrelZ1F1.push_back( std::pair<int,float>(0,m_innerBarrelRadius) );
  if (m_inertAdjustLevel>0) {
    barrelZ1F1.push_back( std::pair<int,float>(1,4500.) );
    barrelZ1F1.push_back( std::pair<int,float>(1,6000.) );
  } else if (m_activeAdjustLevel>0) barrelZ1F1.push_back( std::pair<int,float>(0,4450.) );
  if (m_activeAdjustLevel>0) barrelZ1F1.push_back( std::pair<int,float>(0,6500.) );
  if (m_inertAdjustLevel>0) barrelZ1F1.push_back( std::pair<int,float>(1,6800.) );
  if (m_inertAdjustLevel>0) {
    barrelZ1F1.push_back( std::pair<int,float>(1,8900.) );
    barrelZ1F1.push_back( std::pair<int,float>(1,10100.) );
  } else if (m_activeAdjustLevel>0) barrelZ1F1.push_back( std::pair<int,float>(0,8900.) );
  barrelZ1F1.push_back( std::pair<int,float>(0,13000.) );               // outer envelope
  barrelZ1F1.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  std::vector<std::vector<std::vector<std::pair<int,float> > > >  barrelZF(2);
  barrelZF[0].push_back(barrelZ0F0);
  barrelZF[0].push_back(barrelZ0F1);
  barrelZF[1].push_back(barrelZ1F0);
  barrelZF[1].push_back(barrelZ1F1);

  // small wheel 1x2 ( no z BT sector) 
  // non BT sector
  std::vector<std::pair<int,float> >  swZ0F0;
  swZ0F0.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_activeAdjustLevel>1) {
    swZ0F0.push_back( std::pair<int,float>(0,2700.) );                
  }
  if (m_activeAdjustLevel+m_inertAdjustLevel>0) swZ0F0.push_back( std::pair<int,float>(0,4450.) );                
  if (m_activeAdjustLevel>0) {
    swZ0F0.push_back( std::pair<int,float>(0,6560.) );                // BI/BM
    swZ0F0.push_back( std::pair<int,float>(0,8900.) );                // BM/BO
  }
  swZ0F0.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  // phi BT sector
  std::vector<std::pair<int,float> >  swZ0F1;
  swZ0F1.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_activeAdjustLevel>1) swZ0F1.push_back( std::pair<int,float>(0,2700.) );               
  if (m_inertAdjustLevel+m_activeAdjustLevel>0) swZ0F1.push_back( std::pair<int,float>(0,4450.) );
  if (m_inertAdjustLevel>0) swZ0F1.push_back( std::pair<int,float>(1,5900.) );
  if (m_activeAdjustLevel>0) swZ0F1.push_back( std::pair<int,float>(0,6560.) );
  if (m_inertAdjustLevel>0) {
    swZ0F1.push_back( std::pair<int,float>(1,8900.) );
    swZ0F1.push_back( std::pair<int,float>(1,10100.) );
  } else if (m_activeAdjustLevel>0) swZ0F1.push_back( std::pair<int,float>(0,8900.) );
  swZ0F1.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  std::vector<std::vector<std::vector<std::pair<int,float> > > >  swZF(1);
  swZF[0].push_back(swZ0F0);
  swZF[0].push_back(swZ0F1);

  // inner endcap/ECT 2x3
  // ect coil, non-BT z
  std::vector<std::pair<int,float> >  innerZ0F0;
  innerZ0F0.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_inertAdjustLevel>0) innerZ0F0.push_back( std::pair<int,float>(0,1100.) );
  if (m_inertAdjustLevel>1) innerZ0F0.push_back( std::pair<int,float>(1,5150.) );
  if (m_inertAdjustLevel>0) innerZ0F0.push_back( std::pair<int,float>(1,5300.) );
  if (m_activeAdjustLevel>0) {
    innerZ0F0.push_back( std::pair<int,float>(0,6500.) );
    innerZ0F0.push_back( std::pair<int,float>(0,8900.) );
  }
  innerZ0F0.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  // coil gap, non-BT z
  std::vector<std::pair<int,float> >  innerZ0F1;
  innerZ0F1.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_inertAdjustLevel>0) innerZ0F1.push_back( std::pair<int,float>(0,1100.) );
  if (m_inertAdjustLevel>1) {
    innerZ0F1.push_back( std::pair<int,float>(1,1400.) );
    innerZ0F1.push_back( std::pair<int,float>(1,1685.) );
  }
  if (m_inertAdjustLevel>0) {
    innerZ0F1.push_back( std::pair<int,float>(1,4700.) );
    innerZ0F1.push_back( std::pair<int,float>(1,5900.) );
  }
  if (m_activeAdjustLevel>0) {
    innerZ0F1.push_back( std::pair<int,float>(0,6500.) );
    innerZ0F1.push_back( std::pair<int,float>(0,8900.) );
  }
  innerZ0F1.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  // BT coil, no-BT z 
  std::vector<std::pair<int,float> >  innerZ0F2;
  innerZ0F2.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_inertAdjustLevel>0) innerZ0F2.push_back( std::pair<int,float>(0,1100.) );
  if (m_inertAdjustLevel>1) {
    innerZ0F2.push_back( std::pair<int,float>(1,1400.) );
    innerZ0F2.push_back( std::pair<int,float>(1,1685.) );
  }
  if (m_inertAdjustLevel>0) {
    innerZ0F2.push_back( std::pair<int,float>(1,4450.) );
    innerZ0F2.push_back( std::pair<int,float>(1,5900.) );
  }
  if (m_activeAdjustLevel>0) innerZ0F2.push_back( std::pair<int,float>(0,6500.) );
  if (m_inertAdjustLevel>0) {
    innerZ0F2.push_back( std::pair<int,float>(1,8900.) );
    innerZ0F2.push_back( std::pair<int,float>(1,10100.) );
  } else if (m_activeAdjustLevel>0) innerZ0F2.push_back( std::pair<int,float>(0,8900.) );
  innerZ0F2.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  // ect coil, z BT sector
  std::vector<std::pair<int,float> >  innerZ1F0;
  innerZ1F0.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_inertAdjustLevel>0) innerZ1F0.push_back( std::pair<int,float>(0,1100.) );
  if (m_inertAdjustLevel>1) innerZ1F0.push_back( std::pair<int,float>(1,5150.) );
  if (m_inertAdjustLevel>0) innerZ1F0.push_back( std::pair<int,float>(1,5300.) );
  if (m_inertAdjustLevel>0) innerZ1F0.push_back( std::pair<int,float>(1,5800.) );
  if (m_inertAdjustLevel>0) innerZ1F0.push_back( std::pair<int,float>(1,6750.) );
  else if (m_activeAdjustLevel>0) innerZ1F0.push_back( std::pair<int,float>(0,6500.) );
  if (m_inertAdjustLevel>0) {
    innerZ1F0.push_back( std::pair<int,float>(1,8400.) );
    innerZ1F0.push_back( std::pair<int,float>(1,9600.) );
  } else if (m_activeAdjustLevel>0) innerZ1F0.push_back( std::pair<int,float>(0,8900.) );
  innerZ1F0.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  // coil gap, BT z sector
  std::vector<std::pair<int,float> >  innerZ1F1;
  innerZ1F1.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_inertAdjustLevel>0)  innerZ1F1.push_back( std::pair<int,float>(0,1100.) );
  if (m_inertAdjustLevel>1) {
    innerZ1F1.push_back( std::pair<int,float>(1,1400.) );
    innerZ1F1.push_back( std::pair<int,float>(1,1685.) );
  }
  if (m_inertAdjustLevel>0) {
    innerZ1F1.push_back( std::pair<int,float>(1,4700.) );
    innerZ1F1.push_back( std::pair<int,float>(1,5800.) );
    innerZ1F1.push_back( std::pair<int,float>(1,6750.) );
  } else if (m_activeAdjustLevel>0) innerZ1F1.push_back( std::pair<int,float>(0,6500.) );
  if (m_inertAdjustLevel>0) {
    innerZ1F1.push_back( std::pair<int,float>(1,8400.) );
    innerZ1F1.push_back( std::pair<int,float>(1,9600.) );
  } else if (m_activeAdjustLevel>0) innerZ1F1.push_back( std::pair<int,float>(0,8900.) );
  innerZ1F1.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  // BT coil, BT z sector
  std::vector<std::pair<int,float> >  innerZ1F2;
  innerZ1F2.push_back( std::pair<int,float>(0,m_innerShieldRadius) );
  if (m_inertAdjustLevel>0) innerZ1F2.push_back( std::pair<int,float>(0,1100.) );
  if (m_inertAdjustLevel>1) {
    innerZ1F2.push_back( std::pair<int,float>(1,1400.) );
    innerZ1F2.push_back( std::pair<int,float>(1,1685.) );
  }
  innerZ1F2.push_back( std::pair<int,float>(0,4150.) );
  if (m_inertAdjustLevel>0) {
    innerZ1F2.push_back( std::pair<int,float>(1,4700.) );
    innerZ1F2.push_back( std::pair<int,float>(1,5900.) );
    innerZ1F2.push_back( std::pair<int,float>(1,6800.) );
  } else if (m_activeAdjustLevel>0) innerZ1F2.push_back( std::pair<int,float>(0,6500.) );
  if (m_inertAdjustLevel>0) {
    innerZ1F2.push_back( std::pair<int,float>(1,8900.) );
    innerZ1F2.push_back( std::pair<int,float>(1,10100.) );
  } else if (m_activeAdjustLevel>0) innerZ1F2.push_back( std::pair<int,float>(0,8900.) );
  innerZ1F2.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  std::vector<std::vector<std::vector<std::pair<int,float> > > >  innerZF(2);
  innerZF[0].push_back(innerZ0F0);
  innerZF[0].push_back(innerZ0F1);
  innerZF[0].push_back(innerZ0F2);
  innerZF[1].push_back(innerZ1F0);
  innerZF[1].push_back(innerZ1F1);
  innerZF[1].push_back(innerZ1F2);

  // outer 1x1
  std::vector<std::pair<int,float> >  outerZ0F0;
  outerZ0F0.push_back( std::pair<int,float>(0,m_outerShieldRadius) );
  outerZ0F0.push_back( std::pair<int,float>(0,2750.) );       // outer envelope 
  outerZ0F0.push_back( std::pair<int,float>(0,12650.) );      // outer envelope
  outerZ0F0.push_back( std::pair<int,float>(0,13400.) );      // outer envelope
  outerZ0F0.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  std::vector<std::pair<int,float> >  outerZ0F1;
  outerZ0F1.push_back( std::pair<int,float>(0,m_outerShieldRadius) );
  outerZ0F1.push_back( std::pair<int,float>(0,2750.) );      // outer envelope
  if ( m_activeAdjustLevel>0 ) {
    outerZ0F1.push_back( std::pair<int,float>(0,3600.) );
    outerZ0F1.push_back( std::pair<int,float>(0,5300.) );
    outerZ0F1.push_back( std::pair<int,float>(0,7000.) );
    outerZ0F1.push_back( std::pair<int,float>(0,8500.) );
    outerZ0F1.push_back( std::pair<int,float>(0,10000.) );
    outerZ0F1.push_back( std::pair<int,float>(0,12000.) );
  }
  outerZ0F1.push_back( std::pair<int,float>(0,12650.) );      // outer envelope
  outerZ0F1.push_back( std::pair<int,float>(0,13400.) );      // outer envelope
  outerZ0F1.push_back( std::pair<int,float>(0,m_outerBarrelRadius) );

  std::vector<std::vector<std::vector<std::pair<int,float> > > >  outerZF(2);
  outerZF[0].push_back(outerZ0F0);
  outerZF[0].push_back(outerZ0F0);
  outerZF[0].push_back(outerZ0F0);
  outerZF[1].push_back(outerZ0F1);
  outerZF[1].push_back(outerZ0F1);
  outerZF[1].push_back(outerZ0F1);

  // collect everything
  m_hPartitions.push_back(barrelZF);
  m_hPartitions.push_back(swZF);
  m_hPartitions.push_back(innerZF);
  m_hPartitions.push_back(outerZF);
   
  return;
}

void Muon::MuonTrackingGeometryBuilder::getShieldParts() const
{
  m_shieldZPart.resize(18);

  m_shieldZPart[0]=-21900.;      // elm2
  m_shieldZPart[1]=-21500.;      // elm1
  m_shieldZPart[2]=-21000.;      // octogon
  m_shieldZPart[3]=-18000.;      // tube
  m_shieldZPart[4]=-12882.;      // ect
  m_shieldZPart[5]= -7930.;      // ect
  m_shieldZPart[6]= -7914.;      // cone
  m_shieldZPart[7]= -6941.;      // disk
  m_shieldZPart[8]= -6783.;       //
  for (unsigned int i = 9; i<18 ; i++) m_shieldZPart[i] = - m_shieldZPart[17-i];  

  m_shieldHPart.clear();

  std::vector<std::pair<int,float> >  outerShield;
  outerShield.push_back(std::pair<int,float>(0,m_beamPipeRadius));
  outerShield.push_back(std::pair<int,float>(0,279.));    // outer envelope
  outerShield.push_back(std::pair<int,float>(0,436.7));   // outer envelope
  outerShield.push_back(std::pair<int,float>(0,1050.));   // outer envelope 
  outerShield.push_back(std::pair<int,float>(0,m_outerShieldRadius));
  m_shieldHPart.push_back(outerShield);

  std::vector<std::pair<int,float> >  innerShield;
  innerShield.push_back(std::pair<int,float>(0,m_beamPipeRadius));
  innerShield.push_back(std::pair<int,float>(0,530.));
  innerShield.push_back(std::pair<int,float>(0,m_innerShieldRadius));
  m_shieldHPart.push_back(innerShield);

  std::vector<std::pair<int,float> >  diskShield;
  diskShield.push_back(std::pair<int,float>(0,0.));
  diskShield.push_back(std::pair<int,float>(0,540.));
  diskShield.push_back(std::pair<int,float>(0,750.));
  diskShield.push_back(std::pair<int,float>(0,2700.));
  diskShield.push_back(std::pair<int,float>(0,4255.));
  m_shieldHPart.push_back(diskShield);

  return;
}

double  Muon::MuonTrackingGeometryBuilder::calculateVolume( const Trk::Volume* envelope) const
{
  double envVol = 0.;
  
  if (!envelope) return 0.;
  
  const Trk::CylinderVolumeBounds*  cyl = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(envelope->volumeBounds()));
  const Trk::CuboidVolumeBounds*    box = dynamic_cast<const Trk::CuboidVolumeBounds*> (&(envelope->volumeBounds()));
  const Trk::TrapezoidVolumeBounds* trd = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(envelope->volumeBounds()));
  const Trk::BevelledCylinderVolumeBounds*  bcyl = dynamic_cast<const Trk::BevelledCylinderVolumeBounds*> (&(envelope->volumeBounds()));
  const Trk::PrismVolumeBounds* prism = dynamic_cast<const Trk::PrismVolumeBounds*> (&(envelope->volumeBounds()));
  const Trk::SimplePolygonBrepVolumeBounds* spb = dynamic_cast<const Trk::SimplePolygonBrepVolumeBounds*> (&(envelope->volumeBounds()));
  const Trk::CombinedVolumeBounds*  comb = dynamic_cast<const Trk::CombinedVolumeBounds*> (&(envelope->volumeBounds()));
  const Trk::SubtractedVolumeBounds*  sub = dynamic_cast<const Trk::SubtractedVolumeBounds*> (&(envelope->volumeBounds()));  

  if ( cyl ) envVol = 2*cyl->halfPhiSector()*(cyl->outerRadius()*cyl->outerRadius()-cyl->innerRadius()*cyl->innerRadius())*cyl->halflengthZ();
  if ( box ) envVol = (8*box->halflengthX()*box->halflengthY()*box->halflengthZ());
  if ( trd ) envVol = (4*(trd->minHalflengthX()+trd->maxHalflengthX())*trd->halflengthY()*trd->halflengthZ());
  if ( bcyl ) {
    int type = bcyl->type();
    if ( type<1 ) envVol = 2*bcyl->halfPhiSector()*(bcyl->outerRadius()*bcyl->outerRadius()-bcyl->innerRadius()*bcyl->innerRadius())*bcyl->halflengthZ(); 
    if ( type==1 ) envVol = 2*bcyl->halflengthZ()*( bcyl->halfPhiSector()*bcyl->outerRadius()*bcyl->outerRadius()
						    -bcyl->innerRadius()*bcyl->innerRadius()*tan(bcyl->halfPhiSector()) ); 
    if ( type==2 ) envVol = 2*bcyl->halflengthZ()*( -bcyl->halfPhiSector()*bcyl->innerRadius()*bcyl->innerRadius()
						    +bcyl->outerRadius()*bcyl->outerRadius()*tan(bcyl->halfPhiSector()) ); 
    if ( type==3 ) envVol = 2*bcyl->halflengthZ()*tan(bcyl->halfPhiSector())*( bcyl->outerRadius()*bcyl->outerRadius() 
									       -bcyl->innerRadius()*bcyl->innerRadius()); 
  }
  if ( prism ) {
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif    
    std::vector<std::pair<double,double> > v=prism->xyVertices();
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif      
    double a2 = v[1].first*v[1].first+v[1].second*v[1].second
               +v[0].first*v[0].first+v[0].second*v[0].second
	    -2*(v[0].first*v[1].first+v[0].second*v[1].second);
    double c2 = v[2].first*v[2].first+v[2].second*v[2].second
               +v[0].first*v[0].first+v[0].second*v[0].second
	    -2*(v[0].first*v[2].first+v[0].second*v[2].second);
    double ca = v[1].first*v[2].first+v[1].second*v[2].second
               +v[0].first*v[0].first+v[0].second*v[0].second
	       -v[0].first*v[1].first-v[0].second*v[1].second
	       -v[0].first*v[2].first-v[0].second*v[2].second;
    double vv = sqrt(c2-ca*ca/a2);
    envVol = vv*sqrt(a2)*prism->halflengthZ();
  }
  if ( spb ) {
    envVol = calculateVolume(spb->combinedVolume());    // exceptional use of combined volume (no intersections)
  }   
  if ( comb ) {
    envVol = calculateVolume(comb->first()) + calculateVolume(comb->second());
  }
  if ( sub ) {
    return -1;
  }

  return envVol;
}

void Muon::MuonTrackingGeometryBuilder::blendMaterial() const
{
  // loop over map
  //std::map<const Trk::DetachedTrackingVolume*,std::vector<const Trk::TrackingVolume*>* >::iterator mIter = m_blendMap.begin();
  std::vector<const Trk::DetachedTrackingVolume*>::iterator viter = m_blendVols.begin();

  std::vector<std::pair<const Trk::Volume*,float> >* cs = 0;
  
  //  for ( ; mIter!= m_blendMap.end(); mIter++) {
  //  cs = (*mIter).first->constituents();     
  for (;viter!= m_blendVols.end();++viter) {
    cs = (*viter)->constituents();
    if (!cs) continue;
    // find material source
    //const Trk::Material* detMat = (*mIter).first->trackingVolume();
    const Trk::Material* detMat = (*viter)->trackingVolume();
    //if ( (*mIter).first->trackingVolume()->confinedDenseVolumes()) detMat = (*(*mIter).first->trackingVolume()->confinedDenseVolumes())[0];
    if ( (*viter)->trackingVolume()->confinedDenseVolumes()) detMat = (*(*viter)->trackingVolume()->confinedDenseVolumes())[0];
    for (unsigned int ic=0; ic<cs->size(); ic++) {
      //const Trk::Volume* nCs = new Trk::Volume(*((*cs)[ic].first),(*mIter).first->trackingVolume()->transform());
      const Trk::Volume* nCs = new Trk::Volume(*((*cs)[ic].first),(*viter)->trackingVolume()->transform());
      double fraction = (*cs)[ic].second;
      double csVol = fraction*calculateVolume(nCs);      
      const Muon::Span* s = findVolumeSpan(&(nCs->volumeBounds()), nCs->transform(), 0.,0.) ;
      if (s) {
	ATH_MSG_VERBOSE("constituent:"<<ic<<":z:"<< (*s)[0]<<","<<(*s)[1]<<":r:"<< (*s)[4]<<","<<(*s)[5]
			<<":phi:"<<(*s)[2]<<","<<(*s)[3]);      
	double enVol = 0.;
	// loop over frame volumes, check if confined
	//std::vector<const Trk::TrackingVolume*>::iterator fIter = (*mIter).second->begin(); 
	std::vector<const Trk::TrackingVolume*>* vv =m_blendMap[*viter]; 
	std::vector<const Trk::TrackingVolume*>::iterator fIter=vv->begin(); 
	std::vector<bool> fEncl; 
	fEncl.clear();
	// blending factors can be saved, and not recalculated for each clone
	//for ( ; fIter!=(*mIter).second->end(); fIter++) {
	for ( ; fIter!=vv->end(); fIter++) {
	  fEncl.push_back(enclosed(*fIter,s));
	  if ( fEncl.back() ) enVol += calculateVolume(*fIter);
	}
	// diluting factor
	double dil =  enVol>0. ?  csVol/enVol : 0.;
	//std::cout << "const:dil:"<< ic<<","<<dil<< std::endl;
	if (dil>0.) { 
	  //for ( fIter=(*mIter).second->begin(); fIter!=(*mIter).second->end(); fIter++) { 
	  for ( fIter=vv->begin(); fIter!=vv->end(); fIter++) { 
	    //if (fEncl[fIter-(*mIter).second->begin()]) { (*fIter)->addMaterial(*detMat,dil); if (m_colorCode==0) (*fIter)->registerColorCode(12) ; 
	    if (fEncl[fIter-vv->begin()]) { (*fIter)->addMaterial(*detMat,dil); if (m_colorCode==0) (*fIter)->registerColorCode(12) ; 
	      //ATH_MSG_VERBOSE((*fIter)->volumeName()<<" acquires material from "<<  (*mIter).first->name());  }
	      ATH_MSG_VERBOSE((*fIter)->volumeName()<<" acquires material from "<<  (*viter)->name());  }
	  }
	  ATH_MSG_VERBOSE("diluting factor:"<< dil<<" for "<< (*viter)->name()<<","<<ic);
	} else {
	  ATH_MSG_VERBOSE("diluting factor:"<< dil<<" for "<< (*viter)->name()<<","<<ic);
	}
      }
      delete nCs; delete s;
    }
    if ( m_removeBlended ) {  ATH_MSG_VERBOSE("deleting "<< (*viter)->name()); delete *viter; }
  }
}
