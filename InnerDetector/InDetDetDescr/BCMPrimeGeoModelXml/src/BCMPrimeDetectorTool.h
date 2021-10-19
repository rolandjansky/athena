/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORTOOL_H
#define BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORTOOL_H

#include <GaudiKernel/ServiceHandle.h>
#include <GeoModelInterfaces/IGeoDbTagSvc.h>
#include <GeoModelUtilities/GeoModelTool.h>
#include <RDBAccessSvc/IRDBAccessSvc.h>

class IRDBAccessSvc;
class IGeoDbTagSvc;

namespace InDetDD
{
  class BCMPrimeDetectorManager;
}

/** @class BCMPrimeDetectorTool

  Create an Athena Tool; handle Athena services and Tools needed for
  building the BCM' geometry. Then create the geometry using the BCMPrimeDetectorFactory.

  @author  Jakob Novak <jakob.novak@cern.ch>
*/

class BCMPrimeDetectorTool : public GeoModelTool
{
public:
  BCMPrimeDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~BCMPrimeDetectorTool() = default;
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  virtual StatusCode registerCallback() override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;

private:
  const InDetDD::BCMPrimeDetectorManager *m_manager{};

  Gaudi::Property<std::string> m_detectorName{this, "DetectorName", "BCMPrime", ""};
  Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
  Gaudi::Property<std::string> m_gmxFilename{this, "GmxFilename", "", ""};
  ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc{this, "RDBAccessSvc", "RDBAccessSvc", ""};
  ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc{this, "GeoDbTagSvc", "GeoDbTagSvc", ""};
};

#endif // BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORTOOL_H
