/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDetectorFactorySR1.h"

#include "PixelSwitches.h" 

// Envelope, as a starting point of the geometry
#include "GeoPixelBarrel.h"
#include "GeoPixelEndCap.h"
#include "GeoPixelServices.h"

// GeoModel includes
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GaudiKernel/SystemOfUnits.h"

// InDetReadoutGeometry
#include "InDetReadoutGeometry/SiCommonItems.h" 
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "PixelGeoModelAthenaComps.h"
#include "OraclePixGeoManager.h"

#include "InDetIdentifier/PixelID.h"

using InDetDD::PixelDetectorManager; 
using InDetDD::SiCommonItems; 

PixelDetectorFactorySR1::PixelDetectorFactorySR1(const PixelGeoModelAthenaComps * athenaComps,
						 const PixelSwitches & switches)
  : InDetDD::DetectorFactoryBase(athenaComps),
    m_detectorManager(0)
{
  // Create the detector manager
  m_detectorManager = new PixelDetectorManager(detStore());
  GeoVPixelFactory::SetDDMgr(m_detectorManager);

  // Create the geometry manager.
  m_geometryManager =  new OraclePixGeoManager(athenaComps);
  GeoVPixelFactory::setGeometryManager(m_geometryManager);
  
  // Pass the switches
  m_geometryManager->SetServices(switches.services());
  m_geometryManager->SetServicesOnLadder(switches.servicesOnLadder()); //JBdV
  m_geometryManager->SetDC1Geometry(switches.dc1Geometry());
  m_geometryManager->SetAlignable(switches.alignable());
  m_geometryManager->SetInitialLayout(switches.initialLayout());


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
  std::string versionTag = m_geometryManager->versionTag();
  std::string versionName = "SR1";
  std::string layout = "SR1";
  std::string description = "SR1 Geometry";
  int versionMajorNumber = 4;
  int versionMinorNumber = 0;
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


PixelDetectorFactorySR1::~PixelDetectorFactorySR1()
{
  delete m_geometryManager;
}



//## Other Operations (implementation)
void PixelDetectorFactorySR1::create(GeoPhysVol *world)
{
  m_geometryManager->SetCurrentLD(0);
  m_geometryManager->SetBarrel();
  if(msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Building Pixel Detector" << endmsg;
    msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;

    // Printout the parameters that are different in DC1 and DC2.
    msg(MSG::INFO) << " B-Layer basic eta pitch: " << m_geometryManager->DesignPitchZ()/Gaudi::Units::micrometer << "um" << endmsg;
    msg(MSG::INFO) << " B-Layer sensor thickness: " << m_geometryManager->PixelBoardThickness()/Gaudi::Units::micrometer << "um" << endmsg;   
  }  

  bool barrelPresent   = m_geometryManager->partPresent("Barrel");
  bool endcapAPresent  = m_geometryManager->partPresent("EndcapA");
  bool endcapCPresent  = m_geometryManager->partPresent("EndcapC");

  // Check that there is one and only one part built.
  // Could have more than one part but then need to decide
  // what to do about top level alignable transform.
  
  if ((barrelPresent && endcapAPresent) ||
      (barrelPresent && endcapCPresent) ||
      (endcapAPresent && endcapCPresent)) {
    msg(MSG::ERROR) << "SR1 geometry can only have 1 part" << endmsg;
  }

  if (!barrelPresent && !endcapAPresent && !endcapCPresent) {
    msg(MSG::ERROR) << "SR1 geometry must have 1 part" << endmsg;
  }

  GeoVPhysVol* physVol = 0;   
  GeoAlignableTransform * transform = 0;

  GeoPixelServices * pixServices = 0;
  if(m_geometryManager->DoServices() ) {
    pixServices = new GeoPixelServices(0);
  } 
  
  // Top level transform
  GeoTrf::Transform3D topTransform = m_geometryManager->partTransform("Pixel");

  if (barrelPresent) {
    //
    // Add the Barrel:
    //
    m_geometryManager->SetBarrel();
    GeoPixelBarrel brl(pixServices);
    physVol = brl.Build();   

    GeoTrf::Transform3D barrelTransform = m_geometryManager->partTransform("Barrel");
    transform = new GeoAlignableTransform(topTransform*barrelTransform);

    // Add this to the world
    GeoNameTag* tag = new GeoNameTag("Pixel");
    world->add(tag);
    world->add(new GeoIdentifierTag(0));
    world->add(physVol);
    world->add(transform);

  }
  
  //
  // Add the EndCaps
  if (endcapAPresent || endcapCPresent) {
    m_geometryManager->SetEndcap();

    GeoPixelEndCap pec(pixServices);
    double zpos = (m_geometryManager->PixelEndcapZMax()+m_geometryManager->PixelEndcapZMin())/2.;
    

    // EndCap A
    if (endcapAPresent) {
    
      m_geometryManager->SetPos();
      physVol = pec.Build();
      
      GeoTrf::Transform3D endcapATransform = m_geometryManager->partTransform("EndcapA");
      transform = new GeoAlignableTransform(topTransform * endcapATransform * GeoTrf::TranslateZ3D(zpos));
      
      GeoNameTag* tag  = new GeoNameTag("Pixel");
      world->add(tag);
      world->add(new GeoIdentifierTag(2));
      world->add(transform);
      world->add(physVol);
    }
    
   // EndCap C
    if (endcapCPresent) {
    
      m_geometryManager->SetNeg();
      physVol = pec.Build();
      
      GeoTrf::Transform3D endcapCTransform = m_geometryManager->partTransform("EndcapC");
      transform = new GeoAlignableTransform(topTransform * endcapCTransform * GeoTrf::TranslateZ3D(-zpos) * GeoTrf::RotateY3D(180*Gaudi::Units::deg));
      
      GeoNameTag* tag  = new GeoNameTag("Pixel");
      world->add(tag);
      world->add(new GeoIdentifierTag(-2));
      world->add(transform);
      world->add(physVol);
    }

  }
  
  // Delete pixServices 
  if(pixServices) delete pixServices;

  // Should not happen as we check before that we have at least one part built.
  if (!physVol) {
    msg(MSG::ERROR) << "Unexpected condition. Top level volume has zero pointer" << endmsg;
  }

  // Store alignable transform
  Identifier id = m_geometryManager->getIdHelper()->wafer_id(0,0,0,0);
  m_detectorManager->addAlignableTransform(2, id, transform, physVol);

  //
  // Add this to the list of top level physical volumes:             
  //
  m_detectorManager->addTreeTop(physVol);                                       
  
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
    if (barrelPresent) {
      m_detectorManager->addChannel("/Indet/Align/PIXB1",  0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/PIXB2",  0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/PIXB3",  0, InDetDD::local);
    }
    if (endcapAPresent) {
      m_detectorManager->addChannel("/Indet/Align/PIXEA1", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/PIXEA2", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/PIXEA3", 0, InDetDD::local);
    } 
    if (endcapCPresent) {
      m_detectorManager->addChannel("/Indet/Align/PIXEC1", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/PIXEC2", 0, InDetDD::local);
      m_detectorManager->addChannel("/Indet/Align/PIXEC3", 0, InDetDD::local);
    }
  }
}

const PixelDetectorManager * PixelDetectorFactorySR1::getDetectorManager() const
{
  return m_detectorManager;
}

