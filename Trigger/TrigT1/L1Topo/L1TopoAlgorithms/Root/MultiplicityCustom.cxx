/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * MultiplicityCustom.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V SOrin 2014
 *
 * algo that just apply et cut and then multiplicity of remaining TOBs
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/MultiplicityCustom.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(MultiplicityCustom)


TCS::MultiplicityCustom::MultiplicityCustom(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("NumResultBits", 1);
   defineParameter("MinET",1);
   defineParameter("MinEta",0);
   defineParameter("MaxEta",49);
   defineParameter("MinMultiplicity",1);

   setNumberOutputBits(1);
}

TCS::MultiplicityCustom::~MultiplicityCustom(){}


TCS::StatusCode
TCS::MultiplicityCustom::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   p_MinET = parameter("MinET").value();
   p_Multiplicity = parameter("MinMultiplicity").value();

   p_EtaMin = parameter("MinEta").value();
   p_EtaMax = parameter("MaxEta").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("Multiplicity          : " << p_Multiplicity);
   TRG_MSG_INFO("MinETa          : " << p_EtaMin);
   TRG_MSG_INFO("MaxETa          : " << p_EtaMax);

   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::MultiplicityCustom::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}


TCS::StatusCode
TCS::MultiplicityCustom::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 1) {
      
     // loop over  jets
     unsigned int nLeading = p_NumberLeading1;

     // counter
     unsigned int nmult = 0;
     // vector of tobs passing cuts
     std::vector<TCS::GenericTOB*> TOBvector;

     for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
           ++tob1) 
         {
          if (parType_t(fabs((*tob1)-> eta())) < p_EtaMin) continue;
          if (parType_t(fabs((*tob1)-> eta())) > p_EtaMax) continue;

          if( parType_t((*tob1)->Et())  <= p_MinET ) continue; // E_T cut
     //
          TRG_MSG_DEBUG("Jet : ET = " << parType_t((*tob1)->Et()));
          ++nmult;
          // keep a list of all TOB that contributed
          TOBvector.push_back( *tob1 );
          
     }
     //
      
     bool accept = nmult >= p_Multiplicity;

     decision.setBit( 0, accept );

     if(accept) output[0]->push_back(TCS::CompositeTOB(TOBvector));
     TRG_MSG_DEBUG("Decision : " << (accept?"pass":"fail") << " Mult = " << nmult);


   } else {

      TCS_EXCEPTION("MultiplicityCustom alg must have  1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
