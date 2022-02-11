/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  jJetSort.cxx
//  TopoCore
//  algorithm to make sorted jJets lists
//
#include "L1TopoAlgorithms/jJetSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jJetSort)

bool SortByEtLargestjJet(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() > tob2->Et();
}

// constructor
TCS::jJetSort::jJetSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 10 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 31);
}


TCS::jJetSort::~jJetSort()
{}



TCS::StatusCode
TCS::jJetSort::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::jJetSort::sort(const InputTOBArray & input, TOBArray & output) {
  
   const jJetTOBArray & jets = dynamic_cast<const jJetTOBArray&>(input);
   
   // fill output array with GenericTOBs builds from jets
   for(jJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
     if ( parType_t(std::abs((*jet)-> eta())) < m_minEta ) continue; 
     if ( parType_t(std::abs((*jet)-> eta())) > m_maxEta ) continue;      	
     output.push_back( GenericTOB(**jet)  );
   }

   // sort
   output.sort(SortByEtLargestjJet);
   
   // keep only max number of jets
   int par = m_numberOfJets;
   unsigned int maxNumberOfJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfJets>0) {
      while( output.size()> maxNumberOfJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

