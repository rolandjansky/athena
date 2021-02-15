// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  interpolate.h
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Polynomial interpolation in a table.
 *
 * This function performs polynomial interpolation within a table.
 */

#ifndef CALOCLUSTERCORRECTION_INTERPOLATE_H
#define CALOCLUSTERCORRECTION_INTERPOLATE_H


#include "CaloConditions/Array.h"



namespace CaloClusterCorr {


/**
 * @brief Polynomial interpolation in a table.
 * @param a Interpolation table.
 *          The x values are in the first column (column 0),
 *          and by default, the y values are in the second column (column 1).
 *          The x values must be in ascending order, with no duplicates.
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
		   unsigned int ycol = 1,
                   const CaloRec::Array<1>& regions = CaloRec::Array<1>(),
                   int n_points = -1,
                   bool fixZero = false);


} // namespace CaloClusterCorr



#endif // not CALOCLUSTERCORRECTION_INTERPOLATE_H
