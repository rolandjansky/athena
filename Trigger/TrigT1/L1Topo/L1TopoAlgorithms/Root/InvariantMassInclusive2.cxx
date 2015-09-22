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
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
//

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

   unsigned int
   calcInvMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      auto bit_cosheta = TSU::L1TopoDataTypes<19,7>(TSU::Hyperbolic::Cosh.at(abs(tob1->eta() - tob2->eta())));
      auto bit_cosphi = TSU::L1TopoDataTypes<9,7>(TSU::Trigo::Cos.at(abs(tob1->phi() - tob2->phi())));
      TSU::L1TopoDataTypes<11,0> bit_Et1(tob1->Et());
      TSU::L1TopoDataTypes<11,0> bit_Et2(tob2->Et());
      auto bit_invmass2 = 2*bit_Et1*bit_Et2*(bit_cosheta - bit_cosphi);
      
      return int(bit_invmass2) ;
  
   }
  
}


TCS::InvariantMassInclusive2::InvariantMassInclusive2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 6);
   defineParameter("MinMSqr",  0, 0);
   defineParameter("MaxMSqr", 999, 0);
   defineParameter("MinMSqr",  0, 1);
   defineParameter("MaxMSqr",  999, 1);
   defineParameter("MinMSqr", 0, 2);
   defineParameter("MaxMSqr", 999, 2);
   defineParameter("MinMSqr",  0, 3);
   defineParameter("MaxMSqr", 999, 3);
   defineParameter("MinMSqr",  0, 4);
   defineParameter("MaxMSqr",  999, 4);
   defineParameter("MinMSqr", 0, 5);
   defineParameter("MaxMSqr", 999, 5);
   defineParameter("MinET1",0,0);
   defineParameter("MinET2",0,0);
   defineParameter("MinET1",0,1);
   defineParameter("MinET2",0,1);
   defineParameter("MinET1",0,2);
   defineParameter("MinET2",0,2);
   defineParameter("MinET1",0,3);
   defineParameter("MinET2",0,3);
   defineParameter("MinET1",0,4);
   defineParameter("MinET2",0,4);
   defineParameter("MinET1",0,5);
   defineParameter("MinET2",0,5);

   setNumberOutputBits(6);
}

TCS::InvariantMassInclusive2::~InvariantMassInclusive2(){}


TCS::StatusCode
TCS::InvariantMassInclusive2::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_InvMassMin[i] = parameter("MinMSqr", i).value();
      p_InvMassMax[i] = parameter("MaxMSqr", i).value();
   
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();
   }
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);  
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    TRG_MSG_INFO("InvMassMin   : " << p_InvMassMin[i]);
    TRG_MSG_INFO("InvMassMax   : " << p_InvMassMax[i]);
    TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
    TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);

   }

   TRG_MSG_INFO("number output : " << numberOutputBits());
 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::InvariantMassInclusive2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

      
   if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               
	       // Inv Mass calculation

	       unsigned int invmass2 = calcInvMassBW( *tob1, *tob2 );


               bool accept[6];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                  if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut

               
                  accept[i] = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i]; 
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


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               
	       // Inv Mass calculation

	       unsigned int invmass2 = calcInvMass( *tob1, *tob2 );


               bool accept[6];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                  if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut

               
                  accept[i] = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i]; 
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
