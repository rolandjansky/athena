/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ----------------------------------------------------------------
 * This is a helper function that provides the suffix for ECF and 
 * ECF ratio decorations.  No suffix is used for beta = 1.0 and
 * other values are given a suffix of _BetaN where N is the value
 * with decimal points removed.  This assumes beta values of [0.0,9.9]
 * and that the maximum precision is 0.1.
 * ----------------------------------------------------------------
 */

#ifndef jetsubstructuremomenttools_ecfhelper_header
#define jetsubstructuremomenttools_ecfhelper_header

#include <string>
#include <cmath>

std::string GetBetaSuffix(float beta);

#endif
