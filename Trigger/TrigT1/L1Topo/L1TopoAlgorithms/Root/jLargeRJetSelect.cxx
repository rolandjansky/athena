/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  jLargeRJetSelect.cxx
//  TopoCore
//  algorithm to create abbreviated jLargeRJet lists
//
#include "L1TopoAlgorithms/jLargeRJetSelect.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jLargeRJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jLargeRJetSelect)


// constructor
TCS::jLargeRJetSelect::jLargeRJetSelect(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 10 );
   defineParameter( "MinET", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 31);
   defineParameter( "DoEtaCut", 1);
}


TCS::jLargeRJetSelect::~jLargeRJetSelect()
{}

TCS::StatusCode
TCS::jLargeRJetSelect::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_et = parameter("MinET").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_doEtaCut = parameter("DoEtaCut").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::jLargeRJetSelect::sort(const InputTOBArray & input, TOBArray & output) {

  const jLargeRJetTOBArray & jets = dynamic_cast<const jLargeRJetTOBArray&>(input);

  // fill output array with GenericTOBs builds from jets
  for(jLargeRJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
    unsigned int Et = parType_t((*jet)->Et()); 
    if( Et <= m_et ) continue; // ET cut
    if (m_doEtaCut && (parType_t(std::abs((*jet)-> eta())) < m_minEta)) continue; 
    if (m_doEtaCut && (parType_t(std::abs((*jet)-> eta())) > m_maxEta)) continue;      	

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
