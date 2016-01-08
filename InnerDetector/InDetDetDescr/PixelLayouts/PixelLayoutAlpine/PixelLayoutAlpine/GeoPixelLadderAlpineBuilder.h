/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLADDERALPINETOOL_H
#define GEOPIXELLADDERALPINETOOL_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "GaudiKernel/ServiceHandle.h"
#include "PixelInterfaces/IPixelModuleSvc.h"
#include "PixelInterfaces/IPixelDesignSvc.h"

#include "PixelGeoComponent/GeoStaveAlpine.h"
#include "PixelGeoComponent/GeoDetModule.h"
namespace InDet{
  class GeoComponent;
  class GeoStaveAlpine;
}

namespace InDet{
  class StaveTmp;
}

namespace InDetDD{
  class PixelModuleDesign;
}

class GeoLogVol;
class PixelGeoBuilderBasics;
class GeoPixelStaveSupportAlpineBuilder;

class GeoPixelLadderAlpineBuilder : public PixelGeoBuilder {

 public:
  GeoPixelLadderAlpineBuilder(const PixelGeoBuilderBasics* basics,
			      const InDet::StaveTmp *staveTmp, 
			      int iLayer, HepGeom::Transform3D trf);
  virtual void preBuild();

  virtual GeoVPhysVol* Build();
  virtual InDet::GeoComponent* getGeoCompVol() const {return m_ladderGeoComp;}

  double thickness() const {return m_thickness;}
  double thicknessP() const {return m_thicknessP;}
  double thicknessN() const {return m_thicknessN;}
  double width() const {return m_width;}
  double length() const {return m_length;}
  double rmin() const {return m_rmin; }
  double rmax() const {return m_rmax; }

  void setSector(int sector) {m_sector = sector; }
  void setLadderTransform(HepGeom::Transform3D trf) { m_ladderTransform = trf; }

 private:

  void calcRminmax();
  std::vector<HepGeom::Point3D<double>> getVertexShape(const GeoLogVol *logVol, HepGeom::Transform3D trf) const; 
  std::vector<HepGeom::Point3D<double>> getVertexShape(const GeoShape* shape, HepGeom::Transform3D trf) const; 
  void calcXMinMax(const GeoLogVol *logVol, HepGeom::Transform3D trf, double &xmin, double &xmax);
  void calcYMinMax(const GeoLogVol *logVol, HepGeom::Transform3D trf, double &ymin, double &ymax);
  void calcZMinMax(const GeoLogVol *logVol, HepGeom::Transform3D trf, double &zmin, double &zmax);
  std::vector<HepGeom::Point3D<double> > DecodeEndCapModulePositions(double xPos, double yPos, std::vector<double> v0,std::vector<double> v1);
  void computeRadiusMinMax(HepGeom::Transform3D trf, double length, double width, double thick, double &rMIn, double &rMax); 
  void showTransform(HepGeom::Transform3D trf ) const;
  HepGeom::Point3D<double> computePoint( HepGeom::Point3D<double>p1, HepGeom::Point3D<double>p2, double alpha);

  GeoVPhysVol* m_physVol;
  const GeoLogVol* m_theLadder;

  const InDet::StaveTmp* m_staveTmp;
 
  int m_layer;
  HepGeom::Transform3D m_localTrf;
  int m_sector;
  HepGeom::Transform3D m_ladderTransform;

  std::string m_layerName;
  double m_thickness;
  double m_thicknessP;
  double m_thicknessN;
  double m_width;
  double m_length;
  double m_rmin;
  double m_rmax;
  double m_FacePlateThick;

  int m_barrelModuleNumber;
  std::string m_barrelModuleType;
  double m_barrelModuleGap;

  int m_endcapModuleNumber;
  std::string m_endcapModuleType;
  std::vector<double> m_endcapModulePos;
  double m_endcapModuleGap;
  double m_endcapTiltAngle;
  std::vector<HepGeom::Point3D<double> > m_endcapModPos;

  std::vector<double> m_transModulePos;
  std::string m_transitionModuleType;
  double m_transitionTiltAngle;

  GeoDetModulePixel* m_barrelModule;
  InDetDD::PixelModuleDesign* m_barrelModuleDesign;
  GeoDetModulePixel* m_endcapModule;
  InDetDD::PixelModuleDesign* m_endcapModuleDesign;
  GeoDetModulePixel* m_transitionModule;
  InDetDD::PixelModuleDesign* m_transitionModuleDesign;

  InDet::GeoDetModule* m_barrelModule_geocomp;
  InDet::GeoDetModule* m_endcapModule_geocomp;
  InDet::GeoDetModule* m_transitionModule_geocomp;

  GeoPixelStaveSupportAlpineBuilder* m_staveSupport;
  const GeoShape* m_ladderShape;

  ServiceHandle<IPixelModuleSvc> m_pixelModuleSvc;
  ServiceHandle<IPixelDesignSvc> m_pixelDesignSvc;

  bool m_bGeoComp;
  InDet::GeoStaveAlpine *m_ladderGeoComp;

};

#endif
