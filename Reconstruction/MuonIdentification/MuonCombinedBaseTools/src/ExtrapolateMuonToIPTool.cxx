/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ExtrapolateMuonToIPTool.h"

#include <vector>

#include "TrkParameters/TrackParameters.h"

ExtrapolateMuonToIPTool::ExtrapolateMuonToIPTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p) {
    declareInterface<Muon::IMuonTrackExtrapolationTool>(this);
}

// Initialize method:
StatusCode ExtrapolateMuonToIPTool::initialize() {
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_CHECK(m_muonExtrapolator.retrieve());
    if (!m_edmHelperSvc.empty()) {
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_MSG_DEBUG("Retrieved helper " << m_edmHelperSvc);
    }
    if (!m_printer.empty()) {
        ATH_CHECK(m_printer.retrieve());
        ATH_MSG_DEBUG("Retrieved printer " << m_printer);
    }
    ATH_CHECK(m_trackSummary.retrieve());
    return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode ExtrapolateMuonToIPTool::finalize() {
    double scale = m_nextrapolations != 0 ? 1. / m_nextrapolations : 1.;
    ATH_MSG_INFO("Total number of extrapolations " << m_nextrapolations << " good fraction " << scale * m_nextrapolations
                                                   << " listing failures");
    ATH_MSG_INFO("  no closest parameters           " << scale * m_failedClosestPars);
    ATH_MSG_INFO("  no extrapolation, low pt        " << scale * m_failedExtrapolationLowMom);
    ATH_MSG_INFO("  no extrapolation, high pt       " << scale * m_failedExtrapolationHighMom);
    ATH_MSG_INFO("  problem with perigee creations  " << scale * m_failedPerigeeCreation);
    return StatusCode::SUCCESS;
}

std::unique_ptr<TrackCollection> ExtrapolateMuonToIPTool::extrapolate(const TrackCollection& muonTracks, const EventContext& ctx) const {
    std::unique_ptr<TrackCollection> extrapolateTracks = std::make_unique<TrackCollection>();
    extrapolateTracks->reserve(muonTracks.size());

    ATH_MSG_DEBUG("Extrapolated tracks: " << muonTracks.size());

    // loop over muon tracks and extrapolate them to the IP
    for (const Trk::Track* trk : muonTracks) {
        std::unique_ptr<Trk::Track> extrapolateTrack = extrapolate(*trk, ctx);
        if (!extrapolateTrack) {
            ATH_MSG_DEBUG("Extrapolation of muon to IP failed");
            continue;
        }

        ATH_MSG_DEBUG("Extrapolated track " << m_printer->print(*extrapolateTrack));

        extrapolateTracks->push_back(std::move(extrapolateTrack));
    }
    return extrapolateTracks;
}

std::unique_ptr<Trk::Track>
ExtrapolateMuonToIPTool::extrapolate(const Trk::Track& track,
                                     const EventContext& ctx) const
{

  const Trk::TrackInfo& trackInfo = track.info();
    auto particleType = trackInfo.trackProperties(Trk::TrackInfo::StraightTrack) ? Trk::nonInteracting : Trk::muon;
    const Trk::TrackParameters* closestPars = findMeasuredParametersClosestToIP(track);
    ATH_MSG_DEBUG("Extrapolating track " << m_printer->print(track) << " type " << particleType << std::endl
                                         << m_printer->printStations(track));

    if (!closestPars) {
        ATH_MSG_WARNING("Failed to find closest parameters ");
        ++m_failedClosestPars;
        return nullptr;
    }

    {
        // get perigee parameters
        const Trk::Perigee* perigee = track.perigeeParameters();

        if (!perigee) {
            ATH_MSG_WARNING("Muon Track without perigee, skipping ");
        } else {
            ATH_MSG_DEBUG("Closest parameters " << m_printer->print(*closestPars) << endmsg << " perigee            "
                                                << m_printer->print(*perigee));
        }
    }

    double dirPosProduct = closestPars->position().dot(closestPars->momentum());
    Trk::PropDirection propDir = dirPosProduct < 0. ? Trk::alongMomentum : Trk::oppositeMomentum;

    if (propDir == Trk::alongMomentum) {
        ATH_MSG_DEBUG(" scalar product " << dirPosProduct << "  extrapolating "
                                         << " along momentum");
    } else {
        ATH_MSG_DEBUG(" scalar product " << dirPosProduct << "  extrapolating "
                                         << " opposite momentum");
    }

    Trk::PerigeeSurface perigeeSurface(Amg::Vector3D(0., 0., 0.));
    // extrapolate back to IP
    std::unique_ptr<const Trk::TrackParameters> ipPars{m_extrapolator->extrapolate(ctx, *closestPars, perigeeSurface, propDir, false)};
    if (!ipPars) {
        // if extrapolation failed go in other direction
        propDir = (propDir == Trk::alongMomentum) ? Trk::oppositeMomentum : Trk::alongMomentum;
        ipPars = m_extrapolator->extrapolate(ctx,*closestPars, perigeeSurface, propDir, false, particleType);

        if (propDir == Trk::alongMomentum) {
            ATH_MSG_DEBUG(" retrying opposite momentum extrapolating "
                          << " along momentum");
        } else {
            ATH_MSG_DEBUG(" retrying opposite momentum extrapolating "
                          << " opposite momentum");
        }

        if (!ipPars) {
            if (closestPars->momentum().mag() > 5000.)
                ++m_failedExtrapolationLowMom;
            else
                ++m_failedExtrapolationHighMom;
            return nullptr;
        }
    }

    // create the new track
    // create new perigee
    std::unique_ptr<const Trk::Perigee> ipPerigee_unique;
    const Trk::Perigee* ipPerigee = dynamic_cast<const Trk::Perigee*>(ipPars.get());
    if (!ipPerigee) {
        ipPerigee_unique = createPerigee(*ipPars, ctx);
        ipPerigee = ipPerigee_unique.get();
    }

    if (!ipPerigee) {
        ATH_MSG_WARNING("Failed to create perigee for extrapolate track, skipping ");
        ++m_failedPerigeeCreation;
        return nullptr;
    }

    // create new TSOS DataVector and reserve enough space to fit all old TSOS + one new TSOS
    const DataVector<const Trk::TrackStateOnSurface>* oldTSOT = track.trackStateOnSurfaces();
    auto trackStateOnSurfaces = DataVector<const Trk::TrackStateOnSurface>();
    unsigned int newSize = oldTSOT->size() + 1;
    trackStateOnSurfaces.reserve(newSize);

    Amg::Vector3D perDir = ipPerigee->momentum().unit();

    for (const Trk::TrackStateOnSurface* tsit : *oldTSOT) {
        // remove old perigee if we didn't start from a parameter in the muon system
        if (tsit->type(Trk::TrackStateOnSurface::Perigee)) continue;

        const Trk::TrackParameters* pars = tsit->trackParameters();
        if (!pars) continue;

        if (ipPerigee) {
            double distanceOfPerigeeToCurrent = perDir.dot(pars->position() - ipPerigee->position());

            if (distanceOfPerigeeToCurrent > 0.) {
                std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
                typePattern.set(Trk::TrackStateOnSurface::Perigee);
                trackStateOnSurfaces.push_back(new Trk::TrackStateOnSurface(nullptr, ipPerigee->uniqueClone(), nullptr, nullptr, typePattern));
            }
        }

        // copy remainging TSOS
        trackStateOnSurfaces.push_back(tsit->clone());
    }

    if (ipPerigee) {
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
        typePattern.set(Trk::TrackStateOnSurface::Perigee);
        trackStateOnSurfaces.push_back(new Trk::TrackStateOnSurface(nullptr, ipPerigee->uniqueClone(), nullptr, nullptr, typePattern));
    }
    ATH_MSG_DEBUG(" creating new track ");

    Trk::TrackInfo info(track.info().trackFitter(), track.info().particleHypothesis());
    info.setPatternRecognitionInfo(Trk::TrackInfo::MuidStandAlone);
    // create new track
    std::unique_ptr<Trk::Track> extrapolateTrack = std::make_unique<Trk::Track>(
      info,
      std::move(trackStateOnSurfaces),
      track.fitQuality() ? track.fitQuality()->clone() : nullptr);
    // create track summary
    m_trackSummary->updateTrack(ctx, *extrapolateTrack);
    return extrapolateTrack;
}

const Trk::TrackParameters* ExtrapolateMuonToIPTool::findMeasuredParametersClosestToIP(const Trk::Track& track) const {
    // create new TSOS DataVector and reserve enough space to fit all old TSOS + one new TSOS
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if (!states) return nullptr;

    Trk::PerigeeSurface persurf;
    double rmin{1e9}, rminMeas{1e9};
    const Trk::TrackParameters* closestPars = nullptr;
    const Trk::TrackParameters* closestParsMeas = nullptr;
    for (const Trk::TrackStateOnSurface* tsit : *states) {
        const Trk::TrackParameters* pars = tsit->trackParameters();
        if (!pars) continue;

        double rpars = pars->position().perp();
        if (!closestPars || rpars < rmin) {
            rmin = rpars;
            closestPars = pars;
        }

        if (pars->covariance() && (!closestParsMeas || rpars < rminMeas)) {
            rminMeas = rpars;
            closestParsMeas = pars;
        }
    }

    if (closestParsMeas) {
        return closestParsMeas;
    } else {
        ATH_MSG_DEBUG(" No measured closest parameters found, using none measured parameters");
    }
    return closestPars;
}

std::unique_ptr<const Trk::Perigee> ExtrapolateMuonToIPTool::createPerigee(const Trk::TrackParameters& pars,
                                                                           const EventContext& ctx) const {
    std::unique_ptr<const Trk::Perigee> perigee;
    if (m_muonExtrapolator.empty()) { return perigee; }
    Trk::PerigeeSurface persurf(pars.position());
    const Trk::TrackParameters* exPars = m_muonExtrapolator->extrapolateDirectly(ctx, pars, persurf).release();
    const Trk::Perigee* pp = dynamic_cast<const Trk::Perigee*>(exPars);
    if (!pp) {
        ATH_MSG_WARNING(" Extrapolation to Perigee surface did not return a perigee!! ");
        delete exPars;
        return perigee;
    }
    perigee.reset(pp);
    return perigee;
}
