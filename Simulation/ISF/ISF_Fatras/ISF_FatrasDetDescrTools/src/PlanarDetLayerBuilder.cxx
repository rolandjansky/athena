/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarDetLayerBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ISF_FatrasDetDescrTools/PlanarDetLayerBuilder.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"
#include "ISF_FatrasDetDescrModel/PlanarOverlapDescriptor.h"
#include "ISF_FatrasDetDescrModel/DiscOverlapDescriptor.h"
#include "ISF_FatrasDetDescrTools/InputLayerMaterialProvider.h"

// Trk inlcude
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/BinnedArray2D.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// STL
#include <map>
// Amg
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
// InDet 
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
// Root
#include "TTree.h"
#include "TFile.h"
#include "TH2D.h"
// Math
#include "TMath.h"
#include <math.h>
#include <vector>

// constructor
iFatras::PlanarDetLayerBuilder::PlanarDetLayerBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_pixelCase(true),
  m_siMgr(0),
  m_siMgrLocation("Pixel"),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  m_detElementMapName("Pixel_IdHashDetElementMap"),
  m_setLayerAssociation(true),
  m_identification("Pixel"),
  m_moreSurfacesToOverlapDescriptor(false),
  m_barrelLayerBinsZ(100),
  m_barrelLayerBinsPhi(1),
  m_barrelLayers(1),
  m_layerOffset(0),
  m_layerEquidistantBinning(true),
  m_layerSCTlike(false),
  //m_layerInvertOtherSurface(true),
  m_endcapLayerBinsR(100),
  m_endcapLayerBinsPhi(1),
  m_endcapDiscs(0),
  m_discOffset(0),
  m_useTrapezoidalBounds(false),
  m_discSCTlike(false),
  m_inputLayerMaterialProvider(""),
  m_customMaterial(false),
  m_customMaterialThickness(0.),
  m_customMaterialX0(0.),       
  m_customMaterialL0(0.),       
  m_customMaterialA(0.),      
  m_customMaterialZ(0.),       
  m_customMaterialRho(0.),
  m_barrelEnvelope(0.1),
  m_endcapEnvelope(0.1),
  m_checkGeo(false),
  m_thistSvc(NULL),
  m_outputFile(NULL),
  m_currentTree(NULL),
  m_x_local(0),
  m_y_local(0),
  m_x_global(0),
  m_y_global(0),
  m_z_global(0)
{
  // general steering
  declareProperty("PixelCase"                        , m_pixelCase);
  declareProperty("SiDetManagerLocation"             , m_siMgrLocation);
  declareProperty("SetLayerAssociation"              , m_setLayerAssociation);
  
  // identification
  declareProperty("Identification"       , m_identification);
  declareProperty("DetElementMapName"    , m_detElementMapName);

  // for overlap descriptor
  declareProperty("MoreSurfacesToOverlapDescriptor", m_moreSurfacesToOverlapDescriptor);
    
  // Assign custom material
  declareProperty("InputLayerMaterialProvider", m_inputLayerMaterialProvider);
  declareProperty("BarrelLayerBinsZ"     , m_barrelLayerBinsZ);
  declareProperty("BarrelLayerBinsPhi"   , m_barrelLayerBinsPhi);
  declareProperty("EndcapLayerBinsR"     , m_endcapLayerBinsR);
  declareProperty("EndcapLayerBinsPhi"   , m_endcapLayerBinsPhi);
  declareProperty("CustomMaterial"             , m_customMaterial);
  declareProperty("CustomMaterialThickness"    , m_customMaterialThickness);
  declareProperty("CustomMaterialX0"           , m_customMaterialX0);
  declareProperty("CustomMaterialL0"           , m_customMaterialL0);
  declareProperty("CustomMaterialA"            , m_customMaterialA);
  declareProperty("CustomMaterialZ"            , m_customMaterialZ);
  declareProperty("CustomMaterialRho"          , m_customMaterialRho);

  // For the Active Barrel Material
  declareProperty("BarrelLayers"         , m_barrelLayers);
  declareProperty("LayerOffset"          , m_layerOffset);
  declareProperty("LayerEquidistantBinning", m_layerEquidistantBinning);
  declareProperty("LayersZsectors"       , m_layerZsectors);
  declareProperty("LayerPhiSectors"      , m_layerPhiSectors);
  declareProperty("LayerTilt"            , m_layerTilt);
  declareProperty("LayerTiltedTilt"      , m_layerTiltedTilt);
  declareProperty("LayerMinPhi"          , m_layerMinPhi);
  declareProperty("LayerMaxPhi"          , m_layerMaxPhi);
  declareProperty("LayerPhiOffset"       , m_layerPhiOffset);
  declareProperty("LayerTiltedPhiOffset" , m_layerTiltedPhiOffset);
  declareProperty("LayerMinZ"            , m_layerMinZ);
  declareProperty("LayerMaxZ"            , m_layerMaxZ);
  declareProperty("LayerRadius"          , m_layerRadius);
  declareProperty("LayerTiltedRadius"    , m_layerTiltedRadius);
  declareProperty("LayerRSeparation"     , m_layerRseparation);
  declareProperty("LayerTiltedRSeparation", m_layerTiltedRseparation);
  declareProperty("LayerZSeparation"     , m_layerZseparation);
  declareProperty("LayerTiltedZSeparation", m_layerTiltedZseparation);
  declareProperty("LayerTiltedZGroups",    m_layerTiltedZgroups);
  declareProperty("LayerTiltedZPosition" , m_layerTiltedZposition);
  declareProperty("LayerBarrelZPosition" , m_layerBarrelZposition);
  declareProperty("LayerThickness"       , m_layerThickness);
  declareProperty("LayerLengthY"         , m_layerLengthY); 
  declareProperty("LayerTiltedLengthY"   , m_layerTiltedLengthY); 
  declareProperty("LayerLengthXmin"      , m_layerLengthXmin); 
  declareProperty("LayerLengthXmax"      , m_layerLengthXmax); 
  declareProperty("LayerTiltedLengthXmin", m_layerTiltedLengthXmin); 
  declareProperty("LayerTiltedLengthXmax", m_layerTiltedLengthXmax); 
  declareProperty("LayerPitchX"          , m_layerPitchX); 
  declareProperty("LayerPitchY"          , m_layerPitchY); 
  declareProperty("LayerRotation"        , m_layerRotation); 
  declareProperty("LayerMultipleRotation", m_layerMultipleRotation);
  declareProperty("LayerSCTlike"         , m_layerSCTlike);
  //declareProperty("LayerInvertOtherSurface", m_layerInvertOtherSurface);
  declareProperty("LayerStereo"          , m_layerStereo); 
  declareProperty("LayerTiltedStereo"    , m_layerTiltedStereo); 
  declareProperty("BarrelEnvelope"       , m_barrelEnvelope);
  declareProperty("LayerStereoSeparation", m_layerStereoSeparation); 
  declareProperty("LayerTiltedStereoZSeparation", m_layerTiltedStereoZSeparation); 
  declareProperty("AdditionalLayerRadius", m_layerAdditionalLayerR);
  
  // For the Active Endcap Material
  declareProperty("EndcapDiscs"          , m_endcapDiscs);
  declareProperty("DiscOffset"           , m_discOffset);
  declareProperty("DiscPhiSectors"       , m_discPhiSectors);
  declareProperty("DiscZpos"             , m_discZpos);
  declareProperty("DiscRingMinR"         , m_discRingMinR);
  declareProperty("DiscRingMaxR"         , m_discRingMaxR);
  declareProperty("DiscMinPhi"           , m_discMinPhi);
  declareProperty("DiscMaxPhi"           , m_discMaxPhi);
  declareProperty("DiscThickness"        , m_discThickness);
  declareProperty("DiscLengthY"          , m_discLengthY); 
  declareProperty("DiscLengthXmin"       , m_discLengthXmin); 
  declareProperty("DiscLengthXmax"       , m_discLengthXmax); 
  declareProperty("DiscPitchX"           , m_discPitchX); 
  declareProperty("DiscPitchY"           , m_discPitchY); 
  declareProperty("DiscSeparation"       , m_discSeparation);
  declareProperty("RingDisplacement"     , m_ringDisplacement);
  declareProperty("DiscSCTlike"          , m_discSCTlike);
  declareProperty("DiscStereo"           , m_discStereo); 
  declareProperty("EndcapEnvelope"       , m_endcapEnvelope);
  declareProperty("DiscStereoSeparation" , m_discStereoSeparation); 
  declareProperty("AdditionalDiscZpos"   , m_discAdditionalLayerPosZ);
  declareProperty("UseTrapezoidalBounds", m_useTrapezoidalBounds);
  declareProperty("CheckGeo"             , m_checkGeo);
}

// destructor
iFatras::PlanarDetLayerBuilder::~PlanarDetLayerBuilder()
{}

// Athena standard methods
// initialize
StatusCode iFatras::PlanarDetLayerBuilder::initialize()
{
  ATH_MSG_INFO( "initialize()" );

  // get Pixel Detector Description Manager
  if (m_pixelCase) {
    const InDetDD::PixelDetectorManager* pixMgr = 0;
    if ((detStore()->retrieve(pixMgr, m_siMgrLocation)).isFailure()) {
      ATH_MSG_ERROR( "Could not get PixelDetectorManager '" << m_siMgrLocation << "', no layers for Pixel Detector will be built. " );
    } else {
      ATH_MSG_VERBOSE( "PlanarDetLayerBuilder: PixelDetectorManager retrieved with key '" << m_siMgrLocation <<"'." );
      // assign the detector manager to the silicon manager
      m_siMgr = pixMgr;
      if (detStore()->retrieve(m_pixIdHelper, "PixelID").isFailure())
	ATH_MSG_ERROR("Could not get Pixel ID helper");
    }
  } else {
    const InDetDD::SCT_DetectorManager* sctMgr = 0;
    if ((detStore()->retrieve(sctMgr, m_siMgrLocation)).isFailure()) {
      ATH_MSG_ERROR( "Could not get SCT_DetectorManager '" << m_siMgrLocation << "', no layers for SCT Detector will be built." );
    } else {
      ATH_MSG_VERBOSE( "SCT_DetectorManager retrieved with key '" << m_siMgrLocation <<"'." );
      // assign the detector manager to the silicon manager
      m_siMgr = sctMgr;
      if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure())
	ATH_MSG_ERROR("Could not get SCT ID helper");
    }
  }
 
  if (!m_layerEquidistantBinning)
    m_layerZboundaries.reserve(m_layerZsectors.size());

  m_layerRmin.resize(m_barrelLayers, 9999);
  m_layerRmax.resize(m_barrelLayers, -9999);

  // Fill the radius vector ( m_discRmin & m_discRmax )
  m_discRmin.clear();
  m_discRmax.clear();

  unsigned int Discs = m_discPhiSectors.size();

  for (unsigned int discCounter = 0; discCounter < Discs; discCounter++) {
    
    m_discRmin.push_back(m_discRingMinR[discCounter][0]);
    m_discRmax.push_back(m_discRingMaxR[discCounter][(m_discRingMaxR[discCounter].size())-1]);

  }

  //Retrieve and/or store the map with IdHash to DetElement 
  if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_detElementMapName))){
    m_detElementMap = nullptr;
    if((detStore()->retrieve(m_detElementMap, m_detElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not retrieve collection " << m_detElementMapName);
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_VERBOSE("Found and Retrieved collection " << m_detElementMapName);
  }else{
    m_detElementMap = new iFatras::IdHashDetElementCollection;
    if((detStore()->record(m_detElementMap, m_detElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not record collection " << m_detElementMapName);
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_VERBOSE("Stored and Retrieved collection " << m_detElementMapName);
  } 

  if (m_inputLayerMaterialProvider.retrieve().isSuccess()){
    ATH_MSG_VERBOSE ( "Using InputLayerMaterialProvider " << m_inputLayerMaterialProvider);      
    m_layerMaterial = m_inputLayerMaterialProvider->handleMaterial();
    ATH_MSG_VERBOSE ( "handleMaterial Done!");      
  }

  if (m_checkGeo){

    // get THistSvc
    if (service("THistSvc",m_thistSvc).isFailure()) {
      ATH_MSG_ERROR("Cannot find THistSvc ");
      return StatusCode::FAILURE;
    }
    m_outputFile = new TFile("CheckGeometry.root","RECREATE");
    m_currentTree = new TTree("GeometryTree","Check Geometry");
    m_currentTree->Branch("local_X"       , &m_x_local, "m_x_local/D");
    m_currentTree->Branch("local_Y"       , &m_y_local, "m_y_local/D");
    m_currentTree->Branch("global_X"     , &m_x_global, "m_x_global/D");
    m_currentTree->Branch("global_Y"     , &m_y_global, "m_y_global/D");
    m_currentTree->Branch("global_Z"     , &m_z_global, "m_z_global/D");

    if( m_thistSvc->regTree("GeometryTree",m_currentTree).isFailure()) {
      ATH_MSG_ERROR("Cannot register Ttree");
      return StatusCode::FAILURE;
    }else{
      ATH_MSG_VERBOSE ( "Ttree registered" );      
    }
  }

  // Initialize SurfaceCollection for VP1
  StatusCode sc = evtStore()->retrieve(m_surfcoll,"SurfaceCollection");
  if ( sc.isFailure() ) {
    m_surfcoll = new SurfaceCollection(SG::VIEW_ELEMENTS );
    StatusCode sc = evtStore()->record(m_surfcoll,"SurfaceCollection");
    if ( sc.isFailure() )
      ATH_MSG_ERROR("Could not register SurfaceCollection in StoreGate"); 
    else
      ATH_MSG_VERBOSE("SurfaceCollection with " << m_surfcoll->size() << " elements successfully registered in StoreGate");
  }

  return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::PlanarDetLayerBuilder::finalize()
{

  if (m_checkGeo){
    m_outputFile->cd();
    m_currentTree->Write();
    m_outputFile->Close();
    ATH_MSG_VERBOSE ( "SiSmearedDigitizationTool : Writing Tree" );
  }

  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

/** LayerBuilder interface method - returning Barrel-like layers */
const std::vector<const iFatras::PlanarDetElement*>* iFatras::PlanarDetLayerBuilder::CreateElements(unsigned int layerCounter, bool isCylinder) const {

  std::vector<iFatras::PlanarDetElement*>* cElements = new std::vector<iFatras::PlanarDetElement*>();;

  if (isCylinder){

    cElements->reserve(m_layerZsectors[layerCounter]*m_layerPhiSectors[layerCounter]);

    m_previousSliceElements = new std::vector<iFatras::PlanarDetElement*>(m_layerPhiSectors[layerCounter]);
    
    m_layerZboundaries.push_back(std::vector< float >());
    
    for (unsigned int NumZ = 1; NumZ <= m_layerZsectors[layerCounter]; ++NumZ){      
      for (unsigned int NumPhi = 1; NumPhi <= m_layerPhiSectors[layerCounter]; ++NumPhi){
	iFatras::PlanarDetElement* csf = CylinderDetElement(layerCounter, NumPhi, NumZ);
	if (csf) cElements->push_back(csf);
      }
    }

    // check how many surfaces had been created
    if ( cElements->size() != m_layerZsectors[layerCounter]*m_layerPhiSectors[layerCounter])
      ATH_MSG_WARNING("Number of created elements (" << cElements->size() 
		      <<  ") does not correspond to chosen number (" << m_layerZsectors[layerCounter]*m_layerPhiSectors[layerCounter] << ")." );
    else ATH_MSG_VERBOSE("Number of created elements = " << cElements->size());

  }
  else{
    unsigned int posPhi = (layerCounter >= m_discPhiSectors.size()) ? (layerCounter - m_discPhiSectors.size()): layerCounter;
    size_t discRsectors = m_discPhiSectors[posPhi].size();

    unsigned int numPhiElement = 0;
    for (size_t irings=0; irings<discRsectors; ++irings) 
      numPhiElement+=(int)m_discPhiSectors[posPhi][irings];
        
    cElements->reserve(numPhiElement);

    for (unsigned int iring=0; iring<discRsectors; ++iring) {
      for (unsigned int NumPhi = 1; NumPhi <= (unsigned int)m_discPhiSectors[posPhi][iring]; ++NumPhi){
	iFatras::PlanarDetElement* csf = DiscDetElement(layerCounter, iring, NumPhi);
	if (csf) cElements->push_back(csf);
      }
    }

    // check how many surfaces had been created
    if ( cElements->size() != numPhiElement)
      ATH_MSG_WARNING("Number of created elements (" << cElements->size() 
		      <<  ") does not correspond to chosen number (" <<  discRsectors*numPhiElement << ")." );
    else ATH_MSG_VERBOSE("Number of created elements = " << cElements->size());

  }

  unsigned int tot_elem = cElements->size();

  std::vector<const iFatras::PlanarDetElement*>* const_Elements = new std::vector<const iFatras::PlanarDetElement*>();

  for (unsigned int El = 0; El < tot_elem; El++) {
    const_Elements->push_back(dynamic_cast<const iFatras::PlanarDetElement*>(cElements->at(El)));
  }

  return const_Elements;
}

iFatras::PlanarDetElement* iFatras::PlanarDetLayerBuilder::CylinderDetElement(unsigned int layerCounter, unsigned int Phi, unsigned int Z) const
{
  int brl_ec = 0; // For identifier : barrel elements brl_ec = 0
  int layer = m_layerOffset+layerCounter;

  int etaModule = (2*Z-m_layerZsectors[layerCounter]-1)/2; // For identifier: 0 in the central element. 
  // Then you have negative and positive values.
                                                             
  if (!(m_layerZsectors[layerCounter]%2) && etaModule>=0) // There is no 0 element if the Z segmentation is given by an even number
    etaModule+=1;      

  // For identifier: 0 is the element that corresponds to 0 rad (different convention wrt the binutility that uses [-pi; pi])
  int phiModule = Phi+m_layerPhiSectors[layerCounter]/2; 
  if ( 2*(Phi + 1) > m_layerPhiSectors[layerCounter])
    phiModule = Phi-m_layerPhiSectors[layerCounter]/2; 

  Identifier idwafer(0);
  IdentifierHash idhash(0);
  
  if(m_pixelCase) {
    idwafer = m_pixIdHelper->wafer_id(brl_ec, layer, phiModule, etaModule);
    idhash  = m_pixIdHelper->wafer_hash(m_pixIdHelper->wafer_id(idwafer));
  } else {
    idwafer = m_sctIdHelper->wafer_id(brl_ec, layer, phiModule, etaModule, 0);
    idhash  = m_sctIdHelper->wafer_hash(m_sctIdHelper->wafer_id(idwafer));
  }   

  unsigned int Tilted = 0; // Number of tilted elements in the barrel (considering only one side)
  unsigned int NonTilted = m_layerZsectors[layerCounter]; // Number of non tilted elements in the barrel (considering the entire barrel). By default it is the number of elements in z. It is changed below if we have tilted elements

  bool isTilted  = false;
  
  double rot = 0.0;
  if (!m_layerRotation.empty())
    rot = m_layerRotation[layerCounter]*TMath::Pi()/180.;
  
  if (!m_layerMultipleRotation.empty() && !m_layerMultipleRotation[layerCounter].empty()) {
    Tilted = m_layerMultipleRotation[layerCounter].size();
    NonTilted = m_layerZsectors[layerCounter] - 2*Tilted;  // Number of non tilted elements in the barrel changed
    if (Z <= Tilted) {
      rot = m_layerMultipleRotation[layerCounter][Z-1]*TMath::Pi()/180.;
      isTilted = true;
    }
    if (Z > NonTilted+Tilted) { 
      rot = m_layerMultipleRotation[layerCounter][m_layerZsectors[layerCounter]/2-etaModule]*TMath::Pi()/180.;
      isTilted = true;
    }
  }

  double PhiOffset = isTilted ? ((!m_layerTiltedPhiOffset.empty()) ? TMath::Pi()/180.*m_layerTiltedPhiOffset[layerCounter] : 0.0) : ((!m_layerPhiOffset.empty()) ? TMath::Pi()/180.*m_layerPhiOffset[layerCounter] : 0.0);

  // create the transform parameters
  double phistep = TMath::Pi()/180. *(m_layerMaxPhi[layerCounter]-m_layerMinPhi[layerCounter])/double(m_layerPhiSectors[layerCounter]);
  double phi     = TMath::Pi()/180. * m_layerMinPhi[layerCounter] + (Phi-1) * phistep + PhiOffset; // used to determine the center translation
  double phi_angle = (Phi-1) * phistep + PhiOffset; // use to determine the element rotation
  
  double stereo = 0.0;
  if (!m_layerStereo.empty())
    stereo = m_layerStereo[layerCounter]*TMath::Pi()/180.;

  if (isTilted && !m_layerTiltedStereo.empty())
    stereo = m_layerTiltedStereo[layerCounter]*TMath::Pi()/180.;;    

  double tilt = TMath::Pi()/180. * (isTilted ? m_layerTiltedTilt[layerCounter] : m_layerTilt[layerCounter]);
  
  // create the translation matrix
  double zstep   = (m_layerMaxZ[layerCounter]-m_layerMinZ[layerCounter])/double(m_layerZsectors[layerCounter]);
  double z       = 0.;

  if (!m_layerEquidistantBinning && m_layerZboundaries[layerCounter].empty()) {// if the z boundaries vector is empty, fill it with the minimum z value
    m_layerZboundaries[layerCounter].push_back(float(m_layerMinZ[layerCounter]));
  }

  if(m_layerEquidistantBinning)
    z = m_layerMinZ[layerCounter]   +   (Z-1) * zstep   + zstep/2.;
  else
    if (isTilted) {
      z = (Z <= Tilted) ? m_layerTiltedZposition[layerCounter][Z-1] : std::abs(m_layerTiltedZposition[layerCounter][m_layerZsectors[layerCounter]/2-etaModule]);
      if (Phi==m_layerPhiSectors[layerCounter]) {
	if (Z<Tilted) 
	  m_layerZboundaries[layerCounter].push_back(float(0.5*(m_layerTiltedZposition[layerCounter][Z-1]+m_layerTiltedZposition[layerCounter][Z])));// this is the center + 1/2 module length + 1/2 separation between consecutive modules
	else if (Z==Tilted) {
	  double zBound = z+m_layerTiltedLengthY[layerCounter]/2.*std::cos(m_layerMultipleRotation[layerCounter][Z-1]*TMath::Pi()/180.);
	  double TiltedBarrelSeparation = fabs(zBound) - fabs(m_layerBarrelZposition[layerCounter]);
	  m_layerZboundaries[layerCounter].push_back(float(zBound+TiltedBarrelSeparation/2.));// this is the center + 1/2 module length + 1/2 separation between tilted/non tilted modules 
	} else {
	  m_layerZboundaries[layerCounter].push_back((Z==m_layerZsectors[layerCounter]) ? float(m_layerMaxZ[layerCounter]) : float(0.5*fabs(m_layerTiltedZposition[layerCounter][m_layerZsectors[layerCounter]/2-etaModule]+m_layerTiltedZposition[layerCounter][m_layerZsectors[layerCounter]/2-etaModule-1])));// this is the center + 1/2 module length + 1/2 separation between consecutive modules // this is the maximum z value for the last module
	}
      }
    } else {
      double BarrelZstep = 2.*fabs(m_layerBarrelZposition[layerCounter])/double(NonTilted);
      z = m_layerBarrelZposition[layerCounter] +  (Z-1-Tilted) * BarrelZstep   + BarrelZstep/2.;
      if (Phi==m_layerPhiSectors[layerCounter])
	m_layerZboundaries[layerCounter].push_back(float(z+ BarrelZstep/2.));// this is the center + 1/2 module length
    }
  
  if (z>m_layerMaxZ[layerCounter])
    ATH_MSG_ERROR("z value for module bigger than the maximum value!! z = " << z << "  layerMaxZ = " << m_layerMaxZ[layerCounter]);
  
  double Rseparation = 0.;
  if (!m_layerRseparation.empty() && etaModule != 0) {
    if (isTilted) 
      Rseparation = (etaModule%2) ? -m_layerTiltedRseparation[layerCounter]*etaModule/abs(etaModule) : m_layerTiltedRseparation[layerCounter]*etaModule/abs(etaModule); 
    else 
      Rseparation = (etaModule%2) ? -m_layerRseparation[layerCounter]*etaModule/abs(etaModule) : m_layerRseparation[layerCounter]*etaModule/abs(etaModule);
  }

  double radius = (isTilted && !m_layerTiltedRadius.empty()) ? m_layerTiltedRadius[layerCounter] : m_layerRadius[layerCounter];

  double Zseparation = 0.0; // Z shift for tilted elements at same R value 
  if (isTilted && !m_layerTiltedZseparation.empty()) {
    if (Phi%2)
      Zseparation = -m_layerTiltedZseparation[layerCounter]*etaModule/abs(etaModule);
    // Add different way to group elements and shifting them in z --> For tilted elements only
    // the numbers of elements per group must be odd to be taken into account.
    // Groups of even elements has been already taken into account in the previous Zseparation evaluation
    if (!m_layerTiltedZgroups.empty() && (int)m_layerTiltedZgroups[layerCounter]%2) { 
      int Nel = m_layerTiltedZgroups[layerCounter];
      if ((Phi%Nel) == (unsigned int)(Nel-1))
	Zseparation = -m_layerTiltedZseparation[layerCounter]*etaModule/abs(etaModule);
      else if ((Phi%Nel)%2)
	Zseparation = m_layerTiltedZseparation[layerCounter]*etaModule/abs(etaModule);
      else
	Zseparation = 0.;
    }
  }

  double dx = (radius-Rseparation/2.) * cos(phi);
  double dy = (radius-Rseparation/2.) * sin(phi);
  double dz = z + Zseparation;

  Amg::Vector3D transl(dx,dy,dz);

  // create the rotation 
  Amg::RotationMatrix3D rotation;     
  
  rotation = Amg::AngleAxis3D(-TMath::Pi()/2, Amg::Vector3D::UnitX())* // you need -TMath::Pi()/2 to make the correct rotation
    Amg::AngleAxis3D(-TMath::Pi()/2 - phi_angle - tilt, Amg::Vector3D::UnitY())* // you need -TMath::Pi()/2 to make the correct rotation
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitZ());
   
  Amg::Transform3D transform(rotation, transl);
   
  if (m_layerSCTlike)
    transform*= Amg::AngleAxis3D(-stereo, Amg::Vector3D::UnitZ());
  
  // apply the rotation angle
  if (etaModule != 0)
    transform*=Amg::AngleAxis3D(etaModule/abs(etaModule)*rot, Amg::Vector3D::UnitX());
   
  bool isOuterMost = false;
  
  if(layerCounter == m_layerRadius.size())
    isOuterMost = true;
  
  if (m_layerSCTlike && m_layerStereoSeparation[layerCounter] > 0.)
    isOuterMost = false;
   
  double lengthY = m_layerLengthY[layerCounter];
  if (isTilted) lengthY = m_layerTiltedLengthY[layerCounter];
  
  double lengthXmin = m_layerLengthXmin[layerCounter];
  if (isTilted) lengthXmin = m_layerTiltedLengthXmin[layerCounter];  
  
  double lengthXmax = 0.;
  if (!isTilted &&   !m_layerLengthXmax.empty()) 
    lengthXmax = m_layerLengthXmax[layerCounter];
  else if (isTilted &&   !m_layerTiltedLengthXmax.empty()) 
    lengthXmax = m_layerTiltedLengthXmax[layerCounter];
  
  ATH_MSG_VERBOSE("Inside CylinderDetElement() --> Creating PlanarDetElement with " << Amg::toString(transform));
   
  // get the layer material
  const Trk::LayerMaterialProperties* layerMaterial = m_layerMaterial;
  
  iFatras::PlanarDetElement* planElement = new iFatras::PlanarDetElement(idwafer, idhash, transl, transform, *layerMaterial, m_layerThickness[layerCounter], lengthY, lengthXmin, lengthXmax, 0., 0., m_layerPitchX[layerCounter], m_layerPitchY[layerCounter], stereo, m_pixelCase, true, isOuterMost, m_useTrapezoidalBounds, false);
  
  double rmin = 0.;
  double rmax = 0.;
  computeRadiusMinMax(transform, planElement, rmin, rmax);
  
  if (rmin < m_layerRmin.at(layerCounter))
    m_layerRmin.at(layerCounter) = rmin;
  if (rmax > m_layerRmax.at(layerCounter))
    m_layerRmax.at(layerCounter) = rmax;
  
  if (!planElement) 
    ATH_MSG_WARNING("Inside CylinderDetElement() --> Null poiter for the Planar Detector Element.");
  
  m_detElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, planElement)); 
  
  if (m_layerSCTlike) {
    
    if(layerCounter == m_layerRadius.size())
      isOuterMost = true;
    
    double stereoRSeparation = m_layerStereoSeparation[layerCounter];
    
    double dx_os = dx + stereoRSeparation * cos(phi);
    double dy_os = dy + stereoRSeparation * sin(phi);
    double dz_os = dz;
    if (!m_layerTiltedStereoZSeparation.empty())
      dz_os += etaModule/abs(etaModule)*m_layerTiltedStereoZSeparation[layerCounter];
    
    // use the same rotation and create the translation
    Amg::Vector3D transl_os(dx_os,dy_os,dz_os);
    Amg::Transform3D transform_os(rotation, transl_os);
    
    // add the stereo angle and 180 degree rotation (if needed)
    transform_os*= Amg::AngleAxis3D(stereo, Amg::Vector3D::UnitZ());
    
    //if (m_layerInvertOtherSurface)
    //transform_os*=Amg::AngleAxis3D(TMath::Pi(), Amg::Vector3D::UnitY());
    
    // apply the rotation angle
    if (etaModule != 0)
      transform_os*=Amg::AngleAxis3D(etaModule/abs(etaModule)*rot, Amg::Vector3D::UnitX());
    
    const Trk::LayerMaterialProperties* layerMaterial_os = m_layerMaterial;
    
    if(m_pixelCase) {
      idwafer = m_pixIdHelper->wafer_id(brl_ec, layer, phiModule+1, etaModule);
      idhash  = m_pixIdHelper->wafer_hash(m_pixIdHelper->wafer_id(idwafer));
    } else {
      idwafer = m_sctIdHelper->wafer_id(brl_ec, layer, phiModule, etaModule, 1);
      idhash  = m_sctIdHelper->wafer_hash(m_sctIdHelper->wafer_id(idwafer));
    }
    
    ATH_MSG_VERBOSE("Inside CylinderDetElement() --> Creating other PlanarDetElement with " << Amg::toString(transform_os));
    
    iFatras::PlanarDetElement* planElement_os = new iFatras::PlanarDetElement(idwafer, idhash, transl_os, transform_os, *layerMaterial_os, m_layerThickness[layerCounter], lengthY, lengthXmin, lengthXmax, 0., 0., m_layerPitchX[layerCounter], m_layerPitchY[layerCounter], -stereo, m_pixelCase, true, isOuterMost, m_useTrapezoidalBounds, false);

    if (!planElement_os) ATH_MSG_WARNING("Inside CylinderDetElement() --> Null poiter for the other side Planar Detector Element.");

    double rmin_os = 0.;
    double rmax_os = 0.;
    computeRadiusMinMax(transform_os, planElement_os, rmin_os, rmax_os);
    
    if (rmin_os < m_layerRmin.at(layerCounter))
      m_layerRmin.at(layerCounter) = rmin_os;
    if (rmax_os > m_layerRmax.at(layerCounter))
      m_layerRmax.at(layerCounter) = rmax_os;
    
    // each element points to the other face
    planElement->setBackSide(planElement_os);  
    planElement_os->setFrontSide(planElement); 
    
    m_detElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, planElement_os));
    
  }
    
  //Set next/previous in phi if other elements have been already built

  if (Phi == 1) // Is the first element in phi 
    m_firstLayerElement = planElement;

  if (planElement != m_firstLayerElement)  { // if it is not the first element 
    planElement->setPrevInPhi(m_previousLayerElement);
    m_previousLayerElement->setNextInPhi(planElement);
    if (planElement->otherSide() && m_previousLayerElement->otherSide()) {
      (planElement->otherSide())->setPrevInPhi(m_previousLayerElement->otherSide());
      (m_previousLayerElement->otherSide())->setNextInPhi(planElement->otherSide());
    }
  }

  if(Phi == m_layerPhiSectors[layerCounter]) { // Phi is closed -> the last has as next the first one and the first has as previous the last.
    planElement->setNextInPhi(m_firstLayerElement);
    m_firstLayerElement->setPrevInPhi(planElement);
    if (planElement->otherSide() && m_firstLayerElement->otherSide()) {
      (planElement->otherSide())->setNextInPhi(m_firstLayerElement->otherSide());
      (m_firstLayerElement->otherSide())->setPrevInPhi(planElement->otherSide());
    }
  }
  
  m_previousLayerElement = planElement;
  
  //Set next/previous in eta if other elements have been already built
  // Eta is open -> the first slice of the cylinder has not previous elements and the last ones has not next. 
  // All the other elements have previous and next.
  if (Z == 1)
    m_previousSliceElements->at(Phi-1) = planElement;
  else{
    planElement->setPrevInEta(m_previousSliceElements->at(Phi-1));
    (m_previousSliceElements->at(Phi-1))->setNextInEta(planElement);
    if (planElement->otherSide() && m_previousSliceElements->at(Phi-1)->otherSide()) {
      (planElement->otherSide())->setPrevInEta((m_previousSliceElements->at(Phi-1))->otherSide());
      ((m_previousSliceElements->at(Phi-1))->otherSide())->setNextInEta(planElement->otherSide());
    }
    m_previousSliceElements->at(Phi-1) = planElement;    
  }

  return planElement;  
}

iFatras::PlanarDetElement* iFatras::PlanarDetLayerBuilder::DiscDetElement(unsigned int discCounter, unsigned int iring, unsigned int Phi) const
{
  unsigned int VectPos = (discCounter >= m_discPhiSectors.size()) ? (discCounter - m_discPhiSectors.size()): discCounter;
  
  unsigned int ZdiscCounter = (discCounter >= m_discPhiSectors.size()) ? discCounter : (m_discPhiSectors.size()-discCounter-1);

  int brl_ec = 2*m_discZpos[ZdiscCounter]/std::abs(m_discZpos[ZdiscCounter]); // For identifier : endcap element brl_ec = +/-2 (neg ec/pos ec)
  int disc = m_discOffset+VectPos;

  int phiModule = (Phi-1) + m_discPhiSectors[VectPos][iring]/2; 
  if ( 2*Phi > m_discPhiSectors[VectPos][iring])
    phiModule = (Phi-1) - m_discPhiSectors[VectPos][iring]/2;

  int etaModule = m_discPhiSectors[VectPos].size()-iring-1; // it is numerated from the external one --> it is the contrary of the ring iterator iring
  
  Identifier idwafer(0);
  IdentifierHash idhash(0);    
  
  if(m_pixelCase) {
    idwafer = m_pixIdHelper->wafer_id(brl_ec, disc, phiModule, etaModule);
    idhash = m_pixIdHelper->wafer_hash(m_pixIdHelper->wafer_id(idwafer));
  } else {
    idwafer = m_sctIdHelper->wafer_id(brl_ec, disc, phiModule, etaModule, 0);
    idhash = m_sctIdHelper->wafer_hash(m_sctIdHelper->wafer_id(idwafer));
  }

  // create the transform parameters
  double phistep = TMath::Pi()/180. * (m_discMaxPhi[VectPos][iring]-m_discMinPhi[VectPos][iring])/double(m_discPhiSectors[VectPos][iring]);
  double phi = TMath::Pi()/180. * m_discMinPhi[VectPos][iring] + (Phi-1) * phistep + phistep/2.;
  double phi_angle = (Phi-1) * phistep + phistep/2.;

  double discLengthY    = (m_discRingMaxR[VectPos][iring] - m_discRingMinR[VectPos][iring])* cos(phistep/2.);
  double dx = (m_discRingMaxR[VectPos][iring]*cos(phistep/2.)-discLengthY/2.) * cos(phi);
  double dy = (m_discRingMaxR[VectPos][iring]*cos(phistep/2.)-discLengthY/2.) * sin(phi);

  int sign = 1;
  if (Phi%2) sign*=-1;

  double disc_separation = 0.;
  if (!m_discSeparation.empty())
    disc_separation = m_discSeparation[VectPos][iring];

  double dz = m_discZpos[ZdiscCounter]+sign*disc_separation/2.;

  if (!m_ringDisplacement.empty()) {
    dz += (etaModule%2) ? (m_discZpos[ZdiscCounter]/fabs(m_discZpos[ZdiscCounter])*m_ringDisplacement[VectPos][iring]) : (-m_discZpos[ZdiscCounter]/fabs(m_discZpos[ZdiscCounter])*m_ringDisplacement[VectPos][iring]);
  }

  double lengthXmax = 0.;
  if (!m_discLengthXmax.empty())
    lengthXmax = m_discLengthXmax[VectPos][iring];

  double stereo = 0.0;
  if (!m_discStereo.empty())
    stereo = m_discZpos[ZdiscCounter]/std::abs(m_discZpos[ZdiscCounter])*TMath::Pi()/180.*m_discStereo[VectPos][iring]; // Add the stereo angle (the rotation depends from pos/neg z)
  
  
  Amg::Vector3D centerOnModule(dx,dy,dz);

  //  IMPORTANT FOR DISCTRAPEZOIDALBOUNDS
  // If you have trapezoidal shape elements and you want to have DiscTrapezoidalBounds
  // In this case you have a DiscSurface and you need to redefine the translation vector and the rotation matrix
  // --> For the centerOnModule use the medium point of the height
  // --> For the translation use the center of the disc surface 
  // if you have no stereo angle it is (0., 0., z) for the disc
  // if you have a stereo angle != 0. then you need to shift it accordingly to the tilt angle
  // --> The rotation matrix will be the identity (you rotate the disc only in case you have the back surface)

  if (!m_useTrapezoidalBounds && !m_discLengthXmax.empty()) {
    double hmax = sqrt(m_discRingMaxR[VectPos][iring]*m_discRingMaxR[VectPos][iring]-lengthXmax/2.*lengthXmax/2.);
    double hmin = sqrt(m_discRingMinR[VectPos][iring]*m_discRingMinR[VectPos][iring]-m_discLengthXmin[VectPos][iring]/2.*m_discLengthXmin[VectPos][iring]/2.);
    double r = (hmax+hmin)/2.;
    centerOnModule = Amg::Vector3D(r*cos(phi),r*sin(phi),dz);
    if (m_discStereo.empty() || stereo == 0.) {// the easy case first so that you don't need to calculate anything since the result will be (0., 0., z.)
      dx = 0.; dy = 0. ;    
    } else { 
      double rMedium = centerOnModule.perp();
      double rphi = centerOnModule.phi();
      dx = -dz/fabs(dz)*2.*rMedium*fabs(sin(stereo/2.))*sin(stereo/2.+ M_PI + rphi);
      dy = dz/fabs(dz)*2.*rMedium*fabs(sin(stereo/2.))*cos(stereo/2.+ M_PI + rphi);
    } 
  }
  
  Amg::Vector3D transl(dx,dy,dz);
  
  // create the rotation
  Amg::RotationMatrix3D rotation;     
  if (m_useTrapezoidalBounds || m_discLengthXmax.empty()) 
    rotation = Amg::AngleAxis3D(0., Amg::Vector3D::UnitX())*
      Amg::AngleAxis3D(0., Amg::Vector3D::UnitY())*
      Amg::AngleAxis3D(TMath::Pi()/2.+phi_angle, Amg::Vector3D::UnitZ()); // you need TMath::Pi()/2 to make the correct rotation
  else 
    rotation = Amg::AngleAxis3D(0., Amg::Vector3D::UnitX())*
      Amg::AngleAxis3D(0., Amg::Vector3D::UnitY())*
      Amg::AngleAxis3D(0., Amg::Vector3D::UnitZ()); // you need TMath::Pi()/2 to make the correct rotation
  
  Amg::Transform3D transform(rotation, transl);
  
  // add the stereo angle
  if (m_discSCTlike && (m_useTrapezoidalBounds || m_discLengthXmax.empty()))
    transform*=Amg::AngleAxis3D(-m_discZpos[ZdiscCounter]/std::abs(m_discZpos[ZdiscCounter])*stereo, Amg::Vector3D::UnitZ());
  
  bool isOuterMost = false;
  
  if(!(m_discZpos.size()%(discCounter+1)))
    isOuterMost = true;
  
  if (m_discSCTlike)
    if (m_discStereoSeparation[VectPos][iring] > 0.)
      isOuterMost = false;
  
  // get the layer material 
  const Trk::LayerMaterialProperties* layerMaterial = m_layerMaterial;
  
  iFatras::PlanarDetElement* planElement = new iFatras::PlanarDetElement(idwafer, idhash, centerOnModule, transform, *layerMaterial, m_discThickness[VectPos], m_discLengthY[VectPos][iring], m_discLengthXmin[VectPos][iring], lengthXmax, m_discRingMinR[VectPos][iring], m_discRingMaxR[VectPos][iring], m_discPitchX[VectPos][iring], m_discPitchY[VectPos][iring], stereo, m_pixelCase, false, isOuterMost, m_useTrapezoidalBounds, false);
  
  ATH_MSG_VERBOSE("Inside DiscDetElement() --> Created PlanarDetElement with " << Amg::toString(transform));
  
  if (!planElement) ATH_MSG_WARNING("Inside DiscDetElement() --> Null poiter for the Planar Detector Element.");

  m_detElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, planElement));

  if (m_discSCTlike) {
    if (m_discStereoSeparation[VectPos][iring] > 0.) {
  
      if(!(m_discZpos.size()%(discCounter+1)))
	isOuterMost = true;
  
      double dx_os = (m_discRingMaxR[VectPos][iring]*cos(phistep/2.)-discLengthY/2.) * cos(phi);
      double dy_os = (m_discRingMaxR[VectPos][iring]*cos(phistep/2.)-discLengthY/2.) * sin(phi);
      double dz_os = dz+m_discZpos[ZdiscCounter]/std::abs(m_discZpos[ZdiscCounter])*m_discStereoSeparation[VectPos][iring];
      
      Amg::Vector3D centerOnModule_os(dx_os,dy_os,dz_os);
      
      //  IMPORTANT FOR DISCTRAPEZOIDALBOUNDS
      // If you have trapezoidal shape elements and you want to have DiscTrapezoidalBounds
      // In this case you have a DiscSurface and you need to redefine the translation vector and the rotation matrix
      // --> For the centerOnModule use the medium point of the height
      // --> For the translation use the center of the disc surface 
      // if you have no stereo angle it is (0., 0., z) for the disc
      // if you have a stereo angle != 0. then you need to shift it accordingly to the tilt angle
      // --> The rotation matrix will be the identity (you rotate the disc only in case you have the back surface)
      
      if (!m_useTrapezoidalBounds && !m_discLengthXmax.empty()) {
	double hmax = sqrt(m_discRingMaxR[VectPos][iring]*m_discRingMaxR[VectPos][iring]-lengthXmax/2.*lengthXmax/2.);
	double hmin = sqrt(m_discRingMinR[VectPos][iring]*m_discRingMinR[VectPos][iring]-m_discLengthXmin[VectPos][iring]/2.*m_discLengthXmin[VectPos][iring]/2.);
	double r = (hmax+hmin)/2.;
	centerOnModule_os = Amg::Vector3D(r*cos(phi),r*sin(phi),dz_os);
	if (m_discStereo.empty() || stereo == 0.) {// the easy case first so that you don't need to calculate anything since the result will be (0., 0., z.)
	  dx_os = 0.; dy_os = 0. ;    
	} else { 
	  double rMedium = centerOnModule_os.perp();
	  double rphi = centerOnModule_os.phi();
	  dx_os = dz_os/fabs(dz_os)*2.*rMedium*fabs(sin(stereo/2.))*sin(-stereo/2.+ M_PI + rphi);
	  dy_os = -dz_os/fabs(dz_os)*2.*rMedium*fabs(sin(stereo/2.))*cos(-stereo/2.+ M_PI + rphi);
	} 
      }
      
      Amg::Vector3D transl_os(dx_os,dy_os,dz_os);
      
      // use the same rotation and create the translation
      Amg::Transform3D transform_os(rotation, transl_os);

      // add the stereo angle
      if (m_useTrapezoidalBounds || m_discLengthXmax.empty())
	transform_os*=Amg::AngleAxis3D(m_discZpos[ZdiscCounter]/std::abs(m_discZpos[ZdiscCounter])*stereo, Amg::Vector3D::UnitZ());
      
      //transform_os*=Amg::AngleAxis3D(TMath::Pi(), Amg::Vector3D::UnitY());
      
      const Trk::LayerMaterialProperties* layerMaterial_os = m_layerMaterial;
	    
      if(!m_pixelCase) {
	idwafer = m_sctIdHelper->wafer_id(brl_ec, disc, phiModule, etaModule, 1);
	idhash = m_sctIdHelper->wafer_hash(m_sctIdHelper->wafer_id(idwafer));
      }
      
      ATH_MSG_VERBOSE("Inside DiscDetElement() --> Creating other PlanarDetElement with " << Amg::toString(transform_os));
      
      iFatras::PlanarDetElement* planElement_os = new iFatras::PlanarDetElement(idwafer, idhash, centerOnModule_os, transform_os, *layerMaterial_os, m_discThickness[VectPos], m_discLengthY[VectPos][iring], m_discLengthXmin[VectPos][iring], lengthXmax, m_discRingMinR[VectPos][iring], m_discRingMaxR[VectPos][iring], m_discPitchX[VectPos][iring], m_discPitchY[VectPos][iring], -stereo, m_pixelCase, false, isOuterMost, m_useTrapezoidalBounds, false);
  
      // each element points to the other face
      planElement->setBackSide(planElement_os); 
      planElement_os->setFrontSide(planElement); 
  
      if (!planElement_os) ATH_MSG_WARNING("Inside DiscDetElement() --> Null poiter for the other side Planar Detector Element.");

      m_detElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, planElement_os));

    }
    else ATH_MSG_WARNING("DiscDetElement() --> Distance between stereo surfaces is negative or zero. Please use a positive distance. Surface not created.");
  }

  //Set next/previous in phi if other elements have been already built

  if (Phi == 1) // Is the first element in phi 
    m_firstDiscElement = planElement;

  if (planElement != m_firstDiscElement)  { // if it is not the first element 
    planElement -> setPrevInPhi(m_previousDiscElement);
    m_previousDiscElement -> setNextInPhi(planElement);
  }

  if(Phi == m_discPhiSectors[VectPos][iring]) { // Phi is closed (the last has as next the first one and the first has as previous the last)
    planElement -> setNextInPhi(m_firstDiscElement);
    m_firstDiscElement -> setPrevInPhi(planElement);
  }

  m_previousDiscElement = planElement;

  return planElement;  
}


/** LayerBuilder interface method - returning Barrel-like layers */
const std::vector< const Trk::CylinderLayer* >* iFatras::PlanarDetLayerBuilder::cylindricalLayers() const
{
  // screen output
  ATH_MSG_INFO( "Configured to build " << m_barrelLayers << " (active) barrel layers");

  // ------------------------ Construction of the layers -----------------------------------          
  // construct the layers
  std::vector< const Trk::CylinderLayer* > cylinderDetectionLayers;// = new std::vector< const Trk::CylinderLayer* >;
  std::vector<double>::const_iterator layerRadiusIter = m_layerRadius.begin();   

  // for the additional layer
  //std::vector<double>::const_iterator addLayerIter    = m_layerAdditionalLayerR.begin();
  //std::vector<double>::const_iterator addLayerIterEnd = m_layerAdditionalLayerR.end();

  int layerCounter          = 0;
  double currentLayerExtend = 0.;
  size_t sumCheckBarrelModules  = 0;
  m_barrelElements.reserve(m_barrelLayers);
 
  for ( ; layerRadiusIter != m_layerRadius.end(); ){ 

    // Create the detector elements
    m_barrelElements[layerCounter] = CreateElements(layerCounter);
    
    ATH_MSG_VERBOSE( "Created " << m_barrelElements[layerCounter]->size() <<" Elements with counter " << layerCounter );
    
    m_layerSurfaces.push_back(std::vector< Trk::SurfaceOrderPosition >());
    
    // Put the surfaces in the vector< vector< Trk::SurfaceOrderPosition > >
    std::vector<const iFatras::PlanarDetElement*>::const_iterator Elem_Iter = m_barrelElements[layerCounter]->begin();
    for ( ; Elem_Iter != m_barrelElements[layerCounter]->end(); Elem_Iter++){
      const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
      Amg::Vector3D orderPosition((*Elem_Iter)->centerOnModule());
      // register the module surface
      Trk::SharedObject<const Trk::Surface> * sharedSurface = new Trk::SharedObject<const Trk::Surface>(moduleSurface);
      Trk::SurfaceOrderPosition surfaceOrder(*sharedSurface, orderPosition);
      m_layerSurfaces[layerCounter].push_back(surfaceOrder);
      
      if(m_pixelCase){
	ATH_MSG_VERBOSE("Surface " << (*moduleSurface));
	ATH_MSG_VERBOSE("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
	if (m_checkGeo)
	  {
	    
	    Amg::Vector3D glopos = (*moduleSurface).center();
	    //Take info to store in the tree
	    m_x_local = ((*moduleSurface).globalToLocal(glopos))->x();
	    m_y_local = ((*moduleSurface).globalToLocal(glopos))->y();
	    
	    m_x_global = glopos.x();
	    m_y_global = glopos.y();
	    m_z_global = glopos.z();
	    
	    m_currentTree -> Fill();
	  }
	
	Identifier hitId         = moduleSurface->associatedDetectorElementIdentifier();   
	IdentifierHash hitIdHash = m_pixIdHelper->wafer_hash(hitId);
	ATH_MSG_VERBOSE("ID " << hitId << ", hash " << hitIdHash );
	ATH_MSG_VERBOSE("barrel_ec " << m_pixIdHelper->barrel_ec(hitId) << ", layer_disk " << m_pixIdHelper->layer_disk(hitId) << ", phi_module " <<m_pixIdHelper->phi_module(hitId) << ", eta_module " << m_pixIdHelper->eta_module(hitId) );
	ATH_MSG_VERBOSE("Phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta());
      } else {
	ATH_MSG_VERBOSE("Surface " << (*moduleSurface));
	ATH_MSG_VERBOSE("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
	if (m_checkGeo)
	  {
	    Amg::Vector3D glopos = (*moduleSurface).center();
	    //Take info to store in the tree
	    m_x_local = ((*moduleSurface).globalToLocal(glopos))->x();
	    m_y_local = ((*moduleSurface).globalToLocal(glopos))->y();
	    
	    m_x_global = glopos.x();
	    m_y_global = glopos.y();
	    m_z_global = glopos.z();
	    
	    m_currentTree -> Fill();
	  }
	
	Identifier hitId         = moduleSurface->associatedDetectorElementIdentifier();   
	IdentifierHash hitIdHash = m_sctIdHelper->wafer_hash(hitId);
	ATH_MSG_VERBOSE("ID " << hitId << ", hash " << hitIdHash );
	ATH_MSG_VERBOSE("barrel_ec " << m_sctIdHelper->barrel_ec(hitId) << ", layer_disk " << m_sctIdHelper->layer_disk(hitId) << ", phi_module " <<m_sctIdHelper->phi_module(hitId) << ", eta_module " << m_sctIdHelper->eta_module(hitId) << ", side " << m_sctIdHelper->side(hitId));
	ATH_MSG_VERBOSE("Phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta());
      }
    }
    
    Trk::CylinderLayer* activeLayer   = 0;
    
    if (layerRadiusIter != m_layerRadius.end()){
      
      // create the BinKeyUtility - the phi binning is identical
      double halfPhiStep = TMath::Pi()/m_layerPhiSectors[layerCounter];
      // now prepare the phi values
      ATH_MSG_VERBOSE("Preparing the Phi-binning for   : " << m_layerPhiSectors[layerCounter] << " sectors.");
      ATH_MSG_VERBOSE("    min phi / max phi detected  : " << m_layerMinPhi[layerCounter] << " / " << m_layerMaxPhi[layerCounter] );
      
      double PhiOffset = (!m_layerPhiOffset.empty()) ? TMath::Pi()/180.*m_layerPhiOffset[layerCounter] : 0.0;
      
      double minPhiCorrected = TMath::Pi()/180.*m_layerMinPhi[layerCounter]-halfPhiStep + PhiOffset;
      double maxPhiCorrected = TMath::Pi()/180.*m_layerMaxPhi[layerCounter]-halfPhiStep + PhiOffset;//+halfPhiStep;
      
      ATH_MSG_VERBOSE("    min phi / max phi corrected : " << minPhiCorrected << " / " << maxPhiCorrected );
      
      Trk::BinUtility* currentBinUtility   =  new Trk::BinUtility(m_layerPhiSectors[layerCounter],
								  minPhiCorrected,
								  maxPhiCorrected,
								  Trk::closed, Trk::binPhi);
      
      if (m_layerEquidistantBinning)
	(*currentBinUtility) += Trk::BinUtility(m_layerZsectors[layerCounter],
						m_layerMinZ[layerCounter],
						m_layerMaxZ[layerCounter],
						  Trk::open,
						Trk::binZ);
      else
	(*currentBinUtility) += Trk::BinUtility(m_layerZboundaries[layerCounter],
						Trk::open,
						Trk::binZ);
      // creating the binned array output
      ATH_MSG_VERBOSE("Creating the binned array for the sensitive detector elements with BinUtility :");
      ATH_MSG_VERBOSE( *currentBinUtility );
      // the binned array for the senstive surfaces to be built            
      Trk::BinnedArray<Trk::Surface>* currentBinnedArray = 
	new Trk::BinnedArray2D<Trk::Surface>(m_layerSurfaces[layerCounter],currentBinUtility);       
      const std::vector<const Trk::Surface*>& arraySurfaces = currentBinnedArray->arrayObjects(); 
      
      // checking for :
      //   - empty surface bins
      //   - doubly filled bins
      std::map< const Trk::Surface*,Amg::Vector3D > uniqueSurfaceMap;
      std::map< const Trk::Surface*,Amg::Vector3D >::iterator usmIter = uniqueSurfaceMap.end();
      // ------- iterate  
      std::vector<const Trk::Surface*>::const_iterator asurfIter = arraySurfaces.begin();
      std::vector<const Trk::Surface*>::const_iterator asurfIterEnd = arraySurfaces.end();
      for ( ; asurfIter != asurfIterEnd; ++asurfIter){
	if ( (*asurfIter) ) {
	  ++sumCheckBarrelModules;
	  usmIter = uniqueSurfaceMap.find(*asurfIter);
	  if ( usmIter != uniqueSurfaceMap.end() )
	    ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() << " / " << (*asurfIter)->center().phi());
	    else uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
	}
	else{
	  ATH_MSG_WARNING("Null surface defined in BinUtility ArraySurfaces vector");
	}
      }
      
      currentLayerExtend = m_layerMinZ[layerCounter]*m_layerMinZ[layerCounter] > m_layerMaxZ[layerCounter]*m_layerMaxZ[layerCounter] ?
	fabs(m_layerMinZ[layerCounter]) : m_layerMaxZ[layerCounter];
      
      double activeLayerRadius = (m_layerRmin[layerCounter]+m_layerRmax[layerCounter])/2.;
      
      // create the material
      const Trk::LayerMaterialProperties* layerMaterial = barrelLayerMaterial(activeLayerRadius, currentLayerExtend);
      
      // screen output
      ATH_MSG_VERBOSE( "Construct BinnedArray for CylinderLayer with "<< (m_layerSurfaces[layerCounter]).size() << " SubSurfaces." );
      ATH_MSG_VERBOSE( "Building a CylinderLayer with " << m_layerPhiSectors[layerCounter]
		       << " / " <<   m_layerZsectors[layerCounter] << " phi/z bins. "   );
      if (!m_layerTiltedRadius.empty())
	ATH_MSG_VERBOSE( "  -> With Barrel Radius     :  " << m_layerRadius[layerCounter] << "  with Tilted Radius  :   " << m_layerTiltedRadius[layerCounter]  );
      else
	ATH_MSG_VERBOSE( "  -> With Radius     :  " << m_layerRadius[layerCounter]    );       
      ATH_MSG_VERBOSE( "  -> With Thickness  :  " << m_layerThickness[layerCounter] );
      ATH_MSG_VERBOSE( "  -> With Zmin/Zmax  :  " << -currentLayerExtend << " / " << currentLayerExtend );
      
      // prepare the right overlap descriptor       
      Trk::OverlapDescriptor* olDescriptor = new iFatras::PlanarOverlapDescriptor(m_moreSurfacesToOverlapDescriptor);
      
      // layer thickness
      double activeThickness = (m_layerRmax[layerCounter]+m_barrelEnvelope)-(m_layerRmin[layerCounter]-m_barrelEnvelope);
      ATH_MSG_VERBOSE( "  -> With Active Thickness       :  " << activeThickness  );
      
      // construct the layer (finally)
      activeLayer = new Trk::CylinderLayer(new Trk::CylinderBounds(activeLayerRadius,currentLayerExtend),
					   currentBinnedArray,
					   *layerMaterial,
					   activeThickness,
					   olDescriptor);
      
      // increase the iterator
      ++layerRadiusIter;                                                        
      // cleanup of the layer material --------------------------------------------------------------
      delete layerMaterial; 
      // register the layer to the surfaces
      const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
      registerSurfacesToLayer(layerSurfaces,*activeLayer);
      
      layerToSurfaceCollection(activeLayer);
      
    }
    
   
    ATH_MSG_INFO( "Adding active layer");
    // register the layers 
    if (activeLayer)  cylinderDetectionLayers.push_back(activeLayer);
   
    // increase the layer counter --- it is built
    ++layerCounter;    

  }// layer construction
  // --------------------------- enf of layer construction loop ----------------------------------

  // register the layers and add passive layers
  std::vector< const Trk::CylinderLayer* >* cylinderLayers = dressCylinderLayers(cylinderDetectionLayers);

  return cylinderLayers;   

} 

std::vector< const Trk::CylinderLayer* >* iFatras::PlanarDetLayerBuilder::dressCylinderLayers(const std::vector< const Trk::CylinderLayer* >& detectionLayers ) const {
  
  std::vector< const Trk::CylinderLayer* >* cylinderLayers = new std::vector< const Trk::CylinderLayer* >;

  // construct the passive layers: --- given by jobOptions -------------------------------------
  if (m_layerAdditionalLayerR.size()){
    //bool buildPassiveLayer = m_layerAdditionalLayerR.size() && (addLayerIter != addLayerIterEnd);
    auto cylLayerIter         = detectionLayers.begin();
    auto cylLayerIterEnd      = detectionLayers.end();
    auto addLayerIter         = m_layerAdditionalLayerR.begin();
    auto addLayerIterEnd      = m_layerAdditionalLayerR.end();
    double cylLayerExtend     = 0;
    
    for ( ; addLayerIter != addLayerIterEnd;) {
      if  ( cylLayerIter == cylLayerIterEnd || (*addLayerIter) < (*cylLayerIter)->bounds().r() ){
	cylLayerExtend = (cylLayerIter == cylLayerIterEnd) ? cylLayerExtend : (*cylLayerIter)->bounds().halflengthZ() ;  
	ATH_MSG_VERBOSE( "Building an additional CylinderLayer w/o sensitive modules at");
	ATH_MSG_VERBOSE( "  -> With Radius     :  " << *addLayerIter   );       
	ATH_MSG_VERBOSE( "  -> With Zmin/Zmax  :  " << -cylLayerExtend << " / " << cylLayerExtend );
	// the material for the passive layer
	const Trk::LayerMaterialProperties* passiveLayerMaterial = barrelLayerMaterial(*addLayerIter,cylLayerExtend, false);
	// create the passive layer
	cylinderLayers->push_back(new Trk::CylinderLayer(new Trk::CylinderBounds(*addLayerIter,cylLayerExtend),
							 *passiveLayerMaterial,
							 1.*Gaudi::Units::mm,
							 0,0));
	// cleanup of the layer material --------------------------------------------------------------
	delete passiveLayerMaterial;       

	// increase the additional layer radii
	++addLayerIter;
	continue;
      }
      ATH_MSG_VERBOSE("Registering detection CylinderLayer");
      ATH_MSG_VERBOSE( "  -> With Radius     :  " << (*cylLayerIter)->bounds().r()   );
      cylinderLayers->push_back(*cylLayerIter);
      ++cylLayerIter;       
    } 
  } else 
    for (auto& cylLayerIter : detectionLayers ) {
      ATH_MSG_VERBOSE("Registering detection CylinderLayer");
      ATH_MSG_VERBOSE( "  -> With Radius     :  " << cylLayerIter->bounds().r()   );
      cylinderLayers->push_back(cylLayerIter);
    }
  return cylinderLayers;
}

/** LayerBuilder interface method - returning Endcap-like layers */
const std::vector< const Trk::DiscLayer* >* iFatras::PlanarDetLayerBuilder::discLayers() const
{
  // screen output     
  ATH_MSG_INFO( "Configured to build " << m_endcapDiscs << " *2 disc-like layers" );

  // ------------------------ Construction of the layers -----------------------------------
  // construct the layers
  std::vector< const Trk::DiscLayer* >* discLayers = new std::vector< const Trk::DiscLayer* >;
  std::vector<double>::const_iterator discZposIter = m_discZpos.begin();
  int discCounter = 0;
  int sumCheckEndcapModules = 0;
    
  m_endcapElements.reserve(2*m_endcapDiscs);

  for ( ; discZposIter != m_discZpos.end(); ++discZposIter){

    unsigned int ZdiscCounter = ((unsigned int)discCounter >= m_discPhiSectors.size()) ? discCounter : (m_discPhiSectors.size()-discCounter-1);

    // Create the detector elements
    m_endcapElements[discCounter] = CreateElements(discCounter, false);

    ATH_MSG_VERBOSE( "Created " << m_endcapElements[discCounter]->size() <<" Elements with counter " << discCounter );

    m_discSurfaces.push_back(std::vector< Trk::SurfaceOrderPosition >());

    // Put the surfaces in the vector< vector< Trk::SurfaceOrderPosition > >
    std::vector<const iFatras::PlanarDetElement*>::const_iterator Elem_Iter = m_endcapElements[discCounter]->begin();
    for ( ; Elem_Iter != m_endcapElements[discCounter]->end(); Elem_Iter++) {
      const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
      Amg::Vector3D orderPosition((*Elem_Iter)->centerOnModule());
      // register the module surface
      Trk::SharedObject<const Trk::Surface> * sharedSurface = new Trk::SharedObject<const Trk::Surface>(moduleSurface);
      Trk::SurfaceOrderPosition surfaceOrder(*sharedSurface, orderPosition);
      m_discSurfaces[discCounter].push_back(surfaceOrder);
  
      if(m_pixelCase){
	ATH_MSG_VERBOSE("Surface " << (*moduleSurface));
	ATH_MSG_VERBOSE("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
	Identifier hitId         = moduleSurface->associatedDetectorElementIdentifier();   
	IdentifierHash hitIdHash = m_pixIdHelper->wafer_hash(hitId);
	ATH_MSG_VERBOSE("ID " << hitId << ", hash " << hitIdHash );
	ATH_MSG_VERBOSE("barrel_ec " << m_pixIdHelper->barrel_ec(hitId) << ", layer_disk " << m_pixIdHelper->layer_disk(hitId) << ", phi_module " <<m_pixIdHelper->phi_module(hitId) << ", eta_module " << m_pixIdHelper->eta_module(hitId) );
	ATH_MSG_VERBOSE("Phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta());
      }
      else {
	ATH_MSG_VERBOSE("Surface " << (*moduleSurface));
	ATH_MSG_VERBOSE("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
	Identifier hitId         = moduleSurface->associatedDetectorElementIdentifier();   
	IdentifierHash hitIdHash = m_sctIdHelper->wafer_hash(hitId);
	ATH_MSG_VERBOSE("ID " << hitId << ", hash " << hitIdHash );
	ATH_MSG_VERBOSE("barrel_ec " << m_sctIdHelper->barrel_ec(hitId) << ", layer_disk " << m_sctIdHelper->layer_disk(hitId) << ", phi_module " <<m_sctIdHelper->phi_module(hitId) << ", eta_module " << m_sctIdHelper->eta_module(hitId) << ", side " << m_sctIdHelper->side(hitId) );
	ATH_MSG_VERBOSE("Phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta());
      }
	    
    }

    //unsigned int VectPos = discCounter - discCounter/m_discPhiSectors.size()*(2*(discCounter-m_discPhiSectors.size()) + 1);
    unsigned int VectPos = ((unsigned int)discCounter >= m_discPhiSectors.size()) ? (discCounter - m_discPhiSectors.size()): discCounter;

    // the gathered R sectors  
    size_t discRsectors = (m_discPhiSectors[VectPos]).size();
    // dynamic estimation 1: estimate the layer thickness dynamically
    double thickness = m_discThickness[VectPos];       

    // preperare the rboundaries
    std::vector<float> discRboundaries;
    discRboundaries.push_back(float(*m_discRingMinR[VectPos].begin()));
    for ( std::vector<double>::const_iterator ringMaxRIter = m_discRingMaxR[VectPos].begin(); 
	  ringMaxRIter != m_discRingMaxR[VectPos].end(); ++ringMaxRIter)   
      discRboundaries.push_back(float(*ringMaxRIter));

    // screen output           
    ATH_MSG_VERBOSE( "Building a DiscLayer with " << discRsectors << " R sectors. " );
    ATH_MSG_VERBOSE( "  -> At Z - Position      :  " << m_discZpos[ZdiscCounter] );
    ATH_MSG_VERBOSE( "  -> With Thickness       :  " << thickness             );

    ATH_MSG_VERBOSE( "  -> With Rmin/Rmax (est) :  " << m_discRmin[VectPos] << " / " << m_discRmax[VectPos] );
    ATH_MSG_VERBOSE( "  -> With Rings           :  " << discRsectors );

    for (size_t irings=0; irings<discRsectors; ++irings)
      ATH_MSG_VERBOSE( " --> " << irings <<  " R sector has " << (int)m_discPhiSectors[VectPos][irings] << " phi sectors. " );

    // prepare the binned array, it can be with one to several rings            
    Trk::BinnedArray<Trk::Surface>* currentBinnedArray = 0;
    std::vector<Trk::BinUtility*>* singleBinUtils = new std::vector<Trk::BinUtility*>;

    double discSeparation = 0.;
    double ringDisplacement = 0.;
    double discStereoSeparation = 0.;

    if (discRsectors==1){
      double halfPhiStep = TMath::Pi()/m_discPhiSectors[VectPos][0];
      double minPhiCorrected = TMath::Pi()/180.*m_discMinPhi[VectPos][0]-halfPhiStep;
      double maxPhiCorrected = TMath::Pi()/180.*m_discMaxPhi[VectPos][0]+halfPhiStep;

      ATH_MSG_VERBOSE("Constructing a one-dimensional BinnedArray with phiMin / phiMax (bins) = " 
		    << minPhiCorrected << " / " << maxPhiCorrected 
		    << " (" << (int)m_discPhiSectors[VectPos][0] << ")");
      // an easier bin utility can be used                 
      Trk::BinUtility* currentBinUtility = new Trk::BinUtility((int)m_discPhiSectors[VectPos][0] ,
							       minPhiCorrected,
							       maxPhiCorrected,
							       Trk::closed,
							       Trk::binPhi);
      // a one-dimensional BinnedArray is sufficient
      currentBinnedArray = new Trk::BinnedArray1D<Trk::Surface>(m_discSurfaces[discCounter],currentBinUtility);
      // creating the binned array output
      ATH_MSG_VERBOSE("Creating the binned array for the sensitive detector elements with BinUtility :");
      ATH_MSG_VERBOSE( *currentBinUtility );

      if (!m_discSeparation.empty()) 
	discSeparation = m_discSeparation[VectPos][0];
      if (m_discSCTlike) 
	discStereoSeparation = fabs(m_discStereoSeparation[VectPos][0]);
     
    } else {
      ATH_MSG_VERBOSE("Constructing a two-dimensional BinnedArray.");
      // get the binning in R first (can still be improved with non-aequidistant binning) 
      Trk::BinUtility* currentSteerBinUtility = 0;
      currentSteerBinUtility = new Trk::BinUtility(discRboundaries,
						   Trk::open,
						   Trk::binR);
      ATH_MSG_VERBOSE("Steering bin utility constructed as : " << *currentSteerBinUtility);
      // the single phi bin utilities
      singleBinUtils->reserve(discRsectors);
      for (size_t irings=0; irings<discRsectors; ++irings){
	double halfPhiStep = TMath::Pi()/m_discPhiSectors[VectPos][irings];
	ATH_MSG_VERBOSE("    min phi / max phi detected  : " << TMath::Pi()/180.*m_discMinPhi[VectPos][irings] << " / " << TMath::Pi()/180.*m_discMaxPhi[VectPos][irings] );
	
	double minPhiCorrected = TMath::Pi()/180.*m_discMinPhi[VectPos][irings]-halfPhiStep;
	double maxPhiCorrected = TMath::Pi()/180.*m_discMaxPhi[VectPos][irings]+halfPhiStep;
	
	ATH_MSG_VERBOSE("    min phi / max phi corrected : " << minPhiCorrected << " / " << maxPhiCorrected );
	ATH_MSG_VERBOSE("Constructing for ring " << irings << " phi utility with phiMin / phiMax (bins) = " 
		      << minPhiCorrected << " / " << maxPhiCorrected
		      << " (" << (int)m_discPhiSectors[VectPos][irings] << ")") ;
	singleBinUtils->push_back(new Trk::BinUtility((int)m_discPhiSectors[VectPos][irings],
						      minPhiCorrected,
						      maxPhiCorrected,
						      Trk::closed,
						      Trk::binPhi));
	if (!m_discSeparation.empty() && fabs(m_discSeparation[VectPos][irings])>discSeparation) 
	  discSeparation = fabs(m_discSeparation[VectPos][irings]);
	if (!m_ringDisplacement.empty() && fabs(m_ringDisplacement[VectPos][irings])> ringDisplacement) 
	  ringDisplacement = 2*fabs(m_ringDisplacement[VectPos][irings]);
	if (m_discSCTlike && fabs(m_discStereoSeparation[VectPos][irings]) > discStereoSeparation) 
	  discStereoSeparation = fabs(m_discStereoSeparation[VectPos][irings]);
	
      }
      // a two-dimensional BinnedArray is needed                                                        
      currentBinnedArray = new Trk::BinnedArray1D1D<Trk::Surface>(m_discSurfaces[discCounter],
								  currentSteerBinUtility,
								  singleBinUtils);
								
    }
    
    int discSurfacesNum = (m_discSurfaces[discCounter]).size();
    ATH_MSG_VERBOSE("Constructed BinnedArray for DiscLayer with "<< discSurfacesNum << " SubSurfaces." );
    
    // always run the geometry validation to catch flaws
    // checking for :
    //   - empty surface bins
    //   - doubly filled bins
    std::map< const Trk::Surface*,Amg::Vector3D > uniqueSurfaceMap;
    std::map< const Trk::Surface*,Amg::Vector3D >::iterator usmIter = uniqueSurfaceMap.end();
    // check the registered surfaces in the binned array
    const std::vector<const Trk::Surface*>& arraySurfaces = currentBinnedArray->arrayObjects();

    std::vector<const Trk::Surface*>::const_iterator asurfIter = arraySurfaces.begin();
    std::vector<const Trk::Surface*>::const_iterator asurfIterEnd = arraySurfaces.end();
    for ( ; asurfIter != asurfIterEnd; ++asurfIter ){
      if ( (*asurfIter) ) {
	++sumCheckEndcapModules;
	usmIter = uniqueSurfaceMap.find(*asurfIter);
	if ( usmIter != uniqueSurfaceMap.end() )
	  ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() << " / " << (*asurfIter)->center().phi());
	else uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
      } 
      else{
	ATH_MSG_WARNING("Null surface defined in BinUtility ArraySurfaces vector");
      }
    }

    double rMin = m_discRmin[VectPos];
    double rMax = m_discRmax[VectPos];

    // get the layer material from the helper method
    const Trk::LayerMaterialProperties* layerMaterial = endcapLayerMaterial(rMin,rMax);
   
    // position & bounds of the active Layer
    Amg::Transform3D*  activeLayerTransform = new Amg::Transform3D;
    (*activeLayerTransform) = Amg::Translation3D(0.,0.,m_discZpos[ZdiscCounter]);
  
    Trk::DiscBounds* activeLayerBounds    = new Trk::DiscBounds(rMin,rMax);

    // prepare the right overlap descriptor  
    Trk::OverlapDescriptor* olDescriptor = new iFatras::DiscOverlapDescriptor(currentBinnedArray, singleBinUtils, false);

    // layer thickness
    double activeThickness = thickness + discSeparation + ringDisplacement +m_endcapEnvelope;
    
    if (m_discSCTlike)
      activeThickness += thickness/2. + discStereoSeparation;
    
    ATH_MSG_VERBOSE( "  -> With Active Thickness       :  " << activeThickness  );

    // layer creation
    Trk::DiscLayer* activeLayer = new Trk::DiscLayer(activeLayerTransform,
						     activeLayerBounds,
						     currentBinnedArray,
						     *layerMaterial,
						     activeThickness,
						     olDescriptor);

    // cleanup
    delete layerMaterial;
    // register the layer to the surfaces --- if necessary to the other sie as well
    const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
    registerSurfacesToLayer(layerSurfaces,*activeLayer);
    
    layerToSurfaceCollection(activeLayer);
    
     ATH_MSG_INFO( "Adding active disc");
    // register the active layers
    if (activeLayer)  discLayers->push_back(activeLayer);

    // increase the disc counter by one
    ++discCounter;     

  }    // active disc construction

  // if there are additional layers to be built
  if (m_discAdditionalLayerPosZ.size()){

    std::vector<const Trk::DiscLayer*>::iterator sortIter  = discLayers->begin();
    std::vector<const Trk::DiscLayer*>::iterator sortEnd   = discLayers->end(); 

    std::vector< double > RMin;
    std::vector< double > RMax;

    for ( ; sortIter != sortEnd; ) {
      // get the bounds for the rMin / rMax setting
      const Trk::DiscBounds* currentBounds = dynamic_cast<const Trk::DiscBounds*>(&((*sortIter)->surfaceRepresentation().bounds()));

      double rmin = currentBounds ? currentBounds->rMin() : 0.;
      RMin.push_back(rmin);
      double rmax = currentBounds ? currentBounds->rMax() : 0.;
      RMax.push_back(rmax);

      ++sortIter;    
    }

    std::vector<double>::const_iterator addLayerIter    = m_discAdditionalLayerPosZ.begin();
    std::vector<double>::const_iterator addLayerIterEnd = m_discAdditionalLayerPosZ.end();

    std::vector<double>::const_iterator rMin_iter = RMin.begin();
    std::vector<double>::const_iterator rMax_iter = RMax.begin();
  
    // build the additional layers -------------------------------------------
    for ( ; (rMin_iter != RMin.end() && rMax_iter != RMax.end()) || addLayerIter != addLayerIterEnd; ){

      double rMin = 0.;
      double rMax = 0.;
  
      if (rMin_iter != RMin.end() && rMax_iter != RMax.end()) {
	rMin = *rMin_iter;
	rMax = *rMax_iter;
	rMin_iter++;
	rMax_iter++;
      }
  
  
      if ( addLayerIter != addLayerIterEnd){
	// create the material and the passive layer
	const Trk::LayerMaterialProperties* passiveLayerMaterial = endcapLayerMaterial(rMin,rMax, false);
	// passive layer creation
	Amg::Transform3D* passiveDiscTransf = new Amg::Transform3D;
	(*passiveDiscTransf) = Amg::Translation3D(0.,0.,*addLayerIter);
	Trk::DiscLayer* passiveLayer = new Trk::DiscLayer(passiveDiscTransf,
							  new Trk::DiscBounds(rMin,rMax),
							  *passiveLayerMaterial,
							  1.*Gaudi::Units::mm);
						      
	// register the layers
	if (passiveLayer) discLayers->push_back(passiveLayer);

	// increase the iterator
	++addLayerIter; 


      }

    } // the additional passive discs are built ------------------------------------

  }  // layer construction

  // --------------------------- enf of layer construction loop ----------------------------------
  return discLayers;
}


const Trk::LayerMaterialProperties* iFatras::PlanarDetLayerBuilder::barrelLayerMaterial(double r, double hz, bool isActive) const{

  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------
  // -- material with 1D binning
  Trk::BinUtility layerBinUtilityZ(m_barrelLayerBinsZ, -hz, hz, Trk::open, Trk::binZ);
  if (m_barrelLayerBinsPhi==1 || isActive){
    layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityZ);
  } else if (!m_customMaterial) { // -- material with 2D binning
    Trk::BinUtility layerBinUtilityRPhiZ(m_barrelLayerBinsPhi,
					 -r*TMath::Pi(), r*TMath::Pi(),
					 Trk::closed,
					 Trk::binRPhi);
    layerBinUtilityRPhiZ += layerBinUtilityZ;                                                       
    layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityRPhiZ);
  } else { // -- homogeneous material 
    Trk::MaterialProperties customMaterial(m_customMaterialThickness,
					   m_customMaterialX0,
					   m_customMaterialL0,
					   m_customMaterialA,
					   m_customMaterialZ,
					   m_customMaterialRho);
    layerMaterial = new Trk::HomogeneousLayerMaterial(customMaterial, 1.0);         
  }
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;    

}

const Trk::LayerMaterialProperties* iFatras::PlanarDetLayerBuilder::endcapLayerMaterial(double rMin, double rMax, bool isActive) const
{
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------

  Trk::BinUtility layerBinUtilityR(m_endcapLayerBinsR,rMin,rMax,Trk::open, Trk::binR);
  // -- material with 1D binning
  if (m_barrelLayerBinsPhi==1 || isActive){
    layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  } else if (!m_customMaterial) { // -- material with 2D binning
    Trk::BinUtility layerBinUtilityPhi(m_barrelLayerBinsPhi,-TMath::Pi(),TMath::Pi(),Trk::closed,Trk::binPhi);
    layerBinUtilityR += layerBinUtilityPhi;
    layerMaterial     = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  } else { // -- homogeneous material 
    Trk::MaterialProperties customMaterial(m_customMaterialThickness,
					   m_customMaterialX0,
					   m_customMaterialL0,
					   m_customMaterialA,
					   m_customMaterialZ,
					   m_customMaterialRho);
    layerMaterial = new Trk::HomogeneousLayerMaterial(customMaterial, 1.0);         
  }    
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;    
}     

// 


void iFatras::PlanarDetLayerBuilder::registerSurfacesToLayer(const std::vector<const Trk::Surface*>& layerSurfaces, const Trk::Layer& lay) const
{
  if (!m_setLayerAssociation) return;

  std::vector<const Trk::Surface*>::const_iterator laySurfIter    = layerSurfaces.begin();
  std::vector<const Trk::Surface*>::const_iterator laySurfIterEnd = layerSurfaces.end();
  // register the surfaces to the layer
  for (; laySurfIter != laySurfIterEnd; ++laySurfIter){
    if (*laySurfIter) { 
      // register the current surface --------------------------------------------------------
      Trk::ILayerBuilder::associateLayer(lay, **laySurfIter);
      const iFatras::PlanarDetElement* detElement 
	= dynamic_cast<const iFatras::PlanarDetElement*>((*laySurfIter)->associatedDetectorElement());
    
      // register the backside if necessary ---------------------------------------------------
      if (detElement && detElement->otherSide())
	Trk::ILayerBuilder::associateLayer(lay, detElement->otherSide()->surface());
    }
  }   
  return;
}

void iFatras::PlanarDetLayerBuilder::layerToSurfaceCollection(Trk::Layer *layer) const
{
  const Trk::SurfaceArray* array = layer->surfaceArray();
  const std::vector<const Trk::Surface*> lsurf = array->arrayObjects();
  for (unsigned int is=0;is<lsurf.size();is++) {
    m_surfcoll->push_back(lsurf.at(is));
    const iFatras::PlanarDetElement* pElement  = dynamic_cast<const iFatras::PlanarDetElement*>((lsurf.at(is))->associatedDetectorElement());
    if (pElement && pElement->otherSide())
      m_surfcoll->push_back(&(pElement->otherSide()->surface()));
  }
}

void iFatras::PlanarDetLayerBuilder::computeRadiusMinMax(Amg::Transform3D trf, iFatras::PlanarDetElement* module, double &rMin, double &rMax) const
{
  std::vector<Amg::Vector3D >corners;
  
  // coordinates before any transform.
  double xmod = (module->lengthXmax()>0.) ? module->lengthXmax()*.5 : module->lengthXmin()*.5;
  double ymod = module->lengthY()*.5;
  double zmod = module->thickness()*.5;
  
  // top corners
  corners.push_back( Amg::Vector3D (-xmod, ymod, -zmod));
  corners.push_back( Amg::Vector3D ( xmod, ymod, -zmod));
  corners.push_back( Amg::Vector3D (-xmod, ymod,  zmod));
  corners.push_back( Amg::Vector3D ( xmod, ymod,  zmod));
  
  // bottom corners
  corners.push_back( Amg::Vector3D (-xmod, -ymod, -zmod));
  corners.push_back( Amg::Vector3D ( xmod, -ymod, -zmod));
  corners.push_back( Amg::Vector3D (-xmod, -ymod,  zmod));
  corners.push_back( Amg::Vector3D ( xmod, -ymod,  zmod));
    
  double vMin=9999.;
  double vMax=-9999.;
  
  for(unsigned int i=0; i<corners.size(); i++)
    {
      Amg::Vector3D p = trf*corners[i];
      double r = sqrt(p.x()*p.x()+p.y()*p.y());
      vMin = std::min(vMin,r);
      vMax = std::max(vMax,r);
    }
  
  rMin = vMin;
  rMax = vMax;
  
}
