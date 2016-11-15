/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHoughAlg.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"

#include "StoreGate/StoreGateSvc.h"

MuonLayerHoughAlg::MuonLayerHoughAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator), 
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  m_layerTool("Muon::MuonLayerHoughTool/MuonLayerHoughTool")
{

  declareProperty("CscPrepDataContainer", m_keyCsc = "CSC_Clusters");
  declareProperty("MdtPrepDataContainer", m_keyMdt = "MDT_DriftCircles");
  declareProperty("RpcPrepDataContainer", m_keyRpc = "RPC_Measurements");
  declareProperty("TgcPrepDataContainer", m_keyTgc = "TGC_Measurements");
  declareProperty("TgcPrepDataContainerPriorBC", m_keyTgcPriorBC = "TGC_MeasurementsPriorBC");
  declareProperty("TgcPrepDataContainerNextBC", m_keyTgcNextBC  = "TGC_MeasurementsNextBC");
  declareProperty("sTgcPrepDataContainer", m_keysTgc = "STGC_Measurements");
  declareProperty("MMPrepDataContainer", m_keyMM = "MM_Measurements");
  declareProperty("PrintSummary", m_printSummary = false );

  declareProperty("MuonLayerScanTool", m_layerTool );
}

MuonLayerHoughAlg::~MuonLayerHoughAlg()
{
}

StatusCode MuonLayerHoughAlg::initialize()
{
  ATH_CHECK( m_layerTool.retrieve() );
  ATH_CHECK( m_printer.retrieve() );

  return StatusCode::SUCCESS; 
}

StatusCode MuonLayerHoughAlg::execute()
{
  

  const Muon::RpcPrepDataContainer* rpcPrds = 0;      
  if( evtStore()->contains<Muon::RpcPrepDataContainer>(m_keyRpc) && evtStore()->retrieve(rpcPrds,m_keyRpc).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve RpcPrepDataContainer " << m_keyRpc );
  }
  const Muon::MdtPrepDataContainer* mdtPrds = 0;      
  if( evtStore()->contains<Muon::MdtPrepDataContainer>(m_keyMdt) && evtStore()->retrieve(mdtPrds,m_keyMdt).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve MdtPrepDataContainer " << m_keyMdt );
  }
  const Muon::TgcPrepDataContainer* tgcPrds = 0;      
  if( evtStore()->contains<Muon::TgcPrepDataContainer>(m_keyTgc) && evtStore()->retrieve(tgcPrds,m_keyTgc).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve TgcPrepDataContainer " << m_keyTgc );
  }
  const Muon::CscPrepDataContainer* cscPrds = 0;      
  if( evtStore()->contains<Muon::CscPrepDataContainer>(m_keyCsc) && evtStore()->retrieve(cscPrds,m_keyCsc).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve CscPrepDataContainer " << m_keyCsc );
  }
  const Muon::sTgcPrepDataContainer* stgcPrds = 0;      
  if( evtStore()->contains<Muon::sTgcPrepDataContainer>(m_keysTgc) && evtStore()->retrieve(stgcPrds,m_keysTgc).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve sTgcPrepDataContainer " << m_keysTgc );
  }
  const Muon::MMPrepDataContainer* mmPrds = 0;      
  if( evtStore()->contains<Muon::MMPrepDataContainer>(m_keyMM) && evtStore()->retrieve(mmPrds,m_keyMM).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve MmPrepDataContainer " << m_keyMM );
  }

  ATH_MSG_VERBOSE("calling layer tool ");
  MuonPatternCombinationCollection* combis = m_layerTool->analyse(mdtPrds,cscPrds,tgcPrds,rpcPrds,stgcPrds,mmPrds);
  if( combis ){
    if (evtStore()->record(combis,"MuonLayerHoughCombis").isFailure()) {
      ATH_MSG_WARNING("Failed to record MuonPatternCombinationCollection at MuonLayerHoughCombis");
    }else{
      ATH_MSG_DEBUG("Recorded MuonPatternCombinationCollection at MuonLayerHoughCombis: size " << combis->size());
      if( m_printSummary || msgLvl(MSG::DEBUG) ){
        msg() << msg().level() << "Number of MuonPatternCombinations  " << combis->size() << std::endl << m_printer->print(*combis) << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
} // execute





StatusCode MuonLayerHoughAlg::finalize()
{
  
  return AthAlgorithm::finalize();
}
