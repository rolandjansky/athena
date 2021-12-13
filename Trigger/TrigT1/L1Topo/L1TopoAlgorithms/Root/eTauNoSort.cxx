/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
   defineParameter( "IsoMask", 0);
   defineParameter( "DoIsoCut", 1);
}


// destructor
TCS::eTauNoSort::~eTauNoSort() {}

TCS::StatusCode
TCS::eTauNoSort::initialize() {
   m_numberOfeTaus = parameter("OutputWidth").value();
   m_iso = parameter("IsoMask").value();
   m_doIsoCut = parameter("DoIsoCut").value();
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::eTauNoSort::sort(const InputTOBArray & input, TOBArray & output) {

   const eTauTOBArray & clusters = dynamic_cast<const eTauTOBArray&>(input);

   // fill output array with GenericTOB built from clusters
   for(eTauTOBArray::const_iterator cl = clusters.begin(); cl!= clusters.end(); ++cl ) {

      // Isolation cut
      if (m_doIsoCut && (m_iso != 0 )) {
          if((parType_t((*cl)->isolation()) & m_iso) != m_iso ) continue;
      }
      
      const GenericTOB gtob(**cl);
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

