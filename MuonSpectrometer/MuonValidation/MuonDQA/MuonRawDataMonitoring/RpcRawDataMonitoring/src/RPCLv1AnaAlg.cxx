/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcRawDataMonitoring/RPCLv1AnaAlg.h"

//========================================================================================================
RPCLv1AnaAlg::RPCLv1AnaAlg (const std::string& name, ISvcLocator* pSvcLocator)
  :AthMonitorAlgorithm(name,pSvcLocator)
{}

RPCLv1AnaAlg::~RPCLv1AnaAlg() {} 

//========================================================================================================
StatusCode RPCLv1AnaAlg::initialize()
{
  ATH_CHECK( m_MuonContainerKey  .initialize());
  ATH_CHECK( m_rpcPadContainerKey.initialize());
  ATH_CHECK( m_l1RoiContainerKey .initialize(SG::AllowEmpty));

  std::vector<std::string> triggerThrs = {"1", "2", "3", "4", "5", "6"};
  m_TriggerThrGroup = Monitored::buildToolMap<int>(m_tools, "RPCLv1AnaAlg", triggerThrs);

  return AthMonitorAlgorithm::initialize();
}

//========================================================================================================
StatusCode RPCLv1AnaAlg::fillHistograms(const EventContext& ctx) const
{
  using namespace Monitored;
  auto tool = getGroup(m_packageName);
  
  std::vector<double> muPtfullVec     = {};
  std::vector<double> muPtDenVec      = {};


  int nmuon = 0;
  int nmuon_barrel = 0;
  int nmuon_barrel_midium = 0;

  //
  // read muons
  //
  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);

  if(!muons.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::FAILURE;
  }

  //
  // fill muon Pt
  //
  muPtfullVec.reserve(muons->size());
  for(const auto muonItr : *muons) {
    nmuon ++;
    muPtfullVec.push_back(muonItr->pt());
  }
  auto muPtFullCollection = Collection("muPt_full", muPtfullVec);

  //
  // read rois
  // 
  std::vector<double> roiEtaVec       = {};
  std::vector<double> roiBarrelEtaVec = {};
  std::vector<int>    roiBarrelThrVec = {};
  std::vector<const xAOD::MuonRoI*> roisBarrel;
  std::vector< std::vector<const xAOD::MuonRoI*> > roisBarrelThr1(6);

  /* raw LVL1MuonRoIs */
  if (!m_l1RoiContainerKey.empty()) {
    SG::ReadHandle<xAOD::MuonRoIContainer> muonRoIs(m_l1RoiContainerKey, ctx);

    if(!muonRoIs.isValid()){
      ATH_MSG_ERROR("evtStore() does not contain muon L1 ROI Collection with name "<< m_l1RoiContainerKey);
      return StatusCode::FAILURE;
    }

    roiEtaVec.reserve(muonRoIs->size());
    roiBarrelEtaVec.reserve(muonRoIs->size());
    roiBarrelThrVec.reserve(muonRoIs->size());
    roisBarrel.reserve(muonRoIs->size());
    for(const auto roi : *muonRoIs) {
      roiEtaVec.push_back(roi->eta());
      if(roi->getSource() != xAOD::MuonRoI::RoISource::Barrel) {
        continue;
      }

      roiBarrelEtaVec.push_back(roi->eta());
      roiBarrelThrVec.push_back(roi->getThrNumber());
      roisBarrel.push_back(roi);
      //
      // collect roi according to the threshold
      //
      int thr = roi->getThrNumber();
      for (int i_thr=0;i_thr < thr ;i_thr++){
        roisBarrelThr1[i_thr].push_back(roi);
      }
    }

    auto roiEtaCollection       = Collection("roiEta",       roiEtaVec);
    auto roiBarrelEtaCollection = Collection("roiBarrelEta", roiBarrelEtaVec);
    auto roiBarrelThrCollection = Collection("roiBarrelThr", roiBarrelThrVec);
    fill(tool, roiEtaCollection);
    fill(tool, roiBarrelEtaCollection);
    fill(tool, roiBarrelThrCollection);


    //
    // match muon and roi
    //
    muPtDenVec.reserve(muons->size());
    for(const auto muon : *muons) {

      double pt  = muon->pt();
      double eta = muon->eta();
      double phi = muon->phi();

      xAOD::Muon::Quality quality = muon->quality();

      if(std::abs(pt) < m_minPt || std::abs(eta) < m_minEta || std::abs(eta) > m_maxEta) {
        continue;
      }
      nmuon_barrel ++;

      if(!(quality <= m_quality)) {
        continue;
      }
      nmuon_barrel_midium ++;

      // fill numerator 
      auto i_pt           = Scalar<double>("muPt",       pt);
      auto i_eta          = Scalar<double>("muEta",      eta);
      auto i_etaDen       = Scalar<double>("muEtaDen",   eta);
      auto i_phi          = Scalar<double>("muPhi",      phi);
      auto i_phiDen       = Scalar<double>("muPhiDen",      phi);
      auto i_passTrigger  = Scalar<bool>("passTrigger", false);

      for (int i_thr=0;i_thr<6;i_thr++){
        i_passTrigger = false;

        for(const auto& roi : roisBarrelThr1[i_thr]) {

          const double dphi = TVector2::Phi_mpi_pi(roi->phi() - phi);
          const double deta = roi->eta() - eta;
          const double dr   = std::sqrt(dphi*dphi + deta*deta);

          if(dr > m_l1trigMatchWindow) {
            continue;
          }
          i_passTrigger = true;
        }
        
        fill(m_tools[m_TriggerThrGroup.at(std::to_string(i_thr+1))], i_pt, i_eta, i_phi, i_passTrigger);
      }

      //
      // fill denominator 
      //
      muPtDenVec .push_back(muon->pt());
      fill(tool, i_etaDen, i_phiDen);
    }

    auto Nmuon               = Scalar<int>("nMu",               nmuon);
    auto Nmuon_barrel        = Scalar<int>("nMuBarrel",         nmuon_barrel);
    auto Nmuon_barrel_midium = Scalar<int>("nMuBarrel_medium",  nmuon_barrel_midium);

    auto muPtCollection      = Collection("muPtDen",  muPtDenVec);

    //
    // Fill histograms
    //
    fill(tool, Nmuon, Nmuon_barrel, Nmuon_barrel_midium);
    fill(tool, muPtFullCollection);
    fill(tool, muPtCollection);
  }

  return StatusCode::SUCCESS;
}

