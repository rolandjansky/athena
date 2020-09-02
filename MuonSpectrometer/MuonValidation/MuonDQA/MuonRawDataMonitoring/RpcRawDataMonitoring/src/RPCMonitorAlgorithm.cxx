/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcRawDataMonitoring/RPCMonitorAlgorithm.h"

//========================================================================================================
RPCMonitorAlgorithm::RPCMonitorAlgorithm (const std::string& name, ISvcLocator* pSvcLocator)
  :AthMonitorAlgorithm(name,pSvcLocator),
  m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool")
{
  //  Declare the properties
  declareProperty("MinRoIDR",    m_minRoIDR = 0.3);
  declareProperty("MinPt",       m_minPt    = 2.0e3);
  declareProperty("MinEta",      m_minEta   = 0.0);
  declareProperty("MaxEta",      m_maxEta   = 1.05);
  declareProperty("MuQuality",   m_quality  = 1);
}

RPCMonitorAlgorithm::~RPCMonitorAlgorithm() {} 

//========================================================================================================
StatusCode RPCMonitorAlgorithm::initialize()
{
  ATH_CHECK( m_MuonContainerKey  .initialize());
  ATH_CHECK( m_rpcPadContainerKey.initialize());
  ATH_CHECK( m_l1RoiContainerKey .initialize());

  ATH_CHECK( m_muonSelectionTool.retrieve());

  return AthMonitorAlgorithm::initialize();
}

//========================================================================================================
StatusCode RPCMonitorAlgorithm::fillHistograms(const EventContext& ctx) const
{
  using namespace Monitored;
  
  //Declare the quantities which should be monitored
  auto run = Monitored::Scalar<int>("run", 0);
  auto nMu = Monitored::Scalar<int>("nMu", 0);
  auto nMuBarrel = Monitored::Scalar<int>("nMuBarrel", 0);

  std::vector<double> muPtVec         = {};
  std::vector<double> roiEtaVec       = {};
  std::vector<double> roiBarrelEtaVec = {};

  std::vector<int> roiBarrelThrVec = {};

  ATH_MSG_VERBOSE("Events processed: "<<ctx.evt());

  run = GetEventInfo(ctx)->runNumber();
  
  //
  // read muons
  //
  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);

  if(!muons.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::FAILURE;
  }
  
  nMu = muons->size();

  //
  // fill muon Pt
  //
  for(const auto& muonItr : *muons) {
    muPtVec.push_back(muonItr->pt());
  }
  auto muPtCollection = Collection("muPtCollection", muPtVec);

  //
  // read rois
  // 
  SG::ReadHandle<xAOD::MuonRoIContainer> muonRoIs(m_l1RoiContainerKey, ctx);
  if(!muonRoIs.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain muon L1 ROI Collection with name "<< m_l1RoiContainerKey);
    return StatusCode::FAILURE;
  }

  std::vector<const xAOD::MuonRoI*> roisBarrel;
  std::vector<const xAOD::MuonRoI*> roisBarrelThr1;

  for(const auto& roi : *muonRoIs) {
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
    if(thr >= 1) roisBarrelThr1.push_back(roi);
  }
  auto roiEtaCollection       = Collection("roiEtaCollection", roiEtaVec);
  auto roiBarrelEtaCollection = Collection("roiBarrelEtaCollection", roiBarrelEtaVec);
  auto roiBarrelThrCollection = Collection("roiBarrelThrCollection", roiBarrelThrVec);

  //
  // match muon and roi
  //
  for(const auto& muon : *muons) {

    float pt  = muon->pt();
    float eta = muon->eta();
    float phi = muon->phi();

    xAOD::Muon::Quality quality = m_muonSelectionTool->getQuality(*muon);

    if(std::abs(pt) < m_minPt || std::abs(eta) < m_minEta || std::abs(eta) > m_maxEta) {
      continue;
    }
    if(!(quality <= m_quality)) {
      continue;
    }
    // to add impact parameter cuts
    nMuBarrel++;
    //
    // to fill denominator 
    //
    auto ptDen  = Monitored::Scalar<float>("ptDen",  pt);
    auto etaDen = Monitored::Scalar<float>("etaDen", eta);
    auto phiDen = Monitored::Scalar<float>("phiDen", phi);
    auto isPassed = Monitored::Scalar<bool>("isPassed", false);

    for(const auto& roi : roisBarrelThr1) {

      const double dphi = TVector2::Phi_mpi_pi(roi->phi() - phi);
      const double deta = roi->eta() - eta;
      const double dr   = std::sqrt(dphi*dphi + deta*deta);

      if(dr > m_minRoIDR) {
        continue;
      }
      isPassed = true;
    }
    
    fill("RPCMonitorAlgorithm", ptDen, etaDen, phiDen, isPassed);

    if(isPassed) {
      auto ptNumThr1  = Monitored::Scalar<float>("ptNumThr1",  pt);
      auto etaNumThr1 = Monitored::Scalar<float>("etaNumThr1", eta);
      auto phiNumThr1 = Monitored::Scalar<float>("phiNumThr1", phi);
      fill("RPCMonitorAlgorithm", ptNumThr1, etaNumThr1, phiNumThr1);
    }
  }

  //
  // read rpcPad
  //
  // * Problematic for MuonRPC_CablingSvc...
  /*SG::ReadHandle<RpcPadContainer> rpcRDO(m_rpcPadContainerKey, ctx);
  if(!rpcRDO.isValid()) {
    ATH_MSG_ERROR("evtStore() does not contain RpcPadContainer with name "<<m_rpcPadContainerKey);
    return StatusCode::FAILURE;
  }

  RpcPadContainer::const_iterator pad = rpcRDO->begin();
  RpcPadContainer::const_iterator endpad = rpcRDO->end();
  for(; pad != endpad; pad++) {
    ATH_MSG_INFO("Got pad back " << pad->sector());
  }*/

  //
  // Fill histograms
  //
  auto tool = getGroup("RPCMonitorAlgorithm");

  fill(tool, muPtCollection);
  fill(tool, roiEtaCollection);
  fill(tool, roiBarrelEtaCollection);
  fill(tool, roiBarrelThrCollection);

  fill(tool, run);
  fill(tool, nMu);
  fill(tool, nMuBarrel);
  
  return StatusCode::SUCCESS;
}
