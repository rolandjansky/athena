/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/src/sincosf.cxx
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Jan, 2010
 * @brief Implementation of sincosf function for MacOSX
 */

#include "CxxUtils/sincosf.h"
#include <cmath>

#ifdef __APPLE__
void sincosf(float x, float* sn, float* cs)
{
	*sn = std::sin(x);
	*cs = std::cos(x);
}
#endif
