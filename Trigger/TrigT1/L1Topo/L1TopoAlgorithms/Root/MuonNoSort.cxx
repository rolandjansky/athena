/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  MuonNoSort.cxx
//  TopoCore
//  Created by Joerg Stelzer, V Sorin 2014.
//  algorithm to make sorted muon lists
//
#include "L1TopoAlgorithms/MuonNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/MuonTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(MuonNoSort)

// constructor
TCS::MuonNoSort::MuonNoSort(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 32 ); // for FW
   defineParameter( "OutputWidth", 6 );
}


// destructor
TCS::MuonNoSort::~MuonNoSort() {}

TCS::StatusCode
TCS::MuonNoSort::initialize() {
   m_numberOfMuons = parameter("InputWidth").value();

   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::MuonNoSort::sort(const InputTOBArray & input, TOBArray & output) {

   const MuonTOBArray & muons = dynamic_cast<const MuonTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   for(MuonTOBArray::const_iterator cl = muons.begin(); cl!= muons.end(); ++cl ) {
      const GenericTOB gtob(**cl);

      
      output.push_back( gtob );
   }

   // keep only max number of clusters
   if( m_numberOfMuons > 0) {
      while( output.size()> m_numberOfMuons ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

