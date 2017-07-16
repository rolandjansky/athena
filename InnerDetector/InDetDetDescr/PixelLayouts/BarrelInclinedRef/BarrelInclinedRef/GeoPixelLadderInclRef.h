/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXEL_LadderInclRef_H
#define GEOPIXEL_LadderInclRef_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "PixelInterfaces/IPixelModuleSvc.h"
#include "PixelInterfaces/IPixelDesignSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "PixelInterfaces/IPixelServicesTool.h"

namespace InDet{
  class StaveTmp;
}

namespace InDetDD{
  class PixelModuleDesign;
}

class GeoLogVol;
class GeoDetModulePixel;
class GeoPixelStaveSupportInclRef;
class GeoDetModulePixelBuilder;

class GeoPixelLadderInclRef : public PixelGeoBuilder {

 public:
  GeoPixelLadderInclRef(const PixelGeoBuilderBasics* basics, const InDet::StaveTmp *staveTmp, int iLayer, int nSectors, int nSectorsLastLayer, int nSectorsNextLayer, double phiOfStaveZero, double phiOfStaveZeroLastLayer, HepGeom::Transform3D trf);
  virtual GeoVPhysVol* Build();
  virtual void preBuild();

  double thickness() const {return m_thickness;}
  double thicknessP() const {return m_thicknessP;}
  double thicknessN() const {return m_thicknessN;}
  double width() const {return m_width;}
  double length() const {return m_length;}
  double envLength() const {return m_envLength;}
  double rmin() const {return m_rmin; }
  double rmax() const {return m_rmax; }

  void setSector(int sector) {m_sector = sector; }
  
 private:

  std::vector<HepGeom::Point3D<double> > DecodeEndCapModulePositions(double xPos, double yPos, std::vector<double> v0,std::vector<double> v1); 
  void computeRadiusMinMax(HepGeom::Transform3D trf, double length, double width, double thick, double &rMin, double &rMax);
  double calcThickness();
  double calcWidth();
  GeoPhysVol* createServiceVolume(double length, double thick, double width, std::vector<int> nModuleSvc);
  void BuildAndPlaceModuleService(std::vector<int> moduleNumber, double zInit, double zFinal, double locTilt, GeoPhysVol* volPhys, std::string type);
  std::vector<double> ConstructAndPlaceModuleService(std::vector<int> moduleNumber, double zInit, double zFinal, double locTilt, GeoPhysVol* volPhys, std::string type, bool  build=false);

  GeoVPhysVol* m_physVol;
  const GeoLogVol* m_theLadder;

  const InDet::StaveTmp* m_staveTmp;
 
  int m_layer;
  int m_nSectors;
  int m_nSectorsLastLayer;
  int m_nSectorsNextLayer;
  double m_phiOfStaveZero;
  double m_phiOfStaveZeroLastLayer;
  HepGeom::Transform3D m_localTrf;
  int m_sector;
  HepGeom::Transform3D m_ladderTransform;

  std::string m_svcRouting; 
  std::string m_layerName;
  double m_thickness;
  double m_thicknessP;
  double m_thicknessN;
  double m_width;
  double m_length;
  double m_rmin;
  double m_rmax;
  double m_envLength;
  double m_moduleSvcThickness;

  double m_staggerDist;
  double m_staggerSign;
  double m_moduleCenterShift;

  int m_barrelModuleNumber;
  std::string m_barrelModuleType;
  double m_barrelModuleGap;
  double m_barrelModuleTilt;
  double m_barrelModuleDZ;
  double m_moduleTilt;

  int m_endcapModuleNumber;
  std::string m_endcapModuleType;
  std::vector<double> m_endcapModulePos;
  double m_endcapModuleGap;
  double m_endcapInclAngle;
  double m_endcapModuleRshift;
  double m_endcapModuleRtilt;
  std::vector<HepGeom::Point3D<double> > m_endcapModPos;

  int m_transitionModuleNumber;
  std::vector<double> m_transModulePos;
  std::string m_transitionModuleType;
  double m_transitionTiltAngle;

  int m_svcMaterialCmpt;
  double m_gapPlanarStave;

  GeoDetModulePixel* m_barrelModule;
  InDetDD::PixelModuleDesign* m_barrelModuleDesign;
  GeoDetModulePixel* m_endcapModule;
  InDetDD::PixelModuleDesign* m_endcapModuleDesign;
  GeoDetModulePixel* m_transitionModule;
  InDetDD::PixelModuleDesign* m_transitionModuleDesign;

  GeoPixelStaveSupportInclRef* m_staveSupport;
  const GeoShape* m_ladderShape;

  bool m_minmaxDefined;

  ServiceHandle<IPixelModuleSvc> m_pixelModuleSvc;
  ServiceHandle<IPixelDesignSvc> m_pixelDesignSvc;
  ToolHandle<IPixelServicesTool> m_IDserviceTool;

};

#endif
