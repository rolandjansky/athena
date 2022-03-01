/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuTagMatchingTool.h"

#include <cmath>

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventPrimitives/JacobianPhiThetaLocalAngles.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/Track.h"

MuTagMatchingTool::MuTagMatchingTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p) {
    declareInterface<IMuTagMatchingTool>(this);

    declareProperty("GlobalThetaCut", m_GLOBAL_THETA_CUT = 0.1);
    declareProperty("GlobalPhiCut", m_GLOBAL_PHI_CUT = 0.5);
    declareProperty("GlobalRCut", m_GLOBAL_R_CUT = 1000.);

    declareProperty("ThetaCut", m_MATCH_THETA = 5.);            // pull cut in local precision position (bending plane)
    declareProperty("PhiCut", m_MATCH_PHI = 30.);               // pull cut in local NON precision position
    declareProperty("ThetaAngleCut", m_MATCH_THETAANGLE = 5.);  // pull cut in local precision direction
    declareProperty("PhiAngleCut", m_MATCH_PHIANGLE = 30.);     // pull cut in local NON precision direction

    declareProperty("ErrorProtectionPosCut", m_ERROR_EX_POS_CUT = 5000.);
    declareProperty("ErrorProtectionDirCut", m_ERROR_EX_ANGLE_CUT = 0.35);

    declareProperty("DoDistanceCut", m_doDistCut = true);
    declareProperty("PositionDistance", m_DIST_POS = 2000.);
    declareProperty("AngleDistance", m_DIST_ANGLE = 0.7);

    declareProperty("ThetaSafety",
                    m_SAFE_THETA = 25.);              // additional uncertainty on precision plane position (bendig plane)
    declareProperty("PhiSafety", m_SAFE_PHI = 100.);  //  additional uncertainty on NON precision plane position
    declareProperty("ThetaAngleSafety",
                    m_SAFE_THETAANGLE = 0.0015);  //  additional uncertainty on precision plane direction
    declareProperty("PhiAngleSafety",
                    m_SAFE_PHIANGLE = 0.25);  //  additional uncertainty on NON precision plane direction

    declareProperty("ChamberPullCut",
                    m_chamberPullCut = 5.);  // Inside chamber pull is negative for outside chamber pull > 5 standard
                                             // deviations (from edge) in non precision plane
    declareProperty("CombinedPullCut",
                    m_combinedPullCut = 5.);  // Combined pull build from local precision position and angle including the correlation

    declareProperty("AssumeLocalErrors", m_assumeLocalErrors = true);
    declareProperty("ExtrapolatePerigee", m_extrapolatePerigee = false);
    declareProperty("MuonChamberT0s", m_t0Location = "MooreMuonChamberT0s");
}

StatusCode MuTagMatchingTool::initialize() {
    ATH_MSG_DEBUG("================================");
    ATH_MSG_DEBUG("=Proprieties are ");
    ATH_MSG_DEBUG("GlobalThetaCut" << std::setw(10) << m_GLOBAL_THETA_CUT);
    ATH_MSG_DEBUG("GlobaPhiCut" << std::setw(10) << m_GLOBAL_PHI_CUT);
    ATH_MSG_DEBUG("GlobalRCut" << std::setw(10) << m_GLOBAL_R_CUT);
    ATH_MSG_DEBUG("ThetaCut" << std::setw(10) << m_MATCH_THETA);
    ATH_MSG_DEBUG("PhiCut" << std::setw(10) << m_MATCH_PHI);
    ATH_MSG_DEBUG("ThetaAngleCut" << std::setw(10) << m_MATCH_THETAANGLE);
    ATH_MSG_DEBUG("PhiAngleCut" << std::setw(10) << m_MATCH_PHIANGLE);
    ATH_MSG_DEBUG("DoDistanceCut" << std::setw(10) << m_doDistCut);
    ATH_MSG_DEBUG("ThetaDistance" << std::setw(10) << m_DIST_POS);
    ATH_MSG_DEBUG("ThetaAngleDistance" << std::setw(10) << m_DIST_ANGLE);
    ATH_MSG_DEBUG("ThetaSafety" << std::setw(10) << m_SAFE_THETA);
    ATH_MSG_DEBUG("PhiSafety" << std::setw(10) << m_SAFE_PHI);
    ATH_MSG_DEBUG("ThetaAngleSafety" << std::setw(10) << m_SAFE_THETAANGLE);
    ATH_MSG_DEBUG("PhiAngleSafety" << std::setw(10) << m_SAFE_PHIANGLE);

    ATH_MSG_DEBUG("AssumeLocalErrors " << std::setw(10) << m_assumeLocalErrors);
    ATH_MSG_DEBUG("ExtrapolatePerigee " << std::setw(10) << m_extrapolatePerigee);
    ATH_MSG_DEBUG("================================");

    // Retrieve m_IExtrapolator
    ATH_CHECK(m_IExtrapolator.retrieve());
    ATH_MSG_DEBUG("Retrieved tool " << m_IExtrapolator);

    ATH_CHECK(m_propagator.retrieve());

    ATH_CHECK(m_hitSummaryTool.retrieve());
    ATH_CHECK(m_selectionTool.retrieve());

    // MuonDetectorManager from the conditions store
    ATH_CHECK(m_DetectorManagerKey.initialize());

    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_pullCalculator.retrieve());

    if (!m_trackingGeometryReadKey.empty()) {
        ATH_CHECK(m_trackingGeometryReadKey.initialize());
    } else {
        ATH_MSG_ERROR("Could not retrieve a valid tracking geometry");
    }

    return StatusCode::SUCCESS;
}

bool MuTagMatchingTool::match(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment,
                              int surfaceName) const {
    ///////////////////////////////////////////////////////////////////////////
    // performs a rough match to an abstract MS surface (disc or cylinder)
    // by matching on station name (I M O, S L, B E) and a crude phi-position
    // match, followed by either a match in theta-position or r-position
    ///////////////////////////////////////////////////////////////////////////    
    if (!surfaceMatch(segment, surfaceName)) return false;

    if (!phiMatch(atSurface, segment)) return false;

    const Identifier id = m_edmHelperSvc->chamberId(segment);

    /// We need a barrel function here
    if (m_idHelperSvc->regionIndex(id) == Muon::MuonStationIndex::Barrel)
        return thetaMatch(atSurface, segment);
    else
        return rMatch(atSurface, segment);
}



bool MuTagMatchingTool::surfaceMatch(const Muon::MuonSegment& segment, int surfaceName) const {    
    const Identifier id = m_edmHelperSvc->chamberId(segment);
    return surfaceName == MuonCombined::MuonSegmentTagSurfaces::stIdxToSurfDef(m_idHelperSvc->stationIndex(id), m_idHelperSvc->stationEta(id)>0);
}

/** Get extrapolation at MS entrance: this will be the starting point to following extrapolations*/
std::unique_ptr<Trk::TrackParameters> MuTagMatchingTool::ExtrapolateTrktoMSEntrance(const EventContext& ctx, const Trk::Track& pTrack,
                                                                                          Trk::PropDirection direction) const {
  

    const Trk::TrackingVolume* MSEntranceVolume = getVolume(ctx, "MuonSpectrometerEntrance");
    if (!MSEntranceVolume) { return nullptr; }

    std::unique_ptr<Trk::TrackParameters> exTrk{
        m_IExtrapolator->extrapolateToVolume(ctx, *(pTrack.perigeeParameters()), *MSEntranceVolume, direction, Trk::muon)};

    if (!exTrk)
        ATH_MSG_DEBUG("Track could not be extrapolated to MS entrance...");
    else
        ATH_MSG_DEBUG("Track extrapolated to MS entrance Radius " << exTrk->position().perp() << " Z " << exTrk->position().z());

    return exTrk;
}

/** Get extrapolation at MSSurface level*/
std::unique_ptr<Trk::TrackParameters> MuTagMatchingTool::ExtrapolateTrktoMSSurface(const EventContext& ctx,
                                                                                         const Trk::Surface& pSurface,
                                                                                         const Trk::TrackParameters& pTrack,
                                                                                         Trk::PropDirection direction) const {
    std::unique_ptr<Trk::TrackParameters> exTrk{m_IExtrapolator->extrapolate(ctx, pTrack, pSurface, direction, false, Trk::muon)};
    if (!exTrk) {
        ATH_MSG_DEBUG(" didn't manage to extrapolate TrackParameters to abstract surface Radius " << pSurface.center().perp() << " Z "
                                                                                                  << pSurface.center().z());
    }
    return exTrk;
}

std::unique_ptr<Trk::Perigee> MuTagMatchingTool::flipDirection(const Trk::Perigee& inputPars) const {
    const AmgVector(5)& pars = inputPars.parameters();

    double flippedPhi = pars[2] + M_PI;
    if (flippedPhi > M_PI) flippedPhi -= 2 * M_PI;
    double flippedTheta = M_PI - pars[3];
    if (flippedTheta < 0.) flippedTheta += M_PI;
    if (inputPars.covariance()) {
        AmgSymMatrix(5) covMat = AmgSymMatrix(5)(*inputPars.covariance());
        const Trk::PerigeeSurface perSurf;
        return std::make_unique<Trk::Perigee>(-pars[0], pars[1], flippedPhi, flippedTheta, pars[4], perSurf, covMat);
    } else {
        ATH_MSG_DEBUG("flipDirection: no covariance associated to input parameters " << inputPars);
        return nullptr;
    }
}

/** Get extrapolation at Segment Plane Surface level*/
std::shared_ptr<Trk::AtaPlane> MuTagMatchingTool::ExtrapolateTrktoSegmentSurface(const EventContext& ctx,
                                                                                 const Muon::MuonSegment& segment,
                                                                                 const Trk::TrackParameters& pTrack,
                                                                                 Trk::PropDirection direction) const {
    
    unsigned int hits{cscHits(segment)};
    bool isCsc{hits>0};   
    if (isCsc) {
        ATH_MSG_DEBUG("CSC segment has " << hits << " hits");
        if (hits < 5) {
            ATH_MSG_DEBUG("CSC segment is identified to be 2D. Not extrapolating.");
            return nullptr;
        }
    }

    std::shared_ptr<Trk::TrackParameters> atap{
        m_IExtrapolator->extrapolate(ctx, pTrack, segment.associatedSurface(), direction, false, Trk::muon)};
    if (!atap) {
        ATH_MSG_DEBUG(" didn't manage to extrapolate TrackParameters to segment surface Radius"
                      << segment.associatedSurface().center().perp() << " Z " << segment.associatedSurface().center().z());
        return nullptr;
    }

    std::shared_ptr<Trk::AtaPlane> matap = std::dynamic_pointer_cast<Trk::AtaPlane>(atap);

    if (!matap) {
        ATH_MSG_DEBUG(" didn't manage to extrapolate MeasuredTrackParameters to segment surface atap Radius "
                      << atap->position().perp() << " Z" << atap->position().z());
        return nullptr;
    } else {
        ATH_MSG_DEBUG(" Succesfull extrapolation segment surface matap Radius " << matap->position().perp() << " Z"
                                                                                << matap->position().z());
    }   
    return matap;
}

bool MuTagMatchingTool::phiMatch(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment) const {
    double PHI_CUT = m_GLOBAL_PHI_CUT;
    if (hasPhi(segment)) PHI_CUT = m_GLOBAL_PHI_CUT / 2.;

    const Amg::Vector3D exTrkPos = atSurface.position();
    const Amg::Vector3D& segPos = segment.globalPosition();

    const double deltaPhi = exTrkPos.deltaPhi(segPos);
    const Identifier id = m_edmHelperSvc->chamberId(segment);

    double sigma_phi = 0.;
    if (atSurface.associatedSurface().type() == Trk::SurfaceType::Cylinder) {
        const AmgSymMatrix(5)& covAtCyl = *atSurface.covariance();
        double r = atSurface.associatedSurface().bounds().r();            
        if ( r > 0 && covAtCyl(Trk::locRPhi, Trk::locRPhi) >= 0.) sigma_phi = Amg::error(covAtCyl, Trk::locRPhi) / r;
    } else if (atSurface.associatedSurface().type() == Trk::SurfaceType::Plane) {
            const AmgSymMatrix(5)& covAtPlane = *atSurface.covariance();
            sigma_phi = Amg::error(covAtPlane, Trk::phi0);
    } else if (atSurface.associatedSurface().type() == Trk::SurfaceType::Disc) {  // error in endcap
        const AmgSymMatrix(5)& covAtDisc = *atSurface.covariance();
        if (covAtDisc(Trk::locPhi, Trk::locPhi) >= 0.) sigma_phi = Amg::error(covAtDisc, Trk::locPhi);    
    }
    double errPhi = std::hypot(PHI_CUT, sigma_phi);
    // if the difference between exTrk and Segment phi position falls within the errPhi, accept as rough match
    if (std::abs(deltaPhi) < errPhi)
        return true;  // BRes: TEMPORARY - segment direction not sure, so I'm making this match symmetric wrt Pi/2    
    ATH_MSG_DEBUG(std::setw(30) << "roughPhi failed:  d_phi = " << std::setw(10) << std::abs(deltaPhi)  // BRes: TEMPORARY - same
                                    << " while the cut is set on " << std::setw(10) << errPhi);
    return false;
}

bool MuTagMatchingTool::hasPhi(const Muon::MuonSegment& seg) const {
    Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(seg);
    return hitCounts.nexpectedTrigHitLayers > 1;
}

bool MuTagMatchingTool::thetaMatch(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment) const {
    double THETA_CUT = m_GLOBAL_THETA_CUT;
    const Amg::Vector3D exTrkPos = atSurface.position();
    const Amg::Vector3D& segPos = segment.globalPosition();
    const double dTheta = std::abs(exTrkPos.theta() - segPos.theta());
    if (dTheta < THETA_CUT)
        return true;
    
    ATH_MSG_DEBUG(std::setw(30) << "roughTheta failed: d_theta = " << std::setw(10) << dTheta << " while the cut is set on "
                                    << std::setw(10) << THETA_CUT);
    return false;
}

bool MuTagMatchingTool::rMatch(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment) const {
    const Amg::Vector3D exTrkPos = atSurface.position();
    double L = exTrkPos.mag();
    double R_CUT = m_GLOBAL_R_CUT * (L / 7500.);  // mm

    const Amg::Vector3D& segPos = segment.globalPosition();
    const double dPerp = std::abs(exTrkPos.perp() - segPos.perp());
    if (dPerp < R_CUT)
        return true;
    
    ATH_MSG_DEBUG(std::setw(30) << "rough  R   failed:   d_R    = " << std::setw(10) << dPerp << " while the cut is set on "
                                    << std::setw(10) << R_CUT);
    return false;
}

double MuTagMatchingTool::errorProtection(double exTrk_Err, bool isAngle) const {
    double newError = exTrk_Err;
    if (!isAngle && (exTrk_Err > m_ERROR_EX_POS_CUT)) newError = m_ERROR_EX_POS_CUT;
    if (isAngle && (exTrk_Err > m_ERROR_EX_ANGLE_CUT)) newError = m_ERROR_EX_ANGLE_CUT;

    return newError;
}

bool MuTagMatchingTool::matchSegmentPosition(const MuonCombined::MuonSegmentInfo& info, bool idHasEtaHits) const {    
    if (!info.segment || !info.trackAtSegment) {
        ATH_MSG_DEBUG(" No segment and or trackAtSegment pointer matchSegmentPosition ");
        return false;
    }

    bool pass(false);
    
    if (idHasEtaHits) {
        if (std::abs(info.pullY) < m_MATCH_THETA || std::abs(info.pullCY) <  m_combinedPullCut) pass = true;
    } else {
        if (std::abs(info.pullX) < m_MATCH_PHI) pass = true;
    }

    return pass;
}
bool MuTagMatchingTool::matchSegmentDirection(const MuonCombined::MuonSegmentInfo& info, bool idHasEtaHits) const {   
    if (!info.segment || !info.trackAtSegment) {
        ATH_MSG_DEBUG(" No segment and or trackAtSegment pointer matchSegmentDirection ");
        return false;
    } 
    if (idHasEtaHits) {
        return std::abs(info.pullYZ) < m_MATCH_THETAANGLE || std::abs(info.pullCY) <  m_combinedPullCut;
    }
     return true;  
}

bool MuTagMatchingTool::matchCombinedPull(const MuonCombined::MuonSegmentInfo& info) const {
    bool pass(true);    
    if (!info.segment || !info.trackAtSegment) {
        ATH_MSG_DEBUG(" No segment and or trackAtSegment pointer matchCombinedPull ");
        return false;
    }
    if (info.pullChamber > m_chamberPullCut) pass = false;
    if (std::abs(info.pullCY) > m_combinedPullCut) pass = false;
    
    ATH_MSG_DEBUG(" matchCombinedPull MuTagSegmentInfo hasPhi "
                  << info.hasPhi << " minimumPullPhi " << info.minimumPullPhi << " pullChamber " << info.pullChamber << " cut "
                  << m_chamberPullCut << " pullCY " << info.pullCY << " cut m_combinedPullCut " << m_combinedPullCut << " pass " << pass);

    return pass;
}

bool MuTagMatchingTool::matchPtDependentPull(const MuonCombined::MuonSegmentInfo& info, const Trk::Track& trk) const {
    bool pass(true);  
    if (!info.segment || !info.trackAtSegment) {
        ATH_MSG_DEBUG(" No segment and or trackAtSegment pointermatchPtDependentPull ");
        return false;
    }
    double pT(100.);
    const Trk::Perigee* aMeasPer = trk.perigeeParameters();
    if (aMeasPer) {
        pT = aMeasPer->pT();
    }
    double Pullcut = 2.0;

    if (pT > 2.) { Pullcut = 5.0 - 6.0 / pT; }
    if (Pullcut > m_combinedPullCut) Pullcut = m_combinedPullCut;

    if (std::abs(info.pullCY) > Pullcut) pass = false;

    return pass;
}

void MuTagMatchingTool::testExtrapolation(const Trk::Surface& pSurface, const Trk::Track& pTrack) const {
    const Trk::Perigee* oriPerigee = pTrack.perigeeParameters();
    if (!oriPerigee) {
        ATH_MSG_DEBUG("Couldn't get the measured Perigee from TP");
        return;
    }
    const EventContext& ctx = Gaudi::Hive::currentContext(); 
    const AmgVector(5)& oripars = oriPerigee->parameters();
    const Trk::PerigeeSurface periSurf;
    std::unique_ptr<Trk::Perigee> pPerigee = std::make_unique<Trk::Perigee>(oripars[0], oripars[1], oripars[2], oripars[3], 0., periSurf, std::nullopt);
    const Amg::Vector3D startPos = pPerigee->position();
    const Amg::Vector3D startMom = pPerigee->momentum();
    const AmgVector(5)& pars = pPerigee->parameters();

    ATH_MSG_DEBUG("==============================================================================");
    ATH_MSG_DEBUG("======= INITIAL TRACK PARAMETERS (PERIGEE)");
    ATH_MSG_DEBUG("=== parameters are " << pars[0] << "  " << pars[1] << " " << pars[2] << "  " << pars[3] << " " << pars[4]);
    ATH_MSG_DEBUG("=== global position " << startPos.x() << "  " << startPos.y() << "  " << startPos.z());
    ATH_MSG_DEBUG("=== global directn  " << startMom.phi() << "  " << startMom.theta());

    std::unique_ptr<Trk::TrackParameters> alongPars{
        m_IExtrapolator->extrapolate(ctx, *pPerigee, pSurface, Trk::alongMomentum, false, Trk::muon)};

    ATH_MSG_DEBUG("======= EXTRAPOLATED ALONG MOMENTUM ORIGINAL PERIGEE");
    if (alongPars) {
        const Amg::Vector3D alongPos = alongPars->position();
        const Amg::Vector3D alongMom = alongPars->momentum();

        ATH_MSG_DEBUG("=== global position " << alongPos.x() << "  " << alongPos.y() << "  " << alongPos.z());
        ATH_MSG_DEBUG("=== global position phi theta " << alongPos.phi() << "  " << alongPos.theta());
        ATH_MSG_DEBUG("=== global directn  " << alongMom.phi() << "  " << alongMom.theta());
    } else
        ATH_MSG_DEBUG("======= NOT EXTRAPOLATED");

    ATH_MSG_DEBUG("======= EXTRAPOLATED OPPOSITE MOMENTUM ORIGINAL PERIGEE");
    std::unique_ptr<const Trk::TrackParameters> oppositePars{
        m_IExtrapolator->extrapolate(ctx, *pPerigee, pSurface, Trk::oppositeMomentum, false, Trk::muon)};
    if (oppositePars) {
        const Amg::Vector3D oppositePos = oppositePars->position();
        const Amg::Vector3D oppositeMom = oppositePars->momentum();
        ATH_MSG_DEBUG("=== global position " << oppositePos.x() << "  " << oppositePos.y() << "  " << oppositePos.z());
        ATH_MSG_DEBUG("=== global position phi theta " << oppositePos.phi() << "  " << oppositePos.theta());
        ATH_MSG_DEBUG("=== global directn  " << oppositeMom.phi() << "  " << oppositeMom.theta());
    } else
        ATH_MSG_DEBUG("======= NOT EXTRAPOLATED");

 ;
    std::unique_ptr<const Trk::Perigee> flippedPerigee = flipDirection(*pPerigee);
    const AmgVector(5)& flipPars = flippedPerigee->parameters();
    const Amg::Vector3D flipPos = flippedPerigee->position();
    const Amg::Vector3D flipMom = flippedPerigee->momentum();

    ATH_MSG_DEBUG("======= FLIPPED TRACK PARAMETERS (PERIGEE)");
    ATH_MSG_DEBUG("=== phi and theta were " << pars[2] << "  " << pars[3] << " and are flipped to ");
    ATH_MSG_DEBUG("=== parameters are " << flipPars[0] << "  " << flipPars[1] << "  " << flipPars[2] << "  " << flipPars[3] << "  "
                                        << flipPars[4]);
    ATH_MSG_DEBUG("=== global position " << flipPos.x() << "  " << flipPos.y() << "  " << flipPos.z());
    ATH_MSG_DEBUG("=== global directn  " << flipMom.phi() << "  " << flipMom.theta());

    std::unique_ptr<const Trk::TrackParameters> alongFlipPars{
        m_IExtrapolator->extrapolate(ctx,*flippedPerigee, pSurface, Trk::alongMomentum, false, Trk::muon)};

    ATH_MSG_DEBUG("======= EXTRAPOLATED ALONGFLIP MOMENTUM ORIGINAL PERIGEE");
    if (alongFlipPars) {
        const Amg::Vector3D alongFlipPos = alongFlipPars->position();
        const Amg::Vector3D alongFlipMom = alongFlipPars->momentum();
        ATH_MSG_DEBUG("=== global position " << alongFlipPos.x() << "  " << alongFlipPos.y() << "  " << alongFlipPos.z());
        ATH_MSG_DEBUG("=== global position phi theta " << alongFlipPos.phi() << "  " << alongFlipPos.theta());
        ATH_MSG_DEBUG("=== global directn  " << alongFlipMom.phi() << "  " << alongFlipMom.theta());
    } else
        ATH_MSG_DEBUG("======= NOT EXTRAPOLATED");

    ATH_MSG_DEBUG("======= EXTRAPOLATED OPPOSITEFLIP MOMENTUM ORIGINAL PERIGEE");
    std::unique_ptr<const Trk::TrackParameters> oppositeFlipPars{
        m_IExtrapolator->extrapolate(ctx, *flippedPerigee, pSurface, Trk::oppositeMomentum, false, Trk::muon)};
    if (oppositeFlipPars) {
        const Amg::Vector3D oppositeFlipPos = oppositeFlipPars->position();
        const Amg::Vector3D oppositeFlipMom = oppositeFlipPars->momentum();
        ATH_MSG_DEBUG("=== global position " << oppositeFlipPos.x() << "  " << oppositeFlipPos.y() << "  " << oppositeFlipPos.z());
        ATH_MSG_DEBUG("=== global position phi theta " << oppositeFlipPos.phi() << "  " << oppositeFlipPos.theta());
        ATH_MSG_DEBUG("=== global directn  " << oppositeFlipMom.phi() << "  " << oppositeFlipMom.theta());
    } else
        ATH_MSG_DEBUG("======= NOT EXTRAPOLATED");

    ATH_MSG_DEBUG("==============================================================================");
   
}

//==========Counts ROTs and CompetingROTs on segment.
void MuTagMatchingTool::nrTriggerHits(const Muon::MuonSegment& seg, int& nRPC, int& nTGC) const {
    nRPC = 0;
    nTGC = 0;
    for (const Trk::MeasurementBase* seg_meas : seg.containedMeasurements()) {
        const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(seg_meas);
        if (!rot) {
            const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(seg_meas);
            if (crot) rot = &crot->rioOnTrack(0);
        }
        if (!rot) { continue; }
        if (m_idHelperSvc->isRpc(rot->identify())) ++nRPC;
        if (m_idHelperSvc->isTgc(rot->identify())) ++nTGC;
    }
}

MuonCombined::MuonSegmentInfo MuTagMatchingTool::muTagSegmentInfo(const EventContext& ctx, const Trk::Track* track, const Muon::MuonSegment& segment,
                                                                  std::shared_ptr<const Trk::AtaPlane> exTrack) const {
    MuonCombined::MuonSegmentInfo info;
  
    //  segment and track pointers

    info.track = track;
    info.segment = &segment;
    info.trackAtSegment = exTrack;   

    Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(segment);

    // quality

    info.quality = m_selectionTool->quality(segment);

    // Global angular differences

    info.dtheta = exTrack->momentum().theta() - segment.globalDirection().theta();
    info.dphi =  exTrack->momentum().deltaPhi(segment.globalDirection());

    // Global angular differences for position

    info.dthetaPos = exTrack->position().theta() - segment.globalPosition().theta();
    info.dphiPos = exTrack->position().deltaPhi (segment.globalPosition());

    // Local positions

    info.resY = exTrack->parameters()[Trk::locY] - segment.localParameters()[Trk::locY];  // delta_theta
    info.resX = exTrack->parameters()[Trk::locX] - segment.localParameters()[Trk::locX];  // delta_phi

    // Local errors and covariance Matrix

    info.exCovYTheta = 0.;   
    if (exTrack->covariance()) {
        ATH_MSG_DEBUG("Measured Parameters: error x " << (*exTrack->covariance())(Trk::locX, Trk::locX) << " y "
                                                      << (*exTrack->covariance())(Trk::locY, Trk::locY) << " pull x "
                                                      << info.resX / std::sqrt((*exTrack->covariance())(Trk::locX, Trk::locX)) << " pull y "
                                                      << info.resY / std::sqrt((*exTrack->covariance())(Trk::locY, Trk::locY)));
        info.exErrorX = std::sqrt((*exTrack->covariance())(Trk::locX, Trk::locX));
        info.exErrorY = std::sqrt((*exTrack->covariance())(Trk::locY, Trk::locY));
        if ((*exTrack->covariance())(Trk::theta, Trk::locY) != 0) { info.exCovYTheta = (*exTrack->covariance())(Trk::theta, Trk::locY); }
    } else {
        info.exErrorX = -999.;
        info.exErrorY = -999.;
    }

    // Segment local errors
    info.segErrorX = std::sqrt((segment.localCovariance())(Trk::locX, Trk::locX));
    info.segErrorY = std::sqrt((segment.localCovariance())(Trk::locY, Trk::locY));

    // Pull local X and Y
    double erexX = errorProtection(info.exErrorX, false);
    double erexY = errorProtection(info.exErrorY, false);
    info.pullX = info.resX / std::hypot(erexX, info.segErrorX, m_SAFE_PHI);
    info.pullY = info.resY / std::hypot(erexY, info.segErrorY, m_SAFE_THETA);

    // Local angles

    Trk::LocalDirection segLocDir = segment.localDirection();
    Trk::LocalDirection exTrkLocDir;
    exTrack->associatedSurface().globalToLocalDirection(exTrack->momentum(), exTrkLocDir);
    info.dangleYZ = exTrkLocDir.angleYZ() - segLocDir.angleYZ();  // deltaYZ
    info.dangleXZ = exTrkLocDir.angleXZ() - segLocDir.angleXZ();  // deltaXZ
    // delete exTrkLocDir;

    // Errors on local angles
    double exTrkErrXZ(0.), exTrkErrYZ(0.), segErrXZ(0.), segErrYZ(0.), covLocYYZ(0.);

    calculateLocalAngleErrors(*exTrack, exTrkErrXZ, exTrkErrYZ, covLocYYZ);
    info.exErrorXZ = exTrkErrXZ;
    info.exErrorYZ = exTrkErrYZ;
    info.exCovYZY = covLocYYZ;
    calculateLocalAngleErrors(segment, segErrXZ, segErrYZ);
    info.segErrorXZ = segErrXZ;
    info.segErrorYZ = segErrYZ;
    double erexXZ = errorProtection(info.exErrorXZ, true);
    double erexYZ = errorProtection(info.exErrorYZ, true);

    ATH_MSG_DEBUG(" exTrkLocDir.angleYZ() " << exTrkLocDir.angleYZ() << " segLocDir.angleYZ() " << segLocDir.angleYZ());
    ATH_MSG_DEBUG(" exTrkLocDir.angleXZ() " << exTrkLocDir.angleXZ() << " segLocDir.angleXZ() " << segLocDir.angleXZ());

    ATH_MSG_DEBUG(" info.exErrorYZ  " << info.exErrorYZ << " info.segErrorYZ " << info.segErrorYZ << " info.exCovYZY " << info.exCovYZY);
    ATH_MSG_DEBUG(" info.exErrorXZ  " << info.exErrorXZ << " info.segErrorXZ " << info.segErrorXZ);

    info.pullXZ = info.dangleXZ / std::hypot(erexXZ, info.segErrorXZ, m_SAFE_PHIANGLE);
    info.pullYZ = info.dangleYZ / std::hypot(erexYZ, info.segErrorYZ, m_SAFE_THETAANGLE);

    ATH_MSG_DEBUG(" info.pullXZ " << info.pullXZ);

    Identifier chId = m_edmHelperSvc->chamberId(segment);
    Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chId);
    //
    //  residuals and pulls in X coordinate (along tube)
    //

    // MuonDetectorManager from the conditions store
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey, ctx};
    const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr();
    if (!MuonDetMgr) {
        ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
        return info;
    }
    double maxResXMdt{-1e9}, maxResPhi{-1e9}, maxPullPhi{-1e9}, minResPhi{1e9}, minPullPhi{1e9};
    // loop over hits
    for (const Trk::MeasurementBase* seg_it : segment.containedMeasurements()) {
        const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(seg_it);
        if (mdt) {
            // get id and check that it is a muon hit id
            Identifier id = mdt->identify();

            // get layer index
            int lay = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
            int tube = m_idHelperSvc->mdtIdHelper().tube(id);

            const MuonGM::MdtReadoutElement* detEl =
                mdt->prepRawData() ? mdt->prepRawData()->detectorElement() : MuonDetMgr->getMdtReadoutElement(id);
            if (!detEl) {
                ATH_MSG_WARNING(" could not get MdtReadoutElement for tube " << m_idHelperSvc->toString(id));
                continue;
            }
            double tubeLen = detEl->getActiveTubeLength(lay, tube);

            // use SL within station to speed up extrapolation
            // lifetime only in this scope
            std::unique_ptr<Trk::TrackParameters> exP = m_propagator->propagate(ctx, *exTrack, mdt->associatedSurface(), Trk::anyDirection, false, Trk::NoField);
            if (!exP) {
                ATH_MSG_WARNING("Failed to extrapolate to " << m_idHelperSvc->toString(id));
                continue;
            }
            ATH_MSG_DEBUG(m_idHelperSvc->toString(id)
                          << " exPos " << exP->parameters()[Trk::locR] << " y " << exP->parameters()[Trk::locZ] << " tubeL " << tubeLen);
            double exResidual = std::abs(exP->parameters()[Trk::locZ]) - 0.5 * tubeLen;
            if (maxResXMdt < exResidual) maxResXMdt = exResidual;
            if (exResidual > 0.) ATH_MSG_DEBUG("Extrapolated position outside tube, " << exResidual);
        } else {
            // get id and check that it is a muon hit id
            Identifier id = m_edmHelperSvc->getIdentifier(*seg_it);
            if (!id.is_valid() || !m_idHelperSvc->isMuon(id)) continue;
            if (!m_idHelperSvc->measuresPhi(id)) continue;
            // lifetime only in this scope
            std::unique_ptr<Trk::TrackParameters> exP{
                m_propagator->propagate(ctx, *exTrack, seg_it->associatedSurface(), Trk::anyDirection, false, Trk::NoField)};
            if (!exP) {
                ATH_MSG_WARNING("Failed to extrapolate to " << m_idHelperSvc->toString(id));
                continue;
            }
            std::unique_ptr<const Trk::ResidualPull> resPull{
                m_pullCalculator->residualPull(seg_it, exP.get(), Trk::ResidualPull::Unbiased)};
            if (!resPull) {
                ATH_MSG_WARNING(" calculation of residual/pull failed !!!!! ");
                // delete exP;
                continue;
            }
            const double residual = resPull->residual().front();
            const double pull = resPull->pull().front();
            
            maxResPhi = std::max(residual, maxResPhi);
            minResPhi = std::min(residual, minResPhi);
            maxPullPhi = std::max(pull, maxPullPhi);
            minPullPhi = std::min(pull, minPullPhi);
            
            ATH_MSG_DEBUG(m_idHelperSvc->toString(id) << " residual " << residual << " pull " << pull);
        }
    }
    ATH_MSG_DEBUG("Residual phi min " << minResPhi << " max " << maxResPhi << " pull min " << minPullPhi << " max " << maxPullPhi
                                      << " dist from tube end " << maxResXMdt);

    //
    //   Store the local X variables
    //
    info.maximumResidualAlongTube = maxResXMdt;
    info.maximumResidualPhi = maxResPhi;
    info.maximumPullPhi = maxPullPhi;

    info.minimumResidualPhi = minResPhi;
    info.minimumPullPhi = minPullPhi;

    info.pullChamber = maxResXMdt / info.exErrorX;

    //  Scale covariance Matrix (if needed)
    /// The cut off variable is used to avoid FPEs risen where
    /// one of the matrix enties exceeds a large number and hence
    /// the determinant becomes tremendously larger
    constexpr double matrix_cutoff = 1.e20;
    double a = std::pow(std::min(matrix_cutoff, info.exErrorY), 2);
    double b = std::abs(info.exCovYZY) < matrix_cutoff ? info.exCovYZY : (info.exCovYZY < 0 ? -1. : 1) * matrix_cutoff;
    double d = std::pow(std::min(matrix_cutoff, info.exErrorYZ), 2);
    double det = a * d - b * b;

    double scale = 1.;
    if (det < 0.1 * a * d) {
        scale = std::sqrt(0.9 * a * d) / std::abs(b);
        det = a * d - scale * b * scale * b;
    }
    if (det ==0.) {
        ATH_MSG_DEBUG("Found singular determinant");
        return info;
    }
    double dydyz = scale * info.exCovYZY;
    double correction = dydyz / std::max(info.exErrorYZ * info.exErrorYZ, DBL_EPSILON);

    //
    //  residual after taking into account the correlation with the angle YZ
    //

    info.resCY = info.resY - correction * info.dangleYZ;

    ATH_MSG_DEBUG(" segment direction theta " << segment.globalDirection().theta() << " position theta "
                                              << segment.globalPosition().theta() << " correction " << correction);
    ATH_MSG_DEBUG(" resY " << info.resY << " dangleYZ " << info.dangleYZ << " resCY " << info.resCY);
    double error_rescy = info.exErrorY * info.exErrorY - correction * correction * info.exErrorYZ * info.exErrorYZ;
    double error_segcy = info.segErrorY * info.segErrorY + correction * correction * info.segErrorYZ * info.segErrorYZ;
    if (error_rescy > 0) {
        error_rescy = std::sqrt(error_rescy + error_segcy);
    } else {
        error_rescy = std::sqrt(info.exErrorY * info.exErrorY + error_segcy);
    }

    //  pull after taking into account the correlation with the angle YZ

    info.pullCY = info.resCY / std::hypot(error_rescy, m_SAFE_THETA, correction * m_SAFE_THETAANGLE);

    // chi2 with full covariance matrix in Y and YZ plane

    double chi2Y = d * info.resY * info.resY - 2 * b * scale * info.resY * info.dangleYZ + a * info.dangleYZ * info.dangleYZ;
    info.chi2Y = 0.5*( chi2Y / det);
    if (info.chi2Y < 0) ATH_MSG_DEBUG(" NEGATIVE chi2Y " << chi2Y << " dydyz " << dydyz << " determinant " << det);

    bool hasPhi = hitCounts.nexpectedTrigHitLayers > 1;
    
    //
    // Store hasphi
    //
    info.hasPhi = hasPhi;
    info.t0 = 0.;

    // station layer
    info.stationLayer = 0;
    if (stIndex == Muon::MuonStationIndex::BI) info.stationLayer = 1;
    if (stIndex == Muon::MuonStationIndex::BM) info.stationLayer = 2;
    if (stIndex == Muon::MuonStationIndex::BO) info.stationLayer = 3;
    if (stIndex == Muon::MuonStationIndex::BE) info.stationLayer = 4;
    if (stIndex == Muon::MuonStationIndex::EI) info.stationLayer = m_idHelperSvc->isMdt(chId) ? 11 : 21;
    if (stIndex == Muon::MuonStationIndex::EM) info.stationLayer = 12;
    if (stIndex == Muon::MuonStationIndex::EO) info.stationLayer = 13;
    if (stIndex == Muon::MuonStationIndex::EE) info.stationLayer = 14;

    ATH_MSG_DEBUG("stationLayer " << info.stationLayer);
    // number of holes
    info.nholes = 0;
    const Muon::MuonSegmentQuality* q = dynamic_cast<const Muon::MuonSegmentQuality*>(segment.fitQuality());
    if (q) info.nholes = q->numberOfHoles();

    // number of segments
    info.nsegments = 1;

    // single ML
    info.singleML = 1;
    if (hitCounts.nmdtHitsMl1 > 0 && hitCounts.nmdtHitsMl2 > 0) info.singleML = 0;

    //   RLocY parametrized rejection as a function of locY
    //   RAYZ parametrized rejection as a function of Angle YZ

    //   parameters are used for selecting best segment in ambiguity solver

    /// What do these magic numbers represent?
    constexpr double a_locY{13.8269}, b_locY{1.23548}, c_locY{2.73400}, a_AYZ{12.0655}, b_AYZ{1.87578}, c_AYZ{1.88660}, width_locY{20.},
        width_AYZ{0.004};

    double dlocY = info.resY;
    double dAYZ = info.dangleYZ;

    double logLocY = std::log(1 + std::abs(dlocY / width_locY));
    double logAYZ = std::log(1 + std::abs(dAYZ / width_AYZ));

    info.RLocY = a_locY / (1. + b_locY * (logLocY) * (logLocY) + c_locY * (logLocY) * (logLocY) * (logLocY));
    info.RAYZ = a_AYZ / (1. + b_AYZ * (logAYZ) * (logAYZ) + c_AYZ * (logAYZ) * (logAYZ) * (logAYZ));

    int selected = 0;
    scale = 1.;

    if (std::abs(info.pullY) < m_MATCH_THETA && std::abs(info.pullYZ) < m_MATCH_THETAANGLE &&
        std::abs(info.pullCY) < scale * m_combinedPullCut && info.pullChamber < m_chamberPullCut) {
        bool pass = true;
        if (pass) selected = 1;
    }

    info.selected = selected;

    return info;
}
void MuTagMatchingTool::calculateLocalAngleErrors(const Muon::MuonSegment& segment, double& angleXZerror, double& angleYZerror) const {
    const Amg::MatrixX& segGlobCov = segment.localCovariance();
    angleXZerror = 999.;
    angleYZerror = 999.;
    //
    // the segment errors are stored in local angles (m_assumeLocalErrors = true);
    //

    if (m_assumeLocalErrors) {
        if (segGlobCov(Trk::phi, Trk::phi) >= 0 && segGlobCov(Trk::phi, Trk::phi) <= 999.0)
            angleXZerror = std::sqrt(segGlobCov(Trk::phi, Trk::phi));
        if (segGlobCov(Trk::theta, Trk::theta) >= 0 && segGlobCov(Trk::theta, Trk::theta) <= 999.0)
            angleYZerror = std::sqrt(segGlobCov(Trk::theta, Trk::theta));
    }
}

void MuTagMatchingTool::calculateLocalAngleErrors(const Trk::AtaPlane& exTrack, double& angleXZerror, double& angleYZerror,
                                                  double& covLocYYZ) const {
    // Parameters are described as Trk::LocX, Trk::locY, Trk::phi, Trk::theta
    // So the errormatrix of the track 'localErrorMatrix' still holds global angle representation!!!!
    // retrieve Jabcobian to transform the global errors err_phi,err_theta to local errors err_alphaXZ, err_alphaYZ
    const Amg::RotationMatrix3D glob2loc = exTrack.associatedSurface().transform().rotation().inverse();
    const AmgVector(5)& exTrkParms = exTrack.parameters();
    Trk::JacobianPhiThetaLocalAngles jacobianExTrk(exTrkParms[Trk::phi], exTrkParms[Trk::theta], glob2loc);

    // start with global angles error matrix
    Amg::MatrixX anglesCovGlob(2, 2);
    anglesCovGlob(0, 0) = (*exTrack.covariance())(Trk::phi, Trk::phi);
    anglesCovGlob(1, 0) = (*exTrack.covariance())(Trk::theta, Trk::phi);
    anglesCovGlob(0, 1) = (*exTrack.covariance())(Trk::phi, Trk::theta);
    anglesCovGlob(1, 1) = (*exTrack.covariance())(Trk::theta, Trk::theta);

    // NEW METHOD
    const AmgSymMatrix(2)& anglesCovLoc = anglesCovGlob.similarity(jacobianExTrk);

    if (anglesCovLoc(0, 0) >= 0) angleXZerror = std::sqrt(anglesCovLoc(0, 0));
    if (anglesCovLoc(1, 1) >= 0) angleYZerror = std::sqrt(anglesCovLoc(1, 1));

    covLocYYZ = (*exTrack.covariance())(Trk::locY, Trk::phi) * jacobianExTrk(0, 1) +
                (*exTrack.covariance())(Trk::locY, Trk::theta) * jacobianExTrk(1, 1);
    ATH_MSG_DEBUG(" covLocYYZ " << covLocYYZ << " covlocytheta " << (*exTrack.covariance())(Trk::locY, Trk::theta) << " J01 "
                                << jacobianExTrk(0, 1) << " J11 " << jacobianExTrk(0, 1));
    // some printout
    ATH_MSG_DEBUG(std::setw(20) << "Angles Jacobian used for TRACK angle errors below: " << jacobianExTrk);

    ATH_MSG_DEBUG(std::setw(20) << "NEW TRACK angleXZ error = " << std::setprecision(6) << std::setw(10) << angleXZerror << std::setw(20)
                                << " and angleYZ error = " << std::setw(10) << angleYZerror);
}
bool MuTagMatchingTool::matchDistance(const MuonCombined::MuonSegmentInfo& info) const {
    bool pass(true);    
    if (!info.segment || !info.trackAtSegment) {
        ATH_MSG_DEBUG(" No segment and or trackAtSegment pointer matchDistance ");
        return false;
    }
    if (!m_doDistCut) return pass;

    if (info.hasPhi) {
        if (std::abs(info.resX) > m_DIST_POS) pass = false;
        if (std::abs(info.dangleXZ) > m_DIST_ANGLE) pass = false;
    }
    if (info.maximumResidualAlongTube > m_DIST_POS) pass = false;
    if (std::abs(info.resY) > m_DIST_POS) pass = false;
    if (std::abs(info.dangleYZ) > m_DIST_ANGLE) pass = false;

    return pass;
}

bool MuTagMatchingTool::isCscSegment(const Muon::MuonSegment& seg) const {
    for (const Trk::MeasurementBase* seg_meas : seg.containedMeasurements()) {
        const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(seg_meas);
        if (!rot) {
            const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(seg_meas);
            if (crot) rot = &crot->rioOnTrack(0);
        }
        if (!rot) { continue; }
        if (m_idHelperSvc->isCsc(rot->identify())) return true;
    }

    return false;
}

unsigned int MuTagMatchingTool::cscHits(const Muon::MuonSegment& seg) const {
    unsigned int nrHits{0};
    for (const Trk::MeasurementBase* seg_meas : seg.containedMeasurements()) {
        const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(seg_meas);
        if (!rot) {
            const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(seg_meas);
            if (crot) rot = &crot->rioOnTrack(0);
        }
        if (!rot) { continue; }
        if (m_idHelperSvc->isCsc(rot->identify())) ++nrHits;
    }

    return nrHits;
}
