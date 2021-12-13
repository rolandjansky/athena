/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaRSqrIncl2Charge.cxx
 * Created by Paula Martinez based on Joerg Stelzer / V Sorin.
 *
 * @brief algorithm calculates the R2-distance between two lists and applies dR criteria
 * Events containing a pair of TGC muons with same charge are rejected
 *
 * @param NumberLeading
**********************************/

#include "L1TopoAlgorithms/DeltaRSqrIncl2Charge.h"

#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

#include <cmath>

REGISTER_ALG_TCS(DeltaRSqrIncl2Charge)


TCS::DeltaRSqrIncl2Charge::DeltaRSqrIncl2Charge(const std::string & name) : DecisionAlg(name)
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

TCS::DeltaRSqrIncl2Charge::~DeltaRSqrIncl2Charge(){}


TCS::StatusCode
TCS::DeltaRSqrIncl2Charge::initialize() {
   
   m_NumberLeading1 = parameter("InputWidth1").value();
   m_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) m_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) m_NumberLeading2 = parameter("MaxTob2").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      m_DeltaRMin[i] = parameter("DeltaRMin", i).value();
      m_DeltaRMax[i] = parameter("DeltaRMax", i).value();
      m_MinET1[i] = parameter("MinET1",i).value();
      m_MinET2[i] = parameter("MinET2",i).value();
      TRG_MSG_INFO("DeltaRMin" << i << "  : " << m_DeltaRMin[i]);
      TRG_MSG_INFO("DeltaRMax" << i << "  : " << m_DeltaRMax[i]);
      
      TRG_MSG_INFO("MinET1          : " << m_MinET1[i]);
      TRG_MSG_INFO("MinET2          : " << m_MinET2[i]);

   }

   TRG_MSG_INFO("NumberLeading1 : " << m_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << m_NumberLeading2);  

   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hDeltaRSqrIncl2Charge_accept_bit"+std::to_string(static_cast<int>(i));
       std::string hname_reject = "hDeltaRSqrIncl2Charge_reject_bit"+std::to_string(static_cast<int>(i));
       // dR
       bookHist(m_histAccept, hname_accept, "DR", 100, std::sqrt(m_DeltaRMin[i]), std::sqrt(m_DeltaRMax[i]));
       bookHist(m_histReject, hname_reject, "DR", 100, std::sqrt(m_DeltaRMin[i]), std::sqrt(m_DeltaRMax[i]));
       // eta2 vs. eta1
       bookHist(m_histAcceptEta1Eta2, hname_accept, "ETA vs ETA", 100, -70, 70, 100, -70, 70);
       bookHist(m_histRejectEta1Eta2, hname_reject, "ETA vs ETA", 100, -70, 70, 100, -70, 70);
  }
    
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::DeltaRSqrIncl2Charge::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                              const std::vector<TCS::TOBArray *> & output,
                              Decision & decision )
{

   if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < m_NumberLeading1;
           ++tob1)
         {
             for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                  tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < m_NumberLeading2;
                  ++tob2) {
                 // test DeltaR2Min, DeltaR2Max
                 unsigned int deltaR2 = calcDeltaR2BW( *tob1, *tob2 );
                 TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", deltaR2 = " << deltaR2);
                 // Charge cut ( 1 = positive, -1 = negative, 0 = undefined (RPC) )
                 int charge1 = (*tob1)->charge();
                 int charge2 = (*tob2)->charge();
                 int totalCharge = charge1 + charge2;
                 bool acceptCharge = true;
                 if ( std::abs(totalCharge) == 2 ) { acceptCharge = false; }
                 for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= m_MinET1[i]) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= m_MinET2[i]) continue; // ET cut
                   accept = deltaR2 >= m_DeltaRMin[i] && deltaR2 <= m_DeltaRMax[i] && acceptCharge; 
		   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		   const bool fillReject = fillHistos() and not fillAccept;
		   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
		     decision.setBit(i, true);
		     output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                   }
		   if(fillAccept and not alreadyFilled) {
		     fillHist1D(m_histAccept[i],std::sqrt(static_cast<float>(deltaR2)));
                     fillHist2D(m_histAcceptEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		   } else if(fillReject) {
		     fillHist1D(m_histReject[i],std::sqrt(static_cast<float>(deltaR2)));
                     fillHist2D(m_histRejectEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
                 }
             }
         }
   } else {
       TCS_EXCEPTION("DeltaRSqrIncl2Charge alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaRSqrIncl2Charge::process( const std::vector<TCS::TOBArray const *> & input,
                              const std::vector<TCS::TOBArray *> & output,
                              Decision & decision )
{

   if( input.size() == 2) {
      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < m_NumberLeading1;
           ++tob1)
         {


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < m_NumberLeading2;
                 ++tob2) {
               // test DeltaR2Min, DeltaR2Max
               unsigned int deltaR2 = calcDeltaR2( *tob1, *tob2 );
               TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", deltaR2 = " << deltaR2);
               // Charge cut ( 1 = positive, -1 = negative, 0 = undefined (RPC) )
               int charge1 = (*tob1)->charge();
               int charge2 = (*tob2)->charge();
               int totalCharge = charge1 + charge2;
               bool acceptCharge = true;
               if ( std::abs(totalCharge) == 2 ) { acceptCharge = false; }
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
		 bool accept = false;
		 if( parType_t((*tob1)->Et()) <= m_MinET1[i]) continue; // ET cut
		 if( parType_t((*tob2)->Et()) <= m_MinET2[i]) continue; // ET cut
		 accept = deltaR2 >= m_DeltaRMin[i] && deltaR2 <= m_DeltaRMax[i] && acceptCharge;
		 const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		 const bool fillReject = fillHistos() and not fillAccept;
		 const bool alreadyFilled = decision.bit(i);
		 if( accept ) {
		   decision.setBit(i, true);
		   output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
		 }
		 if(fillAccept and not alreadyFilled) {
		   fillHist1D(m_histAccept[i],std::sqrt(static_cast<float>(deltaR2)));
                   fillHist2D(m_histAcceptEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		 } else if(fillReject) {
		   fillHist1D(m_histReject[i],std::sqrt(static_cast<float>(deltaR2)));
                   fillHist2D(m_histRejectEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		 }
		 TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
               }
            }
         }
   } else {
      TCS_EXCEPTION("DeltaRSqrIncl2Charge alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
