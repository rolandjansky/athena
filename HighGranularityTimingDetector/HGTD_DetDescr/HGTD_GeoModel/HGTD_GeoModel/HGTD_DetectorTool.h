/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
#define HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
//
//    Create an Athena Tool; handle Athena services and Tools needed for 
//    building the HGTD geometry. Then create the geometry using the HGTD_DetectorFactory.
//    This is the entry to the HGTD_GeoModel package.
//
#include "GeoModelUtilities/GeoModelTool.h"
// #include "PixelInterfaces/IGeoPixelLayerTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

namespace InDetDD {
    class AthenaComps;
}

class HGTD_DetectorManager;
class IGeoModelSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;

/** @class HGTD_DetectorTool
    @brief HGTD_DetectorTool is a standard GeoModel tool, which calls HGTD_DetectorFactory::create(),
    stores HGTD_DetectorManager to the Detector Store and also registers a callback function
    align() which applies misalignments on top of the 'regular' geometry.
*/
class HGTD_DetectorTool : public GeoModelTool {
public:
    HGTD_DetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
    virtual ~HGTD_DetectorTool();
    virtual StatusCode create(StoreGateSvc* detStore);
    virtual StatusCode clear(StoreGateSvc* detStore);
    virtual StatusCode registerCallback(StoreGateSvc* detStore);
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys));

private:
    std::string m_detectorName;
    bool m_alignable;
    const HGTD_DetectorManager *m_manager;
    InDetDD::AthenaComps *m_athenaComps;
    ServiceHandle<IGeoModelSvc> m_geoModelSvc;
    ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc;
    ServiceHandle<IGeometryDBSvc> m_geometryDBSvc;

    std::string m_geometryConfig; // FULL, SIMU, RECO

    // this is for the HGTD readout definition
    bool m_HGTD_isbaseline;

};

#endif // HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
