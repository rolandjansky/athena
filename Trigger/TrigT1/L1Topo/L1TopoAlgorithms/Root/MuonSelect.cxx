/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  MuonSelect.cxx
//  TopoCore
//  Created by Joerg Stelzer, V Sorin 2014.
//  algorithm to make abbreviated muon lists
//  updated in 20/09/2021 to use flags selection

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
  defineParameter( "MinEta", 0 );
  defineParameter( "MaxEta", 7 );
  defineParameter( "MinEtTGC", 0 );
  defineParameter( "MinEtRPC", 0 );
  defineParameter( "InnerCoinCut", 0 );
  defineParameter( "FullStationCut", 0 );
  defineParameter( "GoodMFieldCut", 0 );
  defineParameter( "MinET", 0 );
}


// destructor
TCS::MuonSelect::~MuonSelect() {}


TCS::StatusCode
TCS::MuonSelect::initialize() {
  m_numberOfMuons = parameter("InputWidth").value();
  m_minEta = parameter("MinEta").value();
  m_maxEta = parameter("MaxEta").value();
  m_MinEtTGC = parameter("MinEtTGC").value();	    
  m_MinEtRPC = parameter("MinEtRPC").value();	    
  m_InnerCoinCut = parameter("InnerCoinCut").value();  
  m_FullStationCut = parameter("FullStationCut").value();
  m_GoodMFieldCut = parameter("GoodMFieldCut").value(); 
  m_et = parameter("MinET").value();
  
  return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::MuonSelect::sort(const InputTOBArray & input, TOBArray & output) {

  const MuonTOBArray & muons = dynamic_cast<const MuonTOBArray&>(input);
  
  // fill output array with GenericTOB buildt from clusters
  for(MuonTOBArray::const_iterator cl = muons.begin(); cl!= muons.end(); ++cl ) {
    const GenericTOB gtob(**cl);

    // Tmp, to be removed after new menu is implemented    
    if( parType_t((*cl)->Et()) <= m_et ) continue; // ET cut

    // Apply the relevant cut to the right kind of muon
    if( (parType_t((*cl)->Et()) <= m_MinEtTGC && parType_t((*cl)->isTGC())) ||  
	(parType_t((*cl)->Et()) <= m_MinEtRPC && (!parType_t((*cl)->isTGC()))) ) continue; // ET cut
    
    // eta cut
    if (parType_t(std::abs((*cl)-> eta())) < m_minEta) continue; 
    if (parType_t(std::abs((*cl)-> eta())) > m_maxEta) continue;  

    // Apply flag selection only for TGC muons. The flag selection is applied only if the corresponing parameter from the menu is 1.  
    if ( parType_t((*cl)->isTGC()) )
      {
	if(m_InnerCoinCut == 1 && ( ! ((int)parType_t((*cl)->innerCoin()) == (int)m_InnerCoinCut ) ) ) continue;
	if(m_FullStationCut == 1 && ( ! ((int)parType_t((*cl)->bw2or3()) == (int)m_FullStationCut ) ) ) continue;
	if(m_GoodMFieldCut == 1 && ( ! ((int)parType_t((*cl)->goodMF()) == (int)m_GoodMFieldCut ) ) ) continue;
      }

    output.push_back( gtob );
  }

  // keep only max number of muons
  if( m_numberOfMuons > 0) {
    while( output.size()> m_numberOfMuons ) {
      output.pop_back();
    }
  }
  return TCS::StatusCode::SUCCESS;
}


