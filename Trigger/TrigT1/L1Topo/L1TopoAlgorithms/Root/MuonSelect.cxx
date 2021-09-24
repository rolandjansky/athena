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
  defineParameter( "InnerCoinCut", -1 );
  defineParameter( "FullStationCut", -1 );
  defineParameter( "GoodMFieldCut", -1 );
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

    // Check the definition of sectorName at MuCTPIL1TopoCandidate.h (for TGC muons: sectorName.at(0)=E or F, for RPC muons: sectorName.at(0)=B)
    // Apply the relevant cut to the right kind of muon
    std::string sector = (*cl)->sectorName();
    if( parType_t((*cl)->Et()) <= m_MinEtTGC && sector.at(0) != 'B' ||  
	parType_t((*cl)->Et()) <= m_MinEtRPC && sector.at(0) == 'B') continue; // ET cut

    // eta cut
    if (parType_t(std::abs((*cl)-> eta())) < m_minEta) continue; 
    if (parType_t(std::abs((*cl)-> eta())) > m_maxEta) continue;  
 
    // implementation of the selection with flags based on https://indico.cern.ch/event/1072597/contributions/4510314/attachments/2302129/3916048/MuonSelectWithFlags.pdf
    // Apply the cut only for TGC muons 
    if ( !( parType_t((*cl)->innerCoin()) >= m_InnerCoinCut &&
	    parType_t((*cl)->bw2or3()) >= m_FullStationCut &&
	    parType_t((*cl)->goodMF()) >= m_GoodMFieldCut &&
	    sector.at(0) != 'B'))
      continue;

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


