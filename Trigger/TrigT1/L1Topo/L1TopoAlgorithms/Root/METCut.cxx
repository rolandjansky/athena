/*********************************
 * METCut.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/METCut.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

#include "L1TopoEvent/MetTOB.h"

REGISTER_ALG_TCS(METCut)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::METCut:     "


TCS::METCut::METCut(const std::string & name) : DecisionAlg(name)
{
   defineParameter("XE",0);
   setNumberOutputBits(1);
}

TCS::METCut::~METCut(){}


TCS::StatusCode
TCS::METCut::initialize() {
   p_XE = parameter("XE").value();
   LOG << "XE             : " << p_XE << endl;
   LOG << "nummber output : " << numberOutputBits() << endl;
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::METCut::process( const std::vector<TCS::TOBArray const *> & input,
                      const std::vector<TCS::TOBArray *> & /*output*/,
                      Decision & decision )
{

   if(input.size()!=1) {
      TCS_EXCEPTION("METCut alg must have either 1 input list (MET list), but got " << input.size());
      return TCS::StatusCode::FAILURE;
   }

   if (input[0]->size()!=1) {
      TCS_EXCEPTION("METCut alg needs input list with a single MET TOB, got " << input[0]->size());
      return TCS::StatusCode::FAILURE;
   }

   const TCS::MetTOB & met = dynamic_cast<const TCS::MetTOB&>( (*input[0])[0]);

   // need to calculate MET ET properly
   unsigned int metSq = met.Ex()*met.Ex() + met.Ey() * met.Ey();

   decision.setBit(0, metSq >= p_XE * p_XE );

   return TCS::StatusCode::SUCCESS;
}
