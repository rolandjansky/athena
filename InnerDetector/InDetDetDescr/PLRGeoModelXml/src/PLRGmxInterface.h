/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLRGEOMODELXML_PLRGMXINTERFACE_H
#define PLRGEOMODELXML_PLRGMXINTERFACE_H

#include <PixelGeoModelXml/PixelGmxInterface.h>

#include <map>
#include <sstream>
#include <string>

// Most functions for this interface are derived from PixelGeoModelXml/PixelGmxInterface.h
// The functions here are similar functions to that of Pixel, but have a few PLR specific differences
// such as only loading the correct PLR sensor type and identifying the modules as InDetDD::PLR

namespace InDetDD
{

class PixelDetectorManager;
class SiCommonItems;

class PLRGmxInterface : public ITk::PixelGmxInterface
{
public:
  PLRGmxInterface(PixelDetectorManager *detectorManager,
                  SiCommonItems *commonItems,
                  WaferTree *moduleTree);

  virtual void addSensorType(const std::string& clas,
                             const std::string& typeName,
                             const std::map<std::string, std::string>& parameters) override final;

private:
  void makePLRModule(const std::string& typeName,
                     const std::map<std::string, std::string> &parameters);

  PixelDetectorManager *m_detectorManager{};
};

} // namespace InDetDD

#endif // PLRGEOMODELXML_PLRGMXINTERFACE_H
