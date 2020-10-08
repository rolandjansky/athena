/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * METCut.cpp
 * Created by Joerg Stelzer on 11/16/12.
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


TCS::METCut::METCut(const std::string & name) : DecisionAlg(name)
{
   defineParameter("XE",0);
   setNumberOutputBits(1);
}

TCS::METCut::~METCut(){}


TCS::StatusCode
TCS::METCut::initialize() {
   p_XE = parameter("XE").value();
   TRG_MSG_INFO("XE             : " << p_XE);
   TRG_MSG_INFO("nummber output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::METCut::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}


TCS::StatusCode
TCS::METCut::process( const std::vector<TCS::TOBArray const *> & input,
                      const std::vector<TCS::TOBArray *> & /*output*/,
                      Decision & decision )
{

   if(input.size()!=1) {
      TCS_EXCEPTION("METCut alg must have exactly one input list (MET list), but got " << input.size());
   }

   if (input[0]->size()!=1) {
      TCS_EXCEPTION("METCut alg needs input list with a single MET TOB, got " << input[0]->size());
   }

   const TCS::GenericTOB & met = (*input[0])[0];

   TRG_MSG_DEBUG("Comparing MET " << met.Et() << " with cut " << p_XE << ". " << (met.Et() >= p_XE?"Pass":"Fail"));


   decision.setBit(0, met.Et() >= p_XE );

   return TCS::StatusCode::SUCCESS;
}
