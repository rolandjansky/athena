/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * LAr.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V SOrin 2014
 *
 * @brief algorithm calculates the phi-distance between one  lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/EtCut.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(EtCut)


TCS::EtCut::EtCut(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 1);
   defineParameter("MinET",1);
   setNumberOutputBits(1);
}

TCS::EtCut::~EtCut(){}


TCS::StatusCode
TCS::EtCut::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   if(parameter("MaxTob").value() > 0) p_NumberLeading1 = parameter("MaxTob").value();
   p_MinET = parameter("MinET").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("nummber output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::EtCut::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
  //Bitwise implementation should be fully identical 
  //to the non-bitwise one due to its nature as a simple ET cut
	return process(input,output,decision);
}


TCS::StatusCode
TCS::EtCut::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 1) {

      TRG_MSG_DEBUG("input size     : " << input[0]->size());

      unsigned int nLeading = p_NumberLeading1;
      bool accept{false};

      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
           ++tob1) 
         {
            
            if( parType_t((*tob1)->Et()) <= p_MinET ) continue; // ET cut

            accept = true;

            output[0]->push_back(TCS::CompositeTOB(*tob1));

            TRG_MSG_DEBUG("TOB " << distance(input[0]->begin(), tob1) << " ET = " << (*tob1)->Et() );
         }

      decision.setBit( 0, accept );



   } else {

      TCS_EXCEPTION("EtCut alg must have  1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
