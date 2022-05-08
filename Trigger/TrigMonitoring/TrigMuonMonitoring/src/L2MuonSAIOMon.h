/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORING_L2MUONSAIOMON_H
#define TRIGMUONMONITORING_L2MUONSAIOMON_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"

/*
This is a class for monitoring L2MuonSAIO.
 */
class L2MuonSAIOMon : public TrigMuonMonitorAlgorithm{

 public:
  L2MuonSAIOMon(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;
  virtual StatusCode fillVariablesPerOfflineMuonPerChain(const EventContext& ctx, const xAOD::Muon* mu, const std::string &chain) const override;

 private:
  Gaudi::Property<std::vector<float> > m_monitored_chains_plateau {this, "Plateaus", {}, "List of plateaus of measured trigger"};
  std::map<std::string, double> m_plateaus {};
  SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_L2MuonCBIOContainerKey {this, "L2CombinedMuonContainerName_IO", "HLT_MuonL2CBInfoIOmode", "L2MuonCBIO container"};
  StatusCode matchL2IO_wContainer(const EventContext &ctx, const std::string &chain, std::vector< const xAOD::L2CombinedMuon* > &Trig_L2IOobjects) const;
  StatusCode L2OverlapRemover( std::vector< const xAOD::L2CombinedMuon* > matchSA_L2IOobjects, std::vector< bool > &isoverlap, std::vector< bool > &passOR ) const;
  bool isOverlap( const xAOD::L2CombinedMuon* matchSA_L2IOobject1, const xAOD::L2CombinedMuon* matchSA_L2IOobject2 ) const;
  StatusCode chooseBestMuon( std::vector< const xAOD::L2CombinedMuon* > matchSA_L2IOobjects, std::vector< bool > &passOR, std::vector< unsigned int > &mucombResult ) const;
  bool muCombHypo_TDTworkaround( const std::string &chain, const std::vector< const xAOD::L2CombinedMuon* >& Trig_L2IOobjects, std::vector< bool > &pass_muCombHypo ) const;
  bool isPassedmuCombHypo( const std::string &chain, const xAOD::L2CombinedMuon* Trig_L2IOobjects ) const;
  StatusCode decision_ptthreshold( const std::string &chain, std::vector< float > &my_EtaBins, std::vector< float > &my_muCombThres,
                                   bool &my_pikCuts, float &my_maxPtToApplyPik, float &my_chi2MaxID ) const;

  const xAOD::L2CombinedMuon* searchL2InsideOut(const EventContext &ctx, const xAOD::Muon *mu, const std::string& trigger) const;
  const xAOD::Muon* searchTagOfflineMuon( const EventContext& ctx, const xAOD::Muon* probe ) const;

  static inline std::tuple<float,float,float> L2ORPosForMatchFunc(const xAOD::L2StandAloneMuon *trig);
  static inline std::tuple<float,float,float> L2ORPosForMatchFunc(const xAOD::L2CombinedMuon *trig);

  // these threshold is same as the L2OverlapRemover in TrigMufastHypoTool
  Gaudi::Property< std::vector<float> > m_etaBins {this, "EtaBins", {0, 0.9, 1.1, 1.9, 2.1, 9.9}, "eta bins of DR and mass thresholds"};
  Gaudi::Property< std::vector<float> > m_dRCBThres {this, "DRThres", {0.002, 0.001, 0.002, 0.002, 0.002}, "DR threshold of L2CB"};
  Gaudi::Property< std::vector<float> > m_dRbySAThres {this, "dRbySAThres", {0}, "mufast DR threshold of L2CB"};
  Gaudi::Property< std::vector<float> > m_massCBThres {this, "MassThres", {0.004, 0.002, 0.006, 0.006, 0.006}, "mass threshold of L2CB"};

  // config of L2OverlapRemover for L2Inside-Out
  bool m_RequireDR = true;
  bool m_RequireDRbySA  = false;
  bool m_RequireMass = true;
  bool m_RequireSameSign = true;

};

#endif //TRIGMUONMONITORING_L2MUONSAIOMON_H
