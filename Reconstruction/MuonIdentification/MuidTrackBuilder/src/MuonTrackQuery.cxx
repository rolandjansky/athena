/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuidTrackBuilder/MuonTrackQuery.h"

#include <cmath>
#include <iomanip>

#include "AthenaKernel/Units.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "MuidEvent/FieldIntegral.h"
#include "MuidEvent/ScatteringAngleSignificance.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkEventPrimitives/DriftCircleSide.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "muonEvent/CaloEnergy.h"

namespace Units = Athena::Units;
namespace {
    constexpr double OneOverSqrt2 = M_SQRT1_2;
}
namespace Rec {

    MuonTrackQuery::MuonTrackQuery(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonTrackQuery>(this);
    }

    StatusCode MuonTrackQuery::initialize() {
        ATH_MSG_DEBUG("Initializing MuonTrackQuery");

        // tool needed to refit slimmed tracks
        if (!m_fitter.empty()) {
            ATH_CHECK(m_fitter.retrieve());
            ATH_MSG_DEBUG("Retrieved tool " << m_fitter);
        }

        // multipurpose and identifier helper tools for spectrometer
        ATH_CHECK(m_edmHelperSvc.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_edmHelperSvc);

        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_idHelperSvc);

        // tools needed when flipping tracks to outgoing
        if (!m_mdtRotCreator.empty()) {
            ATH_CHECK(m_mdtRotCreator.retrieve());
            ATH_MSG_DEBUG("Retrieved tool " << m_mdtRotCreator);
        }

        // need to know which TrackingVolume we are in: indet/calo/spectrometer
        if (!m_trackingGeometryReadKey.empty()) {
            ATH_CHECK(m_trackingGeometryReadKey.initialize());
        } else {
            ATH_MSG_ERROR("Could not retrieve a valid tracking geometry");
        }
        return StatusCode::SUCCESS;
    }

    const CaloEnergy* MuonTrackQuery::caloEnergy(const Trk::Track& track) const {
        const CaloEnergy* caloEnergy = nullptr;

        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            // select MaterialEffects inside calorimeter volume
            if (!tsos->type(Trk::TrackStateOnSurface::CaloDeposit) || !tsos->materialEffectsOnTrack()) { continue; }

            const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());

            if (!meot) { continue; }

            caloEnergy = dynamic_cast<const CaloEnergy*>(meot->energyLoss());
            if (caloEnergy) { break; }
        }

        ATH_MSG_VERBOSE("caloEnergy " << caloEnergy);

        return caloEnergy;
    }
    double MuonTrackQuery::caloEnergyDeposit(const Trk::Track& track, const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return 0.;
        }

        const Trk::TrackingVolume* indetVolume = getVolume("InDet::Containers::InnerDetector", ctx);

        if (!indetVolume) {
            ATH_MSG_WARNING("Failed to retrieve InDeT volume ");
            return 0.;
        }

        const Trk::TrackParameters* indetExitParameters = nullptr;
        const Trk::TrackParameters* caloExitParameters = nullptr;

        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            if (!tsos->trackParameters()) continue;

            if (indetVolume->inside(tsos->trackParameters()->position())) {
                indetExitParameters = tsos->trackParameters();
                continue;
            }

            if (!calorimeterVolume->inside(tsos->trackParameters()->position()) && caloExitParameters) { break; }

            caloExitParameters = tsos->trackParameters();
        }

        if (!indetExitParameters || !caloExitParameters) { return 0.; }

        double energyDeposit = indetExitParameters->momentum().mag() - caloExitParameters->momentum().mag();

        ATH_MSG_VERBOSE(" calo energy deposit ");

        return energyDeposit;
    }

    FieldIntegral MuonTrackQuery::fieldIntegral(const Trk::Track& track, const EventContext& ctx) const {
        // field integral null for straight tracks + method is not valid for slimmed tracks
        if (isLineFit(track) || isSlimmed(track)) { return FieldIntegral(); }

        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return FieldIntegral();
        }

        const Trk::TrackingVolume* indetVolume = getVolume("InDet::Containers::InnerDetector", ctx);

        if (!indetVolume) {
            ATH_MSG_WARNING("Failed to retrieve InDeT volume ");
            return FieldIntegral();
        }

        // sum Bdl between measurements
        double betweenInDetMeasurements = 0.;
        double betweenSpectrometerMeasurements = 0.;

        bool haveInDet = false;
        bool haveSpectrometer = false;

        Amg::Vector3D integratedMomentumKick = Amg::Vector3D(0., 0., 0.);

        const Trk::TrackParameters* parameters = nullptr;

        // loop over TSOS to integrate vector Bdl
        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            // not going to use trigger hits, outliers or pseudoMeasurements
            bool isPreciseHit = (tsos->measurementOnTrack() && dynamic_cast<const Trk::MeasurementBase*>(tsos->measurementOnTrack()) &&
                                 !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos->measurementOnTrack()) &&
                                 !tsos->type(Trk::TrackStateOnSurface::Outlier));

            // skip slimmed measurements
            if (!tsos->trackParameters() || dynamic_cast<const Trk::Perigee*>(tsos->trackParameters())) { continue; }

            // skip spectrometer phi measurements
            if (isPreciseHit && !calorimeterVolume->inside(tsos->trackParameters()->position())) {
                Identifier id = m_edmHelperSvc->getIdentifier(*tsos->measurementOnTrack());
                isPreciseHit = (id.is_valid() && !m_idHelperSvc->measuresPhi(id));
            }

            if (!tsos->materialEffectsOnTrack() && !isPreciseHit) { continue; }

            // careful with first hit + skip any flipped parameters
            if (!parameters) { parameters = tsos->trackParameters(); }

            Amg::Vector3D startMomentum = parameters->momentum();
            parameters = tsos->trackParameters();
            Amg::Vector3D endDirection = parameters->momentum().unit();

            if (startMomentum.dot(endDirection) < 0.) continue;

            // subtract scattering angle
            if (tsos->materialEffectsOnTrack()) {
                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());

                if (meot && meot->scatteringAngles()) {
                    double theta = endDirection.theta() - meot->scatteringAngles()->deltaTheta();
                    double phi = endDirection.phi() - meot->scatteringAngles()->deltaPhi();
                    double cosTheta = std::cos(theta);
                    double sinTheta = std::sqrt(1. - cosTheta * cosTheta);

                    endDirection = Amg::Vector3D(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
                }
            }

            Amg::Vector3D momentumKick = startMomentum.cross(endDirection) / (0.3 * Units::GeV);
            integratedMomentumKick += momentumKick;

            // accumulate abs(Bdl) between measurements (distinguish indet and spectrometer)
            if (!isPreciseHit) continue;

            if (indetVolume->inside(parameters->position())) {
                if (haveInDet) {
                    betweenInDetMeasurements += integratedMomentumKick.mag();
                    integratedMomentumKick = Amg::Vector3D(0., 0., 0.);
                } else {
                    haveInDet = true;
                    integratedMomentumKick = Amg::Vector3D(0., 0., 0.);
                }
            } else {
                if (haveSpectrometer) {
                    betweenSpectrometerMeasurements += integratedMomentumKick.mag();
                    integratedMomentumKick = Amg::Vector3D(0., 0., 0.);
                } else {
                    haveSpectrometer = true;
                    integratedMomentumKick = Amg::Vector3D(0., 0., 0.);
                }
            }
        }

        ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                      << " field integrals for track at eta, phi " << std::setw(6) << std::setprecision(2)
                      << track.perigeeParameters()->momentum().eta() << "," << std::setw(6) << std::setprecision(2)
                      << track.perigeeParameters()->momentum().phi() << " : betweenInDetMeasurements " << std::setw(8)
                      << std::setprecision(3) << betweenInDetMeasurements << "   betweenSpectrometerMeasurements " << std::setw(8)
                      << std::setprecision(3) << betweenSpectrometerMeasurements);

        return FieldIntegral(betweenInDetMeasurements, betweenSpectrometerMeasurements, 0., 0.);
    }

    bool MuonTrackQuery::isCaloAssociated(const Trk::Track& track, const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return false;
        }

        const Trk::TrackingVolume* indetVolume = getVolume("InDet::Containers::InnerDetector", ctx);

        if (!indetVolume) {
            ATH_MSG_WARNING("Failed to retrieve InDeT volume ");
            return false;
        }

        bool haveCaloDeposit = false;
        int numberCaloTSOS = 0;

        for (const Trk::TrackStateOnSurface* s : *track.trackStateOnSurfaces()) {
            // CaloDeposit?
            if (s->type(Trk::TrackStateOnSurface::CaloDeposit)) {
                ATH_MSG_VERBOSE("haveCaloDeposit, " << numberCaloTSOS << " TSOS in calo volume");
                haveCaloDeposit = true;
            }

            // select MaterialEffects inside calorimeter volume
            if (!s->materialEffectsOnTrack()) { continue; }

            Amg::Vector3D position = s->materialEffectsOnTrack()->associatedSurface().globalReferencePoint();

            if (indetVolume->inside(position)) { continue; }
            if (!calorimeterVolume->inside(position)) { break; }
            if (++numberCaloTSOS > 2 && haveCaloDeposit) { return true; }
        }

        ATH_MSG_VERBOSE("association false, " << numberCaloTSOS << " TSOS in calo volume");

        return false;
    }

    bool MuonTrackQuery::isCombined(const Trk::Track& track, const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return false;
        }

        const Trk::TrackingVolume* indetVolume = getVolume("InDet::Containers::InnerDetector", ctx);

        if (!indetVolume) {
            ATH_MSG_WARNING("Failed to retrieve InDeT volume ");
            return false;
        }

        // combined tracks have indet and spectrometer active measurements
        bool indet = false;
        bool spectrometer = false;

        // find innermost measurement (assume some degree of measurement ordering)
        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            if (!tsos->measurementOnTrack() || tsos->type(Trk::TrackStateOnSurface::Outlier) ||
                dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos->measurementOnTrack())) {
                continue;
            }

            if (indetVolume->inside(tsos->measurementOnTrack()->globalPosition())) {
                indet = true;
                break;
            } else if (!calorimeterVolume->inside(tsos->measurementOnTrack()->globalPosition())) {
                spectrometer = true;
                break;
            }
        }

        // find outermost measurement
        DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rs = track.trackStateOnSurfaces()->rbegin();
        for (; rs != track.trackStateOnSurfaces()->rend(); ++rs) {
            if (!(**rs).measurementOnTrack() || (**rs).type(Trk::TrackStateOnSurface::Outlier) ||
                dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**rs).measurementOnTrack())) {
                continue;
            }

            if (indetVolume->inside((**rs).measurementOnTrack()->globalPosition())) {
                indet = true;
                break;
            } else if (!calorimeterVolume->inside((**rs).measurementOnTrack()->globalPosition())) {
                spectrometer = true;
                break;
            }
        }

        return indet && spectrometer;
    }

    bool MuonTrackQuery::isExtrapolated(const Trk::Track& track, const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return false;
        }

        const Trk::TrackingVolume* indetVolume = getVolume("InDet::Containers::InnerDetector", ctx);

        if (!indetVolume) {
            ATH_MSG_WARNING("Failed to retrieve InDeT volume ");
            return false;
        }

        // perigee needs to be inside indet
        const Trk::TrackParameters* parameters = track.perigeeParameters();
        if (!parameters || !indetVolume->inside(parameters->position())) { return false; }

        // extrapolated tracks have indet TSOS but not active measurements
        // and spectrometer active measurements
        bool indet = false;
        bool spectrometer = false;

        // find innermost measurement (assume some degree of measurement ordering)
        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            if (!tsos->measurementOnTrack() || tsos->type(Trk::TrackStateOnSurface::Outlier) ||
                dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos->measurementOnTrack())) {
                continue;
            }

            if (indetVolume->inside(tsos->measurementOnTrack()->globalPosition())) {
                indet = true;
                break;
            } else if (!calorimeterVolume->inside(tsos->measurementOnTrack()->globalPosition())) {
                spectrometer = true;
                break;
            }
        }

        // find outermost measurement (assume some degree of measurement ordering)
        DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rs = track.trackStateOnSurfaces()->rbegin();
        for (; rs != track.trackStateOnSurfaces()->rend(); ++rs) {
            if (!(**rs).measurementOnTrack() || (**rs).type(Trk::TrackStateOnSurface::Outlier) ||
                dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**rs).measurementOnTrack())) {
                continue;
            }

            if (indetVolume->inside((**rs).measurementOnTrack()->globalPosition())) {
                indet = true;
                break;
            } else if (!calorimeterVolume->inside((**rs).measurementOnTrack()->globalPosition())) {
                spectrometer = true;
                break;
            }
        }

        return !indet && spectrometer;
    }

    bool MuonTrackQuery::isLineFit(const Trk::Track& track) const {
        if (track.info().trackProperties(Trk::TrackInfo::StraightTrack)) { return true; }

        // for backwards compatibility
        const Trk::Perigee* measuredPerigee = track.perigeeParameters();

        if (!measuredPerigee) return false;

        if (!measuredPerigee->covariance()) {
            ATH_MSG_WARNING(" measured Perigee has no covariance!");
            return false;
        }

        double momentumCovariance = (*measuredPerigee->covariance())(Trk::qOverP, Trk::qOverP);

        double relativeMomentumCovariance = momentumCovariance * measuredPerigee->momentum().mag2();

        // unphysically small value used to signal SL
        if (momentumCovariance < 1.E-19 || relativeMomentumCovariance < 1.E-6) return true;

        // or if curvature unmeasured (error > 1000%)
        if (relativeMomentumCovariance > 100.) return true;

        return false;
    }

    bool MuonTrackQuery::isProjective(const Trk::Track& track) const {
        // get start and end measured positions
        Amg::Vector3D startPosition(0., 0., 0.);
        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            if (tsos->measurementOnTrack() && !tsos->type(Trk::TrackStateOnSurface::Outlier) &&
                !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos->measurementOnTrack())) {
                startPosition = tsos->measurementOnTrack()->globalPosition();
                break;
            }
        }

        Amg::Vector3D endPosition(0., 0., 0.);

        DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rs = track.trackStateOnSurfaces()->rbegin();
        for (; rs != track.trackStateOnSurfaces()->rend(); ++rs) {
            if ((**rs).measurementOnTrack() && !(**rs).type(Trk::TrackStateOnSurface::Outlier) &&
                !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**rs).measurementOnTrack())) {
                endPosition = (**rs).measurementOnTrack()->globalPosition();
                break;
            }
        }

        // same-side check: kill if through (up-down) track
        double side = endPosition.dot(startPosition);
        if (side < 0.) {
            ATH_MSG_DEBUG("track is not projective: fails same-side check ");
            return false;
        }

        // hence change in phi between these measurements
        double norm = 1. / std::sqrt(startPosition.perp2() * endPosition.perp2());
        double cosDeltaPhi = norm * (endPosition.x() * startPosition.x() + endPosition.y() * startPosition.y());
        double sinDeltaPhi = norm * (endPosition.x() * startPosition.y() - endPosition.y() * startPosition.x());

        // cut on change of long phi sector
        if (std::abs(sinDeltaPhi) > M_PI / 8.) {
            ATH_MSG_DEBUG("track is not projective: sinDeltaPhi " << sinDeltaPhi);
            return false;
        }

        // cut on change of hemisphere (except for far forward tracks)
        if (cosDeltaPhi < 0.) {
            double cotTheta = startPosition.z() / startPosition.perp();
            if (std::abs(startPosition.z()) > std::abs(endPosition.z())) { cotTheta = endPosition.z() / endPosition.perp(); }

            // FIXME: isn't this same-side again?
            if (startPosition.z() * endPosition.z() < 0. || std::abs(cotTheta) < 2.0) {
                ATH_MSG_DEBUG("track is not projective: cosDeltaPhi " << cosDeltaPhi);
                return false;
            }

            ATH_MSG_WARNING("forward track changes hemisphere: cotTheta " << cotTheta << "  cosDeltaPhi " << cosDeltaPhi);
        }

        // OK - the track is at least roughly projective
        return true;
    }

    bool MuonTrackQuery::isSectorOverlap(const Trk::Track& track) const {
        constexpr double sectorOffset = M_PI / 16.;
        bool isOverlap = true;
        double cosPhi = 0.;
        double sinPhi = 0.;

        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            if (!tsos->measurementOnTrack() || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos->measurementOnTrack())) { continue; }

            if (isOverlap) {
                isOverlap = false;

                const Amg::Vector3D& position = tsos->measurementOnTrack()->associatedSurface().globalReferencePoint();

                cosPhi = position.x() / position.perp();
                sinPhi = position.y() / position.perp();
            } else {
                const Amg::Vector3D& position = tsos->measurementOnTrack()->associatedSurface().globalReferencePoint();

                double sinDeltaPhi = (position.x() * sinPhi - position.y() * cosPhi) / position.perp();

                if (std::abs(sinDeltaPhi) > sectorOffset) {
                    ATH_MSG_DEBUG("found overlap: sinDeltaPhi " << sinDeltaPhi);
                    isOverlap = true;
                    break;
                }
            }
        }

        return isOverlap;
    }

    bool MuonTrackQuery::isSlimmed(const Trk::Track& track) const {
        DataVector<const Trk::TrackStateOnSurface>::const_iterator s = track.trackStateOnSurfaces()->begin();
        for (; s != track.trackStateOnSurfaces()->end(); ++s) {
            if ((**s).trackParameters()) continue;

            ATH_MSG_VERBOSE("track is slimmed (found TSOS without trackParameters) ");
            return true;
        }

        ATH_MSG_VERBOSE("track is not slimmed (all TSOS have trackParameters) ");

        return false;
    }

    double MuonTrackQuery::momentumBalanceSignificance(const Trk::Track& track, const EventContext& ctx) const {
        // significance only defined for combined muons
        if (!isCombined(track, ctx)) return 0.;

        // find the TSOS carrying caloEnergy (TSOS type CaloDeposit)
        // compare parameters with those from previous TSOS
        double energyBalance = 0.;

        const Trk::EnergyLoss* energyLoss = nullptr;
        const Trk::TrackParameters* previousParameters = nullptr;

        for (const Trk::TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
            if (!tsos->trackParameters()) continue;

            if (tsos->materialEffectsOnTrack()) {
                const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());

                if (!meot) continue;
                energyLoss = meot->energyLoss();

                if (tsos->type(Trk::TrackStateOnSurface::CaloDeposit) && energyLoss && previousParameters) {
                    energyBalance = previousParameters->momentum().mag() - energyLoss->deltaE() - tsos->trackParameters()->momentum().mag();

                    if (std::abs(energyBalance) < energyLoss->sigmaDeltaE()) {
                        ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                                      << " momentum balance  " << std::setw(6) << std::setprecision(2) << energyBalance / Units::GeV
                                      << "   significance " << std::setw(6) << std::setprecision(1)
                                      << energyBalance / energyLoss->sigmaDeltaE() << "  p before/after calo" << std::setw(7)
                                      << std::setprecision(2) << previousParameters->momentum().mag() / Units::GeV << " /" << std::setw(7)
                                      << std::setprecision(2) << tsos->trackParameters()->momentum().mag() / Units::GeV
                                      << "   energy deposit sigma  " << energyLoss->sigmaDeltaE() / Units::GeV << " GeV");

                        energyBalance /= energyLoss->sigmaDeltaE();
                    } else if (energyBalance < 0.) {
                        ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                                      << " momentum balance  " << std::setw(6) << std::setprecision(2) << energyBalance / Units::GeV
                                      << "   significance " << std::setw(6) << std::setprecision(1)
                                      << energyBalance / energyLoss->sigmaDeltaE() << "  p before/after calo" << std::setw(7)
                                      << std::setprecision(2) << previousParameters->momentum().mag() / Units::GeV << " /" << std::setw(7)
                                      << std::setprecision(2) << tsos->trackParameters()->momentum().mag() / Units::GeV
                                      << "   energy deposit sigma- " << energyLoss->sigmaMinusDeltaE() / Units::GeV << " GeV");

                        energyBalance /= energyLoss->sigmaMinusDeltaE();
                    } else {
                        ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                                      << " momentum balance  " << std::setw(6) << std::setprecision(2) << energyBalance / Units::GeV
                                      << "   significance " << std::setw(6) << std::setprecision(1)
                                      << energyBalance / energyLoss->sigmaDeltaE() << "  p before/after calo" << std::setw(7)
                                      << std::setprecision(2) << previousParameters->momentum().mag() / Units::GeV << " /" << std::setw(7)
                                      << std::setprecision(2) << tsos->trackParameters()->momentum().mag() / Units::GeV
                                      << "   energy deposit sigma+ " << energyLoss->sigmaPlusDeltaE() / Units::GeV << " GeV");

                        energyBalance /= energyLoss->sigmaPlusDeltaE();
                    }
                    break;
                }
            }

            // update previous parameters
            previousParameters = tsos->trackParameters();
        }

        return energyBalance;
    }

    unsigned MuonTrackQuery::numberPseudoMeasurements(const Trk::Track& track) const {
        unsigned numberPseudo = 0;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator s = track.trackStateOnSurfaces()->begin();
        for (; s != track.trackStateOnSurfaces()->end(); ++s) {
            if ((**s).measurementOnTrack() && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) {
                ++numberPseudo;
            }
        }

        ATH_MSG_VERBOSE(" track has " << numberPseudo << " PseudoMeasurements");

        return numberPseudo;
    }

    std::unique_ptr<const Trk::Perigee> MuonTrackQuery::outgoingPerigee(const Trk::Track& track) const {
        // only flip Perigee
        const Trk::Perigee* perigee = track.perigeeParameters();
        if (!perigee) {
            ATH_MSG_WARNING(" input track without Perigee");
            return nullptr;
        }

        const Trk::PerigeeSurface& surface = perigee->associatedSurface();

        const Trk::TrackStateOnSurface* outerTSOS = track.trackStateOnSurfaces()->back();
        if (!perigee->covariance()) {
            ATH_MSG_WARNING(" perigee has no covariance");
            return perigee->uniqueClone();
        }

        if (outerTSOS->trackParameters() == perigee && perigee->momentum().dot(perigee->position()) <= 0.) {
            const AmgSymMatrix(5)& perigeeCov = (*perigee->covariance());

            // need to flip
            return std::make_unique<Trk::Perigee>(perigee->position(), -perigee->momentum(), -perigee->charge(), surface, perigeeCov);
        }

        return perigee->uniqueClone();
    }
    ScatteringAngleSignificance MuonTrackQuery::scatteringAngleSignificance(const Trk::Track& track, const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);
        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return ScatteringAngleSignificance{};
        }

        const Trk::TrackingVolume* indetVolume = getVolume("InDet::Containers::InnerDetector", ctx);

        if (!indetVolume) {
            ATH_MSG_WARNING("Failed to retrieve InDeT volume ");
            return ScatteringAngleSignificance{};
        }

        // provide refit for slimmed tracks
        std::unique_ptr<const Trk::Track> refittedTrack;

        if (isSlimmed(track)) {
            if (!m_fitter.empty()) {
                ATH_MSG_VERBOSE(" perform refit as track has been slimmed");
                refittedTrack = m_fitter->fit(ctx, track, false, Trk::muon);
            }
            if (!refittedTrack) return ScatteringAngleSignificance(0);
        } else {
            refittedTrack = std::make_unique<Trk::Track>(track);
        }

        // collect sigma of scatterer up to TSOS carrying caloEnergy
        double charge = refittedTrack->perigeeParameters()->charge();
        bool haveMeasurement = false;
        unsigned scatterers = 0;
        double totalSigma = 0.;

        std::vector<bool> isMeasurement;
        std::vector<double> sigmas;
        std::vector<double> radii;

        for (const Trk::TrackStateOnSurface* tsos : *refittedTrack->trackStateOnSurfaces()) {
            // skip leading material
            if (!haveMeasurement) {
                if (tsos->measurementOnTrack()) { haveMeasurement = true; }
                continue;
            }

            // select MaterialEffects up to calorimeter volume
            if (!tsos->materialEffectsOnTrack()) continue;

            const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());

            const Amg::Vector3D& position = meot->associatedSurface().globalReferencePoint();

            if (!calorimeterVolume->inside(position)) break;

            if (!indetVolume->inside(position) && meot->energyLoss()) break;

            // get scattering angle
            const Trk::ScatteringAngles* scattering = meot->scatteringAngles();
            if (!scattering) continue;

            ++scatterers;

            if (tsos->measurementOnTrack()) {
                isMeasurement.push_back(true);
            } else {
                isMeasurement.push_back(false);
            }

            radii.push_back(tsos->trackParameters()->position().perp());
            double sigma = charge * scattering->deltaPhi() / scattering->sigmaDeltaPhi();
            totalSigma += sigma;
            sigmas.push_back(sigma);

            ATH_MSG_VERBOSE(scatterers << " radius " << tsos->trackParameters()->position().perp() << "   deltaPhi "
                                       << scattering->deltaPhi() << "   significance " << sigma << "  totalSignificance " << totalSigma);
        }

        if (!scatterers) { return ScatteringAngleSignificance(0); }

        // search for discontinuity point (sign change of integral scattering)
        // and maximum of neighbouring scatterers
        // FIXME: neighbouring means scatterers spanning neighbouring measurements
        double curvatureSignificance = totalSigma;
        double curvatureRadius = 0.;
        double neighbourSignificance = 0.;
        double neighbourRadius = 0.;
        double previousSignificance = 0.;
        double previousRadius = 0.;
        unsigned index = scatterers - 1;

        std::vector<double>::const_reverse_iterator rs = sigmas.rbegin();
        for (; rs != sigmas.rend(); ++rs, --index) {
            totalSigma -= 2. * (*rs);

            if (std::abs(totalSigma) > std::abs(curvatureSignificance)) {
                curvatureSignificance = totalSigma;
                curvatureRadius = radii[index];
            }

            if (std::abs(sigmas[index] + previousSignificance) > std::abs(neighbourSignificance)) {
                neighbourSignificance = sigmas[index] + previousSignificance;

                if (std::abs(neighbourSignificance) > 0.) {
                    neighbourRadius = (sigmas[index] * radii[index] + previousSignificance * previousRadius) / neighbourSignificance;
                } else {
                    neighbourRadius = 0.;
                }
            }

            previousSignificance = sigmas[index];
            previousRadius = radii[index];
        }

        // normalize
        curvatureSignificance /= std::sqrt(static_cast<double>(scatterers));
        neighbourSignificance *= OneOverSqrt2;

        ATH_MSG_DEBUG(" scatteringAngleSignificance " << curvatureSignificance << " at radius " << curvatureRadius
                                                      << " neighbourSignificance " << neighbourSignificance << " at radius "
                                                      << neighbourRadius << " from " << scatterers << " scatterers");

        return ScatteringAngleSignificance(scatterers, curvatureRadius, curvatureSignificance, neighbourRadius, neighbourSignificance);
    }

    std::unique_ptr<Trk::TrackParameters> MuonTrackQuery::spectrometerParameters(const Trk::Track& track, const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return nullptr;
        }

        // find parameters at innermost spectrometer measurement
        // clone perigee if track has been slimmed
        const Trk::TrackParameters* parametersView{};
        for (const Trk::TrackStateOnSurface* pThisTrackState : *track.trackStateOnSurfaces()) {
            if (!pThisTrackState->measurementOnTrack() || pThisTrackState->type(Trk::TrackStateOnSurface::Outlier) ||
                !pThisTrackState->trackParameters() ||
                dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(pThisTrackState->measurementOnTrack()) ||
                calorimeterVolume->inside(pThisTrackState->trackParameters()->position())) {
                continue;
            }

            if (parametersView && pThisTrackState->trackParameters()->position().mag() > parametersView->position().mag()) { break; }
            parametersView = pThisTrackState->trackParameters();
        }

        if (!parametersView) { parametersView = track.perigeeParameters(); }

        // if necessary, flip to outgoing parameters
        if (parametersView->momentum().dot(parametersView->position()) > 0.) {
            return parametersView->uniqueClone();  // creates new parameters as clone
        } else {
            ATH_MSG_VERBOSE(" flip spectrometer parameters ");
            return flippedParameters(*parametersView);  // creates new parameters as flipped clone
        }
    }

    /** IMuonTrackQuery interface:
        assess the number of additional phi measurements needed for MS (or SA) track fit */
    unsigned MuonTrackQuery::spectrometerPhiQuality(const Trk::Track& track, const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return false;
        }

        // quality 0 - no additional phi measurement needed
        //         1 - phi direction un- (or poorly) constrained
        //         2 - no phi hits on track
        const Trk::TrackStateOnSurface* leadingPhiMeasurement = nullptr;
        const Trk::TrackStateOnSurface* trailingPhiMeasurement = nullptr;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator s = track.trackStateOnSurfaces()->begin();
        for (; s != track.trackStateOnSurfaces()->end(); ++s) {
            if (!(**s).measurementOnTrack() || (**s).type(Trk::TrackStateOnSurface::Outlier) ||
                dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack()) || !(**s).trackParameters() ||
                calorimeterVolume->inside((**s).trackParameters()->position())) {
                continue;
            }

            Identifier id = m_edmHelperSvc->getIdentifier(*(**s).measurementOnTrack());
            if (!id.is_valid() || !m_idHelperSvc->measuresPhi(id)) { continue; }

            // require phi measurement from CSC or CompetingROT
            if (!dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>((**s).measurementOnTrack()) && !m_idHelperSvc->isCsc(id)) {
                continue;
            }

            leadingPhiMeasurement = *s;
            break;
        }

        if (!leadingPhiMeasurement) return 2;

        DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator r = track.trackStateOnSurfaces()->rbegin();
        for (; r != track.trackStateOnSurfaces()->rend(); ++r) {
            if (!(**r).measurementOnTrack() || (**r).type(Trk::TrackStateOnSurface::Outlier) ||
                dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**r).measurementOnTrack()) || !(**r).trackParameters()) {
                continue;
            }

            if (calorimeterVolume->inside((**r).trackParameters()->position())) { break; }

            Identifier id = m_edmHelperSvc->getIdentifier(*(**r).measurementOnTrack());

            if (!id.is_valid() || !m_idHelperSvc->measuresPhi(id)) { continue; }

            if (*r != leadingPhiMeasurement) { trailingPhiMeasurement = *r; }

            break;
        }

        if (!trailingPhiMeasurement) { return 1; }

        // FIXME: check separation between phi meas
        double separation =
            (leadingPhiMeasurement->trackParameters()->position() - trailingPhiMeasurement->trackParameters()->position()).mag();

        if (separation < 2. * Units::meter) { return 1; }

        return 0;
    }

    std::unique_ptr<const Trk::TrackParameters> MuonTrackQuery::triggerStationParameters(const Trk::Track& track,
                                                                                         const EventContext& ctx) const {
        const Trk::TrackingVolume* calorimeterVolume = getVolume("Calo::Container", ctx);

        if (!calorimeterVolume) {
            ATH_MSG_WARNING("Failed to retrieve Calo volume ");
            return nullptr;
        }

        // find parameters at innermost trigger station measurement
        // fails if track has been slimmed
        std::unique_ptr<const Trk::TrackParameters> parameters;

        DataVector<const Trk::TrackStateOnSurface>::const_iterator s = track.trackStateOnSurfaces()->begin();
        for (; s != track.trackStateOnSurfaces()->end(); ++s) {
            if (!(**s).measurementOnTrack() || (**s).type(Trk::TrackStateOnSurface::Outlier) || !(**s).trackParameters() ||
                !dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>((**s).measurementOnTrack()) ||
                calorimeterVolume->inside((**s).trackParameters()->position())) {
                continue;
            }

            if (parameters && (**s).trackParameters()->position().mag() > parameters->position().mag()) { break; }

            parameters = (**s).trackParameters()->uniqueClone();
        }

        if (!parameters) return nullptr;

        // if necessary, flip to outgoing parameters
        if (parameters->momentum().dot(parameters->position()) > 0.) {
            parameters = parameters->uniqueClone();
        } else {
            ATH_MSG_VERBOSE(" flip spectrometer parameters ");
            parameters = flippedParameters(*parameters);
        }

        return parameters;
    }

    std::unique_ptr<Trk::TrackParameters> MuonTrackQuery::flippedParameters(const Trk::TrackParameters& parameters) {
        double phi = parameters.parameters()[Trk::phi0];
        if (phi > 0.) {
            phi -= M_PI;
        } else {
            phi += M_PI;
        }

        double theta = M_PI - parameters.parameters()[Trk::theta];
        double qOverP = -parameters.parameters()[Trk::qOverP];
        const Trk::Surface* surface = &(parameters.associatedSurface());

        return surface->createUniqueTrackParameters(
            parameters.parameters()[Trk::d0], parameters.parameters()[Trk::z0], phi, theta, qOverP,
            parameters.covariance() ? std::optional<AmgSymMatrix(5)>(*parameters.covariance()) : std::nullopt);
    }

}  // namespace Rec
