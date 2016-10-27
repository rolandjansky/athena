/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelInclinedRef/GeoPixelBarrelInclRefTool.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelInterfaces/IPixelServicesTool.h"
#include "PixelInterfaces/IGeoPixelLayerTool.h"
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

GeoPixelBarrelInclRefTool::GeoPixelBarrelInclRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_IDserviceTool(),
    m_layerInnerTool("InnerPixelLayerTool"),
    m_layerOuterTool("OuterPixelLayerTool"),
    m_layerInnerMax(9999),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelBarrelTool>(this);

  declareProperty("PixelServicesTool", m_IDserviceTool);
  declareProperty("InnerPixelLayerTool", m_layerInnerTool);
  declareProperty("OuterPixelLayerTool", m_layerOuterTool);
  declareProperty("MaxInnerLayerMax", m_layerInnerMax=9999);
}


//================ Initialisation =================================================

StatusCode GeoPixelBarrelInclRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelBarrelInclRefTool initialize() successful in " << name());

  sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::DEBUG) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelBarrelInclRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelBarrelInclRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelBarrelInclRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

// GeoVPhysVol* GeoPixelBarrelInclRefTool::Build( ) {

//   return 0;

// }

void GeoPixelBarrelInclRefTool::preBuild(const PixelGeoBuilderBasics* basics)
{

  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();

  msg(MSG::DEBUG) << "GeoPixelBarrelInclinedTool: rmin " << rmin << " rmax " << rmax << " halflen " << halflen << endreq;
  m_barrelRegion.push_back("B");
  m_barrelSvcRegions.push_back(new InDetDD::TubeZone("B",-halflen, halflen, rmin, rmax));

}

GeoVPhysVol* GeoPixelBarrelInclRefTool::buildBarrel(const PixelGeoBuilderBasics* basics)
{

  msg(MSG::DEBUG)<<"GeoBarrelInclRef : GeoVPhysVol* GeoPixelBarrelInclRefTool::Build( )"<<endreq;

  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML", basics);

  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();
  bool cylBarrel = genDBHelper.isBarrelCylindrical();

  msg(MSG::DEBUG) << "GeoPixelBarrelInclRefTool: rmin " << rmin << " rmax " << rmax << " halflen " << halflen << endreq;

  const std::string matEnvelope("std::Air");
  GeoPhysVol* barrelPhys = 0;

  std::vector<std::string> barrelRegion;
  if(cylBarrel) {
    const GeoMaterial* air = m_matMgr->getMaterial("std::Air");
    GeoTube* barrelTube = new GeoTube(rmin,rmax,halflen);
    GeoLogVol* barrelLog = new GeoLogVol("Barrel",barrelTube,air);
    barrelPhys = new GeoPhysVol(barrelLog);
  }
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

  int numLayers = m_xmlReader->nbOfPixelBarrelLayers();
  //  msg(MSG::INFO)<<"INCLINED - GeoPixelBarrelInclinedRefTool - # layers "<<numLayers<<endreq;

  // Set numerology
  basics->getDetectorManager()->numerology().setNumLayers(numLayers);

  // Build the layers inside
  for(int ii = 0; ii < numLayers; ii++)
  {
      //cout << "Layer" << ii << endl;
      //      gmt_mgr->SetCurrentLD(ii);

      std::ostringstream lname;
      lname << "Layer" << ii;

      //      GeoPixelLayerInclRef layer(basics, ii);
      GeoAlignableTransform * xform = new GeoAlignableTransform(HepGeom::Transform3D()); 
      GeoVPhysVol* layerphys =  0;
      if(ii<m_layerInnerMax)
	layerphys = m_layerInnerTool->buildLayer(basics, ii);
      else
	layerphys = m_layerOuterTool->buildLayer(basics, ii);

      GeoNameTag *tag = new GeoNameTag(lname.str());         
      barrelPhys->add(tag);
      barrelPhys->add(new GeoIdentifierTag(ii));
      barrelPhys->add(xform);
      barrelPhys->add(layerphys);
      
      // Store the transform (at level 1)
      //      Identifier id = gmt_mgr->getIdHelper()->wafer_id(0,ii,0,0);
      //      DDmgr->addAlignableTransform(1, id, xform, layerphys);
      Identifier id = basics->getIdHelper()->wafer_id(0,ii,0,0);
      basics->getDetectorManager()->addAlignableTransform(1, id, xform, layerphys);
      
    }


  if(m_IDserviceTool){
    m_IDserviceTool->buildAndPlace("B", barrelPhys);
  }

  return barrelPhys;

//   // Extra Material
//   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
//   xMat.add(barrelPhys,"PixelBarrel");
}

const GeoShape *GeoPixelBarrelInclRefTool::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{

  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}


