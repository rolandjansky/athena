/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// MultiTruth
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

#include "TrigFTKSim/MultiTruth.h"

using namespace std;

void
MultiTruth::display() const
{
  const string padding("                         ");
  cout << padding << "MultiTruth (event index,barcode) <=> pt/(1 MeV) with " << _truth.size() << endl;
  if( _truth.empty() ) { return; }
  for( TruthMap::const_iterator i=_truth.begin(), f=_truth.end(); i!=f; ++i ) {
    cout << padding << "   " << (i->first).first << "," << (i->first).second
	 << " <=> " << i->second << endl;
  }
  Barcode code;
  Weight weight(0);
  const bool ok = best( code , weight );
  cout << padding << "      best: ";
  if( !ok ) { cout << "N/A"; }
  else { cout << code.first << "," << code.second << " " << weight; }
  cout << endl;
}

long int
MultiTruth::best_barcode() const
{
  Barcode code;
  Weight weight;
  const bool ok = best( code , weight );
  if( !ok ) { return -999; }
  return code.second;
}

ClassImp(MultiTruth)
