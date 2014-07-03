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

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();


 private:

  /** storegate location of the MuonPrepDataContainer for all four technologies */
  std::string         m_keyTgc;
  std::string         m_keyTgcPriorBC;
  std::string         m_keyTgcNextBC;
  std::string         m_keyRpc;
  std::string         m_keyCsc;
  std::string         m_keyMdt;
  std::string         m_keysTgc;
  std::string         m_keyMM;

  ToolHandle<Muon::MuonEDMPrinterTool> m_printer;     
  ToolHandle<Muon::MuonLayerHoughTool> m_layerTool;     
  bool m_printSummary;
};


#endif
