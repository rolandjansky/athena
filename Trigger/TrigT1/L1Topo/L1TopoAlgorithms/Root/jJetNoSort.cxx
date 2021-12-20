/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// jJetNoSort.cxx
// TopoCore
// Algorithm to generate ALL lists of jJet TOBs

#include "L1TopoAlgorithms/jJetNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jJetNoSort)


// constructor
TCS::jJetNoSort::jJetNoSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
   defineParameter( "jJetSize", 2 );
}


TCS::jJetNoSort::~jJetNoSort()
{}

TCS::StatusCode
TCS::jJetNoSort::initialize() {
   m_numberOfjJets = parameter("OutputWidth").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::jJetNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const jJetTOBArray & jets = dynamic_cast<const jJetTOBArray&>(input);

   // fill output array with GenericTOBs builds from jets
   for(jJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
       output.push_back( GenericTOB(**jet) );
   }


   // keep only max number of jets
   int par = m_numberOfjJets ;
   unsigned int maxNumberOfjJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfjJets>0) {
      while( output.size()> maxNumberOfjJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

