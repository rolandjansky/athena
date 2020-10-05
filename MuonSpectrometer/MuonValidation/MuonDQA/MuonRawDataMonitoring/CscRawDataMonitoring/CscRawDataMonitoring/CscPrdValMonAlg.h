/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscPrdValMonAlg_H
#define CscPrdValMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "CscClusterization/ICscStripFitter.h"


class CscPrdValMonAlg: public AthMonitorAlgorithm  {
    
public:
    CscPrdValMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~CscPrdValMonAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
    
 private:
    
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  SG::ReadHandleKey<Muon::CscStripPrepDataContainer> m_cscPrdKey{this,"CSCPrepRawDataKey","CSC_Measurements","CSC PRDs"};
        
  // Strip fitter
  ToolHandle<ICscStripFitter> m_stripFitter{this, "CSCStripFitter", "CSCStripFitter", "Strip fitter Tool"};

  Gaudi::Property<bool> m_mapxyrz{this,"MapYXandRZ",false};
  Gaudi::Property<size_t> m_cscNoiseCut{this,"NoiseCutADC",50};

};

#endif
