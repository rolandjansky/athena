/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauMETL1Sel.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L1 MET selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TrigTauPerformAnalysis/TrigTauMETL1Sel.h"

#include "TrigTauPerformNtuple/TrigTauEtMissL1Object.h"

#include <math.h>
#include <iostream>
using namespace std;

TrigTauMETL1Sel::TrigTauMETL1Sel() : cut_MET(0.), _debug(false)
{}

TrigTauMETL1Sel::TrigTauMETL1Sel(const TrigTauMETL1Sel& other) : 
  cut_MET(other.cut_MET),
  _debug(other._debug)
{}

void
TrigTauMETL1Sel::setCuts(float MET) {
  cut_MET = MET;
}

void
TrigTauMETL1Sel::print() const {
  cout << " L1 MET cuts :" << endl;

  if (cut_MET > 1.e-10) {
    cout << " L1 MET    > "<<cut_MET  <<endl;
  } else {
    cout << " L1 MET:     not applied" << endl;
  }
}

bool
TrigTauMETL1Sel::passed(const TrigTauEtMissL1Object& tau) const {
  if( _debug ) cout << " L1 MET: event MET is : "<< tau.m_EtMiss << endl;
  if( cut_MET  >1.e-10 && cut_MET  >= tau.m_EtMiss ) { 
    if( _debug ) cout << "  --- rejected \n";
    return false;}
  if( _debug ) cout << "  --- accepted \n";
  return true;
}
