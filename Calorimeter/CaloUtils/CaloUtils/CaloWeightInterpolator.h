/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOWEIGHTINTERPOLATOR_H
#define CALOUTILS_CALOWEIGHTINTERPOLATOR_H

/**
 * @struct CaloWeightInterpolator
 * @version \$Id: CaloWeightInterpolator.h,v 1.0 2009-06-12 09:09:14 gunal Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 12-June-2009
 * @brief calculates an interpolated weight in n dimensions
 *
 * helper struct to interpolate the 2^n weights w in n-dimensions by
 * 2^n-1 successive linear interpolations. The order of the vector w
 * is by dimension - i.e. the first dimension changes in every element
 * of w, the second every 2, the 3rd every 4, and so on. The vector x
 * is supposed to carry the n relative positions within the current
 * bin.
 */

#include <vector>

struct CaloWeightInterpolator 
{
  static double getWeight( std::vector<double> & w, 
			   std::vector<double> & x);
};
#endif
