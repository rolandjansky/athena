/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidMuonRecovery
//  AlgTool performing MS hit reallocation for a likely spectrometer-indet
//  match which has given combined fit problems.
//  Extrapolates indet track to MS.
//  Returns a combined track with full track fit.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include "MuidTrackBuilder/MuidMuonRecovery.h"

#include <cmath>
#include <iomanip>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"

namespace Rec {


MuidMuonRecovery::MuidMuonRecovery(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_minP(10. * Gaudi::Units::GeV),
      m_minPt(5. * Gaudi::Units::GeV),
      m_recoveryAttempts(0),
      m_recoveryFitFailure(0),
      m_recoverySuccess(0)
{
    declareInterface<IMuidMuonRecovery>(this);
    declareProperty("MinP", m_minP);
    declareProperty("MinPt", m_minPt);
    declareProperty("PullCut", m_pullCut = 5);
}


//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>


StatusCode
MuidMuonRecovery::initialize()
{
    ATH_MSG_INFO("Initializing MuidMuonRecovery - package version " << PACKAGE_VERSION);

    // get the Tools
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_extrapolator);
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_edmHelperSvc);
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_idHelperSvc);
    ATH_CHECK(m_printer.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_printer);
    ATH_CHECK(m_residualCalculator.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_residualCalculator);

    if (!m_trackBuilder.empty()) {
        ATH_CHECK(m_trackBuilder.retrieve());
        ATH_MSG_INFO("Retrieved tool " << m_trackBuilder);
    }

    return StatusCode::SUCCESS;
}


StatusCode
MuidMuonRecovery::finalize()
{
    ATH_MSG_INFO("Recovery attempts " << m_recoveryAttempts << ", failedFit " << m_recoveryFitFailure << ", success "
                                      << m_recoverySuccess);

    return StatusCode::SUCCESS;
}


Trk::Track*
MuidMuonRecovery::recoverableMatch(const Trk::Track& indetTrack, const Trk::Track& spectrometerTrack) const
{
    // skip low pt ID tracks
    if (!indetTrack.perigeeParameters() || indetTrack.perigeeParameters()->momentum().mag() < m_minP
        || indetTrack.perigeeParameters()->momentum().perp() < m_minPt)
    {

        return nullptr;
    }

    ++m_recoveryAttempts;

    ATH_MSG_DEBUG("Entering new recovery" << std::endl
                                          << " ID track " << m_printer->print(indetTrack) << std::endl
                                          << " MS track " << m_printer->print(spectrometerTrack) << std::endl
                                          << m_printer->printStations(spectrometerTrack));

    const Trk::TrackParameters* lastIndetPars = nullptr;
    int                         index         = static_cast<int>(indetTrack.trackParameters()->size());

    while (!lastIndetPars && index > 0) {
        --index;
        lastIndetPars = (*indetTrack.trackParameters())[index] ? (*indetTrack.trackParameters())[index] : nullptr;
    }

    if (!lastIndetPars) {
        ATH_MSG_WARNING("ID track parameters don't have error matrix!");
        return nullptr;
    }

    // track builder prefers estimate of inner, middle and outer spectrometer track parameters
    const Trk::TrackParameters* innerParameters  = nullptr;
    const Trk::TrackParameters* middleParameters = nullptr;
    const Trk::TrackParameters* outerParameters  = nullptr;
    const Trk::TrackParameters* lastPars         = lastIndetPars;

    std::vector<const Trk::TrackStateOnSurface*> stations;
    std::set<Muon::MuonStationIndex::StIndex>    etaIndices;
    std::set<Muon::MuonStationIndex::StIndex>    phiIndices;
    std::set<Muon::MuonStationIndex::StIndex>    badEtaIndices;
    std::set<Muon::MuonStationIndex::StIndex>    badPhiIndices;

    unsigned int nmeas = 0;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosit =
        spectrometerTrack.trackStateOnSurfaces()->begin();

    for (; tsosit != spectrometerTrack.trackStateOnSurfaces()->end(); ++tsosit) {
        const Trk::MeasurementBase* meas = (*tsosit)->measurementOnTrack();
        if (!meas) continue;

        if ((*tsosit)->type(Trk::TrackStateOnSurface::Outlier)) continue;

        Identifier id = m_edmHelperSvc->getIdentifier(*meas);
        if (!id.is_valid()) continue;

        Muon::MuonStationIndex::StIndex index       = m_idHelperSvc->stationIndex(id);
        bool                            measuresPhi = m_idHelperSvc->measuresPhi(id);
        ++nmeas;

        if (measuresPhi) {
            if (phiIndices.count(index)) continue;
            ATH_MSG_DEBUG("Adding phi station " << m_idHelperSvc->toString(id));
            phiIndices.insert(index);
        } else if (m_idHelperSvc->isMdt(id) || (m_idHelperSvc->isCsc(id))) {
            if (etaIndices.count(index)) continue;

            ATH_MSG_DEBUG("Adding eta station " << m_idHelperSvc->toString(id));
            etaIndices.insert(index);
        } else {
            continue;
        }

        const Trk::TrackParameters* exPars = nullptr;
        if (lastPars->associatedSurface() == meas->associatedSurface()) {
            ATH_MSG_DEBUG("Using existing pars");
            exPars = lastPars;
        } else {
            exPars =
                m_extrapolator->extrapolate(*lastPars, meas->associatedSurface(), Trk::alongMomentum, false, Trk::muon);
        }

        if (!exPars) {
            ATH_MSG_DEBUG("Failed to extrapolate to station" << m_idHelperSvc->toStringChamber(id));
            continue;
        }

        const Trk::ResidualPull* res = m_residualCalculator->residualPull(meas, exPars, Trk::ResidualPull::Unbiased);

        ATH_MSG_DEBUG(" " << m_idHelperSvc->toStringChamber(id) << "  residual " << m_printer->print(*res));

        if (fabs(res->pull().front()) > m_pullCut) {
            if (measuresPhi) {
                badPhiIndices.insert(index);
            } else {
                badEtaIndices.insert(index);
            }
        }
        delete res;

        if (msgLvl(MSG::DEBUG)) {

            if (!m_idHelperSvc->measuresPhi(id)) {
                const MuonGM::MuonReadoutElement* detEl = nullptr;

                if (m_idHelperSvc->isMdt(id)) {
                    const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(meas);

                    if (mdt) {
                        detEl = mdt->detectorElement();
                    }
                } else if (m_idHelperSvc->isCsc(id)) {
                    const Muon::CscClusterOnTrack* csc = dynamic_cast<const Muon::CscClusterOnTrack*>(meas);

                    if (csc) {
                        detEl = csc->detectorElement();
                    }
                }

                if (detEl) {
                    const Trk::PlaneSurface* detSurf = dynamic_cast<const Trk::PlaneSurface*>(&detEl->surface());

                    if (detSurf) {
                        Trk::LocalDirection* idDir = new Trk::LocalDirection;
                        detSurf->globalToLocalDirection(exPars->momentum(), *idDir);

                        const Trk::TrackParameters* pars  = (*tsosit)->trackParameters();
                        Trk::LocalDirection*        msDir = new Trk::LocalDirection;
                        detSurf->globalToLocalDirection(pars->momentum(), *msDir);

                        if (idDir && msDir) {
                            ATH_MSG_DEBUG(" local Angles: id (" << idDir->angleXZ() << "," << idDir->angleYZ()
                                                                << ")  ms (" << msDir->angleXZ() << ","
                                                                << msDir->angleYZ() << ")");
                        }
                        delete idDir;
                        delete msDir;
                    }
                }
            }
        }  // end DEBUG toggle

        if (lastPars == lastIndetPars) {
            if (!innerParameters && exPars != lastPars) {
                innerParameters = exPars;
            }
        } else if (lastPars != exPars) {
            if (innerParameters && !middleParameters && middleParameters != exPars) {
                middleParameters = exPars;
            } else if (lastPars != innerParameters && lastPars != middleParameters) {
                delete lastPars;
            }
        }

        lastPars = exPars;
    }

    if (middleParameters) {
        if (lastPars != middleParameters) {
            outerParameters = lastPars;
        }
    } else {
        middleParameters = innerParameters;
        innerParameters  = nullptr;

        if (!middleParameters) {
            ATH_MSG_DEBUG("parameter extrapolation failed");
            return nullptr;
        }
    }

    bool cleanEta = badEtaIndices.size() == 1 && etaIndices.size() > 1;
    bool cleanPhi = badPhiIndices.size() == 1;

    if (!cleanPhi && !cleanEta) {
        ATH_MSG_DEBUG("No layers removed");
        delete innerParameters;
        delete middleParameters;
        delete outerParameters;
        return nullptr;
    }

    if (badEtaIndices.size() == etaIndices.size()) {
        ATH_MSG_DEBUG("All layers removed");
        delete innerParameters;
        delete middleParameters;
        delete outerParameters;
        return nullptr;
    }

    Trk::MeasurementSet spectrometerMeasurements;
    tsosit = spectrometerTrack.trackStateOnSurfaces()->begin();
    for (; tsosit != spectrometerTrack.trackStateOnSurfaces()->end(); ++tsosit) {
        const Trk::MeasurementBase* meas = (*tsosit)->measurementOnTrack();
        if (!meas) continue;
        if ((*tsosit)->type(Trk::TrackStateOnSurface::Outlier)) continue;

        Identifier id = m_edmHelperSvc->getIdentifier(*meas);
        if (!id.is_valid()) continue;

        Muon::MuonStationIndex::StIndex index       = m_idHelperSvc->stationIndex(id);
        bool                            measuresPhi = m_idHelperSvc->measuresPhi(id);
        if (cleanEta && !measuresPhi && badEtaIndices.count(index)) continue;
        if (cleanPhi && measuresPhi && badPhiIndices.count(index)) continue;
        spectrometerMeasurements.push_back(meas);
    }

    ATH_MSG_DEBUG("Number of measurements before cleaning " << nmeas << " after cleaning "
                                                            << spectrometerMeasurements.size());

    if (spectrometerMeasurements.size() < 6) {
        ATH_MSG_DEBUG("Too few hits left - discarding fit");
        delete innerParameters;
        delete middleParameters;
        delete outerParameters;
        return nullptr;
    }

    // fit the combined track
    Trk::Track* combinedTrack = nullptr;
    if (!m_trackBuilder.empty()) {
        combinedTrack = m_trackBuilder->indetExtension(indetTrack, spectrometerMeasurements, innerParameters,
                                                       middleParameters, outerParameters);
    }

    delete innerParameters;
    delete middleParameters;
    delete outerParameters;

    if (combinedTrack) {
        ++m_recoverySuccess;
        combinedTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidMuonRecoveryTool);

        ATH_MSG_DEBUG("Recovered track " << std::endl
                                         << m_printer->print(*combinedTrack) << std::endl
                                         << m_printer->printStations(*combinedTrack));
    } else {
        ++m_recoveryFitFailure;
        ATH_MSG_DEBUG("track fit failure ");
    }

    return combinedTrack;
}


}  // namespace Rec
