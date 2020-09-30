#ifndef PixelRingSupportXMLHelper_H
#define PixelRingSupportXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "PixelLayoutUtils/GeoXMLUtils.h"

using namespace xercesc;

class PixelRingSupportXMLHelper :  public GeoXMLUtils, public PixelGeoBuilder  {

 public:
  PixelRingSupportXMLHelper(const PixelGeoBuilderBasics* basics);
  ~PixelRingSupportXMLHelper();
  bool  putLHBeforeRHrings();
  bool swapFrontBackModulePhiPosition();
  int getNbSupport(int layer, int ring=0);
  double getRingSupportRMin(int iSupport) const;
  double getRingSupportRMax(int iSupport) const;
  double getRingSupportThickness(int iSupport) const;
  int getRingSupportNSectors(int iSupport) const;
  double getRingSupportSPhi(int iSupport) const;
  double getRingSupportDPhi(int iSupport) const;
  std::string getRingSupportMaterial(int iSupport) const;

  int getNbLayerSupport(int layer);
  std::vector<double> getLayerSupportRadius(int iSupport) const;
  std::vector<double> getLayerSupportZ(int iSupport) const;
  std::string getLayerSupportMaterial(int iSupport) const;

  std::vector<int> getNbLayerSupportIndex(int layer);
  std::vector<double> getLayerSupportRadiusAtIndex(int index) const;
  std::vector<double> getLayerSupportZAtIndex(int index) const;
  std::string getLayerSupportMaterialAtIndex(int index) const;

  std::vector<int> getNbShellSupportIndex(int layer);
  std::vector<double> getLayerShellRadius(int layer) const;
  std::vector<double> getLayerShellZBounds(int layer) const;
  std::vector<double> getCorrugatedShellZClearance(int layer) const;
  double getCorrugatedCableClearance(int layer) const;
  double getCorrugatedShellDepth(int layer) const;
  double getLayerShellThickness(int layer) const;
  std::string getLayerShellMaterial(int layer) const;
  
 private:
  int m_ringGeoIndex;
  bool m_bXMLfileExist;
};

#endif
