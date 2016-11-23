/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// ================================================
// CreateMisalignAlg_ITk
// ================================================
//
// CreateMisalignAlg_ITk.cxx
// Source file for CreateMisalignAlg_ITk
//
// Namespace LocalChi2Align
// Header include

// Gaudi & StoreGate
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/SmartDataPtr.h"  //NTupleFilePtr
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// Geometry Stuff
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "DetDescrConditions/AlignableTransform.h"

// Alignment DB Stuff
#include "InDetAlignGenTools/IInDetAlignDBTool.h"
#include "AthenaKernel/IOVRange.h"
#include "InDetAlignGenAlgs/CreateMisalignAlg_ITk.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include <math.h>

namespace InDetAlignment
{
  // Constructor
  CreateMisalignAlg_ITk::CreateMisalignAlg_ITk(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_idHelper(nullptr),
    m_pixelIdHelper(nullptr),
    m_sctIdHelper(nullptr),
    m_pixelManager(nullptr),
    m_SCT_Manager(nullptr),
    m_IDAlignDBTool("InDetAlignDBTool"),
    m_asciiFileNameBase("MisalignmentSet"),
    m_SQLiteTag("test_tag"),
    m_firstEvent(true),
    m_createFreshDB(true),
    m_MisalignmentMode(0),
    m_nEvents(0),
    m_Misalign_x(0),
    m_Misalign_y(0),
    m_Misalign_z(0),
    m_Misalign_alpha(0),
    m_Misalign_beta(0),
    m_Misalign_gamma(0),
    m_Misalign_maxShift(1*CLHEP::mm),
    m_Misalign_maxShift_Inner(50*CLHEP::micrometer),
    m_ScalePixelBarrel(1.),
    m_ScalePixelEndcap(1.),
    m_ScaleSCTBarrel(1.),
    m_ScaleSCTEndcap(1.),
    m_VisualizationLookupTree(nullptr),
    m_AthenaHashedID(-1),
    m_HumanReadableID(-1)
      {
	declareProperty("ASCIIFilenameBase"             ,     m_asciiFileNameBase);
	declareProperty("SQLiteTag"                     ,     m_SQLiteTag);
	declareProperty("MisalignMode"                  ,     m_MisalignmentMode);
	declareProperty("MisalignmentX"                 ,     m_Misalign_x);
	declareProperty("MisalignmentY"                 ,     m_Misalign_y);
	declareProperty("MisalignmentZ"                 ,     m_Misalign_z);
	declareProperty("MisalignmentAlpha"             ,     m_Misalign_alpha);
	declareProperty("MisalignmentBeta"              ,     m_Misalign_beta);
	declareProperty("MisalignmentGamma"             ,     m_Misalign_gamma);
	declareProperty("MaxShift"                      ,     m_Misalign_maxShift);
        declareProperty("MaxShiftInner"                 ,     m_Misalign_maxShift_Inner);
	declareProperty("CreateFreshDB"                 ,     m_createFreshDB);
	declareProperty("IDAlignDBTool"                 ,     m_IDAlignDBTool);
	declareProperty("ScalePixelBarrel"              ,     m_ScalePixelBarrel);
	declareProperty("ScalePixelEndcap"              ,     m_ScalePixelEndcap);
	declareProperty("ScaleSCTBarrel"                ,     m_ScaleSCTBarrel);
	declareProperty("ScaleSCTEndcap"                ,     m_ScaleSCTEndcap);
      }
	
  //__________________________________________________________________________
  // Destructor
  CreateMisalignAlg_ITk::~CreateMisalignAlg_ITk(void) {
    ATH_MSG_DEBUG( "CreateMisalignAlg_ITk destructor called" );
  }
  
  //__________________________________________________________________________
  StatusCode CreateMisalignAlg_ITk::initialize() {
    
    ATH_MSG_DEBUG("CreateMisalignAlg_ITk initialize()" );
    
    if (m_IDAlignDBTool.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Can not retrieve InDetAlignDBTool " << m_IDAlignDBTool );
      return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG( "Retrieved InDetAlignDBTool " << m_IDAlignDBTool );
    
    //ID helper
    // Pixel
    if (detStore()->retrieve(m_pixelIdHelper, "PixelID").isFailure()) {
      ATH_MSG_FATAL( "Could not get Pixel ID helper" );
      return StatusCode::FAILURE; 
    } else ATH_MSG_DEBUG( "Retrieved PixelID helper");
    
    // SCT
    if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure()) {
      ATH_MSG_FATAL( "Could not get SCT ID helper" );
      return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG( "Retrieved SCT ID helper");

    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      ATH_MSG_FATAL( "Could not get AtlasDetectorID helper" );
      return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG( "Retrieved AtlasDetectorID helper");
    
    StatusCode sc(StatusCode::SUCCESS);
    
    //pixel and SCT manager
    sc = detStore()->retrieve(m_pixelManager, "Pixel");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get PixelManager !" );
      return sc;
    } else ATH_MSG_DEBUG( "Retrieved PixelManager");
    
    sc = detStore()->retrieve(m_SCT_Manager, "SCT");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get SCT_Manager !" );
      return sc;
    } else ATH_MSG_DEBUG( "Retrieved SCT_Manager");
    
    // Retrieve the Histo Service
    ITHistSvc* hist_svc;
    sc=service("THistSvc",hist_svc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not find HistService" );
      return sc;
    } else ATH_MSG_DEBUG( "Retrieved HistService");
    
    //Registering TTree for Visualization Lookup
    m_VisualizationLookupTree = new TTree("IdentifierTree", "Visualization Identifier Lookup Tree");
    
    sc = hist_svc->regTree("/IDENTIFIERTREE/IdentifierTree", m_VisualizationLookupTree);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to register TTree : " << "/TTree/VisualizationLookup/IdentifierTree" );
      return sc;
    }
    
    m_VisualizationLookupTree->Branch ("AthenaHashedID", &m_AthenaHashedID, "AthenaID/i");
    m_VisualizationLookupTree->Branch ("HumanReadableID", &m_HumanReadableID, "HumanID/I");
    
    // initialize generated Initial Alignment NTuple
    
    NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/CREATEMISALIGN");
    NTuplePtr nt(ntupleSvc(), "/NTUPLES/CREATEMISALIGN/InitialAlignment");
    
    if ( !nt ) {       // Check if already booked
      nt = ntupleSvc()->book("/NTUPLES/CREATEMISALIGN/InitialAlignment", CLID_ColumnWiseTuple, "InitialAlignment");
      if ( nt ) {
	ATH_MSG_INFO( "InitialAlignment ntuple booked." );
	
	ATH_CHECK( nt->addItem("x"                ,m_AlignResults_x) );
	ATH_CHECK( nt->addItem("y"                ,m_AlignResults_y) );
	ATH_CHECK( nt->addItem("z"                ,m_AlignResults_z) );
	ATH_CHECK( nt->addItem("alpha"            ,m_AlignResults_alpha) );
	ATH_CHECK( nt->addItem("beta"             ,m_AlignResults_beta) );
	ATH_CHECK( nt->addItem("gamma"            ,m_AlignResults_gamma) );
	ATH_CHECK( nt->addItem("ID"               ,m_AlignResults_Identifier_ID) );
	ATH_CHECK( nt->addItem("PixelSCT"         ,m_AlignResults_Identifier_PixelSCT) );
	ATH_CHECK( nt->addItem("BarrelEC"         ,m_AlignResults_Identifier_BarrelEC) );
	ATH_CHECK( nt->addItem("LayerDisc"        ,m_AlignResults_Identifier_LayerDisc) );
	ATH_CHECK( nt->addItem("Phi"              ,m_AlignResults_Identifier_Phi) );
	ATH_CHECK( nt->addItem("Eta"              ,m_AlignResults_Identifier_Eta) );
	ATH_CHECK( nt->addItem("center_x"         ,m_AlignResults_center_x) );
	ATH_CHECK( nt->addItem("center_y"         ,m_AlignResults_center_y) );
        ATH_CHECK( nt->addItem("center_z"         ,m_AlignResults_center_z) );
	
	if (sc.isFailure()) {
	  ATH_MSG_ERROR( "Problems creating InitialAlignment ntuple structure !" );
	  return sc;
	}
      } else {  // did not manage to book the N tuple....
        ATH_MSG_ERROR( "Failed to book InitialAlignment ntuple." );
      }
    }
    if (m_MisalignmentMode) {
      ATH_MSG_INFO( "Misalignment mode chosen: " << m_MisalignmentMode );
      if (m_MisalignmentMode == 1) {
	ATH_MSG_INFO( "MisalignmentX     : " << m_Misalign_x     / CLHEP::micrometer << " micrometer" );
	ATH_MSG_INFO( "MisalignmentY     : " << m_Misalign_y     / CLHEP::micrometer << " micrometer" );
	ATH_MSG_INFO( "MisalignmentZ     : " << m_Misalign_z     / CLHEP::micrometer << " micrometer" );
	ATH_MSG_INFO( "MisalignmentAlpha : " << m_Misalign_alpha / CLHEP::mrad       << " mrad"       );
	ATH_MSG_INFO( "MisalignmentBeta  : " << m_Misalign_beta  / CLHEP::mrad       << " mrad"       );
	ATH_MSG_INFO( "MisalignmentGamma : " << m_Misalign_gamma / CLHEP::mrad       << " mrad"       );
      } else {
	ATH_MSG_INFO( "with maximum shift of " << m_Misalign_maxShift / CLHEP::micrometer << " micrometer" );
      }
    } else {
      ATH_MSG_INFO( "Dry run, no misalignment will be generated." );
    }
    
    
   ATH_MSG_DEBUG( "Initialization completed!" );

   return sc;
      
  }

  //__________________________________________________________________________
  StatusCode CreateMisalignAlg_ITk::execute() {
    
    ATH_MSG_DEBUG( "CreateMisalignAlg_ITk execute()" );
    ++m_nEvents;
    
    if (m_firstEvent) {
      int nPixel = 0;
      int nSCT   = 0;
      
      if (m_createFreshDB) m_IDAlignDBTool->createDB();
      
      setupPixel_AlignModule(nPixel);
      setupSCT_AlignModule(nSCT);
      
      ATH_MSG_INFO( "Back from AlignModuleObject Setup. " );
      ATH_MSG_INFO(  nPixel << " Pixel modules found." );
      ATH_MSG_INFO(  nSCT   << "  SCT  modules found," );
      
      ATH_MSG_INFO( m_ModuleList.size() << " entries in identifier list" );
      
      if (StatusCode::SUCCESS!=GenerateMisaligment()) {
	ATH_MSG_ERROR( "GenerateMisalignment failed!" );
	return StatusCode::FAILURE;
      }
      m_firstEvent = false;
    }
   
   return StatusCode::SUCCESS;
  
  }

  //__________________________________________________________________________
  StatusCode CreateMisalignAlg_ITk::finalize() {
    
    ATH_MSG_DEBUG( "CreateMisalignAlg_ITk finalize()" );
    
    m_ModuleList.clear();
    
    return StatusCode::SUCCESS;
    
  }
  
  //__________________________________________________________________________
  void CreateMisalignAlg_ITk::setupSCT_AlignModule(int& nSCT) {
    InDetDD::SiDetectorElementCollection::const_iterator iter;
    
    for (iter = m_SCT_Manager->getDetectorElementBegin(); iter != m_SCT_Manager->getDetectorElementEnd(); ++iter) {
      const Identifier SCT_ModuleID = m_sctIdHelper->module_id((*iter)->identify()); //from wafer id to module id
      if (m_ModuleList.find(SCT_ModuleID) == m_ModuleList.end()) {
	const InDetDD::SiDetectorElement *module = m_SCT_Manager->getDetectorElement(SCT_ModuleID);
	m_ModuleList[SCT_ModuleID][0] = module->center()[0];
	m_ModuleList[SCT_ModuleID][1] = module->center()[1];
	m_ModuleList[SCT_ModuleID][2] = module->center()[2];
	++nSCT;
	ATH_MSG_INFO( "SCT module " << nSCT );
      }
      if (m_sctIdHelper->side((*iter)->identify()) == 0) { // inner side case
	// Write out Visualization Lookup Tree
	m_AthenaHashedID = SCT_ModuleID.get_identifier32().get_compact();
	m_HumanReadableID = 1000000*2 /*2 = SCT*/
	+ 100000*m_sctIdHelper->layer_disk(SCT_ModuleID)
	+ 1000*(10+m_sctIdHelper->eta_module(SCT_ModuleID))
	+ m_sctIdHelper->phi_module(SCT_ModuleID);
	if ( m_sctIdHelper->barrel_ec(SCT_ModuleID) != 0 ) 
	  m_HumanReadableID = m_sctIdHelper->barrel_ec(SCT_ModuleID)*(m_HumanReadableID + 10000000);
	
	ATH_MSG_INFO( "Human Readable ID: " << m_HumanReadableID );
	
	m_VisualizationLookupTree->Fill();
	
	// Syntax is (ID, Level) where Level is from 1 to 3 (3 is single module level)
	
	if (msgLvl(MSG::INFO)) {
	  HepGeom::Transform3D InitialAlignment = Amg::EigenTransformToCLHEP(m_IDAlignDBTool->getTrans(SCT_ModuleID,3));
	  msg() << "Initial Alignment of module " << m_idHelper->show_to_string(SCT_ModuleID,0,'/') << endreq;
	  msg() << "Alignment x = ("  << InitialAlignment.getTranslation().x() / CLHEP::micrometer  << ") micron" << endreq;
	  msg() << "Alignment y = ("  << InitialAlignment.getTranslation().y() / CLHEP::micrometer  << ") micron" << endreq;
	  msg() << "Alignment z = ("  << InitialAlignment.getTranslation().z() / CLHEP::micrometer  << ") micron" << endreq;
	  msg() << "Alignment x phi   = ("  << InitialAlignment.getRotation().phiX() / CLHEP::deg   << ") degree" << endreq;
	  msg() << "Alignment x Theta = ("  << InitialAlignment.getRotation().thetaX() / CLHEP::deg << ") degree" << endreq;
	  msg() << "Alignment y phi   = ("  << InitialAlignment.getRotation().phiY() / CLHEP::deg   << ") degree" << endreq;
	  msg() << "Alignment y Theta = ("  << InitialAlignment.getRotation().thetaY() / CLHEP::deg << ") degree" << endreq;
	  msg() << "Alignment z phi   = ("  << InitialAlignment.getRotation().phiZ() / CLHEP::deg   << ") degree" << endreq;
	  msg() << "Alignment z Theta = ("  << InitialAlignment.getRotation().thetaZ() / CLHEP::deg << ") degree" << endreq;
	  msg() << endreq;
	}
      } // end inner side case
    } //end loop over SCT elements
  }

  //__________________________________________________________________________
  void CreateMisalignAlg_ITk::setupPixel_AlignModule(int& nPixel) {
    
    InDetDD::SiDetectorElementCollection::const_iterator iter;
    
    for (iter = m_pixelManager->getDetectorElementBegin(); iter != m_pixelManager->getDetectorElementEnd(); ++iter) {
      
      // get the ID
      Identifier Pixel_ModuleID = (*iter)->identify();
      // check the validity
      if (Pixel_ModuleID.is_valid()) {
	if (m_ModuleList.find(Pixel_ModuleID) == m_ModuleList.end()) {
	  const InDetDD::SiDetectorElement *module = m_pixelManager->getDetectorElement(Pixel_ModuleID);
	  m_ModuleList[Pixel_ModuleID][0] = module->center()[0];
	  m_ModuleList[Pixel_ModuleID][1] = module->center()[1];
	  m_ModuleList[Pixel_ModuleID][2] = module->center()[2];
	  
	  ++nPixel;
	  ATH_MSG_INFO( "Pixel module " << nPixel );
	  
	  // Write out Visualization Lookup Tree
	  m_AthenaHashedID = Pixel_ModuleID.get_identifier32().get_compact();
	  m_HumanReadableID = 1000000*1 /*1 = Pixel*/
	  + 100000*m_pixelIdHelper->layer_disk(Pixel_ModuleID)
	  + 1000*(10+m_pixelIdHelper->eta_module(Pixel_ModuleID))
	  + m_pixelIdHelper->phi_module(Pixel_ModuleID);
	  if ( m_pixelIdHelper->barrel_ec(Pixel_ModuleID) != 0 ) m_HumanReadableID = m_pixelIdHelper->barrel_ec(Pixel_ModuleID)*(m_HumanReadableID + 10000000);
	  
	  ATH_MSG_INFO( "Human Readable ID: " << m_HumanReadableID );
	  
	  m_VisualizationLookupTree->Fill();
	  
	  if (msgLvl(MSG::INFO)) {
	    HepGeom::Transform3D InitialAlignment = Amg::EigenTransformToCLHEP(m_IDAlignDBTool->getTrans(Pixel_ModuleID,3));
	    msg() << "Initial Alignment of module " << m_idHelper->show_to_string(Pixel_ModuleID,0,'/') << endreq;
	    msg() << "Alignment x = ("  << InitialAlignment.getTranslation().x() / CLHEP::micrometer << ") micron"  << endreq;
	    msg() << "Alignment y = ("  << InitialAlignment.getTranslation().y() / CLHEP::micrometer << ") micron"  << endreq;
	    msg() << "Alignment z = ("  << InitialAlignment.getTranslation().z() / CLHEP::micrometer << ") micron"  << endreq;
	    msg() << "Alignment x phi   = ("  << InitialAlignment.getRotation().phiX() / CLHEP::deg << ") degree"   << endreq;
	    msg() << "Alignment x Theta = ("  << InitialAlignment.getRotation().thetaX() / CLHEP::deg << ") degree" << endreq;
	    msg() << "Alignment y phi   = ("  << InitialAlignment.getRotation().phiY() / CLHEP::deg << ") degree"   << endreq;
	    msg() << "Alignment y Theta = ("  << InitialAlignment.getRotation().thetaY() / CLHEP::deg << ") degree" << endreq;
	    msg() << "Alignment z phi   = ("  << InitialAlignment.getRotation().phiZ() / CLHEP::deg << ") degree"   << endreq;
	    msg() << "Alignment z Theta = ("  << InitialAlignment.getRotation().thetaZ() / CLHEP::deg << ") degree" << endreq;
	    msg() << endreq;
	  }
	}
      } else {
	ATH_MSG_INFO( "not a valid PIXEL Module ID (setup)" <<  endreq;
      }
    }
  }
  
  //__________________________________________________________________________
  StatusCode  CreateMisalignAlg_ITk::GenerateMisaligment() {
    IRndmGenSvc* randsvc;
    if (StatusCode::SUCCESS!=service("RndmGenSvc",randsvc,true)) {
      ATH_MSG_WARNING( "Cannot find RndmGenSvc" );
    } else { ATH_MSG_DEBUG( "Got RndmGenSvc" ); }
    
    int i = 0;
    /*
      ===================================
      Documentation of misalignment modes
      ===================================
      
      MisalignMode =
      0: nothing, no misalignments are generated
      1: Misalignment of whole InDet by 6 parameters
      2: random misalignment
      3: IBL-stave temperature dependent bowing --> Removed for the moment
      
      ====================================================
      Global Distortions according to David Brown (LHC Detector Alignment Workshop 2006-09-04, slides page 11)
      ====================================================
      11: R delta R:     Radial expansion linearly with r
      12: Phi delta R:   radial expansion sinuisoidally with phi
      13: Z delta R:     radial expansion linearly with z
      21: R delta Phi:   rotation linearly with r
      22: Phi delta Phi: rotation sinusoidally with phi
      23: Z delta Phi:   rotation linearly with z
      31: R delta Z:     z-shift linearly with r
      32: Phi delta Z:   z-shift sinusoidally with phi
      33: Z delta Z:     z-shift linearly with z
    */
    
    const double maxRadius=  1000.*CLHEP::cm;  // maximum radius of Silicon Detector (=outermost SCT radius)
    const double minRadius=    39.*CLHEP::mm;  // minimum radius of Silicon Detector (=innermost PIX radius)
    const double maxLength=  1372.*CLHEP::cm;  // maximum length of Silicon Detector barrel (=length of SCT barrel)
    
    const double maxDeltaR = m_Misalign_maxShift;
    const double maxAngle = 2 * asin( m_Misalign_maxShift / (2*maxRadius));
    const double maxAngleInner = 2 * asin ( m_Misalign_maxShift_Inner / (2*minRadius));
    const double maxDeltaZ = m_Misalign_maxShift;
    if (msgLvl(MSG::DEBUG)) {
      msg() << "maximum deltaPhi              = " << maxAngle/CLHEP::mrad << " mrad" << endreq;
      msg() << "maximum deltaPhi for 1/r term = " << maxAngleInner/CLHEP::mrad << " mrad" << endreq;
    }
    
    for (std::map<Identifier, HepGeom::Point3D<double> >::const_iterator iter = m_ModuleList.begin(); iter != m_ModuleList.end(); ++iter) {
      ++i;
      
      const Identifier& ModuleID = iter->first;
      
//       if (m_idHelper->is_pixel(ModuleID)) {
// 	ATH_MSG_INFO ("Pixel Module: " << m_pixelIdHelper->barrel_ec(ModuleID) << " - " << m_pixelIdHelper->layer_disk(ModuleID));
// 	if (m_pixelIdHelper->barrel_ec(ModuleID)!=0 or m_pixelIdHelper->layer_disk(ModuleID)!=0) {
// 	  ATH_MSG_INFO ("Skipping!!");
// 	  continue;
// 	}
//       }
      
      const InDetDD::SiDetectorElement * SiModule = 0; //dummy to get moduleTransform() for silicon
      
      if (m_idHelper->is_pixel(ModuleID))    SiModule = m_pixelManager->getDetectorElement(ModuleID);
      else if (m_idHelper->is_sct(ModuleID)) SiModule = m_SCT_Manager->getDetectorElement(ModuleID);
      else ATH_MSG_WARNING( "Something fishy, identifier is neither Pixel, nor SCT or TRT!" );
      
      //Si have a local coordinate system
      // Take care: For SCT we have to ensure that module's
      // system is taken, not the system of one of the wafers!
      
      const HepGeom::Transform3D localToGlobal = Amg::EigenTransformToCLHEP(SiModule->moduleTransform());
      
      const HepGeom::Point3D<double> center = iter->second;
      
      //center of module in global coordinates
      double r = center.rho(); //distance from beampipe
      double phi = center.phi();
      double z = center.z();
      
      HepGeom::Transform3D parameterizedTrafo;
      HepGeom::Transform3D alignmentTrafo;
      
      // prepare scale factor for different subsystems:
      double ScaleFactor = 1.;
      
      if (m_idHelper->is_pixel(ModuleID))
      {
	if (m_pixelIdHelper->is_barrel(ModuleID))   {
	  ScaleFactor=m_ScalePixelBarrel;
	} else {
	  ScaleFactor=m_ScalePixelEndcap;
	}
      } else if (m_idHelper->is_sct(ModuleID)) {
	
	if (m_sctIdHelper->is_barrel(ModuleID)) {
	  ScaleFactor=m_ScaleSCTBarrel;
	} else {
	  ScaleFactor=m_ScaleSCTEndcap;
	}
      } else {
	ATH_MSG_WARNING( "Something fishy, identifier is neither Pixel, nor SCT or TRT!" );
      }
	
      ATH_MSG_INFO(  "ID Module " << i << " with ID " << m_idHelper->show_to_string(ModuleID,0,'/') );
      if (msgLvl(MSG::DEBUG)) {
	msg() << "radius "  << r / CLHEP::cm << " centimeter"  << endreq;
	msg() << "phi "  << phi << endreq;
	msg() << "z "  << z / CLHEP::cm << " centimeter" << endreq;
	if (msgLvl(MSG::VERBOSE)) {
	  msg() << "localToGlobal transformation:" << endreq;
	  msg() << "translation: "  << localToGlobal.dx() / CLHEP::cm << ";" << localToGlobal.dy() / CLHEP::cm << ";" << localToGlobal.dz() / CLHEP::cm << endreq;
	  msg() << "rotation: "  << endreq;
	  msg() << localToGlobal.xx() << " " << localToGlobal.xy() << " " << localToGlobal.xz() << endreq;
	  msg() << localToGlobal.yx() << " " << localToGlobal.yy() << " " << localToGlobal.yz() << endreq;
	  msg() << localToGlobal.zx() << " " << localToGlobal.zy() << " " << localToGlobal.zz() << endreq;
	}
      }
      
      if (!m_MisalignmentMode) {
      
        //no misalignment mode set
        parameterizedTrafo = HepGeom::Transform3D(); // initialized as identity transformation
      
      } else if (m_MisalignmentMode==1) {
        
        //shift whole detector
        HepGeom::Vector3D<double> shift(ScaleFactor*m_Misalign_x, ScaleFactor*m_Misalign_y, ScaleFactor*m_Misalign_z);
        
        CLHEP::HepRotation rot;
        rot = CLHEP::HepRotationX(ScaleFactor*m_Misalign_alpha) * CLHEP::HepRotationY(ScaleFactor*m_Misalign_beta) * CLHEP::HepRotationZ(ScaleFactor*m_Misalign_gamma);
        
        if (ScaleFactor == 0.0)  {
	  parameterizedTrafo = HepGeom::Transform3D(); // initialized as identity transformation
        } else {
	  parameterizedTrafo = HepGeom::Transform3D(rot, shift);
        }
      
      } else if (m_MisalignmentMode == 2) {
	
	ATH_MSG_INFO ( "Working on layer with index = " << m_pixelIdHelper->layer_disk(ModuleID));
        
        // randomly misalign modules at L3
        
        Rndm::Numbers RandMisX(randsvc, Rndm::Gauss(0.,ScaleFactor*m_Misalign_x));
        Rndm::Numbers RandMisY(randsvc, Rndm::Gauss(0.,ScaleFactor*m_Misalign_y));
        Rndm::Numbers RandMisZ(randsvc, Rndm::Gauss(0.,ScaleFactor*m_Misalign_z));
        Rndm::Numbers RandMisalpha(randsvc, Rndm::Gauss(0.,ScaleFactor*m_Misalign_alpha));
        Rndm::Numbers RandMisbeta(randsvc, Rndm::Gauss(0.,ScaleFactor*m_Misalign_beta));
        Rndm::Numbers RandMisgamma(randsvc, Rndm::Gauss(0.,ScaleFactor*m_Misalign_gamma));
        
        if (msgLvl(MSG::DEBUG)) {
	  msg() << "JRS ScaleFactor: " << ScaleFactor );
	  msg() << "JRS X: " <<   RandMisX() << endreq;
	  msg() << "JRS Y: " <<   RandMisY() << endreq;
	  msg() << "JRS Z: " <<   RandMisZ() << endreq;
	  msg() << "JRS a: " <<   RandMisalpha() << endreq;
	  msg() << "JRS b: " <<   RandMisbeta()  << endreq;
	  msg() << "JRS c: " <<   RandMisgamma() << endreq;
        }
        
        HepGeom::Vector3D<double> shift(RandMisX(), RandMisY(), RandMisZ());
	//HepGeom::Vector3D<double> shift(0.01,0.,0.);
        CLHEP::HepRotation rot;
        rot = CLHEP::HepRotationX(RandMisalpha()) * CLHEP::HepRotationY(RandMisbeta()) * CLHEP::HepRotationZ(RandMisgamma());
        
        if (ScaleFactor == 0.0)  {
	  parameterizedTrafo = HepGeom::Transform3D(); // initialized as identity transformation
	} else {
	  parameterizedTrafo = HepGeom::Transform3D(rot, shift);
	}
   
      } else { // systematic misalignments
	
	if (m_MisalignmentMode/10==1) {
	  //radial misalignments
	  double deltaR;
	  if (m_MisalignmentMode==11) {
	    //R deltaR = radial expansion
	    //deltaR = 0.5 * cos ( 2*phi ) * r/maxRadius * maxDeltaR;
	    deltaR = r/maxRadius * maxDeltaR; //scale linearly in r
	  } else if (m_MisalignmentMode==12) {
	    //Phi deltaR = elliptical (egg-shape)
	    // deltaR = 0.5 * cos ( 2*phi ) * r/maxRadius * maxDeltaR;
	    deltaR = cos ( 2*phi ) * r/maxRadius * maxDeltaR;
	  } else if (m_MisalignmentMode==13) {
	    //Z deltaR = funnel
	    //deltaR = z/maxLength * maxDeltaR; // linearly in z
	    deltaR = 2. * z/maxLength * maxDeltaR; // linearly in z 
	  } else {
	    ATH_MSG_WARNING( "Wrong misalignment mode entered, doing nothing." );
	    deltaR=0;
	  }
	  
	  ATH_MSG_DEBUG( "deltaR for this module: "  << deltaR / CLHEP::micrometer << " um" );
	  parameterizedTrafo = HepGeom::Translate3D(deltaR*cos(phi),deltaR * sin(phi),0.); // translation along R vector
	  
	} else if (m_MisalignmentMode/10==2) {
	  
	  //azimuthal misalignments
	  double deltaPhi;
	  if (m_MisalignmentMode==21) {
	    //R deltaPhi = Curl
	    // const double a = maxAngle / maxRadius;
	    // const double c = maxAngleInner * minRadius;
	    // deltaPhi = r/maxRadius * maxAngle; //linearly in r
	    // deltaPhi = a*r + c/r; //linearly + reciprocal term in r
	    deltaPhi = r/maxRadius * maxAngle + minRadius/r * maxAngleInner; //linearly + reciprocal term in r
	  } else if (m_MisalignmentMode==22) {
	    //Phi deltaPhi = clamshell
	    // deltaPhi = fabs( sin ( phi )) * maxAngle;
	    // deltaPhi = 0.5 * cos ( 2*phi ) * maxAngle;
	    deltaPhi =  cos ( 2*phi ) * maxAngle;
	  } else if (m_MisalignmentMode==23) {
	    //Z deltaPhi = Twist
	    deltaPhi = 2*z/maxLength * maxAngle;
	    //deltaPhi = z/maxLength * maxAngle;
	  } else {
	    ATH_MSG_WARNING( "Wrong misalignment mode entered, doing nothing." );
	    deltaPhi=0;
	  }
	  ATH_MSG_DEBUG( "deltaPhi for this module: "  << deltaPhi/CLHEP::mrad << " mrad" );
	  parameterizedTrafo = HepGeom::RotateZ3D(deltaPhi); // rotation around z axis => in phi
	
	} else if (m_MisalignmentMode/10==3) {
	  //z misalignments
	  double deltaZ;
	  if (m_MisalignmentMode==31) {
	    //R deltaZ = Telescope
	    deltaZ = r/maxRadius * maxDeltaZ; //scale linearly in r
	  } else if (m_MisalignmentMode==32) {
	    //Phi deltaZ = Skew
	    //deltaZ = fabs (sin ( phi )) * maxDeltaZ;
	    //deltaZ =       sin ( phi )  * maxDeltaZ;
	    //deltaZ = 0.5 * cos ( 2*phi ) * maxDeltaZ;
	    deltaZ =  cos ( 2*phi ) * maxDeltaZ;
	  } else if (m_MisalignmentMode==33) {
	    //Z deltaZ = Z expansion
	    // deltaZ = z/maxLength * maxDeltaZ;
	    deltaZ = 2. * z/maxLength * maxDeltaZ;
	  } else {
	    ATH_MSG_WARNING( "Wrong misalignment mode entered, doing nothing." );
	    deltaZ=0;
	  }
	  ATH_MSG_DEBUG( "deltaZ for this module: "  << deltaZ/CLHEP::micrometer << " um" );
	  parameterizedTrafo = HepGeom::Translate3D(0,0,deltaZ); // translation in z direction
	  
	} else if (m_MisalignmentMode/10==4) { 
	  // catenary mode applied
	  // --> here we fix the length of the stave
	  // --> and there is a fixed constant that I have evaluated to assure 
	  // that the maximum shift is the one you have as input
	  double deltaY = 0.;
	  
	  if (m_idHelper->is_pixel(ModuleID) && m_pixelIdHelper->barrel_ec(ModuleID) == 0 ) {
	    ATH_MSG_INFO( "Evaluating deformation for module >> " << ModuleID );
	    double length = 1250. ; // this is the length of the 2 innermost barrel layers
	    if (m_pixelIdHelper->layer_disk(ModuleID) > 1)
	      length = 780.;
	    double a = 0.5*length*length/m_Misalign_y;
	    double z = SiModule->center().z();
	    deltaY = a*cosh(z/a)-a;
	  }
	  parameterizedTrafo = HepGeom::Translate3D(0,deltaY,0); // translation in global Y direction
	} else {
	  //no or wrong misalignment selected
	  ATH_MSG_WARNING( "Wrong misalignment mode entered, doing nothing." );
	  parameterizedTrafo = HepGeom::Transform3D(); // initialized as identity transformation
	}
      } //end of misalignment
      
      if (m_MisalignmentMode==2) { //random misalignment in local frame
	alignmentTrafo = parameterizedTrafo;
      } else {
	// final transformation executed in global coordinates, converted to local coordinates
	alignmentTrafo = localToGlobal.inverse() * parameterizedTrafo * localToGlobal; 
      }
      
      if (msgLvl(MSG::INFO)) {
	msg() << "Align Transformation x = ("  << alignmentTrafo.getTranslation().x() / CLHEP::micrometer << " um)" << endreq;
	msg() << "Align Transformation y = ("  << alignmentTrafo.getTranslation().y() / CLHEP::micrometer << " um)" << endreq;
	msg() << "Align Transformation z = ("  << alignmentTrafo.getTranslation().z() / CLHEP::micrometer << " um)" << endreq;
	msg() << "Align Transformation x phi   = ("  << alignmentTrafo.getRotation().phiX()   / CLHEP::deg <<   ")" << endreq;
	msg() << "Align Transformation x Theta = ("  << alignmentTrafo.getRotation().thetaX() / CLHEP::deg <<   ")" << endreq;
	msg() << "Align Transformation y phi   = ("  << alignmentTrafo.getRotation().phiY()   / CLHEP::deg <<   ")" << endreq;
	msg() << "Align Transformation y Theta = ("  << alignmentTrafo.getRotation().thetaY() / CLHEP::deg <<   ")" << endreq;
	msg() << "Align Transformation z phi   = ("  << alignmentTrafo.getRotation().phiZ()   / CLHEP::deg <<   ")" << endreq;
	msg() << "Align Transformation z Theta = ("  << alignmentTrafo.getRotation().thetaZ() / CLHEP::deg <<   ")" << endreq;
      }
      
      // suppress tiny translations that occur due to trafo.inverse*trafo numerics
      if ( fabs(alignmentTrafo.getTranslation().x()) < 1e-10) {
        HepGeom::Vector3D<double> zeroSuppressedTranslation(0,alignmentTrafo.getTranslation().y(),alignmentTrafo.getTranslation().z());
        alignmentTrafo = HepGeom::Transform3D(alignmentTrafo.getRotation(),zeroSuppressedTranslation);
      }
      
      if ( fabs(alignmentTrafo.getTranslation().y()) < 1e-10) {
        HepGeom::Vector3D<double> zeroSuppressedTranslation(alignmentTrafo.getTranslation().x(),0,alignmentTrafo.getTranslation().z());
        alignmentTrafo = HepGeom::Transform3D(alignmentTrafo.getRotation(),zeroSuppressedTranslation);
      }
      
      if ( fabs(alignmentTrafo.getTranslation().z()) < 1e-10) {
        HepGeom::Vector3D<double> zeroSuppressedTranslation(alignmentTrafo.getTranslation().x(),alignmentTrafo.getTranslation().y(),0);
        alignmentTrafo = HepGeom::Transform3D(alignmentTrafo.getRotation(),zeroSuppressedTranslation);
      }
      
      if ( fabs(alignmentTrafo.getRotation().getDelta()) < 1e-10) {
        CLHEP::HepRotation zeroSuppressedRotation(alignmentTrafo.getRotation());
        zeroSuppressedRotation.setDelta(0.);
        alignmentTrafo = HepGeom::Transform3D(zeroSuppressedRotation,alignmentTrafo.getTranslation());
      }
    
      Amg::Transform3D alignmentTrafoAmg = Amg::CLHEPTransformToEigen(alignmentTrafo);
      
      if (m_idHelper->is_sct(ModuleID) || m_idHelper->is_pixel(ModuleID)) {
        if (m_IDAlignDBTool->tweakTrans(ModuleID,3, alignmentTrafoAmg)) {
          ATH_MSG_INFO( "Update of alignment constants for module " << m_idHelper->show_to_string(ModuleID,0,'/') << " successful" );
        } else {
          ATH_MSG_ERROR( "Update of alignment constants for module " << m_idHelper->show_to_string(ModuleID,0,'/') << " not successful" );
        }
      } else {
        ATH_MSG_WARNING( "Something fishy, identifier is neither Pixel, nor SCT or TRT!" );
      }
    
      double alpha, beta, gamma;
      m_IDAlignDBTool->extractAlphaBetaGamma(alignmentTrafoAmg, alpha, beta, gamma);
      
      m_AlignResults_x = alignmentTrafo.getTranslation().x();
      m_AlignResults_y = alignmentTrafo.getTranslation().y();
      m_AlignResults_z = alignmentTrafo.getTranslation().z();
      m_AlignResults_alpha = alpha;
      m_AlignResults_beta = beta;
      m_AlignResults_gamma = gamma;
      
      m_AlignResults_center_x = center.x();
      m_AlignResults_center_y = center.y();
      m_AlignResults_center_z = center.z();
    
      if (m_idHelper->is_sct(ModuleID)) {
        m_AlignResults_Identifier_ID = 2;
        m_AlignResults_Identifier_PixelSCT = 2;
        m_AlignResults_Identifier_BarrelEC = m_sctIdHelper->barrel_ec(ModuleID);
        m_AlignResults_Identifier_LayerDisc = m_sctIdHelper->layer_disk(ModuleID);
        m_AlignResults_Identifier_Phi = m_sctIdHelper->phi_module(ModuleID);
        m_AlignResults_Identifier_Eta = m_sctIdHelper->eta_module(ModuleID);
      } else if (m_idHelper->is_pixel(ModuleID)) {
        m_AlignResults_Identifier_ID = 2;
        m_AlignResults_Identifier_PixelSCT = 1;
        m_AlignResults_Identifier_BarrelEC = m_pixelIdHelper->barrel_ec(ModuleID);
        m_AlignResults_Identifier_LayerDisc = m_pixelIdHelper->layer_disk(ModuleID);
        m_AlignResults_Identifier_Phi = m_pixelIdHelper->phi_module(ModuleID);
        m_AlignResults_Identifier_Eta = m_pixelIdHelper->eta_module(ModuleID);
      } else {
        ATH_MSG_WARNING( "Something fishy, identifier is neither Pixel, nor SCT or TRT!" );
      }
      // Write out AlignResults ntuple
      if (StatusCode::SUCCESS!=ntupleSvc()->writeRecord("NTUPLES/CREATEMISALIGN/InitialAlignment")) {
        ATH_MSG_ERROR( "Could not write InitialAlignment ntuple." );
      }
    }// end of module loop
    
    if (StatusCode::SUCCESS!=m_IDAlignDBTool->outputObjs()) {
      ATH_MSG_ERROR( "Writing of AlignableTransforms failed" );
    } else {
      ATH_MSG_INFO( "AlignableTransforms were written" );
      ATH_MSG_INFO( "Writing database to textfile" );
      m_IDAlignDBTool->writeFile(false,m_asciiFileNameBase+"_Si.txt");
      ATH_MSG_INFO( "Writing IoV information to mysql file" );
      m_IDAlignDBTool->fillDB("InDetSi_"+m_SQLiteTag,IOVTime::MINRUN,IOVTime::MINEVENT,IOVTime::MAXRUN,IOVTime::MAXEVENT);
    }
    
    return StatusCode::SUCCESS;               
    
  }
  
  //__________________________________________________________________________
  const HepGeom::Transform3D CreateMisalignAlg_ITk::BuildAlignTransform(const CLHEP::HepVector & AlignParams)
  {
    HepGeom::Vector3D<double> AlignShift(AlignParams[0],AlignParams[1],AlignParams[2]);
    CLHEP::HepRotation AlignRot;
    AlignRot = CLHEP::HepRotationX(AlignParams[3]) * CLHEP::HepRotationY(AlignParams[4]) * CLHEP::HepRotationZ(AlignParams[5]);
    HepGeom::Transform3D AlignTransform = HepGeom::Transform3D(AlignRot,AlignShift);
    return AlignTransform;
  }
  
} // end of namespace bracket
