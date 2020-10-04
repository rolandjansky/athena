/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * Ratio.cpp
 * Created by Joerg Stelzer on 11/16/12.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/Ratio.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(Ratio)

// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << name() << ":     "


TCS::Ratio::Ratio(const std::string & name) : DecisionAlg(name)
{  
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0);
   defineParameter("NumResultBits", 2);
   defineParameter("isXE2",0);
   defineParameter("MinET2",0);
   defineParameter("EtaMin",0);
   defineParameter("EtaMax",49);
   defineParameter("MinET1",0);
   defineParameter("HT",0);
   defineParameter("Ratio",0,0);
   defineParameter("Ratio",0,1);
   setNumberOutputBits(2);
}

TCS::Ratio::~Ratio()
{}


TCS::StatusCode
TCS::Ratio::initialize() {
   p_MinET1  = parameter("MinET1").value();
   p_MinET2  = parameter("MinET2").value();
   p_EtaMin = parameter("EtaMin").value();
   p_EtaMax = parameter("EtaMax").value();
   p_HT     = parameter("HT").value();
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("EtaMin         : " << p_EtaMin);
   TRG_MSG_INFO("EtaMax         : " << p_EtaMax);
   TRG_MSG_INFO("HT             : " << p_HT);

   p_isXE2 = parameter("isXE2").value();
   TRG_MSG_INFO("isXE2             : " << p_isXE2);

   for(int i=0; i<2; ++i) {
      p_Ratio[i] = parameter("Ratio", i).value();
      TRG_MSG_INFO("Ratio " << i << " : " << p_Ratio[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::Ratio::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}


TCS::StatusCode
TCS::Ratio::process( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )
{

   if(input.size()!=2) {
      TCS_EXCEPTION("Ratio alg must have exactly 2 input lists, but got " << input.size());
   }

   unsigned int sumET = 0;


   const TCS::GenericTOB & met = (*input[0])[0];


   // loop over all jets
   unsigned int objC(0);
   for( TCS::GenericTOB * tob : *input[1]) {
      

      if( parType_t(std::abs(tob->eta())) > p_EtaMax ) continue; // Eta cut
      if( parType_t(std::abs(tob->eta())) < p_EtaMin ) continue; // Eta cut
      if( tob->Et() <= p_MinET2 ) continue; // E_T cut

      TRG_MSG_DEBUG("Jet  ET = " << tob->Et());
      ++objC;
      sumET += tob->Et();
   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

      bool accept = objC!=0 && met.Et() > p_MinET1 && 10*met.Et() >= p_Ratio[i]*sumET;

      if (p_isXE2) accept = objC!=0 && met.Et() > p_MinET1 && 10*(met.Et()*met.Et()) >= p_Ratio[i]*sumET;

      decision.setBit( i, accept );

      if(accept)
         output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(sumET,0,0) ) ));

      TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " HT = " << sumET << " XE = " << met.Et() );

   }

   return TCS::StatusCode::SUCCESS;
}
