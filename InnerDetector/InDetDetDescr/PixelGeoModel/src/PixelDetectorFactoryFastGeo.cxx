/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/PixelDetectorFactoryFastGeo.h"

#include "PixelGeoModel/PixelSwitches.h" 
#include "PixelGeoModel/GeoVPixelFactory.h" 

// Envelope, as a starting point of the geometry
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"

#include "PixelInterfaces/IGeoPixelEnvelopeTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "InDetReadoutGeometry/SiDetectorElementMap.h"
#include "PixelGeoComponent/GeoEnvelopeAlpine.h"

// GeoModel includes
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  

// InDetReadoutGeometry
#include "InDetReadoutGeometry/SiCommonItems.h" 
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "PixelGeoModel/OraclePixGeoManager.h"
#include "PixelGeoModel/PixelGeoModelAthenaComps.h"

#include "InDetIdentifier/PixelID.h"
#include "StoreGate/StoreGateSvc.h"

using InDetDD::PixelDetectorManager; 
using InDetDD::SiCommonItems; 

PixelDetectorFactoryFastGeo::PixelDetectorFactoryFastGeo(const PixelGeoModelAthenaComps * athenaComps,
							 const PixelSwitches & switches,
							 std::string geoBuilderName,
							 bool bConfigGeoAlgTool):
  InDetDD::DetectorFactoryBase(athenaComps),
  m_detectorManager(0),
  m_geomLayoutName(geoBuilderName),
  m_geomBuilderTool(geoBuilderName),
  m_bConfigGeoAlgTool(bConfigGeoAlgTool)
{
  // Create the detector manager
  m_detectorManager = new PixelDetectorManager(detStore());
  GeoVPixelFactory::SetDDMgr(m_detectorManager);

  // Create the geometry manager.
  m_geometryManager =  new OraclePixGeoManager(athenaComps);
  GeoVPixelFactory::setGeometryManager(m_geometryManager);

  // Pass the switches
  m_geometryManager->SetServices(switches.services());
  m_geometryManager->SetServicesOnLadder(switches.servicesOnLadder());
  m_geometryManager->SetDC1Geometry(switches.dc1Geometry());
  m_geometryManager->SetAlignable(switches.alignable());
  m_geometryManager->SetInitialLayout(switches.initialLayout());
  m_geometryManager->SetSLHC(switches.slhc());
  m_geometryManager->SetIBL(switches.ibl());

  // get switch for DBM
  m_geometryManager->SetDBMFlag(switches.dbm());
  msg(MSG::INFO) << "DBM switch = SetDBMFlag: "<< m_geometryManager->dbm() << endreq;
   
  // Create SiCommonItems ans store it in geometry manager. 
  // These are items that are shared by all elements
  SiCommonItems * commonItems = new SiCommonItems(athenaComps->getIdHelper());
  commonItems->setLorentzAngleSvc(athenaComps->lorentzAngleSvc());
  m_geometryManager->setCommonItems(commonItems);
 
  // Determine if initial layer and tag from the id dict are consistent
  bool initialLayoutIdDict = (m_detectorManager->tag() == "initial_layout");
  if (m_geometryManager->InitialLayout() != initialLayoutIdDict ) {
    if(msgLvl(MSG::WARNING)) 
      msg(MSG::WARNING) << "IdDict tag is \"" << m_detectorManager->tag() 
			<< "\" which is inconsistent with the layout choosen!"
			<< endreq;
  } 

 
  // Check if the envelope is defined as a tool and available in ToolSvc
  std::cout<<"RETRIEVE GEOMETRY BUILDER TOOL   -  algTools " <<m_bConfigGeoAlgTool<<std::endl;
  if(m_bConfigGeoAlgTool){
    StatusCode sc = m_geomBuilderTool.retrieve();
    if(sc.isFailure())
      {
	msg(MSG::ERROR) << "Could not retrieve geometry builder tool " <<  m_geomBuilderTool << ",  some services will not be built." << endreq;
      } 
    else 
      {
	msg(MSG::INFO) << "Geometry builder tool retrieved: " << m_geomBuilderTool << endreq;
      }
  }
  else
    msg(MSG::INFO) << "Geometry builder tool not retrieved:  sequential building procees" << endreq;
    

  //
  // Set Version information
  //
  std::string versionTag  = m_geometryManager->versionTag();
  std::string versionName = m_geometryManager->versionName();
  std::string layout = m_geometryManager->versionLayout();
  std::string description = m_geometryManager->versionDescription() ;
  int versionMajorNumber = 5;
  int versionMinorNumber = 1;
  int versionPatchNumber = 0;

  if (m_geometryManager->InitialLayout()) {
    layout = "Initial";
  }
  
  InDetDD::Version version(versionTag,
			   versionName, 
			   layout, 
			   description, 
			   versionMajorNumber,
			   versionMinorNumber,
			   versionPatchNumber);
  m_detectorManager->setVersion(version);

}


PixelDetectorFactoryFastGeo::~PixelDetectorFactoryFastGeo()
{
  delete m_geometryManager;
}



//## Other Operations (implementation)
void PixelDetectorFactoryFastGeo::create(GeoPhysVol *world)
{
  if(msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Building Pixel Detector" << endreq;
    msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endreq;
  }
  // Printout the parameters that are different in DC1 and DC2.
  m_geometryManager->SetCurrentLD(0);
  m_geometryManager->SetBarrel();
  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " B-Layer basic eta pitch: " << m_geometryManager->DesignPitchZ()/CLHEP::micrometer << "um" << endreq;  
    msg(MSG::DEBUG) << " B-Layer sensor thickness: " << m_geometryManager->PixelBoardThickness()/CLHEP::micrometer << "um" << endreq;   
  }

  GeoVPhysVol* pephys = 0;
  
  // Switch to configurable geometry (anvelope define as AthAlgTools)
  if(m_bConfigGeoAlgTool)
    {

      OraclePixGeoAccessor geoAccessor( getAthenaComps(), "Pixel");
      
      // SES : add materials defined in PixelWeight tag
      InDetMaterialManager* mat_mgr = new InDetMaterialManager("PixelMaterialManager", getAthenaComps());
      IRDBRecordset_ptr weightTable = geoAccessor.getTable("PixelWeights");
      mat_mgr->addWeightTable(weightTable, "pix");
      IRDBRecordset_ptr scalingTable = geoAccessor.getTableInDet("PixelMatScaling");
      mat_mgr->addScalingTable(scalingTable);
      
      /*
      // Top level transform
      HepGeom::Transform3D topTransform = m_geometryManager->partTransform("Pixel");
      */
      
      Athena::MsgStreamMember msgStreamMember(Athena::Options::Eager,"StdMsgStream");
      
      // Create the Pixel Envelope...
      bool bGeoComponent = false;
      PixelGeoBuilderBasics* geoBasics = new PixelGeoBuilderBasics(&geoAccessor, mat_mgr, msgStreamMember,
								   bGeoComponent,
								   m_geometryManager->getIdHelper(),
								   m_detectorManager,
								   m_geometryManager->commonItems());
      if(bGeoComponent){
	InDet::GeoEnvelopeAlpine* envelope = dynamic_cast<InDet::GeoEnvelopeAlpine*>(m_geomBuilderTool->buildEnvelopeGeoComp(geoBasics));
	pephys = envelope->physVolume<GeoFullPhysVol>();
      }
      else
	pephys = (m_geomBuilderTool->buildEnvelope(geoBasics));
    } 

//   else if(m_geomLayoutName=="GeoPixelEnvelopeAlpine")
//     {
//       //
//       // Create the Pixel Envelope...
//       GeoPixelEnvelopeAlpine pe;
//       pephys = pe.Build() ;
//     }
//   else if(m_geomLayoutName=="GeoPixelEnvelopeLoI")
//     {
//       //
//       // Create the Pixel Envelope...
//       GeoPixelEnvelopeLoI pe;
//       pephys = pe.Build() ;
//     }

  // Top level transform
  HepGeom::Transform3D topTransform = m_geometryManager->partTransform("Pixel");
  
  GeoAlignableTransform * transform = new GeoAlignableTransform(topTransform);

  //
  // Add this to the world
  //
  GeoNameTag *tag = new GeoNameTag("Pixel");         
  world->add(tag);
  world->add(transform);
  world->add(pephys);

  // Store alignable transform
  Identifier id = m_geometryManager->getIdHelper()->wafer_id(0,0,0,0);
  m_detectorManager->addAlignableTransform(2, id, transform, pephys);

  //
  // Add this to the list of top level physical volumes:             
  //
  m_detectorManager->addTreeTop(pephys);                                       

  
  // Initialize the neighbours
  m_detectorManager->initNeighbours();

  // Set maximum rows/columns in numerology
  for (int iDesign = 0;  iDesign < m_detectorManager->numDesigns(); iDesign++) {
    m_detectorManager->numerology().setMaxNumPhiCells(m_detectorManager->getPixelDesign(iDesign)->rows());
    m_detectorManager->numerology().setMaxNumEtaCells(m_detectorManager->getPixelDesign(iDesign)->columns());
  }
  
  // Register the callbacks and keys and the level corresponding to the key.
  if (m_geometryManager->Alignable()) {
    m_detectorManager->addFolder("/Indet/Align");
    m_detectorManager->addChannel("/Indet/Align/ID",     2, InDetDD::global);
    m_detectorManager->addChannel("/Indet/Align/PIX",    1, InDetDD::global);
    m_detectorManager->addChannel("/Indet/Align/PIXB1",  0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXB2",  0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXB3",  0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXB4",  0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEA1", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEA2", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEA3", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEC1", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEC2", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEC3", 0, InDetDD::local);
  }

  // Check that there are no missing elements.
  // Bypass checks for standard ATLAS.
  if (m_geometryManager->ibl() || m_geometryManager->slhc()||m_bConfigGeoAlgTool) {
    doChecks();
  }

}

const PixelDetectorManager * PixelDetectorFactoryFastGeo::getDetectorManager() const
{
  return m_detectorManager;
}

void 
PixelDetectorFactoryFastGeo::doChecks()
{
  const PixelID * idHelper = m_geometryManager->athenaComps()->getIdHelper();
  const PixelDetectorManager * manager = m_detectorManager;

  SiDetectorElementMap* detElementMap = new SiDetectorElementMap();

  msg(MSG::INFO) << "###############################################################################" << endreq;
  msg(MSG::INFO) << "###############################################################################" << endreq;
  msg(MSG::INFO) << "Factory - FastGeo - Doing consistency checks." << endreq;

  unsigned int maxHash = idHelper->wafer_hash_max();

  int count = 0;
  int missingCount = 0;
  InDetDD::SiDetectorElementCollection::const_iterator iter;  
  for (iter = manager->getDetectorElementBegin(); iter != manager->getDetectorElementEnd(); ++iter){
    count++;
    const InDetDD::SiDetectorElement * element = *iter; 
    if (!element) {
      msg(MSG::WARNING) << "MISSING ELEMENT!!!!!!!!!!! - Element # " << count-1 << endreq;
      missingCount++;
    }
  }
  
  if (missingCount) { 
    msg(MSG::ERROR) << "There are missing elements in element array." << endreq;
    msg(MSG::INFO) << "Number of elements: " << count << endreq;
    msg(MSG::INFO) << "Number missing:     " << missingCount << endreq;
  } 


  // ***********************************************************************************
  //  Loop over modules
  // ***********************************************************************************

  // Barrel
  int barrelCount = 0;
  int barrelCountError = 0;

  std::cout<<"Number of barrel elements : "<<manager->numerology().numBarrels()<<std::endl;

  for (int iBarrelIndex = 0; iBarrelIndex < manager->numerology().numBarrels(); iBarrelIndex++) {
    int iBarrel = manager->numerology().barrelId(iBarrelIndex);

    std::cout<<"Number of layers elements : "<<manager->numerology().numLayers()<<std::endl;

    for (int iLayer = 0; iLayer < manager->numerology().numLayers(); iLayer++) {

      // TEMPORARY FIX
      // Temporary fix for IBL. Numerology class needs to be fixed.
      m_geometryManager->SetCurrentLD(iLayer);
      int etaCorrection = 0;
      if (!m_geometryManager->allowSkipEtaZero() && manager->numerology().skipEtaZeroForLayer(iLayer)) {
	//std::cout << "ETA CORRECTION" << std::endl;
	etaCorrection = 1;
      }
      // END TEMPORARY FIX

      if (manager->numerology().useLayer(iLayer)) {

	std::cout<<"  -> brwose layer "<<iLayer<<"  # modules "<<
	  manager->numerology().numPhiModulesForLayer(iLayer)<<" x "<<manager->numerology().endEtaModuleForLayer(iLayer)<<std::endl;

	for (int iPhi = 0; iPhi < manager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
	  for (int iEta = manager->numerology().beginEtaModuleForLayer(iLayer); 
	       iEta < manager->numerology().endEtaModuleForLayer(iLayer) - etaCorrection; 
	       iEta++) {
	    //if (!iEta && manager->numerology().skipEtaZeroForLayer(iLayer)) continue;
	    if (!etaCorrection && !iEta && manager->numerology().skipEtaZeroForLayer(iLayer)) continue; // TEMPORARY FIX
	    Identifier id = idHelper->wafer_id(iBarrel,iLayer,iPhi,iEta);
	    const InDetDD::SiDetectorElement * element = manager->getDetectorElement(id);
	    barrelCount++;
	    std::stringstream ostr;
	    ostr << "[2.1 . " << iBarrel << " . " << iLayer << " . " << iPhi << " . " << iEta << " . 0]"; 
	    if (!element) {
	      barrelCountError++;
	      msg(MSG::WARNING) << "   No element found for id: " << ostr.str() << " " << idHelper->show_to_string(id) << endreq;
	    }
	    else
	      detElementMap->addEntry(idHelper->wafer_hash(id), element);
	  } // iEta
	} //iPhi
      }
    } //iLayer
  } // Barrel


  //IdentifierHash      wafer_hash      (Identifier wafer_id) const;


  // Endcap
  int endcapCount = 0;
  int endcapCountError = 0;

  std::cout<<"Number of endcap elements : "<<manager->numerology().numEndcaps()<<std::endl;

  for (int iEndcapIndex = 0; iEndcapIndex < manager->numerology().numEndcaps(); iEndcapIndex++) {
    int iEndcap = manager->numerology().endcapId(iEndcapIndex);
    for (int iDisk = 0; iDisk < manager->numerology().numDisks(); iDisk++) {
      if (manager->numerology().useDisk(iDisk)) { 
	for (int iEta = 0; iEta < manager->numerology().numRingsForDisk(iDisk); iEta++) {
	  for (int iPhi = 0; iPhi < manager->numerology().numPhiModulesForDiskRing(iDisk,iEta); iPhi++) {
	    Identifier id = idHelper->wafer_id(iEndcap,iDisk,iPhi,iEta);
	    const InDetDD::SiDetectorElement * element = manager->getDetectorElement(id);
	    endcapCount++;
	    std::stringstream ostr;
	    ostr << "[2.1." << iEndcap << "." << iDisk << "." << iPhi << "." << iEta << ".0]"; 
	    if (!element) {
	      endcapCountError++;
	      msg(MSG::WARNING) << "    No element found for id: " << ostr.str() << " " << idHelper->show_to_string(id) << endreq;
	    }
	    else
	      detElementMap->addEntry(idHelper->wafer_hash(id), element);
	  } // iEta
	} //iPhi
      }
    } //iDisk
  } // Endcap;

  // Check DBM endcap modules
  int endcapCountDBM = 0;
  int endcapCountErrorDBM = 0;
  if (m_geometryManager->dbm()) {
    //    endcapCount += 24;

    // Endcap
    for (int iEndcapIndex = 0; iEndcapIndex < manager->numerology().numEndcapsDBM(); iEndcapIndex++) {
      int iEndcap = manager->numerology().endcapIdDBM(iEndcapIndex);
      for (int iDisk = 0; iDisk < manager->numerology().numDisksDBM(); iDisk++) {
	if (manager->numerology().useDiskDBM(iDisk)) { 
	  for (int iEta = 0; iEta < manager->numerology().numRingsForDiskDBM(iDisk); iEta++) {
	    for (int iPhi = 0; iPhi < manager->numerology().numPhiModulesForDiskRingDBM(iDisk,iEta); iPhi++) {
	      Identifier id = idHelper->wafer_id(iEndcap,iDisk,iPhi,iEta);
	      const InDetDD::SiDetectorElement * element = manager->getDetectorElement(id);
	      endcapCountDBM++;
	      std::stringstream ostr;
	      ostr << "[2.1." << iEndcap << "." << iDisk << "." << iPhi << "." << iEta << ".0]"; 
	      if (!element) {
		endcapCountErrorDBM++;
		msg(MSG::WARNING) << "    No element found for id (DBM): " << ostr.str() << " " << idHelper->show_to_string(id) << endreq;
	      }
	    } // iEta
	  } //iPhi
	}
      } //iDisk
    } // Endcap;
  }
  
  if (barrelCountError || endcapCountError || endcapCountErrorDBM) {
    msg(MSG::ERROR) << "There are elements which cannot be found."  << endreq;
    msg(MSG::INFO) << "Number of barrel elements not found : " << barrelCountError << endreq;
    msg(MSG::INFO) << "Number of pixel endcap elements not found : " << endcapCountError << endreq;
    msg(MSG::INFO) << "Number of DBM endcap elements not found : " << endcapCountErrorDBM << endreq;
  }

  if ( barrelCount+endcapCount+endcapCountDBM != int(maxHash)) { 
     msg(MSG::ERROR) << "Total count does not match maxHash." << endreq;
     msg(MSG::INFO) << "Number of barrel elements       : " << barrelCount << endreq;
     msg(MSG::INFO) << "Number of endcap elements       : " << endcapCount << endreq;
     msg(MSG::INFO) << "Number of endcap elements (DBM) : " << endcapCountDBM << endreq;
     msg(MSG::INFO) << "Total                           : " << barrelCount+endcapCount+endcapCountDBM << endreq;
     msg(MSG::INFO) << "MaxHash                         : " << maxHash << endreq;
  }

     msg(MSG::INFO) << "Number of barrel elements       : " << barrelCount << endreq;
     msg(MSG::INFO) << "Number of endcap elements       : " << endcapCount << endreq;
     msg(MSG::INFO) << "Number of endcap elements (DBM) : " << endcapCountDBM << endreq;
     msg(MSG::INFO) << "Total                           : " << barrelCount+endcapCount+endcapCountDBM << endreq;
     msg(MSG::INFO) << "MaxHash                         : " << maxHash << endreq;


  // Register SiDetectorElement map to store gate 
  if(detStore()->record(detElementMap, "SiDetectorElementMap").isFailure())
    std::cout<<"StoreGate - SiDetectorElement map : storegate registration failed"<<std::endl;
  else 
    std::cout<<"StoreGate - SiDetectorElement map : storegate registration successfull"<<std::endl;




//   if(detStore()->setConst(detElementMap).isFailure())
//     {
//       std::cout<<"StoreGate - SiDetectorElementMap : erreur setConst storegate"<<std::endl;
//     }


  msg(MSG::INFO) << "###############################################################################" << endreq;
  msg(MSG::INFO) << "###############################################################################" << endreq;

  detStore()->dump();
  
  msg(MSG::INFO) << "###############################################################################" << endreq;
  msg(MSG::INFO) << "###############################################################################" << endreq;
 
  // Retrieve DetElement maps
  SiDetectorElementMap* detElementMapRead;
  if(detStore()->retrieve(detElementMapRead, "SiDetectorElementMap").isFailure())
    msg(MSG::INFO)<<"Could not get SiDetectorElementMap"<< endreq;
  else
    msg(MSG::INFO)<<"Retrieve SiDetectorElementMap successfull"<< endreq;

  msg(MSG::INFO) << "###############################################################################" << endreq;
  msg(MSG::INFO) << "###############################################################################" << endreq;
 
}
