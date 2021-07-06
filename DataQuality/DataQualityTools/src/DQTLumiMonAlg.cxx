/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityTools/DQTLumiMonAlg.h"

DQTLumiMonAlg::DQTLumiMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthMonitorAlgorithm(name,pSvcLocator)
{}

DQTLumiMonAlg::~DQTLumiMonAlg() {}

StatusCode DQTLumiMonAlg::initialize(){
    // need to do this first, else we don't get environment() set up properly!
    ATH_CHECK( AthMonitorAlgorithm::initialize() );
    ATH_CHECK( m_VertexContainerKey.initialize() );
    ATH_CHECK( m_PixelClustersKey.initialize( environment() != Environment_t::AOD ) );
    ATH_CHECK( m_PixelIDKey.initialize() );
    return StatusCode::SUCCESS;
}

StatusCode DQTLumiMonAlg::fillHistograms(const EventContext& ctx) const {
    using namespace Monitored;
    auto group = getGroup("default");

    RH<xAOD::EventInfo> eventInfo{GetEventInfo(ctx)};
    if ( !eventInfo.isValid() ) {
        ATH_MSG_WARNING("Could not retrieve EventInfo.");
        return StatusCode::FAILURE;
    }

    auto lumiBlock = Scalar<int>("LB", eventInfo->lumiBlock());

    auto avgLumi = Scalar("avgLumi", lbAverageLuminosity(ctx));
    auto avgIntPerXing = Scalar("avgIntPerXing", lbAverageInteractionsPerCrossing(ctx));
    float avgMuInverse = avgIntPerXing>0 ? 1./avgIntPerXing : 0.;
    auto lumiPerBCID = Scalar("lumiPerBCID", lbLuminosityPerBCID(ctx));
    auto intPerXing = Scalar("intPerXing", lbInteractionsPerCrossing(ctx));
    auto duration = Scalar("duration", lbDuration(ctx));
    auto avgLiveFrac = Scalar("avgLiveFrac", lbAverageLivefraction(ctx));
    auto liveFracPerBCID = Scalar("liveFracPerBCID", livefractionPerBCID(ctx));
    auto lumiWeight = Scalar("lumiWeight", lbLumiWeight(ctx));

    RH<xAOD::VertexContainer> vertices(m_VertexContainerKey,ctx);
    auto nLooseVtx = Scalar<int>("nLooseVtx",0);
    auto nTightVtx = Scalar<int>("nTightVtx",0);
    if ( vertices.isValid() ) {
        for ( const auto vertex : *vertices ) {
            if (!vertex || !vertex->nTrackParticles()) continue;
            nLooseVtx++;

            int nGoodTracks = 0;
            for (size_t i = 0; i < vertex->nTrackParticles(); ++i) {
                if (vertex->trackWeight(i) >= m_tightTrackWeight) { ++nGoodTracks; }
            }
            if ( nGoodTracks>=m_tightNTracks ) {
                nTightVtx++;
            }
        }
    } else {
        ATH_MSG_WARNING("Could not retrieve Vertex Container.");
    }
    auto nLooseVtxPerAvgMu = Scalar<float>("nLooseVtxPerAvgMu",nLooseVtx*avgMuInverse);
    auto nTightVtxPerAvgMu = Scalar<float>("nTightVtxPerAvgMu",nTightVtx*avgMuInverse);

    fill(group,lumiBlock,avgLumi,avgIntPerXing,lumiPerBCID,intPerXing,duration,
         avgLiveFrac,liveFracPerBCID,lumiWeight,nLooseVtx,nTightVtx,nLooseVtxPerAvgMu,
         nTightVtxPerAvgMu);

    if ( m_environment!=Environment_t::AOD ) {
        RH<InDet::PixelClusterContainer> pixelClusters(m_PixelClustersKey,ctx);
        RH<PixelID> pixelID(m_PixelIDKey,ctx);
        if ( !pixelClusters.isValid() ) {
            ATH_MSG_WARNING("Could not retrieve Pixel Clusters.");
        } else if ( !pixelID.isValid() ) {
            ATH_MSG_WARNING("Could not retrieve Pixel ID.");
        } else {
            auto nClustersAll = Scalar<int>("nClustersAll",0);
            auto nClustersECA = Scalar<int>("nClustersECA",0);
            auto nClustersECC = Scalar<int>("nClustersECC",0);
            auto nClustersB0 = Scalar<int>("nClustersB0",0);
            auto nClustersB1 = Scalar<int>("nClustersB1",0);
            auto nClustersB2 = Scalar<int>("nClustersB2",0);
            for ( auto cluster : *pixelClusters ) {
                if (!cluster)
                    continue;
                for ( auto pixel : *cluster ) {
                    Identifier clusterID = pixel->identify();
                    nClustersAll++;
                    int ec = pixelID->barrel_ec(clusterID);
                    if (ec==2) nClustersECA++;
                    else if (ec==-2) nClustersECC++;
                    else if (ec==0) {
                        int ld = pixelID->layer_disk(clusterID);
                        if (ld==0) nClustersB0++;
                        else if(ld==1) nClustersB1++;
                        else if(ld==2) nClustersB2++;
                    }
                }
                ATH_MSG_DEBUG("nClustersAll is " << nClustersAll);
            }
            auto nClustersAllPerAvgMu = Scalar<int>("nClustersAllPerAvg",nClustersAll*avgMuInverse);
            auto nClustersECAPerAvgMu = Scalar<int>("nClustersECAPerAvg",nClustersECA*avgMuInverse);
            auto nClustersECCPerAvgMu = Scalar<int>("nClustersECCPerAvg",nClustersECC*avgMuInverse);
            auto nClustersB0PerAvgMu = Scalar<int>("nClustersB0PerAvg",nClustersB0*avgMuInverse);
            auto nClustersB1PerAvgMu = Scalar<int>("nClustersB1PerAvg",nClustersB1*avgMuInverse);
            auto nClustersB2PerAvgMu = Scalar<int>("nClustersB2PerAvg",nClustersB2*avgMuInverse);

            fill("pixel",lumiBlock,
                 nClustersAll,nClustersECA,nClustersECC,
                 nClustersB0,nClustersB1,nClustersB2,
                 nClustersAllPerAvgMu,nClustersECAPerAvgMu,nClustersECCPerAvgMu,
                 nClustersB0PerAvgMu,nClustersB1PerAvgMu,nClustersB2PerAvgMu);
        }
    }

    return StatusCode::SUCCESS;
}
