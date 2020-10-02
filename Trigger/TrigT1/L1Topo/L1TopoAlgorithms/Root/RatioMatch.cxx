/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * RatioMatch.cpp
 * Created by Joerg Stelzer on 11/16/12.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/RatioMatch.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(RatioMatch)


// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << name() << ":     "

/* NOT USED
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
*/


TCS::RatioMatch::RatioMatch(const std::string & name) : DecisionAlg(name)
{ 
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0);
   defineParameter("NumResultBits", 2);
   defineParameter("MinET1",0);
   defineParameter("MinET2",0);
   // FW version assumes clusters so no deltar, also no eta
   //defineParameter("EtaMin",0);
   //defineParameter("EtaMax",49);
   //defineParameter("DeltaR",0);
   defineParameter("Ratio",0,0);
   defineParameter("Ratio",0,1);
   setNumberOutputBits(2);
}

TCS::RatioMatch::~RatioMatch()
{}


TCS::StatusCode
TCS::RatioMatch::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

   p_MinET1  = parameter("MinET1").value();
   p_MinET2  = parameter("MinET2").value();
//   p_EtaMin = parameter("EtaMin").value();
//   p_EtaMax = parameter("EtaMax").value();
//   p_DeltaR     = parameter("DeltaR").value();
   

   TRG_MSG_INFO("Maxtob 1          : " << p_NumberLeading1);
   TRG_MSG_INFO("Maxtob 2          : " << p_NumberLeading2);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
//   TRG_MSG_INFO("EtaMin         : " << p_EtaMin);
//   TRG_MSG_INFO("EtaMax         : " << p_EtaMax);
//   TRG_MSG_INFO("DeltaR          : " << p_DeltaR);

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_Ratio[i] = parameter("Ratio", i).value();
      TRG_MSG_INFO("Ratio " << i << " : " << p_Ratio[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::RatioMatch::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}

TCS::StatusCode
TCS::RatioMatch::process( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )
{

   if(input.size()!=2) {
      TCS_EXCEPTION("RatioMatch alg must have exactly 2 input lists, but got " << input.size());
   }

   unsigned int deltaR2 = 999;

   for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {

            if( parType_t((*tob1)->Et()) <= p_MinET1) continue; // ET cut
            deltaR2 = 999;

            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut
             //  if( parType_t(fabs((*tob2)->eta())) > p_EtaMax ) continue; // Eta cut
             //  if( parType_t(fabs((*tob2)->eta())) < p_EtaMin ) continue; // Eta cut

	       // test DeltaR2Min, DeltaR2Max
               //deltaR2 = calcDeltaR2( *tob1, *tob2 );
               // test Deltaeta, Deltaphi
               //                
               if (((*tob1)->eta() != (*tob2)->eta()) || ((*tob1)->phi() != (*tob2)->phi()) ) continue; // EM does not matches TAU
               //
	       //
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   accept =  10*parType_t((*tob1)->Et()) >= p_Ratio[i]*parType_t((*tob2)->Et());
//                   accept = deltaR2 <= p_DeltaR && 10*parType_t((*tob1)->Et()) >= p_Ratio[i]*parType_t((*tob2)->Et());
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2  );
               }
            }
        }
   return TCS::StatusCode::SUCCESS;
}
