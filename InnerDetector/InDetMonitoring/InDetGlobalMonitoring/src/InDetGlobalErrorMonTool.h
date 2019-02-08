/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGlobalErrorMonTool_H
#define InDetGlobalErrorMonTool_H

#include <string>
#include <map>
#include <utility>
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"

namespace InDetDD
{
    class PixelDetectorManager;
}

class PixelID;
class SCT_ID;
class TH2F;

class InDetGlobalErrorMonTool : public ManagedMonitorToolBase {
    // First pair is eta and second pair is phi.
    // First element of pair is minimum second is maximum.
    typedef std::pair< std::pair<double, double>, std::pair<double, double> > moduleGeo_t;
    typedef std::map< IdentifierHash , moduleGeo_t > geoContainer_t;
    typedef std::map< Identifier , moduleGeo_t > geoContainerPure_t;

public:
    InDetGlobalErrorMonTool( 
	const std::string & type,
	const std::string & name,
	const IInterface* parent );

    virtual StatusCode initialize();
    
    virtual StatusCode bookHistogramsRecurrent();

    virtual StatusCode fillHistograms();

    virtual StatusCode procHistograms();

private:
    bool SyncPixel();
    bool SyncDisabledSCT();
    bool SyncErrorSCT();

    void FillModule( moduleGeo_t module, TH2F * histo );
    
    const PixelID * m_pixID;
    const InDetDD::PixelDetectorManager * m_pixManager;
    const SCT_ID * m_sctID;
    
    ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};
    ToolHandle<ISCT_ConfigurationConditionsTool> m_ConfigurationTool{this, "conditionsTool",
        "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
    ToolHandle<ISCT_ByteStreamErrorsTool> m_byteStreamErrTool{this, "SCT_ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
    ServiceHandle<IPixelByteStreamErrorsSvc> m_ErrorSvc;

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

    geoContainer_t m_errorGeoPixel;
    geoContainer_t m_disabledGeoPixel;
    geoContainer_t m_errorGeoSCT;
    geoContainerPure_t m_disabledGeoSCT;
    
    TH2F * m_disabledModulesMapPixel;
    TH2F * m_disabledModulesMapSCT;
    TH2F * m_badModulesMapPixel;
    TH2F * m_errorModulesMapPixel;
    TH2F * m_errorModulesMapSCT;
    TH2F * m_totalModulesMapPixel;
    TH2F * m_totalModulesMapSCT;
    TH2F * m_totalModulesMapSi;

    const unsigned int m_c_nBinsEta;
    const float        m_c_rangeEta;
    const unsigned int m_c_nBinsPhi;
};

#endif
