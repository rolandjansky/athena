/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#include "PixelServicesTool/PixelRoutingServiceXMLHelper.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

#include "CLHEP/Evaluator/Evaluator.h"

PixelRoutingServiceXMLHelper::PixelRoutingServiceXMLHelper(const std::string& envName, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics)  
{

  msg(MSG::DEBUG)<<"XML helper - PixelRoutingServiceXMLHelper"<<endmsg;
    
  msg(MSG::DEBUG)<<"SimpleServiceVolumeMakerMgr : env name "<<envName<<endmsg;
  std::string fileName;
  if(const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      msg(MSG::DEBUG)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      msg(MSG::DEBUG)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      msg(MSG::DEBUG)<<" PixelServices : "<<file<<endmsg;
      InitializeXML();
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    msg(MSG::ERROR) << "XML file "<<fileName<<"("<<envName<<")"<<" not found"<<endmsg;
    return;
    }
}

PixelRoutingServiceXMLHelper::~PixelRoutingServiceXMLHelper()
{
  TerminateXML();
}


int PixelRoutingServiceXMLHelper::getRouteNumber() const
{
  return getChildCount("PixelRoutingServices",0,"PixelSvcRoute");

}

std::vector<int> PixelRoutingServiceXMLHelper::getRouteLayerList( int index) const
{
  return getVectorInt("PixelSvcRoute",index,"layer");
}

bool PixelRoutingServiceXMLHelper::isBarrelRoute(int index) const
{
  return (getString("PixelSvcRoute",index,"type").find("barrel")==0);

}


int PixelRoutingServiceXMLHelper::getRouteServiceMaterialIndex(int index, int iLayer) const
{
  std::vector<std::string> svcList = getVectorString("PixelSvcRoute",index,"service");

  if(svcList.size()==1) 
    return getChildValue_Index("ServiceSet","name",-1,svcList[0]);

  std::vector<int> layerList = getRouteLayerList(index);
  int iCmpt=0;
  for (auto & layer:layerList){
    if(layer==iLayer)
      return getChildValue_Index("ServiceSet","name",-1,svcList[iCmpt]);
    iCmpt++;
  }
  
  return -1;
}

double PixelRoutingServiceXMLHelper::getRouteThickness( int index) const
{
  return getDouble("PixelSvcRoute",index,"thickness");  
}

std::vector<std::string> PixelRoutingServiceXMLHelper::getRouteRadialPositions(int index) const
{
  return getVectorString("PixelSvcRoute",index,"r");  
}

std::vector<std::string> PixelRoutingServiceXMLHelper::getRouteZPositions( int index) const
{
  return getVectorString("PixelSvcRoute",index,"z");  
}

std::string PixelRoutingServiceXMLHelper::getRouteType(int index) const
{
  return getString("PixelSvcRoute",index,"type");  
}

bool PixelRoutingServiceXMLHelper::isPhiRouting(int index) const
{
  if(getChildCount("PixelSvcRoute",index,"isPhiRouting") > 0)
    return getInt("PixelSvcRoute",index,"isPhiRouting");
  return false;
}

bool PixelRoutingServiceXMLHelper::isPhiRouting(const std::string& ctype, int layer) const
{
  int nbRoute = getRouteNumber();
  for(int irt=0; irt<nbRoute; irt++) {
    std::string t = getRouteType(irt);
    if(t == ctype){
      std::vector<int> routeLayerList = getRouteLayerList(irt);
      for (auto & routeLayer:routeLayerList){
	if(routeLayer==layer) {
	  if(getChildCount("PixelSvcRoute",irt,"isPhiRouting") > 0){
	    return getInt("PixelSvcRoute",irt,"isPhiRouting");
	  }
	  else
	    return false;
	}
      }
    }
  }
  return false;
}

int PixelRoutingServiceXMLHelper::getServiceSetIndex(const std::string& ctype, int layer, int module) const
{
  if(ctype == "barrel"){
    int brlSvcTypeIndex = getChildValue_Index("PixelBarrelSvcType","layer",layer);   
    std::vector<std::string> tmp = getVectorString("PixelBarrelSvcType",brlSvcTypeIndex,"service");
    return getChildValue_Index("ServiceSet","name",-1,tmp[module]);
  }
  
  int nbRoute = getRouteNumber();
  for(int irt=0; irt<nbRoute; irt++) {
    std::string t = getRouteType(irt);
    if(t == ctype){
      std::vector<int> routeLayerList = getRouteLayerList(irt);
      for (auto & routeLayer:routeLayerList){
	if(routeLayer==layer) {
	  std::string serviceClass = "service_L"; serviceClass += std::to_string(layer);
	  std::vector<std::string> serviceName;
	  if (getChildCount("PixelSvcRoute",irt,serviceClass.c_str()) > 0)
	    serviceName = getVectorString("PixelSvcRoute",irt,serviceClass.c_str());
	  else if (getChildCount("PixelSvcRoute",irt,"service") > 0)
	    serviceName = getVectorString("PixelSvcRoute",irt,"service");
	  else
	    return getServiceSetIndex("barrel", layer, module);
	  
	  return getChildValue_Index("ServiceSet","name",-1,serviceName[module]);
	}
      }
    }
  }
  return -1;
}

std::vector<std::string> PixelRoutingServiceXMLHelper::getServiceSetContent(int index) const
{
  std::vector<std::string> res;

  int nbSvc=getChildCount("ServiceSet",index,"Service");  
  for(int i=0; i<nbSvc; i++)
    res.push_back( "svc "+getString("ServiceSet",index,"Service",i));  

  nbSvc=getChildCount("ServiceSet",index,"Cooling");  
  for(int i=0; i<nbSvc; i++)
    res.push_back( "cooling "+getString("ServiceSet",index,"Cooling",i));  

  return res;
}

std::string PixelRoutingServiceXMLHelper::getServiceSetName(int index) const
{
  return getString("ServiceSet",index,"name");  
}

std::vector<std::string> PixelRoutingServiceXMLHelper::getTypeMaterialNames( int layer, const std::string& pattern) const
{
  std::vector<std::string> matNameList;
  int brlSvcTypeIndex = getChildValue_Index("PixelBarrelSvcType","layer",layer);
  
  std::vector<std::string> tmp = getVectorString("PixelBarrelSvcType",brlSvcTypeIndex,"service");  

  for(auto& it : tmp){
    int svcIndex = getChildValue_Index("ServiceSet","name",-1,it);
    std::vector<std::string> svcList = getServiceSetContent(svcIndex);

    for(auto& it2 : svcList) {
      // Decode string vs ' ' pattern
      std::vector<std::string>res;
      std::istringstream s(it2);
      std::string tmp;
      while (s >> tmp) res.push_back(tmp);

      std::string multiplicity = res[2];
      if (multiplicity.find(pattern)!=std::string::npos) matNameList.push_back(res[1]);
    }
  }

  std::sort(matNameList.begin(), matNameList.end()); 
  auto last = std::unique(matNameList.begin(), matNameList.end());
  matNameList.erase(last, matNameList.end());
  return matNameList;
}


/// EOS card parameters

int PixelRoutingServiceXMLHelper::getRouteEOSCardIndex(int index) const
{
  std::string EOSname=getString("PixelSvcRoute",index,"EOS");
  return getChildValue_Index("EOSCard","name",-1,EOSname.c_str());
}

double PixelRoutingServiceXMLHelper::getEOSsvcLength(int index) const
{
  return getDouble("PixelSvcRoute",index,"EOSsvclength",0.);  
}

double PixelRoutingServiceXMLHelper::getEOSCardLength(int index) const
{
  return getDouble("PixelBarrelEOScard",index,"length");
}

std::vector<double> PixelRoutingServiceXMLHelper::EOScardLength(int index) const
{
  std::vector<double> cardLength;
  if(!isBarrelRoute(index)) return cardLength;

  std::vector<std::string> cardList = getVectorString("PixelSvcRoute",index,"EOScard");  


  for(auto& it : cardList)
    {
      int cardIndex = getChildValue_Index("PixelBarrelEOScard","name",-1,it);
      msg(MSG::DEBUG)<<"CARD : "<<it<<" "<<cardIndex<<endmsg;
      cardLength.push_back(getEOSCardLength(cardIndex));
    }
  return cardLength;
}

double PixelRoutingServiceXMLHelper::getZGap(int index) const
{
  return getDouble("PixelSvcRoute",index,"zgap", 0);
}

double PixelRoutingServiceXMLHelper::getRGap(int index) const
{
  return getDouble("PixelSvcRoute",index,"rgap", 0);
}

double PixelRoutingServiceXMLHelper::getMaterialFudgeModuleSvc(int iLayer) const
{
  std::ostringstream targetString;
  targetString << iLayer;
  return getMaterialFudgeGeneric(targetString.str(), "MaterialFudgeModuleSvc");
}

double PixelRoutingServiceXMLHelper::getMaterialFudgeSvcEc(int iLayer) const
{
  std::ostringstream targetString;
  if (iLayer<4) {
    targetString << iLayer;
  } else if (iLayer==4) {
    targetString << "00";
  }
  return getMaterialFudgeGeneric(targetString.str(), "MaterialFudgeSvcEc");
}

double PixelRoutingServiceXMLHelper::getMaterialFudgeGeneric(const std::string& Layer, const std::string& node) const
{

  std::ostringstream targetString;
  targetString << "FudgeL" << Layer;
  //
  bool foundFudges=false;
  bool foundFudgedSvcs=false;
  std::vector<std::string> nodes = getNodeList("PixelRoutingServices");
  for (unsigned int i=0; i<nodes.size(); i++) {
    if (nodes[i]=="MaterialFudgeFactors") foundFudges=true;
    if (nodes[i]==node) foundFudgedSvcs=true;
  }
  //
  HepTool::Evaluator eval;
  if (foundFudges) {
    std::vector<std::string> fudges = getNodeList("MaterialFudgeFactors");
    for (unsigned int i=0; i<fudges.size(); i++) {
      eval.setVariable(fudges[i].c_str(),getDouble("MaterialFudgeFactors",0,fudges[i].c_str()));
    }
  }
  //
  double theFudge = 1.;
  if (!foundFudgedSvcs) return theFudge;
  std::vector<std::string> fudges2 = getNodeList(node.c_str());
  for (unsigned int i=0; i<fudges2.size(); i++) {
    if (fudges2[i]==targetString.str()) {
      std::string fudgeString = getString(node.c_str(),0,fudges2[i].c_str());
      theFudge = eval.evaluate(fudgeString.c_str());
      if (eval.status() != HepTool::Evaluator::OK) {
	std::cerr << "PixelRoutingServiceXMLHelper: Error processing CLHEP Evaluator expression. Error name "
		  << eval.error_name() << std::endl << "Message: ";
	eval.print_error();
	std::cerr << fudgeString << std::endl;
	std::cerr << std::string(eval.error_position(), '-') << '^' << '\n';
	std::cerr << "Exiting program.\n";
	throw std::runtime_error("Error processing CLHEP evaluator for Pixel Service Routing!");

      }
    }
  }
  return theFudge;

}

int  PixelRoutingServiceXMLHelper::getNumSectors(int index) const
{
  if(getChildCount("PixelSvcRoute",index,"nSectors") > 0)
    return getInt("PixelSvcRoute",index,"nSectors");
  return 1;
}

double  PixelRoutingServiceXMLHelper::getPhiRefFirstSector(int index) const
{
  if(getChildCount("PixelSvcRoute",index,"phiRefFirstSector") > 0)
    return getDouble("PixelSvcRoute",index,"phiRefFirstSector");
  return 0.;
}

double  PixelRoutingServiceXMLHelper::getSectorVolumeWidth(int index) const
{
  if(getChildCount("PixelSvcRoute",index,"sectorVolumeWidth") > 0)
    return getDouble("PixelSvcRoute",index,"sectorVolumeWidth");
  return 0.;
}

bool PixelRoutingServiceXMLHelper::splitLayersInPhi(int index) const
{
  if(getChildCount("PixelSvcRoute",index,"splitLayersInPhi") > 0)
    return getInt("PixelSvcRoute",index,"splitLayersInPhi");
  return false;
}

int PixelRoutingServiceXMLHelper::phiSplitStepInSectors(int index) const
{
  if(getChildCount("PixelSvcRoute",index,"phiSplitStepInSectors") > 0)
    return getInt("PixelSvcRoute",index,"phiSplitStepInSectors");
  return 1;
}
