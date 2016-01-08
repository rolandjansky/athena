/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelServicesTool/PixelServicesTool.h"

#include "PixelSCTDBHelper/GeoPixelServicesDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelGeneralDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelStaveDBHelper.h"

#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"
#include "PixelLayoutUtils/GeoPixelXMLMaterial.h"

#include "PixelServicesTool/VolumeStandardBuilder.h"
#include "PixelServicesTool/ServiceStaticBuilder.h"
#include "PixelServicesTool/ServiceDynamicBuilder.h"

// #include "InDetGeoModelUtils/GeoSimpleObject.h"
// #include "InDetGeoModelUtils/GeoServiceAssembly.h"

#include "PixelInterfaces/IPixelServicesTool.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 
/*#include "InDetGeoModelStdUtils/GeoEnvelopeStd.h"*/
// #include "InDetGeoModelStdUtils/GeoBarrelStd.h"
// #include "InDetGeoModelStdUtils/GeoLayerStd.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPcon.h"

#include "PathResolver/PathResolver.h"
#include <iostream>

PixelServicesTool::PixelServicesTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_pixServices(0),
    m_dynServices(0),
    m_bReadSvcFromDB(false),
    m_bSetupBarrelModuleMaterial(false),
    m_svcListFromDB("all")
{
  declareInterface<IPixelServicesTool>(this);

  //default settings
  //  declareProperty("ServiceBuilderTool", m_serviceBuilderTool);
  declareProperty("ReadSvcFromDB",m_bReadSvcFromDB);
  declareProperty("SvcDynAutomated",m_bSvcDynAutomated=true);
  declareProperty("BarrelModuleMaterial",m_bSetupBarrelModuleMaterial=false);
}

//================ Destructor =================================================

PixelServicesTool::~PixelServicesTool()
{
  if(m_pixServices){
    if(m_pixServices) delete m_pixServices;
  }
  if(m_dynServices){
    if(m_dynServices) delete m_dynServices;
  }
}


//================ Initialisation =================================================

StatusCode PixelServicesTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    {
      ATH_MSG_INFO("PixelServicesToolr initialize() failed " << name());
      return sc;
    }
  ATH_MSG_INFO("PixelServicesToolr initialize() successful in " << name());

  return StatusCode::SUCCESS;
}


//================ Finalisation =================================================

StatusCode PixelServicesTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// // Register callback function on ConDB object
// // Empty for now
// StatusCode PixelServicesTool::registerCallback( StoreGateSvc*)
// {
//   return StatusCode::SUCCESS;
// }


void PixelServicesTool::resetServices()
{
  if(m_pixServices) {
    delete  m_pixServices; m_pixServices = 0;
  }
  if(m_dynServices) {
    delete  m_dynServices; m_dynServices = 0;
  }

}

// Build the services and service regions
//
//  m_pixService dedicated to static services, i.e services defined in xml files as simple shapes+material 
//  m_dynService dedicated to dynamic servcies built and routed auto-magically through the ID
//
//  the svcRegions corresponds to the envelopes registered while building the geometry (envelope/barrel/layer/endcap/...)
//

void PixelServicesTool::buildServices(const PixelGeoBuilderBasics* basics, std::vector<InDetDD::TubeZone*> svcRegions )
{
  
  // Services already defined
  if(m_pixServices||m_dynServices){
    std::cout<<"InDetServicesTool::buildServices - services already built"<<std::endl;
    return;
  }

  std::cout<<"InDetServicesTool::buildServices -built services"<<std::endl;

  // Add service materials
  std::string fileName="ServiceMaterial.xml";
  if(const char* env_p = std::getenv("PIXEL_PIXELSERVICEMATERIAL_GEO_XML")) fileName = std::string(env_p);
  GeoPixelXMLMaterial matBuilder(basics,fileName);
  matBuilder.Build("indet");

  // Link to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();


  // Access to teh xml file that defines the geometry envelope
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");
  
  //  const GeoShape * envelopeShape;
  double pixelRmin = genDBHelper.getEnvelopeRMin();
  double pixelRmax = genDBHelper.getEnvelopeRMax();
  double pixelZmax = genDBHelper.getCommonHalfLength();

  double barrelRmin = genDBHelper.getBarrelRMin();
  double barrelRmax = genDBHelper.getBarrelRMax();
  double barrelZmin = -genDBHelper.getBarrelHalfLength();
  double barrelZmax = genDBHelper.getBarrelHalfLength();

  bool endcapAPresent  = genDBHelper.isEndcapPresentA();
  bool endcapCPresent  = genDBHelper.isEndcapPresentC();

  double endcapRmin = -1.;
  double endcapRmax = -1.;
  double endcapZmin = -1.;
  double endcapZmax = -1.;

  if(endcapAPresent||endcapCPresent){
    endcapRmin = genDBHelper.getEndcapRMin();
    endcapRmax = genDBHelper.getEndcapRMax();
    endcapZmin = genDBHelper.getEndcapZMin();
    endcapZmax = genDBHelper.getEndcapZMax();
  }

  std::cout<<"GEOPIXELSERVICES pixel : "<<pixelRmin<<" "<<pixelRmax<<" //  "<<pixelZmax<<std::endl;
  std::cout<<"GEOPIXELSERVICES barrel : "<<barrelRmin<<" "<<barrelRmax<<" //  "<<barrelZmin<<" "<<barrelZmax<<std::endl;
  std::cout<<"GEOPIXELSERVICES endcap : "<<endcapRmin<<" "<<endcapRmax<<" //  "<<endcapZmin<<" "<<endcapZmax<<std::endl;


  // Barrel/endcap volume
  bool cylBarrel = genDBHelper.isBarrelCylindrical();
  bool cylEndcap = genDBHelper.isBarrelCylindrical();

  // Top volume / pixel envelope
  InDetDD::UnboundedZone topZone("Mother");

  // SvcRegion defined locally
  std::cout<<"GEOPIXELSERVICES pixel : "<<svcRegions.size()<<"  cyl : "<<cylBarrel<<" "<<cylEndcap<<"  dyn "<<std::endl;
//   if(svcRegions.size()==0)
//     {
//       if(cylBarrel||cylEndcap){
// 	double delta=0.001;
// 	pixZone->add(new InDetDD::TubeZone("P",-pixelZmax+delta,pixelZmax-delta,pixelRmin+delta,pixelRmax-delta));
//       }
//       else {
// 	if(endcapCPresent)pixZone->add(new InDetDD::TubeZone("C",-endcapZmax,-endcapZmin,endcapRmin,endcapRmax, true));
// 	pixZone->add(new InDetDD::TubeZone("B",barrelZmin,barrelZmax,barrelRmin,barrelRmax));
// 	if(endcapAPresent) pixZone->add(new InDetDD::TubeZone("A",endcapZmin,endcapZmax,endcapRmin,endcapRmax));
//       }
//     }

  // Check if services are to be defined automatically - GeoModel automated system (used for initial LoI geometries)
  bool bDynBuilder=false;
  fileName = "";
  if(const char* env_p = std::getenv("PIXEL_PIXELDYNAMICSERVICE_GEO_XML")) fileName = std::string(env_p);
  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  if(file.size()>0 && m_bSvcDynAutomated) bDynBuilder = true;
  
  // Check if services are to be defined automatically - automated system based on XML file description
  fileName = "";
  if(const char* env_p = std::getenv("PIXEL_PIXELROUTINGSERVICE_GEO_XML")) fileName = std::string(env_p);
  file = PathResolver::find_file (fileName, "DATAPATH");
  if(file.size()>0&&!m_bSvcDynAutomated) bDynBuilder = true;

  std::cout<<"GEOPIXELSERVICES pixel : dynamic services "<<bDynBuilder<<std::endl;

  // Define service envelopes for standard cylindrical barrel/endcap geometries
  if(cylBarrel){

    std::cout<<"GEOPIXELSERVICES pixel : cylindrical barrel/endcap"<<std::endl;
    double delta =0.;
    InDetDD::TubeZone* pixZone = new InDetDD::TubeZone("Pixel",-pixelZmax+delta,pixelZmax-delta,pixelRmin+delta,pixelRmax-delta);
    InDetDD::UnboundedZone topZone("Mother");

    for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
      pixZone->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax(),(*it)->rotated()));
    }
    topZone.add(pixZone);
	
    m_pixServices = new ServiceStaticBuilder(basics,/*&*m_serviceBuilderTool,*/ &topZone);
    if(bDynBuilder) m_dynServices = new ServiceDynamicBuilder(basics,/*&*m_serviceBuilderTool,*/ &topZone, m_bSvcDynAutomated,m_bSetupBarrelModuleMaterial);

    return;
  }
    
  // Define service envelopes for complexe barrel/endcap geometries
  //  -> only one instance of m_pixService and m_dynServices is create,
  //  -> different topzones are successively added to each service builder

  // --------------------------------------------------------------------------------
  std::cout<<"-> non cylindrical  sections : BARREL"<<std::endl;
  
  double delta=0.001;
  std::vector<double> radiusList = genDBHelper.getBarrelRadiusList();
  std::vector<double> hLenList = genDBHelper.getBarrelHalfLengthList();
  int radiusNumber = (int)radiusList.size();
  for(int i=0; i<radiusNumber-1; i++)
    {
      double rmin = radiusList[i];
      double rmax = radiusList[i+1];
      double zmin = -hLenList[i];
      double zmax = hLenList[i];
      
      if(i==radiusNumber-2) rmax=pixelRmax;
      
      std::ostringstream name; 
      name << "Barrel " << i;
      
      std::ostringstream os; 
      //	  os << "Barrel" << i;
      os << "P";
      InDetDD::TubeZone* pixZone = new InDetDD::TubeZone(os.str(),zmin, zmax, rmin, rmax);
      InDetDD::UnboundedZone topZone("Mother");
      int nbZone=0;
      for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
	//	    std::cout<<"   check svcRegion inside zone "<<(*it)->label()<<" : "<<(*it)->getZmin()<<" "<<(*it)->getZmax()<<" / "<<(*it)->getRmin()<<" "<<(*it)->getRmax()<<" "<<std::endl;
	bool bRadial = ((*it)->getRmin()>rmin&&(*it)->getRmax()<rmax);
	bool bZpos = ((*it)->getZmin()>zmin&&(*it)->getZmax()<zmax);
	if(bRadial&&bZpos){
	  //	      std::cout<<"   check if point inside svcRegion - OK "<<(*it)->label()<<std::endl;
	  pixZone->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax()));
	  nbZone++;
	}
      }
      
      //	  os << "gbl";
      if(nbZone==0)pixZone->add(new InDetDD::TubeZone(os.str(),zmin+delta, zmax-delta, rmin+delta, rmax-delta));
      std::cout<<"   nbZone final : "<<name.str()<<" : "<<os.str()<<" "<<nbZone<<std::endl;
      
      topZone.add(pixZone);
      
      // Takes ownership of pixZone
      if(i==0) {
	m_pixServices = new ServiceStaticBuilder(basics,/*&*m_serviceBuilderTool,*/ &topZone);
	if(bDynBuilder) m_dynServices = new ServiceDynamicBuilder(basics,/*&*m_serviceBuilderTool,*/ &topZone, m_bSvcDynAutomated, m_bSetupBarrelModuleMaterial);
      }
      else {
	m_pixServices->addTopZone(&topZone);
	if(bDynBuilder) m_dynServices->addTopZone(&topZone);
	
      }
    }
  
  // --------------------------------------------------------------------------------
  std::cout<<"-> non cylindrical  sections : ENDCAP"<<std::endl;
  
  if(endcapAPresent)
    {    
      radiusList = genDBHelper.getEndcapRadiusList();
      int radiusNumber = (int)radiusList.size();
      std::vector<double> zminList = genDBHelper.getEndcapZMinList();
      for(int iRad=0; iRad<radiusNumber-1; iRad++)
	{
	  double rmin = radiusList[iRad];
	  double rmax = radiusList[iRad+1];
	  double zmin_loc = zminList[iRad];
	  double halflen_loc = (endcapZmax-zmin_loc)*.5;
	  double middle_loc =  (endcapZmax+zmin_loc)*.5;
	  
	  if(iRad==radiusNumber-2) rmax=pixelRmax;
	  
	  double zmin = middle_loc-halflen_loc;
	  double zmax = middle_loc+halflen_loc;
	  
	  std::ostringstream name; 
	  name << "EndcapA " << iRad;
	  std::ostringstream os; 
	  //	  os << "EndcapA" << i;
	  os << "P";
	  InDetDD::Zone* pixZoneA = new InDetDD::TubeZone(os.str(),zmin,zmax,rmin,rmax);
	  InDetDD::UnboundedZone topZoneA("Mother");
	  
	  std::cout<<"TubeZone A : "<<os.str()<<" / "<<zmin<<" "<<zmax<<" / "<<rmin<<" "<<rmax<<"     Radius : "<<iRad<<"/"<<radiusList.size()<<std::endl;
	  
	  int nbZone=0;
	  for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
	    //	    std::cout<<"   check svcRegion inside zone "<<(*it)->label()<<" : "<<(*it)->getZmin()<<" "<<(*it)->getZmax()<<" / "<<(*it)->getRmin()<<" "<<(*it)->getRmax()<<" "<<std::endl;
	    bool bRadial = ((*it)->getRmin()>rmin&&(*it)->getRmax()<rmax);
	    bool bZpos = ((*it)->getZmin()>zmin&&(*it)->getZmax()<zmax);
	    if(bRadial&&bZpos){
	      //	      std::cout<<"   check if point inside svcRegion - OK "<<(*it)->label()<<std::endl;
	      pixZoneA->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax()));
	      nbZone++;
	    }
	    else std::cout<<"   check if point inside svcRegion - "<<bRadial<<" "<<bZpos<<std::endl;
	  }
	  if(nbZone==0)pixZoneA->add(new InDetDD::TubeZone(os.str(),zmin+delta,zmax-delta,rmin+delta,rmax-delta));
	  std::cout<<"   nbZone "<<name.str()<<" : "<<os.str()<<" "<<nbZone<<" / "<<svcRegions.size()<<std::endl;
	  topZoneA.add(pixZoneA);
	  
	  // Takes ownership of pixZone
	  m_pixServices->addTopZone(&topZoneA);
	  if(bDynBuilder) m_dynServices->addTopZone(&topZoneA);
	}
    }

  // --------------------------------------------------------------------------------
  if(endcapCPresent)
    {    
      radiusList = genDBHelper.getEndcapRadiusList();
      int radiusNumber = (int)radiusList.size();
      std::vector<double> zminList = genDBHelper.getEndcapZMinList();
      for(int iRad=0; iRad<radiusNumber-1; iRad++)
	{
	  double rmin = radiusList[iRad];
	  double rmax = radiusList[iRad+1];
	  double zmin_loc = zminList[iRad];
	  double halflen_loc = (endcapZmax-zmin_loc)*.5;
	  double middle_loc =  (endcapZmax+zmin_loc)*.5;
	  
	  if(iRad==radiusNumber-2) rmax=pixelRmax;
	  
	  double zmin = -(middle_loc+halflen_loc);
	  double zmax = -(middle_loc-halflen_loc);
	  
	  std::ostringstream nameC; 
	  nameC << "EndcapC " << iRad;
	  std::ostringstream osC; 
	  //	  osC << "EndcapC" << i;
	  osC << "P";
	  InDetDD::Zone* pixZoneC = new InDetDD::TubeZone(osC.str(),zmin,zmax,rmin,rmax);
	  InDetDD::UnboundedZone topZoneC("Mother");
	  
	  std::cout<<"TubeZone C : "<<osC.str()<<" / "<<zmin<<" "<<zmax<<" / "<<rmin<<" "<<rmax<<"     Radius : "<<iRad<<"/"<<radiusList.size()<<std::endl;
	  
	  int nbZone=0;
	  for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
	    //	    std::cout<<"   check svcRegion inside zone "<<(*it)->label()<<" : "<<(*it)->getZmin()<<" "<<(*it)->getZmax()<<" / "<<(*it)->getRmin()<<" "<<(*it)->getRmax()<<" "<<std::endl;
	    bool bRadial = ((*it)->getRmin()>rmin&&(*it)->getRmax()<rmax);
	    bool bZpos = ((*it)->getZmin()>zmin&&(*it)->getZmax()<zmax);
	    if(bRadial&&bZpos){
	      //	      std::cout<<"   check if point inside svcRegion - OK "<<(*it)->label()<<std::endl;
	      pixZoneC->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax(),true));
	      nbZone++;
	    }
	    //	    else std::cout<<"   check if point inside svcRegion - "<<bRadial<<" "<<bZpos<<std::endl;
	  }
	  if(nbZone==0)pixZoneC->add(new InDetDD::TubeZone(osC.str(),-zmax+delta,-zmin-delta,rmin+delta,rmax-delta,true));
	  std::cout<<"   nbZone "<<nameC.str()<<" : "<<osC.str()<<" "<<nbZone<<" / "<<svcRegions.size()<<std::endl;
	  topZoneC.add(pixZoneC);
	  
	  // Takes ownership of pixZone
	  m_pixServices->addTopZone(&topZoneC);
	  if(bDynBuilder) m_dynServices->addTopZone(&topZoneC);
	  
	}
    }	

}

// Build and place the services in a GeoPhysVol object
void PixelServicesTool::buildAndPlace(const std::string & region, GeoPhysVol * parent, double zcenter, std::vector<std::string> svcList,
				      bool bStatic, bool bDynamic) 
{
  std::cout<<"GEOPIXELSERVICES : tool buildandplace PhysVol "<<region<<std::endl;
  //  m_pixServices->getBuilder(i)->resetServices();

  if(m_pixServices==0){
    msg(MSG::INFO)<<"InDetServicesTool::BuildAndPlace "<<region<<"  : no static services defined"<<endreq;
  }
  if(m_dynServices==0){
    msg(MSG::INFO)<<"InDetServicesTool::BuildAndPlace "<<region<<"  : no dynamic services defined"<<endreq;
  }

  if(m_pixServices&&bStatic)
    for(int i=0; i< m_pixServices->getVolumeNumber(); i++)
      m_pixServices->getBuilder(i)->buildAndPlace(region, parent, zcenter, svcList);

  if(m_dynServices&&bDynamic)
    for(int i=0; i< m_dynServices->getVolumeNumber(); i++)
      m_dynServices->getBuilder(i)->buildAndPlace(region, parent, zcenter, svcList);
}

// Build and place the services in a GeoFullPhysVol object
void PixelServicesTool::buildAndPlace(const std::string & region, GeoFullPhysVol * parent, double zcenter, std::vector<std::string> svcList,
				      bool bStatic, bool bDynamic) 
					
{

  std::cout<<"GEOPIXELSERVICES : tool buildandplace FullPhysVol "<<region<<std::endl;
  //  m_pixServices->getBuilder(i)->resetServices();

  if(m_pixServices==0){
    msg(MSG::INFO)<<"InDetServicesTool::BuildAndPlace "<<region<<"  : no static services defined"<<endreq;
  }
  if(m_dynServices==0){
    msg(MSG::INFO)<<"InDetServicesTool::BuildAndPlace "<<region<<"  : no dynamic services defined"<<endreq;
  }

  if(m_pixServices&&bStatic)
    for(int i=0; i< m_pixServices->getVolumeNumber(); i++)
      m_pixServices->getBuilder(i)->buildAndPlace(region, parent, zcenter, svcList);

  if(m_dynServices&&bDynamic)
    for(int i=0; i< m_dynServices->getVolumeNumber(); i++)
      m_dynServices->getBuilder(i)->buildAndPlace(region, parent, zcenter, svcList);
}



//--------------------------------------------------------------------------------
// Compute z/r min/max values for the defined services
//--------------------------------------------------------------------------------

double PixelServicesTool::computeRmin(const std::string & region, std::vector<std::string> svcList) const
{
  double rMin = 999999.;

  if(m_pixServices)
    {
      for(int i=0; i< m_pixServices->getVolumeNumber(); i++){
	double rMin1 =  m_pixServices->getBuilder(i)->computeRmin(region, svcList);
	rMin=std::min(rMin,rMin1);
      }
    }

  if(m_dynServices)
    {
      for(int i=0; i< m_dynServices->getVolumeNumber(); i++){
	double rMin2 = m_dynServices->getBuilder(i)->computeRmin(region, svcList);
	rMin=std::min(rMin,rMin2);
      }
    }

  return rMin;
}

double PixelServicesTool::computeRmax(const std::string & region, std::vector<std::string> svcList) const
{
  double rMax = -999999.;

  if(m_pixServices)
    {
      for(int i=0; i< m_pixServices->getVolumeNumber(); i++) {
	double rMax1 =  m_pixServices->getBuilder(i)->computeRmax(region, svcList);
	rMax=std::max(rMax,rMax1);
      }
  }

  if(m_dynServices)
    {
      for(int i=0; i< m_dynServices->getVolumeNumber(); i++){
	double rMax2 = m_dynServices->getBuilder(i)->computeRmax(region, svcList);
	rMax=std::max(rMax,rMax2);
      }
    }

  return rMax;
}


double PixelServicesTool::computeZmin(const std::string & region, std::vector<std::string> svcList) const
{
  double rMin = 999999.;

  if(m_pixServices)
    {
      for(int i=0; i< m_pixServices->getVolumeNumber(); i++) {
	double rMin1 =  m_pixServices->getBuilder(i)->computeZmin(region, svcList);
	rMin=std::min(rMin,rMin1);
      }
  }

  if(m_dynServices)
    {
      for(int i=0; i< m_dynServices->getVolumeNumber(); i++){
	double rMin2 = m_dynServices->getBuilder(i)->computeZmin(region, svcList);
	rMin=std::min(rMin,rMin2);
      }
    }

  return rMin;
}

double PixelServicesTool::computeZmax(const std::string & region, std::vector<std::string> svcList) const
{
  double rMax = -999999.;

  if(m_pixServices)
    {
      for(int i=0; i< m_pixServices->getVolumeNumber(); i++) {
	double rMax1 =  m_pixServices->getBuilder(i)->computeZmax(region, svcList);
	rMax=std::max(rMax,rMax1);
      }
  }

  if(m_dynServices)
    {
      for(int i=0; i< m_dynServices->getVolumeNumber(); i++){
	double rMax2 = m_dynServices->getBuilder(i)->computeZmax(region, svcList);
	rMax=std::max(rMax,rMax2);
      }
  }

  return rMax;
}


//-----------------------------------------------------------------------------------------
// Get the name of the service material defined for a givem layer/module set 
//   ( barrel layer/module services are not placed auto-magically - only 
//            their corresponding material are defined automatically - see RoutingDyn.cxx) 
//-----------------------------------------------------------------------------------------

std::string PixelServicesTool::getLayerModuleMaterialName(int iLayer, int nbModule) const
{

  // No dynamic services defined -> no barrel module material defined
  if(!m_dynServices) return "None";

  std::map<std::string,std::string> svcMaterialNameTable = m_dynServices->getSvcMaterialNameTable();
  std::ostringstream idName;
  idName<<"Barrel_L"<<iLayer<<"_M"<<nbModule;

  //  std::cout<<">> svcMaterialTable - PixelServicesTool "<<idName.str()<<std::endl;
  //   std::map<std::string,std::string>::iterator it2;
  //   for (it2 = svcMaterialNameTable.begin(); it2 !=svcMaterialNameTable.end(); ++it2)
  //     std::cout<<"- svcMaterialTable : "<<it2->first<<" "<<it2->second<<std::endl;
  
  std::map<std::string, std::string>::iterator it;
  it  = svcMaterialNameTable.find(idName.str());

  if(it!=svcMaterialNameTable.end()) return it->second;
  return "None";
}

std::string PixelServicesTool::getLayerModuleMaterialName(int iLayer, std::vector<int> nbModule) const
{

  // No dynamic services defined -> no barrel module material defined
  if(!m_dynServices) return "None";

  std::map<std::string,std::string> svcMaterialNameTable = m_dynServices->getSvcMaterialNameTable();
  std::ostringstream idName;
  idName<<"Barrel_L"<<iLayer;
  //  for(int i=0 ; i<(int)nbModule.size(); i++) idName<<"_M"<<nbModule[i];
  for(auto& it : nbModule) idName<<"_M"<<it;
  
  std::map<std::string, std::string>::iterator it;
  it  = svcMaterialNameTable.find(idName.str());

  if(it!=svcMaterialNameTable.end()) return it->second;
  return "None";
}


// std::vector<GeoServiceAssembly* > PixelServicesTool::getServiceAssemblies()
// {

// //   std::vector<GeoServiceAssembly* > v;

// //   int nbSvc = m_pixServices->getServiceNumber();

// //   for(int i=0; i<nbSvc; i++)
// //     {
// //       GeoSimpleObject *obj=m_pixServices->getServiceObject(i);

// //       //      std::cout<<"Service material : "<<obj->getMaterialName()<<std::endl;

// //       if(obj->getMaterialName()=="special::ether"||obj->getMaterialName()=="ether"){
// // 	GeoServiceAssembly *svc=new GeoServiceAssembly(obj->physVolume<GeoPhysVol>(), 0, obj->getTransform(), obj->name(), false);
// // 	svc->placeService(*obj, HepGeom::Transform3D(),0);
// // 	v.push_back(svc);
// //       }
// //     }

// //   return v;
  
// }


// std::vector<GeoSimpleObject* > PixelServicesTool::getServiceObjects()
// {

//   std::vector<GeoSimpleObject* > v;

//   int nbSvc = m_pixServices->getServiceNumber();

//   for(int i=0; i<nbSvc; i++)
//     {
//       GeoSimpleObject *obj=m_pixServices->getServiceObject(i);

//       if(obj->getMaterialName()!="special::ether"&&obj->getMaterialName()!="ether")v.push_back(obj);
//     }

//   return v;
  
// }




