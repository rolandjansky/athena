/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// SCT_DetectorFactory: This is the top level node 
//


#include "SCT_GeoModel/SCT_DetectorFactory.h" 
#include "SCT_GeoModel/SCT_Identifier.h"
#include "SCT_GeoModel/SCT_GeometryManager.h" 
#include "SCT_GeoModel/SCT_MaterialManager.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "InDetReadoutGeometry/Version.h" 
#include "InDetReadoutGeometry/SiCommonItems.h" 
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h" 

#include "SCT_GeoModel/SCT_Barrel.h"
#include "SCT_GeoModel/SCT_Forward.h"
#include "SCT_GeoModel/SCT_DataBase.h"
#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"

//
// GeoModel include files:
//
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"



#include <iostream> 
#include <iomanip> 
#include <string>
 
using InDetDD::SCT_DetectorManager; 
using InDetDD::SiCommonItems; 

SCT_DetectorFactory::SCT_DetectorFactory(const SCT_GeoModelAthenaComps * athenaComps,
					 const SCT_Options & options)
  : InDetDD::DetectorFactoryBase(athenaComps),
    m_useDynamicAlignFolders(false)
{ 
  
  // Create the detector manager
  m_detectorManager = new SCT_DetectorManager(detStore());

  //
  // Create the geometry manager.
  //
  
  m_geometryManager = new SCT_GeometryManager();
  m_geometryManager->setOptions(options);
  m_geometryManager->setAthenaComps(athenaComps);

  m_useDynamicAlignFolders = options.dynamicAlignFolders();
 
  // Pass the Athena components the data base access class
  SCT_DataBase::setAthenaComps(athenaComps);
 
  // Create SiCommonItems. These are items that are shared by all elements
  SiCommonItems * commonItems = new SiCommonItems(athenaComps->getIdHelper());
  m_geometryManager->setCommonItems(commonItems);
  

  // Set Version information
  // Get the geometry tag
  DecodeVersionKey versionKey(geoDbTagSvc(),"SCT");
  IRDBRecordset_ptr switchSet
    = rdbAccessSvc()->getRecordsetPtr("SctSwitches", versionKey.tag(), versionKey.node());
  const IRDBRecord    *switches   = (*switchSet)[0];
  
  std::string layout = "Final";
  std::string description;
  if (!switches->isFieldNull("LAYOUT")) {
    layout = switches->getString("LAYOUT");
  }
  if (!switches->isFieldNull("DESCRIPTION")) {
    description = switches->getString("DESCRIPTION");
  }

  std::string versionTag = rdbAccessSvc()->getChildTag("SCT", versionKey.tag(), versionKey.node());
  std::string versionName = switches->getString("VERSIONNAME");
  int versionMajorNumber = 3;
  int versionMinorNumber = 6;
  int versionPatchNumber = 0;
  InDetDD::Version version(versionTag,
                           versionName, 
                           layout, 
                           description, 
                           versionMajorNumber,
                           versionMinorNumber,
                           versionPatchNumber);
  m_detectorManager->setVersion(version);

  // Initailize some static variables in various classes/
  SCT_ComponentFactory::setDetectorManager(m_detectorManager);
  SCT_ComponentFactory::setGeometryManager(m_geometryManager);

  SCT_Identifier::setIdHelper(athenaComps->getIdHelper());
 
} 
 
 
SCT_DetectorFactory::~SCT_DetectorFactory() 
{ 
  // NB the detector manager (m_detectorManager)is stored in the detector store by the
  // Tool and so we don't delete it.
  delete m_geometryManager;

} 

void SCT_DetectorFactory::create(GeoPhysVol *world) 
{ 

  msg(MSG::INFO) << "Building SCT Detector." << endmsg;
  msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;

  // Change precision.
  int oldPrecision = std::cout.precision(6);

  // The tree tops get added to world. We name it "indet" though.
  GeoPhysVol *indet = world;

  // Set default gas throughout detector.
  // Air is the default if this is not set.
  // SCT_MaterialManager::setGasMaterial("std::Air"); 

  const SCT_GeneralParameters * sctGeneral = m_geometryManager->generalParameters();

  GeoTrf::Transform3D sctTransform = sctGeneral->partTransform("SCT");

  std::string barrelLabel = "Barrel";
  std::string forwardPlusLabel = "EndcapA";
  std::string forwardMinusLabel = "EndcapC";

  bool barrelPresent       = sctGeneral->partPresent(barrelLabel);
  bool forwardPlusPresent  = sctGeneral->partPresent(forwardPlusLabel);
  bool forwardMinusPresent = sctGeneral->partPresent(forwardMinusLabel);



  //
  // The Barrel
  //  
  if (barrelPresent) {
   
    msg(MSG::DEBUG) << "Building the SCT Barrel." << endmsg;

    m_detectorManager->numerology().addBarrel(0);

    // Create the SCT Barrel
    SCT_Barrel sctBarrel("SCT_Barrel");
  
    SCT_Identifier id;
    id.setBarrelEC(0);
    GeoVPhysVol * barrelPV = sctBarrel.build(id);
    GeoAlignableTransform * barrelTransform = new GeoAlignableTransform(sctTransform * sctGeneral->partTransform(barrelLabel));

    //indet->add(new GeoNameTag("SCT_Barrel"));
    // The name tag here is what is used by the GeoModel viewer.
    GeoNameTag *topLevelNameTag = new GeoNameTag("SCT");
    indet->add(topLevelNameTag);
    indet->add(new GeoIdentifierTag(0));
    indet->add(barrelTransform);
    indet->add(barrelPV);
    m_detectorManager->addTreeTop(barrelPV);

    // Store alignable transform
    m_detectorManager->addAlignableTransform(3, id.getWaferId(), barrelTransform, barrelPV);
    
  }

  //
  // The Positive Z Endcap (EndcapA)
  //  
  if (forwardPlusPresent) {

    msg(MSG::DEBUG) << "Building the SCT Endcap A (positive z)." << endmsg;

    m_detectorManager->numerology().addEndcap(2);

    // Create the Forward
    SCT_Forward sctForwardPlus("SCT_ForwardA", +2);
    
    SCT_Identifier idFwdPlus;
    idFwdPlus.setBarrelEC(2);
    GeoVPhysVol * forwardPlusPV = sctForwardPlus.build(idFwdPlus);
    GeoTrf::Transform3D fwdTransformPlus(sctTransform 
                                          * sctGeneral->partTransform(forwardPlusLabel) 
                                          * GeoTrf::TranslateZ3D(sctForwardPlus.zCenter()));
    GeoAlignableTransform * fwdGeoTransformPlus = new GeoAlignableTransform(fwdTransformPlus);
    
    //indet->add(new GeoNameTag("SCT_ForwardPlus"));
    // The name tag here is what is used by the GeoModel viewer.
    GeoNameTag *topLevelNameTag = new GeoNameTag("SCT");
    indet->add(topLevelNameTag);
    indet->add(new GeoIdentifierTag(2));
    indet->add(fwdGeoTransformPlus);
    indet->add(forwardPlusPV);
    m_detectorManager->addTreeTop(forwardPlusPV);

    // Store alignable transform
    m_detectorManager->addAlignableTransform(3, idFwdPlus.getWaferId(), fwdGeoTransformPlus, forwardPlusPV);
  }
  
  //
  // The Negative Z Endcap (EndcapC)
  //  

  if (forwardMinusPresent) {

    msg(MSG::DEBUG) << "Building the SCT Endcap C (negative z)." << endmsg;

    m_detectorManager->numerology().addEndcap(-2);
    
    SCT_Forward sctForwardMinus("SCT_ForwardC",-2);

    SCT_Identifier idFwdMinus;
    idFwdMinus.setBarrelEC(-2);
    GeoVPhysVol * forwardMinusPV = sctForwardMinus.build(idFwdMinus);

    GeoTrf::Transform3D rot;
    rot = GeoTrf::RotateY3D(180 * Gaudi::Units::degree);
  
    GeoTrf::Transform3D fwdTransformMinus(sctTransform  
                                           * sctGeneral->partTransform(forwardMinusLabel)  
                                           * rot  
                                           * GeoTrf::TranslateZ3D(sctForwardMinus.zCenter()));
    GeoAlignableTransform * fwdGeoTransformMinus = new GeoAlignableTransform(fwdTransformMinus);

    //indet->add(new GeoNameTag("SCT_ForwardMinus"));
    // The name tag here is what is used by the GeoModel viewer.
    GeoNameTag *topLevelNameTag = new GeoNameTag("SCT");
    indet->add(topLevelNameTag);
    indet->add(new GeoIdentifierTag(-2));
    indet->add(fwdGeoTransformMinus);
    indet->add(forwardMinusPV);
    m_detectorManager->addTreeTop(forwardMinusPV);


    // Store alignable transform
    m_detectorManager->addAlignableTransform(3, idFwdMinus.getWaferId(), fwdGeoTransformMinus, forwardMinusPV);
  }  

  // Set the neighbours
  m_detectorManager->initNeighbours();

  // Set maximum number of strips in numerology.
  for (int iDesign = 0;  iDesign <  m_detectorManager->numDesigns(); iDesign++) {
    m_detectorManager->numerology().setMaxNumPhiCells(m_detectorManager->getSCT_Design(iDesign)->cells());
  }

  // Register the keys and the level corresponding to the key
  // and whether it expects a global or local shift.
  // level 0: sensor, level 1: module, level 2, layer/disc, level 3: whole barrel/enccap


  if (!m_useDynamicAlignFolders){

    m_detectorManager->addAlignFolderType(InDetDD::static_run1);
    m_detectorManager->addFolder("/Indet/Align");
    m_detectorManager->addChannel("/Indet/Align/ID",3,InDetDD::global);
    m_detectorManager->addChannel("/Indet/Align/SCT",2,InDetDD::global);
    
    if (barrelPresent) {
      m_detectorManager->addChannel("/Indet/Align/SCTB1",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTB2",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTB3",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTB4",1,InDetDD::local);
    }
    if (forwardPlusPresent) {
      m_detectorManager->addChannel("/Indet/Align/SCTEA1",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA2",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA3",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA4",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA5",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA6",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA7",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA8",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEA9",1,InDetDD::local);
    }
    if  (forwardMinusPresent) {
      m_detectorManager->addChannel("/Indet/Align/SCTEC1",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC2",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC3",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC4",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC5",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC6",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC7",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC8",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/SCTEC9",1,InDetDD::local);
    }
  }
  
  else {
    m_detectorManager->addAlignFolderType(InDetDD::timedependent_run2);
    m_detectorManager->addGlobalFolder("/Indet/AlignL1/ID");
    m_detectorManager->addGlobalFolder("/Indet/AlignL2/SCT");
    m_detectorManager->addChannel("/Indet/AlignL1/ID",3,InDetDD::global);
    m_detectorManager->addChannel("/Indet/AlignL2/SCT",2,InDetDD::global);
    m_detectorManager->addFolder("/Indet/AlignL3");

    if (barrelPresent) {
      m_detectorManager->addChannel("/Indet/AlignL3/SCTB1",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTB2",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTB3",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTB4",1,InDetDD::local);
    }
    if (forwardPlusPresent) {
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA1",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA2",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA3",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA4",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA5",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA6",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA7",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA8",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEA9",1,InDetDD::local);
    }
    if  (forwardMinusPresent) {
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC1",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC2",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC3",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC4",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC5",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC6",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC7",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC8",1,InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/SCTEC9",1,InDetDD::local);
    }
  }

  // Return precision to its original value
  std::cout.precision(oldPrecision);

} 
 

const SCT_DetectorManager * SCT_DetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}
 

