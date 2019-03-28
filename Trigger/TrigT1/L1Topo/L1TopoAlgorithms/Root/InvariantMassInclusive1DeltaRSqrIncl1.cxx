/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * Based on InvariantMassInclusive1 and DeltaRSqrIncl1. 01/03/2019.
 * Created by V Sorin and Joerg Stelzer 2014.
 * 
 * @brief algorithm calculates the sqr of the INVMASS and DeltaR between one list and applies invmass and deltaR criteria
 *
 * @param NumberLeading
 * 
 * For questions contact atlas-trig-l1topo-algcom@cern.ch. 
**********************************/

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include "TH1F.h"

#include "L1TopoAlgorithms/InvariantMassInclusive1DeltaRSqrIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"

//

REGISTER_ALG_TCS(InvariantMassInclusive1DeltaRSqrIncl1)

using namespace std;

TCS::InvariantMassInclusive1DeltaRSqrIncl1::InvariantMassInclusive1DeltaRSqrIncl1(const std::string & name) : DecisionAlg(name)
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
   defineParameter("DeltaRMin", 0, 0);
   defineParameter("DeltaRMax", 0, 0);
   defineParameter("DeltaRMin", 0, 1);
   defineParameter("DeltaRMax", 0, 1);
   defineParameter("DeltaRMin", 0, 2);
   defineParameter("DeltaRMax", 0, 2);
   defineParameter("DeltaRMin", 0, 3);
   defineParameter("DeltaRMax", 0, 3);
   defineParameter("DeltaRMin", 0, 4);
   defineParameter("DeltaRMax", 0, 4);
   defineParameter("DeltaRMin", 0, 5);
   defineParameter("DeltaRMax", 0, 5);

 
   setNumberOutputBits(6);
}

TCS::InvariantMassInclusive1DeltaRSqrIncl1::~InvariantMassInclusive1DeltaRSqrIncl1(){}


TCS::StatusCode
TCS::InvariantMassInclusive1DeltaRSqrIncl1::initialize() {
   if(parameter("MaxTob").value() > 0) {
      p_NumberLeading1 = parameter("MaxTob").value();
      p_NumberLeading2 = parameter("MaxTob").value();
   } else {
      p_NumberLeading1 = parameter("InputWidth").value();
      p_NumberLeading2 = parameter("InputWidth").value();
   }

   p_OneBarrel = parameter("RequireOneBarrel").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("RequireOneBarrel : " << p_OneBarrel);

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_InvMassMin[i] = parameter("MinMSqr", i).value();
      p_InvMassMax[i] = parameter("MaxMSqr", i).value();
      p_DeltaRMin[i] = parameter("DeltaRMin", i).value();
      p_DeltaRMax[i] = parameter("DeltaRMax", i).value();
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();

      TRG_MSG_INFO("InvMassMin "<< i << "  : " << p_InvMassMin[i]);
      TRG_MSG_INFO("InvMassMax "<< i << "  : " << p_InvMassMax[i]);
      TRG_MSG_INFO("MinET1     "<< i << "  : " << p_MinET1[i]);
      TRG_MSG_INFO("MinET2     "<< i << "  : " << p_MinET2[i]);
      TRG_MSG_INFO("DeltaRMin  "<< i << "  : " << p_DeltaRMin[i]);
      TRG_MSG_INFO("DeltaRMax  "<< i << "  : " << p_DeltaRMax[i]);
   }

   TRG_MSG_INFO("number output : " << numberOutputBits());

   for (unsigned int i=0; i<numberOutputBits();i++) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int mass_min = sqrt(p_InvMassMin[i]);
       int mass_max = sqrt(p_InvMassMax[i]);
       int deltaR_min = sqrt(p_DeltaRMin[i]);
       int deltaR_max = sqrt(p_DeltaRMax[i]);
       // mass
       snprintf(hname_accept, buf_len, "Accept_InvariantMassInclusive1DeltaRSqrIncl1_bit%d_%dM%d_Mass", i, mass_min, mass_max);
       snprintf(hname_reject, buf_len, "Reject_InvariantMassInclusive1DeltaRSqrIncl1_bit%d_%dM%d_Mass", i, mass_min, mass_max);
       registerHist(m_histAcceptM[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*mass_max));
       registerHist(m_histRejectM[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*mass_max));
       // delta R
       snprintf(hname_accept, buf_len, "Accept_InvariantMassInclusive1DeltaRSqrIncl1_bit%d_%dDR%d_DR", i, deltaR_min, deltaR_max);
       snprintf(hname_reject, buf_len, "Reject_InvariantMassInclusive1DeltaRSqrIncl1_bit%d_%dDR%d_DR", i, deltaR_min, deltaR_max);
       registerHist(m_histAcceptDR[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*deltaR_max));
       registerHist(m_histRejectDR[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*deltaR_max));
  }

 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::InvariantMassInclusive1DeltaRSqrIncl1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
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
	       // test DeltaR2Min, DeltaR2Max                                                                                                
	       unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob2 );
	       TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", invmass2 = " << invmass2 << ", deltaR2 = " << deltaR2);
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
                   accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i] && deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i];
                   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                   const bool fillReject = fillHistos() and not fillAccept;
                   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                   }
                   if(fillAccept and not alreadyFilled) {
                       m_histAcceptM[i]->Fill(sqrt((float)invmass2));
                       m_histAcceptDR[i]->Fill(sqrt((float)deltaR2));
                   } else if(fillReject) {
                       m_histRejectM[i]->Fill(sqrt((float)invmass2));
                       m_histRejectDR[i]->Fill(sqrt((float)deltaR2));
                   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2 << " deltaR2 = " << deltaR2 );
               }
            }
         }
   } else {

      TCS_EXCEPTION("InvariantMassInclusive1 alg must have either 1  inputs, but got " << input.size());

   }

   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::InvariantMassInclusive1DeltaRSqrIncl1::process( const std::vector<TCS::TOBArray const *> & input,
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
	       // test DeltaR2Min, DeltaR2Max                                                                                                  
	       unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2( *tob1, *tob2 );
	       TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", invmass2 = " << invmass2 << ", deltaR2 = " << deltaR2);
	       
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                  if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                  if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                  if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
                  accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i] && deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i]; 
                  const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                  const bool fillReject = fillHistos() and not fillAccept;
                  const bool alreadyFilled = decision.bit(i);
                  if( accept ) {
                      decision.setBit(i, true);
                      output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                  if(fillAccept and not alreadyFilled) {
                      m_histAcceptM[i]->Fill(sqrt((float)invmass2));
                      m_histAcceptDR[i]->Fill(sqrt((float)deltaR2));
                  } else if(fillReject) {
                      m_histRejectM[i]->Fill(sqrt((float)invmass2));
                      m_histRejectDR[i]->Fill(sqrt((float)deltaR2));
                  }
                  TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2 << " deltaR2 = " << deltaR2 );
               }
            }
         }
   } else {

      TCS_EXCEPTION("InvariantMassInclusive1 alg must have either 1  inputs, but got " << input.size());

   }

   return TCS::StatusCode::SUCCESS;
}
