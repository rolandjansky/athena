/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMONITOR_H
#define DATAMONITOR_H

#include "GaudiKernel/Algorithm.h"

#include "StoreGate/StoreGateSvc.h"


#include "RPCgeometry/RPCgeometrySvc.h"
#include "MDTcabling/IMDTcablingSvc.h"
#include "TGCgeometry/TGCgeometrySvc.h"
#include "CSCgeometry/CSCgeometrySvc.h"

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuServiceInterface.h"

#include "TrigmuFast/OnlineSurvey.h"

#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

namespace AIDA{
    class IHistogram1D;
    class IHistogram2D;
}


class DataMonitor : public Algorithm
{
    public:
    DataMonitor(const std::string&, ISvcLocator*);
    ~DataMonitor(){};

    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();

    
    private:
    StoreGateSvc*        m_pStoreGate;      // pointer to Storegate

    ROBDataProviderSvc*  m_pROBDataProvider; // pointer to the ROB data p.
    
    //new tools for the Raw data conversion
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_rpc_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_mdt_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_tgc_converter;
    ToolHandle<Muon::IMuonRawDataProviderTool>   m_csc_converter;

    ServiceHandle<IRPCgeometrySvc> m_rpc_geometry;
    ServiceHandle<TGCgeometrySvc>  m_tgc_geometry;
    ServiceHandle<CSCgeometrySvc>  m_csc_geometry;
    ServiceHandle<IMDTcablingSvc>  m_mdt_cabling;

    MuMessageInterface* m_msg;
    MuServiceInterface* m_svc;
    
    OnlineSurvey*  m_monitor;
    
    private:
    std::vector<int> m_RobIds;
    std::vector<int> m_MdtIds;
    IntegerProperty m_subsysID;
    IntegerProperty m_RoINumber;
    IntegerProperty m_sectorID;
    
    BooleanProperty m_use_calib;
    
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

#endif // DATAMONITOR_H
