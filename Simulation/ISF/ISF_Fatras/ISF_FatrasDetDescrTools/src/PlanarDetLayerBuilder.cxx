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
  AthAlgTool(t,n,p),
  m_pixelCase(true),
  m_siMgr(0),
  m_siMgrLocation("Pixel"),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  m_detElementMapName("Pixel_IdHashDetElementMap"),
  m_setLayerAssociation(true),
  m_identification("Pixel"),
  m_barrelLayerBinsZ(100),
  m_barrelLayerBinsPhi(1),
  m_barrelLayers(1),
  m_layerSCTlike(false),
  m_endcapLayerBinsR(100),
  m_endcapLayerBinsPhi(1),
  m_endcapDiscs(0),  
  m_discSCTlike(false),
  m_inputLayerMaterialProvider(""),
  m_customMaterial(false),
  m_customMaterialThickness(0.),
  m_customMaterialX0(0.),       
  m_customMaterialL0(0.),       
  m_customMaterialA(0.),      
  m_customMaterialZ(0.),       
  m_customMaterialRho(0.),
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
  declareInterface<Trk::ILayerBuilder>(this);
  // general steering
  declareProperty("PixelCase"                        , m_pixelCase);
  declareProperty("SiDetManagerLocation"             , m_siMgrLocation);
  declareProperty("SetLayerAssociation"  , m_setLayerAssociation);
  // identification
  declareProperty("Identification"       , m_identification);

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
  declareProperty("LayersZsectors"       , m_layerZsectors);
  declareProperty("LayerPhiSectors"      , m_layerPhiSectors);
  declareProperty("LayerTilt"            , m_layerTilt);
  declareProperty("LayerMinPhi"          , m_layerMinPhi);
  declareProperty("LayerMaxPhi"          , m_layerMaxPhi);
  declareProperty("LayerMinZ"            , m_layerMinZ);
  declareProperty("LayerMaxZ"            , m_layerMaxZ);
  declareProperty("LayerRadius"          , m_layerRadius);
  declareProperty("LayerThickness"       , m_layerThickness);
  declareProperty("LayerLengthY"         , m_layerLengthY); 
  declareProperty("LayerLengthXmin"      , m_layerLengthXmin); 
  declareProperty("LayerLengthXmax"      , m_layerLengthXmax); 
  declareProperty("LayerPitchX"          , m_layerPitchX); 
  declareProperty("LayerPitchY"          , m_layerPitchY); 
  declareProperty("LayerRotation"        , m_layerRotation); 
  //  declareProperty("LayerSeparation"      , m_layerSeparation); 
  declareProperty("LayerStereo"          , m_layerStereo); 
  declareProperty("LayerStereoSeparation", m_layerStereoSeparation); 
  declareProperty("AdditionalLayerRadius", m_layerAdditionalLayerR);
  
  // For the Active Endcap Material
  declareProperty("EndcapDiscs"          , m_endcapDiscs);
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
  declareProperty("DiscStereo"           , m_discStereo); 
  declareProperty("DiscStereoSeparation" , m_discStereoSeparation); 
  declareProperty("AdditionalDiscZpos"   , m_discAdditionalLayerPosZ);

  declareProperty("CheckGeo", m_checkGeo);
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
  
  // Fill the radius vector ( m_discRmin & m_discRmax )
  m_discRmin.clear();
  m_discRmax.clear();
  
  unsigned int Discs = m_discPhiSectors.size();
  
  for (unsigned int discCounter = 0; discCounter < Discs; discCounter++) {
    
    m_discRmin.push_back(m_discRingMinR[discCounter][0]);
    m_discRmax.push_back(m_discRingMaxR[discCounter][(m_discRingMaxR[discCounter].size())-1]);
    
  }

  if (!m_layerStereoSeparation.empty())
    m_layerSCTlike = true;
  
  if (!m_discStereoSeparation.empty())
    m_discSCTlike = true;

  m_detElementMap = new iFatras::IdHashDetElementCollection;
  //Retrieve and/or store the map with IdHash to DetElement 
  if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_detElementMapName))){
    if((detStore()->retrieve(m_detElementMap, m_detElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not retrieve collection " << m_detElementMapName);
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_DEBUG("Found and Retrieved collection " << m_detElementMapName);
  }else{
    if((detStore()->record(m_detElementMap, m_detElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not record collection " << m_detElementMapName);
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_DEBUG("Stored and Retrieved collection " << m_detElementMapName);
    } 

  if (m_inputLayerMaterialProvider.retrieve().isSuccess()){
    ATH_MSG_INFO ( "Using InputLayerMaterialProvider " << m_inputLayerMaterialProvider);      
    m_layerMaterial = m_inputLayerMaterialProvider->handleMaterial();
    ATH_MSG_INFO ( "handleMaterial Done!");      
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
	ATH_MSG_DEBUG ( "Ttree registered" );      
      }
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
    ATH_MSG_DEBUG ( "SiSmearedDigitizationTool : Writing Tree" );
  }
  
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

/** LayerBuilder interface method - returning Barrel-like layers */
const std::vector<const iFatras::PlanarDetElement*>* iFatras::PlanarDetLayerBuilder::CreateElements(unsigned int layerCounter, bool isCylinder) const {

  std::vector<iFatras::PlanarDetElement*>* cElements = new std::vector<iFatras::PlanarDetElement*>();

  std::vector<const iFatras::PlanarDetElement*>* const_Elements = new std::vector<const iFatras::PlanarDetElement*>();

  if (isCylinder){
    
    cElements->reserve(m_layerZsectors[layerCounter]*m_layerPhiSectors[layerCounter]);
    
    m_previousSliceElements = new std::vector<iFatras::PlanarDetElement*>(m_layerPhiSectors[layerCounter]);

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
    unsigned int posPhi = layerCounter - layerCounter/(m_discPhiSectors.size())*(2*(layerCounter-m_discPhiSectors.size()) + 1);

    size_t discRsectors = m_discPhiSectors[posPhi].size();
    unsigned int numPhiElement = 0;
    for (size_t irings=0; irings < discRsectors; ++irings){
      numPhiElement+=(int)m_discPhiSectors[posPhi][irings];
    }
    cElements->reserve(numPhiElement);

    for (unsigned int iring = 0; iring < discRsectors; ++iring){
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

  for (unsigned int El = 0; El < tot_elem; El++) {
    const_Elements->push_back(dynamic_cast<const iFatras::PlanarDetElement*>(cElements->at(El)));
  }
  
  return const_Elements;
}

iFatras::PlanarDetElement* iFatras::PlanarDetLayerBuilder::CylinderDetElement(unsigned int layerCounter, unsigned int Phi, unsigned int Z) const
{
  double tilt = TMath::Pi()/180. * m_layerTilt[layerCounter];

  int brl_ec = 0; // For identifier : barrel elements brl_ec = 0
  int layer = layerCounter;
  int etaModule = (2*Z-m_layerZsectors[layerCounter]-1)/2; // For identifier: 0 in the central element. Then you have negative and positive values.

  // For identifier: 0 is the element that corresponds to 0 rad (different convention wrt the binutility that uses [-pi; pi])
  int phiModule = Phi+m_layerPhiSectors[layerCounter]/2; 
  if ( 2*(Phi + 1) > m_layerPhiSectors[layerCounter])
    phiModule = Phi-m_layerPhiSectors[layerCounter]/2; 
  
  Identifier idwafer(0);
  if(m_pixelCase)
    idwafer = m_pixIdHelper->wafer_id(brl_ec, layer, phiModule, etaModule);

  //std::cout << "layer = " << layer << " phiModule = " << phiModule << " etaModule = " << etaModule << std::endl;

  IdentifierHash idhash(0);
  if(m_pixelCase)
    idhash = m_pixIdHelper->wafer_hash(m_pixIdHelper->wafer_id(idwafer));
 
  //std::cout << "layer = " << layerCounter << " Phi = " << Phi << " Z = "<< Z <<" id = " << idwafer << " idhash = " << idhash << std::endl;
  
  // create the transform parameters
  double phistep = TMath::Pi()/180. *(m_layerMaxPhi[layerCounter]-m_layerMinPhi[layerCounter])/double(m_layerPhiSectors[layerCounter]);
  double phi     = TMath::Pi()/180. * m_layerMinPhi[layerCounter] + (Phi-1) * phistep + phistep/2.;
  double phi_angle = (Phi-1) * phistep + phistep/2.;
 
  double zstep   = (m_layerMaxZ[layerCounter]-m_layerMinZ[layerCounter])/double(m_layerZsectors[layerCounter]);
  double z       = m_layerMinZ[layerCounter]   +   (Z-1) * zstep   + zstep/2.;
  
  double dx = m_layerRadius[layerCounter] * cos(phi);
  double dy = m_layerRadius[layerCounter] * sin(phi);
  double dz = z;

  Amg::Vector3D transl(dx,dy,dz);
    
  double rot = 0.0;
  if (!m_layerRotation.empty())
    rot =m_layerRotation[layerCounter]*TMath::Pi()/180.;
  
  double stereo = 0.0;
  if (!m_layerStereo.empty())
    stereo = m_layerStereo[layerCounter]*TMath::Pi()/180.;
  
  // create the rotation 
  Amg::RotationMatrix3D rotation;     
  rotation = Amg::AngleAxis3D(-TMath::Pi()/2 + rot, Amg::Vector3D::UnitX())* // you need -TMath::Pi()/2 to make the correct rotation
    Amg::AngleAxis3D(-TMath::Pi()/2 - phi_angle - tilt, Amg::Vector3D::UnitY())* // you need -TMath::Pi()/2 to make the correct rotation
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitZ());

  // Amg::Vector3D colx( -sin(stereo)*sin(rot)*cos(phi+tilt) - cos(stereo)*sin(phi+tilt), -sin(stereo)*sin(rot)*sin(phi+tilt) + cos(stereo)*cos(phi+tilt), sin(stereo)*cos(rot));
  // Amg::Vector3D coly( cos(stereo)*sin(rot)*cos(phi+tilt) - sin(stereo)*sin(phi+tilt), cos(stereo)*sin(rot)*sin(phi+tilt)- sin(stereo)*cos(phi+tilt), +cos(stereo)*cos(rot));
  // Amg::Vector3D colz( cos(rot)*cos(phi+tilt), cos(rot)*sin(phi+tilt), -sin(rot));
  
  // rotation.col(0) = colx;
  // rotation.col(1) = coly;
  // rotation.col(2) = colz;

  Amg::Transform3D transform(rotation, transl);

  bool isOuterMost = false;

  if(layerCounter == m_layerRadius.size())
    isOuterMost = true;
  
  if (m_layerSCTlike)
    if( m_layerStereoSeparation[layerCounter] > 0.)
      isOuterMost = false;
  
  // double layer_separation = 0.;
  // if (!m_layerSeparation.empty())
  //   layer_separation = m_layerSeparation[layerCounter];

  double lengthXmax = 0.;
  if (!m_layerLengthXmax.empty())
    lengthXmax = m_layerLengthXmax[layerCounter];
  
  ATH_MSG_VERBOSE("Inside CylinderDetElement() --> Creating PlanarDetElement with " << Amg::toString(transform));

  // get the layer material from the helper method
  //const Trk::LayerMaterialProperties* layerMaterial = surfaceLayerMaterial(m_layerPitchX[layerCounter], m_layerLengthXmin[layerCounter], m_layerPitchY[layerCounter], m_layerLengthY[layerCounter]);
  
  const Trk::LayerMaterialProperties* layerMaterial = m_layerMaterial;
  
  //  ATH_MSG_INFO("PlanarDetElement " << idwafer << "  " << idhash << " with material property " << *layerMaterial);
  
  iFatras::PlanarDetElement* planElement = new iFatras::PlanarDetElement(idwafer, idhash, transform, *layerMaterial, m_layerThickness[layerCounter], m_layerLengthY[layerCounter], m_layerLengthXmin[layerCounter], lengthXmax, m_layerPitchX[layerCounter], m_layerPitchY[layerCounter], m_pixelCase, true, isOuterMost);

  if (!planElement) ATH_MSG_WARNING("Inside CylinderDetElement() --> Null poiter for the Planar Detector Element.");

  if (m_layerSCTlike) {
    if (m_layerStereoSeparation[layerCounter] > 0.) {
      
      if(layerCounter == m_layerRadius.size())
	isOuterMost = true;
      
      double dx_os = dx + m_layerStereoSeparation[layerCounter] * cos(phi);
      double dy_os = dy + m_layerStereoSeparation[layerCounter] * sin(phi);
      double dz_os = dz;
      
      Amg::Vector3D transl_os(dx_os,dy_os,dz_os);
      
      // create the rotation 
      Amg::RotationMatrix3D rotation_os;     
      rotation_os =  Amg::AngleAxis3D(-TMath::Pi()/2 + rot, Amg::Vector3D::UnitX())* // you need -TMath::Pi()/2 to make the correct rotation
	Amg::AngleAxis3D(-TMath::Pi()/2 - phi_angle - tilt, Amg::Vector3D::UnitY())* // you need -TMath::Pi()/2 to make the correct rotation
    	Amg::AngleAxis3D(stereo, Amg::Vector3D::UnitZ());
      
      Amg::Transform3D transform_os(rotation_os, transl_os);

      // get the layer material from the helper method
      //const Trk::LayerMaterialProperties* layerMaterial_os = surfaceLayerMaterial(m_layerPitchX[layerCounter], m_layerLengthXmin[layerCounter], m_layerPitchY[layerCounter], m_layerLengthY[layerCounter]);
      
      const Trk::LayerMaterialProperties* layerMaterial_os = m_layerMaterial;
      
      iFatras::PlanarDetElement* planElement_os = new iFatras::PlanarDetElement(idwafer, idhash, transform_os, *layerMaterial_os, m_layerThickness[layerCounter], m_layerLengthY[layerCounter], m_layerLengthXmin[layerCounter], lengthXmax, m_layerPitchX[layerCounter], m_layerPitchY[layerCounter], m_pixelCase, true, isOuterMost);
      
      // each element points to the other face
      planElement->setBackSide(planElement_os);  
      planElement_os->setFrontSide(planElement); 
      
      if (!planElement_os) ATH_MSG_WARNING("Inside CylinderDetElement() --> Null poiter for the other side Planar Detector Element.");
      
    }
    else  ATH_MSG_WARNING("CylinderDetElement() --> Distance between stereo surfaces is negative or zero. Please use a positive distance. Surface not created.");
  }

  //Set next/previous in phi if other elements have been already built
  
  if (Phi == 1) // Is the first element in phi 
    m_firstLayerElement = planElement;
    
  if (planElement != m_firstLayerElement)  { // if it is not the first element 
    planElement -> setPrevInPhi(m_previousLayerElement);
    m_previousLayerElement -> setNextInPhi(planElement);
  }
  
  if(Phi == m_layerPhiSectors[layerCounter]) { // Phi is closed -> the last has as next the first one and the first has as previous the last.
    planElement -> setNextInPhi(m_firstLayerElement);
    m_firstLayerElement -> setPrevInPhi(planElement);
  }
  
  m_previousLayerElement = planElement;
  
  //Set next/previous in eta if other elements have been already built
  // Eta is open -> the first slice of the cylinder has not previous elements and the last ones has not next. 
  // All the other elements have previous and next.
  if (Z == 1)
    m_previousSliceElements->at(Phi-1) = planElement;
  else{
    planElement -> setPrevInEta(m_previousSliceElements->at(Phi-1));
    (m_previousSliceElements->at(Phi-1)) -> setNextInEta(planElement);
    m_previousSliceElements->at(Phi-1) = planElement;    
  }

  m_detElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, planElement)); 
    
  return planElement;  
}

iFatras::PlanarDetElement* iFatras::PlanarDetLayerBuilder::DiscDetElement(unsigned int discCounter, unsigned int iring, unsigned int Phi) const
{
  unsigned int VectPos = discCounter - discCounter/m_discPhiSectors.size()*(2*(discCounter-m_discPhiSectors.size()) + 1);

  int brl_ec = 2*m_discZpos[discCounter]/abs(m_discZpos[discCounter]); // For identifier : endcap element brl_ec = +/-2 (neg ec/pos ec)
  int disc = abs(VectPos-m_discZpos.size()/m_discPhiSectors.size());
  int phiModule = (Phi-1) + m_discPhiSectors[VectPos][iring]/2; 
   if ( 2*Phi > m_discPhiSectors[VectPos][iring])
     phiModule = (Phi-1) - m_discPhiSectors[VectPos][iring]/2;
  
  int etaModule = iring;
  
  Identifier idwafer(0);
  if(m_pixelCase)
    idwafer = m_pixIdHelper->wafer_id(brl_ec, disc, phiModule, etaModule);

  // std::cout << "disc = " << disc << " phiModule= " << phiModule << " etaModule = " << etaModule << std::endl;
   
  IdentifierHash idhash(0);
  if(m_pixelCase)
    idhash = m_pixIdHelper->wafer_hash(m_pixIdHelper->wafer_id(idwafer));
    
  // create the transform parameters
  double phistep = TMath::Pi()/180. * (m_discMaxPhi[VectPos][iring]-m_discMinPhi[VectPos][iring])/double(m_discPhiSectors[VectPos][iring]);
  double phi = TMath::Pi()/180. * m_discMinPhi[VectPos][iring] + (Phi-1) * phistep + phistep/2.;
  double phi_angle = (Phi-1) * phistep + phistep/2.;
    
  double discLengthY    = (m_discRingMaxR[VectPos][iring] - m_discRingMinR[VectPos][iring])* cos(phistep/2.);
  // double discLengthMaxX = 2*m_discRingMaxR[VectPos][iring] * sin(phistep/2.);
  
  double dx = (m_discRingMaxR[VectPos][iring]*cos(phistep/2.)-discLengthY/2.) * cos(phi);
  double dy = (m_discRingMaxR[VectPos][iring]*cos(phistep/2.)-discLengthY/2.) * sin(phi);
  
  int sign = 1;
  if (Phi%2) sign*=-1;

  double disc_separation = 0.;
  if (!m_discSeparation.empty())
    disc_separation = m_discSeparation[VectPos][iring];
    
  double dz = m_discZpos[discCounter]+sign*disc_separation/2.;
  
  Amg::Vector3D transl(dx,dy,dz);
    
  // create the rotation
  Amg::RotationMatrix3D rotation;     
  rotation = Amg::AngleAxis3D(0, Amg::Vector3D::UnitX())*
    Amg::AngleAxis3D(0, Amg::Vector3D::UnitY())*
    Amg::AngleAxis3D(TMath::Pi()/2+phi_angle, Amg::Vector3D::UnitZ()); // you need TMath::Pi()/2 to make the correct rotation
  
  // phi -= TMath::Pi()/2.;
  // Amg::Vector3D colx(  -cos(phi), -sin(phi),   0);
  // Amg::Vector3D coly( -sin(phi), cos(phi),   0);
  // Amg::Vector3D colz(         0,        0,   -1);
  
  // rotation.col(0) = colx;
  // rotation.col(1) = coly;
  // rotation.col(2) = colz;
 
  Amg::Transform3D transform(rotation, transl);
  
  bool isOuterMost = false;

  if(discCounter == 1 || discCounter == m_discZpos.size())
    isOuterMost = true;
  
  if (m_discSCTlike)
    if (m_discStereoSeparation[VectPos][iring] > 0.)
      isOuterMost = false;

  double lengthXmax = 0.;
  if (!m_discLengthXmax.empty())
    lengthXmax = m_discLengthXmax[VectPos][iring];

  // get the layer material from the helper method
  //const Trk::LayerMaterialProperties* layerMaterial = surfaceLayerMaterial(m_discPitchX[VectPos][iring], m_discLengthXmin[VectPos][iring], m_discPitchY[VectPos][iring],m_discLengthY[VectPos][iring]);

  const Trk::LayerMaterialProperties* layerMaterial = m_layerMaterial;
  
  // ATH_MSG_INFO("PlanarDetElement " << idwafer << "  " << idhash << " with material property " << *layerMaterial);
    
  iFatras::PlanarDetElement* planElement = new iFatras::PlanarDetElement(idwafer, idhash, transform, *layerMaterial, m_discThickness[VectPos], m_discLengthY[VectPos][iring], m_discLengthXmin[VectPos][iring], lengthXmax, m_discPitchX[VectPos][iring], m_discPitchY[VectPos][iring], m_pixelCase, false, isOuterMost);

  ATH_MSG_VERBOSE("Inside DiscDetElement() --> Created PlanarDetElement with " << Amg::toString(transform));

  if (!planElement) ATH_MSG_WARNING("Inside DiscDetElement() --> Null poiter for the Planar Detector Element.");
  
  if (m_discSCTlike) {
    if (m_discStereoSeparation[VectPos][iring] > 0.) {
      
      if(discCounter == 1 || discCounter == m_discZpos.size())
	isOuterMost = true;
      
      double dx_os = dx;
      double dy_os = dy;
      double dz_os = dz+m_discZpos[discCounter]/abs(m_discZpos[discCounter])*m_discStereoSeparation[VectPos][iring];
      
      Amg::Vector3D transl_os(dx_os,dy_os,dz_os);
      phi += m_discZpos[discCounter]/abs(m_discZpos[discCounter])*TMath::Pi()/180. *m_discStereo[VectPos][iring]; // Add the stereo angle (the rotation depends from pos/neg z)
      // create the rotation 
      Amg::RotationMatrix3D rotation_os;     
      rotation_os = Amg::AngleAxis3D(0, Amg::Vector3D::UnitX())*
	Amg::AngleAxis3D(0, Amg::Vector3D::UnitY())*
	Amg::AngleAxis3D(phi, Amg::Vector3D::UnitZ());
      
      Amg::Transform3D transform_os(rotation_os, transl_os);

      // get the layer material from the helper method
      //const Trk::LayerMaterialProperties* layerMaterial_os = surfaceLayerMaterial(m_discPitchX[VectPos][iring], m_discLengthXmin[VectPos][iring], m_discPitchY[VectPos][iring],m_discLengthY[VectPos][iring]);
      
      const Trk::LayerMaterialProperties* layerMaterial_os = m_layerMaterial;
      
      iFatras::PlanarDetElement* planElement_os = new iFatras::PlanarDetElement(idwafer, idhash, transform_os, *layerMaterial_os, m_discThickness[VectPos], m_discLengthY[VectPos][iring], m_discLengthXmin[VectPos][iring], lengthXmax, m_discPitchX[VectPos][iring], m_discPitchY[VectPos][iring], m_pixelCase, false, isOuterMost);
      
      // each element points to the other face
      planElement->setBackSide(planElement_os); 
      planElement_os->setFrontSide(planElement); 
      
      if (!planElement_os) ATH_MSG_WARNING("Inside DiscDetElement() --> Null poiter for the other side Planar Detector Element.");
      
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

  m_detElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, planElement));
  
  return planElement;  
}


/** LayerBuilder interface method - returning Barrel-like layers */
  const std::vector< const Trk::CylinderLayer* >* iFatras::PlanarDetLayerBuilder::cylindricalLayers() const
{
  // screen output
  ATH_MSG_INFO( "Configured to build " << m_barrelLayers << " (active) barrel layers");
  
  // ------------------------ Construction of the layers -----------------------------------          
  // construct the layers
  std::vector< const Trk::CylinderLayer* >* cylinderLayers = new std::vector< const Trk::CylinderLayer* >;
  std::vector<double>::const_iterator layerRadiusIter = m_layerRadius.begin();   
  
  // for the additional layer
  std::vector<double>::const_iterator addLayerIter    = m_layerAdditionalLayerR.begin();
  std::vector<double>::const_iterator addLayerIterEnd = m_layerAdditionalLayerR.end();
  
  int layerCounter          = 0;
  double currentLayerExtend = 0.;
  size_t sumCheckBarrelModules  = 0;
  m_barrelElements.reserve(m_barrelLayers);
    
  for ( ; layerRadiusIter != m_layerRadius.end() || addLayerIter != m_layerAdditionalLayerR.end(); ){

    double currentLayerRadius         = 10e10;
    
    // Create the detector elements
    m_barrelElements[layerCounter] = CreateElements(layerCounter);

    ATH_MSG_VERBOSE( "Created " << m_barrelElements[layerCounter]->size() <<" Elements with counter " << layerCounter );

    m_layerSurfaces.push_back(std::vector< Trk::SurfaceOrderPosition >());
    
    // Put the surfaces in the vector< vector< Trk::SurfaceOrderPosition > >
    std::vector<const iFatras::PlanarDetElement*>::const_iterator Elem_Iter = m_barrelElements[layerCounter]->begin();
    for ( ; Elem_Iter != m_barrelElements[layerCounter]->end(); Elem_Iter++){
      const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
      Amg::Vector3D orderPosition((*Elem_Iter)->center());
      // register the module surface
      Trk::SharedObject<const Trk::Surface> * sharedSurface = new Trk::SharedObject<const Trk::Surface>(moduleSurface);
      Trk::SurfaceOrderPosition surfaceOrder(*sharedSurface, orderPosition);
      m_layerSurfaces[layerCounter].push_back(surfaceOrder);

      if(m_pixelCase){
       	ATH_MSG_DEBUG("Surface " << (*moduleSurface));
	ATH_MSG_DEBUG("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
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
	ATH_MSG_DEBUG("ID " << hitId << ", hash " << hitIdHash );
	ATH_MSG_DEBUG("barrel_ec " << m_pixIdHelper->barrel_ec(hitId) << ", layer_disk " << m_pixIdHelper->layer_disk(hitId) << ", phi_module " <<m_pixIdHelper->phi_module(hitId) << ", eta_module " << m_pixIdHelper->eta_module(hitId) );
	ATH_MSG_DEBUG("Phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta());
      }
    }
    
    Trk::CylinderLayer* activeLayer   = 0;

    if (layerRadiusIter != m_layerRadius.end()){
      
      // create the BinKeyUtility - the phi binning is identical
      double halfPhiStep = TMath::Pi()/m_layerPhiSectors[layerCounter];
      Trk::BinUtility* currentBinUtility   =  new Trk::BinUtility(m_layerPhiSectors[layerCounter],
								  (*layerRadiusIter)*(TMath::Pi()/180. *m_layerMinPhi[layerCounter]-halfPhiStep),
								  (*layerRadiusIter)*(TMath::Pi()/180. *m_layerMaxPhi[layerCounter]+halfPhiStep),
								  Trk::closed,
								  Trk::binRPhi);
      (*currentBinUtility) += Trk::BinUtility(m_layerZsectors[layerCounter],
					      m_layerMinZ[layerCounter],
					      m_layerMaxZ[layerCounter],
					      Trk::open,
					      Trk::binZ);
      // creating the binned array output
      ATH_MSG_DEBUG("Creating the binned array for the sensitive detector elements with BinUtility :");
      ATH_MSG_DEBUG( *currentBinUtility );
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

      currentLayerExtend = m_layerLengthY[layerCounter]/2.;
      
      // create the material
      const Trk::LayerMaterialProperties* layerMaterial = barrelLayerMaterial(m_layerRadius[layerCounter], currentLayerExtend);
     
      // screen output
      ATH_MSG_VERBOSE( "Construct BinnedArray for CylinderLayer with "<< (m_layerSurfaces[layerCounter]).size() << " SubSurfaces." );
      ATH_MSG_VERBOSE( "Building a CylinderLayer with " << m_layerPhiSectors[layerCounter]
		     << " / " <<   m_layerZsectors[layerCounter] << " phi/z bins. "   );
      ATH_MSG_VERBOSE( "  -> With Radius     :  " << m_layerRadius[layerCounter]    );       
      ATH_MSG_VERBOSE( "  -> With Thickness  :  " << m_layerThickness[layerCounter] );
      ATH_MSG_VERBOSE( "  -> With Zmin/Zmax  :  " << -currentLayerExtend << " / " << currentLayerExtend );
      
      // prepare the right overlap descriptor       
      //Trk::OverlapDescriptor* olDescriptor = 0;
      Trk::OverlapDescriptor* olDescriptor = new iFatras::PlanarOverlapDescriptor();
         
      // for eventual use with the passive layer
      currentLayerRadius = m_layerRadius[layerCounter];
      
      // construct the layer (finally)
      activeLayer = new Trk::CylinderLayer(new Trk::CylinderBounds(m_layerRadius[layerCounter],currentLayerExtend),
					   currentBinnedArray,
					   *layerMaterial,
					   m_layerThickness[layerCounter],
					   olDescriptor);

      // increase the iterator
      ++layerRadiusIter;                                                        
      // cleanup of the layer material --------------------------------------------------------------
      delete layerMaterial; 
      // register the layer to the surfaces
      const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
      registerSurfacesToLayer(layerSurfaces,*activeLayer);
      
    }   
    
    // construct the passive layers: --- given by jobOptions -------------------------------------
    bool buildPassiveLayer = m_layerAdditionalLayerR.size() && (addLayerIter != addLayerIterEnd);
    Trk::CylinderLayer* passiveLayer = 0;
    // check if an additional layer should be built --------------------------------------------            
    if (buildPassiveLayer && (*addLayerIter) < currentLayerRadius){
      // reset the current layer radius
      currentLayerRadius =  *addLayerIter;
      ATH_MSG_VERBOSE("Building an additional CylinderLayer w/o sensitive modules at");
      ATH_MSG_VERBOSE( "  -> With Radius     :  " << currentLayerRadius   );       
      ATH_MSG_VERBOSE( "  -> With Zmin/Zmax  :  " << -currentLayerExtend << " / " << currentLayerExtend );
      // the material for the passive layer
      const Trk::LayerMaterialProperties* passiveLayerMaterial = barrelLayerMaterial(currentLayerRadius,currentLayerExtend);
      // create the passive layer
      passiveLayer = new Trk::CylinderLayer(new Trk::CylinderBounds(*addLayerIter,currentLayerExtend),
					    *passiveLayerMaterial,
					    1.*Gaudi::Units::mm,
					    0,0);
      //increase the iterator
      ++addLayerIter;
      // cleanup of the layer material --------------------------------------------------------------
      delete passiveLayerMaterial;                                                         
    }
    
    // register the layers 
    if (activeLayer)  cylinderLayers->push_back(activeLayer);
    if (passiveLayer) cylinderLayers->push_back(passiveLayer);
    
    // increase the layer counter --- it is built
    ++layerCounter;    

  }// layer construction
  // --------------------------- enf of layer construction loop ----------------------------------
  
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
  
    // Create the detector elements
    m_endcapElements[discCounter] = CreateElements(discCounter, false);

    ATH_MSG_VERBOSE( "Created " << m_endcapElements[discCounter]->size() <<" Elements with counter " << discCounter );

    m_discSurfaces.push_back(std::vector< Trk::SurfaceOrderPosition >());

    // Put the surfaces in the vector< vector< Trk::SurfaceOrderPosition > >
    std::vector<const iFatras::PlanarDetElement*>::const_iterator Elem_Iter = m_endcapElements[discCounter]->begin();
    for ( ; Elem_Iter != m_endcapElements[discCounter]->end(); Elem_Iter++) {
      const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
      Amg::Vector3D orderPosition((*Elem_Iter)->center());
      // register the module surface
      Trk::SharedObject<const Trk::Surface> sharedSurface(moduleSurface);
      Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
      m_discSurfaces[discCounter].push_back(surfaceOrder);
      
      if(m_pixelCase){
	ATH_MSG_DEBUG("Surface " << (*moduleSurface));
	ATH_MSG_DEBUG("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
	Identifier hitId         = moduleSurface->associatedDetectorElementIdentifier();   
	IdentifierHash hitIdHash = m_pixIdHelper->wafer_hash(hitId);
	ATH_MSG_DEBUG("ID " << hitId << ", hash " << hitIdHash );
	ATH_MSG_DEBUG("barrel_ec " << m_pixIdHelper->barrel_ec(hitId) << ", layer_disk " << m_pixIdHelper->layer_disk(hitId) << ", phi_module " <<m_pixIdHelper->phi_module(hitId) << ", eta_module " << m_pixIdHelper->eta_module(hitId) );
	ATH_MSG_DEBUG("Phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta());
      }
    }

    unsigned int VectPos = discCounter - discCounter/m_discPhiSectors.size()*(2*(discCounter-m_discPhiSectors.size()) + 1);
    
    // the gathered R sectors  
    size_t discRsectors = (m_discPhiSectors[VectPos]).size();
    // dynamic estimation 1: estimate the layer thickness dynamically
    double thickness = m_discThickness[VectPos];       
    
    // preperare the rboundaries
    std::vector<double> discRboundaries;
    discRboundaries.push_back(*(m_discRingMinR[VectPos].begin()));
    for ( std::vector<double>::const_iterator ringMaxRIter = m_discRingMaxR[VectPos].begin(); 
	  ringMaxRIter != m_discRingMaxR[VectPos].end(); ++ringMaxRIter)   
      discRboundaries.push_back(*ringMaxRIter);
    
    // screen output           
    ATH_MSG_VERBOSE( "Building a DiscLayer with " << discRsectors << " R sectors. " );
    ATH_MSG_VERBOSE( "  -> At Z - Position      :  " << m_discZpos[discCounter] );
    ATH_MSG_VERBOSE( "  -> With Thickness       :  " << thickness             );

    ATH_MSG_VERBOSE( "  -> With Rmin/Rmax (est) :  " << m_discRmin[VectPos] << " / " << m_discRmax[VectPos] );
    ATH_MSG_VERBOSE( "  -> With Rings           :  " << discRsectors );
    
    for (size_t irings=0; irings<discRsectors; ++irings)
      ATH_MSG_VERBOSE( " --> " << irings <<  " R sector has " << (int)m_discPhiSectors[VectPos][irings] << " phi sectors. " );
    
    // prepare the binned array, it can be with one to several rings            
    Trk::BinnedArray<Trk::Surface>* currentBinnedArray = 0;
    std::vector<Trk::BinUtility*>* singleBinUtils = new std::vector<Trk::BinUtility*>;
    
    if (discRsectors==1){
      double halfPhiStep = TMath::Pi()/m_discPhiSectors[VectPos][0];
      ATH_MSG_VERBOSE("Constructing a one-dimensional BinnedArray with phiMin / phiMax (bins) = " 
		      << TMath::Pi()/180. * m_discMinPhi[VectPos][0]-halfPhiStep << " / " << TMath::Pi()/180. * m_discMaxPhi[VectPos][0]+halfPhiStep 
		      << " (" << (int)m_discPhiSectors[VectPos][0] << ")");
      // an easier bin utility can be used                 
      Trk::BinUtility* currentBinUtility = new Trk::BinUtility((int)m_discPhiSectors[VectPos][0] ,
							       TMath::Pi()/180. * m_discMinPhi[VectPos][0]-halfPhiStep,
							       TMath::Pi()/180. * m_discMaxPhi[VectPos][0]+halfPhiStep,
							       Trk::closed,
							       Trk::binPhi);
      // a one-dimensional BinnedArray is sufficient
      currentBinnedArray = new Trk::BinnedArray1D<Trk::Surface>(m_discSurfaces[discCounter],currentBinUtility);
      // creating the binned array output
      ATH_MSG_VERBOSE("Creating the binned array for the sensitive detector elements with BinUtility :");
      ATH_MSG_VERBOSE( *currentBinUtility );
         
    } else {
      ATH_MSG_VERBOSE("Constructing a two-dimensional BinnedArray.");
      // get the binning in R first (can still be improved with non-aequidistant binning) 
      Trk::BinUtility* currentSteerBinUtility = 0;
      currentSteerBinUtility =  new Trk::BinUtility(discRsectors,
						    m_discRmin[VectPos],
						    m_discRmax[VectPos],
						    Trk::open,
						    Trk::binR);
      ATH_MSG_VERBOSE("Steering bin utility constructed as : " << *currentSteerBinUtility);
      // the single phi bin utilities
      //std::vector<Trk::BinUtility*>* singleBinUtils = new std::vector<Trk::BinUtility*>;
      singleBinUtils->reserve(discRsectors);
	    for (size_t irings=0; irings < discRsectors; ++irings){
	      double halfPhiStep = TMath::Pi()/m_discPhiSectors[VectPos][irings];
	      ATH_MSG_VERBOSE("Constructing for ring " << irings << " phi utility with phiMin / phiMax (bins) = " 
			      <<TMath::Pi()/180. * m_discMinPhi[VectPos][irings]-halfPhiStep << " / " << TMath::Pi()/180. * m_discMaxPhi[VectPos][irings]+halfPhiStep 
			      << " (" << (int)m_discPhiSectors[VectPos][irings] << ")") ;
	      singleBinUtils->push_back(new Trk::BinUtility((int)m_discPhiSectors[VectPos][irings],
							    TMath::Pi()/180. * m_discMinPhi[VectPos][irings]-halfPhiStep,
							    TMath::Pi()/180. * m_discMaxPhi[VectPos][irings]+halfPhiStep,
							    Trk::closed,
							    Trk::binPhi));
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
    (*activeLayerTransform) = Amg::Translation3D(0.,0.,m_discZpos[discCounter]);
      
    Trk::DiscBounds* activeLayerBounds    = new Trk::DiscBounds(rMin,rMax);
    
    // prepare the right overlap descriptor  
    //Trk::OverlapDescriptor* olDescriptor = 0;
    Trk::OverlapDescriptor* olDescriptor = new iFatras::DiscOverlapDescriptor(currentBinnedArray, singleBinUtils);
    
    // layer creation
    Trk::DiscLayer* activeLayer = new Trk::DiscLayer(activeLayerTransform,
						    activeLayerBounds,
						    currentBinnedArray,
						    *layerMaterial,
						    thickness,
						    olDescriptor);

    // cleanup
    delete layerMaterial;
    // register the layer to the surfaces --- if necessary to the other sie as well
    const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
    registerSurfacesToLayer(layerSurfaces,*activeLayer);
    
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
	const Trk::LayerMaterialProperties* passiveLayerMaterial = endcapLayerMaterial(rMin,rMax);
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
 
 
const Trk::LayerMaterialProperties* iFatras::PlanarDetLayerBuilder::barrelLayerMaterial(double r, double hz) const{
   
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------
  // -- material with 1D binning
  Trk::BinUtility layerBinUtilityZ(m_barrelLayerBinsZ, -hz, hz, Trk::open, Trk::binZ);
  if (m_barrelLayerBinsPhi==1){
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

const Trk::LayerMaterialProperties* iFatras::PlanarDetLayerBuilder::endcapLayerMaterial(double rMin, double rMax) const
{
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------

  Trk::BinUtility layerBinUtilityR(m_endcapLayerBinsR,rMin,rMax,Trk::open, Trk::binR);
  // -- material with 1D binning
  if (m_barrelLayerBinsPhi==1){
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
	    
            // register the backise if necessary ---------------------------------------------------
	    const iFatras::PlanarDetElement* otherSideElement = detElement ?  detElement->backSide() : 0;                 
	    otherSideElement = otherSideElement ?  otherSideElement : detElement->frontSide();   
            const Trk::Surface* otherSideSurface = otherSideElement ? &(otherSideElement->surface()) : 0;
            if (otherSideSurface) Trk::ILayerBuilder::associateLayer(lay, *otherSideSurface);
	}
    }   
    
    return;
}

