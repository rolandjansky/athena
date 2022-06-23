/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jXENoSort.cxx

#include "L1TopoAlgorithms/jXENoSort.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jXETOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(jXENoSort)


// constructor
TCS::jXENoSort::jXENoSort(const std::string & name) : SortingAlg(name) {

   defineParameter( "InputWidth", 2 ); // for FW
   defineParameter( "OutputWidth", 2 ); // for FW

}

// destructor
TCS::jXENoSort::~jXENoSort() {}

TCS::StatusCode
TCS::jXENoSort::initialize() {
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::jXENoSort::sort(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
     TCS_EXCEPTION("jXE sort alg expects exactly single jXE TOB, got " << input.size());
   }

   const jXETOBArray & jxes = dynamic_cast<const jXETOBArray&>(input);

   for(jXETOBArray::const_iterator jxe = jxes.begin(); jxe!= jxes.end(); ++jxe ) { 
     output.push_back( GenericTOB(**jxe) );
   }
	
   return TCS::StatusCode::SUCCESS;

}
