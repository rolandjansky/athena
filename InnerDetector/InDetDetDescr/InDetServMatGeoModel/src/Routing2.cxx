/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/Routing2.h"

#include "InDetServMatGeoModel/ServicesLayer.h"
#include "InDetServMatGeoModel/InDetServMatGeometryManager.h"

#include <algorithm>

#include <iostream>
using namespace std; // for DEBUG

Routing2::Routing2(const Athena::MsgStreamMember& msg):
  m_msg(msg)
{
  m_routePixelBarrelOnPST = false;
  m_pixelAlongBarrelStrip = false;
  m_c_nInnerPixelLayers = 2;

  m_c_bpEosLength = 40; // mm
  m_c_epEosLength = 30;
  m_c_bsEosLength = 50;
  m_c_safetyGap = 0.001;

  // all units in cm
  m_c_EosTolerance = 1.; // mm
  m_c_halfEosThick = 3;
  m_c_EosTolerance2 = 0.1;
  m_c_ServiceDiskThickness = 10;
  m_c_ServiceCylinderThickness = 6;
  m_c_LayerLengthTolerance = 1;

  m_ISTexists = false;

}

void Routing2::createRoutingVolumes(ServicesTracker& tracker)
{
  m_ISTexists = (tracker.geoMgr()->SupportTubeRMin("IST") > 0.1); 

  createRoutes(tracker);

  routeOuterBarrelPixel(tracker);
  routeEndcapPixel( tracker);

  // Link routes
  connectRoutes( m_bpVRoute, m_epHRoute);
  if (!m_pixelAlongBarrelStrip && !m_ISTexists) {
    connectRoutes( m_epHRoute, m_pixelV2Route);
    connectRoutes( m_pixelV2Route, m_pixelH2Route);
  }

  // Create Strip layer routes
  routeBarrelStrip( tracker);
  if (m_pixelAlongBarrelStrip) connectRoutes( m_epHRoute, m_bsVRoute);
  
  // Create pixel layer routes
  routeInnerBarrelPixel(tracker);
  connectRoutes( m_istVRoute, m_istHRoute);

  // Output route details in debug mode
  if(msgLvl(MSG::DEBUG)){
    dumpRoute( m_bpVRoute);
    dumpRoute( m_epHRoute);
    dumpRoute( m_bsVRoute);
    dumpRoute( m_istVRoute);
    dumpRoute( m_istHRoute);
    if (!m_pixelAlongBarrelStrip && !m_ISTexists) {
      dumpRoute( m_pixelV2Route);
      dumpRoute( m_pixelH2Route);
    }
  }
  tracker.setServiceVolumes(m_volumes);
}

void Routing2::createRoutes(ServicesTracker& tracker) 
{
  // barrel pixel vertical route
  const LayerContainer& bplc = tracker.barrelPixelLayers();
  const LayerContainer& eplc = tracker.endcapPixelLayers();
  const LayerContainer& bslc = tracker.barrelStripLayers();

  double bpZmax = 0;
  for (LayerContainer::const_iterator bl=bplc.begin()+m_c_nInnerPixelLayers; bl!=bplc.end(); ++bl)
    bpZmax = std::max( bpZmax, (**bl).zPos() + (**bl).halfLength());

  double bpVertRouteZpos = bpZmax+m_c_EosTolerance+m_c_bpEosLength +  0.5*m_c_ServiceDiskThickness + m_c_safetyGap;
  double bpVertRouteRmin = bplc[m_c_nInnerPixelLayers]->radius(); // change if along PST

  // Find max endcap pixel layer radius
  double epRmax = 0;
  for (LayerContainer::const_iterator i=eplc.begin(); i!=eplc.end(); ++i) 
    epRmax = std::max( epRmax, (**i).rMax());

  // Find max radius for vertical route
  double bpVertRouteRmax;
  if (!m_ISTexists) {
    bpVertRouteRmax = 0.5*(epRmax + m_c_epEosLength + tracker.geoMgr()->sctInnerSupport());
    // Pixel horizontal route in the middle between pixel disks and first strip layer
    if ( epRmax + m_c_epEosLength + m_c_ServiceCylinderThickness > tracker.geoMgr()->sctInnerSupport())
      msg(MSG::WARNING)<< " No space for services between pixel diskd and sct support" << endmsg;
  }
  else {
    // services along PST, as close as they can get
    bpVertRouteRmax = tracker.geoMgr()->SupportTubeRMin("PST") - m_c_safetyGap - 0.5*m_c_ServiceCylinderThickness;
  }

  double bpHorRouteR = bpVertRouteRmax;
  double bpHRouteZmin = bpVertRouteZpos +  0.5*m_c_ServiceDiskThickness + m_c_safetyGap;
  double bpHRouteZmax = eplc.back()->zPos(); // prolong if along PST ?
  msg(MSG::INFO) << "Route2: setting bpHRouteZmax to " << bpHRouteZmax << endmsg;

  /// Assume same length barrel, the loop is to make sure there are no volume overlaps
  /// in case strip barrel layers are slightly different
  double bsZmax = 0;
  for (LayerContainer::const_iterator i=bslc.begin(); i!=bslc.end(); ++i)
    bsZmax = std::max( bsZmax, (**i).zPos() + (**i).halfLength());

  double bsVertRouteZpos = bsZmax+m_c_EosTolerance+m_c_bsEosLength + 0.5*m_c_ServiceDiskThickness + m_c_safetyGap;

  bpHRouteZmax = tracker.geoMgr()->SupportTubeZMax("PST");
  
  double bpHRouteZmax_mode = tracker.geoMgr()->SupportTubeZMin("mode");

  bool bMSTI = tracker.geoMgr()->SupportTubeExists("MSTI");
  bool bMSTM = tracker.geoMgr()->SupportTubeExists("MSTM");
  bool bMSTO = tracker.geoMgr()->SupportTubeExists("MSTO");
  if(bMSTI||bMSTM||bMSTO)
    if(bpHRouteZmax_mode>0.1&&bpHRouteZmax_mode<bpHRouteZmax) bpHRouteZmax = bpHRouteZmax_mode-0.001;

  msg(MSG::INFO)<< "Changing bpHRouteZmax to " << bpHRouteZmax << endmsg;

  double bsVertRouteRmin = bpHorRouteR + 0.5*m_c_ServiceCylinderThickness + m_c_safetyGap;
  double bsVertRouteRmax = bslc.back()->radius() + m_c_ServiceCylinderThickness; // approx

  m_bpVRoute = VRoute( bpVertRouteZpos, bpVertRouteRmin, bpVertRouteRmax, bpVertRouteRmax, "OuterPixelRPath");

  if (m_pixelAlongBarrelStrip)
    m_bpHRoute = HRoute( bpHorRouteR, bpHRouteZmin, bpHRouteZmax, bsVertRouteZpos,"OuterPixelZPath"); // different if along PST
  else 
    m_bpHRoute = HRoute( bpHorRouteR, bpHRouteZmin, bpHRouteZmax, bpHRouteZmax,"OuterPixelZPath");

  m_epHRoute = m_bpHRoute;  // different if along PST
  m_bsVRoute = VRoute( bsVertRouteZpos, bsVertRouteRmin, bsVertRouteRmax, bsVertRouteRmax,"BarrelStripRPath");

  m_bpVRoute.setNextRoute(&m_bpHRoute);
  if (m_pixelAlongBarrelStrip) m_bpHRoute.setNextRoute(&m_bsVRoute);

 // Additional Routes:
  // Outside MSTOuter
  double bpMSTO_R =  tracker.geoMgr()->SupportTubeRMax("MSTO") + m_c_safetyGap + 0.5*m_c_ServiceCylinderThickness;
  m_MSTO_HRoute = HRoute( bpMSTO_R, bpHRouteZmin, bpHRouteZmax, bpHRouteZmax, "MSTOPixelZPath"); 

  // Outside MSTMiddle
  double bpMSTM_R =  tracker.geoMgr()->SupportTubeRMax("MSTM") + m_c_safetyGap + 0.5*m_c_ServiceCylinderThickness;
  m_MSTM_HRoute = HRoute( bpMSTM_R, bpHRouteZmin, bpHRouteZmax, bpHRouteZmax, "MSTMPixelZPath"); 

  // Outside MSTInner
  double bpMSTI_R =  tracker.geoMgr()->SupportTubeRMax("MSTI") + m_c_safetyGap + 0.5*m_c_ServiceCylinderThickness;
  m_MSTI_HRoute = HRoute( bpMSTI_R, bpHRouteZmin, bpHRouteZmax, bpHRouteZmax, "MSTIPixelZPath"); 

  // Inside MSTMiddle
  double bpMSTM_RI =  tracker.geoMgr()->SupportTubeRMin("MSTM") - m_c_safetyGap - 0.5*m_c_ServiceCylinderThickness;
  m_MSTM_HRouteInner = HRoute( bpMSTM_RI, bpHRouteZmin, bpHRouteZmax, bpHRouteZmax, "MSTMPixelZPathInner"); 

  // Exit route for pixel
  if (!m_pixelAlongBarrelStrip) createOuterPixelRoutes(tracker);

  // route inside PST
  createRoutesInIST(tracker);
}

void Routing2::createOuterPixelRoutes(ServicesTracker& tracker) 
{
  if (!m_ISTexists) { // PST is actually IST, and there is no PST
    // we need to reduce the route radius after the pixel disks to avoid conflict with SCT disks
    double routeRadius = tracker.geoMgr()->pixelEnvelopeRMax() - m_c_safetyGap - m_c_ServiceCylinderThickness/2;
    //double zpos = 0.5 * (tracker.endcapPixelLayers().back()->zPos() + tracker.endcapStripLayers().front()->zPos());
    double zpos = m_epHRoute.zMax() + m_c_ServiceDiskThickness/2 + m_c_safetyGap;

    m_pixelV2Route = VRoute( zpos, routeRadius, m_bpHRoute.radius(), routeRadius, "OuterPixelRPath2");

    double pstLen = tracker.geoMgr()->SupportTubeZMax("PST");
    m_pixelH2Route = HRoute( routeRadius, zpos + m_c_ServiceDiskThickness/2 + m_c_safetyGap, pstLen, pstLen,"OuterPixelZPath2");
    m_epHRoute.setNextRoute( &m_pixelV2Route);
    m_pixelV2Route.setNextRoute( &m_pixelH2Route);
  }
}

void Routing2::createRoutesInIST(ServicesTracker& tracker) 
{
  const LayerContainer& bplc = tracker.barrelPixelLayers();
  double bpZmax = 0;
  for (LayerContainer::const_iterator bl=bplc.begin(); bl!=bplc.begin()+m_c_nInnerPixelLayers; ++bl) {
    bpZmax = std::max( bpZmax, (**bl).zPos() + (**bl).halfLength());
  }

  double istVRouteZpos = bpZmax+m_c_EosTolerance+m_c_bpEosLength +
    0.5*m_c_ServiceDiskThickness + m_c_safetyGap;
  double istVRouteRmin = bplc.front()->radius();

  double istRmin;
  double istZmax;
  if (m_ISTexists) { 
    // use IST if it exists
   // use IST if it exists
    istRmin = tracker.geoMgr()->SupportTubeRMin("IST");
    istZmax = tracker.geoMgr()->SupportTubeZMax("IST");
  }
  else { 
    // use PST as IST for backward compatibility
   // use PST as IST for backward compatibility
    istRmin = tracker.geoMgr()->SupportTubeRMin("PST");
    istZmax = tracker.geoMgr()->SupportTubeZMax("PST");
  }

  double istVRouteRmax = istRmin - 0.5*m_c_ServiceCylinderThickness - m_c_safetyGap;
  m_istVRoute = VRoute( istVRouteZpos, istVRouteRmin, istVRouteRmax, istVRouteRmax, "InnerPixelRPath");

  double istHRouteZmin = istVRouteZpos + 0.5*m_c_ServiceDiskThickness + m_c_safetyGap;
  double istHRouteZmax = istZmax;
  m_istHRoute = HRoute( istVRouteRmax, istHRouteZmin, istHRouteZmax, istHRouteZmax, "InnerPixelZPath");
  m_istVRoute.setNextRoute(&m_istHRoute);
}

void Routing2::routeOuterBarrelPixel(ServicesTracker& tracker)
{

  const LayerContainer& bls = tracker.barrelPixelLayers();
  for (LayerContainer::const_iterator bl=bls.begin()+m_c_nInnerPixelLayers; bl!=bls.end(); ++bl) {
    routeBarrelLayer( bl, bls.end(), tracker, m_bpVRoute);
  }
}

void Routing2::routeBarrelStrip(ServicesTracker& tracker)
{
  const LayerContainer& bls = tracker.barrelStripLayers();
  for (LayerContainer::const_iterator bl=bls.begin(); bl!=bls.end(); ++bl) {
    routeBarrelLayer( bl, bls.end(), tracker, m_bsVRoute);
  }
}

void Routing2::routeEndcapPixel(ServicesTracker& tracker)
{

//   const LayerContainer& lc = tracker.endcapPixelLayers();
//   for (LayerContainer::const_iterator bl=lc.begin(); bl!=lc.end(); ++bl) {
//     routeEndcapLayer( bl, lc.end(), tracker, m_epHRoute);
//   }

  const LayerContainer& lc = tracker.endcapPixelLayers();
  bool bMSTI = tracker.geoMgr()->SupportTubeExists("MSTI");
  bool bMSTM = tracker.geoMgr()->SupportTubeExists("MSTM");
  bool bMSTO = tracker.geoMgr()->SupportTubeExists("MSTO");

  for (LayerContainer::const_iterator bl=lc.begin(); bl!=lc.end(); ++bl)
  {

    if (bMSTI)
    {
      if (tracker.geoMgr()->pixelDiskServiceRoute( (*bl)->number() )  == "MSTI")
	routeEndcapLayer( bl, lc.end(), tracker, m_MSTI_HRoute);
    }

    // MSTM can have service on inside or outside
    if (bMSTM)
    {
      if (tracker.geoMgr()->pixelDiskServiceRoute( (*bl)->number() )  == "MSTM")
      {
	if (isRoutedOutsideSupportTube(bl, m_MSTM_HRoute))
	  routeEndcapLayer( bl, lc.end(), tracker, m_MSTM_HRoute);
	else
	  routeEndcapLayer( bl, lc.end(), tracker, m_MSTM_HRouteInner);
      }
    }

    if (bMSTO)
    {
      if (tracker.geoMgr()->pixelDiskServiceRoute( (*bl)->number() )  == "MSTO")
	routeEndcapLayer( bl, lc.end(), tracker, m_MSTO_HRoute);
    }
  }

  if(!bMSTI&&!bMSTM&&!bMSTO)
   for (LayerContainer::const_iterator bl=lc.begin(); bl!=lc.end(); ++bl) 
     routeEndcapLayer( bl, lc.end(), tracker, m_epHRoute);

}


bool Routing2::isRoutedOutsideSupportTube(LayerContainer::const_iterator bl, HRoute& route)
{
  if ( (**bl).rMax() > route.radius())  return true;
  return false;
}

void Routing2::routeInnerBarrelPixel(ServicesTracker& tracker)
{
  const LayerContainer& bls = tracker.barrelPixelLayers();
  for (LayerContainer::const_iterator bl=bls.begin(); bl!=bls.begin()+m_c_nInnerPixelLayers; ++bl) {
    routeBarrelLayer( bl, bls.begin()+m_c_nInnerPixelLayers, tracker, m_istVRoute);
  }
}

void Routing2::routeBarrelLayer(LayerContainer::const_iterator bl,
				LayerContainer::const_iterator blend, 
				ServicesTracker& /*tracker*/, VRoute& route)
{
  // 1. construct end-of-stave volume 
  double zEosMin = (*bl)->halfLength() + eosTolerance( (*bl)->type(), DetType::Barrel);
  double zEosMax = zEosMin + eosLength( (*bl)->type(), DetType::Barrel);

  if (zEosMax < route.position() - 0.5*m_c_ServiceDiskThickness) {
    zEosMax = route.position() - 0.5*m_c_ServiceDiskThickness - m_c_safetyGap;
  }
  else {
    msg(MSG::WARNING) << "not enough space for end of stave of barrel layer at radius "
		      << (**bl).radius() << endmsg;
  }
  double halfEosThick = eosHalfThickness( (*bl)->type(), DetType::Barrel);
  ServiceVolume* eosCylinder = new ServiceVolume( ServiceVolume::Cylinder,
						  (*bl)->radius()-halfEosThick,
						  (*bl)->radius()+halfEosThick,
						  zEosMin, zEosMax, (**bl).name() + "EOS");
  eosCylinder->addLayer(*bl);
  addVolume( eosCylinder);
  eosCylinder->addEosServices(*bl); // add all the stave EOS stuff

  // 2. Connect to route
  double rMin, rMax;
  if (route.volumes().empty()) rMin = route.rMin();
  else rMin = route.volumes().back()->rMax() + m_c_safetyGap;

  LayerContainer::const_iterator blnext = bl+1;
  if (blnext != blend) {
    rMax = std::min((**blnext).radius(), route.rExit());
  }
  else rMax = route.rExit();

  ServiceVolume* newDisk = new ServiceVolume( ServiceVolume::Disk, rMin, rMax, 
					      route.zPos()-0.5*m_c_ServiceDiskThickness,
					      route.zPos()+0.5*m_c_ServiceDiskThickness,
					      nextVolumeName(route));
  newDisk->dump(true);
  // newDisk->addLayer(*bl); // done by connect()
  connect( eosCylinder, newDisk);
  if (!route.volumes().empty()) connect( route.volumes().back(), newDisk);
  route.addVolume( newDisk);
  addVolume( newDisk);
}

void Routing2::routeEndcapLayer(LayerContainer::const_iterator bl,
				LayerContainer::const_iterator blend, 
				ServicesTracker& tracker, HRoute& route)
{
  // 1. construct end-of-stave volume 
//   double rEosMin = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetType::Endcap);
//   double rEosMax = rEosMin + eosLength( (*bl)->type(), DetType::Endcap);
//   if (rEosMax > route.position() - 0.5*m_c_ServiceCylinderThickness) {
//     msg(MSG::WARNING) << "not enough space for end of stave of endcap layer at Z = "
// 		      << (**bl).zPos() << endmsg;
//   }
//   rEosMax = route.position() - 0.5*m_c_ServiceCylinderThickness - m_c_safetyGap;
//   if (rEosMax < rEosMin) {
//     msg(MSG::WARNING) << "no space for routing of endcap layer at Z = "
// 		      << (**bl).zPos() << endmsg;
//   }

  double rEosMin = 0.0;
  double rEosMax = 0.0;
  std::string SupportName =  tracker.geoMgr()->pixelDiskServiceRoute( (*bl)->number() );
  double EOSZOffset = 0.0;
  
  bool bRoutedOutsideSupport = isRoutedOutsideSupportTube(bl, route);

  if (SupportName == "PST")
  {
    rEosMin = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetType::Endcap);   // Disk outer edge + safety
    rEosMax = route.position() - 0.5*m_c_ServiceCylinderThickness - m_c_safetyGap; // support tube - safety
    EOSZOffset = tracker.geoMgr()->pixelDiskEOSZOffset( (*bl)->number() );
    if (rEosMax < rEosMin) msg(MSG::WARNING) << "No space for routing of endcap layer at Z = " << (**bl).zPos() << endmsg;
  }
  else if (SupportName == "MST")
  {
    rEosMin = (*bl)->rMin() + eosTolerance( (*bl)->type(), DetType::Endcap); // EOS same size as Disk supports
    rEosMax = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetType::Endcap);
    EOSZOffset = tracker.geoMgr()->pixelDiskEOSZOffset( (*bl)->number() );
  }
  else if (SupportName == "IST")
  {
    rEosMin = route.position() + 0.5*m_c_ServiceCylinderThickness + m_c_safetyGap; // support tube + safety
    rEosMax = (*bl)->rMin() - eosTolerance( (*bl)->type(), DetType::Endcap);   // Disk outer edge - safety
    EOSZOffset = tracker.geoMgr()->pixelDiskEOSZOffset( (*bl)->number() );
    if (rEosMax < rEosMin) msg(MSG::WARNING) << "No space for routing of endcap layer at Z = " << (**bl).zPos() << endmsg;
  }
  else if (SupportName == "MSTO" || SupportName == "MSTM" || SupportName == "MSTI" )
  {
    // rEosMin/Max are different if disk runs inside or outside the support tube
    if (bRoutedOutsideSupport)
    {
      rEosMin = route.position() + 0.5*m_c_ServiceCylinderThickness + m_c_safetyGap;
      rEosMax = (*bl)->rMax() - eosTolerance( (*bl)->type(), DetType::Endcap);
    }
    else
    {
       rEosMax = route.position() - 0.5*m_c_ServiceCylinderThickness - m_c_safetyGap;
       rEosMin = (**bl).rMin() - eosTolerance( (**bl).type(), DetType::Endcap);
    }
    EOSZOffset = tracker.geoMgr()->pixelDiskEOSZOffset( (*bl)->number() );
  }
  else if (SupportName=="StdRoute")
    {
      rEosMin = (*bl)->rMax() + eosTolerance( (*bl)->type(), DetType::Endcap);
      rEosMax = rEosMin + eosLength( (*bl)->type(), DetType::Endcap);
      EOSZOffset = 0;
      if (rEosMax > route.position() - 0.5*m_c_ServiceCylinderThickness) {
	msg(MSG::WARNING) << "not enough space for end of stave of endcap layer at Z = "
			  << (**bl).zPos() << endmsg;
      }
      rEosMax = route.position() - 0.5*m_c_ServiceCylinderThickness - m_c_safetyGap;
      if (rEosMax < rEosMin) {
	msg(MSG::WARNING) << "no space for routing of endcap layer at Z = "
			  << (**bl).zPos() << endmsg;
      }
    }
  else
  {
    msg(MSG::ERROR) << "Specified support name (" << SupportName<< ") not recognised - EOS not created!" << endmsg;
    return;
  }

  double halfEosThick = eosHalfThickness( (*bl)->type(), DetType::Endcap);
  ServiceVolume* eosVol = new ServiceVolume( ServiceVolume::Disk, rEosMin, rEosMax,
					     (*bl)->zPos()-halfEosThick+ EOSZOffset,
					     (*bl)->zPos()+halfEosThick+ EOSZOffset,
					     (**bl).name() + "EOS");
  eosVol->addLayer(*bl);
  addVolume( eosVol);
  eosVol->addEosServices(*bl); // add all the stave EOS stuff

  // 2. Connect to route
  double zMin, zMax;
  if (route.volumes().empty()) {
    zMin = (*bl)->zPos() + EOSZOffset;
    if ( zMin - route.zMin() > m_c_EosTolerance) { // FIXME use specific tolerance
      ServiceVolume* beg = new ServiceVolume( ServiceVolume::Cylinder,  
					      route.radius()-0.5*m_c_ServiceCylinderThickness,
					      route.radius()+0.5*m_c_ServiceCylinderThickness,
					      route.zMin(), zMin - m_c_safetyGap,
					      nextVolumeName(route));
      route.addVolume( beg); // beg has no services at this time
      addVolume( beg);
    }
  }
  else zMin = route.volumes().back()->zMax() + m_c_safetyGap;

  // Assume no further elements to link on the route then find next potential link 
  // Make sure next one is on the same support tube, and on the same side (inside or outside)
  zMax = route.zExit();
  for (LayerContainer::const_iterator blnext = bl+1; blnext != blend; ++blnext)
  {
    if ( tracker.geoMgr()->pixelDiskServiceRoute((*blnext)->number()) == SupportName     &&   isRoutedOutsideSupportTube(blnext, route) == bRoutedOutsideSupport ) 
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
    ServiceVolume* exitVol = new ServiceVolume( ServiceVolume::Cylinder,  
						route.radius()-0.5*m_c_ServiceCylinderThickness,
						route.radius()+0.5*m_c_ServiceCylinderThickness,
						route.zExit(), route.zExit(),
						route.name()+"ExitVol");
    // connect the last volume to the exit
    connect( route.volumes().back(), exitVol);
    route.setExitVolume( exitVol);
    zMin = max( route.zExit(), route.volumes().back()->zMax());
    zMax = (**bl).zPos();
    reverse = true;
  }

  ServiceVolume* newCyl = new ServiceVolume( ServiceVolume::Cylinder,  
					     route.radius()-0.5*m_c_ServiceCylinderThickness,
					     route.radius()+0.5*m_c_ServiceCylinderThickness,
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

void Routing2::connect( ServiceVolume* prev, ServiceVolume* newv)
{
  // checks on geometrical compatibility should be done by the caller
  newv->addPrevious(prev);
  prev->setNext(newv);
  newv->addLayers(prev->layers());
}

void Routing2::connectRoutes( Route& in, Route& out)
{
  // choose volume to connect to
  ServiceVolume* entryVol = out.entryVolume(in.position(),true,msgStream());
  if (entryVol == 0) entryVol = createSingleRouteVolume( out);
  ServiceVolume* exitVol = in.exitVolume(true,msgStream());
  // maybe check volumes are connectable?
  entryVol->addPrevious(exitVol);
  exitVol->setNext(entryVol);

  // add the services of the in route to all volumes of out route, up to it's exit
  ServiceVolume* nextVol = entryVol;
  while (true) {
    nextVol->addLayers( exitVol->layers());
    nextVol = nextVol->next();
    if (nextVol == 0) break;
  }
}

ServiceVolume* Routing2::createSingleRouteVolume( Route& rt)
{
  ServiceVolume* vol(0);
  HRoute* hrt = dynamic_cast<HRoute*>(&rt);
  if (hrt != 0) {
    vol = new ServiceVolume( ServiceVolume::Cylinder,  
			     rt.position()-0.5*m_c_ServiceCylinderThickness,
			     rt.position()+0.5*m_c_ServiceCylinderThickness,
			     hrt->zMin(), hrt->zMax(), rt.name());
  }
  else {
    VRoute* vrt = dynamic_cast<VRoute*>(&rt);
    if(vrt) vol = new ServiceVolume( ServiceVolume::Disk, vrt->rMin(), vrt->rMax(),
				     rt.position()-0.5*m_c_ServiceDiskThickness,
				     rt.position()+0.5*m_c_ServiceDiskThickness, rt.name());
  }
  rt.addVolume( vol);
  addVolume(vol);
  return vol;
}

double Routing2::eosTolerance( DetType::Type /*type*/, DetType::Part /*part*/) 
{
  return m_c_EosTolerance;
}

double Routing2::eosLength( DetType::Type type, DetType::Part part)
{
  using namespace DetType;
  if (type == Pixel) {
    if (part == Barrel) return m_c_bpEosLength;
    else                return m_c_epEosLength;
  }
  else  {
    return m_c_bsEosLength; 
    // FIXME add part for endcap strip
  }
}

  
double Routing2::eosHalfThickness( DetType::Type /*type*/, DetType::Part /*part*/)
{
  return m_c_halfEosThick;
}

void Routing2::dumpRoute( const Route& route) 
{
  using namespace std;
  msg(MSG::INFO)<< "Dumping route at pos " << route.position() 
		<< " with exit at " << route.exit() << endmsg;
  for ( Route::VolumeContainer::const_iterator iv = route.volumes().begin(); 
	iv != route.volumes().end(); ++iv) {
    (**iv).dump(false);
  }
}

std::string Routing2::nextVolumeName( const Route& route) const
{
  ostringstream os;
  os << route.volumes().size();
  return route.name() + "Vol" + os.str();
}
