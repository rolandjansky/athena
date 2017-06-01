//  MetSort.cxx
//  author: Joerg Stelzer
//  creation: 27/08/14.
//  modify V Sorin, algorithm to calculate MET and convert in genericTOB

#include "L1TopoAlgorithms/MetSort.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/MetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>
#include <cmath>

// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"

using std::sqrt;
using std::round;

//
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
TCS::MetSort::sortBitCorrect(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("MET sort alg expects exactly single MET TOB, got " << input.size());
   }

   const MetTOBArray & mets = dynamic_cast<const MetTOBArray&>(input);
   int missingET = TSU::Kinematics::quadraticSumBW(mets[0].Ex(), mets[0].Ey());
   int metphi = TSU::Trigo::atan2(mets[0].Ex(),mets[0].Ey());

   TRG_MSG_DEBUG("MET phi values " << metphi << " " );
   output.push_back( GenericTOB( missingET, 0, metphi ) );

   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::MetSort::sort(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("MET sort alg expects exactly single MET TOB, got " << input.size());
   }

   const MetTOBArray & mets = dynamic_cast<const MetTOBArray&>(input);
   int missingET = sqrt(mets[0].Ex()*mets[0].Ex() + mets[0].Ey() * mets[0].Ey());
   // this returns values between -pi and pi. it needs to match the format for phi for jets and em/tau. The algorithm that uses this is mindeltaphi which
   // uses phiDouble, filled with the phi value as returned from calosim, which also uses -pi pi. However, if replaced by phi, this is an integer between 0 63, so
   // atan2 should be changed to match this. If all is replaced by the bitwise then all should match as both , the trigo function below and the TOB phi are given in the range 0 63 
   int metphi = round(10*atan2(mets[0].Ey(),mets[0].Ex()));
   // bitwise atan2 
   //int metphi = TSU::Trigo::atan2(mets[0].Ex(),mets[0].Ey());
   //
   TRG_MSG_DEBUG("MET phi values " << metphi << " " );
   output.push_back( GenericTOB( missingET, 0, metphi ) );
   
   // later we will add the Kalman MET here and add it to the output list
   // algorithms working with MET will have to modify their input check

   return TCS::StatusCode::SUCCESS;
}

