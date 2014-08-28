/*********************************
 * DeltaPhiIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DeltaPhiIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaPhiIncl1)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::DeltaPhiIncl1:     "




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
   defineParameter("NumberLeading1", 3);
   defineParameter("NumberLeading2", 3); 
   defineParameter("DeltaPhiMin",  0, 0);
   defineParameter("DeltaPhiMax", 63, 0);
   defineParameter("DeltaPhiMin",  0, 1);
   defineParameter("DeltaPhiMax",  5, 1);
   defineParameter("DeltaPhiMin", 25, 2);
   defineParameter("DeltaPhiMax", 32, 2);
   defineParameter("MinET",1);
   setNumberOutputBits(3);
}

TCS::DeltaPhiIncl1::~DeltaPhiIncl1(){}


TCS::StatusCode
TCS::DeltaPhiIncl1::initialize() {
   p_NumberLeading1 = parameter("NumberLeading1").value();
   p_NumberLeading2 = parameter("NumberLeading2").value();
   for(int i=0; i<3; ++i) {
      p_DeltaPhiMin[i] = parameter("DeltaPhiMin", i).value();
      p_DeltaPhiMax[i] = parameter("DeltaPhiMax", i).value();
   }
   p_MinET = parameter("MinET").value();

   LOG << "NumberLeading1 : " << p_NumberLeading1 << endl;  // note that the reading of generic parameters doesn't work yet
   LOG << "NumberLeading2 : " << p_NumberLeading2 << endl;
   LOG << "DeltaPhiMin0   : " << p_DeltaPhiMin[0] << endl;
   LOG << "DeltaPhiMax0   : " << p_DeltaPhiMax[0] << endl;
   LOG << "DeltaPhiMin1   : " << p_DeltaPhiMin[1] << endl;
   LOG << "DeltaPhiMax1   : " << p_DeltaPhiMax[1] << endl;
   LOG << "DeltaPhiMin2   : " << p_DeltaPhiMin[2] << endl;
   LOG << "DeltaPhiMax2   : " << p_DeltaPhiMax[2] << endl;
   LOG << "MinET          : " << p_MinET << endl;
   LOG << "nummber output : " << numberOutputBits() << endl;
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaPhiIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

   if(input.size() == 1) {

      LOG << "input size     : " << input[0]->size() << endl;

      unsigned int nLeading = p_NumberLeading1;

      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
           ++tob1) 
         {
            
            if( parType_t((*tob1)->Et()) < p_MinET ) continue; // ET cut
            
            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < nLeading;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) < p_MinET) continue; // ET cut

               // DeltaPhi cuts
               unsigned int deltaPhi = calcDeltaPhi( *tob1, *tob2 );

               LOG << "    Combination : " << distance( input[0]->begin(), tob1) << " x " << distance( input[0]->begin(), tob2) << "  phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                   << ", DeltaPhi = " << deltaPhi << " -> ";

               bool accept[3];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  accept[i] = deltaPhi > p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i];
                  if( accept[i] ) {
                     decison.setBit(i, true);  
                     output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                  cout << (accept[i]?"pass":"fail") << "|";
               }
               cout << endl;
            }
         }
      
   } else if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) <= p_NumberLeading1;
           ++tob1)
         {

            if( parType_t((*tob1)->Et()) < p_MinET) continue; // ET cut

            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) <= p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) < p_MinET) continue; // ET cut

               // test DeltaPhiMin, DeltaPhiMax
               unsigned int deltaPhi = calcDeltaPhi( *tob1, *tob2 );

               bool accept[3];
               for(unsigned int i=0; i<3; ++i) {
                  accept[i] = deltaPhi > p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i];
                  if( accept[i] ) {
                     decison.setBit(i, true);
                     output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                  }
               }
               LOG << "      DeltaPhi = " << deltaPhi << " -> " 
                   << (accept[0]?"pass":"fail") << "|"
                   << (accept[1]?"pass":"fail") << "|"
                   << (accept[2]?"pass":"fail") << endl;
            }
         }

   } else {

      TCS_EXCEPTION("DeltaPhiIncl1 alg must have either 1 or 2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
