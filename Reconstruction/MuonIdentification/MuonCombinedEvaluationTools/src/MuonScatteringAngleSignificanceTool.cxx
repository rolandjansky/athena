/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonScatteringAngleSignificanceTool.h"

#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"

namespace Rec {

MuonScatteringAngleSignificanceTool::MuonScatteringAngleSignificanceTool(const std::string& type,
                                                                         const std::string& name,
                                                                         const IInterface*  parent)
    : AthAlgTool(type, name, parent),
      m_calorimeterVolume(0),
      m_indetVolume(0),
      m_inDetOnly(true),
      m_refitInDetOnly(true)
{
    declareInterface<IMuonScatteringAngleSignificance>(this);
    declareProperty("InDetOnly", m_inDetOnly);
    declareProperty("RefitInDetOnly", m_refitInDetOnly);
}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>
StatusCode
MuonScatteringAngleSignificanceTool::initialize()
{
    ATH_MSG_INFO("Initializing MuonScatAngleSignificanceTool - package version " << PACKAGE_VERSION);

    // tool needed to refit slimmed tracks
    if (!m_fitter.empty()) {
        if (m_fitter.retrieve().isFailure()) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_fitter);
            m_fitter.setTypeAndName("");  // Bang!
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_fitter);
        }
    }

    // need to know which TrackingVolume we are in: indet/calo/spectrometer
    if (m_trackingVolumesSvc.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve Svc " << m_trackingVolumesSvc);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_DEBUG("Retrieved Svc " << m_trackingVolumesSvc);
        m_calorimeterVolume =
            new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
        m_indetVolume = new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
    }

    return StatusCode::SUCCESS;
}

ScatteringAngleSignificance
MuonScatteringAngleSignificanceTool::scatteringAngleSignificance(const xAOD::Muon& muon) const
{

    if (muon.muonType() == xAOD::Muon::MuonStandAlone) return ScatteringAngleSignificance(0);

    const Trk::Track* theTrack = muon.trackParticle(xAOD::Muon::CombinedTrackParticle)
                                     ? muon.trackParticle(xAOD::Muon::CombinedTrackParticle)->track()
                                     : 0;

    if (theTrack == NULL) {
        theTrack = muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle)
                       ? muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->track()
                       : 0;
    }

    if (theTrack == NULL) {
        ATH_MSG_DEBUG("No track, returning empty scatterer-significance object.");
        return ScatteringAngleSignificance(0);
    } else
        return scatteringAngleSignificance(*theTrack);
}

ScatteringAngleSignificance
MuonScatteringAngleSignificanceTool::scatteringAngleSignificance(const Trk::Track& track) const
{
    // provide refit for slimmed tracks
    const Trk::Track* fullTrack     = &track;
    const Trk::Track* refittedTrack = 0;
    //  if (track.info().trackProperties(Trk::TrackInfo::SlimmedTrack)) {
    if (isSlimmed(track)) {  // TrackInfo::SlimmedTrack wouldn't detect STACO tracks as (half-)slimmed

        if (!m_fitter.empty() && track.perigeeParameters() && track.measurementsOnTrack()
            && track.measurementsOnTrack()->size() > 3)
        {

            ATH_MSG_VERBOSE(" perform refit as track has been slimmed");
            if (m_refitInDetOnly) {
                std::vector<const Trk::MeasurementBase*>               vec;
                DataVector<const Trk::MeasurementBase>::const_iterator measIter = track.measurementsOnTrack()->begin();
                for (; measIter != track.measurementsOnTrack()->end(); ++measIter) {
                    const Amg::Vector3D& position = (*measIter)->globalPosition();
                    if (!m_indetVolume->inside(position)) continue;
                    vec.push_back((*measIter));
                }
                ATH_MSG_DEBUG("Refitting ID-part of track, meast vector of size " << vec.size());
                refittedTrack = m_fitter->fit(vec, *(track.perigeeParameters()), false, Trk::muon);
            } else {
                refittedTrack = m_fitter->fit(track, false, Trk::muon);
            }
            fullTrack = refittedTrack;
        }
        ATH_MSG_DEBUG("Detected slimmed track, "
                      << (refittedTrack ? "refit was successful" : "but could not refit it!"));

        if (!refittedTrack) return ScatteringAngleSignificance(0);
    }

    // collect sigma of phi scatterers up to last inDet measurement TSOS or to CaloDeposit TSOS
    // (according to configuration).
    // Define scattering in direction of curvature change to be +ve
    double                charge                = fullTrack->perigeeParameters()->charge();
    unsigned              measurements          = 0;
    double                neighbourRadius       = 0.;
    double                neighbourSignificance = 0.;
    unsigned              neighbours            = 0;
    unsigned              scatterers            = 0;
    unsigned              startMeasurement      = 0;
    std::vector<bool>     isMeasurement;
    std::vector<unsigned> measurementNumber;
    std::vector<double>   radii;
    std::vector<double>   sigmas;
    double                significance   = 0.;
    double                totalSigma     = 0.;
    double                weightedRadius = 0.;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator s = fullTrack->trackStateOnSurfaces()->begin();
         s != fullTrack->trackStateOnSurfaces()->end(); ++s)
    {
        // skip leading material
        if ((**s).measurementOnTrack()) {
            ++measurements;
        } else if (!measurements)
            continue;

        // select MaterialEffects up to calorimeter energy deposit
        if (!(**s).materialEffectsOnTrack()) continue;
        if ((**s).type(Trk::TrackStateOnSurface::CaloDeposit)) break;
        const Amg::Vector3D& position = (**s).materialEffectsOnTrack()->associatedSurface().globalReferencePoint();
        if (!m_calorimeterVolume->inside(position)) break;

        const Trk::MaterialEffectsOnTrack* meot =
            dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**s).materialEffectsOnTrack());
        if (!meot) continue;

        // get scattering angle
        const Trk::ScatteringAngles* scattering = meot->scatteringAngles();
        if (!scattering) continue;

        ++scatterers;
        double radius = (**s).trackParameters()->position().perp();
        double sigma  = charge * scattering->deltaPhi() / scattering->sigmaDeltaPhi();
        if ((**s).measurementOnTrack()) {
            isMeasurement.push_back(true);
        } else {
            isMeasurement.push_back(false);
        }
        measurementNumber.push_back(measurements);

        // keep maximum neighbour significance
        // (summed scattering significance for scatterers spanning neighbouring measurements)
        ++neighbours;
        significance += sigma;
        weightedRadius += sigma * radius;
        if (neighbours > 1 && measurements > startMeasurement) {
            double norm = 1. / sqrt(static_cast<double>(neighbours));
            if (norm * fabs(significance) > fabs(neighbourSignificance)) {
                if (significance == 0.) {
                    neighbourRadius = radius;
                } else {
                    neighbourRadius = weightedRadius / significance;
                }
                neighbourSignificance = norm * significance;
                ATH_MSG_VERBOSE("   current maximum for neighbourSignificance "
                                << neighbourSignificance << " from " << neighbours << " scatterers at radius "
                                << neighbourRadius);
            }
            ATH_MSG_VERBOSE("   reset neighbour after " << significance << " /sqrt(" << neighbours << ") at radius "
                                                        << radius);
            neighbours       = 0;
            significance     = 0.;
            startMeasurement = measurements;
            weightedRadius   = 0.;
            if (isMeasurement.back()) {
                ++neighbours;
                significance += sigma;
                weightedRadius += sigma * radius;
            }
        }

        radii.push_back(radius);
        sigmas.push_back(sigma);
        totalSigma += sigma;

        ATH_MSG_VERBOSE(scatterers << " " << measurements << " " << isMeasurement.back() << " radius "
                                   << (**s).trackParameters()->position().perp() << "   deltaPhi "
                                   << scattering->deltaPhi() << "   significance " << sigma << "  integralSignificance "
                                   << totalSigma);
    }

    // pop_back to last measurement unless configured to use calo
    delete refittedTrack;
    if (m_inDetOnly) {
        while (scatterers && measurementNumber.back() == measurements && !isMeasurement.back()) {
            --scatterers;
            totalSigma -= sigmas.back();
            isMeasurement.pop_back();
            measurementNumber.pop_back();
            radii.pop_back();
            sigmas.pop_back();
        }
    }

    // quit if no scatterers selected
    if (!scatterers) {
        ATH_MSG_DEBUG(" no scatterers selected, giving up.");
        return ScatteringAngleSignificance(0);
    }

    // search for discontinuity point (sign change of integral scattering)
    // maximum value determines curvatureSignificance
    double   curvatureSignificance = totalSigma;
    double   curvatureRadius       = *radii.rbegin();
    unsigned index                 = scatterers - 1;

    ATH_MSG_VERBOSE(" scatteringAngleSignificance " << totalSigma << " at radius " << radii[index] << " at scatterer "
                                                    << scatterers << "  sigmas.size() " << sigmas.size());

    for (std::vector<double>::const_reverse_iterator r = sigmas.rbegin(); r != sigmas.rend(); ++r, --index) {
        totalSigma -= 2. * (*r);
        if (index > 1)
            ATH_MSG_VERBOSE(" scatteringAngleSignificance " << totalSigma << " at radius " << radii[index - 1]
                                                            << " at scatterer " << index);

        if (fabs(totalSigma) > fabs(curvatureSignificance) && index > 1) {
            curvatureSignificance = totalSigma;
            curvatureRadius       = radii[index - 1];
        }
    }

    // normalize final curvature significance
    curvatureSignificance /= sqrt(static_cast<double>(scatterers));

    ATH_MSG_DEBUG(" scatteringAngleSignificance"
                  << std::setiosflags(std::ios::fixed) << std::setw(7) << std::setprecision(2) << curvatureSignificance
                  << " at radius " << std::setw(6) << std::setprecision(1) << curvatureRadius
                  << " neighbourSignificance " << std::setw(7) << std::setprecision(2) << neighbourSignificance
                  << " at radius " << std::setw(6) << std::setprecision(1) << neighbourRadius << " from " << scatterers
                  << " scatterers");

    return ScatteringAngleSignificance(scatterers, curvatureRadius, curvatureSignificance, neighbourRadius,
                                       neighbourSignificance);
}

bool
MuonScatteringAngleSignificanceTool::isSlimmed(const Trk::Track& track) const
{
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator s = track.trackStateOnSurfaces()->begin();
         s != track.trackStateOnSurfaces()->end(); ++s)
    {
        if ((**s).trackParameters()) continue;

        ATH_MSG_VERBOSE("track is slimmed (found TSOS without trackParameters) ");
        return true;
    }

    ATH_MSG_VERBOSE("track is not slimmed (all TSOS have trackParameters) ");

    return false;
}

}  // namespace Rec
