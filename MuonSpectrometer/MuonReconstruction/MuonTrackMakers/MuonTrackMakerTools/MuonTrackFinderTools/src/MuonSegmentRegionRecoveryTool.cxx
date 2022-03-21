/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentRegionRecoveryTool.h"

#include <ostream>

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonTrackMakerUtils/MuonGetClosestParameters.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"
#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"
#include "MuonTrackMakerUtils/SortTracksByHitNumber.h"
#include "RoiDescriptor/RoiDescriptor.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"

namespace Muon {

    MuonSegmentRegionRecoveryTool::MuonSegmentRegionRecoveryTool(const std::string& ty, const std::string& na, const IInterface* pa) :
        AthAlgTool(ty, na, pa) {
        declareInterface<IMuonHoleRecoveryTool>(this);
    }

    StatusCode MuonSegmentRegionRecoveryTool::initialize() {
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_chamberGeoKey.initialize());
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_seededSegmentFinder.retrieve());

        if (!m_trackSegmentMatchingTool.empty()) {
            ATH_CHECK(m_trackSegmentMatchingTool.retrieve());
            ATH_MSG_INFO("Using matching tool " << m_trackSegmentMatchingTool);
        } else {
            ATH_MSG_DEBUG("No matching tool selected ");
        }

        ATH_CHECK(m_chamberHoleRecoveryTool.retrieve());
        ATH_CHECK(m_extrapolator.retrieve());
        if (m_onlyEO) {
            ATH_CHECK(m_fitter.retrieve());
            m_builder.disable();
        } else {
            m_fitter.disable();
            ATH_CHECK(m_builder.retrieve());
        }
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_hitSummaryTool.retrieve());
        /// Old chamber technologies
        ATH_CHECK(m_regsel_mdt.retrieve());
        ATH_CHECK(m_regsel_rpc.retrieve());
        ATH_CHECK(m_regsel_tgc.retrieve());

        if (m_idHelperSvc->recoCSC()) {
            ATH_CHECK(m_regsel_csc.retrieve());
        } else
            m_regsel_csc.disable();

        if (m_idHelperSvc->recosTgc() && m_recoverSTGC) {
            ATH_CHECK(m_regsel_stgc.retrieve());
        } else {
            m_regsel_stgc.disable();
            m_recoverSTGC = false;
        }
        if (m_idHelperSvc->recoMM() && m_recoverMM) {
            ATH_CHECK(m_regsel_mm.retrieve());
        } else {
            m_regsel_mm.disable();
            m_recoverMM = false;
        }      
        return StatusCode::SUCCESS;
    }

    void MuonSegmentRegionRecoveryTool::createHoleTSOSsForClusterChamber(
        const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars, std::set<Identifier>& layIds,
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>>& states) const {
        m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber(detElId, ctx, pars, layIds, states);
    }

    /**Algorithmic part
         Strategy:
         1) Extrapolate back to MS entry record
         2) Collect hashes of chambers close to where the track passed
         3) Compare collected Hashes with the ones already on track
         4) Recover segments on the missing chambers by launching the SeededSegmentFinder
      */
    std::unique_ptr<Trk::Track> MuonSegmentRegionRecoveryTool::recover(const Trk::Track& track, const EventContext& ctx) const {
        // Welcome into the implementation of the recovery method
        ATH_MSG_VERBOSE(" Entering segment recovery method " << m_printer->print(track) << std::endl << m_printer->printStations(track));

        // First run the MuonChamberHoleRecoveryTool on the input track
        std::unique_ptr<Trk::Track> chRecTrack(m_chamberHoleRecoveryTool->recover(track, ctx));
        if (!chRecTrack) {
            ATH_MSG_DEBUG(" MuonChamberHoleRecoveryTool failed to create a new track "
                          << " Returning input (unrecovered) track ");
            return std::make_unique<Trk::Track>(track);
        }
        ATH_MSG_VERBOSE(" After chamber hole recovery " << m_printer->print(*chRecTrack) << std::endl
                                                        << m_printer->printStations(*chRecTrack));
        // only run this on single station EM tracks
        if (m_onlyEO) {
            // should be a sl track
            // using release until parent tools use unique_ptr
            if (!m_edmHelperSvc->isSLTrack(*chRecTrack)) return chRecTrack;

            // get hit summary
            IMuonHitSummaryTool::CompactSummary hitSummary = m_hitSummaryTool->summary(*chRecTrack);
            // should be single station
            // using release until parent tools use unique_ptr
            if (hitSummary.stationLayers.size() != 1 || !hitSummary.stationLayers.count(MuonStationIndex::EM)) return chRecTrack;
            ATH_MSG_DEBUG("Single station track, checking for EO hits");
        }

        // 1) Extrapolate back to MS entry record
        //     const Trk::TrackParameters* msEntryPars = goToMSEntryRecord( *chRecTrack );
        //     if(!msEntryPars) return chRecTrack;

        /** 2)  Collect hashes of chambers close to where the track passed
        2a) Extrapolate forward to the next station
        2b) Launch selector to get IDHashes
        2c) push_back hashes for this station into a set
        2d) stop where outer layers are
        2e) the outcome of all of this is a std::set of collected hashes
            (some are already on the track, no need to recover)  */

        MuonData muonData;
        // extract the hashes already on track
        fillOnTrackChambers(*chRecTrack, muonData);

        // collect the hashes not in track close to the track
        collectCrossedChambers(*chRecTrack, muonData);

        // 3b) compare the two std::sets and make a final std::set of not-yet-on-track Hashes
        std::unique_ptr<Trk::Track> triggerRecTrack = addMissingChambers(ctx, *chRecTrack, muonData, false);
        if (triggerRecTrack) chRecTrack.swap(triggerRecTrack);

        std::unique_ptr<Trk::Track> mdtRecTrack = addMissingChambers(ctx, *chRecTrack, muonData, true);
        if (mdtRecTrack) chRecTrack.swap(mdtRecTrack);

        std::unique_ptr<Trk::Track> mdtRecTrackWithHoles = findHoles(ctx, *chRecTrack, muonData);
        if (mdtRecTrackWithHoles) chRecTrack.swap(mdtRecTrackWithHoles);

        // recovered track, success!
        // use release until calling tools also use unique_ptr
        return chRecTrack;
    }

    //// NO, NO, NO, should pass in an IRoiDescriptor
    // void MuonSegmentRegionRecoveryTool::addHashes( DETID type, double etamin, double etamax, double phimin, double phimax,
    void MuonSegmentRegionRecoveryTool::addHashes(DETID type, const IRoiDescriptor& roi, std::set<IdentifierHash>& hashes,
                                                  const std::set<IdentifierHash>& exclusion) const {
        // if only looking at EO, skip all but MDT chambers
        if (m_onlyEO && type != MDT) return;

        std::vector<IdentifierHash> crossed;

        if (type == MDT) m_regsel_mdt->HashIDList(roi, crossed);
        if (type == CSC) m_regsel_csc->HashIDList(roi, crossed);
        if (type == RPC) m_regsel_rpc->HashIDList(roi, crossed);
        if (type == TGC) m_regsel_tgc->HashIDList(roi, crossed);
        if (type == STGC) m_regsel_stgc->HashIDList(roi, crossed);
        if (type == MM) m_regsel_mm->HashIDList(roi, crossed);

        for (std::vector<IdentifierHash>::iterator it = crossed.begin(); it != crossed.end(); ++it) {
            if (!exclusion.count(*it) && !hashes.count(*it)) {
                if (type == MDT) {
                    Identifier chId;
                    IdContext otCont = m_idHelperSvc->mdtIdHelper().module_context();
                    m_idHelperSvc->mdtIdHelper().get_id(*it, chId, &otCont);

                    if (m_excludeEES && m_idHelperSvc->chamberIndex(chId) == MuonStationIndex::EES) {
                        ATH_MSG_VERBOSE("  excluding " << *it << " " << m_idHelperSvc->toStringChamber(chId));
                        continue;
                    }
                    if (m_onlyEO && m_idHelperSvc->stationIndex(chId) != MuonStationIndex::EO) {
                        ATH_MSG_VERBOSE("  excluding " << *it << " " << m_idHelperSvc->toStringChamber(chId));
                        continue;
                    }
                    ATH_MSG_VERBOSE("  -- hash " << *it << " " << m_idHelperSvc->toStringChamber(chId));
                }
                if (type == CSC) {
                    Identifier chId;
                    IdContext otCont = m_idHelperSvc->cscIdHelper().module_context();
                    m_idHelperSvc->cscIdHelper().get_id(*it, chId, &otCont);
                    ATH_MSG_VERBOSE("  -- csc hash " << *it << " " << m_idHelperSvc->toStringChamber(chId));
                }
                if (type == STGC) {
                    Identifier chId;
                    IdContext otCont = m_idHelperSvc->stgcIdHelper().detectorElement_context();
                    m_idHelperSvc->stgcIdHelper().get_id(*it, chId, &otCont);
                    ATH_MSG_VERBOSE("  -- stgc hash " << *it << " " << m_idHelperSvc->toStringChamber(chId));
                }
                if (type == MM) {
                    Identifier chId;
                    IdContext otCont = m_idHelperSvc->mmIdHelper().detectorElement_context();
                    m_idHelperSvc->mmIdHelper().get_id(*it, chId, &otCont);
                    ATH_MSG_VERBOSE("  -- mm hash " << *it << " " << m_idHelperSvc->toStringChamber(chId));
                }
                hashes.insert(*it);
            }
        }
    }

    void MuonSegmentRegionRecoveryTool::collectCrossedChambers(const Trk::Track& track,
                                                               MuonSegmentRegionRecoveryTool::MuonData& data) const {
        ATH_MSG_VERBOSE(" Collecting all crossed chambers");

        const Trk::TrackStates* states = track.trackStateOnSurfaces();
        if (!states) {
            ATH_MSG_WARNING(" track without states, cannot perform cleaning ");
            return;
        }

        std::set<MuonStationIndex::StIndex> stations;
        double etamin{1e9}, etamax{-1e9}, phimin{1e9}, phimax{-1e9};
        // loop over TSOSs
        Trk::TrackStates::const_iterator tsit = states->begin();
        Trk::TrackStates::const_iterator tsit_end = states->end();

        for (; tsit != tsit_end; ++tsit) {
            const Trk::TrackParameters* pars = (*tsit)->trackParameters();
            if (!pars) continue;

            double rpos = pars->position().perp();
            double zpos = std::abs(pars->position().z());
            if (rpos < 2500 && zpos < 4000) continue;
            double eta = pars->position().eta();
            double phi = pars->position().phi();
            if (msgLvl(MSG::DEBUG)) {
                msg() << "All State parameters: eta: " << eta << " ,phi: " << phi << " pos r " << rpos << " z " << zpos;
                if (pars->covariance()) msg() << MSG::DEBUG << " extrapolation error " << Amg::error(*pars->covariance(), Trk::locX);
                msg() << endmsg;
            }

            // check whether state is a measurement
            const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
            if (!meas) continue;

            Identifier id = m_edmHelperSvc->getIdentifier(*meas);
            bool pseudo = !id.is_valid();

            if (pseudo || !m_idHelperSvc->mdtIdHelper().is_muon(id)) continue;

            if (eta > etamax) etamax = eta;
            if (eta < etamin) etamin = eta;
            if (phi > phimax) phimax = phi;
            if (phi < phimin) phimin = phi;
            if (msgLvl(MSG::DEBUG)) {
                msg() << "Selected State parameters: eta: " << eta << " ,phi: " << phi << " pos r " << rpos << " z " << zpos;
                if (pars->covariance()) msg() << MSG::DEBUG << " extrapolation error " << Amg::error(*pars->covariance(), Trk::locX);
                msg() << endmsg;
            }

            MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
            if (stations.count(stIndex)) continue;
            stations.insert(stIndex);

            // At each station launch the Reg Sel and stuff the Id hashes in a vector
        }
        etamin -= m_deta;
        etamax += m_deta;
        phimin -= m_dphi;
        phimax += m_dphi;

        ATH_MSG_DEBUG("Eta range: " << etamin << " " << etamax << " Phi range " << phimin << " " << phimax);
        if (etamin > etamax) {  // something went wrong with the fit and no hits were selected in the loop
            ATH_MSG_DEBUG("no hits selected, nothing further will be done");
            return;
        }

        RoiDescriptor roi(etamin, etamax, phimin, phimax);

        if (m_idHelperSvc->hasMDT() && (m_idHelperSvc->mdtIdHelper().isInitialized())) addHashes(MDT, roi, data.mdt, data.mdtTrack);
        if (m_idHelperSvc->hasRPC() && (m_idHelperSvc->rpcIdHelper().isInitialized())) addHashes(RPC, roi, data.rpc, data.rpcTrack);
        if (m_idHelperSvc->hasTGC() && (m_idHelperSvc->tgcIdHelper().isInitialized())) addHashes(TGC, roi, data.tgc, data.tgcTrack);
        if (m_idHelperSvc->recoCSC()) addHashes(CSC, roi, data.csc, data.cscTrack);
        if (m_recoverSTGC) addHashes(STGC, roi, data.stgc, data.stgcTrack);
        if (m_recoverMM) addHashes(MM, roi, data.mm, data.mmTrack);

        std::set<IdentifierHash>::iterator hsit = data.mdt.begin();
        std::set<IdentifierHash>::iterator hsit_end = data.mdt.end();
        for (; hsit != hsit_end; ++hsit) {
            Identifier chId;
            IdContext otCont = m_idHelperSvc->mdtIdHelper().module_context();
            m_idHelperSvc->mdtIdHelper().get_id(*hsit, chId, &otCont);
            MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
            data.mdtPerStation[chIndex].insert(*hsit);
            ATH_MSG_VERBOSE(" chamberlayer  " << MuonStationIndex::chName(chIndex) << "  " << m_idHelperSvc->toStringChamber(chId));
        }
        hsit = data.csc.begin();
        hsit_end = data.csc.end();
        for (; hsit != hsit_end; ++hsit) {
            Identifier chId;
            IdContext otCont = m_idHelperSvc->cscIdHelper().module_context();
            m_idHelperSvc->cscIdHelper().get_id(*hsit, chId, &otCont);
            chId = m_idHelperSvc->chamberId(chId);
            MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
            ATH_MSG_VERBOSE(" chamberlayer  " << MuonStationIndex::chName(chIndex) << "  " << m_idHelperSvc->toString(chId));
        }
    }

    void MuonSegmentRegionRecoveryTool::fillOnTrackChambers(const Trk::Track& track, MuonData& data) const {
        ATH_MSG_VERBOSE(" making set of already-on-track chambers");

        // Collect track PRD's
        const Trk::TrackStates* states = track.trackStateOnSurfaces();
        if (!states) {
            ATH_MSG_WARNING(" track without states, cannot perform cleaning ");
            return;
        }

        std::set<MuonStationIndex::StIndex> stations;

        // loop over TSOSs
        Trk::TrackStates::const_iterator tsit = states->begin();
        Trk::TrackStates::const_iterator tsit_end = states->end();
        for (; tsit != tsit_end; ++tsit) {
            const Trk::TrackParameters* pars = (*tsit)->trackParameters();
            if (!pars) continue;

            // check whether state is a measurement
            const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
            if (!meas) continue;

            const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(meas);
            if (mdt && !data.mdtTrack.count(mdt->collectionHash())) {
                data.mdtTrack.insert(mdt->collectionHash());
                ATH_MSG_VERBOSE("Adding " << m_idHelperSvc->toStringChamber(mdt->identify()) << " hash " << mdt->collectionHash());
                continue;
            }

            const MuonClusterOnTrack* clus = nullptr;
            const CompetingMuonClustersOnTrack* compRot = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
            if (compRot) {
                clus = &compRot->rioOnTrack(0);
            } else {
                clus = dynamic_cast<const MuonClusterOnTrack*>(meas);
            }
            if (!clus) continue;

            const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(clus);
            if (rpc && !data.rpcTrack.count(rpc->collectionHash())) {
                data.rpcTrack.insert(rpc->collectionHash());
                ATH_MSG_VERBOSE("Adding " << m_idHelperSvc->toStringChamber(clus->identify()) << " hash " << clus->collectionHash());
                continue;
            }
            const TgcClusterOnTrack* tgc = dynamic_cast<const TgcClusterOnTrack*>(clus);
            if (tgc && !data.tgcTrack.count(tgc->collectionHash())) {
                data.tgcTrack.insert(tgc->collectionHash());
                ATH_MSG_VERBOSE("Adding " << m_idHelperSvc->toStringChamber(clus->identify()) << " hash " << clus->collectionHash());
                continue;
            }
            const CscClusterOnTrack* csc = dynamic_cast<const CscClusterOnTrack*>(clus);
            if (csc && !data.cscTrack.count(csc->collectionHash())) {
                data.cscTrack.insert(csc->collectionHash());
                ATH_MSG_VERBOSE("Adding " << m_idHelperSvc->toStringChamber(clus->identify()) << " hash " << clus->collectionHash());
                continue;
            }
            // New Small Wheel
            const sTgcClusterOnTrack* stgc = dynamic_cast<const sTgcClusterOnTrack*>(clus);
            if (stgc && !data.stgcTrack.count(stgc->collectionHash())) {
                data.stgcTrack.insert(stgc->collectionHash());
                ATH_MSG_VERBOSE("Adding " << m_idHelperSvc->toStringChamber(clus->identify()) << " hash " << clus->collectionHash());
                continue;
            }
            const MMClusterOnTrack* mm = dynamic_cast<const MMClusterOnTrack*>(clus);
            if (mm && !data.mmTrack.count(mm->collectionHash())) {
                data.mmTrack.insert(mm->collectionHash());
                ATH_MSG_VERBOSE("Adding " << m_idHelperSvc->toStringChamber(clus->identify()) << " hash " << clus->collectionHash());
                continue;
            }
        }
    }

    std::unique_ptr<Trk::Track> MuonSegmentRegionRecoveryTool::findHoles(const EventContext& ctx, const Trk::Track& track,
                                                                         MuonData& data) const {
        const Trk::TrackStates* oldStates = track.trackStateOnSurfaces();
        if (!oldStates) {
            ATH_MSG_WARNING(" track without states, cannot perform mdt hole search ");
            return nullptr;
        }

        SG::ReadCondHandle<Muon::MuonIntersectGeoData> InterSectSvc{m_chamberGeoKey,ctx};
        if (!InterSectSvc.isValid())   {
            ATH_MSG_ERROR("Failed to retrieve chamber intersection service");
            return nullptr;
        }
        const MuonGM::MuonDetectorManager* MuonDetMgr = InterSectSvc->detMgr();

        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> states;
        unsigned int nholes = 0;
        std::set<Identifier> chambersInSearch;
        for (const IdentifierHash& ith : data.mdt) {
            // ignore hashes already on track
            if (data.mdtTrack.count(ith)) {
                ATH_MSG_VERBOSE("Chamber already on track " << ith);
                continue;
            }
            Identifier chId;
            IdContext otCont = m_idHelperSvc->mdtIdHelper().module_context();
            m_idHelperSvc->mdtIdHelper().get_id(ith, chId, &otCont);

            // ignore chambers that already had a hole search
            if (chambersInSearch.count(chId)) {
                ATH_MSG_VERBOSE("Chamber already on track " << ith << " " << m_idHelperSvc->toStringChamber(chId));
                continue;
            }
            const MuonGM::MdtReadoutElement* detEl = MuonDetMgr->getMdtReadoutElement(chId);
            if (!detEl) {
                ATH_MSG_WARNING("Found no detector element for " << ith);
                continue;
            }
            std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *detEl, true)};
            if (!exPars) {
                ATH_MSG_DEBUG("Did not reach " << m_idHelperSvc->toStringChamber(chId) << " hash " << ith);
                continue;
            }
            ATH_MSG_DEBUG("Reached " << m_idHelperSvc->toStringChamber(chId) << " hash " << ith);

            // calculate crossed tubes
            const MuonStationIntersect intersect = InterSectSvc->tubesCrossedByTrack(chId, exPars->position(), exPars->momentum().unit());
           
            // clear hole vector
            for (unsigned int ii = 0; ii < intersect.tubeIntersects().size(); ++ii) {
                const MuonTubeIntersect& tint = intersect.tubeIntersects()[ii];
                const Identifier& id = tint.tubeId;
                const MuonGM::MdtReadoutElement* detElLoc = MuonDetMgr->getMdtReadoutElement(id);

                // addition skip for cases when the track crosses inbetween two chambers
                if (data.mdtTrack.count(detElLoc->collectionHash())) continue;

                Identifier ch = m_idHelperSvc->chamberId(id);
                chambersInSearch.insert(ch);

                const Trk::Surface& surf = detElLoc->surface(id);
                std::unique_ptr<const Trk::TrackParameters> tubePars{
                    m_extrapolator->extrapolateDirectly(ctx, *exPars, surf, Trk::anyDirection, false, Trk::muon)};
                if (!tubePars) {
                    ATH_MSG_WARNING("Failed to extrapolate to tube " << m_idHelperSvc->toString(id));
                    continue;
                }
                int lay = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
                int tube = m_idHelperSvc->mdtIdHelper().tube(id);
                double tubeLen = detElLoc->getActiveTubeLength(lay, tube);
                double distEdge = std::abs(tubePars->parameters()[Trk::locZ]) - 0.5 * tubeLen;
                double pullEdge = tubePars->covariance() && Amg::saneCovarianceDiagonal(*tubePars->covariance())
                                      ? distEdge / Amg::error(*tubePars->covariance(), Trk::locZ)
                                      : distEdge / 20.;
                std::optional<Amg::Vector2D> locPos = surf.Trk::Surface::globalToLocal(tubePars->position());

                bool inBounds = false;
                if (locPos) {
                    // perform bound check do not count holes with 100. mm of bound edge
                    inBounds = surf.bounds().insideLoc2(*locPos, -100.) && std::abs((*locPos)[Trk::locR]) <= detElLoc->innerTubeRadius();
                }
                if (!inBounds) {
                    ATH_MSG_VERBOSE(" discarding hole " << m_idHelperSvc->toString(id) << " dist wire " << tubePars->parameters()[Trk::locR]
                                                        << " outside bounds ");
                    continue;
                }
                ATH_MSG_VERBOSE(" new hole " << m_idHelperSvc->toString(id) << " dist wire " << tubePars->parameters()[Trk::locR]
                                             << " dist tube edge " << distEdge << " pullEdge " << pullEdge);
                ++nholes;
                std::unique_ptr<Trk::TrackStateOnSurface> tsos = MuonTSOSHelper::createHoleTSOS(std::move(tubePars));
                states.emplace_back(std::move(tsos));
            }
            if (!nholes) ATH_MSG_DEBUG("found holes " << nholes);
        }

        for (const IdentifierHash& ith : data.csc) {
            // ignore hashes already on track
            if (data.cscTrack.count(ith)) {
                ATH_MSG_VERBOSE("Chamber already on track " << ith);
                continue;
            }
            Identifier chId;
            IdContext otCont = m_idHelperSvc->cscIdHelper().module_context();
            m_idHelperSvc->cscIdHelper().get_id(ith, chId, &otCont);
            chId = m_idHelperSvc->chamberId(chId);

            const MuonGM::CscReadoutElement* detEl = MuonDetMgr->getCscReadoutElement(chId);
            if (!detEl) {
                ATH_MSG_WARNING("Found no detector element for " << ith << " " << m_idHelperSvc->toString(chId));
                continue;
            }

            std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *detEl, true)};
            if (!exPars) {
                ATH_MSG_DEBUG("Did not reach " << m_idHelperSvc->toStringChamber(chId) << " hash " << ith);
                continue;
            }
            Identifier detElId = m_idHelperSvc->detElId(chId);
            std::set<Identifier> layIds;
            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> cscstates;
            createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, cscstates);
            ATH_MSG_DEBUG("Reached " << m_idHelperSvc->toString(chId) << " hash " << ith << " holes " << cscstates.size());

            for (std::unique_ptr<const Trk::TrackStateOnSurface>& cscit : cscstates) {
                ATH_MSG_VERBOSE("Adding " << cscit.get());
                states.emplace_back(std::move(cscit));
            }
        }
        for (const IdentifierHash& ith : data.tgc) {
            // ignore hashes already on track
            if (data.tgcTrack.count(ith)) {
                ATH_MSG_VERBOSE("Chamber already on track " << ith);
                continue;
            }
            Identifier chId;
            IdContext otCont = m_idHelperSvc->tgcIdHelper().module_context();
            m_idHelperSvc->tgcIdHelper().get_id(ith, chId, &otCont);
            chId = m_idHelperSvc->chamberId(chId);

            const MuonGM::TgcReadoutElement* detEl = MuonDetMgr->getTgcReadoutElement(chId);
            if (!detEl) {
                ATH_MSG_WARNING("Found no detector element for " << ith << " " << m_idHelperSvc->toString(chId));
                continue;
            }

            std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *detEl, true)};
            if (!exPars) {
                ATH_MSG_DEBUG("Did not reach " << m_idHelperSvc->toStringChamber(chId) << " hash " << ith);
                continue;
            }
            Identifier detElId = m_idHelperSvc->detElId(chId);
            std::set<Identifier> layIds;
            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> tgcstates;
            createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, tgcstates);
            ATH_MSG_DEBUG("Reached " << m_idHelperSvc->toString(chId) << " hash " << ith << " holes " << tgcstates.size());

            for (std::unique_ptr<const Trk::TrackStateOnSurface>& tgcit : tgcstates) {
                ATH_MSG_VERBOSE("Adding " << tgcit.get());
                states.emplace_back(std::move(tgcit));
            }
        }

        for (const IdentifierHash& ith : data.rpc) {
            // ignore hashes already on track
            if (data.rpcTrack.count(ith)) {
                ATH_MSG_VERBOSE("Chamber already on track " << ith);
                continue;
            }
            Identifier chId;
            IdContext otCont = m_idHelperSvc->rpcIdHelper().module_context();
            m_idHelperSvc->rpcIdHelper().get_id(ith, chId, &otCont);
            chId = m_idHelperSvc->chamberId(chId);

            const MuonGM::RpcReadoutElement* detEl = MuonDetMgr->getRpcReadoutElement(chId);
            if (!detEl) {
                ATH_MSG_WARNING("Found no detector element for " << ith << " " << m_idHelperSvc->toString(chId));
                continue;
            }

            std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *detEl, true)};
            if (!exPars) {
                ATH_MSG_DEBUG("Did not reach " << m_idHelperSvc->toStringChamber(chId) << " hash " << ith);
                continue;
            }
            Identifier detElId = m_idHelperSvc->detElId(chId);
            std::set<Identifier> layIds;
            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> rpcstates;
            createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, rpcstates);
            ATH_MSG_DEBUG("Reached " << m_idHelperSvc->toString(chId) << " hash " << ith << " holes " << rpcstates.size());

            for (std::unique_ptr<const Trk::TrackStateOnSurface>& rpcit : rpcstates) {
                ATH_MSG_VERBOSE("Adding " << rpcit.get());
                states.emplace_back(std::move(rpcit));
            }
        }

        // New Small Wheel
        // sTGC
        for (const IdentifierHash& ith : data.stgc) {
            // ignore hashes already on track
            if (data.stgcTrack.count(ith)) {
                ATH_MSG_VERBOSE("Chamber already on track " << ith);
                continue;
            }
            Identifier chId;
            IdContext otCont = m_idHelperSvc->stgcIdHelper().module_context();
            m_idHelperSvc->stgcIdHelper().get_id(ith, chId, &otCont);

            if (!chId.is_valid()) {
                ATH_MSG_VERBOSE("invalid chId for stgc data " << ith);
                continue;
            }

            chId = m_idHelperSvc->chamberId(chId);

            const MuonGM::sTgcReadoutElement* detEl = MuonDetMgr->getsTgcReadoutElement(chId);
            if (!detEl) {
                ATH_MSG_WARNING("Found no detector element for " << ith << " " << m_idHelperSvc->toString(chId));
                continue;
            }

            std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *detEl, true)};
            if (!exPars) {
                ATH_MSG_DEBUG("Did not reach " << m_idHelperSvc->toStringChamber(chId) << " hash " << ith);
                continue;
            }
            Identifier detElId = m_idHelperSvc->detElId(chId);
            std::set<Identifier> layIds;
            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> stgcstates;
            createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, stgcstates);
            ATH_MSG_DEBUG("Reached " << m_idHelperSvc->toString(chId) << " hash " << ith << " holes " << stgcstates.size());

            for (std::unique_ptr<const Trk::TrackStateOnSurface>& stgcit : stgcstates) {
                ATH_MSG_VERBOSE("Adding " << stgcit.get());
                states.push_back(std::move(stgcit));
            }
        }

        // MM
        for (const IdentifierHash& ith : data.mm) {
            // ignore hashes already on track
            if (data.mmTrack.count(ith)) {
                ATH_MSG_VERBOSE("Chamber already on track " << ith);
                continue;
            }
            Identifier chId;
            IdContext otCont = m_idHelperSvc->mmIdHelper().module_context();
            m_idHelperSvc->mmIdHelper().get_id(ith, chId, &otCont);

            if (!chId.is_valid()) {
                ATH_MSG_VERBOSE("invalid chId for mm data " << ith);
                continue;
            }

            chId = m_idHelperSvc->chamberId(chId);

            const MuonGM::MMReadoutElement* detEl = MuonDetMgr->getMMReadoutElement(chId);
            if (!detEl) {
                ATH_MSG_WARNING("Found no detector element for " << ith << " " << m_idHelperSvc->toString(chId));
                continue;
            }

            std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *detEl, true)};
            if (!exPars) {
                ATH_MSG_DEBUG("Did not reach " << m_idHelperSvc->toStringChamber(chId) << " hash " << ith);
                continue;
            }
            Identifier detElId = m_idHelperSvc->detElId(chId);
            std::set<Identifier> layIds;
            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> mmstates;
            createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, mmstates);
            ATH_MSG_DEBUG("Reached " << m_idHelperSvc->toString(chId) << " hash " << ith << " holes " << mmstates.size());

            for (std::unique_ptr<const Trk::TrackStateOnSurface>& mmit : mmstates) {
                ATH_MSG_VERBOSE("Adding " << mmit.get());
                states.emplace_back(std::move(mmit));
            }
        }
        if (!states.empty()) {
            // states were added, create a new track
            auto trackStateOnSurfaces = DataVector<const Trk::TrackStateOnSurface>();
            trackStateOnSurfaces.reserve(oldStates->size() + states.size());

            std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> toBeSorted;
            toBeSorted.reserve(oldStates->size() + states.size());

            for (const Trk::TrackStateOnSurface* sit : *oldStates) toBeSorted.emplace_back(sit->clone());

            for (std::unique_ptr<const Trk::TrackStateOnSurface>& stit : states) toBeSorted.emplace_back(std::move(stit));

            std::stable_sort(toBeSorted.begin(), toBeSorted.end(), SortTSOSs(&*m_edmHelperSvc, &*m_idHelperSvc));

            for (std::unique_ptr<const Trk::TrackStateOnSurface>& sorted : toBeSorted) { trackStateOnSurfaces.push_back(sorted.release()); }
            std::unique_ptr<Trk::Track> trackWithHoles = std::make_unique<Trk::Track>(
                track.info(), std::move(trackStateOnSurfaces), track.fitQuality() ? track.fitQuality()->clone() : nullptr);
            // generate a track summary for this track
            if (m_trackSummaryTool.isEnabled()) { m_trackSummaryTool->computeAndReplaceTrackSummary(ctx, *trackWithHoles, nullptr, false); }
            ATH_MSG_DEBUG("Track with holes " << m_printer->print(*trackWithHoles) << std::endl
                                              << m_printer->printStations(*trackWithHoles));
            return trackWithHoles;
        }
        return nullptr;
    }

    std::unique_ptr<Trk::Track> MuonSegmentRegionRecoveryTool::addMissingChambers(const EventContext& ctx, const Trk::Track& track,
                                                                                  MuonData& data, bool addMdt) const {
        // states were added, create a new track
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> states;
        std::set<IdentifierHash> newMdtHashes;
        std::set<IdentifierHash> newRpcHashes;
        std::set<IdentifierHash> newTgcHashes;
        std::set<IdentifierHash> newCscHashes;
        // New Small Wheel
        std::set<IdentifierHash> newMMHashes;
        std::set<IdentifierHash> newsTgcHashes;
        if (addMdt) {
            ATH_MSG_DEBUG("Adding Missing MDT chambers: regions " << data.mdtPerStation.size());
            std::vector<const MdtPrepDataCollection*> newmcols;
            for (const auto& chit : data.mdtPerStation) {
                ATH_MSG_VERBOSE("Region " << MuonStationIndex::chName(chit.first) << " size  " << chit.second.size());
                std::vector<const MdtPrepDataCollection*> cols;
                m_seededSegmentFinder->extractMdtPrdCols(ctx, chit.second, cols);
                std::map<int, std::vector<const MdtPrepData*>> mdtPrds;
                std::unique_ptr<const Trk::TrackParameters> exParsFirst;
                for (const MdtPrepDataCollection* mit : cols) {
                    std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *mit->front()->detectorElement())};
                    if (exPars) {
                        int sector = m_idHelperSvc->sector(mit->identify());
                        ATH_MSG_DEBUG("New chamber " << m_idHelperSvc->toStringChamber(mit->identify()) << " hash " << mit->identifyHash()
                                                     << " sector " << sector);
                        newmcols.emplace_back(mit);
                        std::vector<const MdtPrepData*>& col = mdtPrds[sector];
                        col.insert(col.end(), mit->begin(), mit->end());
                        if (!exParsFirst) exParsFirst.swap(exPars);

                    } else {
                        ATH_MSG_DEBUG("Did not reach chamber " << m_idHelperSvc->toStringChamber(mit->identify()) << " hash "
                                                               << mit->identifyHash());
                    }
                }
                std::vector<const MdtPrepData*>* prds = nullptr;
                std::map<int, std::vector<const MdtPrepData*>>::iterator sectorIt = mdtPrds.begin();
                if (mdtPrds.empty()) {
                    ATH_MSG_VERBOSE("No hits selected");
                } else if (mdtPrds.size() == 1) {
                    prds = &sectorIt->second;
                } else {
                    IMuonHitSummaryTool::CompactSummary hitSummary = m_hitSummaryTool->summary(track);
                    ATH_MSG_VERBOSE("Multiple sectors selected, using main sector: " << hitSummary.mainSector);
                    std::map<int, std::vector<const MdtPrepData*>>::iterator pos = mdtPrds.find(hitSummary.mainSector);
                    if (pos != mdtPrds.end())
                        prds = &pos->second;
                    else {
                        ATH_MSG_DEBUG("Failed to find prds in main sector: " << hitSummary.mainSector);
                    }
                }
                if (prds && exParsFirst) {
                    std::unique_ptr<Trk::SegmentCollection> segments = m_seededSegmentFinder->find(ctx, *exParsFirst, *prds);
                    if (segments) {
                        if (!segments->empty()) ATH_MSG_DEBUG("found segments " << segments->size());

                        MuonSegment* bestSegment = nullptr;
                        std::unique_ptr<const Trk::TrackParameters> bestSegmentPars;
                        for (Trk::Segment* tseg : *segments) {
                            MuonSegment* mseg = dynamic_cast<MuonSegment*>(tseg);

                            if (m_trackSegmentMatchingTool.empty())
                                ATH_MSG_VERBOSE("No track/segment matching");
                            else if (!m_trackSegmentMatchingTool->match(ctx, track, *mseg, true)) {
                                ATH_MSG_DEBUG(" Segment does not match with track ");
                                continue;
                            } else {
                                ATH_MSG_DEBUG(" Segment/track matched successfully using " << m_trackSegmentMatchingTool);
                            }
                            std::unique_ptr<const Trk::TrackParameters> segPars{m_extrapolator->extrapolateDirectly(
                                ctx, *exParsFirst, mseg->associatedSurface(), Trk::anyDirection, false, Trk::muon)};
                            if (segPars) {
                                double resy = mseg->localParameters()[Trk::locY] - segPars->parameters()[Trk::locY];
                                Trk::LocalDirection locDir;
                                mseg->associatedSurface().globalToLocalDirection(segPars->momentum(), locDir);
                                double dangleYZ = mseg->localDirection().angleYZ() - locDir.angleYZ();
                                ATH_MSG_DEBUG("resy " << resy << " dangleYZ " << dangleYZ << " " << m_printer->print(*mseg));
                                if (std::abs(dangleYZ) < 0.05) {
                                    bestSegment = mseg;
                                    bestSegmentPars.swap(segPars);
                                }
                            } else {
                                ATH_MSG_DEBUG("Did not reach " << m_printer->print(*mseg));
                            }
                        }
                        if (bestSegment) {
                            for (const Trk::MeasurementBase* hit : bestSegment->containedMeasurements()) {
                                std::unique_ptr<const Trk::TrackParameters> hitPars{m_extrapolator->extrapolateDirectly(
                                    ctx, *bestSegmentPars, hit->associatedSurface(), Trk::anyDirection, false, Trk::muon)};
                                if (hitPars) {
                                    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
                                    typePattern.set(Trk::TrackStateOnSurface::Measurement);
                                    Trk::TrackStateOnSurface* tsos =
                                        new Trk::TrackStateOnSurface(hit->uniqueClone(), std::move(hitPars), nullptr, nullptr, typePattern);
                                    states.emplace_back(tsos);
                                    const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(hit);
                                    if (mdt) newMdtHashes.insert(mdt->collectionHash());
                                } else {
                                    ATH_MSG_WARNING("Failed to extrapolate to MDT hit ");
                                }
                            }
                        }
                    }
                }
            }
            data.mdtCols = std::move(newmcols);
        } else {
            unsigned int nstates = states.size();
            {
                m_seededSegmentFinder->extractRpcPrdCols(ctx, data.rpc, data.rpcCols);
                std::vector<const RpcPrepDataCollection*> newtcols;
                for (const RpcPrepDataCollection* rit : data.rpcCols) {
                    std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *(rit)->front()->detectorElement())};
                    if (exPars) {
                        newtcols.emplace_back(rit);
                        Identifier detElId = m_idHelperSvc->detElId(rit->identify());
                        std::set<Identifier> layIds;
                        createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, states);
                        if (states.size() != nstates) {
                            nstates = states.size();
                            newRpcHashes.insert(rit->identifyHash());
                        }
                    }
                }
                data.rpcCols = std::move(newtcols);
            }
            {
                m_seededSegmentFinder->extractTgcPrdCols(ctx, data.tgc, data.tgcCols);
                std::vector<const TgcPrepDataCollection*> newtcols;
                for (const TgcPrepDataCollection* tgcit : data.tgcCols) {
                    std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *(tgcit)->front()->detectorElement())};
                    if (exPars) {
                        newtcols.emplace_back(tgcit);
                        Identifier detElId = m_idHelperSvc->detElId(tgcit->identify());
                        std::set<Identifier> layIds;
                        createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, states);
                        if (states.size() != nstates) {
                            nstates = states.size();
                            newTgcHashes.insert(tgcit->identifyHash());
                        }
                    }
                }
                data.tgcCols = std::move(newtcols);
            }
            if (m_idHelperSvc->recoCSC()) {
                m_seededSegmentFinder->extractCscPrdCols(data.csc, data.cscCols);
                std::vector<const CscPrepDataCollection*> newccols;
                for (const CscPrepDataCollection* cit : data.cscCols) {
                    std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *(cit)->front()->detectorElement())};
                    if (exPars) {
                        newccols.push_back(cit);
                        Identifier detElId = m_idHelperSvc->detElId(cit->identify());
                        std::set<Identifier> layIds;
                        createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, states);
                        if (states.size() != nstates) {
                            nstates = states.size();
                            newCscHashes.insert(cit->identifyHash());
                        }
                    }
                }
                data.cscCols = std::move(newccols);
            }

            nstates = states.size();
            if (m_recoverSTGC) {
                m_seededSegmentFinder->extractsTgcPrdCols(ctx, data.stgc, data.stgcCols);
                std::vector<const sTgcPrepDataCollection*> newstcols;
                ATH_MSG_DEBUG(" extractsTgcPrdCols data.stgcCols.size() " << data.stgcCols.size());
                for (const sTgcPrepDataCollection* stgcit : data.stgcCols) {
                    std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *(stgcit)->front()->detectorElement())};
                    if (exPars) {
                        newstcols.push_back(stgcit);
                        Identifier detElId = m_idHelperSvc->detElId(stgcit->identify());
                        std::set<Identifier> layIds;
                        createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, states);
                        ATH_MSG_DEBUG("Collected new sTgc states: " << states.size());
                        if (states.size() != nstates) {
                            nstates = states.size();
                            newsTgcHashes.insert(stgcit->identifyHash());
                        }
                    }
                }
                data.stgcCols = std::move(newstcols);
            }

            if (m_recoverMM) {
                m_seededSegmentFinder->extractMMPrdCols(data.mm, data.mmCols);
                ATH_MSG_DEBUG(" extractMMPrdCols data.mmCols.size() " << data.mmCols.size());
                std::vector<const MMPrepDataCollection*> newmcols;
                for (const MMPrepDataCollection* mit : data.mmCols) {
                    std::unique_ptr<const Trk::TrackParameters> exPars{reachableDetEl(ctx, track, *mit->front()->detectorElement())};
                    if (exPars) {
                        newmcols.push_back(mit);
                        Identifier detElId = m_idHelperSvc->detElId(mit->identify());
                        std::set<Identifier> layIds;
                        createHoleTSOSsForClusterChamber(detElId, ctx, *exPars, layIds, states);
                        ATH_MSG_DEBUG("Collected new Mm states: " << states.size());
                        if (states.size() != nstates) {
                            nstates = states.size();
                            newMMHashes.insert(mit->identifyHash());
                        }
                    }
                }
                data.mmCols = std::move(newmcols);
            }
        }
        if (!states.empty()) {
            ATH_MSG_DEBUG("Collected new states: " << states.size());

            const Trk::TrackStates* oldStates = track.trackStateOnSurfaces();
            if (!oldStates) {
                ATH_MSG_WARNING(" track without states, cannot perform cleaning ");
                return nullptr;
            }

            ATH_MSG_DEBUG("Copying old TSOSs " << oldStates->size());
            for (const Trk::TrackStateOnSurface* tsit : *oldStates) states.emplace_back(tsit->clone());

            std::stable_sort(states.begin(), states.end(), SortTSOSs(m_edmHelperSvc.get(), m_idHelperSvc.get()));
            ATH_MSG_DEBUG("Filling DataVector with TSOSs " << states.size());
            auto trackStateOnSurfaces = DataVector<const Trk::TrackStateOnSurface>();
            trackStateOnSurfaces.reserve(states.size());
            for (std::unique_ptr<const Trk::TrackStateOnSurface>& sorted : states) { trackStateOnSurfaces.push_back(sorted.release()); }
            ATH_MSG_DEBUG("Creating new Track " << states.size());
            std::unique_ptr<Trk::Track> newTrack = std::make_unique<Trk::Track>(track.info(), std::move(trackStateOnSurfaces),
                                                                                track.fitQuality() ? track.fitQuality()->clone() : nullptr);
            std::unique_ptr<Trk::Track> refittedTrack;
            if (m_onlyEO)
                refittedTrack = std::unique_ptr<Trk::Track>(m_fitter->fit(ctx, *newTrack, m_useFitterOutlierLogic, Trk::muon));
            else
                refittedTrack = std::unique_ptr<Trk::Track>(m_builder->fit(*newTrack, ctx, m_useFitterOutlierLogic, Trk::muon));
            if (refittedTrack) {
                ATH_MSG_DEBUG("New Track " << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));

                // fit ok, add new hashs to lists
                data.mdtTrack.insert(newMdtHashes.begin(), newMdtHashes.end());
                data.rpcTrack.insert(newRpcHashes.begin(), newRpcHashes.end());
                data.tgcTrack.insert(newTgcHashes.begin(), newTgcHashes.end());
                data.cscTrack.insert(newCscHashes.begin(), newCscHashes.end());
                // New Small Wheel
                data.stgcTrack.insert(newsTgcHashes.begin(), newsTgcHashes.end());
                data.mmTrack.insert(newMMHashes.begin(), newMMHashes.end());
            }
            return refittedTrack;
        }
        return nullptr;
    }

    std::unique_ptr<const Trk::TrackParameters> MuonSegmentRegionRecoveryTool::reachableDetEl(const EventContext& ctx,
                                                                                              const Trk::Track& track,
                                                                                              const Trk::TrkDetElementBase& detEl,
                                                                                              bool smallerBounds) const {
        ATH_MSG_VERBOSE("reachableDetEl() " << m_idHelperSvc->toStringDetEl(detEl.identify()) << " at " << detEl.center());
        std::unique_ptr<const Trk::TrackParameters> exPars;
        std::unique_ptr<const Trk::TrackParameters> closest{MuonGetClosestParameters::closestParameters(track, detEl.surface(), true)};
        if (closest) {
            ATH_MSG_VERBOSE("Extrapolating from closest point:\n" << m_printer->print(*closest));
            exPars = m_extrapolator->extrapolateDirectly(ctx, *closest, detEl.surface(), Trk::anyDirection, false, Trk::muon);

        } else {
            ATH_MSG_VERBOSE("Extrapolating from track (no closest point found):\n" << m_printer->print(track));
            exPars = m_extrapolator->extrapolate(ctx, track, detEl.surface(), Trk::anyDirection, false, Trk::muon);
        }
        if (!exPars) {
            ATH_MSG_DEBUG("Extrapolation did not succeed");
            return nullptr;
        }
        Amg::Vector2D locPos(exPars->parameters()[Trk::locX], exPars->parameters()[Trk::locY]);

        double tolx = 100.;  // positive -> large surface
        double toly = 100.;  // positive -> large surface
        const AmgSymMatrix(5)* errMat = exPars->covariance();
        if (errMat) {
            ATH_MSG_VERBOSE("Extrapolated local Position & Error matrix\n:" << locPos << "\n" << Amg::toString(*errMat));
            // MJW check validity of errors before using them (fix for bug #82920)
            double covx = (*errMat)(Trk::locX, Trk::locX);
            double covy = (*errMat)(Trk::locY, Trk::locY);
            if (covx > 0.0 && covy > 0.0) {
                tolx = 3 * covx;
                toly = 3 * covy;
            }
        }

        // in case we are looking for holes, only ignore stations if we are within 100 mm of the chamber edge
        // all other holes are counted even if the extrapolation errors are large.
        if (smallerBounds) {
            if (tolx > 10.) tolx = 10.;
            if (toly > 10.) toly = 10.;
            // refer sign to check 'inside' the bounds
            tolx *= -1.;
            toly *= -1.;
        }
        bool inbounds = detEl.surface().insideBounds(locPos, tolx, toly);
        if (msgLvl(MSG::DEBUG)) {
            std::ostringstream parsType;
            parsType << "pos=(" << locPos[Trk::locX] << "," << locPos[Trk::locY] << ")";
            if (errMat) { parsType << "  exError=(" << Amg::error(*errMat, Trk::locX) << "," << Amg::error(*errMat, Trk::locY) << ")"; }
            parsType << "  tolerance=(" << tolx << "," << toly << ")";
            if (inbounds)
                parsType << " => inbounds";
            else
                parsType << " => outbounds";
            ATH_MSG_DEBUG(" " << m_idHelperSvc->toStringChamber(detEl.identify()) << " pars  " << parsType.str());
        }
        if (!inbounds) { return nullptr; }
        return exPars;
    }

}  // namespace Muon
