/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityTools/DQTBackgroundMon.h"

DQTBackgroundMon::DQTBackgroundMon( const std::string& name, ISvcLocator* pSvcLocator )
: AthMonitorAlgorithm(name,pSvcLocator)
{}

DQTBackgroundMon::~DQTBackgroundMon() {}

StatusCode DQTBackgroundMon::initialize() {
    ATH_CHECK( m_RawInfoSummaryForTagKey.initialize() );
    ATH_CHECK( m_LArCollisionTimeKey.initialize() );
    ATH_CHECK( m_MBTSCollisionTimeKey.initialize() );
    ATH_CHECK( m_TileCellContainerKey.initialize() );
    ATH_CHECK( m_LUCID_RawDataContainerKey.initialize() );
    ATH_CHECK( m_BeamBackgroundDataKey.initialize() );
    ATH_CHECK( m_VertexContainerKey.initialize() );

    return AthMonitorAlgorithm::initialize();
}

StatusCode DQTBackgroundMon::fillHistograms( const EventContext& ctx ) const {
    ATH_MSG_DEBUG("In DQTBackgroundMon::fillHistograms()");
    using namespace Monitored;

    auto group = getGroup("default");

    auto filled = Scalar<bool>("filled",false);
    auto empty = Scalar<bool>("empty",false);
    auto unpairIso = Scalar<bool>("unpairIso",false);
    auto unpairNonIso = Scalar<bool>("unpairNonIso",false);

    RH<xAOD::EventInfo> eventInfo{GetEventInfo(ctx)};
    RH<RawInfoSummaryForTag> rawInfo(m_RawInfoSummaryForTagKey,ctx);
    RH<LArCollisionTime> tps(m_LArCollisionTimeKey,ctx);
    RH<MBTSCollisionTime> mbtsTime(m_MBTSCollisionTimeKey,ctx);
    RH<TileCellContainer> tileCellContainer(m_TileCellContainerKey,ctx);
    RH<LUCID_RawDataContainer> LUCID_RawDataContainer(m_LUCID_RawDataContainerKey,ctx);
    RH<BeamBackgroundData> beamBackgroundData(m_BeamBackgroundDataKey,ctx);
    RH<xAOD::VertexContainer> vertexContainer(m_VertexContainerKey,ctx);

    if ( eventInfo.isValid() ) {
        if (!getTrigDecisionTool().empty()) {
            unsigned int bgCode = getTrigDecisionTool()->getBGCode();
            filled = bgCode & (1<<m_filledBG);
            empty = bgCode & (1<<m_emptyBG);
            unpairIso = bgCode & (1<<m_unpairIsoBG);
            unpairNonIso = bgCode & (1<<m_unpairNonIsoBG);
        }

        std::vector<int> bitVec = {};
        std::vector<int> filledVec = {};
        std::vector<int> emptyVec = {};
        std::vector<int> unpairIsoVec = {};
        std::vector<int> unpairNonIsoVec = {};

        for ( int bkg=0; bkg<m_nBkgWords; bkg++ ) {
            unsigned int bgWord = eventInfo->eventFlags(xAOD::EventInfo::Background);
            ATH_MSG_DEBUG("Background word is "<<bgWord);
            if ( !(bgWord & 1<<bkg) ) {
                continue;
            }

            bitVec.push_back(bkg);
            if (filled) {
                filledVec.push_back(bkg);
            } else if (empty) {
                emptyVec.push_back(bkg);
            } else if (unpairIso) {
                unpairIsoVec.push_back(bkg);
            } else if (unpairNonIso) {
                unpairNonIsoVec.push_back(bkg);
            }
        }

        auto bitSet = Collection("bitSet",bitVec);
        auto bitSetFilled = Collection("bitSetFilled",filledVec);
        auto bitSetEmpty = Collection("bitSetEmpty",emptyVec);
        auto bitSetUnpairIso = Collection("bitSetUnpairIso",unpairIsoVec);
        auto bitSetUnpairNonIso = Collection("bitSetUnpairNonIso",unpairNonIsoVec);
        fill(group,bitSet,bitSetFilled,bitSetEmpty,bitSetUnpairIso,bitSetUnpairNonIso);
    } else {
        ATH_MSG_WARNING("Event data invalid. Background word histograms are being skipped.");
    }

    if ( rawInfo.isValid() ) {
        auto nPixSPs = Scalar<float>("nPixSPs",rawInfo->getNpixSPs());
        auto nSctSPs = Scalar<float>("nSctSPs",rawInfo->getNsctSPs());
        fill(group,nPixSPs,nSctSPs,unpairIso,unpairNonIso);
        if ( nPixSPs<m_upPixSP )
            fill("lowMultiplicityPixGroup",nPixSPs,unpairIso,unpairNonIso);
        if ( nSctSPs<m_upSctSP )
            fill("lowMultiplicitySctGroup",nSctSPs,unpairIso,unpairNonIso);
    } else {
        ATH_MSG_WARNING("Raw data invalid. nPix/nSct is being skipped.");
    }

    if ( tps.isValid() ) {
        if ( tps->ncellA()>m_LArECSideCut && tps->ncellC()>m_LArECSideCut ) {
            auto LArECTimeDiff = Scalar<float>("LArECTimeDiff", tps->timeA()-tps->timeC());
            fill(group,LArECTimeDiff);
        }
    } else {
        ATH_MSG_WARNING("TPS data invalid. LArECTimeDiff is being skipped.");
    }

    if ( mbtsTime.isValid() ) {
        if ( mbtsTime->ncellA()>m_MBTSSideCut && mbtsTime->ncellC()>m_MBTSSideCut ) {
            auto MBTStimeDiff = Scalar<float>("MBTStimeDiff", mbtsTime->time());
            fill(group,MBTStimeDiff);
        }
    } else {
        ATH_MSG_WARNING("MBTSCollisionTime data invalid. MBTStimeDiff is being skipped.");
    }

    if ( tileCellContainer.isValid() ) {
        auto MBTSCount = Scalar<int>("MBTSvetoHits",0);
        for ( auto tileCell : *tileCellContainer ) {
            if (tileCell->energy()>m_MBTSThresholdCut &&
                (tileCell->qbit1() & m_MBTSMask)==m_MBTSPattern &&
                fabs(tileCell->time())<m_MBTSTimeCut )
                MBTSCount++;
        }
        fill(group,MBTSCount);
    } else {
        ATH_MSG_WARNING("TileCell data invalid. MBTSvetoHits is being skipped.");
    }

    if ( LUCID_RawDataContainer.isValid() ) {
        auto LUCIDCounter = Scalar<int>("LUCIDCounter",0);
        for ( auto LUCID_RawData : *LUCID_RawDataContainer ) {
            LUCIDCounter += LUCID_RawData->getNhitsPMTsideA() + LUCID_RawData->getNhitsPMTsideC();
        }
        fill(group,LUCIDCounter);
    } else {
        ATH_MSG_WARNING("LUCID data invalid. LUCID is being skipped.");
    }

    if ( m_doMuons ) {
    	if (beamBackgroundData.isValid() ) {
            // Muon segments and clusters
            std::vector<double> muSegXVec = {};
            std::vector<double> muSegYVec = {};
            for (int iSeg=0; iSeg<beamBackgroundData->GetNumSegment(); iSeg++) {
                const Trk::Segment* trkSeg = beamBackgroundData->GetIndexSeg(iSeg);
                const xAOD::MuonSegment* muonSeg = dynamic_cast<const xAOD::MuonSegment*>(trkSeg);
                if (muonSeg) {
                    muSegXVec.push_back(muonSeg->x()*1e-3);
                    muSegYVec.push_back(muonSeg->y()*1e-3);
                }
            }
            auto muSegXCollection = Collection("muonSegmentX",muSegXVec);
            auto muSegYCollection = Collection("muonSegmentY",muSegYVec);

            std::vector<double> clusterEnergyVec = {};
            std::vector<double> clusterEtaVec = {};
            std::vector<double> clusterPhiVec = {};
            std::vector<double> clusterTimeVec = {};
            for (int i=0; i<beamBackgroundData->GetNumMatched(); i++) {
                const xAOD::CaloCluster* cluster = beamBackgroundData->GetIndexClus(i);
                clusterEnergyVec.push_back(cluster->e()*1e-3);
                clusterEtaVec.push_back(cluster->eta());
                clusterPhiVec.push_back(cluster->phi());
                clusterTimeVec.push_back(cluster->time());
            }
            auto clusterEnergyCollection = Collection("clusterEnergy",clusterEnergyVec);
            auto clusterEtaCollection = Collection("clusterEta",clusterEtaVec);
            auto clusterPhiCollection = Collection("clusterPhi",clusterPhiVec);
            auto clusterTimeCollection = Collection("clusterTime",clusterTimeVec);

            fill(group,muSegXCollection,muSegYCollection,clusterEnergyCollection,
                 clusterEtaCollection,clusterPhiCollection,clusterTimeCollection);

            // fake jets
            if ( beamBackgroundData->GetNumNoTimeTight() ) {
                std::vector<int> jetIndexVec = {};
                for (int i=0; i<beamBackgroundData->GetNumJet(); i++) {
                    const xAOD::Jet* jet = beamBackgroundData->GetIndexJet(i);
                    int jetIndex = beamBackgroundData->GetIndexJet(jet);
                    jetIndexVec.push_back(jetIndex);

                    // Leading jet
                    if ( jetIndex==0 ) {
                        auto pt = Scalar<double>("leadingJetPt", jet->pt());
                        auto eta = Scalar<double>("leadingJetEta", jet->eta());
                        auto phi = Scalar<double>("leadingJetPhi", jet->phi());
                        double timeValue = jet->getAttribute<float>(xAOD::JetAttribute::Timing);
                        auto time = Scalar<double>("leadingJetTime", timeValue);
                        double emfValue = jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
                        auto emf = Scalar<double>("leadingJetEMFrac", emfValue);

                        std::vector<float> sumPt_v;
                        auto trkPtAttr = xAOD::JetAttribute::SumPtTrkPt1000;
                        bool hasSumPt = jet->getAttribute<std::vector<float>>(trkPtAttr, sumPt_v);
                        double sumPtTrk = hasSumPt ? sumPt_v[0] : 0.;
                        auto chf = Scalar<double>("leadingJetCHF", sumPtTrk / pt);

                        fill(group,pt,eta,phi,time,emf,chf);
                    }
                }
                auto jetIndexCollection = Collection("jetIndex",jetIndexVec);

                auto bcid = Scalar<int>("bcid",2);
                auto isAC = Scalar<bool>("isAC", beamBackgroundData->GetDirection()>0);
                auto isCA = Scalar<bool>("isCA", beamBackgroundData->GetDirection()<0);
                bool isOneSided = beamBackgroundData->GetNumOneSidedLoose();
                auto isACOneSided = Scalar<bool>("isACOneSided", isAC && isOneSided );
                auto isCAOneSided = Scalar<bool>("isCAOneSided", isCA && isOneSided );
                fill(group,jetIndexCollection,bcid,isAC,isCA,isACOneSided,isCAOneSided);
            } // end fake jets
        } else {
            ATH_MSG_WARNING("BeamBackgroundData data invalid. Muons are being skipped.");
        }
    } // end muons

    if (vertexContainer.isValid()) {
        // count vertices, excluding the dummy vertex
        auto nVertex = Scalar<int>("nVertex", vertexContainer->size()-1);
        fill(group,nVertex,unpairIso,unpairNonIso);
    } else {
        ATH_MSG_DEBUG("Primary vertex data invalid. Primary vertex counts are being skipped.");
    }
    return StatusCode::SUCCESS;
}
