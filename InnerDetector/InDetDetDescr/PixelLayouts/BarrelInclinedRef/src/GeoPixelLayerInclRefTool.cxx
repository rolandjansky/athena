/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"
#include "BarrelInclinedRef/GeoPixelLayerInclRefTool.h"
#include "BarrelInclinedRef/GeoPixelSlimStaveSupportInclRef.h"
#include "PixelInterfaces/IPixelLayerValidationTool.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

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

GeoPixelLayerInclRefTool::GeoPixelLayerInclRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_layer(-1),
    m_bPreBuild(false),
    //    m_IDserviceTool("InDetservicesTool/PixelServicesTool"),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc"),
    m_validationTool("LayerValidationTool"),
    m_validationMode(false)
{
  declareInterface<IGeoPixelLayerTool>(this);

  declareProperty("LayerValidationTool", m_validationTool);
  declareProperty("LayerValidation", m_validationMode);
}

GeoPixelLayerInclRefTool::~GeoPixelLayerInclRefTool()
{
}

//================ Initialisation =================================================

StatusCode GeoPixelLayerInclRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelLayerInclRefTool initialize() successful in " << name());

//   sc = m_IDserviceTool.retrieve(); 
//   if (sc.isFailure()){
//     msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
//     return sc;
//   }
//   msg(MSG::DEBUG) << "Service builder tool retrieved: " << endreq;

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }

  if (m_validationMode) {
    if (m_validationTool.retrieve().isSuccess()){
      ATH_MSG_DEBUG("Layer validation tool successfully retrieved " << m_validationTool );
    } else {
      ATH_MSG_INFO("layer validation tool: Couldn't retrieve " << m_validationTool );
    }
  }
    
  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelLayerInclRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelLayerInclRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelLayerInclRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}


void GeoPixelLayerInclRefTool::preBuild(const PixelGeoBuilderBasics* /*basics*/, int iLayer)
{
  m_layer=iLayer;
  m_bPreBuild = true;

}

GeoVPhysVol* GeoPixelLayerInclRefTool::buildLayer(const PixelGeoBuilderBasics* basics, int iLayer) 
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
  double staveOffset       = layerTmp->stave_zoffset;   
  std::string staveType    = layerTmp->stave_type;
  double phiOfStaveZero    = layerTmp->phioffset;
  double phiOfStaveZeroLastLayer = (m_layer == 0) ? 0 : m_xmlReader->getPixelBarrelLayerTemplate(m_layer-1)->phioffset;    
  int nSectorsLastLayer = (m_layer == 0) ? 0 : m_xmlReader->getPixelBarrelLayerTemplate(m_layer-1)->stave_n; 
  int nSectorsNextLayer = (m_layer == int(m_xmlReader->nbOfPixelBarrelLayers())-1) ? 0 : m_xmlReader->getPixelBarrelLayerTemplate(m_layer+1)->stave_n; 
  

  msg(MSG::DEBUG)<<layerName<<" "<<staveType<<" "<<nSectors<<endreq;
  msg(MSG::DEBUG)<<"*****************************************************************************"<<endreq;

  double deltaPhi = 360.*CLHEP::deg/(double)nSectors;

  HepGeom::Transform3D transRadiusAndTilt = HepGeom::TranslateX3D(layerRadius)*HepGeom::RotateZ3D(ladderTilt);

  std::vector<InDet::StaveTmp*> staveTmp = m_xmlReader->getPixelStaveTemplate(m_layer);
  std::vector<GeoPixelLadderInclRef* > vPixelLadder;

  int nbLadderType = staveTmp.size();
  for(int iLadder=0; iLadder<nbLadderType; iLadder++)
    {
      vPixelLadder.push_back( new GeoPixelLadderInclRef(basics, staveTmp[iLadder], m_layer, nSectors, nSectorsLastLayer, nSectorsNextLayer, phiOfStaveZero, phiOfStaveZeroLastLayer, transRadiusAndTilt)); 
      if(iLadder==0)ComputeLayerThickness(*vPixelLadder[iLadder], ladderTilt, layerRadius);
    }

  // Register the number of stave defined for the layer
  basics->getDetectorManager()->numerology().setNumPhiModulesForLayer(m_layer,nSectors);

  GeoFullPhysVol* layerPhys = 0;

  // Loop over the sectors and place everything
  //
  int ladderCmpt=0;
  for(int ii = 0; ii < nSectors; ii++) {

    msg(MSG::DEBUG)<<"LADDER build : "<<ladderCmpt<<"  / "<<nbLadderType<<endreq;

    // Build ladder
    vPixelLadder[ladderCmpt]->setSector(ii);
    GeoVPhysVol *ladderPhys=vPixelLadder[ladderCmpt]->Build();

    if(ii==0){

      double safety = 5. * CLHEP::mm;
      //      double rmin =  layerRadius-m_layerThicknessN - safety;
      //      double rmax =  layerRadius+m_layerThicknessP + safety;
      double rmin =  vPixelLadder[ladderCmpt]->rmin()-safety;
      double rmax =  vPixelLadder[ladderCmpt]->rmax()+safety;
      double ladderLength = vPixelLadder[ladderCmpt]->envLength() + 4*basics->epsilon(); // Ladder has length gmt_mgr->PixelLadderLength() +  2*m_epsilon

      msg(MSG::DEBUG)<<"Layer "<<m_layer<<" in/out radius init "<<rmin<<"  "<<rmax<<endreq;


      // Enlarge layer envelope if longerons are being built.
      PixelInclRefStaveXMLHelper staveHelperCurrLayer(m_layer, basics);
      if (staveHelperCurrLayer.getStaveSupportType() == "Longeron") {
	rmax = staveHelperCurrLayer.getRadialMidpointAtEOS();
	
	// Last layer longeron conects to PST, so extend envelope some more (refine this!)
	if (m_layer == int(m_xmlReader->nbOfPixelBarrelLayers())-1) rmax += fabs(staveHelperCurrLayer.getRadialMidpointAtEOS() - layerRadius);
      }

      // If previous layer used a longeron, extend this layer's envelope down to meet it
      if (m_layer>0) {
	PixelInclRefStaveXMLHelper staveHelperLastLayer(m_layer-1, basics);
	if (staveHelperLastLayer.getStaveSupportType() == "Longeron") rmin = staveHelperLastLayer.getRadialMidpointAtEOS();
      }

      // Now make the layer envelope
      msg(MSG::DEBUG)<<"Layer "<<m_layer<<" in/out radius "<<rmin<<"  "<<rmax<<endreq;
      const GeoMaterial* air = basics->matMgr()->getMaterial("std::Air");
      std::ostringstream lname;
      lname << "Layer" << m_layer;
      const GeoTube* layerTube = new GeoTube(rmin,rmax,0.5*ladderLength); //solid
      const GeoLogVol* layerLog = new GeoLogVol(lname.str(),layerTube,air); //log volume
      layerPhys = new GeoFullPhysVol(layerLog); // phys vol
    }

    double ladderOffset = (ii%2==0)? staveOffset: -staveOffset;
    double phiOfSector = phiOfStaveZero + ii*deltaPhi;
    HepGeom::Transform3D ladderTransform = HepGeom::TranslateZ3D(ladderOffset) * HepGeom::RotateZ3D(phiOfSector) * transRadiusAndTilt;

    //
    // Place the active ladders
    //
    std::ostringstream nameTag; 
    nameTag << "Ladder" << ii;
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    GeoTransform* xform = new GeoTransform(ladderTransform);
    layerPhys->add(tag);
    layerPhys->add(new GeoIdentifierTag(ii) );
    layerPhys->add(xform);
    layerPhys->add(ladderPhys);   //pixelLadder->Build());

    ladderCmpt++;
    if(ladderCmpt>nbLadderType-1) ladderCmpt=0;
  }




  // Shorthand for enum (should this be in header?)
  typedef GeoPixelSlimStaveSupportInclRef::halfStaveType halfStaveType;

  // Check both sides of the layer to see if a half-stave shape is needed
  for (auto staveHalf : std::vector<halfStaveType> {halfStaveType::INNER, halfStaveType::OUTER}){
    bool buildStaveSupport = false;
    std::vector<halfStaveType> ShellsToBuild;

    int innerLayerID = -1;
    int owningLayerID = -1; // Phi rotation done relative to this layer ID
    
    PixelInclRefStaveXMLHelper staveDBHelper(m_layer, basics);

    // Do we need an inner shell to complete the outer shell built on previous layer?
    if (staveHalf == halfStaveType::INNER && m_layer != 0 ) {
      PixelInclRefStaveXMLHelper staveDBHelper_LastLayer(m_layer-1, basics);
      if (staveDBHelper_LastLayer.getStaveSupportType() == "Longeron") {
	buildStaveSupport = true;
	innerLayerID = m_layer-1;
	owningLayerID = staveDBHelper_LastLayer.getOwningLayer();
	ShellsToBuild.push_back(halfStaveType::INNER);
      }
    }

    // Default - build a stave half-shell going outwards
    if (staveHalf == halfStaveType::OUTER  &&  staveDBHelper.getStaveSupportType() == "Longeron") {
      buildStaveSupport = true;
      innerLayerID = m_layer;
      owningLayerID = staveDBHelper.getOwningLayer();
      ShellsToBuild.push_back(halfStaveType::OUTER);
      
      // Last layer is a special case - outer shell will be full longeron (inner+outer halves)
      if (m_layer == m_xmlReader->nbOfPixelBarrelLayers()-1) ShellsToBuild.push_back(halfStaveType::INNER);
    }
    
   
    // Construct the actual stave shape - should this take into account owning layer starting phi?
    // Typically only build one shell.  However, the last layer has to build three half-shells
    //  Inner therefore completes last layer longeron, outer = inner+outer of shell linking to PST
    if (buildStaveSupport) {
       for (auto staveBuildHalf : ShellsToBuild) {
	 double gapPlanarStave = (m_layer > 1) ? 4.0 : 0; // Why hardcoded?

	 int nOwningLayerStaves = m_xmlReader->getPixelBarrelLayerTemplate(owningLayerID)->stave_n;
	 double staveDeltaPhi = (360.0*CLHEP::deg) / double(nOwningLayerStaves);
	 
	 PixelInclRefStaveXMLHelper innerLayerStaveDBHelper(innerLayerID, basics);
	 double layerPhiModuleZero = m_xmlReader->getPixelBarrelLayerTemplate(owningLayerID)->phioffset;
	 double currentPhi = (staveDeltaPhi * innerLayerStaveDBHelper.getStartPhi()) + layerPhiModuleZero;
	 double maxPhi     = (currentPhi < 0) ?  360.*CLHEP::deg + currentPhi : 360.*CLHEP::deg;
	 
	 int staveShapeIndex = 0;
	 while (currentPhi < maxPhi){
	   GeoPixelSlimStaveSupportInclRef *m_staveSupport = new GeoPixelSlimStaveSupportInclRef(basics, innerLayerID, gapPlanarStave, staveBuildHalf, staveShapeIndex);
	   GeoVPhysVol *stavePhys = m_staveSupport->Build();
	   
	   // Computer phi angle of stave
	   HepGeom::Transform3D staveTransform = HepGeom::RotateZ3D(currentPhi) * m_staveSupport->transform();
	   GeoTransform* xformStave = new GeoTransform(staveTransform);
	   
	   layerPhys->add(xformStave);
	   layerPhys->add(stavePhys);
	   
	   // update phi angle for next module
	   currentPhi   += staveDeltaPhi * innerLayerStaveDBHelper.getPhiStepSize(staveShapeIndex);
	   staveShapeIndex++;
	   if (staveShapeIndex == innerLayerStaveDBHelper.getNStaveShapes()) staveShapeIndex = 0;
	 }
       }
    }
  }


  if (m_validationMode && m_validationTool) m_validationTool->printInfo(layerPhys,iLayer);

  return layerPhys;

}


void GeoPixelLayerInclRefTool::ComputeLayerThickness(const GeoPixelLadderInclRef& pixelLadder, double ladderTilt, double layerRadius)
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

