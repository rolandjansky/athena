/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  jEmSelect.cxx
//  TopoCore
//  algorithm to create abbreviated jEm lists
//
#include "L1TopoAlgorithms/jEmSelect.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jEmTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jEmSelect)


// constructor
TCS::jEmSelect::jEmSelect(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 10 );
   defineParameter( "MinET", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 0 );
   defineParameter( "IsoMin", 0 );
   defineParameter( "Frac1Min", 0 );
   defineParameter( "Frac2Min", 0 );
}


TCS::jEmSelect::~jEmSelect()
{}

TCS::StatusCode
TCS::jEmSelect::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_et = parameter("MinET").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_iso = parameter("IsoMin").value();
   m_frac1 = parameter("Frac1Min").value();
   m_frac2 = parameter("Frac2Min").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::jEmSelect::sort(const InputTOBArray & input, TOBArray & output) {

  const jEmTOBArray & jets = dynamic_cast<const jEmTOBArray&>(input);

  // fill output array with GenericTOBs builds from jets
  for(jEmTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
    // ET cut
    if ( parType_t((*jet)->Et()) <= m_et ) continue;
    // Isolation cuts
    if ( !isocut(m_iso, (*jet)-> isolation()) ) continue;
    if ( !isocut(m_frac1, (*jet)-> frac1()) ) continue;
    if ( !isocut(m_frac2, (*jet)-> frac2()) ) continue;
    // Eta cut
    if ( parType_t(std::abs((*jet)-> eta())) < m_minEta ) continue; 
    if ( parType_t(std::abs((*jet)-> eta())) > m_maxEta ) continue;      	

    output.push_back( GenericTOB(**jet) );
  }

   // keep only max number of jets
   int par = m_numberOfJets ;
   unsigned int maxNumberOfJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfJets>0) {
      while( output.size()> maxNumberOfJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

