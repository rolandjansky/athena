/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternTools/MuonHoughPatternFinderTool.h"

#include <map>
#include <memory>
#include <set>

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonHoughPatternEvent/MuonHoughHitContainer.h"
#include "MuonPrepRawData/MdtDriftCircleStatus.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"  // for csc's
#include "TFile.h"
#include "TH1F.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/MatchDCWithLine.h"
#include "TrkDriftCircleMath/TangentToCircles.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/ConcurrencyFlags.h"

using namespace TrkDriftCircleMath;

namespace Muon {

    MuonHoughPatternFinderTool::MuonHoughPatternFinderTool(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t, n, p), m_weight_csc_on_segment(2.) {
        declareInterface<IMuonHoughPatternFinderTool>(this);
    }
    MuonHoughPatternFinderTool::~MuonHoughPatternFinderTool() = default;
    StatusCode MuonHoughPatternFinderTool::initialize() {
        if (m_use_histos) {
            if (Gaudi::Concurrency::ConcurrencyFlags::numThreads() > 1) {
              ATH_MSG_FATAL("Filling histograms not supported in MT jobs.");
              return StatusCode::FAILURE;
            }

            m_h = std::make_unique<Hists>();
            m_h->m_file = std::make_unique<TFile>("Hough_histos.root", "RECREATE");
            m_h->m_weighthistogram = std::make_unique<TH1F>("weighthisto", "weighthisto", 100, -0.5, 2);
            m_h->m_weighthistogrammdt = std::make_unique<TH1F>("weighthistomdt", "weighthistomdt", 100, -0.3, 2.2);
            m_h->m_weighthistogramrpc = std::make_unique<TH1F>("weighthistorpc", "weighthistorpc", 100, -0.3, 2.2);
            m_h->m_weighthistogramcsc = std::make_unique<TH1F>("weighthistocsc", "weighthistocsc", 100, -0.3, 2.2);
            m_h->m_weighthistogramtgc = std::make_unique<TH1F>("weighthistotgc", "weighthistotgc", 100, -0.3, 2.2);
        }

        ATH_MSG_VERBOSE("MuonHoughPatternFinderTool::Initializing");
        ATH_CHECK(m_muonCombinePatternTool.retrieve());
        ATH_MSG_VERBOSE("found Service MuonCombinePatternTool " << m_muonCombinePatternTool);

        ATH_CHECK(m_muonHoughPatternTool.retrieve());
        ATH_MSG_VERBOSE("found Service muonHoughPatternTool: " << m_muonHoughPatternTool);

        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_idHelperSvc);

        ATH_CHECK(m_printer.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_printer);

        if (m_hit_reweights) { ATH_MSG_DEBUG("Hit Reweighting " << m_hit_reweights); }

        ATH_CHECK(m_CosmicPhiPatternsKey.initialize(m_recordAllOutput));
        ATH_CHECK(m_CosmicEtaPatternsKey.initialize(m_recordAllOutput));
        ATH_CHECK(m_COMBINED_PATTERNSKey.initialize(m_recordAllOutput));

        ATH_MSG_VERBOSE("End of Initializing");
        return StatusCode::SUCCESS;
    }

    template <class T> std::vector<const T*> MuonHoughPatternFinderTool::stdVec(const MuonPrepDataContainer<T>* cont) const {
        std::vector<const T*> vec;
        if (cont) {
            vec.reserve(cont->size());
            for (const auto& ele : *cont) { vec.push_back(ele); }
        }
        return vec;
    }
    std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<HoughDataPerSectorVec>> MuonHoughPatternFinderTool::find(
        const MdtPrepDataContainer* mdtCont, const CscPrepDataContainer* cscCont, const TgcPrepDataContainer* tgcCont,
        const RpcPrepDataContainer* rpcCont, const sTgcPrepDataContainer* stgcCont, const MMPrepDataContainer* mmCont,
        const EventContext& ctx) const {
        if (stgcCont || mmCont) {
            ATH_MSG_FATAL("NSW is not yet implemented in this tool");
            return std::make_pair(std::make_unique<MuonPatternCombinationCollection>(), std::make_unique<HoughDataPerSectorVec>());
        }
        return find(stdVec(mdtCont), stdVec(cscCont), stdVec(tgcCont), stdVec(rpcCont), nullptr, ctx);
    }
    std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<Muon::HoughDataPerSectorVec>>
    MuonHoughPatternFinderTool::find(const std::vector<const MdtPrepDataCollection*>& mdtCols,
                                     const std::vector<const CscPrepDataCollection*>& cscCols,
                                     const std::vector<const TgcPrepDataCollection*>& tgcCols,
                                     const std::vector<const RpcPrepDataCollection*>& rpcCols,
                                     const MuonSegmentCombinationCollection* cscSegmentCombis, const EventContext& ctx) const {
        /** map between mdt chamber identifiers and corresponding rpc hits
         * (hit_no_begin and hit_no_end)*/
        std::map<int, std::vector<std::pair<int, int>>> rpcmdtstationmap;
        /** map between mdt chamber identifiers and corresponding tgc hits
         * (hit_no_begin and hit_no_end)*/
        std::map<int, std::vector<std::pair<int, int>>> tgcmdtstationmap;

        /** map for association between trigger eta hits (first) and phi hits (second)
         * within the same gasgap, used for combining patterns in
         * MuonCombinePatternTool */
        auto phietahitassociation =
            std::make_unique<std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>>();

        // read event_data:
        std::unique_ptr<MuonHoughHitContainer> hitcontainer{
            getAllHits(mdtCols, cscCols, tgcCols, rpcCols, cscSegmentCombis, rpcmdtstationmap, tgcmdtstationmap, *phietahitassociation)};
        // analyse data
        std::unique_ptr<MuonPatternCombinationCollection> patCombiCol;
        if (hitcontainer) {
            patCombiCol.reset(analyse(*hitcontainer, phietahitassociation.get(), ctx));
        } else {
            ATH_MSG_INFO(" No hit container created! ");
        }

        // ensure we always output a collection
        if (!patCombiCol) {
            ATH_MSG_DEBUG(" NO pattern combinations found, creating empty collection ");
            patCombiCol = std::make_unique<MuonPatternCombinationCollection>();
        }

        // summary
        if (m_summary || msgLvl(MSG::DEBUG)) {
            if (patCombiCol->empty())
                ATH_MSG_DEBUG(" summarizing output: Combined pattern combination empty");
            else
                ATH_MSG_DEBUG(" summarizing Combined pattern combination output: " << m_printer->print(*patCombiCol));
        }

        // clean up tool for next call

        // clear stationmaps
        rpcmdtstationmap.clear();
        tgcmdtstationmap.clear();
        // clear etaphi association map
        phietahitassociation->clear();

        ATH_MSG_VERBOSE("execute(end) ");

        // return result
        return {std::move(patCombiCol), nullptr};
    }

    StatusCode MuonHoughPatternFinderTool::finalize() {
        if (m_use_histos) {
            auto save_histo = [this](std::unique_ptr<TH1>& h_ptr) {
                if (!h_ptr) return;
                m_h->m_file->WriteObject(h_ptr.get(), h_ptr->GetName());
                h_ptr.reset();
            };
            save_histo(m_h->m_weighthistogram);
            save_histo(m_h->m_weighthistogrammdt);
            save_histo(m_h->m_weighthistogramrpc);
            save_histo(m_h->m_weighthistogramcsc);
            save_histo(m_h->m_weighthistogramtgc);
        }
        ATH_MSG_VERBOSE("finalize()");

        return StatusCode::SUCCESS;
    }

    MuonPatternCombinationCollection* MuonHoughPatternFinderTool::analyse(
        const MuonHoughHitContainer& hitcontainer,
        const std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>* phietahitassociation,
        const EventContext& ctx) const {
        ATH_MSG_DEBUG("size of event: " << hitcontainer.size());

        /** reconstructed patterns stored per [number_of_ids][level][which_segment] */
        MuonHoughPatternContainerShip houghpattern = m_muonHoughPatternTool->emptyHoughPattern();
        //  pass through hitcontainer (better still: preprawdata and only after make
        //  internal hitcontainer)
        m_muonHoughPatternTool->makePatterns(hitcontainer, houghpattern);

        std::unique_ptr<MuonPrdPatternCollection> phipatterns{m_muonHoughPatternTool->getPhiMuonPatterns(houghpattern)};
        std::unique_ptr<MuonPrdPatternCollection> etapatterns{m_muonHoughPatternTool->getEtaMuonPatterns(houghpattern)};

        if (m_summary || msgLvl(MSG::DEBUG)) {
            if (phipatterns->empty())
                ATH_MSG_DEBUG(" summarizing input: Phi pattern combination empty");
            else
                ATH_MSG_DEBUG(" summarizing Phi pattern combination input: " << std::endl << m_printer->print(*phipatterns));
            if (etapatterns->empty())
                ATH_MSG_DEBUG(" summarizing input: Eta pattern combination empty");
            else
                ATH_MSG_DEBUG(" summarizing Eta pattern combination input: " << std::endl << m_printer->print(*etapatterns));
        }

        ATH_MSG_DEBUG("writePatterns");
        ATH_MSG_DEBUG("size: phi: " << phipatterns->size() << " eta: " << etapatterns->size());

        std::unique_ptr<MuonPrdPatternCollection> combinedpatterns;
        MuonPatternCombinationCollection* patterncombinations = nullptr;

        // make + write muonpatterncombinations
        if (!etapatterns->empty()) {
            combinedpatterns.reset(
                m_muonCombinePatternTool->combineEtaPhiPatterns(phipatterns.get(), etapatterns.get(), phietahitassociation));
        }

        if (combinedpatterns) {
            patterncombinations = m_muonCombinePatternTool->makePatternCombinations(combinedpatterns.get());
        } else {
            ATH_MSG_DEBUG("No combined patterns, creating dummy.");
            combinedpatterns = std::make_unique<MuonPrdPatternCollection>();
        }

        record(phipatterns, m_CosmicPhiPatternsKey, ctx);
        record(etapatterns, m_CosmicEtaPatternsKey, ctx);
        record(combinedpatterns, m_COMBINED_PATTERNSKey, ctx);

        /** empty and clear the houghpattern vectors */
        houghpattern.clear();
        return patterncombinations;
    }

    std::unique_ptr<MuonHoughHitContainer> MuonHoughPatternFinderTool::getAllHits(
        const std::vector<const MdtPrepDataCollection*>& mdtCols, const std::vector<const CscPrepDataCollection*>& cscCols,
        const std::vector<const TgcPrepDataCollection*>& tgcCols, const std::vector<const RpcPrepDataCollection*>& rpcCols,
        const MuonSegmentCombinationCollection* cscSegmentCombis, std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap,
        std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap,
        std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const {
        ATH_MSG_VERBOSE("getAllHits()");

        std::unique_ptr<MuonHoughHitContainer> hitcontainer = std::make_unique<MuonHoughHitContainer>();
        // reserve space for 5000 hits (arbitrary), this should gain some cpu/memory
        // for background, but will lose for lower occupancy. If anyone knows a way to
        // predict the number of muon hits, I'd like to hear it.
        hitcontainer->reserve(5000);

        const bool use_csc_segments = (cscSegmentCombis != nullptr);

        if (use_csc_segments && m_use_csc) {
            std::set<Identifier> csc_set;  // set to make sure every csc hit is only
                                           // passed to hitcontainer once
            std::pair<std::set<Identifier>::iterator, bool> csc_pair;
            std::map<int, int> number_of_hits_per_layer;  // map that connect layer number (1000*eta +
                                                          // 100*phi + 10*chamberlayer+ 2*wirelayer +
                                                          // eta/phi)

            std::vector<const Muon::CscClusterOnTrack*> csc_rots;  // csc rots
            std::vector<const Muon::CscPrepData*> csc_prds;

            std::vector<int> layer_ids;  // if 0 then prd already added

            csc_rots.reserve(400);  // again arbitrary, atm (May 2008), the max number of
                                    // csc segments is 50 (times 8 hits = 400)
            csc_prds.reserve(400);
            layer_ids.reserve(400);

            // two loops needed as number of hits per layer needs to be known
            for (const Muon::MuonSegmentCombination* msc : *cscSegmentCombis) {
                ATH_MSG_VERBOSE("CSC combo segments loop, segmentcombo " << msc);
                for (unsigned int ss = 0; ss < msc->numberOfStations(); ++ss) {
                    for (const std::unique_ptr<MuonSegment>& ms : *msc->stationSegments(ss)) {
                        if (!ms) {
                            ATH_MSG_DEBUG("Segment has been already skimmed");
                            continue;
                        }
                        ATH_MSG_VERBOSE("CSC segments loop, segment: " << ms.get());
                        std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess> phi_set;
                        std::vector<const Trk::PrepRawData*> eta_vector;
                        std::pair<std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>::iterator, bool> phi_pair;

                        int nRoTs = ms->numberOfContainedROTs();
                        for (int i = 0; i < nRoTs; ++i) {
                            const Muon::CscClusterOnTrack* cscOnSeg = dynamic_cast<const Muon::CscClusterOnTrack*>(ms->rioOnTrack(i));
                            if (!cscOnSeg) {
                                ATH_MSG_INFO("Dynamic cast to CscClusterOnTrack failed!");
                                continue;
                            }
                            const Muon::CscPrepData* prd = dynamic_cast<const Muon::CscPrepData*>(cscOnSeg->prepRawData());
                            if (!prd) {
                                ATH_MSG_INFO("Dynamic cast to CscPrepData failed!");
                                continue;
                            }

                            csc_rots.push_back(cscOnSeg);
                            csc_prds.push_back(prd);

                            Identifier id = prd->identify();
                            bool channel_type = m_idHelperSvc->cscIdHelper().measuresPhi(id);
                            csc_pair = csc_set.insert(id);
                            if (!csc_pair.second) {
                                ATH_MSG_DEBUG(" CSC hit was already added, weight set to 0");
                                layer_ids.push_back(0);
                            } else {
                                const int layer_id = 1000 * m_idHelperSvc->cscIdHelper().stationEta(id) +
                                                     100 * m_idHelperSvc->cscIdHelper().stationPhi(id) +
                                                     10 * m_idHelperSvc->cscIdHelper().chamberLayer(id) +
                                                     2 * m_idHelperSvc->cscIdHelper().wireLayer(id) + channel_type;
                                ATH_MSG_DEBUG("csc layer_id: " << layer_id);
                                ++number_of_hits_per_layer[layer_id];
                                layer_ids.push_back(layer_id);
                            }

                            if (channel_type) {  // phi hit
                                phi_pair = phi_set.insert(prd);
                                if (!phi_pair.second) { ATH_MSG_INFO(" CSC phi hit was already added"); }
                            } else {  // eta hit
                                eta_vector.push_back(prd);
                            }
                        }  // rots
                        // add hit association from segment to map:
                        if (!phi_set.empty()) {
                            ATH_MSG_VERBOSE("Number of Phi Csc hits in segment: " << phi_set.size());
                            for (const Trk::PrepRawData* prd : eta_vector) { phietahitassociation.insert(std::make_pair(prd, phi_set)); }
                        }
                    }
                }
            }

            for (unsigned int i = 0; i < csc_rots.size(); i++) {
                const Muon::CscPrepData* prd = csc_prds[i];

                const Amg::Vector3D& globalpos = csc_rots[i]->globalPosition();
                bool channel_type = m_idHelperSvc->cscIdHelper().measuresPhi(csc_rots[i]->identify());

                double weight = 0.;
                if (layer_ids[i] != 0) {  // not yet added
                    double number_of_hits = (double)number_of_hits_per_layer[layer_ids[i]];
                    weight = m_weight_csc_on_segment / (0.75 * std::sqrt(number_of_hits) + 0.25 * number_of_hits);
                }

                ATH_MSG_DEBUG(m_printer->print(*prd) << " weight " << weight);
                MuonHoughHit* hit = new MuonHoughHit(globalpos, channel_type, MuonHough::CSC, 1., weight, prd);

                hitcontainer->addHit(hit);
                if (m_use_histos) {
                  Hists& h = getHists();
                  h.m_weighthistogram->Fill(weight);
                  h.m_weighthistogramcsc->Fill(weight);
                }
            }
        }  // use_csc_segments
        // taken and modified from
        // DetectorDescription/GeoModel/HitDisplay/src/HitDisplaySystem.cxx

        if (m_use_rpc) {
            for (const RpcPrepDataCollection* rpc_coll : rpcCols) {
                addRpcCollection(rpc_coll, *hitcontainer, rpcmdtstationmap, phietahitassociation);
            }
        }

        if (m_use_tgc) {
            for (const TgcPrepDataCollection* tgc_coll : tgcCols) {
                addTgcCollection(tgc_coll, *hitcontainer, tgcmdtstationmap, phietahitassociation);
            }
        }

        if (m_use_mdt) {
            for (const MdtPrepDataCollection* prep_coll : mdtCols) {
                addMdtCollection(prep_coll, *hitcontainer, rpcmdtstationmap, tgcmdtstationmap);
            }
        }

        if (m_use_csc && !use_csc_segments) {
            for (const CscPrepDataCollection* csc_coll : cscCols) { addCscCollection(csc_coll, *hitcontainer, phietahitassociation); }
        }

        if (msgLevel(MSG::VERBOSE)) {
            ATH_MSG_VERBOSE("MuonHoughPatternFinderTool::getAllHits() saving " << hitcontainer->size() << " converted hits");
            for (unsigned int i = 0; i < hitcontainer->size(); i++) {
                ATH_MSG_VERBOSE(" hit " << hitcontainer->getHit(i)->getWhichDetector() << " (" << hitcontainer->getHit(i)->getHitx() << ","
                                        << hitcontainer->getHit(i)->getHity() << "," << hitcontainer->getHit(i)->getHitz() << ") "
                                        << " weight: " << hitcontainer->getHit(i)->getWeight()
                                        << " measures phi: " << hitcontainer->getHit(i)->getMeasuresPhi());
            }
        }

        ATH_MSG_VERBOSE("MuonHoughPatternFinderTool::getAllHits() saving " << phietahitassociation.size() << "associated hits ");
        return hitcontainer;

    }  // getAllHits

    void MuonHoughPatternFinderTool::record(std::unique_ptr<MuonPrdPatternCollection>& patCol,
                                            const SG::WriteHandleKey<MuonPrdPatternCollection>& key, const EventContext& ctx) const {
        if (!patCol) {
            ATH_MSG_WARNING("Zero pointer, could not save patterns!!! ");
            return;
        }

        // check whether we are writing patterns to storegate, if not delete pattern
        if (!m_recordAllOutput) {
            ATH_MSG_DEBUG("Deleted patterns: " << patCol->size() << "  at " << key.key());
            // since patCol Datavector, it owns (by defaults its elements)

        } else {
            SG::WriteHandle<MuonPrdPatternCollection> handle(key, ctx);
            StatusCode sc = handle.record(std::move(patCol));
            if (sc.isFailure()) {
                ATH_MSG_WARNING("Could not save patterns at " << key.key());
            } else {
                ATH_MSG_DEBUG("Saved patterns: " << patCol->size() << "  at " << key.key());
            }
        }
    }
    void MuonHoughPatternFinderTool::addRpcCollection(
        const RpcPrepDataCollection* rpc_coll, MuonHoughHitContainer& hitcontainer,
        std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap,
        std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const {
        if (rpc_coll->size() == 0) return;
        std::map<int, int> number_of_hits_per_layer;
        std::set<int> layers;  // different layer definition between the two!!

        int size_begin = hitcontainer.size();

        if (m_hit_reweights)  // reweight  hits, according to Peter's new algorithm
        {
            std::vector<double> strips(2 * m_idHelperSvc->rpcIdHelper().stripMax() +
                                       2);  // filled strips, to determine whether it was no noise rpc hit
                                            // (confirmation of ((neighbouring) layer))

            for (const Muon::RpcPrepData* prd : *rpc_coll) {
                const bool channel_type = m_idHelperSvc->rpcIdHelper().measuresPhi(prd->identify());
                const Identifier id = prd->identify();
                int strip = m_idHelperSvc->rpcIdHelper().strip(id);  // strip between 1 and 99!!
                if (channel_type) { strip += m_idHelperSvc->rpcIdHelper().stripMax(); }
                strips[strip] += 1.;
                strips[strip + 1] += 0.5;
                strips[strip - 1] += 0.5;
            }
            for (const Muon::RpcPrepData* prd : *rpc_coll) {
                Identifier id = prd->identify();
                const bool channel_type = m_idHelperSvc->rpcIdHelper().measuresPhi(prd->identify());
                int strip = m_idHelperSvc->rpcIdHelper().strip(id);  // strip between 1 and 99!!
                if (channel_type) { strip += m_idHelperSvc->rpcIdHelper().stripMax(); }

                if (strips[strip] > 1) {
                    const int doubletR = m_idHelperSvc->rpcIdHelper().doubletR(id);
                    const int doubletPhi = m_idHelperSvc->rpcIdHelper().doubletPhi(id);
                    const int doubletZ = m_idHelperSvc->rpcIdHelper().doubletZ(id);
                    const int gasGap = m_idHelperSvc->rpcIdHelper().gasGap(id);
                    int layer_number =
                        (gasGap - 1) * 12 + (doubletR - 1) * 6 + (doubletPhi - 1) * 3 + (doubletZ - 1);  // layer_number ranges from 0..35
                    if (channel_type) layer_number = layer_number + 36;

                    ++number_of_hits_per_layer[layer_number];

                    int layer = 2 * (doubletR - 1) + (gasGap - 1);  // layer ranges from 0..8
                    if (channel_type) layer = layer + 4;

                    layers.insert(layer);
                    ATH_MSG_VERBOSE("layer_number: " << layer_number << " doubletR: " << doubletR << " doubletZ: " << doubletZ
                                                     << " doubletPhi " << doubletPhi << " gasGap " << gasGap << " layer " << layer);
                }
            }
        }

        std::map<const Identifier, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>
            gasgapphimap;  // map between gasgapidentifier and phi hits
        std::map<const Identifier, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>::iterator gg_it;
        std::pair<std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>::iterator, bool> gg_insert;

        for (const Muon::RpcPrepData* prd : *rpc_coll) {
            const Amg::Vector3D& globalpos = prd->globalPosition();
            const Identifier id = prd->identify();
            const bool channel_type = m_idHelperSvc->rpcIdHelper().measuresPhi(id);

            // gasgapmap

            if (channel_type)  // phi hit
            {
                const Identifier gasGapId = m_idHelperSvc->gasGapId(id);
                gg_it = gasgapphimap.find(gasGapId);
                if (gg_it == gasgapphimap.end()) {  // gasgapid not yet in map
                    gasgapphimap[gasGapId] = {prd};
                } else {  // gasgapid already in set
                    gg_insert = (*gg_it).second.insert(prd);
                    if (!gg_insert.second) { ATH_MSG_DEBUG("WARNING::RPC hit already in set? "); }
                }
            }

            double weight = 1.;
            double prob = 1.;

            if (m_hit_reweights) {
                if (layers.size() <= 1) {
                    weight = 0.;
                    prob = 0.;
                } else {
                    const int doubletZ = m_idHelperSvc->rpcIdHelper().doubletZ(id);
                    const int doubletPhi = m_idHelperSvc->rpcIdHelper().doubletPhi(id);
                    const int doubletR = m_idHelperSvc->rpcIdHelper().doubletR(id);
                    const int gasGap = m_idHelperSvc->rpcIdHelper().gasGap(id);
                    int layer_number =
                        (gasGap - 1) * 12 + (doubletR - 1) * 6 + (doubletPhi - 1) * 3 + (doubletZ - 1);  // layer_number ranges from 0..35
                    if (channel_type) layer_number = layer_number + 36;

                    double number_of_hits = (double)number_of_hits_per_layer[layer_number];

                    if (number_of_hits > 0) {
                        //	      weight = 1. /
                        //(0.75*std::sqrt(number_of_hits)+0.25*number_of_hits);
                        weight = 1. / (0.25 * std::sqrt(number_of_hits) + 0.75 * number_of_hits);
                        if (layers.size() == 2) weight = weight / 2.;
                    } else {
                        weight = 0.;
                        prob = 0.;
                    }
                }
            }
            MuonHoughHit* hit = new MuonHoughHit(globalpos, channel_type, MuonHough::RPC, prob, weight, prd);
            hitcontainer.addHit(hit);
            ATH_MSG_DEBUG(m_printer->print(*prd) << " NEW weight " << weight);

            if (m_use_histos) {
              Hists& h = getHists();
              h.m_weighthistogram->Fill(weight);
              h.m_weighthistogramrpc->Fill(weight);
            }
        }

        int size_end = hitcontainer.size();

        updateRpcMdtStationMap((*rpc_coll->begin())->identify(), size_begin, size_end, rpcmdtstationmap);

        // extract preprawdata from gasgapmap // might not be fastest way (filling
        // immidiately saves this second loop)

        for (const Muon::RpcPrepData* prd : *rpc_coll) {
            const Identifier id = prd->identify();
            if (!m_idHelperSvc->rpcIdHelper().measuresPhi(id)) {  // eta hit

                const Identifier gasGapId = m_idHelperSvc->gasGapId(id);

                gg_it = gasgapphimap.find(gasGapId);
                if (gg_it != gasgapphimap.end()) { phietahitassociation.insert(std::make_pair(prd, (*gg_it).second)); }
            }
        }
    }

    void MuonHoughPatternFinderTool::addTgcCollection(
        const Muon::TgcPrepDataCollection* tgc_coll, MuonHoughHitContainer& hitcontainer,
        std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap,
        std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const {
        if (tgc_coll->size() == 0) return;
        std::map<int, int> number_of_hits_per_layer;
        std::set<int> layers;  // different layer definition between the two!!

        int size_begin = hitcontainer.size();

        if (m_hit_reweights) {
            std::vector<double> channels(2 * m_idHelperSvc->tgcIdHelper().channelMax() +
                                         2);  // filled strips, to determine whether it was no noise rpc hit
                                              // (confirmation of ((neighbouring) layer))
            for (const Muon::TgcPrepData* prd : *tgc_coll) {
                Identifier id = prd->identify();
                bool channel_type = m_idHelperSvc->tgcIdHelper().isStrip(id);  // like measuresPhi()
                int channel = m_idHelperSvc->tgcIdHelper().channel(id);        // between 1 and 135!
                if (channel_type) { channel += m_idHelperSvc->tgcIdHelper().channelMax(); }
                channels[channel] += 1.;
                channels[channel + 1] += 0.55;
                channels[channel - 1] += 0.55;
            }
            for (const Muon::TgcPrepData* prd : *tgc_coll) {
                const Identifier id = prd->identify();
                const bool channel_type = m_idHelperSvc->tgcIdHelper().isStrip(id);  // like measuresPhi()
                int channel = m_idHelperSvc->tgcIdHelper().channel(id);              // between 1 and 135!
                if (channel_type) { channel += m_idHelperSvc->tgcIdHelper().channelMax(); }

                if (channels[channel] > 1) {
                    const int gasgap = m_idHelperSvc->tgcIdHelper().gasGap(id);
                    int layer_number = (gasgap - 1);  // layer_number ranges from 0..5
                    if (channel_type) { layer_number = layer_number + 3; }

                    ++number_of_hits_per_layer[layer_number];

                    const int layer = layer_number;  // layer ranges from 0..5

                    layers.insert(layer);
                    ATH_MSG_VERBOSE("gasgap: " << gasgap << " layer: " << layer_number);
                }
            }
        }

        std::map<const Identifier, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>
            gasgapphimap;  // map between gasgapidentifier and phi hits
        std::map<const Identifier, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>::iterator gg_it;
        std::pair<std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>::iterator, bool> gg_insert;

        for (const Muon::TgcPrepData* prd : *tgc_coll) {
            const Amg::Vector3D& globalpos = prd->globalPosition();
            Identifier id = prd->identify();
            bool channel_type = m_idHelperSvc->tgcIdHelper().isStrip(id);  // like measuresPhi()

            constexpr double big_number = 250000;

            if (std::isnan(globalpos[Amg::x]) || std::abs(globalpos[Amg::y]) > big_number || std::abs(globalpos[Amg::y]) > big_number ||
                std::abs(globalpos[Amg::z]) > big_number)  // to avoid crashing with TGC hits
            {
                ATH_MSG_WARNING("TGC hit not physical: hitx: " << globalpos);
            } else {
                if (channel_type)  // phi hit
                {
                    const Identifier gasGapId = m_idHelperSvc->gasGapId(id);
                    gg_it = gasgapphimap.find(gasGapId);
                    if (gg_it == gasgapphimap.end()) {  // gasgapid not yet in map
                        gasgapphimap[gasGapId] = {prd};
                    } else {  // gasgapid already in set
                        gg_insert = (*gg_it).second.insert(prd);
                        if (!gg_insert.second) { ATH_MSG_DEBUG("WARNING::TGC hit already in set? "); }
                    }
                }
                double weight = 1.;
                double prob = 1.;
                if (m_hit_reweights) {
                    if (layers.size() <= 1) {
                        weight = 0.;
                        prob = 0.;
                    } else {
                        const int gasgap = m_idHelperSvc->tgcIdHelper().gasGap(id);
                        int layer_number = (gasgap - 1);  // layer_number ranges from 0..1/2
                        if (channel_type) layer_number = layer_number + 3;
                        double number_of_hits = (double)number_of_hits_per_layer[layer_number];
                        if (number_of_hits > 0) {
                            weight = 1. / (0.25 * std::sqrt(number_of_hits) + 0.75 * number_of_hits);
                            if (layers.size() == 2) weight = weight / 2.;
                        } else {
                            weight = 0.;
                            prob = 0.;
                        }
                    }
                }
                MuonHoughHit* hit = new MuonHoughHit(globalpos, channel_type, MuonHough::TGC, prob, weight,
                                                     prd);  // getPrd
                hitcontainer.addHit(hit);
                ATH_MSG_DEBUG(m_printer->print(*prd) << " NEW weight " << weight);
                if (m_use_histos) {
                  Hists& h = getHists();
                  h.m_weighthistogram->Fill(weight);
                  h.m_weighthistogramtgc->Fill(weight);
                }
            }
        }

        int size_end = hitcontainer.size();
        updateTgcMdtStationMap((*tgc_coll->begin())->identify(), size_begin, size_end, tgcmdtstationmap);

        // extract preprawdata from gasgapmap // might not be fastest way (filling
        // immidiately saves this second loop)

        for (const Muon::TgcPrepData* prd : *tgc_coll) {
            const Identifier id = prd->identify();
            if (!static_cast<bool>(m_idHelperSvc->tgcIdHelper().isStrip(id))) {  // eta hit
                const Identifier gasGapId = m_idHelperSvc->gasGapId(id);
                gg_it = gasgapphimap.find(gasGapId);
                if (gg_it != gasgapphimap.end()) { phietahitassociation.insert(std::make_pair(prd, (*gg_it).second)); }
            }
        }
    }

    void MuonHoughPatternFinderTool::addMdtCollection(const MdtPrepDataCollection* mdt_coll, MuonHoughHitContainer& hitcontainer,
                                                      std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap,
                                                      std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap) const {
        const int size = mdt_coll->size();
        if (!size) return;

        auto new_mdt_hit = [](const Muon::MdtPrepData* mdt_hit, double prob, double weight) {
            return new MuonHoughHit(mdt_hit->globalPosition(), false /*measures_phi*/, MuonHough::MDT, prob, weight, mdt_hit);  // getPrd
        };
        if (m_showerskip) {
            const Muon::MdtPrepData* mdt = (*mdt_coll->begin());
            const MuonGM::MdtReadoutElement* detEl = mdt->detectorElement();
            unsigned int channels = 2 * detEl->getNLayers() * detEl->getNtubesperlayer();  // Factor 2 for number of multilayers, should
                                                                                           // be changed when only 1 detector element per
                                                                                           // chamber (the chambers with only 1
                                                                                           // multilayer have a twice less severe cut
                                                                                           // (for convenience))
            double occupancy = (double)size / (double)channels;

            ATH_MSG_DEBUG(" size: " << size << " channels: " << channels << " occupancy: " << occupancy);

            // if more than m_showerskipperc (default 30%) of all hits in the chamber is
            // hit then all weights to 0 only done for large chambers (more than 50
            // hits)
            if (occupancy > m_showerskipperc && size > 50) {
                ATH_MSG_DEBUG("Chamber skipped! Too high occupancy (>" << m_showerskipperc << "%): " << occupancy
                                                                       << " association to pattern still possible");

                for (const MdtPrepData* mdt_hit : *mdt_coll) {
                    if (m_mdt_tdc_cut && mdt_hit->status() != Muon::MdtStatusDriftTime) continue;

                    if ((m_mdt_adc_cut && (mdt_hit->adc() > m_mdt_adc_min)) || !m_mdt_adc_cut) {
                        ATH_MSG_DEBUG(m_printer->print(*mdt_hit));
                        hitcontainer.addHit(new_mdt_hit(mdt_hit, 0., 0.));
                    }
                }
                return;
            }
        }

        std::map<int, int> number_of_hits_per_layer;
        std::map<int, int> number_of_hots_per_layer;  // number of trigger confirmed or hits on segment
                                                      // within layer (key)

        std::vector<double> radius;
        std::vector<double> errradius;
        std::vector<double> weights;
        std::vector<double> prob;  // artificial probability that hit belongs to true muon
        std::vector<int> multilayer;
        std::vector<int> tubelayer;
        std::vector<int> tubes;
        std::vector<int> onsegment;  // non-zero if on segment, int indicates how many hits in same
                                     // layer are on segment (used in weighting)
        std::vector<double> psi;
        std::vector<double> weight_trigger;
        std::vector<int> tr_confirmation;  // 1 if confirmation from trigger hits else 0
        std::vector<Identifier> ids;
        std::vector<int> layers;
        std::vector<const Muon::MdtPrepData*> prd;

        radius.reserve(size);
        errradius.reserve(size);
        weights.reserve(size);
        prob.reserve(size);
        multilayer.reserve(size);
        tubelayer.reserve(size);
        tubes.reserve(size);
        onsegment.reserve(size);
        psi.reserve(size);
        weight_trigger.reserve(size);
        tr_confirmation.reserve(size);
        ids.reserve(size);
        layers.reserve(size);
        prd.reserve(size);

        std::vector<double> tubecount(m_idHelperSvc->mdtIdHelper().tubeMax() + 2);

        for (const Muon::MdtPrepData* mdt : *mdt_coll)  // first
        {
            if (m_mdt_tdc_cut && mdt->status() != Muon::MdtStatusDriftTime) continue;

            if ((m_mdt_adc_cut && (mdt->adc() > m_mdt_adc_min)) || !m_mdt_adc_cut) {
                Identifier id = mdt->identify();

                prd.push_back(mdt);

                radius.push_back(mdt->localPosition()[0]);
                errradius.push_back(Amg::error(mdt->localCovariance(), 0));
                weights.push_back(1.);
                prob.push_back(1.);
                const int tube = m_idHelperSvc->mdtIdHelper().tube(id);
                tubes.push_back(tube);
                onsegment.push_back(0);
                psi.push_back(0.);
                weight_trigger.push_back(0.);
                tr_confirmation.push_back(0);
                ids.push_back(id);

                const int multi_layer = m_idHelperSvc->mdtIdHelper().multilayer(id);
                const int tube_layer = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
                multilayer.push_back(multi_layer);
                tubelayer.push_back(tube_layer);

                int layer_number =
                    (multi_layer - 1) * m_idHelperSvc->mdtIdHelper().tubeLayerMax() + (tube_layer - 1);  // layer_number ranges from 0..5/7
                layers.push_back(layer_number);

                tubecount[tube] += 1.;
                tubecount[tube - 1] += 0.5;
                tubecount[tube + 1] += 0.5;

                ATH_MSG_VERBOSE(" layer_number: " << layer_number << " multi_layer: " << multi_layer << " tube_layer: " << tube_layer);

            }  // adc cut
        }

        const unsigned int prdsize = prd.size();

        if (!prdsize) return;

        if (!m_hit_reweights) {
            for (const Muon::MdtPrepData* mdt_hit : prd) {
                ATH_MSG_DEBUG(m_printer->print(*mdt_hit));
                hitcontainer.addHit(new_mdt_hit(mdt_hit, 1., 1.));
            }
            return;
        }

        double tubem = *(std::max_element(tubecount.begin(), tubecount.end()));

        // allweights 0
        if (tubem < 2.01) {
            ATH_MSG_VERBOSE(" TOO SMALL tubem : " << tubem);
            for (const Muon::MdtPrepData* mdt_hit : prd) {
                ATH_MSG_DEBUG(m_printer->print(*mdt_hit) << " weight " << 0 << " adc: " << mdt_hit->adc());
                hitcontainer.addHit(new_mdt_hit(mdt_hit, 0., 0.));
                if (m_use_histos) {
                  Hists& h = getHists();
                  h.m_weighthistogram->Fill(0);
                  h.m_weighthistogrammdt->Fill(0);
                }

            }  // collection
            return;
        }

        // fast segment search:

        for (unsigned int i = 0; i < prdsize; i++) {
            if (tubecount[tubes[i]] > 1) ++number_of_hits_per_layer[layers[i]];

            // KILL 1 hit cases
            if (tubecount[tubes[i]] <= 1) prob[i] = 0;
        }  // end hit loop i

        int ml1{0}, ml2{0};
        for (const auto& map_it : number_of_hits_per_layer) {
            /// Avoid unneccary branching here
            const bool count_1 = map_it.first >= m_idHelperSvc->mdtIdHelper().tubeLayerMax();
            ml1 += count_1;
            ml2 += !count_1;
        }

        // allweights = 0
        if (ml1 + ml2 < 2.01) {
            ATH_MSG_VERBOSE(" TOO SMALL ml1 + ml2 : " << ml1 << " ml2 " << ml2);
            for (const Muon::MdtPrepData* mdt_hit : prd) {
                ATH_MSG_DEBUG(m_printer->print(*mdt_hit) << " weight " << 0);
                hitcontainer.addHit(new_mdt_hit(mdt_hit, 0., 0.));
                if (m_use_histos) {
                  Hists& h = getHists();
                  h.m_weighthistogram->Fill(0);
                  h.m_weighthistogrammdt->Fill(0);
                }
            }  // collection
            return;
        }

        bool barrel = m_idHelperSvc->mdtIdHelper().isBarrel(ids[0]);

        DCVec dcs;
        dcs.reserve(prdsize);

        for (unsigned int i = 0; i < prdsize; ++i) {
            if (prob[i] < 0.01) continue;
            // create identifier
            TrkDriftCircleMath::MdtId mdtid(barrel, multilayer[i] - 1, tubelayer[i] - 1, tubes[i] - 1);
            // create new DriftCircircleMath::DriftCircle::DriftState
            const Amg::Vector3D& globalpos = prd[i]->globalPosition();
            TrkDriftCircleMath::DriftCircle dc(TrkDriftCircleMath::LocVec2D(globalpos.perp(), globalpos.z()), radius[i], errradius[i],
                                               TrkDriftCircleMath::DriftCircle::InTime, mdtid, i);
            dcs.emplace_back(std::move(dc));
        }

        bool seg_found = true;
        while (seg_found) {
            std::vector<int> sel(dcs.size());
            double angleDif = 0.;

            fastSegmentFinder(dcs, ml1, ml2, angleDif, sel);

            if (ml1 + ml2 >= 2.1) {
                int removed_hits = 0;  // keeps track of number of removed hits
                for (unsigned int i = 0; i < sel.size(); ++i) {
                    if (sel[i] != 0) {
                        unsigned int j = (unsigned int)dcs[i - removed_hits].index();  // index of position in prd vec
                        onsegment[j] = 1;
                        psi[j] = angleDif;

                        ++number_of_hots_per_layer[layers[j]];

                        // remove hit from dcs container for next iteration!!
                        dcs.erase(dcs.begin() + i - removed_hits);
                        ++removed_hits;
                    }
                }
            } else {
                seg_found = false;
            }
        }

        // trigger confirmation checks:

        int stationcode = stationCode(ids[0]);

        // rpc:

        std::map<int, std::vector<std::pair<int, int>>>::const_iterator stationmap_it = rpcmdtstationmap.find(stationcode);

        if (stationmap_it != rpcmdtstationmap.end()) {
            const std::vector<std::pair<int, int>>& stationhits = (*stationmap_it).second;

            // stationloop
            for (unsigned int i = 0; i < stationhits.size(); i++) {
                // rpc hit loop
                for (int j = stationhits[i].first; j < stationhits[i].second; j++) {
                    const MuonHoughHit* rpchit = hitcontainer.getHit(j);
                    if (rpchit->getWeight() < 0.01) continue;

                    const double rpcx = hitcontainer.getHitx(j);
                    const double rpcy = hitcontainer.getHity(j);
                    const double rpcz = hitcontainer.getHitz(j);
                    const double rpc_radius = std::hypot(rpcx, rpcy);
                    const double rpc_rz_ratio = rpc_radius / rpcz;
                    const double rpc_inv_rz_ratio = 1. / rpc_rz_ratio;

                    for (unsigned int k = 0; k < prdsize; k++) {
                        // Mdt hit loop
                        double dis = 0.;
                        const Amg::Vector3D& globalpos = prd[k]->globalPosition();
                        if (barrel) {
                            dis = globalpos[Amg::AxisDefs::z] - globalpos.perp() * rpc_inv_rz_ratio;
                        } else {  // can that happen?
                            dis = globalpos.perp() - rpc_rz_ratio * globalpos[Amg::AxisDefs::z];
                        }

                        if (weight_trigger[k] < 0.1) { weight_trigger[k] = 1.; }

                        if (std::abs(dis) < 250.) {
                            double wnew = 1.5 + (250. - std::abs(dis)) / 251.;
                            if (wnew > weight_trigger[k]) { weight_trigger[k] = wnew; }
                        }
                    }
                }
            }
        }

        // tgc:

        stationmap_it = tgcmdtstationmap.find(stationcode);

        if (stationmap_it != tgcmdtstationmap.end()) {
            const std::vector<std::pair<int, int>>& stationhits = (*stationmap_it).second;

            // stationloop
            for (unsigned int i = 0; i < stationhits.size(); i++) {
                // tgc hit loop
                for (int j = stationhits[i].first; j < stationhits[i].second; j++) {
                    const MuonHoughHit* tgchit = hitcontainer.getHit(j);
                    if (tgchit) {
                        if (tgchit->getWeight() < 0.01) continue;

                        const double tgcx = tgchit->getHitx();
                        const double tgcy = tgchit->getHity();
                        const double tgcz = tgchit->getHitz();
                        const double tgc_rz_ratio = std::hypot(tgcx, tgcy) / tgcz;

                        for (unsigned int k = 0; k < prdsize; k++) {
                            // Mdt hit loop
                            if (weight_trigger[k] < 0.1) weight_trigger[k] = 3.;
                            const Amg::Vector3D& globalpos = prd[k]->globalPosition();
                            double dis = globalpos.perp() - tgc_rz_ratio * globalpos[Amg::AxisDefs::z];  // only endcap extrapolation
                            if (std::abs(dis) < 250.) {
                                double wnew = 3.5 + (250. - std::abs(dis)) / 251.;
                                if (wnew > weight_trigger[k]) weight_trigger[k] = wnew;
                            }
                        }
                    }
                }
            }
        }

        // define trigger confirmation:

        for (unsigned int i = 0; i < prdsize; i++) {
            // for MDTs require trigger chamber confirmation
            //                  or segment with selected hits

            if (weight_trigger[i] > 1.5 && weight_trigger[i] < 2.55) tr_confirmation[i] = 1;
            if (weight_trigger[i] > 3.5 && weight_trigger[i] < 4.55) tr_confirmation[i] = 1;

            // add confirmed hits to hots layer count:
            if (tr_confirmation[i] == 1 && onsegment[i] == 0) {  // else already added
                ++number_of_hots_per_layer[layers[i]];
            }
        }

        // calculate final weights:

        for (unsigned int i = 0; i < prdsize; i++) {
            if (prob[i] < 0.01) {
                weights[i] = 0;
                continue;
            }  // throw away hits that are not significant

            // correct for several number of hits in layer:
            std::map<int, int>::const_iterator map_it = number_of_hits_per_layer.find(layers[i]);
            if (map_it != number_of_hits_per_layer.end()) {
                int layerhits = (*map_it).second;
                double layer_weight = 1. / (0.25 * layerhits + 0.75 * std::sqrt(layerhits));

                if (0 == tr_confirmation[i] && onsegment[i] == 0) {
                    // downweighting for non-confirmed hits:
                    prob[i] = prob[i] - 0.2;
                    if (prob[i] < 0) prob[i] = 0.;

                    // correct for several number of hits in layer:
                    weights[i] = prob[i] * layer_weight;
                }

                else {
                    // Correct probabilities for hits on segment or confirmed by RPC/TGC
                    double rej = 1. / (1. - layer_weight + 0.10);
                    double rej0 = 1.;  // irrevelant value

                    if (onsegment[i] != 0 && tr_confirmation[i] != 0) {
                        rej0 = 30;
                    } else if (onsegment[i] != 0) {
                        rej0 = 1.75 / (psi[i] + 0.05);
                    }  // 1.75 = 5*0.35
                    else if (tr_confirmation[i] != 0) {
                        rej0 = 8;
                    }

                    double rej_total = rej * rej0;
                    prob[i] = rej_total / (1. + rej_total);

                    // correct for several number of confirmed hits in layer:
                    map_it = number_of_hots_per_layer.find(layers[i]);
                    if (map_it != number_of_hots_per_layer.end()) {
                        int layerhits_conf = (*map_it).second;
                        weights[i] = prob[i] / (0.25 * layerhits_conf + 0.75 * std::sqrt(layerhits_conf));
                    } else {
                        ATH_MSG_INFO("Entry not in map! This should not happen");
                        weights[i] = prob[i];
                    }
                }
            } else {
                ATH_MSG_INFO("Entry not in map! This should not happen");
                weights[i] = prob[i];
            }
        }

        // and finally add hits to container:

        for (unsigned int i = 0; i < prdsize; i++) {
            ATH_MSG_DEBUG(m_printer->print(*prd[i]) << " trigger weight " << weight_trigger[i] << " on segment " << onsegment[i] << " psi "
                                                    << psi[i] << " prob " << prob[i] << " weight " << weights[i]);
            hitcontainer.addHit(new_mdt_hit(prd[i], prob[i], weights[i]));
            if (m_use_histos) {
                Hists& h = getHists();
                h.m_weighthistogram->Fill(weights[i]);
                h.m_weighthistogrammdt->Fill(weights[i]);
            }

        }  // collection
    }

    void MuonHoughPatternFinderTool::addCscCollection(
        const Muon::CscPrepDataCollection* csc_coll, MuonHoughHitContainer& hitcontainer,
        std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const {
        /// For the moment deactivate the CSCs to reconcile the CI
        if (true || csc_coll->size() == 0) return;
        std::map<int, int> number_of_hits_per_layer;
        if (m_hit_reweights)  // reweight  hits, according to Niels' and Peters new
                              // algorithm
        {
            for (const Muon::CscPrepData* prd : *csc_coll) {
                Identifier id = prd->identify();
                const bool channel_type = m_idHelperSvc->cscIdHelper().measuresPhi(prd->identify());

                const int chamber_layer = m_idHelperSvc->cscIdHelper().chamberLayer(id);
                const int chamber_layer_max = m_idHelperSvc->cscIdHelper().chamberLayerMax(id);
                const int wire_layer = m_idHelperSvc->cscIdHelper().wireLayer(id);
                int layer_number = (chamber_layer - 1) + chamber_layer_max * (wire_layer - 1);  // layer_number ranges from 0..7
                if (channel_type) layer_number = layer_number + 8;

                ++number_of_hits_per_layer[layer_number];

                ATH_MSG_DEBUG("chamber_layer: " << chamber_layer << " chamber_layer_max: " << chamber_layer_max
                                                << " wire_layer: " << wire_layer);
            }
        }

        std::map<const Identifier, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>
            gasgapphimap;  // map between gasgapidentifier and phi hits
        std::map<const Identifier, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>::iterator gg_it;
        std::pair<std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>::iterator, bool> gg_insert;

        for (const Muon::CscPrepData* prd : *csc_coll) {
            const Amg::Vector3D& globalpos = prd->globalPosition();
            Identifier id = prd->identify();
            bool channel_type = m_idHelperSvc->cscIdHelper().measuresPhi(id);

            if (channel_type)  // phi hit
            {
                const Identifier gasGapId = m_idHelperSvc->gasGapId(id);
                gg_it = gasgapphimap.find(gasGapId);
                if (gg_it == gasgapphimap.end()) {  // gasgapid not yet in map
                    gasgapphimap[gasGapId] = {prd};
                } else {  // gasgapid already in set
                    gg_insert = (*gg_it).second.insert(prd);
                    if (!gg_insert.second) { ATH_MSG_DEBUG("WARNING::CSC hit already in set? "); }
                }
            }
            double weight = 1.;
            if (m_hit_reweights) {
                const int chamber_layer = m_idHelperSvc->cscIdHelper().chamberLayer(id);
                const int chamber_layer_max = m_idHelperSvc->cscIdHelper().chamberLayerMax(id);
                const int wire_layer = m_idHelperSvc->cscIdHelper().wireLayer(id);
                int layer_number = (chamber_layer - 1) + chamber_layer_max * (wire_layer - 1);  // layer_number ranges from 0..7
                if (channel_type) layer_number = layer_number + 8;
                double weight_factor_csc = 1.;
                double number_of_hits = (double)number_of_hits_per_layer[layer_number];
                weight = weight_factor_csc / std::sqrt(number_of_hits);

                ATH_MSG_DEBUG("CSC weight: " << weight);
            }

            MuonHoughHit* hit = new MuonHoughHit(globalpos, channel_type, MuonHough::CSC, 1., weight,
                                                 prd);  // getPrd
            hitcontainer.addHit(hit);
            ATH_MSG_DEBUG(m_printer->print(*prd) << " weight " << weight);
            if (m_use_histos) {
                Hists& h = getHists();
                h.m_weighthistogram->Fill(weight);
                h.m_weighthistogramcsc->Fill(weight);
            }
        }
        // extract preprawdata from gasgapmap // might not be fastest way (filling
        // immidiately saves this second loop)

        for (const Muon::CscPrepData* prd : *csc_coll) {
            const Identifier id = prd->identify();
            if (!m_idHelperSvc->cscIdHelper().measuresPhi(id)) {  // eta hit

                const Identifier gasGapId = m_idHelperSvc->gasGapId(id);
                gg_it = gasgapphimap.find(gasGapId);
                if (gg_it != gasgapphimap.end()) { phietahitassociation.insert(std::make_pair(prd, (*gg_it).second)); }
            }
        }
    }

    void MuonHoughPatternFinderTool::updateRpcMdtStationMap(const Identifier rpcid, const int hit_begin, const int hit_end,
                                                            std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap) const {
        //  input is a RPC identifier, begin container and end container
        //  rpcmdtstationmap is updated
        //
        // called once per rpc collection/station

        std::string st = m_idHelperSvc->rpcIdHelper().stationNameString(m_idHelperSvc->rpcIdHelper().stationName(rpcid));
        ATH_MSG_VERBOSE("updateRpcMdtStationMap" << st);
        if (st[0] != 'B') return;  // no rpc for endcap chambers

        std::map<int, std::vector<std::pair<int, int>>>::iterator it;
        int stationcode = stationCode(rpcid);

        // store station code

        addToStationMap(rpcmdtstationmap, it, stationcode, hit_begin, hit_end);

        int idphi = m_idHelperSvc->rpcIdHelper().stationPhi(rpcid);
        int ideta = m_idHelperSvc->rpcIdHelper().stationEta(rpcid);

        int idphi1 = idphi - 1;
        if (idphi1 == 0) idphi1 = 8;
        int idphi2 = idphi + 1;
        if (idphi2 > 8) idphi2 = 1;

        std::string station;
        if (st == "BOL") {
            station = "BOS";
        } else if (st == "BOS") {
            station = "BOL";
        } else if (st == "BMS") {
            station = "BML";
        } else if (st == "BML") {
            station = "BMS";
        } else
            return;

        // store Neighbouring station codes
        int stationNameMDT = m_idHelperSvc->mdtIdHelper().stationNameIndex(station);

        stationcode = stationCode(stationNameMDT, idphi1, ideta);
        addToStationMap(rpcmdtstationmap, it, stationcode, hit_begin, hit_end);

        stationcode = stationCode(stationNameMDT, idphi2, ideta);
        addToStationMap(rpcmdtstationmap, it, stationcode, hit_begin, hit_end);

        //  Also look into Inner station

        if (st == "BMS") {
            station = "BIS";
        } else if (st == "BML") {
            station = "BIL";
        } else
            return;

        stationNameMDT = m_idHelperSvc->mdtIdHelper().stationNameIndex(station);
        stationcode = stationCode(stationNameMDT, idphi, ideta);
        addToStationMap(rpcmdtstationmap, it, stationcode, hit_begin, hit_end);
    }

    void MuonHoughPatternFinderTool::updateTgcMdtStationMap(const Identifier tgcid, int hit_begin, int hit_end,
                                                            std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap) const {
        //  input is a TGC identifier, begin container and end container
        //  tgcmdtstationmap is updated
        //
        // called once per tgc collection/station
        std::string st = m_idHelperSvc->tgcIdHelper().stationNameString(m_idHelperSvc->tgcIdHelper().stationName(tgcid));
        if (st[0] != 'T') return;

        std::vector<int> T31(5);
        T31[0] = 2;
        T31[1] = 3;
        T31[2] = 3;
        T31[3] = 4;
        T31[4] = 4;
        std::vector<int> T32(5);
        T32[0] = 3;
        T32[1] = 4;
        T32[2] = 4;
        T32[3] = 5;
        T32[4] = 5;
        std::vector<int> T11(5);
        T11[0] = 2;
        T11[1] = 3;
        T11[2] = 4;
        T11[3] = 4;
        T11[4] = 4;
        std::vector<int> T12(5);
        T12[0] = 3;
        T12[1] = 4;
        T12[2] = 5;
        T12[3] = 5;
        T12[4] = 5;

        std::map<int, std::vector<std::pair<int, int>>>::iterator it;

        // Determine station phi in MDT

        int modphiTGC = 48;
        if (st[2] == 'F') modphiTGC = 24;
        if (st[1] == '4') modphiTGC = 24;

        int idphi = m_idHelperSvc->tgcIdHelper().stationPhi(tgcid);
        int ideta = m_idHelperSvc->tgcIdHelper().stationEta(tgcid);
        int index = abs(ideta) - 1;
        int idphi1MDT = 1 + int(8. * (idphi + 1) / modphiTGC);
        int idphi2MDT = 1 + int(8. * (idphi - 1) / modphiTGC);
        if (idphi1MDT > 8) idphi1MDT = 1;
        if (idphi2MDT > 8) idphi2MDT = 1;

        int sign = 1;
        if (ideta < 0) sign = -1;

        // Determine two station etas  in MDT

        int ideta1MDT = 0;
        int ideta2MDT = 0;
        if (st[2] == 'F') {
            ideta1MDT = sign * 1;
            ideta2MDT = sign * 2;
        }
        if (st[2] == 'E') {
            if (st[1] == '4') {
                // T4
                ideta1MDT = sign * 4;
                ideta2MDT = sign * 5;
            } else if (st[1] == '3') {
                // T3
                ideta1MDT = sign * T31[index];
                ideta2MDT = sign * T32[index];
            } else {
                // T1 or T2
                ideta1MDT = sign * T11[index];
                ideta2MDT = sign * T12[index];
            }
        }
        std::string station1 = "EML";
        std::string station2 = "EMS";
        if (st[1] == '4') {
            station1 = "EIL";
            station2 = "EIS";
        }
        int stationNameMDT1 = m_idHelperSvc->mdtIdHelper().stationNameIndex(station1);
        int stationNameMDT2 = m_idHelperSvc->mdtIdHelper().stationNameIndex(station2);

        // store station Inner and Middle codes

        int stationcode = stationCode(stationNameMDT1, idphi1MDT, ideta1MDT);
        addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
        stationcode = stationCode(stationNameMDT2, idphi1MDT, ideta1MDT);
        addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
        if (ideta1MDT != ideta2MDT) {
            stationcode = stationCode(stationNameMDT1, idphi1MDT, ideta2MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
            stationcode = stationCode(stationNameMDT2, idphi1MDT, ideta2MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
        }
        if (idphi1MDT != idphi2MDT) {
            stationcode = stationCode(stationNameMDT1, idphi2MDT, ideta1MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
            stationcode = stationCode(stationNameMDT2, idphi2MDT, ideta1MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
            if (ideta1MDT != ideta2MDT) {
                stationcode = stationCode(stationNameMDT1, idphi2MDT, ideta2MDT);
                addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
                stationcode = stationCode(stationNameMDT2, idphi2MDT, ideta2MDT);
                addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
            }
        }
        // Store corresponding Outer stations

        if (station1 == "EMS") { station1 = "EOS"; }
        if (station2 == "EML") {
            station2 = "EOL";
        } else
            return;

        stationNameMDT1 = m_idHelperSvc->mdtIdHelper().stationNameIndex(station1);
        stationNameMDT2 = m_idHelperSvc->mdtIdHelper().stationNameIndex(station2);

        stationcode = stationCode(stationNameMDT1, idphi1MDT, ideta1MDT);
        addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
        stationcode = stationCode(stationNameMDT2, idphi1MDT, ideta1MDT);
        addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
        if (ideta1MDT != ideta2MDT) {
            stationcode = stationCode(stationNameMDT1, idphi1MDT, ideta2MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
            stationcode = stationCode(stationNameMDT2, idphi1MDT, ideta2MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
        }
        if (idphi1MDT != idphi2MDT) {
            stationcode = stationCode(stationNameMDT1, idphi2MDT, ideta1MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
            stationcode = stationCode(stationNameMDT2, idphi2MDT, ideta1MDT);
            addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);

            if (ideta1MDT != ideta2MDT) {
                stationcode = stationCode(stationNameMDT1, idphi2MDT, ideta2MDT);
                addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
                stationcode = stationCode(stationNameMDT2, idphi2MDT, ideta2MDT);
                addToStationMap(tgcmdtstationmap, it, stationcode, hit_begin, hit_end);
            }
        }
    }

    int MuonHoughPatternFinderTool::stationCode(Identifier id) const {
        return 10000000 * m_idHelperSvc->mdtIdHelper().stationName(id) + 100000 * m_idHelperSvc->mdtIdHelper().stationPhi(id) +
               1000 * (m_idHelperSvc->mdtIdHelper().stationEta(id) + 10);
    }

    int MuonHoughPatternFinderTool::stationCode(int stationname, int phi, int eta) {
        return 10000000 * stationname + 100000 * phi + 1000 * (eta + 10);
    }

    void MuonHoughPatternFinderTool::addToStationMap(std::map<int, std::vector<std::pair<int, int>>>& stationmap,
                                                     std::map<int, std::vector<std::pair<int, int>>>::iterator& it, int& stationcode,
                                                     const int& hit_begin, const int& hit_end) {
        it = stationmap.find(stationcode);
        if (it == stationmap.end()) {
            std::vector<std::pair<int, int>> dummyvec;
            dummyvec.emplace_back(hit_begin, hit_end);
            stationmap[stationcode] = dummyvec;
        } else {
            (*it).second.emplace_back(hit_begin, hit_end);
        }
    }

    void MuonHoughPatternFinderTool::fastSegmentFinder(TrkDriftCircleMath::DCVec& dcs, int& nl1, int& nl2, double& angleDif,
                                                       std::vector<int>& sel) const {
        //
        // Input:  vector of driftcircles per chamber
        // Output: nl1 = segment hits in multilayer 1 and nl2 = segment hits in
        // multilayer 2
        //       : sel(1:dcs.size)  = 0 NOT selected  = 1 on segment
        //
        // Method: constructs the tangent lines to all driftcircle combinations and
        // counts hits in a road of 1.5 mm
        //         segment = combination with most hits
        //         uses TrkDriftCircleMath software
        //

        // Layers with more than 10 hits are skipped as seed, if all layers have more
        // than 10 hits, no fits are tried

        nl1 = 0;
        nl2 = 0;
        angleDif = -1.;
        if (dcs.empty()) return;

        DCCit it_end = dcs.end();
        DCCit it1 = dcs.begin();
        std::map<int, DCVec> layerHits;  // map between layer and driftcircles
        std::map<int, int> dcsId;        // map between 'idnumber' and position

        std::map<int, DCVec>::iterator map_it;
        int nhits = 0;
        for (; it1 != it_end; ++it1, nhits++) {
            sel[nhits] = 0;
            int isort = MdtIdHelper::maxNTubesPerLayer * (4 * (it1->id().ml()) + it1->id().lay()) + it1->id().tube();
            dcsId[isort] = nhits;
            int ilay = 4 * (it1->id().ml()) + it1->id().lay();
            ATH_MSG_VERBOSE(" ilay " << ilay << " isort " << isort);

            map_it = layerHits.find(ilay);
            if (map_it != layerHits.end()) {
                (*map_it).second.push_back(*it1);
            } else {
                DCVec dcl;
                dcl.reserve(dcs.size());
                dcl.push_back(*it1);
                layerHits[ilay] = dcl;
            }
        }

        unsigned int nHits = 0;  // is maximalized
        unsigned int nHitsLine = 0;
        unsigned int nPassedTubes = 0;
        double roadWidth = 1.5;
        TrkDriftCircleMath::DCOnTrackVec hitsOnLineSel;
        TrkDriftCircleMath::TangentToCircles tanCreator;
        TrkDriftCircleMath::MatchDCWithLine matchWithLine;
        bool stop = false;
        for (int i = 0; i < 8; i++) {
            if (layerHits.count(i) != 1) continue;
            DCVec& dci = layerHits[i];
            if (dci.size() > 10) continue;
            DCCit iti = dci.begin();
            DCCit iti_end = dci.end();
            for (; iti != iti_end; ++iti) {
                // One seed selected
                float tubeRadius = 14.6;
                if ((*iti).rot()) {  // if no access to rot, can't do anything here
                    tubeRadius = (*iti).rot()->detectorElement()->innerTubeRadius();
                }
                for (int j = 7; j > i; j--) {
                    if (layerHits.count(j) != 1) continue;
                    DCVec& dcj = layerHits[j];
                    if (dcj.size() > 10) continue;
                    DCCit itj = dcj.begin();
                    DCCit itj_end = dcj.end();
                    for (; itj != itj_end; ++itj) {
                        // Second seed selected
                        double hitx = (*itj).x();
                        double hity = (*itj).y();
                        double norm = std::sqrt(hitx * hitx + hity * hity);
                        double cphi = hitx / norm;
                        double sphi = hity / norm;
                        TrkDriftCircleMath::TangentToCircles::LineVec lines = tanCreator.tangentLines(*iti, *itj);
                        for (TrkDriftCircleMath::TangentToCircles::LineVec::const_iterator lit = lines.begin(); lit != lines.end(); ++lit) {
                            double coshit = std::cos((*lit).phi());
                            double sinhit = std::sin((*lit).phi());
                            double cospsi = coshit * cphi + sinhit * sphi;
                            if (cospsi > 1.)
                                cospsi = 1.;
                            else if (cospsi < -1.)
                                cospsi = -1.;
                            double psi = std::acos(cospsi);
                            if (psi > 0.3) continue;
                            matchWithLine.set(*lit, roadWidth, TrkDriftCircleMath::MatchDCWithLine::Road, tubeRadius);
                            const TrkDriftCircleMath::DCOnTrackVec& hitsOnLine = matchWithLine.match(dcs);
                            unsigned int matchedHits = matchWithLine.hitsOnTrack();
                            ATH_MSG_VERBOSE(" Summary nHits " << matchedHits << " nl1 " << matchWithLine.hitsMl1() << " nl2 "
                                                              << matchWithLine.hitsMl2());
                            if (matchedHits > nHits || (matchedHits == nHits && psi < angleDif)) {
                                int dnl = std::abs(static_cast<int>(matchWithLine.hitsMl1()) - static_cast<int>(matchWithLine.hitsMl2()));
                                ATH_MSG_DEBUG(" matchWithLine.hitsOnTrack() >  nHits old " << nHits << " new: " << matchedHits);
                                ATH_MSG_DEBUG(" dnl " << dnl << " old dnl " << std::abs(nl1 - nl2));
                                ATH_MSG_DEBUG(" hit cos phi " << cphi << " line " << coshit << " sin phi " << sphi << " line " << sinhit
                                                              << " psi " << psi);

                                // update of variables:
                                nHits = matchedHits;
                                nl1 = matchWithLine.hitsMl1();
                                nl2 = matchWithLine.hitsMl2();
                                nHitsLine = hitsOnLine.size();
                                nPassedTubes = matchWithLine.passedTubes();
                                hitsOnLineSel = hitsOnLine;
                                angleDif = psi;
                            }

                            ATH_MSG_VERBOSE(" Select nHits " << nHits << " nl1 " << nl1 << " nl2 " << nl2);
                            if (nHits >= dcs.size()) stop = true;
                        }  // end lines
                        if (stop) break;
                    }  // end itj
                    if (stop) break;
                }  // end j
                if (stop) break;
            }  // end iti
            if (stop) break;
        }  // end i

        ATH_MSG_DEBUG(" Fast segment finder Max Layers hit " << dcs.size() << " nHitsLine - nHits " << nHitsLine - nl1 - nl2
                                                             << " passed Tubes -nHits " << nPassedTubes - nl1 - nl2 << " nl1 " << nl1
                                                             << " nl2 " << nl2 << " angleDif " << angleDif);

        TrkDriftCircleMath::DCOnTrackIt itt = hitsOnLineSel.begin();
        TrkDriftCircleMath::DCOnTrackIt itt_end = hitsOnLineSel.end();
        int i = 0;
        for (; itt != itt_end; ++itt, i++) {
            int isort = MdtIdHelper::maxNTubesPerLayer * (4 * (itt->id().ml()) + itt->id().lay()) + itt->id().tube();
            if (dcsId.count(isort) == 1) {
                int dcsIndex = dcsId[isort];
                sel[dcsIndex] = 1;

                ATH_MSG_DEBUG(" Selected Hit index " << dcsIndex << " MultiLayer " << itt->id().ml() << " layer " << itt->id().lay()
                                                     << " tube " << itt->id().tube());
            } else {
                ATH_MSG_WARNING(" ALARM fastSegmentFinder hit NOT found " << i << " isort " << isort);
            }
        }
    }


    MuonHoughPatternFinderTool::Hists&
    MuonHoughPatternFinderTool::getHists() const
    {
      // We earlier checked that no more than one thread is being used.
      Hists* h ATLAS_THREAD_SAFE = m_h.get();
      return *h;
    }


}  // namespace Muon
