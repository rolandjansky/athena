/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RZPair.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef RZPAIR_H
#define RZPAIR_H

#include <utility>
#include <vector>

// define the (r,z) pair values
typedef std::pair<double,double>   RZPair;

// vector of (r,z) pairs
typedef std::vector<RZPair>        RZPairVector;

#endif // RZPAIR_H

