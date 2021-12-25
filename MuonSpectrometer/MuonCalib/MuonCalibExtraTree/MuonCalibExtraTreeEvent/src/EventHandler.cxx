/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/EventHandler.h"

#include <iostream>
#include <sstream>
#include <utility>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    EventHandler::EventHandler() : m_event(nullptr), m_mooreIndices(4, 0), m_muonboyIndices(3, 100), m_debug(false) {}

    EventHandler::~EventHandler() { clear(); }

    const EventHandler::TrackVec& EventHandler::extendedTracks(int author) const {
        AuthorTrackVecMap::const_iterator itr = m_extendedTracks.find(author);
        if (itr != m_extendedTracks.end()) return itr->second;
        static TrackVec dummy_vec;
        return dummy_vec;
    }

    /** get extended tracks */
    const EventHandler::SegmentVec& EventHandler::extendedSegments(int author) const {
        AuthorSegmentVecMap::const_iterator itr = m_extendedSegments.find(author);
        if (itr != m_extendedSegments.end()) return itr->second;
        static SegmentVec dummy_vec;
        return dummy_vec;
    }

    /** get extended raw hits */
    const MuonCalibExtendedRawHits& EventHandler::extendedRawHits() const { return m_extendedRawHits; }

    /** get event */
    const MuonCalibEvent_E& EventHandler::event() const { return *m_event; }

    /** get author indices for Moore */
    const EventHandler::AuthorIndices& EventHandler::mooreIndices() const { return m_mooreIndices; }

    /** get author indices for Muonboy */
    const EventHandler::AuthorIndices& EventHandler::muonboyIndices() const { return m_muonboyIndices; }

    void EventHandler::setdbg(const bool dbg_on) { m_debug = dbg_on; }

    void EventHandler::clear() {
        m_extendedTracks.clear();
        m_extendedSegments.clear();
    }

    void EventHandler::setEvent(std::shared_ptr<const MuonCalibEvent_E> event) {
        // reset previous event and set new event
        clear();
        m_event = std::move(event);

        // create extended tracks
        createExtendedTracks();

        // create extended truth tracks
        createExtendedTruthTracks();

        // create extended tracks
        createExtendedSegments();

        // create extended raw hits
        createExtendedRawHits();

        AuthorTrackVecMap::iterator mit = m_extendedTracks.begin();
        AuthorTrackVecMap::iterator mit_end = m_extendedTracks.end();
        for (; mit != mit_end; ++mit) {
            if (mit->first < 500) linkTracks(mit->first);

            // associate segments with tracks
            if (mit->first < 99)
                associateSegmentsWithTracks(m_extendedSegments[m_mooreIndices.segment], mit->second);
            else
                associateSegmentsWithTracks(m_extendedSegments[m_muonboyIndices.segment], mit->second);
        }
    }

    std::string EventHandler::dumpRawHits() const {
        std::ostringstream sout;
        sout << m_extendedRawHits.dumpMdtHits();
        return sout.str();
    }

    std::string EventHandler::dumpTracks(const EventHandler::TrackVec& tracks) const {
        std::ostringstream sout;
        TrackVec::const_iterator trk = tracks.begin();
        TrackVec::const_iterator trk_end = tracks.end();
        TrackVec::const_iterator trk_last = trk_end - 1;
        for (; trk != trk_end; ++trk) {
            sout << (*trk)->dump();
            if (trk != trk_last) sout << std::endl;
        }
        return sout.str();
    }

    std::string EventHandler::dumpSegments(const EventHandler::SegmentVec& segments) const {
        std::ostringstream sout;
        SegmentVec::const_iterator seg = segments.begin();
        SegmentVec::const_iterator seg_end = segments.end();
        SegmentVec::const_iterator seg_last = seg_end - 1;
        for (; seg != seg_end; ++seg) {
            sout << (*seg)->dump();
            if (seg != seg_last) sout << std::endl;
        }
        return sout.str();
    }

    std::string EventHandler::dumpSegments() const {
        std::ostringstream sout;
        if (m_extendedSegments.count(m_muonboyIndices.segment) && !extendedSegments(m_muonboyIndices.segment).empty()) {
            sout << " Muonboy Segments " << extendedSegments(m_muonboyIndices.segment).size() << std::endl
                 << dumpSegments(extendedSegments(m_muonboyIndices.segment));
            if (!extendedSegments(m_mooreIndices.segment).empty()) sout << std::endl;
        }
        if (m_extendedSegments.count(m_mooreIndices.segment) && !extendedSegments(m_mooreIndices.segment).empty()) {
            sout << " Moore Segments " << extendedSegments(m_mooreIndices.segment).size() << std::endl
                 << dumpSegments(extendedSegments(m_mooreIndices.segment));
        }
        return sout.str();
    }

    std::string EventHandler::dumpEvent() const {
        std::ostringstream sout;
        sout << dumpRawHits() << std::endl << dumpTracks() << std::endl << dumpSegments();
        return sout.str();
    }

    std::string EventHandler::dumpTracks() const {
        std::ostringstream sout;
        for (const auto& it : m_extendedTracks) {
            // skip standalone tracks at IP
            if (it.first == 1 || it.first == 2) continue;

            sout << " Track Author " << it.first << " number of tracks " << it.second.size();
            // only dump Muon tracks
            if (it.first < 1000) sout << std::endl << dumpTracks(it.second);
            sout << std::endl;
        }
        return sout.str();
    }

    void EventHandler::createExtendedRawHits() {
        if (!m_event || !m_event->rawHitCollection()) return;

        AuthorTrackVecMap::iterator trk_itr = m_extendedTracks.find(m_mooreIndices.track);
        AuthorSegmentVecMap::iterator segIt = m_extendedSegments.find(m_mooreIndices.segment);

        SegmentVec seg_to_pipe;
        TrackVec trk_to_pipe;
        if (segIt != m_extendedSegments.end()) seg_to_pipe = segIt->second;
        if (trk_itr != m_extendedTracks.end()) trk_to_pipe = trk_itr->second;

        m_extendedRawHits = MuonCalibExtendedRawHits(*m_event->rawHitCollection(), seg_to_pipe, trk_to_pipe);
    }

    void EventHandler::createExtendedTracks() {
        // check that event is set
        if (!m_event) return;

        // hack to get the associations right...
        ExtendedTrackPtr previousMooreTrack;

        for (const MuonCalibEvent_E::MuonCalibTrkPtr& track_ptr : m_event->tracks()) {
            // get author Moore/Muonboy tracks author == 0 and corresponding TrackVec
            int author = track_ptr->author();
            TrackVec& trackVec = m_extendedTracks[author];

            // create new extended track and add it to TrackVec
            ExtendedTrackPtr extendedTrack = std::make_shared<MuonCalibExtendedTrack>(*track_ptr, 0, 0);

            // store previous moore track so we can add the back extrapolated ones if any
#ifdef TO_DO
            if (author == 0) {
                previousMooreTrack = extendedTrack;
            } else if ((author == 1 || author == 2) && previousMooreTrack) {
                std::cout << "Perform proper track association " << __FILE__ << " " << __LINE__ << std::endl;
                // previousMooreTrack->addAssociatedTrack(extendedTrack);
                // extendedTrack->addAssociatedTrack(previousMooreTrack);
            } else if (author == 10 && track_ptr->nrHits() == 0) {
                // Associate MuidSA/MooreExtrapolate to IP to author 0 if no hits on track
                TrackVec& trackVec0 = m_extendedTracks[0];
                double dotprodmax = -1.;
                if (trackVec0.size() > 0) {
                    int trackcount10 = trackVec.size();
                    TrackVec::iterator it0 = trackVec0.begin();
                    TrackVec::iterator it0_end = trackVec0.end();
                    int trackcount0 = 0;
                    const MuonCalibExtendedTrack* associatedMooreTrack = 0;
                    for (; it0 != it0_end; ++it0) {
                        trackcount0++;
                        if (trackcount10 > trackcount0) continue;
                        double dotprod = extendedTrack->direction().dot((*it0)->direction());
                        if (extendedTrack->isAssociated(*it0)) continue;
                        if ((*it0)->isAssociated(extendedTrack)) continue;
                        if (dotprod > dotprodmax) {
                            dotprodmax = dotprod;
                            associatedMooreTrack = *it0;
                            if (dotprodmax > 0.8) break;
                        }
                    }
                    if (dotprodmax > 0.8 && associatedMooreTrack) {
                        extendedTrack->addAssociatedTrack(associatedMooreTrack);
                        associatedMooreTrack->addAssociatedTrack(extendedTrack);
                    }
                }
            }
#endif
            trackVec.push_back(extendedTrack);
        }
    }

    template <typename T> void EventHandler::split_along_barcode(const std::vector<T>& container, std::map<int, std::set<T>>& barcode_map) {
        for (const auto& hit_ptr : container) {
            int barcode = hit_ptr->barCode();
            if (barcode <= 0) continue;
            barcode_map[barcode].insert(hit_ptr);
        }
    }

    void EventHandler::createExtendedTruthTracks() {
        // check that event is set
        if (!m_event) return;
        //
        // Truth track collection
        //

        const MuonCalibTruthCollection* truth = m_event->calibTruthCollection();
        if (!truth) return;

        if (truth->TruthContainer().empty()) return;

        std::map<int, TruthPtr> truthTrkME, truthTrkIP, truthTrkRest;

        std::map<int, std::set<TruthMdtPtr>> truthTrkMdtHitMap;
        std::map<int, std::set<TruthCscPtr>> truthTrkCscHitMap;
        std::map<int, std::set<TruthRpcPtr>> truthTrkRpcHitMap;
        std::map<int, std::set<TruthTgcPtr>> truthTrkTgcHitMap;

        MsgStream log(Athena::getMessageSvc(), "EventHandler");
        for (const TruthPtr& truth_part : truth->TruthContainer()) {
            int barcode = truth_part->barCode();
            if (barcode <= 0) continue;
            if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "createExtendedTruthTracks() truth track barcode " << barcode << endmsg;

            if (truth_part->position().perp() < 4000 && std::abs(truth_part->position().z()) < 6000.) {
                if (truth_part->position().perp() < 100 && std::abs(truth_part->position().z()) < 500.) {
                    // Close to IP
                    truthTrkIP[barcode] = truth_part;
                } else {
                    truthTrkRest[barcode] = truth_part;
                }
            } else {
                // Take track at Muon Entry
                truthTrkME[barcode] = truth_part;
            }
        }

        //
        //  Mdt truth hit information
        //
        split_along_barcode(truth->MdtContainer(), truthTrkMdtHitMap);

        if (truthTrkMdtHitMap.empty()) return;

        //
        //  Csc truth hit information
        //
        split_along_barcode(truth->CscContainer(), truthTrkCscHitMap);
        //
        //  Rpc truth hit information
        //

        split_along_barcode(truth->RpcContainer(), truthTrkRpcHitMap);

        //
        //  Tgc truth hit information
        //
        split_along_barcode(truth->TgcContainer(), truthTrkTgcHitMap);

        for (const auto& it_IP : truthTrkIP) {
            std::shared_ptr<MuonCalibTrack_E> truth_trk =
                transform_to_trk(it_IP.second, truthTrkMdtHitMap, truthTrkCscHitMap, truthTrkRpcHitMap, truthTrkTgcHitMap);
            int barcode = it_IP.first;
            int pdgcode = it_IP.second->PDGCode();
            int author = -13;
            if (std::abs(pdgcode) != 13) author = -113;
            TrackVec& trackVec = m_extendedTracks[author];
            std::shared_ptr<MuonCalibExtendedTrack> extendedTrack = std::make_shared<MuonCalibExtendedTrack>(*truth_trk, pdgcode, barcode);
            trackVec.push_back(extendedTrack);
            // dump truth track
            if (m_debug) extendedTrack->dump();
        }

        for (const auto& it_ME : truthTrkME) {
            std::shared_ptr<MuonCalibTrack_E> truth_trk =
                transform_to_trk(it_ME.second, truthTrkMdtHitMap, truthTrkCscHitMap, truthTrkRpcHitMap, truthTrkTgcHitMap);

            int barcode = it_ME.first;
            int pdgcode = it_ME.second->PDGCode();
            int author = -1013;
            if (std::abs(pdgcode) != 13) author = -1113;
            TrackVec& trackVec = m_extendedTracks[author];
            std::shared_ptr<MuonCalibExtendedTrack> extendedTrack = std::make_shared<MuonCalibExtendedTrack>(*truth_trk, pdgcode, barcode);
            trackVec.push_back(extendedTrack);
            // dump truth track
            if (m_debug) extendedTrack->dump();
        }
    }
    std::shared_ptr<MuonCalibTrack_E> EventHandler::transform_to_trk(const TruthPtr& truth_part,
                                                                     const std::map<int, std::set<TruthMdtPtr>>& mdt_hits,
                                                                     const std::map<int, std::set<TruthCscPtr>>& csc_hits,
                                                                     const std::map<int, std::set<TruthRpcPtr>>& rpc_hits,
                                                                     const std::map<int, std::set<TruthTgcPtr>>& tgc_hits) {
        const int barcode = truth_part->barCode();
        MuonCalibTrack_E::defineParams build_pars{};
        build_pars.x0 = truth_part->position().x();
        build_pars.y0 = truth_part->position().y();
        build_pars.z0 = truth_part->position().z();
        build_pars.phi = truth_part->momentum().phi();
        build_pars.theta = truth_part->momentum().theta();
        build_pars.author = std::abs(truth_part->PDGCode());
        build_pars.qOverP = (1. - 2. * (truth_part->PDGCode() < 0)) / (truth_part->momentum().mag());
        build_pars.cov.setZero();
        std::shared_ptr<MuonCalibTrack_E> track_ptr = std::make_shared<MuonCalibTrack_E>(build_pars);

        std::map<int, std::set<TruthMdtPtr>>::const_iterator mdt_itr = mdt_hits.find(barcode);
        if (mdt_itr != mdt_hits.end()) {
            for (const TruthMdtPtr& mdt_hit : mdt_itr->second) {
                MuonCalibHit_E::definePars hit_pars{};
                hit_pars.driftRadius = mdt_hit->driftRadius();
                hit_pars.pos = Amg::Vector3D(0., mdt_hit->positionAlongTube(), 0.);
                track_ptr->addHit(std::make_shared<MuonCalibHit_E>(mdt_hit->identify(), hit_pars));
            }
        }
        std::map<int, std::set<TruthCscPtr>>::const_iterator csc_itr = csc_hits.find(barcode);
        if (csc_itr != csc_hits.end()) {
            for (const TruthCscPtr& csc_hit : csc_itr->second) {
                MuonCalibHit_E::definePars hit_pars{};
                hit_pars.driftRadius = csc_hit->time();
                track_ptr->addHit(std::make_shared<MuonCalibHit_E>(csc_hit->identify(), hit_pars));
            }
        }

        std::map<int, std::set<TruthRpcPtr>>::const_iterator rpc_itr = rpc_hits.find(barcode);
        if (rpc_itr != rpc_hits.end()) {
            for (const TruthRpcPtr& rpc_hit : rpc_itr->second) {
                MuonCalibHit_E::definePars hit_pars{};
                hit_pars.driftRadius = rpc_hit->time();
                track_ptr->addHit(std::make_shared<MuonCalibHit_E>(rpc_hit->identify(), hit_pars));
            }
        }
        std::map<int, std::set<TruthTgcPtr>>::const_iterator tgc_itr = tgc_hits.find(barcode);
        if (tgc_itr != tgc_hits.end()) {
            for (const TruthTgcPtr& tgc_hit : tgc_itr->second) {
                MuonCalibHit_E::definePars hit_pars{};
                hit_pars.driftRadius = tgc_hit->time();
                track_ptr->addHit(std::make_shared<MuonCalibHit_E>(tgc_hit->identify(), hit_pars));
            }
        }
        return track_ptr;
    }

    void EventHandler::createExtendedSegments() {
        // check that event is set
        if (!m_event) return;

        for (const MuonCalibEvent::PatternPtr& pat_ptr : m_event->pattern()) {
            for (const MuonCalibPattern::MuonCalibSegPtr& seg_ptr : pat_ptr->muonSegs()) {
                // get author
                int author = seg_ptr->author();
                SegmentVec& segVec = m_extendedSegments[author];

                // create new extended segment
                MuonCalibExtendedSegment* extendedSegment = new MuonCalibExtendedSegment(*seg_ptr);
                segVec.emplace_back(extendedSegment);
            }
        }
    }

    void EventHandler::associateSegmentsWithTracks(SegmentVec& segments, TrackVec& tracks) {
        /// helper struct to evaluate the segment match quality
        struct segmentMatchQual {
            ExtendedSegmentPtr seg;
            int assoc_quality{0};
            bool vetoed{false};
            bool operator<(const segmentMatchQual& other) const {
                if (assoc_quality != other.assoc_quality) return assoc_quality > other.assoc_quality;
                return seg->chi2() < other.seg->chi2();
            };
            segmentMatchQual(ExtendedSegmentPtr _ptr, int score) : seg{std::move(_ptr)}, assoc_quality{score} {}
        };

        for (const ExtendedTrackPtr& trk : tracks) {
            std::vector<segmentMatchQual> segmentTrackMatch;
            for (const ExtendedSegmentPtr& seg : segments) {
                int match = associateSegmentWithTrack(*seg, *trk);
                if (match > 200) { segmentTrackMatch.emplace_back(seg, match); }
            }
            std::vector<segmentMatchQual>::iterator mit = segmentTrackMatch.begin();
            std::vector<segmentMatchQual>::iterator mit_end = segmentTrackMatch.end();
            std::sort(mit, mit_end);
            for (; mit != mit_end; ++mit) {
                std::vector<segmentMatchQual>::iterator mit2 = mit + 1;
                for (; mit2 != mit_end; ++mit2) {
                    if (segmentOverlap(*mit->seg, *mit2->seg) > 1) { mit2->vetoed = true; }
                }

                std::cout << "We need to refine the track to segment association " << __FILE__ << " " << __LINE__ << std::endl;
                // if (!mit->vetoed) {
                //     mit->seg->addTrack(trk);
                //     trk->addSegment(mit->seg);
                // }
            }
        }
    }
    int EventHandler::segmentOverlap(const MuonCalibExtendedSegment& segment1, const MuonCalibExtendedSegment& segment2) {
        int overlap = 0;
        if (segment1.mdtHitsOnTrack() > 0 && segment2.mdtHitsOnTrack() > 0) {
            std::set<MuonFixedId> segmentIds;
            for (const MuonCalibSegment::MdtHitPtr& mdt_hit : segment1.mdtHOT()) {
                const MuonFixedId& id = mdt_hit->identify();
                if (!id.isValid()) {
                    MsgStream log(Athena::getMessageSvc(), "EventHandler");
                    log << MSG::WARNING << "segmentOverlap(), invalid MDT id! " << endmsg;
                    continue;
                }
                segmentIds.insert(id);
            }
            for (const MuonCalibSegment::MdtHitPtr& mdt_hit : segment2.mdtHOT()) {
                const MuonFixedId& id = mdt_hit->identify();
                if (!id.isValid()) {
                    MsgStream log(Athena::getMessageSvc(), "EventHandler");
                    log << MSG::WARNING << "segmentOverlap(), invalid MDT id! " << endmsg;
                    continue;
                }
                overlap += (segmentIds.count(id) > 0);
            }
            return overlap;
        } else if (segment1.cscHitsOnTrack() > 0 && segment2.cscHitsOnTrack() > 0) {
            std::set<MuonFixedId> segmentIds;
            for (const MuonCalibSegment::CscHitPtr& csc_hit : segment1.cscHOT()) {
                const MuonFixedId& id = csc_hit->identify();
                if (!id.isValid()) {
                    MsgStream log(Athena::getMessageSvc(), "EventHandler");
                    log << MSG::WARNING << "segmentOverlap(), invalid CSC id! " << endmsg;
                    continue;
                }
                segmentIds.insert(id);
            }
            for (const MuonCalibSegment::CscHitPtr& csc_hit : segment1.cscHOT()) {
                MuonFixedId id = csc_hit->identify();
                if (!id.isValid()) {
                    MsgStream log(Athena::getMessageSvc(), "EventHandler");
                    log << MSG::WARNING << "segmentOverlap(), invalid CSC id! " << endmsg;
                    continue;
                }
                overlap += (segmentIds.count(id) > 0);
            }
        }
        return overlap;
    }
    int EventHandler::associateSegmentWithTrack(const MuonCalibExtendedSegment& segment, const MuonCalibExtendedTrack& track) {
        double minRadius = 1.;  // the sign of all hits with a drift radius smaller that minRadius is not taken into account
        unsigned int noppositeSign{0}, nsameSign{0}, nnotOnTrack{0};
        for (const MuonCalibSegment::MdtHitPtr& mdt_hit : segment.mdtHOT()) {
            const MuonFixedId& id = mdt_hit->identify();
            if (!id.isValid()) {
                MsgStream log(Athena::getMessageSvc(), "EventHandler");
                log << MSG::WARNING << "associateSegmentWithTrack(), invalid MDT id! " << endmsg;
                continue;
            }
            // look up hit on track
            IdHitMap::const_iterator trkHit = track.idHitMap().find(id);
            if (trkHit != track.idHitMap().end()) {
                // found hit
                double signSeg = mdt_hit->driftRadius() < -minRadius ? -1. : 1.;
                double signTrk = trkHit->second->driftRadius() < -minRadius ? -1. : 1.;
                if (signSeg == signTrk)
                    ++nsameSign;
                else
                    ++noppositeSign;
            } else {
                ++nnotOnTrack;
            }
        }

        for (const MuonCalibSegment::CscHitPtr& csc_hit : segment.cscHOT()) {
            MuonFixedId id = csc_hit->identify();
            if (!id.isValid()) {
                MsgStream log(Athena::getMessageSvc(), "EventHandler");
                log << MSG::WARNING << "associateSegmentWithTrack(), invalid CSC id! " << endmsg;
                continue;
            }
            // look up hit on track
            IdHitMap::const_iterator trkHit = track.idHitMap().find(id);
            if (trkHit != track.idHitMap().end()) {
                // found hit
                ++nsameSign;
            } else {
                ++nnotOnTrack;
            }
        }
        // Okay the weighting factors seem a bit arbitrary
        return nnotOnTrack + 10 * noppositeSign + 100 * nsameSign;
    }

    void EventHandler::linkTracks(int referenceIndex) {
        // check if author found in list
        AuthorTrackVecMap::iterator pos = m_extendedTracks.find(referenceIndex);
        if (pos == m_extendedTracks.end()) return;

        TrackVec& refTracks = pos->second;

        // loop over reference tracks and associate other authors
        TrackVec::iterator rit = refTracks.begin();
        TrackVec::iterator rit_end = refTracks.end();
        for (; rit != rit_end; ++rit) {
            // loop over authors
            AuthorTrackVecMap::iterator ait = m_extendedTracks.begin();
            AuthorTrackVecMap::iterator ait_end = m_extendedTracks.end();
            for (; ait != ait_end; ++ait) {
                // skip same collection
                if (ait->first == referenceIndex) continue;

                TrackVec& tracks = ait->second;

                match(**rit, tracks);
            }
        }
    }

    void EventHandler::match(const MuonCalibExtendedTrack& reference, EventHandler::TrackVec& tracks) {
        const MuonCalibExtendedTrack* bestMatch = nullptr;
        MuonCalibExtendedTrackOverlap bestOverlap;
        // loop over tracks and calcualte overlap with reference
        TrackVec::iterator it = tracks.begin();
        TrackVec::iterator it_end = tracks.end();
        for (; it != it_end; ++it) {
            // check whether the two tracks are not already associated

            // if (reference.isAssociated(*it)) continue;

            MuonCalibExtendedTrackOverlap overlap = reference.calculateHitOverlap(**it);
            if (overlap.sharedPrecisionHits() > 0) {
                if (overlap.sharedPrecisionHits() > bestOverlap.sharedPrecisionHits()) {
                    bestMatch = it->get();
                    bestOverlap = overlap;
                }
            }
        }

        std::cout << " Still need to be done a proper association " << __FILE__ << " " << __LINE__ << std::endl;
        if (bestMatch) {
            //     reference.addAssociatedTrack(bestMatch);
            //     bestMatch->addAssociatedTrack(&reference);
        }
    }

}  // namespace MuonCalib
