/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ClusterAnalysis.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/EventContext.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

SCT_ClusterAnalysis::SCT_ClusterAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator) {}

StatusCode SCT_ClusterAnalysis::initialize() {
    ATH_MSG_DEBUG( "Initializing SCT_ClusterAnalysis" );

    ATH_CHECK( m_inputKey.initialize() );

    ATH_CHECK(detStore()->retrieve(m_stripID, "SCT_ID"));

    ATH_CHECK(m_thistSvc.retrieve());

    m_tree = new TTree(TString(m_ntupleTreeName.value()), "SCT_ClusterAna");
    std::string fullNtupleName = m_ntupleFileName.value() + m_ntupleDirName.value() + m_ntupleTreeName.value();
    ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));

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
        m_tree->Branch("localY", &m_localY);
        m_tree->Branch("localCovXX", &m_localCovXX);
        m_tree->Branch("localCovYY", &m_localCovYY);
        m_tree->Branch("localCovXY", &m_localCovXY);
        m_tree->Branch("localCovYX", &m_localCovYX);
        m_tree->Branch("sizeX", &m_sizeX);
        m_tree->Branch("sizeY", &m_sizeY);
    } else {
        ATH_MSG_ERROR("No tree found!");
    }

    m_h_globalZR = new TH2F("m_h_globalZR","m_h_globalZR; z [mm]; r [mm]",1500,-3000.,3000,400,400.,1200);
    ATH_CHECK(m_thistSvc->regHist(m_path.value() + m_h_globalZR->GetName(), m_h_globalZR));

    m_h_etaCluster = new TH1F("m_h_etaCluster","m_h_etaCluster; cluster #eta",100, -5, 5);
    ATH_CHECK(m_thistSvc->regHist(m_path.value() + m_h_etaCluster->GetName(), m_h_etaCluster));

    return StatusCode::SUCCESS;
}

StatusCode SCT_ClusterAnalysis::execute() {
    ATH_MSG_DEBUG(" In SCT_ClusterAnalysis::execute()" );

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
    m_localY->clear();
    m_localCovXX->clear();
    m_localCovYY->clear();
    m_localCovXY->clear();
    m_localCovYX->clear();
    m_sizeX->clear();
    m_sizeY->clear();

    const EventContext& ctx = Algorithm::getContext();

    SG::ReadHandle<InDet::SCT_ClusterContainer> stripContainer (m_inputKey, ctx);
    if( stripContainer.isValid() ) {
        for( const InDet::SCT_ClusterCollection* stripCollection : *stripContainer ) {
            if( !stripCollection ) continue;
            for( const InDet::SCT_Cluster* sCluster : *stripCollection ) {

                const Identifier idColl(stripCollection->identify());
                const int sctBrlEc(m_stripID->barrel_ec(idColl));
                const int sctLayerDisk(m_stripID->layer_disk(idColl));
                const int sctPhiMod(m_stripID->phi_module(idColl));
                const int sctEtaMod(m_stripID->eta_module(idColl));
                const int sctSideMod(m_stripID->side(idColl));

                m_barrelEndcap->push_back(sctBrlEc);
                m_layerDisk->push_back(sctLayerDisk);
                m_phiModule->push_back(sctPhiMod);
                m_etaModule->push_back(sctEtaMod);
                m_sideModule->push_back(sctSideMod);

                auto localPos = sCluster->localPosition();
                auto localCov = sCluster->localCovariance();
                auto globalPos = sCluster->globalPosition();
                auto width = sCluster->width();

                m_eta->push_back(globalPos.eta());
                m_globalX->push_back(globalPos.x());
                m_globalY->push_back(globalPos.y());
                m_globalZ->push_back(globalPos.z());

                m_localX->push_back(localPos[0]);
                m_localY->push_back(localPos[1]);

                m_localCovXX->push_back(localCov(0, 0));
                m_localCovXY->push_back(localCov(0, 1));
                m_localCovYY->push_back(localCov(1, 1));
                m_localCovYX->push_back(localCov(1, 0));

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
