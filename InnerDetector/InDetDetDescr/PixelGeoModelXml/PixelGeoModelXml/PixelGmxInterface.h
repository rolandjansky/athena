/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGEOMODELXML_PIXELGMXINTERFACE_H
#define PIXELGEOMODELXML_PIXELGMXINTERFACE_H

#include <AthenaBaseComps/AthMessaging.h>
#include <GeoModelXml/GmxInterface.h>
#include <InDetGeoModelUtils/WaferTree.h>

#include <map>
#include <sstream>
#include <string>

namespace InDetDD {

class PixelDetectorManager;
class PixelDiodeMatrix;
class SiCommonItems;
class SiDetectorDesign;

namespace ITk
{
class PixelGmxInterface : public GmxInterface, public AthMessaging
{
public:
  PixelGmxInterface(PixelDetectorManager *detectorManager,
                    SiCommonItems *commonItems,
                    WaferTree *moduleTree);

  virtual int sensorId(std::map<std::string, int> &index) const override final;
  virtual void addSensorType(const std::string& clas,
                             const std::string& typeName,
                             const std::map<std::string, std::string>& parameters) override;
  virtual void addSensor(const std::string& typeName,
                         std::map<std::string, int> &index,
                         int sequentialId,
                         GeoVFullPhysVol *fpv) override final;
  // virtual void addAlignable(int level,
  //                           std::map<std::string, int> &index,
  //                           GeoVFullPhysVol *fpv,
  //                           GeoAlignableTransform *transform) override final;

  std::shared_ptr<const PixelDiodeMatrix> buildMatrix(double phiPitch, double etaPitch,
						      double phiPitchLong, double phiPitchEnd,
						      double etaPitchLong, double etaPitchEnd,
						      int nPhiLong, int nPhiEnd,
						      int nEtaLong, int nEtaEnd,
						      int circuitsPhi, int circuitsEta,
						      int diodeColPerCirc, int diodeRowPerCirc) const;

protected:
  std::map<std::string, int> m_geometryMap;

private:
  void makePixelModule(const std::string& typeName,
                       const std::map<std::string, std::string> &parameters);

  PixelDetectorManager *m_detectorManager{};
  SiCommonItems *m_commonItems{};
  WaferTree *m_moduleTree{};
};

} // namespace ITk
} // namespace InDetDD

#endif // PIXELGEOMODELXML_PIXELGMXINTERFACE_H
