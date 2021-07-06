/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGEOMODELXML_PIXELGMXINTERFACE_H
#define PIXELGEOMODELXML_PIXELGMXINTERFACE_H

#include <AthenaBaseComps/AthMessaging.h>
#include <GeoModelXml/GmxInterface.h>
#include <InDetGeoModelUtils/WaferTree.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace InDetDD {class SiDetectorDesign; class PixelDetectorManager; class PixelDiodeMatrix; class SiCommonItems;}

namespace ITk
{
class PixelGmxInterface : public GmxInterface, public AthMessaging
{
public:
  PixelGmxInterface(InDetDD::PixelDetectorManager *detectorManager,
                    InDetDD::SiCommonItems *commonItems,
                    WaferTree *moduleTree);

  virtual int sensorId(std::map<std::string, int> &index) const override final;
  virtual void addSensorType(std::string clas,
                             std::string typeName,
                             std::map<std::string, std::string> parameters) override final;
  virtual void addSensor(std::string typeName,
                         std::map<std::string, int> &index,
                         int sequentialId,
                         GeoVFullPhysVol *fpv) override final;
  // virtual void addAlignable(int level,
  //                           std::map<std::string, int> &index,
  //                           GeoVFullPhysVol *fpv,
  //                           GeoAlignableTransform *transform) override final;


private:
  void makePixelModule(const std::string& typeName,
                       const std::map<std::string, std::string> &parameters);

  std::shared_ptr<const InDetDD::PixelDiodeMatrix> buildMatrix(double phiPitch, double etaPitch,
                                                               double etaPitchLong, double etaPitchEnd,
                                                               double phiPitchLong, double phiPitchEnd,
                                                               int nEtaLong, int nEtaEnd,
                                                               int nPhiLong, int nPhiEnd,
                                                               int circuitsPhi, int circuitsEta,
                                                               int diodeColPerCirc, int diodeRowPerCirc) const;

  std::map<std::string, int> m_geometryMap;
  InDetDD::PixelDetectorManager *m_detectorManager{};
  InDetDD::SiCommonItems *m_commonItems{};
  WaferTree *m_moduleTree{};
};

} // namespace ITk

#endif // PIXELGEOMODELXML_PIXELGMXINTERFACE_H
