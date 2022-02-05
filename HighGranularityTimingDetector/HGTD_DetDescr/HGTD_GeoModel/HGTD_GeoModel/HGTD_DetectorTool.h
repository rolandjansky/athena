/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
#define HGTD_GEOMODEL_HGTD_DETECTORTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoModelUtilities/GeoModelTool.h"
#include "HGTD_GeoModel/HGTD_GeoModelAthenaComps.h" 
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include <string>

class HGTD_DetectorManager;

namespace InDetDD {
    class SiCommonItems;
}

/** @class HGTD_DetectorTool
    @brief HGTD_DetectorTool is a standard GeoModel tool, which calls HGTD_DetectorFactory::create(),
    stores HGTD_DetectorManager to the Detector Store and also registers a callback function
    align() which applies misalignments on top of the 'regular' geometry.
*/
class HGTD_DetectorTool : public GeoModelTool {
 public:
    // Standard Constructor
    HGTD_DetectorTool(const std::string &type, const std::string &name, const IInterface *parent);

    virtual StatusCode create() override final;
    virtual StatusCode clear() override final;

    // Register callback function on CondDB object
    virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;

    // Callback function itself
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override final;

 private:

    StringProperty m_detectorName{this, "DetectorName", "HGTD"};
    BooleanProperty m_alignable{this, "Alignable", false};

    ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc;
    ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc;

    const HGTD_DetectorManager* m_detectorManager;

    HGTD_GeoModelAthenaComps m_athenaComps;

    bool m_printIDdict;
};

#endif // HGTD_GEOMODEL_HGTD_DETECTORTOOL_H
