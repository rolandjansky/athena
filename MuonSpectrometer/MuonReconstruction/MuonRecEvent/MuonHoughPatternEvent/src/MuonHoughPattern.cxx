/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughPattern.h"

#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/sincos.h"
#include "GaudiKernel/MsgStream.h"

MuonHoughPattern::MuonHoughPattern(int id_number, bool ownhits) : MuonHoughHitContainer(ownhits), m_id_number(id_number) {}

void MuonHoughPattern::resetTracksegment() {
    for (unsigned int i = 0; i < m_hit.size(); i++) {
        delete m_hit[i];
        m_hit[i] = nullptr;
    }

    m_hit.clear();
}

bool MuonHoughPattern::hitInHoughPattern(MuonHoughHit* hit) const  // adviced not to use, slow function
{
    for (unsigned int i = 0; i < size(); i++) {
        if (m_hit[i]->getId() == hit->getId()) { return true; }
    }

    return false;
}

double MuonHoughPattern::patternLength() const {
    // takes the first 2 hits and calculates distance and then takes next hit, and calculates from previous 2 hits which 2 are farthest
    // away, etc.. also possible to calculate point closest to IP and determine if left/ right to pattern, app. just as fast.

    double max_patternlength = 0;

    if (m_hit.size() >= 2) {
        double pattern_length1 = 0, pattern_length2 = 0;
        int hitno1 = 0;
        int hitno2 = 1;

        Amg::Vector3D diff = m_hit[hitno1]->getPosition() - m_hit[hitno2]->getPosition();
        max_patternlength = diff.mag();

        for (unsigned int i = 2; i < m_hit.size(); i++) {
            diff = m_hit[hitno1]->getPosition() - m_hit[i]->getPosition();
            pattern_length1 = diff.mag();

            diff = m_hit[hitno2]->getPosition() - m_hit[i]->getPosition();
            pattern_length2 = diff.mag();

            if (pattern_length1 <= max_patternlength && pattern_length2 <= max_patternlength) {
                // nothing happens..
            } else if (pattern_length1 > max_patternlength && pattern_length1 >= pattern_length2) {
                hitno2 = i;
                max_patternlength = pattern_length1;
            } else if (pattern_length2 > max_patternlength && pattern_length2 > pattern_length1) {
                hitno1 = i;
                max_patternlength = pattern_length2;
            }
        }
    } else {
        MsgStream log(Athena::getMessageSvc(), "MuonHoughPattern::patternLength");
        if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "MuonHoughPattern::pattern_size <2" << endmsg;
    }

    return max_patternlength;
}

double MuonHoughPattern::calculateEZ() const {
    double ez = 0;
    double count = 0;
    //  double r0 = m_erphi;

    for (unsigned int hitno = 0; hitno < m_hit.size(); hitno++) {
        // for each hit the distance from the hit to x,y line through (0,0) in the xy plane is calcualted and then distance*cos(theta) gives
        // z_hit - z

        double distance = MuonHoughMathUtils::signedDistanceToLine(getHit(hitno)->getHitx(), getHit(hitno)->getHity(), 0, m_ephi - M_PI);

        double z = getHit(hitno)->getHitz() -
                   (distance * std::cos(m_etheta) / std::sin(m_etheta));  // distance * sin (m_etheta) = L  // - sign correct?
        // hough correction in here?

        // straight line fitter?

        ez += z;
        count++;
    }

    ez = ez / count;

    return ez;
}

void MuonHoughPattern::printHoughPattern() const {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughPattern::printHoughPattern");
    if (m_hit.empty()) {
        if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "MuonHoughPattern::No pattern_Segment" << endmsg;
    }

    if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "MuonHoughPattern::Size of MuonHoughPattern: " << m_hit.size() << endmsg;
    for (unsigned int i = 0; i < m_hit.size(); i++) {
        if (log.level() <= MSG::VERBOSE)
            log << MSG::VERBOSE << m_hit[i]->getHitx() << " " << m_hit[i]->getHity() << " " << m_hit[i]->getHitz() << " "
                << m_hit[i]->getId() << endmsg;
    }
}

double MuonHoughPattern::getEAngle() const {
    double eangle = 0;
    switch (m_id_number) {
        case MuonHough::hough_xy: eangle = m_ephi; break;
        case MuonHough::hough_rz:
        case MuonHough::hough_rzcosmics:
        case MuonHough::hough_rz_rpc:
        case MuonHough::hough_rz_mdt:
        case MuonHough::hough_curved_at_a_cylinder: eangle = m_etheta; break;
        default:
            MsgStream log(Athena::getMessageSvc(), "MuonHoughPattern::getEAngle");
            if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "MuonHoughPattern::no valid id_number" << endmsg;
    }
    return eangle;
}

double MuonHoughPattern::getER() const {
    double er = 0;
    switch (m_id_number) {
        case MuonHough::hough_xy: er = m_erphi; break;
        case MuonHough::hough_rz:
        case MuonHough::hough_rzcosmics:
        case MuonHough::hough_rz_rpc:
        case MuonHough::hough_rz_mdt:
        case MuonHough::hough_curved_at_a_cylinder: er = m_ertheta; break;
        default:
            MsgStream log(Athena::getMessageSvc(), "MuonHoughPattern::getER");
            if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "MuonHoughPattern::no valid id_number" << endmsg;
    }
    return er;
}

void MuonHoughPattern::setEAngle(double eangle) {
    switch (m_id_number) {
        case MuonHough::hough_xy: m_ephi = eangle; break;
        case MuonHough::hough_rz:
        case MuonHough::hough_rzcosmics:
        case MuonHough::hough_rz_rpc:
        case MuonHough::hough_rz_mdt:
        case MuonHough::hough_curved_at_a_cylinder: m_etheta = eangle; break;
        default:
            MsgStream log(Athena::getMessageSvc(), "MuonHoughPattern::setEAngle");
            if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "MuonHoughPattern::no valid id_number" << endmsg;
    }
}

void MuonHoughPattern::setER(double er) {
    switch (m_id_number) {
        case MuonHough::hough_xy: m_erphi = er; break;
        case MuonHough::hough_rz:
        case MuonHough::hough_rzcosmics:
        case MuonHough::hough_rz_rpc:
        case MuonHough::hough_rz_mdt:
        case MuonHough::hough_curved_at_a_cylinder: m_ertheta = er; break;
        default:
            MsgStream log(Athena::getMessageSvc(), "MuonHoughPattern::setER");
            if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "MuonHoughPattern::no valid id_number" << endmsg;
    }
}

Amg::Vector3D MuonHoughPattern::getEPos() const {
    // similar to StandardAlgs::shortestPointOfLineToOrigin3D

    // should be maybe really shortest point, but

    // problem is that there is no starting point on the line (well this calc. pos could be available)

    Amg::Vector3D pos;

    pos[0] = m_erphi * std::sin(m_ephi);
    pos[1] = -m_erphi * std::cos(m_ephi);

    if (m_etheta != 0) {
        pos[2] = m_ertheta / std::sin(m_etheta);
    } else {
        pos[2] = 0;
    }
    return pos;
}

Amg::Vector3D MuonHoughPattern::getEDir() const {
    ///
    CxxUtils::sincos scphi(m_ephi);
    CxxUtils::sincos sctheta(m_etheta);
    const Amg::Vector3D dir{scphi.cs * sctheta.sn, scphi.sn * sctheta.sn, sctheta.cs};
    return dir;
}

void MuonHoughPattern::updateParametersRPhi(bool cosmics) {
    if (empty()) return;

    double sum_x = 0.;
    double sum_y = 0.;

    int hitsno = size();

    for (unsigned int i = 0; i < size(); i++) {
        sum_x += getHitx(i);  //*getOrigWeight(i);
        sum_y += getHity(i);  //*getOrigWeight(i);
    }

    // adding interaction point constraint:
    if (!cosmics || size() == 1) hitsno += size();

    const double av_x = sum_x / (hitsno + 0.);
    const double av_y = sum_y / (hitsno + 0.);

    double sumx = 0.;
    double sumy = 0.;
    for (unsigned int i = 0; i < size(); i++) {
        double hitx = getHitx(i);
        double hity = getHity(i);
        double x_offset = hitx - av_x;
        double y_offset = hity - av_y;
        double weight = (x_offset * x_offset + y_offset * y_offset);  //*getOrigWeight(i);
        int sign = 1;
        if (!cosmics) {
            if (x_offset * hitx + y_offset * hity < 0) { sign = -1; }
        } else {
            if (y_offset < 0) { sign = -1; }  // assume cosmic muon comes from above
        }
        sumx += weight * sign * x_offset;
        sumy += weight * sign * y_offset;
    }
    // adding interaction point (count size)
    if (!cosmics) {
        double weight = av_x * av_x + av_y * av_y;
        sumx += size() * weight * av_x;
        sumy += size() * weight * av_y;
    }

    if (std::abs(sumx) < 0.000001 || std::abs(sumy) < 0.000001) { return; }

    double phi = std::atan2(sumy, sumx);
    if (cosmics) {
        if (phi > 0) phi -= M_PI;  // phi between 0,-Pi for cosmics!
    }

    CxxUtils::sincos scphi(phi);

    const double r0 = scphi.apply(av_x, -av_y);

    setEPhi(phi);
    setERPhi(r0);
}
