/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jLJetNoSort.cxx
// TopoCore
// Algorithm to generate ALL lists of jLJet TOBs

#include "L1TopoAlgorithms/jLJetNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jLJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jLJetNoSort)


// constructor
TCS::jLJetNoSort::jLJetNoSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
}


TCS::jLJetNoSort::~jLJetNoSort()
{}

TCS::StatusCode
TCS::jLJetNoSort::initialize() {
   m_numberOfjLJets = parameter("OutputWidth").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::jLJetNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const jLJetTOBArray & jets = dynamic_cast<const jLJetTOBArray&>(input);

   // fill output array with GenericTOBs builds from jets
   for(jLJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
       output.push_back( GenericTOB(**jet) );
   }


   // keep only max number of jets
   int par = m_numberOfjLJets ;
   unsigned int maxNumberOfjLJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfjLJets>0) {
      while( output.size()> maxNumberOfjLJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

