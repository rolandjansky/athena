//  MuonSort_1BC.cxx
//  TopoCore
//  Created by Sue Cheatham based on MuonSelect created by Joerg Stelzer, V Sorin 2014.  Selects LateMuons from BC+1
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//  algorithm to make abbreviated latemuon lists
//
#include "L1TopoAlgorithms/MuonSort_1BC.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/MuonTOBArray.h"
#include "L1TopoEvent/LateMuonTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(MuonSort_1BC)

// constructor
TCS::MuonSort_1BC::MuonSort_1BC(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 32 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 6 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 7); 
   defineParameter( "nDelayedMuons", 1 );  

}

// destructor
TCS::MuonSort_1BC::~MuonSort_1BC() {}

TCS::StatusCode
TCS::MuonSort_1BC::initialize() {

   m_numberOfMuons = parameter("nDelayedMuons").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();

   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::MuonSort_1BC::sort(const InputTOBArray & input, TOBArray & output) {

  TRG_MSG_DEBUG("MuonSort_1BC");

  const LateMuonTOBArray & muons = dynamic_cast<const LateMuonTOBArray&>(input);

  // fill output array with GenericTOB built from late muons
  int ii=0;
  for(LateMuonTOBArray::const_iterator lm = muons.begin(); lm!= muons.end(); ++lm ) {

    const GenericTOB gtob(**lm);
    ++ii;

    // eta cut
    if (parType_t(std::abs((*lm)-> eta())) < m_minEta) continue; 
    if (parType_t(std::abs((*lm)-> eta())) > m_maxEta) continue;  

    output.push_back( gtob );
  }
  
  // sort
  output.sort( [] (auto tob1, auto tob2) {return tob1->Et() > tob2->Et();} );

  // keep only max number of muons
  if( m_numberOfMuons > 0) {
    while( output.size()> m_numberOfMuons ) {
      output.pop_back();
    }
  }

  return TCS::StatusCode::SUCCESS;
}

