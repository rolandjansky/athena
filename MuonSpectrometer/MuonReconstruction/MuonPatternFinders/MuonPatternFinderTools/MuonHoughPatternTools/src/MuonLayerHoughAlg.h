/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLAYERHOUGHALG_H
#define MUONLAYERHOUGHALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

class MuonLayerHoughAlg : public AthAlgorithm
{
 public:
  MuonLayerHoughAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MuonLayerHoughAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  template<class T> 
  const T* GetObject(SG::ReadHandleKey<T> &key);

  SG::ReadHandleKey<Muon::TgcPrepDataContainer>   m_keyTgc{this,"TgcPrepDataContainer","TGC_Measurements"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer>   m_keyRpc{this,"RpcPrepDataContainer","RPC_Measurements"};
  SG::ReadHandleKey<Muon::CscPrepDataContainer>   m_keyCsc{this,"CscPrepDataContainer","CSC_Clusters"};
  SG::ReadHandleKey<Muon::MdtPrepDataContainer>   m_keyMdt{this,"MdtPrepDataContainer","MDT_DriftCircles"};
  SG::ReadHandleKey<Muon::sTgcPrepDataContainer>  m_keysTgc{this,"sTgcPrepDataContainer","STGC_Measurements"};
  SG::ReadHandleKey<Muon::MMPrepDataContainer>    m_keyMM{this,"MMPrepDataContainer","MM_Measurements"};

  SG::WriteHandleKey<MuonPatternCombinationCollection> m_combis{this,"MuonPatternCombinationCollection","MuonLayerHoughCombis"};
  SG::WriteHandleKey<Muon::MuonLayerHoughTool::HoughDataPerSectorVec> m_houghDataPerSectorVecKey {this, 
    "Key_MuonLayerHoughToolHoughDataPerSectorVec", "HoughDataPerSectorVec", "HoughDataPerSectorVec key"};
  ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this,"printerTool","Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
  ToolHandle<Muon::MuonLayerHoughTool> m_layerTool{this,"MuonLayerScanTool","Muon::MuonLayerHoughTool/MuonLayerHoughTool"};
  Gaudi::Property<bool> m_printSummary{this,"PrintSummary",false};
};


template<class T>
const T* MuonLayerHoughAlg::GetObject(SG::ReadHandleKey<T> &key){
  SG::ReadHandle<T> handle( key);
  if( handle.isPresent() && !handle.isValid()) {
    ATH_MSG_WARNING("MuonLayerHoughAlg Cannot retrieve " << handle.key() );
    return nullptr;
  }
  return handle.cptr();
}

#endif
