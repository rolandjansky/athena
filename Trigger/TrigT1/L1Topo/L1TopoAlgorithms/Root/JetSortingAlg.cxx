/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  JetSortingAlg.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoAlgorithms/JetSortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(JetSortingAlg)

bool SortByEtLargestFirst2(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() > tob2->Et();
}

bool SortByEtSmallestFirst2(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() < tob2->Et();
}

// constructor
TCS::JetSortingAlg::JetSortingAlg(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "NumberOfJets", 0 );
   defineParameter( "ReverseOrder", 0 );
   defineParameter( "JetSize", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 49);
   m_jetsize = JetTOB::JS1;
}


TCS::JetSortingAlg::~JetSortingAlg()
{}






TCS::StatusCode
TCS::JetSortingAlg::sort(const InputTOBArray & input, TOBArray & output) {
   const JetTOBArray & jets = dynamic_cast<const JetTOBArray&>(input);
   // because hw seems to be using different notation, for now 2 means 8x8 or JS1, and 1 JS2 a 4x4
   m_jetsize = parameter("JetSize").value()==2?JetTOB::JS1:JetTOB::JS2; 
   
   // fill output array with GenericTOBs builds from jets
   for(JetTOBArray::const_iterator cl = jets.begin(); cl!= jets.end(); ++cl ) {
     if (parType_t(std::abs((*cl)-> eta())) < parameter("MinEta").value()) continue; 
     if (parType_t(std::abs((*cl)-> eta())) > parameter("MaxEta").value()) continue;
     output.push_back( GenericTOB(**cl, m_jetsize)  );
   }

   // sort, possibly in reversed order
   int reverseOrder = parameter("ReverseOrder").value();
   if(reverseOrder==1) {
      output.sort(SortByEtSmallestFirst2);
   } else {
      output.sort(SortByEtLargestFirst2);
   }

   // keep only max number of jets
   int par = parameter("NumberOfJets").value();
   unsigned int maxNumberOfJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfJets>0) {
      while( output.size()> maxNumberOfJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

