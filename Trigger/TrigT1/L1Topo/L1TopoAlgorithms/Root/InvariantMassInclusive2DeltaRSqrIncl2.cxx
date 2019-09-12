/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * Based on InvariantMassInclusive2 and DeltaRSqrIncl2. 01/03/2019.
 * Created by V Sorin and Joerg Stelzer 2014.
 *
 * @brief algorithm calculates the sqr of the INVMASS and DeltaR between two lists and applies invmass and deltaR criteria
 *
 * @param NumberLeading
 *
 * For questions contact atlas-trig-l1topo-algcom@cern.ch. 
**********************************/


#include "L1TopoAlgorithms/InvariantMassInclusive2DeltaRSqrIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"
//
#include "TH1F.h"
#include "TH2F.h"

#include <cmath>

REGISTER_ALG_TCS(InvariantMassInclusive2DeltaRSqrIncl2)

using namespace std;


TCS::InvariantMassInclusive2DeltaRSqrIncl2::InvariantMassInclusive2DeltaRSqrIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0);
   defineParameter("MaxTob2", 0);
   defineParameter("NumResultBits", 6);
   defineParameter("MinMSqr",   0, 0);
   defineParameter("MaxMSqr", 999, 0);
   defineParameter("MinMSqr",   0, 1);
   defineParameter("MaxMSqr", 999, 1);
   defineParameter("MinMSqr",   0, 2);
   defineParameter("MaxMSqr", 999, 2);
   defineParameter("MinMSqr",   0, 3);
   defineParameter("MaxMSqr", 999, 3);
   defineParameter("MinMSqr",   0, 4);
   defineParameter("MaxMSqr", 999, 4);
   defineParameter("MinMSqr",   0, 5);
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
   defineParameter("ApplyEtaCut", 0);
   defineParameter("MinEta1",  0);
   defineParameter("MaxEta1", 31);
   defineParameter("MinEta2",  0);
   defineParameter("MaxEta2", 31);
   defineParameter("DeltaRMin",  0, 0);
   defineParameter("DeltaRMax",  0, 0);
   defineParameter("DeltaRMin",  0, 1);
   defineParameter("DeltaRMax",  0, 1);
   defineParameter("DeltaRMin",  0, 2);
   defineParameter("DeltaRMax",  0, 2);
   defineParameter("DeltaRMin",  0, 3);
   defineParameter("DeltaRMax",  0, 3);
   defineParameter("DeltaRMin",  0, 4);
   defineParameter("DeltaRMax",  0, 4);
   defineParameter("DeltaRMin",  0, 5);
   defineParameter("DeltaRMax",  0, 5);

   setNumberOutputBits(6);
}

TCS::InvariantMassInclusive2DeltaRSqrIncl2::~InvariantMassInclusive2DeltaRSqrIncl2(){}


TCS::StatusCode
TCS::InvariantMassInclusive2DeltaRSqrIncl2::initialize() {

   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

     p_InvMassMin[i] = parameter("MinMSqr", i).value();
     p_InvMassMax[i] = parameter("MaxMSqr", i).value();
     p_MinET1[i] = parameter("MinET1",i).value();
     p_MinET2[i] = parameter("MinET2",i).value();
     p_DeltaRMin[i] = parameter("DeltaRMin", i).value();
     p_DeltaRMax[i] = parameter("DeltaRMax", i).value();
     
     TRG_MSG_INFO("InvMassMin "<< i << "  : " << p_InvMassMin[i]);
     TRG_MSG_INFO("InvMassMax "<< i << "  : " << p_InvMassMax[i]);
     TRG_MSG_INFO("MinET1     "<< i << "  : " << p_MinET1[i]);
     TRG_MSG_INFO("MinET2     "<< i << "  : " << p_MinET2[i]);
     TRG_MSG_INFO("DeltaRMin  "<< i << "  : " << p_DeltaRMin[i]);
     TRG_MSG_INFO("DeltaRMax  "<< i << "  : " << p_DeltaRMax[i]);

   }

   p_ApplyEtaCut = parameter("ApplyEtaCut").value();
   p_MinEta1     = parameter("MinEta1"    ).value();
   p_MaxEta1     = parameter("MaxEta1"    ).value();
   p_MinEta2     = parameter("MinEta2"    ).value();
   p_MaxEta2     = parameter("MaxEta2"    ).value();
   TRG_MSG_INFO("ApplyEtaCut : "<<p_ApplyEtaCut );
   TRG_MSG_INFO("MinEta1     : "<<p_MinEta1     );
   TRG_MSG_INFO("MaxEta1     : "<<p_MaxEta1     );
   TRG_MSG_INFO("MinEta2     : "<<p_MinEta2     );
   TRG_MSG_INFO("MaxEta2     : "<<p_MaxEta2     );

   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int mass_min = sqrt(p_InvMassMin[i]);
       int mass_max = sqrt(p_InvMassMax[i]);
       int deltaR_min = sqrt(p_DeltaRMin[i]);
       int deltaR_max = sqrt(p_DeltaRMax[i]);
       // mass
       snprintf(hname_accept, buf_len, "Accept_InvariantMassInclusive2_bit%d_%dM%d_Mass", i, mass_min, mass_max);
       snprintf(hname_reject, buf_len, "Reject_InvariantMassInclusive2_bit%d_%dM%d_Mass", i, mass_min, mass_max);
       registerHist(m_histAcceptM[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*mass_max));
       registerHist(m_histRejectM[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*mass_max));
       // delta R
       snprintf(hname_accept, buf_len, "Accept_InvariantMassInclusive2DeltaRSqrIncl2_bit%d_%dDR%d_DR", i, deltaR_min, deltaR_max);
       snprintf(hname_reject, buf_len, "Reject_InvariantMassInclusive2DeltaRSqrIncl2_bit%d_%dDR%d_DR", i, deltaR_min, deltaR_max);
       registerHist(m_histAcceptDR[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*deltaR_max));
       registerHist(m_histRejectDR[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*deltaR_max));
       // eta2 vs. eta1
       snprintf(hname_accept, buf_len, "Accept_InvariantMassInclusive2_bit%d_%dM%d_Eta1Eta2", i, mass_min, mass_max);
       snprintf(hname_reject, buf_len, "Reject_InvariantMassInclusive2_bit%d_%dM%d_Eta1Eta2", i, mass_min, mass_max);
       registerHist(m_histAcceptEta1Eta2[i] = new TH2F(hname_accept, hname_accept, 100, -50.0, +50.0, 100, -50.0, +50.0));
       registerHist(m_histRejectEta1Eta2[i] = new TH2F(hname_reject, hname_reject, 100, -50.0, +50.0, 100, -50.0, +50.0));
   }
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::InvariantMassInclusive2DeltaRSqrIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
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
                // Inv Mass calculation
                unsigned int invmass2 = TSU::Kinematics::calcInvMassBW( *tob1, *tob2 );
		// test DeltaR2Min, DeltaR2Max                                                                                                
		unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob2 );
		TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", invmass2 = " << invmass2 << ", deltaR2 = " << deltaR2);
                const int eta1 = (*tob1)->eta();
                const int eta2 = (*tob2)->eta();
                const unsigned int aeta1 = std::abs(eta1);
                const unsigned int aeta2 = std::abs(eta2);
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                   if(p_ApplyEtaCut &&
                      ((aeta1 < p_MinEta1 || aeta1 > p_MaxEta1 ) ||
                       (aeta2 < p_MinEta2 || aeta2 > p_MaxEta2 ) ))  continue;
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
                       m_histAcceptEta1Eta2[i]->Fill(eta1, eta2);
                   } else if(fillReject) {
                       m_histRejectM[i]->Fill(sqrt((float)invmass2));
                       m_histRejectDR[i]->Fill(sqrt((float)deltaR2));
                       m_histRejectEta1Eta2[i]->Fill(eta1, eta2);
                   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2 << " deltaR2 = " << deltaR2 );
               }
            }
         }
   } else {

      TCS_EXCEPTION("InvariantMassInclusive2DeltaRSqrIncl2 alg must have  2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::InvariantMassInclusive2DeltaRSqrIncl2::process( const std::vector<TCS::TOBArray const *> & input,
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
	      // Inv Mass calculation
	      unsigned int invmass2 = TSU::Kinematics::calcInvMass( *tob1, *tob2 );
	      // test DeltaR2Min, DeltaR2Max                                                                                                  
	      unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2( *tob1, *tob2 );
	      TRG_MSG_DEBUG("Jet1 = " << **tob1 << ", Jet2 = " << **tob2 << ", invmass2 = " << invmass2 << ", deltaR2 = " << deltaR2);
	      const int eta1 = (*tob1)->eta();
	      const int eta2 = (*tob2)->eta();
	      const unsigned int aeta1 = std::abs(eta1);
	      const unsigned int aeta2 = std::abs(eta2);
	      for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                   if(p_ApplyEtaCut &&
                      ((aeta1 < p_MinEta1 || aeta1 > p_MaxEta1 ) ||
                       (aeta2 < p_MinEta2 || aeta2 > p_MaxEta2 ) )) continue;
                   bool accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i] && deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i];
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
                       m_histAcceptEta1Eta2[i]->Fill(eta1, eta2);
                   } else if(fillReject) {
                       m_histRejectM[i]->Fill(sqrt((float)invmass2));
                       m_histRejectDR[i]->Fill(sqrt((float)deltaR2));
                       m_histRejectEta1Eta2[i]->Fill(eta1, eta2);
                   }
		  TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2 << " deltaR2 = " << deltaR2 );
               }
            }
         }
   } else {
      TCS_EXCEPTION("InvariantMassInclusive2DeltaRSqrIncl2 alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
