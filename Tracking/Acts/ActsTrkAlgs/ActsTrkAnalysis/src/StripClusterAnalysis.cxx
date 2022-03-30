/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "StripClusterAnalysis.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/EventContext.h"

ActsTrk::StripClusterAnalysis::StripClusterAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator) {}

StatusCode ActsTrk::StripClusterAnalysis::initialize() {
    ATH_MSG_DEBUG( "Initializing ActsTrk::StripClusterAnalysis" );

    ATH_CHECK( m_stripClusterContainerKey.initialize() );

    ATH_CHECK(detStore()->retrieve(m_stripID, "SCT_ID"));

    ATH_CHECK(m_thistSvc.retrieve());

    m_tree = new TTree(m_ntupleName.value().c_str(), "StripClustersAnalysis");
    ATH_CHECK(m_thistSvc->regTree(m_ntuplePath + m_ntupleName, m_tree));

    if (m_tree) {
        m_tree->Branch("barrelEndcap", &m_barrelEndcap);
        m_tree->Branch("layerDisk", &m_layerDisk);
        m_tree->Branch("phiModule", &m_phiModule);
        m_tree->Branch("etaModule", &m_etaModule);
        m_tree->Branch("sideModule", &m_sideModule);
        m_tree->Branch("eta", &m_eta);
        m_tree->Branch("globalX", &m_globalX);
        m_tree->Branch("globalY", &m_globalY);
        m_tree->Branch("globalZ", &m_globalZ);
        m_tree->Branch("localX", &m_localX);
        m_tree->Branch("localCovXX", &m_localCovXX);
        m_tree->Branch("sizeX", &m_sizeX);
    } else {
        ATH_MSG_ERROR("No tree found!");
    }

    m_h_globalZR = new TH2F("m_h_globalZR","m_h_globalZR; z [mm]; r [mm]",1500,-3000.,3000,400,400.,1200);
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_globalZR->GetName(), m_h_globalZR));

    m_h_etaCluster = new TH1F("m_h_etaCluster","m_h_etaCluster; cluster #eta",100, -5, 5);
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_etaCluster->GetName(), m_h_etaCluster));

    return StatusCode::SUCCESS;
}

StatusCode ActsTrk::StripClusterAnalysis::execute() {
    ATH_MSG_DEBUG(" In ActsTrk::StripClusterAnalysis::execute()" );

    m_barrelEndcap->clear();
    m_layerDisk->clear();
    m_phiModule->clear();
    m_etaModule->clear();
    m_sideModule->clear();
    m_eta->clear();
    m_globalX->clear();
    m_globalY->clear();
    m_globalZ->clear();
    m_localX->clear();
    m_localCovXX->clear();
    m_sizeX->clear();

    const EventContext& ctx = Algorithm::getContext();

    SG::ReadHandle<xAOD::StripClusterContainer> inputStripClusterContainer( m_stripClusterContainerKey, ctx );
    if (!inputStripClusterContainer.isValid()){
        ATH_MSG_FATAL("xAOD::StripClusterContainer with key " << m_stripClusterContainerKey.key() << " is not available...");
        return StatusCode::FAILURE;
    }

    for ( auto cluster : *inputStripClusterContainer ) {
        const Identifier id = m_stripID->wafer_id(cluster->identifierHash());
        const int sctBrlEc(m_stripID->barrel_ec(id));
        const int sctLayerDisk(m_stripID->layer_disk(id));
        const int sctPhiMod(m_stripID->phi_module(id));
        const int sctEtaMod(m_stripID->eta_module(id));
        const int sctSideMod(m_stripID->side(id));

        m_barrelEndcap->push_back(sctBrlEc);
        m_layerDisk->push_back(sctLayerDisk);
        m_phiModule->push_back(sctPhiMod);
        m_etaModule->push_back(sctEtaMod);
        m_sideModule->push_back(sctSideMod);

        auto localPos = cluster->localPosition<1>();
        auto localCov = cluster->localCovariance<1>();
        auto globalPos = cluster->globalPosition();

        Amg::Vector3D globalPosition(globalPos(0, 0), globalPos(0, 1), globalPos(0, 2));

        m_eta->push_back(globalPosition.eta());
        m_globalX->push_back(globalPosition.x());
        m_globalY->push_back(globalPosition.y());
        m_globalZ->push_back(globalPosition.z());

        m_localX->push_back(localPos(0, 0));

        m_localCovXX->push_back(localCov(0, 0));

        m_sizeX->push_back(cluster->channelsInPhi());

        m_h_globalZR->Fill(globalPosition.z(), globalPosition.perp());
        m_h_etaCluster->Fill(globalPosition.eta());

    }

    if (m_tree) {
        m_tree->Fill();
    }

    return StatusCode::SUCCESS;
}
