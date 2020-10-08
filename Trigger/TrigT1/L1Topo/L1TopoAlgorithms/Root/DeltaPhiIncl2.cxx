/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaPhiIncl2.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V Sorin
 *
 * @brief algorithm calculates the phi-distance between  two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "TH1F.h"

#include "L1TopoAlgorithms/DeltaPhiIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaPhiIncl2)

using namespace std;

TCS::DeltaPhiIncl2::DeltaPhiIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 8);
   defineParameter("InputWidth2", 8);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 2);
   defineParameter("MinET1",0,0);
   defineParameter("MinET2",0,0); 
   defineParameter("MinDeltaPhi",  0, 0);
   defineParameter("MaxDeltaPhi", 31, 0);
   defineParameter("MinET1",0,1);
   defineParameter("MinET2",0,1);
   defineParameter("MinDeltaPhi",  0, 1);
   defineParameter("MaxDeltaPhi", 31, 1);
   setNumberOutputBits(2);
}

TCS::DeltaPhiIncl2::~DeltaPhiIncl2(){}


TCS::StatusCode
TCS::DeltaPhiIncl2::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_DeltaPhiMin[i] = parameter("MinDeltaPhi", i).value();
      p_DeltaPhiMax[i] = parameter("MaxDeltaPhi", i).value();
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();
      
      TRG_MSG_INFO("DeltaPhiMin   : " << p_DeltaPhiMin[i]);
      TRG_MSG_INFO("DeltaPhiMax   : " << p_DeltaPhiMax[i]);
      TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
      TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);

   }

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hDeltaPhiIncl2_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hDeltaPhiIncl2_reject_bit"+std::to_string((int)i);
       // mass
       bookHist(m_histAccept, hname_accept, "DPHI", 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
       bookHist(m_histReject, hname_reject, "DPHI", 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
   }

   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaPhiIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
    if( input.size() == 2) {
        std::vector<bool> iaccept (numberOutputBits());
        for( TOBArray::const_iterator tob1 = input[0]->begin(); 
             tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
             ++tob1)
            {
                for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                     tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    // test DeltaPhiMin, DeltaPhiMax
                    unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhiBW( *tob1, *tob2 );
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                        accept = deltaPhi >= p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i];
			const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
			const bool fillReject = fillHistos() and not fillAccept;
			const bool alreadyFilled = decision.bit(i);
                        if( accept ) {
                            decision.setBit(i, true);
                            output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
			}
 			if(fillAccept and not alreadyFilled) {
			  fillHist1D(m_histAccept[i],deltaPhi);
			} else if(fillReject) {
			  fillHist1D(m_histReject[i],deltaPhi);
			}
                       TRG_MSG_DEBUG("DeltaPhiIncl2 = " << deltaPhi << " -> " 
                                      << (accept?"pass":"fail"));
                    }
                }
            }
    } else {
      TCS_EXCEPTION("DeltaPhiIncl2 alg must have  2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::DeltaPhiIncl2::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
    if( input.size() == 2) {
        std::vector<bool> iaccept (numberOutputBits());
        for( TOBArray::const_iterator tob1 = input[0]->begin(); 
             tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
             ++tob1)
            {
                for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                     tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    // test DeltaPhiMin, DeltaPhiMax
                    unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhi( *tob1, *tob2 );
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                        accept = deltaPhi >= p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i];
 			const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
			const bool fillReject = fillHistos() and not fillAccept;
			const bool alreadyFilled = decision.bit(i);
                        if( accept ) {
                            decision.setBit(i, true);
                            output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
			}
 			if(fillAccept and not alreadyFilled) {
			  fillHist1D(m_histAccept[i],deltaPhi);
			} else if(fillReject) {
			  fillHist1D(m_histReject[i],deltaPhi);
			}
                        TRG_MSG_DEBUG("DeltaPhiIncl2 = " << deltaPhi << " -> " 
                                      << (accept?"pass":"fail"));
                    }
                }
            }
    } else {
        TCS_EXCEPTION("DeltaPhiIncl2 alg must have  2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
