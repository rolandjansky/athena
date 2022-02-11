/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jEmNoSort.cxx
// TopoCore
// Algorithm to generate ALL lists of jEm TOBs

#include "L1TopoAlgorithms/jEmNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jEmTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jEmNoSort)


// constructor
TCS::jEmNoSort::jEmNoSort(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
}


TCS::jEmNoSort::~jEmNoSort()
{}

TCS::StatusCode
TCS::jEmNoSort::initialize() {
   m_numberOfjEms = parameter("OutputWidth").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::jEmNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const jEmTOBArray & jets = dynamic_cast<const jEmTOBArray&>(input);

   // fill output array with GenericTOBs builds from jets
   for(jEmTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
       output.push_back( GenericTOB(**jet) );
   }


   // keep only max number of jets
   int par = m_numberOfjEms ;
   unsigned int maxNumberOfjEms = (unsigned int)(par<0?0:par);
   if(maxNumberOfjEms>0) {
      while( output.size()> maxNumberOfjEms ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

