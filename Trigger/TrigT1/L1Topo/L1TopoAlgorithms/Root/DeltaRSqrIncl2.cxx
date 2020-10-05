/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaRSqrIncl2.cpp
 * Created by Joerg Stelzer / V Sorin on 9/16/14.
 *
 * @brief algorithm calculates the R2-distance between two lists and applies dR criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include "TH1F.h"
#include "TH2F.h"

#include "L1TopoAlgorithms/DeltaRSqrIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaRSqrIncl2)


// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << "TCS::DeltaRSqrIncl2:     "

TCS::DeltaRSqrIncl2::DeltaRSqrIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 3);
   defineParameter("MinET1",1,0);
   defineParameter("MinET2",1,0);
   defineParameter("DeltaRMin",  0, 0);
   defineParameter("DeltaRMax",  0, 0);
   defineParameter("MinET1",1,1);
   defineParameter("MinET2",1,1);
   defineParameter("DeltaRMin",  0, 1);
   defineParameter("DeltaRMax",  0, 1);
   defineParameter("MinET1",1,2);
   defineParameter("MinET2",1,2);
   defineParameter("DeltaRMin",  0, 2);
   defineParameter("DeltaRMax",  0, 2);
   setNumberOutputBits(3);
}

TCS::DeltaRSqrIncl2::~DeltaRSqrIncl2(){}


TCS::StatusCode
TCS::DeltaRSqrIncl2::initialize() {
   
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_DeltaRMin[i] = parameter("DeltaRMin", i).value();
      p_DeltaRMax[i] = parameter("DeltaRMax", i).value();
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();
      TRG_MSG_INFO("DeltaRMin" << i << "  : " << p_DeltaRMin[i]);
      TRG_MSG_INFO("DeltaRMax" << i << "  : " << p_DeltaRMax[i]);
      
      TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
      TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);

   }

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);  

   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hDeltaRSqrIncl2_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hDeltaRSqrIncl2_reject_bit"+std::to_string((int)i);
       // dR
       bookHist(m_histAccept, hname_accept, "DR", 100, sqrt(p_DeltaRMin[i]), sqrt(p_DeltaRMax[i]));
       bookHist(m_histReject, hname_reject, "DR", 100, sqrt(p_DeltaRMin[i]), sqrt(p_DeltaRMax[i]));
  }
    
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::DeltaRSqrIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                              const std::vector<TCS::TOBArray *> & output,
                              Decision & decision )
{

   if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {
             for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                  tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                  ++tob2) {
                 // test DeltaR2Min, DeltaR2Max
                 unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob2 );
                 TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", deltaR2 = " << deltaR2);
                 for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                   accept = deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i];
		   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		   const bool fillReject = fillHistos() and not fillAccept;
		   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
		     decision.setBit(i, true);
		     output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                   }
		   if(fillAccept and not alreadyFilled) {
		     fillHist1D(m_histAccept[i],sqrt((float)deltaR2));
		   } else if(fillReject) {
		     fillHist1D(m_histReject[i],sqrt((float)deltaR2));
		   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
                 }
             }
         }
   } else {
       TCS_EXCEPTION("DeltaRSqrIncl2 alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaRSqrIncl2::process( const std::vector<TCS::TOBArray const *> & input,
                              const std::vector<TCS::TOBArray *> & output,
                              Decision & decision )
{

   if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {
               // test DeltaR2Min, DeltaR2Max
               unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2( *tob1, *tob2 );

               TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", deltaR2 = " << deltaR2);
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
		 bool accept = false;
		 if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		 if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
		 accept = deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i];
		 const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		 const bool fillReject = fillHistos() and not fillAccept;
		 const bool alreadyFilled = decision.bit(i);
		 if( accept ) {
		   decision.setBit(i, true);
		   output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
		 }
		 if(fillAccept and not alreadyFilled) {
		   fillHist1D(m_histAccept[i],sqrt((float)deltaR2));
		 } else if(fillReject) {
		   fillHist1D(m_histReject[i],sqrt((float)deltaR2));
		 }
		 TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
               }
            }
         }
   } else {
      TCS_EXCEPTION("DeltaRSqrIncl2 alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
