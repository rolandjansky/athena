/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ----------------------------------------------------------------
 * This is a helper function that provides the suffix for NSubjettiness 
 * and NSubjettiness ratio decorations.  No suffix is used for alpha = 1.0 
 * and other values are given a suffix of _AlphaN where N is the value
 * with decimal points removed.  This assumes alpha values of [0.0,9.9]
 * and that the maximum precision is 0.1.
 * ----------------------------------------------------------------
 */

#ifndef jetsubstructuremomenttools_nsubjettinesshelper_header
#define jetsubstructuremomenttools_nsubjettinesshelper_header

#include <string>
#include <cmath>

std::string GetAlphaSuffix(float alpha);

#endif
