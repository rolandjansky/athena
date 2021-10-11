/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEGEOMODELXML_BCMPRIMEGMXINTERFACE_H
#define BCMPRIMEGEOMODELXML_BCMPRIMEGMXINTERFACE_H

#include "GeoModelXml/GmxInterface.h"

#include <map>
#include <memory>
#include <sstream>
#include <string>

class MsgStream;
namespace InDetDD {class BCMPrimeDetectorManager;}

namespace ITk {

  class BCMPrimeGmxInterface: public GmxInterface {
  public:
    BCMPrimeGmxInterface(InDetDD::BCMPrimeDetectorManager *detectorManager);
    ~BCMPrimeGmxInterface();
    virtual int sensorId(std::map<std::string, int> &index) const override final;
    virtual void addAlignable(int /*level*/, std::map<std::string, int> &index,
                              GeoVFullPhysVol *fpv, GeoAlignableTransform *transform) override final;

  private:
    InDetDD::BCMPrimeDetectorManager *m_detectorManager;
    std::unique_ptr<MsgStream> m_log;
  };

} // namespace ITk

#endif // BCMPRIMEGEOMODELXML_BCMPRIMEGMXINTERFACE_H
