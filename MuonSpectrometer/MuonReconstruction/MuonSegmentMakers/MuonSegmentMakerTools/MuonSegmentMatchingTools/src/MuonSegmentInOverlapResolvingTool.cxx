/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentInOverlapResolvingTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkParameters/TrackParameters.h"

namespace Muon {

MuonSegmentInOverlapResolvingTool::MuonSegmentInOverlapResolvingTool(const std::string& ty, const std::string& na,
                                                                     const IInterface* pa)
    : AthAlgTool(ty, na, pa), m_magFieldProperties(Trk::NoField)
{
    declareInterface<IMuonSegmentInOverlapResolvingTool>(this);
}

StatusCode
MuonSegmentInOverlapResolvingTool::initialize()
{
    ATH_CHECK(AthAlgTool::initialize());

    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_propagator.retrieve());
    ATH_CHECK(m_pullCalculator.retrieve());

    return StatusCode::SUCCESS;
}

Amg::Vector3D
MuonSegmentInOverlapResolvingTool::updateSegmentDirection(const MuonSegment& seg, double phi) const
{

    Amg::Transform3D gToStation = seg.associatedSurface().transform().inverse();
    Amg::Vector3D    phiDir(1., 0., 0.);
    Amg::setThetaPhi(phiDir, seg.globalDirection().theta(), phi);
    Amg::Vector3D lphiDir = gToStation.linear() * phiDir;
    Amg::Vector3D lsegDir = gToStation.linear() * seg.globalDirection();
    double        road_dz = lphiDir.z();
    double        seg_dy  = lsegDir.y();
    double        seg_dz  = lsegDir.z();
    if (road_dz * seg_dz < 0) {
        seg_dy *= -1.;
        seg_dz *= -1.;
    }
    if (fabs(seg_dz) < 1e-6) {
        seg_dz = 1e-6;
        ATH_MSG_DEBUG(" Unexpected local direction of segment " << lsegDir);
    }
    double scale = road_dz / seg_dz;
    seg_dy *= scale;
    Amg::Vector3D locDir(lphiDir.x(), seg_dy, road_dz);
    Amg::Vector3D gDir = seg.associatedSurface().transform().linear() * locDir;
    return gDir;
}

Amg::Vector3D
MuonSegmentInOverlapResolvingTool::estimateSegmentDirection(const MuonSegment& seg1, const MuonSegment& seg2,
                                                            double& phi, double& stereoangle) const
{
    //
    //    From two segments the direction and position of the segments are determined
    //                           assuming a straight line
    //
    //    if a stereoangle is present one can determine: phi and x local for both segments
    //

    //     bool check = false;

    //           Confusing  use of objects:
    //  This is needed because otherwise the Transform3D DOES NOT know
    //             whether it transforms a position or a direction
    //
    const Amg::Transform3D& gToGlobal1 = seg1.associatedSurface().transform();
    const Amg::Transform3D& gToGlobal2 = seg2.associatedSurface().transform();
    Amg::Transform3D        gToLocal1  = seg1.associatedSurface().transform().inverse();
    Amg::Transform3D        gToLocal2  = seg2.associatedSurface().transform().inverse();
    //
    const Amg::Vector3D& gDir1 = seg1.globalDirection();
    const Amg::Vector3D& gDir2 = seg2.globalDirection();
    // Confusing but  Amg::Vector3D can hold a 3D local direction
    Amg::Vector3D lDir1 = gToLocal1.linear() * gDir1;
    // Amg::Vector3D lDir2 = gToLocal2.linear()*gDir2;
    // segment 2 local direction in frame of segment 1
    Amg::Vector3D lDir12 = gToLocal1.linear() * gDir2;
    // Amg::Vector3D lDir21 = gToLocal2.linear()*gDir1;

    Amg::Vector3D segLocDiro(1., 0., 0.);
    // orthogonal vector to segment 2 in local frame of segment 1
    Amg::Vector3D lDiro12 = gToLocal1.linear() * (gToGlobal2.linear() * segLocDiro);
    // orthogonal vector to segment 1in local frame of segment 2
    Amg::Vector3D lDiro21 = gToLocal2.linear() * (gToGlobal1.linear() * segLocDiro);

    stereoangle = acos(lDiro12.x());

    //  We have the following equations for segment 2 in local frame of segment 1
    //      dx1  = free
    //      dy1  = lDir1.y()    =   a * lDir12.y() + b * lDiro12.y()
    //      dz1  = lDir1.z()    =   a * lDir12.z() + b * lDiro12.z()

    double b   = lDir1.y() * lDir12.z() - lDir1.z() * lDir12.y();
    double a   = lDir1.y() * lDiro12.z() - lDir1.z() * lDiro12.y();
    double dxn = lDir12.x();
    double dyn = lDir12.y();
    double dzn = lDir12.z();
    if (fabs(a) > 1e-2) {
        dxn = lDir12.x() - b * lDiro12.x() / a;
        dyn = lDir12.y() - b * lDiro12.y() / a;
        dzn = lDir12.z() - b * lDiro12.z() / a;
    }
    double norm = std::sqrt(dxn * dxn + dyn * dyn + dzn * dzn);
    if (norm < 1e-6) {
        ATH_MSG_DEBUG(" Unexpected normalisation " << norm);
        norm = 1e-6;
    }
    if (dxn * lDir1.x() + dyn * lDir1.y() + dzn * lDir1.z() < 0) norm = -norm;
    dxn = dxn / norm;
    dyn = dyn / norm;
    dzn = dzn / norm;

    // The final result for the direction of the two segments

    // New Local direction of segment 1
    Amg::Vector3D lDirn1(dxn, dyn, dzn);
    // Global direction of both segments
    Amg::Vector3D gDirn = gToGlobal1.linear() * lDirn1;
    // New Local direction of segment 2
    Amg::Vector3D lDirn2 = gToLocal2.linear() * gDirn;
    phi                  = gDirn.phi();
    double theta         = gDirn.theta();

    const Amg::Vector3D& gPos1 = seg1.globalPosition();
    // Confusing but  Amg::Vector3D can hold a 3D local position
    Amg::Vector3D lPos1  = gToLocal1 * gPos1;
    Amg::Vector3D lPos21 = gToLocal2 * gPos1;
    Amg::Vector3D gPos2  = seg2.globalPosition();
    Amg::Vector3D lPos12 = gToLocal1 * gPos2;
    Amg::Vector3D lPos2  = gToLocal2 * gPos2;
    // In local frame of segment 2 shift segment 1 to obtain zero residual
    double res21   = (lPos2.y() - lPos21.y()) * lDirn2.z() - (lPos2.z() - lPos21.z()) * lDirn2.y();
    double localx1 = 0.;
    double step    = (lDiro21.y() * lDirn2.z() - lDiro21.z() * lDirn2.y());
    if (fabs(step) > 1e-5) {
        localx1 = res21 / step;
    }
    ATH_MSG_DEBUG(" localx1 " << localx1 << " res21 " << res21 << " step " << step);

    // Result: New local and global position of segment 1
    Amg::Vector3D lPosn1(lPos1.x() + localx1, lPos1.y(), lPos1.z());
    Amg::Vector3D gPosn1 = gToGlobal1 * lPosn1;
    // In local frame of segment 1 shift segment 2 to obtain zero residual
    double res12   = (lPos1.y() - lPos12.y()) * lDirn1.z() - (lPos1.z() - lPos12.z()) * lDirn1.y();
    step           = (lDiro12.y() * lDirn1.z() - lDiro12.z() * lDirn1.y());
    double localx2 = 0.;
    if (fabs(step) > 1e-5) {
        localx2 = res12 / step;
    }
    ATH_MSG_DEBUG(" localx2 " << localx2 << " res12 " << res12 << " step " << step);

    // Result: New local and global position of segment 2
    Amg::Vector3D lPosn2(lPos2.x() + localx2, lPos2.y(), lPos2.z());
    Amg::Vector3D gPosn2 = gToGlobal2 * lPosn2;
    ATH_MSG_DEBUG(" segment 1 local position "
                  << lPos1 << " new " << lPosn1 << std::endl
                  << " segment 1 global position " << gPos1 << " new " << gPosn1 << std::endl
                  << " segment 2 local position " << lPos2 << " new " << lPosn2 << std::endl
                  << " segment 2 global position " << gPos2 << " new " << gPosn2);

    // This gives the direction from the position of the segments
    Amg::Vector3D gDirPos = gPosn2 - gPosn1;
    if (gDir1.x() * gDirPos.x() + gDir1.y() * gDirPos.y() + gDir1.z() * gDirPos.z() < 0) {
        gDirPos = -gPosn2 + gPosn1;
    }
    double dtheta = theta - gDirPos.theta();
    double dphi   = phi - gDirPos.phi();
    ATH_MSG_DEBUG(" theta " << theta << " gDirPos theta " << gDirPos.theta() << " dtheta " << dtheta << " phi " << phi
                            << " gDirPos phi " << gDirPos.phi() << " dphi " << dphi);
    return gDirn;
}


MuonSegmentInOverlapResolvingTool::SegmentPhiMatchResult
MuonSegmentInOverlapResolvingTool::bestPhiMatchAnalytic(const MuonSegment& seg1, const MuonSegment& seg2) const
{

    //
    //    From two segments the direction and position of the segments are determined
    //                           assuming a straight line
    //
    //    if a stereoangle is present one can determine: phi and x local for both segments
    //

    // bool check = false;

    //           Confusing  use of objects:
    //  Trk::GlobalDirection can and will hold a 3D LOCAL direction
    //  Trk::GlobalPositionn can and will hold a 3D LOCAL position
    //  This is needed because otherwise the Amg::Transform3D DOES NOT know
    //             whether it transforms a position or a direction
    //
    const Amg::Transform3D& gToGlobal1 = seg1.associatedSurface().transform();
    const Amg::Transform3D& gToGlobal2 = seg2.associatedSurface().transform();
    Amg::Transform3D        gToLocal1  = seg1.associatedSurface().transform().inverse();
    // Amg::Transform3D gToLocal2 = seg2.associatedSurface().transform().inverse();
    //
    const Amg::Vector3D& gDir1 = seg1.globalDirection();
    const Amg::Vector3D& gDir2 = seg2.globalDirection();
    // Confusing but  Amg::Vector3D can hold a 3D local direction
    Amg::Vector3D lDir1 = gToLocal1.linear() * gDir1;
    // Amg::Vector3D lDir2 = gToLocal2.linear()*gDir2;
    // segment 2 local direction in frame of segment 1
    Amg::Vector3D lDir12 = gToLocal1.linear() * gDir2;
    // Amg::Vector3D lDir21 = gToLocal2.linear()*gDir1;

    Amg::Vector3D segLocDiro(1., 0., 0.);
    // orthogonal vector to segment 2 in local frame of segment 1
    Amg::Vector3D lDiro12 = gToLocal1.linear() * (gToGlobal2.linear() * segLocDiro);
    // orthogonal vector to segment 1in local frame of segment 2
    // Amg::Vector3D lDiro21 = gToLocal2.linear()*(gToGlobal1.linear()*segLocDiro);

    // stereoangle = acos(lDiro12.x());

    //  We have the following equations for segment 2 in local frame of segment 1
    //      dx1  = free
    //      dy1  = lDir1.y()    =   a * lDir12.y() + b * lDiro12.y()
    //      dz1  = lDir1.z()    =   a * lDir12.z() + b * lDiro12.z()

    double b   = lDir1.y() * lDir12.z() - lDir1.z() * lDir12.y();
    double a   = lDir1.y() * lDiro12.z() - lDir1.z() * lDiro12.y();
    double dxn = lDir12.x();
    double dyn = lDir12.y();
    double dzn = lDir12.z();
    if (fabs(a) > 1e-2) {
        dxn = lDir12.x() - b * lDiro12.x() / a;
        dyn = lDir12.y() - b * lDiro12.y() / a;
        dzn = lDir12.z() - b * lDiro12.z() / a;
    }
    double norm = std::sqrt(dxn * dxn + dyn * dyn + dzn * dzn);
    if (norm < 1e-6) {
        ATH_MSG_DEBUG(" Unexpected normalisation " << norm);
        norm = 1e-6;
    }
    if (dxn * lDir1.x() + dyn * lDir1.y() + dzn * lDir1.z() < 0) norm = -norm;
    dxn = dxn / norm;
    dyn = dyn / norm;
    dzn = dzn / norm;

    // The final result for the direction of the two segments

    // New Local direction of segment 1
    Amg::Vector3D lDirn1(dxn, dyn, dzn);
    // Global direction of both segments
    Amg::Vector3D       segDir1Min = gToGlobal1.linear() * lDirn1;
    Amg::Vector3D       segDir2Min = segDir1Min;  // updateSegmentDirection(seg2,segDir1Min.phi());
    Trk::LocalDirection segLocDir1;
    seg2.associatedSurface().globalToLocalDirection(segDir1Min, segLocDir1);
    Trk::LocalDirection segLocDir2;
    seg2.associatedSurface().globalToLocalDirection(segDir2Min, segLocDir2);
    double dyz = fabs(segLocDir1.angleYZ() - segLocDir2.angleYZ());
    return SegmentPhiMatchResult(segDir1Min, segDir2Min, dyz);
}

MuonSegmentInOverlapResolvingTool::SegmentPhiMatchResult
MuonSegmentInOverlapResolvingTool::bestPhiMatch(const MuonSegment& seg1, const MuonSegment& seg2) const
{

    unsigned int nbins = 11;
    // unsigned int bestBin = 0;
    double scanRange = 1.;
    double scanStep  = scanRange / (nbins - 1);
    double phiStart  = seg1.globalDirection().phi() - 0.5 * scanRange;

    double              dthetaMin  = 1e9;
    Amg::Vector3D       segDir1Min = seg1.globalDirection();
    Trk::LocalDirection segLocDir2;
    seg2.associatedSurface().globalToLocalDirection(seg2.globalDirection(), segLocDir2);
    for (unsigned int i = 0; i < nbins; ++i) {
        double              phi     = phiStart + scanStep * i;
        Amg::Vector3D       segDir1 = updateSegmentDirection(seg1, phi);
        Trk::LocalDirection segLocDir12;
        seg2.associatedSurface().globalToLocalDirection(segDir1, segLocDir12);
        double dyz = fabs(segLocDir12.angleYZ() - segLocDir2.angleYZ());
        if (dyz < dthetaMin) {
            dthetaMin  = dyz;
            segDir1Min = segDir1;
            // bestBin = i;
        }
    }
    return SegmentPhiMatchResult(segDir1Min, updateSegmentDirection(seg2, segDir1Min.phi()), dthetaMin);
}


MuonSegmentInOverlapResolvingTool::SegmentPositionMatchResult
MuonSegmentInOverlapResolvingTool::bestPositionAlongTubeMatch(const MuonSegment& seg1, const MuonSegment& seg2,
                                                              const Amg::Vector3D& segDir1Min) const
{
    bool goodMatch = true;

    // get geomtry summary for segment
    SegmentGeometrySummary segmentGeometry  = segmentGeometrySummary(seg1);
    SegmentGeometrySummary segmentGeometry2 = segmentGeometrySummary(seg2);

    int    nbins    = 2;
    double tubeStep = segmentGeometry.hvPosInSegFrame - segmentGeometry.roPosInSegFrame;

    double posStep = tubeStep / (nbins - 1);

    double resfirst = 1e9;
    double reslast  = 1e9;
    double posfirst = 1e9;
    double poslast  = 1e9;
    double locy     = seg1.localParameters().contains(Trk::locY) ? seg1.localParameters()[Trk::locY] : 0.;

    for (int j = 0; j < nbins; ++j) {
        double distToRO = segmentGeometry.roPosInSegFrame + posStep * j;

        // create local position
        Amg::Vector2D lpos(distToRO, locy);

        // segment reference surface
        const Trk::PlaneSurface& segmentSurface = seg2.associatedSurface();

        // get global position
        Amg::Vector3D gpos;
        seg1.associatedSurface().localToGlobal(lpos, segDir1Min, gpos);

        // calculate intersection
        Trk::Intersection intersect = segmentSurface.straightLineIntersection(gpos, segDir1Min, false, false);
        if (!intersect.valid || !segmentSurface.globalToLocal(intersect.position, segDir1Min, lpos)) {
            //      if( !intersect.valid || !segmentSurface.globalToLocal(intersect.intersection,segDir1Min,lpos) ){
            ATH_MSG_WARNING(" Intersect with surface  position " << Amg::toString(gpos) << " direction: phi "
                                                                 << segDir1Min.phi() << " theta "
                                                                 << segDir1Min.theta());
        } else {

            // now we have the prediction of the track in the segment frame, lets look at residuals
            double resy = lpos[Trk::locY] - seg2.localParameters()[Trk::locY];

            if (j == 0) {
                resfirst = resy;
                posfirst = distToRO;
            }
            if (j == nbins - 1) {
                reslast = resy;
                poslast = distToRO;
            }
        }
    }

    double        distPosMin2    = 1e9;
    double        distPosInTube2 = 1e9;
    double        distPosMin     = 1e9;
    double        distPosInTube  = 1e9;
    double        resyMin        = 1e9;
    Amg::Vector3D segPos(0., 0., 0.);
    double        rangeCut = 1e5;
    if (resfirst < rangeCut && reslast < rangeCut && posfirst < rangeCut && poslast < rangeCut) {
        double resDif = reslast - resfirst;
        double posDif = poslast - posfirst;
        if (fabs(resDif) < 1e-6) {
            ATH_MSG_DEBUG("  Unexpected residual difference " << resDif);
            resDif = resDif < 0. ? -1e-6 : 1e-6;
        }
        if (fabs(posDif) < 1e-6) {
            ATH_MSG_DEBUG("  Unexpected position difference " << posDif);
            posDif = posDif < 0. ? -1e-6 : 1e-6;
        }
        distPosMin    = posfirst - resfirst / (resDif) * (posDif);
        distPosInTube = segmentGeometry.positionInsideTube(distPosMin);
        resyMin       = resfirst + (resDif) * (distPosInTube - posfirst) / (posDif);

        double locx = distPosInTube;
        double locy = seg1.localParameters().contains(Trk::locY) ? seg1.localParameters()[Trk::locY] : 0.;

        // create local position
        Amg::Vector2D lpos(locx, locy);

        // segment reference surface
        const Trk::PlaneSurface& segmentSurface = seg2.associatedSurface();

        // get global position
        Amg::Vector3D gpos;
        seg1.associatedSurface().localToGlobal(lpos, segDir1Min, gpos);
        segPos = gpos;

        // calculate intersection
        Trk::Intersection intersect = segmentSurface.straightLineIntersection(gpos, segDir1Min, false, false);
        if (!intersect.valid) {
            ATH_MSG_WARNING(" Intersect with surface  position " << Amg::toString(gpos) << " direction: phi "
                                                                 << segDir1Min.phi() << " theta "
                                                                 << segDir1Min.theta());
            goodMatch = false;
        } else {
            Amg::Vector3D locExSeg2 = segmentGeometry2.globalToSeg * intersect.position;
            distPosMin2             = locExSeg2.x();
            distPosInTube2          = segmentGeometry2.positionInsideTube(distPosMin2);
        }

    } else {
        goodMatch = false;
    }

    return SegmentPositionMatchResult(distPosMin, distPosInTube, segmentGeometry.shortestChannelLength, distPosMin2,
                                      distPosInTube2, segmentGeometry2.shortestChannelLength, resyMin, goodMatch,
                                      segPos);
}


MuonSegmentInOverlapResolvingTool::SegmentGeometrySummary
MuonSegmentInOverlapResolvingTool::segmentGeometrySummary(const MuonSegment& seg) const
{

    SegmentGeometrySummary summary;
    // loop over hits
    Identifier                       tubeId1;
    const MuonGM::MdtReadoutElement* detEl           = 0;
    double                           shortestTubeLen = 1e9;
    Amg::Vector3D                    roPos;
    Amg::Vector3D                    tubeCenter;
    MeasCit                          sit     = seg.containedMeasurements().begin();
    MeasCit                          sit_end = seg.containedMeasurements().end();
    bool                             hasMdt  = false;
    for (; sit != sit_end; ++sit) {

        const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*sit);
        if (mdt) {
            hasMdt                    = true;
            const Identifier& id      = mdt->identify();
            int               layer   = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
            int               tube    = m_idHelperSvc->mdtIdHelper().tube(id);
            double            tubelen = mdt->prepRawData()->detectorElement()->getActiveTubeLength(layer, tube);
            if (tubelen < shortestTubeLen) {
                shortestTubeLen = tubelen;
                roPos           = mdt->prepRawData()->detectorElement()->ROPos(id);
                tubeCenter      = mdt->prepRawData()->detectorElement()->surface(id).center();
                detEl           = mdt->prepRawData()->detectorElement();
                tubeId1         = id;
            }
        }
    }
    summary.detEl                 = detEl;
    summary.hasMdt                = hasMdt;
    summary.segToGlobal           = seg.associatedSurface().transform();
    summary.globalToSeg           = summary.segToGlobal.inverse();
    summary.roPosInSegFrame       = (summary.globalToSeg * roPos).x();
    double distTubeCenterFromRO   = (summary.globalToSeg * tubeCenter).x() - summary.roPosInSegFrame;
    summary.hvPosInSegFrame       = summary.roPosInSegFrame + 2 * distTubeCenterFromRO;
    summary.shortestChannelLength = fabs(2 * distTubeCenterFromRO);

    return summary;
}

MuonSegmentInOverlapResolvingTool::SegmentMatchResult
MuonSegmentInOverlapResolvingTool::matchResult(const MuonSegment& seg1, const MuonSegment& seg2) const
{

    ATH_MSG_DEBUG(" First segment  " << m_printer->print(seg1) << std::endl
                                     << " Second segment " << m_printer->print(seg2));
    SegmentMatchResult result = SegmentMatchResult();

    // calculate the phi angle that matches the two local segment angles
    result.phiResult = bestPhiMatchAnalytic(seg1, seg2);

    // calculate the position along the tube that minimizes the position residual for both segments
    result.segmentResult1 = bestPositionAlongTubeMatch(seg1, seg2, result.phiResult.segmentDirection1);
    result.segmentResult2 = bestPositionAlongTubeMatch(seg2, seg1, result.phiResult.segmentDirection2);

    // calculate the average pull of the phi hits on the segments with the new parameters
    result.averagePhiHitPullSegment1 = checkPhiHitConsistency(seg1, result.phiResult, result.segmentResult1);
    result.averagePhiHitPullSegment2 = checkPhiHitConsistency(seg2, result.phiResult, result.segmentResult2);

    if (result.segmentResult1.goodMatch && result.segmentResult2.goodMatch) {

        // calculate vector connecting the two new segment positions
        Amg::Vector3D difPos = result.segmentResult2.segmentPosition - result.segmentResult1.segmentPosition;

        // check if vectors are pointing in the same direction, else flip
        if (result.phiResult.segmentDirection1.y() * difPos.y() < 0.) {
            difPos *= -1.;
        }

        // calculate difference in angle between phi from phi match and the two new segment positions
        double norm = difPos.perp() * result.phiResult.segmentDirection1.perp();
        double cosdphi =
            (result.phiResult.segmentDirection1.x() * difPos.x() + result.phiResult.segmentDirection1.y() * difPos.y())
            / norm;
        if (cosdphi > 0.999999)
            cosdphi = 0.999999;
        else if (cosdphi < -0.999999)
            cosdphi = -0.999999;
        result.angularDifferencePhi = std::acos(cosdphi);
    }
    return result;
}


double
MuonSegmentInOverlapResolvingTool::checkPhiHitConsistency(const Muon::MuonSegment&    segment,
                                                          SegmentPhiMatchResult&      phiMatchResult,
                                                          SegmentPositionMatchResult& posMatchResult) const
{

    // calculate average pull of phi measurements
    unsigned int nphiMeas    = 0;
    double       averagePull = 0.;

    // calculate segment AtaPlane
    // SegmentGeometrySummary geometry = segmentGeometrySummary( segment );
    double        locx = posMatchResult.positionInTube1;
    double        locy = segment.localParameters().contains(Trk::locY) ? segment.localParameters()[Trk::locY] : 0.;
    Trk::AtaPlane segPars(locx, locy, phiMatchResult.segmentDirection1.phi(), phiMatchResult.segmentDirection1.theta(),
                          0., segment.associatedSurface());

    // loop over hits and calculate residuals for phi hits
    MeasCit mit     = segment.containedMeasurements().begin();
    MeasCit mit_end = segment.containedMeasurements().end();
    for (; mit != mit_end; ++mit) {

        Identifier id = m_edmHelperSvc->getIdentifier(**mit);
        if (!id.is_valid() || !m_idHelperSvc->measuresPhi(id)) continue;

        // predict onto segment surface
        const Trk::Surface& measSurf = (**mit).associatedSurface();

        // propagate station parameters to segment
        const Trk::TrackParameters* exPars =
            m_propagator->propagate(segPars, measSurf, Trk::anyDirection, false, m_magFieldProperties);
        if (!exPars) {
            ATH_MSG_WARNING("  Failed to propagate parameter to segment surface" << std::endl
                                                                                 << " pars "
                                                                                 << m_printer->print(segPars));
        } else {

            const Trk::ResidualPull* resPull =
                m_pullCalculator->residualPull(*mit, exPars, Trk::ResidualPull::Unbiased);
            if (!resPull) {
                ATH_MSG_DEBUG(" calculation of residual/pull failed !!!!! ");
                delete exPars;
                continue;
            }

            // sanity check
            if (resPull->pull().size() != 1) {
                ATH_MSG_WARNING(" ResidualPull with empty pull vector for channel " << m_idHelperSvc->toString(id));
                delete resPull;
                delete exPars;
                continue;
            }

            double pull = resPull->pull().front();

            averagePull += pull;
            ++nphiMeas;
            delete resPull;
            delete exPars;
        }
    }
    if (nphiMeas != 0) averagePull /= nphiMeas;
    return averagePull;
}

}  // namespace Muon
