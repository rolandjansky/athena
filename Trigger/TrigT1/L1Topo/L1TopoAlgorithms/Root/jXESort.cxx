/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// jXESort.cxx

#include "L1TopoAlgorithms/jXESort.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/jXETOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>
#include <cmath>

// Bitwise implementation utils
#include "L1TopoSimulationUtils/Trigo.h"

REGISTER_ALG_TCS(jXESort)


// constructor
TCS::jXESort::jXESort(const std::string & name) : SortingAlg(name) {

   defineParameter( "InputWidth", 2 ); // for FW
   defineParameter( "OutputWidth", 2 ); // for FW

}

// destructor
TCS::jXESort::~jXESort() {}


TCS::StatusCode
TCS::jXESort::initialize() {
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::jXESort::sortBitCorrect(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("jXE sort alg expects exactly single jXE TOB, got " << input.size());
   }

   const jXETOBArray & jxes = dynamic_cast<const jXETOBArray&>(input);
   // BW calculation of missing Et and phi
   int EtBW = quadraticSumBW(jxes[0].Ex(), jxes[0].Ey());
   int phiBW = TSU::Trigo::atan2(jxes[0].Ex(),jxes[0].Ey());

   if (phiBW < 0) { phiBW += 128; }

   TRG_MSG_DEBUG("jXE phi values " << phiBW << " " );
   output.push_back( GenericTOB( EtBW, 0, phiBW ) );

   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::jXESort::sort(const InputTOBArray & input, TOBArray & output) {

   if(input.size()!=1) {
      TCS_EXCEPTION("jXE sort alg expects exactly single jXE TOB, got " << input.size());
   }

   const jXETOBArray & jxes = dynamic_cast<const jXETOBArray&>(input);
   // NonBW calculation of missing Et and phi
   int EtNonBW = std::sqrt(jxes[0].Ex()*jxes[0].Ex() + jxes[0].Ey()*jxes[0].Ey());
   int phiNonBW = std::round(20*atan2(jxes[0].Ey(),jxes[0].Ex()));

   // Convert phi from (-pi,pi) to (0,2pi) if necessary
   if (phiNonBW < 0) { phiNonBW += 128; }

   TRG_MSG_DEBUG("jXE phi values " << phiNonBW << " " );
   output.push_back( GenericTOB( EtNonBW, 0, phiNonBW ) );
   
   return TCS::StatusCode::SUCCESS;
}

