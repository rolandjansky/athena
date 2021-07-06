/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  METNoSort.cxx
//  author: Joerg Stelzer
//  creation: 27/08/14.
//  modify V Sorin, algorithm to calculate MET and convert in genericTOB

#include "L1TopoAlgorithms/METNoSort.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/MetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(METNoSort)


// constructor
TCS::METNoSort::METNoSort(const std::string & name) : SortingAlg(name) {

   defineParameter( "InputWidth", 2 ); // for FW
   defineParameter( "OutputWidth", 2 ); // for FW

}

// destructor
TCS::METNoSort::~METNoSort() {}


TCS::StatusCode
TCS::METNoSort::initialize() {
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::METNoSort::sort(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("MET sort alg expects exactly single MET TOB, got " << input.size());
   }

   const MetTOBArray & mets = dynamic_cast<const MetTOBArray&>(input);
//   int missingET = sqrt(mets[0].Ex()*mets[0].Ex() + mets[0].Ey() * mets[0].Ey());
//   int metphi = round(10*atan2(mets[0].Ey(),mets[0].Ex()));

   for(MetTOBArray::const_iterator cl = mets.begin(); cl!= mets.end(); ++cl ) {
	
	     output.push_back( GenericTOB(**cl) );
    }
	
//   output.push_back( GenericTOB( &mets[0] ));
   

   return TCS::StatusCode::SUCCESS;
}

