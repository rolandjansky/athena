/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TgcRawDataMonitorAlgorithm.h"

#include "TObjArray.h"

namespace {
    // Cut values on pt bein exploited throughout the monitoring
    constexpr float pt_30_cut = 30. * Gaudi::Units::GeV;
    constexpr float pt_15_cut = 15. * Gaudi::Units::GeV;
    constexpr float pt_10_cut = 10. * Gaudi::Units::GeV;

    /// End of the barrel region
    constexpr float barrel_end = 1.05;
    constexpr float trigger_end = 2.4;

}
TgcRawDataMonitorAlgorithm::TgcRawDataMonitorAlgorithm(const std::string &name, ISvcLocator *pSvcLocator) :
        AthMonitorAlgorithm(name, pSvcLocator) {
}

StatusCode TgcRawDataMonitorAlgorithm::initialize() {
    ATH_CHECK(AthMonitorAlgorithm::initialize());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_CHECK(m_MuonContainerKey.initialize());
    ATH_CHECK(m_MuonRoIContainerKey.initialize(SG::AllowEmpty));
    ATH_CHECK(m_TgcPrepDataContainerKey.initialize());
    ATH_CHECK(m_TgcCoinDataContainerCurrBCKey.initialize());
    ATH_CHECK(m_TgcCoinDataContainerNextBCKey.initialize());
    ATH_CHECK(m_TgcCoinDataContainerPrevBCKey.initialize());

    m_extZposition.push_back(m_M1_Z.value());
    m_extZposition.push_back(m_M2_Z.value());
    m_extZposition.push_back(m_M3_Z.value());
    m_extZposition.push_back(m_EI_Z.value());
    m_extZposition.push_back(m_FI_Z.value());
    m_extZposition.push_back(-m_M1_Z.value());
    m_extZposition.push_back(-m_M2_Z.value());
    m_extZposition.push_back(-m_M3_Z.value());
    m_extZposition.push_back(-m_EI_Z.value());
    m_extZposition.push_back(-m_FI_Z.value());

    TObjArray *tagList = TString(m_trigTagList.value()).Tokenize(",");
    std::set < TString > alllist;
    for (int i = 0; i < tagList->GetEntries(); i++) {
        TString tagTrig = tagList->At(i)->GetName();
        if (alllist.find(tagTrig) != alllist.end()) continue;
        alllist.insert(tagTrig);
        TObjArray *arr = tagTrig.Tokenize(";");
        if (arr->GetEntries() == 0) continue;
        TagDef def;
        def.eventTrig = TString(arr->At(0)->GetName());
        def.tagTrig = def.eventTrig;
        if (arr->GetEntries() == 2) def.tagTrig = TString(arr->At(1)->GetName());
        m_trigTagDefs.push_back(def);
    }

    return StatusCode::SUCCESS;
}

StatusCode TgcRawDataMonitorAlgorithm::fillHistograms(const EventContext &ctx) const {
  MonVariables variables;
    

    auto bcid = Monitored::Scalar<int>("bcid", GetEventInfo(ctx)->bcid());
    auto lumiPerBCID = Monitored::Scalar<int>("lumiPerBCID", lbAverageInteractionsPerCrossing(ctx));
    auto lb = Monitored::Scalar<int>("lb", GetEventInfo(ctx)->lumiBlock());
    auto run = Monitored::Scalar<int>("run", GetEventInfo(ctx)->runNumber());

    variables.push_back(bcid);
    variables.push_back(lumiPerBCID);
    variables.push_back(lb);
    variables.push_back(run);

    if (!m_anaMuonRoI.value()) {
        fill(m_packageName, variables);
        variables.clear();
        return StatusCode::SUCCESS;
    }
    
    
    const xAOD::MuonRoIContainer *rois = nullptr;
    if (!m_MuonRoIContainerKey.empty()) {
        /* raw LVL1MuonRoIs distributions */
         SG::ReadHandle<xAOD::MuonRoIContainer > handle( m_MuonRoIContainerKey, ctx);
        if(!handle.isValid()) {
            ATH_MSG_ERROR("evtStore() does not contain muon RoI Collection with name " << m_MuonRoIContainerKey);
            return StatusCode::FAILURE;
        }
        rois = handle.cptr();
        if (!rois) {
            ATH_MSG_ERROR("evtStore() does not contain muon RoI Collection with name " << m_MuonRoIContainerKey);
            return StatusCode::FAILURE;
        }
        
        MonVariables  roi_variables;
        auto roi_eta = Monitored::Collection("roi_eta", *rois, [](const xAOD::MuonRoI *m) {
            return m->eta();
        });
        roi_variables.push_back(roi_eta);
        
        auto roi_phi = Monitored::Collection("roi_phi", *rois, [](const xAOD::MuonRoI *m) {
            return m->phi();
        });
        roi_variables.push_back(roi_phi);
        auto roi_phi_rpc = Monitored::Collection("roi_phi_rpc", *rois, [](const xAOD::MuonRoI *m) {
            return (m->getSource() == xAOD::MuonRoI::Barrel) ? m->phi() : -10;
        });
        roi_variables.push_back(roi_phi_rpc);
        auto roi_phi_tgc = Monitored::Collection("roi_phi_tgc", *rois, [](const xAOD::MuonRoI *m) {
            return (m->getSource() != xAOD::MuonRoI::Barrel) ? m->phi() : -10;
        });
        variables.push_back(roi_phi_tgc);
        auto roi_thr = Monitored::Collection("roi_thr", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber();
        });
        roi_variables.push_back(roi_thr);
        auto roi_rpc = Monitored::Collection("roi_rpc", *rois, [](const xAOD::MuonRoI *m) {
            return m->getSource() == xAOD::MuonRoI::Barrel;
        });
        roi_variables.push_back(roi_rpc);
        auto roi_tgc = Monitored::Collection("roi_tgc", *rois, [](const xAOD::MuonRoI *m) {
            return m->getSource() != xAOD::MuonRoI::Barrel;
        });
        roi_variables.push_back(roi_tgc);

        auto roi_barrel = Monitored::Collection("roi_barrel", *rois, [](const xAOD::MuonRoI *m) {
            return m->getSource() == xAOD::MuonRoI::Barrel;
        });
        roi_variables.push_back(roi_barrel);
        auto roi_endcap = Monitored::Collection("roi_endcap", *rois, [](const xAOD::MuonRoI *m) {
            return m->getSource() == xAOD::MuonRoI::Endcap;
        });
        roi_variables.push_back(roi_endcap);
        auto roi_forward = Monitored::Collection("roi_forward", *rois, [](const xAOD::MuonRoI *m) {
            return m->getSource() == xAOD::MuonRoI::Forward;
        });
        roi_variables.push_back(roi_forward);

        auto roi_phi_barrel = Monitored::Collection("roi_phi_barrel", *rois, [](const xAOD::MuonRoI *m) {
            return (m->getSource() == xAOD::MuonRoI::Barrel) ? m->phi() : -10;
        });
        roi_variables.push_back(roi_phi_barrel);
        auto roi_phi_endcap = Monitored::Collection("roi_phi_endcap", *rois, [](const xAOD::MuonRoI *m) {
            return (m->getSource() == xAOD::MuonRoI::Endcap) ? m->phi() : -10;
        });
        roi_variables.push_back(roi_phi_endcap);
        auto roi_phi_forward = Monitored::Collection("roi_phi_forward", *rois, [](const xAOD::MuonRoI *m) {
            return (m->getSource() == xAOD::MuonRoI::Forward) ? m->phi() : -10;
        });
        roi_variables.push_back(roi_phi_forward);

        auto roi_sideA = Monitored::Collection("roi_sideA", *rois, [](const xAOD::MuonRoI *m) {
            return m->getHemisphere() == xAOD::MuonRoI::Positive;
        });
        roi_variables.push_back(roi_sideA);
        auto roi_sideC = Monitored::Collection("roi_sideC", *rois, [](const xAOD::MuonRoI *m) {
            return m->getHemisphere() == xAOD::MuonRoI::Negative;
        });
        roi_variables.push_back(roi_sideC);

        auto thrmask1 = Monitored::Collection("thrmask1", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 1;
        });
        roi_variables.push_back(thrmask1);
        auto thrmask2 = Monitored::Collection("thrmask2", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 2;
        });
        roi_variables.push_back(thrmask2);
        auto thrmask3 = Monitored::Collection("thrmask3", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 3;
        });
        roi_variables.push_back(thrmask3);
        auto thrmask4 = Monitored::Collection("thrmask4", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 4;
        });
        roi_variables.push_back(thrmask4);
        auto thrmask5 = Monitored::Collection("thrmask5", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 5;
        });
        roi_variables.push_back(thrmask5);
        auto thrmask6 = Monitored::Collection("thrmask6", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 6;
        });
        roi_variables.push_back(thrmask6);
        auto thrmask7 = Monitored::Collection("thrmask7", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 7;
        });
        roi_variables.push_back(thrmask7);
        auto thrmask8 = Monitored::Collection("thrmask8", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 8;
        });
        roi_variables.push_back(thrmask8);
        auto thrmask9 = Monitored::Collection("thrmask9", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 9;
        });
        roi_variables.push_back(thrmask9);
        auto thrmask10 = Monitored::Collection("thrmask10", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 10;
        });
        roi_variables.push_back(thrmask10);
        auto thrmask11 = Monitored::Collection("thrmask11", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 11;
        });
        roi_variables.push_back(thrmask11);
        auto thrmask12 = Monitored::Collection("thrmask12", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 12;
        });
        roi_variables.push_back(thrmask12);
        auto thrmask13 = Monitored::Collection("thrmask13", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 13;
        });
        roi_variables.push_back(thrmask13);
        auto thrmask14 = Monitored::Collection("thrmask14", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 14;
        });
        roi_variables.push_back(thrmask14);
        auto thrmask15 = Monitored::Collection("thrmask15", *rois, [](const xAOD::MuonRoI *m) {
            return m->getThrNumber() == 15;
        });
        roi_variables.push_back(thrmask15);
        fill(m_packageName, roi_variables);     
    }
    if (!m_anaOfflMuon.value()) {
        fill(m_packageName, variables);
        return StatusCode::SUCCESS;
    }
   
    SG::ReadHandle < xAOD::MuonContainer > muons(m_MuonContainerKey, ctx);
    if (!muons.isValid()) {
        ATH_MSG_ERROR("evtStore() does not contain muon Collection with name " << m_MuonContainerKey);
        return StatusCode::FAILURE;
    }

    std::vector < MyMuon > mymuons;
    for (const auto &muon : *muons) {
        if (muon->muonType() != xAOD::Muon::Combined) continue;
        if (muon->author() != xAOD::Muon::MuidCo && muon->author() != xAOD::Muon::STACO) continue;
        if (muon->quality() != xAOD::Muon::Tight && muon->quality() != xAOD::Muon::Medium) continue;

        MyMuon mymuon;
        /* fill basic info */
        mymuon.muon = muon;
        mymuon.fourvec.SetPtEtaPhiM(muon->pt(), muon->eta(), muon->phi(), m_muonMass.value());
        /* fill tag of tag-and-probe info */
        mymuon.tagged = triggerMatching(muon, m_trigTagDefs);
        /* fill info of isolation among muons */
        mymuon.isolated = true;
        mymuon.probeOK = false;
        if (!m_TagAndProbe.value()) mymuon.probeOK = true;
        for (const auto &muon2 : *muons) {
            if (muon == muon2) continue;
            if (muon2->muonType() != xAOD::Muon::Combined) continue;
            if (muon2->author() != xAOD::Muon::MuidCo && muon2->author() != xAOD::Muon::STACO) continue;
            if (muon2->quality() != xAOD::Muon::Tight && muon2->quality() != xAOD::Muon::Medium) continue;
            if (!triggerMatching(muon2, m_trigTagDefs)) continue;

            TLorentzVector muonvec2;
            muonvec2.SetPtEtaPhiM(muon2->pt(), muon2->eta(), muon2->phi(), m_muonMass.value());
            float dr = muonvec2.DeltaR(mymuon.fourvec);
            if (dr < m_isolationWindow.value()) {
                mymuon.isolated = false;
            }
            if (!m_TagAndProbeZmumu.value()) {
                mymuon.probeOK = true;
            } else {
                if (muon->charge() == muon2->charge()) continue;
                double dimuon_mass = (muonvec2 + mymuon.fourvec).M();
                if (std::abs(dimuon_mass - m_zMass.value()) > m_zMassWindow.value()) continue;
                mymuon.probeOK = true;
            }
            break;
        }
        /* fill extrapolation info (only to TGC) */
        extrapolate(muon, mymuon);
        /* L1Muon RoI matching */
        float max_dr = 999;
        const float pt = mymuon.muon->pt();

        if (pt > pt_15_cut) max_dr = m_l1trigMatchWindow1.value();
        else if (pt > pt_10_cut) max_dr = m_l1trigMatchWindow2.value() + m_l1trigMatchWindow3.value() * pt / Gaudi::Units::GeV;
        else max_dr = m_l1trigMatchWindow4.value() + m_l1trigMatchWindow5.value() * pt / Gaudi::Units::GeV;

        if (!rois) {
            ATH_MSG_DEBUG("No RoI matching possible as no container has been retrieved");
            mymuons.push_back(mymuon);
            continue;
        }
        for (const auto &roi : *rois) {
            TLorentzVector roivec;
            roivec.SetPtEtaPhiM(1, roi->eta(), roi->phi(), 1);
            float dr = roivec.DeltaR(mymuon.fourvec);
            if (dr < max_dr) {
                mymuon.matchedL1ThrExclusive.insert(roi->getThrNumber());
            }
        }
        const bool L1ThrsEmpt = mymuon.matchedL1ThrExclusive.empty();
        for (int ithr = 0; ithr < 16 && !L1ThrsEmpt; ++ithr) {
            bool pass = false;
            for (const auto &thr : mymuon.matchedL1ThrExclusive) {
                if (thr >= ithr) {
                    pass = true;
                    break;
                }
            }
            if (pass) mymuon.matchedL1ThrInclusive.insert(ithr);
        }

        /* store MyMuon */
        mymuons.push_back(mymuon);
    }

    auto muon_eta = Monitored::Collection("muon_eta", mymuons, [](const MyMuon &m) {
        return (m.probeOK && m.muon->pt() > pt_30_cut) ? m.muon->eta() : -10;
    });
    variables.push_back(muon_eta);
    auto muon_phi = Monitored::Collection("muon_phi", mymuons, [](const MyMuon &m) {
        return (m.probeOK && m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
    });
    variables.push_back(muon_phi);
    auto muon_phi_rpc = Monitored::Collection("muon_phi_rpc", mymuons, [](const MyMuon &m) {
        return (m.probeOK && std::abs(m.muon->eta()) < barrel_end && m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
    });
    variables.push_back(muon_phi_rpc);
    auto muon_phi_tgc = Monitored::Collection("muon_phi_tgc", mymuons, [](const MyMuon &m) {
        return (m.probeOK && std::abs(m.muon->eta()) > barrel_end && std::abs(m.muon->eta()) < trigger_end && m.muon->pt() > pt_30_cut) ? m.muon->phi() : -10;
    });
    variables.push_back(muon_phi_tgc);
    auto muon_pt_rpc = Monitored::Collection("muon_pt_rpc", mymuons, [](const MyMuon &m) {
        return (m.probeOK && std::abs(m.muon->eta()) < barrel_end) ? m.muon->pt() / Gaudi::Units::GeV : -10;
    });
    variables.push_back(muon_pt_rpc);
    auto muon_pt_tgc = Monitored::Collection("muon_pt_tgc", mymuons, [](const MyMuon &m) {
        return (m.probeOK && std::abs(m.muon->eta()) > barrel_end && std::abs(m.muon->eta()) < trigger_end) ? m.muon->pt() / Gaudi::Units::GeV : -10;
    });
    variables.push_back(muon_pt_tgc);
    auto muon_l1passThr1 = Monitored::Collection("muon_l1passThr1", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(1) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr1);
    auto muon_l1passThr2 = Monitored::Collection("muon_l1passThr2", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(2) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr2);
    auto muon_l1passThr3 = Monitored::Collection("muon_l1passThr3", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(3) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr3);
    auto muon_l1passThr4 = Monitored::Collection("muon_l1passThr4", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(4) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr4);
    auto muon_l1passThr5 = Monitored::Collection("muon_l1passThr5", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(5) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr5);
    auto muon_l1passThr6 = Monitored::Collection("muon_l1passThr6", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(6) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr6);
    auto muon_l1passThr7 = Monitored::Collection("muon_l1passThr7", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(7) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr7);
    auto muon_l1passThr8 = Monitored::Collection("muon_l1passThr8", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(8) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr8);
    auto muon_l1passThr9 = Monitored::Collection("muon_l1passThr9", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(9) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr9);
    auto muon_l1passThr10 = Monitored::Collection("muon_l1passThr10", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(10) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr10);
    auto muon_l1passThr11 = Monitored::Collection("muon_l1passThr11", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(11) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr11);
    auto muon_l1passThr12 = Monitored::Collection("muon_l1passThr12", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(12) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr12);
    auto muon_l1passThr13 = Monitored::Collection("muon_l1passThr13", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(13) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr13);
    auto muon_l1passThr14 = Monitored::Collection("muon_l1passThr14", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(14) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr14);
    auto muon_l1passThr15 = Monitored::Collection("muon_l1passThr15", mymuons, [](const MyMuon &m) {
        return m.matchedL1ThrInclusive.find(15) != m.matchedL1ThrInclusive.end();
    });
    variables.push_back(muon_l1passThr15);

    if (!m_anaTgcPrd.value()) {
        fill(m_packageName, variables);
        variables.clear();
        return StatusCode::SUCCESS;
    }

    SG::ReadHandle < Muon::TgcPrepDataContainer > tgcPrd(m_TgcPrepDataContainerKey, ctx);
    if (!tgcPrd.isValid()) {
        ATH_MSG_ERROR("evtStore() does not contain TgcPrepDataContainer with name " << m_TgcPrepDataContainerKey);
        return StatusCode::FAILURE;
    }
    const TgcIdHelper &tgcIdHelper = m_idHelperSvc->tgcIdHelper();
    std::vector < TgcHit > tgcHits;
    std::set < TString > chamber_list;
    std::map<TString, std::vector<TgcHit>> tgcHitsMap;
    std::map<TString, std::vector<int>> tgcHitPhiMap;
    std::map<TString, std::vector<int>> tgcHitEtaMap;
    std::map<TString, std::vector<int>> tgcHitPhiMapGlobal;
    std::map<TString, std::vector<int>> tgcHitTiming;
    for (auto tgccnt : *tgcPrd) {
        for (auto data : *tgccnt) {
            TgcHit tgcHit;
            int bunch = -10;
            if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_PREVIOUS) == Muon::TgcPrepData::BCBIT_PREVIOUS) bunch = -1;
            if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_CURRENT) == Muon::TgcPrepData::BCBIT_CURRENT) bunch = 0;
            if ((data->getBcBitMap() & Muon::TgcPrepData::BCBIT_NEXT) == Muon::TgcPrepData::BCBIT_NEXT) bunch = +1;
            const MuonGM::TgcReadoutElement *element = data->detectorElement();
            const Identifier id = data->identify();
            const int gasGap = tgcIdHelper.gasGap(id);
            const int channel = tgcIdHelper.channel(id);
            const bool isStrip = tgcIdHelper.isStrip(id);
            const Amg::Vector3D &pos = isStrip ? element->stripPos(gasGap, channel) : element->gangPos(gasGap, channel);
            tgcHit.x = pos[0];
            tgcHit.y = pos[1];
            tgcHit.z = pos[2];
            if (isStrip) {
                tgcHit.shortWidth = element->stripShortWidth(gasGap, channel);
                tgcHit.longWidth = element->stripLongWidth(gasGap, channel);
                tgcHit.length = element->stripLength(gasGap, channel);
            } else {
                tgcHit.shortWidth = element->gangShortWidth(gasGap, channel);
                tgcHit.longWidth = element->gangLongWidth(gasGap, channel);
                tgcHit.length = element->gangLength(gasGap, channel);
            }
            tgcHit.isStrip = tgcIdHelper.isStrip(id);
            tgcHit.gasGap = tgcIdHelper.gasGap(id);
            tgcHit.channel = tgcIdHelper.channel(id);
            tgcHit.eta = tgcIdHelper.stationEta(id);
            tgcHit.phi = tgcIdHelper.stationPhi(id);
            tgcHit.station = tgcIdHelper.stationName(id);
            tgcHit.bunch = bunch;

            tgcHit.igasGap = tgcHit.gasGap;
            tgcHit.ieta = tgcHit.eta;
            tgcHit.iphi = tgcHit.phi;
            tgcHit.side = (tgcHit.ieta > 0) ? ("A") : ("C");
            tgcHit.iside = (tgcHit.ieta > 0) ? (0) : (1);
            tgcHit.M = 0;
            tgcHit.istation = tgcHit.station;
            if (tgcHit.istation == 41 || tgcHit.istation == 42) tgcHit.M = 1;
            else if (tgcHit.istation == 43 || tgcHit.istation == 44) tgcHit.M = 2;
            else if (tgcHit.istation == 45 || tgcHit.istation == 46) tgcHit.M = 3;
            else if (tgcHit.istation == 47 || tgcHit.istation == 48) tgcHit.M = 4; // EIFI
            if (tgcHit.M == 0) {
                ATH_MSG_ERROR("unknown station: " << tgcHit.istation);
            }

            if (tgcHit.M != 4) { // Big Wheel, (M1,M2,M3)
                if (tgcHit.istation % 2 == 0) { // Endcap
                    int iphi2 = tgcHit.iphi + 1; // 2,3,4,..,49
                    if (iphi2 >= 48) iphi2 -= 48; // 0,1,2,3 ..., 47
                    tgcHit.sector = int(iphi2 / 4) + 1; // 1,2,3,,,12
                    tgcHit.f = iphi2 - (tgcHit.sector - 1) * 4; // 0,1,2,3
                    tgcHit.E = (tgcHit.M == 1) ? (5 - TMath::Abs(tgcHit.ieta)) : (6 - TMath::Abs(tgcHit.ieta));
                    tgcHit.L = tgcHit.igasGap;
                    tgcHit.name = Form("%s%02iM%02if%02iE%02iL%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.M, tgcHit.f, tgcHit.E, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
                } else { // Forward
                    int iphi2 = tgcHit.iphi; // 1,2,3,4,..,25
                    if (iphi2 >= 24) iphi2 -= 24; // 0,1,2,3 ...,23
                    tgcHit.sector = int(iphi2 / 2) + 1; // 1,2,3,,,12
                    tgcHit.f = iphi2 - (tgcHit.sector - 1) * 2; // 0,1
                    if (tgcHit.f == 1) tgcHit.f = 2; //0,2
                    tgcHit.E = 0; // F
                    tgcHit.L = tgcHit.igasGap;
                    tgcHit.name = Form("%s%02iM%02if%02iF00L%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.M, tgcHit.f, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
                }
            } else { // Small Wheel (M4)
                if (tgcHit.istation == 47) { // FI
                    tgcHit.sector = 0;
                    tgcHit.f = tgcHit.iphi; // 1,2,3..24
                    tgcHit.E = 0;
                    tgcHit.L = tgcHit.igasGap;
                    tgcHit.name = Form("%s00M04f%02iF00L%02i%s", tgcHit.side.Data(), tgcHit.f, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
                } else if (tgcHit.istation == 48) { // EI
                    int iphi2 = (tgcHit.iphi >= 21) ? (tgcHit.iphi - 21) : (tgcHit.iphi); // 0,1,2,..,20
                    if (iphi2 >= 0 && iphi2 <= 2) {
                        tgcHit.sector = 1;
                        tgcHit.f = iphi2;
                    } else if (iphi2 >= 3 && iphi2 <= 5) {
                        tgcHit.sector = 3;
                        tgcHit.f = iphi2 - 3;
                    } else if (iphi2 >= 6 && iphi2 <= 8) {
                        tgcHit.sector = 5;
                        tgcHit.f = iphi2 - 6;
                    } else if (iphi2 >= 9 && iphi2 <= 10) {
                        tgcHit.sector = 7;
                        tgcHit.f = iphi2 - 9 + 1;
                    } else if (iphi2 >= 11 && iphi2 <= 13) {
                        tgcHit.sector = 9;
                        tgcHit.f = iphi2 - 11;
                    } else if (iphi2 >= 14 && iphi2 <= 15) {
                        tgcHit.sector = 11;
                        tgcHit.f = iphi2 - 13;
                    } else if (iphi2 >= 16 && iphi2 <= 18) {
                        tgcHit.sector = 13;
                        tgcHit.f = iphi2 - 16;
                    } else if (iphi2 >= 19 && iphi2 <= 20) {
                        tgcHit.sector = 15;
                        tgcHit.f = iphi2 - 19 + 1;
                    }
                    tgcHit.E = 1;
                    tgcHit.L = tgcHit.igasGap;
                    tgcHit.name = Form("%s%02iM04f%02iE01L%02i%s", tgcHit.side.Data(), tgcHit.sector, tgcHit.f, tgcHit.L, (tgcHit.isStrip > 0) ? ("S") : ("W"));
                } else {
                    ATH_MSG_ERROR("Unknown detector");
                }
            }
            tgcHits.push_back(tgcHit);
            tgcHitsMap[tgcHit.name].push_back(tgcHit);
            chamber_list.insert(tgcHit.name);

            TString station_name = Form("%sM%02i%s", tgcHit.side.Data(), tgcHit.M, (tgcHit.isStrip > 0) ? ("S") : ("W"));
            int phimap_index = 0;
            int etamap_index = 0;
            int phimap_global_index = 0; // no empty bins compare to the above index
            if (tgcHit.M == 1) {
                phimap_index = (tgcHit.sector - 1) * 4 + tgcHit.f + 1;
                int tmpeta = (tgcHit.E == 0) ? (5) : (tgcHit.E);
                etamap_index = (tmpeta - 1) * 3 + tgcHit.L;
                int tmpphi = tgcHit.f;
                if (tgcHit.E == 0 && tgcHit.f == 2) tmpphi = 1;
                if (tgcHit.E > 0) phimap_global_index = (tmpeta - 1) * 144 + (tgcHit.sector - 1) * 12 + tmpphi * 3 + tgcHit.L;
                else phimap_global_index = 576 + (tgcHit.sector - 1) * 6 + tmpphi * 3 + tgcHit.L;
            } else if (tgcHit.M == 2 || tgcHit.M == 3) {
                phimap_index = (tgcHit.sector - 1) * 4 + tgcHit.f + 1;
                int tmpeta = (tgcHit.E == 0) ? (6) : (tgcHit.E);
                etamap_index = (tmpeta - 1) * 2 + tgcHit.L;
                int tmpphi = tgcHit.f;
                if (tgcHit.E == 0 && tgcHit.f == 2) tmpphi = 1;
                if (tgcHit.E > 0) phimap_global_index = (tmpeta - 1) * 96 + (tgcHit.sector - 1) * 8 + tmpphi * 2 + tgcHit.L;
                else phimap_global_index = 480 + (tgcHit.sector - 1) * 4 + tmpphi * 2 + tgcHit.L;
            } else if (tgcHit.M == 4) {
                phimap_index = tgcHit.iphi;
                int tmpeta = (tgcHit.E == 0) ? (2) : (tgcHit.E);
                etamap_index = (tmpeta - 1) * 2 + tgcHit.L;
                if (tgcHit.E > 0) phimap_global_index = (tgcHit.iphi - 1) * 2 + tgcHit.L;
                else phimap_global_index = 42 + (tgcHit.iphi - 1) * 2 + tgcHit.L;
            }
            tgcHitPhiMap[station_name].push_back(phimap_index);
            tgcHitEtaMap[station_name].push_back(etamap_index);
            tgcHitPhiMapGlobal[station_name].push_back(phimap_global_index);
            tgcHitTiming[station_name].push_back(bunch);

        }
    }

    auto hit_n = Monitored::Scalar<int>("hit_n", tgcHits.size());
    variables.push_back(hit_n);
    auto hit_bunch = Monitored::Collection("hit_bunch", tgcHits, [](const TgcHit &m) {
        return m.bunch;
    });
    variables.push_back(hit_bunch);
    auto hit_sideA = Monitored::Collection("hit_sideA", tgcHits, [](const TgcHit &m) {
        return m.z > 0;
    });
    variables.push_back(hit_sideA);
    auto hit_sideC = Monitored::Collection("hit_sideC", tgcHits, [](const TgcHit &m) {
        return m.z < 0;
    });
    variables.push_back(hit_sideC);

    for (const auto &chamber_name : chamber_list) {
        auto hits_on_a_chamber = Monitored::Collection(Form("hits_on_%s", chamber_name.Data()), tgcHitsMap[chamber_name], [](const TgcHit &m) {
            return m.channel;
        });
        fill(m_packageName, hits_on_a_chamber);
    }

    for (const auto &phimap : tgcHitPhiMap) {
        auto x = Monitored::Collection(Form("x_%s", phimap.first.Data()), tgcHitEtaMap[phimap.first], [](const int &m) {
            return m;
        });
        auto y = Monitored::Collection(Form("y_%s", phimap.first.Data()), phimap.second, [](const int &m) {
            return m;
        });
        fill(m_packageName, x, y);
    }
    for (const auto &phimap : tgcHitPhiMapGlobal) {
        auto x = Monitored::Scalar<int>(Form("lb_for_%s", phimap.first.Data()), GetEventInfo(ctx)->lumiBlock());
        auto y = Monitored::Collection(Form("%s", phimap.first.Data()), phimap.second, [](const int &m) {
            return m;
        });
        auto z = Monitored::Collection(Form("timing_for_%s", phimap.first.Data()), tgcHitTiming[phimap.first], [](const int &m) {
            return m;
        });
        fill(m_packageName, x, y, z);
    }

    SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinCurr(m_TgcCoinDataContainerCurrBCKey, ctx);
    if (!tgcCoinCurr.isValid()) {
        ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name " << m_TgcCoinDataContainerCurrBCKey);
        return StatusCode::FAILURE;
    }
    SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinNext(m_TgcCoinDataContainerNextBCKey, ctx);
    if (!tgcCoinNext.isValid()) {
        ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name " << m_TgcCoinDataContainerNextBCKey);
        return StatusCode::FAILURE;
    }
    SG::ReadHandle < Muon::TgcCoinDataContainer > tgcCoinPrev(m_TgcCoinDataContainerPrevBCKey, ctx);
    if (!tgcCoinPrev.isValid()) {
        ATH_MSG_ERROR("evtStore() does not contain TgcCoinDataContainer with name " << m_TgcCoinDataContainerPrevBCKey);
        return StatusCode::FAILURE;
    }
    std::map<int, SG::ReadHandle<Muon::TgcCoinDataContainer> > tgcCoin;
    tgcCoin[0] = tgcCoinCurr;
    tgcCoin[+1] = tgcCoinNext;
    tgcCoin[-1] = tgcCoinPrev;
    std::vector < TgcTrig > tgcTrigs_SL;
    std::vector < TgcTrig > tgcTrigs_SL_Endcap;
    std::vector < TgcTrig > tgcTrigs_SL_Forward;
    std::vector < TgcTrig > tgcTrigs_HPT_Wire;
    std::vector < TgcTrig > tgcTrigs_HPT_Endcap_Wire;
    std::vector < TgcTrig > tgcTrigs_HPT_Forward_Wire;
    std::vector < TgcTrig > tgcTrigs_HPT_Strip;
    std::vector < TgcTrig > tgcTrigs_HPT_Endcap_Strip;
    std::vector < TgcTrig > tgcTrigs_HPT_Forward_Strip;
    std::vector < TgcTrig > tgcTrigs_LPT_Wire;
    std::vector < TgcTrig > tgcTrigs_LPT_Endcap_Wire;
    std::vector < TgcTrig > tgcTrigs_LPT_Forward_Wire;
    std::vector < TgcTrig > tgcTrigs_LPT_Strip;
    std::vector < TgcTrig > tgcTrigs_LPT_Endcap_Strip;
    std::vector < TgcTrig > tgcTrigs_LPT_Forward_Strip;
    int n_TgcCoin_detElementIsNull = 0;
    int n_TgcCoin_postOutPtrIsNull = 0;
    for (auto thisCoin : tgcCoin) {
        int bunch = thisCoin.first;
        for (auto tgccnt : *(thisCoin.second)) {
            for (const auto &data : *tgccnt) {

	      if ( data->detectorElementOut() == nullptr ) n_TgcCoin_detElementIsNull++;
	      if ( data->posOutPtr() == nullptr ) n_TgcCoin_postOutPtrIsNull++;
	      if ( data->detectorElementOut() == nullptr ||
		   data->posOutPtr() == nullptr )continue; // to avoid FPE

                TgcTrig tgcTrig;
                tgcTrig.lb = GetEventInfo(ctx)->lumiBlock();
                const int type = data->type();
                const Amg::Vector3D &posIn = data->globalposIn();
                tgcTrig.x_In = posIn[0];
                tgcTrig.y_In = posIn[1];
                tgcTrig.z_In = posIn[2];
                const Amg::Vector3D &posOut = data->globalposOut();
                tgcTrig.x_Out = posOut[0];
                tgcTrig.y_Out = posOut[1];
                tgcTrig.z_Out = posOut[2];
                tgcTrig.eta = posOut.eta();
                tgcTrig.phi = posOut.phi();
                tgcTrig.width_In = data->widthIn();
                tgcTrig.width_Out = data->widthOut();
                if (type == Muon::TgcCoinData::TYPE_SL) {
                    const Amg::MatrixX &matrix = data->errMat();
                    tgcTrig.width_R = matrix(0, 0);
                    tgcTrig.width_Phi = matrix(1, 1);
                } else {
                    tgcTrig.width_R = 0.;
                    tgcTrig.width_Phi = 0.;
                }
		int etaout = 0;
		int etain = 0;
		const Identifier tcdidout = data->channelIdOut();
		if (tcdidout.is_valid()) {
		  etaout = std::abs(int(m_idHelperSvc->tgcIdHelper().stationEta(tcdidout)));
		}
		const Identifier tcdidin  = data->channelIdIn();
		if (tcdidin.is_valid()) {
		  etain  = std::abs(int(m_idHelperSvc->tgcIdHelper().stationEta(tcdidin)));
		}
		tgcTrig.etaout = etaout;
		tgcTrig.etain = etain;
                tgcTrig.isAside = data->isAside();
                tgcTrig.isForward = data->isForward();
                tgcTrig.isStrip = data->isStrip();
                tgcTrig.isInner = data->isInner();
                tgcTrig.isPositiveDeltaR = data->isPositiveDeltaR();
                tgcTrig.type = type;
                tgcTrig.trackletId = data->trackletId();
                tgcTrig.trackletIdStrip = data->trackletIdStrip();
                tgcTrig.sector = (data->isAside())?(data->phi()):(-data->phi());
                tgcTrig.roi = data->roi();
                tgcTrig.pt = data->pt();
                tgcTrig.delta = data->delta();
                tgcTrig.sub = data->sub();
                tgcTrig.veto = data->veto();
                tgcTrig.bunch = bunch;
                tgcTrig.inner = data->inner();
                if (type == Muon::TgcCoinData::TYPE_SL && !tgcTrig.isForward) {
		  tgcTrigs_SL_Endcap.push_back(tgcTrig);
		  tgcTrigs_SL.push_back(tgcTrig);
		}else if (type == Muon::TgcCoinData::TYPE_SL && tgcTrig.isForward) {
		  tgcTrigs_SL_Forward.push_back(tgcTrig);
		  tgcTrigs_SL.push_back(tgcTrig);
		}else if(type == Muon::TgcCoinData::TYPE_HIPT && !tgcTrig.isForward){
		  if(tgcTrig.isStrip){
		    tgcTrigs_HPT_Endcap_Strip.push_back(tgcTrig);
		    tgcTrigs_HPT_Strip.push_back(tgcTrig);
		  }else{
		    tgcTrigs_HPT_Endcap_Wire.push_back(tgcTrig);
		    tgcTrigs_HPT_Wire.push_back(tgcTrig);
		  }
		}else if(type == Muon::TgcCoinData::TYPE_HIPT && tgcTrig.isForward){
		  if(tgcTrig.isStrip){
		    tgcTrigs_HPT_Forward_Strip.push_back(tgcTrig);
		    tgcTrigs_HPT_Strip.push_back(tgcTrig);
		  }else{
		    tgcTrigs_HPT_Forward_Wire.push_back(tgcTrig);
		    tgcTrigs_HPT_Wire.push_back(tgcTrig);
		  }
		}else if(type == Muon::TgcCoinData::TYPE_TRACKLET && !tgcTrig.isForward){
		  if(tgcTrig.isStrip){
		    tgcTrigs_LPT_Endcap_Strip.push_back(tgcTrig);
		    tgcTrigs_LPT_Strip.push_back(tgcTrig);
		  }else{
		    tgcTrigs_LPT_Endcap_Wire.push_back(tgcTrig);
		    tgcTrigs_LPT_Wire.push_back(tgcTrig);
		  }
		}else if(type == Muon::TgcCoinData::TYPE_TRACKLET && tgcTrig.isForward){
		  if(tgcTrig.isStrip){
		    tgcTrigs_LPT_Forward_Strip.push_back(tgcTrig);
		    tgcTrigs_LPT_Strip.push_back(tgcTrig);
		  }else{
		    tgcTrigs_LPT_Forward_Wire.push_back(tgcTrig);
		    tgcTrigs_LPT_Wire.push_back(tgcTrig);
		  }
		}
            }
        }
    }

    auto mon_nTgcCoin_detElementIsNull = Monitored::Scalar<int>("nTgcCoinDetElementIsNull", n_TgcCoin_detElementIsNull);
    auto mon_nTgcCoin_postOutPtrIsNull = Monitored::Scalar<int>("nTgcCoinPostOutPtrIsNull", n_TgcCoin_postOutPtrIsNull);
    variables.push_back(mon_nTgcCoin_detElementIsNull);
    variables.push_back(mon_nTgcCoin_postOutPtrIsNull);

    fillTgcCoin(tgcTrigs_SL,"SL");
    fillTgcCoin(tgcTrigs_SL_Endcap,"SL_Endcap");
    fillTgcCoin(tgcTrigs_SL_Forward,"SL_Forward");

    fillTgcCoin(tgcTrigs_HPT_Wire,"HPT_Wire");
    fillTgcCoin(tgcTrigs_HPT_Endcap_Wire,"HPT_Endcap_Wire");
    fillTgcCoin(tgcTrigs_HPT_Forward_Wire,"HPT_Forward_Wire");

    fillTgcCoin(tgcTrigs_HPT_Strip,"HPT_Strip");
    fillTgcCoin(tgcTrigs_HPT_Endcap_Strip,"HPT_Endcap_Strip");
    fillTgcCoin(tgcTrigs_HPT_Forward_Strip,"HPT_Forward_Strip");

    fillTgcCoin(tgcTrigs_LPT_Wire,"LPT_Wire");
    fillTgcCoin(tgcTrigs_LPT_Endcap_Wire,"LPT_Endcap_Wire");
    fillTgcCoin(tgcTrigs_LPT_Forward_Wire,"LPT_Forward_Wire");

    fillTgcCoin(tgcTrigs_LPT_Strip,"LPT_Strip");
    fillTgcCoin(tgcTrigs_LPT_Endcap_Strip,"LPT_Endcap_Strip");
    fillTgcCoin(tgcTrigs_LPT_Forward_Strip,"LPT_Forward_Strip");

    fill(m_packageName, variables);
    return StatusCode::SUCCESS;
}

void TgcRawDataMonitorAlgorithm::fillTgcCoin(const std::vector<TgcTrig>& tgcTrigs, const std::string type ) const {

  MonVariables variables;

  auto coin_lb = Monitored::Collection(type+"_coin_lb", tgcTrigs, [](const TgcTrig &m) {
      return m.lb;
    });
  variables.push_back(coin_lb);
  auto coin_eta = Monitored::Collection(type+"_coin_eta", tgcTrigs, [](const TgcTrig &m) {
      return m.eta;
    });
  variables.push_back(coin_eta);
  auto coin_phi = Monitored::Collection(type+"_coin_phi", tgcTrigs, [](const TgcTrig &m) {
      return m.phi;
    });
  variables.push_back(coin_phi);
  auto coin_bunch = Monitored::Collection(type+"_coin_bunch", tgcTrigs, [](const TgcTrig &m) {
      return m.bunch;
    });
  variables.push_back(coin_bunch);
  auto coin_roi = Monitored::Collection(type+"_coin_roi", tgcTrigs, [](const TgcTrig &m) {
      return m.roi;
    });
  variables.push_back(coin_roi);
  auto coin_sector = Monitored::Collection(type+"_coin_sector", tgcTrigs, [](const TgcTrig &m) {
      return m.sector;
    });
  variables.push_back(coin_sector);
  auto coin_pt = Monitored::Collection(type+"_coin_pt", tgcTrigs, [](const TgcTrig &m) {
      return m.pt;
    });
  variables.push_back(coin_pt);
  auto coin_veto = Monitored::Collection(type+"_coin_veto", tgcTrigs, [](const TgcTrig &m) {
      return m.veto;
    });
  variables.push_back(coin_veto);
  auto coin_veto_sector = Monitored::Collection(type+"_coin_veto_sector", tgcTrigs, [](const TgcTrig &m) {
      return (m.veto==1)?(m.sector):(-1);
    });
  variables.push_back(coin_veto_sector);
  auto coin_veto_roi = Monitored::Collection(type+"_coin_veto_roi", tgcTrigs, [](const TgcTrig &m) {
      return (m.veto==1)?(m.roi):(-1);
    });
  variables.push_back(coin_veto_roi);
  auto coin_isPositiveDeltaR = Monitored::Collection(type+"_coin_isPositiveDeltaR", tgcTrigs, [](const TgcTrig &m) {
      return m.isPositiveDeltaR;
    });
  variables.push_back(coin_isPositiveDeltaR);

  auto coin_cutmask_pt1 = Monitored::Collection(type+"_coin_cutmask_pt1", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==1);
    });
  variables.push_back(coin_cutmask_pt1);
  auto coin_cutmask_pt2 = Monitored::Collection(type+"_coin_cutmask_pt2", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==2);
    });
  variables.push_back(coin_cutmask_pt2);
  auto coin_cutmask_pt3 = Monitored::Collection(type+"_coin_cutmask_pt3", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==3);
    });
  variables.push_back(coin_cutmask_pt3);
  auto coin_cutmask_pt4 = Monitored::Collection(type+"_coin_cutmask_pt4", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==4);
    });
  variables.push_back(coin_cutmask_pt4);
  auto coin_cutmask_pt5 = Monitored::Collection(type+"_coin_cutmask_pt5", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==5);
    });
  variables.push_back(coin_cutmask_pt5);
  auto coin_cutmask_pt6 = Monitored::Collection(type+"_coin_cutmask_pt6", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==6);
    });
  variables.push_back(coin_cutmask_pt6);
  auto coin_cutmask_pt7 = Monitored::Collection(type+"_coin_cutmask_pt7", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==7);
    });
  variables.push_back(coin_cutmask_pt7);
  auto coin_cutmask_pt8 = Monitored::Collection(type+"_coin_cutmask_pt8", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==8);
    });
  variables.push_back(coin_cutmask_pt8);
  auto coin_cutmask_pt9 = Monitored::Collection(type+"_coin_cutmask_pt9", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==9);
    });
  variables.push_back(coin_cutmask_pt9);
  auto coin_cutmask_pt10 = Monitored::Collection(type+"_coin_cutmask_pt10", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==10);
    });
  variables.push_back(coin_cutmask_pt10);
  auto coin_cutmask_pt11 = Monitored::Collection(type+"_coin_cutmask_pt11", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==11);
    });
  variables.push_back(coin_cutmask_pt11);
  auto coin_cutmask_pt12 = Monitored::Collection(type+"_coin_cutmask_pt12", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==12);
    });
  variables.push_back(coin_cutmask_pt12);
  auto coin_cutmask_pt13 = Monitored::Collection(type+"_coin_cutmask_pt13", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==13);
    });
  variables.push_back(coin_cutmask_pt13);
  auto coin_cutmask_pt14 = Monitored::Collection(type+"_coin_cutmask_pt14", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==14);
    });
  variables.push_back(coin_cutmask_pt14);
  auto coin_cutmask_pt15 = Monitored::Collection(type+"_coin_cutmask_pt15", tgcTrigs, [](const TgcTrig &m) {
      return (m.pt==15);
    });
  variables.push_back(coin_cutmask_pt15);

  fill(m_packageName, variables);

}
///////////////////////////////////////////////////////////////
bool TgcRawDataMonitorAlgorithm::triggerMatching(const xAOD::Muon *offline_muon, const std::vector<TagDef> &list_of_triggers) const {
    if (!m_TagAndProbe.value()) return true;
    if (getTrigDecisionTool().empty()) return true;
    TVector3 muonvec;
    muonvec.SetPtEtaPhi(offline_muon->pt(), offline_muon->eta(), offline_muon->phi());
    for (const auto &tagTrig : list_of_triggers) {
        if (!getTrigDecisionTool()->isPassed(tagTrig.eventTrig.Data())) return false;
        auto features = getTrigDecisionTool()->features < xAOD::MuonContainer > (tagTrig.tagTrig.Data(), TrigDefs::Physics, m_MuonEFContainerName.value());
        for (auto aaa : features) {
            if (!aaa.isValid()) return false;
            auto trigmuon_link = aaa.link;
            auto trigmuon = *trigmuon_link;
            TVector3 trigvec;
            trigvec.SetPtEtaPhi(trigmuon->pt(), trigmuon->eta(), trigmuon->phi());
            if (trigvec.DeltaR(muonvec) < m_trigMatchWindow.value()) return true;
        }
    }
    return false;
}
///////////////////////////////////////////////////////////////
void TgcRawDataMonitorAlgorithm::extrapolate(const xAOD::Muon *muon, MyMuon &mymuon) const {
    const xAOD::TrackParticle *track = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);
    if (!track) return;
    mymuon.extPos.reserve(m_extZposition.size());
    mymuon.extVec.reserve(m_extZposition.size());
    for (const auto &z : m_extZposition) {
        Amg::Vector2D etaDeta(0, 0);
        Amg::Vector2D phiDphi(0, 0);
        Amg::Vector3D mom(0, 0, 0);
        Amg::Vector3D extrapolateTo(0, 0, z);
        if (extrapolate(track, extrapolateTo, TGC, etaDeta, phiDphi, mom)) {
            double pt = extrapolateTo.z() / std::sinh(etaDeta[0]);
            TVector3 vec, pos;
            pos.SetPtEtaPhi(pt, etaDeta[0], phiDphi[0]);
            vec.SetXYZ(mom.x(), mom.y(), mom.z());
            mymuon.extPosZ.push_back(z);
            mymuon.extPos.push_back(pos);
            mymuon.extVec.push_back(vec);
        }
    }
}
///////////////////////////////////////////////////////////////
/* track extrapolator tool */
bool TgcRawDataMonitorAlgorithm::extrapolate(const xAOD::TrackParticle *trackParticle, const Amg::Vector3D &pos, const int detector, Amg::Vector2D &eta, Amg::Vector2D &phi, Amg::Vector3D &mom) const {
    if (!trackParticle) {
        return false;
    }
    const Trk::Track *track = trackParticle->track();
    if (!track) {
        return false;
    }
    const DataVector<const Trk::TrackStateOnSurface> *trackOnSurf = track->trackStateOnSurfaces();
    if (!trackOnSurf) {
        return false;
    }
    std::vector<double> vExtEta, vExtPhi, vExtPx, vExtPy, vExtPz, vExtDeltaEta, vExtDeltaPhi, vExtDistance;
    int counter = 0;
    for (auto it : *trackOnSurf) {
        if (!it->type(Trk::TrackStateOnSurface::Measurement)) {
            continue;
        }
        std::unique_ptr<const Trk::TrackParameters> point = nullptr;
        Amg::Vector2D distance(-1.e30, -1.e30);
        if (detector == TgcRawDataMonitorAlgorithm::TGC) {
            point = extrapolateToTGC(it, pos, distance);
        } else if (detector == TgcRawDataMonitorAlgorithm::RPC) {
            point = extrapolateToRPC(it, pos, distance);
        } else {
            continue;
        }
        if (!point) {
            continue;
        }
        vExtEta.push_back(point->position().eta());
        vExtPhi.push_back(point->position().phi());
        vExtPx.push_back(point->momentum().x());
        vExtPy.push_back(point->momentum().y());
        vExtPz.push_back(point->momentum().z());
        vExtDistance.push_back(distance.y());
        counter++;
    }
    if (counter == 0) {
        return false;
    }
    double minimum = 1.e30;
    int number = -1;
    for (int ii = 0; ii < counter; ii++) {
        if (minimum > vExtDistance.at(ii)) {
            minimum = vExtDistance.at(ii);
            number = ii;
        }
    }
    if (number == -1) {
        return false;
    }
    eta[0] = vExtEta.at(number);
    phi[0] = vExtPhi.at(number);
    eta[1] = getError(vExtEta);
    phi[1] = getError(vExtPhi);
    mom[0] = vExtPx.at(number);
    mom[1] = vExtPy.at(number);
    mom[2] = vExtPz.at(number);
    return true;
}
///////////////////////////////////////////////////////////////
std::unique_ptr<const Trk::TrackParameters> TgcRawDataMonitorAlgorithm::extrapolateToTGC(const Trk::TrackStateOnSurface *tsos, const Amg::Vector3D &pos, Amg::Vector2D &distance) const {
    const Trk::TrackParameters *track = tsos->trackParameters();
    if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
        return nullptr;
    }
    double targetZ = pos.z();
    double trackZ = track->position().z();
    if (std::abs(trackZ) < std::abs(targetZ) - 2000. || std::abs(trackZ) > std::abs(targetZ) + 2000.) {
        return nullptr;
    }
    auto matrix = std::make_unique<Amg::Transform3D>();
    matrix->setIdentity();
    matrix->translation().z() = targetZ;
    auto disc = std::make_unique < Trk::DiscSurface > (matrix.release(), m_endcapPivotPlaneMinimumRadius.value(), m_endcapPivotPlaneMaximumRadius.value());
    distance[0] = trackZ;
    distance[1] = std::abs(trackZ - targetZ);
    const bool boundaryCheck = true;
    auto param = std::unique_ptr<const Trk::TrackParameters>(m_extrapolator->extrapolate(*track, *disc, Trk::anyDirection, boundaryCheck, Trk::muon));
    if (!param) {
        return nullptr;
    }
    // We want disc
    if (param->surfaceType() != Trk::Surface::Disc) {
        return nullptr;
    }
    return param;
}
///////////////////////////////////////////////////////////////
std::unique_ptr<const Trk::TrackParameters> TgcRawDataMonitorAlgorithm::extrapolateToRPC(const Trk::TrackStateOnSurface *tsos, const Amg::Vector3D &pos, Amg::Vector2D &distance) const {
    const Trk::TrackParameters *track = tsos->trackParameters();
    if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
        return nullptr;
    }
    double radius = pos.perp();
    double trackRadius = track->position().perp();
    if (trackRadius < radius - 2000. || trackRadius > radius + 2000.) {
        return nullptr;
    }
    auto matrix = std::make_unique<Amg::Transform3D>();
    matrix->setIdentity();
    auto cylinder = std::make_unique < Trk::CylinderSurface > (matrix.release(), radius, m_barrelPivotPlaneHalfLength.value());

    distance[0] = trackRadius;
    distance[1] = trackRadius - radius;
    const bool boundaryCheck = true;
    auto param = std::unique_ptr<const Trk::TrackParameters>(m_extrapolator->extrapolate(*track, *cylinder, Trk::anyDirection, boundaryCheck, Trk::muon));

    if (!param) {
        return nullptr;
    }
    //It has to be cylinder
    if (param->surfaceType() != Trk::Surface::Cylinder) {
        return nullptr;
    }
    return param;
}
///////////////////////////////////////////////////////////////
double TgcRawDataMonitorAlgorithm::getError(const std::vector<double> &inputVec) const {
    /// Assign dummy value of 1 as error
    if (inputVec.empty()) return 1.;

    const int nSize = inputVec.size();
    double sum = 0;
    double sum2 = 0;
    for (int ii = 0; ii < nSize; ii++) {
        sum += inputVec.at(ii);
    }
    const double mean = sum / nSize;
    for (int jj = 0; jj < nSize; jj++) {
        sum2 += std::pow((inputVec.at(jj) - mean), 2);
    }
    const double stdDev = std::sqrt(sum2 / nSize);
    return stdDev;
}

