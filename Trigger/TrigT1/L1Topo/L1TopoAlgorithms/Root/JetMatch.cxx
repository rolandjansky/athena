/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * JetMatch.cpp
 * Created by V.Sorin, Joerg Stelzer on 11/16/12.
 *
 * @brief algorithm calculates mult of jets passing cuts on 8x8 and 4x4 sizes, cut on mult of jets 
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/JetMatch.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(JetMatch)


// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << name() << ":     "


TCS::JetMatch::JetMatch(const std::string & name) : DecisionAlg(name)
{  
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 1);
   defineParameter("MinET1",0);
   defineParameter("MinET2",0);
   defineParameter("EtaMin",0);
   defineParameter("EtaMax",49);
   setNumberOutputBits(1);
}

TCS::JetMatch::~JetMatch()
{}


TCS::StatusCode
TCS::JetMatch::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   if(parameter("MaxTob").value() > 0) p_NumberLeading1 = parameter("MaxTob").value();
   p_MinET1  = parameter("MinET1").value();
   p_MinET2  = parameter("MinET2").value();
   p_EtaMin = parameter("EtaMin").value();
   p_EtaMax = parameter("EtaMax").value();
   
   TRG_MSG_INFO("MaxTob          : " << p_NumberLeading1);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("EtaMin         : " << p_EtaMin);
   TRG_MSG_INFO("EtaMax         : " << p_EtaMax);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::JetMatch::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}


TCS::StatusCode
TCS::JetMatch::process( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )
{

   if(input.size()!=1) {
      TCS_EXCEPTION("JetMatch alg must have exactly 1 input list, but got " << input.size());
   }

   // vector of passing tob
   std::vector<TCS::GenericTOB*> TOBvector;

   // loop over all jets
   unsigned int objC(0);
   for( TCS::GenericTOB * tob : *input[0]) {
      ++objC;

      if( parType_t(std::abs(tob->eta())) > p_EtaMax ) continue; // Eta cut
      if( parType_t(std::abs(tob->eta())) < p_EtaMin ) continue; // Eta cut
      if( tob->EtWide() <= p_MinET1 ) continue; // E_T cut

      if( tob->EtNarrow() <= p_MinET2 ) continue; // E_T cut

      TRG_MSG_DEBUG("Jet " << objC-1 << ": ETWide = " << tob->EtWide() << ", ETNarrow = " << tob->EtNarrow());

      // keep a list of all TOB that contributed
      TOBvector.push_back( tob );
      
   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

      bool accept = TOBvector.size() >= p_NumberLeading1 ;

      decision.setBit( i, accept );

      if(accept)
         output[i]->push_back( TCS::CompositeTOB(TOBvector));

      TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " Njets = " << TOBvector.size());

   }

   return TCS::StatusCode::SUCCESS;
}
