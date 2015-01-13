/*********************************
 * InvariantMassInclusive2.cpp
 * Created by V SOrin 2014.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the sqr of the INVMASS between  two lists and applies invmass criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/InvariantMassInclusive2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(InvariantMassInclusive2)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::InvariantMassInclusive2:     "




namespace {
   unsigned int
   calcInvMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = fabs( tob1->etaDouble() - tob2->etaDouble() );
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;

      double cosheta = cosh ( deta);
      double cosphi = cos ( dphi);
      double invmass2 = 2*tob1->Et()*tob2->Et()*(cosheta - cosphi);
      return round( invmass2 );
   }
}


TCS::InvariantMassInclusive2::InvariantMassInclusive2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("NumberLeading1", 6);
   defineParameter("NumberLeading2", 6); 
   defineParameter("NumResultBits", 3);
   defineParameter("InvMassMin",  0, 0);
   defineParameter("InvMassMax", 999, 0);
   defineParameter("InvMassMin",  0, 1);
   defineParameter("InvMassMax",  999, 1);
   defineParameter("InvMassMin", 0, 2);
   defineParameter("InvMassMax", 999, 2);
   defineParameter("MinET1",0);
   defineParameter("MinET2",0);
   setNumberOutputBits(3);
}

TCS::InvariantMassInclusive2::~InvariantMassInclusive2(){}


TCS::StatusCode
TCS::InvariantMassInclusive2::initialize() {
   p_NumberLeading1 = parameter("NumberLeading1").value();
   p_NumberLeading2 = parameter("NumberLeading2").value();
   for(int i=0; i<3; ++i) {
      p_InvMassMin[i] = parameter("InvMassMin", i).value();
      p_InvMassMax[i] = parameter("InvMassMax", i).value();
   }
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);  
   for(int i=0; i<3; ++i) {
    TRG_MSG_INFO("InvMassMin   : " << p_InvMassMin[i]);
    TRG_MSG_INFO("InvMassMax   : " << p_InvMassMax[i]);

   }
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);

   TRG_MSG_INFO("number output : " << numberOutputBits());
 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::InvariantMassInclusive2::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

      
   if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {

            if( parType_t((*tob1)->Et()) <= p_MinET1) continue; // ET cut

            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut
               
	       // Inv Mass calculation

	       unsigned int invmass2 = calcInvMass( *tob1, *tob2 );


               bool accept[3];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  accept[i] = invmass2 >= p_InvMassMin[i]*p_InvMassMin[i] && invmass2 <= p_InvMassMax[i]*p_InvMassMax[i]; // to-do : confirm that  param is InvM, and not sq
                  if( accept[i] ) {
                     decison.setBit(i, true);
                     output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                  TRG_MSG_DEBUG("Decision " << i << ": " << (accept[i]?"pass":"fail") << " invmass2 = " << invmass2);

               }
  


	    }
         }

   } else {

      TCS_EXCEPTION("InvariantMassInclusive2 alg must have  2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
