/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_GeoModel/TRTDetectorFactory_Full.h"
#include "TRT_GeoModel/TRT_DetDescrDB_ParameterInterface.h"

#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetReadoutGeometry/TRT_BarrelDescriptor.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetReadoutGeometry/TRT_EndcapDescriptor.h"
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/Version.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/TRT_Conditions.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TRT_GeoModel/ArrayFunction.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "StoreGate/StoreGateSvc.h"

#include <vector>
#include <sstream>

//TK: get rid of these and use Genfun:: and GeoXF:: instead
using namespace Genfun;
using namespace GeoXF;



/////////////////////////////////// Constructor //////////////////////////////////
//
TRTDetectorFactory_Full::TRTDetectorFactory_Full(const InDetDD::AthenaComps * athenaComps,
						 ServiceHandle<ITRT_StrawStatusSummarySvc> m_summarySvc, // added for Argon
						 bool useOldActiveGasMixture,
						 bool DC2CompatibleBarrelCoordinates,
						 int overridedigversion,
						 bool alignable,
						 bool doArgon,
						 bool doKrypton,
						 bool useDynamicAlignmentFolders)
  : InDetDD::DetectorFactoryBase(athenaComps), 
    m_detectorManager(0), 
    m_materialManager(0),
    m_data(0), 
    m_useOldActiveGasMixture(useOldActiveGasMixture),
    m_DC2CompatibleBarrelCoordinates(DC2CompatibleBarrelCoordinates),
    m_overridedigversion(overridedigversion),
    m_alignable(alignable),
    m_sumSvc("TRT_StrawStatusSummarySvc","InDetTRTStrawStatusSummarySvc"),
    m_strawsvcavailable(0),
    m_doArgon(doArgon),
    m_doKrypton(doKrypton),
    m_useDynamicAlignFolders(useDynamicAlignmentFolders)
{ 
  m_sumSvc=m_summarySvc;
}
//////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// Destructor ///////////////////////////////////
//
TRTDetectorFactory_Full::~TRTDetectorFactory_Full() 
{ 
  delete m_materialManager;
  delete m_data; 
}
//////////////////////////////////////////////////////////////////////////////////



///////////////////////////// getDetectorManager /////////////////////////////////
//
//  The method that actually returns the TRT_DetectorManager, which was created
//  and filled by the create() method
//
const InDetDD::TRT_DetectorManager * TRTDetectorFactory_Full::getDetectorManager() const
{
  //TK: Maybe check that m_detectorManager!=0 ?
  return m_detectorManager;
}
//////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// create ///////////////////////////////////////
//
//  This is where the actual building of the geometry is performed.
//
//  The purpose of this is to create a new TRT_DetectorManager and fill it with
//  all the information relevant for detector description.
//
//  The TRT_DetectorManager itself, along with helper classes, descriptors, etc.
//  is located in InDetDetDescr/InDetReadoutGeometry.
//
void TRTDetectorFactory_Full::create(GeoPhysVol *world)
{
  // Create a new detectormanager.
  m_detectorManager = new InDetDD::TRT_DetectorManager();

  //---------------------- Initialize the parameter interface ------------------------//

  msg(MSG::DEBUG) << " Getting primary numbers from the Detector Description Database " << endmsg;  
  TRT_DetDescrDB_ParameterInterface * parameterInterface = new TRT_DetDescrDB_ParameterInterface(getAthenaComps());
  m_data = parameterInterface;

  //---------------------- Initialize the InnerDetector material manager ------------------------//

  m_materialManager = new InDetMaterialManager("TRT_MaterialManager", getAthenaComps());
  m_materialManager->addScalingTable(parameterInterface->scalingTable());

  //---------------------- Check if the folder TRT/Cond/StatusHT is in place ------------------------//
  m_strawsvcavailable = false;
  if (m_doArgon || m_doKrypton){
  	m_strawsvcavailable = detStore()->contains<TRTCond::StrawStatusMultChanContainer>("/TRT/Cond/StatusHT");
	m_strawsvcavailable &= (m_sumSvc->getStrawStatusHTContainer() != nullptr);
  }
  msg(MSG::DEBUG) << "The folder of /TRT/Cond/StatusHT is available? " << m_strawsvcavailable << endreq ;
  if (!m_strawsvcavailable) msg(MSG::WARNING) << "The folder of /TRT/Cond/StatusHT is NOT available, WHOLE TRT RUNNING XENON" << endreq;
  if (!m_doArgon  )	msg(MSG::WARNING) << "Tool setup will force to NOT to use ARGON. Ignore this warning if you are running RECONSTRUCTION or DIGI, but cross-check if you are running SIMULATION" << endreq;
  if (!m_doKrypton)	msg(MSG::WARNING) << "Tool setup will force to NOT to use KRYPTON. Ignore this warning if you are running RECONSTRUCTION or DIGI, but cross-check if you are running SIMULATION" << endreq;
  
 
  //---------------------- Initialize ID Helper ------------------------------------//

  // Initialize the ID helper:
//  bool idHelperInitialized=false;

  const TRT_ID *idHelper = 0;

  if (detStore()->retrieve(idHelper, "TRT_ID").isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve TRT ID Helper" << endreq;
  } //else {
    //idHelperInitialized = true;
  //}
  m_detectorManager->setIdHelper(idHelper,false);

  //---------------------- Set and Print Version Information ------------------------------------//

  //Set active gas type information.
  if (m_useOldActiveGasMixture) m_detectorManager->setGasType(InDetDD::TRT_DetectorManager::oldgas);
  else m_detectorManager->setGasType(InDetDD::TRT_DetectorManager::newgas);

  // Set Version information
  // Some of these get overwritten for new configurations.
  std::string versionTag = m_data->versionTag;
  std::string versionName = "DC2";
  std::string layout = "Final";
  std::string description = "DC2 Geometry";
  int versionMajorNumber = 2;
  int versionMinorNumber = 1;
  int versionPatchNumber = 0;

  if (m_data->initialLayout) layout = "Initial";
  //In principle we dont need to let the minor number reflect the
  //gastype anymore, but it doesn't hurt:
  if (m_useOldActiveGasMixture) versionMinorNumber = 0;
  if (!m_DC2CompatibleBarrelCoordinates) {
    versionMajorNumber = 3;
    versionName = "Rome";
    description = "Geometry for Rome 2005";
  }

  if (m_data->isCosmicRun) {
    layout = "SR1";
    description = "Geometry for SR1";
  }


  // If new configuration we get the version information from the database.
  // The version numbers can be incremented as one sees fit.
  // In principle they should be changed whenever there are any code changes.
  if (!m_data->oldConfiguration) {
    versionName = m_data->versionName;
    layout = m_data->layout;
    description = m_data->versionDescription;
    versionMajorNumber = 4;
    versionMinorNumber = 1;
    versionPatchNumber = 1;
  }

  InDetDD::Version version(versionTag,
			   versionName,
			   layout,
			   description,
			   versionMajorNumber,
			   versionMinorNumber,
			   versionPatchNumber);

  m_detectorManager->setVersion(version);


  // Print version information.
  msg(MSG::INFO) << "In TRT Detector Factory (For DC2 and later geometries)" << endmsg;
  msg(MSG::INFO) << " " << version.fullDescription() << endmsg;


  //---------- Set flags for which parts of the detector are built -----------//

  std::string barrelLabel = "Barrel";
  std::string endcapA_WheelAB_Label =  "EndcapA_WheelAB";
  std::string endcapC_WheelAB_Label =  "EndcapC_WheelAB";
  std::string endcapA_WheelC_Label  =  "EndcapA_WheelC";
  std::string endcapC_WheelC_Label  =  "EndcapC_WheelC";

  // Check if old naming scheme (which was a bit confusing with endcap C label)
  if (m_data->partPresent("EndcapAB_Plus")) {
    barrelLabel = "Barrel";
    endcapA_WheelAB_Label = "EndcapAB_Plus";
    endcapC_WheelAB_Label = "EndcapAB_Minus";
    endcapA_WheelC_Label  = "EndcapC_Plus";
    endcapC_WheelC_Label  = "EndcapC_Minus";
  }


  bool barrelPresent         = m_data->partPresent(barrelLabel);
  bool endcapABPlusPresent   = m_data->partPresent(endcapA_WheelAB_Label);
  bool endcapABMinusPresent  = m_data->partPresent(endcapC_WheelAB_Label);
  bool endcapCPlusPresent    = m_data->partPresent(endcapA_WheelC_Label);
  bool endcapCMinusPresent   = m_data->partPresent(endcapC_WheelC_Label);
  // Overall transform (probably will always be identifty - but just in case)
  HepGeom::Transform3D trtTransform =  m_data->partTransform("TRT");

  // For old configurations we need to set which parts are  present.
  //
  if (m_data->oldConfiguration) {
    if (m_data->isCosmicRun) {
      endcapABPlusPresent = false;
      endcapABMinusPresent = false;
      endcapCPlusPresent = false;
      endcapCMinusPresent = false;
    }
    if (m_data->initialLayout) {
      endcapCPlusPresent  = false;
      endcapCMinusPresent = false;
    }
  }


  //---------- Alignmnent and Conditions -----------//


  // Register the channels for alignment constants
  // and the level corresponding to the channel.
  // Not the levels are an internal definition . They are not the same as
  // the usual alignment levels
  const int AlignmentLevelSubWheel  = 1; // Level 2 in endcap. Not used in barrel
  const int AlignmentLevelModule    = 2; // Level 2 in barrel. Deprecated (wheel level) in endcap.
  const int AlignmentLevelTop       = 3; // Level 1

  if (m_alignable) {

    if (!m_useDynamicAlignFolders){
      m_detectorManager->addAlignFolderType(InDetDD::static_run1);
      m_detectorManager->addFolder("/TRT/Align");
      m_detectorManager->addChannel("/TRT/Align/TRT", AlignmentLevelTop, InDetDD::global);

      if (barrelPresent) {
        m_detectorManager->addChannel("/TRT/Align/B0",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/Align/B1",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/Align/B2",  AlignmentLevelModule, InDetDD::global);
      }
      if (endcapABPlusPresent) { // EndcapA
        m_detectorManager->addChannel("/TRT/Align/L2A", AlignmentLevelSubWheel, InDetDD::global);
      }
      if (endcapABMinusPresent) {// EndcapC
        m_detectorManager->addChannel("/TRT/Align/L2C", AlignmentLevelSubWheel, InDetDD::global);
      }
    }

    else {
      m_detectorManager->addAlignFolderType(InDetDD::timedependent_run2);

      m_detectorManager->addGlobalFolder("/TRT/AlignL1/TRT");
      m_detectorManager->addChannel("/TRT/AlignL1/TRT", AlignmentLevelTop, InDetDD::global);
      m_detectorManager->addFolder("/TRT/AlignL2");

      if (barrelPresent) {
        m_detectorManager->addChannel("/TRT/AlignL2/B0",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/AlignL2/B1",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/AlignL2/B2",  AlignmentLevelModule, InDetDD::global);
      }

      if (endcapABPlusPresent) { // EndcapA 
        m_detectorManager->addChannel("/TRT/AlignL2/L2A", AlignmentLevelSubWheel, InDetDD::global);
      }
      if (endcapABMinusPresent) {// EndcapC 
        m_detectorManager->addChannel("/TRT/AlignL2/L2C", AlignmentLevelSubWheel, InDetDD::global);
      }
    }

    // Matthias: Is this obsolete? No clear answer from alignment group january 2016
    // Commented for now
    /*if (endcapABPlusPresent) { // EndcapA
      m_detectorManager->addChannel("/TRT/Align/A0",  AlignmentLevelModule, InDetDD::global);
      m_detectorManager->addChannel("/TRT/Align/A1",  AlignmentLevelModule, InDetDD::global);
      m_detectorManager->addChannel("/TRT/Align/A2",  AlignmentLevelModule, InDetDD::global);   
      m_detectorManager->addChannel("/TRT/Align/A3",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A4",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A5",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A6",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A7",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A8",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A9",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A10", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A11", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A12", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/A13", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/L2A", AlignmentLevelSubWheel, InDetDD::global);   
    }                                                                                             
    if (endcapABMinusPresent) {// EndcapC                                                         
      m_detectorManager->addChannel("/TRT/Align/C0",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C1",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C2",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C3",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C4",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C5",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C6",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C7",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C8",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C9",  AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C10", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C11", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C12", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/C13", AlignmentLevelModule, InDetDD::global);     
      m_detectorManager->addChannel("/TRT/Align/L2C", AlignmentLevelSubWheel, InDetDD::global);   
      }*/

    // Unchanged in Run1 and new Run2 schema                                                     
    m_detectorManager->addSpecialFolder("/TRT/Calib/DX");
  }
                                               
  

  // Interface to conditions
  InDetDD::TRT_Conditions * conditions = new InDetDD::TRT_Conditions;
  m_detectorManager->setConditions(conditions);


  //Uncomment for testing:
  //  m_data->ShowValues();

  //---------- Digitization Version Info for dig. and recon r-t -----------//
  if (m_overridedigversion < 0 ) {
    m_detectorManager->setDigitizationVersion(m_data->digversion,m_data->digversionname);
  } else {
    m_detectorManager->setDigitizationVersion(m_overridedigversion,"CUSTOMOVERRIDDEN");
    msg(MSG::INFO) << "Digversion overridden via joboptions from " 
		      << m_data->digversion << " ('" << m_data->digversionname << "') to " 
		      << m_detectorManager->digitizationVersion()<< " ('" 
		      << m_detectorManager->digitizationVersionName()<<"')" 
		      << endmsg;
  }


  //----------------------Initialize the numerology------------------------//

  for (unsigned int m=0;m<m_data->nBarrelRings;m++) {
    m_detectorManager->getNumerology()->setNBarrelLayers(m, m_data->barrelNumberOfStrawLayersInModule[m]);
  }

  m_detectorManager->getNumerology()->setNBarrelRings(m_data->nBarrelRings);
  //Note: This next line is now consistent with TRT_TestBeamDetDescr.
  m_detectorManager->getNumerology()->setNBarrelPhi(m_data->nBarrelModulesUsed);
  //  m_detectorManager->getNumerology()->setNBarrelPhi(m_data->nBarrelPhi);

  unsigned int nEndcapWheels = 0;
  if (endcapABPlusPresent||endcapABMinusPresent) nEndcapWheels += m_data->endcapNumberOfAWheels + m_data->endcapNumberOfBWheels;
  if (endcapCPlusPresent||endcapCMinusPresent)   nEndcapWheels += m_data->endcapNumberOfCWheels;

  m_detectorManager->getNumerology()->setNEndcapWheels(nEndcapWheels);
  m_detectorManager->getNumerology()->setNEndcapPhi(m_data->nEndcapPhi);

  for (unsigned int w=0;w<m_detectorManager->getNumerology()->getNEndcapWheels();w++) {
    unsigned int nlayers;
    if ( w < m_data->endcapNumberOfAWheels )
      nlayers = m_data->endCapNumberOfStrawLayersPerWheelA;
    else if ( w < ( m_data->endcapNumberOfAWheels + m_data->endcapNumberOfBWheels ) )
      nlayers = m_data->endCapNumberOfStrawLayersPerWheelB;
    else
      nlayers = m_data->endCapNumberOfStrawLayersPerWheelC;
    m_detectorManager->getNumerology()->setNEndcapLayers(w, nlayers ) ;
  }

  //---------------------- Top level volumes ------------------------//

  // This is used by HepVis.
  GeoNameTag * topLevelNameTag = new GeoNameTag("TRT");

  // The top level volumes
  GeoFullPhysVol *pBarrelVol = 0;
  GeoFullPhysVol *pEndCapABPlus = 0;
  GeoFullPhysVol *pEndCapCPlus = 0;
  GeoFullPhysVol *pEndCapABMinus = 0;
  GeoFullPhysVol *pEndCapCMinus = 0;



  //
  // Barrel volume:
  //

  if (barrelPresent) {
    GeoTube* sBarrelVol = new GeoTube( m_data->virtualBarrelInnerRadius,
				       m_data->virtualBarrelOuterRadius,
				       m_data->virtualBarrelVolumeLength );
    //     GeoTube    *sBarrelVol;
    //     if (m_data->includeBarServiceAndFlange) {
    //       sBarrelVol = new GeoTube( m_data->virtualBarrelInnerRadius,
    // 				m_data->virtualBarrelOuterRadius,
    // 				m_data->virtualBarrelVolumeLength );
    //     } else if (!(m_data->includeBarServiceAndFlange)) {
    //       sBarrelVol = new GeoTube( m_data->innerRadiusOfBarrelVolume - m_data->thicknessOfBarrelInnerSupport,
    // 				m_data->outerRadiusOfBarrelVolume,
    // 				m_data->lengthOfBarrelVolume/2.);
    //     }

    GeoLogVol  *lBarrelVol = new GeoLogVol("TRTBarrel", sBarrelVol, m_materialManager->getMaterial("trt::CO2"));
    pBarrelVol = new GeoFullPhysVol(lBarrelVol);

    msg(MSG::DEBUG) << "Virtuel TRT Barrel volume defined by RMin = "<<m_data->virtualBarrelInnerRadius 
		    <<", Rmax = "<<m_data->virtualBarrelOuterRadius<<" Zmax = "<<m_data->virtualBarrelVolumeLength  << endmsg;

    // Common Endcap volumes (one for forward, one for backward):
    //GeoPhysVol *pCommonEndcapVolume[2];

    GeoAlignableTransform * barrelTransform =
      new GeoAlignableTransform(trtTransform * m_data->partTransform(barrelLabel));

    world->add(topLevelNameTag);
    world->add(barrelTransform);
    world->add(pBarrelVol);
    m_detectorManager->addTreeTop(pBarrelVol);
    // Use barrel_ec_id = -1 (+ve and -ve barrel is treated as one alignable object)
    Identifier id = idHelper->barrel_ec_id(-1);
    m_detectorManager->addAlignableTransform(AlignmentLevelTop, id, barrelTransform, pBarrelVol); // global if other selected

  }

  //
  // End-cap volume AB:
  //
  GeoLogVol * lEndCapVolumeAB = 0;
  if (endcapABPlusPresent || endcapABMinusPresent) {
    GeoTube * sEndCapVolumeAB_unshifted = new GeoTube (m_data->innerRadiusOfEndCapVolumeAB,
						       m_data->outerRadiusOfEndCapVolumeAB,
						       m_data->lengthOfEndCapVolumeAB/2.);
    const GeoShape & sEndCapVolumeAB
      = ( *sEndCapVolumeAB_unshifted << HepGeom::TranslateZ3D(m_data->positionOfEndCapVolumeAB));

    lEndCapVolumeAB = new GeoLogVol("TRTEndcapWheelAB", &sEndCapVolumeAB, m_materialManager->getMaterial("trt::CO2"));
  }

  if (endcapABPlusPresent) {
    pEndCapABPlus = new GeoFullPhysVol(lEndCapVolumeAB);

    GeoAlignableTransform * transform =
      new GeoAlignableTransform(trtTransform * m_data->partTransform(endcapA_WheelAB_Label));

    world->add(topLevelNameTag);
    world->add(transform);
    world->add(new GeoIdentifierTag(0));
    world->add(pEndCapABPlus);
    m_detectorManager->addTreeTop(pEndCapABPlus);
    Identifier id = idHelper->barrel_ec_id(2);
    m_detectorManager->addAlignableTransform(AlignmentLevelTop, id, transform, pEndCapABPlus); // global if other selected
  }

  if (endcapABMinusPresent) {
    pEndCapABMinus = new GeoFullPhysVol(lEndCapVolumeAB);

    GeoAlignableTransform * transform =
      new GeoAlignableTransform(trtTransform * m_data->partTransform(endcapC_WheelAB_Label) * HepGeom::RotateY3D(180*CLHEP::deg));

    world->add(topLevelNameTag);
    world->add(transform);
    world->add(new GeoIdentifierTag(1));
    world->add(pEndCapABMinus);
    m_detectorManager->addTreeTop(pEndCapABMinus);
    Identifier id = idHelper->barrel_ec_id(-2);
    m_detectorManager->addAlignableTransform(AlignmentLevelTop, id, transform, pEndCapABMinus); // global if other selected
  }

  //
  // End-cap volume C:
  //
  GeoLogVol * lEndCapVolumeC = 0;
  if (endcapCPlusPresent || endcapCMinusPresent) {
    GeoTube * sEndCapVolumeC_unshifted = new GeoTube (m_data->innerRadiusOfEndCapVolumeC,
						      m_data->outerRadiusOfEndCapVolumeC,
						      m_data->lengthOfEndCapVolumeC/2.);
    const GeoShape & sEndCapVolumeC
      = ( *sEndCapVolumeC_unshifted << HepGeom::TranslateZ3D(m_data->positionOfEndCapVolumeC));

    lEndCapVolumeC = new GeoLogVol("TRTEndcapWheelC", &sEndCapVolumeC, m_materialManager->getMaterial("trt::CO2"));
  }

  if (endcapCPlusPresent) {
    pEndCapCPlus = new GeoFullPhysVol(lEndCapVolumeC);

    GeoAlignableTransform * transform =
      new GeoAlignableTransform(trtTransform * m_data->partTransform(endcapA_WheelC_Label));

    world->add(topLevelNameTag);
    world->add(transform);
    world->add(new GeoIdentifierTag(0));
    world->add(pEndCapCPlus);
    m_detectorManager->addTreeTop(pEndCapCPlus);
    // If we ever have a wheel C, it will need to be identified differently than wheels AB
    // Identifier id = idHelper->barrel_ec_id(2);
    // m_detectorManager->addAlignableTransform(AlignmentLevelTop, id, transform, pEndCapCPlus); // global if other selected
  }

  if (endcapCMinusPresent) {
    pEndCapCMinus = new GeoFullPhysVol(lEndCapVolumeC);

    GeoAlignableTransform * transform =
      new GeoAlignableTransform(trtTransform * m_data->partTransform(endcapC_WheelC_Label) * HepGeom::RotateY3D(180*CLHEP::deg));

    world->add(topLevelNameTag);
    world->add(transform);
    world->add(new GeoIdentifierTag(0));
    world->add(pEndCapCMinus);
    m_detectorManager->addTreeTop(pEndCapCMinus);
    // If we ever have a wheel C, it will need to be identified differently than wheels AB
    // Identifier id = idHelper->barrel_ec_id(-2);
    // m_detectorManager->addAlignableTransform(3, id, transform, pEndCapCMinus); // global if other selected
  }

  // Pointers to the Endcap volumes (index 0: for forward, index 1: for backward):
  GeoFullPhysVol *pCommonEndcapAB[2];
  GeoFullPhysVol *pCommonEndcapC[2];

  pCommonEndcapAB[0] = pEndCapABPlus;
  pCommonEndcapAB[1] = pEndCapABMinus;
  pCommonEndcapC[0] = pEndCapCPlus;
  pCommonEndcapC[1] = pEndCapCMinus;


  //-----------------------------------------------------------------------//
  //                                                                       //
  // Extra Material                                                        //
  //                                                                       //
  //-----------------------------------------------------------------------//
  if (pBarrelVol) {
    InDetDD::ExtraMaterial xMat(m_data->distortedMatManager());
    xMat.add(pBarrelVol, "TRTBarrel");
  } 
  if (pEndCapABPlus) {
    InDetDD::ExtraMaterial xMat(m_data->distortedMatManager());
    xMat.add(pEndCapABPlus, "TRTEndcap");
    xMat.add(pEndCapABPlus, "TRTEndcapA");
  }   
  if (pEndCapABMinus) {
    InDetDD::ExtraMaterial xMat(m_data->distortedMatManager());
    xMat.add(pEndCapABMinus, "TRTEndcap");
    xMat.add(pEndCapABMinus, "TRTEndcapC");
  }   

  // Just for completeness
  if (pEndCapCPlus) {
    InDetDD::ExtraMaterial xMat(m_data->distortedMatManager());
    xMat.add(pEndCapCPlus, "TRTEndcap_WheelC");
    xMat.add(pEndCapCPlus, "TRTEndcapA_WheelC");
  }   
  if (pEndCapCMinus) {
    InDetDD::ExtraMaterial xMat(m_data->distortedMatManager());
    xMat.add(pEndCapCMinus, "TRTEndcap_WheelC");
    xMat.add(pEndCapCMinus, "TRTEndcapC_WheelC");
  }   



  //-----------------------------------------------------------------------//
  //                                                                       //
  // Barrel                                                                //
  //                                                                       //
  //-----------------------------------------------------------------------//


  if (pBarrelVol) {
    //-----------------------------------------------------------------------//
    //                                                                       //
    // Barrel Outer and Inner Supports                                       //
    //                                                                       //
    //-----------------------------------------------------------------------//
    // Barrel inner support:
    GeoTube    *sBarrelInnerSupport = new GeoTube(m_data->innerRadiusOfBarrelVolume,
						  m_data->innerRadiusOfBarrelVolume + m_data->thicknessOfBarrelInnerSupport,
						  m_data->lengthOfBarrelVolume/2);

    //ugly, but necessary check due to changes in database.
    GeoLogVol  *lBarrelInnerSupport = new GeoLogVol("BarrelInnerSupport", sBarrelInnerSupport,
						    m_materialManager->getMaterial("trt::BarrelInnerSupport") ?
						    m_materialManager->getMaterial("trt::BarrelInnerSupport") :
						    m_materialManager->getMaterial("trt::BarrelSupport") );


    GeoPhysVol *pBarrelInnerSupport = new GeoPhysVol(lBarrelInnerSupport);
    pBarrelVol->add(pBarrelInnerSupport);

    // Barrel outer support:
    GeoTube    *sBarrelOuterSupport = new GeoTube(m_data->outerRadiusOfBarrelVolume - m_data->thicknessOfBarrelOuterSupport,
						  m_data->outerRadiusOfBarrelVolume, m_data->lengthOfBarrelVolume/2);

    GeoLogVol  *lBarrelOuterSupport = new GeoLogVol("BarrelOuterSupport", sBarrelOuterSupport,
						    m_materialManager->getMaterial("trt::BarrelOuterSupport") ?
						    m_materialManager->getMaterial("trt::BarrelOuterSupport") :
						    m_materialManager->getMaterial("trt::BarrelSupport") );


    GeoPhysVol *pBarrelOuterSupport = new GeoPhysVol(lBarrelOuterSupport);
    pBarrelVol->add(pBarrelOuterSupport);



    if (m_data->includeBarServiceAndFlange) {

      //-----------------------------------------------------------------------//
      //                                                                       //
      // Barrel End Flange region                                              //
      //                                                                       //
      //-----------------------------------------------------------------------//


      GeoTube      *sEndFlangeRegion = new GeoTube(m_data->barFlangeRMin, m_data->barFlangeRMax,
						   (m_data->barFlangeZMax - m_data->barFlangeZMin)/2);
      GeoLogVol    *lEndFlangeRegion = new GeoLogVol("EndFlangeRegion", sEndFlangeRegion,
						     m_materialManager->getMaterial("trt::EndFlangeRegion") );

      GeoPhysVol *pEndFlangeRegion = new GeoPhysVol(lEndFlangeRegion);

      double zPosEndFlange = (m_data->barFlangeZMin+m_data->barFlangeZMax)/2;
      GeoTransform *xfEndFlangeRegionPlus  = new GeoTransform(HepGeom::TranslateZ3D(zPosEndFlange));
      GeoTransform *xfEndFlangeRegionMinus = new GeoTransform(HepGeom::TranslateZ3D(-zPosEndFlange));

      pBarrelVol->add(xfEndFlangeRegionPlus);
      pBarrelVol->add(pEndFlangeRegion);
      pBarrelVol->add(xfEndFlangeRegionMinus);
      pBarrelVol->add(pEndFlangeRegion);


      //-----------------------------------------------------------------------//
      //                                                                       //
      // Barrel service region                                                 //
      //                                                                       //
      //-----------------------------------------------------------------------//

      GeoTube      *sServices = new GeoTube(m_data->barServicesRMin, m_data->barServicesRMax,
					    (m_data->barServicesZMax - m_data->barServicesZMin)/2);
      GeoLogVol    *lServices = new GeoLogVol("Services", sServices,
					      m_materialManager->getMaterial("trt::Services") );

      GeoPhysVol *pServices = new GeoPhysVol(lServices);

      double zPosServices = (m_data->barServicesZMin+m_data->barServicesZMax)/2;
      GeoTransform *xfServicesPlus = new GeoTransform(HepGeom::TranslateZ3D(zPosServices));
      GeoTransform *xfServicesMinus = new GeoTransform(HepGeom::TranslateZ3D(-zPosServices));

      pBarrelVol->add(xfServicesPlus);
      pBarrelVol->add(pServices);
      pBarrelVol->add(xfServicesMinus);
      pBarrelVol->add(pServices);

    }
  
  

    //-----------------------------------------------------------------------//
    //                                                                       //
    // Barrel Modules                                                        //
    //                                                                       //
    //-----------------------------------------------------------------------//

    std::vector<InDetDD::TRT_BarrelDescriptor *> bDescriptor;

    // Create some shared stuff to stick into each module.

    // The cooling tube:
    GeoTube    *sCoolingTube = new GeoTube(0, m_data->barrelOuterRadiusOfCoolingTube, m_data->lengthOfBarrelVolume/2.0);
    GeoLogVol  *lCoolingTube = new GeoLogVol("CoolingTube",sCoolingTube,m_materialManager->getMaterial("trt::CoolingTube"));
    GeoPhysVol *pCoolingTube = new GeoPhysVol(lCoolingTube);

    GeoTube   *sCoolingFluid = new GeoTube(0, m_data->barrelInnerRadiusOfCoolingTube, m_data->lengthOfBarrelVolume/2.0);
    GeoLogVol *lCoolingFluid = new GeoLogVol("CoolingFluid",sCoolingFluid,m_materialManager->getMaterial("trt::CoolingFluid"));
    GeoPhysVol*pCoolingFluid = new GeoPhysVol(lCoolingFluid);

    pCoolingTube->add(pCoolingFluid);

    double activeGasZPositionNormalStraws, activeGasZPositionStrawsWithLargeDeadRegion;
	//AALONSO, create the Argon Straws
    GeoPhysVol* pHoleForMixedStrawAR                    = nullptr;
    GeoPhysVol* pHoleForMixedStrawWithLargeDeadRegionAR = nullptr;
    if (m_doArgon)
      {
      pHoleForMixedStrawAR                    = makeStraw(activeGasZPositionNormalStraws, false, GM_ARGON);
      pHoleForMixedStrawWithLargeDeadRegionAR = makeStraw(activeGasZPositionStrawsWithLargeDeadRegion,true, GM_ARGON);
      }
    // and krypton straws
    GeoPhysVol* pHoleForMixedStrawKR                    = nullptr;
    GeoPhysVol* pHoleForMixedStrawWithLargeDeadRegionKR = nullptr;
    if (m_doKrypton)
      {
      pHoleForMixedStrawKR                    = makeStraw(activeGasZPositionNormalStraws, false, GM_KRYPTON);
      pHoleForMixedStrawWithLargeDeadRegionKR = makeStraw(activeGasZPositionStrawsWithLargeDeadRegion,true, GM_KRYPTON);
      }

    // The barrel straw (including the "hole" in the radiator around it):
    GeoPhysVol* pHoleForMixedStraw = makeStraw(activeGasZPositionNormalStraws);
    // The straws in the inner layers of module A have a large dead region, and are thus different.
    GeoPhysVol* pHoleForMixedStrawWithLargeDeadRegion = makeStraw(activeGasZPositionStrawsWithLargeDeadRegion,true);
  
    // The modules themselves.
    for (size_t iABC=0;iABC<m_data->nBarrelRings;iABC++) {

      // Create a shape for the modules of each layer (shell)
      // STS: Initialize raditator and shell LogVol
      GeoLogVol  *lRad = 0;
      GeoLogVol  *lShell = 0; 

      // The shell volume:
      std::ostringstream shellstream;
      shellstream << "Shell" << iABC;
      CLHEP::Hep2Vector shellCorner1(m_data->shellCornerXPosition[iABC][0],m_data->shellCornerYPosition[iABC][0]);
      CLHEP::Hep2Vector shellCorner2(m_data->shellCornerXPosition[iABC][1],m_data->shellCornerYPosition[iABC][1]);
      CLHEP::Hep2Vector shellCorner3(m_data->shellCornerXPosition[iABC][2],m_data->shellCornerYPosition[iABC][2]);
      CLHEP::Hep2Vector shellCorner4(m_data->shellCornerXPosition[iABC][3],m_data->shellCornerYPosition[iABC][3]);
      HepGeom::Transform3D shellPosition;
      const GeoShape * sShell = makeModule(m_data->lengthOfBarrelVolume,
					   shellCorner1 > 0 ? shellCorner1 : shellCorner1,
					   shellCorner2 > 0 ? shellCorner2 : shellCorner2,
					   shellCorner3 > 0 ? shellCorner3 : shellCorner3,
				   shellCorner4 > 0 ? shellCorner4 : shellCorner4,
					   shellPosition);

      // STS: We have three different shellmodules and radiators densities. 
      std::ostringstream layerstr;
      layerstr << iABC;

      std::string shellMatName = "trt::ModuleShell"+layerstr.str();
      std::string shellName = "ModuleShell"+layerstr.str();

      const GeoMaterial * shellMat = m_materialManager->getMaterial(shellMatName);
      if (!shellMat) shellMat = m_materialManager->getMaterial("trt::ModuleShell");
      lShell = new GeoLogVol(shellName, sShell, shellMat);

      //---------------------------------------------------------------------------------------------------------------
      // Some shared stuff for all of the modules within a layer:

      // Make a Radiator
      HepGeom::Transform3D radAbsolutePosition;
      const GeoShape * sRad = makeModule(m_data->lengthOfBarrelVolume,
					 shellCorner1 > 0 ? shellCorner1 : shellCorner1,
					 shellCorner2 > 0 ? shellCorner2 : shellCorner2,
					 shellCorner3 > 0 ? shellCorner3 : shellCorner3,
					 shellCorner4 > 0 ? shellCorner4 : shellCorner4,
					 radAbsolutePosition,m_data->barrelThicknessOfModuleWalls);

      // FibreRadiator will have three different densities for each type of module
      std::string radMatName = "trt::FibreRadiator"+layerstr.str();
      std::string radName = "FibreRadiator"+layerstr.str();

      const GeoMaterial * radMat = m_materialManager->getMaterial(radMatName);
      if (!radMat) radMat = m_materialManager->getMaterial("trt::FibreRadiator");

      lRad = new GeoLogVol(radName, sRad, radMat);
      GeoPhysVol *pRad   = nullptr;
      GeoPhysVol *pRadAR = nullptr;
      GeoPhysVol *pRadKR = nullptr;
      pRad = new GeoPhysVol(lRad);
      if (m_doArgon  ) pRadAR = new GeoPhysVol(lRad);
      if (m_doKrypton) pRadKR = new GeoPhysVol(lRad);

      //---------------------------------------------------------------------------------------------------------------
      // Place the cooling tubes in the Radiator

      GeoTransform  *xCool1 = new GeoTransform(shellPosition.inverse()
					       *HepGeom::Translate3D(m_data->barrelXOfCoolingTube[iABC][0],m_data->barrelYOfCoolingTube[iABC][0],0));
      GeoTransform  *xCool2 = new GeoTransform(shellPosition.inverse()
					       *HepGeom::Translate3D(m_data->barrelXOfCoolingTube[iABC][1],m_data->barrelYOfCoolingTube[iABC][1],0));

      pRad->add(xCool1);
      pRad->add(pCoolingTube);
      pRad->add(xCool2);
      pRad->add(pCoolingTube);
      if (m_doArgon)
        {
        pRadAR->add(xCool1);
        pRadAR->add(pCoolingTube);
        pRadAR->add(xCool2);
        pRadAR->add(pCoolingTube);
        }
      if (m_doKrypton)
        {
        pRadKR->add(xCool1);
        pRadKR->add(pCoolingTube);
        pRadKR->add(xCool2);
        pRadKR->add(pCoolingTube);
        }

      //----------------------------------------------------------------------------------------------------------------
      // Parameterize all of the straws and put them within the radiator.

      // Figure out how many straws have a large dead region
      size_t nStrawsWithLargeDeadRegion = 0;
      if (iABC==0) {
	for (size_t iLayer = 0; iLayer<m_data->barrelNumberOfLayersWithLargeDeadRegion; iLayer++) {
	  nStrawsWithLargeDeadRegion += m_data->barrelNumberOfStrawsInStrawLayer[iABC][iLayer];
	}
      }
    
      // Generators:
      HepGeom::TranslateX3D Xx(1.0);
      HepGeom::TranslateY3D Xy(1.0);

      GENFUNCTION  fx = ArrayFunction(&m_data->strawXPosition[iABC][0+nStrawsWithLargeDeadRegion],
				      &m_data->strawXPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      //TK: why ..[0]+n and not ..[n] ?
      GENFUNCTION  fy = ArrayFunction(&m_data->strawYPosition[iABC][0+nStrawsWithLargeDeadRegion],
				      &m_data->strawYPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      TRANSFUNCTION tx1 = Pow(Xx,fx)*Pow(Xy,fy);

      //Functions for straw with large dead regions
      GENFUNCTION  fxDead = ArrayFunction(&m_data->strawXPosition[iABC][0], &m_data->strawXPosition[iABC][0+nStrawsWithLargeDeadRegion]);
      GENFUNCTION  fyDead = ArrayFunction(&m_data->strawYPosition[iABC][0], &m_data->strawYPosition[iABC][0+nStrawsWithLargeDeadRegion]);
      TRANSFUNCTION tx1Dead = Pow(Xx,fxDead)*Pow(Xy,fyDead);

      //TK: Quick fix, might waste a few KB of memory.
      //TK: only use when iABC==0
      GENFUNCTION  fxAll = ArrayFunction(&m_data->strawXPosition[iABC][0], &m_data->strawXPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      GENFUNCTION  fyAll = ArrayFunction(&m_data->strawYPosition[iABC][0], &m_data->strawYPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      TRANSFUNCTION tx1All = Pow(Xx,fxAll)*Pow(Xy,fyAll);


      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //Calculation of needed transforms
      //First get the global and local positions of the two alignment straws:
      //USE HEP2VECTORS!!!

      CLHEP::Hep3Vector Align1Global(m_data->barrelXOfFirstGlobalAlignmentStraw[iABC],  m_data->barrelYOfFirstGlobalAlignmentStraw[iABC], 0);
      CLHEP::Hep3Vector Align2Global(m_data->barrelXOfSecondGlobalAlignmentStraw[iABC], m_data->barrelYOfSecondGlobalAlignmentStraw[iABC],0);
      CLHEP::Hep3Vector Align1Local(m_data->strawXPosition[iABC][0],m_data->strawYPosition[iABC][0],0);
      CLHEP::Hep3Vector Align2Local(m_data->strawXPosition[iABC][m_data->barrelIndexOfSecondGlobalAlignmentStraw[iABC]],
			     m_data->strawYPosition[iABC][m_data->barrelIndexOfSecondGlobalAlignmentStraw[iABC]],0);

      //We need to make first a translation which puts the first alignment straw into place:

      //And we need to make a rotation which puts the second one on its position:

      CLHEP::Hep2Vector local12     = CLHEP::Hep2Vector( Align2Local  - Align1Local  );
      CLHEP::Hep2Vector global12    = CLHEP::Hep2Vector( Align2Global - Align1Global );
      double zrotang = global12.phi()-local12.phi();

      //Here we combine these two into a HepGeom::Transform3D:

      HepGeom::Transform3D absStrawXForm = HepGeom::Translate3D(Align1Global)*HepGeom::RotateZ3D( zrotang )*HepGeom::Translate3D((-Align1Local));
    
      //
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      //Why not use radiator instead of shell?
      TRANSFUNCTION tx2=shellPosition.inverse()*absStrawXForm*tx1;
      TRANSFUNCTION tx2Dead=shellPosition.inverse()*absStrawXForm*tx1Dead;
      TRANSFUNCTION tx2All=shellPosition.inverse()*absStrawXForm*tx1All;
      if (iABC==0) {
	//TK: move rest of ...All stuff here?
	m_detectorManager->setBarrelTransformField(iABC,tx2All.clone());
      } else {
	m_detectorManager->setBarrelTransformField(iABC,tx2.clone());
      }
	//AALONSO
      GeoSerialTransformer *serialTransformer = NULL;
      GeoSerialTransformer *serialTransformerAR = NULL;  //Ruslan
      GeoSerialTransformer *serialTransformerKR = NULL;  //Artem

      serialTransformer     = new GeoSerialTransformer(pHoleForMixedStraw,   &tx2, m_data->barrelNumberOfStrawsInModule[iABC]-nStrawsWithLargeDeadRegion);
      if (m_doArgon)
        serialTransformerAR = new GeoSerialTransformer(pHoleForMixedStrawAR, &tx2, m_data->barrelNumberOfStrawsInModule[iABC]-nStrawsWithLargeDeadRegion);
      if (m_doKrypton)
        serialTransformerKR = new GeoSerialTransformer(pHoleForMixedStrawKR, &tx2, m_data->barrelNumberOfStrawsInModule[iABC]-nStrawsWithLargeDeadRegion);

      //GeoSerialTransformer *serialTransformer=new GeoSerialTransformer(pHoleForMixedStraw, &tx2,
      //							       m_data->barrelNumberOfStrawsInModule[iABC]-nStrawsWithLargeDeadRegion);

      GeoSerialTransformer *serialTransformerDead = NULL;
      GeoSerialTransformer *serialTransformerDeadAR = NULL;
      GeoSerialTransformer *serialTransformerDeadKR = NULL;
      //if (iABC==0) serialTransformerDead = new GeoSerialTransformer(pHoleForMixedStrawWithLargeDeadRegionAR, &tx2Dead,
      //							    nStrawsWithLargeDeadRegion);
      serialTransformerDead     = new GeoSerialTransformer(pHoleForMixedStrawWithLargeDeadRegion  , &tx2Dead,
								      nStrawsWithLargeDeadRegion);
      if (m_doArgon)
        serialTransformerDeadAR = new GeoSerialTransformer(pHoleForMixedStrawWithLargeDeadRegionAR, &tx2Dead,
                      nStrawsWithLargeDeadRegion);
      if (m_doKrypton)
        serialTransformerDeadKR = new GeoSerialTransformer(pHoleForMixedStrawWithLargeDeadRegionKR, &tx2Dead,
                      nStrawsWithLargeDeadRegion);

      //if (iABC==0) serialTransformerDead = new GeoSerialTransformer(pHoleForMixedStrawWithLargeDeadRegion, &tx2Dead,
	//							    nStrawsWithLargeDeadRegion);
     
      
      pRad->add(new GeoSerialIdentifier(0));
      if (m_doArgon  ) pRadAR->add(new GeoSerialIdentifier(0));
      if (m_doKrypton) pRadKR->add(new GeoSerialIdentifier(0));
      if (iABC==0) {
        pRad->add(serialTransformerDead);
        if (m_doArgon  ) pRadAR->add(serialTransformerDeadAR);
        if (m_doKrypton) pRadKR->add(serialTransformerDeadKR);
      }
      pRad->add(serialTransformer);
      if (m_doArgon  ) pRadAR->add(serialTransformerAR);
      if (m_doKrypton) pRadKR->add(serialTransformerKR);

      // Adds one straw from each layer (reformulate..) (should be done via m_data from database)
      double oldx=-999*CLHEP::cm, oldz=-999*CLHEP::cm;
      unsigned int c=0;
      size_t iLayer=0;
      while (c< m_data->barrelNumberOfStrawsInModule[iABC] ) {

	HepGeom::Point3D<double> p;
	if (iABC==0)
	  p = tx2All(c)*HepGeom::Point3D<double>(0,0,0);
	else
	  p = tx2(c)*HepGeom::Point3D<double>(0,0,0);

	double x = p.x();
	double z = p.z();

	//TK: use arrays!! update this...
	if (sqrt((x-oldx)*(x-oldx)+ (z-oldz)*(z-oldz))> 5*CLHEP::cm) {
	  iLayer++;
	  bDescriptor.push_back(new InDetDD::TRT_BarrelDescriptor());
	  bDescriptor.back()->setStrawTransformField(m_detectorManager->barrelTransformField(iABC),c);

	  //TK: Next, we are providing information about the Z
	  //dimensions of the active gas, to be used for reconstruction
	  //purposes. Personally I find "strawZDead" to be a slightly
	  //confusing choice of name for that method.

	  if((iABC==0)&&(iLayer<=m_data->barrelNumberOfLayersWithLargeDeadRegion )) {
	    //TK: these things should come back from makestraw...
	    double lengthOfActiveGas=
	      (m_data->barrelLengthOfStraw-m_data->barrelLengthOfTwister)/2.0 - m_data->lengthOfDeadRegion-m_data->barrelLengthOfLargeDeadRegion;
	    double startZOfActiveGas=activeGasZPositionStrawsWithLargeDeadRegion-lengthOfActiveGas/2.0;
	    bDescriptor.back()->strawZPos(activeGasZPositionStrawsWithLargeDeadRegion);
	    bDescriptor.back()->strawZDead(startZOfActiveGas);
	    bDescriptor.back()->strawLength(lengthOfActiveGas);
	    //std::cout << "TK inner [mm] ZPos = "<< bDescriptor.back()->strawZPos()/mm << ", ZDead = " <<
	    //  bDescriptor.back()->strawZDead()/mm << ", Length = "<<bDescriptor.back()->strawLength()/mm << std::endl;
	  } else {
	    double lengthOfActiveGas=(m_data->barrelLengthOfStraw-m_data->barrelLengthOfTwister)/2.0 - 2*m_data->lengthOfDeadRegion;
	    double startZOfActiveGas=activeGasZPositionNormalStraws-lengthOfActiveGas/2.0;
	    bDescriptor.back()->strawZPos(activeGasZPositionNormalStraws);
	    bDescriptor.back()->strawZDead(startZOfActiveGas);
	    bDescriptor.back()->strawLength(lengthOfActiveGas);
	    //std::cout << "TK outer [mm] ZPos = "<< bDescriptor.back()->strawZPos()/mm << ", ZDead = " <<
	    // bDescriptor.back()->strawZDead()/mm << ", Length = "<<bDescriptor.back()->strawLength()/mm << std::endl;
	  }

	}
	bDescriptor.back()->addStraw(z,x);
	oldx=x; oldz=z;
	c++;

      }



      // Now create m_data->nBarrelModulesUsed unique modules within each layer.
      pBarrelVol->add(new GeoSerialIdentifier(0));
      for (size_t iMod = 0; iMod<m_data->nBarrelModulesUsed;iMod++) {
	double delta = iMod*360*CLHEP::deg/m_data->nBarrelModules;

         

      
	GeoFullPhysVol * pShell = new GeoFullPhysVol(lShell);

	// This is where the shell is pushed out to its place
	//GeoTransform * xfx1 = new GeoTransform(HepGeom::RotateZ3D(delta)*shellPosition);
	GeoAlignableTransform * xfx1 = new GeoAlignableTransform(HepGeom::RotateZ3D(delta)*shellPosition);
	pBarrelVol->add(xfx1);
	pBarrelVol->add(pShell);

	// Register the alignable transfrom to the manager
	// +ve and -ve are part of the same barrel. We use barrel_ec = -1.
	Identifier idModule = idHelper->module_id(-1, iMod, iABC);
	// In barrel frame (generally the same as the global frame)
	m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfx1, pShell, pBarrelVol);

  Identifier TRT_Identifier;
	// Add the substructure here:
	pShell->add(new GeoIdentifierTag(iABC));
  TRT_Identifier = idHelper->straw_id(1, iMod, iABC, 1, 1);
  int strawStatusHT = TRTCond::StrawStatus::Good;
  if (m_strawsvcavailable && (m_doArgon || m_doKrypton)) strawStatusHT = m_sumSvc->getStatusHT(TRT_Identifier);
  ActiveGasMixture agm = DecideGasMixture(strawStatusHT);

  // Ruslan: insert radiators with Ar-straws
  // Artem: same for Kr
  switch (agm)
    {
    case GM_ARGON:
      msg(MSG::DEBUG) << "Marking Argon straws from /TRT/Cond/StatusHT:\t"
                      << idHelper->print_to_string(TRT_Identifier) << endmsg; 
      pShell->add(pRadAR);
      break;
    case GM_KRYPTON:
      msg(MSG::DEBUG) << "Marking Krypton straws from /TRT/Cond/StatusHT:\t"
                      << idHelper->print_to_string(TRT_Identifier) << endmsg;
      pShell->add(pRadKR);
      break;
    case GM_XENON:
      msg(MSG::DEBUG) << "Marking Xenon straws from /TRT/Cond/StatusHT:\t"
                      << idHelper->print_to_string(TRT_Identifier) << endmsg;
      pShell->add(pRad);
      break;
    default:
      msg(MSG::FATAL) << "Unexpected gas mixture: " << agm << endmsg; 
      throw std::runtime_error("Unexpected gas mixture");
      return;
    }

	//-------------------------------------------------------------------//
	//                                                                   //
	// Barrel readout:                                                   //
	//                                                                   //
	//-------------------------------------------------------------------//

	//
	// Get the number of straw layers in each module:
	//

	unsigned int nStrawLayers = m_detectorManager->getNumerology()->getNBarrelLayers(iABC);
	for (unsigned int iStrawLayer=0;iStrawLayer<nStrawLayers; iStrawLayer++) { // limit stored as float!

	  unsigned int jStrawLayer=iStrawLayer;
	  if (iABC>0) jStrawLayer += m_detectorManager->getNumerology()->getNBarrelLayers(0);
	  if (iABC>1) jStrawLayer += m_detectorManager->getNumerology()->getNBarrelLayers(1);
	  //TK: just go from jStrawLayer=layerstart;jStrawLayer<layerend ?

	  InDetDD::TRT_BarrelDescriptor *bD=bDescriptor[jStrawLayer];

	  InDetDD::TRT_BarrelElement *element0 = new InDetDD::TRT_BarrelElement(pShell, bD, 0  , iABC, iMod, iStrawLayer, idHelper, conditions);
	  InDetDD::TRT_BarrelElement *element1 = new InDetDD::TRT_BarrelElement(pShell, bD, 1  , iABC, iMod, iStrawLayer, idHelper, conditions);

	  m_detectorManager->manageBarrelElement(element0);
	  m_detectorManager->manageBarrelElement(element1);
	}

      }//End "for (size_t iMod = ..." loop.

    }

    // Set up the nearest neighbor pointers: in R.
    for (unsigned int e=0;e<2;e++) {
      for  (unsigned int iMod=0;iMod<m_data->nBarrelModulesUsed; iMod++) {
	InDetDD::TRT_BarrelElement *prev=NULL;
	for (unsigned int iABC=0;iABC<m_data->nBarrelRings;iABC++) {
	  for (unsigned int s=0;s<m_detectorManager->getNumerology()->getNBarrelLayers(iABC); s++) {
	    InDetDD::TRT_BarrelElement *current = m_detectorManager->getBarrelElement(e,iABC, iMod, s);
	    if (prev && current) {
	      prev->setNextInR(current);
	      current->setPreviousInR(prev);
	    }
	    prev=current;
	  }
	}
      }
    }

    // Set up the nearest neighbor pointers: in Phi.
    for (unsigned int e=0;e<2;e++) {
      for (unsigned int iABC=0;iABC<m_data->nBarrelRings;iABC++) {
	for (unsigned int s=0;s<m_detectorManager->getNumerology()->getNBarrelLayers(iABC); s++) {
	  InDetDD::TRT_BarrelElement *prev=NULL;
	  for  (unsigned int iMod=0;iMod<m_data->nBarrelModulesUsed; iMod++) {
	    InDetDD::TRT_BarrelElement *current = m_detectorManager->getBarrelElement(e,iABC, iMod, s);
	    if (prev && current) {
	      prev->setNextInPhi(current);
	      current->setPreviousInPhi(prev);
	    }
	    prev=current;
	  }
	  if (m_data->nBarrelModulesUsed==m_data->nBarrelModules) { // Full complement; then, we wrap!:
	    InDetDD::TRT_BarrelElement *first=m_detectorManager->getBarrelElement(e,iABC,0,s);
	    InDetDD::TRT_BarrelElement *last =m_detectorManager->getBarrelElement(e,iABC,m_data->nBarrelModules-1,s);
	    if (first && last) {
	      first->setPreviousInPhi(last);
	      last->setNextInPhi(first);
	    }
	  }
	}
      }
    }
  }//end of if (pBarrelVol)

  //       {  //Final straw positions test
  //       const int side = 0;
  //       const unsigned int imod  = 0;
  //       for  (unsigned int iRing = 0; iRing < m_data->nBarrelRings ; iRing++) {
  //       unsigned int absStrawNum = 0;
  //       for (unsigned int iLayerInMod = 0; iLayerInMod < m_detectorManager->getNumerology()->getNBarrelLayers(iRing); iLayerInMod++ ) {
  //       InDetDD::TRT_BarrelElement * elem = m_detectorManager->getBarrelElement(side,iRing,imod,iLayerInMod);
  //       //      int nstrawsafterlayer=absStrawNum+elem->nStraws();
  //       for (unsigned int iStraw = 0 ; iStraw <  elem->nStraws(); iStraw++) {
  //       //      int newabsnum = static_cast<int>(absStrawNum)-2*static_cast<int>(iStraw)+static_cast<int>((elem->nStraws())) -1;
  //       // 	    int newabsnum = absStrawNum-2*iStraw+elem->nStraws() -1;
  //       //  	    std::cout << "TKstrawpos: Ring = "<<iRing<< ", layer = " <<iLayerInMod << ", straw = " << iStraw
  //       // 		      << " (absstraw =" << absStrawNum << ", newabsstrawnum = "<< newabsnum<<") => (x,y) = "
  //       //  		      << elem->strawCenter(iStraw).x()<< ", " << elem->strawCenter(iStraw).y() << ")" << std::endl;
  //       std::cout << " strawXPosition["<<iRing<<"]["<<absStrawNum<<"]="<< elem->strawCenter(iStraw).x() << ";"<<std::endl;
  //       std::cout << " strawYPosition["<<iRing<<"]["<<absStrawNum<<"]="<< elem->strawCenter(iStraw).y() << ";"<<std::endl;
  //       //  	    std::cout << " newstrawXPosition["<<iRing<<"]["<<newabsnum<<"]="<< elem->strawCenter(iStraw).x() << ";"<<std::endl;
  //       //  	    std::cout << " newstrawYPosition["<<iRing<<"]["<<newabsnum<<"]="<< elem->strawCenter(iStraw).y() << ";"<<std::endl;

  //       absStrawNum++;
  //       }
  //       }
  //       }
  //       }

  //-----------------------------------------------------------------------//
  //                                                                       //
  // Endcap Modules                                                        //
  //                                                                       //
  //-----------------------------------------------------------------------//
  
  // TK: This part could really use some cleanup and reordering.
  //     There is no need to repeat the same code for A, B & C endcaps.


  //if (m_data->isCosmicRun) return;
  // if none of the endcaps is being built we can return.
  if (!(endcapABPlusPresent || endcapABMinusPresent || endcapCPlusPresent || endcapCMinusPresent))  return;

  unsigned int firstIndexOfA = 0;
  unsigned int firstIndexOfB = m_data->endcapNumberOfAWheels;
  unsigned int firstIndexOfC = m_data->endcapNumberOfAWheels + m_data->endcapNumberOfBWheels;

  unsigned int indexUpperBound = firstIndexOfA + m_detectorManager->getNumerology()->getNEndcapWheels();

  if (m_data->initialLayout) indexUpperBound = firstIndexOfC; // No wheel C.

  const unsigned int nSides = 2;
  const unsigned int nStrawLayMaxEc = 8;//hardcoded...

  unsigned int iiSide, iiWheel, iiPlane, iiPhi, counter;    //set of counters
  int sign;
  double zdelta = 0.024; // try to make smaller gaps for Endcap Inner/OuterSupportGapper
  GeoTransform *xfRadiator, *xfPlane, *xfHeatExchanger, *xfFaradayFoilFront, *xfFaradayFoilBack;
  GeoTransform *xfInnerSupportGapperA,*xfOuterSupportGapperA, *xfInnerSupportGapperB, *xfOuterSupportGapperB; 
  GeoFullPhysVol *childPlane = 0;


  double RotationsOfStrawPlanes[nStrawLayMaxEc]; //8 is hardcoded
  double shiftForEachRotation = m_data->endCapShiftForEachRotation; // in units of deltaPhi
  RotationsOfStrawPlanes[0] = 0.;

  bool oldGeometry = true;
  // Temporary way to determine old from new
  if (shiftForEachRotation < 0) oldGeometry = false;

  if (oldGeometry) {
    // For old geometry
    for (counter = 1; counter < nStrawLayMaxEc; counter++)
      {
	RotationsOfStrawPlanes[counter] = RotationsOfStrawPlanes[counter-1] + shiftForEachRotation;
	if (RotationsOfStrawPlanes[counter] >= 1.)
	  RotationsOfStrawPlanes[counter] -= 1.;
      }
  } else {
    // New geometry
    double RotationsOfStrawPlanesTmp[nStrawLayMaxEc] = {0,0,0,0,2,2,2,2}; 
    for (counter = 0; counter < nStrawLayMaxEc; counter++)
      {
	RotationsOfStrawPlanes[counter] = (counter * shiftForEachRotation) +  RotationsOfStrawPlanesTmp[counter];
      }
  }

  // Create and initialize by 0 arrays of descriptors
  InDetDD::TRT_EndcapDescriptor* descriptorsAB[nSides][nStrawLayMaxEc][m_data->nEndcapPhi];
  InDetDD::TRT_EndcapDescriptor* descriptorsC[nSides][nStrawLayMaxEc][m_data->nEndcapPhi];
  InDetDD::TRT_EndcapDescriptor* pDescriptor = 0;
  InDetDD::TRT_EndcapElement* element = 0;

  for(iiSide = 0; iiSide<nSides; iiSide++)
    for(iiPlane = 0; iiPlane < nStrawLayMaxEc; iiPlane++)
      for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
	{
	  descriptorsAB[iiSide][iiPlane][iiPhi] = 0;
	  descriptorsC[iiSide][iiPlane][iiPhi] = 0;
	}



  // Do Wheels A and B if one of them is present
  if (endcapABPlusPresent || endcapABMinusPresent) {
    // --------------   Wheel A  -----------------------

    // Inner/Outer supports
    GeoTube* sInnerSupportA = new GeoTube(m_data->endCapInnerRadiusOfSupportA,
					  m_data->endCapInnerRadiusOfSupportA + m_data->endCapRadialThicknessOfInnerSupportA,
					  m_data->endCapLengthOfWheelsA/2);
    GeoLogVol* lInnerSupportA = new GeoLogVol("InnerSupportA", sInnerSupportA, m_materialManager->getMaterial("trt::InnerSupportA"));
    GeoPhysVol* pInnerSupportA = new GeoPhysVol(lInnerSupportA);

    GeoTube* sOuterSupportA = new GeoTube(m_data->endCapOuterRadiusOfSupportA - m_data->endCapRadialThicknessOfOuterSupportA,
					  m_data->endCapOuterRadiusOfSupportA, m_data->endCapLengthOfWheelsA/2);
    GeoLogVol* lOuterSupportA = new GeoLogVol("OuterSupportA", sOuterSupportA, m_materialManager->getMaterial("trt::OuterSupportA"));
    GeoPhysVol* pOuterSupportA = new GeoPhysVol(lOuterSupportA);

    // Straw plane
    GeoFullPhysVol* pStrawPlaneA_Kr 	= nullptr;
    GeoFullPhysVol* pStrawPlaneA_Ar 	= nullptr;
    if (m_doKrypton)
      pStrawPlaneA_Kr	= makeStrawPlane(firstIndexOfA, GM_KRYPTON);
    if (m_doArgon)
      pStrawPlaneA_Ar = makeStrawPlane(firstIndexOfA, GM_ARGON);
    GeoFullPhysVol* pStrawPlaneA      = makeStrawPlane(firstIndexOfA);
    // pStrawPlaneA->ref();

    //TK:
    // Instead of this confusing stuf (main, thin, middle??), make:
    // 1) An array which gives the exact thicknesses of the various radiators
    // 2) A "makeradiator" method like makestrawplane which checks
    //    internally whether it already has created a radiator of a
    //    given thickness.
    //
    // Then just loop over the radiators
    //

    // Radiators
    GeoTube* sMainRadiatorA = new GeoTube(m_data->endCapInnerRadiusOfSupportA + m_data->endCapRadialThicknessOfInnerSupportA,
					  m_data->endCapOuterRadiusOfSupportA - m_data->endCapRadialThicknessOfOuterSupportA
					  - m_data->endCapRadialDistFromRadToOuterSupportA, m_data->endCapMainRadiatorThicknessA/2);
    GeoLogVol* lMainRadiatorA = new GeoLogVol("MainRadiatorA",sMainRadiatorA,  m_materialManager->getMaterial("trt::FoilRadiatorAC"));
    GeoPhysVol* pMainRadiatorA = new GeoPhysVol(lMainRadiatorA);

    GeoTube* sThinRadiatorA = new GeoTube(m_data->endCapInnerRadiusOfSupportA + m_data->endCapRadialThicknessOfInnerSupportA,
					  m_data->endCapOuterRadiusOfSupportA - m_data->endCapRadialThicknessOfOuterSupportA
					  - m_data->endCapRadialDistFromRadToOuterSupportA, m_data->endCapThinRadiatorThicknessA/2);
    GeoLogVol* lThinRadiatorA = new GeoLogVol("ThinRadiatorA",sThinRadiatorA,  m_materialManager->getMaterial("trt::FoilRadiatorAC"));
    GeoPhysVol* pThinRadiatorA = new GeoPhysVol(lThinRadiatorA);

    // Wheel
    GeoTube* sWheelA  = new GeoTube( m_data->endCapInnerRadiusOfSupportA,m_data->endCapOuterRadiusOfSupportA, m_data->endCapLengthOfWheelsA/2);
    GeoLogVol* lWheelA  = new GeoLogVol("WheelA", sWheelA,  m_materialManager->getMaterial("trt::CO2"));

    // This is the straw pitch.
    double deltaPhiForStrawsA = 360.*CLHEP::deg/m_data->endcapNumberOfStrawsInStrawLayer_AWheels;


    // In reality the positive and negative endcaps are built identical, both in 
    // geometry and readout. The offline numbering however keeps phi numbering going 
    // in the same direction as global phi (righthanded direction). 
    
    // For the latest version we build +ve and negative endcaps identical.
    // We also build the descriptors identical apart from the setting of startphi.
    //
    // The mapping is fixed (this must be reproduced in the sensitive
    // detector and readout geometry) The mapping is 1-1 for the 
    // +ve endcap, for the -ve endcap it is as follows:
    //
    //   ***************************************************************
    //   *  Negative endcap (Endcap C) mapping.                        *
    //   *                                                             *
    //   *  nSectors = 32                                              *
    //   *  nStraws = num straws in sector                             *
    //   *  sector -> (nSectors + nSectors/2 - sector - 1) % nSectors  *
    //   *  straw  -> nStraws - 1 - straw                              *  
    //   ***************************************************************
    //
    // For compatibility with old (wrong geometry) we rotate the strawlayers 
    // differently for the negative endcap than we do for the positive endcap.
    // This is to allow the sensitive detector and readout geometry to have
    // the same code for both layouts. 
    //
    // Below we refere to online as the physical readout and offline as the offline
    // identifier convetions.
    // iiPhi corresponds to the "online" readout phi sector. This goes
    // right handed in positive endcap and left handed in negative, where handedness 
    // is wrt to global frame.
    // iiPhiOffline is the offline numbering which is always right handed.

    for(iiSide=0; iiSide<nSides; iiSide++) {
      // Wheel A
      if (pCommonEndcapAB[iiSide]) {

	double WheelPlacerA = m_data->endCapPositionOfFirstWheelA[iiSide];

	for(iiWheel=firstIndexOfA; iiWheel < firstIndexOfB; iiWheel++)
	  {
	    //prepair to place wheel
	    WheelPlacerA += m_data->endCapDistanceBetweenWheelCentersA[iiSide][iiWheel] ;

	    GeoFullPhysVol* pWheelA = new GeoFullPhysVol(lWheelA);

	    GeoAlignableTransform * xfAlignableModule = 0;

	    // Place planes in the wheel
	    for (iiPlane = 0; iiPlane < m_data->endCapNumberOfStrawLayersPerWheelA; iiPlane++)
	      {


    
		// WheelA is subdivided into 4 alignable objects. (Every 4th straw layer)
		// We create an alignable transform for each alignable module 
		// and multiply this by the transform for every straw layer in the "alignable module" 
		// The tranform is by default Identity.
		if (iiPlane % 4 == 0) {
		  // Register alignable node
		  int barrel_ec = (iiSide) ? -2 : +2;
		  xfAlignableModule = new GeoAlignableTransform(HepGeom::Transform3D());
		  Identifier idSubModule = idHelper->layer_id(barrel_ec, 0, iiWheel, iiPlane); 
		  // We pass the parent volume as the local delta for this correction is the same as a local delta
		  // on the transformation of the wheel.
		  m_detectorManager->addAlignableTransform(AlignmentLevelSubWheel, idSubModule, xfAlignableModule, pWheelA); 	    
		}

		// phiPlane is phi of straw 0, sector 0 (online numbering)
		double phiPlane = m_data->endCapPhiOfFirstStraw + RotationsOfStrawPlanes[iiPlane%nStrawLayMaxEc]*deltaPhiForStrawsA;

		// For compatibility with old geometry we have to shift every eighth wheel by 1 straw pitch.
		if(iiSide && oldGeometry && (iiPlane%8 == 0)) {
		  phiPlane +=  deltaPhiForStrawsA;
		}

    Identifier TRT_Identifier;
    int bar_ec = (iiSide) ? -2 : +2;
    TRT_Identifier = idHelper->straw_id(bar_ec, 1, iiWheel, 1, 1);
    int strawStatusHT = TRTCond::StrawStatus::Good;
    if (m_strawsvcavailable && (m_doArgon || m_doKrypton)) strawStatusHT = m_sumSvc->getStatusHT(TRT_Identifier);
    ActiveGasMixture agm = DecideGasMixture(strawStatusHT);

    // Ruslan: insert plane with Ar-straws
    // Artem: same for Kr
    switch (agm)
      {
      case GM_ARGON:
        msg(MSG::DEBUG) << "Marking Argon straws from /TRT/Cond/StatusHT:\t"
                        << idHelper->print_to_string(TRT_Identifier) << endmsg;
        childPlane = pStrawPlaneA_Ar->clone(); 
        break;
      case GM_KRYPTON:
        msg(MSG::DEBUG) << "Marking Krypton straws from /TRT/Cond/StatusHT:\t"
                        << idHelper->print_to_string(TRT_Identifier) << endmsg;
        childPlane = pStrawPlaneA_Kr->clone();
        break;
      case GM_XENON:
        msg(MSG::DEBUG) << "Marking Xenon straws from /TRT/Cond/StatusHT:\t"
                        << idHelper->print_to_string(TRT_Identifier) << endmsg;
        childPlane = pStrawPlaneA->clone();
        break;
      default:
        msg(MSG::FATAL) << "Unexpected gas mixture: " << agm << endmsg; 
        throw std::runtime_error("Unexpected gas mixture");
        return;
      }


		xfPlane = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionA[iiPlane] - m_data->endCapLengthOfWheelsA/2)*HepGeom::RotateZ3D(phiPlane));

		if (xfAlignableModule) pWheelA->add(xfAlignableModule);
		pWheelA->add(xfPlane);
		pWheelA->add(new GeoIdentifierTag(iiPlane));
		pWheelA->add(childPlane);

		// Create descriptors
		// Just do it for the first wheel
		if(iiWheel==firstIndexOfA && iiPlane < nStrawLayMaxEc)
		  for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		    {
		
		      pDescriptor = new InDetDD::TRT_EndcapDescriptor();

		      pDescriptor->nStraws() = m_data->endcapNumberOfStrawsInStrawLayer_AWheels/m_data->nEndcapPhi;
		      pDescriptor->strawPitch() = deltaPhiForStrawsA;

		      double startPhi = phiPlane + iiPhi * pDescriptor->strawPitch() * pDescriptor->nStraws();

		      // For negative endcap the startPhi is the last straw in the physical sector
		      //   phi -> phi + strawPitch*(n-1)
		      // it then gets rotated 180 around y axis 
		      //   phi -> pi - phi
		      if (iiSide) {
			startPhi = CLHEP::pi - (startPhi + pDescriptor->strawPitch() * (pDescriptor->nStraws() - 1));
		      }
		      
		      // Make sure its between -pi and pi.
		      if (startPhi <= -CLHEP::pi) startPhi += 2*CLHEP::pi;
		      if (startPhi > CLHEP::pi) startPhi -= 2*CLHEP::pi;
		      
		      pDescriptor->startPhi() = startPhi;
		      
		      pDescriptor->strawLength() = m_data->endCapOuterRadiusOfSupportA - m_data->endCapRadialThicknessOfOuterSupportA
			- 2*m_data->lengthOfDeadRegion - m_data->endCapRadialThicknessOfInnerSupportA - m_data->endCapInnerRadiusOfSupportA;
		      pDescriptor->innerRadius() = m_data->endCapInnerRadiusOfSupportA + m_data->endCapRadialThicknessOfInnerSupportA
			+ m_data->lengthOfDeadRegion;
		      pDescriptor->setStrawTransformField(m_detectorManager->endcapTransformField(0),iiPhi*pDescriptor->nStraws());

		      //		      if(iiPlane > nStrawLayMaxEc)
		      //	std::cerr << "\nTRTDetectorFactory_Full ERROR: Cannot place descriptor in the array\n"
		      //		  << "Number of layers got from database exceeds nStrawLayMaxEc constant!\n";

		      descriptorsAB[iiSide][iiPlane%nStrawLayMaxEc][iiPhi] = pDescriptor;
		    }
		// Create elements
		for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		  {
		    // m_data->nEndcapPhi assumed to be even.
		    // For positive endcap online == offline. For negative endcap we rotate 180 deg about y axis so 
		    // sector 0 -> 15, 15 -> 0, 16 -> 31, 31 -> 16, etc. This is achieved with 
		    // sector -> (nSectors + nSectors/2 - sector - 1) % nSectors
		    int iiPhiOffline = (iiSide==0) ? iiPhi :  (3*m_data->nEndcapPhi/2 - iiPhi - 1)% m_data->nEndcapPhi;
		    element = new InDetDD::TRT_EndcapElement(childPlane,
							     descriptorsAB[iiSide][iiPlane%nStrawLayMaxEc][iiPhi],
							     iiSide==0,
							     iiWheel,
							     iiPlane,
							     iiPhiOffline,
							     idHelper,
							     conditions);
		    m_detectorManager->manageEndcapElement(element);
		  }
	      }
	    // Place radiators in the wheel
	    for (counter = 1; counter <= m_data->endCapNumberOfStrawLayersPerWheelA; counter++)
	      {
		if (counter % 4 == 1)
		  {
		    xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionA[counter-1] - m_data->endCapLengthOfWheelsA/2
								  - m_data->outerRadiusOfStraw - m_data->endCapThinRadiatorThicknessA/2));
		    pWheelA->add(xfRadiator);
		    pWheelA->add(pThinRadiatorA);
		  }

		if (counter % 4 == 0)
		  {
		    xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionA[counter-1] - m_data->endCapLengthOfWheelsA/2
								  + m_data->outerRadiusOfStraw  + m_data->endCapThinRadiatorThicknessA/2));
		    pWheelA->add(xfRadiator);
		    pWheelA->add(pThinRadiatorA);
		    continue;
		  }

		xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionA[counter-1] - m_data->endCapLengthOfWheelsA/2
							      + m_data->outerRadiusOfStraw  + m_data->endCapMainRadiatorThicknessA/2));
		pWheelA->add(xfRadiator);
		pWheelA->add(pMainRadiatorA);
	      }
	    // Place Inner/Outer supports in the wheel
	    pWheelA->add(pInnerSupportA);
	    pWheelA->add(pOuterSupportA);
	  
	    // Place wheel in the Endcap Volume
	    GeoAlignableTransform * xfWheel = new GeoAlignableTransform( HepGeom::TranslateZ3D(WheelPlacerA) );

	    pCommonEndcapAB[iiSide]->add(xfWheel);
	    pCommonEndcapAB[iiSide]->add(new GeoIdentifierTag(iiWheel));
	    pCommonEndcapAB[iiSide]->add(pWheelA);

	    // Register alignable node
	    int barrel_ec = (iiSide) ? -2 : +2;
	    Identifier idModule = idHelper->module_id(barrel_ec, 0, iiWheel);
	    m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfWheel, pWheelA); 	    
	  
	    if (m_data->includeECFoilHeatExchangerAndMembranes) {

	      // Faraday Foils added 
	      // same Faraday foils for both wheel A and B
              // KaptonFoil radius is between R_max of InnerSupport and R_min of OuterSupport....STS 
	      GeoTube* sFaradayFoilWheelAB = new GeoTube(m_data->endCapInnerRadiusOfSupportA + m_data->endCapRadialThicknessOfInnerSupportA, 
							 m_data->endCapOuterRadiusOfSupportA - m_data->endCapRadialThicknessOfOuterSupportA,
							 m_data->endCapFaradayFoilThickness/2);
	      GeoLogVol* lFaradayFoilWheelAB = new GeoLogVol("FaradayKaptonFoil",sFaradayFoilWheelAB, m_materialManager->getMaterial("trt::FaradayFoilMaterial"));
	      GeoPhysVol* pFaradayFoilWheelAB = new GeoPhysVol(lFaradayFoilWheelAB);
	      
	      // Heat Exchanger added
	      GeoTube* sHeatExchangerA = new GeoTube(m_data->endCapRMinOfHeatExchanger,m_data->endCapRMaxOfHeatExchanger,m_data->endCapHeatExchangerThicknessA/2);
	      GeoLogVol* lHeatExchangerA = new GeoLogVol("HeatExchangerA",sHeatExchangerA, m_materialManager->getMaterial("trt::HeatExchangerAMat"));
	      GeoPhysVol* pHeatExchangerA = new GeoPhysVol(lHeatExchangerA);
	      pHeatExchangerA->ref(); // So we can unref later since doesn't get used for every wheel

	      // Inner/Outer Support Gapper added
	      GeoTube* sInnerSupportGapperA = new GeoTube(m_data->endCapInnerRadiusOfSupportA,
					                  m_data->endCapInnerRadiusOfSupportA + m_data->endCapRadialThicknessOfInnerSupportA,
                                                          m_data->endCapHeatExchangerThicknessA/2 + m_data->endCapFaradayFoilThickness/2 + zdelta);
	      GeoLogVol* lInnerSupportGapperA = new GeoLogVol("InnerSupportGapperA", sInnerSupportGapperA, m_materialManager->getMaterial("trt::InnerSupportA"));
	      GeoPhysVol* pInnerSupportGapperA = new GeoPhysVol(lInnerSupportGapperA);
	      pInnerSupportGapperA->ref(); // So we can unref later since doesn't get used for every wheel

	      GeoTube* sOuterSupportGapperA = new GeoTube(m_data->endCapOuterRadiusOfSupportA - m_data->endCapRadialThicknessOfOuterSupportA,
					                  m_data->endCapOuterRadiusOfSupportA, 
							  m_data->endCapHeatExchangerThicknessA/2 + m_data->endCapFaradayFoilThickness/2 + zdelta);
	      GeoLogVol* lOuterSupportGapperA = new GeoLogVol("OuterSupportGapperA", sOuterSupportGapperA, m_materialManager->getMaterial("trt::OuterSupportA"));
	      GeoPhysVol* pOuterSupportGapperA = new GeoPhysVol(lOuterSupportGapperA);
              pOuterSupportGapperA->ref(); // So we can unref later since doesn't get used for every wheel

	      if(iiWheel<=firstIndexOfB-1) 
		{ 
		  xfFaradayFoilFront = new GeoTransform(HepGeom::TranslateZ3D(WheelPlacerA 
									- m_data->endCapLengthOfWheelsA/2
									- m_data->endCapFaradayFoilThickness/2.0));
		  xfFaradayFoilBack = new GeoTransform(HepGeom::TranslateZ3D(WheelPlacerA 
								       + m_data->endCapLengthOfWheelsA/2
								       + m_data->endCapFaradayFoilThickness/2.0));
		  pCommonEndcapAB[iiSide]->add(xfFaradayFoilFront);
		  pCommonEndcapAB[iiSide]->add(pFaradayFoilWheelAB);
		  pCommonEndcapAB[iiSide]->add(xfFaradayFoilBack);
		  pCommonEndcapAB[iiSide]->add(pFaradayFoilWheelAB);
		}
	      // Place HeatExchanger after putiing wheel only. No heat exchanger after the last wheel.

              // Ditto for Inner/OuterSupportGapper 
	      if(iiWheel<firstIndexOfB-1) 
		{
		  xfHeatExchanger = new GeoTransform(HepGeom::TranslateZ3D( WheelPlacerA
								      + m_data->endCapLengthOfWheelsA/2
								      + m_data->endCapFaradayFoilThickness
								      + m_data->endCapHeatExchangerThicknessA/2));
		  pCommonEndcapAB[iiSide]->add(xfHeatExchanger);
		  pCommonEndcapAB[iiSide]->add(pHeatExchangerA);

		  xfInnerSupportGapperA = new GeoTransform(HepGeom::TranslateZ3D( WheelPlacerA
								      + m_data->endCapLengthOfWheelsA/2
								      + m_data->endCapFaradayFoilThickness
								      + m_data->endCapHeatExchangerThicknessA/2));
		  xfOuterSupportGapperA = new GeoTransform(HepGeom::TranslateZ3D( WheelPlacerA
								      + m_data->endCapLengthOfWheelsA/2
								      + m_data->endCapFaradayFoilThickness
								      + m_data->endCapHeatExchangerThicknessA/2));
		  pCommonEndcapAB[iiSide]->add(xfInnerSupportGapperA);
		  pCommonEndcapAB[iiSide]->add(pInnerSupportGapperA);
		  pCommonEndcapAB[iiSide]->add(xfOuterSupportGapperA);
		  pCommonEndcapAB[iiSide]->add(pOuterSupportGapperA);
		}
	      // Unref a few volumes. This will delete them if they did not get added.
	      pHeatExchangerA->unref();
	      pInnerSupportGapperA->unref();
	      pOuterSupportGapperA->unref();

	    } //include membrane, heat  exchanger and foil 
	  } // iiWheel loop for Wheel A
      } // if (pCommonEndcapAB[iiSide]) block for Wheel A
    } // iiSide loop for Wheel A
    
    //pStrawPlaneA->unref(); // Get eventual seg fault if unref. Clone doesn't increment ref count of orig, See bug #34074


      // ---------------  Wheel B  ----------------------------

      //Check here that (m_data->endcapNumberOfStrawsInStrawLayer_AWheels == m_data->endcapNumberOfStrawsInStrawLayer_BWheels) !!
      //We assume this in several places!

      // Inner/Outer supports

    GeoTube* sInnerSupportB = new GeoTube(m_data->endCapInnerRadiusOfSupportB,
					  m_data->endCapInnerRadiusOfSupportB + m_data->endCapRadialThicknessOfInnerSupportB,
					  m_data->endCapLengthOfWheelsB/2);
    GeoLogVol* lInnerSupportB = new GeoLogVol("InnerSupportB", sInnerSupportB, m_materialManager->getMaterial("trt::InnerSupportB"));
    GeoPhysVol* pInnerSupportB = new GeoPhysVol(lInnerSupportB);

    GeoTube* sOuterSupportB = new GeoTube(m_data->endCapOuterRadiusOfSupportB - m_data->endCapRadialThicknessOfOuterSupportB,
					  m_data->endCapOuterRadiusOfSupportB, m_data->endCapLengthOfWheelsB/2);
    GeoLogVol* lOuterSupportB = new GeoLogVol("OuterSupportB", sOuterSupportB, m_materialManager->getMaterial("trt::OuterSupportB"));
    GeoPhysVol* pOuterSupportB = new GeoPhysVol(lOuterSupportB);

    // Straw plane
    GeoFullPhysVol* pStrawPlaneB_Kr = nullptr;
    GeoFullPhysVol* pStrawPlaneB_Ar = nullptr;
    if (m_doKrypton)
      pStrawPlaneB_Kr = makeStrawPlane(firstIndexOfB,GM_KRYPTON);
    if (m_doArgon)
      pStrawPlaneB_Ar = makeStrawPlane(firstIndexOfB,GM_ARGON);
    GeoFullPhysVol* pStrawPlaneB    = makeStrawPlane(firstIndexOfB);
//    pStrawPlaneB->ref();

    // Radiators
    
    GeoTube* sMainRadiatorB = new GeoTube(m_data->endCapInnerRadiusOfSupportB + m_data->endCapRadialThicknessOfInnerSupportB,  //TK: no dist between rad&support at inner??????????????????????????????????????
					  m_data->endCapOuterRadiusOfSupportB - m_data->endCapRadialThicknessOfOuterSupportB
					  - m_data->endCapRadialDistFromRadToOuterSupportB, m_data->endCapMainRadiatorThicknessB/2);
    GeoLogVol* lMainRadiatorB = new GeoLogVol("MainRadiatorB",sMainRadiatorB,  m_materialManager->getMaterial("trt::FoilRadiatorB"));
    GeoPhysVol* pMainRadiatorB = new GeoPhysVol(lMainRadiatorB);

    GeoTube* sThinRadiatorB = new GeoTube(m_data->endCapInnerRadiusOfSupportB + m_data->endCapRadialThicknessOfInnerSupportB,
					  m_data->endCapOuterRadiusOfSupportB - m_data->endCapRadialThicknessOfOuterSupportB
					  - m_data->endCapRadialDistFromRadToOuterSupportB,
					  m_data->endCapThinRadiatorThicknessB/2);
    GeoLogVol* lThinRadiatorB = new GeoLogVol("ThinRadiatorB",sThinRadiatorB,  m_materialManager->getMaterial("trt::FoilRadiatorB"));
    GeoPhysVol* pThinRadiatorB = new GeoPhysVol(lThinRadiatorB);

    GeoTube* sMiddleRadiatorB = new GeoTube(m_data->endCapInnerRadiusOfSupportB + m_data->endCapRadialThicknessOfInnerSupportB,
					    m_data->endCapOuterRadiusOfSupportB - m_data->endCapRadialThicknessOfOuterSupportB
					    - m_data->endCapRadialDistFromRadToOuterSupportB, m_data->endCapMiddleRadiatorThicknessB/2);
    GeoLogVol* lMiddleRadiatorB = new GeoLogVol("MiddleRadiatorB",sMiddleRadiatorB,  m_materialManager->getMaterial("trt::FoilRadiatorB"));
    GeoPhysVol* pMiddleRadiatorB = new GeoPhysVol(lMiddleRadiatorB);

    // Wheel
    //TK: endCapAmountWheelLengthExceedsSumOfLayer = ...................
    GeoTube* sWheelB  = new GeoTube( m_data->endCapInnerRadiusOfSupportB,m_data->endCapOuterRadiusOfSupportB, m_data->endCapLengthOfWheelsB/2);
    GeoLogVol* lWheelB  = new GeoLogVol("WheelB", sWheelB,  m_materialManager->getMaterial("trt::CO2"));

    // This is the straw pitch.
    double deltaPhiForStrawsB = 360.*CLHEP::deg/m_data->endcapNumberOfStrawsInStrawLayer_BWheels;

    for(iiSide=0; iiSide<nSides; iiSide++) {

      double WheelPlacerB = m_data->endCapPositionOfFirstWheelB[iiSide];

      // Wheel B
      if (pCommonEndcapAB[iiSide]) {
	for(iiWheel=firstIndexOfB; iiWheel < firstIndexOfC; iiWheel++)
	  {
            //prepair to place wheel
	    WheelPlacerB += m_data->endCapDistanceBetweenWheelCentersB[iiSide][iiWheel];
	    
	    GeoFullPhysVol* pWheelB  = new GeoFullPhysVol(lWheelB);

	    GeoAlignableTransform * xfAlignableModule = 0;	    

	    // Place planes in the wheel
	    for (iiPlane = 0; iiPlane < m_data->endCapNumberOfStrawLayersPerWheelB; iiPlane++)
	      {

		// Each wheel in WheelB is subdivided into 2 alignable objects (every 4th straw layer)
		// We create an alignable transform for each alignable module 
		// and multiply this by the transform for every straw layer in the "alignable module" 
		// The tranform is by default Identity.
		if (iiPlane % 4 == 0) {
		  // Register alignable node
		  int barrel_ec = (iiSide) ? -2 : +2;
		  xfAlignableModule = new GeoAlignableTransform(HepGeom::Transform3D());
		  Identifier idSubModule = idHelper->layer_id(barrel_ec, 0, iiWheel, iiPlane); 
		  // We pass the parent volume as the local delta for this correction is the same as a local delta
		  // on the transformation of the wheel.
		  m_detectorManager->addAlignableTransform(AlignmentLevelSubWheel, idSubModule, xfAlignableModule, pWheelB); 	    
		}

    Identifier TRT_Identifier;
    int bar_ec = (iiSide) ? -2 : +2;
    TRT_Identifier = idHelper->straw_id(bar_ec, 1, iiWheel, 1, 1);
    int strawStatusHT = TRTCond::StrawStatus::Good;
    if (m_strawsvcavailable && (m_doArgon || m_doKrypton)) strawStatusHT = m_sumSvc->getStatusHT(TRT_Identifier);
    ActiveGasMixture agm = DecideGasMixture(strawStatusHT);

    //Ruslan: insert plane with Ar-straws
    //Artem: same for Kr
    switch (agm)
      {
      case GM_ARGON:
        msg(MSG::DEBUG) << "Marking Argon straws from /TRT/Cond/StatusHT:\t"
                        << idHelper->print_to_string(TRT_Identifier) << endmsg;
        childPlane = pStrawPlaneB_Ar->clone();
        break;
      case GM_KRYPTON:
        msg(MSG::DEBUG) << "Marking Krypton straws from /TRT/Cond/StatusHT:\t"
                        << idHelper->print_to_string(TRT_Identifier) << endmsg;
        childPlane = pStrawPlaneB_Kr->clone();
        break;
      case GM_XENON:
        msg(MSG::DEBUG) << "Marking Xenon straws from /TRT/Cond/StatusHT:\t"
                        << idHelper->print_to_string(TRT_Identifier) << endmsg;
        childPlane = pStrawPlaneB->clone();
        break;
      default:
        msg(MSG::FATAL) << "Unexpected gas mixture: " << agm << endmsg; 
        throw std::runtime_error("Unexpected gas mixture");
        return;
      }
        
//		childPlane = pStrawPlaneB->clone();
		
		// phiPlane is phi of straw 0, sector 0 (online numbering)
		double phiPlane = m_data->endCapPhiOfFirstStraw + RotationsOfStrawPlanes[iiPlane%nStrawLayMaxEc]*deltaPhiForStrawsB;

		// For compatibility with old geometry we have to shift every eighth wheel by 1 straw pitch.
		if(iiSide && oldGeometry && (iiPlane%8 == 0)) {
		  phiPlane +=  deltaPhiForStrawsB;
		}

		xfPlane = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionB[iiPlane]
							   - m_data->endCapLengthOfWheelsB/2)*HepGeom::RotateZ3D(phiPlane));

		if (xfAlignableModule) pWheelB->add(xfAlignableModule);
		pWheelB->add(xfPlane);
		pWheelB->add(new GeoIdentifierTag(iiPlane));
		pWheelB->add(childPlane);

		// Create elements
		for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		  {
		    // m_data->nEndcapPhi assumed to be even.
		    // For positive endcap online == offline. For negative endcap we rotate 180 deg about y axis so 
		    // sector 0 -> 15, 15 -> 0, 16 -> 31, 31 -> 16, etc. This is achieved with 
		    // sector -> (nSectors + nSectors/2 - sector - 1) % nSectors
		    int iiPhiOffline = (iiSide==0) ? iiPhi :  (3*m_data->nEndcapPhi/2 - iiPhi - 1)% m_data->nEndcapPhi;
		    element = new InDetDD::TRT_EndcapElement(childPlane,
							     descriptorsAB[iiSide][iiPlane%nStrawLayMaxEc][iiPhi],
							     iiSide==0,
							     iiWheel,
							     iiPlane,
							     iiPhiOffline,
							     idHelper,
							     conditions);
		    m_detectorManager->manageEndcapElement(element);
		  }
	      }

	    // Place radiators in the wheel
	    for (counter = 1; counter <= m_data->endCapNumberOfStrawLayersPerWheelB; counter++)
	      {
		// Main radiators
		if (counter % 4 != 0)
		  {
		    xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionB[counter-1] - m_data->endCapLengthOfWheelsB/2
								  + m_data->outerRadiusOfStraw  + m_data->endCapMainRadiatorThicknessB/2));
		    pWheelB->add(xfRadiator);
		    pWheelB->add(pMainRadiatorB);
		  }

		// Thin radiators
		if (counter == 1 || counter == 8)
		  {
		    sign = counter == 1? -1 : 1;
		    xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionB[counter-1] - m_data->endCapLengthOfWheelsB/2 + sign*(m_data->outerRadiusOfStraw  + m_data->endCapThinRadiatorThicknessB/2)));
		    pWheelB->add(xfRadiator);
		    pWheelB->add(pThinRadiatorB);
		  }

		// Middle radiators
		if (counter == 4 || counter == 5)
		  {
		    sign = counter == 4 ? 1 : -1;
		    xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionB[counter-1] - m_data->endCapLengthOfWheelsB/2 + sign*(m_data->outerRadiusOfStraw  + m_data->endCapMiddleRadiatorThicknessB/2)));
		    pWheelB->add(xfRadiator);
		    pWheelB->add(pMiddleRadiatorB);
		  }
	      }

	    // Place Inner/Outer supports in the wheel
	    pWheelB->add(pInnerSupportB);
	    pWheelB->add(pOuterSupportB);

	    // Place wheel in the Endcap Volume
	    GeoAlignableTransform * xfWheel = new GeoAlignableTransform(HepGeom::TranslateZ3D( WheelPlacerB ));


	    pCommonEndcapAB[iiSide]->add(xfWheel);
	    pCommonEndcapAB[iiSide]->add(new GeoIdentifierTag(iiWheel));
	    pCommonEndcapAB[iiSide]->add(pWheelB);

	    // Register alignable node
	    int barrel_ec = (iiSide) ? -2 : +2;
	    Identifier idModule = idHelper->module_id(barrel_ec, 0, iiWheel);
	    m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfWheel, pWheelB); 	    


	    if (m_data->includeECFoilHeatExchangerAndMembranes) {
	      
	      // Faraday Foils added 
	      // same Faraday foils for both wheel A and B
              // KaptonFoil radius is between R_max of InnerSupport and R_min of OuterSupport.....STS
	      GeoTube* sFaradayFoilWheelAB = new GeoTube(m_data->endCapInnerRadiusOfSupportB + m_data->endCapRadialThicknessOfInnerSupportB, 
							 m_data->endCapOuterRadiusOfSupportB - m_data->endCapRadialThicknessOfOuterSupportB,
							 m_data->endCapFaradayFoilThickness/2);
	      GeoLogVol* lFaradayFoilWheelAB = new GeoLogVol("FaradayKaptonFoil",sFaradayFoilWheelAB,  m_materialManager->getMaterial("trt::FaradayFoilMaterial"));
	      GeoPhysVol* pFaradayFoilWheelAB = new GeoPhysVol(lFaradayFoilWheelAB);


	      // Heat Exchanger 
	      GeoTube* sHeatExchangerB = new GeoTube(m_data->endCapRMinOfHeatExchanger,m_data->endCapRMaxOfHeatExchanger,m_data->endCapHeatExchangerThicknessB/2);

	      GeoLogVol* lHeatExchangerB = new GeoLogVol("HeatExchangerB", sHeatExchangerB, m_materialManager->getMaterial("trt::HeatExchangerBMat"));
	      GeoPhysVol* pHeatExchangerB = new GeoPhysVol(lHeatExchangerB);
	      pHeatExchangerB->ref(); // Doesn't get used for every wheel// Doesn't get used for every wheel

	      // Inner/Outer Support Gapper added
	      GeoTube* sInnerSupportGapperB = new GeoTube(m_data->endCapInnerRadiusOfSupportB,
							  m_data->endCapInnerRadiusOfSupportB + m_data->endCapRadialThicknessOfInnerSupportB,
                                                          m_data->endCapHeatExchangerThicknessB/2 + m_data->endCapFaradayFoilThickness/2 + zdelta);
	      GeoLogVol* lInnerSupportGapperB = new GeoLogVol("InnerSupportGapperB", sInnerSupportGapperB, m_materialManager->getMaterial("trt::InnerSupportB"));
	      GeoPhysVol* pInnerSupportGapperB = new GeoPhysVol(lInnerSupportGapperB);
	      pInnerSupportGapperB->ref(); // So we can unref later since doesn't get used for every wheel

	      GeoTube* sOuterSupportGapperB = new GeoTube(m_data->endCapOuterRadiusOfSupportB - m_data->endCapRadialThicknessOfOuterSupportB,
							  m_data->endCapOuterRadiusOfSupportB,
							  m_data->endCapHeatExchangerThicknessB/2 + m_data->endCapFaradayFoilThickness/2 + zdelta);
	      GeoLogVol* lOuterSupportGapperB = new GeoLogVol("OuterSupportGapperB", sOuterSupportGapperB, m_materialManager->getMaterial("trt::OuterSupportB"));
	      GeoPhysVol* pOuterSupportGapperB = new GeoPhysVol(lOuterSupportGapperB);
	      pOuterSupportGapperB->ref(); // So we can unref later since doesn't get used for every wheel

	      // Place kapton foils on a wheel just like a sandwitch 
	      if(iiWheel-firstIndexOfB<firstIndexOfC-firstIndexOfB)
		{ 
		  xfFaradayFoilFront = new GeoTransform(HepGeom::TranslateZ3D(WheelPlacerB 
									- m_data->endCapLengthOfWheelsB/2
									- m_data->endCapFaradayFoilThickness/2.0));
		  xfFaradayFoilBack = new GeoTransform(HepGeom::TranslateZ3D(WheelPlacerB 
								       + m_data->endCapLengthOfWheelsB/2
								       + m_data->endCapFaradayFoilThickness/2.0));
		  
		  pCommonEndcapAB[iiSide]->add(xfFaradayFoilFront);
		  pCommonEndcapAB[iiSide]->add(pFaradayFoilWheelAB);
		  pCommonEndcapAB[iiSide]->add(xfFaradayFoilBack);
		  pCommonEndcapAB[iiSide]->add(pFaradayFoilWheelAB);
		}

	      // Place HeatExchanger after putiing wheel only. No heat exchanger after the last wheel 
              // Ditto for Inner/OuterSupportGapper 
	      if(iiWheel-firstIndexOfB<firstIndexOfC-firstIndexOfB-1) 
	      {
		xfHeatExchanger = new GeoTransform(HepGeom::TranslateZ3D( WheelPlacerB
								    + m_data->endCapLengthOfWheelsB/2
								    + m_data->endCapFaradayFoilThickness
								    + m_data->endCapHeatExchangerThicknessB/2));
		pCommonEndcapAB[iiSide]->add(xfHeatExchanger);
		pCommonEndcapAB[iiSide]->add(pHeatExchangerB);

		xfInnerSupportGapperB = new GeoTransform(HepGeom::TranslateZ3D(WheelPlacerB
								    + m_data->endCapLengthOfWheelsB/2
								    + m_data->endCapFaradayFoilThickness
								    + m_data->endCapHeatExchangerThicknessB/2)); 

		xfOuterSupportGapperB = new GeoTransform(HepGeom::TranslateZ3D(WheelPlacerB
								    + m_data->endCapLengthOfWheelsB/2
								    + m_data->endCapFaradayFoilThickness
								    + m_data->endCapHeatExchangerThicknessB/2));
		pCommonEndcapAB[iiSide]->add(xfInnerSupportGapperB);
		pCommonEndcapAB[iiSide]->add(pInnerSupportGapperB);
		pCommonEndcapAB[iiSide]->add(xfOuterSupportGapperB);
		pCommonEndcapAB[iiSide]->add(pOuterSupportGapperB);
	      }
	      // Unref a few volumes. This will delete them if they did not get added.
	      pHeatExchangerB->unref();
	      pInnerSupportGapperB->unref();
	      pOuterSupportGapperB->unref();

	    } // include foil, heat exchanger and membrane
	  }// iiWheel loop  for Wheel B
      } // if (pCommonEndcapAB[iiSide]) block for Wheel B
    } // iiSide loop for Wheel B
    //pStrawPlaneB->unref(); // Get eventual seg fault if unref. Clone doesn't increment ref count of orig, See bug #34074

  } // end AB








  if (m_data->includeECFoilHeatExchangerAndMembranes) {
    // Membranes 
    //double thicknessOfMbrane = m_data->zMaxPositionOfMbraneWheelA1-m_data->zMinPositionOfMbraneWheelA1;
    
    GeoTube* sMbrane = new GeoTube(m_data->endCapRMinOfMbrane, m_data->endCapRMaxOfMbrane, m_data->endCapThicknessOfMbrane/2.0); 
    GeoLogVol* lMbrane = new GeoLogVol("Membrane", sMbrane, m_materialManager->getMaterial("trt::EndCapMbrane"));
    //GeoLogVol* lMbrane = new GeoLogVol("Membrane", sMbrane, m_materialManager->getMaterial("trt::FaradayFoilMaterial"));
    GeoPhysVol* pMbrane = new GeoPhysVol(lMbrane);
    
    GeoTransform *xfMbraneWheelA1 = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapZMinOfMbraneWheelA1 + m_data->endCapThicknessOfMbrane/2.0));
    GeoTransform *xfMbraneWheelA2 = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapZMinOfMbraneWheelA2 + m_data->endCapThicknessOfMbrane/2.0));
    GeoTransform *xfMbraneWheelB1 = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapZMinOfMbraneWheelB1 + m_data->endCapThicknessOfMbrane/2.0));
    GeoTransform *xfMbraneWheelB2 = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapZMinOfMbraneWheelB2 + m_data->endCapThicknessOfMbrane/2.0));
    
    for(iiSide=0; iiSide<nSides; iiSide++) {   
      pCommonEndcapAB[iiSide]->add(xfMbraneWheelA1);     
      pCommonEndcapAB[iiSide]->add(pMbrane);   
      pCommonEndcapAB[iiSide]->add(xfMbraneWheelA2);     
      pCommonEndcapAB[iiSide]->add(pMbrane);     
      pCommonEndcapAB[iiSide]->add(xfMbraneWheelB1);     
      pCommonEndcapAB[iiSide]->add(pMbrane);   
      pCommonEndcapAB[iiSide]->add(xfMbraneWheelB2);     
      pCommonEndcapAB[iiSide]->add(pMbrane);     
    }
  }



    // ----------------  Wheel C  ---------------------------
    // Not present in initial layout
  if (endcapCPlusPresent || endcapCMinusPresent) {
    // Inner/Outer supports
    GeoTube* sInnerSupportC = new GeoTube(m_data->endCapInnerRadiusOfSupportC, m_data->endCapInnerRadiusOfSupportC
					  + m_data->endCapRadialThicknessOfInnerSupportC, m_data->endCapLengthOfWheelsC/2);
    GeoLogVol* lInnerSupportC = new GeoLogVol("InnerSupportC", sInnerSupportC, m_materialManager->getMaterial("trt::InnerSupportC"));
    GeoPhysVol* pInnerSupportC = new GeoPhysVol(lInnerSupportC);

    GeoTube* sOuterSupportC = new GeoTube(m_data->endCapOuterRadiusOfSupportC - m_data->endCapRadialThicknessOfOuterSupportC,
					  m_data->endCapOuterRadiusOfSupportC, m_data->endCapLengthOfWheelsC/2);
    GeoLogVol* lOuterSupportC = new GeoLogVol("OuterSupportC", sOuterSupportC, m_materialManager->getMaterial("trt::OuterSupportC"));
    GeoPhysVol* pOuterSupportC = new GeoPhysVol(lOuterSupportC);

    // Straw plane
    GeoFullPhysVol* pStrawPlaneC 	= makeStrawPlane(firstIndexOfC);
    // pStrawPlaneC->ref();

    // Radiators
    GeoTube* sMainRadiatorC = new GeoTube(m_data->endCapInnerRadiusOfSupportC + m_data->endCapRadialThicknessOfInnerSupportC,
					  m_data->endCapOuterRadiusOfSupportC - m_data->endCapRadialThicknessOfOuterSupportC
					  - m_data->endCapRadialDistFromRadToOuterSupportC, m_data->endCapMainRadiatorThicknessC/2);
    GeoLogVol* lMainRadiatorC = new GeoLogVol("MainRadiatorC",sMainRadiatorC,  m_materialManager->getMaterial("trt::FoilRadiatorAC"));
    GeoPhysVol* pMainRadiatorC = new GeoPhysVol(lMainRadiatorC);

    GeoTube* sThinRadiatorC = new GeoTube(m_data->endCapInnerRadiusOfSupportC + m_data->endCapRadialThicknessOfInnerSupportC,
					  m_data->endCapOuterRadiusOfSupportC - m_data->endCapRadialThicknessOfOuterSupportC
					  - m_data->endCapRadialDistFromRadToOuterSupportC, m_data->endCapThinRadiatorThicknessC/2);
    GeoLogVol* lThinRadiatorC = new GeoLogVol("ThinRadiatorC",sThinRadiatorC,  m_materialManager->getMaterial("trt::FoilRadiatorAC"));
    GeoPhysVol* pThinRadiatorC = new GeoPhysVol(lThinRadiatorC);

    // Wheel
    GeoTube* sWheelC  = new GeoTube( m_data->endCapInnerRadiusOfSupportC,m_data->endCapOuterRadiusOfSupportC, m_data->endCapLengthOfWheelsC/2);
    GeoLogVol* lWheelC  = new GeoLogVol("WheelC", sWheelC,  m_materialManager->getMaterial("trt::CO2"));

    // This is the straw pitch.
    double deltaPhiForStrawsC = 360.*CLHEP::deg/m_data->endcapNumberOfStrawsInStrawLayer_CWheels;
  
    for(iiSide=0; iiSide<nSides; iiSide++) {
      // Wheel C
      if (pCommonEndcapC[iiSide]) {
	for(iiWheel=firstIndexOfC; iiWheel < indexUpperBound; iiWheel++)
	  {
	    GeoFullPhysVol* pWheelC  = new GeoFullPhysVol(lWheelC);

	    // Place planes in the wheel
	    for (iiPlane = 0; iiPlane < m_data->endCapNumberOfStrawLayersPerWheelC; iiPlane++)
	      {
		// phiPlane is phi of straw 0, sector 0 (online numbering)
		double phiPlane = m_data->endCapPhiOfFirstStraw +  RotationsOfStrawPlanes[iiPlane%nStrawLayMaxEc]*deltaPhiForStrawsC;

		// For compatibility with old geometry we have to shift every eighth wheel by 1 straw pitch.
		if(iiSide && oldGeometry && (iiPlane%8 == 0)) {
		  phiPlane +=  deltaPhiForStrawsC;
		}

		//AALONSO
		//if ((iiSide >= 0) &&  (iiWheel > 0) ) 		childPlane = pStrawPlaneC_Ar->clone();   // FOr side A, wheel 3
	//	else 						childPlane = pStrawPlaneC->clone();
		childPlane = pStrawPlaneC->clone();

		xfPlane = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionC[iiPlane]
							   - m_data->endCapLengthOfWheelsC/2)*HepGeom::RotateZ3D(phiPlane));
		pWheelC->add(xfPlane);
		pWheelC->add(new GeoIdentifierTag(iiPlane));
		pWheelC->add(childPlane);

		// Create descriptors
		// Just do it for the first wheel
		if(iiWheel==firstIndexOfC && iiPlane < nStrawLayMaxEc)
		  for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		    {
		      pDescriptor = new InDetDD::TRT_EndcapDescriptor();

		      pDescriptor->nStraws() = m_data->endcapNumberOfStrawsInStrawLayer_CWheels/m_data->nEndcapPhi;
		      pDescriptor->strawPitch() = deltaPhiForStrawsC;


		      double startPhi = phiPlane + iiPhi * pDescriptor->strawPitch() * pDescriptor->nStraws();

		      // For negative endcap the startPhi is the last straw in the physical sector, it then gets
		      // rotated 180 around y axis (phi -> pi - phi)
		      if (iiSide) {
			startPhi = CLHEP::pi - (startPhi + pDescriptor->strawPitch() * (pDescriptor->nStraws() - 1));
		      }
		      
		      // Make sure its between -pi and pi.
		      if (startPhi <= -CLHEP::pi) startPhi += 2*CLHEP::pi;
		      if (startPhi > CLHEP::pi) startPhi -= 2*CLHEP::pi;
		      
		 		      
		      pDescriptor->startPhi() = startPhi;

		      pDescriptor->strawLength() = m_data->endCapOuterRadiusOfSupportC - m_data->endCapRadialThicknessOfOuterSupportC
			- 2*m_data->lengthOfDeadRegion - m_data->endCapRadialThicknessOfInnerSupportC - m_data->endCapInnerRadiusOfSupportC;
		      pDescriptor->innerRadius() = m_data->endCapInnerRadiusOfSupportC + m_data->endCapRadialThicknessOfInnerSupportC + m_data->lengthOfDeadRegion;
		      pDescriptor->setStrawTransformField(m_detectorManager->endcapTransformField(2),iiPhi*pDescriptor->nStraws());
		      
		      //if(iiPlane > nStrawLayMaxEc)
		      //	std::cerr << "\nTRTDetectorFactory_Full ERROR: Cannot place descriptor in the array\n"
		      //		  << "Number of layers got from database exceeds nStrawLayMaxEc constant!\n";

		      descriptorsC[iiSide][iiPlane%nStrawLayMaxEc][iiPhi] = pDescriptor;
		    }

		// Create elements
		for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		  {
		    // m_data->nEndcapPhi assumed to be even.
		    // For positive endcap online == offline. For negative endcap we rotate 180 deg about y axis so 
		    // sector 0 -> 15, 15 -> 0, 16 -> 31, 31 -> 16, etc. This is achieved with 
		    // sector -> (nSectors + nSectors/2 - sector - 1) % nSectors
		    int iiPhiOffline = (iiSide==0) ? iiPhi :  (3*m_data->nEndcapPhi/2 - iiPhi - 1)% m_data->nEndcapPhi;
		    element = new InDetDD::TRT_EndcapElement(childPlane,
							     descriptorsC[iiSide][iiPlane%nStrawLayMaxEc][iiPhi],
							     iiSide==0,
							     iiWheel,
							     iiPlane,
							     iiPhiOffline,
							     idHelper,
							     conditions);
		    m_detectorManager->manageEndcapElement(element);
		  }
	      }

	    // Place radiators in the wheel
	    for (counter = 1; counter <= m_data->endCapNumberOfStrawLayersPerWheelC; counter++)
	      {
		if (counter % 4 == 1)
		  {
		    xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionC[counter-1] - m_data->endCapLengthOfWheelsC/2
								  - m_data->lengthOfDeadRegion  - m_data->endCapThinRadiatorThicknessC/2));
		    pWheelC->add(xfRadiator);
		    pWheelC->add(pThinRadiatorC);
		  }

		if (counter % 4 == 0)
		  {
		    xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionC[counter-1] - m_data->endCapLengthOfWheelsC/2
								  + m_data->outerRadiusOfStraw  + m_data->endCapThinRadiatorThicknessC/2));
		    pWheelC->add(xfRadiator);
		    pWheelC->add(pThinRadiatorC);
		    continue;
		  }

		xfRadiator = new GeoTransform(HepGeom::TranslateZ3D(m_data->endCapLayerZPositionC[counter-1] - m_data->endCapLengthOfWheelsC/2
							      + m_data->outerRadiusOfStraw  + m_data->endCapMainRadiatorThicknessC/2));
		pWheelC->add(xfRadiator);
		pWheelC->add(pMainRadiatorC);
	      }

	    // Place Inner/Outer supports in the wheel
	    pWheelC->add(pInnerSupportC);
	    pWheelC->add(pOuterSupportC);

	    // Place wheel in the Endcap Volume
	    GeoAlignableTransform * xfWheel 
	      = new GeoAlignableTransform(HepGeom::TranslateZ3D(m_data->endCapPositionOfFirstWheelC
							  + (iiWheel - firstIndexOfC)*m_data->endCapDistanceBetweenWheelCentersC));

	    pCommonEndcapC[iiSide]->add(xfWheel);
	    pCommonEndcapC[iiSide]->add(new GeoIdentifierTag(iiWheel));
	    pCommonEndcapC[iiSide]->add(pWheelC);

	    // Register alignable node
	    int barrel_ec = (iiSide) ? -2 : +2;
	    Identifier idModule = idHelper->module_id(barrel_ec, 0, iiWheel);
	    m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfWheel, pWheelC); 	    
	    

	  } // iiWheel loop for Wheel C
      } // if (pCommonEndcapC[iiSide]) block for Wheel C
    } // iiSide loop for Wheel C
    //pStrawPlaneC->unref(); // Get eventual seg fault if unref. Clone doesn't increment ref count of orig, See bug #34074

  } // End Wheel C


    // Set up the nearest neighbor pointers: in Z
  for (iiSide=0; iiSide<2; iiSide++)
    for(iiPhi=0; iiPhi<m_data->nEndcapPhi; iiPhi++)
      {
	InDetDD::TRT_EndcapElement *prev = 0;
	for (iiWheel=0; iiWheel<indexUpperBound; iiWheel++)
	  for (iiPlane=0; iiPlane<m_detectorManager->getNumerology()->getNEndcapLayers(iiWheel); iiPlane++)
	    {
	      InDetDD::TRT_EndcapElement *current = m_detectorManager->getEndcapElement(iiSide, iiWheel, iiPlane, iiPhi);
	      if (prev && current)
		{
		  prev->setNextInZ(current);
		  current->setPreviousInZ(prev);
		}
	      prev=current;
	    }
      }
}

//////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  Here follows private helper methods, used to construct the barrel module    //
//  forms (makeModule), the barrel straws (makeStraw) and the endcap straw      //
//  planes (makeStrawPlane)                                                     //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// makeModule ///////////////////////////////////
//
const GeoShape * TRTDetectorFactory_Full::makeModule ( double length, CLHEP::Hep2Vector corner1, CLHEP::Hep2Vector corner2,
						       CLHEP::Hep2Vector corner3, CLHEP::Hep2Vector corner4, HepGeom::Transform3D & modulePosition, double shrinkDist/*=0*/) const {


  // This method takes the absolute coordinates of the four corners,
  // constructs the relevant shape, centered around the intersections
  // of its diagonals, and returns it along with the absolute
  // coordinates of that center (modulePosition).
  //
  // We know that the four corners _approximately_ can be described as
  // the union of two isosceles triangles with same side-lengths and
  // different bases.
  //
  // shrinkDist is for the radiator, which must be similar to the
  // shell, but shrunken by some distance.

  // First we calculate the relative vectors of the edges:

  CLHEP::Hep2Vector delta12 = corner1 - corner2;   CLHEP::Hep2Vector delta23 = corner2 - corner3;
  CLHEP::Hep2Vector delta34 = corner3 - corner4;   CLHEP::Hep2Vector delta14 = corner1 - corner4;


  /* Why doesnt this work?? (well, it works, but are the results ok?)
     if (shrinkDist!=0) {
     // We should move the corners inwards (there are many ways - this one should leave the right shell thickness).

     // Unit-vectors normal to the edges, pointing inwards.
     CLHEP::Hep2Vector e12 = delta12.unit();     CLHEP::Hep2Vector e23 = delta23.unit();
     CLHEP::Hep2Vector e34 = delta34.unit();     CLHEP::Hep2Vector e14 = delta14.unit();
     e12.rotate(90*CLHEP::deg); e23.rotate(90*CLHEP::deg); e34.rotate(90*CLHEP::deg); e14.rotate(-90*CLHEP::deg);

     std::cout << "TK: makeModule : corner1 before (cm). X = " <<  corner1.x()/CLHEP::cm << " Y = " <<corner1.y()/CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner2 before (cm). X = " <<  corner2.x()/CLHEP::cm << " Y = " <<corner2.y()/CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner3 before (cm). X = " <<  corner3.x()/CLHEP::cm << " Y = " <<corner3.y()/CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner4 before (cm). X = " <<  corner4.x()/CLHEP::cm << " Y = " <<corner4.y()/CLHEP::cm <<std::endl;

     //Moving the corners
     corner1 += shrinkDist * ( e12 + e14 );    corner2 += shrinkDist * ( e12 + e23 );
     corner3 += shrinkDist * ( e23 + e34 );    corner4 += shrinkDist * ( e34 + e14 );

     std::cout << "TK: makeModule : corner1 after (cm). X = " <<  corner1.x()/CLHEP::cm << " Y = " <<corner1.y()/CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner2 after (cm). X = " <<  corner2.x()/CLHEP::cm << " Y = " <<corner2.y()/CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner3 after (cm). X = " <<  corner3.x()/CLHEP::cm << " Y = " <<corner3.y()/CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner4 after (cm). X = " <<  corner4.x()/CLHEP::cm << " Y = " <<corner4.y()/CLHEP::cm <<std::endl;

     std::cout << "TK: makeModule : corner1 moved (cm) " <<  shrinkDist * ( e12 + e14 ).mag() /CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner2 moved (cm) " <<  shrinkDist * ( e12 + e23 ).mag() /CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner3 moved (cm) " <<  shrinkDist * ( e23 + e34 ).mag() /CLHEP::cm <<std::endl;
     std::cout << "TK: makeModule : corner4 moved (cm) " <<  shrinkDist * ( e34 + e14 ).mag() /CLHEP::cm <<std::endl;

     // Recalculate since we changed the corners:
     delta12 = corner1 - corner2;    delta23 = corner2 - corner3;
     delta34 = corner3 - corner4;    delta14 = corner1 - corner4;
     }
  */

  // We also need the diagonals.
  CLHEP::Hep2Vector delta24 = corner2 - corner4;
  CLHEP::Hep2Vector delta13 = corner1 - corner3;

  // Then we find out which way the module bends (NB: .angle returns the UNSIGNED angle!).
  double openingAngleOfFirstCorner= delta12.angle(delta14);
  int sign = ( openingAngleOfFirstCorner < 90*CLHEP::deg ? 1 : -1);

  // If our approximation with triangles were correct, three of the
  // lengths (of edges and diagonals) would be equal. We force this
  // instead.
  //
  // (Whether the involved diagonal is 2-4 or 1-3 depends on the sign).
  double commonSide = (delta14.mag() + delta23.mag() + (sign==1?delta24.mag():delta13.mag()) ) / 3.;;
  double base1 = delta12.mag(); // Inner base
  double base2 = delta34.mag(); // Outer base

  if (shrinkDist!=0) {
    // Since the moving corners bit above doesnt work, we do this instead:
    double cosAlpha= sqrt(commonSide*commonSide-0.25*base1*base1)/commonSide;
    commonSide -= (1+1/cosAlpha)*shrinkDist;
    base1 -= 2*shrinkDist;
    base2 -= 2*shrinkDist;
  }

  double height1 = sqrt (commonSide*commonSide-0.25*base1*base1);
  //  std::cout << "TK: makeModule : height1 is " << height1 << std::endl;
  double height2 = sqrt (commonSide*commonSide-0.25*base2*base2);
  double rot     = atan(base2/height2/2)-atan(base1/height1/2);
  double epsilon = 1*CLHEP::micrometer; // needed to ensure perfect overlaps.
  GeoTrd *trd1 = new GeoTrd(base1/2+epsilon, epsilon, length/2, length/2, height1/2);
  GeoTrd *trd2 = new GeoTrd(epsilon, base2/2+epsilon, length/2, length/2, height2/2);

  double gamma = atan((base2/2+epsilon)*2/height2);
  double r = sqrt((base2/2+epsilon)*(base2/2+epsilon) + height2*height2/4);
  HepGeom::Transform3D xForm=HepGeom::Translate3D(r*sin(sign*(gamma-rot)),0,height1/2-r*cos(gamma-rot))*HepGeom::RotateY3D(sign*rot);
  const GeoShape & sShell = (*trd1).add((*trd2)<<xForm);

  //  We now have the shape we want. We only have left to transform
  //  its position to where we want it.
  //
  //  First, the actual positions of the four corners of
  //  the constructed shape.
  CLHEP::Hep2Vector actualCorner1, actualCorner2, actualCorner3, actualCorner4;
  actualCorner1 = corner1;
  actualCorner2 = corner1 + CLHEP::Hep2Vector(0,base1);
  if (sign==1) {
    actualCorner4 = corner1 + CLHEP::Hep2Vector(height1,base1/2);
    actualCorner3 = actualCorner4 + CLHEP::Hep2Vector(-base2*sin(rot),base2*cos(rot));
  } else {
    actualCorner3 = corner1 + CLHEP::Hep2Vector(height1,base1/2);
    actualCorner4 = actualCorner3 + CLHEP::Hep2Vector(-base2*sin(rot),-base2*cos(rot));
  }
  // The center of our shape is at
  CLHEP::Hep2Vector center= corner1 + CLHEP::Hep2Vector(height1/2,base1/2);

  //  Let us turn the whole module
  double modRot = (-delta12).phi()-CLHEP::Hep2Vector(0,1).phi();

  //  std::cout << "TK: modRot : "<< modRot/CLHEP::degree<<" degrees"<<std::endl;
  actualCorner1.rotate(modRot);
  actualCorner2.rotate(modRot);
  actualCorner3.rotate(modRot);
  actualCorner4.rotate(modRot);
  center.rotate(modRot);

  // Finally, the shape is moved where it fits best with the original corner coordinates.

  CLHEP::Hep2Vector displacement =  0.25*( (corner1+corner2+corner3+corner4) - (actualCorner1+actualCorner2+actualCorner3+actualCorner4) );
  //  .. << ::DEBUG << std::cout << "TK: makeModule : moving a total of (micrometer) " << displacement.mag()/CLHEP::micrometer<< std::endl;
  //  std::cout << "TK: makeModule : moving due to 1 (micrometer) " << 0.25*(corner1-actualCorner1).mag()/CLHEP::micrometer<< std::endl;
  //  std::cout << "TK: makeModule : moving due to 2 (micrometer) " << 0.25*(corner2-actualCorner2).mag()/CLHEP::micrometer<< std::endl;
  //  std::cout << "TK: makeModule : moving due to 3 (micrometer) " << 0.25*(corner3-actualCorner3).mag()/CLHEP::micrometer<< std::endl;
  //  std::cout << "TK: makeModule : moving due to 4 (micrometer) " << 0.25*(corner4-actualCorner4).mag()/CLHEP::micrometer<< std::endl;
  center += displacement;
  actualCorner1 += displacement;
  actualCorner2 += displacement;
  actualCorner3 += displacement;
  actualCorner4 += displacement;

  CLHEP::Hep2Vector remainingOffset =  -0.25*( (corner1+corner2+corner3+corner4) - (actualCorner1+actualCorner2+actualCorner3+actualCorner4) );
  //   std::cout << "TK: makeModule : remaining total offset (should be zero) (micrometer) " << remainingOffset.mag()/CLHEP::micrometer<< std::endl;
  //   std::cout << "TK: makeModule : 1 remaining offset (micrometer) " << (corner1-actualCorner1).mag()/CLHEP::micrometer<< std::endl;
  //   std::cout << "TK: makeModule : 2 remaining offset (micrometer) " << (corner2-actualCorner2).mag()/CLHEP::micrometer<< std::endl;
  //   std::cout << "TK: makeModule : 3 remaining offset (micrometer) " << (corner3-actualCorner3).mag()/CLHEP::micrometer<< std::endl;
  //   std::cout << "TK: makeModule : 4 remaining offset (micrometer) " << (corner4-actualCorner4).mag()/CLHEP::micrometer<< std::endl;

  //  The final positioning includes a few 90deg rotations because the axis's in the GeoTrd's are different from the actual axis's.
  modulePosition = HepGeom::TranslateY3D(center.y())*HepGeom::TranslateX3D(center.x())*HepGeom::RotateZ3D(90.0*CLHEP::deg)*HepGeom::RotateX3D(90.0*CLHEP::deg)*HepGeom::RotateY3D(modRot);

  return &sShell;
}
//////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////// makeStraw ///////////////////////////////////

//
//GeoPhysVol * TRTDetectorFactory_Full::makeStraw( double& activeGasZPosition, bool hasLargeDeadRegion /*= false*/ ) const {
GeoPhysVol * TRTDetectorFactory_Full::makeStraw( double& activeGasZPosition, bool hasLargeDeadRegion /*= false*/, ActiveGasMixture gasMixture) const {

  double lengthOfInnerDeadRegion= hasLargeDeadRegion ? m_data->barrelLengthOfLargeDeadRegion : m_data->lengthOfDeadRegion ;
  double lengthOfActiveGas = (m_data->barrelLengthOfStraw-m_data->barrelLengthOfTwister)/2.0 - m_data->lengthOfDeadRegion - lengthOfInnerDeadRegion;
  double posA   = (lengthOfActiveGas + m_data->barrelLengthOfTwister) / 2. + lengthOfInnerDeadRegion; // middle of lengthOfActiveGas
  double posInnerDeadRegion = ( m_data->barrelLengthOfTwister + lengthOfInnerDeadRegion ) / 2;

  activeGasZPosition=posA;

  // Straws:
  GeoTube    *sHole      = new GeoTube(0,m_data->barrelOuterRadiusOfStrawHole, m_data->barrelLengthOfStraw/2.0);
  GeoLogVol  *lHole      = new GeoLogVol("Hole", sHole, m_materialManager->getMaterial("trt::CO2"));
  GeoPhysVol *pHole      = new GeoPhysVol(lHole);

  // Straws:
  GeoTube    *sStraw      = new GeoTube(0,m_data->outerRadiusOfStraw, m_data->barrelLengthOfStraw/2.0);
  GeoLogVol  *lStrawMixed = new GeoLogVol("StrawM", sStraw, m_materialManager->getMaterial("trt::Straw"));
  GeoPhysVol *pStrawMixed = new GeoPhysVol(lStrawMixed);

  // Dead regions:
  GeoShape   *sDeadRegion = new GeoTube(m_data->outerRadiusOfWire , m_data->innerRadiusOfStraw , m_data->lengthOfDeadRegion/2 );
  GeoLogVol  *lDeadRegion = NULL;
  if (gasMixture == GM_ARGON) 
    lDeadRegion = new GeoLogVol("DeadRegion_Ar", sDeadRegion, m_materialManager->getMaterial("trt::ArCO2O2"));
  else if (gasMixture == GM_KRYPTON)
    lDeadRegion = new GeoLogVol("DeadRegion_Kr", sDeadRegion, m_materialManager->getMaterial("trt::KrCO2O2"));
  else
    lDeadRegion = new GeoLogVol("DeadRegion", sDeadRegion, m_materialManager->getMaterial((m_useOldActiveGasMixture ? "trt:XeCO2CF4" : "trt::XeCO2O2")));
  GeoPhysVol *pDeadRegion = new GeoPhysVol(lDeadRegion);

  // InnerDeadRegions, part II:
  GeoShape  * sInnerDeadRegion = new GeoTube(m_data->outerRadiusOfWire , m_data->innerRadiusOfStraw, lengthOfInnerDeadRegion/2 );
  GeoLogVol * lInnerDeadRegion = NULL;
  if (gasMixture == GM_ARGON)
    lInnerDeadRegion = new GeoLogVol("InnerDeadRegion_Ar", sInnerDeadRegion, m_materialManager->getMaterial("trt::ArCO2O2"));
  else if(gasMixture == GM_KRYPTON)
    lInnerDeadRegion = new GeoLogVol("InnerDeadRegion_Kr", sInnerDeadRegion, m_materialManager->getMaterial("trt::KrCO2O2"));
  else
    lInnerDeadRegion = new GeoLogVol("InnerDeadRegion", sInnerDeadRegion, m_materialManager->getMaterial((m_useOldActiveGasMixture ? "trt:XeCO2CF4" : "trt::XeCO2O2")));
  GeoPhysVol* pInnerDeadRegion = new GeoPhysVol(lInnerDeadRegion);

  // Twisters:
  GeoShape   *sTwister = new GeoTube(m_data->outerRadiusOfWire , m_data->innerRadiusOfStraw, m_data->barrelLengthOfTwister/2);
  GeoLogVol  *lTwister = new GeoLogVol("Twister", sTwister, m_materialManager->getMaterial("trt::Twister"));
  GeoPhysVol *pTwister = new GeoPhysVol(lTwister);

  // Wires:
  GeoShape   *sWire = new GeoTube( 0,m_data->outerRadiusOfWire,m_data->barrelLengthOfStraw/2.0);
  GeoLogVol  *lWire = new GeoLogVol("Wire", sWire, m_materialManager->getMaterial("trt::Wire"));
  GeoPhysVol *pWire = new GeoPhysVol(lWire);

  // NB please see comments in TRT_BarrelElement::getAbsoluteTransform before re-organizing
  // the next few paragraphs!!.

  // Gas for mixed straws, part I:
  GeoTube      *sGasMA    = new GeoTube(m_data->outerRadiusOfWire  , m_data->innerRadiusOfStraw,lengthOfActiveGas/2.0);
  GeoLogVol * lGasMA = NULL;
  if (gasMixture == GM_ARGON)
    lGasMA = new GeoLogVol("GasMA_Ar", sGasMA, m_materialManager->getMaterial("trt::ArCO2O2"));
  else if (gasMixture == GM_KRYPTON)
    lGasMA = new GeoLogVol("GasMA_Kr", sGasMA, m_materialManager->getMaterial("trt::KrCO2O2"));
  else
    lGasMA = new GeoLogVol("GasMA", sGasMA, m_materialManager->getMaterial((m_useOldActiveGasMixture ? "trt:XeCO2CF4" : "trt::XeCO2O2")));
  GeoNameTag   *nGasMAPos = new GeoNameTag("GasMAPos");
  GeoTransform *xGasMAPos = new GeoTransform(HepGeom::RotateY3D(M_PI)*HepGeom::TranslateZ3D(-posA));//the rotation of pi is to... digitization (TK)
  GeoNameTag   *nGasMANeg = new GeoNameTag("GasMANeg");
  GeoTransform *xGasMANeg = new GeoTransform(HepGeom::TranslateZ3D(-posA));
  GeoPhysVol   *pGasMA    = new GeoPhysVol(lGasMA);

  // Assemble gas within straws
  GeoSerialIdentifier *id = new GeoSerialIdentifier(0);
  pStrawMixed->add(id);
  pStrawMixed->add(nGasMANeg);
  pStrawMixed->add(xGasMANeg);
  pStrawMixed->add(pGasMA);
  pStrawMixed->add(nGasMAPos);
  pStrawMixed->add(xGasMAPos);
  pStrawMixed->add(pGasMA);

  // Outer Dead region for mixed straws, part II:
  GeoSerialDenominator   *nDeadMA    = new GeoSerialDenominator("DeadRegionL");
  GeoTransform           *xDeadPosMA = new GeoTransform(HepGeom::TranslateZ3D(+(m_data->barrelLengthOfStraw-m_data->lengthOfDeadRegion)/2.0));
  GeoTransform           *xDeadNegMA = new GeoTransform(HepGeom::TranslateZ3D(-(m_data->barrelLengthOfStraw-m_data->lengthOfDeadRegion)/2.0));

  // Assemble dead regions within straws:
  pStrawMixed->add(nDeadMA);
  pStrawMixed->add(xDeadPosMA);
  pStrawMixed->add(pDeadRegion);
  pStrawMixed->add(xDeadNegMA);
  pStrawMixed->add(pDeadRegion);

  // InnerDeadRegions, part III:
  GeoSerialDenominator   *nInnerDeadMA    = new GeoSerialDenominator("InnerDeadRegionL");
  GeoTransform           *xInnerDeadPosMA = new GeoTransform(HepGeom::TranslateZ3D(+posInnerDeadRegion));
  GeoTransform           *xInnerDeadNegMA = new GeoTransform(HepGeom::TranslateZ3D(-posInnerDeadRegion));
  // add to mixedStraw:
  pStrawMixed->add(nInnerDeadMA);
  pStrawMixed->add(xInnerDeadPosMA);
  pStrawMixed->add(pInnerDeadRegion);
  pStrawMixed->add(xInnerDeadNegMA);
  pStrawMixed->add(pInnerDeadRegion);

  // Assemble twisters within straws.
  GeoNameTag *nTwister = new GeoNameTag("TwisterM");
  pStrawMixed->add(nTwister);
  pStrawMixed->add(pTwister);

  // Assemble wires within straws.
  GeoNameTag *nWire = new GeoNameTag("WireM");
  pStrawMixed->add(nWire);
  pStrawMixed->add(pWire);

  pHole->add(pStrawMixed);

  return pHole;
}
//////////////////////////////////////////////////////////////////////////////////



///////////////////////////////// makeStrawPlane /////////////////////////////////
//
//GeoFullPhysVol * TRTDetectorFactory_Full::makeStrawPlane(size_t w) const {
GeoFullPhysVol * TRTDetectorFactory_Full::makeStrawPlane(size_t w, ActiveGasMixture gasMixture) const {
  // -----------------------------------------------------------------------------------//
  //                                                                                    //
  // There are twelve straw planes; however there are only two kinds, one for sector    //
  // A & B, and one for sector C.  We call the first "type 1" and the second "type 2"   //
  // In order to economize, we shall only create two planes.                            //
  // -----------------------------------------------------------------------------------//

  static GeoFullPhysVol *type1Plane=NULL, *type2Plane=NULL, *type1PlaneAr=NULL, *type2PlaneAr=NULL, *type1PlaneKr=NULL, *type2PlaneKr=NULL;
  size_t nstraws=0;

  //A and B wheels have similar straw planes, but the C wheels are different.
  //  const size_t firstIndexOfC = 15; //hardcoded
  const size_t firstIndexOfC = 14; //hardcoded

  GeoFullPhysVol *&cur_type1Plane = (gasMixture == GM_KRYPTON) ? type1PlaneKr :
                                    (gasMixture == GM_ARGON  ) ? type1PlaneAr :
                                                                 type1Plane;
  GeoFullPhysVol *&cur_type2Plane = (gasMixture == GM_KRYPTON) ? type2PlaneKr :
                                    (gasMixture == GM_ARGON  ) ? type2PlaneAr :
                                                                 type2Plane;

  if (w>=firstIndexOfC) {
    if (cur_type2Plane!=NULL) {
      return cur_type2Plane;
    }
    nstraws=m_data->endcapNumberOfStrawsInStrawLayer_CWheels;
  } 
  else {
    if (cur_type1Plane!=NULL) {
      return cur_type1Plane;
    }
    nstraws=m_data->endcapNumberOfStrawsInStrawLayer_AWheels;
    //Check here that (m_data->endcapNumberOfStrawsInStrawLayer_AWheels == m_data->endcapNumberOfStrawsInStrawLayer_BWheels) !!
  }

  double MultiplierForStrawLength = 0.999;//TK: update... to avoid conflicts? should be 0.9999??

  double ldead     = m_data->lengthOfDeadRegion;
  double r0        = m_data->outerRadiusOfWire;
  double r1        = m_data->innerRadiusOfStraw;
  double r2        = m_data->outerRadiusOfStraw;

  double R0, R1;
  if (w >= firstIndexOfC) {
    //C wheels:
    R0     = m_data->endcapOuterRadiusOfInnerSupport_wheelC;
    R1     = m_data->endcapInnerRadiusOfOuterSupport_wheelC;
  } else {
    //A and B wheels:
    R0     = m_data->endcapOuterRadiusOfInnerSupport_wheelAB;
    R1     = m_data->endcapInnerRadiusOfOuterSupport_wheelAB;
  }

  double Length = (R1-R0)*MultiplierForStrawLength;//TK update
  double pos    = 0.5*(R0+R1);


  GeoFullPhysVol *pStrawPlane=NULL;//TK update
  GeoTube   *sStrawPlane = new GeoTube(R0,R1,r2);
//  GeoLogVol *lStrawPlane = NULL;
//  if(isArgon)	lStrawPlane = new GeoLogVol("StrawPlaneAr", sStrawPlane,  m_materialManager->getMaterial("trt::CO2"));
//  else		lStrawPlane = new GeoLogVol("StrawPlane", sStrawPlane,  m_materialManager->getMaterial("trt::CO2"));
  GeoLogVol *lStrawPlane = new GeoLogVol("StrawPlane", sStrawPlane,  m_materialManager->getMaterial("trt::CO2"));
  pStrawPlane            = new GeoFullPhysVol(lStrawPlane); //TK update


  GeoTube    *sStraw = new GeoTube( 0, r2, Length/2.0);
  GeoLogVol  *lStraw = new GeoLogVol("Straw",sStraw, m_materialManager->getMaterial("trt::Straw"));
  GeoPhysVol *pStraw = new GeoPhysVol(lStraw);

  // Positioning of straws :
  double dphi = 2*M_PI/ nstraws;
  HepGeom::RotateZ3D    Rz(1.0);// Radians!
  HepGeom::TranslateX3D Tx(1.0);// MM! TK: actually this doesnt need to be interpreted as mm? Just as a dimensionless 1. (i guess)
  HepGeom::TranslateY3D Ty(1.0);// MM!
  Variable    i;
  Sin sin;
  Cos cos;
  TRANSFUNCTION tx =  Pow(Tx,pos*cos(dphi*i))*Pow(Ty,pos*sin(dphi*i))*Pow(Rz,dphi*i)*HepGeom::RotateY3D(-90*CLHEP::deg);
  GeoSerialTransformer *serialTransformer=new GeoSerialTransformer(pStraw, &tx, nstraws);
  pStrawPlane->add(new GeoSerialIdentifier(0));
  pStrawPlane->add(serialTransformer);

  // Give this parameterization also to the readout geometry:
  if (w<firstIndexOfC) {
    m_detectorManager->setEndcapTransformField(0,tx.clone());
    m_detectorManager->setEndcapTransformField(1,tx.clone());
  }
  else {
    m_detectorManager->setEndcapTransformField(2,tx.clone());
  }

  // Gas :
  //   (Function TRTConstructionOfTube::ConstructAndPosition #2)
  GeoTube    *sGas = new GeoTube (r0,r1,(Length-2*ldead)/2);
  GeoLogVol  *lGas = NULL;
  if (gasMixture == GM_ARGON)
    lGas = new GeoLogVol("Gas_Ar", sGas, m_materialManager->getMaterial("trt::ArCO2O2"));
  else if (gasMixture == GM_KRYPTON)
    lGas = new GeoLogVol("Gas_Kr", sGas, m_materialManager->getMaterial("trt::KrCO2O2"));
  else
    lGas = new GeoLogVol("Gas", sGas, m_materialManager->getMaterial((m_useOldActiveGasMixture ? "trt::XeCO2CF4" : "trt::XeCO2O2")));
  GeoPhysVol *pGas = new GeoPhysVol(lGas);
  pStraw->add(pGas);

  // Dead region :
  GeoTube *sDeadRegion    = new GeoTube(r0,r1,ldead/2);
  GeoLogVol *lDeadRegion  = NULL;
  if (gasMixture == GM_ARGON)
    lDeadRegion = new GeoLogVol("DeadRegion_Ar",sDeadRegion,m_materialManager->getMaterial("trt::ArCO2O2"));
  else if (gasMixture == GM_KRYPTON)
    lDeadRegion = new GeoLogVol("DeadRegion_Kr",sDeadRegion,m_materialManager->getMaterial("trt::KrCO2O2"));
  else
    lDeadRegion = new GeoLogVol("DeadRegion",sDeadRegion,m_materialManager->getMaterial((m_useOldActiveGasMixture ? "trt::XeCO2CF4" : "trt::XeCO2O2")));
  GeoPhysVol *pDeadRegion = new GeoPhysVol(lDeadRegion);

  GeoTransform *xDeadPos = new GeoTransform(HepGeom::TranslateZ3D(+(Length/2-ldead/2)));
  GeoTransform *xDeadNeg = new GeoTransform(HepGeom::TranslateZ3D(-(Length/2-ldead/2)));
  pStraw->add(xDeadPos);
  pStraw->add(pDeadRegion);
  pStraw->add(xDeadNeg);
  pStraw->add(pDeadRegion);


  // Wire :
  GeoTube *sWire = new GeoTube( 0, r0, Length/2);
  GeoLogVol *lWire = new GeoLogVol("Wire", sWire, m_materialManager->getMaterial("trt::Wire"));
  GeoPhysVol *pWire = new GeoPhysVol(lWire);
  pStraw->add(pWire);

  // Look above *type2Plane=NULL
  if (w>=firstIndexOfC && type2Plane!=NULL) {
    cur_type2Plane=pStrawPlane;
    return cur_type2Plane;
  }
  else {
    cur_type1Plane=pStrawPlane;
    return cur_type1Plane;
  }

}


TRTDetectorFactory_Full::ActiveGasMixture TRTDetectorFactory_Full::DecideGasMixture(int strawStatusHT)
  {
  ActiveGasMixture return_agm = GM_XENON;
  if (m_strawsvcavailable && m_doArgon && (strawStatusHT == TRTCond::StrawStatus::Dead ||
                                           strawStatusHT == TRTCond::StrawStatus::Argon))
    return_agm = GM_ARGON;
  else if (m_strawsvcavailable && m_doKrypton && (strawStatusHT == TRTCond::StrawStatus::Krypton))
    return_agm = GM_KRYPTON;
  else if (m_strawsvcavailable && strawStatusHT != TRTCond::StrawStatus::Xenon &&
                                  strawStatusHT != TRTCond::StrawStatus::Good &&
                                  strawStatusHT != TRTCond::StrawStatus::Dead &&
                                  strawStatusHT != TRTCond::StrawStatus::Argon &&
                                  strawStatusHT != TRTCond::StrawStatus::Krypton &&
                                  strawStatusHT != TRTCond::StrawStatus::EmulateArgon &&
                                  strawStatusHT != TRTCond::StrawStatus::EmulateKrypton)
    {
    msg(MSG::FATAL) << "Unexpected StatusHT value: " << strawStatusHT << endmsg; 
    throw std::runtime_error("Unexpected StatusHT value");
    }
  return return_agm; 
  }

//////////////////////////////////////////////////////////////////////////////////
