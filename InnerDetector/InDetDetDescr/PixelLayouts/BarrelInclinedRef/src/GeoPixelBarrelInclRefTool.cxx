/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "PathResolver/PathResolver.h"

#include <sstream>

GeoPixelBarrelInclRefTool::GeoPixelBarrelInclRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_IDserviceTool(),
    m_layerPlanarTool("PlanarPixelLayerTool"),
    m_layerAlpineTool("AlpinePixelLayerTool"),  
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelBarrelTool>(this);

  declareProperty("PixelServicesTool", m_IDserviceTool);
  declareProperty("PlanarPixelLayerTool", m_layerPlanarTool);
  declareProperty("AlpinePixelLayerTool", m_layerAlpineTool);
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
      // pick layer tool according to stave type 
      InDet::BarrelLayerTmp *layerTmp = m_xmlReader->getPixelBarrelLayerTemplate(ii);
      std::string staveType    = layerTmp->stave_type;
      if (staveType == "IBL")  layerphys = m_layerPlanarTool->buildLayer(basics, ii);
      else layerphys = m_layerAlpineTool->buildLayer(basics, ii);

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


  if(!cylBarrel) {
    std::vector<double> hlenList = genDBHelper.getBarrelHalfLengthList();
    std::vector<double> radiusList = genDBHelper.getBarrelRadiusList();

    std::vector<double> zoffsetList = genDBHelper.getBarrelSupportZOffsetList();
    std::vector<double> thicknessList = genDBHelper.getBarrelSupportThicknessList();
    std::vector<std::string> materialList = genDBHelper.getBarrelSupportMaterialList();
    std::vector<double> rmininnerList = genDBHelper.getBarrelSupportRminInnerList();
    std::vector<double> rmaxinnerList = genDBHelper.getBarrelSupportRmaxInnerList();
    std::vector<double> rminouterList = genDBHelper.getBarrelSupportRminOuterList();
    std::vector<double> rmaxouterList = genDBHelper.getBarrelSupportRmaxOuterList();
    std::vector<int> nsectorsList = genDBHelper.getBarrelSupportNSectorsList();
    std::vector<double> sphiList = genDBHelper.getBarrelSupportSPhiList();
    std::vector<double> dphiList = genDBHelper.getBarrelSupportDPhiList();
    
    for (int side=-1; side<=1; side=side+2)
      {
	for(size_t i=0; i<rmininnerList.size(); i++)
	  {
	    double halflen = hlenList[0];

	    double zoffset = zoffsetList[i];
	    double thickness = thicknessList[i];
	    double zpos = (halflen - (thickness/1.99) - zoffset) * side;
	    std::string matName = materialList[i];

	    double rmin_inner = rmininnerList[i];
	    double rmax_inner = rmaxinnerList[i];
	    double rmin_outer = rminouterList[i];
	    double rmax_outer = rmaxouterList[i];

	    int nsectors = nsectorsList[i];
	    double sphiSvc = sphiList[i];
	    double dphiSvc = dphiList[i];
	    
	    const GeoTube* supTube_inner = new GeoTube(rmin_inner,rmax_inner,thickness*.5);
	    double matVolume_inner = supTube_inner->volume();
	    const GeoMaterial* supMat_inner = m_matMgr->getMaterial(matName);
	    ATH_MSG_DEBUG("Density = " << supMat_inner->getDensity() << " Mass = " << ( matVolume_inner * supMat_inner->getDensity() ));
	    GeoLogVol* _supLog_inner = new GeoLogVol("supLog_inner",supTube_inner,supMat_inner);
	    GeoPhysVol* supPhys_inner = new GeoPhysVol(_supLog_inner);
	    GeoTransform* xform_inner = new GeoTransform( HepGeom::Translate3D(0., 0., zpos) );
	    barrelPhys->add(xform_inner);
	    barrelPhys->add(supPhys_inner);
	    
	    for (int i_sector = 0; i_sector < nsectors; i_sector++) {

	      if ((360. / nsectors) < dphiSvc) {
		ATH_MSG_WARNING("Arms will overlap. Do not implement them.");
		continue;
	      }

	      double Sphi  = (sphiSvc + 360. / nsectors * i_sector) * CLHEP::deg;
	      double Dphi  = dphiSvc * CLHEP::deg;

	      const GeoShape* supTubs_arm = new GeoTubs(rmax_inner,rmin_outer,thickness*.5,Sphi,Dphi);
	      double matVolume_arm = supTubs_arm->volume();
	      const GeoMaterial* supMat_arm = m_matMgr->getMaterial(matName);
	      ATH_MSG_DEBUG("Density = " << supMat_arm->getDensity() << " Mass = " << ( matVolume_arm * supMat_arm->getDensity() ));
	      GeoLogVol* _supLog_arm = new GeoLogVol("supLog_arm",supTubs_arm,supMat_arm);
	      GeoPhysVol* supPhys_arm = new GeoPhysVol(_supLog_arm);
	      GeoTransform* xform_arm = new GeoTransform( HepGeom::Translate3D(0., 0., zpos) );
	      barrelPhys->add(xform_arm);
	      barrelPhys->add(supPhys_arm);

	    }

	    const GeoTube* supTube_outer = new GeoTube(rmin_outer,rmax_outer,thickness*.5);
	    double matVolume_outer = supTube_outer->volume();
	    const GeoMaterial* supMat_outer = m_matMgr->getMaterial(matName);
	    ATH_MSG_DEBUG("Density = " << supMat_outer->getDensity() << " Mass = " << ( matVolume_outer * supMat_outer->getDensity() ));
	    GeoLogVol* _supLog_outer = new GeoLogVol("supLog_outer",supTube_outer,supMat_outer);
	    GeoPhysVol* supPhys_outer = new GeoPhysVol(_supLog_outer);
	    GeoTransform* xform_outer = new GeoTransform( HepGeom::Translate3D(0., 0., zpos) );
	    barrelPhys->add(xform_outer);
	    barrelPhys->add(supPhys_outer);

	  }
      }
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


