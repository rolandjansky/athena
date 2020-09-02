/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * TransverseMassInclusive1.cpp
 * Created by V Sorin 2014.
 * 
 * @brief algorithm calculates the sqr of the transverse MASS between one  list and XE and applies mass criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include "TH1F.h"

#include "L1TopoAlgorithms/TransverseMassInclusive1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"

//
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

   unsigned int
   calcTMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      auto bit_cosphi = TSU::L1TopoDataTypes<9,7>(TSU::Trigo::Cos.at(abs(tob1->phi() - tob2->phi())));
      TSU::L1TopoDataTypes<11,0> bit_Et1(tob1->Et());
      TSU::L1TopoDataTypes<11,0> bit_Et2(tob2->Et());
      TSU::L1TopoDataTypes<22,0> bit_tmass2 = 2*bit_Et1*bit_Et2*(1.  - bit_cosphi);
      return int(bit_tmass2) ;
     // end bitwise implementation
   }
                                                                        
      
}


TCS::TransverseMassInclusive1::TransverseMassInclusive1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 6);
   defineParameter("MinET1",0,0);
   defineParameter("MinET2",0,0);
   defineParameter("MinMTSqr",  0, 0);
   defineParameter("MinET1",0,1);
   defineParameter("MinET2",0,1);
   defineParameter("MinMTSqr",  0, 1);
   defineParameter("MinET1",0,2);
   defineParameter("MinET2",0,2);
   defineParameter("MinMTSqr", 0, 2);
   defineParameter("MinET1",0,3);
   defineParameter("MinET2",0,3);
   defineParameter("MinMTSqr", 0, 3);
   defineParameter("MinET1",0,4);
   defineParameter("MinET2",0,4);
   defineParameter("MinMTSqr", 0, 4);
   defineParameter("MinET1",0,5);
   defineParameter("MinET2",0,5);
   defineParameter("MinMTSqr", 0, 5);
 
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
    
      p_MinET1[i] = parameter("MinET1",i).value();
      TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
      p_MinET2[i] = parameter("MinET2",i).value();
      TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);

      TRG_MSG_INFO("TMassMin   : " << p_TMassMin[i]);
   }
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);   
   TRG_MSG_INFO("number output : " << numberOutputBits());

   for (unsigned int i=0; i<numberOutputBits();i++) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int Tmass_min = p_TMassMin[i];
       int Tmass_max = 1000;
       // Tmass
       snprintf(hname_accept, buf_len, "Accept_TransverseMassInclusive1_bit%d_%dM%d_Mass", i, Tmass_min, Tmass_max);
       snprintf(hname_reject, buf_len, "Reject_TransverseMassInclusive1_bit%d_%dM%d_Mass", i, Tmass_min, Tmass_max);
       registerHist(m_histAcceptM[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*Tmass_max));
       registerHist(m_histRejectM[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*Tmass_max));
  }
 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::TransverseMassInclusive1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 2) { // 2 lists because one is always MET



      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            
            
            for( TOBArray::const_iterator tob2 = input[1]->begin();
                 tob2 != input[1]->end() ;
                 ++tob2) {

               // T Mass calculation
             
	       unsigned int tmass2 = calcTMassBW( *tob1, *tob2 );


               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                   accept = tmass2 >= p_TMassMin[i] ; // 
                   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                   const bool fillReject = fillHistos() and not fillAccept;
                   const bool alreadyFilled = decision.bit(i);
		   if(accept) {
                       decision.setBit(i, true);  
                       output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                   }
                   if(fillAccept and not alreadyFilled) {
		     fillHist1D(m_histAcceptM[i]->GetName(),(float)tmass2);
                   } else if(fillReject) {
		     fillHist1D(m_histRejectM[i]->GetName(),(float)tmass2);
                   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " tmass2 = " << tmass2);
               }
            }
         }
   } else {

      TCS_EXCEPTION("TransverseMassInclusive1 alg must have either 1  inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::TransverseMassInclusive1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 2) { // 2 lists because one is always MET



      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            
            
            for( TOBArray::const_iterator tob2 = input[1]->begin();
                 tob2 != input[1]->end() ;
                 ++tob2) {

               // T Mass calculation
             
	       unsigned int tmass2 = calcTMass( *tob1, *tob2 );

               for(unsigned int i=0; i<numberOutputBits(); ++i) {
		  bool accept = false;
		  if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut

                  if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut

                  accept = tmass2 >= p_TMassMin[i] ; // 
                  const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                  const bool fillReject = fillHistos() and not fillAccept;
                  const bool alreadyFilled = decision.bit(i);
                  if( accept ) {
                     decision.setBit(i, true);  
                     output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                   if(fillAccept and not alreadyFilled) {
		     fillHist1D(m_histAcceptM[i]->GetName(),(float)tmass2);
                   } else if(fillReject) {
		     fillHist1D(m_histRejectM[i]->GetName(),(float)tmass2);
                   }
                  TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " tmass2 = " << tmass2);

               }
            }
         }
   } else {

      TCS_EXCEPTION("TransverseMassInclusive1 alg must have either 1  inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
