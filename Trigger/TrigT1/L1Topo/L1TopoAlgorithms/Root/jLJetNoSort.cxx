/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// jLJetNoSort.cxx
// TopoCore
// Algorithm to generate ALL lists of jLargeRJet TOBs

#include "L1TopoAlgorithms/jLJetNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jLargeRJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jLJetNoSort)


// constructor
TCS::jLJetNoSort::jLJetNoSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
   defineParameter( "jLargeRJetSize", 2 );
}


TCS::jLJetNoSort::~jLJetNoSort()
{}

TCS::StatusCode
TCS::jLJetNoSort::initialize() {
   m_numberOfjLargeRJets = parameter("OutputWidth").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::jLJetNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const jLargeRJetTOBArray & jets = dynamic_cast<const jLargeRJetTOBArray&>(input);

   // fill output array with GenericTOBs builds from jets
   for(jLargeRJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
       output.push_back( GenericTOB(**jet) );
   }


   // keep only max number of jets
   int par = m_numberOfjLargeRJets ;
   unsigned int maxNumberOfjLargeRJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfjLargeRJets>0) {
      while( output.size()> maxNumberOfjLargeRJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

