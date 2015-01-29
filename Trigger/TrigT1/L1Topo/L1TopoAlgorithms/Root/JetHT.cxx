/*********************************
 * JetHT.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the sum of jets ET, makes cut on HT criteria 
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/JetHT.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(JetHT)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << name() << ":     "


TCS::JetHT::JetHT(const std::string & name) : DecisionAlg(name)
{  
   defineParameter("InputWidth", 0);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits",6);
   defineParameter("NumRegisters", 2); 
   defineParameter("MinEt",0);
   defineParameter("MinEta",0);
   defineParameter("MaxEta",31);
   defineParameter("MinHt",0,0);
   defineParameter("MinHt",0,1);
   defineParameter("MinHt",0,2);
   defineParameter("MinHt",0,3);
   defineParameter("MinHt",0,4);
   defineParameter("MinHt",0,5);
   setNumberOutputBits(6);
}

TCS::JetHT::~JetHT()
{}


TCS::StatusCode
TCS::JetHT::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   if(parameter("MaxTob").value() > 0) p_NumberLeading1 = parameter("MaxTob").value();
   p_MinET  = parameter("MinEt").value();
   p_EtaMin = parameter("MinEta").value();
   p_EtaMax = parameter("MaxEta").value();

   TRG_MSG_INFO("MaxTob          : " << p_NumberLeading1);
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("EtaMin         : " << p_EtaMin);
   TRG_MSG_INFO("EtaMax         : " << p_EtaMax);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_HT[i] = parameter("MinHt", i).value();
      TRG_MSG_INFO("HT " << i << " : " << p_HT[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::JetHT::process( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )
{

   if(input.size()!=1) {
      TCS_EXCEPTION("JetHT alg must have exactly 1 input list, but got " << input.size());
      return TCS::StatusCode::FAILURE;
   }

   unsigned int sumET = 0;

   // loop over all jets
   unsigned int objC(0);
   for( TCS::GenericTOB * tob : *input[0]) {
      ++objC;

      if( parType_t(fabs(tob->eta())) > p_EtaMax ) continue; // Eta cut
      if( parType_t(fabs(tob->eta())) < p_EtaMin ) continue; // Eta cut
      if( tob->Et() <= p_MinET ) continue; // E_T cut

      TRG_MSG_DEBUG("Jet " << objC-1 << ": ET = " << tob->Et());

      sumET += tob->Et();
   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

      bool accept = sumET > p_HT[i];

      decision.setBit( i, accept );

      if(accept)
         output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(sumET,0,0) ) ));

      TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " HT = " << sumET);

   }

   return TCS::StatusCode::SUCCESS;
}
