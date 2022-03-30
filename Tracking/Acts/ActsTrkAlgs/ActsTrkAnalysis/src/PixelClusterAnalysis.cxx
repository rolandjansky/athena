/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelClusterAnalysis.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/EventContext.h"

ActsTrk::PixelClusterAnalysis::PixelClusterAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator) {}

StatusCode ActsTrk::PixelClusterAnalysis::initialize() {
    ATH_MSG_DEBUG( "Initializing ActsTrk::PixelClusterAnalysis" );

    ATH_CHECK( m_pixelClusterContainerKey.initialize() );

    ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));

    ATH_CHECK(m_thistSvc.retrieve());

    m_tree = new TTree(m_ntupleName.value().c_str(), "PixelClustersAnalysis");
    ATH_CHECK(m_thistSvc->regTree(m_ntuplePath + m_ntupleName, m_tree));

    if (m_tree) {
        m_tree->Branch("barrelEndcap", &m_barrelEndcap);
        m_tree->Branch("layerDisk", &m_layerDisk);
        m_tree->Branch("phiModule", &m_phiModule);
        m_tree->Branch("etaModule", &m_etaModule);
        m_tree->Branch("isInnermost", &m_isInnermost);
        m_tree->Branch("isNextToInnermost", &m_isNextToInnermost);
        m_tree->Branch("eta", &m_eta);
        m_tree->Branch("globalX", &m_globalX);
        m_tree->Branch("globalY", &m_globalY);
        m_tree->Branch("globalZ", &m_globalZ);
        m_tree->Branch("localX", &m_localX);
        m_tree->Branch("localY", &m_localY);
        m_tree->Branch("localCovXX", &m_localCovXX);
        m_tree->Branch("localCovYY", &m_localCovYY);
        m_tree->Branch("sizeX", &m_sizeX);
        m_tree->Branch("sizeY", &m_sizeY);
    } else {
        ATH_MSG_ERROR("No tree found!");
    }

    m_h_globalZR = new TH2F("h_globalZR","h_globalZR; z [mm]; r [mm]",1500,-3000.,3000,400,0.,400);
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_globalZR->GetName(), m_h_globalZR));

    m_h_etaCluster = new TH1F("m_h_etaCluster","m_h_etaCluster; cluster #eta",100, -5, 5);
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_h_etaCluster->GetName(), m_h_etaCluster));

    return StatusCode::SUCCESS;
}

StatusCode ActsTrk::PixelClusterAnalysis::execute() {
    ATH_MSG_DEBUG(" In ActsTrk::PixelClusterAnalysis::execute()" );

    m_barrelEndcap->clear();
    m_layerDisk->clear();
    m_phiModule->clear();
    m_etaModule->clear();
    m_isInnermost->clear();
    m_isNextToInnermost->clear();
    m_eta->clear();
    m_globalX->clear();
    m_globalY->clear();
    m_globalZ->clear();
    m_localX->clear();
    m_localY->clear();
    m_localCovXX->clear();
    m_localCovYY->clear();
    m_sizeX->clear();
    m_sizeY->clear();

    const EventContext& ctx = Algorithm::getContext();

    SG::ReadHandle<xAOD::PixelClusterContainer> inputPixelClusterContainer( m_pixelClusterContainerKey, ctx );
    if (!inputPixelClusterContainer.isValid()){
        ATH_MSG_FATAL("xAOD::PixelClusterContainer with key " << m_pixelClusterContainerKey.key() << " is not available...");
        return StatusCode::FAILURE;
    }

    for ( auto cluster : *inputPixelClusterContainer ) {
        const Identifier id = m_pixelID->wafer_id(cluster->identifierHash());
        const int pixBrlEc(m_pixelID->barrel_ec(id));
        const int pixLayerDisk(m_pixelID->layer_disk(id));
        const int pixPhiMod(m_pixelID->phi_module(id));
        const int pixEtaMod(m_pixelID->eta_module(id));

        bool isInnermost = (pixLayerDisk==0);
        bool isNextToInnermost = (pixLayerDisk==1) or (pixBrlEc!=0 and pixLayerDisk==2);

        m_barrelEndcap->push_back(pixBrlEc);
        m_layerDisk->push_back(pixLayerDisk);
        m_phiModule->push_back(pixPhiMod);
        m_etaModule->push_back(pixEtaMod);
        m_isInnermost->push_back(int(isInnermost));
        m_isNextToInnermost->push_back(int(isNextToInnermost));

        auto localPos = cluster->localPosition<2>();
        auto localCov = cluster->localCovariance<2>();
        auto globalPos = cluster->globalPosition();

        Amg::Vector3D globalPosition(globalPos(0, 0), globalPos(0, 1), globalPos(0, 2));

        m_eta->push_back(globalPosition.eta());
        m_globalX->push_back(globalPosition.x());
        m_globalY->push_back(globalPosition.y());
        m_globalZ->push_back(globalPosition.z());

        m_localX->push_back(localPos(0, 0));
        m_localY->push_back(localPos(0, 1));

        m_localCovXX->push_back(localCov(0, 0));
        m_localCovYY->push_back(localCov(1, 1));

        m_sizeX->push_back(cluster->channelsInPhi());
        m_sizeY->push_back(cluster->channelsInEta());

        m_h_globalZR->Fill(globalPos.z(), globalPos.perp());
        m_h_etaCluster->Fill(globalPos.eta());
    }

    if (m_tree) {
        m_tree->Fill();
    }

    return StatusCode::SUCCESS;
}
