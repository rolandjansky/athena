/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSurfaces/AnnulusBoundsPC.h"
#include "TrkSurfaces/AnnulusBounds.h"
#include <cmath>

#include <iostream>

Trk::AnnulusBoundsPC::AnnulusBoundsPC(double minR,
                                      double maxR,
                                      double phiMin,
                                      double phiMax,
                                      Amg::Vector2D moduleOrigin,
                                      double phiAvg)
  : m_minR(minR),
    m_maxR(maxR),
    m_phiMin(phiMin),
    m_phiMax(phiMax),
    m_moduleOrigin(moduleOrigin),
    m_phiAvg(phiAvg)
{

  m_boundValues.resize(7);
  m_boundValues[AnnulusBoundsPC::bv_minR] = minR;
  m_boundValues[AnnulusBoundsPC::bv_maxR] = maxR;
  m_boundValues[AnnulusBoundsPC::bv_phiMin] = phiMin;
  m_boundValues[AnnulusBoundsPC::bv_phiMax] = phiMax;
  m_boundValues[AnnulusBoundsPC::bv_phiAvg] = phiAvg;
  m_boundValues[AnnulusBoundsPC::bv_originX] = moduleOrigin.x();
  m_boundValues[AnnulusBoundsPC::bv_originY] = moduleOrigin.y();

  m_rotationStripPC = Eigen::Translation<double, 2>(Amg::Vector2D(0, -m_phiAvg));
  m_translation = Eigen::Translation<double, 2>(m_moduleOrigin);

  m_shiftXY = m_moduleOrigin * -1;
  m_shiftPC = Amg::Vector2D(m_shiftXY.perp(), m_shiftXY.phi());

  // we need the corner points of the module to do the inside
  // checking, calculate them here once, they don't change

  // find inner outter radius at edges in STRIP PC
  auto circIx = [](double O_x, double O_y, double r, double phi)
    -> Amg::Vector2D {
    //                      _____________________________________________
    //                     /      2  2                    2    2  2    2
    //     O_x + O_y*m - \/  - O_x *m  + 2*O_x*O_y*m - O_y  + m *r  + r
    // x = --------------------------------------------------------------
    //                                  2
    //                                 m  + 1
    //
    // y = m*x
    //
    double m = std::tan(phi);
    Amg::Vector2D dir(std::cos(phi), std::sin(phi));
    double x1 = (O_x + O_y*m - std::sqrt(-std::pow(O_x, 2)*std::pow(m, 2) + 2*O_x*O_y*m - std::pow(O_y, 2) + std::pow(m, 2)*std::pow(r, 2) + std::pow(r, 2)))/(std::pow(m, 2) + 1);
    double x2 = (O_x + O_y*m + std::sqrt(-std::pow(O_x, 2)*std::pow(m, 2) + 2*O_x*O_y*m - std::pow(O_y, 2) + std::pow(m, 2)*std::pow(r, 2) + std::pow(r, 2)))/(std::pow(m, 2) + 1);

    Amg::Vector2D v1(x1, m*x1);
    if(v1.dot(dir) > 0) return v1;
    return {x2, m*x2};
  };

  // calculate corners in STRIP XY, keep them we need them for minDistance()
  m_outLeftStripXY = circIx(m_moduleOrigin[Trk::locX], m_moduleOrigin[Trk::locY], m_maxR, m_phiMax);
  m_inLeftStripXY = circIx(m_moduleOrigin[Trk::locX], m_moduleOrigin[Trk::locY], m_minR, m_phiMax);
  m_outRightStripXY = circIx(m_moduleOrigin[Trk::locX], m_moduleOrigin[Trk::locY], m_maxR, m_phiMin);
  m_inRightStripXY = circIx(m_moduleOrigin[Trk::locX], m_moduleOrigin[Trk::locY], m_minR, m_phiMin);

  m_outLeftStripPC = {m_outLeftStripXY.norm(), m_outLeftStripXY.phi()};
  m_inLeftStripPC = {m_inLeftStripXY.norm(), m_inLeftStripXY.phi()};
  m_outRightStripPC = {m_outRightStripXY.norm(), m_outRightStripXY.phi()};
  m_inRightStripPC= {m_inRightStripXY.norm(), m_inRightStripXY.phi()};

  m_outLeftModulePC = stripXYToModulePC(m_outLeftStripXY);
  m_inLeftModulePC = stripXYToModulePC(m_inLeftStripXY);
  m_outRightModulePC = stripXYToModulePC(m_outRightStripXY);
  m_inRightModulePC = stripXYToModulePC(m_inRightStripXY);
}

std::pair<Trk::AnnulusBoundsPC, double>
Trk::AnnulusBoundsPC::fromCartesian(Trk::AnnulusBounds& annbo) //TODO: Removed const, check that this is OK
{
  auto [k_L, k_R, d_L, d_R] = annbo.getEdgeLines();

  double O_x = (d_L - d_R) / (k_R - k_L);
  double O_y = std::fma(O_x, k_L, d_L); // O_x * k_L + d_L
  Amg::Vector2D originStripXY(-O_x, -O_y);

  auto bounds = annbo.getBoundsValues();

  double minR = bounds[AnnulusBounds::bv_minR];
  double maxR = bounds[AnnulusBounds::bv_maxR];
  double phi = bounds[AnnulusBounds::bv_phi];
  double phiS = bounds[AnnulusBounds::bv_phiS];
  double phiAvg = 0; // phiAvg is the bounds-internal local rotation. We don't want one

  // phi is the total opening angle, set up symmetric phi bounds
  double phiMax = phi/2.0;
  double phiMin = -phiMax;

  // need to rotate pi/2 to reproduce ABCartesian orientation, phiS so that phi=0 is center and symmetric
  double phiShift = M_PI_2 - phiS;

  // we need to rotate the origin into the bounds local coordinate system
  Transform2D avgPhiRotation;
  avgPhiRotation = Rotation2D(-phiShift);
  Amg::Vector2D originStripXYRotated = avgPhiRotation * originStripXY;

  AnnulusBoundsPC abpc(minR,
                       maxR,
                       phiMin,
                       phiMax,
                       originStripXYRotated,
                       phiAvg);

  return std::make_pair(std::move(abpc), phiShift);
}

bool
Trk::AnnulusBoundsPC::inside(const Amg::Vector2D& locpo, double tol1, double tol2) const
{
  // locpo is PC in STRIP SYSTEM
  // need to perform internal rotation induced by m_phiAvg
  Amg::Vector2D locpo_rotated = m_rotationStripPC * locpo;
  double tolR = tol1, tolPhi = tol2;
  double phiLoc = locpo_rotated[Trk::locPhi];
  double rLoc = locpo_rotated[Trk::locR];

  if (phiLoc < (m_phiMin - tolPhi) || phiLoc > (m_phiMax + tolPhi)) {
    return false;
  }

  // calculate R in MODULE SYSTEM to evaluate R-bounds
  if (tolR == 0.) {
    // don't need R, can use R^2
    double r_mod2 = m_shiftPC[Trk::locR]*m_shiftPC[Trk::locR]
        + rLoc*rLoc + 2*m_shiftPC[Trk::locR]*rLoc * std::cos(phiLoc - m_shiftPC[Trk::locPhi]);

    if (r_mod2 < m_minR*m_minR || r_mod2 > m_maxR*m_maxR) {
      return false;
    }
  }
  else {
    // use R
    double r_mod = std::sqrt(m_shiftPC[Trk::locR]*m_shiftPC[Trk::locR]
        + rLoc*rLoc + 2*m_shiftPC[Trk::locR]*rLoc * cos(phiLoc - m_shiftPC[Trk::locPhi]));

    if (r_mod < (m_minR - tolR) || r_mod > (m_maxR + tolR)) {
      return false;
    }
  }

  return true;
}


bool
Trk::AnnulusBoundsPC::inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const
{

  // locpo is PC in STRIP SYSTEM

  if(bchk.bcType == BoundaryCheck::absolute) {
    // unpack tolerances and use other methods
    if(bchk.checkLoc1 && bchk.checkLoc2) {
      return inside(locpo, bchk.toleranceLoc1, bchk.toleranceLoc2);
    }
    else {
      if(bchk.checkLoc1) {
        return insideLoc1(locpo, bchk.toleranceLoc1);
      }
      else /* bchk.checkLoc2 */ {
        return insideLoc2(locpo, bchk.toleranceLoc2);
      }
    }
  }
  else {
    // first check if inside. We don't need to look into the covariance if inside
    if(inside(locpo)) {
      return true;
    }

    // we need to rotated the locpo
    Amg::Vector2D locpo_rotated = m_rotationStripPC * locpo;

    // covariance is given in STRIP SYSTEM in PC
    // we need to convert the covariance to the MODULE SYSTEM in PC
    // via jacobian.
    // The following transforms into STRIP XY, does the shift into MODULE XY,
    // and then transforms into MODULE PC
    double dphi = m_phiAvg;
    double phi_strip = locpo_rotated[Trk::locPhi];
    double r_strip = locpo_rotated[Trk::locR];
    double O_x = m_shiftXY[Trk::locX];
    double O_y = m_shiftXY[Trk::locY];

    // For a transformation from cartesian into polar coordinates
    //
    //              [         _________      ]
    //              [        /  2    2       ]
    //              [      \/  x  + y        ]
    //     [ r' ]   [                        ]
    // v = [    ] = [      /       y        \]
    //     [phi']   [2*atan|----------------|]
    //              [      |       _________|]
    //              [      |      /  2    2 |]
    //              [      \x + \/  x  + y  /]
    //
    // Where x, y are polar coordinates that can be rotated by dPhi
    //
    // [x]   [O_x + r*cos(dPhi - phi)]
    // [ ] = [                       ]
    // [y]   [O_y - r*sin(dPhi - phi)]
    //
    // The general jacobian is:
    //
    //        [d        d      ]
    //        [--(f_x)  --(f_x)]
    //        [dx       dy     ]
    // Jgen = [                ]
    //        [d        d      ]
    //        [--(f_y)  --(f_y)]
    //        [dx       dy     ]
    //
    // which means in this case:
    //
    //     [     d                   d           ]
    //     [ ----------(rMod)    ---------(rMod) ]
    //     [ dr_{strip}          dphiStrip       ]
    // J = [                                     ]
    //     [    d                   d            ]
    //     [----------(phiMod)  ---------(phiMod)]
    //     [dr_{strip}          dphiStrip        ]
    //
    // Performing the derivative one gets:
    //
    //     [B*O_x + C*O_y + rStrip  rStrip*(B*O_y + O_x*sin(dPhi - phiStrip))]
    //     [----------------------  -----------------------------------------]
    //     [          ___                               ___                  ]
    //     [        \/ A                              \/ A                   ]
    // J = [                                                                 ]
    //     [  -(B*O_y - C*O_x)           rStrip*(B*O_x + C*O_y + rStrip)     ]
    //     [  -----------------          -------------------------------     ]
    //     [          A                                 A                    ]
    //
    // where
    //        2                                          2                                             2
    // A = O_x  + 2*O_x*rStrip*cos(dPhi - phiStrip) + O_y  - 2*O_y*rStrip*sin(dPhi - phiStrip) + rStrip
    // B = cos(dPhi - phiStrip)
    // C = -sin(dPhi - phiStrip)

    double cosDPhiPhiStrip = std::cos(dphi - phi_strip);
    double sinDPhiPhiStrip = std::sin(dphi - phi_strip);

    double A = O_x*O_x + 2*O_x*r_strip*cosDPhiPhiStrip
      + O_y*O_y - 2*O_y*r_strip*sinDPhiPhiStrip
      + r_strip*r_strip;
    double sqrtA = std::sqrt(A);


    double B = cosDPhiPhiStrip;
    double C = -sinDPhiPhiStrip;
    Eigen::Matrix<double, 2, 2> jacobianStripPCToModulePC;
    jacobianStripPCToModulePC(0, 0) = (B*O_x + C*O_y + r_strip)/sqrtA;
    jacobianStripPCToModulePC(0, 1) = r_strip*(B*O_y + O_x*sinDPhiPhiStrip)/sqrtA;
    jacobianStripPCToModulePC(1, 0) = -(B*O_y - C*O_x)/A;
    jacobianStripPCToModulePC(1, 1) = r_strip*(B*O_x + C*O_y + r_strip)/A;

    // covariance is given in STRIP PC
    Matrix2D covStripPC = bchk.lCovariance;
    // calculate covariance in MODULE PC using jacobian from above
    Matrix2D covModulePC;
    covModulePC = jacobianStripPCToModulePC * covStripPC * jacobianStripPCToModulePC.transpose();

    // Mahalanobis distance uses inverse covariance as weights
    Matrix2D weightStripPC = covStripPC.inverse();
    Matrix2D weightModulePC = covModulePC.inverse();


    double minDist = std::numeric_limits<double>::max();

    Amg::Vector2D currentClosest;
    double currentDist{0.0}; // initialise to zero

    // do projection in STRIP PC

    // first: STRIP system. locpo is in STRIP PC already
    currentClosest = closestOnSegment(m_inLeftStripPC, m_outLeftStripPC, locpo_rotated, weightStripPC);
    currentDist = squaredNorm(locpo_rotated-currentClosest, weightStripPC);
    minDist = currentDist;

    currentClosest = closestOnSegment(m_inRightStripPC, m_outRightStripPC, locpo_rotated, weightStripPC);
    currentDist = squaredNorm(locpo_rotated-currentClosest, weightStripPC);
    if(currentDist < minDist) {
      minDist = currentDist;
    }

    // now: MODULE system. Need to transform locpo to MODULE PC
    //  transform is STRIP PC -> STRIP XY -> MODULE XY -> MODULE PC
    Amg::Vector2D locpoStripXY(locpo_rotated[Trk::locR]*std::cos(locpo_rotated[Trk::locPhi]),
                               locpo_rotated[Trk::locR]*std::sin(locpo_rotated[Trk::locPhi]));
    Amg::Vector2D locpoModulePC = stripXYToModulePC(locpoStripXY);


    // now check edges in MODULE PC (inner and outer circle)
    // assuming Mahalanobis distances are of same unit if covariance
    // is correctly transformed
    currentClosest = closestOnSegment(m_inLeftModulePC, m_inRightModulePC, locpoModulePC, weightModulePC);
    currentDist = squaredNorm(locpoModulePC-currentClosest, weightModulePC);
    if(currentDist < minDist) {
      minDist = currentDist;
    }

    currentClosest = closestOnSegment(m_outLeftModulePC, m_outRightModulePC, locpoModulePC, weightModulePC);
    currentDist = squaredNorm(locpoModulePC-currentClosest, weightModulePC);
    if(currentDist < minDist) {
      minDist = currentDist;
    }

    // compare resulting Mahalanobis distance to configured
    // "number of sigmas"
    // we square it b/c we never took the square root of the distance
    return minDist < bchk.nSigmas*bchk.nSigmas;
  }
}

bool
Trk::AnnulusBoundsPC::insideLoc2(const Amg::Vector2D& locpo, double tol2) const
{
  // locpo is PC in STRIP SYSTEM
  // need to perform internal rotation induced by m_phiAvg
  Amg::Vector2D locpo_rotated = m_rotationStripPC * locpo;
  double tolPhi = tol2;
  double phiLoc = locpo_rotated[Trk::locPhi];

  return !(phiLoc < (m_phiMin - tolPhi) || phiLoc >= (m_phiMax + tolPhi));
}
bool
Trk::AnnulusBoundsPC::insideLoc1(const Amg::Vector2D& locpo, double tol1) const
{
  // locpo is PC in STRIP SYSTEM
  // need to perform internal rotation induced by m_phiAvg
  Amg::Vector2D locpo_rotated = m_rotationStripPC * locpo;
  double tolR = tol1;
  double phiLoc = locpo_rotated[Trk::locPhi];
  double rLoc = locpo_rotated[Trk::locR];

  // calculate R in MODULE SYSTEM to evaluate R-bounds
  if (tolR == 0.) {
    // don't need R, can use R^2
    double r_mod2 = m_shiftPC[Trk::locR]*m_shiftPC[Trk::locR]
        + rLoc*rLoc + 2.0*m_shiftPC[Trk::locR]*rLoc * std::cos(phiLoc - m_shiftPC[Trk::locPhi]);

    if (r_mod2 < m_minR*m_minR || r_mod2 > m_maxR*m_maxR) {
      return false;
    }
  }
  else {
    // use R
    double r_mod = sqrt(m_shiftPC[Trk::locR]*m_shiftPC[Trk::locR]
        + rLoc*rLoc + 2.0*m_shiftPC[Trk::locR]*rLoc * std::cos(phiLoc - m_shiftPC[Trk::locPhi]));

    if (r_mod < (m_minR - tolR) || r_mod > (m_maxR + tolR)) {
      return false;
    }
  }

  return true;
}

double
Trk::AnnulusBoundsPC::minDistance(const Amg::Vector2D& locpo) const
{
  // find the closest point on all edges, calculate distance
  // return smallest one
  // closest distance is cartesian, we want the result in mm.

  Amg::Vector2D locpo_rotated = m_rotationStripPC * locpo;

  // locpo is given in STRIP PC, we need it in STRIP XY and possibly MODULE XY
  double rStrip = locpo_rotated[Trk::locR];
  double phiStrip = locpo_rotated[Trk::locPhi];
  Amg::Vector2D locpoStripXY(rStrip*std::cos(phiStrip), rStrip*std::sin(phiStrip));
  Amg::Vector2D locpoModuleXY = locpoStripXY + m_shiftXY;
  double rMod = locpoModuleXY.norm();
  double phiMod = locpoModuleXY.phi();

  Amg::Vector2D closestStripPC;
  double minDist = std::numeric_limits<double>::max();;
  double curDist{0.0}; // initialise to 0

  // for rmin
  if (m_inRightModulePC[Trk::locPhi] <= phiMod && phiMod < m_inLeftModulePC[Trk::locPhi]) {
    // is inside phi bounds, to comparison to rmin and r max
    // r min
    curDist = std::abs(m_minR - rMod);
    minDist = std::min(minDist, curDist);
  }
  else {
    // is outside phi bounds, closest can only be the edge points here

    // in left
    curDist = (m_inLeftStripXY - locpoStripXY).norm();
    minDist = std::min(minDist, curDist);

    // in right
    curDist = (m_inRightStripXY - locpoStripXY).norm();
    minDist = std::min(minDist, curDist);
  }

  if (m_phiMin <= phiStrip && phiStrip < m_phiMax) {
    // r max
    curDist = std::abs(m_maxR - rMod);
    minDist = std::min(minDist, curDist);
  }
  else {
    // out left
    curDist = (m_outLeftStripXY - locpoStripXY).norm();
    minDist = std::min(minDist, curDist);

    // out right
    curDist = (m_outRightStripXY - locpoStripXY).norm();
    minDist = std::min(minDist, curDist);
  }

  Matrix2D weight = Matrix2D::Identity();

  // phi left
  Amg::Vector2D closestLeft = closestOnSegment(m_inLeftStripXY, m_outLeftStripXY, locpoStripXY, weight);
  curDist = (closestLeft - locpoStripXY).norm();
  if (curDist < minDist) {
    minDist = curDist;
  }

  // phi right
  Amg::Vector2D closestRight = closestOnSegment(m_inRightStripXY, m_outRightStripXY, locpoStripXY, weight);
  curDist = (closestRight - locpoStripXY).norm();
  if (curDist < minDist) {
    minDist = curDist;
  }

  return minDist;
}

double
Trk::AnnulusBoundsPC::r() const
{
  return (rMax() + rMin()) * 0.5;
}

MsgStream& Trk::AnnulusBoundsPC::dump( MsgStream& sl ) const
{
  std::stringstream ss;
  dump(ss);
  sl << ss.str();
  return sl;
}

std::ostream& Trk::AnnulusBoundsPC::dump( std::ostream& sl ) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::AnnulusBoundsPC:  (minR, maxR, phiMin, phiMax, phiAvg, origin(x, y)) = " << "(" ;
  sl << m_boundValues[AnnulusBoundsPC::bv_minR] << ", ";
  sl << m_boundValues[AnnulusBoundsPC::bv_maxR] << ", ";
  sl << m_boundValues[AnnulusBoundsPC::bv_phiMin] << ", ";
  sl << m_boundValues[AnnulusBoundsPC::bv_phiMax] << ", ";
  sl << m_boundValues[AnnulusBoundsPC::bv_phiAvg] << ", ";
  sl << m_boundValues[AnnulusBoundsPC::bv_originX] << ", ";
  sl << m_boundValues[AnnulusBoundsPC::bv_originY];
  sl << ")";
  sl << std::setprecision(-1);
  return sl;
}

std::array<std::pair<double, double>,4 >
Trk::AnnulusBoundsPC::corners() const {
  auto rot = m_rotationStripPC.inverse();

  auto to_pair = [](const Amg::Vector2D& v) -> std::pair<double, double>
  {
    return {v[0], v[1]};
  };

  return {
    to_pair(rot * m_outRightStripPC),
    to_pair(rot * m_outLeftStripPC),
    to_pair(rot * m_inLeftStripPC),
    to_pair(rot * m_inRightStripPC)
  };

}

Amg::Vector2D
Trk::AnnulusBoundsPC::stripXYToModulePC(const Amg::Vector2D& vStripXY) const
{
  Amg::Vector2D vecModuleXY = vStripXY + m_shiftXY;
  return {vecModuleXY.norm(), vecModuleXY.phi()};
}

Amg::Vector2D
Trk::AnnulusBoundsPC::closestOnSegment(const Amg::Vector2D& a,
                                       const Amg::Vector2D& b,
                                       const Amg::Vector2D& p,
                                       const Eigen::Matrix<double, 2, 2>& weight) 
{
  // connecting vector
  auto     n       = b - a;
  // squared norm of line
  auto     f       = (n.transpose() * weight * n).value();
  // weighted scalar product of line to point and segment line
  auto     u       = ((p - a).transpose() * weight * n).value() / f;
  // clamp to [0, 1], convert to point
  return std::min(std::max(u, 0.0), 1.0) * n + a;
}

double
Trk::AnnulusBoundsPC::squaredNorm(const Amg::Vector2D& v, const Eigen::Matrix<double, 2, 2>& weight) 
{
  return (v.transpose() * weight * v).value();
}

Amg::Vector2D
Trk::AnnulusBoundsPC::moduleOrigin() const
{
  return Eigen::Rotation2D<double>(m_phiAvg) * m_moduleOrigin;
}
