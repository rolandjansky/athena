/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AnnulusBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/AnnulusBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <algorithm> //std::max_element
#include <cmath>     //sin, cos etc
#include <iomanip>
#include <ostream>

namespace {
constexpr double twoPi = 2.0 * M_PI;
}

// Class checking the interface of an ellipse with a circle
class EllipseCollisionTest
{
private:
  int m_maxIterations;
  bool iterate(double x, double y, double c0x, double c0y, double c2x, double c2y, double rr) const
  {
    std::vector<double> innerPolygonCoef(m_maxIterations + 1);
    std::vector<double> outerPolygonCoef(m_maxIterations + 1);

    for (int t = 1; t <= m_maxIterations; t++) {
      int numNodes = 4 << t;
      // innerPolygonCoef[t] = 0.5/std::cos(4*std::acos(0.0)/numNodes);
      innerPolygonCoef[t] = 0.5 / std::cos(twoPi / numNodes);
      double c1x = (c0x + c2x) * innerPolygonCoef[t];
      double c1y = (c0y + c2y) * innerPolygonCoef[t];
      double tx = x - c1x; // t indicates a translated coordinate
      double ty = y - c1y;
      if (tx * tx + ty * ty <= rr) {
        return true; // collision with t1
      }
      double t2x = c2x - c1x;
      double t2y = c2y - c1y;
      if (tx * t2x + ty * t2y >= 0 && tx * t2x + ty * t2y <= t2x * t2x + t2y * t2y &&
          (ty * t2x - tx * t2y >= 0 || rr * (t2x * t2x + t2y * t2y) >= (ty * t2x - tx * t2y) * (ty * t2x - tx * t2y))) {
        return true; // collision with t1---t2
      }
      double t0x = c0x - c1x;
      double t0y = c0y - c1y;
      if (tx * t0x + ty * t0y >= 0 && tx * t0x + ty * t0y <= t0x * t0x + t0y * t0y &&
          (ty * t0x - tx * t0y <= 0 || rr * (t0x * t0x + t0y * t0y) >= (ty * t0x - tx * t0y) * (ty * t0x - tx * t0y))) {
        return true; // collision with t1---t0
      }
      outerPolygonCoef[t] = 0.5 / (std::cos(M_PI / numNodes) * std::cos(M_PI / numNodes));
      double c3x = (c0x + c1x) * outerPolygonCoef[t];
      double c3y = (c0y + c1y) * outerPolygonCoef[t];
      if ((c3x - x) * (c3x - x) + (c3y - y) * (c3y - y) < rr) {
        c2x = c1x;
        c2y = c1y;
        continue; // t3 is inside circle
      }
      double c4x = c1x - c3x + c1x;
      double c4y = c1y - c3y + c1y;
      if ((c4x - x) * (c4x - x) + (c4y - y) * (c4y - y) < rr) {
        c0x = c1x;
        c0y = c1y;
        continue; // t4 is inside circle
      }
      double t3x = c3x - c1x;
      double t3y = c3y - c1y;
      if (ty * t3x - tx * t3y <= 0 || rr * (t3x * t3x + t3y * t3y) > (ty * t3x - tx * t3y) * (ty * t3x - tx * t3y)) {
        if (tx * t3x + ty * t3y > 0) {
          if (std::abs(tx * t3x + ty * t3y) <= t3x * t3x + t3y * t3y ||
              (x - c3x) * (c0x - c3x) + (y - c3y) * (c0y - c3y) >= 0) {
            c2x = c1x;
            c2y = c1y;
            continue; // circle center is inside t0---t1---t3
          }
        } else if (-(tx * t3x + ty * t3y) <= t3x * t3x + t3y * t3y ||
                   (x - c4x) * (c2x - c4x) + (y - c4y) * (c2y - c4y) >= 0) {
          c0x = c1x;
          c0y = c1y;
          continue; // circle center is inside t1---t2---t4
        }
      }
      return false; // no collision possible
    }
    return false; // out of iterations so it is unsure if there was a collision. But have to return something.
  }

public:
  // test for collision between an ellipse of horizontal radius w and vertical radius h at (x0, y0) and a circle of
  // radius r at (x1, y1)
  bool collide(double x0, double y0, double w, double h, double x1, double y1, double r) const
  {
    double x = std::fabs(x1 - x0);
    double y = std::fabs(y1 - y0);

    //		return iterate(x, y, w, 0, 0, h, r*r);

    if (r > 0) {
      if (x * x + (h - y) * (h - y) <= r * r || (w - x) * (w - x) + y * y <= r * r ||
          x * h + y * w <= w * h // collision with (0, h)
          || ((x * h + y * w - w * h) * (x * h + y * w - w * h) <= r * r * (w * w + h * h) && x * w - y * h >= -h * h &&
              x * w - y * h <= w * w)) { // collision with (0, h)---(w, 0)
        return true;
      }
        if ((x - w) * (x - w) + (y - h) * (y - h) <= r * r || (x <= w && y - r <= h) || (y <= h && x - r <= w)) {
          return iterate(x, y, w, 0, 0, h, r * r); // collision within triangle (0, h) (w, h) (0, 0) is possible
        }
        return false;

    }
      double R = -r;
      double localCos = x / R;
      double deltaR = std::sqrt(h * h + (w * w - h * h) * localCos * localCos);
      return deltaR >= R - std::sqrt(x * x + y * y);

  }
  EllipseCollisionTest(int maxIterations) { this->m_maxIterations = maxIterations; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

// default constructor
Trk::AnnulusBounds::AnnulusBounds()
  : //    Trk::SurfaceBounds()
  m_boundValues(AnnulusBounds::bv_length, 0.)
  , m_maxYout{}
  , m_minYout{}
  , m_maxXout{}
  , m_minXout{}
  , m_maxYin{}
  , m_minYin{}
  , m_maxXin{}
  , m_minXin{}
  , m_k_L{}
  , m_k_R{}
  , m_d_L{}
  , m_d_R{}
  , m_solution_L_min{}
  , m_solution_L_max{}
  , m_solution_R_min{}
  , m_solution_R_max{}
{
  //    nop
}

// constructor from arguments I
Trk::AnnulusBounds::AnnulusBounds(double minR, double maxR, double R, double phi, double phiS)
  : m_boundValues(AnnulusBounds::bv_length, 0.)
{
  m_boundValues[AnnulusBounds::bv_minR] = std::fabs(minR);
  m_boundValues[AnnulusBounds::bv_maxR] = std::fabs(maxR);
  m_boundValues[AnnulusBounds::bv_R] = std::fabs(R);
  m_boundValues[AnnulusBounds::bv_phi] = std::fabs(phi);
  m_boundValues[AnnulusBounds::bv_phiS] = std::fabs(phiS);
  if (m_boundValues[AnnulusBounds::bv_minR] > m_boundValues[AnnulusBounds::bv_maxR])
    swap(m_boundValues[AnnulusBounds::bv_minR], m_boundValues[AnnulusBounds::bv_maxR]);

  m_k_L = std::tan((M_PI + phi) / 2. + phiS);
  m_k_R = std::tan((M_PI - phi) / 2. + phiS);

  m_d_L = R * std::sin(phiS) * std::tan((M_PI - phi) / 2. - phiS) + R * (1. - std::cos(phiS));
  m_d_R = R * std::sin(phiS) * std::tan((M_PI + phi) / 2. - phiS) + R * (1. - std::cos(phiS));

  // solving quadratic equation to find four corners of the AnnulusBounds
  m_solution_L_min = circleLineIntersection(minR, m_k_L, m_d_L);
  m_solution_L_max = circleLineIntersection(maxR, m_k_L, m_d_L);
  m_solution_R_min = circleLineIntersection(minR, m_k_R, m_d_R);
  m_solution_R_max = circleLineIntersection(maxR, m_k_R, m_d_R);

  std::vector<TDD_real_t> XX;
  XX.push_back(m_solution_L_min[0]);
  XX.push_back(m_solution_L_max[0]);
  XX.push_back(m_solution_R_min[0]);
  XX.push_back(m_solution_R_max[0]);

  std::vector<TDD_real_t> YY;
  YY.push_back(m_solution_L_min[1]);
  YY.push_back(m_solution_L_max[1]);
  YY.push_back(m_solution_R_min[1]);
  YY.push_back(m_solution_R_max[1]);
  YY.push_back(maxR);

  m_maxXout = *std::max_element(XX.begin(), XX.end());
  m_minXout = *std::min_element(XX.begin(), XX.end());
  m_maxYout = *std::max_element(YY.begin(), YY.end());
  m_minYout = *std::min_element(YY.begin(), YY.end());

  m_maxXin = std::min(m_solution_R_min[0], m_solution_R_max[0]);
  m_minXin = std::max(m_solution_L_min[0], m_solution_L_max[0]);
  m_maxYin = std::min(m_solution_R_max[1], m_solution_L_max[1]);
  m_minYin = minR;
}

bool
Trk::AnnulusBounds::operator==(const Trk::SurfaceBounds& sbo) const
{
  // check the type first not to compare apples with oranges
  const Trk::AnnulusBounds* annbo = dynamic_cast<const Trk::AnnulusBounds*>(&sbo);
  if (!annbo)
    return false;
  return (m_boundValues == annbo->m_boundValues);
}

// checking if inside bounds
bool
Trk::AnnulusBounds::inside(const Amg::Vector2D& locpo, double tol1, double tol2) const

{
  // a fast FALSE
  double localY = locpo[Trk::locY];
  if (localY > (m_maxYout + tol2) || localY < (m_minYout - tol2))
    return false;
  // a fast FALSE
  double localX = locpo[Trk::locX];
  if (localX > (m_maxXout + tol1) || localX < (m_minXout - tol1))
    return false;
  // a fast TRUE
  if (localX > (m_minXin - tol1) && localX < (m_maxXin + tol1) && localY > (m_minYin - tol2) &&
      localY < (m_maxYin + tol2))
    return true;

  ///////
  //	if (this->minDistance(locpo)>std::max(tol1,tol2)) return false;

  double localR2 = localX * localX + localY * localY;
  double localR = std::sqrt(localR2);
  double localCos = localX / localR;
  double localSin = localY / localR;
  double deltaR = std::sqrt(tol2 * tol2 * localSin * localSin + tol1 * tol1 * localCos * localCos);

  double minR = m_boundValues[AnnulusBounds::bv_minR];
  double maxR = m_boundValues[AnnulusBounds::bv_maxR];

  bool condRad = (localR < maxR + deltaR && localR > minR - deltaR);
  bool condL =
    (isRight(locpo, tol1, tol2, m_solution_L_max[0], m_solution_L_max[1], m_solution_L_min[0], m_solution_L_min[1]));
  bool condR =
    (isLeft(locpo, tol1, tol2, m_solution_R_max[0], m_solution_R_max[1], m_solution_R_min[0], m_solution_R_min[1]));

  return (condRad && condL && condR);
}

bool
Trk::AnnulusBounds::inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const
{

  if (bchk.bcType == 0 || bchk.nSigmas == 0)
    return AnnulusBounds::inside(locpo, bchk.toleranceLoc1, bchk.toleranceLoc2);

  sincosCache scResult = bchk.FastSinCos(locpo(1, 0));

  EllipseCollisionTest test(4);

  const Amg::Vector2D& locpoCar = locpo;
  AmgMatrix(2, 2) lCovarianceCar = bchk.lCovariance;

  // ellipse is always at (0,0), surface is moved to ellipse position and then rotated
  double w = bchk.nSigmas * std::sqrt(lCovarianceCar(0, 0));
  double h = bchk.nSigmas * std::sqrt(lCovarianceCar(1, 1));

  // a fast FALSE
  double maxTol = std::max(w, h);
  double minTol = std::min(w, h);
  double localY = locpo[Trk::locY];
  if (localY > (m_maxYout + maxTol) || localY < (m_minYout - maxTol))
    return false;
  // a fast FALSE
  double localX = locpo[Trk::locX];
  if (localX > (m_maxXout + maxTol) || localX < (m_minXout - maxTol))
    return false;
  // a fast TRUE
  if (localX > (m_minXin - minTol) && localX < (m_maxXin + minTol) && localY > (m_minYin - minTol) &&
      localY < (m_maxYin + minTol))
    return true;

  double x0 = 0;
  double y0 = 0;
  float theta = (lCovarianceCar(1, 0) != 0 && (lCovarianceCar(1, 1) - lCovarianceCar(0, 0)) != 0)
                  ? .5 * bchk.FastArcTan(2 * lCovarianceCar(1, 0) / (lCovarianceCar(1, 1) - lCovarianceCar(0, 0)))
                  : 0.;
  scResult = bchk.FastSinCos(theta);
  AmgMatrix(2, 2) rotMatrix;
  rotMatrix << scResult.cosC, scResult.sinC, -scResult.sinC, scResult.cosC;
  Amg::Vector2D tmp = rotMatrix * (-locpoCar);
  double x1 = tmp(0, 0);
  double y1 = tmp(1, 0);
  double maxR = m_boundValues[AnnulusBounds::bv_maxR];
  double minR = m_boundValues[AnnulusBounds::bv_minR];

  bool condR = (test.collide(x0, y0, w, h, x1, y1, -minR) && test.collide(x0, y0, w, h, x1, y1, maxR));

  // compute KDOP and axes for surface polygon
  std::vector<KDOP> elementKDOP(4);
  std::vector<Amg::Vector2D> elementP(4);

  AmgMatrix(2, 2) normal;
  normal << 0, -1, 1, 0;

  // ellipse is always at (0,0), surface is moved to ellipse position and then rotated
  Amg::Vector2D p;
  p << m_solution_L_min[0], m_solution_L_min[1];
  elementP[0] = (rotMatrix * (p - locpo));

  p << m_solution_L_max[0], m_solution_L_max[1];
  elementP[1] = (rotMatrix * (p - locpo));

  p << m_solution_R_max[0], m_solution_R_max[1];
  elementP[2] = (rotMatrix * (p - locpo));

  p << m_solution_R_min[0], m_solution_R_min[1];
  elementP[3] = (rotMatrix * (p - locpo));

  std::vector<Amg::Vector2D> axis = { normal * (elementP[0] - elementP[1]),
                                      normal * (elementP[1] - elementP[2]),
                                      normal * (elementP[2] - elementP[3]),
                                      normal * (elementP[3] - elementP[0]) };
  bchk.ComputeKDOP(elementP, axis, elementKDOP);
  // compute KDOP for error ellipse
  std::vector<KDOP> errelipseKDOP(4);
  bchk.ComputeKDOP(bchk.EllipseToPoly(4), axis, errelipseKDOP);

  bool condSide = bchk.TestKDOPKDOP(elementKDOP, errelipseKDOP);

  bool condLine =
    (isAbove(locpo, 0, 0, m_solution_L_max[0], m_solution_L_max[1], m_solution_R_max[0], m_solution_R_max[1]) &&
     isRight(locpo, 0, 0, m_solution_L_max[0], m_solution_L_max[1], m_solution_L_min[0], m_solution_L_min[1]) &&
     isLeft(locpo, 0, 0, m_solution_R_max[0], m_solution_R_max[1], m_solution_R_min[0], m_solution_R_min[1]));

  if (condLine){
    return condR;
  }

  return (condR && condSide);
}

// checking if local point lies above a line
bool
Trk::AnnulusBounds::isAbove(const Amg::Vector2D& locpo,
                            double tol1,
                            double tol2,
                            double x1,
                            double y1,
                            double x2,
                            double y2) const
{
  if (x2 != x1) {
    double k = (y2 - y1) / (x2 - x1);
    double d = y1 - k * x1;
    // the most tolerant approach for tol1 and tol2
    double sign = k > 0. ? -1. : +1.;
    return (locpo[Trk::locY] + tol2 > (k * (locpo[Trk::locX] + sign * tol1) + d));
  }
    return false;
}

// checking if local point right from a line
bool
Trk::AnnulusBounds::isRight(const Amg::Vector2D& locpo,
                            double tol1,
                            double tol2,
                            double x1,
                            double y1,
                            double x2,
                            double y2) const
{

  if (x1 != x2) {
    double k = (y2 - y1) / (x2 - x1);
    double d = y1 - k * x1;

    if (k > 0){
      return (locpo[Trk::locY] < (k * locpo[Trk::locX] + d) ||
              EllipseIntersectLine(locpo, tol1, tol2, x1, y1, x2, y2));
    }
    if (k < 0){
      return (locpo[Trk::locY] > (k * locpo[Trk::locX] + d) ||
              EllipseIntersectLine(locpo, tol1, tol2, x1, y1, x2, y2));
    }

    return false;
  }
  return (locpo[Trk::locX] > x1 ||
          EllipseIntersectLine(locpo, tol1, tol2, x1, y1, x2, y2));
}

// checking if local point left from a line
bool
Trk::AnnulusBounds::isLeft(const Amg::Vector2D& locpo,
                           double tol1,
                           double tol2,
                           double x1,
                           double y1,
                           double x2,
                           double y2) const
{

  if (x1 != x2) {
    double k = (y2 - y1) / (x2 - x1);
    double d = y1 - k * x1;

    if (k < 0){
      return (locpo[Trk::locY] < (k * locpo[Trk::locX] + d) ||
              EllipseIntersectLine(locpo, tol1, tol2, x1, y1, x2, y2));
    }
    if (k > 0){
      return (locpo[Trk::locY] > (k * locpo[Trk::locX] + d) ||
              EllipseIntersectLine(locpo, tol1, tol2, x1, y1, x2, y2));
    }

    return false;
  }
  return (locpo[Trk::locX] < x1 ||
          EllipseIntersectLine(locpo, tol1, tol2, x1, y1, x2, y2));
}

double
Trk::AnnulusBounds::minDistance(const Amg::Vector2D& locpo) const
{

  // Calculate four corner points - crossings of an arc with a line
  double minR = m_boundValues[AnnulusBounds::bv_minR];
  double maxR = m_boundValues[AnnulusBounds::bv_maxR];

  // distance to left and right line
  double distLine_L = distanceToLine(locpo, m_solution_L_min, m_solution_L_max);
  double distLine_R = distanceToLine(locpo, m_solution_R_min, m_solution_R_max);

  double dist = std::min(distLine_L, distLine_R);

  // calculate distance to both arcs
  double distMin = distanceToArc(locpo, minR, m_solution_L_min, m_solution_R_min);
  double distMax = distanceToArc(locpo, maxR, m_solution_L_max, m_solution_R_max);

  double distArc = std::min(distMin, distMax);

  dist = std::min(dist, distArc);

  if (inside(locpo, 0., 0.)){
    dist = -dist;
  }
  return dist;
}

/** Circle and line intersection **/
std::vector<double>
Trk::AnnulusBounds::circleLineIntersection(double R, double k, double d) const
{
  // change k, d -> phi, d
  // think: which of two intersection points to chose
  std::vector<double> solution;
  double x1;
  double y1;
  double x2;
  double y2;

  // Intersection of a line with an arc
  // equation:   (1+k^2)*x^2 + 2kdx +d^2 - R^2 = 0
  double delta = 4. * k * d * k * d - 4. * (1 + k * k) * (d * d - R * R);

  if (delta < 0){
    return solution;
  }
    x1 = (-2. * k * d - std::sqrt(delta)) / (2. * (1 + k * k));
    x2 = (-2. * k * d + std::sqrt(delta)) / (2. * (1 + k * k));
    y1 = k * x1 + d;
    y2 = k * x2 + d;

  if (y1 > y2) {
    solution.push_back(x1);
    solution.push_back(y1);
  } else {
    solution.push_back(x2);
    solution.push_back(y2);
  }
  return solution;
}

/** Distance to line */
double
Trk::AnnulusBounds::distanceToLine(const Amg::Vector2D& locpo,
                                   std::vector<TDD_real_t> P1,
                                   std::vector<TDD_real_t> P2) const
{
  double P1x = P1[0];
  double P1y = P1[1];
  double P2x = P2[0];
  double P2y = P2[1];

  double A = P2x - P1x;
  double B = P2y - P1y;
  double P3x;
  double P3y;

  double X = locpo[Trk::locX];
  double Y = locpo[Trk::locY];

  double u = (A * (X - P1x) + B * (Y - P1y)) / (A * A + B * B);
  if (u <= 0) {
    P3x = P1x;
    P3y = P1y;
  } else if (u >= 1) {
    P3x = P2x;
    P3y = P2y;
  } else {
    P3x = P1x + u * A;
    P3y = P1y + u * B;
  }
  return std::sqrt((X - P3x) * (X - P3x) + (Y - P3y) * (Y - P3y));
}

/** Distance to arc */
double
Trk::AnnulusBounds::distanceToArc(const Amg::Vector2D& locpo,
                                  double R,
                                  std::vector<TDD_real_t> sL,
                                  std::vector<TDD_real_t> sR) const
{

  double X = locpo[Trk::locX];
  double Y = locpo[Trk::locY];

  double tanlocPhi = X / Y;
  double tanPhi_L = sL[0] / sL[1];
  double tanPhi_R = sR[0] / sR[1];

  if (tanlocPhi > tanPhi_L && tanlocPhi < tanPhi_R){
    return std::fabs(std::sqrt(X * X + Y * Y) - R);
  }

  return 9999999999.;
}

// ellipse and line intersection
bool
Trk::AnnulusBounds::EllipseIntersectLine(const Amg::Vector2D& locpo,
                                         double h,
                                         double k,
                                         double x1,
                                         double y1,
                                         double x2,
                                         double y2) const
{
  // h, k - ellipse axis (h - horizontal, k - vertical)
  // x1, y1, x2, y2 - define a line

  // Solving two equations
  // x*x/(h*h) + y*y/(k*k) = 1
  //
  // y = y1 + (y2-y1) * (x-x1) / (x2-x1)
  //

  // fast false - if the tolerance is zero
  if (h == 0 && k == 0)
    return false;

  double r;
  double s;
  double t;
  double m;
  double c;
  double d;

  x1 = x1 - locpo[Trk::locX];
  y1 = y1 - locpo[Trk::locY];
  x2 = x2 - locpo[Trk::locX];
  y2 = y2 - locpo[Trk::locY];

  //
  if (x1 != x2) {
    m = (y2 - y1) / (x2 - x1);
    c = y1 - m * x1;

    if (h == 0)
      return (std::fabs(c) < k);
    if (k == 0)
      return (std::fabs(c / m) < h);

    r = m * m * h * h + k * k;
    s = 2 * m * c * h * h;
    t = h * h * c * c - h * h * k * k;

    d = s * s - 4 * r * t;

  } else {
    //
    // vertical line case
    //

    d = std::fabs(x1) - h;
  }

  return (d >= 0.0); // intersection if d>=0
}

MsgStream&
Trk::AnnulusBounds::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::AnnulusBounds:  (minR, maxR, phi) = "
     << "(" << m_boundValues[AnnulusBounds::bv_minR] << ", " << m_boundValues[AnnulusBounds::bv_maxR] << ", "
     << m_boundValues[AnnulusBounds::bv_phi] << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::ostream&
Trk::AnnulusBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::AnnulusBounds:  (minR, maxR, phi) = "
     << "(" << m_boundValues[AnnulusBounds::bv_minR] << ", " << m_boundValues[AnnulusBounds::bv_maxR] << ", "
     << m_boundValues[AnnulusBounds::bv_phi] << ")";
  sl << std::setprecision(-1);
  return sl;
}
