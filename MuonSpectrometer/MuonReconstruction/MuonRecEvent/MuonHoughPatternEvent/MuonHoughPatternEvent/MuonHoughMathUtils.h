/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHMATHUTILS_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHMATHUTILS_H

#include <cmath>
#include <string>
#include <vector>

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonHoughPatternEvent/MuonHoughHit.h"

namespace MuonHough {
    constexpr double two_Pi = 2 * M_PI;
    constexpr double degree_rad_conversion_factor = M_PI / 180.;
    constexpr double rad_degree_conversion_factor = 180. / M_PI;

    // Geometry for curved Hough extrapolation
    /** relation for transition between endcap and barrel 11.43 m (r) / 14m (z) */
    constexpr double tan_barrel = 11430. / 14000.;
    /** radius of cylinder */
    constexpr double radius_cylinder = 4000.;
    /** length of cylinder */
    constexpr double z_cylinder = 6000.;
    /** z value whereafter no magnetic field / curvature */
    constexpr double z_end = 15000.;
    /** range where hit is curved in endcap region */
    constexpr double z_magnetic_range = z_end - z_cylinder;
    /** range where hit is curved in endcap region ('squared') */
    constexpr double z_magnetic_range_squared = z_end * z_end - z_cylinder * z_cylinder;
}  // namespace MuonHough

class MuonHoughMathUtils {
public:
    /** default constructor */
    MuonHoughMathUtils();
    /** destructor */
    virtual ~MuonHoughMathUtils() = default;

    /** sign (-1 or 1) of a double */
    static int sgn(double d);
    /** step function at place x0 */
    static int step(double d, double x0 = 0);

    /** distance from (x0,y0) to the line (r0,phi), phi in rad */
    static double signedDistanceToLine(double x0, double y0, double r0, double phi);
    /** distance from (x0,y0) to the line (r0,phi), phi in rad */
    static double distanceToLine(double x0, double y0, double r0, double phi);
    /** increments x with inc till above x */
    static double incrementTillAbove0(double x, double inc, double zero = 0);
    /** computes angle in degrees between 0 and 360 */
    static double angleFrom0To360(double angle);
    /** computes angle in degrees between 0 and 180 */
    static double angleFrom0To180(double angle);
    /** computes angle in rad between 0 and Pi */
    static double angleFrom0ToPi(double angle);
    /** computes angle in rad between -Pi and Pi */
    static double angleFromMinusPiToPi(double angle);
    /** converts angle in rad to degrees */
    double angleFromRadialToGrad(double angle) const;
    /** converts angle in degrees to rad */
    double angleFromGradToRadial(double angle) const;

    /** converts integer to string */
    static std::string intToString(int i);
    /** converts string to char* */
    static const char* stringToChar(std::string& s);
    /** converts integer to char* */
    static const char* intToChar(int i);

    /** distance from (x0,y0) to line (r,phi) */
    double distanceToLine2D(double x0, double y0, double r, double phi) const;
    /** distance from (x0,y0,z0) to line (x,y,z,phi,theta) */
    static double distanceToLine3D(const Amg::Vector3D& point, const Amg::Vector3D& l_trans, double phi, double theta);

    /** distance of line y = ax + b to origin */
    static double distanceOfLineToOrigin2D(double a, double b);
    /** signed distance of line with point (x,y) and angle phi to origin */
    static double signedDistanceOfLineToOrigin2D(double x, double y, double phi);

    /** calculates theta at (x,y,z) for curved track model */
    static double thetaForCurvedHit(double invcurvature, MuonHoughHit* hit);
    /** calculates theta at (x,y,z) for curved track model, for positive and negative curvature */
    static void thetasForCurvedHit(double ratio, MuonHoughHit* hit, double& theta1, double& theta2);
    /** calculates distance of point (x,y,z) to curved track with z0, theta and invcurvature for curved track model */
    static double signedDistanceCurvedToHit(double z0, double theta, double invcurvature, const Amg::Vector3D& hit);

    /**
     * @brief extrapolates road to global position
     * @param[in] roadpos The position of the combined pattern (should be perigee position)
     * @param[in] roadmom The momentum of the combined pattern (should be perigee momentum)
     * @param[in] pos The global position to extrapolate to
     * @param[out] roadpose The nearest to pos, estimated road position
     * @param[out] roadpose The nearest to pos, estimated road direction
     */

    static void extrapolateCurvedRoad(const Amg::Vector3D& roadpos, const Amg::Vector3D& roadmom, const Amg::Vector3D& pos,
                                      Amg::Vector3D& roadpose, Amg::Vector3D& roaddire);

    /** calculates the 3d-point closest to origin in xy-plane */
    static Amg::Vector3D shortestPointOfLineToOrigin3D(const Amg::Vector3D& vec, double phi, double theta);

    /** calculates the 3d-point closest to origin */
    static Amg::Vector3D shortestPointOfLineToOrigin(const Amg::Vector3D& vec, double phi, double theta);

    /** calculates if line (x,y,z,phi,theta) crosses cylinder (r_0,z_0) around origin */
    static bool lineThroughCylinder(const Amg::Vector3D& vec, double phi, double theta, double r_0, double z_0);

};

inline double MuonHoughMathUtils::angleFromRadialToGrad(double angle) const {
    return (angle / MuonHough::degree_rad_conversion_factor);
}  // angle in radial
inline double MuonHoughMathUtils::angleFromGradToRadial(double angle) const {
    return (angle * MuonHough::degree_rad_conversion_factor);
}  // angle in grad

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHMATHUTILS_H
