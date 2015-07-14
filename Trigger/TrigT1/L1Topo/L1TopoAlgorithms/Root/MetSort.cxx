//  MetSort.cxx
//  author: Joerg Stelzer
//  creation: 27/08/14.
//  modify V Sorin, algorithm to calculate MET and convert in genericTOB

#include "L1TopoAlgorithms/MetSort.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/MetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(MetSort)


// constructor
TCS::MetSort::MetSort(const std::string & name) : SortingAlg(name) {

   defineParameter( "InputWidth", 2 ); // for FW
   defineParameter( "OutputWidth", 2 ); // for FW

}

// destructor
TCS::MetSort::~MetSort() {}


TCS::StatusCode
TCS::MetSort::initialize() {
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::MetSort::sort(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("MET sort alg expects exactly single MET TOB, got " << input.size());
      return TCS::StatusCode::FAILURE;
   }

   const MetTOBArray & mets = dynamic_cast<const MetTOBArray&>(input);
   int missingET = sqrt(mets[0].Ex()*mets[0].Ex() + mets[0].Ey() * mets[0].Ey());
   int metphi = round(10*atan2(mets[0].Ey(),mets[0].Ex()));

   output.push_back( GenericTOB( missingET, 0, metphi ) );
   
   // later we will add the Kalman MET here and add it to the output list
   // algorithms working with MET will have to modify their input check

   return TCS::StatusCode::SUCCESS;
}

