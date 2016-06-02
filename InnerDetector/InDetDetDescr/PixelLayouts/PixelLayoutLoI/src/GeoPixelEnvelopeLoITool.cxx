/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelLayoutLoI/GeoPixelEnvelopeLoITool.h"

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


GeoPixelEnvelopeLoITool::GeoPixelEnvelopeLoITool(const std::string& type, const std::string& name, const IInterface*  parent )
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

StatusCode GeoPixelEnvelopeLoITool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    {
      ATH_MSG_INFO("GeoPixelEnvelopeLoITool initialize() failed " << name());
      return sc;
    }
  ATH_MSG_INFO("GeoPixelEnvelopeLoITool initialize() successful in " << name());

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

StatusCode GeoPixelEnvelopeLoITool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelEnvelopeLoITool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelEnvelopeLoITool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

GeoVPhysVol* GeoPixelEnvelopeLoITool::Build( ) {
  return 0;
}

GeoVPhysVol* GeoPixelEnvelopeLoITool::buildEnvelope(const PixelGeoBuilderBasics* basics)
{
  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  // Add materials defined in XML files
  // Load material file 
  std::string fileName="Material.xml";
  if(const char* env_p = std::getenv("PIXEL_MATERIAL_GEO_XML")) fileName = std::string(env_p);
  GeoPixelXMLMaterial matBuilder(basics,fileName);
  matBuilder.Build();

  // Envelope interface
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  std::cout<<"GeoEnvelopeLoI : GeoVPhysVol* GeoPixelEnvelopeAlpineTool::Build( )"<<std::endl;

  bool barrelPresent   = genDBHelper.isBarrelPresent();
  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  // Service regions : defines the envelopes used to place the automated defined services 
  //    barrel and endcap preBuild => defines barrel and endcaps envelopes
  if(m_IDserviceTool){
    std::vector<InDetDD::TubeZone*> svcRegion_brl;
    std::vector<InDetDD::TubeZone*> svcRegion_ec;
    if(m_barrelTool){
      m_barrelTool->preBuild(basics);
      svcRegion_brl = m_barrelTool->getSvcRegions();
    }
    if(m_endcapTool){
      m_endcapTool->preBuild(basics);
      svcRegion_ec = m_endcapTool->getSvcRegions();
    }
    
    std::cout<<"Collected svcRegion : b/ec : "<<svcRegion_brl.size()<<" "<<svcRegion_ec.size()<<std::endl;
    
    // Build the services (PixelServicesTool)
    svcRegion_brl.insert( svcRegion_brl.end(), svcRegion_ec.begin(), svcRegion_ec.end());
    std::vector<InDetDD::TubeZone*> svcRegions =  SortServiceRegions(svcRegion_brl);
    m_IDserviceTool->buildServices(basics, svcRegions);
  }
  
  //
  // create the External volume where I'll put all the pixel detector
  // 
  const GeoShape * envelopeShape;

  double rmin = genDBHelper.getEnvelopeRMin();
  double rmax = genDBHelper.getEnvelopeRMax();
  double halflength = genDBHelper.getEnvelopeHalfLength();
  envelopeShape = new GeoTube(rmin,rmax,halflength);

  const GeoMaterial* air = m_matMgr->getMaterial("std::Air");
  const GeoLogVol* envelopeLog = new GeoLogVol("Pixel",envelopeShape,air);
  GeoFullPhysVol* envelopePhys = new GeoFullPhysVol(envelopeLog);;

  //
  // Build barrel
  //
  if (barrelPresent) {
    
    // register barrel
    basics->getDetectorManager()->numerology().addBarrel(0);

    // Add the Barrel:
    //
    HepGeom::Transform3D barrelTransform = HepGeom::Transform3D();

    GeoNameTag* tag = new GeoNameTag("Barrel");
    GeoVPhysVol* barrelPhys =  m_barrelTool->buildBarrel(basics) ;
    envelopePhys->add(tag);
    envelopePhys->add(new GeoIdentifierTag(0));
    envelopePhys->add(new GeoTransform(barrelTransform));
    envelopePhys->add(barrelPhys );

  }
  std::cout<<"*****************************************************************************"<<std::endl;
  std::cout<<"*****************************************************************************"<<std::endl;


  //
  // Add the EndCaps
  //
  if (endcapAPresent || endcapCPresent) {

    // EndCap A
    if (endcapAPresent) {

      if(!m_endcapTool) {
	std::cout<<"No endcap defined on side "<<std::endl;
      }
      else{

	// register endcap A
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
	// register endcap C
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

  // Place pixel envelope services  
  if(m_IDserviceTool) m_IDserviceTool->buildAndPlace("Pixel", envelopePhys);

  return envelopePhys;
  
}


std::vector<InDetDD::TubeZone*> GeoPixelEnvelopeLoITool::SortServiceRegions(std::vector<InDetDD::TubeZone*>& svcRegions )
{

  std::vector<InDetDD::TubeZone*> newSvcRegions;

  std::cout<<"Collected svcRegion - initial number : "<<svcRegions.size()<<std::endl;
  
  for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); it++)
    {
      bool bAlreadyExists=false;
      for(std::vector<InDetDD::TubeZone*>::iterator itnew=newSvcRegions.begin(); itnew!=newSvcRegions.end()&&!bAlreadyExists; itnew++)
	{
	  if((*it)->label()==(*itnew)->label()) bAlreadyExists=true;
	}
      if(!bAlreadyExists){
	newSvcRegions.push_back((*it));
	std::cout<<"Collected svcRegion : "<<(*it)->label()<<" "<<(*it)->getZmin()<<" "<<(*it)->getZmax()<<" / "<<(*it)->getRmin()<<" "<<(*it)->getRmax()<<" / "<<std::endl;
      }
      else 
	{
	  std::cout<<"Collected svcRegion - already defined : "<<(*it)->label()<<std::endl;	  
	}
    }
  
  return newSvcRegions;
}

