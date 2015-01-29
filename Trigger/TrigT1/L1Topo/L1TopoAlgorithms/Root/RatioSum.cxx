/*********************************
 * RatioSum.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/RatioSum.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(RatioSum)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << name() << ":     "


TCS::RatioSum::RatioSum(const std::string & name) : DecisionAlg(name)
{  
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 2);
   defineParameter("MinET1",0);
   defineParameter("MinET2",0);
   defineParameter("EtaMin1",0);
   defineParameter("EtaMax1",49);
   defineParameter("EtaMin2",0);
   defineParameter("EtaMax2",49);
   defineParameter("HT",0,0);
   defineParameter("HT",0,1);
   defineParameter("SUM",0,0);
   defineParameter("SUM",0,1);
   defineParameter("Ratio",0,0);
   defineParameter("Ratio",0,1);
   setNumberOutputBits(2);
}

TCS::RatioSum::~RatioSum()
{}


TCS::StatusCode
TCS::RatioSum::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();
   p_MinET1  = parameter("MinET1").value();
   p_MinET2  = parameter("MinET2").value();
   p_EtaMin1 = parameter("EtaMin1").value();
   p_EtaMax1 = parameter("EtaMax1").value();
   p_EtaMin2 = parameter("EtaMin2").value();
   p_EtaMax2 = parameter("EtaMax2").value();


   TRG_MSG_INFO("Maxtob 1          : " << p_NumberLeading1);
   TRG_MSG_INFO("Maxtob 2          : " << p_NumberLeading2);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("EtaMin1         : " << p_EtaMin1);
   TRG_MSG_INFO("EtaMax1         : " << p_EtaMax1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("EtaMin2         : " << p_EtaMin2);
   TRG_MSG_INFO("EtaMax2         : " << p_EtaMax2);

   for(int i=0; i<2; ++i) {
      p_HT[i] = parameter("HT", i).value();
      p_SUM[i] = parameter("SUM", i).value();
      p_Ratio[i] = parameter("Ratio", i).value();

      TRG_MSG_INFO("Ratio " << i << " : " << p_Ratio[i]);
      TRG_MSG_INFO("HT " << i << " : " << p_HT[i]);
      TRG_MSG_INFO("SUM " << i << " : " << p_SUM[i]);
   }
   TRG_MSG_INFO("nummber output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::RatioSum::process( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )
{

   if(input.size()!=3) {
      TCS_EXCEPTION("RatioSum alg must have exactly 3 input lists, but got " << input.size());
      return TCS::StatusCode::FAILURE;
   }

   unsigned int sumET = 0;
   unsigned int sumET2 = 0;   
   unsigned int nLeading1 = p_NumberLeading1;


   const TCS::GenericTOB & met = (*input[0])[0];
   
   // loop over all jets
   unsigned int objC(0);
   for( TCS::GenericTOB * tob : *input[2]) {
      

  
      if( parType_t(fabs(tob->eta())) > p_EtaMax2 ) continue; // Eta cut
      if( parType_t(fabs(tob->eta())) < p_EtaMin2 ) continue; // Eta cut
      if( tob->Et() <= p_MinET2 ) continue; // E_T cut

      TRG_MSG_DEBUG("Jet : ET = " << tob->Et());
      ++objC;
      sumET2 += tob->Et();
   }

   sumET = sumET2;

   for( TOBArray::const_iterator tob1 = input[1]->begin(); 
           tob1 != input[1]->end() && distance( input[1]->begin(), tob1) < nLeading1;
           ++tob1) 
         {
	
          if( parType_t(fabs((*tob1)->eta())) > p_EtaMax1 ) continue; // Eta cut
          if( parType_t(fabs((*tob1)->eta())) < p_EtaMin1 ) continue; // Eta cut
          if( (*tob1)->Et() <= p_MinET1 ) continue; // E_T cut
        
          sumET += (*tob1)->Et() ;

   }
   
   for(unsigned int i=0; i<numberOutputBits(); ++i) {

      bool accept = objC!=0 && sumET!=sumET2 && sumET2 > p_HT[i] && sumET > p_SUM[i] && 10*met.Et() >= p_Ratio[i]*sumET;

      decision.setBit( i, accept );

      if(accept)
         output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(sumET,0,0) ) ));

      TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " HT = " << sumET2 << " SUM = " << sumET);

   }

   return TCS::StatusCode::SUCCESS;
}
