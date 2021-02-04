/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
#define HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
//
//    Dec 15, 2020, Lianyou SHAN move it toward GeoModelXml
//    Create an Athena Tool; handle Athena services and Tools needed for 
//    building the HGTD geometry. Then create the geometry using the HGTD_DetectorFactory.
//    This is the entry to the HGTD_GeoModel package.
//

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoModelUtilities/GeoModelTool.h"
//#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include <string>

class HGTD_DetectorManager;

class ISiLorentzAngleSvc;
class IGeoModelSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;

namespace InDetDD {
    class AthenaComps;
    class SiCommonItems;
}

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

    std::string m_detectorName ;
    bool m_alignable ;

    std::string m_gmxFilename;

    ServiceHandle<IGeoModelSvc> m_geoModelSvc ;
    ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc ;
    ServiceHandle<IGeometryDBSvc> m_geometryDBSvc ;
    ServiceHandle<ISiLorentzAngleSvc> m_lorentzAngleSvc;

    const HGTD_DetectorManager *m_detectorManager ;
    InDetDD::AthenaComps  *m_athenaComps ;
    InDetDD::SiCommonItems *m_commonItems ;

    bool m_printIDdict ;
};

#endif // HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
