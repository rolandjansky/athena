/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BarrelInclinedRef/GeoPixelEnvelopeInclRefTool.h"

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


GeoPixelEnvelopeInclRefTool::GeoPixelEnvelopeInclRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
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

StatusCode GeoPixelEnvelopeInclRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    {
      ATH_MSG_INFO("GeoPixelEnvelopeInclRefTool initialize() failed " << name());
      return sc;
    }
  ATH_MSG_INFO("GeoPixelEnvelopeInclRefTool initialize() successful in " << name());

  sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    ATH_MSG_ERROR("Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built.");
  }
  else{
    ATH_MSG_INFO("Service builder tool retrieved: " << m_IDserviceTool);
  }

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelEnvelopeInclRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelEnvelopeInclRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelEnvelopeInclRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

GeoVPhysVol* GeoPixelEnvelopeInclRefTool::Build( ) {
  return 0;
}

GeoVPhysVol* GeoPixelEnvelopeInclRefTool::buildEnvelope(const PixelGeoBuilderBasics* basics)
{
  //
  // Control which parts get built. This is mainly of use for
  // Cosmic runs where the whole detector is not present
  //

  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  // Add materials defined in XML files
  // Load material file 
  std::string fileName="Material.xml";
  if(const char* env_p = std::getenv("PIXEL_MATERIAL_GEO_XML")) fileName = std::string(env_p);
  ATH_MSG_DEBUG("SES - XML "<<fileName);

  GeoPixelXMLMaterial matBuilder(basics,fileName);
  matBuilder.Build();

  // Envelope interface
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML", basics);

  std::cout<<"GeoEnvelopeInclRef : GeoVPhysVol* GeoPixelEnvelopeAlpineTool::Build( )"<<std::endl;

  bool barrelPresent   = genDBHelper.isBarrelPresent();
  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  // Service regions
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
    
    ATH_MSG_DEBUG("Collected svcRegion : b/ec : "<<svcRegion_brl.size()<<" "<<svcRegion_ec.size());
    
    svcRegion_brl.insert( svcRegion_brl.end(), svcRegion_ec.begin(), svcRegion_ec.end());
    std::vector<InDetDD::TubeZone*> svcRegions =  SortServiceRegions(svcRegion_brl);
    m_IDserviceTool->buildServices(basics, svcRegions);
  }
  
   // NB. We apply the part transforms only to the active parts.
  // In general though they should always be zero. It would make more sense to apply
  // a transform to the overl pixel if one wants to move the detector.

  //
  // create the External volume where I'll put all the pixel detector
  // This can either be a simple tube or a more general PCON.
  // 
  const GeoShape * envelopeShape;
  //  InDetDD::Zone * pixZone = 0; 

  double rmin = genDBHelper.getEnvelopeRMin();
  double rmax = genDBHelper.getEnvelopeRMax();
  double halflength = genDBHelper.getEnvelopeHalfLength();
  envelopeShape = new GeoTube(rmin,rmax,halflength);
  //  pixZone = new InDetDD::TubeZone("Pixel",-halflength,halflength,rmin,rmax);

  const GeoMaterial* air = m_matMgr->getMaterial("std::Air");
  const GeoLogVol* envelopeLog = new GeoLogVol("Pixel",envelopeShape,air);
  GeoFullPhysVol* envelopePhys = new GeoFullPhysVol(envelopeLog);;


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

//     GeoPixelBarrel brl(pixServices);
//     GeoNameTag* tag = new GeoNameTag("Barrel");
//     GeoVPhysVol* barrelPhys =  brl.Build() ;
//     envelopePhys->add(tag);
//     envelopePhys->add(new GeoIdentifierTag(0));
//     envelopePhys->add(new GeoTransform(barrelTransform));
//     envelopePhys->add(barrelPhys );
  }


  //
  // Add the EndCaps
  if (endcapAPresent || endcapCPresent) {

    //    gmt_mgr->SetEndcap();
    // EndCap A
    if (endcapAPresent) {

      if(!m_endcapTool) {
	ATH_MSG_WARNING("No endcap defined on side ");
      }
      else{

	//      DDmgr->numerology().addEndcap(2);
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
	ATH_MSG_WARNING("No endcap defined on side ");
      }
      else{
	//      DDmgr->numerology().addEndcap(-2);
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

  //   pixServices = false;
  
//     if(pixServices) {
//       //
//       // Add the Barrel and endcap services outside the barrel and endcap volume
//       //
//       pixServices->getBuilder()->buildAndPlace("Pixel", envelopePhys);
//       if (pixServices->getServMatBuilder()) {
//         pixServices->getServMatBuilder()->buildAndPlace("Pixel", envelopePhys);
//       }
  
  //   // Extra Material 
  //   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
  //   xMat.add(envelopePhys,"Pixel");
  
  // //  // reset BarrelEndcap value to zero
  // //  gmt_mgr->SetBarrel();
  
  // Place pixel envelope services - static and dynamic services
  // if(m_IDserviceTool) m_IDserviceTool->buildAndPlace("Pixel", envelopePhys);
  if(m_IDserviceTool){
    double zOffset = 0.;
    std::vector<std::string> svcList;    
    m_IDserviceTool->buildAndPlace("P", envelopePhys, zOffset, svcList, true, true);
  }

  return envelopePhys;
  
}


std::vector<InDetDD::TubeZone*> GeoPixelEnvelopeInclRefTool::SortServiceRegions(std::vector<InDetDD::TubeZone*>& svcRegions )
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
	ATH_MSG_DEBUG("Collected svcRegion : "<<(*it)->label()); //<< " " << (*it)->getZmin()<<" "<<(*it)->getZmax()<<" / "<<(*it)->getRmin()<<" "<<(*it)->getRmax()<<" / ");
      }
      else 
	{
	  ATH_MSG_DEBUG("Collected svcRegion - already defined : "<<(*it)->label());	  
	}
    }
  
  return newSvcRegions;
}

