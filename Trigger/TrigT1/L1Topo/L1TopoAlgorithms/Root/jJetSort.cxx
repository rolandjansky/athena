/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  jJetSort.cxx
//  TopoCore
//  algorithm to make sorted jJets lists
//
#include "L1TopoAlgorithms/jJetSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
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
   defineParameter( "JetSize", 2 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 31);
   defineParameter( "DoEtaCut", 1);
   m_jetsize = JetTOB::JS1;
}


TCS::jJetSort::~jJetSort()
{}



TCS::StatusCode
TCS::jJetSort::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_jsize = parameter("JetSize").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_doEtaCut = parameter("DoEtaCut").value();
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::jJetSort::sort(const InputTOBArray & input, TOBArray & output) {
   const JetTOBArray & jets = dynamic_cast<const JetTOBArray&>(input);
   // because hw seems to be using different notation, for now 2 means 8x8 or JS1, and 1 JS2 a 4x4
   m_jetsize = m_jsize==2?JetTOB::JS1:JetTOB::JS2; 
   
   // fill output array with GenericTOBs builds from jets
   for(JetTOBArray::const_iterator cl = jets.begin(); cl!= jets.end(); ++cl ) {
     if (m_doEtaCut && (parType_t(std::abs((*cl)-> eta())) < m_minEta)) continue; 
     if (m_doEtaCut && (parType_t(std::abs((*cl)-> eta())) > m_maxEta)) continue;      	
     output.push_back( GenericTOB(**cl, m_jetsize)  );
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

