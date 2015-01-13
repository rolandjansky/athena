/*********************************
 * DisambiguationIncl2.cpp
 * Created by Joerg Stelzer / V Sorin on 2014.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the dR distance between objects in two lists, accept if at least a pair does not match in dR 
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DisambiguationIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DisambiguationIncl2)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::DisambiguationIncl2:     "




namespace {
   unsigned int
   calcDeltaR2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = ( tob1->etaDouble() - tob2->etaDouble() );
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;

      return round ( 100 * ((dphi)*(dphi) + (deta)*(deta) )) ;

   }
}


TCS::DisambiguationIncl2::DisambiguationIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("NumberLeading1", 3);
   defineParameter("NumberLeading2", 3); 
   defineParameter("NumResultBits", 2);
   defineParameter("MinET1",1);
   defineParameter("MinET2",1);
   defineParameter("EtaMin1",0);
   defineParameter("EtaMax1",49);
   defineParameter("EtaMin2",0);
   defineParameter("EtaMax2",49);
   defineParameter("DisambDR",0,0);
   defineParameter("DisambDR",0,1); 
   setNumberOutputBits(2);
}

TCS::DisambiguationIncl2::~DisambiguationIncl2(){}


TCS::StatusCode
TCS::DisambiguationIncl2::initialize() {
   p_NumberLeading1 = parameter("NumberLeading1").value();
   p_NumberLeading2 = parameter("NumberLeading2").value();
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();
   p_EtaMin1 = parameter("EtaMin1").value();
   p_EtaMax1 = parameter("EtaMax1").value();
   p_EtaMin2 = parameter("EtaMin2").value();
   p_EtaMax2 = parameter("EtaMax2").value();
   for(int i=0; i<2; ++i) {
      p_DisambDR[i] = parameter("DisambDR", i).value();
   }



   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("EtaMin1         : " << p_EtaMin1);
   TRG_MSG_INFO("EtaMax1         : " << p_EtaMax1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("EtaMin2         : " << p_EtaMin2);
   TRG_MSG_INFO("EtaMax2         : " << p_EtaMax2);
   TRG_MSG_INFO("DisambDR0         : " << p_DisambDR[0]);
   TRG_MSG_INFO("DisambDR1         : " << p_DisambDR[1]);
   TRG_MSG_INFO("number output : " << numberOutputBits());


   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DisambiguationIncl2::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

      
   if( input.size() == 2) {

      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {

            if( parType_t((*tob1)->Et()) <= p_MinET1) continue; // ET cut
            if( parType_t(fabs((*tob1)->eta())) > p_EtaMax1 ) continue; // Eta cut
            if( parType_t(fabs((*tob1)->eta())) < p_EtaMin1 ) continue; // Eta cut

            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut
               if( parType_t(fabs((*tob2)->eta())) > p_EtaMax2 ) continue; // Eta cut
               if( parType_t(fabs((*tob2)->eta())) < p_EtaMin2 ) continue; // Eta cut

               // test DeltaR2Min, DeltaR2Max
               unsigned int deltaR2 = calcDeltaR2( *tob1, *tob2 );
               
	       bool accept[3];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  accept[i] = deltaR2 > p_DisambDR[i]*p_DisambDR[i] ;
                  if( accept[i] ) {
                     decision.setBit(i, true);
                     output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                  }
                  TRG_MSG_DEBUG("Decision " << i << ": " << (accept[i]?"pass":"fail") << " deltaR2 = " << deltaR2);

               }

            }



         }


   } else {

      TCS_EXCEPTION("DisambiguationIncl2 alg must have  2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
