/*********************************
 * TransverseMassInclusive1.cpp
 * Created by V Sorin 2014.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 * 
 * @brief algorithm calculates the sqr of the transverse MASS between one  list and XE and applies mass criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/TransverseMassInclusive1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(TransverseMassInclusive1)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::TransverseMassInclusive1:     "




namespace {
   unsigned int
   calcTMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;
      
      double cosphi = cos ( dphi);
      double tmass2 = 2*tob1->Et()*tob2->Et()*(1 - cosphi);
      return round( tmass2 );
   }
}


TCS::TransverseMassInclusive1::TransverseMassInclusive1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 6);
   defineParameter("MinMTSqr",  0, 0);
   defineParameter("MinMTSqr",  0, 1);
   defineParameter("MinMTSqr", 0, 2);
   defineParameter("MinMTSqr", 0, 3);
   defineParameter("MinMTSqr", 0, 4);
   defineParameter("MinMTSqr", 0, 5);
   defineParameter("MinET1",0);
   
   setNumberOutputBits(6);
}

TCS::TransverseMassInclusive1::~TransverseMassInclusive1(){}


TCS::StatusCode
TCS::TransverseMassInclusive1::initialize() {
   if(parameter("MaxTob").value() > 0) {
    p_NumberLeading1 = parameter("MaxTob").value();
   } else {
    p_NumberLeading1 = parameter("InputWidth").value();
   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_TMassMin[i] = parameter("MinMTSqr", i).value();
   } 
   p_MinET1 = parameter("MinET1").value();
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    TRG_MSG_INFO("TMassMin   : " << p_TMassMin[i]);
   }
   
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("number output : " << numberOutputBits());
 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::TransverseMassInclusive1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

   if(input.size() == 2) { // 2 lists because one is always MET



      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            
            if( parType_t((*tob1)->Et()) <= p_MinET1) continue; // ET cut
            
            for( TOBArray::const_iterator tob2 = input[1]->begin();
                 tob2 != input[1]->end() ;
                 ++tob2) {


               // T Mass calculation
             
	       unsigned int tmass2 = calcTMass( *tob1, *tob2 );


               bool accept[6];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {


                  accept[i] = tmass2 >= p_TMassMin[i] ; // 
                  if( accept[i] ) {
                     decison.setBit(i, true);  
                     output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                  TRG_MSG_DEBUG("Decision " << i << ": " << (accept[i]?"pass":"fail") << " tmass2 = " << tmass2);

               }
            }
         }
   } else {

      TCS_EXCEPTION("TransverseMassInclusive1 alg must have either 1  inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
