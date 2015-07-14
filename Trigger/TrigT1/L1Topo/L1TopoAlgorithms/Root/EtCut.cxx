/*********************************
 * EtCut.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/EtCut.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

#include "L1TopoEvent/MetTOB.h"

REGISTER_ALG_TCS(EtCut)

using namespace std;


TCS::EtCut::EtCut(const std::string & name) : DecisionAlg(name)
{
   defineParameter("MinET",0);
   setNumberOutputBits(1);
}

TCS::EtCut::~EtCut(){}


TCS::StatusCode
TCS::EtCut::initialize() {
   p_MinET = parameter("MinET").value();
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::EtCut::process( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )
{

   if(input.size()!=1) {
      TCS_EXCEPTION("EtCut alg must have exactly 1 input list, but got " << input.size());
      return TCS::StatusCode::FAILURE;
   }

   bool accept{false};

   for( TCS::TOBArray::const_iterator tob = input[0]->begin(); tob != input[0]->end(); ++tob) {

      if( parType_t((*tob)->Et()) < p_MinET) continue; // ET cut

      accept = true;

      output[0]->push_back(TCS::CompositeTOB(*tob));

      TRG_MSG_DEBUG("Jet " << distance(input[0]->begin(), tob) << " ET = " << (*tob)->Et());
   }

   decision.setBit( 0, accept );

   return TCS::StatusCode::SUCCESS;
}
