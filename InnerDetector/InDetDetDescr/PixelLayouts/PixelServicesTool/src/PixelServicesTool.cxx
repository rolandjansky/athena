/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#include "PixelServicesTool/PixelServicesTool.h"

#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"
#include "PixelLayoutUtils/GeoPixelXMLMaterial.h"
#include "PixelServicesTool/PixelRoutingServiceXMLHelper.h"

#include "PixelServicesTool/VolumeStandardBuilder.h"
#include "PixelServicesTool/ServiceStaticBuilder.h"
#include "PixelServicesTool/ServiceDynamicBuilder.h"

#include "PixelInterfaces/IPixelServicesTool.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

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
  declareProperty("ReadSvcFromDB",m_bReadSvcFromDB);
  declareProperty("SvcDynAutomated",m_bSvcDynAutomated=true);
  declareProperty("BarrelModuleMaterial",m_bSetupBarrelModuleMaterial=false);
}

//================ Destructor =================================================

PixelServicesTool::~PixelServicesTool()
{
  if(m_pixServices) delete m_pixServices;
  if(m_dynServices) delete m_dynServices;
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

void PixelServicesTool::resetServices()
{
  if(m_pixServices) {
    delete m_pixServices; 
    m_pixServices = 0;
  }
  if(m_dynServices) {
    delete m_dynServices; 
    m_dynServices = 0;
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
    ATH_MSG_WARNING("InDetServicesTool::buildServices - services already built");
    return;
  }

  ATH_MSG_INFO("InDetServicesTool::buildServices - building services");

  // Link to material manager and msgstream
  m_basics = basics;
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();


  // Access to teh xml file that defines the geometry envelope
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML",m_basics);
  
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

  // Necessary? Under what circumstances would endcapA(C)Present == false?
  if(endcapAPresent||endcapCPresent){
    endcapRmin = genDBHelper.getEndcapRMin();
    endcapRmax = genDBHelper.getEndcapRMax();
    endcapZmin = genDBHelper.getEndcapZMin();
    endcapZmax = genDBHelper.getEndcapZMax();
  }

  ATH_MSG_DEBUG("GEOPIXELSERVICES pixel : "<<pixelRmin<<" "<<pixelRmax<<" //  "<<pixelZmax);
  ATH_MSG_DEBUG("GEOPIXELSERVICES barrel : "<<barrelRmin<<" "<<barrelRmax<<" //  "<<barrelZmin<<" "<<barrelZmax);
  ATH_MSG_DEBUG("GEOPIXELSERVICES endcap : "<<endcapRmin<<" "<<endcapRmax<<" //  "<<endcapZmin<<" "<<endcapZmax);


  // Barrel/endcap volume
  bool cylBarrel = genDBHelper.isBarrelCylindrical();
  bool cylEndcap = genDBHelper.isEndcapCylindrical();

  // Top volume / pixel envelope

  // SvcRegion defined locally
  ATH_MSG_DEBUG("GEOPIXELSERVICES pixel : "<<svcRegions.size()<<"  cyl : "<<cylBarrel<<" "<<cylEndcap<<"  dyn ");

  // Define service envelopes for standard cylindrical barrel/endcap geometries
  if(cylBarrel){
    ATH_MSG_DEBUG("GEOPIXELSERVICES pixel : cylindrical barrel/endcap");
    double delta =0.;
    InDetDD::TubeZone* pixZone = new InDetDD::TubeZone("Pixel",-pixelZmax+delta,pixelZmax-delta,pixelRmin+delta,pixelRmax-delta);
    InDetDD::UnboundedZone topZone("Mother");

    // for (auto it : svcRegions){   (??)
    for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
      pixZone->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax(),(*it)->rotated()));
    }
    topZone.add(pixZone);
	
    m_pixServices = new ServiceStaticBuilder(basics, &topZone);
    m_dynServices = new ServiceDynamicBuilder(m_basics, &topZone,m_bSvcDynAutomated,m_bSetupBarrelModuleMaterial);

    return;
  }
    
  // Define service envelopes for complexe barrel/endcap geometries
  //  -> only one instance of m_pixService and m_dynServices is create,
  //  -> different topzones are successively added to each service builder

  // --------------------------------------------------------------------------------
  ATH_MSG_DEBUG("-> non cylindrical  sections : BARREL");
  
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
      
      // Why radiusNumber - 2? 
      //Because the barrel envelope is 3 radius values for two merged cylinders, does this rteally mean i==0?
      if(i==radiusNumber-2) rmax=pixelRmax;
      
      std::ostringstream name; 
      name << "Barrel " << i;
      
      std::ostringstream os; 
      //	  os << "Barrel" << i;
      os << "P";
      InDetDD::TubeZone* pixZone = new InDetDD::TubeZone(os.str(),zmin, zmax, rmin, rmax);
      InDetDD::UnboundedZone topZone("Mother");
      int nbZone=0;
      // for (auto it : svcRegions){ ??
      for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
	bool bRadial = ((*it)->getRmin()>rmin && (*it)->getRmax()<rmax);
	bool bZpos = ((*it)->getZmin()>zmin && (*it)->getZmax()<zmax);
	if(bRadial&&bZpos){
	  pixZone->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax()));
	  nbZone++;
	}
	else {
	  ATH_MSG_DEBUG("   check if point inside svcRegion - "<<bRadial<<" "<<bZpos);
	}
      }
      
      //	  os << "gbl";
      if(nbZone==0)pixZone->add(new InDetDD::TubeZone(os.str(),zmin+delta, zmax-delta, rmin+delta, rmax-delta));
      ATH_MSG_DEBUG("   nbZone final : "<<name.str()<<" : "<<os.str()<<" "<<nbZone);
      
      topZone.add(pixZone);
      
      // Takes ownership of pixZone
      if(i==0) {
	m_pixServices = new ServiceStaticBuilder(basics,/*&*m_serviceBuilderTool,*/ &topZone); 
	m_ECmatrialFudges = getMaterialFudgesSvcEc();
	m_dynServices = new ServiceDynamicBuilder(basics,/*&*m_serviceBuilderTool,*/ &topZone, m_bSvcDynAutomated, m_bSetupBarrelModuleMaterial, &m_ECmatrialFudges);
      }
      else {
	m_pixServices->addTopZone(&topZone);
	m_dynServices->addTopZone(&topZone);
	
      }
    }
  
  // --------------------------------------------------------------------------------
  ATH_MSG_DEBUG("-> non cylindrical  sections : ENDCAP");
  
  if(endcapAPresent) {    
      radiusList = genDBHelper.getEndcapRadiusList();
      int radiusNumber = (int)radiusList.size();
      std::vector<double> zminList = genDBHelper.getEndcapZMinList();
      for(int iRad=0; iRad<radiusNumber-1; iRad++) {
	  double rmin = radiusList[iRad];
	  double rmax = radiusList[iRad+1];
	  double zmin_loc = zminList[iRad];
	  double halflen_loc = (endcapZmax-zmin_loc)*.5;
	  double middle_loc =  (endcapZmax+zmin_loc)*.5;
	  
	  // Why radiusNumber - 2?
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
	  
	  ATH_MSG_DEBUG("TubeZone A : "<<os.str()<<" / "<<zmin<<" "<<zmax<<" / "<<rmin<<" "<<rmax<<"     Radius : "<<iRad<<"/"<<radiusList.size());
	  
	  int nbZone=0;
	  //for (auto it : svcRegions) ??
	  for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
	    bool bRadial = ((*it)->getRmin()>rmin&&(*it)->getRmax()<rmax);
	    bool bZpos = ((*it)->getZmin()>zmin&&(*it)->getZmax()<zmax);
	    if(bRadial&&bZpos){
	      pixZoneA->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax()));
	      nbZone++;
	    }
	    else {
	      ATH_MSG_DEBUG("   check if point inside svcRegion - "<<bRadial<<" "<<bZpos);
	    }
	  }
	  if(nbZone==0)pixZoneA->add(new InDetDD::TubeZone(os.str(),zmin+delta,zmax-delta,rmin+delta,rmax-delta));
	  ATH_MSG_DEBUG("   nbZone "<<name.str()<<" : "<<os.str()<<" "<<nbZone<<" / "<<svcRegions.size());
	  topZoneA.add(pixZoneA);
	  
	  // Takes ownership of pixZone
	  m_pixServices->addTopZone(&topZoneA);
	  m_dynServices->addTopZone(&topZoneA);
	}
    }

  // --------------------------------------------------------------------------------
  if(endcapCPresent) {    
      radiusList = genDBHelper.getEndcapRadiusList();
      int radiusNumber = (int)radiusList.size();
      std::vector<double> zminList = genDBHelper.getEndcapZMinList();
      for(int iRad=0; iRad<radiusNumber-1; iRad++) {
	  double rmin = radiusList[iRad];
	  double rmax = radiusList[iRad+1];
	  double zmin_loc = zminList[iRad];
	  double halflen_loc = (endcapZmax-zmin_loc)*.5;
	  double middle_loc =  (endcapZmax+zmin_loc)*.5;
	 
	  // Why radiusNumber-2?
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
	  
	  ATH_MSG_DEBUG("TubeZone C : "<<osC.str()<<" / "<<zmin<<" "<<zmax<<" / "<<rmin<<" "<<rmax<<"     Radius : "<<iRad<<"/"<<radiusList.size());
	  
	  int nbZone=0;
	  for(std::vector<InDetDD::TubeZone*>::iterator it=svcRegions.begin(); it!=svcRegions.end(); ++it){
	    bool bRadial = ((*it)->getRmin()>rmin&&(*it)->getRmax()<rmax);
	    bool bZpos = ((*it)->getZmin()>zmin&&(*it)->getZmax()<zmax);
	    if(bRadial&&bZpos){
	      pixZoneC->add(new InDetDD::TubeZone((*it)->label(),(*it)->getZmin(),(*it)->getZmax(),(*it)->getRmin(),(*it)->getRmax(),true));
	      nbZone++;
	    }
	    else {
	      ATH_MSG_DEBUG("   check if point inside svcRegion - "<<bRadial<<" "<<bZpos);
	    }
	  }
	  if(nbZone==0)pixZoneC->add(new InDetDD::TubeZone(osC.str(),-zmax+delta,-zmin-delta,rmin+delta,rmax-delta,true));
	  ATH_MSG_DEBUG("   nbZone "<<nameC.str()<<" : "<<osC.str()<<" "<<nbZone<<" / "<<svcRegions.size());
	  topZoneC.add(pixZoneC);
	  
	  // Takes ownership of pixZone
	  m_pixServices->addTopZone(&topZoneC);
	  m_dynServices->addTopZone(&topZoneC);
	  
	}
    }	

}

// Following two functions are iudentical, except for "parent" declared as GeoPhysVol or GeoFullPhysVol
// Both inherit from GeoVPhysVol... Can we just make one function?

// Build and place the services in a GeoPhysVol object
void PixelServicesTool::buildAndPlace(const std::string & region, GeoPhysVol * parent, double zcenter, std::vector<std::string> svcList,
				      bool bStatic, bool bDynamic) 
{
  ATH_MSG_DEBUG("GEOPIXELSERVICES : tool buildandplace PhysVol "<<region);
  //  m_pixServices->getBuilder(i)->resetServices();

  if(m_pixServices==0){
    ATH_MSG_INFO("InDetServicesTool::BuildAndPlace "<<region<<"  : no static services defined");
  }
  if(m_dynServices==0){
    ATH_MSG_INFO("InDetServicesTool::BuildAndPlace "<<region<<"  : no dynamic services defined");
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

  ATH_MSG_DEBUG("GEOPIXELSERVICES : tool buildandplace FullPhysVol "<<region);
  //  m_pixServices->getBuilder(i)->resetServices();

  if(m_pixServices==0){
    ATH_MSG_ERROR("InDetServicesTool::BuildAndPlace "<<region<<"  : no static services defined");
  }
  if(m_dynServices==0){
    ATH_MSG_ERROR("InDetServicesTool::BuildAndPlace "<<region<<"  : no dynamic services defined");
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


std::string PixelServicesTool::getLayerStaveModuleMaterialName(int iLayer, int iStave, int nbModule) const
{

  // No dynamic services defined -> no barrel module material defined
  if(!m_dynServices) return "None";

  std::map<std::string,std::string> svcMaterialNameTable = m_dynServices->getSvcMaterialNameTable();
  std::ostringstream idName;
  idName<<"Barrel_L"<<iLayer<<"_S"<<iStave<<"_M"<<nbModule;
  
  std::map<std::string, std::string>::iterator it;
  it  = svcMaterialNameTable.find(idName.str());

  if(it!=svcMaterialNameTable.end()) return it->second;
  return "None";
}

std::string PixelServicesTool::getLayerStaveModuleMaterialName(int iLayer, int iStave, std::vector<int> nbModule) const
{

  // No dynamic services defined -> no barrel module material defined
  if(!m_dynServices) return "None";

  std::map<std::string,std::string> svcMaterialNameTable = m_dynServices->getSvcMaterialNameTable();
  std::ostringstream idName;
  idName<<"Barrel_L"<<iLayer<<"_S"<<iStave;
  //  for(int i=0 ; i<(int)nbModule.size(); i++) idName<<"_M"<<nbModule[i];
  for(auto& it : nbModule) idName<<"_M"<<it;
  
  std::map<std::string, std::string>::iterator it;
  it  = svcMaterialNameTable.find(idName.str());

  if(it!=svcMaterialNameTable.end()) return it->second;
  return "None";
}


// Do we still want to keep the following material fudge factors?
double PixelServicesTool::getMaterialFudgeModuleSvc(int iLayer) const
{
  
  PixelRoutingServiceXMLHelper routingXmlHelper("PIXEL_PIXELROUTINGSERVICE_GEO_XML",m_basics);
  return routingXmlHelper.getMaterialFudgeModuleSvc(iLayer);

}

double PixelServicesTool::getMaterialFudgeSvcEc(int iLayer) const
{
  
  PixelRoutingServiceXMLHelper routingXmlHelper("PIXEL_PIXELROUTINGSERVICE_GEO_XML",m_basics);
  return routingXmlHelper.getMaterialFudgeSvcEc(iLayer);

}

ServiceDynamicBuilder::SvcEcMaterialFudges PixelServicesTool::getMaterialFudgesSvcEc() const
{

  ServiceDynamicBuilder::SvcEcMaterialFudges factors;
  factors.L0 = getMaterialFudgeSvcEc(0);
  factors.L00 = getMaterialFudgeSvcEc(4);
  factors.L1 = getMaterialFudgeSvcEc(1);
  factors.L2 = getMaterialFudgeSvcEc(2);
  factors.L3 = getMaterialFudgeSvcEc(3);
  return factors;

}
