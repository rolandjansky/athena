/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutAlpine/GeoPixelBarrelAlpineTool.h"

#include "PixelLayoutAlpine/GeoPixelLayerAlpineBuilder.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelSCTDBHelper/GeoPixelGeneralDBHelper.h"

#include "PixelInterfaces/IPixelServicesTool.h"

#include "PixelGeoComponent/GeoBarrelAlpine.h"

#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include <sstream>

using namespace std;
// GeoPixelBarrelAlpineTool::GeoPixelBarrelAlpineTool( const GeoPixelServices * pixServices)
//   : GeoPixelBarrel(pixServices), XMLUtils()
// {}


GeoPixelBarrelAlpineTool::GeoPixelBarrelAlpineTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_IDserviceTool(),
    m_xmlReader("InDet::XmlReaderSvc/InDetXMLReaderSvc","XMLReaderSvc"),
    m_barrelGeoComp(0)
{
  declareInterface<IGeoPixelBarrelTool>(this);

  declareProperty("PixelServicesTool", m_IDserviceTool);
}


//================ Initialisation =================================================

StatusCode GeoPixelBarrelAlpineTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelBarrelAlpineTool initialize() successful in " << name());

  sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  if (m_xmlReader.retrieve().isSuccess()){
    msg(MSG::INFO)<<"XMLReadeSvcr successfully retrieved " << m_xmlReader <<endreq;
  } else {
    msg(MSG::INFO)<<"XMLReaderSvc: Couldn't retrieve " << m_xmlReader <<endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelBarrelAlpineTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelBarrelAlpineTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelBarrelAlpineTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

GeoVPhysVol* GeoPixelBarrelAlpineTool::Build( ) {

  return 0;

}

void GeoPixelBarrelAlpineTool::preBuild(const PixelGeoBuilderBasics* basics)
{

  m_barrelRegion.clear();
  m_barrelSvcRegions.clear();
  m_bBuildStaticSvc = false;

  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  std::cout<<"ALPINE - GeoPixelBarrelAlpineTool"<<std::endl;

  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();
  bool cylBarrel = genDBHelper.isBarrelCylindrical();

  msg(MSG::INFO) << "GeoPixelBarrelAlpineTool: rmin " << rmin << " rmax " << rmax << " halflen " << halflen << endreq;

  if(cylBarrel) {
    m_barrelRegion.push_back("B");
    m_barrelSvcRegions.push_back(new InDetDD::TubeZone("B",-halflen, halflen, rmin, rmax));
    m_bBuildStaticSvc = true;
  }
  else {
    //    m_barrelRegion.push_back("P");
    double pixelRmin = genDBHelper.getEnvelopeRMin();
    double pixelRmax = genDBHelper.getEnvelopeRMax();
    double pixelZmax = genDBHelper.getCommonHalfLength();
    double delta=0.001;
    m_barrelSvcRegions.push_back(new InDetDD::TubeZone("P",-pixelZmax+delta,pixelZmax-delta,pixelRmin+delta,pixelRmax-delta));
  }

}

GeoVPhysVol* GeoPixelBarrelAlpineTool::buildBarrel(const PixelGeoBuilderBasics* basics)
{
  //
  // create the Barrel Mother volume
  // 
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();
  m_bGeoComp = basics->isGeoComponent();

  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  std::cout<<"ALPINE - GeoPixelBarrelAlpineTool"<<std::endl;

  double rmin = genDBHelper.getBarrelRMin();
  double rmax = genDBHelper.getBarrelRMax();
  double halflen = genDBHelper.getBarrelHalfLength();
  bool cylBarrel = genDBHelper.isBarrelCylindrical();

  msg(MSG::INFO) << "GeoPixelBarrelAlpineTool: rmin " << rmin << " rmax " << rmax << " halflen " << halflen << endreq;

  const std::string matEnvelope("std::Air");
  //  const GeoMaterial* air = m_matMgr->getMaterial(matEnvelope);
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

  if(m_bGeoComp)
    m_barrelGeoComp = new InDet::GeoBarrelAlpine(barrelPhys);

  std::cout<<"ALPINE - GeoPixelBarrelAlpineTool - file parsed "<<std::endl;

  unsigned int numLayers = m_xmlReader->nbOfPixelBarrelLayers();
  std::cout<<"ALPINE - GeoPixelBarrelAlpineTool - # layers "<<numLayers<<std::endl;

  // Set numerology
  basics->getDetectorManager()->numerology().setNumLayers(numLayers);

  // Build the layers inside

  for(unsigned int ii = 0; ii < numLayers; ii++)
    {
      //cout << "Layer" << ii << endl;
      //      gmt_mgr->SetCurrentLD(ii);

      double layerRmin = genDBHelper.getLayerRMin(ii);
      double layerRmax = genDBHelper.getLayerRMax(ii);
      std::cout<<"Barrel layer rminmax : "<<layerRmin<<"  "<<layerRmax<<std::endl;

      std::cout<<"LAYER  %d\n"<<ii<<std::endl;
      std::ostringstream lname;
      lname << "Layer" << ii;
      
      GeoPixelLayerAlpineBuilder layerBuilder(basics, ii, layerRmin, layerRmax);
      
      GeoAlignableTransform * xform = new GeoAlignableTransform(HepGeom::Transform3D()); 
      GeoVPhysVol* layerphys = 0;
      if(!m_bGeoComp){
	layerphys = layerBuilder.getPhysVol();
	GeoNameTag *tag = new GeoNameTag(lname.str());         
	barrelPhys->add(tag);
	barrelPhys->add(new GeoIdentifierTag(ii));
	barrelPhys->add(xform);
	barrelPhys->add(layerphys);
      }
      else {
	InDet::GeoLayerAlpine *layer = dynamic_cast<InDet::GeoLayerAlpine*>(layerBuilder.getGeoCompVol());
	layerphys = layer->physVolume<GeoFullPhysVol>();
	m_barrelGeoComp->placeLayer(*layer,HepGeom::Transform3D(),ii);
      }

      // Store the transform (at level 1)
      //      Identifier id = gmt_mgr->getIdHelper()->wafer_id(0,ii,0,0);
      //      DDmgr->addAlignableTransform(1, id, xform, layerphys);
      Identifier id = basics->getIdHelper()->wafer_id(0,ii,0,0);
      basics->getDetectorManager()->addAlignableTransform(1, id, xform, layerphys);
      
    }

  if(m_IDserviceTool){
    std::cout<<"GeoPixelBarrelAlpineTool : buildandplace - # barrel region "<<m_barrelRegion.size()<<std::endl;
    double zOffset =0.;
    std::vector<std::string> svcList;
    for(std::vector<std::string>::iterator it=m_barrelRegion.begin(); it!=m_barrelRegion.end(); ++it)
      m_IDserviceTool->buildAndPlace((*it), barrelPhys, zOffset, svcList, m_bBuildStaticSvc, true);
  }

  return barrelPhys;

//   // Extra Material
//   InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
//   xMat.add(barrelPhys,"PixelBarrel");

}


const GeoShape *GeoPixelBarrelAlpineTool::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{

  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}



