/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_MUONROICHAINFINDTOOL_H 
#define TRIGGER_MUONROICHAINFINDTOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigMuonCoinHierarchy/IMuonRoiChainFindTool.h"

namespace Trigger {

class MuonRoiChainFindTool : virtual public IMuonRoiChainFindTool, virtual public AthAlgTool {
 public:
  /** Constructor */
  MuonRoiChainFindTool(const std::string& t, const std::string& n, const IInterface* p);

  /** Destructor */
  virtual ~MuonRoiChainFindTool(void) {};

  /** Query inteface */
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  /** Provide InterfaceID */
  static const InterfaceID& interfaceID() { return IMuonRoiChainFindTool::interfaceID(); }

  /** Initialize */
  virtual StatusCode initialize();
  /** Finalize */
  virtual StatusCode finalize();

  /** Find MuonRoiChains */
  virtual StatusCode find(std::vector<MuonRoiChain*>* chains);

 private:
  /** Get Bunch Crossing Id from CTP */ 
  StatusCode getBunchCrossingId();

  /** Find MuonRoiChains which have Muon_ROI */
  StatusCode createChainsWithMuon_ROI();
  /** Find MuonRoiChains which do no have Muon_ROI but do have MuCTPI_RDO dataWord */
  StatusCode associateAndCreateChainsWithMuCTPI_RDO_dataWord(); 
  /** Find MuonRoiChains which have TGC Sector Logic only */
  StatusCode associateAndCreateChainsWithTgcCoinData();

  /** Show WARNING message on a Muon_ROI for debugging */
  void showWARNINGforSetMuon_ROI(const Muon_ROI* muonRoi, int roiIndex);
  /** Show WARNING message on a MuCTPI_RDO dataWord for debugging */
  void showWARNINGforSetMuCTPI_RDO_dataWord(uint32_t muctpiRdoDataWord, int dataWordIndex);
  /** Show WARNING message on a TgcCoinData (Sector Logic) */
  void showWARNINGforSetTgcCoinData(const Muon::TgcCoinData* tgcSL);

  /** Pointer of MuonRoiChains */
  std::vector<MuonRoiChain*>* m_chains;

  /** LVL1_ROI name */
  std::string m_LVL1_ROIName;
  /** MuCTPI_RDO Name */
  std::string m_MuCTPI_RDOName;
  /** TgcCoinDataContainer name */
  std::string m_tgcCoinDataContainerName;

  /** Bunch Crossing Id from CTP */
  int m_bunchCrossingId;  

  /** Flag to skip threshold number comparison between Muon_ROI and MuCTPI_RDO */ 
  bool m_skipRoiThresholdComparison; 
};

} // end of namespace Trigger

#endif // TRIGGER_MUONROICHAINFINDTOOL_H
