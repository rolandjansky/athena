/*********************************
 * DeltaPhiIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V SOrin 2014
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one  lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DeltaPhiIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaPhiIncl1)

using namespace std;

namespace {
   unsigned int
   calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;
      
      return round( 10 * dphi );
   }
}


TCS::DeltaPhiIncl1::DeltaPhiIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 0);
   defineParameter("NumResultBits", 1);
   defineParameter("MinEt1",0);
   defineParameter("MinEt2",0);
   defineParameter("MinDeltaPhi",  0, 0);
   defineParameter("MaxDeltaPhi", 31, 0);
   setNumberOutputBits(1);
}

TCS::DeltaPhiIncl1::~DeltaPhiIncl1(){}


TCS::StatusCode
TCS::DeltaPhiIncl1::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   p_NumberLeading2 = parameter("InputWidth").value();
   for(int i=0; i<1; ++i) {
      p_DeltaPhiMin[i] = parameter("MinDeltaPhi", i).value();
      p_DeltaPhiMax[i] = parameter("MaxDeltaPhi", i).value();
   }
   p_MinET1 = parameter("MinEt1").value();
   p_MinET2 = parameter("MinEt2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("DeltaPhiMin0   : " << p_DeltaPhiMin[0]);
   TRG_MSG_INFO("DeltaPhiMax0   : " << p_DeltaPhiMax[0]);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaPhiIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

   if(input.size() == 1) {

      TRG_MSG_DEBUG("input size     : " << input[0]->size());

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

               // DeltaPhi cuts
               unsigned int deltaPhi = calcDeltaPhi( *tob1, *tob2 );

               std::stringstream msgss;
               msgss << "    Combination : " << distance( input[0]->begin(), tob1) << " x " << distance( input[0]->begin(), tob2) << "  phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                     << ", DeltaPhi = " << deltaPhi << " -> ";

               bool accept[3];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  accept[i] = deltaPhi >= p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i];
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

      TCS_EXCEPTION("DeltaPhiIncl1 alg must have  1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
