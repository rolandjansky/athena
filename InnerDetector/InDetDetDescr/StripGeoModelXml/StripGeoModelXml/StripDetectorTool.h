/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StripGeoModelXml_STRIPDETECTORTOOL_H
#define StripGeoModelXml_STRIPDETECTORTOOL_H
//
//    Create an Athena Tool; handle Athena services and Tools needed for 
//    building the SCT geometry. Then create the geometry using the SCT_DetectorFactory.
//    This is the entry to the StripGeoModelXml package.
//
#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

//   ADA   class ISiLorentzAngleSvc;
class IGeoModelSvc;
class IRDBAccessSvc;
class IGeoDbTagSvc;
class IGeometryDBSvc;

namespace InDetDD {
    class SCT_DetectorManager;
    class AthenaComps;
    class SiCommonItems;
}

class StripDetectorTool : public GeoModelTool {
public:
    StripDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
    virtual ~StripDetectorTool();
    StatusCode create() override final;
    StatusCode clear() override final;
    StatusCode registerCallback() override final;
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override;

private:
    std::string m_detectorName;
    bool m_alignable;
    std::string m_gmxFilename;
    const InDetDD::SCT_DetectorManager *m_manager;
    InDetDD::AthenaComps *m_athenaComps;
    InDetDD::SiCommonItems *m_commonItems;
    ServiceHandle<IGeoModelSvc> m_geoModelSvc;
    ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc;
    ServiceHandle<IGeometryDBSvc> m_geometryDBSvc;
    ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
    //   ADA   ServiceHandle<ISiLorentzAngleSvc> m_lorentzAngleSvc;
};

#endif // StripGeoModelXml_STRIPDETECTORTOOL_H
