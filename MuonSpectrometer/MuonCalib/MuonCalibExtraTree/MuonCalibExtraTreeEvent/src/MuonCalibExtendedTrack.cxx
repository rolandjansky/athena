/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"

#include <sstream>

#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraUtils/MuonCalibSimpleHoleSearch.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    const std::vector<std::shared_ptr<MuonCalibExtendedSegment>>& MuonCalibExtendedTrack::associatedSegments() const {
        return m_associatedSegments;
    }

    void MuonCalibExtendedTrack::addSegment(MuonCalibExtendedSegment* seg) { m_associatedSegments.emplace_back(seg); }
    MuonCalibExtendedTrack::~MuonCalibExtendedTrack() = default;
    MuonCalibExtendedTrack::MuonCalibExtendedTrack(const MuonCalibTrack_E& track, int pdgCode, int barCode) :
        MuonCalibTrack_E(track), m_pdgCode{pdgCode}, m_barCode{barCode} {
        m_summary.isTrack = true;
        MuonFixedIdManipulator idManip;

        MuonCalibTrackSummary::ChamberHitSummary* currentChamberSummary = nullptr;

        for (const MuonCalibTrack_E::CalibHitPtr& hit : track.hits()) {
            int type = hit->type();
            // count scatters
            if (type > 10) {
                ++m_summary.nscatters;
                continue;
            }

            const MuonFixedId& id = hit->identify();
            if (!id.isValid()) {
                if (type == 3)
                    ++m_summary.npseudo;
                else {
                    // these are the ID hits
                    if (hit->position().perp() < 200.) {
                        ++m_summary.npixel;
                    } else if (hit->position().perp() < 540.) {
                        ++m_summary.nsct;
                    } else if (hit->position().perp() < 1100.) {
                        if (std::abs(hit->position().z()) < 750.)
                            ++m_summary.ntrtBarrel;
                        else
                            ++m_summary.ntrtEndcap;
                    }
                }
                continue;
            }

            MuonFixedId chId = idManip.chamberIdentifier(id);

            bool measuresPhi = idManip.measuresPhi(id);
            bool isMdt = id.is_mdt();
            bool isFirst = isMdt ? id.mdtMultilayer() == 1 : !measuresPhi;

            // check whether first chamber or new chamber
            if (!currentChamberSummary || currentChamberSummary->chId != chId) {
                m_summary.chamberHitSummary.push_back(MuonCalibTrackSummary::ChamberHitSummary(chId));
                currentChamberSummary = &m_summary.chamberHitSummary.back();
            }

            MuonCalibTrackSummary::ChamberHitSummary::Projection& proj =
                isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

            MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);
            MuonFixedId chamberId = idManip.chamberIdentifier(id);

            if (type == 6) {
                ++m_summary.nholes;
                ++proj.nholes;
            } else if (type == 4) {
                ++m_summary.noutliers;
                ++proj.noutliers;
            } else if (type == 5) {
                ++m_summary.noutliers;
                ++proj.ndeltas;
            } else {
                IdHitMap::iterator hitPos = m_hitIdMap.find(id);
                if (hitPos == m_hitIdMap.end()) m_hitIdMap[id] = hit;
                m_hitsPerChamber[chamberId].emplace_back(hit);

                ++proj.nhits;
                // hits counts
                ++m_summary.nhits;
                if ((*hit).position().y() < 0.)
                    ++m_summary.nhitsLowerHemisphere;
                else
                    ++m_summary.nhitsUpperHemisphere;

                if (id.is_mdt()) {
                    ++m_summary.nmdtHits;
                    m_mdtHitsPerStationIndex[stationIndex].emplace_back(hit);
                    if (idManip.isEndcap(id)) {
                        if (id.eta() < 0.)
                            m_summary.hasEndcapA = true;
                        else
                            m_summary.hasEndcapC = true;
                        if (stationIndex == MuonFixedIdManipulator::EIA || stationIndex == MuonFixedIdManipulator::EIC ||
                            stationIndex == MuonFixedIdManipulator::EMA || stationIndex == MuonFixedIdManipulator::EMC)
                            m_summary.hasEndcapLayersWithTGC = true;
                    } else {
                        m_summary.hasBarrel = true;
                        if (stationIndex == MuonFixedIdManipulator::BM || stationIndex == MuonFixedIdManipulator::BO)
                            m_summary.hasBarrelLayersWithRPC = true;
                    }

                    m_summary.precisionStationLayers.insert(stationIndex);
                    if (currentChamberSummary->nMdtHitsMl1() > 0 && currentChamberSummary->nMdtHitsMl2() &&
                        currentChamberSummary->nhits() > 3) {
                        m_summary.goodPrecisionStationLayers.insert(stationIndex);
                    }
                } else if (id.is_rpc()) {
                    if (id.rpcMeasuresPhi()) {
                        ++m_summary.nrpcPhiHits;
                    } else {
                        ++m_summary.nrpcEtaHits;
                    }
                    m_rpcHitsPerStationIndex[stationIndex].emplace_back(hit);
                    m_summary.hasBarrel = true;

                    MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
                    if (measuresPhi) m_summary.phiStationLayers.insert(phiStationIndex);
                    if (currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0) {
                        m_summary.phiEtaStationLayers.insert(phiStationIndex);
                    }
                } else if (id.is_tgc()) {
                    if (id.tgcIsStrip())
                        ++m_summary.ntgcPhiHits;
                    else
                        ++m_summary.ntgcEtaHits;
                    m_tgcHitsPerStationIndex[stationIndex].emplace_back(hit);

                    if (id.eta() < 0.)
                        m_summary.hasEndcapA = true;
                    else
                        m_summary.hasEndcapC = true;
                    m_summary.hasEndcapLayersWithTGC = true;

                    MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
                    if (measuresPhi) m_summary.phiStationLayers.insert(phiStationIndex);
                    if (currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0) {
                        m_summary.phiEtaStationLayers.insert(phiStationIndex);
                    }
                } else if (id.is_csc()) {
                    if (id.cscMeasuresPhi())
                        ++m_summary.ncscPhiHits;
                    else
                        ++m_summary.ncscEtaHits;
                    m_cscHitsPerStationIndex[stationIndex].emplace_back(hit);
                    if (id.eta() < 0.)
                        m_summary.hasEndcapA = true;
                    else
                        m_summary.hasEndcapC = true;

                    MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
                    if (measuresPhi) { m_summary.phiStationLayers.insert(phiStationIndex); }
                    m_summary.precisionStationLayers.insert(stationIndex);
                    if (currentChamberSummary->netaHits() > 2 && currentChamberSummary->nphiHits() > 2) {
                        m_summary.goodPrecisionStationLayers.insert(stationIndex);
                        m_summary.phiEtaStationLayers.insert(phiStationIndex);
                    }
                }
            }
        }

        const StationIndexHitsMap& mdtStHitMap = mdtStationIndexHitsMap();
        const StationIndexHitsMap& rpcStHitMap = rpcStationIndexHitsMap();
        const StationIndexHitsMap& tgcStHitMap = tgcStationIndexHitsMap();
        const StationIndexHitsMap& cscStHitMap = cscStationIndexHitsMap();

        MuonCalibSimpleHoleSearch holeSearch;
        double tolerance = 100.;
        MuonCalibSimpleHoleSearch::ResultVec intersectedLayers = holeSearch.intersectsWithGeometry(position(), direction(), tolerance);
        MuonCalibSimpleHoleSearch::ResultIt rit = intersectedLayers.begin();
        MuonCalibSimpleHoleSearch::ResultIt rit_end = intersectedLayers.end();
        for (; rit != rit_end; ++rit) {
            m_intersectedLayers[rit->stationLayerId] = *rit;
            const MuonFixedId& stId = rit->stationLayerId;
            MuonFixedIdManipulator::StationIndex stIndex = idManip.stationLayerIndex(stId);
            bool isEndcap = idManip.isEndcap(stId);

            StationIndexHitsMap::const_iterator mdtIt = mdtStHitMap.find(stIndex);
            unsigned int nmdtHits = mdtIt != mdtStHitMap.end() ? mdtIt->second.size() : 0;

            StationIndexHitsMap::const_iterator rpcIt = rpcStHitMap.find(stIndex);
            unsigned int nrpcHits = rpcIt != rpcStHitMap.end() ? rpcIt->second.size() : 0;

            StationIndexHitsMap::const_iterator tgcIt = tgcStHitMap.find(stIndex);
            unsigned int ntgcHits = tgcIt != tgcStHitMap.end() ? tgcIt->second.size() : 0;

            StationIndexHitsMap::const_iterator cscIt = cscStHitMap.find(stIndex);
            unsigned int ncscHits = cscIt != cscStHitMap.end() ? cscIt->second.size() : 0;

            // flag intersected layers without any hits
            unsigned int ntrig = isEndcap ? ntgcHits : nrpcHits;
            unsigned int nprec = nmdtHits + ncscHits;
            if (nprec + ntrig == 0) m_intersectedLayersWithoutHits.insert(stIndex);

            // flag layers where we would expect RPC hits but they are not on the track
            if (!isEndcap && stIndex != MuonFixedIdManipulator::BI && stIndex != MuonFixedIdManipulator::BE && nrpcHits == 0)
                m_intersectedRpcLayerWithoutHits.insert(stIndex);

            // flag layers where we would expect TGC hits but they are not on the track
            if (isEndcap && stIndex != MuonFixedIdManipulator::EIA && stIndex != MuonFixedIdManipulator::EIC && ntgcHits == 0)
                m_intersectedTgcLayerWithoutHits.insert(stIndex);
        }
    }

    std::string MuonCalibExtendedTrack::dump() const {
        std::ostringstream sout;
        sout << dumpPars() << std::endl << dumpSummary() << dumpIntersects() << std::endl;
        return sout.str();
    }

    std::string MuonCalibExtendedTrack::dumpPars() const {
        std::ostringstream sout;
        double sign = qOverP() < 0. ? -1 : 1;
        sout << "Author " << std::setw(4) << author() << std::setprecision(4) << " chi2 " << chi2() << " ndof " << ndof() << " r "
             << (int)position().perp() << " z " << (int)z0() << std::setprecision(5) << " phi " << phi() << " theta " << theta()
             << std::setw(6) << " q*mom " << (int)p() * sign << " pt " << std::setw(5) << (int)pt() << " association: segments "
             << m_associatedSegments.size() << " tracks " << m_associatedTracks.size();

        // if (author() == 0) {
        //     const MuonCalibExtendedTrack* ipTrack = getAssociatedTrack(1);
        //     if (ipTrack) {
        //         sout << std::endl
        //              << "  at ip "
        //              << " r " << (int)ipTrack->position().perp() << " z " << (int)ipTrack->z0() << std::setprecision(5) << " phi "
        //              << ipTrack->phi() << " theta " << ipTrack->theta() << std::setw(6) << " q*mom " << (int)ipTrack->p() * sign << " pt
        //              "
        //              << std::setw(5) << (int)ipTrack->pt();
        //     }
        // }
        return sout.str();
    }

    std::string MuonCalibExtendedTrack::dumpSummary() const {
        std::ostringstream sout;
        sout << " " << m_summary.dump();
        return sout.str();
    }

    std::string MuonCalibExtendedTrack::dumpIntersects() const {
        MuonFixedIdPrinter printer{};
        MuonFixedIdManipulator idManip;

        const StationIndexHitsMap& mdtStHitMap = mdtStationIndexHitsMap();
        const StationIndexHitsMap& rpcStHitMap = rpcStationIndexHitsMap();
        const StationIndexHitsMap& tgcStHitMap = tgcStationIndexHitsMap();
        const StationIndexHitsMap& cscStHitMap = cscStationIndexHitsMap();

        std::ostringstream sout;
        sout << " Intersected layers: " << m_intersectedLayers.size() << " without: any hits " << m_intersectedLayersWithoutHits.size()
             << "  rpc hits " << m_intersectedRpcLayerWithoutHits.size() << "  tgc hits " << m_intersectedTgcLayerWithoutHits.size()
             << std::endl;
        unsigned int nentries = m_intersectedLayers.size();
        unsigned int currentEntry = 0;
        StationIntersectedLayerMap::const_iterator it = m_intersectedLayers.begin();
        StationIntersectedLayerMap::const_iterator it_end = m_intersectedLayers.end();
        sout.setf(std::ios::left);
        for (; it != it_end; ++it) {
            const MuonFixedId& stId = it->first;
            MuonFixedIdManipulator::StationIndex stIndex = idManip.stationLayerIndex(stId);
            bool isEndcap = idManip.isEndcap(stId);

            StationIndexHitsMap::const_iterator mdtIt = mdtStHitMap.find(stIndex);
            unsigned int nmdtHits = mdtIt != mdtStHitMap.end() ? mdtIt->second.size() : 0;

            StationIndexHitsMap::const_iterator rpcIt = rpcStHitMap.find(stIndex);
            unsigned int nrpcHits = rpcIt != rpcStHitMap.end() ? rpcIt->second.size() : 0;

            StationIndexHitsMap::const_iterator tgcIt = tgcStHitMap.find(stIndex);
            unsigned int ntgcHits = tgcIt != tgcStHitMap.end() ? tgcIt->second.size() : 0;

            StationIndexHitsMap::const_iterator cscIt = cscStHitMap.find(stIndex);
            unsigned int ncscHits = cscIt != cscStHitMap.end() ? cscIt->second.size() : 0;

            unsigned int ntrig = isEndcap ? ntgcHits : nrpcHits;
            unsigned int nprec = nmdtHits + ncscHits;

            sout << "    " << std::setw(5) << printer.stationLayerIdentifier(stId);
            if (nprec + ntrig != 0)
                sout << " presicion hits " << std::setw(3) << nprec << " trigger hits " << std::setw(3) << ntrig;
            else
                sout << std::setw(36) << " no hits in layer";
            sout << "  intersect position " << it->second.intersectPosition;

            // increase count before check to allow equals check
            ++currentEntry;
            if (currentEntry != nentries) sout << std::endl;
        }
        return sout.str();
    }

    MuonCalibExtendedTrackOverlap MuonCalibExtendedTrack::calculateHitOverlap(const MuonCalibExtendedTrack& track) const {
        const IdHitsMap& chHitMap = track.hitsPerChamberMap();
        MuonFixedIdManipulator manip;

        MuonCalibExtendedTrackOverlap overlap;

        std::set<MuonFixedId> sharedChambers;

        // loop over stations and check whether they are also present in other list
        for (const auto& rit : m_hitsPerChamber) {
            IdHitsMap::const_iterator pos = chHitMap.find(rit.first);
            if (pos != chHitMap.end()) {
                // shared chamber add to list
                sharedChambers.insert(rit.first);

                std::set<MuonFixedId> foundIds, sharedEtaLayers, sharedPhiLayers, firstEtaLayers, firstPhiLayers, secondEtaLayers,
                    secondPhiLayers;
                unsigned int wrongSign{0};
                for (const CalibHitE_Ptr& calib_hit : rit.second) {
                    const MuonFixedId& id = calib_hit->identify();
                    bool measuresPhi = manip.measuresPhi(id);
                    MuonFixedId layerId = manip.moduleIdentifier(id, true);

                    MuonCalibTrack_E::HitVector::const_iterator hit =
                        std::find_if(pos->second.begin(), pos->second.end(),
                                     [&id](const MuonCalibTrack_E::CalibHitPtr& hit) { return id == hit->identify(); });
                    if (hit != pos->second.end()) {
                        const MuonCalibTrack_E::CalibHitPtr test_hit = (*hit);
                        foundIds.insert(id);

                        if (id.is_mdt()) {
                            /// Why is it important to be 2 mm apart from the wire in order to swap sign?
                            if (std::abs(calib_hit->driftRadius()) > 2. && std::abs(test_hit->driftRadius()) > 2. &&
                                calib_hit->driftRadius() * test_hit->driftRadius() < 0.)
                                ++wrongSign;
                            else
                                sharedEtaLayers.insert(id);
                        } else {
                            if (measuresPhi)
                                sharedPhiLayers.insert(layerId);
                            else
                                sharedEtaLayers.insert(layerId);
                        }
                    } else {
                        if (!id.is_mdt() && foundIds.count(layerId)) continue;
                        if (measuresPhi)
                            firstPhiLayers.insert(layerId);
                        else
                            firstEtaLayers.insert(layerId);
                    }
                }

                for (const MuonCalibTrack_E::CalibHitPtr& it2 : pos->second) {
                    MuonFixedId id = it2->identify().is_mdt() ? it2->identify() : manip.moduleIdentifier(it2->identify(), true);
                    if (!foundIds.count(id)) {
                        bool measuresPhi = manip.measuresPhi(id);
                        if (measuresPhi)
                            secondPhiLayers.insert(id);
                        else
                            secondEtaLayers.insert(id);
                    }
                }
                MuonCalib::MuonCalibExtendedTrackOverlap::TechnologyOverlap *phi_or{nullptr}, *eta_or{nullptr};
                if (rit.first.is_mdt()) {
                    eta_or = &overlap.mdt;
                } else if (rit.first.is_rpc()) {
                    phi_or = &overlap.rpcPhi;
                    eta_or = &overlap.rpcEta;
                } else if (rit.first.is_tgc()) {
                    phi_or = &overlap.tgcPhi;
                    eta_or = &overlap.tgcEta;
                } else if (rit.first.is_csc()) {
                    phi_or = &overlap.cscPhi;
                    eta_or = &overlap.cscEta;
                }
                if (eta_or) {
                    (*eta_or).shared += sharedEtaLayers.size();
                    ;
                    (*eta_or).first += firstEtaLayers.size();
                    (*eta_or).second += secondEtaLayers.size();
                }
                if (phi_or) {
                    (*phi_or).shared += sharedPhiLayers.size();
                    (*phi_or).first += firstPhiLayers.size();
                    (*phi_or).second += secondPhiLayers.size();
                }
            }
            /// The current hit is not part of the overlap
            else {
                std::set<MuonFixedId> foundIds;
                for (const MuonCalibTrack_E::CalibHitPtr& it1 : rit.second) {
                    if (rit.first.is_mdt()) {
                        ++overlap.mdt.first;
                    } else {
                        MuonFixedId id = manip.moduleIdentifier(it1->identify(), true);
                        if (foundIds.count(id)) continue;
                        foundIds.insert(id);
                        bool measuresPhi = manip.measuresPhi(it1->identify());

                        if (rit.first.is_rpc()) {
                            if (!measuresPhi)
                                ++overlap.rpcEta.first;
                            else
                                ++overlap.rpcPhi.first;
                        } else if (rit.first.is_tgc()) {
                            if (!measuresPhi)
                                ++overlap.tgcEta.first;
                            else
                                ++overlap.tgcPhi.first;
                        } else if (rit.first.is_csc()) {
                            if (!measuresPhi)
                                ++overlap.cscEta.first;
                            else
                                ++overlap.cscPhi.first;
                        }
                    }
                }
            }
        }

        // loop over stations and check whether they are also present in other list
        for (const auto& rit : chHitMap) {
            // skip already handled chambers
            if (sharedChambers.count(rit.first)) continue;
            std::set<MuonFixedId> foundIds;
            for (const MuonCalibTrack_E::CalibHitPtr& it1 : rit.second) {
                if (rit.first.is_mdt()) {
                    ++overlap.mdt.second;
                } else {
                    MuonFixedId id = manip.moduleIdentifier(it1->identify(), true);
                    if (foundIds.count(id)) continue;
                    foundIds.insert(id);
                    bool measuresPhi = manip.measuresPhi(it1->identify());
                    if (rit.first.is_rpc()) {
                        if (!measuresPhi)
                            ++overlap.rpcEta.second;
                        else
                            ++overlap.rpcPhi.second;
                    } else if (rit.first.is_tgc()) {
                        if (!measuresPhi)
                            ++overlap.tgcEta.second;
                        else
                            ++overlap.tgcPhi.second;
                    } else if (rit.first.is_csc()) {
                        if (!measuresPhi)
                            ++overlap.cscEta.second;
                        else
                            ++overlap.cscPhi.second;
                    }
                }
            }
        }
        return overlap;
    }
    // bool MuonCalibExtendedTrack::isAssociated(const MuonCalibExtendedSegment* segment) const {
    //     return std::find_if(m_associatedSegments.begin(), m_associatedSegments.end(),
    //                         [&segment](const std::shared_ptr<MuonCalibExtendedSegment>& test) { return test.get() == segment; }) !=
    //            m_associatedSegments.end();
    // }

    // bool MuonCalibExtendedTrack::isAssociated(const MuonCalibExtendedTrack* track) const {
    //     return std::find_if(m_associatedTracks.begin(), m_associatedTracks.end(),
    //                         [track](const std::shared_ptr<MuonCalibExtendedTrack>& test) { return test.get() == track; }) !=
    //            m_associatedTracks.end();
    // }

    // std::shared_ptr<MuonCalibExtendedTrack> MuonCalibExtendedTrack::getAssociatedTrack(int author) const {
    //     std::vector<std::shared_ptr<MuonCalibExtendedTrack>>::const_iterator pos = std::find_if(
    //         m_associatedTracks.begin(),

    //         m_associatedTracks.end(), [author](const std::shared_ptr<MuonCalibExtendedTrack>& test) { return test->author() == author;
    //         });
    //     if (pos != m_associatedTracks.end()) return *pos;
    //     return nullptr;
    // }

    // float MuonCalibExtendedTrack::d0ip() const {
    //     if (author() == 0) {
    //         std::shared_ptr<MuonCalibExtendedTrack> ipTrack = getAssociatedTrack(10);
    //         if (ipTrack) {
    //             return ipTrack->d0();
    //         } else {
    //             ipTrack = getAssociatedTrack(1);
    //             if (ipTrack) return ipTrack->d0();
    //         }
    //     }
    //     return MuonCalibTrack_E::d0();
    // }

    // float MuonCalibExtendedTrack::z0ip() const {
    //     if (author() == 0) {
    //         std::shared_ptr<MuonCalibExtendedTrack> ipTrack = getAssociatedTrack(10);
    //         if (ipTrack) {
    //             return ipTrack->z0();
    //         } else {
    //             ipTrack = getAssociatedTrack(1);
    //             if (ipTrack) return ipTrack->z0();
    //         }
    //     }
    //     return MuonCalibTrack_E::z0();
    // }

    // float MuonCalibExtendedTrack::pip() const {
    //     if (author() == 0) {
    //         std::shared_ptr<MuonCalibExtendedTrack> ipTrack = getAssociatedTrack(10);
    //         if (ipTrack) {
    //             return ipTrack->p();
    //         } else {
    //             ipTrack = getAssociatedTrack(1);
    //             if (ipTrack) return ipTrack->p();
    //         }
    //     }
    //     return MuonCalibTrack_E::p();
    // }

    int MuonCalibExtendedTrack::pdgCode() const { return m_pdgCode; }

    int MuonCalibExtendedTrack::barCode() const { return m_barCode; }

    /// Documentation needed
    bool MuonCalibExtendedTrack::isIDConfirmed() const {
        //   static const   std::set<int> idAuthors{20, 25, 30, 35, 40, 41, 42};

        // for (const std::shared_ptr<MuonCalibExtendedTrack>& it : m_associatedTracks) {
        //     if (idAuthors.count(it->author())) return true;
        //     if (it)->author() == 140 ) {
        //             std::shared_ptr<const MuonCalibExtendedTrack> mbTrack = it->getAssociatedTrack(100);
        //             if (mbTrack) {
        //                 if (mbTrack->ndof() != it->ndof())
        //                     return true;
        //                 else {
        //                     MsgStream log(Athena::getMessageSvc(), "MuonCalibExtendedTrack");
        //                     log << MSG::WARNING << "STACO track without IDn" << endmsg;
        //                 }
        //             }
        //         }
        // }
        return false;
    }
}  // namespace MuonCalib
