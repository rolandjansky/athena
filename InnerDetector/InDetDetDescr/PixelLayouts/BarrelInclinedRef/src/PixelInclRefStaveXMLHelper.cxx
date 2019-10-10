/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

#include "CLHEP/Evaluator/Evaluator.h"

PixelInclRefStaveXMLHelper::PixelInclRefStaveXMLHelper(int layer, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_layer(layer)
{

  std::string fileName="GenericExtRefStave.xml";
  if(const char* env_p = std::getenv("PIXEL_STAVESUPPORT_GEO_XML")) fileName = std::string(env_p);

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      basics->msgStream()<<MSG::DEBUG<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      setSchemaVersion(dbUtils.getSchemaVersion(fileName));
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      basics->msgStream()<<MSG::DEBUG<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    }

  // No XML file was parsed    
  if(!bParsed){
    basics->msgStream()<<MSG::ERROR<<"XML file "<<fileName<<" not found"<<endreq;
    return;
  }

  m_layerIndices = getChildValue_Indices("PixelStaveGeo","Layer",m_layer);
}

PixelInclRefStaveXMLHelper::~PixelInclRefStaveXMLHelper()
{
  TerminateXML();
}


double PixelInclRefStaveXMLHelper::getClearance() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"ClearanceX");
}

double PixelInclRefStaveXMLHelper::getStaggerDist() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"StaggerDist");
}

double PixelInclRefStaveXMLHelper::getStaggerSign() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "StaggerSign");
}

double PixelInclRefStaveXMLHelper::getCenterShift() const
{
  return getDouble("PixelStaveGeo",m_layerIndices,"ModuleCenterShift");
}

double PixelInclRefStaveXMLHelper::getBarrelModuleDZ() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "BarrelModuleDZ", 0);
}


double PixelInclRefStaveXMLHelper::getStaveSupportLength() const
{
    return getDouble("PixelStaveGeo", m_layerIndices, "EnvelopeLength");
}

double PixelInclRefStaveXMLHelper::getStaveSupportWidth() const
{
  if(getSchemaVersion() > 4) {
    return getDouble("PixelStaveGeo", m_layerIndices, "StaveSupportWidth",0);
  }
  else msg(MSG::DEBUG)<<"XML: PixelStaveGeo StaveSupportWidth not defined in old schema ("<<getSchemaVersion()<<") returning -999 as non-physical value to trigger special behaviour in step 2.2 layouts (ATLAS-P2-ITK-20-04-00)..."<<endreq;
  return -999.0;
}

double PixelInclRefStaveXMLHelper::getStaveSupportThick() const
{
    return getDouble("PixelStaveGeo", m_layerIndices, "LadderThickness");
}

//for pigtail
std::string PixelInclRefStaveXMLHelper::getPigtailMaterial(int shapeIndex) const
{
  std::vector<std::string> vec = getVectorString("PixelStaveGeo", m_layerIndices, "PigtailMaterialGeo");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo PigtailMaterialGeo!"<<endreq;
  return "";
}

double PixelInclRefStaveXMLHelper::getPigtailAngle(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "PigtailAngle");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo PigtailAngle!"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getPigtailDR(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "PigtailDR");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo PigtailDR"<<endreq;
  return 0.0;
}

std::string PixelInclRefStaveXMLHelper::getStaveSupportMaterial(int shapeIndex) const
{
  std::vector<std::string> vec = getVectorString("PixelStaveGeo", m_layerIndices, "StaveSupportMaterialGeo");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo StaveSupportMaterialGeo"<<endreq;
  return "";
}

std::string PixelInclRefStaveXMLHelper::getStaveSupportCornerMaterial(int shapeIndex) const
{
  std::vector<std::string> vec = getVectorString("PixelStaveGeo", m_layerIndices, "StaveSupportCornerMaterialGeo");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Could't find PixelStaveGeo StaveSupportCornerMaterialGeo"<<endreq;
  return "";
}

std::string PixelInclRefStaveXMLHelper::getStaveSupportEOSMaterial(int shapeIndex) const
{
  if(getSchemaVersion() > 1) {
    std::vector<std::string> vec = getVectorString("PixelStaveGeo", m_layerIndices, "StaveSupportEOSMaterialGeo");
    if (vec.size() != 0) return vec.at(shapeIndex);
    else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo StaveSupportEOSMaterialGeo"<<endreq;
    return "";
    }
  else msg(MSG::DEBUG)<<"XML: PixelStaveGeo StaveSupportEOSMaterialGeo not defined in old schema ("<<getSchemaVersion()<<") returning empty string..."<<endreq;
  return "";
}

double PixelInclRefStaveXMLHelper::getMaterialFudge() const
{
  //Is this really still needed?
  bool foundFudges=false;
  std::vector<std::string> nodes = getNodeList("PixelStaveGeos");
  for (unsigned int i=0; i<nodes.size(); i++) {
    if (nodes[i]=="MaterialFudgeFactors") foundFudges=true;
  }
  HepTool::Evaluator eval;
  double theFudge = 1.;
  if (foundFudges) {
    std::vector<std::string> fudges = getNodeList("MaterialFudgeFactors");
    for (unsigned int i=0; i<fudges.size(); i++) {
      eval.setVariable(fudges[i].c_str(),getDouble("MaterialFudgeFactors",0,fudges[i].c_str()));
    }
  }
  else return theFudge;
  //
  std::string fudgeString = getString("PixelStaveGeo", m_layerIndices, "MaterialFudge", 0);
  theFudge = eval.evaluate(fudgeString.c_str());
  if (eval.status() != HepTool::Evaluator::OK) {
    std::cerr << "PixelInclRefStaveXMLHelper: Error processing CLHEP Evaluator expression. Error name " <<
      eval.error_name() << std::endl << "Message: ";
    eval.print_error();
    std::cerr << fudgeString << std::endl;
    std::cerr << std::string(eval.error_position(), '-') << '^' << '\n';
    std::cerr << "Exiting program.\n";
    throw std::runtime_error("Error processing CLHEP evaluator for PixelStaveGeo!");

  }
  return theFudge;
}

double PixelInclRefStaveXMLHelper::getServiceOffsetX() const
{
  if(getSchemaVersion() > 1) {
    return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetX");
  }
  else msg(MSG::DEBUG) << "XML: ServiceOffsetX not defined in old schema ("<<getSchemaVersion()<<") returning 0.0..." << endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getServiceECOffsetX() const
{
  if(getSchemaVersion() > 4) {
    return getDouble("PixelStaveGeo", m_layerIndices, "ServiceECOffsetX", 0);
  }
  else msg(MSG::DEBUG) << "XML: ServiceECOffsetX not (fully) defined in old schema ("<<getSchemaVersion()<<") returning 0.5 for layer 1 if step 3.0 or 3.1, 0.0 otherwise..." << endreq;
  if ((getSchemaVersion() > 1) && (m_layerIndices.back()==2)) return 0.5;
  else return 0.0;
 
}

double PixelInclRefStaveXMLHelper::getServiceOffsetY() const
{
  if(getSchemaVersion() > 1) {
    return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetY");
  }
  else msg(MSG::DEBUG) << "XML: ServiceOffsetY not defined in old schema ("<<getSchemaVersion()<<") returning 0..." << endreq;
  return 0.0;
}

std::string PixelInclRefStaveXMLHelper::getSvcRoutingPos() const
{
  return getString("PixelStaveGeo", m_layerIndices, "ServiceRouting");
}

double PixelInclRefStaveXMLHelper::getMountainEdge() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "MountainEdge");
}

double PixelInclRefStaveXMLHelper::getMountainWidth() const
{
  if(getSchemaVersion() > 1) {
    return getDouble("PixelStaveGeo", m_layerIndices, "MountainWidth", 0);
  }
  else msg(MSG::DEBUG) << "XML: MountainWidth not defined in old schema ("<<getSchemaVersion()<<") going to use default value of 0.7..." << endreq;
  
  return 0.7;
  
}

double PixelInclRefStaveXMLHelper::getCurlyMaterialFudge() const
{
  bool foundFudges=false;
  bool foundCurlyFudge=false;
  std::vector<std::string> nodes = getNodeList("PixelStaveGeos");
  for (unsigned int i=0; i<nodes.size(); i++) {
    if (nodes[i]=="MaterialFudgeFactors") foundFudges=true;
    if (nodes[i]=="CurlyMaterialFudge") foundCurlyFudge=true;
  }
  double theFudge = 1.;
  if (!foundCurlyFudge) return theFudge;
  //
  HepTool::Evaluator eval;
  if (foundFudges) {
    std::vector<std::string> fudges = getNodeList("MaterialFudgeFactors");
    for (unsigned int i=0; i<fudges.size(); i++) {
      eval.setVariable(fudges[i].c_str(),getDouble("MaterialFudgeFactors",0,fudges[i].c_str()));
    }
  }
  //
  std::string fudgeString = getString("PixelStaveGeos", m_layerIndices, "CurlyMaterialFudge", 0);
  theFudge = eval.evaluate(fudgeString.c_str());
  if (eval.status() != HepTool::Evaluator::OK) {
    std::cerr << "PixelInclRefStaveXMLHelper: Error processing CLHEP Evaluator expression. Error name " <<
      eval.error_name() << std::endl << "Message: ";
    eval.print_error();
    std::cerr << fudgeString << std::endl;
    std::cerr << std::string(eval.error_position(), '-') << '^' << '\n';
    std::cerr << "Exiting program.\n";
    throw std::runtime_error("Error processing CLHEP evaluator for PixelStaveGeos!");

  }
  return theFudge;
}


std::string PixelInclRefStaveXMLHelper::getStaveSupportType() const
{
  std::string type = getString("PixelStaveGeo", m_layerIndices, "StaveSupportType");
  if (type == "Standard" || type == "Longeron" || type == "None")  return type;

  // If here, something wrong:
  msg(MSG::WARNING) << "StaveSupportType \"" << type << "\" not recognised, defaulting to Standard (Check your XML)" << endreq;
  return "Standard";
}

// Would prefer this to use getInt - but getInt does not accept m_layerIndices
int PixelInclRefStaveXMLHelper::getNStaveShapes() const
{
  return int(getDouble("PixelStaveGeo", m_layerIndices, "NStaveShapes"));
}

double PixelInclRefStaveXMLHelper::getRadialMidpointAtEOS() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "RadialMidpointEOS");
}

double PixelInclRefStaveXMLHelper::getBaseWidthAtEOS(int shapeIndex) const
{
  // Check all "layer indices" (i.e. "all" and layer <n>", the first valid vector returned gioves the parameter
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "BaseWidthEOS");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo BarrelWidthEOS!"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getTopWidthAtEOS(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "TopWidthEOS");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo TopWidthEOS"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getBarrelWidth(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "BarrelWidth");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo BarrelWidth"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getBarrelZMax(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "BarrelZMax");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo BarrelZMax"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getBarrelZMaxHighR(int shapeIndex) const
{
   if(getSchemaVersion() > 1) {
     std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "BarrelZMaxHighR");
     if (vec.size() != 0) return vec.at(shapeIndex);
     else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo BarrelZMaxHighR"<<endreq;
     return getBarrelZMax(shapeIndex);
   }
   else msg(MSG::DEBUG) << "XML: BarrelZMaxHighR not defined in old schema ("<<getSchemaVersion()<<") going to use BarrelZMax instead..." << endreq;
   return getBarrelZMax(shapeIndex);
}


double PixelInclRefStaveXMLHelper::getRadialLengthAtEOS(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "RadialLengthEOS");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo RadialLengthEOS!"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getWallThickness(int shapeIndex) const
{ 
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "WallThickness");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo WallThickness!"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getCornerThickness(int shapeIndex) const
{ 
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "CornerThickness");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo CornerThickness!"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getXStepLowR(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "XStepLowR");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo TopWidthEOS"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getXStepHighR(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "XStepHighR");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo XStepHighR"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getStartPhi() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "StartPhi");
}

double PixelInclRefStaveXMLHelper::getOwningLayer() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "PhiFromLayer");
}

double PixelInclRefStaveXMLHelper::getPhiStepSize(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "PhiStepSize");
  if (vec.size() != 0) return vec.at(shapeIndex);
  else msg(MSG::ERROR)<<"Couldn't find PixelStaveGeo PhiStepSize!"<<endreq;
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getGapPlanarStave() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "GapPlanarStave");
}
