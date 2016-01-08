/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutLoI/GeoPixelBarrelLoITool.h"
#include "PixelLayoutLoI/GeoPixelLayerLoI.h"
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
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "PathResolver/PathResolver.h"

#include <sstream>

using namespace std;

GeoPixelBarrelLoITool::GeoPixelBarrelLoITool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_IDserviceTool(),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelBarrelTool>(this);

  declareProperty("PixelServicesTool", m_IDserviceTool);
}


//================ Initialisation =================================================

StatusCode GeoPixelBarrelLoITool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelBarrelLoITool initialize() successful in " << name());

  sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }
  
  preBuild();
  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelBarrelLoITool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelBarrelLoITool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelBarrelLoITool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

// GeoVPhysVol* GeoPixelBarrelLoITool::Build( ) {

//   return 0;

// }

void GeoPixelBarrelLoITool::preBuild()
{
  // prebuild function is used to set service region (called by GeoPixelEnvelope)

  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");

  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();

  msg(MSG::INFO) << "GeoPixelBarrelAlpineTool: rmin " << rmin << " rmax " << rmax << " halflen " << halflen << endreq;
  m_barrelRegion.push_back("B");
  m_barrelSvcRegions.push_back(new InDetDD::TubeZone("B",-halflen, halflen, rmin, rmax));
}

GeoVPhysVol* GeoPixelBarrelLoITool::buildBarrel(const PixelGeoBuilderBasics* basics)
{

  std::cout<<"GeoBarrelLoI : GeoVPhysVol* GeoPixelBarrelLoITool::Build( )"<<std::endl;

  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  // Pixel general envelope XML reader
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");
  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();

  // Build barrel envelope
  const GeoMaterial* air = m_matMgr->getMaterial("std::Air");
  const GeoTube* barrelTube = new GeoTube(rmin,rmax,halflen);
  const GeoLogVol* barrelLog = new GeoLogVol("Barrel",barrelTube,air);
  GeoFullPhysVol* barrelPhys = new GeoFullPhysVol(barrelLog);

  int numLayers = m_xmlReader->nbOfPixelBarrelLayers();

  // Set numerology
  basics->getDetectorManager()->numerology().setNumLayers(numLayers);

  // Build the layers inside
  std::vector<GeoPixelLayerLoI> layerLoI;
  for(int ii = 0; ii < numLayers; ii++)
    {
      std::ostringstream lname;
      lname << "Layer" << ii;

      GeoPixelLayerLoI layer(basics, ii);
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

  // Place the service in the barrel envelope
  if(m_IDserviceTool){
    m_IDserviceTool->buildAndPlace("B", barrelPhys);
  }

  std::cout<<"RETURN BARREL PHYS"<<std::endl;

  return barrelPhys;

  
//   // Extra Material
//   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
//   xMat.add(barrelPhys,"PixelBarrel");
}
