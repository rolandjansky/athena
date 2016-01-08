/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelStaveSupportXMLHelper_H
#define PixelStaveSupportXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class PixelStaveSupportXMLHelper :  public GeoXMLUtils  {

 public:
  PixelStaveSupportXMLHelper(int layer);
  ~PixelStaveSupportXMLHelper();

  double getMechanicalStaveLength() const;
  double getStaveLength() const;
  double getMechanicalStaveWidth() const;
  double getTubeOuterDiam(int geoIndex) const;
  double getTubeInnerDiam(int geoIndex) const;
  double getTubeMiddlePos(int geoIndex) const;
  double getTubeCentralPos(int geoIndex) const;
  double getTubeGlueThick(int geoIndex) const;
  double getFacePlateThick(int geoIndex) const;
  double getFacePlateWidth(int geoIndex) const;
  double getFacePlateGlueThick(int geoIndex) const;
  double getFacePlateGreaseThick(int geoIndex) const;
  double getFoamBaseThick(int geoIndex) const;
  double getFoamBaseThickTrans(int geoIndex) const;
  double getFoamEdgeThick(int geoIndex) const;
  double getFoamEdgeThickTrans(int geoIndex) const;
  double getFoamThick(int geoIndex) const;
  double getOmegaThick(int geoIndex) const;
  double getOmegaGlueThick(int geoIndex) const;
  double getOmegaMiddleThick(int geoIndex) const;
  double getOmegaTopWidth(int geoIndex) const;

  int getStaveFlexIndex() const;
  int getStaveFlexSectionNumber(int flexIndex) const;
  std::vector<double> getStaveFlexSectionZmin(int flexIndex) const;
  std::vector<double> getStaveFlexSectionZmax(int flexIndex) const;
  std::vector<double> getStaveFlexSectionLength(int flexIndex) const;
  std::vector<double> getStaveFlexSectionThick(int flexIndex) const;
  std::vector<double> getStaveFlexSectionVolume(int flexIndex) const;
  double getStaveCopperThick(int flexIndex) const;
  std::vector<double> getStaveFlexSectionCopperVolume(int flexIndex) const;
  std::vector<double> getStaveFlexSectionAdhesiveVolume(int flexIndex) const;
  std::vector<double> getStaveFlexSectionPolyimideVolume(int flexIndex) const;
  std::vector<double> getStaveFlexSectionCopperWeight(int flexIndex) const;
  std::vector<double> getStaveFlexSectionAdhesiveWeight(int flexIndex) const;
  std::vector<double> getStaveFlexSectionPolyimideWeight(int flexIndex) const;
  std::vector<int> getStaveFlexSectionTransition(int flexIndex) const;
  double getStaveFlexWingWidth(int flexIndex) const;
  std::vector<std::string> getStaveFlexSectionMaterial(int flexIndex) const;
  std::vector<std::string> getStaveFlexSectionMaterialTrans(int flexIndex) const;
    
  int getStaveGeoIndex() const { return m_staveGeoIndex; }
  int getStaveGeoIndexEC() const { return m_staveGeoIndexEC; }

  
 private:
  int m_layer;
  int m_staveGeoIndex;
  int m_staveGeoIndexEC;

};

#endif
