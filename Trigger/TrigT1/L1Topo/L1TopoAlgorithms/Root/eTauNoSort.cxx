/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// eTauNoSort.cxx
// TopoCore
// Algorithm to generate ALL lists of eTau TOBs

#include "L1TopoAlgorithms/eTauNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/eTauTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(eTauNoSort)

// constructor
TCS::eTauNoSort::eTauNoSort(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 120 ); // for fw
   defineParameter( "OutputWidth", 120 );    
   defineParameter( "RCore", 0);
   defineParameter( "RHad", 0);
}


// destructor
TCS::eTauNoSort::~eTauNoSort() {}

TCS::StatusCode
TCS::eTauNoSort::initialize() {
   m_numberOfeTaus = parameter("OutputWidth").value();
   m_rCore = parameter("RCore").value();
   m_rHad = parameter("RHad").value();
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::eTauNoSort::sort(const InputTOBArray & input, TOBArray & output) {

   const eTauTOBArray & clusters = dynamic_cast<const eTauTOBArray&>(input);

   // fill output array with GenericTOB built from clusters
   for(eTauTOBArray::const_iterator etau = clusters.begin(); etau!= clusters.end(); ++etau ) {

      // TODO: isolation cut
      
      const GenericTOB gtob(**etau);
      output.push_back( gtob );
   }


   // keep only max number of clusters
   int par = m_numberOfeTaus ;
   unsigned int maxNumberOfeTaus = (unsigned int)(par<0?0:par);
   if(maxNumberOfeTaus>0) {
      while( output.size()> maxNumberOfeTaus ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

