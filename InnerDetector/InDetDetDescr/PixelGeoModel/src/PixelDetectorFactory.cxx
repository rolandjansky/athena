/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDetectorFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "PixelSwitches.h" 

// Envelope, as a starting point of the geometry
#include "GeoPixelEnvelope.h"

// GeoModel includes
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GaudiKernel/SystemOfUnits.h"

// InDetReadoutGeometry
#include "InDetReadoutGeometry/SiCommonItems.h" 
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "OraclePixGeoManager.h"
#include "PixelGeoModelAthenaComps.h"

#include "InDetIdentifier/PixelID.h"

#include "DetDescrConditions/AlignableTransformContainer.h"


using InDetDD::PixelDetectorManager; 
using InDetDD::SiCommonItems; 

PixelDetectorFactory::PixelDetectorFactory(const PixelGeoModelAthenaComps * athenaComps,
					   const PixelSwitches & switches)
  : InDetDD::DetectorFactoryBase(athenaComps),
    m_detectorManager(0),
    m_useDynamicAlignFolders(false)
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
  msg(MSG::INFO) << "DBM switch = SetDBMFlag: "<< m_geometryManager->dbm() << endmsg;
   
  // Create SiCommonItems ans store it in geometry manager. 
  // These are items that are shared by all elements
  SiCommonItems * commonItems = new SiCommonItems(athenaComps->getIdHelper());
  m_geometryManager->setCommonItems(commonItems);
 
  // Determine if initial layer and tag from the id dict are consistent
  bool initialLayoutIdDict = (m_detectorManager->tag() == "initial_layout");
  if (m_geometryManager->InitialLayout() != initialLayoutIdDict ) {
    if(msgLvl(MSG::WARNING)) 
      msg(MSG::WARNING) << "IdDict tag is \"" << m_detectorManager->tag() 
			<< "\" which is inconsistent with the layout choosen!"
			<< endmsg;
  } 


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

  m_useDynamicAlignFolders = switches.dynamicAlignFolders();

}


PixelDetectorFactory::~PixelDetectorFactory()
{
  delete m_geometryManager;
}



//## Other Operations (implementation)
void PixelDetectorFactory::create(GeoPhysVol *world)
{
  if(msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Building Pixel Detector" << endmsg;
    msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;
  }
  // Printout the parameters that are different in DC1 and DC2.
  m_geometryManager->SetCurrentLD(0);
  m_geometryManager->SetBarrel();
  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " B-Layer basic eta pitch: " << m_geometryManager->DesignPitchZ()/Gaudi::Units::micrometer << "um" << endmsg;  
    msg(MSG::DEBUG) << " B-Layer sensor thickness: " << m_geometryManager->PixelBoardThickness()/Gaudi::Units::micrometer << "um" << endmsg;   
  }

  // Top level transform
  GeoTrf::Transform3D topTransform = m_geometryManager->partTransform("Pixel");


  //
  // Create the Pixel Envelope...
  GeoPixelEnvelope pe;
  GeoVPhysVol* pephys = pe.Build() ;
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

    if (!m_useDynamicAlignFolders){
      m_detectorManager->addAlignFolderType(InDetDD::static_run1);
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
    
    else {
      m_detectorManager->addAlignFolderType(InDetDD::timedependent_run2);
      m_detectorManager->addGlobalFolder("/Indet/AlignL1/ID");
      m_detectorManager->addGlobalFolder("/Indet/AlignL2/PIX");
      m_detectorManager->addChannel("/Indet/AlignL1/ID",     2, InDetDD::global);
      m_detectorManager->addChannel("/Indet/AlignL2/PIX",    1, InDetDD::global);
      m_detectorManager->addFolder("/Indet/AlignL3");
      m_detectorManager->addChannel("/Indet/AlignL3/PIXB1",  0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXB2",  0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXB3",  0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXB4",  0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXEA1", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXEA2", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXEA3", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXEC1", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXEC2", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/AlignL3/PIXEC3", 0, InDetDD::local);
    }

    // This is the new LB-IOV sensitive IBL bowing DB
    m_detectorManager->addSpecialFolder("/Indet/IBLDist");

  }

  // Check that there are no missing elements.
  // Bypass checks for standard ATLAS.
  if (m_geometryManager->ibl() || m_geometryManager->slhc()) {
    doChecks();
  }
}

const PixelDetectorManager * PixelDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

void 
PixelDetectorFactory::doChecks()
{
  const PixelID * idHelper = m_geometryManager->athenaComps()->getIdHelper();
  const PixelDetectorManager * manager = m_detectorManager;

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


  // ***********************************************************************************
  //  Loop over modules
  // ***********************************************************************************

  // Barrel
  int barrelCount = 0;
  int barrelCountError = 0;
  for (int iBarrelIndex = 0; iBarrelIndex < manager->numerology().numBarrels(); iBarrelIndex++) {
    int iBarrel = manager->numerology().barrelId(iBarrelIndex);
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
	      msg(MSG::WARNING) << "   No element found for id: " << ostr.str() << " " << idHelper->show_to_string(id) << endmsg;
	    }
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
	    Identifier id = idHelper->wafer_id(iEndcap,iDisk,iPhi,iEta);
	    const InDetDD::SiDetectorElement * element = manager->getDetectorElement(id);
	    endcapCount++;
	    std::stringstream ostr;
	    ostr << "[2.1." << iEndcap << "." << iDisk << "." << iPhi << "." << iEta << ".0]"; 
	    if (!element) {
	      endcapCountError++;
	      msg(MSG::WARNING) << "    No element found for id: " << ostr.str() << " " << idHelper->show_to_string(id) << endmsg;
	    }
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
		msg(MSG::WARNING) << "    No element found for id (DBM): " << ostr.str() << " " << idHelper->show_to_string(id) << endmsg;
	      }
	    } // iEta
	  } //iPhi
	}
      } //iDisk
    } // Endcap;
  }
  
  if (barrelCountError || endcapCountError || endcapCountErrorDBM) {
    msg(MSG::ERROR) << "There are elements which cannot be found."  << endmsg;
    msg(MSG::INFO) << "Number of barrel elements not found : " << barrelCountError << endmsg;
    msg(MSG::INFO) << "Number of pixel endcap elements not found : " << endcapCountError << endmsg;
    msg(MSG::INFO) << "Number of DBM endcap elements not found : " << endcapCountErrorDBM << endmsg;
  }

  if ( barrelCount+endcapCount+endcapCountDBM != int(maxHash)) { 
     msg(MSG::ERROR) << "Total count does not match maxHash." << endmsg;
     msg(MSG::INFO) << "Number of barrel elements       : " << barrelCount << endmsg;
     msg(MSG::INFO) << "Number of endcap elements       : " << endcapCount << endmsg;
     msg(MSG::INFO) << "Number of endcap elements (DBM) : " << endcapCountDBM << endmsg;
     msg(MSG::INFO) << "Total                           : " << barrelCount+endcapCount+endcapCountDBM << endmsg;
     msg(MSG::INFO) << "MaxHash                         : " << maxHash << endmsg;
  }

     msg(MSG::INFO) << "Number of barrel elements       : " << barrelCount << endmsg;
     msg(MSG::INFO) << "Number of endcap elements       : " << endcapCount << endmsg;
     msg(MSG::INFO) << "Number of endcap elements (DBM) : " << endcapCountDBM << endmsg;
     msg(MSG::INFO) << "Total                           : " << barrelCount+endcapCount+endcapCountDBM << endmsg;
     msg(MSG::INFO) << "MaxHash                         : " << maxHash << endmsg;

}


