/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SpacePointAnalysis.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/EventContext.h"
#include "ActsUtils/ContainerAccessor.h"

#include "xAODMeasurementBase/UncalibratedMeasurementContainer.h"

ActsTrk::SpacePointAnalysis::SpacePointAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator) {}

StatusCode ActsTrk::SpacePointAnalysis::initialize() {
    ATH_MSG_DEBUG( "Initializing ActsTrk::SpacePointAnalysis" );

    ATH_CHECK( m_spacePointContainerKey.initialize() );
    ATH_CHECK( m_pixelClusterContainerKey.initialize(m_usePixel) );
    ATH_CHECK( m_stripClusterContainerKey.initialize(not m_usePixel) );

    if (m_usePixel)
        ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));
    else
        ATH_CHECK(detStore()->retrieve(m_stripID, "SCT_ID"));

    ATH_CHECK(m_thistSvc.retrieve());

    m_tree = new TTree(m_ntupleName.value().c_str(), "SpacePointAnalysis");
    ATH_CHECK(m_thistSvc->regTree(m_ntuplePath + m_ntupleName, m_tree));

    if (m_tree) {
        m_tree->Branch("barrelEndcap", &m_barrelEndcap);
        m_tree->Branch("layerDisk", &m_layerDisk);
        m_tree->Branch("phiModule", &m_phiModule);
        m_tree->Branch("etaModule", &m_etaModule);
        m_tree->Branch("sideModule", &m_sideModule);
        m_tree->Branch("isInnermost", &m_isInnermost);
        m_tree->Branch("isNextToInnermost", &m_isNextToInnermost);
        m_tree->Branch("isOverlap", &m_isOverlap);
        m_tree->Branch("eta", &m_eta);
        m_tree->Branch("globalX", &m_globalX);
        m_tree->Branch("globalY", &m_globalY);
        m_tree->Branch("globalZ", &m_globalZ);

    } else {
        ATH_MSG_ERROR("No tree found!");
    }

    m_h_globalZR = new TH2F("h_globalZR","h_globalZR; z [mm]; r [mm]",1500,-3000.,3000,1500,0.,1500);
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_histName + "/" + m_h_globalZR->GetName(), m_h_globalZR));

    m_h_etaSpacePoint = new TH1F("m_h_etaSpacePoint","m_h_etaSpacePoint; space point #eta",100, -5, 5);
    ATH_CHECK(m_thistSvc->regHist(m_histPath + m_histName + "/" + m_h_etaSpacePoint->GetName(), m_h_etaSpacePoint));

    if (m_usePixel and m_useOverlap)
        ATH_MSG_INFO("No overlap collection when enabled for pixel space points! Check your configuration if needed.");

    return StatusCode::SUCCESS;
}

StatusCode ActsTrk::SpacePointAnalysis::execute() {
    ATH_MSG_DEBUG(" In ActsTrk::SpacePointAnalysis::execute()" );

    m_barrelEndcap->clear();
    m_layerDisk->clear();
    m_phiModule->clear();
    m_etaModule->clear();
    m_sideModule->clear();
    m_isInnermost->clear();
    m_isNextToInnermost->clear();
    m_isOverlap->clear();
    m_eta->clear();
    m_globalX->clear();
    m_globalY->clear();
    m_globalZ->clear();

    const EventContext& ctx = Algorithm::getContext();

    SG::ReadHandle<ActsTrk::SpacePointContainer> inputSpacePointContainer( m_spacePointContainerKey, ctx );
    if (!inputSpacePointContainer.isValid()){
        ATH_MSG_FATAL("ActsTrk::SpacePointContainer with key " << m_spacePointContainerKey.key() << " is not available...");
        return StatusCode::FAILURE;
    }

    std::variant < const xAOD::PixelClusterContainer*, const xAOD::StripClusterContainer* > inputContainer;

    if (m_usePixel) {
        SG::ReadHandle<xAOD::PixelClusterContainer> inputClusterContainer( m_pixelClusterContainerKey, ctx );
        if (!inputClusterContainer.isValid()){
            ATH_MSG_FATAL("xAOD::PixelClusterContainer with key " << m_pixelClusterContainerKey.key() << " is not available...");
            return StatusCode::FAILURE;
        }
        inputContainer = inputClusterContainer.cptr();
    } else {
        SG::ReadHandle<xAOD::StripClusterContainer> inputClusterContainer( m_stripClusterContainerKey, ctx );
        if (!inputClusterContainer.isValid()){
            ATH_MSG_FATAL("xAOD::StripClusterContainer with key " << m_stripClusterContainerKey.key() << " is not available...");
            return StatusCode::FAILURE;
        }
        inputContainer = inputClusterContainer.cptr();
    }

    std::function<IdentifierHash(const ActsTrk::SpacePoint&)> mapper =
        [inputContainer, this](const ActsTrk::SpacePoint& sp) {
            auto& cluster_indices = sp.measurementIndexes();
            IdentifierHash idHash =
                std::visit([cluster_indices](auto container) -> IdentifierHash {
                    return container->at(cluster_indices[0])->identifierHash();}, inputContainer);
            return idHash;
        };

    ContainerAccessor<ActsTrk::SpacePoint, IdentifierHash>
        spacePointAccessor ( *inputSpacePointContainer.cptr(),
                             mapper );

    const auto& allIdHashes = spacePointAccessor.allIdentifiers();
    for (auto& idHash : allIdHashes) {
        for (auto& this_range : spacePointAccessor.rangesForIdentifierDirect(idHash)) {
            for (auto start=this_range.first; start!=this_range.second; start++) {
                const Identifier id = m_usePixel ?
                    m_pixelID->wafer_id(idHash) : m_stripID->wafer_id(idHash);
                const int brlEc(m_usePixel ? m_pixelID->barrel_ec(id) : m_stripID->barrel_ec(id));
                const int layerDisk(m_usePixel ? m_pixelID->layer_disk(id) : m_stripID->layer_disk(id));
                const int phiMod(m_usePixel ? m_pixelID->phi_module(id) : m_stripID->phi_module(id));
                const int etaMod(m_usePixel ? m_pixelID->eta_module(id) : m_stripID->eta_module(id));
                const int side(m_usePixel ? 0 : m_stripID->side(id));

                bool isInnermost = m_usePixel ? (layerDisk==0) : false;
                bool isNextToInnermost = m_usePixel ? ((layerDisk==1) or (brlEc!=0 and layerDisk==2)) : false;

                m_barrelEndcap->push_back(brlEc);
                m_layerDisk->push_back(layerDisk);
                m_phiModule->push_back(phiMod);
                m_etaModule->push_back(etaMod);
                m_sideModule->push_back(side);
                m_isInnermost->push_back(int(isInnermost));
                m_isNextToInnermost->push_back(int(isNextToInnermost));
                if (m_usePixel)
                    m_isOverlap->push_back(0);
                else
                    m_isOverlap->push_back(int(m_useOverlap));

                auto spacePoint = *start;

                Amg::Vector3D globalPosition(spacePoint->x(), spacePoint->y(), spacePoint->z());

                m_eta->push_back(globalPosition.eta());
                m_globalX->push_back(globalPosition.x());
                m_globalY->push_back(globalPosition.y());
                m_globalZ->push_back(globalPosition.z());

                m_h_globalZR->Fill(globalPosition.z(), globalPosition.perp());
                m_h_etaSpacePoint->Fill(globalPosition.eta());
            }
        }
    }

    if (m_tree) {
        m_tree->Fill();
    }

    return StatusCode::SUCCESS;
}
