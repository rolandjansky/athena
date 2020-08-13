/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "PixelGeoComponent/GeoDetModuleSimple.h"
#include "PixelGeoComponent/GeoSimpleObject.h"
#include "PixelGeoComponent/GeoSensor.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

using std::max;


GeoDetModulePixel::GeoDetModulePixel(const PixelGeoBuilderBasics* basics, int moduleIndex,  int brl_ec, int layerdisk):
  GeoXMLUtils(), m_basics(basics), m_moduleIndex(moduleIndex), m_brl_ec(brl_ec), m_layerdisk(layerdisk)
{
//   //
//   // Define the log volume in the constructor, so I do it only once.
//   //
//   // The module orientation is
//   // x normal to the detector.
//   // y in the phi direction
//   // z in the eta (z) direction

//   m_isModule3D=m_theSensor.GetModule3DFlag();
  
  m_matMgr = m_basics->matMgr();
  _id=-1;
  //  m_msg = basics->msgStream();

  preBuild();
  
}

void GeoDetModulePixel::preBuild()
{
  //  m_basics->msgStream()<<" GeoDetModule* GeoDetModulePixel::build"<<endreq;

  // -----------------------------------------------------------------------------
  // Read data from the XML file -------------------------------------------------

  std::string fileName="PixelModules.xml";
  if(const char* env_p = std::getenv("PIXEL_SILICONMODULES_GEO_XML")) fileName = std::string(env_p);
  m_basics->msgStream()<<MSG::DEBUG<<"XML file - modules  "<<fileName<<endreq;

  bool readXMLfromDB = m_basics->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      m_basics->msgStream()<< MSG::DEBUG <<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(m_basics);
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      setSchemaVersion(dbUtils.getSchemaVersion(fileName));
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      m_basics->msgStream()<<MSG::DEBUG << "XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      InitializeXML();
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      //should have here also a method to set the schema Version from an Xml file...
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    m_basics->msgStream()<<MSG::WARNING << "XML file "<<fileName<<" not found"<<endmsg;
    return;
  }

  std::string moduleName = getChildValue("Module", m_moduleIndex, "moduleName");
  std::string chipName = getChildValue("Module", m_moduleIndex, "chipName");
  int chipIndex = getChildValue_Index("FrontEndChip", "name", -1, chipName);

  // Get gchip and hybrid geometry from geo xml file
  int geoModuleIndex = getChildValue_Index("ModuleGeo", "name", -1, moduleName);

  m_basics->msgStream()<<MSG::DEBUG<<"MODULE XML : "<<m_moduleIndex<<"  - "<<moduleName<<" "<<chipName<<" "<<chipIndex<<"   geoIndex "<<geoModuleIndex<<endreq;

  // chip configuration
  int lengthChip = getInt("Module", m_moduleIndex, "lengthInChips");
  int widthChipMax = getInt("Module", m_moduleIndex, "widthMaxInChips", 0);
 

  int widthChip = widthChipMax;
  m_chipNumber = widthChip * lengthChip;

  m_basics->msgStream()<<MSG::DEBUG<<"ChipWidth : "<<widthChip<<endreq;

  //  sensor / chip and hybrid thicknesses 
  m_boardThick =  getDouble("Module", m_moduleIndex, "sensorThickness");
  m_chipThick =  getDouble("Module", m_moduleIndex, "chipThickness", 0);
  m_hybridThick =  getDouble("Module", m_moduleIndex, "hybridThickness", 0);

  //  chip sizes

  double edgel = 0.;
  double edgew = 0.;
  if(getSchemaVersion() < 5){
    edgel = getDouble("FrontEndChip", chipIndex, "inlength");
    edgew = getDouble("FrontEndChip", chipIndex, "wide");
  }
  else{
    edgel = getDouble("Module", m_moduleIndex, "inlength");
    edgew = getDouble("Module", m_moduleIndex, "wide");
  }

  // MODULE sizes : chip sizes are set to the sizes defined in the XML files
  //                edges are added to the sensor size (sensor are named board in this code)

  m_chipWidth = widthChip * getDouble("FrontEndChip", chipIndex, "chipWidth");
  m_chipLength = lengthChip * getDouble("FrontEndChip", chipIndex, "chipLength");
  if(getSchemaVersion() < 5){
    // Not relevant for proper quad chips immplementation
    m_chipWidth  += (widthChip-1)*2.*edgew;
    m_chipLength += (lengthChip-1)*2.*edgel;
  }

  //This is not always present, and so is usually zero
  m_chipGap = 0.0; 
  if(getSchemaVersion() > 3) m_chipGap = getDouble("FrontEndChip", chipIndex, "chipGap", 0);
  else  m_basics->msgStream()<<MSG::DEBUG<<"MODULE XML : "<<m_moduleIndex<<"  - "<<moduleName<<" "<<chipName<<" "<<chipIndex<<"   geoIndex "<<geoModuleIndex<<" old schema ("<<getSchemaVersion()<<") setting chipGap to zero..."<<endreq;

  // sensor geometry
  m_boardWidth = m_chipWidth + 2.*edgew;
  m_boardLength = m_chipLength + 2.*edgel;

  // Hybrid size
  m_hybridWidth = m_hybridLength = 0.;
  if(m_hybridThick>0){
    m_hybridWidth = getDouble("ModuleGeo", geoModuleIndex, "hybridWidth", 0);
    m_hybridLength = getDouble("ModuleGeo", geoModuleIndex, "hybridLength", 0);
  }

  m_chipWidthOffset=0.;
  m_chipLengthOffset=0.;
  m_hybridWidthOffset=0.;
  m_hybridLengthOffset=0.;

  //  m_chipMatName="pix::Chip";
  //  m_hybridMatName="pix::Hybrid";
  m_hybridMatName = getString("ModuleGeo", geoModuleIndex, "hybridMat", 0);
  m_chipMatName = getString("ModuleGeo", geoModuleIndex, "chipMat", 0);
  m_sensorMatName = getString("ModuleGeo", geoModuleIndex, "sensorMat", 0);

  TerminateXML();

  m_basics->msgStream()<<MSG::DEBUG<<"GeoDetModulePixel - Build module "<<moduleName<<"  "<<m_moduleIndex<<endreq;
  m_basics->msgStream()<<MSG::DEBUG<<"board  : "<<m_boardThick<<" "<<m_boardLength<<" "<<m_boardWidth<<endreq;
  m_basics->msgStream()<<MSG::DEBUG<<"hybrid : "<<m_hybridThick<<" "<<m_hybridLength<<" "<<m_hybridWidth<<endreq;
  m_basics->msgStream()<<MSG::DEBUG<<"chip   : "<<m_chipThick<<" "<<m_chipLength<<" "<<m_chipWidth<<"  gap : "<<m_chipGap<<endreq;

  m_basics->msgStream()<<MSG::DEBUG<<"MODULE thickness : "<<ThicknessN()<<" "<<ThicknessP()<<" // "<<Thickness()<<endreq;

  m_basics->msgStream()<<MSG::DEBUG<<" ********** end of module prebuild"<<endreq;

  // -----------------------------------------------------------------------------
  // Build the module ------------------------------------------------------------

  //
  // The Dimensions are in separate routines
  //
  double length = this->Length();
  double thickness = this->Thickness();
  double width = this->Width();

  m_basics->msgStream()<<MSG::DEBUG<<"Size : "<<length<<" "<<thickness<<" "<<width<<endreq;

  const GeoMaterial* air = m_matMgr->getMaterial("std::Air");

  const GeoShape * moduleShape = 0;
  const GeoBox* moduleBox = new GeoBox(thickness/2.,width/2.,length/2.);
  if (ThicknessP() == ThicknessN()) {
    moduleShape = moduleBox;
  } else {
    // Shift so the center of the box is the center of the sensor.
    double shift = 0.5 * (ThicknessP() - ThicknessN());
    const GeoShape & shiftedBox = (*moduleBox) << HepGeom::TranslateX3D(shift);
    moduleShape = &shiftedBox;
  }
  //  std::string logName = gmt_mgr->isBarrel() ? "ModuleBrl" : "ModuleEC";
  std::string logName = "Module";
  m_moduleLogVol = new GeoLogVol(logName,moduleShape,air);

  // Build sensor logical volume
  logName = "siLog";
  //  if(m_brl_ec==0&&m_layerdisk==0)logName = "siBLayLog";
  const GeoMaterial* siMat = m_matMgr->getMaterial(m_sensorMatName);
  const GeoBox* siBox = new GeoBox(m_boardThick/2.,m_boardWidth/2.,m_boardLength/2.);
  m_siLogVol = new GeoLogVol(logName,siBox,siMat);

}


GeoFullPhysVol* GeoDetModulePixel::Build(int brl_ec, int /*layer_disk*/, int /*phi*/, int /*eta*/, int idModTag, std::string modName)
{


  //  m_basics->msgStream()<<MSG::DEBUG<<"GeoVPhysVol* GeoDetModulePixel::Build()"<<endreq;

  GeoFullPhysVol* modulePhys = new GeoFullPhysVol(m_moduleLogVol);

  std::string suffix = (brl_ec==0)? "Brl" : "EC";

  //  m_basics->msgStream()<<MSG::DEBUG<<"Module services sizes : hyb/chip "<<m_hybridThick<<" "<<m_chipThick<<endreq;

  //
  // Place the Hybrid
  //
  if (m_hybridThick>0.00001*CLHEP::mm){

    const GeoBox* hybridBox = new GeoBox(m_hybridThick/2.,m_hybridWidth/2.,m_hybridLength/2.);
    std::string matName = m_hybridMatName;
    const GeoMaterial* hybridMat = m_matMgr->getMaterialForVolume(matName,hybridBox->volume());

    std::string logName = "Hybrid"+suffix;
    GeoLogVol* theHybrid = new GeoLogVol(logName,hybridBox,hybridMat);
    GeoPhysVol* hybridPhys = new GeoPhysVol(theHybrid);
    
    double hybxpos = -0.5*(m_boardThick+m_hybridThick);
    GeoTransform* xform = new GeoTransform(HepGeom::TranslateX3D(hybxpos));
    modulePhys->add(xform);
    modulePhys->add(hybridPhys);
  }

  //
  // Place the Chip
  //

  std::string logName = "Chip"+suffix;
  const GeoBox* chipBox = new GeoBox(m_chipThick/2.,m_chipWidth/2.,m_chipLength/2.);
  std::string matName = m_chipMatName;

  //  std::cout<<"Chip Thick = " << m_chipThick<<" "<<m_chipWidth<<" "<<m_chipLength<<" : "<<matName<<std::endl;
  const GeoMaterial* chipMat = m_matMgr->getMaterialForVolume(matName,chipBox->volume());
  //  m_basics->msgStream() << "Chip Thick = " << m_chipThick<<" "<<m_chipWidth<<" "<<m_chipLength<<" : "<<matName<<endreq;

  GeoLogVol* theChip = new GeoLogVol(logName,chipBox,chipMat);
  GeoPhysVol* chipPhys = new GeoPhysVol(theChip);

  double chipxpos = 0.5*(m_boardThick+m_chipThick)+m_chipGap;
  GeoTransform* xform = new GeoTransform(HepGeom::TranslateX3D(chipxpos));
  modulePhys->add(xform);
  modulePhys->add(chipPhys);


  std::ostringstream nameTag; 
  nameTag << "SensorBrl" << modName;
  GeoNameTag * tag = new GeoNameTag(nameTag.str());

  int idTag = 100; // barrel (same for slhc/non slhc)
  if(brl_ec!=0){
    //    idTag = (gmt_mgr->slhc()) ? 300 : 200; // endcap (300: slhc, 200: non slhc)
    idTag = 300;
  }
  if(idModTag>0) idTag = idModTag;
  GeoFullPhysVol* siPhys = new GeoFullPhysVol(m_siLogVol);
  GeoTransform* si_xform = new GeoTransform(HepGeom::Transform3D());
  modulePhys->add(new GeoIdentifierTag(idTag) );
  modulePhys->add(tag);
  modulePhys->add(si_xform);
  modulePhys->add(siPhys);

//  resetIdentifierFlags();
  return modulePhys;

}


InDet::GeoDetModule* GeoDetModulePixel::buildGeoComp(int /*brl_ec*/, int /*layer_disk*/)
{

  GeoFullPhysVol* modulePhys = new GeoFullPhysVol(m_moduleLogVol);

  InDet::GeoDetModuleSimple *module = new InDet::GeoDetModuleSimple(modulePhys);

  //
  // Place the Hybrid
  //
  if (m_hybridThick>0.00001*CLHEP::mm){

    const GeoBox* hybridBox = new GeoBox(m_hybridThick/2.,m_hybridWidth/2.,m_hybridLength/2.);
    std::string matName = m_hybridMatName;
    const GeoMaterial* hybridMat = m_matMgr->getMaterialForVolume(matName,hybridBox->volume());

    std::string logName = "Hybrid";
    GeoLogVol* theHybrid = new GeoLogVol(logName,hybridBox,hybridMat);
    GeoPhysVol* hybridPhys = new GeoPhysVol(theHybrid);
    
    double hybxpos = -0.5*(m_boardThick+m_hybridThick);
    const HepGeom::Transform3D hybridTrf = HepGeom::TranslateX3D(hybxpos);
    //    GeoTransform* xform = new GeoTransform(HepGeom::TranslateX3D(hybxpos));
    //    modulePhys->add(xform);
    //    modulePhys->add(hybridPhys);
    InDet::GeoSimpleObject* hybridComponent = new InDet::GeoSimpleObject( hybridPhys);
    module->placeSensorService( *hybridComponent, hybridTrf, 0, 0);
  }

  //
  // Place the Chip
  //

  std::string logName = "Chip";
  //m_basics->msgStream() << "Chip Thick = " << thickness << endreq;
  const GeoBox* chipBox = new GeoBox(m_chipThick/2.,m_chipWidth/2.,m_chipLength/2.);

  std::string matName = m_chipMatName;
  const GeoMaterial* chipMat = m_matMgr->getMaterialForVolume(matName,chipBox->volume());

  GeoLogVol* theChip = new GeoLogVol(logName,chipBox,chipMat);
  GeoPhysVol* chipPhys = new GeoPhysVol(theChip);
  double chipxpos = 0.5*(m_boardThick+m_chipThick)+m_chipGap;
  const HepGeom::Transform3D chipTrf = HepGeom::TranslateX3D(chipxpos);
  //  GeoTransform* xform = new GeoTransform(chipTrf);
  //  modulePhys->add(xform);
  //  modulePhys->add(chipPhys);
  InDet::GeoSimpleObject* chipComponent = new InDet::GeoSimpleObject( chipPhys);
  module->placeSensorService( *chipComponent, chipTrf, 0, 0);

  //  const GeoMaterial* siMat = m_matMgr->getMaterial(m_sensorMatName);
  //  const GeoBox* siBox = new GeoBox(m_boardThick/2.,m_boardWidth/2.,m_boardLength/2.);
  // There is not a strong need to give the blayer a different name but leave it for now. 
  logName = "siLog";

//   int idTag = 100; // barrel (same for slhc/non slhc)
//   if(brl_ec!=0){
//     //    idTag = (gmt_mgr->slhc()) ? 300 : 200; // endcap (300: slhc, 200: non slhc)
//     idTag = 300;
//   }
  GeoFullPhysVol* siPhys = new GeoFullPhysVol(m_siLogVol);
  const HepGeom::Transform3D siTrf = HepGeom::Transform3D();
  //  GeoTransform* si_xform = new GeoTransform(HepGeom::Transform3D());
  InDet::GeoSensor* siComponent = new InDet::GeoSensor( siPhys);
  module->placeSensor( *siComponent, siTrf, 0, 0);

  return module;

}

double GeoDetModulePixel::ThicknessN() const  {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessN
  // is the max of ThicknessP and thickness from the module center to
  // the outer surface of the hybrid plus some safety.
  //

   double safety = 0.001*CLHEP::mm; 
   double thickn = 0.5 * m_boardThick+ m_hybridThick + safety;
   //   double thick = std::max(thickn, ThicknessP()); 

   return thickn;
}

double GeoDetModulePixel::ThicknessP() const {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessP
  // is thickness from the module center to the outer surface of the
  // chips plus some safety.

   double safety = 0.001*CLHEP::mm;
   double thick = 0.5 * m_boardThick+m_chipThick+m_chipGap + safety;

   return thick;

}

double GeoDetModulePixel::Thickness() const {
  // This is total thickness of the module envelope
  return ThicknessP()+ThicknessN();
}

double GeoDetModulePixel::Width() const {
  double width = std::max( std::max(
				    m_boardWidth,
				    m_hybridWidth+fabs(m_hybridWidthOffset)),
			   m_chipWidth+fabs(m_chipWidthOffset));
  return width+0.001;
}

double GeoDetModulePixel::Length() const {
  // balcony is zero

  double length = std::max( std::max(m_hybridLength+fabs(m_hybridLengthOffset), m_boardLength),
				     m_chipLength+fabs(m_chipLengthOffset));			    
  return length+0.001;
}

// Identifier GeoDetModulePixel::getID()  {
//   return _id;
// }
