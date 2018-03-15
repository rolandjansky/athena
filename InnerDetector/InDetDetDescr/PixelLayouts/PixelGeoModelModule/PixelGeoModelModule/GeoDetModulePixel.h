/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELGEODETMODULE_H
#define GEOPIXELGEODETMODULE_H

#include "Identifier/Identifier.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class GeoLogVol;
class GeoPhysVol;
class GeoFullPhysVol;
class GeoVPhysVol;
class GeoVFullPhysVol;
class PixelGeoBuilderBasics;
class InDetMaterialManager;
namespace InDet{
  class GeoDetModule;
}

class GeoDetModulePixel : public GeoXMLUtils {

 public:
  GeoDetModulePixel(const PixelGeoBuilderBasics*, int index, int brl_ec=-1, int layerdisk=-1);
  virtual GeoFullPhysVol* Build(int brl_ec, int layer_disk=-1, int phi=-1, int eta=-1, int idTag=-1, std::string modName = "");
  virtual InDet::GeoDetModule* buildGeoComp(int brl_ec, int layer_disk);

  void preBuild();
  double Thickness() const;
  double ThicknessN() const;
  double ThicknessP() const;
  double Width() const;
  double Length() const;
  int ChipNumber() const { return m_chipNumber; }

  const GeoLogVol* getLogVol() const { return m_moduleLogVol; }

  double getModuleSensorWidth() const { return m_boardWidth;}
  double getModuleSensorLength() const { return m_boardLength; }
  double getModuleSensorThick() const { return m_boardThick; }
  
  double getModuleChipWidth() const { return m_chipWidth; }
  double getModuleChipLength() const { return m_chipLength; }
  double getModuleChipThick() const { return m_chipThick; }
  double getModuleChipGap() const { return m_chipGap; }

  double getModuleHybridWidth() const { return m_hybridWidth; }
  double getModuleHybridLength() const { return m_hybridLength; }
  double getModuleHybridThick() const { return m_hybridThick; }


 private:
  const GeoLogVol* m_moduleLogVol;
  const GeoLogVol* m_siLogVol;
  Identifier _id;
  //  GeoPixelSiCrystal& m_theSensor;
  //  bool m_isModule3D;

  const PixelGeoBuilderBasics* m_basics;
  int m_moduleIndex;
  int m_brl_ec;
  int m_layerdisk;

  InDetMaterialManager* m_matMgr;

  double m_boardWidth;
  double m_boardLength;
  double m_boardThick;
  
  double m_chipWidth;
  double m_chipLength;
  double m_chipThick;
  double m_chipWidthOffset;
  double m_chipLengthOffset;
  
  double m_hybridWidth;
  double m_hybridLength;
  double m_hybridThick;
  double m_hybridWidthOffset;
  double m_hybridLengthOffset;
  
  double m_chipGap;
  int m_chipNumber;

  std::string m_chipMatName;
  std::string m_hybridMatName;
  std::string m_sensorMatName;

  //  GeoPixelReadoutGeometry* m_readoutGeoBuilder;

/*   bool m_bIdFlags; */
/*   int m_brl_ec; */
/*   int m_layer_disk; */
/*   int m_phi; */
/*   int m_eta; */


};

#endif
