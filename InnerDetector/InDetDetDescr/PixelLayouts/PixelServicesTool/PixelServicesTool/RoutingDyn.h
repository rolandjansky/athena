/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef RoutingDyn_H
#define RoutingDyn_H

#include "PixelServicesTool/DetTypeDyn.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "PixelServicesTool/ServicesDynTracker.h"

#include <vector>

class SvcRoute;
class VSvcRoute;
class HSvcRoute;
class ServiceDynVolume;

class PixelSimpleServiceXMLHelper;
class PixelGeneralXMLHelper;
class PixelDynamicServiceXMLHelper;
class PixelRoutingServiceXMLHelper;
class PixelGeoBuilderBasics;
class RouteParameter;
class MinMaxHelper;

class RoutingDyn {
public:

  typedef ServicesDynTracker::LayerContainer LayerContainer;
  typedef ServicesDynTracker::Layer2DContainer  Layer2DContainer;

  RoutingDyn(const Athena::MsgStreamMember&, const PixelGeoBuilderBasics* basics);
  ~RoutingDyn();

  void createRoutingVolumes(ServicesDynTracker& tracker);
  void addRouteMaterial(const PixelGeoBuilderBasics* basics);
  void saveLayerSvcLinearMaterial(const PixelGeoBuilderBasics* basics);

  const std::vector<ServiceDynVolume*>& volumes() const {return m_volumes;}
  void computeBarrelModuleMaterial(const PixelGeoBuilderBasics* basics);

  std::map<std::string,std::string> getSvcMaterialNameTable() const { return m_svcMaterialNameTable; }

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

  std::vector<HSvcRoute> m_horizRoutes;
  std::vector<VSvcRoute> m_vertRoutes;
  std::vector<SvcRoute> m_diagRoutes;
  std::vector<std::string > predefinedRouteNames;
  HSvcRoute m_innerPST;
  HSvcRoute m_outerPST;
  HSvcRoute m_innerIST;
  HSvcRoute m_outerIST;
  std::vector<std::string> m_svcMatNames;

  Layer2DContainer m_bplc;
  LayerContainer m_eplc;
  void createRouteFromXML(int iRoute);
  void createRouteSegment(const RouteParameter &param);
  void createEndOfStaveSegment(const RouteParameter &param);
  void createHorizontalRoute(const RouteParameter &param);
  void createVerticalRoute(const RouteParameter &param);
  void organizePredefinedRouteSegment(const HSvcRoute& route);
  void organizePredefinedRouteSegment(const VSvcRoute& route);
  void checkVolumesOverlap();

  double DecodeLayerRadialPosition(std::string r, int layer, double rMin=-99999., double rMax=99999.);
  double DecodeLayerZPosition(std::string z, int layer,  double zShift, double zMin=-99999., double zMax=99999.);
  double DecodeLayerRadialShiftPosition(const std::string& r, double svcThick);
  double DecodeLayerMarginPosition(const std::string& r);
  void   AddRGap(std::string& r, int routeId);

  void createRoutes(ServicesDynTracker& tracker);
  ServiceDynMaterial computeRouteMaterial(const PixelGeoBuilderBasics* basics, DetTypeDyn::Type layerType, DetTypeDyn::Part layerPart, 
					  int layerNumber, int staveTmpNumber,
					  std::vector<int> modulePerStave, std::vector<int> chipPerModule,
					  bool scalePerHalf, bool onStave=true, std::string ctype="", double length = 1., double volume = 1.);
  std::string constructName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const;
  int decodeStaveCableNumber(std::string pattern, int nbStave, int modulePerStave, int chipPerModule);
  MinMaxHelper getLayerMinMaxBox(bool barrel, std::vector<int> layerIndices, std::vector<double> EOScardLength, double EOSlength);

  void addVolume( ServiceDynVolume* v) { m_volumes.push_back( v);}

  void dumpRoute( const SvcRoute& route);
  std::string nextVolumeName( const SvcRoute& route) const;

  std::string constructBarrelLayerName(std::string svcName, std::vector<int> nModule, int iLayer);  
  std::string constructBarrelLayerName(std::string svcName, std::vector<int> nModule, int iLayer, int iStaveTmp);  
  std::map<std::string,std::string> m_svcMaterialNameTable;

  // the message stream (same for all derived classes)
  //  const Athena::MsgStreamMember m_msg;
  const Athena::MsgStreamMember m_msgRouting;
  MsgStream& msg (MSG::Level lvl) const { return m_msgRouting << lvl; }
  const Athena::MsgStreamMember& msgStream() const {return m_msgRouting; }
  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_msgRouting.get().level() <= lvl; }

  PixelSimpleServiceXMLHelper* m_simpleSrvXMLHelper;
  PixelGeneralXMLHelper* m_genXMLHelper;
  PixelDynamicServiceXMLHelper* m_svcDynXMLHelper;
  PixelRoutingServiceXMLHelper* m_svcRoutingXMLHelper;

  std::map<const ServicesDynLayer*, ServiceDynMaterial> m_layerMaterial; // cache the layer services
  
};

// for GMX migration : consider ways to reduce the number of parameters
// maybe consider https://en.wikipedia.org/wiki/Fluent_interface
//  and/or prefer use of const ref

class RouteParameter{
  
 public:
 RouteParameter(int iRoute, int iSeg, bool bBarrel,
		std::string r1, std::string r2, std::string z1, std::string z2, 
		std::vector<int> layerIndices, double svcThick,
		bool bFirstSeg, bool bLastSeg, std::string type, bool isPhiRouting,
		std::vector<double> EOScardLength = std::vector<double>(), double EOSsvcLength = 0.,
                int nSectors = 1, float refPhi = 0., float sectorWidth = 2*M_PI, bool splitLayersInPhi = false, int phiStepInSectors = 1 ):
    m_routeId(iRoute), m_segId(iSeg),
    m_bBarrel(bBarrel),
    m_r1(r1), m_r2(r2), m_z1(z1), m_z2(z2), 
    m_layerIndices(layerIndices), m_svcThick(svcThick),
    m_bFirstSegment(bFirstSeg), m_bLastSegment(bLastSeg),
    m_type(type),
    m_isPhiRouting(isPhiRouting),
    m_EOScardLength(EOScardLength), m_EOSsvcLength(EOSsvcLength),
      m_nSectors(nSectors),m_refPhi(refPhi),m_sectorWidth(sectorWidth),m_splitLayersInPhi(splitLayersInPhi), m_phiStepInSectors(phiStepInSectors)  {};
  ~RouteParameter(){};
  
  int getRouteId() const { return m_routeId; }
  int getSegmentId() const { return m_segId; }
  bool isBarrel() const { return m_bBarrel; }
  std::string getR1() const {return m_r1; }
  std::string getR2() const {return m_r2; }
  std::string getZ1() const {return m_z1; }
  std::string getZ2() const {return m_z2; }
    
  std::vector<int> getLayerIndices() const { return m_layerIndices; }
  double getSvcThickness() const { return m_svcThick; }
  bool isFirstSegment() const { return m_bFirstSegment; }
  bool isLastSegment() const { return m_bLastSegment; }
  std::string getType() const { return m_type; }
  bool isPhiRouting() const { return m_isPhiRouting; }
  std::vector<double> getEOScardLength() const { return m_EOScardLength; }
  double getEOSsvcLength() const { return m_EOSsvcLength; }
  int       getNumSectors() const { return m_nSectors; } 
  double getPhiRefFirstSector() const { return m_refPhi; }
  double getSectorVolumeWidth() const { return m_sectorWidth; }
  bool     splitLayersInPhi() const { return m_splitLayersInPhi; }
  int     phiStep() const { return m_phiStepInSectors; }

 private:
  int m_routeId, m_segId;
  bool m_bBarrel;
  std::string m_r1, m_r2;
  std::string m_z1, m_z2;

  std::vector<int> m_layerIndices;
  double m_svcThick;
  bool m_bFirstSegment, m_bLastSegment;
  std::string m_type;
  bool m_isPhiRouting;
  std::vector<double> m_EOScardLength;
  double m_EOSsvcLength;
  int m_nSectors;
  float m_refPhi;
  float m_sectorWidth;
  bool m_splitLayersInPhi;
  int  m_phiStepInSectors;

};

class MinMaxHelper{
  
 public:
    MinMaxHelper(double min, double max, double rmin, double rmax):
      m_zMin(min), m_zMax(max), m_rMin(rmin), m_rMax(rmax) {};
 ~MinMaxHelper(){};
  
  double getZMin() const { return m_zMin; };
  double getZMax() const { return m_zMax; };
  double getRMin() const { return m_rMin; };
  double getRMax() const { return m_rMax; };
  void update(MinMaxHelper box)
  { m_zMin = std::min(m_zMin,box.m_zMin);
    m_zMax = std::max(m_zMax,box.m_zMax);
    m_rMin = std::min(m_rMin,box.m_rMin);
    m_rMax = std::max(m_rMax,box.m_rMax);
  }
  
 private:
  double m_zMin,m_zMax;
  double m_rMin,m_rMax;
};

class Interval{

 public:
  Interval(double min, double max):
    m_min(min),m_max(max) {};
    ~Interval() {};

  double getMin() const { return m_min; }
  double getMax() const { return m_max; }
  bool isInInterval(double v) const { return ((v-m_min)*(v-m_max)<=0.); }
  bool containsInterval( Interval v) const { return (isInInterval(v.getMin())&&isInInterval(v.getMax())); }
  bool isIdenticalTo(Interval v) const { return (fabs(v.getMin()-m_min)<0.001&&fabs(v.getMax()-m_max)<0.001); }
  
 private:
  double m_min, m_max;
};

#endif
