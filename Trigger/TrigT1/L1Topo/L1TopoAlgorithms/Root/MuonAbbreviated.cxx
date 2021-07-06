/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  MuonAbbreviated.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoAlgorithms/MuonAbbreviated.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/MuonTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(MuonAbbreviated)

// constructor
TCS::MuonAbbreviated::MuonAbbreviated(const std::string & name) : SortingAlg(name) {
   defineParameter( "NumberOfMuons", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 49);
}


// destructor
TCS::MuonAbbreviated::~MuonAbbreviated() {}

TCS::StatusCode
TCS::MuonAbbreviated::initialize() {
   m_numberOfMuons = parameter("NumberOfMuons").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   TRG_MSG_INFO("NumberOfMuons : " << m_numberOfMuons);
   TRG_MSG_INFO("MinEta        : " << m_minEta);
   TRG_MSG_INFO("MaxEta        : " << m_maxEta);
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::MuonAbbreviated::sort(const InputTOBArray & input, TOBArray & output) {

   const MuonTOBArray & muons = dynamic_cast<const MuonTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   //for(MuonTOBArray::const_iterator cl = muons.begin(); cl!= muons.end(); ++cl ) {
   for(const MuonTOB* muon : muons ) {
      const GenericTOB gtob(*muon);

      parType_t muonAbsEta = (parType_t) std::abs( muon->eta() );

      // eta cut
      bool passEta = (muonAbsEta >= m_minEta) && (muonAbsEta <= m_maxEta);

      TRG_MSG_DEBUG("Muon with Et=" << muon->Et() << " and abs(eta)=" << muonAbsEta << " --> " << (passEta?"pass":"fail") );

      if ( ! passEta ) continue;

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

