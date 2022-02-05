/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"

#include <float.h>

#include <cassert>
#include <iostream>
#include <sstream>

#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/sincos.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
MuonHoughMathUtils::MuonHoughMathUtils() {}

int MuonHoughMathUtils::sgn(double d) { return d >= 0 ? 1 : -1; }

int MuonHoughMathUtils::step(double d, double x0) {
    if (d == x0) {
        MsgStream log(Athena::getMessageSvc(), "MuonHoughMathUtils::step");
        if (log.level() <= MSG::WARNING) log << MSG::WARNING << "WARNING: Possible mistake in Step function" << endmsg;
    }
    if (d <= x0) { return 0; }
    if (d > x0) { return 1; }
    return -1;
}

double MuonHoughMathUtils::signedDistanceToLine(
    double x0, double y0, double r0,
    double phi)  // distance from (x0,y0) to the line (r0,phi) , phi in [-Pi,Pi] ,different phi than in calculateangle() (==angle(2))
{
    CxxUtils::sincos scphi(phi);
    double distance = scphi.apply(x0, -y0) - r0;
    return distance;
}

double MuonHoughMathUtils::distanceToLine(double x0, double y0, double r0, double phi) {
    return std::abs(signedDistanceToLine(x0, y0, r0, phi));
}

double MuonHoughMathUtils::incrementTillAbove0(double x, double inc, double zero) {
    while (x > inc + zero) { x -= inc; }
    while (x < zero) { x += inc; }
    return x;
}

double MuonHoughMathUtils::angleFrom0To360(double angle) { return incrementTillAbove0(angle, 360.); }

double MuonHoughMathUtils::angleFrom0To180(double angle) { return incrementTillAbove0(angle, 180.); }

double MuonHoughMathUtils::angleFrom0ToPi(double angle) { return incrementTillAbove0(angle, M_PI); }

double MuonHoughMathUtils::angleFromMinusPiToPi(double angle) { return incrementTillAbove0(angle, 2 * M_PI, -M_PI); }

std::string MuonHoughMathUtils::intToString(int i) {
    std::string s;
    std::stringstream ss;
    ss << i;
    ss >> s;

    return s;
}

const char* MuonHoughMathUtils::stringToChar(std::string& string) {
    const char* constcharstar = string.data();
    return constcharstar;
}

const char* MuonHoughMathUtils::intToChar(int i) {
    std::string string = intToString(i);
    const char* constcharstar = stringToChar(string);
    return constcharstar;
}

double MuonHoughMathUtils::distanceToLine2D(double x0, double y0, double r0, double phi)
    const  // distance from (x0,y0) to line (r,phi) // from mathworld.wolfram.com/Point-LineDistance2-Dimensional.html // better to use
           // distancetoline()
{
    // need two points on line:

    CxxUtils::sincos scphi(phi);

    double x1 = -r0 * scphi.sn;  // point closest to origin
    double y1 = r0 * scphi.cs;

    Amg::Vector3D v{x1 - x0, y1 - y0, 0};  // (p1 - p0)

    Amg::Vector3D r{x1, y1, 0};  // vector perpendicular to line
    double distance = r.dot(v) / r.mag();

    return distance;
}

double MuonHoughMathUtils::distanceToLine3D(
    const Amg::Vector3D& point, const Amg::Vector3D& l_trans, double phi,
    double theta)  // from wolfram: http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
{
    Amg::Vector3D x1 = l_trans - point;  // x1-x0

    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    ///
    const Amg::Vector3D dir{scphi.cs * sctheta.sn, scphi.sn * sctheta.sn, sctheta.cs};

    // sqrt(x3^2) == 1; !

    double distance;
    const Amg::Vector3D x4 = dir.cross(x1);

    distance = x4.mag();

    return distance;
}

double MuonHoughMathUtils::distanceOfLineToOrigin2D(double a, double b) { return std::abs(b / (std::sqrt(a * a + 1))); }

double MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(double x, double y, double phi) {
    CxxUtils::sincos scphi(phi);
    return scphi.apply(x, -y);
}

Amg::Vector3D MuonHoughMathUtils::shortestPointOfLineToOrigin3D(
    const Amg::Vector3D& vec, double phi, double theta)  // actually this is the 3d-point closest to origin in xy-plane
{
    double r0 = signedDistanceOfLineToOrigin2D(vec[Amg::x], vec[Amg::y], phi);

    CxxUtils::sincos scphi(phi);

    double x0 = r0 * scphi.sn;
    double y0 = -r0 * scphi.cs;

    const double d_x = vec[Amg::x] - x0;
    const double d_y = vec[Amg::y] - y0;
    double radius = std::hypot(d_x, d_y);

    if (std::abs(d_y - scphi.sn * radius) > std::abs(d_y + scphi.cs * radius))  // also possible for x
    {
        radius = -radius;
    }

    double z0 = vec[Amg::z] - radius / std::tan(theta);

    const Amg::Vector3D p{x0, y0, z0};
    return p;
}

Amg::Vector3D MuonHoughMathUtils::shortestPointOfLineToOrigin(
    const Amg::Vector3D& line_trans, double phi,
    double theta)  // from wolfram: http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
{
    // origin:
    static const Amg::Vector3D origin{0., 0., 0.};

    const Amg::Vector3D x1 = line_trans - origin;

    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);
    const Amg::Vector3D dir{scphi.cs * sctheta.sn, scphi.sn * sctheta.sn, sctheta.cs};

    double time = 0;
    double x5 = 0;
    x5 = x1.dot(dir);
    time = -x5;

    Amg::Vector3D short_point = x1 + time * dir;
    return short_point;
}

bool MuonHoughMathUtils::lineThroughCylinder(const Amg::Vector3D& line_trans, double phi, double theta, double r_cyl, double z_cyl) {
    // if there is one, then track will be split
    assert(r_cyl >= 0);

    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    // 1 -- check if there is an intersection at z0=+-c0
    double p_1 = line_trans[Amg::z] - z_cyl;
    double p_2 = line_trans[Amg::z] + z_cyl;
    double tantheta = sctheta.sn / sctheta.cs;

    double x_1 = line_trans[Amg::x] - p_1 * scphi.cs * tantheta;
    double y_1 = line_trans[Amg::y] - p_1 * scphi.sn * tantheta;
    double r_1 = std::hypot(x_1, y_1);
    if (r_1 < r_cyl) { return true; }

    double x_2 = line_trans[Amg::x] - p_2 * scphi.cs * tantheta;
    double y_2 = line_trans[Amg::y] - p_2 * scphi.sn * tantheta;
    double r_2 = std::hypot(x_2, y_2);
    if (r_2 < r_cyl) { return true; }

    // 2 -- check if there is an intersection with the circle x^2 + y^2 = r_cyl^2 and the line y=px+q, p = tan(phi), q = y_0 - x_0 tan(phi)
    // <--> r_cyl^2 = (px+q)^2 + x^2
    double r_0 = scphi.apply(-line_trans[Amg::x], line_trans[Amg::y]);

    if (std::abs(r_0) > r_cyl) return false;

    // 3 -- check if the intersection is cylinder: for -z_cyl<z<z_cyl
    double s_1 = -scphi.sn * r_0;
    double s_2 = scphi.cs * std::sqrt(r_cyl * r_cyl - r_0 * r_0);

    x_1 = s_1 + s_2;

    double inv_angle = 1 / (scphi.cs * tantheta);

    double z_1 = line_trans[Amg::z] + (x_1 - line_trans[Amg::x]) * inv_angle;

    if (std::abs(z_1) < z_cyl) return true;

    x_2 = s_1 - s_2;
    double z_2 = line_trans[Amg::z] + (x_2 - line_trans[Amg::x]) * inv_angle;

    return std::abs(z_2) < z_cyl;
}

double MuonHoughMathUtils::signedDistanceCurvedToHit(double z0, double theta, double invcurvature, const Amg::Vector3D& hit) {
    double hitr = hit.perp();

    CxxUtils::sincos sctheta(theta);

    double sdistance = FLT_MAX;
    // if angle rotation larger than Pi/2 then return large distance (formulas don't work for flip in z!)
    if (sctheta.apply(hitr, hit[Amg::z]) < 0) return sdistance;  //  hitr*sctheta.sn + hitz*sctheta.cs < 0

    const int sign = hit[Amg::z] < 0 ? -1 : 1;

    if (std::abs(hitr / hit[Amg::z]) > MuonHough::tan_barrel) {
        // Barrel Extrapolation
        if (std::abs(sctheta.sn) > 1e-7) {
            double diffr = hitr - MuonHough::radius_cylinder;
            double zext = z0 + (hitr * sctheta.cs + diffr * diffr * invcurvature) / sctheta.sn;
            sdistance = (zext - hit[Amg::z]);
        }

    } else {
        if (std::abs(sctheta.sn) > 1e-7) {
            double rext = 0.;
            if (std::abs(hit[Amg::z]) < MuonHough::z_end) {
                // Forward in Toroid
                double diffz = hit[Amg::z] - sign * MuonHough::z_cylinder;
                rext = ((hit[Amg::z] - z0) * sctheta.sn - diffz * diffz * invcurvature) / sctheta.cs;

            } else {
                // Forward OutSide EC Toroid
                rext = ((hit[Amg::z] - z0) * sctheta.sn +
                        (MuonHough::z_magnetic_range_squared - sign * 2 * hit[Amg::z] * (MuonHough::z_magnetic_range)) * invcurvature) /
                       sctheta.cs;
            }
            sdistance = (rext - hitr);
        }
    }
    return sdistance;
}

double MuonHoughMathUtils::thetaForCurvedHit(double invcurvature, MuonHoughHit* hit) {
    double ratio = hit->getMagneticTrackRatio() * invcurvature;
    if (std::abs(ratio) < 1.)
        return hit->getTheta() + std::asin(ratio);
    else
        return -1;
}

void MuonHoughMathUtils::thetasForCurvedHit(double ratio, MuonHoughHit* hit, double& theta1, double& theta2) {
    /** returns angle for positive and negative curvature (positive first) */

    if (std::abs(ratio) < 1.) {
        double asin_ratio = std::asin(ratio);
        theta1 = hit->getTheta() + asin_ratio;
        theta2 = hit->getTheta() - asin_ratio;
    }
}

void MuonHoughMathUtils::extrapolateCurvedRoad(const Amg::Vector3D& roadpos, const Amg::Vector3D& roadmom, const Amg::Vector3D& pos,
                                               Amg::Vector3D& roadpose, Amg::Vector3D& roaddire) {
    /** Extrapolate pattern given by a roadpos and roadmom (should be perigee)
        to a position in space pos
        And determine extrapolated position: roadpose
        and direction: roaddire
        using the curved track model
    */

    //  m_log<< MSG::VERBOSE << "Extrapolate the road to the segment (hit)" <<endmsg;

    const double theta = roadmom.theta();
    const double phi = roadmom.phi();

    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    double tantheta = sctheta.sn / sctheta.cs;

    double r0 = scphi.apply(roadpos.x(), -roadpos.y());
    double charge = 1.;
    if (r0 < 0) charge = -1.;
    double invcurvature = charge / roadmom.mag();
    // No momentum estimate
    if (roadmom.mag() < 2) invcurvature = 0.;

    double posr = std::sqrt(pos.x() * pos.x() + pos.y() * pos.y());
    double thetan = theta;

    int sign = 1;
    if (pos.z() < 0) sign = -1;

    double xe = pos.x();
    double ye = pos.y();
    double ze = pos.z();
    double rotationangle = 0.;

    if (std::abs(posr / pos.z()) > MuonHough::tan_barrel) {
        // Barrel Extrapolation
        if ((posr * posr - r0 * r0) > 0) {
            double lenr = std::sqrt(posr * posr - r0 * r0);
            double len = posr - fabs(r0);
            double diffr = posr - MuonHough::radius_cylinder;
            rotationangle = diffr * invcurvature / sctheta.sn;
            xe = roadpos.x() + lenr * scphi.cs;
            ye = roadpos.y() + lenr * scphi.sn;
            ze = roadpos.z() + len / tantheta + diffr * rotationangle;
            thetan = std::atan2(1., 1 / tantheta + 2 * rotationangle);
        }
    } else {
        double lext = 0., rotationangle = 0.;
        if (std::abs(pos.z()) < MuonHough::z_end) {
            // Forward in Toroid
            double diffz = pos.z() - sign * MuonHough::z_cylinder;
            rotationangle = diffz * invcurvature / sctheta.cs;
            lext = (pos.z() - roadpos.z()) * tantheta - diffz * rotationangle;
        } else {
            // Forward OutSide EC Toroid
            double effcurv = invcurvature / sctheta.cs;
            rotationangle = sign * (MuonHough::z_end - MuonHough::z_cylinder) * effcurv;
            lext = (pos.z() - roadpos.z()) * tantheta +
                   (MuonHough::z_magnetic_range_squared - 2 * sign * pos.z() * MuonHough::z_magnetic_range) * effcurv;
        }
        xe = roadpos.x() + lext * scphi.cs;
        ye = roadpos.y() + lext * scphi.sn;
        double dx = tantheta - 2 * rotationangle;
        if (dx != 0) thetan = std::atan2(1., 1 / dx);
    }

    // In case direction theta is rotated for low momenta: flip direction vector
    CxxUtils::sincos scthetan(thetan);
    if (sctheta.cs * scthetan.cs + sctheta.sn * scthetan.sn < 0) {
        roaddire = Amg::Vector3D(scthetan.sn * scphi.cs, scthetan.sn * scphi.sn, -scthetan.cs);
    } else {
        roaddire = Amg::Vector3D(scthetan.sn * scphi.cs, scthetan.sn * scphi.sn, scthetan.cs);
    }
    roadpose = Amg::Vector3D(xe, ye, ze);
}
