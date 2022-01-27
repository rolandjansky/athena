/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  jMETNoSort.cxx
//  Algorithm to convert jMET into generic TOB

#include "L1TopoAlgorithms/jMETNoSort.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/MetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jMETNoSort)


// constructor
TCS::jMETNoSort::jMETNoSort(const std::string & name) : SortingAlg(name) {

   defineParameter( "InputWidth", 2 ); // for FW
   defineParameter( "OutputWidth", 2 ); // for FW

}

// destructor
TCS::jMETNoSort::~jMETNoSort() {}


TCS::StatusCode
TCS::jMETNoSort::initialize() {
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::jMETNoSort::sort(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("jMET sort alg expects exactly single jMET TOB, got " << input.size());
   }

   const MetTOBArray & mets = dynamic_cast<const MetTOBArray&>(input);

   for(MetTOBArray::const_iterator met = mets.begin(); met!= mets.end(); ++met ) {
       output.push_back( GenericTOB(**met) );
   }
	

   return TCS::StatusCode::SUCCESS;
}

