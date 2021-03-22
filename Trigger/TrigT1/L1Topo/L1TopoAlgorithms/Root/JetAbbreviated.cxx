/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  JetAbbreviated.cxx
//  TopoCore
//  Created by Veronica Sorin on 14/8/14.

#include "L1TopoAlgorithms/JetAbbreviated.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(JetAbbreviated)


// constructor
TCS::JetAbbreviated::JetAbbreviated(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "NumberOfJets", 0 );
   defineParameter( "JetSize", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 49);
   m_jetsize = JetTOB::JS1;
}


TCS::JetAbbreviated::~JetAbbreviated()
{}






TCS::StatusCode
TCS::JetAbbreviated::sort(const InputTOBArray & input, TOBArray & output) {
   const JetTOBArray & jets = dynamic_cast<const JetTOBArray&>(input);
   // because fw seems to have a differnt notation, for now 2 means JS1 8x8
   m_jetsize = parameter("JetSize").value()==2?JetTOB::JS1:JetTOB::JS2; 


   // fill output array with GenericTOBs builds from jets
   for(JetTOBArray::const_iterator cl = jets.begin(); cl!= jets.end(); ++cl ) {
     
     if (parType_t(std::abs((*cl)-> eta())) < parameter("MinEta").value()) continue; 
     if (parType_t(std::abs((*cl)-> eta())) > parameter("MaxEta").value()) continue;      	

     output.push_back( GenericTOB(**cl, m_jetsize) );
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

