/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelExtendedRef/GeoPixelBarrelExtRefTool.h"
#include "BarrelExtendedRef/GeoPixelLayerExtRef.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelInterfaces/IPixelServicesTool.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "PathResolver/PathResolver.h"

#include <sstream>

GeoPixelBarrelExtRefTool::GeoPixelBarrelExtRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_IDserviceTool(),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelBarrelTool>(this);

  declareProperty("PixelServicesTool", m_IDserviceTool);
}


//================ Initialisation =================================================

StatusCode GeoPixelBarrelExtRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelBarrelExtRefTool initialize() successful in " << name());

  sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_INFO("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelBarrelExtRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelBarrelExtRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelBarrelExtRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

// GeoVPhysVol* GeoPixelBarrelExtRefTool::Build( ) {

//   return 0;

// }

void GeoPixelBarrelExtRefTool::preBuild(const PixelGeoBuilderBasics* basics)
{
  // prebuild function is used to set service region (called by GeoPixelEnvelope)
  m_barrelRegion.clear();
  m_barrelSvcRegions.clear();

  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();
  bool cylBarrel = genDBHelper.isBarrelCylindrical();

  msg(MSG::INFO) << "GeoPixelBarrelExtendedTool: rmin " << rmin << " rmax " << rmax << " halflen " << halflen << " "<<cylBarrel<<endreq;

  if(cylBarrel) {
    m_barrelRegion.push_back("B");
    m_barrelSvcRegions.push_back(new InDetDD::TubeZone("B",-halflen, halflen, rmin, rmax));
  }
  else {
    //m_barrelRegion.push_back("P");   collect region but svc are placed in the starting from the envelope  
    double pixelRmin = genDBHelper.getEnvelopeRMin();
    double pixelRmax = genDBHelper.getEnvelopeRMax();
    double pixelZmax = genDBHelper.getCommonHalfLength();
    double delta=0.001;
    m_barrelSvcRegions.push_back(new InDetDD::TubeZone("P",-pixelZmax+delta,pixelZmax-delta,pixelRmin+delta,pixelRmax-delta));
  }

}


GeoVPhysVol* GeoPixelBarrelExtRefTool::buildBarrel(const PixelGeoBuilderBasics* basics)
{

  std::cout<<"GeoBarrelExtRef : GeoVPhysVol* GeoPixelBarrelExtRefTool::Build( )"<<std::endl;

  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  // Pixel general envelope XML reader
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML", basics);
  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();
  bool cylBarrel = genDBHelper.isBarrelCylindrical();

  msg(MSG::INFO) << "GeoPixelBarrelExtRefTool: rmin " << rmin << " rmax " << rmax << " halflen " << halflen << endreq;

  const std::string matEnvelope("std::Air");
  GeoPhysVol* barrelPhys = 0;

  // If cylindrcal barrel/endcap region : use standard construction process
  std::vector<std::string> barrelRegion;
  if(cylBarrel) {
    const GeoMaterial* air = m_matMgr->getMaterial("std::Air");
    GeoTube* barrelTube = new GeoTube(rmin,rmax,halflen);
    GeoLogVol* barrelLog = new GeoLogVol("Barrel",barrelTube,air);
    barrelPhys = new GeoPhysVol(barrelLog);
  }
  //  ... else : build complex shape based on radius lists
  else {
    const GeoMaterial* air = m_matMgr->getMaterial("special::Ether");
    std::vector<double> radiusList = genDBHelper.getBarrelRadiusList();
    std::vector<double> hlenList = genDBHelper.getBarrelHalfLengthList();
    
    const GeoShape * lastShape = 0;
    for(int i=0; i<(int)radiusList.size()-1; i++)
      {
	double rmin = radiusList[i];
	double rmax = radiusList[i+1];
	double halflen = hlenList[i];
	GeoTube* barrelTube = new GeoTube(rmin,rmax,halflen);
	lastShape = addShape(lastShape, barrelTube, HepGeom::Transform3D());
      }
    
    const GeoShape * barrelShape = lastShape;
    GeoLogVol* barrelLog = new GeoLogVol("Barrel",barrelShape,air);
    barrelPhys = new GeoPhysVol(barrelLog);
  }

  // Layer number

  int numLayers = m_xmlReader->nbOfPixelBarrelLayers();
  std::cout<<"ExtBarrel - GeoPixelBarrelExtRefTool - # layers "<<numLayers<<std::endl;

  // Set numerology
  basics->getDetectorManager()->numerology().setNumLayers(numLayers);

  // Build the layers inside
  std::vector<GeoPixelLayerExtRef> layerExtRef;
  for(int ii = 0; ii < numLayers; ii++)
    {
      std::ostringstream lname;
      lname << "Layer" << ii;

      // Build layer
      GeoPixelLayerExtRef layer(basics, ii);
      GeoAlignableTransform * xform = new GeoAlignableTransform(HepGeom::Transform3D()); 
      GeoVPhysVol* layerphys = layer.getPhysVol();

      GeoNameTag *tag = new GeoNameTag(lname.str());         
      barrelPhys->add(tag);
      barrelPhys->add(new GeoIdentifierTag(ii));
      barrelPhys->add(xform);
      barrelPhys->add(layerphys);
      
      // Store the transform (at level 1)
      Identifier id = basics->getIdHelper()->wafer_id(0,ii,0,0);
      basics->getDetectorManager()->addAlignableTransform(1, id, xform, layerphys);
      
    }

  // Add services in barrel volume - dynamic only
  if(m_IDserviceTool){
    double zOffset =0.;
    std::vector<std::string> svcList;
    for(std::vector<std::string>::iterator it=m_barrelRegion.begin(); it!=m_barrelRegion.end(); ++it)
      m_IDserviceTool->buildAndPlace((*it), barrelPhys, zOffset, svcList, false, true);
  }

  std::cout<<"RETURN BARREL PHYS"<<std::endl;

  return barrelPhys;
  
//   // Extra Material
//   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
//   xMat.add(barrelPhys,"PixelBarrel");
}

const GeoShape *GeoPixelBarrelExtRefTool::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{

  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}


