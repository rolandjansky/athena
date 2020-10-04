/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * NotMatch.cpp
 * Created by Joerg Stelzer / V Sorin on 9/16/14.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/NotMatch.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(NotMatch)


// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::NotMatch:     "




namespace {
   unsigned int
   calcDeltaR2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = ( tob1->etaDouble() - tob2->etaDouble() );
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;

      return round ( 100 * ((dphi)*(dphi) + (deta)*(deta) )) ;

   }

   unsigned int
   calcDeltaR2BW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {

      int detaB = abs( tob1->eta() - tob2->eta() );
      int dphiB = abs( tob1->phi() - tob2->phi() );
      if(dphiB>32)
         dphiB = 64 - dphiB;

      unsigned int bit_dr2 = dphiB*dphiB + detaB*detaB;
      return bit_dr2;

   }

}


TCS::NotMatch::NotMatch(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 2);
   defineParameter("MinET1",1,0);
   defineParameter("MinET2",1,0);
   defineParameter("EtaMin1",0,0);
   defineParameter("EtaMax1",49,0);
   defineParameter("EtaMin2",0,0);
   defineParameter("EtaMax2",49,0); 
   defineParameter("DRCut",0,0);
   defineParameter("MinET1",1,1);
   defineParameter("MinET2",1,1);
   defineParameter("EtaMin1",0,1);
   defineParameter("EtaMax1",49,1);
   defineParameter("EtaMin2",0,1);
   defineParameter("EtaMax2",49,1);
   defineParameter("DRCut",0,1);
   setNumberOutputBits(2);
}

TCS::NotMatch::~NotMatch(){}


TCS::StatusCode
TCS::NotMatch::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    p_MinET1[i] = parameter("MinET1",i).value();
    p_MinET2[i] = parameter("MinET2",i).value();
    TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
    TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);

    p_EtaMin1[i] = parameter("EtaMin1",i).value();
    p_EtaMin2[i] = parameter("EtaMin2",i).value();
    TRG_MSG_INFO("MinETa1          : " << p_EtaMin1[i]);
    TRG_MSG_INFO("MinETa2          : " << p_EtaMin2[i]);

    p_EtaMax1[i] = parameter("EtaMax1",i).value();
    p_EtaMax2[i] = parameter("EtaMax2",i).value();
    TRG_MSG_INFO("MinETa1          : " << p_EtaMax1[i]);
    TRG_MSG_INFO("MinETa2          : " << p_EtaMax2[i]);

    p_DRCut[i]   = parameter("DRCut",i).value();
    TRG_MSG_INFO("DRCut          : " << p_DRCut[i]);
   }
   TRG_MSG_INFO("Maxtob 1          : " << p_NumberLeading1);
   TRG_MSG_INFO("Maxtob 2          : " << p_NumberLeading2);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::NotMatch::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
   if( input.size() == 2) {
      bool matched = false;
      unsigned int deltaR2= 999;
      for(unsigned int i=0; i<numberOutputBits(); ++i) {
          bool all_unmatched = true;
	  std::vector<GenericTOB*> unmatched_tobs;
       for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {
            if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
            if( parType_t(std::abs((*tob1)->eta())) > p_EtaMax1[i] ) continue; // Eta cut
            if( parType_t(std::abs((*tob1)->eta())) < p_EtaMin1[i] ) continue; // Eta cut
            matched = false;
            deltaR2 = 999;
            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2 && matched != true ;
                 ++tob2) {
               if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
               if( parType_t(std::abs((*tob2)->eta())) > p_EtaMax2[i] ) continue; // Eta cut
               if( parType_t(std::abs((*tob2)->eta())) < p_EtaMin2[i] ) continue; // Eta cut
               // test DeltaR2Min, DeltaR2Max
               deltaR2 = calcDeltaR2BW( *tob1, *tob2 );
               if (deltaR2 <= p_DRCut[i]) matched = true; 
            } // for(tob2)
            // protection against no jets
            if (deltaR2 == 999) matched = true;
            if(not matched) unmatched_tobs.push_back(*tob1);
            all_unmatched = all_unmatched and not matched;
         } // for(tob1)
       const bool accept = all_unmatched and unmatched_tobs.size()>0;
       if(accept){
           decision.setBit(i, true);
           for(const auto tob : unmatched_tobs)
               output[i]->push_back(TCS::CompositeTOB(tob));
       }
       TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail"));
      } // for(i)
   } else {
      TCS_EXCEPTION("NotMatch alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::NotMatch::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
   if( input.size() == 2) {
      bool matched = false;
      unsigned int deltaR2= 999;
      for(unsigned int i=0; i<numberOutputBits(); ++i) { 
       bool all_unmatched = true;
       std::vector<GenericTOB*> unmatched_tobs;
       for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {
            if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
            if( parType_t(std::abs((*tob1)->eta())) > p_EtaMax1[i] ) continue; // Eta cut
            if( parType_t(std::abs((*tob1)->eta())) < p_EtaMin1[i] ) continue; // Eta cut
            matched = false;
            deltaR2 = 999;
            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2 && matched != true ;
                 ++tob2) {
               if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
               if( parType_t(std::abs((*tob2)->eta())) > p_EtaMax2[i] ) continue; // Eta cut
               if( parType_t(std::abs((*tob2)->eta())) < p_EtaMin2[i] ) continue; // Eta cut
               // test DeltaR2Min, DeltaR2Max
               deltaR2 = calcDeltaR2( *tob1, *tob2 );
               if (deltaR2 <= p_DRCut[i]) matched = true; 
            } // for(tob2)
            // protection against no jets
            if (deltaR2 == 999) matched = true;
            if(not matched) unmatched_tobs.push_back(*tob1);
            all_unmatched = all_unmatched and not matched;
         } // for(tob1)
       const bool accept = all_unmatched and unmatched_tobs.size()>0;
       if(accept){
           decision.setBit(i, true);
           for(const auto tob : unmatched_tobs)
               output[i]->push_back(TCS::CompositeTOB(tob));
       }
       TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail"));
      } // for(i)
   } else {
      TCS_EXCEPTION("NotMatch alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
