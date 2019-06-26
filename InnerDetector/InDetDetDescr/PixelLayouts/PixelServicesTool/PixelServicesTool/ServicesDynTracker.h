/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef ServicesDynTracker_H
#define ServicesDynTracker_H

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "PixelServicesTool/DetTypeDyn.h"
#include "PixelServicesTool/ServiceDynMaterial.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "PixelServicesTool/ServiceDynVolume.h"
#include "PixelServicesTool/ServicesDynLayer.h"

#include <vector>
#include <map>
#include <string>

class ServicesDynTracker : public PixelGeoBuilder {
public:

  /// Construct an empty tracker, to be filled by Builder
  ServicesDynTracker(const PixelGeoBuilderBasics* basics, bool bSvcBrlModule=false);
  ~ServicesDynTracker();

  // Construct somehow (for testing)
  //ServicesDynTracker(bool toy);

  typedef std::vector<const ServicesDynLayer*>  LayerContainer;
  typedef std::vector<std::vector<const ServicesDynLayer*> >   Layer2DContainer;

  void computeServicesPerLayer();

  void finaliseServices();

  // barrel layers are sorted in radius from inside out
  const Layer2DContainer& barrelLayers() const {return m_barrelLayers;}
  Layer2DContainer& barrelLayers() {return m_barrelLayers;}

  Layer2DContainer& barrelPixelLayers() {return m_barrelPixelLayers;}
  LayerContainer& endcapPixelLayers() {return m_endcapPixelLayers;}
  Layer2DContainer& barrelStripLayers() {return m_barrelStripLayers;}
  LayerContainer& endcapStripLayers() {return m_endcapStripLayers;}

  const std::vector<ServiceDynVolume*>& serviceVolumes() const {return m_volumes;}

  // Called by Routing
  void setServiceDynVolumes( const std::vector<ServiceDynVolume*> vc) {m_volumes=vc;}

  // methods called by Builder 
  void constructBarrelLayer( double radius, double zHalfLength, double staveZOffset,
			     DetTypeDyn::Type type, int num, int iStaveTmp,int nstaves, const std::string& suffix,
			     int nModulesPerStave, int nChipsPerModule);
  void constructBarrelLayer( double radius, double zHalfLength, double staveZOffset,
			     DetTypeDyn::Type type, int num, int iStaveTmp, int nstaves, const std::string& suffix,
			     std::vector<int> nModulesPerStave, std::vector<int> nChipsPerModule);
  void constructEndcapLayer( double zpos, double rmin, double rmax, 
			     DetTypeDyn::Type type, int num, int nstaves, const std::string& suffix,
			     int nModulesPerStave, int nChipsPerModule);

  std::map<std::string,std::string> getSvcMaterialNameTable() const { return m_svcMaterialNameTable; }

private:

  bool m_bSvcDynAuto;
  bool m_bSvcBrlModule;

  Layer2DContainer             m_barrelLayers;
  Layer2DContainer             m_barrelPixelLayers;
  LayerContainer               m_endcapPixelLayers;
  Layer2DContainer             m_barrelStripLayers;
  LayerContainer               m_endcapStripLayers;
  
  std::vector<ServiceDynVolume*>  m_volumes;
  std::map<std::string,std::string> m_svcMaterialNameTable;

  void add( std::map<std::string, double>& res, const std::string& name, double len);
  void addEosMaterial( const ServiceDynVolume& vol, std::vector<ServiceDynMaterial>& result);
  
  const Athena::MsgStreamMember m_msg;
  // the message stream (same for all derived classes)
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  const Athena::MsgStreamMember& msgStream() const {return m_msg;}
  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

};

#endif 
