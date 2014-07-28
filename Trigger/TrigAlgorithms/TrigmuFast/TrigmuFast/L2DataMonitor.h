/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L2DATAMONITOR_H
#define L2DATAMONITOR_H


#include "TrigInterfaces/FexAlgo.h"
#include "RegionSelector/IRegSelSvc.h"

#include "TrigmuFast/ROBmapper.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "RPCgeometry/RPCgeometrySvc.h"
#include "TGCgeometry/TGCgeometrySvc.h"
#include "CSCgeometry/CSCgeometrySvc.h"
#include "MDTcabling/IMDTcablingSvc.h"

#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuServiceInterface.h"

#include "Identifier/IdentifierHash.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "GaudiKernel/ToolHandle.h"

namespace AIDA{
    class IHistogram1D;
    class IHistogram2D;
}


class L2DataMonitor : public HLT::FexAlgo
{
    public:
    L2DataMonitor(const std::string&, ISvcLocator*);
    ~L2DataMonitor(){};

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
    HLT::ErrorCode hltFinalize();
    
    private:
    StoreGateSvc*        m_pStoreGate;      // pointer to Storegate
    IRegSelSvc*          m_pRegionSelector; // pointer to Region Selector
    MUON::ROBmapper*     m_pROBmapper;      // pointer to the ROB mapper

    ROBDataProviderSvc*  m_pROBDataProvider; // pointer to the ROB data p.
    
    MuMessageInterface* m_msg;
    MuServiceInterface* m_svc;
    
    std::string m_paramSet;
    
    //new tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_rpc_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_mdt_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_tgc_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_csc_converter;
    
    ServiceHandle<IRPCgeometrySvc> m_rpc_geometry;
    ServiceHandle<TGCgeometrySvc>  m_tgc_geometry;
    ServiceHandle<CSCgeometrySvc>  m_csc_geometry;
    
    ServiceHandle<IMDTcablingSvc> m_mdt_cabling;
    
    private:
    std::vector<int> m_RobIds;
    std::vector<int> m_MdtIds;
    IntegerProperty m_subsysID;
    IntegerProperty m_RoINumber;
    IntegerProperty m_sectorID;
    
    IntegerProperty m_evtToShow;
    
    BooleanProperty m_use_calib;
    
    int m_eventsProcessed;
    
    
    private:
    
    BooleanProperty m_TestBeamRun;

    StringProperty m_ROBmapper_filename;
    
    StringProperty m_histo_path_base;
        
    BooleanProperty m_MUlvl1_info;
    BooleanProperty m_MUtrack_info;
    BooleanProperty m_MUroads_info;
    BooleanProperty m_MUdeco_info;
    BooleanProperty m_MUcont_info;
    BooleanProperty m_MUfit_info;
    BooleanProperty m_MUsag_info;
    BooleanProperty m_MUpt_info;
    StringProperty  m_test_string;

    
    std::vector<uint32_t>       m_robIds;
    std::vector<IdentifierHash> m_listOfMDTIds;
    
    AIDA::IHistogram1D* m_residuals[12];
    AIDA::IHistogram1D* m_mdt_beam_profile[3];
    AIDA::IHistogram1D* m_rpc_beam_profile[3];
    AIDA::IHistogram1D* m_BML_RPC_MDT;
    AIDA::IHistogram1D* m_BOL_RPC_MDT;
    
    
    AIDA::IHistogram2D* m_display;
    AIDA::IHistogram2D* m_MdtDisp;
    AIDA::IHistogram2D* m_RpcDisp;
};

#endif // L2DATAMONITOR_H
