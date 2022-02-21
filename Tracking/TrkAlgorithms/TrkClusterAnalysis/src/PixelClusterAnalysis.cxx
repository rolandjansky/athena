/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelClusterAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

PixelClusterAnalysis::PixelClusterAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
m_inputKey("PixelClusters"),
m_pixelID(nullptr),
m_barrelEndcap(nullptr),
m_layerDisk(nullptr),
m_phiModule(nullptr),
m_etaModule(nullptr),
m_isInnermost(nullptr),
m_isNextToInnermost(nullptr),
m_eta(nullptr),
m_globalX(nullptr),
m_globalY(nullptr),
m_globalZ(nullptr),
m_localX(nullptr),
m_localY(nullptr),
m_localCovXX(nullptr),
m_localCovYY(nullptr),
m_sizeX(nullptr),
m_sizeY(nullptr),
m_h_globalZR(nullptr),
m_h_etaCluster(nullptr),
m_tree(nullptr),
m_ntupleFileName("/ntuples/file1"),
m_ntupleDirName("/PixelClusterAnalysis/"),
m_ntupleTreeName("PixelClusterAna"),
m_path("/PixelClusterAnalysis/"),
m_thistSvc("THistSvc", name)
{
    declareProperty("InputKey", m_inputKey);
    declareProperty("NtupleFileName", m_ntupleFileName);
    declareProperty("NtupleDirectoryName", m_ntupleDirName);
    declareProperty("NtupleTreeName", m_ntupleTreeName);
    declareProperty("HistPath", m_path);
}

StatusCode PixelClusterAnalysis::initialize() {
    ATH_MSG_DEBUG( "Initializing PixelClusterAnalysis" );

    ATH_CHECK( m_inputKey.initialize() );

    ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));

    ATH_CHECK(m_thistSvc.retrieve());

    m_tree = new TTree(TString(m_ntupleTreeName), "PixelClusterAna");
    std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
    ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));

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
    ATH_CHECK(m_thistSvc->regHist(m_path + m_h_globalZR->GetName(), m_h_globalZR));

    m_h_etaCluster = new TH1F("m_h_etaCluster","m_h_etaCluster; cluster #eta",100, -5, 5);
    ATH_CHECK(m_thistSvc->regHist(m_path + m_h_etaCluster->GetName(), m_h_etaCluster));

    return StatusCode::SUCCESS;
}

StatusCode PixelClusterAnalysis::execute() {
    ATH_MSG_DEBUG(" In PixelClusterAnalysis::execute()" );

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

    SG::ReadHandle<InDet::PixelClusterContainer> pixelContainer (m_inputKey);
    if(pixelContainer.isValid()) {
        for( const InDet::PixelClusterCollection* pixelCollection : *pixelContainer) {
            if (!pixelCollection) continue;
            for( const InDet::PixelCluster* pCluster : *pixelCollection) {
                const Identifier idColl(pixelCollection->identify());

                const int pixBrlEc(m_pixelID->barrel_ec(idColl));
                const int pixLayerDisk(m_pixelID->layer_disk(idColl));
                const int pixPhiMod(m_pixelID->phi_module(idColl));
                const int pixEtaMod(m_pixelID->eta_module(idColl));
                const int pixIsInnermost(m_pixelID->is_innermost(idColl));
                const int pixIsNextToInnermost(m_pixelID->is_nexttoinnermost(idColl));

                m_barrelEndcap->push_back(pixBrlEc);
                m_layerDisk->push_back(pixLayerDisk);
                m_phiModule->push_back(pixPhiMod);
                m_etaModule->push_back(pixEtaMod);
                m_isInnermost->push_back(pixIsInnermost);
                m_isNextToInnermost->push_back(pixIsNextToInnermost);

                auto localPos = pCluster->localPosition();
                auto localCov = pCluster->localCovariance();
                auto globalPos = pCluster->globalPosition();
                auto width = pCluster->width();

                m_eta->push_back(globalPos.eta());
                m_globalX->push_back(globalPos.x());
                m_globalY->push_back(globalPos.y());
                m_globalZ->push_back(globalPos.z());

                m_localX->push_back(localPos[0]);
                m_localY->push_back(localPos[1]);

                m_localCovXX->push_back(localCov(0, 0));
                m_localCovYY->push_back(localCov(1, 1));

                m_sizeX->push_back(width.colRow()[0]);
                m_sizeY->push_back(width.colRow()[1]);

                m_h_globalZR->Fill(globalPos.z(), globalPos.perp());
                m_h_etaCluster->Fill(globalPos.eta());

            }
        }
    }

    if (m_tree) {
        m_tree->Fill();
    }

    return StatusCode::SUCCESS;
}
