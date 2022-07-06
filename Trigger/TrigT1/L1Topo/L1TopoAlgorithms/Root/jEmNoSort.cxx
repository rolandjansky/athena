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
   defineParameter( "IsoMin", 0 );
   defineParameter( "Frac1Min", 0 );
   defineParameter( "Frac2Min", 0 );
}


TCS::jEmNoSort::~jEmNoSort()
{}

TCS::StatusCode
TCS::jEmNoSort::initialize() {
   m_numberOfjEms = parameter("OutputWidth").value();
   m_iso = parameter("IsoMin").value();
   m_frac1 = parameter("Frac1Cut").value();
   m_frac2 = parameter("Frac2Cut").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::jEmNoSort::sort(const InputTOBArray & input, TOBArray & output) {
   const jEmTOBArray & jets = dynamic_cast<const jEmTOBArray&>(input);

   // fill output array with GenericTOBs builds from jets
   for(jEmTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
      // Isolation cuts
      if ( !isocut(m_iso, (*jet)-> isolation()) ) continue; 
      if ( !isocut(m_frac1, (*jet)-> frac1()) ) continue; 
      if ( !isocut(m_frac2, (*jet)-> frac2()) ) continue; 

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

