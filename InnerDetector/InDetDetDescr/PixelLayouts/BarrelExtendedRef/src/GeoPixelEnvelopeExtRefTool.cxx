/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BarrelExtendedRef/GeoPixelEnvelopeExtRefTool.h"

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelInterfaces/IGeoPixelBarrelTool.h"
#include "PixelInterfaces/IGeoPixelEndcapTool.h"
#include "PixelInterfaces/IPixelServicesTool.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "PixelLayoutUtils/GeoPixelXMLMaterial.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "GeoModelInterfaces/IGeoSubDetTool.h"
#include "GaudiKernel/StatusCode.h"


GeoPixelEnvelopeExtRefTool::GeoPixelEnvelopeExtRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_barrelTool("GeoPixelBarrelTool"),
    m_endcapTool("GeoPixelEndcapTool"),
    m_IDserviceTool()

{
  declareInterface<IGeoPixelEnvelopeTool>(this);

  //default settings
  declareProperty("GeoPixelBarrelTool",m_barrelTool);
  declareProperty("GeoPixelEndcapTool",m_endcapTool);
  declareProperty("PixelServicesTool", m_IDserviceTool);
}


//================ Initialisation =================================================

StatusCode GeoPixelEnvelopeExtRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    {
      ATH_MSG_INFO("GeoPixelEnvelopeExtRefTool initialize() failed " << name());
      return sc;
    }
  ATH_MSG_INFO("GeoPixelEnvelopeExtRefTool initialize() successful in " << name());

  sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelEnvelopeExtRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelEnvelopeExtRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelEnvelopeExtRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

GeoVPhysVol* GeoPixelEnvelopeExtRefTool::Build( ) {
  return 0;
}

GeoVPhysVol* GeoPixelEnvelopeExtRefTool::buildEnvelope(const PixelGeoBuilderBasics* basics)
{

  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  // Add materials defined in XML files
  // Load material file 
  std::string fileName="Material.xml";
  if(const char* env_p = std::getenv("PIXEL_MATERIAL_GEO_XML")) fileName = std::string(env_p);

  // build material defined in XML file
  GeoPixelXMLMaterial matBuilder(basics,fileName);
  matBuilder.Build();

  // Pixel general envelope XML reader
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");
  bool barrelPresent   = genDBHelper.isBarrelPresent();
  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  // Service regions : defines the envelopes used to place the automated defined services 
  //    barrel and endcap preBuild => defines barrel and endcaps envelopes
  if(m_IDserviceTool){
    std::vector<InDetDD::TubeZone*> svcRegion_brl;
    std::vector<InDetDD::TubeZone*> svcRegion_ec;
    if(m_barrelTool){
      m_barrelTool->preBuild();
      svcRegion_brl = m_barrelTool->getSvcRegions();
    }
    if(m_endcapTool){
      m_endcapTool->preBuild(basics);
      svcRegion_ec = m_endcapTool->getSvcRegions();
    }
    
    std::cout<<"Collected svcRegion : b/ec : "<<svcRegion_brl.size()<<" "<<svcRegion_ec.size()<<std::endl;
    
    svcRegion_brl.insert( svcRegion_brl.end(), svcRegion_ec.begin(), svcRegion_ec.end());
    std::vector<InDetDD::TubeZone*> svcRegions =  SortServiceRegions(svcRegion_brl);

    // Build services
    m_IDserviceTool->buildServices(basics, svcRegions);
  }
  
  //
  // create the External volume where I'll put all the pixel detector
  // This can either be a simple tube or a more general PCON.
  // 
  const GeoShape * envelopeShape;

  double rmin = genDBHelper.getEnvelopeRMin();
  double rmax = genDBHelper.getEnvelopeRMax();
  double halflength = genDBHelper.getEnvelopeHalfLength();
  envelopeShape = new GeoTube(rmin,rmax,halflength);

  const GeoMaterial* air = m_matMgr->getMaterial("std::Air");
  const GeoLogVol* envelopeLog = new GeoLogVol("Pixel",envelopeShape,air);
  GeoFullPhysVol* envelopePhys = new GeoFullPhysVol(envelopeLog);;

  // --------------------------------------------------------------------------
  // build barrel 
  //
  if (barrelPresent) {
    
    //    DDmgr->numerology().addBarrel(0); 
    basics->getDetectorManager()->numerology().addBarrel(0);
   //
    // Add the Barrel:
    //
    HepGeom::Transform3D barrelTransform = HepGeom::Transform3D();
    
    //    gmt_mgr->SetBarrel();

    GeoNameTag* tag = new GeoNameTag("Barrel");
    GeoVPhysVol* barrelPhys =  m_barrelTool->buildBarrel(basics) ;
    envelopePhys->add(tag);
    envelopePhys->add(new GeoIdentifierTag(0));
    envelopePhys->add(new GeoTransform(barrelTransform));
    envelopePhys->add(barrelPhys );

  }
  std::cout<<"*****************************************************************************"<<std::endl;
  std::cout<<"*****************************************************************************"<<std::endl;

  // --------------------------------------------------------------------------
  // build endcapsbarrel 
  // 
  if (endcapAPresent || endcapCPresent) {

    // EndCap A
    if (endcapAPresent) {

      if(!m_endcapTool) {
	std::cout<<"No endcap defined on side "<<std::endl;
      }
      else{
	// Numerology
	basics->getDetectorManager()->numerology().addEndcap(2);
	
	HepGeom::Transform3D endcapATransform = HepGeom::Transform3D();  //gmt_mgr->partTransform("EndcapA");
	
	GeoVPhysVol* envPhysA =  m_endcapTool->buildEndcap(basics,1) ;
	double zpos = (m_endcapTool->getEndcapZMax()+m_endcapTool->getEndcapZMin())*.5;
	
	GeoTransform* xform = new GeoTransform(endcapATransform * HepGeom::TranslateZ3D(zpos));
	GeoNameTag* tag  = new GeoNameTag("EndCapA");
	envelopePhys->add(tag);
	envelopePhys->add(new GeoIdentifierTag(2));
	envelopePhys->add(xform);
	envelopePhys->add(envPhysA);
      }
    }
    
    // EndCap C
    if (endcapCPresent) {
      
      if(!m_endcapTool) {
	std::cout<<"No endcap defined on side "<<std::endl;
      }
      else{
	// Numerology
	basics->getDetectorManager()->numerology().addEndcap(-2);
	
	HepGeom::Transform3D endcapCTransform = HepGeom::Transform3D();  //gmt_mgr->partTransform("EndcapC");
	
	GeoVPhysVol* envPhysC =  m_endcapTool->buildEndcap(basics,-1) ;
	double zpos = (m_endcapTool->getEndcapZMax()+m_endcapTool->getEndcapZMin())*.5;

	GeoTransform* xform = new GeoTransform(endcapCTransform * HepGeom::TranslateZ3D(-zpos) *  HepGeom::RotateY3D(180*CLHEP::deg));
	GeoNameTag* tag  = new GeoNameTag("EndCapC");
	envelopePhys->add(tag);
	envelopePhys->add(new GeoIdentifierTag(-2));
	envelopePhys->add(xform);
	envelopePhys->add(envPhysC);
      }
    }
  }
  
  //   // Extra Material 
  //   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
  //   xMat.add(envelopePhys,"Pixel");

  // Place pixel envelope services - static and dynamic services
  if(m_IDserviceTool){
    double zOffset = 0.;
    std::vector<std::string> svcList;    
    m_IDserviceTool->buildAndPlace("P", envelopePhys, zOffset, svcList, true, true);
  }

  return envelopePhys;
  
}


std::vector<InDetDD::TubeZone*> GeoPixelEnvelopeExtRefTool::SortServiceRegions(std::vector<InDetDD::TubeZone*>& svcRegions )
{

  // Sort service regions and remove duplicate defined regions

  std::vector<InDetDD::TubeZone*> newSvcRegions;  
  for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); it++)
    {
      bool bAlreadyExists=false;
      for(std::vector<InDetDD::TubeZone*>::iterator itnew=newSvcRegions.begin(); itnew!=newSvcRegions.end()&&!bAlreadyExists; itnew++)
	{
	  if((*it)->label()==(*itnew)->label()) bAlreadyExists=true;
	}
      if(!bAlreadyExists){
	newSvcRegions.push_back((*it));
	std::cout<<"Collected region - sorted "<<(*it)->label()<<std::endl;
      }
    }
  
  return newSvcRegions;
}

