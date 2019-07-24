/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelServicesTool/VolumeStandardBuilder.h"
#include "PixelServicesTool/SimpleServiceVolumeMaker.h"

#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

ServiceStaticBuilder::ServiceStaticBuilder(const PixelGeoBuilderBasics* basics, InDetDD::Zone * pixZone) 
  : PixelGeoBuilder(basics) ,
    m_pixServBuilder(0),
    m_services(0)
 {
  // Envelope interface
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",basics);

  // We process all tables. 
  bool barrelPresent   = genDBHelper.isBarrelPresent();
  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  if (barrelPresent) initialize("barrel");
  if (endcapAPresent || endcapCPresent) initialize("endcap");
  initialize("simple");
  
  msg(MSG::INFO)<<"GEOPIXELSERVICES constructor # services : "<<m_services.size()<<endreq;

  msg(MSG::DEBUG)<<" ************************** ServiceStaticBuilder built - BEGIN  - pixServBuilder ****************************"<<endreq;

  InDetDD::VolumeStandardBuilder* volStdBuilder = new InDetDD::VolumeStandardBuilder(*pixZone, m_services);
  volStdBuilder->setMaterialManager(matMgr());

  m_pixServBuilder.push_back(volStdBuilder);

  msg(MSG::DEBUG)<<" ************************** ServiceStaticBuilder built - END  - svcBuilderTool ****************************"<<endreq;
    
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
  msg(MSG::INFO)<<"ServiceStaticBuilder::initialize "<<a<<endreq;

  InDetDD::SimpleServiceVolumeSchema schema;
  std::string label;
  std::string table;
  if (a=="simple") {
    schema.setPixelSchema();
    table = "PIXEL_PIXELSIMPLESERVICE_GEO_XML";
    label = "Svc";
  } else if (a=="barrel") { 
    schema.setPixelSchema();
    table = "PIXEL_PIXELBARRELSERVICE_GEO_XML";
    label = "Brl";
  } else if (a=="endcap") { 
    schema.setPixelSchema();
    table = "PIXEL_PIXELENDCAPSERVICE_GEO_XML";
    label = "EC";
  } else {
    msg(MSG::ERROR) << "Unrecognized service table type: " << a << endreq;
    return;
  } 
  
  msg(MSG::DEBUG)<<"ServiceStaticBuilder::initialize : ServiceVolumeMaker "<<a<<endreq;
  InDetDD::SimpleServiceVolumeMaker volMaker(table,label,schema, getBasics(),false);
  msg(MSG::DEBUG)<<"ServiceStaticBuilder::initialize : ServiceVolumeMaker : # services "<<a<<"  "<<volMaker.numElements()<<endreq;
  for (unsigned int i = 0; i < volMaker.numElements(); ++i) 
      m_services.push_back(volMaker.make(i));
  
  return;
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

