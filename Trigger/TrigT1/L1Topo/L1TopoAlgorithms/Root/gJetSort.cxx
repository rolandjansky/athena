/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  gJetSort.cxx
//  TopoCore
//  algorithm to make sorted gJets lists
//
#include "L1TopoAlgorithms/gJetSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(gJetSort)

bool SortByEtLargestgJet(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() > tob2->Et();
}


// constructor
TCS::gJetSort::gJetSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 10 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 31);
}


TCS::gJetSort::~gJetSort()
{}



TCS::StatusCode
TCS::gJetSort::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::gJetSort::sort(const InputTOBArray & input, TOBArray & output) {
   const gJetTOBArray & jets = dynamic_cast<const gJetTOBArray&>(input);
   
   // fill output array with GenericTOBs builds from jets
   for(gJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
     if ( parType_t(std::abs((*jet)-> eta())) < m_minEta) continue; 
     if ( parType_t(std::abs((*jet)-> eta())) > m_maxEta) continue;      	
     output.push_back( GenericTOB(**jet)  );
   }

   // sort
   output.sort(SortByEtLargestgJet);
   

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

