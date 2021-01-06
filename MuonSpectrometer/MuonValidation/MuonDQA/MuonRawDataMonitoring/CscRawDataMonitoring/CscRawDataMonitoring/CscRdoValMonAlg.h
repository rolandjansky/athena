/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscRdoValMonAlg_H
#define CscRdoValMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/ToolHandle.h"

#include "MuonRDO/CscRawDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

class CscRdoValMonAlg: public AthMonitorAlgorithm
{

public:
  CscRdoValMonAlg (const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~CscRdoValMonAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;

private:
  Gaudi::Property<size_t> m_cscNoiseCut{this, "cscNoiseCut", 50};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<Muon::ICSC_RDO_Decoder> m_cscRdoDecoderTool{this,"CscRDODecoder","Muon::CscRDO_Decoder","Muon::CscRDO_Decoder"};
  SG::ReadHandleKey<CscRawDataContainer> m_cscRdoKey{this,"CSCRawDataKey","CSCRDO","CSC RDO"};

};

#endif
