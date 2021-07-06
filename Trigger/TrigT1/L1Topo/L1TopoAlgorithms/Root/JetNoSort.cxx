/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  JetNoSort.cxx
//  TopoCore
//  Created by Veronica Sorin on 14/8/14.
//  Algorithm to generate ALL lists of JETs TOBs
//
#include "L1TopoAlgorithms/JetNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(JetNoSort)


// constructor
TCS::JetNoSort::JetNoSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
   defineParameter( "JetSize", 2 );
   m_jetsize = JetTOB::JS1;
}


TCS::JetNoSort::~JetNoSort()
{}

TCS::StatusCode
TCS::JetNoSort::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_jsize = parameter("JetSize").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::JetNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const JetTOBArray & jets = dynamic_cast<const JetTOBArray&>(input);
   // because fw seems to have a differnt notation, for now 2 means JS1 8x8
   m_jetsize = m_jsize==2?JetTOB::JS1:JetTOB::JS2; 

   // fill output array with GenericTOBs builds from jets
   for(JetTOBArray::const_iterator cl = jets.begin(); cl!= jets.end(); ++cl ) {

     output.push_back( GenericTOB(**cl, m_jetsize) );
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

