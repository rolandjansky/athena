/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
#include "PixelServicesTool/PixelRoutingServiceXMLHelper.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelRoutingServiceXMLHelper::PixelRoutingServiceXMLHelper(std::string envFileName, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics)  
{

  std::cout<<"XML helper - PixelRoutingServiceXMLHelper"<<std::endl;
    
  std::string envName = envFileName;
  std::cout<<"SimpleServiceVolumeMakerMgr : env name "<<envName<<std::endl;
  std::string fileName;
  if(const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);

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
      std::cout<<" PixelServices : "<<file<<std::endl;
      InitializeXML();
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
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
  return (getString("PixelSvcRoute",index,"type")=="barrel");

}


int PixelRoutingServiceXMLHelper::getRouteServiceMaterialIndex(int index, int iLayer) const
{
  std::vector<std::string> svcList = getVectorString("PixelSvcRoute",index,"service");

  if(svcList.size()==1) 
    return getChildValue_Index("ServiceSet","name",-1,svcList[0]);

  std::vector<int> layerList = getRouteLayerList(index);
  int iCmpt=0;
  for(std::vector<int>::iterator it=layerList.begin(); it!=layerList.end(); ++it){
    if(*it==iLayer)
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


int PixelRoutingServiceXMLHelper::getEndcapServiceSetIndex(int layer) const
{
  int nbRoute = getRouteNumber();
  for(int irt=0; irt<nbRoute; irt++)
    {
      std::string t = getRouteType(irt);
      if(t=="endcap"){
	std::vector<int> routeList = getRouteLayerList(irt);
	for(std::vector<int>::iterator it=routeList.begin(); it!=routeList.end(); ++it)
	  if(*it==layer){
	    std::string serviceName = getString("PixelSvcRoute",irt,"service");
	    return getChildValue_Index("ServiceSet","name",-1,serviceName);
	  }
      }
    }
  return -1;
}

int PixelRoutingServiceXMLHelper::getBarrelServiceSetIndex(int layer, int module) const
{
  int brlSvcTypeIndex = getChildValue_Index("PixelBarrelSvcType","layer",layer);
  
  std::vector<std::string> tmp = getVectorString("PixelBarrelSvcType",brlSvcTypeIndex,"service");  
  return getChildValue_Index("ServiceSet","name",-1,tmp[module]);
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
std::string PixelRoutingServiceXMLHelper::getServiceSetNameId(int index) const
{
  return getString("ServiceSet",index,"id");  
}

std::vector<std::string> PixelRoutingServiceXMLHelper::getTypeMaterialNames( int layer, std::string pattern)
{
  std::vector<std::string> matNameList;
  int brlSvcTypeIndex = getChildValue_Index("PixelBarrelSvcType","layer",layer);
  
  std::vector<std::string> tmp = getVectorString("PixelBarrelSvcType",brlSvcTypeIndex,"service");  

//   for(int iMod=0; iMod<(int)tmp.size(); iMod++){

//     int svcIndex = getChildValue_Index("ServiceSet","name",-1,tmp[iMod]);
//     std::vector<std::string> svcList = getServiceSetContent(svcIndex);

//     for(std::vector<std::string>::iterator it=svcList.begin(); it!=svcList.end(); ++it) {
//       // Decode string vs ' ' pattern
//       std::vector<std::string>res;
//       std::istringstream s(*it);
//       std::string tmp;
//       while (s >> tmp) res.push_back(tmp);

//       std::string multiplicity = res[2];
//       if (multiplicity.find(pattern)!=std::string::npos) matNameList.push_back(res[1]);
//     }
//   }
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

//   for(int i=0; i<(int)cardList.size(); i++)
//     {
//       int cardIndex = getChildValue_Index("PixelBarrelEOScard","name",-1,cardList[i]);
//       std::cout<<"CARD : "<<cardList[i]<<" "<<cardIndex<<std::endl;
//       cardLength.push_back(getEOSCardLength(cardIndex));
//     }
  for(auto& it : cardList)
    {
      int cardIndex = getChildValue_Index("PixelBarrelEOScard","name",-1,it);
      std::cout<<"CARD : "<<it<<" "<<cardIndex<<std::endl;
      cardLength.push_back(getEOSCardLength(cardIndex));
    }
  return cardLength;
}

double PixelRoutingServiceXMLHelper::getZGap(int index) const
{
  return getDouble("PixelSvcRoute",index,"zgap",0.);


}
