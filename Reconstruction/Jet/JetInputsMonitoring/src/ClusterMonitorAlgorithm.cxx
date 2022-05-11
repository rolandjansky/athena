/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusterMonitorAlgorithm.h"
#include "AthenaKernel/Units.h"
using Athena::Units::GeV;

ClusterMonitorAlgorithm::ClusterMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
{

}


ClusterMonitorAlgorithm::~ClusterMonitorAlgorithm() {}


StatusCode ClusterMonitorAlgorithm::initialize() {
    using namespace Monitored;
    
    ATH_CHECK( m_CaloClusterContainerKey.initialize() );
    ATH_CHECK( m_EMClusterContainerKey.initialize() );

    // initialize superclass
    ATH_CHECK( AthMonitorAlgorithm::initialize() );

    return StatusCode::SUCCESS;
}


StatusCode ClusterMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    auto nClusters = Monitored::Scalar<float>("nClusters",0.0);
    auto nEMClusters = Monitored::Scalar<float>("nEMClusters",0.0);

    auto eta = Monitored::Scalar<float>("clusterEta",0.0);
    auto phi = Monitored::Scalar<float>("clusterPhi",0.0);
    auto evt = Monitored::Scalar<int>("event num",0);
    auto run = Monitored::Scalar<int>("run",0);
    auto E   = Monitored::Scalar<float>("clusterE",0.0);
    auto ET  = Monitored::Scalar<float>("clusterET",1.0);

    auto emeta = Monitored::Scalar<float>("emclusterEta",0.0);
    auto emphi = Monitored::Scalar<float>("emclusterPhi",0.0);
    auto emE   = Monitored::Scalar<float>("emclusterE",0.0);
    auto emET  = Monitored::Scalar<float>("emclusterET",1.0);

    auto nCells   = Monitored::Scalar<float>("nCells",0.0);
    auto nBadCells= Monitored::Scalar<float>("nBadCells",0.0);
    auto EBadCells= Monitored::Scalar<float>("EBadCells",0.0);
    auto HotRat   = Monitored::Scalar<float>("HotRat",-1.0);
    auto ClusTime = Monitored::Scalar<float>("clusterTime",-999.0);
    auto Isolation= Monitored::Scalar<float>("clusterIsol",-999);
    auto BadLARQFrac = Monitored::Scalar<float>("BadLARQFrac",-999);
    auto EngPos   = Monitored::Scalar<float>("EngPos",-999);
    auto AveLARQ  = Monitored::Scalar<float>("AveLARQ",-999);
    auto AveTileQ = Monitored::Scalar<float>("AveTileQ",-999);

    auto nCellsEM    = Monitored::Scalar<float>("nCellsEM",0.0);
    auto nBadCellsEM = Monitored::Scalar<float>("nBadCellsEM",0.0);
    auto EBadCellsEM = Monitored::Scalar<float>("EBadCellsEM",0.0);
    auto emHotRat    = Monitored::Scalar<float>("emHotRat",-1.0);
    auto emClusTime  = Monitored::Scalar<float>("emclusterTime",-999.0);
    auto emIsolation = Monitored::Scalar<float>("emclusterIsol",-999);
    auto emBadLARQFrac = Monitored::Scalar<float>("emBadLARQFrac",-999);
    auto emEngPos    = Monitored::Scalar<float>("emEngPos",-999);
    auto emAveLARQ   = Monitored::Scalar<float>("emAveLARQ",-999);

    // Declare cutmasks for high leading cell energy fraction
    auto HighHotRat = Monitored::Scalar<bool>("HighHotRat",false);

    // Declare cutmasks for plots with thresholds
    auto Threshold1 = Monitored::Scalar<bool>("Threshold1",false);
    auto Threshold2 = Monitored::Scalar<bool>("Threshold2",false);
    auto Threshold3 = Monitored::Scalar<bool>("Threshold3",false);
    auto Threshold4 = Monitored::Scalar<bool>("Threshold4",false);

    auto EMThreshold1 = Monitored::Scalar<bool>("EMThreshold1",false);
    auto EMThreshold2 = Monitored::Scalar<bool>("EMThreshold2",false);
    auto EMThreshold3 = Monitored::Scalar<bool>("EMThreshold3",false);
    auto EMThreshold4 = Monitored::Scalar<bool>("EMThreshold4",false);
    
    // Access the Clusters via StoreGate
    SG::ReadHandle<xAOD::CaloClusterContainer> clusters(m_CaloClusterContainerKey, ctx);
    if (! clusters.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain CaloTopoCluster Collection with name "<< m_CaloClusterContainerKey);
      return StatusCode::FAILURE;
    }

    // Access the egamma Clusters via StoreGate
    SG::ReadHandle<xAOD::CaloClusterContainer> emclusters(m_EMClusterContainerKey, ctx);
    if (! emclusters.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain CaloTopoCluster Collection with name "<< m_EMClusterContainerKey);
      return StatusCode::FAILURE;
    }

    // Set the values of the monitored variables for the event
    run = GetEventInfo(ctx)->runNumber();
    evt = GetEventInfo(ctx)->eventNumber();

    // Set monitored variables for this event
    nClusters = clusters->size(); 
    fill("ClusterMonitorAllClusters", nClusters); 
    nEMClusters = emclusters->size(); 
    fill("ClusterMonitorAllEMClusters", nEMClusters);

    for (const auto& cluster : *clusters) {
      eta = cluster->eta();
      phi = cluster->phi();
      E   = cluster->e()/GeV;
      ET  = cluster->pt()/GeV;
      // Access CaloTopoCluster moments
      nCells = cluster->numberCells();
      ClusTime = cluster->time();

      nBadCells = cluster->getMomentValue(xAOD::CaloCluster::N_BAD_CELLS); // return value probably 0 if moment does not exist
      EBadCells = cluster->getMomentValue(xAOD::CaloCluster::ENG_BAD_CELLS)/GeV;
      HotRat    = cluster->getMomentValue(xAOD::CaloCluster::ENG_FRAC_MAX); // return value probably 999 if moment does not exist
      (HotRat > 0.9) ? HighHotRat=true : HighHotRat=false;
      Isolation = cluster->getMomentValue(xAOD::CaloCluster::ISOLATION);
      BadLARQFrac = cluster->getMomentValue(xAOD::CaloCluster::BADLARQ_FRAC);
      EngPos    = cluster->getMomentValue(xAOD::CaloCluster::ENG_POS)/GeV;
      AveLARQ   = cluster->getMomentValue(xAOD::CaloCluster::AVG_LAR_Q);
      AveTileQ  = cluster->getMomentValue(xAOD::CaloCluster::AVG_TILE_Q);

      // Fill. First argument is the tool name, all others are the variables to be saved.
      fill("ClusterMonitorAllClusters", eta, phi, E, ET);
      fill("ClusterMonitorExpertPlots", nCells, ClusTime, nBadCells, EBadCells, HotRat, HighHotRat, Isolation, BadLARQFrac, EngPos, AveLARQ, AveTileQ, E, eta, phi);

      // cutmasks for cluster energy thresholds
      Threshold1 = E>m_lowEthresh;
      Threshold2 = E>m_medEthresh;
      Threshold3 = E>m_medhiEthresh;
      Threshold4 = E>m_hiEthresh;
      // Plot occupancies and energies by calorimeter region
      if (eta > 1.4) {
      	fill("ClusterMonitorCalECA",eta,phi,E,Threshold1,Threshold2,Threshold3,Threshold4);
      }
      else if (eta < -1.4) {
        fill("ClusterMonitorCalECC",eta,phi,E,Threshold1,Threshold2,Threshold3,Threshold4);
      }
      else {
        fill("ClusterMonitorCalBAR",eta,phi,E,Threshold1,Threshold2,Threshold3,Threshold4);
      }
    }

    // Repeat for EM clusters - Set monitored variables for this event
    for (const auto& emcluster : *emclusters) {
      emeta = emcluster->eta();
      emphi = emcluster->phi();
      emE   = emcluster->e()/GeV;
      emET  = emcluster->pt()/GeV;
      // Access CaloTopoCluster moments
      nCellsEM = emcluster->numberCells();
      emClusTime = emcluster->time();

      nBadCellsEM = emcluster->getMomentValue(xAOD::CaloCluster::N_BAD_CELLS); // return value probably 0 if moment does not exist
      EBadCellsEM = emcluster->getMomentValue(xAOD::CaloCluster::ENG_BAD_CELLS)/GeV;
      emHotRat    = emcluster->getMomentValue(xAOD::CaloCluster::ENG_FRAC_MAX); // return value probably 999 if moment does not exist
      (emHotRat > 0.9) ? HighHotRat=true : HighHotRat=false;
      emIsolation = emcluster->getMomentValue(xAOD::CaloCluster::ISOLATION);
      emBadLARQFrac = emcluster->getMomentValue(xAOD::CaloCluster::BADLARQ_FRAC);
      emEngPos    = emcluster->getMomentValue(xAOD::CaloCluster::ENG_POS)/GeV;
      emAveLARQ   = emcluster->getMomentValue(xAOD::CaloCluster::AVG_LAR_Q);

      // Fill. First argument is the tool name, all others are the variables to be saved.
      fill("ClusterMonitorAllEMClusters", emeta, emphi, emE, emET);
      fill("ClusterMonitorExpertPlotsEM", nCellsEM, emClusTime, nBadCellsEM, EBadCellsEM, emHotRat, HighHotRat, emIsolation, emBadLARQFrac, emEngPos, emAveLARQ, emE, emeta, emphi);

      // cutmasks for cluster energy thresholds
      EMThreshold1 = emE>m_EMlowEthresh;
      EMThreshold2 = emE>m_EMmedEthresh;
      EMThreshold3 = emE>m_EMmedhiEthresh;
      EMThreshold4 = emE>m_EMhiEthresh;
      // Plot occupancies and energies by calorimeter region
      if (emeta > 1.4) {
      	fill("ClusterMonitorEMECA",emeta,emphi,emE,EMThreshold1,EMThreshold2,EMThreshold3,EMThreshold4);
      }
      else if (emeta < -1.4) {
        fill("ClusterMonitorEMECC",emeta,emphi,emE,EMThreshold1,EMThreshold2,EMThreshold3,EMThreshold4);
      }
      else {
        fill("ClusterMonitorEMBAR",emeta,emphi,emE,EMThreshold1,EMThreshold2,EMThreshold3,EMThreshold4);
      }

    }
    
    return StatusCode::SUCCESS;
}


