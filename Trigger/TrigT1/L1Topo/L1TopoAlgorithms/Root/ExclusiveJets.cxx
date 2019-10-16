/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * ExclusiveJets.cxx
 * Created by Carlos Moreno based on InvariantMassInclusive1 by V Sorin
 * 
 * For questions contact atlas-trig-l1topo-algcom@cern.ch. 
 * @brief algorithm for exclusive jets acceptance 
 *
 * @param NumberLeading 
 *
**********************************/
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "TH1F.h"
#include "TH2F.h"
#include "L1TopoAlgorithms/ExclusiveJets.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"
//
REGISTER_ALG_TCS(ExclusiveJets)
using namespace std;
TCS::ExclusiveJets::ExclusiveJets(const string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 6);
   defineParameter("MinXi",  0, 0);
   defineParameter("MaxXi", 999, 0);
   defineParameter("MinXi",  0, 1);
   defineParameter("MaxXi",  999, 1);
   defineParameter("MinXi", 0, 2);
   defineParameter("MaxXi", 999, 2);
   defineParameter("MinXi", 0, 3);
   defineParameter("MaxXi", 999, 3);
   defineParameter("MinXi", 0, 4);
   defineParameter("MaxXi", 999, 4);
   defineParameter("MinXi", 0, 5);
   defineParameter("MaxXi", 999, 5);
   defineParameter("MinET1",0,0);
   defineParameter("MinET1",0,1);
   defineParameter("MinET1",0,2);
   defineParameter("MinET1",0,3);
   defineParameter("MinET1",0,4);
   defineParameter("MinET1",0,5);
   defineParameter("ApplyEtaCut", 0);
   defineParameter("MinEta1",  0);
   defineParameter("MaxEta1", 31);
   defineParameter("MinEta2",  0);
   defineParameter("MaxEta2", 31);
   setNumberOutputBits(6);
}
TCS::ExclusiveJets::~ExclusiveJets(){}
TCS::StatusCode
TCS::ExclusiveJets::initialize() {
  if(parameter("MaxTob").value() > 0) {
      p_NumberLeading1 = parameter("MaxTob").value();
   } else {
      p_NumberLeading1 = parameter("InputWidth").value();
   }
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_XiMin[i] = parameter("MinXi", i).value();
      p_XiMax[i] = parameter("MaxXi", i).value();
   
      p_MinET1[i] = parameter("MinET1",i).value();
   }
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      TRG_MSG_INFO("XiMin   : " << p_XiMin[i]);
      TRG_MSG_INFO("XiMax   : " << p_XiMax[i]);
   
      TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
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
   for (unsigned int i=0; i<numberOutputBits();i++) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int xi_min = p_XiMin[i];
       int xi_max = p_XiMax[i];
       // mass histograms
       snprintf(hname_accept, buf_len, "Accept_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, xi_min, xi_max);
       snprintf(hname_reject, buf_len, "Reject_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, xi_min, xi_max);
       registerHist(m_histAcceptExclusiveJets[i] = new TH2F(hname_accept, hname_accept, 100, 0.0, 2*xi_max, 100, 0.0, 2*xi_max));
       registerHist(m_histRejectExclusiveJets[i] = new TH2F(hname_reject, hname_reject, 100, 0.0, 2*xi_max, 100, 0.0, 2*xi_max));
       // eta2 vs. eta1
       snprintf(hname_accept, buf_len, "Accept_%s_%s_bit%d_%dM%d_Eta1Eta2", name().c_str(), className().c_str(), i, xi_min, xi_max);
       snprintf(hname_reject, buf_len, "Reject_%s_%s_bit%d_%dM%d_Eta1Eta2", name().c_str(), className().c_str(), i, xi_min, xi_max);
       registerHist(m_histAcceptEta1Eta2[i] = new TH2F(hname_accept, hname_accept, 100, -50.0, +50.0, 100, -50.0, +50.0));
       registerHist(m_histRejectEta1Eta2[i] = new TH2F(hname_reject, hname_reject, 100, -50.0, +50.0, 100, -50.0, +50.0));
   }
 
   return StatusCode::SUCCESS;
}
TCS::StatusCode
TCS::ExclusiveJets::processBitCorrect( const vector<TCS::TOBArray const *> & input,
				       const vector<TCS::TOBArray *> & output,
				       Decision & decision ) // Not really bitwise, keep the name for the future
{
   if(input.size() == 1) {     
     
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            
            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading1;
                 ++tob2) {
               
	      double xi_1 = (1.4*parType_t((*tob1)->Et())+20.)*exp(0.1*(*tob1)->eta())+(1.4*parType_t((*tob2)->Et())+20.)*exp(0.1*(*tob2)->eta());
	      double xi_2 = (1.4*parType_t((*tob1)->Et())+20.)*exp(-.1*(*tob1)->eta())+(1.4*parType_t((*tob2)->Et())+20.)*exp(-.1*(*tob2)->eta());
	      const int eta1 = (*tob1)->eta();
	      const int eta2 = (*tob2)->eta();
	      const unsigned int aeta1 = std::abs(eta1);
	      const unsigned int aeta2 = std::abs(eta2);
	      
	      for(unsigned int i=0; i<numberOutputBits(); ++i) {
		bool accept = false;
		if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		if(p_ApplyEtaCut &&
		   ((aeta1 < p_MinEta1 || aeta1 > p_MaxEta1 ) ||
		    (aeta2 < p_MinEta2 || aeta2 > p_MaxEta2 ) ))  continue;
		
		accept = (xi_1 >p_XiMin[i]) && (xi_1 < p_XiMax[i]) && (xi_2 > p_XiMin[i]) && (xi_2 < p_XiMax[i]); //
		const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		const bool fillReject = fillHistos() and not fillAccept;
		const bool alreadyFilled = decision.bit(i);
		if( accept ) {
		  decision.setBit(i, true);
		  output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		}
		if(fillAccept and not alreadyFilled) {
		  m_histAcceptExclusiveJets[i]->Fill(xi_1,xi_2);
		  m_histAcceptEta1Eta2[i]->Fill(eta1, eta2);
		} else if(fillReject) {
		  m_histRejectExclusiveJets[i]->Fill(xi_1,xi_2);
		  m_histRejectEta1Eta2[i]->Fill(eta1, eta2);
		}
		TRG_MSG_INFO("Decision " << i << ": " << (accept?"pass":"fail") << " xi_1 = " << xi_1);
		
	      }
	    }
	 }
   } else {
      TCS_EXCEPTION("ExclusiveJets alg must have 1 input list, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
TCS::StatusCode
TCS::ExclusiveJets::process( const vector<TCS::TOBArray const *> & input,
			     const vector<TCS::TOBArray *> & output,
			     Decision & decision )
{
   if(input.size() == 1) {     
     
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            
            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading1;
                 ++tob2) {
               
	      double xi_1 = (1.4*parType_t((*tob1)->Et())+20.)*exp(0.1*(*tob1)->eta())+(1.4*parType_t((*tob2)->Et())+20.)*exp(0.1*(*tob2)->eta());
	      double xi_2 = (1.4*parType_t((*tob1)->Et())+20.)*exp(-.1*(*tob1)->eta())+(1.4*parType_t((*tob2)->Et())+20.)*exp(-.1*(*tob2)->eta());
	      
	      const int eta1 = (*tob1)->eta();
	      const int eta2 = (*tob2)->eta();
	      const unsigned int aeta1 = std::abs(eta1);
	      const unsigned int aeta2 = std::abs(eta2);
	      for(unsigned int i=0; i<numberOutputBits(); ++i) {
		bool accept = false;
		if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		if(p_ApplyEtaCut &&
		   ((aeta1 < p_MinEta1 || aeta1 > p_MaxEta1 ) ||
		    (aeta2 < p_MinEta2 || aeta2 > p_MaxEta2 ) )) continue;
		
		accept = (xi_1 > p_XiMin[i]) && (xi_1 < p_XiMax[i]) && (xi_2 > p_XiMin[i]) && (xi_2 < p_XiMax[i]); //
		const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		const bool fillReject = fillHistos() and not fillAccept;
		const bool alreadyFilled = decision.bit(i);
		if( accept ) {
		  decision.setBit(i, true);
		  output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		}
		if(fillAccept and not alreadyFilled) {
		  m_histAcceptExclusiveJets[i]->Fill(xi_1,xi_2);
		  m_histAcceptEta1Eta2[i]->Fill(eta1, eta2);
		  } else if(fillReject) {
		  m_histRejectExclusiveJets[i]->Fill(xi_1,xi_2);
		  m_histRejectEta1Eta2[i]->Fill(eta1, eta2);
		}
		TRG_MSG_INFO("Decision " << i << ": " << (accept?"pass":"fail") << " xi_1 = " << xi_1);
		  
	      }
	    }
	 }
   } else {
      TCS_EXCEPTION("ExclusiveJets alg must have 1 input list, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
