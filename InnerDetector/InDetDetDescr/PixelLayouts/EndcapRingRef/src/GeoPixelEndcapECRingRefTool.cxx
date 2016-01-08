/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EndcapRingRef/GeoPixelEndcapECRingRefTool.h"
#include "EndcapRingRef/GeoPixelLayerECRingRefTool.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelInterfaces/IGeoPixelEndcapLayerTool.h"
#include "PixelInterfaces/IPixelServicesTool.h"

#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include <sstream>


GeoPixelEndcapECRingRefTool::GeoPixelEndcapECRingRefTool( const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_endcapSvcRegions(0),
    m_layerTool("GeoPixelEndcapLayerTool"),
    m_IDserviceTool(),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelEndcapTool>(this);

  declareProperty("GeoPixelEndcapLayerTool",m_layerTool);
  declareProperty("PixelServicesTool", m_IDserviceTool);
}


//================ Initialisation =================================================

StatusCode GeoPixelEndcapECRingRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelEndcapECRingRefTool initialize() successful in " << name());

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  m_bPrebuild = false;

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelEndcapECRingRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelEndcapECRingRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelEndcapECRingRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

void GeoPixelEndcapECRingRefTool::preBuild(const PixelGeoBuilderBasics* basics)
{

  m_ecPosSvcRegion.clear();
  m_ecNegSvcRegion.clear();
  m_ecSvcRegion.clear();
  m_endcapSvcRegions.clear();

  // Pixel general envelope XML reader
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");

  std::cout<<"ENDCAP RING - GeoPixelEndcapRingTool"<<std::endl;

  // get the min/mac Endcap Mother volume
  m_endcapRMin = genDBHelper.getEndcapRMin();
  m_endcapRMax = genDBHelper.getEndcapRMax();
  m_endcapZMin = genDBHelper.getEndcapZMin();
  m_endcapZMax = genDBHelper.getEndcapZMax();

  // read the number of layers
  m_nlayers = m_xmlReader->nbOfPixelEndcapLayers();

  std::cout<<"ENDCAP : "<<m_endcapRMin<<" "<<m_endcapRMax<<" "<<m_endcapZMin<<" "<<m_endcapZMax<<"   # layer "<<m_nlayers<<std::endl;

  // Build and collect the service regions
  double zMiddle =  (m_endcapZMax+m_endcapZMin)*.5;
  bool cylEndcap = genDBHelper.isBarrelCylindrical();
  m_endcapZOffset = zMiddle;
  m_endcapSvcRegions.clear();

  // If cylindrcal barrel/endcap region : collect standard A/C regions
  if(cylEndcap) {
    m_ecPosSvcRegion.push_back("A");
    m_ecNegSvcRegion.push_back("C");
    m_endcapSvcRegions.push_back(new InDetDD::TubeZone("A",m_endcapZMin,m_endcapZMax,m_endcapRMin,m_endcapRMax));
    m_endcapSvcRegions.push_back(new InDetDD::TubeZone("C",-m_endcapZMax,-m_endcapZMin,m_endcapRMin,m_endcapRMax,true));
  }
  //  ... else : build encompassing pixel region + ring layer regions
  else { 
    double pixelRmin = genDBHelper.getEnvelopeRMin();
    double pixelRmax = genDBHelper.getEnvelopeRMax();
    double pixelZmax = genDBHelper.getCommonHalfLength();
    double delta=0.001;
    //    m_ecSvcRegion.push_back("P");     collect region but svc are placed in the starting from the envelope  
    m_endcapSvcRegions.push_back(new InDetDD::TubeZone("P",-pixelZmax+delta,pixelZmax-delta,pixelRmin+delta,pixelRmax-delta));

    // Loop over layers to get the layer rmin/rmax zmin/zmax sizes
    for(int ilayer = 0; ilayer < m_nlayers; ilayer++) {

      m_layerTool->preBuild(basics,ilayer);
      double zmin = m_layerTool->getLayerZMin();
      double zmax = m_layerTool->getLayerZMax();
      double rmin = m_layerTool->getLayerRMin();
      double rmax = m_layerTool->getLayerRMax();

      std::ostringstream osA; 
      osA << "AL" << ilayer;
      m_layerPosSvcRegion.push_back(osA.str());
      m_endcapSvcRegions.push_back(new InDetDD::TubeZone(osA.str(),zmin,zmax,rmin,rmax));
      std::ostringstream osC; 
      osC << "CL" << ilayer;
      m_layerNegSvcRegion.push_back(osC.str());
      m_endcapSvcRegions.push_back(new InDetDD::TubeZone(osC.str(),-zmax,-zmin,rmin,rmax));
      
    }
  }

  m_bPrebuild=true;
  
}

GeoVPhysVol* GeoPixelEndcapECRingRefTool::buildEndcap(const PixelGeoBuilderBasics* basics, int side)
{
  // Prebuild the endcap in case it is not already done
  if(!m_bPrebuild)preBuild(basics);

  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  // Pixel general envelope XML reader
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");
  m_endcapSide = side;
  double halflength = (m_endcapZMax-m_endcapZMin)*.5;
  double zMiddle =  (m_endcapZMax+m_endcapZMin)*.5;
  bool cylEndcap = genDBHelper.isBarrelCylindrical();
  m_endcapZOffset = zMiddle;
  std::cout<<"ENDCAP : "<<m_endcapRMin<<" "<<m_endcapRMax<<" "<<m_endcapZMin<<" "<<m_endcapZMax<<std::endl;

  GeoFullPhysVol* ecPhys = 0;

  // If cylindrcal barrel/endcap region : use standard construction process  
  if(cylEndcap) {
    const GeoMaterial* air = m_matMgr->getMaterial("std::Air");
    const GeoTube* ecTube = new GeoTube(m_endcapRMin,m_endcapRMax,halflength);
    const GeoLogVol* ecLog = new GeoLogVol("EndCap",ecTube,air);
    ecPhys = new GeoFullPhysVol(ecLog);
  }
  //  ... else : build complex shape based on radius list
  else {
    const GeoMaterial* air = m_matMgr->getMaterial("special::Ether");
    std::vector<double> radiusList = genDBHelper.getEndcapRadiusList();
    std::vector<double> zminList = genDBHelper.getEndcapZMinList();
    
    const GeoShape * lastShape = 0;
    for(int i=0; i<(int)radiusList.size()-1; i++)
      {
	double rmin = radiusList[i];
	double rmax = radiusList[i+1];
	double zmin = zminList[i];
	double halflen_loc = (m_endcapZMax-zmin)*.5;
	double middle_loc =  (m_endcapZMax+zmin)*.5;
	GeoTube* endcapTube = new GeoTube(rmin,rmax,halflen_loc);
	lastShape = addShape(lastShape, endcapTube, HepGeom::TranslateZ3D(middle_loc-zMiddle));
      }

      const GeoShape * endcapShape = lastShape;
      GeoLogVol* endcapLog = new GeoLogVol("Endcap",endcapShape,air);
      ecPhys = new GeoFullPhysVol(endcapLog);
  }


  // Set numerology
  basics->getDetectorManager()->numerology().setNumDisks(m_nlayers);


  // Retrieve the PixelServiceTool
  StatusCode sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::DEBUG) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }
  
  //
  // Build and place the ring layers
  //
  for(int ilayer = 0; ilayer < m_nlayers; ilayer++) {

    GeoVPhysVol* layerPhys = m_layerTool->buildLayer(basics, ilayer, side);

    // position of the layer
    double zlayer = m_layerTool->getZPosition();
    std::cout<<"LAYER position : "<<ilayer<<" "<<zlayer<<"  / zMiddle "<<zMiddle<<"  / side "<<side<<std::endl;

    // add the services in defined layer
    if(m_IDserviceTool){
      std::vector<std::string> svcList;
      std::string svcName=(side>0)?"AL":"CL";
      std::ostringstream os; 
      os << svcName << ilayer;
      m_IDserviceTool->buildAndPlace(os.str(), dynamic_cast<GeoFullPhysVol*>(layerPhys), zlayer, svcList, false, true);
    }

    // place the layer
    std::ostringstream nameTag; 
    nameTag << "RingLayer" << ilayer;
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::TranslateZ3D(zlayer-zMiddle));
    
    ecPhys->add(tag);
    ecPhys->add(new GeoIdentifierTag(ilayer));
    ecPhys->add(xform);
    ecPhys->add(layerPhys);

    // Store the alignable transform
    int brl_ec = 2*side;
    Identifier id = basics->getIdHelper()->wafer_id(brl_ec,ilayer,0,0);
    basics->getDetectorManager()->addAlignableTransform(1, id, xform, layerPhys);
    
  }

  // add the services in the endcap regions - dynamic only
  if(m_IDserviceTool){
    
    std::vector<std::string> svcList;
    if(side>0)
      for(std::vector<std::string>::iterator it=m_ecPosSvcRegion.begin(); it!=m_ecPosSvcRegion.end(); ++it)
	m_IDserviceTool->buildAndPlace((*it), ecPhys, m_endcapZOffset, svcList, false, true);
    else
      for(std::vector<std::string>::iterator it=m_ecNegSvcRegion.begin(); it!=m_ecNegSvcRegion.end(); ++it)
	m_IDserviceTool->buildAndPlace((*it), ecPhys, m_endcapZOffset, svcList, false, true);
    
    for(std::vector<std::string>::iterator it=m_ecSvcRegion.begin(); it!=m_ecSvcRegion.end(); ++it)
      m_IDserviceTool->buildAndPlace((*it), ecPhys, m_endcapZOffset, svcList, false, true);
    
  }
  else
    std::cout<<"ENDCAP : build services - no services defined "<<std::endl;

  return ecPhys;
}


const GeoShape *GeoPixelEndcapECRingRefTool::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{

  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}
