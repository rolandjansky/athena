/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GeoModelXml_HGTD_GMX_DETECTORTOOL_H
#define HGTD_GeoModelXml_HGTD_GMX_DETECTORTOOL_H
//
//    Create an Athena Tool; handle Athena services and Tools needed for 
//    building the HGTD geometry. Then create the geometry using the HGTD_DetectorFactory.
//    This is the entry to the StripGeoModelXml package.
//
#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
// #include "GeoModelInterfaces/IGeoDbTagSvc.h"

#include "HGTD_GeoModel/HGTD_GeoModelAthenaComps.h" 

#include <string>

class HGTD_DetectorManager;

namespace InDetDD {
    class SiCommonItems;
}

class HGTD_GMX_DetectorTool : public GeoModelTool {
public:
    // Standard Constructor
    HGTD_GMX_DetectorTool(const std::string &type, const std::string &name, const IInterface *parent);

    virtual StatusCode create() override final;
    virtual StatusCode clear() override final;

    // Register callback function on CondDB object
    virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;

    // Callback function itself
    // virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override final;
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;

private:
    StringProperty m_detectorName{this, "DetectorName", "HGTD"};
    BooleanProperty m_alignable{this, "Alignable", false};
    StringProperty m_gmxFilename{this, "GmxFilename", ""};

    ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc;
    ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc;

    const HGTD_DetectorManager* m_detectorManager{nullptr};
    HGTD_GeoModelAthenaComps m_athenaComps;
    InDetDD::SiCommonItems* m_commonItems;
};

#endif // HGTD_GeoModelXml_HGTD_GMX_DETECTORTOOL_H
