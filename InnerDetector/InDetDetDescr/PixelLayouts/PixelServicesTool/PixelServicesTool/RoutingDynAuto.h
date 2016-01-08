/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RoutingDynAuto_H
#define RoutingDynAuto_H

#include "PixelServicesTool/ServicesDynTracker.h"
#include "PixelServicesTool/VSvcRoute.h"
#include "PixelServicesTool/HSvcRoute.h"

#include "PixelServicesTool/DetTypeDyn.h"

#include "AthenaKernel/MsgStreamMember.h"
#include <vector>

class PixelSimpleServiceXMLHelper;
class PixelGeneralXMLHelper;
class PixelDynamicServiceXMLHelper;

class RoutingDynAuto {
public:

  typedef ServicesDynTracker::LayerContainer    LayerContainer;

  RoutingDynAuto(const Athena::MsgStreamMember&);

  void createRoutingVolumes(ServicesDynTracker& tracker);

  const std::vector<ServiceDynVolume*>& volumes() const {return m_volumes;}

private:

  bool m_routePixelBarrelOnPST;
  bool m_pixelAlongBarrelStrip;
  bool m_ISTexists;

  bool m_routeBarrel;
  bool m_routeEndcap;

  int c_nInnerPixelLayers;  // FIXME: should come from tracker geometry

  double c_bpEosLength;
  double c_epEosLength;
  double c_bsEosLength;
  double c_safetyGap;

  double c_EosTolerance;
  double c_halfEosThick;
  double c_EosTolerance2;
  double c_ServiceDiskThickness;
  double c_ServiceCylinderThickness; //!
  double c_LayerLengthTolerance;

  std::vector<ServiceDynVolume*> m_volumes;
  ServicesDynLayer* previousBarrelLayer;

  VSvcRoute m_bpVSvcRoute;
  HSvcRoute m_bpHSvcRoute;
  HSvcRoute m_epHSvcRoute;

  VSvcRoute m_pixelV2Route;
  HSvcRoute m_pixelH2Route;

  VSvcRoute m_bsVSvcRoute;
  VSvcRoute m_istVSvcRoute;
  HSvcRoute m_istHSvcRoute;

  HSvcRoute  m_ISTouter_HSvcRoute; // Route running on outside of IST
  HSvcRoute  m_MSTinside_HSvcRoute; // Route running on outside of IST

  HSvcRoute  m_MSTO_HSvcRoute;
  HSvcRoute  m_MSTM_HSvcRoute;
  HSvcRoute  m_MSTM_HSvcRouteInner;
  HSvcRoute  m_MSTI_HSvcRoute;
  std::vector<HSvcRoute> additionalRoutes;

  double eosTolerance( DetTypeDyn::Type type, DetTypeDyn::Part part);
  double eosLength( DetTypeDyn::Type type, DetTypeDyn::Part part);
  double eosHalfThickness( DetTypeDyn::Type type, DetTypeDyn::Part part);

  void connect( ServiceDynVolume* prev, ServiceDynVolume* newv);
  void routeBarrelLayer(LayerContainer::const_iterator bl, LayerContainer::const_iterator blend, ServicesDynTracker& tracker, VSvcRoute& route);
  void routeEndcapLayer(LayerContainer::const_iterator bl, LayerContainer::const_iterator blend, ServicesDynTracker& tracker, HSvcRoute& route, int iLayer=-1);

  bool isRoutedOutsideSupportTube(LayerContainer::const_iterator bl, HSvcRoute& route);
  void routeOuterBarrelPixel(ServicesDynTracker& tracker);
  void routeEndcapPixel(ServicesDynTracker& tracker);
  void routeBarrelStrip(ServicesDynTracker& tracker);
  void routeInnerBarrelPixel(ServicesDynTracker& tracker);

  void createRoutes(ServicesDynTracker& tracker);
  void createRoutesInIST(ServicesDynTracker& tracker);
  void createOuterPixelRoutes(ServicesDynTracker& tracker);

  void addVolume( ServiceDynVolume* v) { m_volumes.push_back( v);}

  ServiceDynVolume* createSingleRouteVolume( SvcRoute& rt);

  void connectRoutes( SvcRoute& in, SvcRoute& out);
  void dumpRoute( const SvcRoute& route);
  std::string nextVolumeName( const SvcRoute& route) const;

  // the message stream (same for all derived classes)
  const Athena::MsgStreamMember m_msg;
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  const Athena::MsgStreamMember& msgStream() const {return m_msg;}
  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

  PixelSimpleServiceXMLHelper* m_simpleSrvXMLHelper;
  PixelGeneralXMLHelper* m_genXMLHelper;
  PixelDynamicServiceXMLHelper* m_svcOtherXMLHelper;
};

#endif
