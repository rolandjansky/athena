/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * SimpleCone.cpp
 *
 * @brief algorithm calculates the sum of jets ET around each jet within radius R, makes cut on leading sum ET
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "TH1F.h"

#include "L1TopoAlgorithms/SimpleCone.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(SimpleCone)

// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << name() << ":     "


TCS::SimpleCone::SimpleCone(const std::string & name) : DecisionAlg(name)
{  
   defineParameter("InputWidth", 0);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits",6);
   defineParameter("NumRegisters", 2); 
   defineParameter("MaxRSqr",10*10);
   defineParameter("MinET",0);
   defineParameter("MinEta",0);
   defineParameter("MaxEta",31);
   defineParameter("MinSumET",0,0);
   defineParameter("MinSumET",0,1);
   defineParameter("MinSumET",0,2);
   defineParameter("MinSumET",0,3);
   defineParameter("MinSumET",0,4);
   defineParameter("MinSumET",0,5);
   setNumberOutputBits(6);
}

TCS::SimpleCone::~SimpleCone()
{}


TCS::StatusCode
TCS::SimpleCone::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   if(parameter("MaxTob").value() > 0) p_NumberLeading1 = parameter("MaxTob").value();
   p_R2 = parameter("MaxRSqr").value();
   p_MinET  = parameter("MinET").value();
   p_EtaMin = parameter("MinEta").value();
   p_EtaMax = parameter("MaxEta").value();

   TRG_MSG_INFO("MaxTob          : " << p_NumberLeading1);
   TRG_MSG_INFO("MaxRSqr         : " << p_R2);
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("EtaMin         : " << p_EtaMin);
   TRG_MSG_INFO("EtaMax         : " << p_EtaMax);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_MinSumET[i] = parameter("MinSumET", i).value();
      TRG_MSG_INFO("SimpleCone " << i << " : " << p_MinSumET[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hSimpleCone_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hSimpleCone_reject_bit"+std::to_string((int)i);
       // mass
       bookHist(m_histAccept, hname_accept, "ET", 100, 0, p_MinSumET[i]);
       bookHist(m_histReject, hname_reject, "ET", 100, 0, p_MinSumET[i]);
   }

   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::SimpleCone::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}


TCS::StatusCode
TCS::SimpleCone::process( const std::vector<TCS::TOBArray const *> & input,
			     const std::vector<TCS::TOBArray *> & output,
			     Decision & decision )
{

  if(input.size()!=1) {
    TCS_EXCEPTION("SimpleCone alg must have exactly 1 input list, but got " << input.size());
  }

  unsigned int leadingET = 0;

  // loop over  jets
  for( TOBArray::const_iterator tob = input[0]->begin(); 
       tob != input[0]->end() && distance(input[0]->begin(), tob) < p_NumberLeading1;
       ++tob) {

    if( parType_t(std::abs((*tob)->eta())) > p_EtaMax ) continue; // Eta cut
    if( parType_t(std::abs((*tob)->eta())) < p_EtaMin ) continue; // Eta cut
    if( parType_t((*tob)->Et()) <= p_MinET ) continue; // E_T cut

    TRG_MSG_DEBUG("Jet : ET = " << (*tob)->Et());      
      
    unsigned int tmp_SumET = (*tob)->Et();
      
    for( TOBArray::const_iterator tob1 = input[0]->begin();
	 tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
	 ++tob1) {
	  
      if( tob1 == tob ) continue; // Avoid double counting of central jet 
      if( parType_t(std::abs((*tob1)->eta())) > p_EtaMax ) continue; // Eta cut
      if( parType_t(std::abs((*tob1)->eta())) < p_EtaMin ) continue; // Eta cut
      if( parType_t((*tob1)->Et()) <= p_MinET ) continue; // E_T cut
      
      double deta = ( (*tob)->etaDouble() - (*tob1)->etaDouble() );
      double dphi = fabs( (*tob)->phiDouble() - (*tob1)->phiDouble() );
      if(dphi>M_PI)
	dphi = 2*M_PI - dphi;
      
      if ( 100 * ((dphi)*(dphi) + (deta)*(deta) ) > p_R2) continue; // Exclude jets outside cone
      
      tmp_SumET += (*tob1)->Et();
    }

    if (tmp_SumET > leadingET) leadingET = tmp_SumET; 
  }

  for(unsigned int i=0; i<numberOutputBits(); ++i) {

    bool accept = leadingET > p_MinSumET[i];
    const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
    const bool fillReject = fillHistos() and not fillAccept;
    decision.setBit( i, accept );
    
    if(accept) {
      output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(leadingET,0,0) ) ));
    }
    if(fillAccept)
        fillHist1D(m_histAccept[i],leadingET);
    else if(fillReject)
        fillHist1D(m_histReject[i],leadingET);
    
    
    TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " SimpleCone = " << leadingET);
    
   }
  
   return TCS::StatusCode::SUCCESS;
}
