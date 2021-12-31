/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUMONITORING_TRIGTAUMONITORALGORITHM_H
#define TRIGTAUMONITORING_TRIGTAUMONITORALGORITHM_H


#include "xAODTau/TauJet.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTrigger/eFexTauRoIContainer.h"

#include "TrigTauInfo.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "CxxUtils/phihelper.h"

class TrigTauMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigTauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigTauMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;



 private:

  std::map<std::string,TrigInfo> m_trigInfo;

  std::map<std::string,TrigInfo> getTrigInfoMap() { return m_trigInfo; }

  TrigInfo getTrigInfo(const std::string&) const;

  void setTrigInfo(const std::string&);

  /*! List of triggers to study */
  std::vector<std::string> m_trigList;

  /*! List of triggers from menu */
  Gaudi::Property<std::vector<std::string>> m_trigInputList{this, "TriggerList", {}};

  /*! navigation method called by executeNavigation */
  StatusCode executeNavigation(const EventContext& ctx, const std::string& trigItem,
                               std::vector<std::pair<const xAOD::TauJet*, const TrigCompositeUtils::Decision*>> &) const;

  void fillRNNInputVars(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec,const std::string& nProng, bool online) const;
  void fillRNNTrack(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec, bool online) const;
  void fillRNNCluster(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec, bool online) const;
  void fillbasicVars(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec, const std::string& nProng, bool online) const;
  void fillDiTauVars(const std::string& trigger, const std::vector<const xAOD::TauJet*>& tau_vec) const;
  void fillL1(const std::string& trigL1Item, const std::vector<const xAOD::EmTauRoI*>& legacyL1rois, const std::vector<const xAOD::eFexTauRoI*>& phase1L1rois, const std::string& nProng)  const;
  void fillDistributions(const EventContext& ctx, const std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >>& pairObjs, const std::string& trigger, float HLTthr) const;
  void fillL1Distributions(const EventContext& ctx, const std::vector< std::pair< const xAOD::TauJet*, const TrigCompositeUtils::Decision * >>& pairObjs, const std::string& trigger, const std::string& trigL1Item, float L1thr) const;
  void fillHLTEfficiencies(const EventContext& ctx,const std::string& trigger, const std::vector<const xAOD::TauJet*>& offline_tau_vec, const std::vector<const xAOD::TauJet*>& online_tau_vec, const std::string& nProng) const;
  void fillDiTauHLTEfficiencies(const EventContext& ctx,const std::string& trigger, const std::vector<const xAOD::TauJet*>& offline_tau_vec, const std::vector<const xAOD::TauJet*>& online_tau_vec) const;
  void fillL1Efficiencies(const EventContext& ctx, const std::vector<const xAOD::TauJet*>& offline_tau_vec, const std::string& nProng, const std::string& trigL1Item, const std::vector<const xAOD::EmTauRoI*>& legacyL1rois, const std::vector<const xAOD::eFexTauRoI*>& phase1L1rois) const;
   

  inline double dR(const double eta1, const double phi1, const double eta2, const double phi2) const
  {
    double deta = std::fabs(eta1 - eta2);
    double dphi = std::fabs(CxxUtils::wrapToPi(phi1-phi2));
    return sqrt(deta*deta + dphi*dphi);
  };

  inline bool HLTMatching(const xAOD::TauJet* offline_tau, std::vector<const xAOD::TauJet*> online_tau_vec, float threshold) const
  {
    for(auto online_tau: online_tau_vec){
      float deltaR = dR(offline_tau->eta(),offline_tau->phi(), online_tau->eta(),online_tau->phi());
      if(deltaR < threshold){
          return true;
      }
    }
    return false;
  };

  
  inline bool legacyL1Matching(const xAOD::TauJet* offline_tau, const xAOD::EmTauRoI* l1roi, float threshold) const
  {
    float deltaR = dR(offline_tau->eta(),offline_tau->phi(), l1roi->eta(),l1roi->phi());
    if(deltaR < threshold){
       return true;
    } else {
       return false;
    }
  }; 

 
  inline bool phase1L1Matching(const xAOD::TauJet* offline_tau, const xAOD::eFexTauRoI* l1roi, float threshold) const
  {
    float deltaR = dR(offline_tau->eta(),offline_tau->phi(), l1roi->eta(),l1roi->phi());
    if(deltaR < threshold){
       return true;
    } else {
       return false;
    }
  };

  SG::ReadHandleKey< xAOD::TauJetContainer> m_offlineTauJetKey { this, "offlineTauJetKey", "TauJets", "Offline taujet container key" };
  SG::ReadHandleKey< xAOD::EmTauRoIContainer> m_legacyl1TauRoIKey { this, "legacyl1TauRoIKey","LVL1EmTauRoIs","Tau Legacy L1 RoI key"};
  SG::ReadHandleKey< xAOD::eFexTauRoIContainer>  m_phase1l1TauRoIKey {this, "phase1l1TauRoIKey", "L1_eTauRoI","Tau Phase1 L1 RoI key"};
  SG::ReadHandleKey< xAOD::TauJetContainer> m_hltTauJetKey { this, "hltTauJetKey", "HLT_TrigTauRecMerged_MVA", "HLT taujet container key" };
  SG::ReadHandleKey< xAOD::TauJetContainer> m_hltTauJetCaloMVAOnlyKey { this, "hltTauJetCaloMVAOnlyKey", "HLT_TrigTauRecMerged_CaloMVAOnly", "HLT taujet container key" };
  SG::ReadHandleKey< xAOD::JetContainer> m_hltSeedJetKey { this, "hltSeedJetKey", "HLT_jet_seed", "HLT jet seed container key" };

};
#endif
