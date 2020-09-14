/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentFittingTool.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkTrack/Track.h"

namespace Muon {

MuonSegmentFittingTool::MuonSegmentFittingTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t, n, p), m_magFieldProperties(Trk::NoField)
{
    declareInterface<IMuonSegmentFittingTool>(this);
    declareProperty("UpdatePrecisionCoordinate", m_updatePrecisionCoordinate = false);
}

StatusCode
MuonSegmentFittingTool::initialize()
{
    ATH_CHECK(m_slPropagator.retrieve());
    ATH_CHECK(m_slTrackFitter.retrieve());
    ATH_CHECK(m_curvedTrackFitter.retrieve());
    ATH_CHECK(m_trackCleaner.retrieve());
    return StatusCode::SUCCESS;
}

Trk::Track*
MuonSegmentFittingTool::fit(const MuonSegment& segment) const
{
    return fit(segment.globalPosition(), segment.globalDirection(), segment.associatedSurface(),
               segment.containedMeasurements());
}

Trk::Track*
MuonSegmentFittingTool::fit(const Amg::Vector3D& gpos, const Amg::Vector3D& gDir, const Trk::PlaneSurface& surf,
                            const std::vector<const Trk::MeasurementBase*>& rioVec) const
{


    ATH_MSG_VERBOSE(" trying to fit segment ");
    Amg::Vector3D gdir = gDir;

    double gdirNorm        = gdir.mag();
    bool   isCurvedSegment = (gdirNorm > 1.5 ? true : false);
    if (isCurvedSegment) {
        // re-normalize the gdir
        gdir = Amg::Vector3D(gdir.x() / gdirNorm, gdir.y() / gdirNorm, gdir.z() / gdirNorm);
    }

    double        charge = 0;
    Trk::AtaPlane segPars(gpos, gdir, charge, surf);

    // extrapolate segment parameters to first measurements
    const Trk::MeasurementBase* firstMeas = rioVec.front();
    const Trk::TrackParameters* exPars    = m_slPropagator->propagate(segPars, firstMeas->associatedSurface(),
                                                                   Trk::anyDirection, false, m_magFieldProperties);
    if (!exPars) {
        ATH_MSG_DEBUG(" Propagation failed!! ");
        return 0;
    }

    // small shift towards the ip
    double        sign   = exPars->position().dot(exPars->momentum()) > 0 ? 1. : -1.;
    Amg::Vector3D perpos = exPars->position() - sign * exPars->momentum().unit();
    delete exPars;
    exPars = 0;

    // create start parameter
    double              phi    = gdir.phi();
    double              theta  = gdir.theta();
    double              qoverp = 0;
    Trk::PerigeeSurface persurf(perpos);
    Trk::Perigee        startpar(0, 0, phi, theta, qoverp, persurf);

    // copy measurements into new vector
    std::vector<const Trk::MeasurementBase*> vec2;
    vec2.reserve(rioVec.size());
    std::copy(rioVec.begin(), rioVec.end(), std::back_inserter(vec2));

    // fit
    Trk::ParticleSwitcher particleSwitch;
    Trk::Track*           newtrack;
    // use the full fitter if the segment is curved
    if (isCurvedSegment) {
        newtrack = m_curvedTrackFitter->fit(vec2, startpar, false, particleSwitch.particle[0]);
    }
    // else use the straight line fitter
    else
    {
        newtrack = m_slTrackFitter->fit(vec2, startpar, false, Trk::nonInteracting);
    }

    if (!newtrack) {
        ATH_MSG_VERBOSE("     fit failed ");
        return 0;
    }

    std::unique_ptr<Trk::Track> cleanTrack = m_trackCleaner->clean(*newtrack);
    if (!cleanTrack && !isCurvedSegment) {
        ATH_MSG_VERBOSE("     lost in cleaner ");
        delete newtrack;
        return 0;
    }

    if (!(*cleanTrack->perigeeParameters() == *newtrack->perigeeParameters()) && !isCurvedSegment) {
        delete newtrack;
        // using release until the entire code can be migrated to use smart pointers
        newtrack = cleanTrack.release();
    }

    const Trk::FitQuality* fq = newtrack->fitQuality();
    if (!fq) {
        // no fit quality!!, discard track
        ATH_MSG_WARNING(" track without fit quality!! ");
        delete newtrack;
        return 0;
    }

    double reducedChi2 = fq->chiSquared() / fq->numberDoF();
    double cut         = 10.;
    // reject fit if larger than cut
    if (reducedChi2 > cut) {
        ATH_MSG_VERBOSE("     reduced chi2 to large " << reducedChi2 << "  cut  " << cut);
        delete newtrack;
        return 0;
    }

    if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG(std::setprecision(5) << " chi2 " << fq->chiSquared() << " ndof " << fq->numberDoF());
        const Trk::Perigee* pp = newtrack->perigeeParameters();
        if (pp) {
            ATH_MSG_DEBUG(" pos " << std::setprecision(5) << pp->position() << " phi " << pp->momentum().phi()
                                  << " theta " << pp->momentum().theta() << " q*mom "
                                  << pp->momentum().mag() * pp->charge() << " pt " << pp->momentum().perp());
        } else {
            ATH_MSG_DEBUG(" no perigee ");
        }
    }
    return newtrack;
}

void
MuonSegmentFittingTool::updateSegmentParameters(const Trk::Track& track, const Trk::PlaneSurface& surf,
                                                Amg::Vector2D& segLocPos, Trk::LocalDirection& segLocDir,
                                                Amg::MatrixX& locerr) const
{

    ATH_MSG_DEBUG(" old segment parameters: pos (" << segLocPos[Trk::locX] << "," << segLocPos[Trk::locY] << ")  dir ("
                                                   << segLocDir.angleXZ() << "," << segLocDir.angleYZ() << ") ");


    const Trk::Perigee* pp = track.perigeeParameters();
    if (!pp) {
        ATH_MSG_WARNING(" track without perigee ");
        return;
    }

    const Trk::TrackParameters* exPars =
        m_slPropagator->propagate(*pp, surf, Trk::anyDirection, false, m_magFieldProperties);
    if (!exPars) {
        ATH_MSG_WARNING(" extrapolation failed, this should not happen ");
        return;
    }

    Amg::Vector2D lpos;
    surf.globalToLocal(exPars->position(), exPars->position(), lpos);
    Trk::LocalDirection ldir;
    surf.globalToLocalDirection(exPars->momentum().unit(), ldir);
    ATH_MSG_DEBUG(" new segment parameters: pos (" << lpos[Trk::locX] << "," << lpos[Trk::locY] << ")  dir ("
                                                   << ldir.angleXZ() << "," << ldir.angleYZ() << ") ");

    segLocPos[Trk::locX] = lpos[Trk::locX];
    if (m_updatePrecisionCoordinate) segLocPos[Trk::locY] = lpos[Trk::locY];
    segLocDir = Trk::LocalDirection(ldir.angleXZ(), m_updatePrecisionCoordinate ? ldir.angleYZ() : segLocDir.angleYZ());

    if (exPars->covariance()) locerr = *exPars->covariance();

    delete exPars;
}

}  // namespace Muon
