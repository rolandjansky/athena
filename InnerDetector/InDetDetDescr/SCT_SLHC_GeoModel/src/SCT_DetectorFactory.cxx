/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// SCT_DetectorFactory: This is the top level node 
//
#include "SCT_SLHC_GeoModel/SCT_DetectorFactory.h" 
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"
#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h" 
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/Version.h" 
#include "InDetReadoutGeometry/SiCommonItems.h" 
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h" 

#include "SCT_SLHC_GeoModel/SCT_Barrel.h"
#include "SCT_SLHC_GeoModel/SCT_Forward.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h"
#include "SCT_SLHC_GeoModel/SCT_ServiceBuilder.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"

//GeoModel include files:
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
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

#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GeoModelKernel/Units.h"

#include <iostream> 
#include <iomanip> 
 
using InDetDD::SCT_DetectorManager; 
using InDetDD::SiCommonItems; 

namespace InDetDDSLHC {

SCT_DetectorFactory::SCT_DetectorFactory(const SCT_GeoModelAthenaComps * athenaComps,
					 const SCT_Options & options)
  : InDetDD::DetectorFactoryBase(athenaComps) 
{ 
  // Create the detector manager
  m_detectorManager = new SCT_DetectorManager(detStore());

  //
  // Create the geometry manager.
  //
  
  m_geometryManager = new SCT_GeometryManager(athenaComps);
  m_geometryManager->setOptions(options);

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
  int versionMinorNumber = 4;
  int versionPatchNumber = 4;
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


void SCT_DetectorFactory::create(GeoPhysVol *world){ 

  msg(MSG::INFO) << "Building Upgrade SCT Detector." << endmsg;
  msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;
  //Change precision (only because somewhere its being set to 2)
  int oldPrecision = std::cout.precision(6);



  const SCT_GeneralParameters * generalParameters = m_geometryManager->generalParameters();
  const SCT_BarrelParameters  * barrelParameters  = m_geometryManager->barrelParameters();
 
  //Create the SCT Barrel
  SCT_Barrel sctBarrel("SCT_Barrel");
  
  //Create the Forward
  SCT_Forward sctForward("SCT_Forward");

  SCT_MaterialManager * materials = m_geometryManager->materialManager();

  const GeoShape * sctEnvelope = 0;
  InDetDD::Zone * sctZone = 0; 
  if (generalParameters->booleanEnvelope()) {
    // Build as boolean
    //envelope for the barrel
    double barrelEnvelope_halflength = sctForward.zCenter()-0.5*sctForward.length();
    const GeoTube* sctBarrelEnvelop = 
      new GeoTube(sctBarrel.innerRadius(), sctBarrel.outerRadius(), barrelEnvelope_halflength);
    //new GeoTube(sctBarrel.innerRadius(), sctBarrel.outerRadius(), 0.5*sctBarrel.length());
    //envelope for the fwd endcap
    const GeoTube* sctForwardEnvelop = 
      new GeoTube(sctForward.innerRadius(), sctForward.outerRadius(), 0.5*sctForward.length());
    //make a single envelope (enveloping the barrel and endcaps)
    const GeoShape & sctEnvelopeTmp = (*sctBarrelEnvelop).
      add(*sctForwardEnvelop << GeoTrf::TranslateZ3D(sctForward.zCenter())).
      add(*sctForwardEnvelop << GeoTrf::TranslateZ3D(-sctForward.zCenter()));

    sctEnvelope = &sctEnvelopeTmp;
  } else if (generalParameters->simpleEnvelope()) {
    // Build as tube
    GeoTube * sctEnvelopeTmp = new GeoTube(generalParameters->envelopeRMin(),
			      generalParameters->envelopeRMax(),
			      0.5*generalParameters->envelopeLength());
    sctZone = new InDetDD::TubeZone("SCT", sctEnvelopeTmp, 0);
    sctEnvelope = sctEnvelopeTmp;
  } else {
    // Build as PCon    
    GeoPcon* sctEnvelopeTmp  = new GeoPcon(0.,2*Gaudi::Units::pi);
    // table contains +ve z values only and envelope is assumed to be symmetric around z.
    int numPlanes = generalParameters->envelopeNumPlanes();
    for (int i = 0; i < numPlanes * 2; i++) {
      int tableIndex = (i < numPlanes) ?  numPlanes - 1 - i :  i - numPlanes;
      int sign       = (i < numPlanes) ? -1 : 1;
      double z = generalParameters->envelopeZ(tableIndex) * sign;
      double rmin = generalParameters->envelopeRMin(tableIndex);
      double rmax = generalParameters->envelopeRMax(tableIndex);
      sctEnvelopeTmp->addPlane(z, rmin, rmax);
    }
    sctZone = new InDetDD::PconZone("SCT", sctEnvelopeTmp);
    sctEnvelope = sctEnvelopeTmp;
  } 
  const GeoLogVol* sctLog = new GeoLogVol("SCT", sctEnvelope, materials->gasMaterial());
  GeoPhysVol* sct = new GeoPhysVol(sctLog);

  InDetDD::Zone * topZone = makeZone(sctZone, sctBarrel, sctForward);
  // takes ownership of topZone
  SCT_ServiceBuilder serviceBuilder(topZone, m_geometryManager);
  sctBarrel.setServiceBuilder(serviceBuilder);
  sctForward.setServiceBuilder(serviceBuilder);

  //The Barrel
  m_detectorManager->numerology().addBarrel(0);

  SCT_Identifier id;
  
  if (generalParameters->partPresent("Barrel")) {
    //this value of id is redefind when building modules (in SCT_Ski.cxx): barrel is +-1
    //the previous comment is obsolete with 64bits Identifier
    id.setBarrelEC(0);
    ///*---
    GeoVPhysVol* barrel = sctBarrel.build(id);
    GeoAlignableTransform* barrelTransform = new GeoAlignableTransform(GeoTrf::Transform3D());
    sct->add(new GeoNameTag("Barrel"));
    sct->add(barrelTransform);
    sct->add(barrel);
    //Store alignable transform
    m_detectorManager->addAlignableTransform(3, id.getWaferId(), barrelTransform, barrel);
    //---*/
  }
  
  // If either endcap we build both (not really any need for a single endcap layout)
  // if do_endcaps
  if (generalParameters->partPresent("EndcapA") || generalParameters->partPresent("EndcapC") ) {

    //Positive z endcap

    m_detectorManager->numerology().addEndcap(2);
    m_detectorManager->numerology().addEndcap(-2);
    
    SCT_Identifier idFwdPlus;
    idFwdPlus.setBarrelEC(2);
    GeoVPhysVol* forwardPlus = sctForward.build(idFwdPlus);
    GeoTrf::Transform3D fwdTransformPlus(GeoTrf::TranslateZ3D(sctForward.zCenter()));
    GeoAlignableTransform* fwdGeoTransformPlus = new GeoAlignableTransform(fwdTransformPlus);
    sct->add(new GeoNameTag("ForwardPlus"));
    sct->add(fwdGeoTransformPlus);
    sct->add(forwardPlus); 
    //Store alignable transform
    m_detectorManager->addAlignableTransform(3, idFwdPlus.getWaferId(), fwdGeoTransformPlus, 
					   forwardPlus);

    //Negative z endcap
    SCT_Identifier idFwdMinus;
    idFwdMinus.setBarrelEC(-2);
    GeoVPhysVol* forwardMinus = sctForward.build(idFwdMinus);
    GeoTrf::Transform3D rot;
    rot = GeoTrf::RotateY3D(180*Gaudi::Units::degree);
    GeoTrf::Transform3D fwdTransformMinus = rot*fwdTransformPlus;
    GeoAlignableTransform* fwdGeoTransformMinus = new GeoAlignableTransform(fwdTransformMinus);
    sct->add(new GeoNameTag("ForwardMinus"));
    sct->add(fwdGeoTransformMinus);
    sct->add(forwardMinus);
  
    //services material between barrel and endcap 
    double safety = 1*Gaudi::Units::mm;//1mm, just to avoid any clash
    double length = sctForward.zCenter()-0.5*sctForward.length()-0.5*sctBarrel.length()-safety;//
    double barrelServicesCylinderLength  = barrelParameters->barrelServicesMaterialCylinderLength();
    //use user lenght paramters only if small than the gap
    if(barrelServicesCylinderLength > length){
      std::cerr<<"SCT_DetectorFactory::ERROR: input barrelServicesCylinderLength "<<barrelServicesCylinderLength<<" > gap between barrel and edcap "<<length<<"! you should decrease barrelServicesCylinderLength.  exit athena."<<std::endl;
      exit(1);
    }
    
    double inner_radius      = sctBarrel.innerRadius();
    double outer_radius      = sctBarrel.outerRadius();
    double material_position = 0.5*(sctBarrel.length() + barrelServicesCylinderLength + safety);
    
  //change material density (carbon fiber here)
  //0.189 is taken from oracle database (CFiberSupport)
    double materialIncreaseFactor              = barrelParameters->barrelServicesMaterialIncreaseFactor();

    if (barrelServicesCylinderLength > 0 && materialIncreaseFactor > 0 && !barrelParameters->barrelServicesMaterial().empty()) {
      //double cf_density                          = 0.189*materialIncreaseFactor*GeoModelKernelUnits::g/Gaudi::Units::cm3;
      //msg(MSG::INFO) <<"----length "<<barrelServicesCylinderLength<<" material "<<barrelParameters->barrelServicesMaterial()<<" IncreaseFactor "<<materialIncreaseFactor<<" cf_density (GeoModelKernelUnits::g/Gaudi::Units::cm3) "<<cf_density/(GeoModelKernelUnits::g/Gaudi::Units::cm3) << endmsg;
      //const GeoMaterial* barrel_serivesMaterial = materials->getMaterial(barrelParameters->barrelServicesMaterial(), cf_density, "UpgradeSCTBarrel_ServicesMaterial");
      const GeoMaterial* barrel_serivesMaterial = materials->getMaterialScaled(barrelParameters->barrelServicesMaterial(), materialIncreaseFactor, "UpgradeSCTBarrel_ServicesMaterial");
      msg(MSG::INFO) <<"----length "<<barrelServicesCylinderLength<<" material "<<barrelParameters->barrelServicesMaterial()<<" IncreaseFactor "<<materialIncreaseFactor<<" density (GeoModelKernelUnits::g/Gaudi::Units::cm3) "<< barrel_serivesMaterial->getDensity()/(GeoModelKernelUnits::g/Gaudi::Units::cm3) << endmsg;
  
    
      const GeoTube*   barrelPos_servicesMaterialShape    = new GeoTube(inner_radius, outer_radius, 0.5*barrelServicesCylinderLength);
      const GeoLogVol* barrelPos_servicesMaterialtLog  = new GeoLogVol("BarrelPos_ServicesMaterial",  barrelPos_servicesMaterialShape, barrel_serivesMaterial);
      GeoPhysVol*      barrelPos_servicesMaterial      = new GeoPhysVol(barrelPos_servicesMaterialtLog);
      sct->add(new GeoTransform(GeoTrf::TranslateZ3D(material_position)));
      sct->add(barrelPos_servicesMaterial);
    
      const GeoTube*   barrelNeg_servicesMaterialShape    = new GeoTube(inner_radius, outer_radius, 0.5*barrelServicesCylinderLength);
      const GeoLogVol* barrelNeg_servicesMaterialtLog  = new GeoLogVol("BarrelNeg_ServicesMaterial",  barrelNeg_servicesMaterialShape, barrel_serivesMaterial);
      GeoPhysVol*      barrelNeg_servicesMaterial      = new GeoPhysVol(barrelNeg_servicesMaterialtLog);
      sct->add(new GeoTransform(GeoTrf::TranslateZ3D(-material_position)));
      sct->add(barrelNeg_servicesMaterial);
    }
    //Store alignable transform
    m_detectorManager->addAlignableTransform(3, idFwdMinus.getWaferId(), fwdGeoTransformMinus,
					     forwardMinus);
  }//end of if(do_endcaps)
  //---*/

  // Build services
  if (serviceBuilder.getServMatBuilder()) {
    serviceBuilder.getServMatBuilder()->buildAndPlace("SCT", sct);
  }

  //Add top envelope to the world.
  //The name tag here is what is used by the GeoModel viewer.
  GeoNameTag* tag = new GeoNameTag("SCT");         
  world->add(tag);
  world->add(sct);
  //Add the sct to the tree tops.
  m_detectorManager->addTreeTop(sct);

  //Set the neighbours
  m_detectorManager->initNeighbours();

  // Set maximum number of strips in numerology.
  for (int iDesign = 0;  iDesign <  m_detectorManager->numDesigns(); iDesign++) {
    m_detectorManager->numerology().setMaxNumPhiCells(m_detectorManager->getSCT_Design(iDesign)->cells());
  }


  /*//---
  //Register the callbacks and keys, the level corresponding to the key
  //and whether it expects a global (true) or local (false) shift.
  if (m_geometryManager->options().alignable()) {
    m_detectorManager->addKey("/Indet/Align/ID",3,true);
    m_detectorManager->addKey("/Indet/Align/SCT",2,true);
    m_detectorManager->addKey("/Indet/Align/SCTB1",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTB2",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTB3",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTB4",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA1",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA2",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA3",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA4",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA5",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA6",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA7",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA8",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEA9",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC1",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC2",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC3",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC4",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC5",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC6",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC7",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC8",1,false);
    m_detectorManager->addKey("/Indet/Align/SCTEC9",1,false);
  }
  */
  //Return precision to its original value
  std::cout.precision(oldPrecision);

  doChecks();

} 
 

const SCT_DetectorManager * SCT_DetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

InDetDD::Zone *
SCT_DetectorFactory::makeZone(InDetDD::Zone * zone, const SCT_Barrel & sctBarrel, const SCT_Forward & sctForward) {
  
  InDetDD::Zone * topZone = new InDetDD::UnboundedZone("Mother");
  if (zone) {
    topZone->add(zone);
  } else {
    zone = topZone;
  }

  double zOffset = sctForward.zCenter();
  zone->add(new InDetDD::TubeZone("C", 
				  -zOffset-0.5*sctForward.length(), -zOffset+0.5*sctForward.length(),
				  sctForward.innerRadius(), sctForward.outerRadius(), true));
  zone->add(new InDetDD::TubeZone("B",
				  -0.5*sctBarrel.length(),  0.5*sctBarrel.length(), 
				  sctBarrel.innerRadius(), sctBarrel.outerRadius()));
  zone->add(new InDetDD::TubeZone("A", 
				  zOffset-0.5*sctForward.length(), zOffset+0.5*sctForward.length(),
				  sctForward.innerRadius(), sctForward.outerRadius()));
  return topZone;
}

void 
SCT_DetectorFactory::doChecks()
{

  const SCT_ID * idHelper = m_geometryManager->athenaComps()->getIdHelper();
  const SCT_DetectorManager * manager = m_detectorManager;

  msg(MSG::INFO) << "Doing consistency checks." << endmsg;

  unsigned int maxHash = idHelper->wafer_hash_max();

  int count = 0;
  int missingCount = 0;
  InDetDD::SiDetectorElementCollection::const_iterator iter;  
  for (iter = manager->getDetectorElementBegin(); iter != manager->getDetectorElementEnd(); ++iter){
    count++;
    const InDetDD::SiDetectorElement * element = *iter; 
    if (!element) {
      msg(MSG::WARNING) << "MISSING ELEMENT!!!!!!!!!!! - Element # " << count-1 << endmsg;
      missingCount++;
    }
  }
  
  if (missingCount) { 
    msg(MSG::ERROR) << "There are missing elements in element array." << endmsg;
    msg(MSG::INFO) << "Number of elements: " << count << endmsg;
    msg(MSG::INFO) << "Number missing:     " << missingCount << endmsg;
  } 


  // TODO: Need to handle single sided modules.
  int nSides = 2;

  // Barrel
  int barrelCount = 0;
  int barrelCountError = 0;
  for (int iBarrelIndex = 0; iBarrelIndex < manager->numerology().numBarrels(); iBarrelIndex++) {
    int iBarrel = manager->numerology().barrelId(iBarrelIndex);
    for (int iLayer = 0; iLayer < manager->numerology().numLayers(); iLayer++) {
      if (manager->numerology().useLayer(iLayer)) {
	for (int iPhi = 0; iPhi < manager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
	  for (int iEta = manager->numerology().beginEtaModuleForLayer(iLayer); 
	       iEta < manager->numerology().endEtaModuleForLayer(iLayer); 
	       iEta++) {
	    for (int iSide = 0; iSide < nSides; iSide++) {
	      if (!iEta && manager->numerology().skipEtaZeroForLayer(iLayer)) continue;
	      Identifier id = idHelper->wafer_id(iBarrel,iLayer,iPhi,iEta,iSide);
	      const InDetDD::SiDetectorElement * element = manager->getDetectorElement(id);
	      barrelCount++;
	      std::stringstream ostr;
	      ostr << "[2.2." << iBarrel << "." << iLayer << "." << iPhi << "." << iEta << "." << iSide << ".0]"; 
	      if (!element) {
		barrelCountError++;
		msg(MSG::WARNING) << "   No element found for id: " << ostr.str() << " " << idHelper->show_to_string(id) << endmsg;
	      }
	    } // iSide
	  } // iEta
	} //iPhi
      }
    } //iLayer
  } // Barrel


  // Endcap
  int endcapCount = 0;
  int endcapCountError = 0;
  for (int iEndcapIndex = 0; iEndcapIndex < manager->numerology().numEndcaps(); iEndcapIndex++) {
    int iEndcap = manager->numerology().endcapId(iEndcapIndex);
    for (int iDisk = 0; iDisk < manager->numerology().numDisks(); iDisk++) {
      if (manager->numerology().useDisk(iDisk)) { 
	for (int iEta = 0; iEta < manager->numerology().numRingsForDisk(iDisk); iEta++) {
	  for (int iPhi = 0; iPhi < manager->numerology().numPhiModulesForDiskRing(iDisk,iEta); iPhi++) {
	    for (int iSide = 0; iSide < nSides; iSide++) {
	      Identifier id = idHelper->wafer_id(iEndcap,iDisk,iPhi,iEta,iSide);
	      const InDetDD::SiDetectorElement * element = manager->getDetectorElement(id);
	      endcapCount++;
	      std::stringstream ostr;
	      ostr << "[2.2." << iEndcap << "." << iDisk << "." << iPhi << "." << iEta << "." << iSide  << ".0]"; 
	      if (!element) {
		endcapCountError++;
		msg(MSG::WARNING) << "    No element found for id: " << ostr.str() << " " << idHelper->show_to_string(id) << endmsg;
	      }
	    } // iSide
	  } // iEta
	} //iPhi
      }
    } //iDisk
  } // Endcap;

  if (barrelCountError || endcapCountError) {
    msg(MSG::ERROR) << "There are elements which cannot be found."  << endmsg;
    msg(MSG::INFO) << "Number of barrel elements not found : " << barrelCountError << endmsg;
    msg(MSG::INFO) << "Number of endcap elements not found : " << endcapCountError << endmsg;
  }

  if ( barrelCount+endcapCount != int(maxHash)) { 
     msg(MSG::ERROR) << "Total count does not match maxHash." << endmsg;
     msg(MSG::INFO) << "Number of barrel elements : " << barrelCount << endmsg;
     msg(MSG::INFO) << "Number of endcap elements : " << endcapCount << endmsg;
     msg(MSG::INFO) << "Total                     : " << barrelCount+endcapCount << endmsg;
     msg(MSG::INFO) << "MaxHash                   : " << maxHash << endmsg;
  }
}


} // End namespace


