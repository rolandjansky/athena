/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelDetectorFactoryDC2.h"

// Envelope, as a starting point of the geometry
//#include "GeoPixelEnvelope.h"
#include "PixelDetectorDC1DC2.h"
#include "PixelSwitches.h" 

// GeoModel includes
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  

// InDetReadoutGeometry
#include "InDetReadoutGeometry/SiCommonItems.h" 
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "PixelGeoModelAthenaComps.h"

#include "InDetIdentifier/PixelID.h"

using InDetDD::PixelDetectorManager; 
using InDetDD::SiCommonItems; 

using namespace PixelGeoDC2;


PixelDetectorFactoryDC2::PixelDetectorFactoryDC2(const PixelGeoModelAthenaComps * athenaComps,
						 const PixelSwitches & switches)						 
  : InDetDD::DetectorFactoryBase(athenaComps),
    m_detectorManager(0)
{
  // Create the detector manager
  m_detectorManager = new PixelDetectorManager(detStore());
  GeoVPixelFactory::SetDDMgr(m_detectorManager);

  // Create the geometry manager.
  PixelGeometryManager * geometryManager =  PixelGeometryManager::GetPointer();
  
  // Pass the switches
  geometryManager->SetServices(switches.services());
  geometryManager->SetG3CompatibleDigits(switches.g3CompatibleDigits());
  geometryManager->SetDC1Geometry(switches.dc1Geometry());
  geometryManager->SetAlignable(switches.alignable());
  geometryManager->SetInitialLayout(switches.initialLayout());

  // Create SiCommonItems ans store it in geometry manager. 
  // These are items that are shared by all elements
  SiCommonItems * commonItems = new SiCommonItems(athenaComps->getIdHelper());
  geometryManager->setCommonItems(commonItems);
 
  bool initialLayoutIdDict = (m_detectorManager->tag() == "initial_layout");
  if (geometryManager->InitialLayout() != initialLayoutIdDict ) {
    if(msgLvl(MSG::WARNING)) 
      msg(MSG::WARNING) << "IdDict tag is \"" << m_detectorManager->tag() 
			<< "\" which is inconsistent with the layout choosen!"
			<< endmsg;
  } 

  
  //
  // Set Version information
  //
  std::string versionTag = geometryManager->versionTag();
  std::string versionName = "DC2";
  std::string layout = "Final";
  std::string description = "DC2 Geometry";
  int versionMajorNumber = 2;
  int versionMinorNumber = 2;
  int versionPatchNumber = 0;

  if (geometryManager->G3CompatibleDigits()) {
    description += ", G3 Compatible Digits";
    versionMinorNumber = 1;
  }

  if (geometryManager->InitialLayout()) {
    layout = "Initial";
  }
  
  // We determine if we are running DC1 geoemtry via 
  // Barrel version number in NOVA 
  if (geometryManager->DC1Geometry()) {
     versionName = "DC1";
     description = "DC1 Geometry (300um B-Layer pixels)";
     versionMajorNumber = 1;
     versionMinorNumber = 2;
     if (geometryManager->G3CompatibleDigits()) {
       description += ", G3 Compatible Digits";
       versionMinorNumber = 1;
     }
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


PixelDetectorFactoryDC2::~PixelDetectorFactoryDC2()
{

}



//## Other Operations (implementation)
void PixelDetectorFactoryDC2::create(GeoPhysVol *world)
{
  PixelGeometryManager * geometryManager =  PixelGeometryManager::GetPointer();

  if(msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Building Pixel Detector" << endmsg;
    msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;

    // Printout the parameters that are different in DC1 and DC2.
    msg(MSG::INFO) << " B-Layer basic eta pitch: " << geometryManager->DesignPitchZ(true)/Gaudi::Units::micrometer << "um" << endmsg;
  }  
  geometryManager->SetCurrentLD(0);
  geometryManager->SetBarrel();
  if(msgLvl(MSG::INFO)) 
    msg(MSG::INFO) << " B-Layer sensor thickness: " << geometryManager->PixelBoardThickness()/Gaudi::Units::micrometer << "um" << endmsg;   
  
  //
  // Create the Pixel Envelope...
  GeoPixelEnvelope pe;
  GeoVPhysVol* pephys = pe.Build() ;
  GeoAlignableTransform * transform = new GeoAlignableTransform(GeoTrf::Transform3D::Identity());
  
  //
  // Add this to the world
  //
  GeoNameTag *tag = new GeoNameTag("Pixel");         
  world->add(tag);
  world->add(transform);
  world->add(pephys);

  // Store alignable transform
  Identifier id = geometryManager->getIdHelper()->wafer_id(0,0,0,0);
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
  if (geometryManager->Alignable()) {
    m_detectorManager->addFolder("/Indet/Align");
    m_detectorManager->addChannel("/Indet/Align/ID",     2, InDetDD::global);
    m_detectorManager->addChannel("/Indet/Align/PIX",    1, InDetDD::global);
    m_detectorManager->addChannel("/Indet/Align/PIXB1",  0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXB2",  0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXB3",  0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEA1", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEA2", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEA3", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEC1", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEC2", 0, InDetDD::local);
    m_detectorManager->addChannel("/Indet/Align/PIXEC3", 0, InDetDD::local);
  }
}

const PixelDetectorManager * PixelDetectorFactoryDC2::getDetectorManager() const
{
  return m_detectorManager;
}

