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

TCS::ExclusiveJets::ExclusiveJets(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 6);
   setNumberOutputBits(6);

   for (unsigned int i=0;i<numberOutputBits();i++){
     defineParameter("MinXi",  0, i);
     defineParameter("MaxXi", 999, i);
     defineParameter("MinET1",0,i);
     defineParameter("ApplyEtaCut",0,i);
     defineParameter("MinEta1",0,i);
     defineParameter("MaxEta1",999,i);
     defineParameter("MinEta2",0,i);
     defineParameter("MaxEta2",999,i);
   }

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
     
     p_ApplyEtaCut[i] = parameter("ApplyEtaCut",i).value();
     p_MinEta1[i]     = parameter("MinEta1"    ,i).value();
     p_MaxEta1[i]     = parameter("MaxEta1"    ,i).value();
     p_MinEta2[i]     = parameter("MinEta2"    ,i).value();
     p_MaxEta2[i]     = parameter("MaxEta2"    ,i).value();
   }
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
     TRG_MSG_INFO("XiMin   : " << p_XiMin[i]);
     TRG_MSG_INFO("XiMax   : " << p_XiMax[i]);
   
     TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
     
     TRG_MSG_INFO("ApplyEtaCut : "<<p_ApplyEtaCut[i] );
     
     TRG_MSG_INFO("MinEta1     : "<<p_MinEta1[i]     );
     TRG_MSG_INFO("MaxEta1     : "<<p_MaxEta1[i]     );
     TRG_MSG_INFO("MinEta2     : "<<p_MinEta2[i]     );
     TRG_MSG_INFO("MaxEta2     : "<<p_MaxEta2[i]     );
  }
   
   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hExclusiveJets_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hExclusiveJets_reject_bit"+std::to_string((int)i);
       // mass
       bookHist(m_histAcceptX, hname_accept, "Xi1 vs Xi2", 100, p_XiMin[i], p_XiMax[i], 100, p_XiMin[i], p_XiMax[i]);
       bookHist(m_histRejectX, hname_reject, "Xi1 vs Xi2", 100, p_XiMin[i], p_XiMax[i], 100, p_XiMin[i], p_XiMax[i]);
       // eta2 vs. eta1
       bookHist(m_histAcceptEta1Eta2, hname_accept, "ETA vs ETA", 100, p_MinEta1[i], p_MaxEta1[i], 100, p_MinEta2[i], p_MaxEta2[i]);
       bookHist(m_histRejectEta1Eta2, hname_reject, "ETA vs ETA", 100, p_MinEta1[i], p_MaxEta1[i], 100, p_MinEta2[i], p_MaxEta2[i]);
   }
   
 
   return StatusCode::SUCCESS;
}
TCS::StatusCode
TCS::ExclusiveJets::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
				       const std::vector<TCS::TOBArray *> & output,
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
	      
	      //In the ticket ATR-17320, pT_offline were defined as A*pT_L1+B, where A=1.4 and B=20 for run2
	      //A and B definition might change according to run3 configuration.               
	      double xi_1 = (1.4*parType_t((*tob1)->Et())+20.)*exp((*tob1)->etaDouble())+(1.4*parType_t((*tob2)->Et())+20.)*exp((*tob2)->etaDouble());
	      double xi_2 = (1.4*parType_t((*tob1)->Et())+20.)*exp(-1.*(*tob1)->etaDouble())+(1.4*parType_t((*tob2)->Et())+20.)*exp(-1.*(*tob2)->etaDouble());
	      
	      const int eta1 = (*tob1)->eta();
	      const int eta2 = (*tob2)->eta();
	      const unsigned int aeta1 = std::abs(eta1);
	      const unsigned int aeta2 = std::abs(eta2);
	      for(unsigned int i=0; i<numberOutputBits(); ++i) {
		bool accept = false;
		if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		if(p_ApplyEtaCut[i] &&
		   ((aeta1 < p_MinEta1[i] || aeta1 > p_MaxEta1[i] ) ||
		    (aeta2 < p_MinEta2[i] || aeta2 > p_MaxEta2[i] ) ))  continue;
		
		accept = (xi_1 >p_XiMin[i]) && (xi_1 < p_XiMax[i]) && (xi_2 > p_XiMin[i]) && (xi_2 < p_XiMax[i]); //
		const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		const bool fillReject = fillHistos() and not fillAccept;
		const bool alreadyFilled = decision.bit(i);
		if( accept ) {
		  decision.setBit(i, true);
		  output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		}
		if(fillAccept and not alreadyFilled) {
		  fillHist2D(m_histAcceptX[i],xi_1,xi_2);
		  fillHist2D(m_histAcceptEta1Eta2[i],eta1, eta2);
		  } else if(fillReject) {
		  fillHist2D(m_histRejectX[i],xi_1,xi_2);
		  fillHist2D(m_histRejectEta1Eta2[i],eta1, eta2);
		}
		TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " xi_1 = " << xi_1);
		
	      }
	    }
	 }
   } else {
      TCS_EXCEPTION("ExclusiveJets alg must have 1 input list, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
TCS::StatusCode
TCS::ExclusiveJets::process( const std::vector<TCS::TOBArray const *> & input,
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
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading1;
                 ++tob2) {

	      //In the ticket ATR-17320, pT_offline were defined as A*pT_L1+B, where A=1.4 and B=20 for run2
	      //A and B definition might change according to run3 configuration.
	      double xi_1 = (1.4*parType_t((*tob1)->Et())+20.)*exp((*tob1)->etaDouble())+(1.4*parType_t((*tob2)->Et())+20.)*exp((*tob2)->etaDouble());
	      double xi_2 = (1.4*parType_t((*tob1)->Et())+20.)*exp(-1.*(*tob1)->etaDouble())+(1.4*parType_t((*tob2)->Et())+20.)*exp(-1.*(*tob2)->etaDouble());
	      
	      const int eta1 = (*tob1)->eta();
	      const int eta2 = (*tob2)->eta();
	      const unsigned int aeta1 = std::abs(eta1);
	      const unsigned int aeta2 = std::abs(eta2);
	      for(unsigned int i=0; i<numberOutputBits(); ++i) {
		bool accept = false;
		if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		if(p_ApplyEtaCut[i] &&
		   ((aeta1 < p_MinEta1[i] || aeta1 > p_MaxEta1[i] ) ||
		    (aeta2 < p_MinEta2[i] || aeta2 > p_MaxEta2[i] ) )) continue;
		
		accept = (xi_1 >p_XiMin[i]) && (xi_1 < p_XiMax[i]) && (xi_2 > p_XiMin[i]) && (xi_2 < p_XiMax[i]); //
		const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		const bool fillReject = fillHistos() and not fillAccept;
		const bool alreadyFilled = decision.bit(i);
		if( accept ) {
		  decision.setBit(i, true);
		  output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		}
		if(fillAccept and not alreadyFilled) {
		  fillHist2D(m_histAcceptX[i],xi_1,xi_2);
		  fillHist2D(m_histAcceptEta1Eta2[i],eta1, eta2);
		  } else if(fillReject) {
		  fillHist2D(m_histRejectX[i],xi_1,xi_2);
		  fillHist2D(m_histRejectEta1Eta2[i],eta1, eta2);
		}
		TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " xi_1 = " << xi_1);
		  
	      }
	    }
	 }
   } else {
      TCS_EXCEPTION("ExclusiveJets alg must have 1 input list, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
