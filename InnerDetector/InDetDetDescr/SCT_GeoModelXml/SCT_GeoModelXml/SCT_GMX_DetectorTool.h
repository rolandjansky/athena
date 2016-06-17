/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModelXml_SCT_GMX_DETECTORTOOL_H
#define SCT_GeoModelXml_SCT_GMX_DETECTORTOOL_H
//
//    Create an Athena Tool; handle Athena services and Tools needed for 
//    building the SCT geometry. Then create the geometry using the SCT_DetectorFactory.
//    This is the entry to the SCT_GeoModelXml package.
//
#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class ISiLorentzAngleSvc;
class IGeoModelSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;
class IInDetServMatBuilderTool;

namespace InDetDD {
    class SCT_DetectorManager;
    class AthenaComps;
    class SiCommonItems;
}

class SCT_GMX_DetectorTool : public GeoModelTool {
public:
    SCT_GMX_DetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
    virtual ~SCT_GMX_DetectorTool();
    virtual StatusCode create(StoreGateSvc* detStore);
    virtual StatusCode clear(StoreGateSvc* detStore);

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
    ServiceHandle<ISiLorentzAngleSvc> m_lorentzAngleSvc;
    ToolHandle<IInDetServMatBuilderTool> m_serviceBuilderTool;
};

#endif // SCT_GeoModelXml_SCT_SLHC_DETECTORTOOL_H
