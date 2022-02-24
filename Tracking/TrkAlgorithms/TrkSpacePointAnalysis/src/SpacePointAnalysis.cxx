/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SpacePointAnalysis.h"
#include "StoreGate/ReadHandle.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "TTree.h"
#include "TString.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

SpacePointAnalysis::SpacePointAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
m_inputKey("PixelClusters"),
m_pixelID(nullptr),
m_barrelEndcap(nullptr),
m_layerDisk(nullptr),
m_phiModule(nullptr),
m_etaModule(nullptr),
m_sideModule(nullptr),
m_isInnermost(nullptr),
m_isNextToInnermost(nullptr),
m_eta(nullptr),
m_globalX(nullptr),
m_globalY(nullptr),
m_globalZ(nullptr),
m_globalCovXX(nullptr),
m_globalCovYY(nullptr),
m_globalCovZZ(nullptr),
m_globalCovXY(nullptr),
m_globalCovXZ(nullptr),
m_globalCovYX(nullptr),
m_globalCovYZ(nullptr),
m_globalCovZX(nullptr),
m_globalCovZY(nullptr),
m_h_globalZR(nullptr),
m_h_etaSpacePoint(nullptr),
m_tree(nullptr),
m_ntupleFileName("/ntuples/file1"),
m_ntupleDirName("/SpacePointAnalysis/"),
m_ntupleTreeName("SpacePointAna"),
m_path("/SpacePointAnalysis/"),
m_thistSvc("THistSvc", name),
m_usePixel(true)
{
    declareProperty("InputKey", m_inputKey);
    declareProperty("NtupleFileName", m_ntupleFileName);
    declareProperty("NtupleDirectoryName", m_ntupleDirName);
    declareProperty("NtupleTreeName", m_ntupleTreeName);
    declareProperty("HistPath", m_path);
    declareProperty("UsePixel", m_usePixel);
}

StatusCode SpacePointAnalysis::initialize() {
    ATH_MSG_DEBUG( "Initializing SpacePointAnalysis" );

    ATH_CHECK( m_inputKey.initialize() );

    ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));

    ATH_CHECK(m_thistSvc.retrieve());

    m_tree = new TTree(TString(m_ntupleTreeName), "SpacePointAna");
    std::string fullNtupleName = m_ntupleFileName + m_ntupleDirName + m_ntupleTreeName;
    ATH_CHECK(m_thistSvc->regTree(fullNtupleName, m_tree));

    if (m_tree) {
        m_tree->Branch("barrelEndcap", &m_barrelEndcap);
        m_tree->Branch("layerDisk", &m_layerDisk);
        m_tree->Branch("phiModule", &m_phiModule);
        m_tree->Branch("etaModule", &m_etaModule);
        m_tree->Branch("sideModule", &m_sideModule);
        m_tree->Branch("isInnermost", &m_isInnermost);
        m_tree->Branch("isNextToInnermost", &m_isNextToInnermost);
        m_tree->Branch("eta", &m_eta);
        m_tree->Branch("globalX", &m_globalX);
        m_tree->Branch("globalY", &m_globalY);
        m_tree->Branch("globalZ", &m_globalZ);
        m_tree->Branch("globalCovXX", &m_globalCovXX);
        m_tree->Branch("globalCovYY", &m_globalCovYY);
        m_tree->Branch("globalCovZZ", &m_globalCovZZ);
        m_tree->Branch("globalCovXY", &m_globalCovXY);
        m_tree->Branch("globalCovXZ", &m_globalCovXZ);
        m_tree->Branch("globalCovYX", &m_globalCovYX);
        m_tree->Branch("globalCovYZ", &m_globalCovYZ);
        m_tree->Branch("globalCovZX", &m_globalCovZX);
        m_tree->Branch("globalCovZY", &m_globalCovZY);

    } else {
        ATH_MSG_ERROR("No tree found!");
    }

    m_h_globalZR = new TH2F("h_globalZR","h_globalZR; z [mm]; r [mm]",1500,-3000.,3000,1500,0.,1500);
    ATH_CHECK(m_thistSvc->regHist(m_path + m_h_globalZR->GetName(), m_h_globalZR));

    m_h_etaSpacePoint = new TH1F("m_h_etaSpacePoint","m_h_etaSpacePoint; cluster #eta",100, -5, 5);
    ATH_CHECK(m_thistSvc->regHist(m_path + m_h_etaSpacePoint->GetName(), m_h_etaSpacePoint));

    return StatusCode::SUCCESS;
}

StatusCode SpacePointAnalysis::execute() {
    ATH_MSG_DEBUG(" In SpacePointAnalysis::execute()" );

    m_barrelEndcap->clear();
    m_layerDisk->clear();
    m_phiModule->clear();
    m_etaModule->clear();
    m_sideModule->clear();
    m_isInnermost->clear();
    m_isNextToInnermost->clear();
    m_eta->clear();
    m_globalX->clear();
    m_globalY->clear();
    m_globalZ->clear();
    m_globalCovXX->clear();
    m_globalCovYY->clear();
    m_globalCovZZ->clear();
    m_globalCovXY->clear();
    m_globalCovXZ->clear();
    m_globalCovYX->clear();
    m_globalCovYZ->clear();
    m_globalCovZX->clear();
    m_globalCovZY->clear();

    SG::ReadHandle<SpacePointContainer> spContainer (m_inputKey);
    if(spContainer.isValid()) {
        for( const SpacePointCollection* spCollection : *spContainer) {
            if (!spCollection) continue;
            for( const Trk::SpacePoint* spacePoint : *spCollection) {
                const Identifier idColl(spCollection->identify());

                const int brlEc(m_usePixel ? m_pixelID->barrel_ec(idColl) : m_stripID->barrel_ec(idColl));
                const int layerDisk(m_usePixel ? m_pixelID->layer_disk(idColl) : m_stripID->layer_disk(idColl));
                const int phiMod(m_usePixel ? m_pixelID->phi_module(idColl) : m_stripID->phi_module(idColl));
                const int etaMod(m_usePixel ? m_pixelID->eta_module(idColl) : m_stripID->eta_module(idColl));
                const int side(m_usePixel ? 0 : m_stripID->side(idColl));

                const int isInnermost(m_usePixel ? m_pixelID->is_innermost(idColl) : false);
                const int isNextToInnermost(m_usePixel ? m_pixelID->is_nexttoinnermost(idColl) : false);

                m_barrelEndcap->push_back(brlEc);
                m_layerDisk->push_back(layerDisk);
                m_phiModule->push_back(phiMod);
                m_etaModule->push_back(etaMod);
                m_sideModule->push_back(side);
                m_isInnermost->push_back(int(isInnermost));
                m_isNextToInnermost->push_back(int(isNextToInnermost));

                auto globalPos = spacePoint->globalPosition();
                auto globalCov = spacePoint->globCovariance();

                m_eta->push_back(globalPos.eta());
                m_globalX->push_back(globalPos.x());
                m_globalY->push_back(globalPos.y());
                m_globalZ->push_back(globalPos.z());

                m_globalCovXX->push_back(globalCov(0,0));
                m_globalCovYY->push_back(globalCov(1,1));
                m_globalCovZZ->push_back(globalCov(2,2));
                m_globalCovXY->push_back(globalCov(0,1));
                m_globalCovXZ->push_back(globalCov(0,2));
                m_globalCovYX->push_back(globalCov(1,0));
                m_globalCovYZ->push_back(globalCov(1,2));
                m_globalCovZX->push_back(globalCov(2,0));
                m_globalCovZY->push_back(globalCov(2,1));

                m_h_globalZR->Fill(globalPos.z(), globalPos.perp());
                m_h_etaSpacePoint->Fill(globalPos.eta());

            }
        }
    }

    if (m_tree) {
        m_tree->Fill();
    }

    return StatusCode::SUCCESS;
}
