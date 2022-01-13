/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonTrackExtrapolationTool.h"

#include <map>

#include "MuonTrackMakerUtils/MuonTSOSHelper.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSurfaces/DistanceSolution.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkVolumes/BoundarySurface.h"

namespace Muon {

    MuonTrackExtrapolationTool::MuonTrackExtrapolationTool(const std::string &ty, const std::string &na, const IInterface *pa) :
        AthAlgTool(ty, na, pa) {
        declareInterface<IMuonTrackExtrapolationTool>(this);
    }

    StatusCode MuonTrackExtrapolationTool::initialize() {
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_idHelperSvc.retrieve());
        if (!m_atlasExtrapolator.empty()) ATH_CHECK(m_atlasExtrapolator.retrieve());
        if (!m_muonExtrapolator.empty()) ATH_CHECK(m_muonExtrapolator.retrieve());
        if (!m_muonExtrapolator2.empty()) ATH_CHECK(m_muonExtrapolator2.retrieve());
        
        if (!m_trackingGeometryReadKey.empty()) {
            ATH_CHECK(m_trackingGeometryReadKey.initialize());
        } else {
            ATH_MSG_ERROR("Could not retrieve a valid tracking geometry");
        }
        ATH_CHECK(m_fieldCacheCondObjInputKey.initialize());

        if (m_cosmics) ATH_MSG_DEBUG("Running in cosmics mode");

        return StatusCode::SUCCESS;
    }

    const Trk::TrackParameters *MuonTrackExtrapolationTool::extrapolateToMuonEntryRecord(const EventContext& ctx, const Trk::TrackParameters &pars,
                                                                                         Trk::ParticleHypothesis particleHypo) const {
        if (m_muonExtrapolator.empty()) return nullptr;
        const Trk::TrackingVolume *msEntrance = getVolume(m_msEntranceName, ctx);

        if (!msEntrance) {
            ATH_MSG_WARNING("  MS entrance not found");
            return nullptr;
        }

        Trk::PropDirection dir = Trk::oppositeMomentum;
        if (msEntrance->inside(pars.position())) { dir = Trk::alongMomentum; }

        if (m_cosmics) {
            // for cosmics try both directions
            const Trk::TrackParameters *entryPars =
                m_muonExtrapolator->extrapolateToVolume(pars, *msEntrance, Trk::oppositeMomentum, particleHypo);
            if (!entryPars) {
                ATH_MSG_VERBOSE(" failed to extrapolate opposite momentum");
                // retry in other direction
                entryPars = m_muonExtrapolator->extrapolateToVolume(pars, *msEntrance, Trk::alongMomentum, particleHypo);
                if (!entryPars) ATH_MSG_VERBOSE(" failed to extrapolate along momentum for the second trial");
            }
            return entryPars;
        }

        return m_muonExtrapolator->extrapolateToVolume(pars, *msEntrance, dir, particleHypo);
    }

    const Trk::TrackParameters *MuonTrackExtrapolationTool::extrapolateToIP(const Trk::TrackParameters &pars,
                                                                            Trk::ParticleHypothesis particleHypo) const {
        if (m_atlasExtrapolator.empty()) return nullptr;

        // temporary hack to avoid crashes in Muid.
        Amg::Vector3D refPos(0.1, 0.1, 0.1);
        Trk::PerigeeSurface persurf(refPos);

        double dirPosProduct = (pars.position() - refPos).dot(pars.momentum());
        Trk::PropDirection propDir = dirPosProduct < 0. ? Trk::alongMomentum : Trk::oppositeMomentum;

        ATH_MSG_VERBOSE(" extrapolating to IP: " << m_printer->print(pars) << std::setprecision(2) << " pos " << pars.position() << " dir "
                                                 << pars.momentum().unit()
                                                 << (propDir == Trk::alongMomentum      ? " going along momentum"
                                                     : propDir == Trk::oppositeMomentum ? " going opposite momentum"
                                                                                        : ""));

        // for cosmics try both directions
        const Trk::TrackParameters *entryPars = m_atlasExtrapolator->extrapolate(pars, persurf, propDir, false, particleHypo);
        if (!entryPars) ATH_MSG_VERBOSE(" failed to extrapolate to IP");

        if (m_cosmics && !entryPars) {
            // flip propagation direction and retry in other direction
            propDir = propDir == Trk::alongMomentum ? Trk::oppositeMomentum : Trk::alongMomentum;
            entryPars = m_atlasExtrapolator->extrapolate(pars, persurf, propDir, false, particleHypo);
            if (!entryPars) ATH_MSG_VERBOSE(" failed to extrapolate to IP in opposite direction");
        }

        return entryPars;
    }

    const Trk::TrackParameters *MuonTrackExtrapolationTool::findClosestParametersToMuonEntry(const EventContext& ctx, const Trk::Track &track) const {
        const Trk::TrackingVolume *msEntrance = getVolume(m_msEntranceName, ctx);

        if (!msEntrance) {
            ATH_MSG_WARNING("Failed to obtain muon entry volume");
            return nullptr;
        }
        const Trk::Perigee *pp = track.perigeeParameters();
        if (!pp) return nullptr;

        const Trk::TrackParameters *closestPars = pp;
        const Trk::TrackParameters *closestMeasPars = pp->covariance() ? pp : nullptr;

        double perp = pp->associatedSurface().center().perp();
        double z = pp->associatedSurface().center().z();
        ATH_MSG_DEBUG(" perigee surface position: r  " << perp << "  z " << z);

        if (m_cosmics) {
            // for cosmics we have to find the closest track parameters to the muon entry record

            double minDistance = 1e9;
            double minMeasDistance = 1e9;

            ATH_MSG_DEBUG(" Cosmic model, starting for measurement closest to muon entry record ");
            const DataVector<const Trk::TrackStateOnSurface> *oldTSOT = track.trackStateOnSurfaces();
            for (const Trk::TrackStateOnSurface *surf : *oldTSOT) {
                const Trk::TrackParameters *pars = surf->trackParameters();
                if (!pars) { continue; }

                // drop states without measurement
                if (!surf->measurementOnTrack()) continue;

                double distance = pars->position().perp();  // estimateDistanceToEntryRecord(ctx, *pars);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestPars = pars;
                }

                if (pars->covariance() && distance < minMeasDistance) {
                    minMeasDistance = distance;
                    closestMeasPars = pars;
                }
                const Trk::MeasurementBase *meas = surf->measurementOnTrack();
                ATH_MSG_VERBOSE(" Dist: " << std::setw(6) << (int)distance << " r " << std::setw(5) << (int)pars->position().perp() << " z "
                                          << std::setw(6) << (int)pars->position().z() << (pars->covariance() ? " measured" : "")
                                          << (meas ? m_idHelperSvc->toString(m_edmHelperSvc->getIdentifier(*meas)) : ""));
            }

        } else if (perp < 1000. && std::abs(z) < 1000.) {
            ATH_MSG_VERBOSE(" track at IP, starting from closest measurement in muon spectrometer ");
            const DataVector<const Trk::TrackStateOnSurface> *oldTSOT = track.trackStateOnSurfaces();
            for (const Trk::TrackStateOnSurface *surf : *oldTSOT) {
                const Trk::TrackParameters *pars = surf->trackParameters();
                if (!pars) { continue; }

                // check whether state is a measurement
                const Trk::MeasurementBase *meas = surf->measurementOnTrack();
                if (!meas) { continue; }

                const Trk::PseudoMeasurementOnTrack *pseudo = dynamic_cast<const Trk::PseudoMeasurementOnTrack *>(meas);
                if (pseudo) { continue; }

                Identifier id = m_edmHelperSvc->getIdentifier(*meas);
                if (!m_idHelperSvc->isMuon(id)) continue;

                closestPars = pars;

                break;
            }
        }

        // if we have MeasuredTrackParameters, use those, else return pars
        if (closestMeasPars) {
            ATH_MSG_DEBUG(" closest measured parameters: " << m_printer->print(*closestMeasPars));
            return closestMeasPars;
        }
        if (closestPars) ATH_MSG_DEBUG(" closest parameters: " << m_printer->print(*closestPars));

        return closestPars;
    }

    double MuonTrackExtrapolationTool::estimateDistanceToEntryRecord(const EventContext& ctx, const Trk::TrackParameters &pars) const {
        const Trk::TrackingVolume *msEntrance = getVolume("Calo::Container", ctx);

        if (!msEntrance) return 0;

        // get boundary surfaces of muon entry record
        const std::vector<Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > > &boundarySurfs =
            msEntrance->boundarySurfaces();

        double minDistance = 1e9;

        // loop over surfaces and find the closest to the parameters
        for (const auto &bit : boundarySurfs) {
            const Trk::Surface &surf = bit->surfaceRepresentation();
            Trk::DistanceSolution solution = surf.straightLineDistanceEstimate(pars.position(), pars.momentum());
            double distance = solution.currentDistance();
            if (distance < minDistance) minDistance = distance;
        }
        return minDistance;
    }

    const Trk::TrackParameters *MuonTrackExtrapolationTool::checkForSecondCrossing(const Trk::TrackParameters &firstCrossing,
                                                                                   const Trk::Track &track) const {
        double dirPosProduct = firstCrossing.position().dot(firstCrossing.momentum());
        double sign = dirPosProduct < 0. ? 1. : -1.;
        // create new TSOS DataVector and reserve enough space to fit all old TSOS + one new TSOS
        const DataVector<const Trk::TrackStateOnSurface> *oldTSOT = track.trackStateOnSurfaces();

        Amg::Vector3D perDir = firstCrossing.momentum().unit();
        double minDistance = 1e9;
        double minMeasDistance = 1e9;
        const Trk::TrackParameters *closestPars = nullptr;
        const Trk::TrackParameters *closestMeasPars = nullptr;

        for (const Trk::TrackStateOnSurface *surf : *oldTSOT) {
            // do not consider perigee
            if (surf->type(Trk::TrackStateOnSurface::Perigee)) continue;

            // drop states without parameters
            const Trk::TrackParameters *pars = surf->trackParameters();
            if (!pars) continue;

            // drop states without measurement
            if (!surf->measurementOnTrack()) continue;

            double distanceOfPerigeeToCurrent = (pars->position() - firstCrossing.position()).dot(perDir);

            if (sign * distanceOfPerigeeToCurrent > 0.) {
                double distance = std::abs(distanceOfPerigeeToCurrent);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestPars = pars;
                }
                if (pars->covariance() && distance < minMeasDistance) {
                    minMeasDistance = distance;
                    closestMeasPars = pars;
                }
            }
        }
        // if we have MeasuredTrackParameters, use those, else return pars
        return closestMeasPars ? closestMeasPars : closestPars;
    }

    std::unique_ptr<Trk::Track> MuonTrackExtrapolationTool::extrapolate(const Trk::Track &track, const EventContext &ctx) const {
        if (m_muonExtrapolator.empty()) return nullptr;
        // if straightline track and the field is on return nullptr
        bool isSL = m_edmHelperSvc->isSLTrack(track);
        if (isSL) {  // check isSL first to limit access overhead
            MagField::AtlasFieldCache fieldCache;
            // Get field cache object
            SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
            const AtlasFieldCacheCondObj *fieldCondObj{*readHandle};

            if (!fieldCondObj) {
                ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
                return nullptr;
            }
            fieldCondObj->getInitializedCache(fieldCache);
            if (fieldCache.toroidOn()) { return nullptr; }
        }

        const Trk::Perigee *pp = track.perigeeParameters();
        if (!pp) return nullptr;

        const Trk::TrackParameters *firstPars = findClosestParametersToMuonEntry(ctx, track);
        if (!firstPars) {
            ATH_MSG_WARNING("failed to find closest parameters to muon entry ");
            return nullptr;
        }

        // extrapolate to muon entry record
        Trk::ParticleHypothesis particleHypo = track.info().particleHypothesis();
        if (isSL) particleHypo = Trk::nonInteracting;
        const Trk::TrackParameters *exPars = extrapolateToMuonEntryRecord(ctx, *firstPars, particleHypo);

        bool atIP = false;
        if (!exPars) {
            ATH_MSG_DEBUG("failed to extrapolate parameters to muon entry, trying perigee ");

            // for cosmics also try extrapolate to IP
            if (m_cosmics) {
                exPars = extrapolateToIP(*firstPars, particleHypo);
                atIP = true;
            }
        }
        if (!exPars) {
            // check mometum, this should always work for high pt track but low momentum track could get stuck
            if (firstPars->momentum().mag() < 7000.)
                ATH_MSG_DEBUG("lower energy muon lost during extrapolation ");
            else
                ATH_MSG_WARNING("failed to extrapolate parameters to muon entry and perigee ");
            return nullptr;
        }

        // sanity check for cosmics, if we are at the IP we should not
        if (m_cosmics && atIP) {
            double tolerance = -50.;
            const Trk::TrackingVolume *msEntrance = getVolume("Calo::Container",ctx);

            if (msEntrance && msEntrance->inside(exPars->position(), tolerance)) {
                ATH_MSG_DEBUG("extrapolate parameters at perigee inside muon entry volume " << m_printer->print(*exPars));
            }
        }

        ATH_MSG_DEBUG(" first pars:       " << m_printer->print(*firstPars) << endmsg << " extrapolated pars " << m_printer->print(*exPars)
                                            << endmsg);
        // create new perigee
        const Trk::Perigee *perigee = dynamic_cast<const Trk::Perigee *>(exPars);
        if (!perigee) {
            perigee = createPerigee(*exPars);
            delete exPars;
        }

        // double check
        if (!perigee) {
            ATH_MSG_WARNING(" failed to create perigee ");
            return nullptr;
        }

        // direction of perigee
        Amg::Vector3D perDir = perigee->momentum().unit();

        // for cosmics we could have hits on both side of the muon entry volume.
        // check whether that is the case and calculate a second perigee in that case
        const Trk::Perigee *secondPerigee = nullptr;
        if (m_cosmics && !atIP) {
            ATH_MSG_DEBUG(" trying to calculate second crossing ");

            const Trk::TrackParameters *secondEntryCrossing = checkForSecondCrossing(*perigee, track);
            if (secondEntryCrossing) {
                ATH_MSG_DEBUG(" Expect second crossing ");

                // create second perigee
                const Trk::TrackParameters *secondExPars = extrapolateToMuonEntryRecord(ctx, *secondEntryCrossing, particleHypo);
                if (secondExPars) {
                    // check distence to first perigee
                    double distance = (secondExPars->position() - perigee->position()).dot(perDir);
                    ATH_MSG_DEBUG(" second crossing: " << m_printer->print(*secondExPars) << " distance to first " << distance);
                    if (std::abs(distance) < 1.) {
                        ATH_MSG_DEBUG(" second perigee too close to first: " << m_printer->print(*secondExPars));
                        delete secondExPars;
                    } else {
                        // create new perigee
                        secondPerigee = dynamic_cast<const Trk::Perigee *>(secondExPars);
                        if (!secondPerigee) {
                            secondPerigee = createPerigee(*secondExPars);
                            delete secondExPars;
                        }
                    }
                } else {
                    ATH_MSG_DEBUG(" Extrapolation to muon entry failed for second crossing ");
                }
            }
        }

        ATH_MSG_DEBUG(" perigee pars:        " << m_printer->print(*perigee));

        // flag whether the perigees were inserted
        bool perigeeWasInserted = false;
        bool secondPerigeeWasInserted = false;

        // flag whether the perigees are pointing towards the IP
        bool perigeePointsToIP = perigee->position().dot(perDir) < 0.;
        bool secondPerigeePointsToIP = false;
        if (secondPerigee) {
            ATH_MSG_DEBUG(" second perigee pars: " << m_printer->print(*secondPerigee));
            secondPerigeePointsToIP = secondPerigee->position().dot(secondPerigee->momentum()) < 0.;
            if (perigeePointsToIP == secondPerigeePointsToIP) {
                ATH_MSG_DEBUG(" Track has two perigee's with the same orientation with respect to the IP ");
            }
        }

        // create new TSOS DataVector and reserve enough space to fit all old TSOS + one new TSOS
        const DataVector<const Trk::TrackStateOnSurface> *oldTSOT = track.trackStateOnSurfaces();
        auto trackStateOnSurfaces = DataVector<const Trk::TrackStateOnSurface>();
        unsigned int newSize = oldTSOT->size();
        trackStateOnSurfaces.reserve(newSize + 11);

        DataVector<const Trk::TrackStateOnSurface>::const_iterator tit = oldTSOT->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator tit_prev = tit;  // iterator pointing to the previous TSOS
        DataVector<const Trk::TrackStateOnSurface>::const_iterator tit_end = oldTSOT->end();
        for (; tit != tit_end; ++tit) {
            // remove old perigee if we didn't start from a parameter in the muon system
            if ((*tit)->trackParameters() == pp) {
                if (m_keepOldPerigee) {
                    const Amg::VectorX &ppars = pp->parameters();
                    Amg::Transform3D ptrans = Amg::Transform3D(pp->associatedSurface().transform());
                    Trk::StraightLineSurface slSurf(ptrans);
                    Trk::AtaStraightLine *slPars = new Trk::AtaStraightLine(ppars[Trk::locR], ppars[Trk::locZ], ppars[Trk::phi],
                                                                            ppars[Trk::theta], ppars[Trk::qOverP], slSurf);
                    trackStateOnSurfaces.push_back(MuonTSOSHelper::createPerigeeTSOS(slPars));
                }
                continue;
            }
            const Trk::TrackParameters *pars = (*tit)->trackParameters();
            if (!pars) {
                // keep state but do not consider any further
                trackStateOnSurfaces.push_back((*tit)->clone());
                continue;
            }

            double distanceOfPerigeeToCurrent = (pars->position() - perigee->position()).dot(perDir);

            ATH_MSG_VERBOSE(" new state " << m_printer->print(*pars) << " dist to perigee " << distanceOfPerigeeToCurrent);

            if (!perigeeWasInserted && distanceOfPerigeeToCurrent > 0.) {
                ATH_MSG_VERBOSE(" inserting first perigee " << m_printer->print(*perigee) << " dist to prev "
                                                            << distanceOfPerigeeToCurrent);

                // check whether we should add material between the previous TSOS and the new perigee
                // make sure that we are not at the same TSOS (could happen if this were the first TSOS
                if (!atIP) {
                    ATH_MSG_VERBOSE(" not at IP ");

                    // check direction of perigee wrt IP
                    if (perigeePointsToIP) {
                        // perigee points to the IP so we have to collect material between the perigee and the previous measurement

                        ATH_MSG_VERBOSE(" perigee points towards IP, inserting material first ");

                        // check whether the previous state is a measurement, else we will assume the material is there
                        if (tit_prev != tit) {
                            const Trk::MeasurementBase *meas = (*tit_prev)->measurementOnTrack();
                            if (meas) {
                                ATH_MSG_VERBOSE(" trying to adding material layers extrapolating to previous measurement ");

                                // collect the material going in opposite direction
                                const std::vector<const Trk::TrackStateOnSurface *> *matvec = m_muonExtrapolator->extrapolateM(
                                    *perigee, meas->associatedSurface(), Trk::oppositeMomentum, false, particleHypo);
                                if (matvec && !matvec->empty()) {
                                    ATH_MSG_VERBOSE(" got material layers " << matvec->size());

                                    trackStateOnSurfaces.insert(trackStateOnSurfaces.end(), matvec->begin(), matvec->end());
                                } else {
                                    ATH_MSG_VERBOSE(" no layers obtained from extrapolator ");
                                }
                                delete matvec;
                            }
                        } else {
                            ATH_MSG_VERBOSE(" first measurement, cannot allocated material ");
                        }
                    } else {
                        // we have to collect material from the perigee to the next measurement

                        ATH_MSG_VERBOSE(" perigee points away from IP, inserting perigee ");

                        // first add perigee
                        trackStateOnSurfaces.push_back(MuonTSOSHelper::createPerigeeTSOS(perigee));
                        perigeeWasInserted = true;

                        // 	    // now look whether there are measurements upstream of this point add add material if needed
                        // 	    DataVector<const Trk::TrackStateOnSurface>::const_iterator tit_next = tit; ++tit_next;
                        // 	    if( tit_next != tit_end ){

                        // check whether a measurement, else we will assume the material is there
                        const Trk::MeasurementBase *meas = (*tit)->measurementOnTrack();
                        if (meas) {
                            ATH_MSG_VERBOSE(" trying to add material layers extrapolating to next measurement  ");
                            const std::vector<const Trk::TrackStateOnSurface *> *matvec = m_muonExtrapolator->extrapolateM(
                                *perigee, meas->associatedSurface(), Trk::alongMomentum, false, particleHypo);
                            if (matvec && !matvec->empty()) {
                                ATH_MSG_VERBOSE(" got material layers " << matvec->size());

                                trackStateOnSurfaces.insert(trackStateOnSurfaces.end(), matvec->begin(), matvec->end());
                            } else {
                                ATH_MSG_VERBOSE(" no layers obtained from extrapolator ");
                            }
                            delete matvec;
                        }
                    }
                }

                // check whether we did not insert the perigee, if not insert
                if (!perigeeWasInserted) {
                    trackStateOnSurfaces.push_back(MuonTSOSHelper::createPerigeeTSOS(perigee));
                    perigeeWasInserted = true;
                    ATH_MSG_VERBOSE(" inserting perigee ");
                }
            }
            if (secondPerigee) {
                double distanceOfSecondPerigeeToCurrent =
                    (pars->position() - secondPerigee->position()).dot(secondPerigee->momentum().unit());
                if (!secondPerigeeWasInserted && distanceOfSecondPerigeeToCurrent > 0.) {
                    // hack copied code should be put into a function

                    ATH_MSG_VERBOSE(" inserting second perigee " << m_printer->print(*secondPerigee) << " dist to prev "
                                                                 << distanceOfSecondPerigeeToCurrent);

                    // check direction of perigee wrt IP
                    if (secondPerigeePointsToIP) {
                        // perigee points to the IP so we have to collect material between the perigee and the previous measurement

                        ATH_MSG_VERBOSE(" perigee points towards IP, inserting material first ");

                        // check whether the previous state is a measurement, else we will assume the material is there
                        if (tit_prev != tit) {
                            const Trk::MeasurementBase *meas = (*tit_prev)->measurementOnTrack();
                            if (meas) {
                                ATH_MSG_VERBOSE(" trying to adding material layers extrapolating to previous measurement ");

                                // collect the material going in opposite direction
                                const std::vector<const Trk::TrackStateOnSurface *> *matvec = m_muonExtrapolator->extrapolateM(
                                    *secondPerigee, meas->associatedSurface(), Trk::oppositeMomentum, false, particleHypo);
                                if (matvec && !matvec->empty()) {
                                    ATH_MSG_VERBOSE(" got material layers " << matvec->size());

                                    trackStateOnSurfaces.insert(trackStateOnSurfaces.end(), matvec->begin(), matvec->end());
                                } else {
                                    ATH_MSG_VERBOSE(" no layers obtained from extrapolator ");
                                }
                                delete matvec;
                            }
                        }
                    } else {
                        // we have to collect material from the perigee to the next measurement

                        ATH_MSG_VERBOSE(" perigee points away from IP, inserting perigee ");

                        // first add perigee
                        trackStateOnSurfaces.push_back(MuonTSOSHelper::createPerigeeTSOS(secondPerigee));
                        secondPerigeeWasInserted = true;

                        // 	    // now look whether there are measurements upstream of this point add add material if needed
                        // 	    DataVector<const Trk::TrackStateOnSurface>::const_iterator tit_next = tit; ++tit_next;
                        // 	    if( tit_next != tit_end ){
                        // check whether a measurement, else we will assume the material is there
                        const Trk::MeasurementBase *meas = (*tit)->measurementOnTrack();
                        if (meas) {
                            ATH_MSG_VERBOSE(" trying to add material layers extrapolating to next measurement  ");
                            const std::vector<const Trk::TrackStateOnSurface *> *matvec = m_muonExtrapolator->extrapolateM(
                                *secondPerigee, meas->associatedSurface(), Trk::alongMomentum, false, particleHypo);
                            if (matvec && !matvec->empty()) {
                                ATH_MSG_VERBOSE(" got material layers " << matvec->size());

                                trackStateOnSurfaces.insert(trackStateOnSurfaces.end(), matvec->begin(), matvec->end());
                            } else {
                                ATH_MSG_VERBOSE(" no layers obtained from extrapolator ");
                            }
                            delete matvec;
                        }
                    }

                    // check whether we did not insert the perigee, if not insert
                    if (!secondPerigeeWasInserted) {
                        trackStateOnSurfaces.push_back(MuonTSOSHelper::createPerigeeTSOS(secondPerigee));
                        secondPerigeeWasInserted = true;
                        ATH_MSG_VERBOSE(" inserting second perigee ");
                    }
                }
            }

            // copy TSOS
            trackStateOnSurfaces.push_back((*tit)->clone());

            // update iterator previous TSTO
            tit_prev = tit;
        }

        if (!perigeeWasInserted) {
            // check whether the previous state is a measurement, else we will assume the material is there
            if (tit_prev != tit_end) {
                const Trk::MeasurementBase *meas = (*tit_prev)->measurementOnTrack();
                if (meas) {
                    ATH_MSG_VERBOSE(" trying to adding material layers extrapolating to previous measurement ");

                    // collect the material going in opposite direction
                    const std::vector<const Trk::TrackStateOnSurface *> *matvec =
                        m_muonExtrapolator->extrapolateM(*perigee, meas->associatedSurface(), Trk::oppositeMomentum, false, particleHypo);
                    if (matvec && !matvec->empty()) {
                        ATH_MSG_VERBOSE(" got material layers " << matvec->size());

                        trackStateOnSurfaces.insert(trackStateOnSurfaces.end(), matvec->begin(), matvec->end());
                    } else {
                        ATH_MSG_VERBOSE(" no layers obtained from extrapolator ");
                    }
                    delete matvec;
                }
            }
            ATH_MSG_VERBOSE(" inserting perigee ");
            trackStateOnSurfaces.push_back(MuonTSOSHelper::createPerigeeTSOS(perigee));
        }
        if (secondPerigee && !secondPerigeeWasInserted) {
            // check whether the previous state is a measurement, else we will assume the material is there
            if (tit_prev != tit_end) {
                const Trk::MeasurementBase *meas = (*tit_prev)->measurementOnTrack();
                if (meas) {
                    ATH_MSG_VERBOSE(" trying to adding material layers extrapolating to previous measurement ");

                    // collect the material going in opposite direction
                    const std::vector<const Trk::TrackStateOnSurface *> *matvec = m_muonExtrapolator->extrapolateM(
                        *secondPerigee, meas->associatedSurface(), Trk::oppositeMomentum, false, particleHypo);
                    if (matvec && !matvec->empty()) {
                        ATH_MSG_VERBOSE(" got material layers " << matvec->size());

                        trackStateOnSurfaces.insert(trackStateOnSurfaces.end(), matvec->begin(), matvec->end());
                    } else {
                        ATH_MSG_VERBOSE(" no layers obtained from extrapolator ");
                    }
                    delete matvec;
                }
            }
            ATH_MSG_VERBOSE(" inserting second perigee ");
            trackStateOnSurfaces.push_back(MuonTSOSHelper::createPerigeeTSOS(secondPerigee));
        }

        // create new track
        return std::make_unique<Trk::Track>(
          track.info(),
          std::move(trackStateOnSurfaces),
          track.fitQuality() ? track.fitQuality()->clone() : nullptr);
    }

    std::unique_ptr<TrackCollection> MuonTrackExtrapolationTool::extrapolate(const TrackCollection &tracks, const EventContext &ctx) const {
        std::unique_ptr<TrackCollection> extrapolateTracks = std::make_unique<TrackCollection>();
        extrapolateTracks->reserve(tracks.size());

        // loop over muon tracks and extrapolate them to the IP
        for (const Trk::Track *tit : tracks) {
            std::unique_ptr<Trk::Track> extrapolateTrack = extrapolate(*tit, ctx);
            if (!extrapolateTrack) { continue; }

            extrapolateTracks->push_back(std::move(extrapolateTrack));
        }
        return extrapolateTracks;
    }

    const Trk::Perigee *MuonTrackExtrapolationTool::createPerigee(const Trk::TrackParameters &pars) const {
        if (m_muonExtrapolator2.empty()) { return nullptr; }
        Trk::PerigeeSurface persurf(pars.position());
        const Trk::TrackParameters *exPars = m_muonExtrapolator2->extrapolateDirectly(pars, persurf);
        const Trk::Perigee *pp = dynamic_cast<const Trk::Perigee *>(exPars);
        if (!pp) {
            ATH_MSG_WARNING(" Extrapolation to Perigee surface did not return a perigee!! ");
            delete exPars;
            return nullptr;
        }
        return pp;
    }
}  // namespace Muon
