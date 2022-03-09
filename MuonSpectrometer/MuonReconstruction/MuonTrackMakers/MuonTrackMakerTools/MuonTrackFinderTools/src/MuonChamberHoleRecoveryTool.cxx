/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonChamberHoleRecoveryTool.h"

#include <map>

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"
#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSurfaces/StraightLineSurface.h"

namespace {
    struct PullCluster {
        double pull{};
        std::unique_ptr<const Trk::TrackParameters> pars;
        std::unique_ptr<const Muon::MuonClusterOnTrack> clus;
    };
    typedef std::map<Identifier, PullCluster> ClusterLayerMap;

}  // namespace
namespace Muon {

    MuonChamberHoleRecoveryTool::MuonChamberHoleRecoveryTool(const std::string& ty, const std::string& na, const IInterface* pa) :
        AthAlgTool(ty, na, pa) {
        declareInterface<IMuonHoleRecoveryTool>(this);
    }

    StatusCode MuonChamberHoleRecoveryTool::initialize() {
        ATH_CHECK(m_DetectorManagerKey.initialize());
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_extrapolator.retrieve());
        ATH_CHECK(m_mdtRotCreator.retrieve());

        if (!m_cscRotCreator.empty()) {
            if (!m_idHelperSvc->recoCSC())
                ATH_MSG_WARNING("The current layout does not have any CSC chamber but you gave a CscRotCreator, ignoring it,"
                                << "  but double-check configuration");
            else
                ATH_CHECK(m_cscRotCreator.retrieve());
        } else {
            ATH_MSG_INFO("CSC rot creation disabled");
        }

        ATH_CHECK(m_clusRotCreator.retrieve());
        ATH_CHECK(m_mmClusRotCreator.retrieve());
        ATH_CHECK(m_pullCalculator.retrieve());
        ATH_CHECK(m_intersectSvc.retrieve());

        ATH_CHECK(m_key_mdt.initialize());
        /// Check that the layout has CSCs and that the key is actually set
        ATH_CHECK(m_key_csc.initialize(!m_key_csc.empty() && m_idHelperSvc->recoCSC()));
        ATH_CHECK(m_key_tgc.initialize());
        ATH_CHECK(m_key_rpc.initialize());
        /// Check that the layout has stgcs and that the key is set
        ATH_CHECK(m_key_stgc.initialize(!m_key_stgc.empty() && m_idHelperSvc->recosTgc()));
        /// Check that the layout has micromegas and that the key is set
        ATH_CHECK(m_key_mm.initialize(!m_key_mm.empty() && m_idHelperSvc->recoMM()));
        ATH_CHECK(m_condKey.initialize(!m_condKey.empty()));

        return StatusCode::SUCCESS;
    }
    std::unique_ptr<Trk::Track> MuonChamberHoleRecoveryTool::recover(const Trk::Track& track, const EventContext& ctx) const {
        std::set<MuonStationIndex::ChIndex> chamberLayersOnTrack;

        // loop over track and calculate residuals
        const Trk::TrackStates* trkstates = track.trackStateOnSurfaces();
        if (!trkstates) {
            ATH_MSG_DEBUG(" track without states, discarding track ");
            return nullptr;
        }
        ATH_MSG_DEBUG(" performing hole search track " << std::endl
                                                       << m_printer->print(track) << std::endl
                                                       << m_printer->printMeasurements(track));
        // vector to store states, the boolean indicated whether the state was created in this routine (true)
        // or belongs to the track (false).
        // If any new state is created, all states will be cloned and a new track will be formed from them.
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> newStates;
        newStates.reserve(trkstates->size() + 5);

        std::vector<const Trk::TrackStateOnSurface*> states;
        states.reserve(trkstates->size());
        states.insert(states.end(), trkstates->begin(), trkstates->end());

        std::set<MuonStationIndex::StIndex> stations;
        unsigned int nholes = 0;
        // loop over TSOSs
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit = states.begin();
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end = states.end();
        for (; tsit != tsit_end; ++tsit) {
            if (!*tsit) continue;  // sanity check

            // drop existing holes on track
            if ((*tsit)->type(Trk::TrackStateOnSurface::Hole)) {
                ATH_MSG_VERBOSE(" new TSOS is holes state, dropping ");
                ++nholes;
                continue;
            }

            // check whether state is a measurement
            const Trk::TrackParameters* pars = (*tsit)->trackParameters();
            if (!pars) {
                ATH_MSG_VERBOSE(" new TSOS without parameters, copying ");
                newStates.emplace_back((*tsit)->clone());
                continue;
            }

            // check whether state is a measurement
            const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
            if (!meas) {
                ATH_MSG_VERBOSE(" new TSOS without measurement, copying ");
                newStates.emplace_back((*tsit)->clone());
                continue;
            }

            Identifier id = m_edmHelperSvc->getIdentifier(*meas);

            // Not a ROT, else it would have had an identifier. Keep the TSOS.
            if (!id.is_valid() || !m_idHelperSvc->mdtIdHelper().is_muon(id)) {
                if (msgLvl(MSG::VERBOSE)) {
                    if (!id.is_valid())
                        ATH_MSG_VERBOSE(" new TSOS without valid identifier, copying ");
                    else if (!m_idHelperSvc->mdtIdHelper().is_muon(id))
                        ATH_MSG_VERBOSE(" new TSOS with none muon identifier, copying ");
                }
                newStates.emplace_back((*tsit)->clone());
                continue;
            }

            if (m_idHelperSvc->isMdt(id)) {
                // get station index
                Identifier chId = m_idHelperSvc->chamberId(id);
                MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chId);
                stations.insert(stIndex);

                // MDT case: Run hole search in chamber.
                tsit = insertMdtsWithHoleSearch(ctx, tsit, tsit_end, newStates, chamberLayersOnTrack);
                continue;
            } else if (m_idHelperSvc->isTrigger(id) || m_idHelperSvc->isCsc(id) || m_idHelperSvc->isMM(id) || m_idHelperSvc->issTgc(id)) {
                // Non-MDT case: Look for missing layers in chamber, add them to track
                tsit = insertClustersWithHoleSearch(ctx, tsit, tsit_end, newStates);
                continue;
            } else {
                ATH_MSG_WARNING(" unknown Identifier ");
            }
        }

        if (newStates.size() + nholes < states.size())
            ATH_MSG_DEBUG("Invalid number of states: collected " << newStates.size() << " holes " << nholes << " original "
                                                                 << states.size());
        // NOTE: no return ?

        ATH_MSG_DEBUG(" track has stations: " << stations.size() << "   original states " << states.size() << " new states "
                                              << newStates.size());
        // states were added, create a new track
        auto trackStateOnSurfaces = DataVector<const Trk::TrackStateOnSurface>();
        trackStateOnSurfaces.reserve(newStates.size());

        for (std::unique_ptr<const Trk::TrackStateOnSurface>& nit : newStates) { trackStateOnSurfaces.push_back(nit.release()); }
        std::unique_ptr<Trk::Track> newTrack = std::make_unique<Trk::Track>(track.info(), std::move(trackStateOnSurfaces),
                                                                            track.fitQuality() ? track.fitQuality()->clone() : nullptr);
        return newTrack;
    }

    std::vector<const Trk::TrackStateOnSurface*>::const_iterator MuonChamberHoleRecoveryTool::insertClustersWithHoleSearch(
        const EventContext& ctx, std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit,
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        // iterator should point to a valid element
        if (tsit == tsit_end) {
            ATH_MSG_WARNING(" iterator pointing to end of vector, this should no happen ");
            return --tsit;
        }

        // check whether state is a measurement
        const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
        const Trk::TrackParameters* pars = (*tsit)->trackParameters();
        if (!meas || !pars) {
            if (tsit + 1 == tsit_end) --tsit;
            return tsit;
        }

        // loop over states until we reached the last tgc hit in this detector element
        // keep trackof the identifiers and the states
        std::set<Identifier> layIds;
        std::unique_ptr<Identifier> detectorElId;  // pointer to detector element ID, created when finding the first TGC
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> newStates;
        for (; tsit != tsit_end; ++tsit) {
            if (!*tsit) continue;

            // skip existing holes as they will be recalculated
            if ((*tsit)->type(Trk::TrackStateOnSurface::Hole)) continue;

            // check whether state is a measurement, keep if not
            const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
            if (!meas) {
                newStates.emplace_back((*tsit)->clone());
                continue;
            }

            // get identifier, keep state if it has no identifier.
            Identifier id = m_edmHelperSvc->getIdentifier(*meas);
            if (!id.is_valid()) {
                newStates.emplace_back((*tsit)->clone());
                continue;
            }

            // get chamber Id
            Identifier detElId = m_idHelperSvc->detElId(id);
            if (!detectorElId) detectorElId = std::make_unique<Identifier>(detElId);

            // check whether we are still in the same chamber, stop loop if not
            if (detElId != *detectorElId) break;

            // sanity check, this SHOULD be a RPC, TGC, MM, STGC or CSC measurement
            if (!(m_idHelperSvc->isTrigger(id) || m_idHelperSvc->isCsc(id) || m_idHelperSvc->isMM(id) || m_idHelperSvc->issTgc(id))) {
                newStates.emplace_back((*tsit)->clone());
                ATH_MSG_WARNING(" Identifier should be TGC/RPC/CSC/MM/STGC: " << m_idHelperSvc->toString(id));
                continue;
            }

            // split competing ROTs into constituents
            const CompetingMuonClustersOnTrack* comp = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
            if (comp) {
                std::vector<const MuonClusterOnTrack*>::const_iterator cl_it = comp->containedROTs().begin();
                std::vector<const MuonClusterOnTrack*>::const_iterator cl_it_end = comp->containedROTs().end();
                for (; cl_it != cl_it_end; ++cl_it) {
                    // get layer Identifier and insert it into set
                    Identifier layId = m_idHelperSvc->layerId((*cl_it)->identify());
                    layIds.insert(layId);
                }
            } else {
                // get layer Identifier and insert it into set
                Identifier layId = m_idHelperSvc->layerId(id);
                layIds.insert(layId);
            }
            if (msgLvl(MSG::VERBOSE)) {
                msg(MSG::VERBOSE) << " new hit " << m_idHelperSvc->toString(id);
                if (comp) msg(MSG::VERBOSE) << " competing, layers: " << layIds.size();
                msg(MSG::VERBOSE) << endmsg;
            }
            newStates.emplace_back((*tsit)->clone());
        }

        // create holes
        if (detectorElId) {
            createHoleTSOSsForClusterChamber(*detectorElId, ctx, *pars, layIds, newStates);
        } else {
            ATH_MSG_WARNING(" no chamber identifier set, this should not happen ");
        }

        // sort all states in this chamber
        std::stable_sort(newStates.begin(), newStates.end(), SortTSOSByDistanceToPars(pars));

        // insert the states into
        for (auto& state : newStates) { states.emplace_back(std::move(state)); }
        // iterator should point to the last TGC in this chamber
        return --tsit;
    }

    std::vector<const Trk::TrackStateOnSurface*>::const_iterator MuonChamberHoleRecoveryTool::insertMdtsWithHoleSearch(
        const EventContext& ctx, std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit,
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states,
        std::set<MuonStationIndex::ChIndex> chamberLayersOnTrack) const {
        // iterator should point to a valid element
        if (tsit == tsit_end) {
            ATH_MSG_WARNING(" iterator pointing to end of vector, this should no happen ");
            return --tsit;
        }

        // check whether state is a measurement
        const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
        const Trk::TrackParameters* pars = (*tsit)->trackParameters();
        if (!meas || !pars) {
            if (tsit + 1 == tsit_end) --tsit;
            return tsit;
        }

        // this should be a MDT
        const MdtDriftCircleOnTrack* mdtFirst = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
        if (!mdtFirst) {
            ATH_MSG_WARNING("Bad hit: not a MDT " << m_idHelperSvc->toString(m_edmHelperSvc->getIdentifier(*meas)));
            if (tsit + 1 == tsit_end) --tsit;
            return tsit;
        }

        // get chamber identifier
        MuonStationIndex::ChIndex currentChIndex = m_idHelperSvc->chamberIndex(mdtFirst->identify());
        Identifier chId = m_idHelperSvc->chamberId(mdtFirst->identify());

        // to correctly handle low pt tracks average the direction of the track in the chamber
        const Trk::TrackParameters* parsLast = nullptr;

        // loop over states until we reached the last mdt hit in this chamber
        // keep trackof the identifiers and the states
        std::set<Identifier> ids;
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> newStates;
        for (; tsit != tsit_end; ++tsit) {
            if (!*tsit) continue;

            // skip existing holes as they will be recalculated
            if ((*tsit)->type(Trk::TrackStateOnSurface::Hole)) continue;

            // check whether state is a measurement
            const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
            if (!meas) {
                newStates.emplace_back((*tsit)->clone());
                continue;
            }

            // don't break on pseudo measurements
            const Trk::PseudoMeasurementOnTrack* psmeas = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas);
            if (psmeas) {
                newStates.emplace_back((*tsit)->clone());
                continue;
            }

            // only use MDTs
            const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
            if (!mdt) break;

            const Identifier& id = mdt->identify();

            // check whether we are still in the same chamber layer
            if (currentChIndex != m_idHelperSvc->chamberIndex(id)) break;

            ids.insert(id);  // insert identifier
            newStates.emplace_back((*tsit)->clone());

            // get pointer to last parameters
            parsLast = (*tsit)->trackParameters();
        }

        // check if chamber index is already processed
        bool doHoleSearch = true;
        if (chamberLayersOnTrack.count(currentChIndex)) {
            if (m_detectBadSort)
                ATH_MSG_WARNING("Detected badly sorted track, not adding holes in current chamber");
            else
                ATH_MSG_DEBUG("Detected badly sorted track, not adding holes in current chamber");
            doHoleSearch = false;
        } else {
            chamberLayersOnTrack.insert(currentChIndex);
        }

        if (doHoleSearch) {
            // ensure that we are not passing in the same parameters
            if (parsLast == pars) parsLast = nullptr;

            // create holes
            createHoleTSOSsForMdtChamber(chId, ctx, *pars, parsLast, ids, newStates);

            ATH_MSG_DEBUG(" Chamber " << m_idHelperSvc->toStringChamber(chId) << " has hits " << ids.size() << " new states "
                                      << newStates.size() - ids.size());

            // sort all states in this chamber
            std::stable_sort(newStates.begin(), newStates.end(), SortTSOSByDistanceToPars(pars));
        }

        // insert the states into
        for (auto& state : newStates) { states.emplace_back(std::move(state)); }
        // iterator should point to the last MDT in this chamber
        return --tsit;
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForClusterChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        ATH_MSG_VERBOSE(" performing holes search in chamber " << m_idHelperSvc->toString(detElId));

        if (m_idHelperSvc->isCsc(detElId))
            createHoleTSOSsForCscChamber(detElId, ctx, pars, layIds, states);
        else if (m_idHelperSvc->isTgc(detElId))
            createHoleTSOSsForTgcChamber(detElId, ctx, pars, layIds, states);
        else if (m_idHelperSvc->isRpc(detElId))
            createHoleTSOSsForRpcChamber(detElId, ctx, pars, layIds, states);
        else if (m_idHelperSvc->isMM(detElId))
            createHoleTSOSsForMmChamber(detElId, ctx, pars, layIds, states);
        else if (m_idHelperSvc->issTgc(detElId))
            createHoleTSOSsForStgcChamber(detElId, ctx, pars, layIds, states);
        else
            ATH_MSG_WARNING(" unexpected Identifier, cannot perform holes search");
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForStgcChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        // get detector element
        const MuonGM::sTgcReadoutElement* detEl = dynamic_cast<const MuonGM::sTgcReadoutElement*>(getDetectorElement(detElId, ctx));
        if (!detEl) {
            ATH_MSG_WARNING(" No detector element found for " << m_idHelperSvc->toStringChamber(detElId));
            return;
        }

        // get list of layers with a hole
        unsigned int nGasGaps = detEl->numberOfLayers(false);
        int multilayer = m_idHelperSvc->stgcIdHelper().multilayer(detElId);
        LayerHoleVec holeVec = holesInClusterChamber(pars, detElId, layIds, nGasGaps);
        if (holeVec.empty()) return;

        // create identifiers from LayerHoleVec
        std::set<Identifier> chHoles;
        LayerHoleVec::iterator it = holeVec.begin();
        LayerHoleVec::iterator it_end = holeVec.end();

        Identifier layId = m_idHelperSvc->stgcIdHelper().channelID(detElId, multilayer, it->first, 1, 1);
        bool measuresPhi1 = m_idHelperSvc->measuresPhi(layId);
        layId = m_idHelperSvc->stgcIdHelper().channelID(detElId, multilayer, it->first, 0, 1);
        bool measuresPhi0 = m_idHelperSvc->measuresPhi(layId);
        layId = m_idHelperSvc->stgcIdHelper().channelID(detElId, multilayer, it->first, 2, 1);
        bool measuresPhi2 = m_idHelperSvc->measuresPhi(layId);

        ATH_MSG_DEBUG(" createHoleTSOSsFor StgcChamber measuresPhi type 0"
                      << (int)measuresPhi0 << " measuresPhi type 1 " << (int)measuresPhi1 << " measuresPhi type 2 " << (int)measuresPhi2);
        for (; it != it_end; ++it) {
            // the second gas gap of the three layer stations does not have a phi measurement
            // type = 0  1 and 2  pad/eta/wires
            // Identifier id = m_idHelper->channelID(parentId, m_idHelper->multilayer(layid), m_idHelper->gasGap(layid),type,1,m_checkIds);
            if (it->second == 0) {
                // eta strip
                chHoles.insert(m_idHelperSvc->stgcIdHelper().channelID(detElId, multilayer, it->first, 1, 1));
            } else {
                if (abs(m_idHelperSvc->stationEta(detElId)) > 2) {
                    // wires
                    chHoles.insert(m_idHelperSvc->stgcIdHelper().channelID(detElId, multilayer, it->first, 2, 1));
                } else {
                    // pads
                    chHoles.insert(m_idHelperSvc->stgcIdHelper().channelID(detElId, multilayer, it->first, 0, 1));
                }
            }
        }

        if (chHoles.empty()) return;

        // loop over Tgc Prds and try to find prds of tubes with holes
        const sTgcPrepDataCollection* prdCol = findStgcPrdCollection(detElId, ctx);
        if (!prdCol) {
            ATH_MSG_DEBUG(" Retrieval of sTgcPrepDataCollection failed!! ");
            return;
        }

        // copy TGC prds into generic MuonCluster vector
        std::vector<const MuonCluster*> prds;
        prds.reserve(prdCol->size());
        sTgcPrepDataCollection::const_iterator hit = prdCol->begin();
        sTgcPrepDataCollection::const_iterator hit_end = prdCol->end();
        for (; hit != hit_end; ++hit) {
            if (detElId == m_idHelperSvc->detElId((*hit)->identify())) prds.push_back(*hit);
        }

        createHoleTSOSsForClusterChamber(detElId, ctx, pars, layIds, chHoles, prds, states);

        return;
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForMmChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        // get detector element
        const MuonGM::MMReadoutElement* detEl = dynamic_cast<const MuonGM::MMReadoutElement*>(getDetectorElement(detElId, ctx));
        if (!detEl) {
            ATH_MSG_WARNING(" No detector element found for " << m_idHelperSvc->toStringChamber(detElId));
            return;
        }

        // get list of layers with a hole
        unsigned int nGasGaps = detEl->numberOfLayers(false);
        int multilayer = m_idHelperSvc->mmIdHelper().multilayer(detElId);
        LayerHoleVec holeVec = holesInClusterChamber(pars, detElId, layIds, nGasGaps);
        if (holeVec.empty()) return;

        // create identifiers from LayerHoleVec
        std::set<Identifier> chHoles;
        LayerHoleVec::iterator it = holeVec.begin();
        LayerHoleVec::iterator it_end = holeVec.end();
        for (; it != it_end; ++it) {
            // eta strip
            if (it->second == 0) chHoles.insert(m_idHelperSvc->mmIdHelper().channelID(detElId, multilayer, it->first, 1));
        }

        if (chHoles.empty()) return;

        // loop over MM Prds and try to find prds of tubes with holes
        const MMPrepDataCollection* prdCol = findMmPrdCollection(detElId, ctx);
        if (!prdCol) {
            ATH_MSG_DEBUG(" Retrieval of MMPrepDataCollection failed!! ");
            return;
        }

        // copy MM prds into generic MuonCluster vector
        std::vector<const MuonCluster*> prds;
        prds.reserve(prdCol->size());
        MMPrepDataCollection::const_iterator hit = prdCol->begin();
        MMPrepDataCollection::const_iterator hit_end = prdCol->end();
        for (; hit != hit_end; ++hit) {
            if (detElId == m_idHelperSvc->detElId((*hit)->identify())) prds.push_back(*hit);
        }

        createHoleTSOSsForClusterChamber(detElId, ctx, pars, layIds, chHoles, prds, states);

        return;
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForCscChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        // get list of layers with a hole
        unsigned int nGasGaps = 4;
        LayerHoleVec holeVec = holesInClusterChamber(pars, detElId, layIds, nGasGaps);
        if (holeVec.empty()) return;

        // create identifiers from LayerHoleVec
        std::set<Identifier> chHoles;
        LayerHoleVec::iterator it = holeVec.begin();
        LayerHoleVec::iterator it_end = holeVec.end();
        for (; it != it_end; ++it) {
            Identifier parentId = m_idHelperSvc->cscIdHelper().parentID(detElId);
            Identifier layId = m_idHelperSvc->cscIdHelper().channelID(parentId, 2, it->first, it->second, 1);
            bool measuresPhi = m_idHelperSvc->measuresPhi(layId);
            // sanity check this should always be true!!!
            if (measuresPhi != (bool)it->second) {
                ATH_MSG_WARNING(" Problem spotted creating CSC identifier, measures Phi not correctly set ");
                continue;
            }
            ATH_MSG_VERBOSE(" adding hole layer " << m_idHelperSvc->toString(layId) << "     layer " << it->first << " measPhi "
                                                  << it->second);
            chHoles.insert(layId);
        }

        if (chHoles.empty()) return;

        // loop over CSC Prds and try to find prds of tubes with holes
        const CscPrepDataCollection* prdCol = findCscPrdCollection(detElId, ctx);
        if (!prdCol) {
            ATH_MSG_DEBUG(" Retrieval of CscPrepDataCollection failed!! ");
            // return;
        }

        // copy CSC prds into generic MuonCluster vector
        std::vector<const MuonCluster*> prds;
        if (prdCol) {
            prds.reserve(prdCol->size());
            prds.insert(prds.begin(), prdCol->begin(), prdCol->end());
        }
        createHoleTSOSsForClusterChamber(detElId, ctx, pars, layIds, chHoles, prds, states);
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForTgcChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        // get detector element
        const MuonGM::TgcReadoutElement* detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(getDetectorElement(detElId, ctx));
        if (!detEl) {
            ATH_MSG_WARNING(" No detector element found for " << m_idHelperSvc->toStringChamber(detElId));
            return;
        }

        // get list of layers with a hole
        unsigned int nGasGaps = detEl->Ngasgaps();
        LayerHoleVec holeVec = holesInClusterChamber(pars, detElId, layIds, nGasGaps);
        if (holeVec.empty()) return;

        // create identifiers from LayerHoleVec
        std::set<Identifier> chHoles;
        LayerHoleVec::iterator it = holeVec.begin();
        LayerHoleVec::iterator it_end = holeVec.end();
        for (; it != it_end; ++it) {
            // the second gas gap of the three layer stations does not have a phi measurement
            if (nGasGaps == 3 && it->first == 2 && it->second) continue;

            chHoles.insert(m_idHelperSvc->tgcIdHelper().channelID(detElId, it->first, it->second, 1));
        }

        if (chHoles.empty()) return;

        // loop over Tgc Prds and try to find prds of tubes with holes
        const TgcPrepDataCollection* prdCol = findTgcPrdCollection(detElId, ctx);
        if (!prdCol) {
            ATH_MSG_DEBUG(" Retrieval of TgcPrepDataCollection failed!! ");
            return;
        }

        // copy TGC prds into generic MuonCluster vector
        std::vector<const MuonCluster*> prds;
        prds.reserve(prdCol->size());
        TgcPrepDataCollection::const_iterator hit = prdCol->begin();
        TgcPrepDataCollection::const_iterator hit_end = prdCol->end();
        for (; hit != hit_end; ++hit) {
            if (detElId == m_idHelperSvc->detElId((*hit)->identify())) prds.push_back(*hit);
        }
        // prds.insert(prds.begin(),prdCol->begin(),prdCol->end());

        createHoleTSOSsForClusterChamber(detElId, ctx, pars, layIds, chHoles, prds, states);
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForRpcChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        // get list of layers with a hole
        unsigned int nGasGaps = 2;
        LayerHoleVec holeVec = holesInClusterChamber(pars, detElId, layIds, nGasGaps);
        if (holeVec.empty()) return;

        const MuonGM::RpcReadoutElement* detEl = dynamic_cast<const MuonGM::RpcReadoutElement*>(getDetectorElement(detElId, ctx));
        if (!detEl) {
            ATH_MSG_WARNING("Could not find ReadoutElement for " << m_idHelperSvc->toString(detElId));
            return;
        }
        int dbZ = detEl->getDoubletZ();
        int dbPhi = detEl->getDoubletPhi();

        ATH_MSG_DEBUG("Looking up RPC structure: dbZ " << dbZ << " dbPhi " << dbPhi << " nphi gp " << detEl->Nphigasgaps());

        // create identifiers from LayerHoleVec
        std::set<Identifier> chHoles;
        LayerHoleVec::iterator it = holeVec.begin();
        LayerHoleVec::iterator it_end = holeVec.end();
        for (; it != it_end; ++it) {
            Identifier clusId = m_idHelperSvc->rpcIdHelper().channelID(detElId, dbZ, dbPhi, it->first, it->second, 1);
            chHoles.insert(clusId);

            ATH_MSG_VERBOSE("Adding hole " << m_idHelperSvc->toString(clusId));
        }

        // loop over Rpc Prds and try to find prds of tubes with holes
        const RpcPrepDataCollection* prdCol = findRpcPrdCollection(detElId, ctx);
        if (!prdCol) {
            ATH_MSG_DEBUG(" Retrieval of RpcPrepDataCollection failed!! ");
            return;
        }

        // copy RPC prds into generic MuonCluster vector
        std::vector<const MuonCluster*> prds;
        prds.reserve(prdCol->size());
        RpcPrepDataCollection::const_iterator hit = prdCol->begin();
        RpcPrepDataCollection::const_iterator hit_end = prdCol->end();
        for (; hit != hit_end; ++hit) {
            if (detElId == m_idHelperSvc->detElId((*hit)->identify())) prds.push_back(*hit);
        }
        // prds.insert(prds.begin(),prdCol->begin(),prdCol->end());

        createHoleTSOSsForClusterChamber(detElId, ctx, pars, layIds, chHoles, prds, states);
    }
    const Trk::TrkDetElementBase* MuonChamberHoleRecoveryTool::getDetectorElement(const Identifier& detElId,
                                                                                  const EventContext& ctx) const {
        SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey, ctx};
        const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle};
        if (!MuonDetMgr) {
            ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
            return nullptr;
        }
        if (m_idHelperSvc->isMdt(detElId))
            return MuonDetMgr->getMdtReadoutElement(detElId);
        else if (m_idHelperSvc->isTgc(detElId))
            return MuonDetMgr->getTgcReadoutElement(detElId);
        else if (m_idHelperSvc->isRpc(detElId))
            return MuonDetMgr->getRpcReadoutElement(detElId);
        else if (m_idHelperSvc->isCsc(detElId))
            return MuonDetMgr->getCscReadoutElement(detElId);
        // New Small Wheel
        else if (m_idHelperSvc->issTgc(detElId))
            return MuonDetMgr->getsTgcReadoutElement(detElId);
        else if (m_idHelperSvc->isMM(detElId))
            return MuonDetMgr->getMMReadoutElement(detElId);
        return nullptr;
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForClusterChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::set<Identifier>& chHoles, const std::vector<const MuonCluster*>& prds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        if (msgLvl(MSG::DEBUG)) {
            msg() << " chamber " << m_idHelperSvc->toString(detElId) << " PRDs in chamber  " << prds.size() << "  holes " << chHoles.size();
            if (msgLvl(MSG::VERBOSE)) {
                if (pars.covariance()) msg() << MSG::VERBOSE << " extrapolation error " << Amg::error(*pars.covariance(), Trk::locX);
            }
            msg() << endmsg;
        }

        ClusterLayerMap cluster_layer_map;

        unsigned int nNewHits = 0;
        // loop over prds
        for (const MuonCluster* clus : prds) {
            // get Identifier, check whether we have a hole in that layer. If not go to next cluster
            const Identifier& id = clus->identify();
            Identifier layId = m_idHelperSvc->layerId(id);

            // do not add hits in layers that are already on track
            if (layIds.count(layId)) { continue; }
            const Trk::Surface& surf = clus->detectorElement()->surface(id);

            std::unique_ptr<const Trk::TrackParameters> exPars;
            if (pars.associatedSurface() == surf) {
                ATH_MSG_VERBOSE(" Same surface, cloning parameters ");
                exPars = pars.uniqueClone();
            } else {
                exPars = m_extrapolator->extrapolateDirectly(ctx, pars, surf, Trk::anyDirection, false, Trk::muon);
                if (!exPars) {
                    ATH_MSG_WARNING(" Propagation cluster PRD failed!! ");
                    continue;
                }
            }
            ATH_MSG_VERBOSE("  --- " << m_idHelperSvc->toString(id) << " error " << Amg::error(clus->localCovariance(), Trk::locX)
                                     << " measured parameters, error " << Amg::error(*exPars->covariance(), Trk::locX));

            std::unique_ptr<const MuonClusterOnTrack> clusterOnTrack;
            if (m_idHelperSvc->isTrigger(clus->identify()) || m_idHelperSvc->issTgc(clus->identify())) {
                clusterOnTrack.reset(m_clusRotCreator->createRIO_OnTrack(*clus, exPars->position(), exPars->momentum().unit()));
            } else if (m_idHelperSvc->isMM(clus->identify())) {
                clusterOnTrack.reset(m_mmClusRotCreator->createRIO_OnTrack(*clus, exPars->position(), exPars->momentum().unit()));
            } else if (!m_cscRotCreator.empty()) {
                clusterOnTrack.reset(m_cscRotCreator->createRIO_OnTrack(*clus, exPars->position(), exPars->momentum().unit()));
            }
            if (!clusterOnTrack) { continue; }

            std::unique_ptr<const Trk::ResidualPull> resPull{
                m_pullCalculator->residualPull(clusterOnTrack.get(), exPars.get(), Trk::ResidualPull::Unbiased)};
            if (!resPull) { continue; }
            if (resPull->pull().empty()) { continue; }
            const double pullCut = m_idHelperSvc->measuresPhi(id) ? m_associationPullCutPhi : m_associationPullCutEta;
            double pull = std::abs(resPull->pull().front());
            Amg::Vector2D locExPos(exPars->parameters()[Trk::locX], exPars->parameters()[Trk::locY]);

            bool inbounds(false);
            if (m_idHelperSvc->isMM(id)) {
                inbounds = ((MuonGM::MMReadoutElement*)clus->detectorElement())->insideActiveBounds(id, locExPos, 10., 10.);
            } else {
                inbounds = surf.insideBounds(locExPos, 10., 10.);
            }

            ATH_MSG_VERBOSE(" found prd: " << m_idHelperSvc->toString(id) << " res " << resPull->residual().front() << " pull " << pull
                                           << (inbounds ? " inside bounds " : " outside bounds "));

            // check whether hit within cut and in bounds
            if (pull < pullCut && inbounds) {
                // check whether there are other hits in the same layer with smaller residual, if not add
                ClusterLayerMap::iterator cl_it = cluster_layer_map.find(layId);
                if (cl_it != cluster_layer_map.end()) {
                    if (pull < cl_it->second.pull) {
                        cl_it->second.pull = pull;
                        cl_it->second.pars = std::move(exPars);
                        cl_it->second.clus = std::move(clusterOnTrack);
                    }
                } else {
                    PullCluster pullClus;
                    pullClus.pull = pull;
                    pullClus.pars = std::move(exPars);
                    pullClus.clus = std::move(clusterOnTrack);
                    cluster_layer_map[layId] = std::move(pullClus);
                }
            }
        }

        // loop over cluster layer map and add the clusters to the state vector
        for (auto& cl_it : cluster_layer_map) {
            ATH_MSG_VERBOSE(" added hit " << m_idHelperSvc->toString(cl_it.second.clus->identify()));
            Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOS(std::move(cl_it.second.clus), std::move(cl_it.second.pars),
                                                                            Trk::TrackStateOnSurface::Measurement);
            states.emplace_back(tsos);
            ++nNewHits;
        }

        unsigned int nholes = 0;
        for (const Identifier& id : chHoles) {
            if (cluster_layer_map.count(id)) continue;
            // if no detEl yet try retrieving if from the detMgr
            const Trk::TrkDetElementBase* detEl = getDetectorElement(id, ctx);

            if (!detEl) {
                ATH_MSG_DEBUG("No detector element found for " << m_idHelperSvc->toString(id));
                continue;
            }

            const Trk::Surface& surf = detEl->surface(id);

            std::unique_ptr<const Trk::TrackParameters> exPars;
            if (pars.associatedSurface() == surf) {
                ATH_MSG_DEBUG(" Same surface, cloning parameters ");
                exPars = pars.uniqueClone();
            } else {
                exPars = m_extrapolator->extrapolateDirectly(ctx, pars, surf, Trk::anyDirection, false, Trk::muon);
                if (!exPars) {
                    ATH_MSG_DEBUG(" Propagation cluster hole failed!! ");
                    continue;
                }
            }

            bool inBounds = false;
            Amg::Vector2D locPos;

            if (surf.globalToLocal(exPars->position(), exPars->momentum(), locPos)) {
                if (m_idHelperSvc->isMM(detElId)) {
                    inBounds = ((const MuonGM::MMReadoutElement*)detEl)->insideActiveBounds(id, locPos, -100., 100.);
                } else {
                    inBounds = surf.insideBounds(locPos, -100., -100.);
                }
            }

            ATH_MSG_VERBOSE(" new hole " << m_idHelperSvc->toString(id) << " position " << exPars->parameters()[Trk::locR]
                                         << (inBounds ? " inside bounds " : " outside bounds "));

            if (!inBounds) { continue; }

            if (m_idHelperSvc->issTgc(id)) ATH_MSG_VERBOSE(" new hole sTgc measuresPhi " << (int)m_idHelperSvc->measuresPhi(id));
            Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createHoleTSOS(std::move(exPars));
            states.emplace_back(tsos);
            ++nholes;
            // break; // only add one hole per det el
        }

        ATH_MSG_DEBUG(" chamber " << m_idHelperSvc->toStringChamber(detElId) << " added hits " << nNewHits << " found holes " << nholes);
    }

    void MuonChamberHoleRecoveryTool::createHoleTSOSsForMdtChamber(
        const Identifier& chId, const EventContext& ctx, const Trk::TrackParameters& pars, const Trk::TrackParameters* parsLast,
        std::set<Identifier>& ids, std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey, ctx};
        const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle};
        if (!MuonDetMgr) {
            ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
            return;
        }

        Amg::Vector3D direction = pars.momentum().unit();

        // check whether we need to average the parameters
        if (parsLast) direction = (parsLast->position() - pars.position()).unit();
        std::set<Identifier> chHoles = holesInMdtChamber(pars.position(), direction, chId, ctx, ids);
        ATH_MSG_VERBOSE(" chamber " << m_idHelperSvc->toStringChamber(chId) << " has holes " << chHoles.size());
        if (chHoles.empty()) return;

        unsigned int nholes = 0;
        unsigned int nstates = 0;

        // set to store chambers that were already handled
        std::set<Identifier> chIds;

        // loop over holes, find prd data collection belonging to hole, recover hole
        for (const Identifier& hit : chHoles) {
            // only handle chamers once
            Identifier chIdentifier = m_idHelperSvc->chamberId(hit);
            if (chIds.count(chIdentifier)) continue;
            chIds.insert(chIdentifier);

            // loop over Mdt Prds and try to find prds of tubes with holes
            const MdtPrepDataCollection* mdtPrdCol = findMdtPrdCollection(chIdentifier, ctx);
            if (!mdtPrdCol) {
                ATH_MSG_DEBUG(" Retrieval of MdtPrepDataCollection failed!! ");
                break;
            }

            std::set<Identifier> associatedHoles;  // holes in tubes that also have a prd
            for (const MdtPrepData* mdtPrd : *mdtPrdCol) {
                const Identifier& id = mdtPrd->identify();

                // only consider holes
                if (!chHoles.count(id)) continue;

                // ensure that Identifiers are not added twice
                if (associatedHoles.count(id)) continue;

                const Trk::Surface& surf = mdtPrd->detectorElement()->surface(id);
                std::unique_ptr<const Trk::TrackParameters> exPars;
                if (pars.associatedSurface() == surf) {
                    exPars = pars.uniqueClone();
                } else {
                    exPars = m_extrapolator->extrapolateDirectly(ctx, pars, surf, Trk::anyDirection, false, Trk::muon);
                    if (!exPars) {
                        ATH_MSG_WARNING(" Propagation to MDT prd failed!! ");
                        continue;
                    }
                }

                Amg::Vector3D momentum = exPars->momentum();

                // calibrate Mdt PRD
                std::unique_ptr<MdtDriftCircleOnTrack> mdtROT{m_mdtRotCreator->createRIO_OnTrack(*mdtPrd, exPars->position(), &momentum)};
                // sanity check
                if (!mdtROT) {
                    ATH_MSG_DEBUG(" failed to calibrate MdtPrepData " << m_idHelperSvc->toString(id));
                    continue;
                }

                const Trk::StraightLineSurface* slSurf = dynamic_cast<const Trk::StraightLineSurface*>(&mdtROT->associatedSurface());
                if (!slSurf) {
                    ATH_MSG_WARNING(" dynamic cast to StraightLineSurface failed for mdt!!! ");
                } else {
                    // calculate Amg::Vector2D using surf to obtain sign
                    Amg::Vector2D locPos;
                    if (!slSurf->globalToLocal(exPars->position(), exPars->momentum(), locPos)) {
                        ATH_MSG_DEBUG(" failed to calculate drift sign ");
                    } else {
                        // calculate side
                        Trk::DriftCircleSide side = locPos[Trk::driftRadius] < 0 ? Trk::LEFT : Trk::RIGHT;

                        // update sign
                        m_mdtRotCreator->updateSign(*mdtROT, side);
                    }
                }

                // check whether MDT ROT has sagged wire surface, if so redo propagation
                if (slSurf != &mdtPrd->detectorElement()->surface(id)) {
                    exPars = m_extrapolator->extrapolateDirectly(ctx, pars, *slSurf, Trk::anyDirection, false, Trk::muon);
                    if (!exPars) {
                        ATH_MSG_WARNING(" Propagation to sagged surface failed!! ");
                        continue;
                    }
                }

                if (msgLvl(MSG::VERBOSE)) {
                    DistanceToPars distToPars(&pars);
                    double distance = distToPars(exPars->position());
                    msg(MSG::VERBOSE) << std::setprecision(4) << " radius  " << std::setw(7) << mdtROT->localParameters()[Trk::locR]
                                      << " exPos " << std::setw(7) << exPars->parameters()[Trk::locR] << " distToPar " << distance << " ";
                }

                // pointer to resPull
                std::unique_ptr<const Trk::ResidualPull> resPull{
                    m_pullCalculator->residualPull(mdtROT.get(), exPars.get(), Trk::ResidualPull::Unbiased)};
                if (!resPull) { continue; }

                double pull = resPull->pull().front();
                double radialResidual = std::abs(mdtROT->localParameters()[Trk::locR]) - std::abs(exPars->parameters()[Trk::locR]);

                unsigned int hitFlag = 1;
                if (mdtPrd->adc() < m_adcCut || mdtPrd->status() != MdtStatusDriftTime)
                    hitFlag = 3;  // noise
                else if (fabs(pull) < m_associationPullCutEta)
                    hitFlag = 0;  // hit on track
                else if (radialResidual > 0.)
                    hitFlag = 2;  // out of time

                bool inBounds = false;
                // calculate Amg::Vector2D using surf to obtain sign
                Amg::Vector2D locPos;
                if (mdtROT->associatedSurface().globalToLocal(exPars->position(), exPars->momentum(), locPos)) {
                    inBounds = mdtROT->associatedSurface().insideBounds(locPos);
                }
                if (msgLvl(MSG::VERBOSE)) {
                    std::string idString = m_idHelperSvc->toString(id);
                    std::string boundCheck = inBounds ? "inBounds" : "outBounds";
                    msg(MSG::VERBOSE) << m_printer->print(*resPull) << " " << idString << " " << boundCheck;
                    std::string hitType = " delta";
                    if (hitFlag == 0)
                        hitType = " ROT";
                    else if (hitFlag == 2)
                        hitType = " Out of Time";
                    else if (hitFlag == 3)
                        hitType = " Noise";
                    msg(MSG::VERBOSE) << hitType << endmsg;
                }

                // if we get here we are sure that this is not a hole
                associatedHoles.insert(id);

                if (!inBounds) { continue; }

                Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOS(
                    std::move(mdtROT), std::move(exPars),
                    (hitFlag != 0 || !m_addMeasurements) ? Trk::TrackStateOnSurface::Outlier : Trk::TrackStateOnSurface::Measurement);
                states.emplace_back(tsos);
                ++nstates;
                if (nstates == chHoles.size()) {
                    ATH_MSG_DEBUG(" recovered Mdts " << nstates << " all holes recovered " << nholes);
                    return;
                }
            }

            for (const Identifier& hit : chHoles) {
                if (associatedHoles.count(hit)) continue;
                // only create holes in the current chamber
                const Identifier ch = m_idHelperSvc->chamberId(hit);
                if (ch != chIdentifier) continue;

                const MuonGM::MdtReadoutElement* detEl = dynamic_cast<const MuonGM::MdtReadoutElement*>(getDetectorElement(hit, ctx));
                if (!detEl) {
                    ATH_MSG_WARNING("Could not pipe " << m_idHelperSvc->toString(hit) << " to a MDT element.");
                    continue;
                }
                const Trk::Surface& surf = detEl->surface(hit);

                std::unique_ptr<const Trk::TrackParameters> exPars;
                if (pars.associatedSurface() == surf) {
                    exPars = pars.uniqueClone();
                } else {
                    exPars = m_extrapolator->extrapolateDirectly(ctx, pars, surf, Trk::anyDirection, false, Trk::muon);
                    if (!exPars) {
                        ATH_MSG_WARNING(" Propagation to MDT hole failed!! ");
                        continue;
                    }
                }

                bool inBounds = false;
                Amg::Vector2D locPos;
                if (surf.globalToLocal(exPars->position(), exPars->momentum(), locPos)) {
                    // perform bound check do not count holes with 100. mm of bound edge
                    inBounds = surf.bounds().insideLoc2(locPos, -100.) && std::abs(locPos[Trk::locR]) <= detEl->innerTubeRadius();
                }
                if (!inBounds) {
                    ATH_MSG_VERBOSE(" discarding hole " << m_idHelperSvc->toString(hit) << " dist wire " << exPars->parameters()[Trk::locR]
                                                        << " outside bounds ");
                    continue;
                }
                ATH_MSG_VERBOSE(" new hole " << m_idHelperSvc->toString(hit) << " dist wire " << exPars->parameters()[Trk::locR]);
                Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createHoleTSOS(std::move(exPars));
                states.emplace_back(tsos);
                ++nholes;
                ++nstates;
            }
        }
        ATH_MSG_DEBUG(" new states " << nstates << " number of holes " << nholes);
    }

    MuonChamberHoleRecoveryTool::LayerHoleVec MuonChamberHoleRecoveryTool::holesInClusterChamber(const Trk::TrackParameters& /*pars*/,
                                                                                                 const Identifier& /*detElId*/,
                                                                                                 const std::set<Identifier>& layIds,
                                                                                                 unsigned nGasGaps) const {
        LayerHoleVec holes;

        // if the number of layers is equal to the number of gas gaps times two in the chamber, there are no holes
        if (layIds.size() == 2 * nGasGaps) return holes;

        // create layer matrix
        using LayerMatrix = std::vector<std::pair<int, int>>;
        LayerMatrix layerMatrix(nGasGaps, std::make_pair(0, 0));

        // loop over layer identifiers and fill
        std::set<Identifier>::const_iterator it = layIds.begin();
        std::set<Identifier>::const_iterator it_end = layIds.end();
        for (; it != it_end; ++it) {
            int gasGap = m_idHelperSvc->gasGap(*it);
            bool measuresPhi = m_idHelperSvc->measuresPhi(*it);
            // sanity check
            if (gasGap < 0 && gasGap >= (int)nGasGaps) {
                ATH_MSG_WARNING(" gas gap index out of range " << m_idHelperSvc->toString(*it));
                continue;
            }

            // counter starts at 1!
            if (measuresPhi)
                layerMatrix[gasGap - 1].second = 1;
            else
                layerMatrix[gasGap - 1].first = 1;
        }

        // loop over the matrix, if a field is zero we have a hole.
        unsigned int gasGap = 1;
        LayerMatrix::iterator lit = layerMatrix.begin();
        LayerMatrix::iterator lit_end = layerMatrix.end();
        for (; lit != lit_end; ++lit) {
            // check whether the eta or phi hit is missing, create a hole if that is the case
            if (!lit->first) holes.push_back(std::make_pair(gasGap, 0));
            if (!lit->second) holes.push_back(std::make_pair(gasGap, 1));

            ++gasGap;
        }
        return holes;
    }

    std::set<Identifier> MuonChamberHoleRecoveryTool::holesInMdtChamber(const Amg::Vector3D& position, const Amg::Vector3D& direction,
                                                                        const Identifier& chId, const EventContext& ctx,
                                                                        const std::set<Identifier>& tubeIds) const {
        // calculate crossed tubes
        const MdtCondDbData* dbData = nullptr;
        if (!m_condKey.empty()) {
            SG::ReadCondHandle<MdtCondDbData> readHandle{m_condKey, ctx};
            dbData = readHandle.cptr();
        }

        SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey, ctx};
        const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle};
        if (!MuonDetMgr) {
            ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
            // return;
        }

        const MuonStationIntersect intersect = m_intersectSvc->tubesCrossedByTrack(chId, position, direction, dbData, MuonDetMgr);

        // clear hole vector
        std::set<Identifier> holes;
        for (unsigned int ii = 0; ii < intersect.tubeIntersects().size(); ++ii) {
            const MuonTubeIntersect& tint = intersect.tubeIntersects()[ii];

            if (tubeIds.count(tint.tubeId)) { continue; }
            ATH_MSG_VERBOSE(" intersect " << m_idHelperSvc->toString(tint.tubeId) << "  dist wire " << tint.rIntersect
                                          << "  dist to tube end " << tint.xIntersect);

            if (std::abs(tint.rIntersect) > 14.4 || tint.xIntersect > -10.) {
                ATH_MSG_VERBOSE(" not counted");
            } else {
                // check whether there is a hit in this tube

                ATH_MSG_VERBOSE(" hole tube");

                holes.insert(tint.tubeId);
            }
        }
        return holes;
    }

    const MdtPrepDataCollection* MuonChamberHoleRecoveryTool::findMdtPrdCollection(const Identifier& chId, const EventContext& ctx) const {
        SG::ReadHandle<Muon::MdtPrepDataContainer> h_mdtPrdCont(m_key_mdt, ctx);
        const Muon::MdtPrepDataContainer* mdtPrdContainer{nullptr};
        if (h_mdtPrdCont.isValid()) {
            mdtPrdContainer = h_mdtPrdCont.cptr();
        } else {
            ATH_MSG_WARNING("Cannot retrieve mdtPrepDataContainer " << m_key_mdt.key());
        }
        if (!mdtPrdContainer || mdtPrdContainer->size() == 0) return nullptr;
        IdentifierHash hash_id;
        m_idHelperSvc->mdtIdHelper().get_module_hash(chId, hash_id);

        const auto* collptr = mdtPrdContainer->indexFindPtr(hash_id);
        if (!collptr) {
            ATH_MSG_DEBUG(" MdtPrepDataCollection for:   " << m_idHelperSvc->toStringChamber(chId) << "  not found in container ");
        }
        return collptr;
    }

    const CscPrepDataCollection* MuonChamberHoleRecoveryTool::findCscPrdCollection(const Identifier& detElId,
                                                                                   const EventContext& ctx) const {
        SG::ReadHandle<Muon::CscPrepDataContainer> h_cscPrdCont(m_key_csc, ctx);
        const Muon::CscPrepDataContainer* cscPrdContainer{nullptr};
        if (h_cscPrdCont.isValid()) {
            cscPrdContainer = h_cscPrdCont.cptr();
        } else {
            ATH_MSG_WARNING("Cannot retrieve cscPrepDataContainer " << m_key_csc.key());
        }
        if (!cscPrdContainer || cscPrdContainer->size() == 0) return nullptr;
        IdentifierHash hash_id;
        m_idHelperSvc->cscIdHelper().get_geo_module_hash(detElId, hash_id);

        const auto* collptr = cscPrdContainer->indexFindPtr(hash_id);
        if (!collptr) {
            ATH_MSG_DEBUG(" CscPrepDataCollection for:   " << m_idHelperSvc->toStringChamber(detElId) << "  not found in container ");
        }
        return collptr;
    }

    const TgcPrepDataCollection* MuonChamberHoleRecoveryTool::findTgcPrdCollection(const Identifier& detElId,
                                                                                   const EventContext& ctx) const {
        SG::ReadHandle<Muon::TgcPrepDataContainer> h_tgcPrdCont(m_key_tgc, ctx);
        const Muon::TgcPrepDataContainer* tgcPrdContainer{nullptr};
        if (h_tgcPrdCont.isValid()) {
            tgcPrdContainer = h_tgcPrdCont.cptr();
        } else {
            ATH_MSG_WARNING("Cannot retrieve tgcPrepDataContainer " << m_key_tgc.key());
        }
        if (!tgcPrdContainer || tgcPrdContainer->size() == 0) return nullptr;
        IdentifierHash hash_id;
        m_idHelperSvc->tgcIdHelper().get_module_hash(detElId, hash_id);

        const auto* collptr = tgcPrdContainer->indexFindPtr(hash_id);
        if (!collptr) {
            ATH_MSG_DEBUG(" TgcPrepDataCollection for:   " << m_idHelperSvc->toStringChamber(detElId) << "  not found in container ");
        }
        return collptr;
    }

    const RpcPrepDataCollection* MuonChamberHoleRecoveryTool::findRpcPrdCollection(const Identifier& detElId,
                                                                                   const EventContext& ctx) const {
        SG::ReadHandle<Muon::RpcPrepDataContainer> h_rpcPrdCont(m_key_rpc, ctx);
        const Muon::RpcPrepDataContainer* rpcPrdContainer{nullptr};
        if (h_rpcPrdCont.isValid()) {
            rpcPrdContainer = h_rpcPrdCont.cptr();
        } else {
            ATH_MSG_WARNING("Cannot retrieve rpcPrepDataContainer " << m_key_rpc.key());
            return nullptr;
        }
        if (!rpcPrdContainer || rpcPrdContainer->size() == 0) return nullptr;
        IdentifierHash hash_id;
        m_idHelperSvc->rpcIdHelper().get_module_hash(detElId, hash_id);
        const auto* collptr = rpcPrdContainer->indexFindPtr(hash_id);
        if (!collptr) {
            ATH_MSG_DEBUG(" RpcPrepDataCollection for:   " << m_idHelperSvc->toStringChamber(detElId) << "  not found in container ");
        }
        return collptr;
    }

    const sTgcPrepDataCollection* MuonChamberHoleRecoveryTool::findStgcPrdCollection(const Identifier& detElId,
                                                                                     const EventContext& ctx) const {
        SG::ReadHandle<Muon::sTgcPrepDataContainer> h_stgcPrdCont(m_key_stgc, ctx);
        const Muon::sTgcPrepDataContainer* stgcPrdContainer{nullptr};
        if (h_stgcPrdCont.isValid()) {
            stgcPrdContainer = h_stgcPrdCont.cptr();
        } else {
            ATH_MSG_WARNING("Cannot retrieve stgcPrepDataContainer " << m_key_stgc.key());
            return nullptr;
        }
        if (!stgcPrdContainer || stgcPrdContainer->size() == 0) return nullptr;
        IdentifierHash hash_id;
        m_idHelperSvc->stgcIdHelper().get_module_hash(detElId, hash_id);

        const auto* collptr = stgcPrdContainer->indexFindPtr(hash_id);
        if (!collptr) {
            ATH_MSG_DEBUG(" StgcPrepDataCollection for:   " << m_idHelperSvc->toStringChamber(detElId) << "  not found in container ");
        }
        return collptr;
    }

    const MMPrepDataCollection* MuonChamberHoleRecoveryTool::findMmPrdCollection(const Identifier& detElId, const EventContext& ctx) const {
        SG::ReadHandle<Muon::MMPrepDataContainer> h_mmPrdCont(m_key_mm, ctx);
        const Muon::MMPrepDataContainer* mmPrdContainer{nullptr};
        if (h_mmPrdCont.isValid()) {
            mmPrdContainer = h_mmPrdCont.cptr();
        } else {
            ATH_MSG_WARNING("Cannot retrieve stgcPrepDataContainer " << m_key_mm.key());
        }
        if (!mmPrdContainer || mmPrdContainer->size() == 0) return nullptr;

        IdentifierHash hash_id;
        m_idHelperSvc->mmIdHelper().get_module_hash(detElId, hash_id);

        const auto* collptr = mmPrdContainer->indexFindPtr(hash_id);
        if (!collptr) {
            ATH_MSG_DEBUG(" MmPrepDataCollection for:   " << m_idHelperSvc->toStringChamber(detElId) << "  not found in container ");
        }
        return collptr;
    }

}  // namespace Muon
