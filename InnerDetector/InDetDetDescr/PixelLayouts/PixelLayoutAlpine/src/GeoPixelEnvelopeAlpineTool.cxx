/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelLayoutAlpine/GeoPixelEnvelopeAlpineTool.h"
#include "PixelLayoutAlpine/GeoPixelBarrelAlpineTool.h"

#include "PixelLayoutUtils/GeoPixelXMLMaterial.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

/*#include "PixelLayoutAlpine/GeoPixelEndCap.h"*/
/*#include "PixelLayoutAlpine/GeoPixelServices.h"*/

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "PixelInterfaces/IGeoPixelBarrelTool.h"
#include "PixelInterfaces/IGeoPixelEndcapTool.h"
#include "PixelInterfaces/IPixelServicesTool.h"

#include "PixelGeoComponent/GeoBarrelAlpine.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "GeoModelInterfaces/IGeoSubDetTool.h"
#include "GaudiKernel/StatusCode.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"

GeoPixelEnvelopeAlpineTool::GeoPixelEnvelopeAlpineTool(const std::string& type, const std::string& name, const IInterface*  parent )
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

StatusCode GeoPixelEnvelopeAlpineTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    {
      ATH_MSG_INFO("GeoPixelEnvelopeAlpineTool initialize() failed " << name());
      return sc;
    }
  ATH_MSG_INFO("GeoPixelEnvelopeAlpineTool initialize() successful in " << name());

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

StatusCode GeoPixelEnvelopeAlpineTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelEnvelopeAlpineTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelEnvelopeAlpineTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

GeoVPhysVol* GeoPixelEnvelopeAlpineTool::Build( ) {
  return 0;
}

GeoVPhysVol* GeoPixelEnvelopeAlpineTool::buildEnvelope(const PixelGeoBuilderBasics* basics)
{

  // Load material file 
  std::string fileName="Material.xml";
  if(const char* env_p = std::getenv("PIXEL_MATERIAL_GEO_XML")) fileName = std::string(env_p);
  std::cout<<"SES - XML "<<fileName<<std::endl;

  GeoPixelXMLMaterial matBuilder(basics,fileName);
  matBuilder.Build();

  //
  // Control which parts get built. This is mainly of use for
  // Cosmic runs where the whole detector is not present
  //

  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  std::cout<<"GeoEnvelopeAlpine : GeoVPhysVol* GeoPixelEnvelopeAlpineTool::Build( )"<<std::endl;

  bool barrelPresent   = genDBHelper.isBarrelPresent();
  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  std::cout<<"XML envelope : "<<barrelPresent<<" "<<endcapAPresent<<" "<<endcapCPresent<<std::endl;

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

  // Service regions
  std::vector<InDetDD::TubeZone*> svcRegion_brl;
  std::vector<InDetDD::TubeZone*> svcRegion_ec;
  if(m_barrelTool){
    m_barrelTool->preBuild(basics);
    svcRegion_brl = m_barrelTool->getSvcRegions();
  }
  if((endcapAPresent||endcapCPresent)&&m_endcapTool){
    m_endcapTool->preBuild(basics);
    svcRegion_ec = m_endcapTool->getSvcRegions();
  }

  std::cout<<"Collected svcRegion : b/ec : "<<svcRegion_brl.size()<<" "<<svcRegion_ec.size()<<std::endl;

  svcRegion_brl.insert( svcRegion_brl.end(), svcRegion_ec.begin(), svcRegion_ec.end());
  std::vector<InDetDD::TubeZone*> svcRegions =  SortServiceRegions(svcRegion_brl);

  if(m_IDserviceTool) m_IDserviceTool->buildServices(basics, svcRegions);

  // Build barrel
  if (barrelPresent) {
    
//     DDmgr->numerology().addBarrel(0);
    basics->getDetectorManager()->numerology().addBarrel(0);

//     //
//     // Add the Barrel:
//     //
//     HepGeom::Transform3D barrelTransform = gmt_mgr->partTransform("Barrel");    
//     gmt_mgr->SetBarrel();

    HepGeom::Transform3D barrelTransform = HepGeom::Transform3D();
    GeoNameTag* tag = new GeoNameTag("Barrel");
    GeoVPhysVol* barrelPhys =  m_barrelTool->buildBarrel(basics) ;
    envelopePhys->add(tag);
    envelopePhys->add(new GeoIdentifierTag(0));
    envelopePhys->add(new GeoTransform(barrelTransform));
    envelopePhys->add(barrelPhys );
  }


    // EndCap A
  if (endcapAPresent) {
    
    if(!m_endcapTool) {
      std::cout<<"No endcap"<<std::endl;
    }
    else{
      
      //    DDmgr->numerology().addEndcap(2);
      basics->getDetectorManager()->numerology().addEndcap(2);
      
      HepGeom::Transform3D endcapATransform = HepGeom::Transform3D();  //gmt_mgr->partTransform("EndcapA");
      
      GeoVPhysVol* envPhysA =  m_endcapTool->buildEndcap(basics,1) ;
      double zpos = (m_endcapTool->getEndcapZMax()+m_endcapTool->getEndcapZMin())*.5;
      
      //    gmt_mgr->SetPos();
      GeoTransform* xform = new GeoTransform(endcapATransform * HepGeom::TranslateZ3D(zpos));
      GeoNameTag* tag  = new GeoNameTag("EndCapA");
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(2));
      envelopePhys->add(xform);
      envelopePhys->add(envPhysA);
    }
  }
  else {
    basics->getDetectorManager()->numerology().addEndcap(2);
    basics->getDetectorManager()->numerology().setNumDisks(0);
  }
    
  // EndCap C
  if (endcapCPresent) {
    
    if(!m_endcapTool) {
      std::cout<<"No endcap"<<std::endl;
    }
    else{
      
      //    DDmgr->numerology().addEndcap(-2);
      basics->getDetectorManager()->numerology().addEndcap(-2);
      
      HepGeom::Transform3D endcapCTransform = HepGeom::Transform3D();    //gmt_mgr->partTransform("EndcapC");
      
      GeoVPhysVol* envPhysC =  m_endcapTool->buildEndcap(basics,-1) ;
      double zpos = (m_endcapTool->getEndcapZMax()+m_endcapTool->getEndcapZMin())*.5;
      
      //      gmt_mgr->SetEndcap();
      //      gmt_mgr->SetNeg();
      GeoTransform* xform = new GeoTransform(endcapCTransform * HepGeom::TranslateZ3D(-zpos) *  HepGeom::RotateY3D(180*CLHEP::deg));
      GeoNameTag* tag  = new GeoNameTag("EndCapC");
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(-2));
      envelopePhys->add(xform);
      envelopePhys->add(envPhysC);
      
    }
  }
  else {
    basics->getDetectorManager()->numerology().addEndcap(-2);
    basics->getDetectorManager()->numerology().setNumDisks(0);
  }


//   // Extra Material 
//   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
//   xMat.add(envelopePhys,"Pixel");

  std::cout<<"GeoPixelEnvelope - build services "<<std::endl;
  if(m_IDserviceTool){
    m_IDserviceTool->buildAndPlace("P", envelopePhys);
    m_IDserviceTool->buildAndPlace("Pixel", envelopePhys);
  }
  return envelopePhys;

}



InDet::GeoEnvelopeAlpine* GeoPixelEnvelopeAlpineTool::buildEnvelopeGeoComp(const PixelGeoBuilderBasics* basics)
{

  // Load material file 
  std::string fileName="Material.xml";
  if(const char* env_p = std::getenv("PIXEL_MATERIAL_GEO_XML")) fileName = std::string(env_p);
  std::cout<<"SES - XML "<<fileName<<std::endl;

  GeoPixelXMLMaterial matBuilder(basics,fileName);
  matBuilder.Build();

  //
  // Control which parts get built. This is mainly of use for
  // Cosmic runs where the whole detector is not present
  //

  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  std::cout<<"GeoEnvelopeAlpine : GeoVPhysVol* GeoPixelEnvelopeAlpineTool::Build( )"<<std::endl;

  bool barrelPresent   = genDBHelper.isBarrelPresent();
  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  std::cout<<"XML envelope : "<<barrelPresent<<" "<<endcapAPresent<<" "<<endcapCPresent<<std::endl;

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

  InDet::GeoEnvelopeAlpine *envelope = new InDet::GeoEnvelopeAlpine( envelopePhys);

  /*
  if(m_IDserviceTool) m_IDserviceTool->buildServices(basics);

  std::cout<<"XML services : "<<(m_IDserviceTool!=0)<<std::endl;
  */

  if (barrelPresent) {
    
    //     DDmgr->numerology().addBarrel(0);
    basics->getDetectorManager()->numerology().addBarrel(0);
    
    //     //
    //     // Add the Barrel:
    //     //
    //     HepGeom::Transform3D barrelTransform = gmt_mgr->partTransform("Barrel");    
    //     gmt_mgr->SetBarrel();

    //    GeoVPhysVol* barrelPhys =  m_barrelTool->buildBarrel(basics) ;
    InDet::GeoBarrelAlpine *barrel = dynamic_cast<InDet::GeoBarrelAlpine*>(m_barrelTool->getGeoCompVol());
    // Create the GeoCompoenent object corresponding to barrel->layer->stave->module->...
    envelope->placeBarrelAlpine( *barrel, HepGeom::Transform3D(), 0);

  }

  /*
    // EndCap A
  if (endcapAPresent) {
    
    if(!m_endcapTool) {
      std::cout<<"No endcap"<<std::endl;
    }
    else{
      
      //    DDmgr->numerology().addEndcap(2);
      basics->getDetectorManager()->numerology().addEndcap(2);
      
      HepGeom::Transform3D endcapATransform = HepGeom::Transform3D();  //gmt_mgr->partTransform("EndcapA");
      
      GeoVPhysVol* envPhysA =  m_endcapTool->buildEndcap(basics,1) ;
      double zpos = (m_endcapTool->getEndcapZMax()+m_endcapTool->getEndcapZMin())*.5;
      
      //    gmt_mgr->SetPos();
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
      std::cout<<"No endcap"<<std::endl;
    }
    else{
      
      //    DDmgr->numerology().addEndcap(-2);
      basics->getDetectorManager()->numerology().addEndcap(-2);
      
      HepGeom::Transform3D endcapCTransform = HepGeom::Transform3D();    //gmt_mgr->partTransform("EndcapC");
      
      GeoVPhysVol* envPhysC =  m_endcapTool->buildEndcap(basics,-1) ;
      double zpos = (m_endcapTool->getEndcapZMax()+m_endcapTool->getEndcapZMin())*.5;
      
      //      gmt_mgr->SetEndcap();
      //      gmt_mgr->SetNeg();
      GeoTransform* xform = new GeoTransform(endcapCTransform * HepGeom::TranslateZ3D(-zpos) *  HepGeom::RotateY3D(180*CLHEP::deg));
      GeoNameTag* tag  = new GeoNameTag("EndCapC");
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(-2));
      envelopePhys->add(xform);
      envelopePhys->add(envPhysC);
      
    }
  }


//   // Extra Material 
//   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
//   xMat.add(envelopePhys,"Pixel");

  std::cout<<"GeoPixelEnvelope - build services "<<std::endl;
  if(m_IDserviceTool) m_IDserviceTool->buildAndPlace("Pixel", envelopePhys);

  return envelopePhys;
  */

  return envelope;

}


std::vector<InDetDD::TubeZone*> GeoPixelEnvelopeAlpineTool::SortServiceRegions(std::vector<InDetDD::TubeZone*>& svcRegions )
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
	std::cout<<"Collected svcRegion : "<<(*it)->label()<<" "<<std::endl; //(*it)->getZmin()<<" "<<(*it)->getZmax()<<" / "<<(*it)->getRmin()<<" "<<(*it)->getRmax()<<" / "<<std::endl;
      }
      else 
	{
	  std::cout<<"Collected svcRegion - already defined : "<<(*it)->label()<<std::endl;	  
	}
    }
  
  return newSvcRegions;
}
