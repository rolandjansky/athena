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

#include "L1TopoAlgorithms/LAr.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(LAr)


TCS::LAr::LAr(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 1);
   defineParameter("PhiMin",  0);
   defineParameter("PhiMax", 63);
   defineParameter("EtaMin",  0);
   defineParameter("EtaMax",  5);
   defineParameter("MinET",1);
   setNumberOutputBits(1);
}

TCS::LAr::~LAr(){}


TCS::StatusCode
TCS::LAr::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   if(parameter("MaxTob").value() > 0) p_NumberLeading1 = parameter("MaxTob").value();
   p_PhiMin = parameter("PhiMin").value();
   p_PhiMax = parameter("PhiMax").value();
   p_EtaMin = parameter("EtaMin").value();
   p_EtaMax = parameter("EtaMax").value();
   p_MinET = parameter("MinET").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("PhiMin   : " << p_PhiMin);
   TRG_MSG_INFO("PhiMax   : " << p_PhiMax);
   TRG_MSG_INFO("EtaMin   : " << p_EtaMin);
   TRG_MSG_INFO("EtaMax   : " << p_EtaMax);
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("nummber output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::LAr::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
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
            if( parType_t((*tob1)->eta()) <= p_EtaMin ) continue; // ETa cut
            if( parType_t((*tob1)->eta()) >= p_EtaMax ) continue; // ETa cut
            if( parType_t(abs((*tob1)->phi())) <= p_PhiMin ) continue; // phi cut
            if( parType_t(abs((*tob1)->phi())) >= p_PhiMax ) continue; // phi cut



            accept = true;

            output[0]->push_back(TCS::CompositeTOB(*tob1));

            TRG_MSG_DEBUG("TOB " << distance(input[0]->begin(), tob1) << " ET = " << (*tob1)->Et() << " ETa = " << (*tob1)->eta() << " phi = " << (*tob1)->phi() );
         }

      decision.setBit( 0, accept );



   } else {

      TCS_EXCEPTION("LAr alg must have  1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::LAr::process( const std::vector<TCS::TOBArray const *> & input,
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
            if( parType_t((*tob1)->eta()) <= p_EtaMin ) continue; // ETa cut
            if( parType_t((*tob1)->eta()) >= p_EtaMax ) continue; // ETa cut
            if( parType_t(abs((*tob1)->phi())) <= p_PhiMin ) continue; // phi cut
            if( parType_t(abs((*tob1)->phi())) >= p_PhiMax ) continue; // phi cut



            accept = true;

            output[0]->push_back(TCS::CompositeTOB(*tob1));

            TRG_MSG_DEBUG("TOB " << distance(input[0]->begin(), tob1) << " ET = " << (*tob1)->Et() << " ETa = " << (*tob1)->eta() << " phi = " << (*tob1)->phi() );
         }

      decision.setBit( 0, accept );



   } else {

      TCS_EXCEPTION("LAr alg must have  1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
