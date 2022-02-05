/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPGEOMODELXML_STRIPGMXINTERFACE_H
#define STRIPGEOMODELXML_STRIPGMXINTERFACE_H

#include <AthenaBaseComps/AthMessaging.h>
#include <GeoModelXml/GmxInterface.h>
#include <InDetGeoModelUtils/WaferTree.h>

#include <map>
#include <string>
#include <sstream>

namespace InDetDD
{

class SCT_DetectorManager;
class SCT_ModuleSideDesign;
class SiCommonItems;
class SiDetectorDesign;

namespace ITk
{

class StripGmxInterface : public GmxInterface, public AthMessaging
{
public:
  StripGmxInterface(SCT_DetectorManager *detectorManager,
                    SiCommonItems *commonItems,
                    WaferTree *waferTree);

  virtual int sensorId(std::map<std::string, int> &index) const override final;
  // For "artificially" adding to Identifiers; specify the field (e.g. "eta_module") and the value to add
  virtual int splitSensorId(std::map<std::string, int> &index,
                            std::pair<std::string, int> &extraIndex,
                            std::map<std::string, int> &updatedIndex) const override final;
  virtual void addSensorType(const std::string& clas,
                             const std::string& typeName,
                             const std::map<std::string, std::string>& parameters) override final;
  void addSensor(const std::string& typeName,
                 std::map<std::string, int> &index,
                 int sequentialId,
                 GeoVFullPhysVol *fpv) override final;
  void addSplitSensor(const std::string& typeName,
                      std::map<std::string, int> &index,
                      std::pair<std::string, int> &extraIndex,
                      int sequentialId,
                      GeoVFullPhysVol *fpv) override final;
  virtual void addAlignable(int level,
                            std::map<std::string, int> &index,
                            GeoVFullPhysVol *fpv,
                            GeoAlignableTransform *transform) override final;

private:
  void makeSiStripBox(const std::string& typeName,
                      const std::map<std::string, std::string> &parameters);
  void makeStereoAnnulus(const std::string& typeName,
                         const std::map<std::string, std::string> &parameters);

  std::map<std::string, const SiDetectorDesign *> m_geometryMap;
  std::map<std::string, const SCT_ModuleSideDesign *> m_motherMap;
  SCT_DetectorManager *m_detectorManager{};
  SiCommonItems *m_commonItems{};
  WaferTree *m_waferTree{};
};

} // namespace ITk
} // namespace InDetDD

#endif // STRIPGEOMODELXML_STRIPGMXINTERFACE_H
