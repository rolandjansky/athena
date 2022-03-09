/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidTrackIsolation
//  AlgTool for estimating the number, total charged momentum and most
//  energetic inner detector tracks in a cone surrounding a muon
//
//////////////////////////////////////////////////////////////////////////////

#include "MuidTrackIsolation.h"

#include <cmath>
#include <iomanip>

#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
namespace Rec {

    MuidTrackIsolation::MuidTrackIsolation(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent), m_etaSafetyFactor(0.1) {
        declareInterface<IMuidTrackIsolation>(this);
    }

    //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

    StatusCode MuidTrackIsolation::initialize() {
        ATH_MSG_INFO("MuidTrackIsolation::initialize()");

        // get the Tools
        ATH_CHECK(m_intersector.retrieve());
        m_trackCone2 = m_trackCone * m_trackCone;
        // create the calo barrel surfaces (cylinder) and 2 endcap discs)
        double radius = 2.0 * Gaudi::Units::meter;
        double halfLength = 4.0 * Gaudi::Units::meter;
        Amg::Transform3D transform;
        transform.setIdentity();
        m_caloCylinder = std::make_unique<Trk::CylinderSurface>(transform, radius, halfLength);

        // the corresponding max barrel cotTheta
        m_barrelCotTheta = halfLength / radius;

        // and the forward/backward endcap disks
        Amg::Transform3D discRotation;
        discRotation.setIdentity();
        Amg::Vector3D forwardDiscPosition(0., 0., halfLength);
        auto transform1 = std::make_unique<Amg::Transform3D>(discRotation * forwardDiscPosition);
        m_caloForwardDisc = std::make_unique<Trk::DiscSurface>(*transform1, 0., radius);
        Amg::Vector3D backwardDiscPosition(0., 0., -halfLength);
        auto transform2 = std::make_unique<Amg::Transform3D>(discRotation * backwardDiscPosition);
        m_caloBackwardDisc = std::make_unique<Trk::DiscSurface>(*transform2, 0., radius);

        ATH_CHECK(m_inDetTracksLocation.initialize());

        return StatusCode::SUCCESS;
    }
    std::pair<int, double> MuidTrackIsolation::trackIsolation(const EventContext& ctx, double eta, double phi) const {
        // debug input quantities
        ATH_MSG_DEBUG(" MuidTrackIsolation:: " << std::setiosflags(std::ios::fixed)
                                               << (m_trackExtrapolation ? "applied after extrapolation to calo, " : "applied at perigee, ")
                                               << " for muon at calo with eta,phi " << std::setw(8) << std::setprecision(3) << eta
                                               << std::setw(8) << std::setprecision(3) << phi);

        // set initial state
        std::pair<int, double> isolation{0, 0.};

        // retrieve track collection
        SG::ReadHandle<TrackCollection> inDetTracks(m_inDetTracksLocation, ctx);
        if (!inDetTracks.isPresent()) {
            ATH_MSG_DEBUG(" no ID Track container at location  " << m_inDetTracksLocation.key());
            return isolation;
        }

        if (!inDetTracks.isValid()) {
            ATH_MSG_WARNING(" ID Track container " << m_inDetTracksLocation.key() << " not valid!");
            return isolation;
        }

        // evaluate isolation according to configuration
        if (m_trackExtrapolation) {
            isolation = trackExtrapolated(inDetTracks.cptr(), eta, phi);
        } else {
            isolation = trackVertex(inDetTracks.cptr(), eta, phi);
        }

        // debug result
        ATH_MSG_DEBUG("Found " << isolation.first << std::setiosflags(std::ios::fixed) << " InDet tracks with total momentum "
                               << std::setw(8) << std::setprecision(1) << isolation.second / Gaudi::Units::GeV << " GeV");

        return isolation;
    }

    std::pair<int, double> MuidTrackIsolation::trackVertex(const TrackCollection* inDetTracks, double eta, double phi) const {
        // set initial state
        double sumP = 0.;
        int numberTracks = 0;

        // choose tracks in cone
        for (const Trk::Track* id : *inDetTracks) {
            const Trk::Perigee& perigee = *id->perigeeParameters();
            if (id->info().trackProperties(Trk::TrackInfo::StraightTrack) || perigee.pT() < m_minPt) continue;

            double inDetPhi = perigee.parameters()[Trk::phi];
            double inDetEta = perigee.eta();

            double diffEta = std::abs(eta - inDetEta);
            double diffPhi = xAOD::P4Helpers::deltaPhi(phi, inDetPhi);

            ATH_MSG_DEBUG(std::endl
                          << std::setiosflags(std::ios::fixed) << " Id track: momentum " << std::setw(8) << std::setprecision(1)
                          << perigee.momentum().mag() / Gaudi::Units::GeV << "  with perigee eta and difference " << std::setw(8)
                          << std::setprecision(3) << perigee.eta() << std::setw(8) << std::setprecision(3) << diffEta
                          << "  and same for phi " << std::setw(8) << std::setprecision(3) << perigee.parameters()[Trk::phi] << std::setw(8)
                          << std::setprecision(3) << diffPhi);

            if ((diffPhi * diffPhi + diffEta * diffEta) > m_trackCone2) continue;
            ++numberTracks;
            const double p = perigee.momentum().mag();
            sumP += p;

            ATH_MSG_VERBOSE("inside cone, track#" << std::setw(3) << numberTracks);
        }

        return std::make_pair(numberTracks, sumP);
    }

    std::pair<int, double> MuidTrackIsolation::trackExtrapolated(const TrackCollection* inDetTracks, double eta, double phi) const {
        // set initial state
        double sumP = 0.;
        int numberTracks = 0;

        // extrapolate close in eta tracks to calorimeter surface
        for (const Trk::Track* id : *inDetTracks) {
            const Trk::Perigee& perigee = *id->perigeeParameters();
            if (id->info().trackProperties(Trk::TrackInfo::StraightTrack) || perigee.pT() < m_minPt) continue;

            double inDetEta = perigee.eta();
            if (std::abs(eta - inDetEta) > m_trackCone + m_etaSafetyFactor) continue;

            // track has sufficient momentum and is close in eta:
            // find intersection at calo surface
            double qOverP = perigee.parameters()[Trk::qOverP];
            /// Use identiy of cot (x) = tan( pi/2 - x)
            double cotTheta = std::tan(M_PI_2 - perigee.parameters()[Trk::theta]);
            Amg::Vector3D direction(std::cos(perigee.parameters()[Trk::phi]), std::sin(perigee.parameters()[Trk::phi]), cotTheta);
            direction /= direction.mag();

            const Trk::TrackSurfaceIntersection idIntersection(perigee.position(), direction, 0.);
            const Trk::Surface* surface = m_caloCylinder.get();
            if (cotTheta > m_barrelCotTheta) {
                surface = m_caloForwardDisc.get();
            } else if (cotTheta < -m_barrelCotTheta) {
                surface = m_caloBackwardDisc.get();
            }
            std::unique_ptr<const Trk::TrackSurfaceIntersection> caloIntersection(
                m_intersector->intersectSurface(*surface, &idIntersection, qOverP));

            // no intersection - should never happen !
            if (!caloIntersection) {
                ATH_MSG_DEBUG(" track didn't find intersection !!! "
                              << std::setiosflags(std::ios::fixed) << " Id track: momentum " << std::setw(8) << std::setprecision(1)
                              << perigee.momentum().mag() / Gaudi::Units::GeV << "  with initial eta " << std::setw(8)
                              << std::setprecision(3) << perigee.eta() << "  and phi " << std::setw(8) << std::setprecision(3)
                              << perigee.parameters()[Trk::phi]);

                continue;
            }

            double diffEta = eta - caloIntersection->position().eta();
            double diffPhi = xAOD::P4Helpers::deltaPhi(phi, caloIntersection->position().phi());
            ATH_MSG_VERBOSE(std::endl
                            << std::setiosflags(std::ios::fixed) << " Id track: momentum " << std::setw(8) << std::setprecision(1)
                            << perigee.momentum().mag() / Gaudi::Units::GeV << "  with initial,extrapolated and calo difference for eta "
                            << std::setw(8) << std::setprecision(3) << perigee.eta() << std::setw(8) << std::setprecision(3)
                            << caloIntersection->position().eta() << std::setw(8) << std::setprecision(3) << diffEta << "  and phi "
                            << std::setw(8) << std::setprecision(3) << perigee.parameters()[Trk::phi] << std::setw(8)
                            << std::setprecision(3) << caloIntersection->position().phi() << std::setw(8) << std::setprecision(3)
                            << diffPhi);

            // check if inside cone
            if ((diffPhi * diffPhi + diffEta * diffEta) < m_trackCone2) {
                ++numberTracks;
                const double p = perigee.momentum().mag();
                sumP += p;

                ATH_MSG_VERBOSE("  inside cone, track#" << std::setw(3) << numberTracks);
            }
        }

        return std::make_pair(numberTracks, sumP);
    }

}  // namespace Rec
