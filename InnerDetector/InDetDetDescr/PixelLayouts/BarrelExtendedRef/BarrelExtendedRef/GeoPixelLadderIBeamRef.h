/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLADDERIBEAMREF_H
#define GEOPIXELLADDERLOI_H

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
class GeoPixelStaveSupportExtRef;
class GeoDetModulePixelBuilder;

class GeoPixelLadderIBeamRef : public PixelGeoBuilder {

 public:
  GeoPixelLadderIBeamRef(const PixelGeoBuilderBasics* basics, const InDet::StaveTmp *staveTmp, int iLayer, HepGeom::Transform3D trf, std::string mode);
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
  void setRsize(double r) {m_rSize = r; }
  void setRmid(double r) {m_rMid = r; }
  void setTilt(double t) {m_tilt = t; }

  GeoVPhysVol* getSvcVol() const {return m_serviceVol;}

 private:

  double calcThickness();
  double calcWidth(); 

  GeoVPhysVol* m_physVol;
  const GeoLogVol* m_theLadder;
  GeoPhysVol* m_serviceVol;

  const InDet::StaveTmp* m_staveTmp;

  int m_layer;
  HepGeom::Transform3D m_localTrf;
  int m_sector;
  std::string m_layerName;
  double m_thickness;
  double m_thicknessP;
  double m_thicknessN;
  double m_width;
  double m_length;
  double m_envLength;
  double m_rmin;
  double m_rmax;
  double m_moduleSvcThickness;
  double m_rSize;
  double m_rMid;

  //  std::vector<int> m_geoLadderIndex;
  int m_barrelModuleNumber;
  std::string m_barrelModuleType;
  double m_barrelModuleGap;
  double m_barrelModuleDZ;
  double m_moduleTilt;

  double m_staggerDist;
  double m_staggerSign;
  double m_moduleCenterShift;

  GeoDetModulePixel* m_barrelModule;
  InDetDD::PixelModuleDesign* m_barrelModuleDesign;
  GeoPixelStaveSupportExtRef* m_staveSupport;
  const GeoShape* m_ladderShape;

  ServiceHandle<IPixelModuleSvc> m_pixelModuleSvc;
  ServiceHandle<IPixelDesignSvc> m_pixelDesignSvc;
  ToolHandle<IPixelServicesTool> m_IDserviceTool;

  std::string m_mode;
  double m_tilt;
};

#endif
