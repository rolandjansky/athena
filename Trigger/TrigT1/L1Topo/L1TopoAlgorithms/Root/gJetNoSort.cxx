/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// gJetNoSort.cxx
// TopoCore
// Algorithm to generate ALL lists of gJet TOBs

#include "L1TopoAlgorithms/gJetNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(gJetNoSort)


// constructor
TCS::gJetNoSort::gJetNoSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
}


TCS::gJetNoSort::~gJetNoSort()
{}

TCS::StatusCode
TCS::gJetNoSort::initialize() {
   m_numberOfgJets = parameter("OutputWidth").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::gJetNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const gJetTOBArray & jets = dynamic_cast<const gJetTOBArray&>(input);

   // fill output array with GenericTOBs builds from jets
   for(gJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
       output.push_back( GenericTOB(**jet) );
   }


   // keep only max number of jets
   int par = m_numberOfgJets ;
   unsigned int maxNumberOfgJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfgJets>0) {
      while( output.size()> maxNumberOfgJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

