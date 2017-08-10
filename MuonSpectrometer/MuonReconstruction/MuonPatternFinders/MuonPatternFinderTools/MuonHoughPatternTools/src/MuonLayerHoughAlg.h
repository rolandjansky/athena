/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLAYERHOUGHALG_H
#define MUONLAYERHOUGHALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h"


class MsgStream;
class StoreGateSvc;

class MuonLayerHoughAlg : public AthAlgorithm
{
 public:
  MuonLayerHoughAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MuonLayerHoughAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;


 private:
  template<class T> 
  const T* GetObject(SG::ReadHandleKey<T> &key);


  /** storegate location of the MuonPrepDataContainer for all four technologies */
  SG::ReadHandleKey<Muon::TgcPrepDataContainer>   m_keyTgc;
  std::string         m_keyTgcPriorBC;//unused
  std::string         m_keyTgcNextBC;//unused
  SG::ReadHandleKey<Muon::RpcPrepDataContainer>   m_keyRpc;
  SG::ReadHandleKey<Muon::CscPrepDataContainer>   m_keyCsc;
  SG::ReadHandleKey<Muon::MdtPrepDataContainer>   m_keyMdt;
  SG::ReadHandleKey<Muon::sTgcPrepDataContainer>  m_keysTgc;
  SG::ReadHandleKey<Muon::MMPrepDataContainer>    m_keyMM;

  SG::WriteHandleKey<MuonPatternCombinationCollection> m_combis;
  ToolHandle<Muon::MuonEDMPrinterTool> m_printer;     
  ToolHandle<Muon::MuonLayerHoughTool> m_layerTool;     
  bool m_printSummary;
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
