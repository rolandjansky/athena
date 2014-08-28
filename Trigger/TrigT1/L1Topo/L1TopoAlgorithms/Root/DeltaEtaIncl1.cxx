/*********************************
 * DeltaEtaIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
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
#define LOG cout << "TCS::DeltaEtaIncl1:     "




namespace {
   unsigned int
   calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = fabs( tob1->eta() - tob2->eta() );
      return deta;
   }
}


TCS::DeltaEtaIncl1::DeltaEtaIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("NumberLeading1", 0);
   defineParameter("NumberLeading2", 0); 
   defineParameter("DeltaEtaMin",  0, 0);
   defineParameter("DeltaEtaMax", 63, 0);
   defineParameter("DeltaEtaMin",  0, 1);
   defineParameter("DeltaEtaMax",  5, 1);
   defineParameter("DeltaEtaMin", 25, 2);
   defineParameter("DeltaEtaMax", 32, 2);
   defineParameter("MinET",0);
   setNumberOutputBits(3);
}

TCS::DeltaEtaIncl1::~DeltaEtaIncl1(){}


TCS::StatusCode
TCS::DeltaEtaIncl1::initialize() {
   p_NumberLeading1 = parameter("NumberLeading1").value();
   p_NumberLeading2 = parameter("NumberLeading2").value();
   for(int i=0; i<3; ++i) {
      p_DeltaEtaMin[i] = parameter("DeltaEtaMin", i).value();
      p_DeltaEtaMax[i] = parameter("DeltaEtaMax", i).value();
   }
   p_MinET = parameter("MinET").value();

   LOG << "NumberLeading1 : " << p_NumberLeading1 << endl;  // note that the reading of generic parameters doesn't work yet
   LOG << "NumberLeading2 : " << p_NumberLeading2 << endl;
   LOG << "DeltaEtaMin0   : " << p_DeltaEtaMin[0] << endl;
   LOG << "DeltaEtaMax0   : " << p_DeltaEtaMax[0] << endl;
   LOG << "DeltaEtaMin1   : " << p_DeltaEtaMin[1] << endl;
   LOG << "DeltaEtaMax1   : " << p_DeltaEtaMax[1] << endl;
   LOG << "DeltaEtaMin2   : " << p_DeltaEtaMin[2] << endl;
   LOG << "DeltaEtaMax2   : " << p_DeltaEtaMax[2] << endl;
   LOG << "MinET          : " << p_MinET << endl;
   LOG << "nummber output : " << numberOutputBits() << endl;
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaEtaIncl1::process( const std::vector<TCS::TOBArray const *> & input,
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

               // DeltaEta cuts
               unsigned int deltaEta = calcDeltaEta( *tob1, *tob2 );

               LOG << "    Combination : " << distance( input[0]->begin(), tob1) << " x " << distance( input[0]->begin(), tob2) << "  phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                   << ", DeltaEta = " << deltaEta << " -> ";

               bool accept[3];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  accept[i] = deltaEta > p_DeltaEtaMin[i] && deltaEta <= p_DeltaEtaMax[i];
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

               // test DeltaEtaMin, DeltaEtaMax
               unsigned int deltaEta = calcDeltaEta( *tob1, *tob2 );

               bool accept[3];
               for(unsigned int i=0; i<3; ++i) {
                  accept[i] = deltaEta > p_DeltaEtaMin[i] && deltaEta <= p_DeltaEtaMax[i];
                  if( accept[i] ) {
                     decison.setBit(i, true);
                     output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                  }
               }
               LOG << "      DeltaEta = " << deltaEta << " -> " 
                   << (accept[0]?"pass":"fail") << "|"
                   << (accept[1]?"pass":"fail") << "|"
                   << (accept[2]?"pass":"fail") << endl;
            }
         }

   } else {

      TCS_EXCEPTION("DeltaEtaIncl1 alg must have either 1 or 2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
