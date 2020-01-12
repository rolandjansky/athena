/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHoughAlg.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"

MuonLayerHoughAlg::MuonLayerHoughAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator), 
  m_keyTgc("TGC_Measurements"),
  m_keyRpc("RPC_Measurements"),
  m_keyCsc("CSC_Clusters"),
  m_keyMdt("MDT_DriftCircles"),
  m_keysTgc("STGC_Measurements"),
  m_keyMM("MM_Measurements"),
  m_combis("MuonLayerHoughCombis"),
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  m_layerTool("Muon::MuonLayerHoughTool/MuonLayerHoughTool", this)
{

  declareProperty("CscPrepDataContainer", m_keyCsc);
  declareProperty("MdtPrepDataContainer", m_keyMdt);
  declareProperty("RpcPrepDataContainer", m_keyRpc);
  declareProperty("TgcPrepDataContainer", m_keyTgc);
  declareProperty("TgcPrepDataContainerPriorBC", m_keyTgcPriorBC = "TGC_MeasurementsPriorBC");//unused
  declareProperty("TgcPrepDataContainerNextBC", m_keyTgcNextBC  = "TGC_MeasurementsNextBC");//unused
  declareProperty("sTgcPrepDataContainer", m_keysTgc);
  declareProperty("MMPrepDataContainer", m_keyMM);
  declareProperty("PrintSummary", m_printSummary = false );
  declareProperty("MuonPatternCombinationCollection", m_combis);
  declareProperty("MuonLayerScanTool", m_layerTool );
}

MuonLayerHoughAlg::~MuonLayerHoughAlg()
{
}

StatusCode MuonLayerHoughAlg::initialize()
{
  if (m_layerTool.empty()) {
    ATH_MSG_ERROR("MuonLayerScanTool property is empty");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_layerTool.retrieve() );
  ATH_CHECK( m_printer.retrieve() );

  ATH_CHECK( m_keyRpc.initialize() );
  ATH_CHECK( m_keyMdt.initialize() );
  ATH_CHECK( m_keyTgc.initialize() );
  ATH_CHECK( m_keyCsc.initialize() );
  ATH_CHECK( m_keysTgc.initialize());
  ATH_CHECK( m_keyMM.initialize()  );
  ATH_CHECK( m_combis.initialize() );
  ATH_CHECK( m_houghDataPerSectorVecKey.initialize() );

  return StatusCode::SUCCESS; 
}

StatusCode MuonLayerHoughAlg::execute()
{
  
  const Muon::RpcPrepDataContainer* rpcPrds = GetObject(m_keyRpc);
  const Muon::MdtPrepDataContainer* mdtPrds = GetObject(m_keyMdt);
  const Muon::TgcPrepDataContainer* tgcPrds = GetObject(m_keyTgc);
  const Muon::CscPrepDataContainer* cscPrds = GetObject(m_keyCsc);      
  const Muon::sTgcPrepDataContainer* stgcPrds = GetObject(m_keysTgc);
  const Muon::MMPrepDataContainer* mmPrds =GetObject(m_keyMM);

  ATH_MSG_VERBOSE("calling layer tool ");
  auto [combis, houghDataPerSectorVec] = m_layerTool->analyse(mdtPrds,cscPrds,tgcPrds,rpcPrds,stgcPrds,mmPrds);

  SG::WriteHandle<MuonPatternCombinationCollection> Handle(m_combis);
  if( combis ){
    if (Handle.record(std::move(combis)).isFailure()) {
      ATH_MSG_WARNING("Failed to record MuonPatternCombinationCollection at MuonLayerHoughCombis");
    }else{
      ATH_MSG_DEBUG("Recorded MuonPatternCombinationCollection at MuonLayerHoughCombis: size " << Handle->size());
      if( m_printSummary || msgLvl(MSG::DEBUG) ){
        msg() << msg().level() << "Number of MuonPatternCombinations  " << Handle->size() << std::endl << m_printer->print(*Handle) << endmsg;
      }
    }
  }
  else{
    ATH_MSG_VERBOSE("CombinationCollection "<<m_combis<<" is empty, recording");
    ATH_CHECK(Handle.record(std::make_unique<MuonPatternCombinationCollection>()));
  }

  // write hough data to SG
  SG::WriteHandle<Muon::MuonLayerHoughTool::HoughDataPerSectorVec> handle {m_houghDataPerSectorVecKey};
  if (houghDataPerSectorVec) {
    ATH_CHECK(handle.record(std::move(houghDataPerSectorVec)));
  }
  else{
    ATH_MSG_VERBOSE("HoughDataPerSectorVec "<<m_houghDataPerSectorVecKey<<" is empty, recording");
    ATH_CHECK(handle.record(std::make_unique<Muon::MuonLayerHoughTool::HoughDataPerSectorVec>()));
  }

  m_layerTool->reset();

  return StatusCode::SUCCESS;
} // execute

StatusCode MuonLayerHoughAlg::finalize()
{
  return StatusCode::SUCCESS;
}

