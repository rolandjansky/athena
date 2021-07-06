/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  MuonSelect.cxx
//  TopoCore
//  Created by Joerg Stelzer, V Sorin 2014.
//  algorithm to make abbreviated muon lists
//
#include "L1TopoAlgorithms/MuonSelect.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/MuonTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(MuonSelect)

// constructor
TCS::MuonSelect::MuonSelect(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 32 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 6 );
   defineParameter( "MinET", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 7);
}


// destructor
TCS::MuonSelect::~MuonSelect() {}

TCS::StatusCode
TCS::MuonSelect::initialize() {
   m_numberOfMuons = parameter("InputWidth").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_et = parameter("MinET").value();

   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::MuonSelect::sort(const InputTOBArray & input, TOBArray & output) {

   const MuonTOBArray & muons = dynamic_cast<const MuonTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   for(MuonTOBArray::const_iterator cl = muons.begin(); cl!= muons.end(); ++cl ) {
      const GenericTOB gtob(**cl);

      if( parType_t((*cl)->Et()) <= m_et ) continue; // ET cut

      // eta cut
      if (parType_t(std::abs((*cl)-> eta())) < m_minEta) continue; 
      if (parType_t(std::abs((*cl)-> eta())) > m_maxEta) continue;  
      
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


