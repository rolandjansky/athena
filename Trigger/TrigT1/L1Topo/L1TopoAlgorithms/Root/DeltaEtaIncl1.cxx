/*********************************
 * DeltaEtaIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V Sorin 2014
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the eta-distance between two TOB from one list and applies delta-eta criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DeltaEtaIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaEtaIncl1)

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


TCS::DeltaEtaIncl1::DeltaEtaIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 0);
   defineParameter("NumResultBits", 1);
   defineParameter("MinEt1",0);
   defineParameter("MinEt2",0); 
   defineParameter("MinDeltaEta",  0, 0);
   defineParameter("MaxDeltaEta", 127, 0);
   setNumberOutputBits(1);
}

TCS::DeltaEtaIncl1::~DeltaEtaIncl1(){}


TCS::StatusCode
TCS::DeltaEtaIncl1::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   p_NumberLeading2 = parameter("InputWidth").value();
   for(int i=0; i<1; ++i) {
      p_DeltaEtaMin[i] = parameter("MinDeltaEta", i).value();
      p_DeltaEtaMax[i] = parameter("MaxDeltaEta", i).value();
   }
   p_MinET1 = parameter("MinEt1").value();
   p_MinET2 = parameter("MinEt2").value();


   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("DeltaEtaMin0   : " << p_DeltaEtaMin[0]);
   TRG_MSG_INFO("DeltaEtaMax0   : " << p_DeltaEtaMax[0]);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaEtaIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

   if(input.size() == 1) {

      //LOG << "input size     : " << input[0]->size() << endl;

      unsigned int nLeading = p_NumberLeading1;
      unsigned int nLeading2 = p_NumberLeading2;

      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
           ++tob1) 
         {
            
            if( parType_t((*tob1)->Et()) <= p_MinET1 ) continue; // ET cut
            
            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < nLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut

               // DeltaEta cuts
               unsigned int deltaEta = calcDeltaEta( *tob1, *tob2 );

               std::stringstream msgss;
               msgss << "Combination : " << distance( input[0]->begin(), tob1) << " x " << distance( input[0]->begin(), tob2) << " eta=" << (*tob1)->eta() << " , eta=" << (*tob2)->eta()
                     << ", DeltaEta = " << deltaEta << " -> ";

               bool accept[1];
               for(unsigned int i=0; i < numberOutputBits(); ++i) {
                  accept[i] = deltaEta >= p_DeltaEtaMin[i] && deltaEta <= p_DeltaEtaMax[i];
                  if( accept[i] ) {
                     decison.setBit(i, true);  
                     output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                  msgss << (accept[i]?"pass":"fail") << "|";
               }
               TRG_MSG_DEBUG(msgss.str());
            }
         }
      

   } else {

      TCS_EXCEPTION("DeltaEtaIncl1 alg must have 1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
