/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// HTTMultiTruth
// ================================================================
// code to match clusters and tracks to GEANT charge deposition
// information
// ================================================================
//   20-04-2009 Antonio Boveia (boveia@hep.uchicago.edu)

#include <map>
#include <vector>
#include <limits>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <cassert>

#include "TrigHTTObjects/HTTMultiTruth.h"

using namespace std;



long int
HTTMultiTruth::best_barcode() const
{
  Barcode code;
  Weight weight;
  const bool ok = best( code , weight );
  if( !ok ) { return -999; }
  return code.second;
}

ClassImp(HTTMultiTruth)
