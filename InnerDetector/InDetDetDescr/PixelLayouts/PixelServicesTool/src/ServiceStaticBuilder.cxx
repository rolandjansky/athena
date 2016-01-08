/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
//   WIDTH (CLHEP::deg): phi width of sector  
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
//   WIDTH (CLHEP::mm): diameter 
//   REPEAT: Repeat this many times in phi with equal distance between them.
// ROD2
//   Ignored: ROUT, RIN2, ROUT2
//   RIN: Radial position of center of tube
//   PHI: phi position of center
//   WIDTH (CLHEP::mm): diameter 
//   REPEAT: Repeat this many times in phi with equal distance between them.
// BOX
//   Ignored: RIN2, ROUT2
//   ROUT-RIN = thickness of box (radially)
//   (RIN+ROUT)/2 = radial poistion of center of box
//   PHI: phi position of center
//   WIDTH (CLHEP::mm) = width of box
//   REPEAT: Repeat this many times in phi with equal distance between them.
// TRAP
//   Ignored: RIN2, ROUT2
//   ROUT-RIN = thickness of trapezoid (radially)
//   (RIN+ROUT)/2 = radial poistion of center of trapzoid
//   PHI: phi position of center
//   WIDTH (CLHEP::mm) = width of trapezoid at center
//   REPEAT: Repeat this many times in phi with equal distance between them.
//
//   **IMPORTANT NOTE** WIDTH can be in degrees or CLHEP::mm. See OraclePixGeoManager

#include "PixelServicesTool/ServiceStaticBuilder.h"
#include "PixelSCTDBHelper/GeoPixelGeneralDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelServicesDBHelper.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

/*#include "PixelServicesTool/VolumeGeoCompBuilder.h"*/
#include "PixelServicesTool/VolumeStandardBuilder.h"
#include "PixelServicesTool/SimpleServiceVolumeMaker.h"

#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

ServiceStaticBuilder::ServiceStaticBuilder(const PixelGeoBuilderBasics* basics, /*IInDetServMatBuilderTool * serviceBuilderTool ,*/InDetDD::Zone * pixZone) 
  : PixelGeoBuilder(basics) ,
    m_pixServBuilder(0),
    m_services(0)
{

  // Envelope interface
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");

  // We process all tables. 
  bool barrelPresent   = genDBHelper.isBarrelPresent();
  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  if (barrelPresent) initialize("barrel");
  if (endcapAPresent || endcapCPresent) initialize("endcap");
  initialize("simple");
  //  initialize("envelope");

  msg(MSG::INFO)<<"GEOPIXELSERVICES constructor # services : "<<m_services.size()<<endreq;

//  // If not slhc, procced as usual : all services are added at a time
//   //  if not SLHC, check all services vs pixel

  msg(MSG::INFO)<<" ************************** ServiceStaticBuilder built - BEGIN  - pixServBuilder ****************************"<<endreq;

  //  m_pixServBuilder = new InDetDD::VolumeStandardBuilder(*pixZone, m_services);
  //  m_pixServBuilder->setMaterialManager(matMgr());

  InDetDD::VolumeStandardBuilder* volStdBuilder = new InDetDD::VolumeStandardBuilder(*pixZone, m_services);
  volStdBuilder->setMaterialManager(matMgr());

  m_pixServBuilder.push_back(volStdBuilder);

  msg(MSG::INFO)<<" ************************** ServiceStaticBuilder built - END  - svcBuilderTool ****************************"<<endreq;
    
  return;

}

ServiceStaticBuilder::~ServiceStaticBuilder() 
{
  // delete contents of m_multiParam
  for (unsigned int i = 0; i < m_services.size(); ++i) {
    delete  m_services[i];
  }
  
  for(std::vector<InDetDD::VolumeStandardBuilder*>::iterator it=m_pixServBuilder.begin(); it!=m_pixServBuilder.end(); ++it)
    delete (*it);
}


void ServiceStaticBuilder::addTopZone( InDetDD::Zone * pixZone)
{
  InDetDD::VolumeStandardBuilder* volStdBuilder = new InDetDD::VolumeStandardBuilder(*pixZone, m_services);
  volStdBuilder->setMaterialManager(matMgr());

  m_pixServBuilder.push_back(volStdBuilder);
}

void ServiceStaticBuilder::initialize(const std::string & a)
{
  // For "simple" services use ServiceVolumeMaker to read in the table. The barrel and endcap can also be read in 
  // with ServiceVolumeMaker but there is still use of parameters specified by radiation length which is not handled by ServiceVolumeMaker
  // so we use the old version. 

  msg(MSG::INFO)<<"ServiceStaticBuilder::initialize "<<a<<endreq;

//   if (a!="simple") {
//     return initializeOld(a);
//   }

  GeoPixelServicesDBHelper srvDBHelper(getBasics());

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
    msg(MSG::ERROR) << "Unrecognized service table type: " << a << endreq;
    return;
  } 

  IRDBRecordset_ptr table = srvDBHelper.getPixelServiceRecordset(a);
  
  msg(MSG::INFO)<<"ServiceStaticBuilder::initialize : ServiceVolumeMaker "<<a<<endreq;
  InDetDD::SimpleServiceVolumeMaker volMaker(label, table, schema, &geoAccessor(),false);
  msg(MSG::INFO)<<"ServiceStaticBuilder::initialize : ServiceVolumeMaker : # services "<<a<<"  "<<volMaker.numElements()<<endreq;
  for (unsigned int i = 0; i < volMaker.numElements(); ++i) {
    m_services.push_back(volMaker.make(i));
  }
}


// See comment in initialize()
void ServiceStaticBuilder::initializeOld(const std::string & a)
{
  //
  // Loop over the service elements:
  //

  GeoPixelServicesDBHelper srvDBHelper(getBasics());


  int numServices =  srvDBHelper.PixelServiceElements(a);
  for(int ii = 0; ii < numServices; ii++) {
    

    // Will return <0 if the element doesn't belong to current inside/outside zone
    // This check is removed as it now checks accroding to the dimensions.
    //if (srvDBHelper.PixelServiceFrameNum(a, ii) < 0) continue;
    //
    //
    // Retrieve/calculate the parameters for the volume.
    //
    InDetDD::ServiceVolume param;
    param.setMaterial(srvDBHelper.PixelServiceMaterial(a, ii));
    param.setRmin(srvDBHelper.PixelServiceRMin(a, ii));
    param.setRmax(srvDBHelper.PixelServiceRMax(a, ii));
    param.setZmin(srvDBHelper.PixelServiceZMin(a, ii));
    param.setZmax(srvDBHelper.PixelServiceZMax(a, ii));
    param.setZsymm(srvDBHelper.PixelServiceZsymm(a, ii));
    param.setVolName(srvDBHelper.PixelServiceName(a, ii));

    // Service envelopes
    int envNum=srvDBHelper.PixelServiceEnvelopeNum(a, ii);
    param.setEnvelopeNum(envNum);
    int envParent=srvDBHelper.PixelServiceParentEnvelopeNum(a, ii);
    param.setParentEnvelope(envParent);
    int volId = srvDBHelper.PixelServiceFrameNum(a, ii);

    bool needsRotation = false;

    // For TUBE there is no need to read the rest 
    std::string shapeType = srvDBHelper.PixelServiceShape(a, ii);
    if (!shapeType.empty() && shapeType != "TUBE") {
      double rmin2 = srvDBHelper.PixelServiceRMin2(a, ii);
      double rmax2 = srvDBHelper.PixelServiceRMax2(a, ii);
      int repeat = srvDBHelper.PixelServiceRepeat(a, ii);
      double phiLoc =  srvDBHelper.PixelServicePhiLoc(a, ii);
      double phiWidth =  srvDBHelper.PixelServiceWidth(a, ii);
      
      // Can be in degree or CLHEP::mm. Usually it is CLHEP::deg expect for BOX, TRAP and ROD shape
      // Geometry manager makes no assumptions about units. So we must interpret here.
      if (shapeType == "BOX" || shapeType == "ROD" || shapeType == "ROD2" || shapeType == "TRAP") {
	phiWidth *= CLHEP::mm;
      } else {
	phiWidth *= CLHEP::degree;
      }
      
      if (phiWidth == 0) phiWidth = 2*3.1415926;  //CLHEP::pi;   FIXME SES
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
GeoVPhysVol* ServiceStaticBuilder::Build() 
{
  return 0;
}


double ServiceStaticBuilder::getServiceNumber(int iBuilder) 
{ 
  return m_pixServBuilder[iBuilder]->getServiceNumber(); 
}

// GeoSimpleObject* ServiceStaticBuilder::getServiceObject(int i) 
// { 
//   //return m_pixServBuilder->getServiceObject(i); 
//   return 0;
// }
