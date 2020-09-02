/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        CollinearityConstraint.cxx
                             Veerle Heijne
 ***************************************************************************/

#include "TrkVertexKinematicFitterUtils/CollinearityConstraint.h"

Amg::VectorX
Trk::CollinearityConstraint::vectorOfValues(
  std::vector<Amg::VectorX>& cart_coordList,
  std::vector<int>& charges,
  Amg::Vector3D refPoint,
  double b_fieldTesla) const
{

  unsigned int ntrack = cart_coordList.size();

  Amg::VectorX vectorOfValues2(2, 0);

  if (ntrack != 2) {
    return vectorOfValues2; // returning vector with zero's
  }

  const  double bend_factor = -0.299792458 * b_fieldTesla;

  const double px1 = cart_coordList[0][0];
  const double py1 = cart_coordList[0][1];
  const double pz1 = cart_coordList[0][2];

  const double pt1 = sqrt(px1 * px1 + py1 * py1);

  const double px2 = cart_coordList[1][0];
  const double py2 = cart_coordList[1][1];
  const double pz2 = cart_coordList[1][2];

  double pt2 = sqrt(px2 * px2 + py2 * py2);

  const double deltaX1 = refPoint[0] - cart_coordList[0][4];
  const double deltaY1 = refPoint[1] - cart_coordList[0][5];
  const double deltaX2 = refPoint[0] - cart_coordList[1][4];
  const double deltaY2 = refPoint[1] - cart_coordList[1][5];

  const double a1 = charges[0] * bend_factor;
  const double a2 = charges[1] * bend_factor;

  // constraint equations, using scalar product:
  vectorOfValues2[0] = 1 - ((px1 - a1 * deltaY1) * (px2 - a2 * deltaY2) +
                            (py1 + a1 * deltaX1) * (py2 + a2 * deltaX2)) /
                             (pt1 * pt2);
  vectorOfValues2[1] = pz1 * pt2 - pz2 * pt1;

  return vectorOfValues2;
}

Amg::MatrixX
Trk::CollinearityConstraint::matrixOfDerivatives(
  std::vector<Amg::VectorX>& cart_coordList,
  std::vector<int>& charges,
  Amg::Vector3D refPoint,
  double b_fieldTesla) const
{

  const unsigned int ntrack = cart_coordList.size();

  Amg::MatrixX matrixDeriv2(2, ntrack * 7);

  if (ntrack != 2) {
    return matrixDeriv2; // returning matrix with zero's
  }

  const double bend_factor = -0.299792458 * b_fieldTesla;

  const double px1 = cart_coordList[0][0];
  const double py1 = cart_coordList[0][1];
  const double pz1 = cart_coordList[0][2];

  const double pt1 = sqrt(px1 * px1 + py1 * py1);

  const double px2 = cart_coordList[1][0];
  const double py2 = cart_coordList[1][1];
  const double pz2 = cart_coordList[1][2];

  const double pt2 = sqrt(px2 * px2 + py2 * py2);

  const double deltaX1 = refPoint[0] - cart_coordList[0][4];
  const double deltaY1 = refPoint[1] - cart_coordList[0][5];
  const double deltaX2 = refPoint[0] - cart_coordList[1][4];
  const double deltaY2 = refPoint[1] - cart_coordList[1][5];

  const double a1 = charges[0] * bend_factor;
  const double a2 = charges[1] * bend_factor;

  // derivatives to first constraint-equation:
  matrixDeriv2(0, 0) = -(px2 - a2 * deltaY2) / (pt1 * pt2) +
                       px1 *
                         ((px1 - a1 * deltaY1) * (px2 - a2 * deltaY2) +
                          (py1 + a1 * deltaX1) * (py2 + a2 * deltaX2)) /
                         (pt1 * pt1 * pt1 * pt2); // vs px1
  matrixDeriv2(0, 1) = -(py2 + a2 * deltaX2) / (pt1 * pt2) +
                       py1 *
                         ((px1 - a1 * deltaY1) * (px2 - a2 * deltaY2) +
                          (py1 + a1 * deltaX1) * (py2 + a2 * deltaX2)) /
                         (pt1 * pt1 * pt1 * pt2); // vs py1
  matrixDeriv2(0, 2) = 0.;                        // vs pz1
  matrixDeriv2(0, 3) = 0.;                        // vs E1

  matrixDeriv2(0, 4) = a1 * (py2 + a2 * deltaX2) / (pt1 * pt2);  // vs x1
  matrixDeriv2(0, 5) = -a1 * (px2 - a2 * deltaY2) / (pt1 * pt2); // vs y1
  matrixDeriv2(0, 6) = 0.;                                       // vs z1

  matrixDeriv2(0, 7) = -(px1 - a1 * deltaY1) / (pt1 * pt2) +
                       px2 *
                         ((px1 - a1 * deltaY1) * (px2 - a2 * deltaY2) +
                          (py1 + a1 * deltaX1) * (py2 + a2 * deltaX2)) /
                         (pt1 * pt2 * pt2 * pt2); // vs px2
  matrixDeriv2(0, 8) = -(py1 + a1 * deltaX1) / (pt1 * pt2) +
                       py2 *
                         ((px1 - a1 * deltaY1) * (px2 - a2 * deltaY2) +
                          (py1 + a1 * deltaX1) * (py2 + a2 * deltaX2)) /
                         (pt1 * pt2 * pt2 * pt2); // vs py2
  matrixDeriv2(0, 9) = 0.;                        // vs pz2
  matrixDeriv2(0, 10) = 0.;                       // vs E2

  matrixDeriv2(0, 11) = a2 * (py1 + a1 * deltaX1) / (pt1 * pt2);  // vs x2
  matrixDeriv2(0, 12) = -a2 * (px1 - a1 * deltaY1) / (pt1 * pt2); // vs y2
  matrixDeriv2(0, 13) = 0.;                                       // vs z2

  // derivatives to second constraint-equation:
  matrixDeriv2(1, 0) = -(px1 * pz2) / pt1; // vs px1
  matrixDeriv2(1, 1) = -(py1 * pz2) / pt1; // vs py1
  matrixDeriv2(1, 2) = pt2;                // vs pz1
  matrixDeriv2(1, 3) = 0.;                 // vs E1

  matrixDeriv2(1, 4) = 0.; // vs x1
  matrixDeriv2(1, 5) = 0.; // vs y1
  matrixDeriv2(1, 6) = 0.; // vs z1

  matrixDeriv2(1, 7) = (px2 * pz1) / pt2; // vs px2
  matrixDeriv2(1, 8) = (py2 * pz1) / pt2; // vs py2
  matrixDeriv2(1, 9) = -pt1;              // vs pz2
  matrixDeriv2(1, 10) = 0;                // vs E2

  matrixDeriv2(1, 11) = 0.; // vs x2
  matrixDeriv2(1, 12) = 0.; // vs y2
  matrixDeriv2(1, 13) = 0.; // vs z2

  return matrixDeriv2;
}

