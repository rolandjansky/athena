/*********************************
 * DeltaEtaTestIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DeltaEtaTestIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaEtaTestIncl1)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << fullname() << ":     "


namespace {
   unsigned int
   calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = fabs( tob1->eta() - tob2->eta() );
      return deta;
   }
}


TCS::DeltaEtaTestIncl1::DeltaEtaTestIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 0);
   defineParameter("MinEt1",0);
   defineParameter("MinEt2",0);
   defineParameter("EtaMin1",0);
   defineParameter("EtaMax1",0);
   defineParameter("EtaMin2",0);
   defineParameter("EtaMax2",0);
   defineParameter("DeltaEtaMin",  0);
   defineParameter("DeltaEtaMax", 63);
   setNumberOutputBits(1);
}

TCS::DeltaEtaTestIncl1::~DeltaEtaTestIncl1(){}


TCS::StatusCode
TCS::DeltaEtaTestIncl1::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   p_DeltaEtaMin = parameter("DeltaEtaMin").value();
   p_DeltaEtaMax = parameter("DeltaEtaMax").value();
   
   p_MinET1 = parameter("MinEt1").value();
   p_MinET2 = parameter("MinEt2").value();
   p_EtaMin1 = parameter("EtaMin1").value();
   p_EtaMax1 = parameter("EtaMax1").value();
   p_EtaMin2 = parameter("EtaMin2").value();
   p_EtaMax2 = parameter("EtaMax2").value();


   TRG_MSG_INFO("InputWidth : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("DeltaEtaMin1   : " << p_DeltaEtaMin);
   TRG_MSG_INFO("DeltaEtaMax1   : " << p_DeltaEtaMax);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("EtaMin1          : " << p_EtaMin1);
   TRG_MSG_INFO("EtaMax1          : " << p_EtaMax1);
   TRG_MSG_INFO("EtaMin2          : " << p_EtaMin2);
   TRG_MSG_INFO("EtaMax2          : " << p_EtaMax2);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaEtaTestIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

   if(input.size() == 1) {

      //LOG << "input size     : " << input[0]->size() << endl;

      unsigned int nLeading = p_NumberLeading1;

      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
           ++tob1) 
         {
            
            if( parType_t((*tob1)->Et()) < p_MinET1 ) continue; // ET cut
            if( parType_t(fabs((*tob1)->etaDouble())) > p_EtaMax1 ) continue; // Eta cut
            if( parType_t(fabs((*tob1)->etaDouble())) < p_EtaMin1 ) continue; // Eta cut
 
            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < nLeading;
                 ++tob2) {

	       if( parType_t((*tob2)->Et()) < p_MinET2 ) continue; // ET cut
               if( parType_t(fabs((*tob2)->etaDouble())) > p_EtaMax2 ) continue; // Eta cut
               if( parType_t(fabs((*tob2)->etaDouble())) < p_EtaMin2 ) continue; // Eta cut
		
               // DeltaEta cuts
               unsigned int deltaEta = calcDeltaEta( *tob1, *tob2 );

               std::stringstream msgss;
               msgss << "Combination : " << distance( input[0]->begin(), tob1) << " x " << distance( input[0]->begin(), tob2) << "  eta1=" << (*tob1)->eta() << " , eta2=" << (*tob2)->eta()
                     << ", DeltaEta = " << deltaEta << " -> ";

               bool accept;
               accept = deltaEta >= p_DeltaEtaMin && deltaEta <= p_DeltaEtaMax;
               if( accept) {
                   decison.setBit(0, true);  
                   output[0]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
               }
               msgss << (accept?"pass":"fail") << "|";
               TRG_MSG_DEBUG(msgss.str());
            }
         }
      

   } else {

      TCS_EXCEPTION("DeltaEtaTestIncl1 alg must have  1  inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
