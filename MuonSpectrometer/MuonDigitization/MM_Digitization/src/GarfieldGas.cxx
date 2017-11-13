/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------
// GarfieldGas
// ----------------------------------------------

#include "MM_Digitization/GarfieldGas.h"

#include "AthenaKernel/MsgStreamMember.h"

#include <iostream>
#include <fstream>

#include <iomanip>

#include <cmath>

#include <string>
#include <vector>

#include <stdlib.h>
#include <string.h>


static const double Small = 1.e-20;

namespace Numerics {


double
Divdif(const std::vector<double>& f, const std::vector<double>& a,
       int nn, double x, int mm) {
  Athena::MsgStreamMember msg("Numerics::Divdif");
  // C++ version of DIVDIF (CERN program library E105) which performs
  // tabular interpolation using symmetrically placed argument points.

  double t[20], d[20];

  const int mmax = 10;

  // Check the arguments.
  if (nn < 2) {
    msg << MSG::INFO << "Divdif:\n";
    msg << MSG::INFO << "    Array length < 2.\n";
    return 0.;
  }
  if (mm < 1) {
    msg << MSG::INFO << "Divdif:\n";
    msg << MSG::INFO << "    Interpolation order < 1.\n";
    return 0.;
  }

  // Deal with the case that X is located at A(1) or A(N).
  if (fabs(x - a[0]) < 1.e-6 * (fabs(a[0]) + fabs(a[nn - 1]))) {
    return f[0];
  }
  if (fabs(x - a[nn - 1]) < 1.e-6 * (fabs(a[0]) + fabs(a[nn - 1]))) {
    return f[nn - 1];
  }

  // Find subscript IX of X in array A.
  int n = nn;
  int m;
  if (mm <= mmax && mm <= n - 1) {
    m = mm;
  } else {
    if (mmax <= n - 1) {
      m = mmax;
    } else {
      m = n - 1;
    }
  }
  int mplus = m + 1;
  int ix = 0;
  int iy = n + 1;
  int mid;
  if (a[0] > a[n - 1]) {
    // Search decreasing arguments.
    do {
      mid = (ix + iy) / 2;
      if (x > a[mid - 1]) {
        iy = mid;
      } else {
        ix = mid;
      }
    } while (iy - ix > 1);
  } else {
    // Search increasing arguments.
    do {
      mid = (ix + iy) / 2;
      if (x < a[mid-1]) {
        iy = mid;
      } else {
        ix = mid;
      }
    } while (iy - ix > 1);
  }
  //  Copy reordered interpolation points into (T[I],D[I]), setting
  //  EXTRA to True if M+2 points to be used.
  int npts = m + 2 - (m % 2);
  int ip = 0;
  int l = 0;
  int isub;
  do {
    isub = ix + l;
    if ((1 > isub) || (isub > n)) {
      // Skip point.
      npts = mplus;
    } else {
      // Insert point.
      ip++;
      t[ip - 1] = a[isub - 1];
      d[ip - 1] = f[isub - 1];
    }
    if (ip < npts) {
      l = -l;
      if(l >= 0) {
        l++;
      }
    }
  } while (ip < npts);

  bool extra = npts != mplus;
  // Replace d by the leading diagonal of a divided-difference table,
  // supplemented by an extra line if EXTRA is True.
  for (int l = 1; l <= m; l++) {
    if (extra) {
      isub = mplus - l;
      d[m + 1] = (d[m + 1] - d[m - 1]) / (t[m + 1] - t[isub - 1]);
    }
    int i = mplus;
    for (int j = l; j <= m; j++) {
      isub = i - l;
      d[i - 1] = (d[i - 1] - d[i - 1 - 1]) / (t[i - 1] - t[isub - 1]);
      i--;
    }
  }
  // Evaluate the Newton interpolation formula at X, averaging two values
  // of last difference if EXTRA is True.
  double sum = d[mplus-1];
  if (extra) {
    sum = 0.5 * (sum + d[m + 1]);
  }
  int j = m;
  for (int l = 1; l <= m; l++) {
    sum = d[j - 1] + (x - t[j - 1]) * sum;
    j--;
  }
  return sum;

}


bool
Boxin3(std::vector<std::vector<std::vector<double> > >& value,
       std::vector<double>& xAxis,
       std::vector<double>& yAxis,
       std::vector<double>& zAxis,
       int nx, int ny, int nz,
       double xx, double yy, double zz, double& f, int iOrder) {
  Athena::MsgStreamMember msg("Numerics::Boxin3");
  // std::cout << nx << ", " << ny << ", " << nz << "\n";
  //-----------------------------------------------------------------------
  //   BOXIN3 - interpolation of order 1 and 2 in an irregular rectangular
  //            3-dimensional grid.
  //   (Last changed on 13/ 2/00.)
  //-----------------------------------------------------------------------

  int iX0 = 0, iX1 = 0;
  int iY0 = 0, iY1 = 0;
  int iZ0 = 0, iZ1 = 0;
  double fX[4], fY[4], fZ[4];

  // Ensure we are in the grid.
  const double x = std::min(std::max(xx, std::min(xAxis[0], xAxis[nx - 1])), std::max(xAxis[0], xAxis[nx - 1]));
  const double y = std::min(std::max(yy, std::min(yAxis[0], yAxis[ny - 1])), std::max(yAxis[0], yAxis[ny - 1]));
  const double z = std::min(std::max(zz, std::min(zAxis[0], zAxis[nz - 1])), std::max(zAxis[0], zAxis[nz - 1]));

  // Make sure we have enough points.
  if (iOrder < 0 || iOrder > 2 ||
      nx < 1 || ny < 1 || nz < 1) {
    msg << MSG::INFO << "Boxin3:\n";
    msg << MSG::INFO << "    Incorrect order or number of points.\n";
    msg << MSG::INFO << "    No interpolation.\n";
    f = 0.;
    return false;
  }

  if (iOrder == 0 || nx == 1) {
    // Zeroth order interpolation in x.
    // Find the nearest node.
    double dist = fabs(x - xAxis[0]);
    int iNode = 0;
    for (int i = 1; i < nx; i++) {
      if (fabs(x - xAxis[i]) < dist) {
        dist = fabs(x - xAxis[i]);
        iNode = i;
      }
    }
    // Set the summing range.
    iX0 = iNode;
    iX1 = iNode;
    // Establish the shape functions.
    fX[0] = 0.;
    fX[1] = 0.;
    fX[2] = 0.;
    fX[3] = 0.;
  } else if (iOrder == 1 || nx == 2) {
    // First order interpolation in x.
    // Find the grid segment containing this point.
    int iGrid = 0;
    for (int i = 1; i < nx; i++) {
      if ((xAxis[i - 1] - x) * (x - xAxis[i]) >= 0.) {
        iGrid = i;
      }
    }
    // Ensure there won't be divisions by zero.
    if (xAxis[iGrid] == xAxis[iGrid - 1]) {
      msg << MSG::INFO << "Boxin3:\n";
      msg << MSG::INFO << "    Incorrect grid; no interpolation.\n";
      f = 0.;
      return false;
    }
    // Compute local coordinates.
    const double xLocal = (x - xAxis[iGrid-1]) /
      (xAxis[iGrid] - xAxis[iGrid - 1]);
    // Set the summing range.
    iX0 = iGrid - 1;
    iX1 = iGrid;
    // Set the shape functions.
    fX[0] = 1. - xLocal;
    fX[1] = xLocal;
    fX[2] = 0.;
    fX[3] = 0.;
  } else if (iOrder == 2) {
    // Second order interpolation in x.
    // Find the grid segment containing this point.
    int iGrid = 0;
    for (int i = 1; i < nx; i++) {
      if ((xAxis[i - 1] - x) * (x - xAxis[i]) >= 0.) {
        iGrid = i;
      }
    }
    // Compute the local coordinate for this grid segment.
    const double xLocal = (x - xAxis[iGrid - 1]) /
      (xAxis[iGrid] - xAxis[iGrid - 1]);
    // Set the summing range and shape functions.
    if (iGrid == 1) {
      iX0 = iGrid - 1;
      iX1 = iGrid + 1;
      if (xAxis[iX0]     == xAxis[iX0 + 1] ||
          xAxis[iX0]     == xAxis[iX0 + 2] ||
          xAxis[iX0 + 1] == xAxis[iX0 + 2]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in x coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fX[0] = (x           - xAxis[iX0 + 1]) *
	(x           - xAxis[iX0 + 2]) /
	((xAxis[iX0] - xAxis[iX0 + 1]) *
	 (xAxis[iX0] - xAxis[iX0 + 2]));
      fX[1] = (x               - xAxis[iX0]) *
	(x               - xAxis[iX0 + 2]) /
	((xAxis[iX0 + 1] - xAxis[iX0]) *
	 (xAxis[iX0 + 1] - xAxis[iX0 + 2]));
      fX[2] = (x               - xAxis[iX0])*
	(x               - xAxis[iX0 + 1]) /
	((xAxis[iX0 + 2] - xAxis[iX0]) *
	 (xAxis[iX0 + 2] - xAxis[iX0 + 1]));
    } else if (iGrid == nx - 1) {
      iX0 = iGrid - 2;
      iX1 = iGrid;
      if (xAxis[iX0]     == xAxis[iX0 + 1] ||
          xAxis[iX0]     == xAxis[iX0 + 2] ||
          xAxis[iX0 + 1] == xAxis[iX0 + 2]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in x coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fX[0] = (x           - xAxis[iX0 + 1]) *
	(x           - xAxis[iX0 + 2]) /
	((xAxis[iX0] - xAxis[iX0 + 1]) *
	 (xAxis[iX0] - xAxis[iX0 + 2]));
      fX[1] = (x               - xAxis[iX0]) *
	(x               - xAxis[iX0 + 2]) /
	((xAxis[iX0 + 1] - xAxis[iX0]) *
	 (xAxis[iX0 + 1] - xAxis[iX0 + 2]));
      fX[2] = (x               - xAxis[iX0])*
	(x               - xAxis[iX0 + 1]) /
	((xAxis[iX0 + 2] - xAxis[iX0]) *
	 (xAxis[iX0 + 2] - xAxis[iX0 + 1]));
    } else {
      iX0 = iGrid - 2;
      iX1 = iGrid + 1;
      if (xAxis[iX0]     == xAxis[iX0 + 1] ||
          xAxis[iX0]     == xAxis[iX0 + 2] ||
          xAxis[iX0]     == xAxis[iX0 + 3] ||
          xAxis[iX0 + 1] == xAxis[iX0 + 2] ||
          xAxis[iX0 + 1] == xAxis[iX0 + 3] ||
          xAxis[iX0 + 2] == xAxis[iX0 + 3]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in x coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fX[0] = (x           - xAxis[iX0 + 1]) *
	(x           - xAxis[iX0 + 2]) /
	((xAxis[iX0] - xAxis[iX0 + 1]) *
	 (xAxis[iX0] - xAxis[iX0 + 2]));
      fX[1] = (x               - xAxis[iX0]) *
	(x               - xAxis[iX0 + 2]) /
	((xAxis[iX0 + 1] - xAxis[iX0]) *
	 (xAxis[iX0 + 1] - xAxis[iX0 + 2]));
      fX[2] = (x               - xAxis[iX0])*
	(x               - xAxis[iX0 + 1]) /
	((xAxis[iX0 + 2] - xAxis[iX0]) *
	 (xAxis[iX0 + 2] - xAxis[iX0 + 1]));
      fX[0] *= (1. - xLocal);
      fX[1] = fX[1] * (1. - xLocal) +
	xLocal * (x - xAxis[iX0 + 2]) *
	(x - xAxis[iX0 + 3]) /
	((xAxis[iX0 + 1] - xAxis[iX0 + 2]) *
	 (xAxis[iX0 + 1] - xAxis[iX0 + 3]));
      fX[2] = fX[2] * (1. - xLocal) +
	xLocal * (x - xAxis[iX0 + 1]) *
	(x - xAxis[iX0 + 3]) /
	((xAxis[iX0 + 2] - xAxis[iX0 + 1]) *
	 (xAxis[iX0 + 2] - xAxis[iX0 + 3]));
      fX[3] = xLocal * (x - xAxis[iX0 + 1]) *
	(x - xAxis[iX0 + 2]) /
	((xAxis[iX0 + 3] - xAxis[iX0 + 1]) *
	 (xAxis[iX0 + 3] - xAxis[iX0 + 2]));
    }
  }

  if (iOrder == 0 || ny == 1) {
    // Zeroth order interpolation in y.
    // Find the nearest node.
    double dist = fabs(y - yAxis[0]);
    int iNode = 0;
    for (int i = 1; i < ny; i++) {
      if (fabs(y - yAxis[i]) < dist) {
        dist = fabs(y - yAxis[i]);
        iNode = i;
      }
    }
    // Set the summing range.
    iY0 = iNode;
    iY1 = iNode;
    // Establish the shape functions.
    fY[0] = 1.;
    fY[1] = 0.;
    fY[2] = 0.;
  } else if (iOrder == 1 || ny == 2) {
    // First order interpolation in y.
    // Find the grid segment containing this point.
    int iGrid = 0;
    for (int i = 1; i < ny; i++) {
      if ((yAxis[i - 1] - y) * (y - yAxis[i]) >= 0.) {
        iGrid = i;
      }
    }
    // Ensure there won't be divisions by zero.
    if (yAxis[iGrid] == yAxis[iGrid - 1]) {
      msg << MSG::INFO << "Boxin3:\n";
      msg << MSG::INFO << "    Incorrect grid; no interpolation.\n";
      f = 0.;
      return false;
    }
    // Compute local coordinates.
    const double yLocal = (y - yAxis[iGrid - 1]) /
      (yAxis[iGrid] - yAxis[iGrid - 1]);
    // Set the summing range.
    iY0 = iGrid - 1;
    iY1 = iGrid;
    // Set the shape functions.
    fY[0] = 1. - yLocal;
    fY[1] = yLocal;
    fY[2] = 0.;
  } else if (iOrder == 2) {
    // Second order interpolation in y.
    // Find the grid segment containing this point.
    int iGrid = 0;
    for (int i = 1; i < ny; i++) {
      if ((yAxis[i - 1] - y) * (y - yAxis[i]) >= 0.) {
        iGrid = i;
      }
    }
    // Compute the local coordinate for this grid segment.
    const double yLocal = (y - yAxis[iGrid - 1]) /
      (yAxis[iGrid] - yAxis[iGrid - 1]);
    // Set the summing range and shape functions.
    // These assignments are shared by all of the following conditions,
    // so it's easier to take them out.
    fY[0] = (y           - yAxis[iY0 + 1]) *
      (y           - yAxis[iY0 + 2]) /
      ((yAxis[iY0] - yAxis[iY0 + 1]) *
       (yAxis[iY0] - yAxis[iY0 + 2]));
    fY[1] = (y               - yAxis[iY0]) *
      (y               - yAxis[iY0 + 2]) /
      ((yAxis[iY0 + 1] - yAxis[iY0]) *
       (yAxis[iY0 + 1] - yAxis[iY0 + 2]));
    fY[2] = (y               - yAxis[iY0])*
      (y               - yAxis[iY0 + 1]) /
      ((yAxis[iY0 + 2] - yAxis[iY0]) *
       (yAxis[iY0 + 2] - yAxis[iY0 + 1]));

    if (iGrid == 1) {
      iY0 = iGrid - 1;
      iY1 = iGrid + 1;
      if (yAxis[iY0]     == yAxis[iY0 + 1] ||
          yAxis[iY0]     == yAxis[iY0 + 2] ||
          yAxis[iY0 + 1] == yAxis[iY0 + 2]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in y coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fY[0] = (y           - yAxis[iY0 + 1]) *
	(y           - yAxis[iY0 + 2]) /
	((yAxis[iY0] - yAxis[iY0 + 1]) *
	 (yAxis[iY0] - yAxis[iY0 + 2]));
      fY[1] = (y               - yAxis[iY0]) *
	(y               - yAxis[iY0 + 2]) /
	((yAxis[iY0 + 1] - yAxis[iY0]) *
	 (yAxis[iY0 + 1] - yAxis[iY0 + 2]));
      fY[2] = (y               - yAxis[iY0])*
	(y               - yAxis[iY0 + 1]) /
	((yAxis[iY0 + 2] - yAxis[iY0]) *
	 (yAxis[iY0 + 2] - yAxis[iY0 + 1]));
    } else if (iGrid == ny - 1) {
      iY0 = iGrid - 2;
      iY1 = iGrid;
      if (yAxis[iY0]     == yAxis[iY0 + 1] ||
          yAxis[iY0]     == yAxis[iY0 + 2] ||
          yAxis[iY0 + 1] == yAxis[iY0 + 2]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in y coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fY[0] = (y           - yAxis[iY0 + 1]) *
	(y           - yAxis[iY0 + 2]) /
	((yAxis[iY0] - yAxis[iY0 + 1]) *
	 (yAxis[iY0] - yAxis[iY0 + 2]));
      fY[1] = (y               - yAxis[iY0]) *
	(y               - yAxis[iY0 + 2]) /
	((yAxis[iY0 + 1] - yAxis[iY0]) *
	 (yAxis[iY0 + 1] - yAxis[iY0 + 2]));
      fY[2] = (y               - yAxis[iY0])*
	(y               - yAxis[iY0 + 1]) /
	((yAxis[iY0 + 2] - yAxis[iY0]) *
	 (yAxis[iY0 + 2] - yAxis[iY0 + 1]));
    } else {
      iY0 = iGrid - 2;
      iY1 = iGrid + 1;
      if (yAxis[iY0]     == yAxis[iY0 + 1] ||
          yAxis[iY0]     == yAxis[iY0 + 2] ||
          yAxis[iY0]     == yAxis[iY0 + 3] ||
          yAxis[iY0 + 1] == yAxis[iY0 + 2] ||
          yAxis[iY0 + 1] == yAxis[iY0 + 3] ||
          yAxis[iY0 + 2] == yAxis[iY0 + 3]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in y coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fY[0] = (y           - yAxis[iY0 + 1]) *
	(y           - yAxis[iY0 + 2]) /
	((yAxis[iY0] - yAxis[iY0 + 1]) *
	 (yAxis[iY0] - yAxis[iY0 + 2]));
      fY[1] = (y               - yAxis[iY0]) *
	(y               - yAxis[iY0 + 2]) /
	((yAxis[iY0 + 1] - yAxis[iY0]) *
	 (yAxis[iY0 + 1] - yAxis[iY0 + 2]));
      fY[2] = (y               - yAxis[iY0])*
	(y               - yAxis[iY0 + 1]) /
	((yAxis[iY0 + 2] - yAxis[iY0]) *
	 (yAxis[iY0 + 2] - yAxis[iY0 + 1]));

      fY[0] *= (1. - yLocal);
      fY[1] = fY[1] * (1. - yLocal) +
	yLocal * (y - yAxis[iY0 + 2]) *
	(y - yAxis[iY0 + 3]) /
	((yAxis[iY0 + 1] - yAxis[iY0 + 2]) *
	 (yAxis[iY0 + 1] - yAxis[iY0 + 3]));
      fY[2] = fY[2] * (1. - yLocal) +
	yLocal * (y - yAxis[iY0 + 1]) *
	(y - yAxis[iY0 + 3]) /
	((yAxis[iY0 + 2] - yAxis[iY0 + 1]) *
	 (yAxis[iY0 + 2] - yAxis[iY0 + 3]));
      fY[3] = yLocal * (y - yAxis[iY0 + 1]) *
	(y- yAxis[iY0 + 2]) /
	((yAxis[iY0 + 3] - yAxis[iY0 + 1]) *
	 (yAxis[iY0 + 3] - yAxis[iY0 + 2]));
    }
  }

  if (iOrder == 0 || nz == 1) {
    // Zeroth order interpolation in z.
    // Find the nearest node.
    double dist = fabs(z - zAxis[0]);
    int iNode = 0;
    for (int i = 1; i < nz; i++) {
      if (fabs(z - zAxis[i]) < dist) {
        dist = fabs(z - zAxis[i]);
        iNode = i;
      }
    }
    // Set the summing range.
    iZ0 = iNode;
    iZ1 = iNode;
    // Establish the shape functions.
    fZ[0] = 1.;
    fZ[1] = 0.;
    fZ[2] = 0.;
  } else if (iOrder == 1 || nz == 2) {
    // First order interpolation in z.
    // Find the grid segment containing this point.
    int iGrid = 0;
    for (int i = 1; i < nz; i++) {
      if ((zAxis[i - 1] - z) * (z - zAxis[i]) >= 0.) {
        iGrid = i;
      }
    }
    // Ensure there won't be divisions by zero.
    if (zAxis[iGrid] == zAxis[iGrid - 1]) {
      msg << MSG::INFO << "Boxin3:\n";
      msg << MSG::INFO << "    Incorrect grid; no interpolation.\n";
      f = 0.;
      return false;
    }
    // Compute local coordinates.
    const double zLocal = (z - zAxis[iGrid - 1]) /
      (zAxis[iGrid] - zAxis[iGrid - 1]);
    // Set the summing range.
    iZ0 = iGrid - 1;
    iZ1 = iGrid;
    // Set the shape functions.
    fZ[0] = 1. - zLocal;
    fZ[1] = zLocal;
    fZ[2] = 0.;
  } else if (iOrder == 2) {
    // Second order interpolation in z.
    // Find the grid segment containing this point.
    int iGrid = 0;
    for (int i = 1; i < nz; i++) {
      if ((zAxis[i - 1] - z) * (z - zAxis[i]) >= 0.) {
        iGrid = i;
      }
    }
    // Compute the local coordinate for this grid segment.
    const double zLocal = (z - zAxis[iGrid - 1]) /
      (zAxis[iGrid] - zAxis[iGrid - 1]);
    // Set the summing range and shape functions.
    // These assignments are shared by all of the following conditions,
    // so it's easier to take them out.
    fZ[0] = (z           - zAxis[iZ0 + 1]) *
      (z           - zAxis[iZ0 + 2]) /
      ((zAxis[iZ0] - zAxis[iZ0 + 1]) *
       (zAxis[iZ0] - zAxis[iZ0 + 2]));
    fZ[1] = (z               - zAxis[iZ0]) *
      (z               - zAxis[iZ0 + 2]) /
      ((zAxis[iZ0 + 1] - zAxis[iZ0]) *
       (zAxis[iZ0 + 1] - zAxis[iZ0 + 2]));
    fZ[2] = (z               - zAxis[iZ0])*
      (z               - zAxis[iZ0 + 1]) /
      ((zAxis[iZ0 + 2] - zAxis[iZ0]) *
       (zAxis[iZ0 + 2] - zAxis[iZ0 + 1]));

    if (iGrid == 1) {
      iZ0 = iGrid - 1;
      iZ1 = iGrid + 1;
      if (zAxis[iZ0]     == zAxis[iZ0 + 1] ||
          zAxis[iZ0]     == zAxis[iZ0 + 2] ||
          zAxis[iZ0 + 1] == zAxis[iZ0 + 2]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in z coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fZ[0] = (z           - zAxis[iZ0 + 1]) *
	(z           - zAxis[iZ0 + 2]) /
	((zAxis[iZ0] - zAxis[iZ0 + 1]) *
	 (zAxis[iZ0] - zAxis[iZ0 + 2]));
      fZ[1] = (z               - zAxis[iZ0]) *
	(z               - zAxis[iZ0 + 2]) /
	((zAxis[iZ0 + 1] - zAxis[iZ0]) *
	 (zAxis[iZ0 + 1] - zAxis[iZ0 + 2]));
      fZ[2] = (z               - zAxis[iZ0])*
	(z               - zAxis[iZ0 + 1]) /
	((zAxis[iZ0 + 2] - zAxis[iZ0]) *
	 (zAxis[iZ0 + 2] - zAxis[iZ0 + 1]));
    } else if (iGrid == nz - 1) {
      iZ0 = iGrid - 2;
      iZ1 = iGrid;
      if (zAxis[iZ0]     == zAxis[iZ0 + 1] ||
          zAxis[iZ0]     == zAxis[iZ0 + 2] ||
          zAxis[iZ0 + 1] == zAxis[iZ0 + 2]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in z coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }
      fZ[0] = (z           - zAxis[iZ0 + 1]) *
	(z           - zAxis[iZ0 + 2]) /
	((zAxis[iZ0] - zAxis[iZ0 + 1]) *
	 (zAxis[iZ0] - zAxis[iZ0 + 2]));
      fZ[1] = (z               - zAxis[iZ0]) *
	(z               - zAxis[iZ0 + 2]) /
	((zAxis[iZ0 + 1] - zAxis[iZ0]) *
	 (zAxis[iZ0 + 1] - zAxis[iZ0 + 2]));
      fZ[2] = (z               - zAxis[iZ0])*
	(z               - zAxis[iZ0 + 1]) /
	((zAxis[iZ0 + 2] - zAxis[iZ0]) *
	 (zAxis[iZ0 + 2] - zAxis[iZ0 + 1]));
    } else {
      iZ0 = iGrid - 2;
      iZ1 = iGrid + 1;

      if (zAxis[iZ0]     == zAxis[iZ0 + 1] ||
          zAxis[iZ0]     == zAxis[iZ0 + 2] ||
          zAxis[iZ0]     == zAxis[iZ0 + 3] ||
          zAxis[iZ0 + 1] == zAxis[iZ0 + 2] ||
          zAxis[iZ0 + 1] == zAxis[iZ0 + 3] ||
          zAxis[iZ0 + 2] == zAxis[iZ0 + 3]) {
	msg << MSG::INFO << "Boxin3:\n";
	msg << MSG::INFO << "    One or more grid points in z coincide.\n";
	msg << MSG::INFO << "    No interpolation.\n";
        f = 0.;
        return false;
      }

      fZ[0] = (z           - zAxis[iZ0 + 1]) *
	(z           - zAxis[iZ0 + 2]) /
	((zAxis[iZ0] - zAxis[iZ0 + 1]) *
	 (zAxis[iZ0] - zAxis[iZ0 + 2]));
      fZ[1] = (z               - zAxis[iZ0]) *
	(z               - zAxis[iZ0 + 2]) /
	((zAxis[iZ0 + 1] - zAxis[iZ0]) *
	 (zAxis[iZ0 + 1] - zAxis[iZ0 + 2]));
      fZ[2] = (z               - zAxis[iZ0])*
	(z               - zAxis[iZ0 + 1]) /
	((zAxis[iZ0 + 2] - zAxis[iZ0]) *
	 (zAxis[iZ0 + 2] - zAxis[iZ0 + 1]));

      fZ[0] *= (1. - zLocal);
      fZ[1] = fZ[1] * (1. - zLocal) +
	zLocal * (z - zAxis[iZ0 + 2]) *
	(z - zAxis[iZ0 + 3]) /
	((zAxis[iZ0 + 1] - zAxis[iZ0 + 2]) *
	 (zAxis[iZ0 + 1] - zAxis[iZ0 + 3]));
      fZ[2] = fZ[2] * (1. - zLocal) +
	zLocal * (z - zAxis[iZ0 + 1]) *
	(z - zAxis[iZ0 + 3]) /
	((zAxis[iZ0 + 2] - zAxis[iZ0 + 1]) *
	 (zAxis[iZ0 + 2] - zAxis[iZ0 + 3]));
      fZ[3] = zLocal * (z - zAxis[iZ0 + 1]) *
	(z - zAxis[iZ0 + 2]) /
	((zAxis[iZ0 + 3] - zAxis[iZ0 + 1]) *
	 (zAxis[iZ0 + 3] - zAxis[iZ0 + 2]));
    }
  }

  f = 0.;
  for (int i = iX0; i <= iX1; ++i) {
    for (int j = iY0; j <= iY1; ++j) {
      for (int k = iZ0; k <= iZ1; ++k) {
        // std::cout << "i = " << i << ", j = " << j << ", k = " << k << "\n";
        // std::cout << "value: " << value[i][j][k] << "\n";
        // std::cout << "fX = " << fX[i - iX0] << ", fY = " << fY[j - iY0] << ", fZ = " << fZ[k - iZ0] << "\n";
        f += value[i][j][k] * fX[i - iX0] * fY[j - iY0] * fZ[k - iZ0];
      }
    }
  }
  // std::cout << f << std::endl;
  return true;

}


}

const int GarfieldGas::nMaxGases;

GarfieldGas::GarfieldGas() :
  name(""),
  temperature(293.15), pressure(760.),
  nComponents(1),
  nExcListElements(0), nIonListElements(0),
  nEfields(0), nBfields(0), nAngles(0),
  map2d(false),
  hasElectronVelocityE(false), hasElectronVelocityB(false), hasElectronVelocityExB(false),
  hasElectronDiffLong(false), hasElectronDiffTrans(false),
  extrLowVelocity(0), extrHighVelocity(0),
  extrLowDiffusion(0), extrHighDiffusion(0),
  extrLowTownsend(0), extrHighTownsend(0),
  extrLowAttachment(0), extrHighAttachment(0),
  extrLowMobility(0), extrHighMobility(0),
  extrLowDissociation(0), extrHighDissociation(0),
  intpVelocity(0),
  intpDiffusion(0),
  intpTownsend(0),
  intpAttachment(0),
  intpMobility(0),
  intpDissociation(0),
  pressureTable(0), temperatureTable(0)
{
  for (int i = nMaxGases; i--;) {
    fraction[i] = 0.;
    gas[i] = "";
  }
  gas[0] = "Ar";
  fraction[0] = 1.;

  nEfields = 0;
  nBfields = 1;
  nAngles  = 1;

  eFields.clear();
  bFields.clear(); bFields.resize(1); bFields[0] = 0.;
  bAngles.clear(); bAngles.resize(1); bAngles[0] = 0.;

  hasElectronVelocityE   = false; tabElectronVelocityE.clear();
  hasElectronVelocityB   = false; tabElectronVelocityB.clear();
  hasElectronVelocityExB = false; tabElectronVelocityExB.clear();
  hasElectronDiffLong    = false; tabElectronDiffLong.clear();
  hasElectronDiffTrans   = false; tabElectronDiffTrans.clear();

  extrLowVelocity     = 0; extrHighVelocity     = 1;
  extrLowDiffusion    = 0; extrHighDiffusion    = 1;

  intpVelocity     = 2;
  intpDiffusion    = 2;

}

bool
GarfieldGas::ElectronVelocity(const double ex, const double ey, const double ez,//cm/V
				const double bx, const double by, const double bz,//T
				double& vx, double& vy, double& vz) {//cm/ns ~4.7x10^-3

  Athena::MsgStreamMember msg("GarfieldGas::ElectronVelocity");
  vx = vy = vz = 0.;
  // Make sure there is at least a table of velocities along E.
  if (!hasElectronVelocityE) return false;

  // Compute the magnitude of the electric field.
  const double e = sqrt(ex * ex + ey * ey + ez * ez);
  const double e0 = ScaleElectricField(e);
  if (e < Small || e0 < Small) return false;

  // Compute the magnitude of the magnetic field.
  const double b = sqrt(bx * bx + by * by + bz * bz);

  // Compute the angle between B field and E field.
  double ebang = 0.;
  if (e * b > 0.) {
    const double eb = fabs(ex * bx + ey * by + ez * bz);
    if (eb > 0.2 * e * b) {
      ebang = asin(std::min(1.,
                            sqrt(pow(ex * by - ey * bx, 2) +
                                 pow(ex * bz - ez * bx, 2) +
                                 pow(ez * by - ey * bz, 2)) / (e * b)));
    } else {
      ebang = acos(std::min(1., eb / (e * b)));
    }
  } else {
    ebang = bAngles[0];
  }

  if (b < Small) {
    // No magnetic field.

    // Calculate the velocity along E.
    double ve = 0.;
    if (map2d) {
      if (!Numerics::Boxin3(tabElectronVelocityE,
                            bAngles, bFields, eFields,
                            nAngles, nBfields, nEfields,
                            ebang, b, e0, ve, intpVelocity)) {
	msg << MSG::INFO << "Interpolation of velocity along E failed.\n";
        return false;
      }
    } else {
      ve = Interpolate1D(e0, tabElectronVelocityE[0][0], eFields,
                         intpVelocity,
                         extrLowVelocity, extrHighVelocity);
    }
    const double q = -1.;
    const double mu = q * ve / e;
    vx = mu * ex;
    vy = mu * ey;
    vz = mu * ez;

  } else if (hasElectronVelocityB && hasElectronVelocityExB) {
    // Magnetic field, velocities along ExB and Bt available

    // Compute unit vectors along E, E x B and Bt.
    double ue[3] = {ex / e, ey / e, ez / e};
    double uexb[3] = {ey * bz - ez * by,
                      ez * bx - ex * bz,
                      ex * by - ey * bx};
    const double exb = sqrt(uexb[0] * uexb[0] +
                            uexb[1] * uexb[1] +
                            uexb[2] * uexb[2]);
    if (exb > 0.) {
      uexb[0] /= exb; uexb[1] /= exb; uexb[2] /= exb;
    } else {
      uexb[0] = ue[0];
      uexb[1] = ue[1];
      uexb[2] = ue[2];
    }

    double ubt[3] = {
      uexb[1] * ez - uexb[2] * ey,
      uexb[2] * ex - uexb[0] * ez,
      uexb[0] * ey - uexb[1] * ex
    };
    const double bt = sqrt(ubt[0] * ubt[0] +
                           ubt[1] * ubt[1] +
                           ubt[2] * ubt[2]);

    if (bt > 0.) {
      ubt[0] /= bt; ubt[1] /= bt; ubt[2] /= bt;
    } else {
      ubt[0] = ue[0];
      ubt[1] = ue[1];
      ubt[2] = ue[2];
    }

    // Calculate the velocities in all directions.
    double ve = 0., vbt = 0., vexb = 0.;
    if (map2d) {
      if (!Numerics::Boxin3(tabElectronVelocityE,
                            bAngles, bFields, eFields,
                            nAngles, nBfields, nEfields,
                            ebang, b, e0, ve, intpVelocity)) {
	msg << MSG::INFO << "Interpolation of velocity along E failed.\n";
        return false;
      }
      if (!Numerics::Boxin3(tabElectronVelocityExB,
                            bAngles, bFields, eFields,
                            nAngles, nBfields, nEfields,
                            ebang, b, e0, vexb, intpVelocity)) {
	msg << MSG::INFO << "Interpolation of velocity along ExB failed.\n";
        return false;
      }
      if (!Numerics::Boxin3(tabElectronVelocityB,
                            bAngles, bFields, eFields,
                            nAngles, nBfields, nEfields,
                            ebang, b, e0, vbt, intpVelocity)) {
	msg << MSG::INFO << "Interpolation of velocity along Bt failed.\n";
        return false;
      }
    } else {
      ve = Interpolate1D(e0, tabElectronVelocityE[0][0], eFields,
                         intpVelocity,
                         extrLowVelocity, extrHighVelocity);
      vbt = Interpolate1D(e0, tabElectronVelocityB[0][0], eFields,
                          intpVelocity,
                          extrLowVelocity, extrHighVelocity);
      vexb = Interpolate1D(e0, tabElectronVelocityExB[0][0], eFields,
                           intpVelocity,
                           extrLowVelocity, extrHighVelocity);
    }

    // fix by Y.Kataoka
    if(ex*bx+ey*by+ez*bz>0) vbt=fabs(vbt);
    else vbt=-fabs(vbt);

    const double q = -1.;
    vx = q * (ve * ue[0] + q * q * vbt * ubt[0] + q * vexb * uexb[0]);
    vy = q * (ve * ue[1] + q * q * vbt * ubt[1] + q * vexb * uexb[1]);
    vz = q * (ve * ue[2] + q * q * vbt * ubt[2] + q * vexb * uexb[2]);

  } else {
    // Magnetic field, velocities along ExB, Bt not available

    // Calculate the velocity along E.
    double ve = 0.;
    if (map2d) {
      if (!Numerics::Boxin3(tabElectronVelocityE,
                            bAngles, bFields, eFields,
                            nAngles, nBfields, nEfields,
                            ebang, b, e0, ve, intpVelocity)) {
	msg << MSG::INFO << "Interpolation of velocity along E failed.\n";
        return false;
      }
    } else {
      ve = Interpolate1D(e0, tabElectronVelocityE[0][0], eFields,
                         intpVelocity,
                         extrLowVelocity, extrHighVelocity);
    }

    const double q = -1.;
    const double mu = q * ve / e;
    const double eb = bx * ex + by * ey + bz * ez;
    const double nom = 1. + pow(mu * b, 2);
    vx = mu * (ex +
               mu * (ey * bz - ez * by) +
               mu * mu * bx * eb) / nom;
    vy = mu * (ey +
               mu * (ez * bx - ex * bz) +
               mu * mu * by * eb) / nom;
    vz = mu * (ez +
               mu * (ex * by - ey * bx) +
               mu * mu * bz * eb) / nom;
  }

  return true;

}



double
GarfieldGas::Interpolate1D(const double e,
			     const std::vector<double>& table,
			     const std::vector<double>& fields,
			     const int intpMeth,
			     const int extrLow, const int extrHigh) {

  // This function is a generalized version of the Fortran functions
  // GASVEL, GASVT1, GASVT2, GASLOR, GASMOB, GASDFT, and GASDFL
  // for the case of a 1D table. All variables are generic.

  const int nSizeTable = fields.size();

  if (e < 0. || nSizeTable < 1) return 0.;

  double result = 0.;

  if (nSizeTable == 1) {
    // Only one point
    result = table[0];
  } else if (e < fields[0]) {
    // Extrapolation towards small fields
    if (fields[0] >= fields[1]) {
       result = table[0];
    } else if (extrLow == 1) {
      // Linear extrapolation
      const double extr4 = (table[1] - table[0]) / (fields[1] - fields[0]);
      const double extr3 = table[0] - extr4 * fields[0];
      result = extr3 + extr4 * e;
    } else if (extrLow == 2) {
      // Logarithmic extrapolation
      const double extr4 = log(table[1] / table[0]) /
	(fields[1] - fields[0]);
      const double extr3 = log(table[0] - extr4 * fields[0]);
      result = std::exp(std::min(50., extr3 + extr4 * e));
    } else {
      result = table[0];
    }
  } else if (e > fields[nSizeTable - 1]) {
    // Extrapolation towards large fields
    if (fields[nSizeTable - 1] <= fields[nSizeTable - 2]) {
      result = table[nSizeTable - 1];
    } else if (extrHigh == 1) {
      // Linear extrapolation
      const double extr2 = (table[nSizeTable - 1] - table[nSizeTable - 2]) /
	(fields[nSizeTable - 1] - fields[nSizeTable - 2]);
      const double extr1 = table[nSizeTable - 1] -
	extr2 * fields[nSizeTable - 1];
      result = extr1 + extr2 * e;
    } else if (extrHigh == 2) {
      // Logarithmic extrapolation
      const double extr2 = log(table[nSizeTable - 1] /
                               table[nSizeTable - 2]) /
	(fields[nSizeTable - 1] - fields[nSizeTable - 2]);
      const double extr1 = log(table[nSizeTable - 1]) -
	extr2 * fields[nSizeTable - 1];
      result = exp(std::min(50., extr1 + extr2 * e));
    } else {
      result = table[nSizeTable - 1];
    }
  } else {
    // Intermediate points, spline interpolation (not implemented).
    // Intermediate points, Newtonian interpolation
    result = Numerics::Divdif(table, fields, nSizeTable, e, intpMeth);
  }

  return result;

}

void
GarfieldGas::InitParamArrays(const int eRes, const int bRes, const int aRes,
                        std::vector<std::vector<std::vector<double> > >& tab,
                        const double val) {
  Athena::MsgStreamMember msg("GarfieldGas::InitParamArrays");
  if (eRes <= 0 || bRes <= 0 || aRes <= 0) {
    msg << MSG::INFO << "Invalid grid.\n";
    return;
  }

  tab.resize(aRes);
  for (int i = aRes; i--;) {
    tab[i].resize(bRes);
    for (int j = bRes; j--;) {
      tab[i][j].resize(eRes);
      for (int k = eRes; k--;) {
        tab[i][j][k] = val;
      }
    }
  }

}


bool GarfieldGas::LoadGasFile(const std::string filename) {
  Athena::MsgStreamMember msg("GarfieldGas::loadGasFile");

  std::ifstream gasfile;
  // Open the file.
  gasfile.open(filename.c_str());
  // Make sure the file could be opened.
  if (!gasfile.is_open()) {
    msg << MSG::INFO << "Gas file could not be opened.\n";
    return false;
  }

  char line[256];
  char* token;

  // GASOK bits
  std::string gasBits = "";

  // Gas composition
  const int nMagboltzGases = 60;
  std::vector<double> mixture(nMagboltzGases);
  for (int i = nMagboltzGases; i--;) mixture[i] = 0.;


  int eFieldRes = 1;
  int bFieldRes = 1;
  int angRes = 1;

  int versionNumber = 12;

  // Start reading the data.
  bool atTables = false;
  while (!atTables) {
    gasfile.getline(line, 256);
    if (strncmp(line, " The gas tables follow:", 8) == 0 ||
        strncmp(line, "The gas tables follow:", 7) == 0) {
      atTables = true;
    }
    if (!atTables) {
      token = strtok(line, " :,%");
      while (token != NULL) {
        if (strcmp(token, "Version") == 0) {
          token = strtok(NULL, " :,%");
          versionNumber = atoi(token);
          // Check the version number.
          if (versionNumber != 10 &&
              versionNumber != 11 &&
              versionNumber != 12) {
	    msg << MSG::INFO << "The file has version number "
                      << versionNumber << ".\n";
	    msg << MSG::INFO << "    Files written in this format cannot be read.\n";
            gasfile.close();
            return false;
          } else {
	    msg << MSG::DEBUG << "Version: " << versionNumber << "\n";
          }
        } else if (strcmp(token, "GASOK") == 0) {
          // Get the GASOK bits indicating if a parameter
          // is present in the table (T) or not (F).
          token = strtok(NULL, " :,%\t");
          token = strtok(NULL, " :,%\t");
          gasBits += token;
        } else if (strcmp(token, "Identifier") == 0) {
          // Get the identification string.
	  std::string identifier = "";
          token = strtok(NULL, "\n");
          if (token != NULL) identifier += token;
        } else if (strcmp(token, "Dimension") == 0) {
          token = strtok(NULL, " :,%\t");
          if (strcmp(token, "F") == 0) {
            map2d = false;
          } else {
            map2d = true;
          }
          token = strtok(NULL, " :,%\t");
          eFieldRes = atoi(token);
          // Check the number of E points.
          if (eFieldRes <= 0) {
	    msg << MSG::INFO << "Number of E fields out of range.\n";
            gasfile.close();
            return false;
          }
          token = strtok(NULL, " :,%\t");
          angRes = atoi(token);
          // Check the number of angles.
          if (map2d && angRes <= 0) {
	    msg << MSG::INFO << "Number of E-B angles out of range.\n";
            gasfile.close();
            return false;
          }

          token = strtok(NULL, " :,%\t");
          bFieldRes = atoi(token);
          // Check the number of B points.
          if (map2d && bFieldRes <= 0) {
	    msg << MSG::INFO << "Number of B fields out of range.\n";
            gasfile.close();
            return false;
          }

          eFields.resize(eFieldRes);
          nEfields = eFieldRes;
          bFields.resize(bFieldRes);
          nBfields = bFieldRes;
          bAngles.resize(angRes);
          nAngles = angRes;

          // Fill in the excitation/ionisation structs
          // Excitation
          token = strtok(NULL, " :,%\t");
          int nexc = atoi(token);
          if (nexc >= 0) nExcListElements = nexc;

          // Ionization
          token = strtok(NULL, " :,%\t");
          int nion = atoi(token);
          if (nion >= 0) nIonListElements = nion;

        } else if (strcmp(token, "E") == 0) {
          token = strtok(NULL, " :,%");
          if (strcmp(token, "fields") == 0) {
            for (int i = 0; i < eFieldRes; i++) {
              gasfile >> eFields[i];
            }
          }
        }  else if (strcmp(token, "E-B") == 0) {
          token = strtok(NULL, " :,%");
          if (strcmp(token, "angles") == 0) {
            for (int i = 0; i < angRes; i++) {
              gasfile >> bAngles[i];
            }
          }
        } else if (strcmp(token, "B") == 0) {
          token = strtok(NULL, " :,%");
          if (strcmp(token, "fields") == 0) {
            double bstore = 0.;
            for (int i = 0; i < bFieldRes; i++) {
              // B fields are stored in hGauss (to be checked!).
              gasfile >> bstore;
              bFields[i] = bstore / 100.;
            }
          }
        } else if (strcmp(token, "Mixture") == 0) {
          for (int i = 0; i < nMagboltzGases; ++i) {
            gasfile >> mixture[i];
          }
        }
        token = strtok(NULL, " :,%");
      }
    }
  }

  // Decode the GASOK bits.
  // GASOK(I)   : .TRUE. if present
  // (1)  electron drift velocity || E
  // (2)  ion mobility,
  // (3)  longitudinal diffusion || E
  // (4)  Townsend coefficient,
  // (5)  cluster size distribution.
  // (6)  attachment coefficient,
  // (7)  Lorentz angle,
  // (8)  transverse diffusion || ExB and Bt
  // (9)  electron drift velocity || Bt
  // (10) electron drift velocity || ExB
  // (11) diffusion tensor
  // (12) ion dissociation
  // (13) allocated for SRIM data (not used)
  // (14) allocated for HEED data (not used)
  // (15) excitation rates
  // (16) ionisation rates


  if (gasBits[0] == 'T') {
    hasElectronVelocityE = true;
    InitParamArrays(eFieldRes, bFieldRes, angRes,
                    tabElectronVelocityE, 0.);
  } else {
    hasElectronVelocityE = false;
    tabElectronVelocityE.clear();
  }
  if (gasBits[2] == 'T') {
    hasElectronDiffLong = true;
    InitParamArrays(eFieldRes, bFieldRes, angRes,
                    tabElectronDiffLong, 0.);
  } else {
    hasElectronDiffLong = false;
    tabElectronDiffLong.clear();
  }
  if (gasBits[7] == 'T') {
    hasElectronDiffTrans = true;
    InitParamArrays(eFieldRes, bFieldRes, angRes,
                    tabElectronDiffTrans, 0.);
  } else {
    hasElectronDiffTrans = false;
    tabElectronDiffTrans.clear();
  }
  if (gasBits[8] == 'T') {
    hasElectronVelocityB = true;
    InitParamArrays(eFieldRes, bFieldRes, angRes,
                    tabElectronVelocityB, 0.);
  } else {
    hasElectronVelocityB = false;
    tabElectronVelocityB.clear();
  }
  if (gasBits[9] == 'T') {
    hasElectronVelocityExB = true;
    InitParamArrays(eFieldRes, bFieldRes, angRes,
                    tabElectronVelocityExB, 0.);
  } else {
    hasElectronVelocityExB = false;
    tabElectronVelocityExB.clear();
  }


  // Check the gas mixture.
  std::vector<std::string> gasnames; gasnames.clear();
  std::vector<double> percentages;   percentages.clear();
  bool gasMixOk = true;
  int gasCount = 0;
  for (int i = 0; i < nMagboltzGases; ++i) {
    if (mixture[i] > 0.) {
      std::string gasname = "";
      if (!GetGasName(i + 1, versionNumber, gasname)) {
	msg << MSG::INFO << "Unknown gas (gas number ";
	msg << MSG::INFO << i + 1 << ")\n";
        gasMixOk = false;
        break;
      }
      gasnames.push_back(gasname);
      percentages.push_back(mixture[i]);
      ++gasCount;
    }
  }
  if (gasCount > nMaxGases) {
    msg << MSG::INFO << "Gas mixture has " << gasCount << " components.\n";
    msg << MSG::INFO << "    Number of gases is limited to " << nMaxGases << ".\n";
    gasMixOk = false;
  } else if (gasCount <= 0) {
    msg << MSG::INFO << "Gas mixture is not defined (zero components).\n";
    gasMixOk = false;
  }
  double sum = 0.;
  for (int i = 0; i < gasCount; ++i) sum += percentages[i];
  if (gasMixOk && sum != 100.) {
    msg << MSG::INFO << "Percentages are not normalized to 100.\n";
    for (int i = 0; i < gasCount; ++i) percentages[i] *= 100. / sum;
  }

  if (gasMixOk) {
    name = "";
    nComponents = gasCount;
    for (int i = 0; i < nComponents; ++i) {
      if (i > 0) name += "/";
      name += gasnames[i];
      gas[i] = gasnames[i];
      fraction[i] = percentages[i] / 100.;
    }
    msg << MSG::DEBUG << "Gas composition set to " << name;
    if (nComponents > 1) {
      msg << MSG::DEBUG << " (" << fraction[0] * 100;
      for (int i = 1; i < nComponents; ++i) {
	msg << MSG::DEBUG << "/" << fraction[i] * 100;
      }
      msg << MSG::DEBUG << ")";
    }
    msg << MSG::DEBUG << "\n";
  } else {
    msg << MSG::INFO << "Gas composition could not be established.\n";
  }

  // Temporary variables
  // Velocities
  double ve = 0., vb = 0., vexb = 0.;
  // Lorentz angle
  double lor = 0.;
  // Diffusion coefficients
  double dl = 0., dt = 0.;
  // Towsend and attachment coefficients
  double alpha = 0., alpha0 = 0., eta = 0.;
  // Ion mobility and dissociation coefficient
  double mu = 0., diss = 0.;
  double diff = 0.;
  double rate = 0.;
  double waste = 0.;

  (void)lor; (void)alpha; (void)alpha0; (void)eta; (void)mu; (void)diss; (void)diff; (void)rate; (void)waste;//to suppress unused warnings

  if (map2d) {
    for (int i = 0; i < eFieldRes; i++) {
      for (int j = 0; j < angRes; j++) {
        for (int k = 0; k < bFieldRes; k++) {
          // Drift velocity along E, Bt and ExB
          gasfile >> ve >> vb >> vexb;
          // Convert from cm / us to cm / ns
          ve *= 1.e-3; vb *= 1.e-3; vexb *= 1.e-3;
          if (hasElectronVelocityE) tabElectronVelocityE[j][k][i] = ve;
          if (hasElectronVelocityB) tabElectronVelocityB[j][k][i] = vb;
          if (hasElectronVelocityExB) tabElectronVelocityExB[j][k][i] = vexb;
          // Longitudinal and transverse diffusion coefficient
          gasfile >> dl >> dt;
          if (hasElectronDiffLong) tabElectronDiffLong[j][k][i] = dl;
          if (hasElectronDiffTrans) tabElectronDiffTrans[j][k][i] = dt;
          // Townsend and attachment coefficient
          gasfile >> alpha >> alpha0 >> eta;
          // Ion mobility
          gasfile >> mu;
          // Convert from cm2 / (V us) to cm2 / (V ns)
          mu *= 1.e-3;
          // Lorentz angle (unused)
          gasfile >> lor;
          // Ion dissociation
          gasfile >> diss;
          // Diffusion tensor
          for (int l = 0; l < 6; l++) {
            gasfile >> diff;
          }
          // Excitation rates
          for (int l = 0; l < nExcListElements; l++) {
            gasfile >> rate;
          }
          // Ionization rates
          for (int l = 0; l < nIonListElements; l++) {
            gasfile >> rate;
          }
        }
      }
    }
  } else {
    for (int i = 0; i < eFieldRes; i++) {
      // Drift velocity along E, Bt, ExB
      gasfile >> ve >> waste >> vb >> waste >> vexb >> waste;
      ve *= 1.e-3; vb *= 1.e-3; vexb *= 1.e-3;
      if (hasElectronVelocityE)   tabElectronVelocityE[0][0][i] = ve;
      if (hasElectronVelocityB)   tabElectronVelocityB[0][0][i] = vb;
      if (hasElectronVelocityExB) tabElectronVelocityExB[0][0][i] = vexb;
      // Longitudinal and transferse diffusion coefficients
      gasfile >> dl >> waste >> dt >> waste;
      if (hasElectronDiffLong)  tabElectronDiffLong[0][0][i] = dl;
      if (hasElectronDiffTrans) tabElectronDiffTrans[0][0][i] = dt;
      // Townsend and attachment coefficients
      gasfile >> alpha >> waste >> alpha0 >> eta >> waste;
      // Ion mobility
      gasfile >> mu >> waste;
      mu *= 1.e-3;
      // Lorentz angle (unused)
      gasfile >> lor >> waste;
      // Ion dissociation
      gasfile >> diss >> waste;
      // Diffusion tensor
      for (int j = 0; j < 6; j++) {
        gasfile >> diff >> waste;
      }
      // Excitation rates
      for (int j = 0; j < nExcListElements; j++) {
	gasfile >> rate >> waste;
      }
      // Ionization rates
      for (int j = 0; j < nIonListElements; j++) {
	gasfile >> rate >> waste;
      }
    }
  }


  // Extrapolation methods
  int hExtrap[13], lExtrap[13];
  // Interpolation methods
  int interpMeth[13];

  // Moving on to the file footer
  bool done = false;
  while (!done) {
    gasfile.getline(line, 256);
    token = strtok(line, " :,%=\t");
    while (token != NULL) {
      if (strcmp(token, "H") == 0) {
	token = strtok(NULL, " :,%=\t");
	for (int i = 0; i < 13; i++) {
	  token = strtok(NULL, " :,%=\t");
	  if (token != NULL) hExtrap[i] = atoi(token);
	}
      } else if (strcmp(token, "L") == 0) {
	token = strtok(NULL, " :,%=\t");
	for (int i = 0; i < 13; i++) {
	  token = strtok(NULL, " :,%=\t");
	  if (token != NULL) lExtrap[i] = atoi(token);
	}
      } else if (strcmp(token, "Thresholds") == 0) {
	token = strtok(NULL, " :,%=\t");
	token = strtok(NULL, " :,%=\t");
	token = strtok(NULL, " :,%=\t");
      } else if (strcmp(token, "Interp") == 0) {
	for (int i = 0; i < 13; i++) {
	  token = strtok(NULL, " :,%=\t");
	  if (token != NULL) interpMeth[i] = atoi(token);
	}
      } else if(strcmp(token, "A") == 0) {
	token = strtok(NULL, " :,%=\t");
	// Parameter for energy loss distribution, currently not used
	// double a;
	// if (token != NULL) a = atof(token);
      } else if (strcmp(token, "Z") == 0) {
	// Parameter for energy loss distribution, currently not used
	token = strtok(NULL, " :,%=\t");
	// double z;
	// if (token != NULL) z = atof(token);
      } else if (strcmp(token, "EMPROB") == 0) {
	// Parameter for energy loss distribution, currently not used
	token = strtok(NULL, " :,%=\t");
	// double emprob;
	// if (token != NULL) emprob = atof(token);
      } else if (strcmp(token, "EPAIR") == 0) {
	// Parameter for energy loss distribution, currently not used
	token = strtok(NULL, " :,%=\t");
	// double epair;
	// if (token != NULL) epair = atof(token);
      } else if (strcmp(token, "Ion") == 0) {
	// Ion diffusion coefficients
	token = strtok(NULL, " :,%=\t");
	token = strtok(NULL, " :,%=\t");
	token = strtok(NULL, " :,%=\t");
      } else if (strcmp(token, "CMEAN") == 0) {
	// Cluster parameter, currently not used
	token = strtok(NULL, " :,%=\t");
	// double clsPerCm;
	// if (token != NULL) clsPerCm = atof(token);
      } else if (strcmp(token, "RHO") == 0) {
	// Parameter for energy loss distribution, currently not used
	token = strtok(NULL, " :,%=\t");
	// double rho;
	// if (token != NULL) rho = atof(token);
      } else if (strcmp(token, "PGAS") == 0) {
	token = strtok(NULL, " :,%=\t");
	double pTorr = 760.;
	if (token != NULL) pTorr = atof(token);
	if (pTorr > 0.) pressure = pTorr;
      } else if (strcmp(token, "TGAS") == 0) {
	token = strtok(NULL, " :,%=\t");
	double tKelvin = 293.15;
	if (token != NULL) tKelvin = atof(token);
	if (tKelvin > 0.) temperature = tKelvin;
	done = true;
	break;
      } else {
	done = true;
	break;
      }
      (void)token; // to suppress unused warning
      token = strtok(NULL, " :,%=\t");
    }
  }
  gasfile.close();

  // Set the reference pressure and temperature.
  pressureTable = pressure;
  temperatureTable = temperature;

  // Multiply the E/p values by the pressure.
  for (int i = eFieldRes; i--;) {
    eFields[i] *= pressureTable;
  }

  // Decode the extrapolation and interpolation tables.
  extrHighVelocity = hExtrap[0];
  extrLowVelocity  = lExtrap[0];
  intpVelocity     = interpMeth[0];
  // Indices 1 and 2 correspond to velocities along Bt and ExB.
  extrHighDiffusion = hExtrap[3];
  extrLowDiffusion  = lExtrap[3];
  intpDiffusion     = interpMeth[3];

  return true;
}


bool
GarfieldGas::GetGasName(const int gasnumber, const int version,
			  std::string& gasname) {

  switch (gasnumber) {
  case 1:
    gasname = "CF4";
    break;
  case 2:
    gasname = "Ar";
    break;
  case 3:
    gasname = "He";
    break;
  case 4:
    gasname = "He-3";
    break;
  case 5:
    gasname = "Ne";
    break;
  case 6:
    gasname = "Kr";
    break;
  case 7:
    gasname = "Xe";
    break;
  case 8:
    gasname = "CH4";
    break;
  case 9:
    gasname = "C2H6";
    break;
  case 10:
    gasname = "C3H8";
    break;
  case 11:
    gasname = "iC4H10";
    break;
  case 12:
    gasname = "CO2";
    break;
  case 13:
    gasname = "neoC5H12";
    break;
  case 14:
    gasname = "H2O";
    break;
  case 15:
    gasname = "O2";
    break;
  case 16:
    gasname = "N2";
    break;
  case 17:
    gasname = "NO";
    break;
  case 18:
    gasname = "N2O";
    break;
  case 19:
    gasname = "C2H4";
    break;
  case 20:
    gasname = "C2H2";
    break;
  case 21:
    gasname = "H2";
    break;
  case 22:
    gasname = "D2";
    break;
  case 23:
    gasname = "CO";
    break;
  case 24:
    gasname = "Methylal";
    break;
  case 25:
    gasname = "DME";
    break;
  case 26:
    gasname = "Reid-Step";
    break;
  case 27:
    gasname = "Maxwell-Model";
    break;
  case 28:
    gasname = "Reid-Ramp";
    break;
  case 29:
    gasname = "C2F6";
    break;
  case 30:
    gasname = "SF6";
    break;
  case 31:
    gasname = "NH3";
    break;
  case 32:
    gasname = "C3H6";
    break;
  case 33:
    gasname = "cC3H6";
    break;
  case 34:
    gasname = "CH3OH";
    break;
  case 35:
    gasname = "C2H5OH";
    break;
  case 36:
    gasname = "C3H7OH";
    break;
  case 37:
    gasname = "Cs";
    break;
  case 38:
    gasname = "F2";
    break;
  case 39:
    gasname = "CS2";
    break;
  case 40:
    gasname = "COS";
    break;
  case 41:
    gasname = "CD4";
    break;
  case 42:
    gasname = "BF3";
    break;
  case 43:
    gasname = "C2H2F4";
    break;
  case 44:
    if (version <= 11) {
      gasname = "He-3";
    } else {
      gasname = "TMA";
    }
    break;
  case 45:
    gasname = "He";
    break;
  case 46:
    gasname = "Ne";
    break;
  case 47:
    gasname = "Ar";
    break;
  case 48:
    gasname = "Kr";
    break;
  case 49:
    gasname = "Xe";
    break;
  case 50:
    gasname = "CHF3";
    break;
  case 51:
    gasname = "CF3Br";
    break;
  case 52:
    gasname = "C3F8";
    break;
  case 53:
    gasname = "O3";
    break;
  case 54:
    gasname = "Hg";
    break;
  case 55:
    gasname = "H2S";
    break;
  case 56:
    gasname = "nC4H10";
    break;
  case 57:
    gasname = "nC5H12";
    break;
  case 58:
    gasname = "N2";
    break;
  case 59:
    gasname = "GeH4";
    break;
  case 60:
    gasname = "SiH4";
    break;
  default:
    gasname = "";
    return false;
    break;
  }
  return true;

}


void
GarfieldGas::PrintGas() {
  Athena::MsgStreamMember msg("GarfieldGas::PrintGas");

  // Print a summary.
  msg << MSG::INFO << "Gas composition: " << name;
  if (nComponents > 1) {
    msg << MSG::INFO << " (" << fraction[0] * 100;
    for (int i = 1; i < nComponents; ++i) {
      msg << MSG::INFO << "/" << fraction[i] * 100;
    }
    msg << MSG::INFO << ")";
  }
  msg << MSG::INFO << "\n";
  msg << MSG::INFO << "    Pressure:    " << pressure << " Torr\n";
  msg << MSG::INFO << "    Temperature: " << temperature << " K\n";
  msg << MSG::INFO << "    Gas file:\n";
  msg << MSG::INFO << "      Pressure:    " << pressureTable << " Torr\n";
  msg << MSG::INFO << "      Temperature: " << temperatureTable << " K\n";
  if (nEfields > 1) {
    msg << MSG::INFO << "    Electric field range:  " << eFields[0]
              << " - " << eFields[nEfields - 1]
              << " V/cm in " << nEfields  - 1 << " steps.\n";
  } else if (nEfields == 1) {
    msg << MSG::INFO << "    Electric field:        " << eFields[0] << " V/cm\n";
  } else {
    msg << MSG::INFO << "    Electric field range: not set\n";
  }
  if (nBfields > 1) {
    msg << MSG::INFO << "    Magnetic field range:  " << bFields[0]
              << " - " << bFields[nBfields - 1]
              << " T in " << nBfields - 1 << " steps.\n";
  } else if (nBfields == 1) {
    msg << MSG::INFO << "    Magnetic field:        " << bFields[0] << "\n";
  } else {
    msg << MSG::INFO << "    Magnetic field range: not set\n";
  }
  if (nAngles > 1) {
    msg << MSG::INFO << "    Angular range:         " << bAngles[0]
              << " - " << bAngles[nAngles - 1]
              << " in " << nAngles - 1 << " steps.\n";
  } else if (nAngles == 1) {
    msg << MSG::INFO << "    Angle between E and B: " << bAngles[0] << "\n";
  } else {
    msg << MSG::INFO << "    Angular range: not set\n";
  }

  msg << MSG::INFO << "    Available electron transport data:\n";
  if (hasElectronVelocityE) {
    msg << MSG::INFO << "      Velocity along E\n";
  }
  if (hasElectronVelocityB) {
    msg << MSG::INFO << "      Velocity along Bt\n";
  }
  if (hasElectronVelocityExB) {
    msg << MSG::INFO << "      Velocity along ExB\n";
  }
  if (hasElectronVelocityE || hasElectronVelocityB ||
      hasElectronVelocityExB) {
    msg << MSG::INFO << "        Low field extrapolation:  ";
    if (extrLowVelocity == 0) msg << MSG::INFO << " constant\n";
    else if (extrLowVelocity == 1) msg << MSG::INFO << " linear\n";
    else if (extrLowVelocity == 2) msg << MSG::INFO << " exponential\n";
    else msg << MSG::INFO << " unknown\n";
    msg << MSG::INFO << "        High field extrapolation: ";
    if (extrHighVelocity == 0) msg << MSG::INFO << " constant\n";
    else if (extrHighVelocity == 1) msg << MSG::INFO << " linear\n";
    else if (extrHighVelocity == 2) msg << MSG::INFO << " exponential\n";
    else msg << MSG::INFO << " unknown\n";
    msg << MSG::INFO << "        Interpolation order: " << intpVelocity << "\n";
  }
  if (hasElectronDiffLong) {
    msg << MSG::INFO << "      Longitudinal diffusion coefficient\n";
  }
  if (hasElectronDiffTrans) {
    msg << MSG::INFO << "      Transverse diffusion coefficient\n";
  }
  if (hasElectronDiffLong || hasElectronDiffTrans){
    msg << MSG::INFO << "        Low field extrapolation:  ";
    if (extrLowDiffusion == 0) msg << MSG::INFO << " constant\n";
    else if (extrLowDiffusion == 1) msg << MSG::INFO << " linear\n";
    else if (extrLowDiffusion == 2) msg << MSG::INFO << " exponential\n";
    else msg << MSG::INFO << " unknown\n";
    msg << MSG::INFO << "        High field extrapolation: ";
    if (extrHighDiffusion == 0) msg << MSG::INFO << " constant\n";
    else if (extrHighDiffusion == 1) msg << MSG::INFO << " linear\n";
    else if (extrHighDiffusion == 2) msg << MSG::INFO << " exponential\n";
    else msg << MSG::INFO << " unknown\n";
    msg << MSG::INFO << "        Interpolation order: " << intpDiffusion << "\n";
  }


}



