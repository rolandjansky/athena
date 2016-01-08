/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/RoutingDynAuto.h"
#include "PixelServicesTool/PixelSimpleServiceXMLHelper.h"
#include "PixelServicesTool/PixelDynamicServiceXMLHelper.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelServicesTool/ServiceDynVolume.h"
#include "PixelServicesTool/ServicesDynLayer.h"

#include <algorithm>
#include <iostream>
using namespace std; // for DEBUG

RoutingDynAuto::RoutingDynAuto(const Athena::MsgStreamMember& msg):
  previousBarrelLayer(0), m_msg(msg)
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
  m_svcOtherXMLHelper = new PixelDynamicServiceXMLHelper("PIXEL_PIXELDYNAMICSERVICE_GEO_XML");

  m_routeBarrel = true;
  m_routeEndcap = true;

//   if(m_svcOtherXMLHelper){
//     m_routeBarrel = m_svcOtherXMLHelper->routeBarrelStandard();
//     m_routeEndcap = m_svcOtherXMLHelper->routeEndcapStandard();
//   }
  
}

void RoutingDynAuto::createRoutingVolumes(ServicesDynTracker& tracker)
{

  std::cout<<"IST : check if defined "<<m_simpleSrvXMLHelper->SupportTubeRMin("IST")<<std::endl;
  m_ISTexists = (m_simpleSrvXMLHelper->SupportTubeRMin("IST") > 0.1); 

  createRoutes(tracker);

  routeOuterBarrelPixel(tracker);
  routeEndcapPixel( tracker);

  // Link routes
  connectRoutes( m_bpVSvcRoute, m_epHSvcRoute);
  if (!m_pixelAlongBarrelStrip && !m_ISTexists) {
    connectRoutes( m_epHSvcRoute, m_pixelV2Route);
    connectRoutes( m_pixelV2Route, m_pixelH2Route);
  }

  // Create Strip layer routes
  routeBarrelStrip( tracker);
  if (m_pixelAlongBarrelStrip) connectRoutes( m_epHSvcRoute, m_bsVSvcRoute);
  
  // Create pixel layer routes
  routeInnerBarrelPixel(tracker);
  connectRoutes( m_istVSvcRoute, m_istHSvcRoute);

  // Output route details in debug mode
  if(msgLvl(MSG::DEBUG)){
    dumpRoute( m_bpVSvcRoute);
    dumpRoute( m_epHSvcRoute);
    dumpRoute( m_bsVSvcRoute);
    dumpRoute( m_istVSvcRoute);
    dumpRoute( m_istHSvcRoute);
    if (!m_pixelAlongBarrelStrip && !m_ISTexists) {
      dumpRoute( m_pixelV2Route);
      dumpRoute( m_pixelH2Route);
    }
  }

  tracker.setServiceDynVolumes(m_volumes);

}

void RoutingDynAuto::createRoutes(ServicesDynTracker& tracker) 
{
  
  //  barrel pixel vertical route
  const LayerContainer& bplc = tracker.barrelPixelLayers();
  const LayerContainer& eplc = tracker.endcapPixelLayers();
  //  const LayerContainer& bslc = tracker.barrelStripLayers();
  
  std::cout<<"Create routes : "<<bplc.size()<<" "<<c_nInnerPixelLayers<<std::endl;

  double bpZmax = 0;
  for (LayerContainer::const_iterator bl=bplc.begin()+c_nInnerPixelLayers; bl!=bplc.end(); ++bl){
    std::cout<<"bpZamx "<<bpZmax<<"  bl==0 "<<((*bl)==0)<<std::endl; 
    bpZmax = std::max( bpZmax, (**bl).zPos() + (**bl).halfLength());
  }

  std::cout<<"Create routes : 1"<<std::endl;
  
  double bpVertRouteZpos = bpZmax+c_EosTolerance+c_bpEosLength +  0.5*c_ServiceDiskThickness + c_safetyGap;
  double bpVertRouteRmin = bplc[c_nInnerPixelLayers]->radius(); //change if along PST
								  
  std::cout<<"Create routes : 2"<<std::endl;

  //  Find max endcap pixel layer radius
  double epRmax = 0;
  for (LayerContainer::const_iterator i=eplc.begin(); i!=eplc.end(); ++i) 
    epRmax = std::max( epRmax, (**i).rMax());

  std::cout<<"Create routes : 3"<<std::endl;

  //Find max radius for vertical route
  double bpVertRouteRmax;
  if (!m_ISTexists) {
    bpVertRouteRmax = 1500.;   //0.5*(epRmax + c_epEosLength + tracker.geoMgr()->sctInnerSupport());    // SES fixme
    //    Pixel horizontal route in the middle between pixel disks and first strip layer
    //    if ( epRmax + c_epEosLength + c_ServiceCylinderThickness > tracker.geoMgr()->sctInnerSupport())
    //      msg(MSG::WARNING)<< " No space for services between pixel diskd and sct support" << endreq;
  }
  else {
    //    services along PST, as close as they can get
    bpVertRouteRmax = m_simpleSrvXMLHelper->SupportTubeRMin("PST") - c_safetyGap - 0.5*c_ServiceCylinderThickness;
  }

  std::cout<<"Create routes : 4"<<std::endl;

  double bpHorRouteR = bpVertRouteRmax;
  std::cout<<"Create routes : 41"<<std::endl;
  double bpHSvcRouteZmin = bpVertRouteZpos +  0.5*c_ServiceDiskThickness + c_safetyGap;
  std::cout<<"Create routes : 42"<<std::endl;
  double bpHSvcRouteZmax = eplc.back()->zPos();   // prolong if along PST ?
  std::cout<<"Create routes : 43"<<std::endl;
  msg(MSG::INFO) << "Route2: setting bpHSvcRouteZmax to " << bpHSvcRouteZmax << endreq;
  std::cout<<"Create routes : 44"<<std::endl;

  if(m_simpleSrvXMLHelper->SupportTubeRMin("PST") > 0.1) 
    bpHSvcRouteZmax = m_simpleSrvXMLHelper->SupportTubeZMax("PST");
  std::cout<<"Create routes : 45"<<std::endl;
  msg(MSG::INFO) << "Route2: changinng bpHSvcRouteZmax (/PST zmax) to " << bpHSvcRouteZmax << endreq;

  std::cout<<"Create routes : 5"<<std::endl;

//   //  Assume same length barrel, the loop is to make sure there are no volume overlaps                  // SES fixme
//   //  in case strip barrel layers are slightly different
//   double bsZmax = 0;
//   for (LayerContainer::const_iterator i=bslc.begin(); i!=bslc.end(); ++i)
//     bsZmax = std::max( bsZmax, (**i).zPos() + (**i).halfLength());
  
  double bsZmax = 2500.;
  double bsVertRouteZpos = bsZmax+c_EosTolerance+c_bsEosLength + 0.5*c_ServiceDiskThickness + c_safetyGap;

//   bpHSvcRouteZmax = m_simpleSrvXMLHelper->SupportTubeZMax("PST");
  
  double bpHSvcRouteZmax_mode = m_simpleSrvXMLHelper->SupportTubeZMin("mode");

  bool bMSTI = m_simpleSrvXMLHelper->SupportTubeExists("MSTI");
  bool bMSTM = m_simpleSrvXMLHelper->SupportTubeExists("MSTM");
  bool bMSTO = m_simpleSrvXMLHelper->SupportTubeExists("MSTO");

  if(bMSTI||bMSTM||bMSTO){
    bpHSvcRouteZmax = m_simpleSrvXMLHelper->SupportTubeZMax("PST");
    if(bpHSvcRouteZmax_mode>0.1&&bpHSvcRouteZmax_mode<bpHSvcRouteZmax) bpHSvcRouteZmax = bpHSvcRouteZmax_mode-0.001;
  }

  std::cout<<"Create routes : 6"<<std::endl;

  if(bpHSvcRouteZmax_mode>0&&bpHSvcRouteZmax>bpHSvcRouteZmax_mode) bpHSvcRouteZmax=bpHSvcRouteZmax_mode;
  msg(MSG::INFO)<< "Changing bpHSvcRouteZmax to " << bpHSvcRouteZmax << " mode : "<<bpHSvcRouteZmax_mode<<endreq;
  
  //  double bsVertRouteRmin = bpHorRouteR + 0.5*c_ServiceCylinderThickness + c_safetyGap;
  //  double bsVertRouteRmax = bslc.back()->radius() + c_ServiceCylinderThickness;                //approx
  
  m_bpVSvcRoute = VSvcRoute( bpVertRouteZpos, bpVertRouteRmin, bpVertRouteRmax, bpVertRouteRmax, "OuterPixelRPath");

  if (m_pixelAlongBarrelStrip)
    m_bpHSvcRoute = HSvcRoute( bpHorRouteR, bpHSvcRouteZmin, bpHSvcRouteZmax, bsVertRouteZpos,"OuterPixelZPath"); //different if along PST
  else 
    m_bpHSvcRoute = HSvcRoute( bpHorRouteR, bpHSvcRouteZmin, bpHSvcRouteZmax, bpHSvcRouteZmax,"OuterPixelZPath");

  m_epHSvcRoute = m_bpHSvcRoute;  //different if along PST
  //  m_bsVSvcRoute = VSvcRoute( bsVertRouteZpos, bsVertRouteRmin, bsVertRouteRmax, bsVertRouteRmax,"BarrelStripRPath");

  std::cout<<"Create routes : 7"<<std::endl;

  m_bpVSvcRoute.setNextRoute(&m_bpHSvcRoute);
  if (m_pixelAlongBarrelStrip) m_bpHSvcRoute.setNextRoute(&m_bsVSvcRoute);

  // Additional Routes:
  //  Outside MSTOuter
  double bpMSTO_R =  m_simpleSrvXMLHelper->SupportTubeRMax("MSTO") + c_safetyGap + 0.5*c_ServiceCylinderThickness;
  m_MSTO_HSvcRoute = HSvcRoute( bpMSTO_R, bpHSvcRouteZmin, bpHSvcRouteZmax, bpHSvcRouteZmax, "MSTOPixelZPath"); 

  //  Outside MSTMiddle
  double bpMSTM_R =  m_simpleSrvXMLHelper->SupportTubeRMax("MSTM") + c_safetyGap + 0.5*c_ServiceCylinderThickness;
  m_MSTM_HSvcRoute = HSvcRoute( bpMSTM_R, bpHSvcRouteZmin, bpHSvcRouteZmax, bpHSvcRouteZmax, "MSTMPixelZPath"); 

  //  Outside MSTInner
  double bpMSTI_R =  m_simpleSrvXMLHelper->SupportTubeRMax("MSTI") + c_safetyGap + 0.5*c_ServiceCylinderThickness;
  m_MSTI_HSvcRoute = HSvcRoute( bpMSTI_R, bpHSvcRouteZmin, bpHSvcRouteZmax, bpHSvcRouteZmax, "MSTIPixelZPath"); 

  //  Inside MSTMiddle
  double bpMSTM_RI =  m_simpleSrvXMLHelper->SupportTubeRMin("MSTM") - c_safetyGap - 0.5*c_ServiceCylinderThickness;
  m_MSTM_HSvcRouteInner = HSvcRoute( bpMSTM_RI, bpHSvcRouteZmin, bpHSvcRouteZmax, bpHSvcRouteZmax, "MSTMPixelZPathInner"); 

  std::cout<<"Create routes : 8"<<std::endl;

  //  Exit route for pixel
  if (!m_pixelAlongBarrelStrip) createOuterPixelRoutes(tracker);

  std::cout<<"Create routes : 9"<<std::endl;

  //  route inside PST
  createRoutesInIST(tracker);

}

void RoutingDynAuto::createOuterPixelRoutes(ServicesDynTracker& /*tracker*/) 
{

  if (!m_ISTexists) { // PST is actually IST, and there is no PST

    // we need to reduce the route radius after the pixel disks to avoid conflict with SCT disks
    double routeRadius = m_genXMLHelper->getEnvelopeRMax() - c_safetyGap - c_ServiceCylinderThickness/2;
    //double zpos = 0.5 * (tracker.endcapPixelLayers().back()->zPos() + tracker.endcapStripLayers().front()->zPos());
    double zpos = m_epHSvcRoute.zMax() + c_ServiceDiskThickness/2 + c_safetyGap;

    m_pixelV2Route = VSvcRoute( zpos, routeRadius, m_bpHSvcRoute.radius(), routeRadius, "OuterPixelRPath2");

    double pstLen = m_simpleSrvXMLHelper->SupportTubeZMax("PST");
    m_pixelH2Route = HSvcRoute( routeRadius, zpos + c_ServiceDiskThickness/2 + c_safetyGap, pstLen, pstLen,"OuterPixelZPath2");
    m_epHSvcRoute.setNextRoute( &m_pixelV2Route);
    m_pixelV2Route.setNextRoute( &m_pixelH2Route);
  }

}

void RoutingDynAuto::createRoutesInIST(ServicesDynTracker& tracker) 
{

  const LayerContainer& bplc = tracker.barrelPixelLayers();
  double bpZmax = 0;
  for (LayerContainer::const_iterator bl=bplc.begin(); bl!=bplc.begin()+c_nInnerPixelLayers; ++bl) {
    bpZmax = std::max( bpZmax, (**bl).zPos() + (**bl).halfLength());
  }

  double istVSvcRouteZpos = bpZmax+c_EosTolerance+c_bpEosLength + 0.5*c_ServiceDiskThickness + c_safetyGap;
  double istVSvcRouteRmin = bplc.front()->radius();

  double istRmin;
  double istZmax;
  if (m_ISTexists) { 
    // use IST if it exists
   // use IST if it exists
    istRmin = m_simpleSrvXMLHelper->SupportTubeRMin("IST");
    istZmax = m_simpleSrvXMLHelper->SupportTubeZMax("IST");
  }
  else { 
    // use PST as IST for backward compatibility
   // use PST as IST for backward compatibility
    istRmin = m_simpleSrvXMLHelper->SupportTubeRMin("PST");
    istZmax = m_simpleSrvXMLHelper->SupportTubeZMax("PST");
  }

  double istVSvcRouteRmax = istRmin - 0.5*c_ServiceCylinderThickness - c_safetyGap;
  m_istVSvcRoute = VSvcRoute( istVSvcRouteZpos, istVSvcRouteRmin, istVSvcRouteRmax, istVSvcRouteRmax, "InnerPixelRPath");

  double istHSvcRouteZmin = istVSvcRouteZpos + 0.5*c_ServiceDiskThickness + c_safetyGap;
  double istHSvcRouteZmax = istZmax;
  m_istHSvcRoute = HSvcRoute( istVSvcRouteRmax, istHSvcRouteZmin, istHSvcRouteZmax, istHSvcRouteZmax, "InnerPixelZPath");
  m_istVSvcRoute.setNextRoute(&m_istHSvcRoute);
  
}

void RoutingDynAuto::routeOuterBarrelPixel(ServicesDynTracker& tracker)
{

  if(!m_routeBarrel) return;
  const LayerContainer& bls = tracker.barrelPixelLayers();
  for (LayerContainer::const_iterator bl=bls.begin()+c_nInnerPixelLayers; bl!=bls.end(); ++bl) {
    routeBarrelLayer( bl, bls.end(), tracker, m_bpVSvcRoute);
  }

}

void RoutingDynAuto::routeBarrelStrip(ServicesDynTracker& tracker)
{

  const LayerContainer& bls = tracker.barrelStripLayers();
  for (LayerContainer::const_iterator bl=bls.begin(); bl!=bls.end(); ++bl) {
    routeBarrelLayer( bl, bls.end(), tracker, m_bsVSvcRoute);
  }

}

void RoutingDynAuto::routeEndcapPixel(ServicesDynTracker& tracker)
{

 //  if(!m_routeEndcap) return;

//   const LayerContainer& lc = tracker.endcapPixelLayers();
//   for (LayerContainer::const_iterator bl=lc.begin(); bl!=lc.end(); ++bl) {
//     routeEndcapLayer( bl, lc.end(), tracker, m_epHSvcRoute);
//   }
// }


  const LayerContainer& lc = tracker.endcapPixelLayers();
  bool bMSTI = m_simpleSrvXMLHelper->SupportTubeExists("MSTI");
  bool bMSTM = m_simpleSrvXMLHelper->SupportTubeExists("MSTM");
  bool bMSTO = m_simpleSrvXMLHelper->SupportTubeExists("MSTO");

  for (LayerContainer::const_iterator bl=lc.begin(); bl!=lc.end(); ++bl)
  {

    std::string svcRoute = m_svcOtherXMLHelper->EndcapDiscRoute((*bl)->number());

    if(svcRoute=="MSTO"){
      routeEndcapLayer( bl, lc.end(), tracker, m_MSTO_HSvcRoute, 2);
    }
    else if(svcRoute=="MSTM"){
	if (isRoutedOutsideSupportTube(bl, m_MSTM_HSvcRoute))
	  routeEndcapLayer( bl, lc.end(), tracker, m_MSTM_HSvcRoute, 1);
	else
	  routeEndcapLayer( bl, lc.end(), tracker, m_MSTM_HSvcRouteInner, 0);
    }


      /*
	if (bMSTI)
	{
	if (tracker.geoMgr()->pixelDiskServiceRoute( (*bl)->number() )  == "MSTI")
	routeEndcapLayer( bl, lc.end(), tracker, m_MSTI_HSvcRoute);
	}
	
	// MSTM can have service on inside or outside
	if (bMSTM)
	{
	if (tracker.geoMgr()->pixelDiskServiceRoute( (*bl)->number() )  == "MSTM")
	{
	if (isRoutedOutsideSupportTube(bl, m_MSTM_HSvcRoute))
	routeEndcapLayer( bl, lc.end(), tracker, m_MSTM_HSvcRoute);
	else
	routeEndcapLayer( bl, lc.end(), tracker, m_MSTM_HSvcRouteInner);
	}
	}
	
	if (bMSTO)
	{
	if (tracker.geoMgr()->pixelDiskServiceRoute( (*bl)->number() )  == "MSTO")
	routeEndcapLayer( bl, lc.end(), tracker, m_MSTO_HSvcRoute);
	}
      */
  }

  if(!bMSTI&&!bMSTM&&!bMSTO)
   for (LayerContainer::const_iterator bl=lc.begin(); bl!=lc.end(); ++bl) 
     routeEndcapLayer( bl, lc.end(), tracker, m_epHSvcRoute);

}


bool RoutingDynAuto::isRoutedOutsideSupportTube(LayerContainer::const_iterator bl, HSvcRoute& route)
{
  if ( (**bl).rMax() > route.radius())  return true;
  return false;
}

void RoutingDynAuto::routeInnerBarrelPixel(ServicesDynTracker& tracker)
{
  const LayerContainer& bls = tracker.barrelPixelLayers();
  for (LayerContainer::const_iterator bl=bls.begin(); bl!=bls.begin()+c_nInnerPixelLayers; ++bl) {
    routeBarrelLayer( bl, bls.begin()+c_nInnerPixelLayers, tracker, m_istVSvcRoute);
  }
}

void RoutingDynAuto::routeBarrelLayer(LayerContainer::const_iterator bl,
				LayerContainer::const_iterator blend, 
				ServicesDynTracker& /*tracker*/, VSvcRoute& route)
{
  // 1. construct end-of-stave volume 
  double zEosMin = (*bl)->halfLength() + eosTolerance( (*bl)->type(), DetTypeDyn::Barrel);
  double zEosMax = zEosMin + eosLength( (*bl)->type(), DetTypeDyn::Barrel);

  if (zEosMax < route.position() - 0.5*c_ServiceDiskThickness) {
    zEosMax = route.position() - 0.5*c_ServiceDiskThickness - c_safetyGap;
  }
  else {
    msg(MSG::WARNING) << "not enough space for end of stave of barrel layer at radius "
		      << (**bl).radius() << endreq;
  }
  double halfEosThick = eosHalfThickness( (*bl)->type(), DetTypeDyn::Barrel);
  ServiceDynVolume* eosCylinder = new ServiceDynVolume( ServiceDynVolume::Cylinder,
						  (*bl)->radius()-halfEosThick,
						  (*bl)->radius()+halfEosThick,
						  zEosMin, zEosMax, (**bl).name() + "EOS");
  eosCylinder->addLayer(*bl);
  addVolume( eosCylinder);
  eosCylinder->addEosServices(*bl); // add all the stave EOS stuff

  // 2. Connect to route
  double rMin, rMax;
  if (route.volumes().empty()) rMin = route.rMin();
  else rMin = route.volumes().back()->rMax() + c_safetyGap;

  LayerContainer::const_iterator blnext = bl+1;
  if (blnext != blend) {
    rMax = std::min((**blnext).radius(), route.rExit());
  }
  else rMax = route.rExit();

  ServiceDynVolume* newDisk = new ServiceDynVolume( ServiceDynVolume::Disk, rMin, rMax, 
					      route.zPos()-0.5*c_ServiceDiskThickness,
					      route.zPos()+0.5*c_ServiceDiskThickness,
					      nextVolumeName(route));
  newDisk->dump(true);
  // newDisk->addLayer(*bl); // done by connect()
  connect( eosCylinder, newDisk);
  if (!route.volumes().empty()) connect( route.volumes().back(), newDisk);
  route.addVolume( newDisk);
  addVolume( newDisk);
}

void RoutingDynAuto::routeEndcapLayer(LayerContainer::const_iterator bl,
				      LayerContainer::const_iterator blend, 
				      ServicesDynTracker& /*tracker*/, HSvcRoute& route,
				      int iLayer)
{
  // 1. construct end-of-stave volume 
//   double rEosMin = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetTypeDyn::Endcap);
//   double rEosMax = rEosMin + eosLength( (*bl)->type(), DetTypeDyn::Endcap);
//   if (rEosMax > route.position() - 0.5*c_ServiceCylinderThickness) {
//     msg(MSG::WARNING) << "not enough space for end of stave of endcap layer at Z = "
// 		      << (**bl).zPos() << endreq;
//   }
//   rEosMax = route.position() - 0.5*c_ServiceCylinderThickness - c_safetyGap;
//   if (rEosMax < rEosMin) {
//     msg(MSG::WARNING) << "no space for routing of endcap layer at Z = "
// 		      << (**bl).zPos() << endreq;
//   }

//  double rEosMin_db = m_svcOtherXMLHelper->EndcapEOSRMin(0);

  double rEosMin = 0.0;
  double rEosMax = 0.0;
  std::string SupportName =  m_svcOtherXMLHelper->EndcapDiscRoute((*bl)->number());
  std::cout<<"ROUTE ENDCAP LAYERS : "<<SupportName<<std::endl;
  double EOSZOffset = 0.0;

  bool bRoutedOutsideSupport = isRoutedOutsideSupportTube(bl, route);

  if (SupportName == "PST")
  {
    rEosMin = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetTypeDyn::Endcap);   // Disk outer edge + safety
    //    std::cout<<"rEOS Min : "<<rEosMin<<" "<<rEosMin_db<<std::endl;
    //    if(rEosMin<rEosMin_db) rEosMin=rEosMin_db;
    rEosMax = route.position() - 0.5*c_ServiceCylinderThickness - c_safetyGap; // support tube - safety
    EOSZOffset =  m_svcOtherXMLHelper->EndcapEOSOffset((*bl)->number());
    if (rEosMax < rEosMin) msg(MSG::WARNING) << "No space for routing of endcap layer at Z = " << (**bl).zPos() << endreq;
  }
  else if (SupportName == "MST")
  {
    rEosMin = (*bl)->rMin() + eosTolerance( (*bl)->type(), DetTypeDyn::Endcap); // EOS same size as Disk supports
    rEosMax = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetTypeDyn::Endcap);
    EOSZOffset = m_svcOtherXMLHelper->EndcapEOSOffset((*bl)->number());
  }
  else if (SupportName == "IST")
  {
    rEosMin = route.position() + 0.5*c_ServiceCylinderThickness + c_safetyGap; // support tube + safety
    rEosMax = (*bl)->rMin() - eosTolerance( (*bl)->type(), DetTypeDyn::Endcap);   // Disk outer edge - safety
    EOSZOffset =  m_svcOtherXMLHelper->EndcapEOSOffset((*bl)->number());
    if (rEosMax < rEosMin) msg(MSG::WARNING) << "No space for routing of endcap layer at Z = " << (**bl).zPos() << endreq;
  }
  else if (SupportName == "MSTO" || SupportName == "MSTM" || SupportName == "MSTI" )
  {
    // rEosMin/Max are different if disk runs inside or outside the support tube
    if (bRoutedOutsideSupport)
    {
      rEosMin = route.position() + 0.5*c_ServiceCylinderThickness + c_safetyGap;
      rEosMax = (*bl)->rMax() - eosTolerance( (*bl)->type(), DetTypeDyn::Endcap);
    }
    else
    {
       rEosMax = route.position() - 0.5*c_ServiceCylinderThickness - c_safetyGap;
       rEosMin = (**bl).rMin() - eosTolerance( (**bl).type(), DetTypeDyn::Endcap);
    }
    EOSZOffset = m_svcOtherXMLHelper->EndcapEOSOffset((*bl)->number());
  }
  else if (SupportName=="StdRoute")
    {
      rEosMin = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetTypeDyn::Endcap);
      rEosMax = rEosMin + eosLength( (*bl)->type(), DetTypeDyn::Endcap);
      EOSZOffset = 0;
      if (rEosMax > route.position() - 0.5*c_ServiceCylinderThickness) {
	msg(MSG::WARNING) << "not enough space for end of stave of endcap layer at Z = "
			  << (**bl).zPos() << endreq;
      }
      rEosMax = route.position() - 0.5*c_ServiceCylinderThickness - c_safetyGap;
      if (rEosMax < rEosMin) {
	msg(MSG::WARNING) << "no space for routing of endcap layer at Z = "
			  << (**bl).zPos() << endreq;
      }
    }
  else
  {
    msg(MSG::WARNING) << "Specified support name (" << SupportName<< ") not recognised - EOS not created!" << endreq;
    return;
  }

  double halfEosThick = eosHalfThickness( (*bl)->type(), DetTypeDyn::Endcap);

  ostringstream eosName;
  eosName<< (**bl).name() + "EOS";
  if(iLayer>-1) eosName <<iLayer;

  ServiceDynVolume* eosVol = new ServiceDynVolume( ServiceDynVolume::Disk, rEosMin, rEosMax,
						   (*bl)->zPos()-halfEosThick+ EOSZOffset,
						   (*bl)->zPos()+halfEosThick+ EOSZOffset,
						   eosName.str());
  eosVol->addLayer(*bl);
  addVolume( eosVol);
  eosVol->addEosServices(*bl); // add all the stave EOS stuff

  // 2. Connect to route
  double zMin, zMax;
  if (route.volumes().empty()) {
    zMin = (*bl)->zPos() + EOSZOffset;
    if ( zMin - route.zMin() > c_EosTolerance) { // FIXME use specific tolerance
      ServiceDynVolume* beg = new ServiceDynVolume( ServiceDynVolume::Cylinder,  
						    route.radius()-0.5*c_ServiceCylinderThickness,
						    route.radius()+0.5*c_ServiceCylinderThickness,
						    route.zMin(), zMin - c_safetyGap,
						    nextVolumeName(route));
      route.addVolume( beg); // beg has no services at this time
      addVolume( beg);
    }
  }
  else zMin = route.volumes().back()->zMax() + c_safetyGap;

  // Assume no further elements to link on the route then find next potential link 
  // Make sure next one is on the same support tube, and on the same side (inside or outside)
  zMax = route.zExit();

  for (LayerContainer::const_iterator blnext = bl+1; blnext != blend; ++blnext)
  {
    if ( m_svcOtherXMLHelper->EndcapDiscRoute((*blnext)->number()) == SupportName &&
	 isRoutedOutsideSupportTube(blnext, route) == bRoutedOutsideSupport ) 
    {
       zMax = std::min((**blnext).zPos() + EOSZOffset, route.zExit());
       break;
    }
  }

//   LayerContainer::const_iterator blnext = bl+1;
//   if (blnext != blend) {
//     zMax = std::min((**blnext).zPos(), route.zExit());
//   }
//   else zMax = route.zExit();

  bool reverse = false;
  //  if (route.zExit() < (**bl).zPos()) {
  if (route.zExit() < zMin) {
    // create a dedicated exit volume with zero length
    ServiceDynVolume* exitVol = new ServiceDynVolume( ServiceDynVolume::Cylinder,  
						route.radius()-0.5*c_ServiceCylinderThickness,
						route.radius()+0.5*c_ServiceCylinderThickness,
						route.zExit(), route.zExit(),
						route.name()+"ExitVol");
    // connect the last volume to the exit
    connect( route.volumes().back(), exitVol);
    route.setExitVolume( exitVol);
    zMin = max( route.zExit(), route.volumes().back()->zMax());
    zMax = (**bl).zPos();
    reverse = true;
  }

  ServiceDynVolume* newCyl = new ServiceDynVolume( ServiceDynVolume::Cylinder,  
					     route.radius()-0.5*c_ServiceCylinderThickness,
					     route.radius()+0.5*c_ServiceCylinderThickness,
					     zMin, zMax, nextVolumeName(route));
  newCyl->dump();
  connect( eosVol, newCyl);
  if (! reverse) {
    if (!route.volumes().empty()) connect( route.volumes().back(), newCyl);
  }
  else {
    // FIXME: the services of the new layer will not propagate thru all volumes
    if (!route.volumes().empty()) connect( newCyl, route.volumes().back());
  }
  route.addVolume( newCyl);
  addVolume( newCyl);

}

void RoutingDynAuto::connect( ServiceDynVolume* prev, ServiceDynVolume* newv)
{
  // checks on geometrical compatibility should be done by the caller
  newv->addPrevious(prev);
  prev->setNext(newv);
  newv->addLayers(prev->layers());
}

void RoutingDynAuto::connectRoutes( SvcRoute& in, SvcRoute& out)
{
  // choose volume to connect to
  ServiceDynVolume* entryVol = out.entryVolume(in.position(),true,msgStream());
  if (entryVol == 0) entryVol = createSingleRouteVolume( out);
  ServiceDynVolume* exitVol = in.exitVolume(true,msgStream());
  // maybe check volumes are connectable?
  entryVol->addPrevious(exitVol);
  exitVol->setNext(entryVol);

  // add the services of the in route to all volumes of out route, up to it's exit
  ServiceDynVolume* nextVol = entryVol;
  while (true) {
    nextVol->addLayers( exitVol->layers());
    nextVol = nextVol->next();
    if (nextVol == 0) break;
  }
}

ServiceDynVolume* RoutingDynAuto::createSingleRouteVolume( SvcRoute& rt)
{
  ServiceDynVolume* vol(0);
  HSvcRoute* hrt = dynamic_cast<HSvcRoute*>(&rt);
  if (hrt != 0) {
    vol = new ServiceDynVolume( ServiceDynVolume::Cylinder,  
			     rt.position()-0.5*c_ServiceCylinderThickness,
			     rt.position()+0.5*c_ServiceCylinderThickness,
			     hrt->zMin(), hrt->zMax(), rt.name());
  }
  else {
    VSvcRoute* vrt = dynamic_cast<VSvcRoute*>(&rt);
    if(vrt) vol = new ServiceDynVolume( ServiceDynVolume::Disk, vrt->rMin(), vrt->rMax(),
				     rt.position()-0.5*c_ServiceDiskThickness,
				     rt.position()+0.5*c_ServiceDiskThickness, rt.name());
  }
  rt.addVolume( vol);
  addVolume(vol);
  return vol;
}

double RoutingDynAuto::eosTolerance( DetTypeDyn::Type /*type*/, DetTypeDyn::Part /*part*/) 
{
  return c_EosTolerance;
}

double RoutingDynAuto::eosLength( DetTypeDyn::Type type, DetTypeDyn::Part part)
{
  using namespace DetTypeDyn;
  if (type == Pixel) {
    if (part == Barrel) return c_bpEosLength;
    else                return c_epEosLength;
  }
  else  {
    return c_bsEosLength; 
    // FIXME add part for endcap strip
  }
}

  
double RoutingDynAuto::eosHalfThickness( DetTypeDyn::Type /*type*/, DetTypeDyn::Part /*part*/)
{
  return c_halfEosThick;
}

void RoutingDynAuto::dumpRoute( const SvcRoute& route) 
{
  using namespace std;
  msg(MSG::INFO)<< "Dumping route at pos " << route.position() 
		<< " with exit at " << route.exit() << endreq;
  for ( SvcRoute::VolumeContainer::const_iterator iv = route.volumes().begin(); 
	iv != route.volumes().end(); ++iv) {
    (**iv).dump(false);
  }
}

std::string RoutingDynAuto::nextVolumeName( const SvcRoute& route) const
{
  ostringstream os;
  os << route.volumes().size();
  return route.name() + "Vol" + os.str();
}
