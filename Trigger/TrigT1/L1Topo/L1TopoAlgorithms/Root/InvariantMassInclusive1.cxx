/*********************************
 * InvariantMassInclusive1.cpp
 * Created by V Sorin 2014.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 * 
 * @brief algorithm calculates the sqr of the INVMASS between TOBs from one  list and applies invmass criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "TH1F.h"

#include "L1TopoAlgorithms/InvariantMassInclusive1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"

//

REGISTER_ALG_TCS(InvariantMassInclusive1)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::InvariantMassInclusive1:     "

TCS::InvariantMassInclusive1::InvariantMassInclusive1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 6);
   defineParameter("RequireOneBarrel", 0);
   defineParameter("MinMSqr",  0, 0);
   defineParameter("MaxMSqr", 999, 0);
   defineParameter("MinMSqr",  0, 1);
   defineParameter("MaxMSqr",  999, 1);
   defineParameter("MinMSqr", 0, 2);
   defineParameter("MaxMSqr", 999, 2);
   defineParameter("MinMSqr", 0, 3);
   defineParameter("MaxMSqr", 999, 3);
   defineParameter("MinMSqr", 0, 4);
   defineParameter("MaxMSqr", 999, 4);
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

TCS::InvariantMassInclusive1::~InvariantMassInclusive1(){}


TCS::StatusCode
TCS::InvariantMassInclusive1::initialize() {
   if(parameter("MaxTob").value() > 0) {
      p_NumberLeading1 = parameter("MaxTob").value();
      p_NumberLeading2 = parameter("MaxTob").value();
   } else {
      p_NumberLeading1 = parameter("InputWidth").value();
      p_NumberLeading2 = parameter("InputWidth").value();
   }

   p_OneBarrel = parameter("RequireOneBarrel").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_InvMassMin[i] = parameter("MinMSqr", i).value();
      p_InvMassMax[i] = parameter("MaxMSqr", i).value();
   
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();
   }
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("RequireOneBarrel : " << p_OneBarrel);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      TRG_MSG_INFO("InvMassMin   : " << p_InvMassMin[i]);
      TRG_MSG_INFO("InvMassMax   : " << p_InvMassMax[i]);

   
      TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
      TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());

   for (unsigned int i=0; i<numberOutputBits();i++) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int mass_min = sqrt(p_InvMassMin[i]);
       int mass_max = sqrt(p_InvMassMax[i]);
       // mass histograms
       snprintf(hname_accept, buf_len, "Accept_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, mass_min, mass_max);
       snprintf(hname_reject, buf_len, "Reject_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, mass_min, mass_max);
       registerHist(m_histAcceptINV1[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*mass_max));
       registerHist(m_histRejectINV1[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*mass_max));
   }

 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::InvariantMassInclusive1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 1) {     
     
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            

            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {


               // OneBarrel
               if (p_OneBarrel && parType_t(abs((*tob1)->eta())) > 10 && parType_t(abs((*tob2)->eta())) > 10 ) continue;
               
               // Inv Mass calculation
               unsigned int invmass2 = TSU::Kinematics::calcInvMassBW( *tob1, *tob2 );
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
                   accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i]; //
                   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                   const bool fillReject = fillHistos() and not fillAccept;
                   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                   }
                   if(fillAccept and not alreadyFilled) {
                       m_histAcceptINV1[i]->Fill(sqrt((float)invmass2));
                   } else if(fillReject) {
                       m_histRejectINV1[i]->Fill(sqrt((float)invmass2));
                   }
                   TRG_MSG_INFO("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2);
               }
            }
         }
   } else {

      TCS_EXCEPTION("InvariantMassInclusive1 alg must have either 1  inputs, but got " << input.size());

   }

   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::InvariantMassInclusive1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 1) {     
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            

            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {


               // OneBarrel
               if (p_OneBarrel && parType_t(abs((*tob1)->eta())) > 10 && parType_t(abs((*tob2)->eta())) > 10 ) continue;
               
               // Inv Mass calculation
             
	       unsigned int invmass2 = TSU::Kinematics::calcInvMass( *tob1, *tob2 );


               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                  if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                  if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                  if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
                  accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i]; // 
                  const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                  const bool fillReject = fillHistos() and not fillAccept;
                  const bool alreadyFilled = decision.bit(i);
                  if( accept ) {
                      decision.setBit(i, true);
                      output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                  if(fillAccept and not alreadyFilled) {
                      m_histAcceptINV1[i]->Fill(sqrt((float)invmass2));
                  } else if(fillReject) {
                      m_histRejectINV1[i]->Fill(sqrt((float)invmass2));
                  }
                  TRG_MSG_INFO("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2);
               }
            }
         }
   } else {

      TCS_EXCEPTION("InvariantMassInclusive1 alg must have either 1  inputs, but got " << input.size());

   }

   return TCS::StatusCode::SUCCESS;
}
