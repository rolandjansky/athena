/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEGEOMODELXML_BCMPRIMEGMXINTERFACE_H
#define BCMPRIMEGEOMODELXML_BCMPRIMEGMXINTERFACE_H

#include <AthenaBaseComps/AthMessaging.h>
#include <GeoModelXml/GmxInterface.h>

#include <map>

namespace InDetDD
{

class BCMPrimeDetectorManager;

class BCMPrimeGmxInterface: public GmxInterface, public AthMessaging
{
public:
  BCMPrimeGmxInterface(InDetDD::BCMPrimeDetectorManager *detectorManager);

  virtual int sensorId(std::map<std::string, int> &index) const override final;
  virtual void addAlignable(int /*level*/, std::map<std::string, int> &index,
                            GeoVFullPhysVol *fpv, GeoAlignableTransform *transform) override final;

private:
  InDetDD::BCMPrimeDetectorManager *m_detectorManager{};
};

} // namespace InDetDD

#endif // BCMPRIMEGEOMODELXML_BCMPRIMEGMXINTERFACE_H
