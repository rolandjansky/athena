/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Services as Cylinders
//
// The services are grouped in 4 groups (as in G3)
// I keep this grouping as it natural in the database.
// They are Barrel-Inside tracking volume
//          Barrel-Outside tracking volume
//          Endcap-Inside tracking volume
//          Edncap-Outside tracking volume
//
// The elements for each group are defined in initialize,
// with Barrel/endcap coming from the geometry manager and inside outside,
// from the mother volume name.
// This class builds the n-th  service element, with n defined in SetCylinder
//
//
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: passed by argument
//
// Contains: nothing
//
// The services support several different shapes. The meaning of the parameters 
// depends on the particular shape: 
// 
// 
// TUBE or empty
//   Ignored: RIN2,ROUT2,PHI,WIDTH,REPEAT
// TUBS
//   Ignored: RIN2,ROUT2
//   PHI: phi start location of tube sector
//   WIDTH (Gaudi::Units::deg): phi width of sector  
//   REPEAT: Repeat the tube sector this many times in phi with equal distance between them.
// CONS, CONE
//   WIDTH,REPEAT ignored if CONE
//   RIN2,ROUT2: rmin, rmx at zmax. Same as RIN, ROUT if <=0.
//   PHI, WIDTH, REPEAT same as TUBS    
// PGON
//   Ignored: WIDTH
//   RIN,ROUT,RIN2,ROUT2 defined at corner of polygon.
//   PHI: phi start location of a corner
//   REPEAT: Sides of polygon
// PGON2
//   Ignored: WIDTH
//   RIN,ROUT,RIN2,ROUT2 defined at side of polygon.
//   PHI: phi start location of center of side.
//   REPEAT: Sides of polygon
// PGON3 (pgon at outer edge, circular inner edge)
//   Ignored: WIDTH
//   RIN,RIN2 radius of inner edge (tube or cone)
//   ROUT,ROUT2 defined at corner of polygon.
//   PHI: phi start location of a corner
//   REPEAT: Sides of polygon
// PGON31 (pgon at inner edge, circular outer edge)
//   Ignored: WIDTH
//   RIN,RIN2 radius of inner edge (tube or cone)
//   ROUT,ROUT2 defined at corner of polygon.
//   PHI: phi start location of a corner
//   REPEAT: Sides of polygon
// PGON2 (pgon at outer edge, circular inner edge)
//   Ignored: WIDTH
//   RIN,RIN2 radius of inner edge (tube or cone)
//   ROUTROUT2 defined at side of polygon.
//   PHI: phi start location of center of side.
//   REPEAT: Sides of polygon
// ROD
//   Ignored: ROUT, RIN2, ROUT2
//   RIN: Radial position of center of tube
//   PHI: phi position of center
//   WIDTH (Gaudi::Units::mm): diameter 
//   REPEAT: Repeat this many times in phi with equal distance between them.
// ROD2
//   Ignored: ROUT, RIN2, ROUT2
//   RIN: Radial position of center of tube
//   PHI: phi position of center
//   WIDTH (Gaudi::Units::mm): diameter 
//   REPEAT: Repeat this many times in phi with equal distance between them.
// BOX
//   Ignored: RIN2, ROUT2
//   ROUT-RIN = thickness of box (radially)
//   (RIN+ROUT)/2 = radial poistion of center of box
//   PHI: phi position of center
//   WIDTH (Gaudi::Units::mm) = width of box
//   REPEAT: Repeat this many times in phi with equal distance between them.
// TRAP
//   Ignored: RIN2, ROUT2
//   ROUT-RIN = thickness of trapezoid (radially)
//   (RIN+ROUT)/2 = radial poistion of center of trapzoid
//   PHI: phi position of center
//   WIDTH (Gaudi::Units::mm) = width of trapezoid at center
//   REPEAT: Repeat this many times in phi with equal distance between them.
//
//   **IMPORTANT NOTE** WIDTH can be in degrees or Gaudi::Units::mm. See OraclePixGeoManager


#include "GeoPixelServices.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/ServiceVolumeMaker.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

GeoPixelServices::GeoPixelServices(InDetDD::PixelDetectorManager* ddmgr,
                                   PixelGeometryManager* mgr,
                                   InDetDD::Zone * pixZone) 
  : GeoVPixelFactory(ddmgr, mgr),
    m_pixServBuilder(nullptr),
    m_servMatBuilder(nullptr),
    m_layerShift(0)
{
  
  // Get dimensions of barrel and endcap
  double barrelRmin = m_gmt_mgr->PixelBarrelRMin();
  double barrelRmax = m_gmt_mgr->PixelBarrelRMax();
  double barrelZmin = -m_gmt_mgr->PixelBarrelHalfLength();
  double barrelZmax = m_gmt_mgr->PixelBarrelHalfLength();
  double endcapRmin = m_gmt_mgr->PixelEndcapRMin();
  double endcapRmax = m_gmt_mgr->PixelEndcapRMax();
  double endcapZmin = m_gmt_mgr->PixelEndcapZMin();
  double endcapZmax = m_gmt_mgr->PixelEndcapZMax();
 
  double pixelRmin, pixelRmax, pixelZmax;
  if (!pixZone) {
    pixelRmin = m_gmt_mgr->PixelRMin();
    pixelRmax = m_gmt_mgr->PixelRMax();
    pixelZmax = m_gmt_mgr->PixelHalfLength();
    pixZone = new InDetDD::TubeZone("Pixel",-pixelZmax,pixelZmax,pixelRmin,pixelRmax);
  }
  else {
    if (m_gmt_mgr->PixelSimpleEnvelope()) {
      pixelRmin = m_gmt_mgr->PixelRMin();
      pixelRmax = m_gmt_mgr->PixelRMax();
      pixelZmax = m_gmt_mgr->PixelHalfLength();
    }
    else {
      pixelZmax = m_gmt_mgr->PixelEnvelopeZ(0);
      pixelRmin = m_gmt_mgr->PixelEnvelopeRMin(0);
      pixelRmax = m_gmt_mgr->PixelEnvelopeRMax(0);
    }
  }  
  
  InDetDD::UnboundedZone topZone("Mother");
  pixZone->add(new InDetDD::TubeZone("C",-endcapZmax,-endcapZmin,endcapRmin,endcapRmax, true));
  pixZone->add(new InDetDD::TubeZone("B",barrelZmin,barrelZmax,barrelRmin,barrelRmax));
  pixZone->add(new InDetDD::TubeZone("A",endcapZmin,endcapZmax,endcapRmin,endcapRmax));
  topZone.add(pixZone);

  // Collect the layer shifts / IBL 2mm shift
  m_layerShift.clear();
  for(int ii = 0; ii < m_gmt_mgr->PixelBarrelNLayer(); ii++){
    m_gmt_mgr->SetCurrentLD(ii);
    m_layerShift.push_back(m_gmt_mgr->PixelLayerGlobalShift());
  }


  // We process all tables. 
  bool barrelPresent   = m_gmt_mgr->partPresent("Barrel");
  bool endcapAPresent  = m_gmt_mgr->partPresent("EndcapA");
  bool endcapCPresent  = m_gmt_mgr->partPresent("EndcapC");

  if (barrelPresent) initialize("barrel");
  if (endcapAPresent || endcapCPresent) initialize("endcap");
  initialize("simple");
  initialize("envelope");

  // all services are added at a time
  m_pixServBuilder = new InDetDD::VolumeBuilder(topZone, m_services);
  m_pixServBuilder->setMaterialManager(m_mat_mgr);
  if (m_gmt_mgr->athenaComps()->serviceBuilderTool()) {
    const std::vector<const InDetDD::ServiceVolume *> & services = m_gmt_mgr->athenaComps()->serviceBuilderTool()->getServices();
    m_servMatBuilder = new InDetDD::VolumeBuilder(topZone, services);
    m_servMatBuilder->setMaterialManager(m_mat_mgr);
  }
}

GeoPixelServices::~GeoPixelServices() 
{
  // delete contents of m_multiParam
  for (unsigned int i = 0; i <  m_services.size(); ++i) {
    delete  m_services[i];
  }
  delete m_pixServBuilder;
  delete m_servMatBuilder;
}


void GeoPixelServices::initialize(const std::string & a)
{
  // For "simple" services use ServiceVolumeMaker to read in the table. The barrel and endcap can also be read in 
  // with ServiceVolumeMaker but there is still use of parameters specified by radiation length which is not handled by ServiceVolumeMaker
  // so we use the old version. 

  if (a!="simple") {
    return initializeOld(a);
  }

  InDetDD::ServiceVolumeSchema schema;
  std::string label;
  if (a=="simple") {
    schema.setSimpleSchema();
    label = "Svc";
  } else if (a=="barrel") { // Not used yet. initializeOld() is used instead
    schema.setPixelSchema();
    label = "Brl";
  } else if (a=="endcap") { // Not used yet. initializeOld() is used instead
    schema.setPixelSchema();
    label = "EC";
  } else {
    m_gmt_mgr->msg(MSG::ERROR) << "Unrecognized service table type: " << a << endmsg;
    return;
  } 

  IRDBRecordset_ptr table = m_gmt_mgr->getPixelServiceRecordset(a);

  InDetDD::ServiceVolumeMaker volMaker(label, table, schema, m_gmt_mgr->athenaComps());
  for (unsigned int i = 0; i < volMaker.numElements(); ++i) {
    m_services.push_back(volMaker.make(i));
  }
}


// See comment in initialize()
void GeoPixelServices::initializeOld(const std::string & a)
{
  //
  // Loop over the service elements:
  //
  int numServices =  m_gmt_mgr->PixelServiceElements(a);
  for(int ii = 0; ii < numServices; ii++) {
    

    // Will return <0 if the element doesn't belong to current inside/outside zone
    // This check is removed as it now checks accroding to the dimensions.
    //if (m_gmt_mgr->PixelServiceFrameNum(a, ii) < 0) continue;
    //
    //
    // Retrieve/calculate the parameters for the volume.
    //
    InDetDD::ServiceVolume param;
    param.setMaterial(m_gmt_mgr->PixelServiceMaterial(a, ii));
    param.setRmin(m_gmt_mgr->PixelServiceRMin(a, ii));
    param.setRmax(m_gmt_mgr->PixelServiceRMax(a, ii));
    param.setZmin(m_gmt_mgr->PixelServiceZMin(a, ii));
    param.setZmax(m_gmt_mgr->PixelServiceZMax(a, ii));
    param.setZsymm(m_gmt_mgr->PixelServiceZsymm(a, ii));
    param.setVolName(m_gmt_mgr->PixelServiceName(a, ii));

    double zShift=0.;           // the famous IBL Z shift
    int iShiftIndex = m_gmt_mgr->PixelServiceShift(a, ii);
    // FIXME: The magic number 100 here should be explained...
    // cppcheck-suppress negativeContainerIndex
    if(iShiftIndex>0) zShift=m_layerShift[iShiftIndex-100];
    param.setZShift(zShift);

    // Service envelopes
    int envNum=m_gmt_mgr->PixelServiceEnvelopeNum(a, ii);
    param.setEnvelopeNum(envNum);
    int envParent=m_gmt_mgr->PixelServiceParentEnvelopeNum(a, ii);
    param.setParentEnvelope(envParent);
    int volId = m_gmt_mgr->PixelServiceFrameNum(a, ii);

    bool needsRotation = false;

    // For TUBE there is no need to read the rest 
    std::string shapeType = m_gmt_mgr->PixelServiceShape(a, ii);
    if (!shapeType.empty() && shapeType != "TUBE") {
      double rmin2 = m_gmt_mgr->PixelServiceRMin2(a, ii);
      double rmax2 = m_gmt_mgr->PixelServiceRMax2(a, ii);
      int repeat = m_gmt_mgr->PixelServiceRepeat(a, ii);
      double phiLoc =  m_gmt_mgr->PixelServicePhiLoc(a, ii);
      double phiWidth =  m_gmt_mgr->PixelServiceWidth(a, ii);
      
      // Can be in degree or Gaudi::Units::mm. Usually it is Gaudi::Units::deg expect for BOX, TRAP and ROD shape
      // Geometry manager makes no assumptions about units. So we must interpret here.
      if (shapeType == "BOX" || shapeType == "ROD" || shapeType == "ROD2" || shapeType == "TRAP") {
	phiWidth *= Gaudi::Units::mm;
      } else {
	phiWidth *= Gaudi::Units::degree;
      }
      
      if (phiWidth == 0) phiWidth = 2*Gaudi::Units::pi;
      if (rmin2 <= 0) rmin2 = param.rmin(); 
      if (rmax2 <= 0) rmax2 = param.rmax(); 
      if (repeat == 0) repeat = 1;
      
      if (shapeType == "PGON"  || shapeType == "PGON2" || 
	  shapeType == "CONE"  || shapeType == "CONS" || 
	  shapeType == "PGON3" || shapeType == "PGON31" || shapeType == "PGON4") {
	if ((rmin2 != param.rmin()) || (rmax2 != param.rmax())) {
	  needsRotation = true;
	}
      }
      
      int sides = 0;
      int nCopies = 1;
      if (shapeType == "PGON"  || shapeType == "PGON2" ||
	  shapeType == "PGON3" || shapeType == "PGON31" || shapeType == "PGON4") {
	sides = repeat;
      } else {
	nCopies = repeat;
      }
      
      // Force nCopies to 1 for TUBE and CONE 
      if (shapeType.empty() || shapeType == "TUBE" || shapeType == "CONE") {
	nCopies = 1;
      }
      
      param.setShapeType(shapeType);
      param.setRmin2(rmin2);
      param.setRmax2(rmax2);
      param.setPhiLoc(phiLoc);
      param.setPhiWidth(phiWidth);
      param.setSides(sides);
      param.setNCopies(nCopies);
    }

    param.setNeedsRotation(needsRotation);
    
    //
    // If zin is 0... (within 10^-5) this is a volume symmetric around
    // the origin
    //
    if(std::abs(param.zmin()) < 0.000001) {
      param.setZmin(-param.zmax());
      param.setZsymm(false);
    }	

    std::string label;
    if(envNum>0) label="Env";
    else if (a == "barrel") label = "Brl";
    else if (a == "endcap") label = "EC";
    else if (a == "simple") label = "Svc";
    param.setLabel(label, volId); 

    // Fill m_multiVolumes;
    m_services.push_back(new InDetDD::ServiceVolume(param));
    
  }
}
  
// For interface
GeoVPhysVol* GeoPixelServices::Build() 
{
  return nullptr;
}
