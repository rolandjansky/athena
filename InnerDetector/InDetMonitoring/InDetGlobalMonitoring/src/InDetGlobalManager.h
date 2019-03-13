/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalManager.h
 * Implementation of inner detector global monitoring manager 
 * Based on AthenaMon.cxx
 *
 * @author Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 *
 * $Id: InDetGlobalManager.h,v 1.17 2009-01-19 18:56:48 kastanas Exp $
 *
 ****************************************************************************/


#ifndef InDetGlobalManager_H
#define InDetGlobalManager_H

//Local includes
#include "InDetGlobalMotherMonTool.h"

//Framework
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "StoreGate/ReadHandleKey.h"

//Standard c++
#include <string>
//Predeclarations
class ISvcLocator;
class InDetGlobalMonitorToolBase;
class StatusCode;
class SCT_IdMapping;
class IIneterface;

/// Top Algorithm of the Inner Detector Global Monitoring

class InDetGlobalManager : public AthenaMonManager
{

public:

    InDetGlobalManager(const std::string &name,ISvcLocator *pSvcLocator);
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

private:
    
    ///@name storegatenames
    ///Names of objects to retrieve and feed into the monitoring AlgTools
    ///@{
    /// Name of combined inner detector tracks container
    SG::ReadHandleKey<TrackCollection> m_CombinedTracksName{this,"CombinedTrackName", "ExtendedTracks", "Combined Track Collection For Monitoring"};
 
    /// Name of trt drift circle container
    SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_TRT_DriftCircleName{this,"TRT_DriftCircleName", "TRT_DriftCircles", "TRT Drift Circle Collection For Monitoring"};

    SG::ReadHandleKey<BCM_RDO_Container> m_BCM_RDOs{this, "BCMRDOName", "BCM_RDOs", "BCM Clusters for Monitoring"};
    SG::ReadHandleKey<PixelRDO_Container> m_PixelRDOs{this, "PixelRDOName", "PixelRDOs", "Pixel Clusters for Monitoring"};
    SG::ReadHandleKey<SCT_RDO_Container> m_SCT_RDOs{this, "SCTRDOName", "SCT_RDOs", "SCT Clusters for monitoring"};
    SG::ReadHandleKey<InDetTimeCollection> m_PixelLVL1ID{this, "PixelLVL1IDName", "PixelLVL1ID", "Pixel LVL1 ID for Monitoring"};
    SG::ReadHandleKey<InDetTimeCollection> m_SCT_LVL1ID{this, "SCTLVL1IDName", "SCT_LVL1ID", "SCT LVL1 ID for Monitoring"};
    SG::ReadHandleKey<InDetTimeCollection> m_TRT_LVL1ID{this, "TRTLVL1IDName", "TRT_LVL1ID", "TRT LVL1 ID for Monitoring"};
    SG::ReadHandleKey<InDetTimeCollection> m_PixelBCID{this, "PixelBCIDName", "PixelBCID", "Pixel BCID for Monitoring"};
    SG::ReadHandleKey<InDetTimeCollection> m_SCT_BCID{this, "SCTBCIDName", "SCT_BCID", "SCT BCID for Monitoring"};
    SG::ReadHandleKey<InDetTimeCollection> m_TRT_BCID{this, "TRTBCIDName", "TRT_BCID", "TRT BCID For Monitoring"};
    SG::ReadHandleKey<ComTime> m_TRT_Phase{this, "TRTPhaseName", "TRT_Phase", "TRT Phase for Monitoring"};
    
    
    ///@}

    //bool m_doTopBottom;   
    Gaudi::Property<bool> m_doTopBottom{this,"doTopBottom", false, "Check differences between top and bottom of detector (seems deprecated...)"};
    Gaudi::Property<bool> m_doPixel{this, "doPixel", true, "Pixel is available in this data"};
    Gaudi::Property<bool> m_doSCT{this, "doSCT", true, "SCT is available in this data"};
    Gaudi::Property<bool> m_doTRT{this, "doTRT", true, "TRT is available in this data"};
   Gaudi::Property<bool> m_doBCM{this, "doBCM", false, "BCM is available in this data"};  
   Gaudi::Property<bool> m_doTiming{this, "doTiming", false, "Timing containers are available (data, not MC)"};
   Gaudi::Property<bool> m_doTRTPhase{this, "doTRTPhase", false, "TRT Phase Information available (cosmics)"};

}; //end of class


#endif
