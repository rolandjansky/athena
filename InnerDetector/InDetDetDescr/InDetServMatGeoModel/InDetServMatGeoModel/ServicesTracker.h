/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServicesTracker_H
#define ServicesTracker_H

//#include "InDetServMatGeoModel/ServicesLayer.h"
#include "InDetServMatGeoModel/DetType.h"
#include "InDetServMatGeoModel/ServiceMaterial.h"
#include "AthenaKernel/MsgStreamMember.h"

#include <vector>
#include <map>
#include <string>

class ServiceVolume;
class ServicesLayer;
class InDetServMatGeometryManager;

class ServicesTracker {
public:

  /// Construct an empty tracker, to be filled by Builder
  ServicesTracker(const Athena::MsgStreamMember&);
  ~ServicesTracker();

  // Construct somehow (for testing)
  //ServicesTracker(bool toy);

  typedef std::vector<ServicesLayer*>  LayerContainer;

  void computeServicesPerLayer();

  void finaliseServices();

  // barrel layers are sorted in radius from inside out
  const LayerContainer& barrelLayers() const {return m_barrelLayers;}
  LayerContainer& barrelLayers() {return m_barrelLayers;}

  LayerContainer& barrelPixelLayers() {return m_barrelPixelLayers;}
  LayerContainer& endcapPixelLayers() {return m_endcapPixelLayers;}
  LayerContainer& barrelStripLayers() {return m_barrelStripLayers;}
  LayerContainer& endcapStripLayers() {return m_endcapStripLayers;}

  const std::vector<ServiceVolume*>& serviceVolumes() const {return m_volumes;}

  // Called by Routing
  void setServiceVolumes( const std::vector<ServiceVolume*>& vc) {m_volumes=vc;}

  // methods called by Builder 
  void constructBarrelLayer( double radius, double zHalfLength, 
			     DetType::Type type, int num, int nstaves, const std::string& suffix,
			     int nModulesPerStave, int nChipsPerModule);
  void constructEndcapLayer( double zpos, double rmin, double rmax, 
			     DetType::Type type, int num, int nstaves, const std::string& suffix,
			     int nModulesPerStave, int nChipsPerModule);

  const InDetServMatGeometryManager* geoMgr() const {return m_geoMgr;}

  void setGeoMgr( const InDetServMatGeometryManager* mgr) {m_geoMgr = mgr;}

private:

  LayerContainer               m_barrelLayers;
  LayerContainer               m_barrelPixelLayers;
  LayerContainer               m_endcapPixelLayers;
  LayerContainer               m_barrelStripLayers;
  LayerContainer               m_endcapStripLayers;

  std::vector<ServiceVolume*>  m_volumes;

  const InDetServMatGeometryManager* m_geoMgr;

  void add( std::map<std::string, double>& res, const std::string& name, double len);
  void addEosMaterial( const ServiceVolume& vol, std::vector<ServiceMaterial>& result);
  
  Athena::MsgStreamMember m_msg;
  // the message stream (same for all derived classes)
  MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }
  Athena::MsgStreamMember& msgStream() {return m_msg;}
  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) { return m_msg.get().level() <= lvl; }

};

#endif 
