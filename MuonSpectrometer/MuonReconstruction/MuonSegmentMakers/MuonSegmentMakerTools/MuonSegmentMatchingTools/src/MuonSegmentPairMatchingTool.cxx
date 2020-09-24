/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentPairMatchingTool.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/LocalDirection.h"

namespace {
// limit angle difference to -pi < x <= pi
inline double
limit_angle_2pi(double x)
{
    while (x <= -M_PI) x += 2.0 * M_PI;
    while (x > +M_PI) x -= 2.0 * M_PI;
    return x;
}

inline double
robust_acos(double x)
{
    if (x >= +1.0) return 0.0;
    if (x <= -1.0) return M_PI;
    return std::acos(x);
}

}  // unnamed namespace


namespace Muon {

MuonSegmentPairMatchingTool::MuonSegmentPairMatchingTool(const std::string& ty, const std::string& na,
                                                         const IInterface* pa)
    : AthAlgTool(ty, na, pa)
{
    declareInterface<IMuonSegmentPairMatchingTool>(this);
}

StatusCode
MuonSegmentPairMatchingTool::initialize()
{
    ATH_CHECK(AthAlgTool::initialize());

    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());

    return StatusCode::SUCCESS;
}

IMuonSegmentPairMatchingTool::SegmentMatchResult
MuonSegmentPairMatchingTool::matchResult(const MuonSegment& seg1, const MuonSegment& seg2) const
{
    IMuonSegmentPairMatchingTool::SegmentMatchResult result;

    // get identifiers
    // and the detector region from identifier
    Identifier                chid1    = m_edmHelperSvc->chamberId(seg1);
    MuonStationIndex::StIndex station1 = m_idHelperSvc->stationIndex(chid1);

    Identifier                chid2    = m_edmHelperSvc->chamberId(seg2);
    MuonStationIndex::StIndex station2 = m_idHelperSvc->stationIndex(chid2);

    // Don't deal with overlap/merge of segments here
    if (chid1 == chid2) return result;
    if (station1 == station2) return result;

    // Don't know how to deal with these cases yet...
    if (station1 == MuonStationIndex::StUnknown) return result;
    if (station2 == MuonStationIndex::StUnknown) return result;
    if (station1 >= MuonStationIndex::StIndexMax) return result;
    if (station2 >= MuonStationIndex::StIndexMax) return result;

    // Here order matters, so sort segment by order a= inner, b= outer
    // MuonStationIndex::StIndex station_a = station1;
    // MuonStationIndex::StIndex station_b = station2;

    const MuonSegment* pSeg_a = 0;
    const MuonSegment* pSeg_b = 0;

    // bool swapped = false;
    if (station1 < station2) {
        pSeg_a = &seg1;
        pSeg_b = &seg2;

    } else {
        // swapped = true;
        pSeg_a = &seg2;
        pSeg_b = &seg1;
        // station_a = station2;
        // station_b = station1;
    }

    const MuonSegment& seg_a(*pSeg_a);
    const MuonSegment& seg_b(*pSeg_b);

    Identifier chid_a = m_edmHelperSvc->chamberId(seg_a);
    Identifier chid_b = m_edmHelperSvc->chamberId(seg_b);

    int phiSector_a = m_idHelperSvc->sector(chid_a);
    int phiSector_b = m_idHelperSvc->sector(chid_b);

    result.chid_a = chid_a;
    result.chid_b = chid_b;

    result.phiSector_a = phiSector_a;
    result.phiSector_b = phiSector_b;

    const Amg::Vector3D& dir_a = seg_a.globalDirection();
    const Amg::Vector3D& dir_b = seg_b.globalDirection();
    const Amg::Vector3D& pos_a = seg_a.globalPosition();
    const Amg::Vector3D& pos_b = seg_b.globalPosition();

    // Define direction between segment position
    Amg::Vector3D vecAB = pos_b - pos_a;

    // compute angle between vector position a and b using dot product
    double sizeA   = dir_a.mag();
    double sizeB   = dir_b.mag();
    double cosAB   = dir_a.dot(dir_b) / (sizeA * sizeB);
    double angleAB = robust_acos(cosAB);

    // compute angle between vector position a and b using dot product
    double sizeC   = vecAB.mag();
    double cosAC   = dir_a.dot(vecAB) / (sizeA * sizeC);
    double angleAC = robust_acos(cosAC);

    // compute angle between vector position a and b using dot product
    double cosBC   = dir_b.dot(vecAB) / (sizeB * sizeC);
    double angleBC = robust_acos(cosBC);


    if (angleAB > M_PI / 2.) angleAB = fabs(angleAB - M_PI);
    if (angleAC > M_PI / 2.) angleAC = fabs(angleAC - M_PI);
    if (angleBC > M_PI / 2.) angleBC = fabs(angleBC - M_PI);

    result.angleAB = angleAB;
    result.angleAC = angleAC;
    result.angleBC = angleBC;

    // Compute local angles
    // for some reason, vec12 gets the direction right far more often than vecAB
    double              dirTheta_a = seg_a.localDirection().angleYZ();
    double              dirTheta_b = seg_b.localDirection().angleYZ();
    Trk::LocalDirection dirPred_a;
    seg_a.associatedSurface().globalToLocalDirection(vecAB, dirPred_a);
    Trk::LocalDirection dirPred_b;
    seg_b.associatedSurface().globalToLocalDirection(vecAB, dirPred_b);
    double deltaTheta_a = limit_angle_2pi(dirTheta_a - dirPred_a.angleYZ());
    double deltaTheta_b = limit_angle_2pi(dirTheta_b - dirPred_b.angleYZ());

    // bool opposite = false;
    // in case both segment directions are opposite to vecAB, try opposite direction
    if (std::abs(deltaTheta_a) > 0.5 * M_PI && std::abs(deltaTheta_b) > 0.5 * M_PI) {
        // opposite = true;
        seg_a.associatedSurface().globalToLocalDirection(-vecAB, dirPred_a);
        seg_b.associatedSurface().globalToLocalDirection(-vecAB, dirPred_b);
        deltaTheta_a = limit_angle_2pi(dirTheta_a - dirPred_a.angleYZ());
        deltaTheta_b = limit_angle_2pi(dirTheta_b - dirPred_b.angleYZ());
    }


    // look at the relative orientation of the local X axes to determine if YZ angles are flipped
    bool                    flipped = false;
    const Amg::Transform3D& trf_a   = seg_a.associatedSurface().transform();
    const Amg::Transform3D& trf_b   = seg_b.associatedSurface().transform();
    double xAxisDotProduct          = trf_a(0, 0) * trf_b(0, 0) + trf_a(1, 0) * trf_b(1, 0) + trf_a(2, 0) * trf_b(2, 0);
    if (xAxisDotProduct < 0.0) flipped = true;

    double deltaTheta = 0.0;
    if (flipped) {
        deltaTheta = std::abs(limit_angle_2pi(deltaTheta_a - deltaTheta_b));
    } else {
        deltaTheta = std::abs(limit_angle_2pi(deltaTheta_a + deltaTheta_b));
    }

    result.deltaTheta_a = deltaTheta_a;
    result.deltaTheta_b = deltaTheta_b;
    result.deltaTheta   = deltaTheta;

    // compute angular differences in second coordinate
    result.deltaPhipos = fabs(seg_a.globalPosition().phi() - seg_b.globalPosition().phi());
    result.deltaPhidir = fabs(seg_a.globalDirection().phi() - seg_b.globalDirection().phi());

    // check presence of phi hits on segment a
    bool                                                     ContainPhiHits = false;
    std::vector<const Trk::MeasurementBase*>::const_iterator hit            = seg_a.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator hit_end        = seg_a.containedMeasurements().end();
    for (; hit != hit_end; ++hit) {
        Identifier              id;
        const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*hit);
        if (rot)
            id = rot->identify();
        else {
            const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*hit);
            if (crot) id = crot->containedROTs().front()->identify();
        }
        if (!id.is_valid()) continue;
        if (m_idHelperSvc->measuresPhi(id)) {
            ContainPhiHits = true;
            break;
        }
    }

    if (ContainPhiHits) {
        result.phiposerr_a = seg_a.localCovariance()(Trk::locX, Trk::locX);
        result.phidirerr_a = seg_a.localCovariance()(Trk::phi, Trk::phi);
    } else {
        result.phiposerr_a = 99999.;
        result.phidirerr_a = 99999.;
    }

    // check presence of phi hits on segment b
    ContainPhiHits = false;
    hit            = seg_b.containedMeasurements().begin();
    hit_end        = seg_b.containedMeasurements().end();
    for (; hit != hit_end; ++hit) {
        Identifier              id;
        const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*hit);
        if (rot)
            id = rot->identify();
        else {
            const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*hit);
            if (crot) id = crot->containedROTs().front()->identify();
        }
        if (!id.is_valid()) continue;
        if (m_idHelperSvc->measuresPhi(id)) {
            ContainPhiHits = true;
            break;
        }
    }

    if (ContainPhiHits) {
        result.phiposerr_b = seg_b.localCovariance()(Trk::locX, Trk::locX);
        result.phidirerr_b = seg_b.localCovariance()(Trk::phi, Trk::phi);
    } else {
        result.phiposerr_b = 99999.;
        result.phidirerr_b = 99999.;
    }

    result.shorttube_a = 99999.;
    if (m_idHelperSvc->isMdt(chid_a)) {
        // make shortest tube calculations
        std::pair<Amg::Vector3D, Amg::Vector3D> stseg1         = getShortestTubePos(seg_a);
        double                                  shorttube_lx_a = stseg1.first.x();
        double                                  shorttube_ly_a = stseg1.first.y();
        double                                  shorttube_rx_a = stseg1.second.x();
        double                                  shorttube_ry_a = stseg1.second.y();

        double dist_x_la = seg_a.globalPosition().x() - shorttube_lx_a;
        double dist_x_ra = seg_a.globalPosition().x() - shorttube_rx_a;

        double dist_y_la = seg_a.globalPosition().y() - shorttube_ly_a;
        double dist_y_ra = seg_a.globalPosition().y() - shorttube_ry_a;

        double dist_la = sqrt(dist_x_la * dist_x_la + dist_y_la * dist_y_la);
        double dist_ra = sqrt(dist_x_ra * dist_x_ra + dist_y_ra * dist_y_ra);

        if (dist_la >= dist_ra)
            result.shorttube_a = dist_ra;
        else
            result.shorttube_a = dist_la;
    }

    result.shorttube_b = 99999.;
    if (m_idHelperSvc->isMdt(chid_b)) {
        std::pair<Amg::Vector3D, Amg::Vector3D> stseg2         = getShortestTubePos(seg_b);
        double                                  shorttube_lx_b = stseg2.first.x();
        double                                  shorttube_ly_b = stseg2.first.y();
        double                                  shorttube_rx_b = stseg2.second.x();
        double                                  shorttube_ry_b = stseg2.second.y();

        double dist_x_lb = seg_b.globalPosition().x() - shorttube_lx_b;
        double dist_x_rb = seg_b.globalPosition().x() - shorttube_rx_b;

        double dist_y_lb = seg_b.globalPosition().y() - shorttube_ly_b;
        double dist_y_rb = seg_b.globalPosition().y() - shorttube_ry_b;

        double dist_lb = sqrt(dist_x_lb * dist_x_lb + dist_y_lb * dist_y_lb);
        double dist_rb = sqrt(dist_x_rb * dist_x_rb + dist_y_rb * dist_y_rb);

        if (dist_lb >= dist_rb)
            result.shorttube_b = dist_rb;
        else
            result.shorttube_b = dist_lb;
    }
    result.matchFlag = true;

    return result;
}

std::pair<Amg::Vector3D, Amg::Vector3D>
MuonSegmentPairMatchingTool::getShortestTubePos(const Muon::MuonSegment& seg) const
{
    const Muon::MdtDriftCircleOnTrack* shortestMdt  = 0;
    double                             storedLength = 999999;

    // loop over hits, cast each hit to drift circle

    const std::vector<const Trk::MeasurementBase*>&          rioVec = seg.containedMeasurements();
    std::vector<const Trk::MeasurementBase*>::const_iterator it     = rioVec.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator it_end = rioVec.end();

    for (; it != it_end; ++it) {
        const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(*it);
        if (!mdt) continue;
        const MuonGM::MdtReadoutElement* roe = mdt->detectorElement();
        if (!roe) continue;

        // sanity check with getActiveTubeLength
        int    layer      = m_idHelperSvc->mdtIdHelper().tubeLayer(mdt->identify());
        int    tube       = m_idHelperSvc->mdtIdHelper().tube(mdt->identify());
        double halfLength = 0.5 * roe->getActiveTubeLength(layer, tube);

        if (2 * halfLength > storedLength) continue;

        storedLength = halfLength;
        shortestMdt  = mdt;
    }
    std::pair<Amg::Vector3D, Amg::Vector3D> shortpos;
    Amg::Vector2D                           lp(0, storedLength);
    if (shortestMdt) shortestMdt->associatedSurface().localToGlobal(lp, Amg::Vector3D::UnitZ(), shortpos.first);
    lp[1] = -storedLength;
    if (shortestMdt) shortestMdt->associatedSurface().localToGlobal(lp, Amg::Vector3D::UnitZ(), shortpos.second);
    return shortpos;
}
}  // namespace Muon
