/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#include "PixelServicesTool/ServiceDynVolume.h"
#include "PixelServicesTool/VSvcRoute.h"
#include "PixelServicesTool/HSvcRoute.h"
#include "PixelServicesTool/ServicesDynLayer.h"

#include "PixelServicesTool/RoutingDyn.h"
#include "PixelServicesTool/PixelSimpleServiceXMLHelper.h"
#include "PixelServicesTool/PixelDynamicServiceXMLHelper.h"
#include "PixelServicesTool/PixelRoutingServiceXMLHelper.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include <algorithm>
#include <iostream>

RoutingDyn::RoutingDyn(const Athena::MsgStreamMember& msg, const PixelGeoBuilderBasics* basics):
  previousBarrelLayer(0) , m_msgRouting(msg)
{
  m_routePixelBarrelOnPST = false;
  m_pixelAlongBarrelStrip = false;
  c_nInnerPixelLayers = 2;

  c_bpEosLength = 40; // mm
  c_epEosLength = 30;
  c_bsEosLength = 50;
  c_safetyGap = 0.001;

  // all units in cm
  c_EosTolerance = 1.; // mm
  c_halfEosThick = 3;
  c_EosTolerance2 = 0.1;
  c_ServiceDiskThickness = 10;
  c_ServiceCylinderThickness = 6;
  c_LayerLengthTolerance = 1;

  m_ISTexists = false;

  m_simpleSrvXMLHelper  = new PixelSimpleServiceXMLHelper ("PIXEL_PIXELSIMPLESERVICE_GEO_XML",  basics);
  m_genXMLHelper        = new PixelGeneralXMLHelper       ("PIXEL_PIXELGENERAL_GEO_XML",        basics);
  m_svcRoutingXMLHelper = new PixelRoutingServiceXMLHelper("PIXEL_PIXELROUTINGSERVICE_GEO_XML", basics);

  m_routeBarrel = true;
  m_routeEndcap = false;
}

RoutingDyn::~RoutingDyn(){
  delete m_simpleSrvXMLHelper;
  delete m_genXMLHelper;
  delete m_svcRoutingXMLHelper;
}

void RoutingDyn::createRoutingVolumes(ServicesDynTracker& tracker)
{
  msg(MSG::DEBUG)<<"IST : check if defined "<<m_simpleSrvXMLHelper->SupportTubeRMin("IST")<<endreq;
  m_ISTexists = (m_simpleSrvXMLHelper->SupportTubeRMin("IST") > 0.1); 

  createRoutes(tracker);

  checkVolumesOverlap();
  
  msg(MSG::DEBUG)<<"Create routes : final volume number "<<m_volumes.size()<<endreq;
  tracker.setServiceDynVolumes(m_volumes);
  
}

void RoutingDyn::createRoutes(ServicesDynTracker& tracker) 
{ 
  //  barrel pixel vertical route
  m_bplc = tracker.barrelPixelLayers();
  m_eplc = tracker.endcapPixelLayers();

  int nbRoutes = m_svcRoutingXMLHelper->getRouteNumber();

  msg(MSG::DEBUG)<<"Create routes : registered barrel and endcap routes "<<nbRoutes<<" "<<endreq;

  m_horizRoutes.clear();
  m_vertRoutes.clear();
  m_diagRoutes.clear();

  // Create barrel routes
  for(int iRoute=0; iRoute<nbRoutes; iRoute++)
    createRouteFromXML(iRoute);
  
  for(const auto& hsvc: m_horizRoutes) organizePredefinedRouteSegment(hsvc);  
  for(const auto& vsvc: m_vertRoutes) organizePredefinedRouteSegment(vsvc);
}


void RoutingDyn::createRouteFromXML(int iRoute)
{
  // Get radial and horizontal points that define the route
  std::vector<std::string> r = m_svcRoutingXMLHelper->getRouteRadialPositions(iRoute);
  std::vector<std::string> z = m_svcRoutingXMLHelper->getRouteZPositions(iRoute);
  bool isPhiRouting = m_svcRoutingXMLHelper->isPhiRouting(iRoute);

  std::vector<int> layerList = m_svcRoutingXMLHelper->getRouteLayerList(iRoute);
  double svcThick = m_svcRoutingXMLHelper->getRouteThickness(iRoute);
  std::string svcType = m_svcRoutingXMLHelper->getRouteType(iRoute);
  std::vector<double> EOScardLength = m_svcRoutingXMLHelper->EOScardLength(iRoute);

  double zShift = 0.;
  bool bBarrel = true;
  if(m_svcRoutingXMLHelper->isBarrelRoute(iRoute))
    zShift =  m_svcRoutingXMLHelper->getEOSsvcLength(iRoute);
  else {
    zShift =  m_svcRoutingXMLHelper->getZGap(iRoute);
    bBarrel = false;
  }

  int nSectors = m_svcRoutingXMLHelper->getNumSectors(iRoute);
  float refPhi = m_svcRoutingXMLHelper->getPhiRefFirstSector(iRoute);
  float sectorWidth = m_svcRoutingXMLHelper->getSectorVolumeWidth(iRoute);
  bool splitLayers = m_svcRoutingXMLHelper->splitLayersInPhi(iRoute);
  int phiStepInSectors = m_svcRoutingXMLHelper->phiSplitStepInSectors(iRoute);

  int nbSegment = r.size()-1;
  for(int iseg=0 ; iseg<nbSegment; iseg++) {
    msg(MSG::DEBUG)<<"######################################################################################################################"<<endreq;
    msg(MSG::DEBUG)<<"######################################################################################################################"<<endreq;
    msg(MSG::DEBUG)<<"-> barrel/endcap route :  route "<<iRoute<<" segment "<<iseg<<" radii : "<<r[iseg]<<" to "<<r[iseg+1]<<"   Z :"<< z[iseg]<<" to "<<z[iseg+1]<<" "<<endreq;

    bool bFirst=(iseg==0);
    bool bLast=(iseg==nbSegment-1);
    RouteParameter param(iRoute,iseg,bBarrel,r[iseg],r[iseg+1], z[iseg],z[iseg+1], layerList, svcThick, bFirst, bLast, svcType, isPhiRouting, EOScardLength, zShift,
			 nSectors, refPhi, sectorWidth, splitLayers, phiStepInSectors);
    createRouteSegment(param);
  }
}



void RoutingDyn::createRouteSegment(const RouteParameter& param)
{

  std::string r1 = param.getR1();
  std::string r2 = param.getR2();
  std::string z1 = param.getZ1();
  std::string z2 = param.getZ2();
  std::vector<int> layerIndices = param.getLayerIndices();
  //  double svcThick = param.getSvcThickness();

  // horizontal route
  if(r1.compare(r2)==0){
    createHorizontalRoute(param);
    return;
  }
  
  // vertical route
  if(z1.compare(z2)==0){
    createVerticalRoute(param);
    return;
  }

}

// FIXME: Delete?
void RoutingDyn::createEndOfStaveSegment(const RouteParameter& /*param*/)
{
}

void RoutingDyn::organizePredefinedRouteSegment(const HSvcRoute& route)
{
  // Loop over the ServiceDynVolumes to collect the Z positions
  std::vector<double> zPos;
  const SvcRoute::VolumeContainer& svcVol=route.volumes();
  int nbSvcVol=(int)svcVol.size();

  msg(MSG::DEBUG)<<"\n------------------------------------------------------------------------------"<<endreq;
  msg(MSG::DEBUG)<<"Re-organize horizontal route : "<<route.name()<<"  "<<nbSvcVol<<endreq;

  if(nbSvcVol==0) return;

  for(int i=0; i<nbSvcVol; i++){
    zPos.push_back(svcVol[i]->zMin());
    zPos.push_back(svcVol[i]->zMax());
  }

  // Sort z positions, print list before and after sorting
  msg(MSG::DEBUG)<<"Z pos INIT : "<<route.name()<<"  ";
  for(auto z : zPos)  msg(MSG::DEBUG)<< z <<" ";
  msg(MSG::DEBUG)<<endreq;

  std::sort( zPos.begin(), zPos.end() );
  zPos.erase( unique( zPos.begin(), zPos.end() ), zPos.end() );

  msg(MSG::DEBUG)<<"Z pos : "<<route.name() << "  ";
  for(auto z : zPos) msg(MSG::DEBUG) << z << " ";
  msg(MSG::DEBUG)<<endreq;

  int nbZpos = (int)zPos.size();
  double radius = svcVol[0]->radius();
  double svcThick = radius-svcVol[0]->rMin();
  HSvcRoute newRoute(radius,zPos[0],zPos[nbZpos-1],zPos[nbZpos-1],route.routing(),route.name());
  
  for(int j=0; j<nbZpos-1; j++){
    std::ostringstream os;
    os << route.name()<<"_Sec"<<j;

    double z1 = zPos[j];
    double z2 = zPos[j+1];
    msg(MSG::DEBUG)<<"   - build horizontal route : "<<route.name()<<" : from "<<z1<<" to "<<z2<<"  at r="<<radius<<"  -> new route "<<os.str()<<endreq;

    ServiceDynVolume* newCyl = new ServiceDynVolume( ServiceDynVolume::Cylinder,
						     route.routing(),
						     radius-svcThick,
						     radius+svcThick,
						     z1,z2,
						     os.str());    
    double zMid = (z1+z2)*.5;
    for(int i=0; i<nbSvcVol; i++) {
        double zMin =svcVol[i]->zMin();
        double zMax =svcVol[i]->zMax();
        if((zMid-zMin)*(zMid-zMax)<0) {
	  for (const auto & bl:svcVol[i]->layers()){ newCyl->addLayer(bl); }
        }
    }

    if (svcVol[0]->getNumSectors()>1 || svcVol[0]->splitLayersInPhi()) newCyl->splitIntoSectors( svcVol[0]->getNumSectors(), svcVol[0]->getRefPhiSector(),
												 svcVol[0]->getSectorWidth(),svcVol[0]->splitLayersInPhi(),svcVol[0]->getPhiStep());  
   
    newRoute.addVolume(newCyl);	
    addVolume(newCyl);
  }
  
  msg(MSG::DEBUG)<<"*****************************************"<<endreq;
  dumpRoute(newRoute);
  msg(MSG::DEBUG)<<"*****************************************"<<endreq;
}



void RoutingDyn::organizePredefinedRouteSegment(const VSvcRoute& route)
{
 
  // Loop over the ServiceDynVolumes to collect the Z positions
  std::vector<double> rPos;
  const SvcRoute::VolumeContainer& svcVol=route.volumes();
  int nbSvcVol=(int)svcVol.size();

  msg(MSG::DEBUG)<<"\n------------------------------------------------------------------------------"<<endreq;
  msg(MSG::DEBUG)<<"Re-organize vertical route : "<<route.name()<<"  "<<nbSvcVol<<endreq;

  if(nbSvcVol==0) return;

  std::vector<Interval> overlapInterval;

  for(int i=0; i<nbSvcVol; i++){
    double rMin = svcVol[i]->rMin();
    double rMax = svcVol[i]->rMax();
    double zMin = svcVol[i]->zMin();
    double zMax = svcVol[i]->zMax();

    // Check intersectios with horizontal routes
    for (const auto& hroute: m_horizRoutes)
      {
	for(auto& itSvc_hrz : hroute.volumes()){
	  double zmin_hrz = itSvc_hrz->zMin();
	  double zmax_hrz = itSvc_hrz->zMax();
	  double rmin_hrz = itSvc_hrz->rMin();
	  double rmax_hrz = itSvc_hrz->rMax();
	  
	  // check overlap vs Z
	  if(!(zmin_hrz>zMax||zmax_hrz<zMin)) {
	    Interval locInter(rmin_hrz,rmax_hrz);
	    Interval svcInter(rMin,rMax);
	    bool bIntersection=false;
	    // check overlap vs R
	    if(locInter.isInInterval(rMin)||locInter.isInInterval(rMax))bIntersection=true;
	    if(svcInter.isInInterval(rmin_hrz)||svcInter.isInInterval(rmax_hrz))bIntersection=true;
	    
	    if(bIntersection){
	      
	      rPos.push_back(rmin_hrz);
	      rPos.push_back(rmax_hrz);
	      bool bAlreadyExists = false;
	      
	      for(auto& itOverlap : overlapInterval)
		if (itOverlap.isIdenticalTo(locInter)) 
		  bAlreadyExists = true;
	      if(!bAlreadyExists) overlapInterval.push_back(locInter);
	    }
	  }
	}
      }
    
    rPos.push_back(rMin);
    rPos.push_back(rMax);
  }
	  
  std::sort( rPos.begin(), rPos.end() );
  rPos.erase( unique( rPos.begin(), rPos.end() ), rPos.end() );

  int nbRpos = (int)rPos.size();

  msg(MSG::DEBUG)<<"Radial pos : "<<route.name()<<"  ";
  for (auto r : rPos)  msg(MSG::DEBUG) << r << " ";
  msg(MSG::DEBUG)<<endreq;
  
  msg(MSG::DEBUG)<<"Overlap_intervals : "<<route.name()<<"  ";
  for (const auto& overlap: overlapInterval)
    msg(MSG::DEBUG)<<(overlap).getMin()<<" "<<(overlap).getMax()<<" // ";
  msg(MSG::DEBUG)<<endreq;  

    
  double zpos = svcVol[0]->zPos();
  double svcThick = fabs(svcVol[0]->zMax()-svcVol[0]->zMin());
  VSvcRoute newRoute(zpos,rPos[0],rPos[nbRpos-1],rPos[nbRpos-1],route.routing(),route.name());
  
  for(int iInter=0; iInter<nbRpos-1; iInter++) {
    double r1 = rPos[iInter];
    double r2 = rPos[iInter+1];
    
    bool bOverlap = false;
    Interval locInt(r1,r2);
    for (const auto& overlap: overlapInterval){
      if((overlap).containsInterval(locInt)){
	bOverlap=true;
	break;
      }
    }
    
    if(bOverlap)
      msg(MSG::DEBUG)<<"   - vertical route : "<<route.name()<<" : "<<r1<<" to "<<r2<<"  at z="<<zpos<<"  OVERLAP"<<endreq;
    else {
      std::ostringstream os;
      os << route.name()<<"_Sec"<<iInter;
      
      double rMid = (r1+r2)*.5;
      std::vector<int> svcList;
      for(int i=0; i<nbSvcVol; i++){
	double rMin =svcVol[i]->rMin();
	double rMax =svcVol[i]->rMax();
	if((rMid-rMin)*(rMid-rMax)<0) {
	  svcList.push_back(i);
	}
      }
      
      if(svcList.size()>0) {
	msg(MSG::DEBUG)<<"   - vertical route : "<<route.name()<<" : "<<r1<<" to "<<r2<<"  at z="<<zpos<<" -> new route "<<os.str()<<endreq;
	ServiceDynVolume* newDisk = new ServiceDynVolume( ServiceDynVolume::Disk,
							  route.routing(),
							  r1,r2,
							  newRoute.zPos()-0.5*svcThick,
							  newRoute.zPos()+0.5*svcThick,
							  os.str());
	
	
	for(auto& itSvc : svcList) {
	  int iVol=itSvc;
	  for (const auto & bl:svcVol[iVol]->layers()){ newDisk->addLayer(bl); }
	}

	if (svcVol[0]->getNumSectors()>1 || svcVol[0]->splitLayersInPhi()) newDisk->splitIntoSectors( svcVol[0]->getNumSectors(), svcVol[0]->getRefPhiSector(),
												      svcVol[0]->getSectorWidth(),svcVol[0]->splitLayersInPhi(), svcVol[0]->getPhiStep());  
  	      

	newRoute.addVolume(newDisk);	
	addVolume(newDisk);
      }
      else
	msg(MSG::DEBUG)<<"   - vertical route : "<<route.name()<<" : "<<r1<<" to "<<r2<<"  at z="<<zpos<<"  EMPTY"<<endreq; 
    }   
  }
  
  msg(MSG::DEBUG)<<"*****************************************"<<endreq;
  dumpRoute(newRoute);
  msg(MSG::DEBUG)<<"*****************************************"<<endreq;
}


void RoutingDyn::checkVolumesOverlap(){
  for(unsigned int iv = 0; iv < m_volumes.size(); iv++){
    for(unsigned int jv = iv+1; jv < m_volumes.size(); jv++){
      ServiceDynVolume* v1 = m_volumes.at(iv);
      ServiceDynVolume* v2 = m_volumes.at(jv);
      
      if( (v1->zMin()<v2->zMin() && v1->zMax()>v2->zMin()) || (v1->zMin()<v2->zMax() && v1->zMax()>v2->zMax()) ) {
	if( (v1->rMin()<v2->rMin() && v1->rMax()>v2->rMin()) || (v1->rMin()<v2->rMax() && v1->rMax()>v2->rMax()) ) {
	  msg(MSG::ERROR)<<"Overlapping Volumes: Check your service!"<<endreq;
	  msg(MSG::ERROR)<<v1->name()<<" and "<<v2->name()<<" ovelap!"<<endreq;
	  msg(MSG::ERROR)<<v1->name()<<"(rMin, rMax, zMin, zMax): ("<<v1->rMin()<<", "<<v1->rMax()<<", "<<v1->zMin()<<", "<<v1->zMax()<<")"<<endreq;
	  msg(MSG::ERROR)<<v2->name()<<"(rMin, rMax, zMin, zMax): ("<<v2->rMin()<<", "<<v2->rMax()<<", "<<v2->zMin()<<", "<<v2->zMax()<<")"<<endreq;
	}
      }
    } 
  }
}

MinMaxHelper RoutingDyn::getLayerMinMaxBox(bool bBarrel, std::vector<int> layerIndices, std::vector<double> /*EOScardLength*/, double /*EOSlength*/)
{
  double zMin=0.; double zMax=0.;
  double rMin=0.; double rMax=0.;

  // z minmax / all selected layers
  int i=0;
  for(auto& it : layerIndices ){
    int layer = it;
    double locZmin=0., locZmax=0., locRmin=0., locRmax=0.;
    if(bBarrel) {
      locZmin = m_bplc[layer][0]->zPos()-m_bplc[layer][0]->halfLength()-m_bplc[layer][0]->staveZOffset();;
      locZmax = m_bplc[layer][0]->zPos()+m_bplc[layer][0]->halfLength()-m_bplc[layer][0]->staveZOffset();;
      locRmin = m_bplc[layer][0]->radius();
      locRmax = m_bplc[layer][0]->radius();
    }
    else {
      locZmin = m_eplc[layer]->zPos();
      locZmax = m_eplc[layer]->zPos();
      locRmin = m_eplc[layer]->rMin();
      locRmax = m_eplc[layer]->rMax();
    }

    if(i==0) {
       zMin=locZmin; zMax=locZmax; 
       rMin=locRmin; rMax=locRmax; 
       i++;
    }
    else {
      zMin=std::min(zMin,locZmin); 
      zMax=std::max(zMax,locZmax); 
      rMin=std::min(rMin,locRmin); 
      rMax=std::max(rMax,locRmax); 
    }

    msg(MSG::DEBUG)<<"MinMax box for layer "<<layer<<" "<<locZmin<<" "<<locZmax<<" - "<<locRmin<<" "<<locRmax<<"  => "<<zMin<<" "<<zMax<<" - "<<rMin<<" "<<rMax<<endreq;
  }

  msg(MSG::DEBUG)<<"MinMax box globale : "<<zMin<<" "<<zMax<<" - "<<rMin<<" "<<rMax<<endreq;
  return MinMaxHelper(zMin,zMax,rMin,rMax);

}


void RoutingDyn::createVerticalRoute(const RouteParameter& param)
{

  int routeId = param.getRouteId();
  int segId = param.getSegmentId();
  int nSectors = param.getNumSectors();
  bool splitLayers = param.splitLayersInPhi();
  int phiStepInSectors = param.phiStep();
  std::string r1 = param.getR1(); 
  std::string r2 = param.getR2();
  std::string z1 = param.getZ1();
  std::string z2 = param.getZ2();
  std::vector<int> layerIndices = param.getLayerIndices();
  double svcThick = param.getSvcThickness();
  std::vector<double> EOScardLength = param.getEOScardLength();
  double EOSlength = param.getEOSsvcLength();
  bool bBarrel = param.isBarrel();
  std::string type = (param.getType().find("endcap")==0)?"Ec":"Brl";
  ServiceDynVolume::Routing routing = (param.isPhiRouting())?ServiceDynVolume::Phi : ServiceDynVolume::R;
  if (param.getType().find("_")!=std::string::npos)
    type+=param.getType().substr( param.getType().find("_")+1, param.getType().size());

  MinMaxHelper boxZR = getLayerMinMaxBox(bBarrel,layerIndices, EOScardLength, EOSlength);
  double rMin = boxZR.getRMin();
  double rMax = boxZR.getRMax();
  double zMin = boxZR.getZMin();
  double zMax = boxZR.getZMax();

  double rMinLoc = 99999.;
  double rMaxLoc = -1.;
  double zpos0 = 0.;
  bool bSvcGrouped = true;
  std::ostringstream os;
  os << "Svc"<<type<<"_"<<routeId<<segId<<"_RadL";
  int i=0;
  for(auto& it : layerIndices ){
    int layer = it;
    double radius1 = DecodeLayerRadialPosition(r1,layer,rMin,rMax);
    double radius2 = DecodeLayerRadialPosition(r2,layer,rMin,rMax);
    double zpos = DecodeLayerZPosition(z1,layer,EOSlength,zMin,zMax);
    if(i==0)
      { zpos0=zpos; i++; }
    else
      if(fabs(zpos-zpos0)>0.001)bSvcGrouped = false;

    rMinLoc = std::min(rMinLoc,radius1);
    rMinLoc = std::min(rMinLoc,radius2);
    rMaxLoc = std::min(rMaxLoc,radius1);
    rMaxLoc = std::min(rMaxLoc,radius2);
  }

  std::ostringstream suffix; 
  suffix<<layerIndices[0]<<"_"<<layerIndices[layerIndices.size()-1];

  VSvcRoute route(zpos0,rMinLoc,rMaxLoc,rMaxLoc,routing,os.str()+suffix.str());
  
  for(auto& it : layerIndices ){
    int layer = it;
    
    double deltaRadius1 = DecodeLayerRadialShiftPosition(r1, svcThick);
    double radius1 = DecodeLayerRadialPosition(r1,layer,rMin,rMax)+deltaRadius1;
    double deltaRadius2 = DecodeLayerRadialShiftPosition(r2, svcThick);
    double radius2 = DecodeLayerRadialPosition(r2,layer,rMin,rMax)+deltaRadius2;
    double z = DecodeLayerZPosition(z1,layer,EOSlength,zMin,zMax);

    if(!param.isFirstSegment())radius1+=svcThick*.5;
    if(!param.isLastSegment())radius2-=svcThick*.5;
    
    std::ostringstream os2;
    os2 <<os.str()<<"_L"<<layer;

    msg(MSG::DEBUG)<<"   - vertical route : "<<os.str()<<" : "<<radius1<<" to "<<radius2<<"  at z="<<z<<"   // def "<<z1<<" "<<z2<<" "<<r1<<" "<<r2<<"  // minmax "<<zMin<<" "<<zMax<<"  "<<EOSlength<<endreq;
    ServiceDynVolume* svcVol = new ServiceDynVolume( ServiceDynVolume::Disk,
						     routing,
						     radius1,
						     radius2,
						     z-0.5*svcThick,
						     z+0.5*svcThick,
						     os2.str());
    
    if(bBarrel)
      svcVol->addLayers(m_bplc[layer]);
    else
      svcVol->addLayer(m_eplc[layer]);
 
    if (nSectors>1 || splitLayers) svcVol->splitIntoSectors( nSectors, param.getPhiRefFirstSector(),param.getSectorVolumeWidth(),splitLayers, phiStepInSectors);  
    
    if(bSvcGrouped)
      route.addVolume(svcVol);
    else {
      VSvcRoute routeLoc(z,radius1,radius2,radius2,routing,os2.str());
      routeLoc.addVolume(svcVol);
      m_vertRoutes.push_back(routeLoc);      
    }
  }
  
  if(bSvcGrouped) m_vertRoutes.push_back(route);

}

void RoutingDyn::AddRGap(std::string& r, int routeId){
  float m = DecodeLayerMarginPosition(r)+m_svcRoutingXMLHelper->getRGap(routeId);
  std::size_t found=r.find_first_of("+-");
  r=r.substr(0,found);
  r+='+'+std::to_string(m);
}


void RoutingDyn::createHorizontalRoute(const RouteParameter& param)
{
  int routeId = param.getRouteId();
  int segId = param.getSegmentId();
  int nSectors = param.getNumSectors();
  bool splitLayers = param.splitLayersInPhi();
  int phiStepInSectors = param.phiStep();
   std::string r1 = param.getR1(); 
  std::string r2 = param.getR2();
  std::string z1 = param.getZ1();
  std::string z2 = param.getZ2();
  std::vector<int> layerIndices = param.getLayerIndices();
  double svcThick = param.getSvcThickness();
  std::vector<double> EOScardLength = param.getEOScardLength();
  double EOSlength = param.getEOSsvcLength();
  bool bBarrel = param.isBarrel();
  std::string type = (param.getType().find("endcap")==0)?"Ec":"Brl";
  ServiceDynVolume::Routing routing = (param.isPhiRouting())?ServiceDynVolume::Phi : ServiceDynVolume::Z;
  if (param.getType().find("_")!=std::string::npos)
    type+=param.getType().substr( param.getType().find("_")+1, param.getType().size());

  if (not bBarrel) AddRGap(r1,param.getRouteId());
  
  MinMaxHelper boxZR = getLayerMinMaxBox(bBarrel,layerIndices, EOScardLength, EOSlength);
  double rMin = boxZR.getRMin();
  double rMax = boxZR.getRMax();
  double zMin = boxZR.getZMin();
  double zMax = boxZR.getZMax();

  // create routes for each layer

  double zMinLoc = 99999.;
  double zMaxLoc = -1.;
  double radius0 = 0.;
  bool bSvcGrouped = true;
  std::ostringstream os;
  os << "Svc"<<type<<"_"<<routeId<<segId<<"_Hor";
  int i=0;
  for(auto& it : layerIndices ){
    int layer = it;
    double zpos1 = DecodeLayerZPosition(z1,layer,EOSlength,zMin,zMax);
    double zpos2 = DecodeLayerZPosition(z2,layer,EOSlength,zMin,zMax);
    double radius = DecodeLayerRadialPosition(r1,layer,rMin,rMax);
    if(i==0)
      { radius0=radius; i++; }
    else
      if(fabs(radius-radius0)>0.001)bSvcGrouped = false;
    
    zMinLoc = std::min(zMinLoc,zpos1);
    zMinLoc = std::min(zMinLoc,zpos2);
    zMaxLoc = std::min(zMaxLoc,zpos1);
    zMaxLoc = std::min(zMaxLoc,zpos2);
  }
  
  std::ostringstream suffix;
  suffix<<layerIndices[0]<<"_L"<<layerIndices[layerIndices.size()-1];
  
  msg(MSG::DEBUG)<<"Create horizontal route for  "<<layerIndices.size()<<" layers : "<<os.str()<<endreq;
  
  HSvcRoute route(radius0,zMinLoc,zMaxLoc,zMaxLoc,routing,os.str()+suffix.str());
  
  for(auto& it : layerIndices ){
    int layer = it;
    std::ostringstream os2;
    os2 << os.str()<<"_L"<<layer;
    
    double deltaRadius = DecodeLayerRadialShiftPosition(r1, svcThick);
    double radius = DecodeLayerRadialPosition(r1,layer,rMin,rMax)+deltaRadius;
    double zpos1 = DecodeLayerZPosition(z1,layer,EOSlength,zMin,zMax);
    double zpos2 = DecodeLayerZPosition(z2,layer,EOSlength,zMin,zMax);
    
    if(!param.isFirstSegment())zpos1-=svcThick*.5;
    if(!param.isLastSegment())zpos2+=svcThick*.5;

    msg(MSG::DEBUG)<<"   - horizontal route : "<<os.str()<<" : "<<zpos1<<" to "<<zpos2<<"  at r="<<radius<<"      "<<r1<<"  generic route"<<endreq;

    ServiceDynVolume* svcVol = new ServiceDynVolume( ServiceDynVolume::Cylinder, 
						     routing,
						     radius-0.5*svcThick+0.001,
						     radius+0.5*svcThick-0.001,
						     zpos1, zpos2,
						     os2.str());
    if(bBarrel)
      svcVol->addLayers(m_bplc[layer]);
    else
      svcVol->addLayer(m_eplc[layer]);

    if (nSectors>1 || splitLayers) svcVol->splitIntoSectors( nSectors, param.getPhiRefFirstSector(),param.getSectorVolumeWidth(),splitLayers, phiStepInSectors);  

    if(bSvcGrouped)
      route.addVolume(svcVol);
    else {
      HSvcRoute routeLoc(radius,zpos1,zpos2,zpos2,routing,os2.str());
      routeLoc.addVolume(svcVol);
      m_horizRoutes.push_back(routeLoc);
    }
  }
  
  if(bSvcGrouped) m_horizRoutes.push_back(route);
  
  return;
}

void RoutingDyn::dumpRoute( const SvcRoute& route) 
{
  using namespace std; // Not necessary?
  msg(MSG::DEBUG)<< "Dumping route at pos " << route.position() 
		<< " with exit at " << route.exit() << endreq;
  for ( const auto& vol: route.volumes()) vol->dump(true);
}

std::string RoutingDyn::nextVolumeName( const SvcRoute& route) const
{
  std::ostringstream os;
  os << route.volumes().size();
  return route.name() + "Vol" + os.str();
}


// Computes the shift due to the thickness of the services 
double RoutingDyn::DecodeLayerRadialShiftPosition(const std::string & r, double svcThick)
{

  if(r.compare("inner_PST")==0) return -svcThick*.5;
  if(r.compare("outer_PST")==0) return svcThick*.5;
  if(r.compare("inner_IST")==0) return -svcThick*.5;
  if(r.compare("outer_IST")==0) return svcThick*.5;
  if(r.compare("inner_QuarterShell")==0) return -svcThick*.5;
  if(r.compare("outer_QuarterShell")==0) return svcThick*.5;

  std::string pattern = "rMax_Disc_";
  if(r.substr(0,pattern.size()).compare(pattern)==0) return svcThick*.5;

  pattern = "rMin_Disc_";
  if(r.substr(0,pattern.size()).compare(pattern)==0) return -svcThick*.5;

  return 0.;
}


// Check if a margin is defined (+/-)
double RoutingDyn::DecodeLayerMarginPosition(const std::string & r)
{

  std::size_t posPlus = r.find("+");
  std::size_t posMinus = r.find("-");
  if (posPlus==std::string::npos&&posMinus==std::string::npos) return 0.;
  
  int iPos = (posPlus!=std::string::npos)?posPlus:posMinus;  
  std::string strMargin = r.substr(iPos,r.size()-iPos);

  return atof(strMargin.c_str());
}

// Decode the radial position 
double RoutingDyn::DecodeLayerRadialPosition(std::string r, int layer, double rGblMin, double rGblMax)
{
  
  double margin = DecodeLayerMarginPosition(r);

  std::size_t posPlus = r.find("+");
  std::size_t posMinus = r.find("-");

  if (posPlus==std::string::npos||posMinus==std::string::npos){
    int iPos = (posPlus!=std::string::npos)?posPlus:posMinus;  
    r=r.substr(0,iPos);
  }

  if(r.compare("r_Layer")==0) return m_bplc[layer][0]->radius()+margin;
  
  if(r.compare("rMax_AllLayer")==0) return rGblMax;
  if(r.compare("rMin_AllLayer")==0) return rGblMin;

  std::string pattern = "rMax_Disc_";
  if(r.substr(0,pattern.size()).compare(pattern)==0){
    int discNumber = atoi((r.substr(pattern.size(),r.size()-pattern.size())).c_str());
    return m_eplc[discNumber]->rMax()+margin;
  }

  pattern = "rMin_Disc_";
  if(r.substr(0,pattern.size()).compare(pattern)==0){
    int discNumber = atoi((r.substr(pattern.size(),r.size()-pattern.size())).c_str());
    return m_eplc[discNumber]->rMin()+margin;
  }

  pattern = "r_Layer_";
  if(r.substr(0,pattern.size()).compare(pattern)==0){
    int layerNumber = atoi((r.substr(pattern.size(),r.size()-pattern.size())).c_str());
    return m_bplc[layerNumber][0]->radius()+margin;
  }
  
  if(r.compare("inner_PST")==0) return m_simpleSrvXMLHelper->SupportTubeRMin("PST")+margin;
  if(r.compare("outer_PST")==0) return m_simpleSrvXMLHelper->SupportTubeRMax("PST")+margin;
  if(r.compare("inner_IST")==0) return m_simpleSrvXMLHelper->SupportTubeRMin("IST")+margin;
  if(r.compare("outer_IST")==0) return m_simpleSrvXMLHelper->SupportTubeRMax("IST")+margin;
  if(r.compare("inner_QuarterShell")==0) return m_simpleSrvXMLHelper->SupportTubeRMin("QuarterShell")+margin;
  if(r.compare("outer_QuarterShell")==0) return m_simpleSrvXMLHelper->SupportTubeRMax("QuarterShell")+margin;

  return atof(r.c_str());
  
}

// Decode the Z position 
double RoutingDyn::DecodeLayerZPosition(std::string z, int layer, double zShift, double zGblMin, double zGblMax)
{

  double margin = DecodeLayerMarginPosition(z);

  std::size_t posPlus = z.find("+");
  std::size_t posMinus = z.find("-");
  if (posPlus==std::string::npos||posMinus==std::string::npos){
    int iPos = (posPlus!=std::string::npos)?posPlus:posMinus;  
    z=z.substr(0,iPos);
  }

  if(z.compare("zMax_Layer")==0) return m_bplc[layer][0]->zPos()+m_bplc[layer][0]->halfLength()+m_bplc[layer][0]->staveZOffset()+margin;

  if(z.compare("zMax_AllLayer")==0) return zGblMax+zShift+margin;
  if(z.compare("zMin_AllLayer")==0) return zGblMin+zShift+margin;

  std::string pattern = "zMax_Layer_";
  if(z.substr(0,pattern.size()).compare(pattern)==0){
    int layerNumber = atoi((z.substr(pattern.size(),z.size()-pattern.size())).c_str());
    return m_bplc[layerNumber][0]->zMax()+zShift+margin;
  }

  pattern = "zMin_Layer_";
  if(z.substr(0,pattern.size()).compare(pattern)==0){
    int layerNumber = atoi((z.substr(pattern.size(),z.size()-pattern.size())).c_str());
    return m_bplc[layerNumber][0]->zMin()+margin;
  }

  if(z.compare("z_Disc")==0) return m_eplc[layer]->zPos()+margin;

  pattern = "z_Disc_";
  if(z.substr(0,pattern.size()).compare(pattern)==0){
    int discNumber = atoi((z.substr(pattern.size(),z.size()-pattern.size())).c_str());
    return m_eplc[discNumber]->zPos()+zShift+margin;
  }

  if(z.compare("zMin_PST")==0) return m_simpleSrvXMLHelper->SupportTubeZMin("PST")+margin;
  if(z.compare("zMax_PST")==0) return m_simpleSrvXMLHelper->SupportTubeZMax("PST")+margin;
  if(z.compare("zMin_IST")==0) return m_simpleSrvXMLHelper->SupportTubeZMin("IST")+margin;
  if(z.compare("zMax_IST")==0) return m_simpleSrvXMLHelper->SupportTubeZMax("IST")+margin;
  if(z.compare("zMin_QuarterShell")==0) return m_simpleSrvXMLHelper->SupportTubeZMin("QuarterShell")+margin;
  if(z.compare("zMax_QuarterShell")==0) return m_simpleSrvXMLHelper->SupportTubeZMax("QuarterShell")+margin;

  return atof(z.c_str());
}



void RoutingDyn::addRouteMaterial(const PixelGeoBuilderBasics* basics)
{
  msg(MSG::DEBUG) << "----------------------------------------------------------------------"<<endreq;
  msg(MSG::DEBUG) << "RoutingDyn::addRouteMaterial called for " << m_volumes.size() << " volumes" << endreq;

  m_layerMaterial.clear();
  m_svcMatNames.clear();

  for(const auto& vol: m_volumes){    
    std::string ctype = vol->name();
    
    ctype=ctype.substr(3,ctype.find("_")-3);
    if (ctype.compare("Ec")==0)
      ctype="endcap";
    else if (ctype.compare("Brl")==0)
      ctype="barrel";    
    else if (ctype.find("Ec")==0)
      ctype="endcap_"+ctype.substr(2, ctype.size());
    else if (ctype.find("Brl")==0)
      ctype="barrel_"+ctype.substr(3, ctype.size());
    else{
      msg(MSG::ERROR) << "service type " <<ctype<<" is not allowed (it must contain either 'Ec' or 'Brl')" <<endreq;
      continue;
    }
      
    msg(MSG::DEBUG) << "*** Service material for volume : "<<vol->name()<<"  add material "<<endreq;
    
    std::vector<ServiceDynMaterial> result;
    
    // Loop over volumes to compute total material
    for (const auto& player: vol->layers()){
      const ServicesDynLayer& layer (*player);
      std::map<const ServicesDynLayer*, ServiceDynMaterial>::iterator iMat = m_layerMaterial.find(player);
      
      // Scale material budget for the barrel layer : half weight per layer side
      bool scalePerHalf = (layer.part()==0);
      // compute and store material
      ServiceDynMaterial layerMat = computeRouteMaterial( basics, layer.type(), layer.part(), layer.number(),layer.numStaveTmp(),
							  layer.modulesPerStave(), layer.chipsPerModule(), scalePerHalf,
							  false, ctype, vol->length(), vol->volume());
      
      m_layerMaterial[player] = layerMat;
      
      result.push_back( layerMat);
    }
    
    vol->setMaterials( result);
  }
}


ServiceDynMaterial RoutingDyn::computeRouteMaterial(const PixelGeoBuilderBasics* basics, 
						    DetTypeDyn::Type layerType, DetTypeDyn::Part layerPart, int layerNumber, int staveNumber,
						    std::vector<int> modulePerStave, std::vector<int> chipPerModule,
						    bool scalePerHalf, bool onStave, std::string ctype, double length, double volume)

{
  msg(MSG::DEBUG)<<endreq;
  msg(MSG::DEBUG)<<"** ComputeRouteMaterial for layer "<<layerNumber<<" type "<<layerType<< " staveTmp "<< staveNumber;
  msg(MSG::DEBUG)<<"  part : "<<((layerType==0)?"brl":"ec")<<"  #module/chip : ";
  for(auto& it : modulePerStave) msg(MSG::DEBUG)<<it<<"  "; 
  msg(MSG::DEBUG)<<"// ";
  for(auto& it : chipPerModule) msg(MSG::DEBUG)<<it<<"  "; 
  msg(MSG::DEBUG)<<endreq;
  
  std::string name = constructName(layerType, layerPart, layerNumber);
  ServiceDynMaterial result( name);
  
  std::string routeType=(layerPart==0)?"barrel":"endcap";
  if(ctype.empty()) ctype=routeType; 
  int nbModuleType = (int)modulePerStave.size();

  // Get the service list corresponding to staves  (taken into account only once per module)
  std::vector<std::string> staveMaterialNames;
  if(layerPart==0) staveMaterialNames = m_svcRoutingXMLHelper->getTypeMaterialNames(layerNumber,"stave");
  msg(MSG::DEBUG)<<"Stave material names : ";
  for (const auto& staveMaterialName: staveMaterialNames) msg(MSG::DEBUG)<<staveMaterialName<<" "; 
  msg(MSG::DEBUG)<<endreq;
  std::vector<std::string> staveMaterialCmpt;
  
  // Loop over the different types of module (barrel/transition/inclined) or (endcap) only
  for(int iModType=0; iModType<nbModuleType; iModType++){
    if(modulePerStave[iModType]>0) {
      int svcSetIndex = m_svcRoutingXMLHelper->getServiceSetIndex(ctype, layerNumber, iModType);      
      
      msg(MSG::DEBUG)<<"*** MODULE TYPE : "<<iModType<<"   for layer "<<layerNumber<<" "<<routeType<<"   ";
      msg(MSG::DEBUG)<<"***             : svcSetIndex : "<<svcSetIndex<<endreq;
      
      std::vector<std::string> svcList_string = m_svcRoutingXMLHelper->getServiceSetContent(svcSetIndex);
      std::string svcSetName = m_svcRoutingXMLHelper->getServiceSetName(svcSetIndex);
      
      // List of the services defined for the module type
      for(const auto& svc_string : svcList_string){ 
	// Decode string vs ' ' pattern
	std::vector<std::string>res;
	std::istringstream s(svc_string);
	std::string tmp;
	while (s >> tmp) res.push_back(tmp);
	
	// Build material name & decode the cable number / multiplicity
	std::ostringstream os;
	os << "pix::"<< svcSetName<<"_"<<res[1];
	
	int numCable = decodeStaveCableNumber(res[2], 1, modulePerStave[iModType], chipPerModule[iModType]);	
	if (!onStave && layerNumber<(int)m_bplc.size() && routeType!="endcap") {
	  numCable *= m_bplc[layerNumber][0]->nStaves();
	}
      
	// Material already defined ?
	bool bMaterialAlreadyDefined=false;
	if(std::find(m_svcMatNames.begin(), m_svcMatNames.end(), os.str())!=m_svcMatNames.end()) // material already defined
	  bMaterialAlreadyDefined=true;
	  
	m_svcMatNames.push_back(os.str());
	
	// Contribution
	std::string contrib = res[2];
	
	//Loop over components
	std::vector<std::string>compName;
	std::vector<double>compWeight;
	std::vector<double>compWeightPercent;
	std::vector<double>compDensity;
	double totWeight=0;
	
	int weightType = -1;
	
	// cable ( weight of each component )
	  if(res[0]=="svc"){
	    if(res[3].compare("fix")==0)
	      weightType=0;
	    else if (res[3].compare("lin")==0)
	      weightType=1;
	    else if (res[3].compare("vol")==0)
	      weightType=2;
	    else{
	      weightType=1;
	      msg(MSG::ERROR)<<"Undefined type of service weight (available types are fix,lin,vol)"<<endreq;
	      msg(MSG::ERROR)<<svc_string<<endreq;
	    }
	 
	    for(int i=4; i<(int)res.size()-1; i+=2) {
	      std::string matName = res[i];
	      compName.push_back(matName);
	      double tmp=atof(res[i+1].c_str());

	      if (weightType==2) tmp*=volume;
	      if (weightType!=1) tmp/=length;
	      
	      const GeoMaterial* mat = basics->matMgr()->getMaterial(matName);                               
	      double density = mat->getDensity()/(CLHEP::g/CLHEP::cm3);

	      compWeight.push_back(tmp);                 // in g/mm
	      totWeight += tmp;
	      compDensity.push_back(density);
	    }
	  }
	  // cooling  ( ID & OD)
	  else {
	    double coolingDiam = atof(res[4].c_str())*.1;  // in cm
	    double pipeDiam = atof(res[5].c_str())*.1;     // in cm
	    double coolingArea = CLHEP::pi*coolingDiam*coolingDiam*.25;
	    double pipeArea = CLHEP::pi*pipeDiam*pipeDiam*.25 - coolingArea;
	    
	    std::string coolingMat = res[6];
	    std::string pipeMat = res[7];
	    
	    compName.push_back(coolingMat);
	    const GeoMaterial* matCooling = basics->matMgr()->getMaterial(coolingMat);
	    double density = matCooling->getDensity()/(CLHEP::g/CLHEP::cm3);
	    compWeight.push_back(coolingArea*density*0.1);     // in g/mm
	    totWeight += coolingArea*density*0.1;              // in g/mm
	    compDensity.push_back(density);
	    
	    compName.push_back(pipeMat);
	    const GeoMaterial* matPipe = basics->matMgr()->getMaterial(pipeMat);
	    density = matPipe->getDensity()/(CLHEP::g/CLHEP::cm3);
	    compWeight.push_back(pipeArea*density*0.1);        // in g/mm
	    totWeight += pipeArea*density*0.1;                 // in g/mm
	    compDensity.push_back(density);	    
	  }

	  // Register material through material manager
	  if(!bMaterialAlreadyDefined) {
	    
	    double densityTmp=0.;
	    for(auto& itWg : compWeight) 
	      {
                if (itWg>0.) {  // ST protect against division by 0
		  double tmp = itWg/totWeight;
		  compWeightPercent.push_back(tmp);
		  densityTmp+=tmp/itWg;   
		}
	      }
	    
	    double density = 1./densityTmp;
	    
	    // Build the material corresponding to the service 
	    GeoMaterial* newMat = new GeoMaterial(os.str(),density*(CLHEP::g/CLHEP::cm3));
	    int compNumber = (int)compName.size();
	    for(int ii=0; ii<compNumber; ii++) {
	      if(compName[ii].substr(0,5)=="std::"||compName[ii].substr(0,5)=="sct::"||compName[ii].substr(0,5)=="pix::"||compName[ii].substr(0,7)=="indet::") {
		std::string tmp = compName[ii];
		GeoMaterial *matComp = const_cast<GeoMaterial*>(basics->matMgr()->getMaterial(tmp));
		if (compWeightPercent[ii]>0) newMat->add(matComp,compWeightPercent[ii]);
		else  msg(MSG::DEBUG)<<"   - material "<<matComp->getName()<<" skipped, weight fraction "<<compWeightPercent[ii]<<endreq;
	      }
	      else
		{
		  GeoElement *matComp = const_cast<GeoElement*>(basics->matMgr()->getElement(compName[ii]));
		  newMat->add(matComp,compWeightPercent[ii]);
		}
	    }		
	    
	    basics->matMgr()->addMaterial(newMat);
	    msg(MSG::DEBUG)<<"    >>>> register new material : "<<os.str()<<" :rho[g/cm3]:  "<<newMat->getDensity()/(CLHEP::g/CLHEP::cm3)
			   <<":X0[mm]:"<< newMat->getRadLength()/CLHEP::mm <<endreq;
	  }
	  
	  // check if material is a stave material -> stave material are taken into account only once
	  bool bAddMaterialToBudget = true;
	  for(const auto& staveMaterialName : staveMaterialNames) {
	    if(res[1].find(staveMaterialName)!=std::string::npos) 
	      {
		bool bStaveMaterialCmpt=(std::find(staveMaterialCmpt.begin(), staveMaterialCmpt.end(), (staveMaterialName))!=staveMaterialCmpt.end());	      
		if(!bStaveMaterialCmpt)
		  staveMaterialCmpt.push_back(staveMaterialName);
		else
		  bAddMaterialToBudget = false;
	      }
	  }
	      
	  // Add material to global budget
	  if(bAddMaterialToBudget) {	    
	    // Add the material in the SvcDynMaterial object 
	    if(scalePerHalf)totWeight*=.5;
	    msg(MSG::DEBUG)<<"          -> material : "<<os.str()<<"      cable number "<<numCable<<"  "<<res[2]<<"  mat lin weight : "<<totWeight<<endreq;

	    result.addEntry( os.str(), numCable , true, totWeight);
	  }
	  else
	    msg(MSG::DEBUG)<<"          -> material : "<<os.str()<<"  already taken into account"<<endreq;
	  
      }
    }
  }
  return result;
}

std::string RoutingDyn::constructName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const
{
  std::string name = DetTypeDyn::name(type, part) + "Services_L";
  std::ostringstream os;

  os << layer;
  return name + os.str();
}


int RoutingDyn::decodeStaveCableNumber(std::string pattern, int nbStave, int modulePerStave, int chipPerModule)
{
  // fixed number 
  std::string prefix=pattern.substr(0,1);
  if(prefix!="/"&&prefix!="*") return atoi(pattern.c_str());
  
  // per stave/ module or chip ?
  std::size_t iPos=pattern.find("_");
  if(iPos==std::string::npos){
    msg(MSG::DEBUG)<<"ERROR : unrecognisable pattern : "<<pattern<<endreq;
    return 0;
  }

  std::string numElt_str = pattern.substr(1,iPos-1);
  int numElt=atoi(numElt_str.c_str());
  std::string nameElt = pattern.substr(iPos+1,pattern.size()-iPos-1);
  
  // defined for 1 object exactly
  if(numElt==1){
    if(nameElt.compare("stave")==0) return nbStave;
    if(nameElt.compare("ring")==0) return nbStave;
    if(nameElt.compare("module")==0) return modulePerStave;
    if(nameElt.compare("chip")==0) return (modulePerStave*chipPerModule);
  }

  // defined per numElt
  if(prefix=="/"){
    if(nameElt.compare("stave")==0) return ceil((double)nbStave/(double)numElt);
    if(nameElt.compare("ring")==0) return ceil((double)nbStave/(double)numElt);
    if(nameElt.compare("module")==0) return ceil((double)modulePerStave/(double)numElt);
    if(nameElt.compare("chip")==0) return ceil((double)(modulePerStave*chipPerModule)/(double)numElt);
  }
  if(nameElt.compare("stave")==0) return numElt*nbStave;
  if(nameElt.compare("ring")==0) return numElt*nbStave;
  if(nameElt.compare("module")==0) return modulePerStave*numElt;
  if(nameElt.compare("chip")==0) return (modulePerStave*chipPerModule)*numElt;

  return 0;

}

// Compute the material budget on the top of each module of a barrel stave
void RoutingDyn::computeBarrelModuleMaterial(const PixelGeoBuilderBasics* basics)
{
  msg(MSG::DEBUG) << "----------------------------------------------------------------------"<<endreq;
  msg(MSG::DEBUG) << "RoutingDyn::ComputeBarrelModuleMaterial "<<m_bplc.size()<<endreq;

  // Loop over the barrel layers to create the service material corresponding to each module

  std::vector<std::string> brlModuleMaterialNames;

  int nbBarrelLayers = (int)m_bplc.size();
  for(int iLayer=0; iLayer<nbBarrelLayers; iLayer++) {
    // Get the service list corresponding to staves  (taken into account only once per module)
    std::vector<std::string> staveMaterialNames = m_svcRoutingXMLHelper->getTypeMaterialNames(iLayer,"stave");
    msg(MSG::DEBUG)<<"Stave material names : ";
    for (const auto& staveMaterialName : staveMaterialNames) msg(MSG::DEBUG)<<staveMaterialName << " "; 
    msg(MSG::DEBUG)<<endreq;
    
    for(int iStaveTmp=0; iStaveTmp<int(m_bplc[iLayer].size()); iStaveTmp++) {
      // Total module number
      int nbModule = m_bplc[iLayer][iStaveTmp]->modulesPerStaveTot()/2;
      if(m_bplc[iLayer][iStaveTmp]->modulesPerStaveTot()%2==1) nbModule++;
      
      // Nb module per type of module
      int nbModuleType = m_bplc[iLayer][iStaveTmp]->moduleTypeNumber();
      std::vector<int> nbModulePerType_init=m_bplc[iLayer][iStaveTmp]->modulesPerStave();
	
      std::map<std::string,std::vector<int> >configurationType;
      std::vector<int> nbModulePerType_tmp;
      
      for(int i=0 ; i<nbModuleType; i++) 
	nbModulePerType_tmp.push_back(nbModulePerType_init[i]/2+nbModulePerType_init[i]%2);
      
      configurationType.insert(std::pair<std::string,std::vector<int> >("even",nbModulePerType_tmp));
      if(nbModulePerType_init[0]%2==1) {
	nbModulePerType_tmp.clear();
	nbModulePerType_tmp.push_back(nbModulePerType_init[0]/2);

	for(int i=1 ; i<nbModuleType; i++) 
	  nbModulePerType_tmp.push_back(nbModulePerType_init[i]/2+nbModulePerType_init[i]%2);

	configurationType.insert(std::pair<std::string,std::vector<int> >("odd",nbModulePerType_tmp));
      }
	

      // Loop over configuration types
      for (const auto& config: configurationType) {	    
	const std::vector<int>* nbModulePerType = &config.second;
	// Loop over the module : starting from the center of a stave
	for(int iModule=1; iModule<nbModule+1; iModule++) {
		
	  // Distribute the module number over the different module types
	  std::vector<int> nbModuleLayer;
	  for(int i=0; i<nbModuleType; i++)nbModuleLayer.push_back(0);
	  bool bEndOfLoop=false;
	  int iCmpt_prev=0;
	  for(int iType=0 ; iType<nbModuleType&&!bEndOfLoop; iType++) {
	    int iCmpt_next = iCmpt_prev+nbModulePerType->at(iType);
	    if(iModule>=iCmpt_next) nbModuleLayer[iType]=nbModulePerType->at(iType);
	    else { nbModuleLayer[iType]=iModule-iCmpt_prev; bEndOfLoop=true; }
	    iCmpt_prev = iCmpt_next;
	  }
	  
	  msg(MSG::DEBUG)<<endreq;
	  msg(MSG::DEBUG)<<"MODULE per layer : "<<iModule<<" : "; for(int i=0; i<(int)nbModuleLayer.size(); i++) msg(MSG::DEBUG)<<nbModuleLayer[i]<<"  ";
	  
	  //empty string used to be materialId, which is not longer defined in Xml files 
	  std::string matName0 = constructBarrelLayerName("", nbModuleLayer, iLayer);     // "old" style / 1 per layer
	  std::string matName = constructBarrelLayerName("", nbModuleLayer, iLayer, iStaveTmp);
	  bool bAlreadyDefined = (std::find(brlModuleMaterialNames.begin(), brlModuleMaterialNames.end(), matName)!=brlModuleMaterialNames.end());	 
	  
	  // Register material name / per stave
	  std::ostringstream idName;
	  idName<<"Barrel_L"<<iLayer<<"_S"<<iStaveTmp;
	  for(int iType=0 ; iType<nbModuleType; iType++) idName<<"_M"<<nbModuleLayer[iType];
	  m_svcMaterialNameTable.insert(std::pair<std::string,std::string>(idName.str(),matName));
	  
	  // Material already defined
	  if(bAlreadyDefined) {
	    msg(MSG::DEBUG) <<"Barrel module material "<<matName<<" already defined"<<endreq;
	  }
	  else {
	    // Compute material corresponding to iModule modules
	    ServiceDynMaterial layerMat = computeRouteMaterial( basics, m_bplc[iLayer][iStaveTmp]->type(), m_bplc[iLayer][iStaveTmp]->part(),
								m_bplc[iLayer][iStaveTmp]->number(), m_bplc[iLayer][iStaveTmp]->numStaveTmp(),
								nbModuleLayer, m_bplc[iLayer][iStaveTmp]->chipsPerModule(), false, true);
		
	    std::vector<std::string> staveMaterialCmpt;
	    staveMaterialCmpt.clear();
	    std::vector<std::string> linearComponents;
	    std::vector<double>      linWeights;
	    double linWeightsTot = 0.;
	    for ( ServiceDynMaterial::EntryIter ient= layerMat.components().begin(); ient!=layerMat.components().end(); ient++) {
	      
	      msg(MSG::DEBUG)<<"Inside components loop, comp = "<<ient->name<<" number "<<ient->number<<" weight "<<ient->weight<<" linear "<<ient->linear<<endreq;
	      
	      std::string prename = ient->name;	      
	      // check if material is a stave material -> stave material are taken into account only once
	      bool bAddMaterialToBudget = true;
	      for (const auto& staveMaterialName : staveMaterialNames) {
		if(ient->name.find(staveMaterialName)!=std::string::npos) {
		  bool bStaveMaterialCmpt=(std::find(staveMaterialCmpt.begin(), staveMaterialCmpt.end(), staveMaterialName)!=staveMaterialCmpt.end());	      
		  if(!bStaveMaterialCmpt)
		    staveMaterialCmpt.push_back(staveMaterialName);
		  else
		    bAddMaterialToBudget = false;
		}
	      }
	      
	      if(bAddMaterialToBudget){
		if (ient->linear) {
		  std::vector<std::string>::iterator it=std::find(linearComponents.begin(), linearComponents.end(), prename);
		  if(it!=linearComponents.end()){
		    int index = std::distance(linearComponents.begin(),it);
		    linWeights[index] += fabs( ient->number * ient->weight);
		  }
		  else {
		    linearComponents.push_back( prename);
		    linWeights.push_back( fabs( ient->number * ient->weight));
		  }
		}
		else {
		  linearComponents.push_back( prename);      // the distiction between linear and not is done in the
		  linWeights.push_back( fabs( ient->weight*ient->number));  // InDetMaterialmanager, based on the weight table flag
		}
		
		linWeightsTot += fabs( ient->number * ient->weight);
	      }
	      else{
		msg(MSG::DEBUG) << "IGNORE : Inside components loop, comp = " << ient->name <<endreq;
	      }
	    }
	    
	    msg(MSG::DEBUG)<<"Barrel module material "<<matName<<" : "<<linWeightsTot<<endreq;
		  
	    // Register material used as base to build weighted material
	    std::string matName_base = matName+"_Base";
	    GeoMaterial* newMat = new GeoMaterial(matName_base,1.*(CLHEP::g/CLHEP::cm3));
	    double invLinWeightsTot = 1./linWeightsTot;
	    int nbComp = (int)linWeights.size();
	    for(int i=0; i<nbComp; i++){
	      GeoMaterial *matComp = const_cast<GeoMaterial*>(basics->matMgr()->getMaterial(linearComponents[i]));
	      newMat->add(matComp, linWeights[i]*invLinWeightsTot);
	    }
	    basics->matMgr()->addMaterial(newMat);
	    
	    // Register weighted material
	    basics->matMgr()->addWeightMaterial(matName, matName_base, linWeightsTot , 1);
	    msg(MSG::DEBUG)<<"Barrel module material "<<matName<<" / "<<matName_base<<" registered"<<endreq;  
	    
	    msg(MSG::DEBUG)<< "  => final material    " << newMat->getName()
			   << "   density : " << newMat->getDensity()/(CLHEP::g/CLHEP::cm3) << " g/cm3"
			   << "     X0 : "    << newMat->getRadLength()/CLHEP::mm << "mm" << endreq;
	    
	    // Save material name
	    brlModuleMaterialNames.push_back(matName);
	    
	    // save 1st stave material as "layer" material
	    if (iStaveTmp==0) {
	      // Register material name / per stave
	      std::ostringstream idNameLay;
	      idNameLay<<"Barrel_L"<<iLayer;
	      for(int iType=0 ; iType<nbModuleType; iType++) idNameLay<<"_M"<<nbModuleLayer[iType];
	      m_svcMaterialNameTable.insert(std::pair<std::string,std::string>(idNameLay.str(),matName0));
	      
	      std::string matName0_base = matName0+"_Base";
	      GeoMaterial* newMat0 = new GeoMaterial(matName0_base,1.*(CLHEP::g/CLHEP::cm3));
	      for(int i=0; i<nbComp; i++) {
		GeoMaterial *matComp = const_cast<GeoMaterial*>(basics->matMgr()->getMaterial(linearComponents[i]));
		newMat0->add(matComp, linWeights[i]*invLinWeightsTot);
	      }
		    
	      basics->matMgr()->addMaterial(newMat0);
		    
	      // Register weighted material
	      basics->matMgr()->addWeightMaterial(matName0, matName0_base, linWeightsTot , 1);
	      msg(MSG::DEBUG)<<"Barrel module material " << matName0<<" / "<<matName0_base<<" registered"<<endreq;  
	      msg(MSG::DEBUG)<< "  => final material    " << newMat0->getName()
			     << "   density : " << newMat0->getDensity()/(CLHEP::g/CLHEP::cm3) << " g/cm3" 
			     << "     X0 : " << newMat0->getRadLength()/CLHEP::mm << "mm" << endreq;
	      
		    // Save material name
	      brlModuleMaterialNames.push_back(matName0);
	    }
	  } // end of bAlreadyDefined	  
	}// end of loop over module
      } // end of loop over configration (odd/even)
    } // end loop over stave templates
  }// end of loop over layer

  msg(MSG::DEBUG) << "********************************************************************************************"<<endreq;
}


std::string RoutingDyn::constructBarrelLayerName(std::string svcName, std::vector<int> nModule, int iLayer)
{
  std::ostringstream os;
  os << "Brl_"<<svcName;
  for(size_t i=0 ; i<nModule.size(); i++) os << "_M" << nModule[i];
  os << "_L"<<iLayer;
  return os.str();
}

std::string RoutingDyn::constructBarrelLayerName(std::string svcName, std::vector<int> nModule, int iLayer, int iStaveTmp)
{
  std::ostringstream os;
  os << "Brl_"<<svcName;
  for(size_t i=0 ; i<nModule.size(); i++) os << "_M" << nModule[i];
  os << "_L"<<iLayer;
  os << "_S"<<iStaveTmp;
  return os.str();
}

void RoutingDyn::saveLayerSvcLinearMaterial(const PixelGeoBuilderBasics* basics)
{
  std::vector<std::string> matList;
  for ( const auto& layVec : m_bplc) {
    // sum the linear weight
    double linWeight = 0.;
    std::vector< ServiceDynMaterial::Entry> components;
    for ( const auto& lay : layVec) {
      std::map<const ServicesDynLayer*, ServiceDynMaterial>::iterator iMat = m_layerMaterial.find(lay);
      if (iMat !=  m_layerMaterial.end()) {
	ServiceDynMaterial layerMat = iMat->second;
        const std::vector< ServiceDynMaterial::Entry>& comps = layerMat.components();
        for ( const auto& cmp : comps ) {
	  linWeight += cmp.weight*cmp.number; 
          components.push_back(cmp);
	}       
      }
    }

    // create new material for 1 cm^3 volume, density corresponds to the linear weight (in g/cm3), scaling by half stave done already
    std::ostringstream layMatName;
    layMatName << "PixelBarrel_LayMatLin_L"<<layVec[0]->number();
    if (std::find(matList.begin(), matList.end(), layMatName.str()) != matList.end()) continue;
    matList.push_back(layMatName.str());

    GeoMaterial* newMat = new GeoMaterial(layMatName.str(),linWeight*CLHEP::g/CLHEP::cm3);
    for ( const auto& cmp : components ) {
      std::string tmp = cmp.name;
      GeoMaterial *matComp = const_cast<GeoMaterial*>(basics->matMgr()->getMaterial(tmp));
      newMat->add(matComp,cmp.weight*cmp.number/linWeight);
    }
    basics->matMgr()->addMaterial(newMat);
  }
  
  // endcap layers
  for ( const auto& lay : m_eplc) {
    std::map<const ServicesDynLayer*, ServiceDynMaterial>::iterator iMat = m_layerMaterial.find(lay);
    double linWeight = 0.;
    if (iMat !=  m_layerMaterial.end()) {
      ServiceDynMaterial layerMat = iMat->second;////
      const std::vector< ServiceDynMaterial::Entry>& comps = layerMat.components();
      for ( const auto& cmp : comps ) { 
	linWeight += cmp.weight*cmp.number;
      }
      // create new material for 1 cm^3 volume, density corresponds to the linear weight (in g/cm3 )
      std::ostringstream layMatName;
      layMatName << "PixelEndcap_LayMatLin_L"<<lay->number(); 
      if (std::find(matList.begin(), matList.end(), layMatName.str()) != matList.end()) continue;
      matList.push_back(layMatName.str());
      
      GeoMaterial* newMat = new GeoMaterial(layMatName.str(),linWeight*CLHEP::g/CLHEP::cm3);
      for ( const auto& cmp : comps ) {
	std::string tmp = cmp.name;
	GeoMaterial *matComp = const_cast<GeoMaterial*>(basics->matMgr()->getMaterial(tmp));
	newMat->add(matComp,cmp.weight*cmp.number/linWeight);
      }
      basics->matMgr()->addMaterial(newMat);      
    }
  }
}
