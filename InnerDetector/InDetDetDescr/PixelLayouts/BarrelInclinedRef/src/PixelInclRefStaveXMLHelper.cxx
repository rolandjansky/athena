
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
      basics->msgStream()<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      basics->msgStream()<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    }

  // No XML file was parsed    
  if(!bParsed){
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
  }

  m_layerIndices = getChildValue_Indices("PixelStaveGeo","Layer",m_layer);

  //  std::cout<<"PixelInclRefStaveXMLHelper for layer "<<m_layer<<" "<<m_layerIndices[0]<<" "<<m_layerIndices[1]<<std::endl;
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
  return getDouble("PixelStaveGeo", m_layerIndices, "BarrelModuleDZ", 0, -1.);
}


double PixelInclRefStaveXMLHelper::getStaveSupportLength() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "EnvelopeLength");
}

double PixelInclRefStaveXMLHelper::getStaveSupportWidth() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "StaveSupportWidth",0, -1);
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
  return "";
}

double PixelInclRefStaveXMLHelper::getPigtailAngle(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "PigtailAngle");
if (vec.size() != 0) return vec.at(shapeIndex);
return 0.0;
}

double PixelInclRefStaveXMLHelper::getPigtailDR(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "PigtailDR");
if (vec.size() != 0) return vec.at(shapeIndex);
return 0.0;
}

std::string PixelInclRefStaveXMLHelper::getStaveSupportMaterial(int shapeIndex) const
{
  std::vector<std::string> vec = getVectorString("PixelStaveGeo", m_layerIndices, "StaveSupportMaterialGeo");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return "";
  //return getString("PixelStaveGeo", m_layerIndices, "StaveSupportMaterialGeo");
}

std::string PixelInclRefStaveXMLHelper::getStaveSupportCornerMaterial(int shapeIndex) const
{
  std::vector<std::string> vec = getVectorString("PixelStaveGeo", m_layerIndices, "StaveSupportCornerMaterialGeo");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return "";
  //return getString("PixelStaveGeo", m_layerIndices, "StaveSupportMaterialGeo");
}

std::string PixelInclRefStaveXMLHelper::getStaveSupportEOSMaterial(int shapeIndex) const
{
  std::vector<std::string> vec = getVectorString("PixelStaveGeo", m_layerIndices, "StaveSupportEOSMaterialGeo");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return "";
}

double PixelInclRefStaveXMLHelper::getMaterialFudge() const
{
  bool foundFudges=false;
  std::vector<std::string> nodes = getNodeList("PixelStaveGeos");
  for (unsigned int i=0; i<nodes.size(); i++) {
    if (nodes[i]=="MaterialFudgeFactors") foundFudges=true;
  }
  HepTool::Evaluator eval;
  if (foundFudges) {
    std::vector<std::string> fudges = getNodeList("MaterialFudgeFactors");
    for (unsigned int i=0; i<fudges.size(); i++) {
      eval.setVariable(fudges[i].c_str(),getDouble("MaterialFudgeFactors",0,fudges[i].c_str()));
    }
  }
  //
  double theFudge = 1.;
  std::string fudgeString = getString("PixelStaveGeo", m_layerIndices, "MaterialFudge", 0, "1.");
  theFudge = eval.evaluate(fudgeString.c_str());
  if (eval.status() != HepTool::Evaluator::OK) {
    std::cerr << "PixelInclRefStaveXMLHelper: Error processing CLHEP Evaluator expression. Error name " <<
      eval.error_name() << std::endl << "Message: ";
    eval.print_error();
    std::cerr << fudgeString << std::endl;
    std::cerr << std::string(eval.error_position(), '-') << '^' << '\n';
    std::cerr << "Exiting program.\n";
    exit(999); // Should do better...
  }
  return theFudge;
}

double PixelInclRefStaveXMLHelper::getServiceOffsetX() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetX");
}

double PixelInclRefStaveXMLHelper::getServiceECOffsetX() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "ServiceECOffsetX", 0, 0.);
}

double PixelInclRefStaveXMLHelper::getServiceOffsetY() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "ServiceOffsetY");
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
  return getDouble("PixelStaveGeo", m_layerIndices, "MountainWidth", 0.0, 0.7);
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
  std::string fudgeString = getString("PixelStaveGeos", m_layerIndices, "CurlyMaterialFudge", 0, "1.");
  theFudge = eval.evaluate(fudgeString.c_str());
  if (eval.status() != HepTool::Evaluator::OK) {
    std::cerr << "PixelInclRefStaveXMLHelper: Error processing CLHEP Evaluator expression. Error name " <<
      eval.error_name() << std::endl << "Message: ";
    eval.print_error();
    std::cerr << fudgeString << std::endl;
    std::cerr << std::string(eval.error_position(), '-') << '^' << '\n';
    std::cerr << "Exiting program.\n";
    exit(999); // Should do better...
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
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getTopWidthAtEOS(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "TopWidthEOS");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getBarrelWidth(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "BarrelWidth");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getBarrelZMax(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "BarrelZMax");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getBarrelZMaxHighR(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "BarrelZMaxHighR");
  if (vec.size() != 0) return vec.at(shapeIndex);
  msg(MSG::DEBUG) << "BarrelZMaxHighR not found: going to use BarrelZMax instead..." << endreq;
  return getBarrelZMax(shapeIndex);
}


double PixelInclRefStaveXMLHelper::getRadialLengthAtEOS(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "RadialLengthEOS");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getWallThickness(int shapeIndex) const
{ 
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "WallThickness");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getCornerThickness(int shapeIndex) const
{ 
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "CornerThickness");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getXStepLowR(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "XStepLowR");
  if (vec.size() != 0) return vec.at(shapeIndex);
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getXStepHighR(int shapeIndex) const
{
  std::vector<double> vec = getVectorDouble("PixelStaveGeo", m_layerIndices, "XStepHighR");
  if (vec.size() != 0) return vec.at(shapeIndex);
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
  return 0.0;
}

double PixelInclRefStaveXMLHelper::getGapPlanarStave() const
{
  return getDouble("PixelStaveGeo", m_layerIndices, "GapPlanarStave");
}
