/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  interpolate.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Polynomial interpolation in a table.
 */


#include "CaloClusterCorrection/interpolate.h"
#include <vector>
#include <cassert>
#include <cmath>
#include <algorithm>


namespace {


// Test to see if two numbers are significantly different.
bool is_different (float a, float b)
{
  float den = std::abs (a) + std::abs (b);
  if (den == 0) return false;
  return std::abs ((a-b)/den) > 1e-6;
}


// Compare the first column of an array with an element.
struct xcompare
{
  bool operator() (const CaloRec::Array<1>& a, CaloRec::Arrayelt b)
  {
    return a[0] < b;
  }
};


} // anonymous namespace.


namespace CaloClusterCorr {


/**
 * @brief Polynomial interpolation in a table.
 * @param a Interpolation table.
 *          The x values are in the first column (column 0),
 *          and by default, the y values are in the second column (column 1).
 * @param x The value to interpolate.
 * @param degree The degree of the interpolating polynomial.
 * @param ycol The column number of the y values.  (0-based.)
 * @param regions Sometimes, you want to divide the range being interpolated
 *                into several distinct regions, where the interpolated
 *                function may be discontinuous at the boundaries
 *                between regions.  To do this, supply this argument,
 *                which should contain a list (in ascending order)
 *                of the x-coordinates of the region boundaries.
 *                When the interpolation runs against a boundary,
 *                the algorithm will add a copy of the last point
 *                before the boundary, positioned at the boundary.
 *                This helps to control runaway extrapolation
 *                leading up to the boundary.
 * @param n_points The number of interpolation points in the table to use.
 *                 If -1, then the entire table is used.
 * @param fixZero If true, remove zeros among y-values by averaging adjacent
 *                points.
 * @return The interpolated value.
 *
 * The method used is Newtonian interpolation.
 * Based on the cernlib routine divdif.
 */
float interpolate (const CaloRec::Array<2>& a,
		   float x,
		   unsigned int degree,
		   unsigned int ycol /*= 1*/,
                   const CaloRec::Array<1>& regions /*= CaloRec::Array<1>()*/,
                   int n_points /*= -1*/,
                   bool fixZero /*= false*/)
{
  const int xcol = 0;

  // Check arguments.
  if (n_points < 0)
    n_points = static_cast<int> (a.size());
  assert (n_points >= 2 && degree >= 1);
  degree = std::min (degree, static_cast<unsigned int> (n_points) - 1);

  // Find subscripts of the input value in the input arrays.
  unsigned int ix = std::lower_bound (a.begin(), a.begin()+n_points, x,
                                      xcompare()) -
                      a.begin();
  unsigned int ir = std::lower_bound (regions.begin(), regions.end(), x) -
                      regions.begin();

  // Number of points to try for.
  // Either degree+1 or degree+2, whichever is even,
  // to give the same number of points on each side.
  // If we run up against an edge or a boundary, we'll
  // fall back to using just degree+1 points (or fewer if we can't
  // even get that many).
  // If we end up using degree+2 points, we'll do two interpolations
  // of degree degree and average them.
  unsigned int npts = degree + 2 - (degree%2);

  // If we run up against the edge of a region boundary,
  // we'll want to add a pseudopoint right at the boundary
  // (copying the point closest to the boundary) instead of the
  // point farthest away from it.
  bool extralo = false;
  bool extrahi = false;

  // Starting point index, not considering edges or boundaries.
  int ilo = ix - npts/2;

  // Make sure this point is within the array range and has not
  // crossed a region boundary.
  if (ilo < 0) {
    ilo = 0;
    npts = degree+1;
  }
  while (ilo < n_points &&
         ir > 0 && a[ilo][xcol] < regions[ir-1])
  {
    ++ilo;
    npts = degree+1;
    extralo = true;
  }

  // Same deal for the right hand edge.
  // ihi is one past the last point to use.
  bool himoved = false;
  int ihi = ilo + npts;
  if (ihi > n_points) {
    ihi = n_points;
    npts = degree+1;
    himoved = true;
  }
  while (ihi > 0 && ir < regions.size() && a[ihi-1][xcol] >= regions[ir]) {
    --ihi;
    npts = degree+1;
    himoved = true;
    extrahi = true;
  }

  // The adjustment due to ihi may have moved the low point of the range
  // back over a boundary.  If so, we lose points from the fit.
  bool lomoved = false;
  ilo = ihi - npts;
  if (ilo < 0) {
    ilo = 0;
    lomoved = true;
  }
  while (ilo < n_points &&
         ir > 0 && a[ilo][xcol] < regions[ir-1])
  {
    ++ilo;
    extralo = true;
    lomoved = true;
  }

  // Copy the points we're going to use to arrays t and d.
  // t gets the x coordinates, d gets the y coordinates.
  // Note that the order doesn't matter.
  // Reserve two extra points in case we end up adding some.
  npts = ihi - ilo;
  std::vector<float> t;
  t.reserve (npts+2);
  std::vector<float> d;
  d.reserve (npts+2);

  // Add the pseudopoints, if needed, removing a point from the other end.
  // Be careful not to duplicate a point if there's already one
  // right at the boundary.
  if (extralo && is_different (a[ilo][xcol], regions[ir-1])) {
    if (!himoved)
      --ihi;
    else
      ++npts;
    t.push_back (regions[ir-1]);
    d.push_back (a[ilo][ycol]);
  }
  if (extrahi && is_different (a[ihi-1][xcol], regions[ir])) {
    if (!lomoved)
      ++ilo;
    else
      ++npts;
    t.push_back (regions[ir]);
    d.push_back (a[ihi-1][ycol]);
  }

  // Add the rest if the points.
  for (; ilo < ihi; ++ilo) {
    t.push_back (a[ilo][xcol]);
    d.push_back (a[ilo][ycol]);
  }

  // Now figure out the interpolation degree we're really going to use.
  assert (t.size() == npts);
  assert (d.size() == npts);
  degree = std::min (degree, npts-1);

  // Option to remove zeros in the interpolation table, by averaging
  // adjacent points.  Used to handle a couple bad points in the rfac-v5
  // table.
  if (fixZero) {
    for (size_t i = 1; i < npts-1; i++) {
      if (d[i] == 0) {
        d[i] = (d[i-1] + d[i+1])/2;
      }
    }
  }

  // True if we're averaging together two interpolations
  // (to get a symmetric range).
  bool extra = npts != degree+1;

  // If we're averaging two interpolations, we need to be sure the
  // two extreme points are at the end of the table.
  // (If extra is true, extrahi and extralo must be false.)
  if (extra) {
    std::swap (t[0], t[npts-2]);
    std::swap (d[0], d[npts-2]);
  }

  // Here we start the actual interpolation.
  // Replace d by the leading diagonal of a divided-difference table,
  // supplemented by an extra line if extra is true.
  for (unsigned int l=0; l<degree; l++) {
    if (extra)
      d[degree+1] = (d[degree+1]-d[degree-1])/(t[degree+1]-t[degree-1-l]);
    for (unsigned int i = degree; i > l; --i)
      d[i] = (d[i]-d[i-1])/(t[i]-t[i-1-l]);
  }

  // Evaluate the Newton interpolation formula at x, averaging
  // two values of the last difference if extra is true.
  float sum = d[degree];
  if (extra)
    sum = 0.5*(sum+d[degree+1]);
  for (int j = degree-1; j >= 0; --j)
    sum = d[j] + (x - t[j]) * sum;

  return sum;
}


} // namespace CaloClusterCorrection

