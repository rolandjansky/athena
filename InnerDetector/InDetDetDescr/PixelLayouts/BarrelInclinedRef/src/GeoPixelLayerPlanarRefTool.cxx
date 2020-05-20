/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#include "BarrelInclinedRef/GeoPixelLayerPlanarRefTool.h"
#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "PathResolver/PathResolver.h"

#include <sstream>

GeoPixelLayerPlanarRefTool::GeoPixelLayerPlanarRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_layer(-1),
    m_bPreBuild(false),
    //    m_IDserviceTool("InDetservicesTool/PixelServicesTool"),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelLayerTool>(this);
}

GeoPixelLayerPlanarRefTool::~GeoPixelLayerPlanarRefTool()
{
}

//================ Initialisation =================================================

StatusCode GeoPixelLayerPlanarRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelLayerPlanarRefTool initialize() successful in " << name());


  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelLayerPlanarRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelLayerPlanarRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelLayerPlanarRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}


void GeoPixelLayerPlanarRefTool::preBuild(const PixelGeoBuilderBasics* /*basics*/, int iLayer)
{
  m_layer=iLayer;
  m_bPreBuild = true;

}

GeoVPhysVol* GeoPixelLayerPlanarRefTool::buildLayer(const PixelGeoBuilderBasics* basics, int iLayer) 
{
  // Switch to prebuild
  if(!m_bPreBuild ||iLayer!=m_layer) preBuild(basics,iLayer);
  
  //
  // create a barrel layer
  //

  InDet::BarrelLayerTmp *layerTmp = m_xmlReader->getPixelBarrelLayerTemplate(m_layer);

  printf("************** BUILD LAYER  %d\n", m_layer);

  // retrieve stave template for this layer
  //  int layerIndex           = layerTmp->index;
  std::string layerName    = layerTmp->name;
  double layerRadius       = layerTmp->radius;
  int    nSectors          = layerTmp->stave_n; 
  double ladderTilt        = layerTmp->stave_tilt;
  //  double staveOffset       = layerTmp->stave_zoffset;   
  std::string staveType    = layerTmp->stave_type;

  double phiOfStaveZero = 0.;

  ATH_MSG_DEBUG(layerName<<" "<<staveType<<" "<<nSectors);
  ATH_MSG_DEBUG("*****************************************************************************");

  double deltaPhi = 360.*CLHEP::deg/(double)nSectors;

  HepGeom::Transform3D transRadiusAndTilt = HepGeom::TranslateX3D(layerRadius)*HepGeom::RotateZ3D(ladderTilt);

  std::vector<InDet::StaveTmp *> staveTmp = m_xmlReader->getPixelStaveTemplate(m_layer);
  GeoPixelLadderPlanarRef pixelLadder(basics, staveTmp[0], m_layer, transRadiusAndTilt); 
  ComputeLayerThickness(pixelLadder, ladderTilt, layerRadius);
  
  basics->getDetectorManager()->numerology().setNumPhiModulesForLayer(m_layer,nSectors);
  
  GeoFullPhysVol* layerPhys = 0;
  PixelInclRefStaveXMLHelper staveDBHelper(m_layer, basics);

  // Loop over the sectors and place everything
  //
  for(int ii = 0; ii < nSectors; ii++) {
    //    gmt_mgr->SetPhi(ii);

    // Build ladder
    pixelLadder.setSector(ii);
    
    GeoVPhysVol *ladderPhys=pixelLadder.Build();
    GeoVPhysVol *pigtailPhys=pixelLadder.BuildPigtail();
    
    if(ii==0){

      double safety = 0.01 * CLHEP::mm;
      double rmin =  layerRadius-m_layerThicknessN - safety;
      double rmax =  layerRadius+m_layerThicknessP + safety;
      //enlarge the rmax to ensure pigtails are included
      if(pigtailPhys != 0){
	InDet::BarrelLayerTmp *lp1 = m_xmlReader->getPixelBarrelLayerTemplate(m_layer+1);
	double lp1_radius = lp1->radius;
	double lp1_tilt = lp1->stave_tilt;	
	HepGeom::Transform3D lp1_xform = HepGeom::TranslateX3D(lp1_radius)*HepGeom::RotateZ3D(lp1_tilt);
	std::vector<InDet::StaveTmp *> lp1_stave = m_xmlReader->getPixelStaveTemplate(m_layer+1);
	GeoPixelLadderPlanarRef lp1_ladder(basics, lp1_stave[0], m_layer+1, lp1_xform); 
		
	double lp1_thickN = lp1_ladder.thicknessN();
	double lp1_halfWidth = lp1_ladder.width()/2;

	HepGeom::Point3D<double> lp1_lowerCorner(-lp1_thickN, lp1_halfWidth, 0);
	lp1_lowerCorner = HepGeom::TranslateX3D(lp1_radius) * HepGeom::RotateZ3D(std::abs(lp1_tilt)) * lp1_lowerCorner;

	rmax = lp1_lowerCorner.perp() - safety;
      }
      double ladderLength = pixelLadder.envLength() + 4*basics->epsilon(); // Ladder has length gmt_mgr->PixelLadderLength() +  2*m_epsilon

      // Now make the layer envelope
      // 
      ATH_MSG_DEBUG("Layer "<<m_layer<<" in/out radius "<<rmin<<"  "<<rmax);
      const GeoMaterial* air = basics->matMgr()->getMaterial("std::Air");
      std::ostringstream lname;
      lname << "Layer" << m_layer;
      const GeoTube* layerTube = new GeoTube(rmin,rmax,0.5*ladderLength); //solid
      const GeoLogVol* layerLog = new GeoLogVol(lname.str(),layerTube,air); //log volume
      layerPhys = new GeoFullPhysVol(layerLog); // phys vol
    }

    double phiOfSector = phiOfStaveZero + ii*deltaPhi;

    // transform equiv  RotZ(phiOfSector)*TransZ(layerRadius)*RotZ(tilt)
    HepGeom::Transform3D ladderTransform = HepGeom::RotateZ3D(phiOfSector) * transRadiusAndTilt;

    //
    // Place the active ladders
    //
    std::ostringstream nameTag; 
    nameTag << "LadderL" << m_layer << "_" << ii;
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    GeoTransform* xform = new GeoTransform(ladderTransform);
    layerPhys->add(tag);
    layerPhys->add(new GeoIdentifierTag(ii) );
    layerPhys->add(xform);
    layerPhys->add(ladderPhys);   //pixelLadder->Build());
    //for pigtail
    if(pigtailPhys!=0){
      double pigtailAngle = staveDBHelper.getPigtailAngle(ii);
      double pigtailDR = staveDBHelper.getPigtailDR();
      double safety = 0.01 * CLHEP::mm;
      
      HepGeom::Transform3D transRadiusAndTiltAndPgAngle = HepGeom::TranslateX3D(layerRadius +m_layerThicknessP +pigtailDR*.5 +safety) * HepGeom::RotateZ3D(ladderTilt+pigtailAngle*CLHEP::deg);
      HepGeom::Transform3D pigtailTransform = HepGeom::RotateZ3D(phiOfSector) * transRadiusAndTiltAndPgAngle;
      
      std::ostringstream nameTagPg;
      nameTagPg << "PigtailL" << m_layer << "_" << ii;
      GeoNameTag * tagPg = new GeoNameTag(nameTagPg.str());
      GeoTransform* xformPg = new GeoTransform(pigtailTransform);
      layerPhys->add(tagPg);
      layerPhys->add(new GeoIdentifierTag(ii) );
      layerPhys->add(xformPg);
      layerPhys->add(pigtailPhys);
    }
  }


  return layerPhys;

}


void GeoPixelLayerPlanarRefTool::ComputeLayerThickness(const GeoPixelLadderPlanarRef& pixelLadder, double ladderTilt, double layerRadius)
{

  //
  // Calculate layerThicknessN: Thickness from layer radius to min radius of envelope
  // Calculate layerThicknessP: Thickness from layer radius to max radius of envelope
  //      
  double ladderHalfThickN = pixelLadder.thicknessN();
  double ladderHalfThickP = pixelLadder.thicknessP();
  double ladderHalfWidth = pixelLadder.width()/2;
  msg(MSG::DEBUG) << "ladderHalfThickN, ladderHalfThickP, ladderHalfWidth, ladderTilt, layerRadius:"
	    << ladderHalfThickN << " "<< ladderHalfThickP << " " << ladderHalfWidth 
	    << " " << ladderTilt << " " << layerRadius << endreq;
  
  // Calculate distance of closest approach to a line following the surface of the ladder.
  double grad = -1/tan(std::abs(ladderTilt)); // Gradient of this line.
  // x1, y1 is the point on the center of ladder surface.
  double y1 = -ladderHalfThickN*sin(std::abs(ladderTilt));
  double x1 = -ladderHalfThickN*cos(std::abs(ladderTilt))+layerRadius;
  // x1, y2 is the point of closest approach.
  double y2 = (y1 - grad*x1)/(1+grad*grad);
  double x2 = -grad*y2;
  // dist is the distance between these two points. If this is further than the width of the ladder we 
  // can use the corner of the ladder.
  double distToClosestPoint = sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
  // distance of closest approach.
  double radClosest = (y1 - grad*x1)/(sqrt(1+grad*grad));

  //msg(MSG::DEBUG) << "Distance of closest approach: " << radClosest << endreq;
  //msg(MSG::DEBUG) << "Distance along ladder surface from center to point of closest approach: " <<  distToClosestPoint << endreq;

  // Calculate the radius of the corners of the ladder.
  HepGeom::Point3D<double> ladderLowerCorner(-ladderHalfThickN, ladderHalfWidth, 0);
  HepGeom::Point3D<double> ladderUpperCorner(ladderHalfThickP, -ladderHalfWidth, 0);
  ladderLowerCorner = HepGeom::TranslateX3D(layerRadius) * HepGeom::RotateZ3D(std::abs(ladderTilt)) * ladderLowerCorner;
  ladderUpperCorner = HepGeom::TranslateX3D(layerRadius) * HepGeom::RotateZ3D(std::abs(ladderTilt)) * ladderUpperCorner;

  m_layerThicknessN = layerRadius - ladderLowerCorner.perp();
  m_layerThicknessP = ladderUpperCorner.perp() - layerRadius; // Will be recalculated below in case of additional services

  //  msg(MSG::DEBUG)<<"Max thickness : ladderhick "<<ladderHalfThickN<<"  "<<ladderHalfThickP<<endreq;
  //  msg(MSG::DEBUG)<<"Max thickness : layerthick "<<m_layerThicknessN<<"  "<<m_layerThicknessP<<endreq;

  //msg(MSG::DEBUG) << "Layer Envelope (using ladder corners): "
  //	    << layerRadius - layerThicknessN << " to " << layerRadius + layerThicknessP <<endreq;

  // If distance of closest approach is within the ladder width we use that instead
  if (distToClosestPoint < ladderHalfWidth) {
    m_layerThicknessN = layerRadius - radClosest;
  }

}
