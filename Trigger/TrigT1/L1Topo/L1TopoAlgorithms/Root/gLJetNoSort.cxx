/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// gLJetNoSort.cxx
// TopoCore
// Algorithm to generate ALL lists of gLJet TOBs

#include "L1TopoAlgorithms/gLJetNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/gLJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(gLJetNoSort)


// constructor
TCS::gLJetNoSort::gLJetNoSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
}


TCS::gLJetNoSort::~gLJetNoSort()
{}

TCS::StatusCode
TCS::gLJetNoSort::initialize() {
   m_numberOfgLJets = parameter("OutputWidth").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::gLJetNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const gLJetTOBArray & jets = dynamic_cast<const gLJetTOBArray&>(input);

   // fill output array with GenericTOBs builds from jets
   for(gLJetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
       output.push_back( GenericTOB(**jet) );
   }


   // keep only max number of jets
   int par = m_numberOfgLJets ;
   unsigned int maxNumberOfgLJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfgLJets>0) {
      while( output.size()> maxNumberOfgLJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

