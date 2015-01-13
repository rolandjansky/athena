/*********************************
 * DeltaRSqrIncl2.cpp
 * Created by Joerg Stelzer / V Sorin on 9/16/14.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the R2-distance between two lists and applies dR criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DeltaRSqrIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaRSqrIncl2)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::DeltaRSqrIncl2:     "




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


TCS::DeltaRSqrIncl2::DeltaRSqrIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("NumberLeading1", 3);
   defineParameter("NumberLeading2", 3); 
   defineParameter("NumResultBits", 3);
   defineParameter("DeltaRMin",  0, 0);
   defineParameter("DeltaRMax",  0, 0);
   defineParameter("DeltaRMin",  0, 1);
   defineParameter("DeltaRMax",  0, 1);
   defineParameter("DeltaRMin",  0, 2);
   defineParameter("DeltaRMax",  0, 2);
   defineParameter("MinET1",1);
   defineParameter("MinET2",1);
   setNumberOutputBits(3);
}

TCS::DeltaRSqrIncl2::~DeltaRSqrIncl2(){}


TCS::StatusCode
TCS::DeltaRSqrIncl2::initialize() {
   p_NumberLeading1 = parameter("NumberLeading1").value();
   p_NumberLeading2 = parameter("NumberLeading2").value();
   for(int i=0; i<3; ++i) {
      p_DeltaRMin[i] = parameter("DeltaRMin", i).value();
      p_DeltaRMax[i] = parameter("DeltaRMax", i).value();
   }
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);  
   for(int i=0; i<3; ++i) {
      TRG_MSG_INFO("DeltaRMin" << i << "  : " << p_DeltaRMin[i]);
      TRG_MSG_INFO("DeltaRMax" << i << "  : " << p_DeltaRMax[i]);
   }
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);

   TRG_MSG_INFO("number output : " << numberOutputBits());

   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaRSqrIncl2::process( const std::vector<TCS::TOBArray const *> & input,
                              const std::vector<TCS::TOBArray *> & output,
                              Decision & decison )
{

   if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {

            if( parType_t((*tob1)->Et()) < p_MinET1) continue; // ET cut

            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) < p_MinET2) continue; // ET cut

               // test DeltaR2Min, DeltaR2Max
               unsigned int deltaR2 = calcDeltaR2( *tob1, *tob2 );

               TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", deltaR2 = " << deltaR2);

               bool accept[3];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  accept[i] = deltaR2 >= (p_DeltaRMin[i]*p_DeltaRMin[i]) && deltaR2 <= (p_DeltaRMax[i]*p_DeltaRMax[i]);
                  if( accept[i] ) {
                     decison.setBit(i, true);
                     output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                  }
                  TRG_MSG_DEBUG("Decision " << i << ": " << (accept[i]?"pass":"fail") << " deltaR2 = " << deltaR2);
               }
            }
         }

   } else {

      TCS_EXCEPTION("DeltaRSqrIncl2 alg must have  2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
