/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  gLJetSelect.cxx
//  TopoCore
//  algorithm to create abbreviated gLargeRJet lists
//
#include "L1TopoAlgorithms/gLJetSelect.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gLargeRJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(gLJetSelect)


// constructor
TCS::gLJetSelect::gLJetSelect(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 10 );
   defineParameter( "MinET", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 31);
}


TCS::gLJetSelect::~gLJetSelect()
{}

TCS::StatusCode
TCS::gLJetSelect::initialize() {
   m_numberOfgLJets = parameter("OutputWidth").value();
   m_et = parameter("MinET").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::gLJetSelect::sort(const InputTOBArray & input, TOBArray & output) {

  const gLargeRJetTOBArray & jets = dynamic_cast<const gLargeRJetTOBArray&>(input);

  // fill output array with GenericTOBs builds from jets
  for(gLargeRJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
    unsigned int Et = parType_t((*jet)->Et()); 
    if( Et <= m_et ) continue; // ET cut
    if ( parType_t(std::abs((*jet)-> eta())) < m_minEta) continue; 
    if ( parType_t(std::abs((*jet)-> eta())) > m_maxEta) continue;      	

    output.push_back( GenericTOB(**jet) );
  }

   // keep only max number of jets
   int par = m_numberOfgLJets ;
   unsigned int maxNumberOfJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfJets>0) {
      while( output.size()> maxNumberOfJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

