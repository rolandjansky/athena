/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORTOOL_H
#define BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class IGeoModelSvc;
class IRDBAccessSvc;
class IGeoDbTagSvc;
class IGeometryDBSvc;

namespace InDetDD {
  class AthenaComps;
  class BCMPrimeDetectorManager;
}

/** @class BCMPrimeDetectorTool

    Create an Athena Tool; handle Athena services and Tools needed for
    building the BCM' geometry. Then create the geometry using the BCMPrimeDetectorFactory.

    @author  Jakob Novak <jakob.novak@cern.ch>

    */

class BCMPrimeDetectorTool : public GeoModelTool {
 public: 
  BCMPrimeDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~BCMPrimeDetectorTool();
  StatusCode create() override final;
  StatusCode clear() override final;
  StatusCode registerCallback() override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override;
  
 private:
  std::string m_detectorName;
  bool m_alignable;
  std::string m_gmxFilename;
  const InDetDD::BCMPrimeDetectorManager *m_manager;
  InDetDD::AthenaComps *m_athenaComps;
  ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc;
  ServiceHandle<IGeometryDBSvc> m_geometryDBSvc;
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
};

#endif // BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORTOOL_H
