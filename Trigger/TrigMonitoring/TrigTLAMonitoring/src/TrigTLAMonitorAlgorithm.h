/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTLAMONITORING_TRIGTLAMONITORALGORITHM_H
#define TRIGTLAMONITORING_TRIGTLAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/PhotonContainer.h"

class TrigTLAMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigTLAMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigTLAMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  Gaudi::Property<std::vector<std::string>> m_allChains{this,"AllChains",{}};

  SG::ReadHandleKey<xAOD::JetContainer   > m_jetContainerKey   {this,"JetContainerName"   ,"HLT_AntiKt4EMTopoJets_subjesIS_TLA"           , "Jet Container Name"};
  SG::ReadHandleKey<xAOD::JetContainer   > m_pfjetContainerKey {this,"PFJetContainerName" ,"HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf_TLA", "Particle Flow Jet Container Name"};
  SG::ReadHandleKey<xAOD::PhotonContainer> m_photonContainerKey{this,"PhotonContainerName","HLT_egamma_Photons_TLA"                       , "Photon Container Name"};
  SG::ReadHandleKey<xAOD::MuonContainer  > m_muonContainerKey  {this,"MuonContainerName"  ,"HLT_MuonsCB_RoI_TLA"                          , "Muon Container Name"};

  //! Fill kinematic histograms for a given particle of type `T`
  /**
   * The name of the histogram is `prefixVAR_trigName`, where the `VAR` is the name
   * of the variable (ie: `pt`).
   *
   * \tparam T Class of particle to fill (ie: `xAOD::Jet`)
   * \param particles Container with particles whose kinematics to fill
   * \param prefix Prefix for histogram name
   * \param trigName Suffix of the histogram name
   */
  template <class T>
    StatusCode fillParticleHistograms(SG::ReadHandle<DataVector<T>> particles, const std::string& prefix, const std::string& trigName) const;

  //! Fill kinematic histograms for a given particle of type `T`
  /**
   * The name of the histogram is `prefixVAR_trigName`, where the `VAR` is the name
   * of the variable (ie: `pt`).
   *
   * \tparam T Class of particle to fill (ie: `xAOD::Jet`)
   * \param particles Container with particles whose kinematics to fill
   * \param prefix Prefix for histogram name
   * \param trigName Suffix of the histogram name
   */
  StatusCode fillDeltaRHistograms(const xAOD::IParticle* p0, const xAOD::IParticle* p1, const std::string& prefix, const std::string& trigName) const;
};

template <class T>
StatusCode TrigTLAMonitorAlgorithm::fillParticleHistograms(SG::ReadHandle<DataVector<T>> particles, const std::string& prefix, const std::string& trigName) const {

  // histograms
  Monitored::Scalar<int   > n   ("n"+prefix+"_"+trigName,0  );
  Monitored::Scalar<double> pt  (prefix+"pt_"  +trigName,0.0);
  Monitored::Scalar<double> eta (prefix+"eta_" +trigName,0.0);
  Monitored::Scalar<double> phi (prefix+"phi_" +trigName,0.0);

  Monitored::Scalar<double> pt0 (prefix+"0pt_" +trigName,0.0);
  Monitored::Scalar<double> eta0(prefix+"0eta_"+trigName,0.0);
  Monitored::Scalar<double> phi0(prefix+"0phi_"+trigName,0.0);

  // fill
  n = particles->size();
  fill("TrigTLAMonitor", n);

  for(const T* particle : *particles) {
    pt  = particle->pt()/1e3;
    eta = particle->eta();
    phi = particle->phi();
    fill("TrigTLAMonitor",pt );
    fill("TrigTLAMonitor",eta);
    fill("TrigTLAMonitor",phi);
  }

  // fill leading particle
  if(particles->size()>0) {
    pt0 =particles->at(0)->pt ()/1e3;
    eta0=particles->at(0)->eta();
    phi0=particles->at(0)->phi();
    fill("TrigTLAMonitor",pt0 );
    fill("TrigTLAMonitor",eta0);
    fill("TrigTLAMonitor",phi0);
  }

  return StatusCode::SUCCESS;

}

#endif
