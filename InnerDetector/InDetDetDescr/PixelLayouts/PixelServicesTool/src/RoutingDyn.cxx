/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/RoutingDyn.h"
#include "PixelServicesTool/PixelSimpleServiceXMLHelper.h"
#include "PixelServicesTool/PixelDynamicServiceXMLHelper.h"
#include "PixelServicesTool/PixelRoutingServiceXMLHelper.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelServicesTool/ServiceDynVolume.h"
#include "PixelServicesTool/ServicesDynLayer.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include <algorithm>
#include <iostream>

RoutingDyn::RoutingDyn(const Athena::MsgStreamMember& msg):
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

  m_simpleSrvXMLHelper = new PixelSimpleServiceXMLHelper("PIXEL_PIXELSIMPLESERVICE_GEO_XML");
  m_genXMLHelper = new PixelGeneralXMLHelper("PIXEL_PIXELGENERAL_GEO_XML");
  m_svcDynXMLHelper = new PixelDynamicServiceXMLHelper("PIXEL_PIXELDYNAMICSERVICE_GEO_XML");
  m_svcRoutingXMLHelper = new PixelRoutingServiceXMLHelper("PIXEL_PIXELROUTINGSERVICE_GEO_XML");

  m_routeBarrel = true;
  m_routeEndcap = false;

  int count=4;
  std::string strArr[4] = {"inner_PST","outer_PST","inner_IST","outer_IST"};  // FIXME : to be defined in XML file
  for(int i=0; i<count; i++) predefinedRouteNames.push_back(strArr[i]);

}

void RoutingDyn::createRoutingVolumes(ServicesDynTracker& tracker)
{

  msg(MSG::INFO)<<"IST : check if defined "<<m_simpleSrvXMLHelper->SupportTubeRMin("IST")<<endreq;
  m_ISTexists = (m_simpleSrvXMLHelper->SupportTubeRMin("IST") > 0.1); 

  createRoutes(tracker);

  msg(MSG::INFO)<<"Create routes : final volume number "<<m_volumes.size()<<endreq;
  tracker.setServiceDynVolumes(m_volumes);

}

void RoutingDyn::createRoutes(ServicesDynTracker& tracker) 
{
  
  //  barrel pixel vertical route
  m_bplc = tracker.barrelPixelLayers();
  m_eplc = tracker.endcapPixelLayers();

  int nbRoutes = m_svcRoutingXMLHelper->getRouteNumber();

  msg(MSG::INFO)<<"Create routes : registered barrel and endcap routes "<<nbRoutes<<" "<<endreq;

  m_horizRoutes.clear();
  m_vertRoutes.clear();
  m_diagRoutes.clear();

  // PST predefined routes
  double rMin = m_simpleSrvXMLHelper->SupportTubeRMin("PST");
  double rMax = m_simpleSrvXMLHelper->SupportTubeRMax("PST");
  double zMin = m_simpleSrvXMLHelper->SupportTubeZMin("PST");
  double zMax = m_simpleSrvXMLHelper->SupportTubeZMax("PST");
  m_innerPST = HSvcRoute(rMin,zMin,zMax,zMax,"InnerPST");
  m_outerPST = HSvcRoute(rMax,zMin,zMax,zMax,"OuterPST");
  
  // IST predefined routes
  rMin = m_simpleSrvXMLHelper->SupportTubeRMin("IST");
  rMax = m_simpleSrvXMLHelper->SupportTubeRMax("IST");
  zMin = m_simpleSrvXMLHelper->SupportTubeZMin("IST");
  zMax = m_simpleSrvXMLHelper->SupportTubeZMax("IST");
  m_innerIST = HSvcRoute(rMin,zMin,zMax,zMax,"InnerIST");
  m_outerIST = HSvcRoute(rMax,zMin,zMax,zMax,"OuterIST");


  // Create barrel routes
  for(int iRoute=0; iRoute<nbRoutes; iRoute++)
    {
      createRouteFromXML(iRoute);
    }

  organizePredefinedRouteSegment(m_innerPST);
  organizePredefinedRouteSegment(m_outerPST);
  organizePredefinedRouteSegment(m_innerIST);
  organizePredefinedRouteSegment(m_outerIST);
  
  for(std::vector<HSvcRoute>::iterator it=m_horizRoutes.begin(); it!=m_horizRoutes.end(); ++it)
    organizePredefinedRouteSegment(*it);

  for(std::vector<VSvcRoute>::iterator it=m_vertRoutes.begin(); it!=m_vertRoutes.end(); ++it)
    organizePredefinedRouteSegment(*it);
  
}


void RoutingDyn::createRouteFromXML(int iRoute)
{
  // Get radial and horizontal points that define the route
  std::vector<std::string> r = m_svcRoutingXMLHelper->getRouteRadialPositions(iRoute);
  std::vector<std::string> z = m_svcRoutingXMLHelper->getRouteZPositions(iRoute);

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

  int nbSegment = r.size()-1;
  for(int iseg=0 ; iseg<nbSegment; iseg++)
    {
      if (msgLvl(MSG::DEBUG)) { 
	msg(MSG::DEBUG)<<"######################################################################################################################"<<endreq;
	msg(MSG::DEBUG)<<"######################################################################################################################"<<endreq;
	msg(MSG::DEBUG)<<"-> barrel/endcap route :  route "<<iRoute<<" segment "<<iseg<<" radii : "<<r[iseg]<<" to "<<r[iseg+1]<<"   Z :"<< z[iseg]<<" to "<<z[iseg+1]<<" "<<endreq;
      }

      bool bFirst=(iseg==0);
      bool bLast=(iseg==nbSegment-1);
      RouteParameter param(iRoute,iseg,bBarrel,r[iseg],r[iseg+1], z[iseg],z[iseg+1], layerList, svcThick, bFirst, bLast, svcType, EOScardLength, zShift);
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

void RoutingDyn::createEndOfStaveSegment(const RouteParameter& /*param*/)
{


}

void RoutingDyn::organizePredefinedRouteSegment(HSvcRoute route)
{
 
  // Loop over the ServiceDynVolumes to collect the Z positions
  std::vector<double> zPos;
  const SvcRoute::VolumeContainer& svcVol=route.volumes();
  int nbSvcVol=(int)svcVol.size();

  if (msgLvl(MSG::DEBUG)) { 
    msg(MSG::DEBUG)<<"\n------------------------------------------------------------------------------"<<endreq;
    msg(MSG::DEBUG)<<"Re-organize horizontal route : "<<route.name()<<"  "<<nbSvcVol<<endreq;
  }

  if(nbSvcVol==0) return;

  for(int i=0; i<nbSvcVol; i++){
    zPos.push_back(svcVol[i]->zMin());
    zPos.push_back(svcVol[i]->zMax());
  }

  if (msgLvl(MSG::DEBUG)) { 
    msg(MSG::DEBUG)<<"Z pos INIT : "<<route.name()<<"  ";
    int nbZposInit = (int)zPos.size();
    for(int i=0; i<nbZposInit; i++)msg(MSG::DEBUG)<<zPos[i]<<" ";msg(MSG::DEBUG)<<endreq;
  }

  std::sort( zPos.begin(), zPos.end() );
  zPos.erase( unique( zPos.begin(), zPos.end() ), zPos.end() );

  int nbZpos = (int)zPos.size();
  if (msgLvl(MSG::DEBUG)) { 
    msg(MSG::DEBUG)<<"Z pos : "<<route.name()<<"  ";
    for(int i=0; i<nbZpos; i++)msg(MSG::DEBUG)<<zPos[i]<<" ";msg(MSG::DEBUG)<<endreq;
  }

  double radius = svcVol[0]->radius();
  double svcThick = radius-svcVol[0]->rMin();
  HSvcRoute newRoute(radius,zPos[0],zPos[nbZpos-1],zPos[nbZpos-1],route.name());
  
  for(int j=0; j<nbZpos-1; j++){

    std::ostringstream os;
    os << route.name()<<"_Sec"<<j;

    double z1 = zPos[j];
    double z2 = zPos[j+1];
    msg(MSG::DEBUG)<<"   - build horizontal route : "<<route.name()<<" : from "<<z1<<" to "<<z2<<"  at r="<<radius<<"  -> new route "<<os.str()<<endreq;

    ServiceDynVolume* newCyl = new ServiceDynVolume( ServiceDynVolume::Cylinder,
						     radius-svcThick,
						     radius+svcThick,
						     z1,z2,
						     os.str());    
    double zMid = (z1+z2)*.5;
    for(int i=0; i<nbSvcVol; i++)
      {
	double zMin =svcVol[i]->zMin();
	double zMax =svcVol[i]->zMax();
	if((zMid-zMin)*(zMid-zMax)<0) {
	  ServiceDynVolume::LayerContainer layerCont = svcVol[i]->layers();
	  for (ServiceDynVolume::LayerContainer::const_iterator bl=layerCont.begin(); bl!=layerCont.end(); ++bl) newCyl->addLayer(*bl);
	}
      }
    
    newRoute.addVolume(newCyl);	
    addVolume(newCyl);
  }
  
  if (msgLvl(MSG::DEBUG)) { 
    msg(MSG::DEBUG)<<"*****************************************"<<endreq;
    dumpRoute( newRoute);
    msg(MSG::DEBUG)<<"*****************************************"<<endreq;
  }
}



void RoutingDyn::organizePredefinedRouteSegment(VSvcRoute route)
{
 
  // Loop over the ServiceDynVolumes to collect the Z positions
  std::vector<double> rPos;
  const SvcRoute::VolumeContainer& svcVol=route.volumes();
  int nbSvcVol=(int)svcVol.size();

  if (msgLvl(MSG::DEBUG)) { 
    msg(MSG::DEBUG)<<"\n------------------------------------------------------------------------------"<<endreq;
    msg(MSG::DEBUG)<<"Re-organize vertical route : "<<route.name()<<"  "<<nbSvcVol<<endreq;
  }

  if(nbSvcVol==0) return;

  std::vector<Interval> overlapInterval;

  for(int i=0; i<nbSvcVol; i++){
    double rMin = svcVol[i]->rMin();
    double rMax = svcVol[i]->rMax();
    // double zPos = svcVol[i]->zPos();
    double zMin = svcVol[i]->zMin();
    double zMax = svcVol[i]->zMax();

    // Check intersectios with horizontal routes
    for(std::vector<HSvcRoute>::iterator it=m_horizRoutes.begin(); it!=m_horizRoutes.end(); ++it)
      {
	const SvcRoute::VolumeContainer& svcVol_hrz=(*it).volumes();
	//	int nbSvcVol_hrz=(int)svcVol_hrz.size();

// 	for(int isvc=0; isvc<nbSvcVol_hrz; isvc++){
// 	  double zmin_hrz = svcVol_hrz[isvc]->zMin();
// 	  double zmax_hrz = svcVol_hrz[isvc]->zMax();
// 	  double rmin_hrz = svcVol_hrz[isvc]->rMin();
// 	  double rmax_hrz = svcVol_hrz[isvc]->rMax();

	for(auto& itSvc_hrz : svcVol_hrz){
	  double zmin_hrz = itSvc_hrz->zMin();
	  double zmax_hrz = itSvc_hrz->zMax();
	  double rmin_hrz = itSvc_hrz->rMin();
	  double rmax_hrz = itSvc_hrz->rMax();

	  // check overlap vs Z
	  if(!(zmin_hrz>zMax||zmax_hrz<zMin))
	    {
	      Interval locInter(rmin_hrz,rmax_hrz);
	      Interval svcInter(rMin,rMax);
	      bool bIntersection=false;
	      // check overlap vs R
	      if(locInter.isInInterval(rMin)||locInter.isInInterval(rMax))bIntersection=true;
	      if(svcInter.isInInterval(rmin_hrz)||svcInter.isInInterval(rmax_hrz))bIntersection=true;
	      
	      if(bIntersection){
		//  	      msg(MSG::INFO)<<"SERVICE INTERSECTION : "<<svcVol[i]->name()<<" "<<svcVol_hrz[isvc]->name()<<" => "<<rMin<<" "<<rMax<<" // "<<rmin_hrz<<" "<<rmax_hrz<<endreq;
		//  	      msg(MSG::INFO)<<"                : "<<(rmax_hrz-rMax)*(rmax_hrz-rMax)<<" "<<(rmax_hrz-rMax)*(rmax_hrz-rMax)<<endreq;
		//  	      msg(MSG::INFO)<<"                : "<<fabs(rMax-rmax_hrz)<<" "<<fabs(rMin-rmin_hrz)<<endreq;
		rPos.push_back(rmin_hrz);
		rPos.push_back(rmax_hrz);
		bool bAlreadyExists = false;
		//		for(std::vector<Interval>::iterator it2=overlapInterval.begin(); it2!=overlapInterval.end(); ++it2)
		//		  if ((*it2).isIdenticalTo(locInter)) bAlreadyExists = true;
		for(auto& itOverlap : overlapInterval)
		  if (itOverlap.isIdenticalTo(locInter)) bAlreadyExists = true;
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
  if (msgLvl(MSG::DEBUG)) { 
    msg(MSG::DEBUG)<<"Radial pos : "<<route.name()<<"  ";
    for(int i=0; i<nbRpos; i++)msg(MSG::DEBUG)<<rPos[i]<<" ";msg(MSG::DEBUG)<<endreq;
    
    msg(MSG::DEBUG)<<"Overlap_intervals : "<<route.name()<<"  ";
    for(std::vector<Interval>::iterator it=overlapInterval.begin(); it!=overlapInterval.end(); ++it) msg(MSG::DEBUG)<<(*it).getMin()<<" "<<(*it).getMax()<<" // ";
    msg(MSG::DEBUG)<<endreq;  
  }
    
  double zpos = svcVol[0]->zPos();
  double svcThick = .5;
  VSvcRoute newRoute(zpos,rPos[0],rPos[nbRpos-1],rPos[nbRpos-1],route.name());

  // int iSection = 0;
  for(int iInter=0; iInter<nbRpos-1; iInter++)
    {
      double r1 = rPos[iInter];
      double r2 = rPos[iInter+1];

      bool bOverlap = false;
      Interval locInt(r1,r2);
      for(std::vector<Interval>::iterator it=overlapInterval.begin(); it!=overlapInterval.end(); ++it){
	if((*it).containsInterval(locInt)) bOverlap=true;
	//	msg(MSG::INFO)<<"-> isInInterval : "<<r1<<" "<<r2<<" // "<<(*it).getMin()<<" "<<(*it).getMax()<<"  => "<<bOverlap<<" "<<(*it).isInInterval(r1)<<" "<<(*it).isInInterval(r2)<<endreq;
      }

      if(bOverlap)
	msg(MSG::DEBUG)<<"   - vertical route : "<<route.name()<<" : "<<r1<<" to "<<r2<<"  at z="<<zpos<<"  OVERLAP"<<endreq;
      else
	{
	  std::ostringstream os;
	  os << route.name()<<"_Sec"<<iInter;
      
	  double rMid = (r1+r2)*.5;
	  std::vector<int> svcList;
	  for(int i=0; i<nbSvcVol; i++){
	    double rMin =svcVol[i]->rMin();
	    double rMax =svcVol[i]->rMax();
	    if((rMid-rMin)*(rMid-rMax)<0) {
	      svcList.push_back(i);
	      //ServiceDynVolume::LayerContainer layerCont = svcVol[i]->layers();
	      //	      for (ServiceDynVolume::LayerContainer::const_iterator bl=layerCont.begin(); bl!=layerCont.end(); ++bl) os << (*bl)->number();
	    }
	  }
	  
	  if(svcList.size()>0)
	    {
	      msg(MSG::DEBUG)<<"   - vertical route : "<<route.name()<<" : "<<r1<<" to "<<r2<<"  at z="<<zpos<<" -> new route "<<os.str()<<endreq;
	      ServiceDynVolume* newDisk = new ServiceDynVolume( ServiceDynVolume::Disk, r1,r2,
								newRoute.zPos()-0.5*svcThick,
								newRoute.zPos()+0.5*svcThick,
								os.str());
	      
// 	      for(int i=0; i<(int)svcList.size(); i++){
// 		int iVol=svcList[i];
// 		ServiceDynVolume::LayerContainer layerCont = svcVol[iVol]->layers();
// 		for (ServiceDynVolume::LayerContainer::const_iterator bl=layerCont.begin(); bl!=layerCont.end(); ++bl) newDisk->addLayer(*bl);
// 	      }
	      for(auto& itSvc : svcList) {
		int iVol=itSvc;
		ServiceDynVolume::LayerContainer layerCont = svcVol[iVol]->layers();
		for (ServiceDynVolume::LayerContainer::const_iterator bl=layerCont.begin(); bl!=layerCont.end(); ++bl) newDisk->addLayer(*bl);
	      }
	      
	      newRoute.addVolume(newDisk);	
	      addVolume(newDisk);
	    }
	  else
	    msg(MSG::DEBUG)<<"   - vertical route : "<<route.name()<<" : "<<r1<<" to "<<r2<<"  at z="<<zpos<<"  EMPTY"<<endreq;

	}
      
    }
  
  if (msgLvl(MSG::DEBUG)) { 
    msg(MSG::DEBUG)<<"*****************************************"<<endreq;
    dumpRoute( newRoute);
    msg(MSG::DEBUG)<<"*****************************************"<<endreq;
  }

}




MinMaxHelper RoutingDyn::getLayerMinMaxBox(bool bBarrel, std::vector<int> layerIndices, std::vector<double> /*EOScardLength*/, double /*EOSlength*/)
{
  double zMin=0.; double zMax=0.;
  double rMin=0.; double rMax=0.;

  // z minmax / all selected layers
  //  for(int i=0; i<(int)layerIndices.size(); i++){
  int i=0;
  for(auto& it : layerIndices ){
    int layer = it;
    double locZmin=0., locZmax=0., locRmin=0., locRmax=0.;
    if(bBarrel) {
      locZmin = m_bplc[layer]->zPos()-m_bplc[layer]->halfLength();
      locZmax = m_bplc[layer]->zPos()+m_bplc[layer]->halfLength();
      locRmin = m_bplc[layer]->radius();
      locRmax = m_bplc[layer]->radius();
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
  std::string r1 = param.getR1(); 
  std::string r2 = param.getR2();
  std::string z1 = param.getZ1();
  std::string z2 = param.getZ2();
  std::vector<int> layerIndices = param.getLayerIndices();
  double svcThick = param.getSvcThickness();
  std::vector<double> EOScardLength = param.getEOScardLength();
  double EOSlength = param.getEOSsvcLength();
  bool bBarrel = param.isBarrel();
  std::string type = (param.getType()=="endcap")?"Ec":"Brl";

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
  os << "Svc"<<type<<routeId<<segId<<"_RadL";
  //  for(int i=0; i<(int)layerIndices.size(); i++){
  int i=0;
  for(auto& it : layerIndices ){
    int layer = it;
    double radius1 = DecodeLayerRadialPosition(r1,layer,rMin,rMax);
    double radius2 = DecodeLayerRadialPosition(r2,layer,rMin,rMax);
    double zpos = DecodeLayerZPosition(z1,layer,EOSlength,zMin,zMax);
//     radius1+= DecodeLayerMarginPosition(r1);
//     radius2+= DecodeLayerMarginPosition(r2);
//     zpos+= DecodeLayerMarginPosition(z1);
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

  VSvcRoute route(zpos0,rMinLoc,rMaxLoc,rMaxLoc,os.str()+suffix.str());
  
  //  for(int i=0; i<(int)layerIndices.size(); i++){
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
						     radius1,
						     radius2,
						     z-0.5*svcThick,
						     z+0.5*svcThick,
						     os2.str());
    
    if(bBarrel)
      svcVol->addLayer(m_bplc[layer]);
    else
      svcVol->addLayer(m_eplc[layer]);
    
    if(bSvcGrouped)
      route.addVolume(svcVol);
    else {
      VSvcRoute routeLoc(z,radius1,radius2,radius2,os2.str());
      routeLoc.addVolume(svcVol);
      m_vertRoutes.push_back(routeLoc);      
    }
  }
  
  if(bSvcGrouped) m_vertRoutes.push_back(route);

}


void RoutingDyn::createHorizontalRoute(const RouteParameter& param)
{
  int routeId = param.getRouteId();
  int segId = param.getSegmentId();
  std::string r1 = param.getR1(); 
  std::string r2 = param.getR2();
  std::string z1 = param.getZ1();
  std::string z2 = param.getZ2();
  std::vector<int> layerIndices = param.getLayerIndices();
  double svcThick = param.getSvcThickness();
  std::vector<double> EOScardLength = param.getEOScardLength();
  double EOSlength = param.getEOSsvcLength();
  bool bBarrel = param.isBarrel();
  std::string type = (param.getType()=="endcap")?"Ec":"Brl";

  MinMaxHelper boxZR = getLayerMinMaxBox(bBarrel,layerIndices, EOScardLength, EOSlength);
  double rMin = boxZR.getRMin();
  double rMax = boxZR.getRMax();
  double zMin = boxZR.getZMin();
  double zMax = boxZR.getZMax();

  bool bPredefinedRoute=(std::find(predefinedRouteNames.begin(), predefinedRouteNames.end(), r1)!=predefinedRouteNames.end());

  if(!bPredefinedRoute)
    {
      // create routes for each layer

      double zMinLoc = 99999.;
      double zMaxLoc = -1.;
      double radius0 = 0.;
      bool bSvcGrouped = true;
      std::ostringstream os;
      os << "Svc"<<type<<routeId<<segId<<"_Hor";
      //      for(int i=0; i<(int)layerIndices.size(); i++){
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
      suffix<<layerIndices[0]<<"_"<<layerIndices[layerIndices.size()-1];

      msg(MSG::DEBUG)<<"Create horizontal route for  "<<layerIndices.size()<<" layers : "<<os.str()<<endreq;
      HSvcRoute route(radius0,zMinLoc,zMaxLoc,zMaxLoc,os.str()+suffix.str());

      //      for(int i=0; i<(int)layerIndices.size(); i++){
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
							 radius-0.5*svcThick+0.001,
							 radius+0.5*svcThick-0.001,
							 zpos1, zpos2,
							 os2.str());
	if(bBarrel)
	  svcVol->addLayer(m_bplc[layer]);
	else
	  svcVol->addLayer(m_eplc[layer]);

	if(bSvcGrouped)
	  route.addVolume(svcVol);
	else {
	  HSvcRoute routeLoc(radius,zpos1,zpos2,zpos2,os2.str());
	  routeLoc.addVolume(svcVol);
	  m_horizRoutes.push_back(routeLoc);
	}
      }

      if(bSvcGrouped) m_horizRoutes.push_back(route);

      return;
    }
  
  // create routes for each layer
  //  for(int i=0; i<(int)layerIndices.size(); i++){
  for(auto& it : layerIndices ){
    int layer = it;
    std::ostringstream os;
    os << "Svc"<<type<<"_HorL"<<layer<<"_"<<r1;
    
    double deltaRadius = DecodeLayerRadialShiftPosition(r1, svcThick);
    double radius = DecodeLayerRadialPosition(r1,layer,rMin,rMax)+deltaRadius;
    double zpos1 = DecodeLayerZPosition(z1,layer,EOSlength,zMin,zMax);
    double zpos2 = DecodeLayerZPosition(z2,layer,EOSlength,zMin,zMax);
    
    if(!param.isFirstSegment())zpos1-=svcThick*.5;
    if(!param.isLastSegment())zpos2+=svcThick*.5;

    msg(MSG::DEBUG)<<"   - horizontal route : "<<os.str()<<" : "<<zpos1<<" to "<<zpos2<<"  at r="<<radius<<"      "<<r1<<"  predefined (IST/PST)"<<endreq;
    ServiceDynVolume* svcVol = new ServiceDynVolume( ServiceDynVolume::Cylinder,  
						     radius-0.5*svcThick+0.001,
						     radius+0.5*svcThick-0.001,
						     zpos1, zpos2,
						     os.str());
    if(bBarrel)
      svcVol->addLayer(m_bplc[layer]);
    else
      svcVol->addLayer(m_eplc[layer]);

    if(r1.compare("inner_PST")==0)
      m_innerPST.addVolume(svcVol);
    else if(r1.compare("outer_PST")==0)
      m_outerPST.addVolume(svcVol);
    else if(r1.compare("inner_IST")==0)
      m_innerIST.addVolume(svcVol);
    else if(r1.compare("outer_IST")==0)
      m_outerIST.addVolume(svcVol);    
  }

}


void RoutingDyn::dumpRoute( const SvcRoute& route) 
{
  using namespace std;
  msg(MSG::INFO)<< "Dumping route at pos " << route.position() 
		<< " with exit at " << route.exit() << endreq;
  for ( SvcRoute::VolumeContainer::const_iterator iv = route.volumes().begin(); 
	iv != route.volumes().end(); ++iv) {
    (**iv).dump(true);
  }
}

std::string RoutingDyn::nextVolumeName( const SvcRoute& route) const
{
  std::ostringstream os;
  os << route.volumes().size();
  return route.name() + "Vol" + os.str();
}


// Computes the shift due to the thickness of the services 
double RoutingDyn::DecodeLayerRadialShiftPosition(std::string r, double svcThick)
{

  if(r.compare("inner_PST")==0) return -svcThick*.5;
  if(r.compare("outer_PST")==0) return svcThick*.5;
  if(r.compare("inner_IST")==0) return -svcThick*.5;
  if(r.compare("outer_IST")==0) return svcThick*.5;

  std::string pattern = "rMax_Disc_";
  if(r.substr(0,pattern.size()).compare(pattern)==0) return svcThick*.5;

  pattern = "rMin_Disc_";
  if(r.substr(0,pattern.size()).compare(pattern)==0) return -svcThick*.5;

  return 0.;
}


// Check if a margin is defined (+/-)
double RoutingDyn::DecodeLayerMarginPosition(std::string r)
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

  if(r.compare("r_Layer")==0) return m_bplc[layer]->radius()+margin;
  
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
    return m_bplc[layerNumber]->radius()+margin;
  }
  
  if(r.compare("inner_PST")==0) return m_simpleSrvXMLHelper->SupportTubeRMin("PST")+margin;
  if(r.compare("outer_PST")==0) return m_simpleSrvXMLHelper->SupportTubeRMax("PST")+margin;
  if(r.compare("inner_IST")==0) return m_simpleSrvXMLHelper->SupportTubeRMin("IST")+margin;
  if(r.compare("outer_IST")==0) return m_simpleSrvXMLHelper->SupportTubeRMax("IST")+margin;

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


  if(z.compare("zMax_Layer")==0) return m_bplc[layer]->zPos()+m_bplc[layer]->halfLength()+margin;

  if(z.compare("zMax_AllLayer")==0) return zGblMax+zShift+margin;
  if(z.compare("zMin_AllLayer")==0) return zGblMin+zShift+margin;

  std::string pattern = "zMax_Layer_";
  if(z.substr(0,pattern.size()).compare(pattern)==0){
    int layerNumber = atoi((z.substr(pattern.size(),z.size()-pattern.size())).c_str());
    //    msg(MSG::INFO)<<"PATTERN : "<<z<<" "<<layerNumber<<" "<<m_bplc[layerNumber]->zMax()<<endreq;
    return m_bplc[layerNumber]->zMax()+zShift+margin;
  }

  pattern = "zMin_Layer_";
  if(z.substr(0,pattern.size()).compare(pattern)==0){
    int layerNumber = atoi((z.substr(pattern.size(),z.size()-pattern.size())).c_str());
    //    msg(MSG::INFO)<<"PATTERN : "<<z<<" "<<layerNumber<<" "<<m_bplc[layerNumber]->zMin()<<endreq;
    return m_bplc[layerNumber]->zMin()+margin;
  }

  if(z.compare("z_Disc")==0) return m_eplc[layer]->zPos()+margin;

  pattern = "z_Disc_";
  if(z.substr(0,pattern.size()).compare(pattern)==0){
    int discNumber = atoi((z.substr(pattern.size(),z.size()-pattern.size())).c_str());
    //    msg(MSG::INFO)<<"PATTERN : "<<z<<" "<<discNumber<<" "<<m_eplc[discNumber]->zMin()<<endreq;
    return m_eplc[discNumber]->zPos()+zShift+margin;
  }

  if(z.compare("zMin_PST")==0) return m_simpleSrvXMLHelper->SupportTubeZMin("PST")+margin;
  if(z.compare("zMax_PST")==0) return m_simpleSrvXMLHelper->SupportTubeZMax("PST")+margin;
  if(z.compare("zMin_IST")==0) return m_simpleSrvXMLHelper->SupportTubeZMin("IST")+margin;
  if(z.compare("zMax_IST")==0) return m_simpleSrvXMLHelper->SupportTubeZMax("IST")+margin;

  //  msg(MSG::INFO)<<"PATTERN - digit only : "<<z<<" "<<endreq;

  return atof(z.c_str());
}



void RoutingDyn::addRouteMaterial(const PixelGeoBuilderBasics* basics)
{

  msg(MSG::INFO) << "----------------------------------------------------------------------"<<endreq;
  msg(MSG::INFO) << "RoutingDyn::addRouteMaterial called for " << m_volumes.size() << " volumes" << endreq;

  std::map<const ServicesDynLayer*, ServiceDynMaterial> layerMaterial; // cache the layer services
  m_svcMatNames.clear();

  typedef  std::vector<ServiceDynVolume*>::iterator VolumeIter;
  for (VolumeIter iv=m_volumes.begin(); iv!=m_volumes.end(); iv++) 
    {

      //      msg(MSG::INFO) << "********************************************************************************************"<<endreq;
      msg(MSG::DEBUG) << "*** Service material for volume : "<<(**iv).name()<<"  add material "<<endreq;
      
      std::vector<ServiceDynMaterial> result; // = (**iv).materials(); // preserve already present mat. (EOS)
      //    if ((**iv).isEOS()) addEosMaterial(**iv, result);

      // Loop over volumes to compute total material
      ServiceDynVolume::LayerContainer layers = (**iv).layers();
      for (ServiceDynVolume::LayerContainer::const_iterator il=layers.begin(); il!=layers.end(); ++il) 
	{
	  const ServicesDynLayer& layer( **il);

	  ServiceDynMaterial layerMat;
	  std::map<const ServicesDynLayer*, ServiceDynMaterial>::iterator iMat = layerMaterial.find(*il);
	  if (iMat !=  layerMaterial.end()) {
	    //	    msg(MSG::INFO)<<"  - material already defined "<<endreq;
	    layerMat = iMat->second;
	  }
	  else {
	    // Scale material budget for the barrel layer : half weight per layer side
	    bool scalePerHalf = (layer.part()==0);
	    // compute and store material
	    layerMat = computeRouteMaterial( basics, layer.type(), layer.part(), layer.number(),
					     layer.modulesPerStave(), layer.chipsPerModule(), scalePerHalf);
	    layerMaterial[*il] = layerMat;
	  }
	  result.push_back( layerMat);
	}
      
      (**iv).setMaterials( result);

      //      msg(MSG::INFO) << "********************************************************************************************"<<endreq;
    }  
  
}


ServiceDynMaterial RoutingDyn::computeRouteMaterial(const PixelGeoBuilderBasics* basics, 
						    DetTypeDyn::Type layerType, DetTypeDyn::Part layerPart, int layerNumber,
						    std::vector<int> modulePerStave, std::vector<int> chipPerModule,
						    bool scalePerHalf)

{
  msg(MSG::INFO)<<endreq;
  msg(MSG::INFO)<<"** ComputeRouteMaterial for layer "<<layerNumber<<" type "<<layerType;
  msg(MSG::INFO)<<"  part : "<<((layerType==0)?"brl":"ec")<<"  #module/chip : ";
  for(auto& it : modulePerStave) msg(MSG::INFO)<<it<<"  "; msg(MSG::INFO)<<"// ";
  for(auto& it : chipPerModule) msg(MSG::INFO)<<it<<"  "; msg(MSG::INFO)<<endreq;

  std::string name = constructName(layerType, layerPart, layerNumber);
  ServiceDynMaterial result( name);
  
  std::string routeType=(layerPart==0)?"barrel":"endcap";
  int nbModuleType = (int)modulePerStave.size();
  int svcSetIndex_endcap = 0;
  if(routeType=="endcap") svcSetIndex_endcap  = m_svcRoutingXMLHelper->getEndcapServiceSetIndex(layerNumber);

  // Get the service list corresponding to staves  (taken into account only once per module)
  std::vector<std::string> staveMaterialNames;
  if(layerPart==0) staveMaterialNames = m_svcRoutingXMLHelper->getTypeMaterialNames(layerNumber,"stave");
  msg(MSG::INFO)<<"Stave material names : ";for(std::vector<std::string>::iterator it=staveMaterialNames.begin(); it!=staveMaterialNames.end(); ++it) msg(MSG::INFO)<<(*it)<<" "; msg(MSG::INFO)<<endreq;
  std::vector<std::string> staveMaterialCmpt;

  // Loop over the different types of module (barrel/transition/inclined) or (endcap) only
  for(int iModType=0; iModType<nbModuleType; iModType++)
    if(modulePerStave[iModType]>0)
    {
      
      int svcSetIndex = (routeType=="endcap") ? svcSetIndex_endcap : m_svcRoutingXMLHelper->getBarrelServiceSetIndex(layerNumber, iModType);
      msg(MSG::INFO)<<"*** MODULE TYPE : "<<iModType<<"   for layer "<<layerNumber<<" "<<routeType<<"   ";
      msg(MSG::INFO)<<"***             : svcSetIndex : "<<svcSetIndex<<endreq;

      std::vector<std::string> svcList_string = m_svcRoutingXMLHelper->getServiceSetContent(svcSetIndex);
      std::string svcSetName = m_svcRoutingXMLHelper->getServiceSetName(svcSetIndex);
      
      //  msg(MSG::INFO)<<"** ComputeRouteMaterial : layer index "<<layerIndex<<"    svc index "<<svcSetIndex<<endreq;
      //  for(std::vector<std::string>::iterator it=svcList_string.begin(); it!=svcList_string.end(); ++it) msg(MSG::INFO)<<"  -> "<<(*it)<<endreq;
      
      // List of the services defined for the module type
      for(std::vector<std::string>::iterator it=svcList_string.begin(); it!=svcList_string.end(); ++it)
	{
	  // Decode string vs ' ' pattern
	  std::vector<std::string>res;
	  std::istringstream s(*it);
	  std::string tmp;
	  while (s >> tmp) res.push_back(tmp);
	  
	  // Build material name & decode the cable number / multiplicity
	  std::ostringstream os;
	  //      os << "pix::"<< svcSetName<<"_L"<<layerNumber<<"_"<<res[0];
	  os << "pix::"<< svcSetName<<"_"<<res[1];
	  int numCable = decodeStaveCableNumber(res[2], 1, modulePerStave[iModType], chipPerModule[iModType]);
	  
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
	  
	  // cable ( weight of each component )
	  if(res[0]=="svc"){
	    for(int i=4; i<(int)res.size()-1; i+=2)
	      {
		std::string matName = res[i];
		compName.push_back(matName);
		double tmp=atof(res[i+1].c_str());
		
		const GeoMaterial* mat = basics->matMgr()->getMaterial(matName);
		double density = mat->getDensity()/(CLHEP::g/CLHEP::cm3);
		compWeight.push_back(tmp);                 // in g
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
	    compWeight.push_back(coolingArea*density);     // in g
	    totWeight += coolingArea*density;              // in g
	    compDensity.push_back(density);
	    
	    compName.push_back(pipeMat);
	    const GeoMaterial* matPipe = basics->matMgr()->getMaterial(pipeMat);
	    density = matPipe->getDensity()/(CLHEP::g/CLHEP::cm3);
	    compWeight.push_back(pipeArea*density);        // in g
	    totWeight += pipeArea*density;                 // in g
	    compDensity.push_back(density);
	    
	  }

	  // Register material through material manager
	  if(!bMaterialAlreadyDefined) {
	    
	    double densityTmp=0.;
// 	    for(int ii=0; ii<(int)compWeight.size(); ii++) 
// 	      {
// 		double tmp =  compWeight[ii]/totWeight;
// 		compWeightPercent.push_back(tmp);
// 		densityTmp+=tmp/compDensity[ii];
// 	      }
	    for(auto& itWg : compWeight) 
	      {
		double tmp = itWg/totWeight;
		compWeightPercent.push_back(tmp);
		densityTmp+=tmp/itWg;
	      }
	    
	    double density = 1./densityTmp;
	    
	    // Build the material corresponding to the service 
	    //	msg(MSG::INFO)<<"xxxxxxxxxxxxxxxx MATERIAL - ready to define "<<os.str()<<"  "<<density<<" g/cm3"<<endreq;
	    GeoMaterial* newMat = new GeoMaterial(os.str(),density*(CLHEP::g/CLHEP::cm3));
	    int compNumber = (int)compName.size();
	    for(int ii=0; ii<compNumber; ii++)
	      {
		//	      msg(MSG::INFO)<<"   - get "<<compName[ii]<<endreq;
		if(compName[ii].substr(0,5)=="std::"||compName[ii].substr(0,5)=="sct::"||compName[ii].substr(0,5)=="pix::"||compName[ii].substr(0,7)=="indet::")
		  {
		    std::string tmp = compName[ii];
		    GeoMaterial *matComp = const_cast<GeoMaterial*>(basics->matMgr()->getMaterial(tmp));
		    //		  msg(MSG::INFO)<<"   - get material "<<(matComp==0)<<endreq;
		    newMat->add(matComp,compWeightPercent[ii]);
		  }
		else
		  {
		    GeoElement *matComp = const_cast<GeoElement*>(basics->matMgr()->getElement(compName[ii]));
		    newMat->add(matComp,compWeightPercent[ii]);
		  }
	      }		
	    
	    msg(MSG::DEBUG)<<"    >>>> register new material : "<<os.str()<<"   "<<newMat->getDensity()/(CLHEP::g/CLHEP::cm3)<<endreq;
	    basics->matMgr()->addMaterial(newMat);
	  }
	  
	  // check if material is a stave material -> stave material are taken into account only once
	  bool bAddMaterialToBudget = true;
	  for(std::vector<std::string>::iterator itMat=staveMaterialNames.begin(); itMat!=staveMaterialNames.end(); ++itMat) {
	    if(res[1].find(*itMat)!=std::string::npos) 
	      {
		bool bStaveMaterialCmpt=(std::find(staveMaterialCmpt.begin(), staveMaterialCmpt.end(), (*itMat))!=staveMaterialCmpt.end());	      
		if(!bStaveMaterialCmpt)
		  staveMaterialCmpt.push_back(*itMat);
		else
		  bAddMaterialToBudget = false;
	      }
	  }
	      
	  // Add material to global budget
	  if(bAddMaterialToBudget){
	    
	    // Add the material in the SvcDynMaterial object 
	    //	        msg(MSG::INFO)<<"  - register "<<os.str()<<" "<<numCable<<" "<<totWeight<<endreq;
	    if(scalePerHalf)totWeight*=.5;
	    msg(MSG::DEBUG)<<"          -> material : "<<os.str()<<"      cable number "<<numCable<<"  "<<res[2]<<"  mat lin weight : "<<totWeight<<endreq;
	    result.addEntry( os.str(), numCable , true, totWeight);
	  }
	  else
	    msg(MSG::DEBUG)<<"          -> material : "<<os.str()<<"  already taken into account"<<endreq;

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
    msg(MSG::INFO)<<"ERROR : unrecognisable pattern : "<<pattern<<endreq;
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
    if(nameElt.compare("stave")==0) return round((double)nbStave/(double)numElt+.5);
    if(nameElt.compare("ring")==0) return round((double)nbStave/(double)numElt+.5);
    if(nameElt.compare("module")==0) return round((double)modulePerStave/(double)numElt+.5);
    if(nameElt.compare("chip")==0) return round((double)(modulePerStave*chipPerModule)/(double)numElt+.5);
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

  msg(MSG::INFO) << "----------------------------------------------------------------------"<<endreq;
  msg(MSG::INFO) << "RoutingDyn::ComputeBarrelModuleMaterial "<<m_bplc.size()<<endreq;

  // Loop over the barrel layers to create the service material corresponding to each module

  std::vector<std::string> brlModuleMaterialNames;

  int nbBarrelLayers = (int)m_bplc.size();
  for(int iLayer=0; iLayer<nbBarrelLayers; iLayer++)
    {
      // Get the service list corresponding to staves  (taken into account only once per module)
      std::vector<std::string> staveMaterialNames = m_svcRoutingXMLHelper->getTypeMaterialNames(iLayer,"stave");
      msg(MSG::INFO)<<"Stave material names : ";for(std::vector<std::string>::iterator it=staveMaterialNames.begin(); it!=staveMaterialNames.end(); ++it) msg(MSG::INFO)<<(*it)<<" "; msg(MSG::INFO)<<endreq;
      // Total module number
      int nbModule = m_bplc[iLayer]->modulesPerStaveTot()/2;
      if(m_bplc[iLayer]->modulesPerStaveTot()%2==1) nbModule++;

      // Nb module per type of module
      int nbModuleType = m_bplc[iLayer]->moduleTypeNumber();
      std::vector<int> nbModulePerType_init=m_bplc[iLayer]->modulesPerStave();

      std::map<std::string,std::vector<int> >configurationType;
      std::vector<int> nbModulePerType_tmp;
      for(int i=0 ; i<nbModuleType; i++) 
	nbModulePerType_tmp.push_back(nbModulePerType_init[i]/2+nbModulePerType_init[i]%2);
      configurationType.insert(std::pair<std::string,std::vector<int> >("even",nbModulePerType_tmp));
      if(nbModulePerType_init[0]%2==1) 
	{
	  nbModulePerType_tmp.clear();
	  nbModulePerType_tmp.push_back(nbModulePerType_init[0]/2);
	  for(int i=1 ; i<nbModuleType; i++) 
	    nbModulePerType_tmp.push_back(nbModulePerType_init[i]/2+nbModulePerType_init[i]%2);
	  configurationType.insert(std::pair<std::string,std::vector<int> >("odd",nbModulePerType_tmp));
	}

      // Material identifiers
      std::string materialId;
      for(int iType=0 ; iType<nbModuleType; iType++) {
	int svcSetIndex  = m_svcRoutingXMLHelper->getBarrelServiceSetIndex(iLayer, iType);
	materialId+=m_svcRoutingXMLHelper->getServiceSetNameId(svcSetIndex);
      }

      // Loop over configuration types
      std::map<std::string,std::vector<int> >::iterator configIterator;
      for(configIterator=configurationType.begin(); configIterator!=configurationType.end(); ++configIterator)
	{

	  std::vector<int> nbModulePerType = configIterator->second;
	  // Loop over the module : starting from the center of a stave
	  for(int iModule=1; iModule<nbModule+1; iModule++)
	    {
	      
	      // Distribute the module number over the different module types
	      std::vector<int> nbModuleLayer;
	      for(int i=0; i<nbModuleType; i++)nbModuleLayer.push_back(0);
	      bool bEndOfLoop=false;
	      int iCmpt_prev=0;
	      for(int iType=0 ; iType<nbModuleType&&!bEndOfLoop; iType++) 
		{
		  int iCmpt_next = iCmpt_prev+nbModulePerType[iType];
		  if(iModule>=iCmpt_next) nbModuleLayer[iType]=nbModulePerType[iType];
		  else { nbModuleLayer[iType]=iModule-iCmpt_prev; bEndOfLoop=true; }
		  iCmpt_prev = iCmpt_next;
		}
	      
	      if (msgLvl(MSG::DEBUG)) { 
		msg(MSG::DEBUG)<<endreq;
		msg(MSG::DEBUG)<<"MODULE per layer : "<<iModule<<" : "; for(int i=0; i<(int)nbModuleLayer.size(); i++) msg(MSG::INFO)<<nbModuleLayer[i]<<"  "; msg(MSG::INFO)<<"// "<<materialId<<endreq;
	      }
	      
	      std::string matName = constructBarrelLayerName(materialId, nbModuleLayer);
	      bool bAlreadyDefined = (std::find(brlModuleMaterialNames.begin(), brlModuleMaterialNames.end(), matName)!=brlModuleMaterialNames.end());	 
	      
	      // Register material name
	      std::ostringstream idName;
	      idName<<"Barrel_L"<<iLayer;
	      for(int iType=0 ; iType<nbModuleType; iType++) idName<<"_M"<<nbModuleLayer[iType];
	      m_svcMaterialNameTable.insert(std::pair<std::string,std::string>(idName.str(),matName));
	      
	      // Material already defined
	      if(bAlreadyDefined) {
		msg(MSG::INFO) <<"Barrel module material "<<matName<<" already defined"<<endreq;
	      }
	      else {
		// Compute material corresponding to iModule modules
		ServiceDynMaterial layerMat = computeRouteMaterial( basics, m_bplc[iLayer]->type(), m_bplc[iLayer]->part(), m_bplc[iLayer]->number(),
								    nbModuleLayer, m_bplc[iLayer]->chipsPerModule(), false);
		
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
		  for(std::vector<std::string>::iterator itMat=staveMaterialNames.begin(); itMat!=staveMaterialNames.end(); ++itMat) {
		    if(ient->name.find(*itMat)!=std::string::npos) 
		      {
			bool bStaveMaterialCmpt=(std::find(staveMaterialCmpt.begin(), staveMaterialCmpt.end(), (*itMat))!=staveMaterialCmpt.end());	      
			if(!bStaveMaterialCmpt)
			  staveMaterialCmpt.push_back(*itMat);
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
		      else{
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
		for(int i=0; i<nbComp; i++)
		  {
		    GeoMaterial *matComp = const_cast<GeoMaterial*>(basics->matMgr()->getMaterial(linearComponents[i]));
		    newMat->add(matComp, linWeights[i]*invLinWeightsTot);
		  }
		basics->matMgr()->addMaterial(newMat);
		
		// Register weighted material
		basics->matMgr()->addWeightMaterial(matName, matName_base, linWeightsTot , 1);
		msg(MSG::DEBUG)<<"Barrel module material "<<matName<<" / "<<matName_base<<" registered"<<endreq;  
		
		msg(MSG::DEBUG)<< "  => final material    " << newMat->getName()<<"   density : "<<newMat->getDensity()/(CLHEP::g/CLHEP::cm3)<<" g/cm3     X0 : "<<newMat->getRadLength()/CLHEP::mm<<"mm"<<endreq;
		
		// Save material name
		brlModuleMaterialNames.push_back(matName);
	      } // end of bAlreadyDefined
	      
	      if (msgLvl(MSG::DEBUG)) { 
		const GeoMaterial * newMat = basics->matMgr()->getMaterialForVolume( matName, 1. );
		msg(MSG::DEBUG) << "  moduleMat ("<<iLayer<<" "<<iModule<<" "<<newMat->getRadLength()/CLHEP::mm<<"),"<<endreq;
	      }

	    }// end of loop over module
	} // end of loop over configration (odd/even)
    }// end of loop over layer

  msg(MSG::DEBUG) << "********************************************************************************************"<<endreq;
  
}


std::string RoutingDyn::constructBarrelLayerName(std::string svcName, std::vector<int> nModule)
{
  std::ostringstream os;
  os << "Brl_"<<svcName;
  for(int i=0 ; i<(int)nModule.size(); i++) os<<"_M"<<nModule[i];
  return os.str();
}


