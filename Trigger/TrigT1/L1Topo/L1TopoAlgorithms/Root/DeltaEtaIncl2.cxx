/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaEtaIncl2.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified V Sorin 2014
 *
 * @brief algorithm calculates the eta-distance between two lists and applies delta-eta criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "TH1F.h"

#include "L1TopoAlgorithms/DeltaEtaIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaEtaIncl2)

// not the best solution but we will move to athena where this comes for free
#define LOG cout << fullname() << ":     "





TCS::DeltaEtaIncl2::DeltaEtaIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 0);
   defineParameter("InputWidth2", 0); 
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
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

TCS::DeltaEtaIncl2::~DeltaEtaIncl2(){}


TCS::StatusCode
TCS::DeltaEtaIncl2::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

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
       std::string hname_accept = "hDeltaEtaIncl2_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hDeltaEtaIncl2_reject_bit"+std::to_string((int)i);
       // mass
       bookHist(m_histAccept, hname_accept, "DETA", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i]);
       bookHist(m_histReject, hname_reject, "DETA", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i]);
   }


   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaEtaIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
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
                    // test DeltaEtaMin, DeltaEtaMax
                    unsigned int deltaEta = TSU::Kinematics::calcDeltaEtaBW( *tob1, *tob2 );
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
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
                        TRG_MSG_DEBUG("DeltaEta = " << deltaEta << " -> accept bit  " << i << " -> "
                                      << (accept?"pass":"fail"));
                    }
                }
            }
    } else {
        TCS_EXCEPTION("DeltaEtaIncl2 alg must have 2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaEtaIncl2::process( const std::vector<TCS::TOBArray const *> & input,
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
                    // test DeltaEtaMin, DeltaEtaMax
                    unsigned int deltaEta = TSU::Kinematics::calcDeltaEta( *tob1, *tob2 );
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
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
                        TRG_MSG_DEBUG("DeltaEta = " << deltaEta << " -> accept bit  " << i << " -> " 
                                      << (accept?"pass":"fail"));
                    } 
                }
            }
    } else {
        TCS_EXCEPTION("DeltaEtaIncl2 alg must have 2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}

