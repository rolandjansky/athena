/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        MassConstraint.cxx
                        maaike.limper@cern.ch
 ***************************************************************************/

#include "TrkVertexKinematicFitterUtils/MassConstraint.h"

Trk::MassConstraint::MassConstraint(double mass)
  : m_mass(mass)
{}

Amg::VectorX
Trk::MassConstraint::vectorOfValues(std::vector<Amg::VectorX>& cart_coordList,
                                    std::vector<int>& charges,
                                    Amg::Vector3D refPoint,
                                    double b_fieldTesla) const
{

  unsigned int ntrack = cart_coordList.size();

  Amg::VectorX vectorOfValues2(1, 0);

  const double bend_factor = -0.299792458 * b_fieldTesla;

  double Etot = 0.;
  double Px = 0.;
  double Py = 0.;
  double Pz = 0.;

  for (unsigned int i = 0; i < ntrack; i++) {
    const double E = cart_coordList[i][3];
    const double px = cart_coordList[i][0];
    const double py = cart_coordList[i][1];
    const double pz = cart_coordList[i][2];

    const double deltaX = refPoint[0] - cart_coordList[i][4];
    const double deltaY = refPoint[1] - cart_coordList[i][5];

    const double a = charges[i] * bend_factor;

    // sum of values of cartesian coordinates at given reference-point
    Etot += E;
    Px += (px - a * deltaY);
    Py += (py + a * deltaX);
    Pz += pz;
  }

  vectorOfValues2[0] =
    Etot * Etot - Px * Px - Py * Py - Pz * Pz -
    m_mass * m_mass; // fill vector using sums made in track-loop

  return vectorOfValues2;
}

Amg::MatrixX
Trk::MassConstraint::matrixOfDerivatives(
  std::vector<Amg::VectorX>& cart_coordList,
  std::vector<int>& charges,
  Amg::Vector3D refPoint,
  double b_fieldTesla) const
{

  const unsigned int ntrack = cart_coordList.size();

  Amg::MatrixX matrixDeriv2(1, ntrack * 7);

  const double bend_factor = -0.299792458 * b_fieldTesla;

  for (unsigned int i = 0; i < ntrack; i++) {
    const double E = cart_coordList[i][3];
    const double px = cart_coordList[i][0];
    const double py = cart_coordList[i][1];
    const double pz = cart_coordList[i][2];

    const double deltaX = refPoint[0] - cart_coordList[i][4];
    const double deltaY = refPoint[1] - cart_coordList[i][5];

    const double a = charges[i] * bend_factor;

    for (unsigned int jtrack = 0; jtrack < ntrack; jtrack++) {
      const unsigned int joff = jtrack * 7;
      matrixDeriv2(0, joff + 0) =
        matrixDeriv2(0, joff + 0) - 2 * (px - a * deltaY);
      matrixDeriv2(0, joff + 1) =
        matrixDeriv2(0, joff + 1) - 2 * (py + a * deltaX);
      matrixDeriv2(0, joff + 2) = matrixDeriv2(0, joff + 2) - 2 * pz;
      matrixDeriv2(0, joff + 3) = matrixDeriv2(0, joff + 3) + 2 * E;

      // shouldn't this go to 0 when deltaX = 0 ?! no! when vertex moves px,py
      // moves and vice-versa so derivative of vertex should depend on px,py
      matrixDeriv2(0, joff + 4) =
        matrixDeriv2(0, joff + 4) + 2 * a * (py + a * deltaX);
      matrixDeriv2(0, joff + 5) =
        matrixDeriv2(0, joff + 5) - 2 * a * (px - a * deltaY);
      //(*p_matrixDeriv)[0][joff+6] = (*p_matrixDeriv)[0][joff+6] + 0.; // no
      //dependence on z!
    }
  }

  return matrixDeriv2;
}

