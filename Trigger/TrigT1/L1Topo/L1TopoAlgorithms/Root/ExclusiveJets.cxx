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
   TRG_MSG_INFO("number output : " << numberOutputBits());

   for (unsigned int i=0; i<numberOutputBits();i++) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int xi_min = p_XiMin[i];
       int xi_max = p_XiMax[i];
       // mass histograms
       snprintf(hname_accept, buf_len, "Accept_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, xi_min, xi_max);
       snprintf(hname_reject, buf_len, "Reject_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, xi_min, xi_max);
       registerHist(m_histAcceptExclusiveJets[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*xi_max));
       registerHist(m_histRejectExclusiveJets[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*xi_max));
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
               
	      double xi_1 = (1.4*parType_t((*tob1)->Et())+20.)*exp(parType_t((*tob1)->eta()))+(1.4*parType_t((*tob2)->Et())+20.)*exp(parType_t((*tob2)->eta()));
	      double xi_2 = (1.4*parType_t((*tob1)->Et())+20.)*exp(-1.*parType_t((*tob1)->eta()))+(1.4*parType_t((*tob2)->Et())+20.)*exp(-1.*parType_t((*tob2)->eta()));

	      for(unsigned int i=0; i<numberOutputBits(); ++i) {
		bool accept = false;
		if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		
		accept = (xi_1 >p_XiMin[i]) && (xi_1 < p_XiMax[i]) && (xi_2 > p_XiMin[i]) && (xi_2 < p_XiMax[i]); //
		const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		const bool fillReject = fillHistos() and not fillAccept;
		const bool alreadyFilled = decision.bit(i);
		if( accept ) {
		  decision.setBit(i, true);
		  output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		}
		if(fillAccept and not alreadyFilled) {
		  m_histAcceptExclusiveJets[i]->Fill(xi_1);
		} else if(fillReject) {
		  m_histRejectExclusiveJets[i]->Fill(xi_1);
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
               
	      double xi_1 = (1.4*parType_t((*tob1)->Et())+20.)*exp(parType_t((*tob1)->eta()))+(1.4*parType_t((*tob2)->Et())+20.)*exp(parType_t((*tob2)->eta()));
	      double xi_2 = (1.4*parType_t((*tob1)->Et())+20.)*exp(-1.*parType_t((*tob1)->eta()))+(1.4*parType_t((*tob2)->Et())+20.)*exp(-1.*parType_t((*tob2)->eta()));

	      for(unsigned int i=0; i<numberOutputBits(); ++i) {
		bool accept = false;
		if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		
		accept = (xi_1 > p_XiMin[i]) && (xi_1 < p_XiMax[i]) && (xi_2 > p_XiMin[i]) && (xi_2 < p_XiMax[i]); //
		const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		const bool fillReject = fillHistos() and not fillAccept;
		const bool alreadyFilled = decision.bit(i);
		if( accept ) {
		  decision.setBit(i, true);
		  output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		}
		if(fillAccept and not alreadyFilled) {
		  m_histAcceptExclusiveJets[i]->Fill(xi_1);
		  } else if(fillReject) {
		  m_histRejectExclusiveJets[i]->Fill(xi_1);
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
