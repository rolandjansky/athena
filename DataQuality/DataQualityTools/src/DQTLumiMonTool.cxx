/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityTools/DQTLumiMonTool.h"

DQTLumiMonTool::DQTLumiMonTool(const std::string& name, ISvcLocator* pSvcLocator)
: AthMonitorAlgorithm(name,pSvcLocator)
{}

DQTLumiMonTool::~DQTLumiMonTool() {}

StatusCode DQTLumiMonTool::initialize(){
    ATH_CHECK( m_EventInfoKey.initialize() );
    ATH_CHECK( m_VertexContainerKey.initialize() );
    ATH_CHECK( m_PixelClustersKey.initialize() );
    ATH_CHECK( m_PixelIDKey.initialize() );
    return AthMonitorAlgorithm::initialize();
}

StatusCode DQTLumiMonTool::fillHistograms(const EventContext& ctx) const {
    using namespace Monitored;
    auto group = getGroup("default");

    RH<xAOD::EventInfo> eventInfo(m_EventInfoKey,ctx);
    if ( !eventInfo.isValid() ) {
        ATH_MSG_WARNING("Could not retrieve EventInfo.");
        return StatusCode::FAILURE;
    }

    auto lumiBlock = Scalar<int>("LB", eventInfo->lumiBlock());
    auto avgMu = Scalar("avgMu", eventInfo->averageInteractionsPerCrossing());
    auto avgMuInverse = Scalar("avgMuInverse",0.0);
    if (avgMu>0) avgMuInverse = 1./avgMu;

    auto avgLumi = Scalar("avgLumi", lbAverageLuminosity(ctx));
    auto avgIntPerXing = Scalar("avgIntPerXing", lbAverageInteractionsPerCrossing(ctx));
    auto lumiPerBCID = Scalar("lumiPerBCID", lbLuminosityPerBCID(ctx));
    auto intPerXing = Scalar("intPerXing", lbInteractionsPerCrossing(ctx));
    auto duration = Scalar("duration", lbDuration(ctx));
    auto avgLiveFrac = Scalar("avgLiveFrac", lbAverageLivefraction(ctx));
    auto liveFracPerBCID = Scalar("liveFracPerBCID", livefractionPerBCID(ctx));
    auto lumiWeight = Scalar("lumiWeight", lbLumiWeight(ctx));

    RH<xAOD::VertexContainer> vertices(m_VertexContainerKey,ctx);
    auto nVtxLoose = Scalar<int>("nVtxLoose",0);
    auto nVtxTight = Scalar<int>("nVtxTight",0);
    if ( vertices.isValid() ) {
        for ( const auto vertex : *vertices ) {
            if (!vertex || !vertex->vxTrackAtVertexAvailable()) continue;
            nVtxLoose++;

            auto tracks = vertex->vxTrackAtVertex();
            int nGoodTracks = std::count_if(tracks.begin(),tracks.end(),
                [this](const auto track){return track.weight()>=m_tightTrackWeight;});
            if ( nGoodTracks>=m_tightNTracks )
                nVtxTight++;
        }
    } else {
        ATH_MSG_WARNING("Could not retrieve Vertex Container.");
    }

    auto nClustersAll = Scalar<int>("nClustersAll",0);
    auto nClustersECA = Scalar<int>("nClustersECA",0);
    auto nClustersECC = Scalar<int>("nClustersECC",0);
    auto nClustersB0 = Scalar<int>("nClustersB0",0);
    auto nClustersB1 = Scalar<int>("nClustersB1",0);
    auto nClustersB2 = Scalar<int>("nClustersB2",0);
    if ( m_environment==Environment_t::tier0Raw ) {
        RH<InDet::PixelClusterContainer> pixelClusters(m_PixelClustersKey,ctx);
        RH<PixelID> pixelID(m_PixelIDKey,ctx);
        if ( !pixelClusters.isValid() ) {
            ATH_MSG_WARNING("Could not retrieve Pixel Clusters.");
        } else if ( !pixelID.isValid() ) {
            ATH_MSG_WARNING("Could not retrieve Pixel ID.");
        } else {
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
        }
    }

    fill(group,lumiBlock,avgMu,avgMuInverse,avgLumi,avgIntPerXing,lumiPerBCID,intPerXing,
         duration,avgLiveFrac,liveFracPerBCID,lumiWeight,nVtxLoose,nVtxTight,nClustersAll,
         nClustersECA,nClustersECC,nClustersB0,nClustersB1,nClustersB2);

    return StatusCode::SUCCESS;
}
