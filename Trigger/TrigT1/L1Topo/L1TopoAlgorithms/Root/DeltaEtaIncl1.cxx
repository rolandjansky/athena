/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaEtaIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V Sorin 2014
 *
 * @brief algorithm calculates the eta-distance between two TOB from one list and applies delta-eta criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "TH1F.h"

#include "L1TopoAlgorithms/DeltaEtaIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaEtaIncl1)

// not the best solution but we will move to athena where this comes for free
#define LOG cout << fullname() << ":     "


TCS::DeltaEtaIncl1::DeltaEtaIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 0);
   defineParameter("MaxTob", 0);
   defineParameter("NumResultBits", 4);
   defineParameter("MinET1",0,0);
   defineParameter("MinET2",0,0);
   defineParameter("MinET1",0,1);
   defineParameter("MinET2",0,1);
   defineParameter("MinET1",0,2);
   defineParameter("MinET2",0,2);
   defineParameter("MinET1",0,3);
   defineParameter("MinET2",0,3);

   defineParameter("MinDeltaEta",  0, 0);
   defineParameter("MaxDeltaEta", 127, 0);
   defineParameter("MinDeltaEta",  0, 1);
   defineParameter("MaxDeltaEta", 127, 1);
   defineParameter("MinDeltaEta",  0, 2);
   defineParameter("MaxDeltaEta", 127, 2);
   defineParameter("MinDeltaEta",  0, 3);
   defineParameter("MaxDeltaEta", 127, 3);

   setNumberOutputBits(4);
}

TCS::DeltaEtaIncl1::~DeltaEtaIncl1(){}


TCS::StatusCode
TCS::DeltaEtaIncl1::initialize() {

   if(parameter("MaxTob").value() > 0) {
    p_NumberLeading1 = parameter("MaxTob").value();
    p_NumberLeading2 = parameter("MaxTob").value();
   } else {
    p_NumberLeading1 = parameter("InputWidth").value();
    p_NumberLeading2 = parameter("InputWidth").value();
   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_DeltaEtaMin[i] = parameter("MinDeltaEta", i).value();
      p_DeltaEtaMax[i] = parameter("MaxDeltaEta", i).value();

      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();
   }

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    TRG_MSG_INFO("DeltaEtaMin0   : " << p_DeltaEtaMin[i]);
    TRG_MSG_INFO("DeltaEtaMax0   : " << p_DeltaEtaMax[i]);
    TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
    TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hDeltaEtaIncl1_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hDeltaEtaIncl1_reject_bit"+std::to_string((int)i);
       // mass
       bookHist(m_histAccept, hname_accept, "DETA", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i]);
       bookHist(m_histReject, hname_reject, "DETA", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i]);
   }


   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::DeltaEtaIncl1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
   if(input.size() == 1) {
       unsigned int nLeading = p_NumberLeading1;
       unsigned int nLeading2 = p_NumberLeading2;
       for( TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
            ++tob1)
           {
               TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;
               for( ;
                    tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < nLeading2;
                    ++tob2) {
                   for(unsigned int i=0; i < numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= std::min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= std::min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( (parType_t((*tob1)->Et()) <= std::max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= std::max(p_MinET1[i],p_MinET2[i]))) continue;
                   // DeltaEta cuts
                   unsigned int deltaEta = TSU::Kinematics::calcDeltaEtaBW( *tob1, *tob2 );
                   std::stringstream msgss;
                   msgss << "Combination : " << distance( input[0]->begin(), tob1)
                         << " x " << distance( input[0]->begin(), tob2)
                         << " eta=" << (*tob1)->eta()
                         << " , eta=" << (*tob2)->eta() 
                         << ", DeltaEta = " << deltaEta << " -> ";
                   accept = deltaEta >= p_DeltaEtaMin[i] && deltaEta <= p_DeltaEtaMax[i];
                   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                   const bool fillReject = fillHistos() and not fillAccept;
                   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                   }
                   if(fillAccept and not alreadyFilled) {
                       fillHist1D(m_histAccept[i],(float)deltaEta);
                   } else if(fillReject) {
                       fillHist1D(m_histReject[i],(float)deltaEta);
                   }
                   msgss << (accept?"pass":"fail") << "|";
                   TRG_MSG_DEBUG(msgss.str());
                   }
               }
           }
   } else {
       TCS_EXCEPTION("DeltaEtaIncl1 alg must have 1 input, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaEtaIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
   if(input.size() == 1) {
       //LOG << "input size     : " << input[0]->size() << endl;
       unsigned int nLeading = p_NumberLeading1;
       unsigned int nLeading2 = p_NumberLeading2;
       for( TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
            ++tob1)
           {
               TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;
               for( ;
                    tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < nLeading2;
                    ++tob2) {
                   for(unsigned int i=0; i < numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= std::min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( parType_t((*tob2)->Et()) <= std::min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                   if( (parType_t((*tob1)->Et()) <= std::max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= std::max(p_MinET1[i],p_MinET2[i]))) continue;
                   // DeltaEta cuts
                   unsigned int deltaEta = TSU::Kinematics::calcDeltaEta( *tob1, *tob2 );
                   std::stringstream msgss;
                   msgss << "Combination : " << distance( input[0]->begin(), tob1)
                         << " x " << distance( input[0]->begin(), tob2)
                         << " eta=" << (*tob1)->eta()
                         << " , eta=" << (*tob2)->eta()
                         << ", DeltaEta = " << deltaEta << " -> ";
                   accept = deltaEta >= p_DeltaEtaMin[i] && deltaEta <= p_DeltaEtaMax[i];
                   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                   const bool fillReject = fillHistos() and not fillAccept;
                   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                   }
                   if(fillAccept and not alreadyFilled) {
                       fillHist1D(m_histAccept[i],(float)deltaEta);
                   } else if(fillReject) {
                       fillHist1D(m_histReject[i],(float)deltaEta);
                   }
                   msgss << (accept?"pass":"fail") << "|";
                   TRG_MSG_DEBUG(msgss.str());
                   }
               }
           }
   } else {
       TCS_EXCEPTION("DeltaEtaIncl1 alg must have 1 input, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
