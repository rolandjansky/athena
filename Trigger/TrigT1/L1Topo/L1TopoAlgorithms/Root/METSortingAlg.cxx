/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  file: METSortingAlg.cxx
//  author: Joerg Stelzer
//  creation: 27/08/14.

#include "L1TopoAlgorithms/METSortingAlg.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/MetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>
#include <cmath>

using std::sqrt;

REGISTER_ALG_TCS(METSortingAlg)


// constructor
TCS::METSortingAlg::METSortingAlg(const std::string & name) : SortingAlg(name) {
   defineParameter( "MinEta", 0);
   defineParameter( "MaxEta", 31);
}

// destructor
TCS::METSortingAlg::~METSortingAlg() {}


TCS::StatusCode
TCS::METSortingAlg::initialize() {
   p_MaxEta = parameter("MinEta").value();
   p_MinEta = parameter("MaxEta").value();
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::METSortingAlg::sort(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("MET sort alg expects exactly single MET TOB, got " << input.size());
   }

   const MetTOBArray & mets = dynamic_cast<const MetTOBArray&>(input);
   int missingET = sqrt(mets[0].Ex()*mets[0].Ex() + mets[0].Ey() * mets[0].Ey());

   output.push_back( GenericTOB( missingET, 0, 0 ) );
   
   // later we will add the Kalman MET here and add it to the output list
   // algorithms working with MET will have to modify their input check

   return TCS::StatusCode::SUCCESS;
}

