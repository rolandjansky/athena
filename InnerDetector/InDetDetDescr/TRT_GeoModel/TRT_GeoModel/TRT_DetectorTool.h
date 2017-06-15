/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_GEOMODEL_TRTDETECTORTOOL_H
#define TRT_GEOMODEL_TRTDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h" //for Argon
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Please consult the README for more information about which options to set in your joboptions file. //
////////////////////////////////////////////////////////////////////////////////////////////////////////

class IGeoModelSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;

namespace InDetDD {
  class TRT_DetectorManager;
  class AthenaComps;
}

class TRT_DetectorTool : public GeoModelTool {

public:

    // Standard Constructor
    TRT_DetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~TRT_DetectorTool();

    virtual StatusCode create( StoreGateSvc* detStore );
    virtual StatusCode clear(StoreGateSvc* detStore);

    // Register callback function on ConDB object
    virtual StatusCode registerCallback( StoreGateSvc* detStore );

    // Callback function itself
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS);

 private:

    bool m_useOldActiveGasMixture;
    bool m_DC2CompatibleBarrelCoordinates;
    bool m_initialLayout;
    int m_overridedigversion;
    bool m_alignable;

    ServiceHandle< IGeoModelSvc > m_geoModelSvc;
    ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
    ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;
    ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc; // added for Argon
 
    bool m_doArgonMixture;
    bool m_doKryptonMixture;
    bool m_useDynamicAlignFolders;
 
    const InDetDD::TRT_DetectorManager * m_manager;
    InDetDD::AthenaComps * m_athenaComps;

};

#endif // TRT_GEOMODEL_TRTDETECTORTOOL_H
