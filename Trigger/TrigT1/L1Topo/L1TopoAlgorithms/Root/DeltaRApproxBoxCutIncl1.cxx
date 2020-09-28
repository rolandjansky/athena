/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaRApproxBoxCutIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V Sorin 
 *
 * @brief algorithm calculates the eta and  phi-distance between one lists and applies box cut criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include "TH1F.h"
#include "TH2F.h"

#include "L1TopoAlgorithms/DeltaRApproxBoxCutIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaRApproxBoxCutIncl1)

namespace {
   unsigned int
   calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;
      
      return round( 10 * dphi );
   }

   unsigned int
   calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = fabs( tob1->eta() - tob2->eta() );
      return deta;
   }
}


TCS::DeltaRApproxBoxCutIncl1::DeltaRApproxBoxCutIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("NumResultBits", 3);
   defineParameter("DeltaPhiMin",  0, 0);
   defineParameter("DeltaPhiMax", 63, 0);
   defineParameter("DeltaPhiMin",  0, 1);
   defineParameter("DeltaPhiMax",  5, 1);
   defineParameter("DeltaPhiMin", 25, 2);
   defineParameter("DeltaPhiMax", 32, 2);
   defineParameter("DeltaEtaMin",  0, 0);
   defineParameter("DeltaEtaMax", 63, 0);
   defineParameter("DeltaEtaMin",  0, 1);
   defineParameter("DeltaEtaMax",  5, 1);
   defineParameter("DeltaEtaMin", 25, 2);
   defineParameter("DeltaEtaMax", 32, 2);
   defineParameter("MinET1",1);
   defineParameter("MinET2",1);
   setNumberOutputBits(3);
}

TCS::DeltaRApproxBoxCutIncl1::~DeltaRApproxBoxCutIncl1(){}


TCS::StatusCode
TCS::DeltaRApproxBoxCutIncl1::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   p_NumberLeading2 = parameter("InputWidth").value();
   for(int i=0; i<3; ++i) {
      p_DeltaPhiMin[i] = parameter("DeltaPhiMin", i).value();
      p_DeltaPhiMax[i] = parameter("DeltaPhiMax", i).value();
      p_DeltaEtaMin[i] = parameter("DeltaEtaMin", i).value();
      p_DeltaEtaMax[i] = parameter("DeltaEtaMax", i).value();

   }
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("DeltaPhiMin0   : " << p_DeltaPhiMin[0]);
   TRG_MSG_INFO("DeltaPhiMax0   : " << p_DeltaPhiMax[0]);
   TRG_MSG_INFO("DeltaPhiMin1   : " << p_DeltaPhiMin[1]);
   TRG_MSG_INFO("DeltaPhiMax1   : " << p_DeltaPhiMax[1]);
   TRG_MSG_INFO("DeltaPhiMin2   : " << p_DeltaPhiMin[2]);
   TRG_MSG_INFO("DeltaPhiMax2   : " << p_DeltaPhiMax[2]);
   TRG_MSG_INFO("DeltaEtaMin0   : " << p_DeltaEtaMin[0]);
   TRG_MSG_INFO("DeltaEtaMax0   : " << p_DeltaEtaMax[0]);
   TRG_MSG_INFO("DeltaEtaMin1   : " << p_DeltaEtaMin[1]);
   TRG_MSG_INFO("DeltaEtaMax1   : " << p_DeltaEtaMax[1]);
   TRG_MSG_INFO("DeltaEtaMin2   : " << p_DeltaEtaMin[2]);
   TRG_MSG_INFO("DeltaEtaMax2   : " << p_DeltaEtaMax[2]);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hDeltaRApproxBoxCutIncl1_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hDeltaRApproxBoxCutIncl1_reject_bit"+std::to_string((int)i);
       // deta vs dphi
       bookHist(m_histAccept, hname_accept, "DETA vs DPHI", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i], 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
       bookHist(m_histReject, hname_reject, "DETA vs DPHI", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i], 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
   }
   
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::DeltaRApproxBoxCutIncl1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}


TCS::StatusCode
TCS::DeltaRApproxBoxCutIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                                       const std::vector<TCS::TOBArray *> & output,
                                       Decision & decision )
{
    if(input.size() == 1) {
        TRG_MSG_DEBUG("input size     : " << input[0]->size());
        unsigned int nLeading = p_NumberLeading1;
        for( TOBArray::const_iterator tob1 = input[0]->begin(); 
             tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
             ++tob1) 
            {
                if( parType_t((*tob1)->Et()) <= std::min(p_MinET1,p_MinET2)) continue; // ET cut
                TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
                for( ;
                     tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    if( parType_t((*tob2)->Et()) <= std::min(p_MinET1,p_MinET2)) continue; // ET cut
                    if( (parType_t((*tob1)->Et()) <= std::max(p_MinET1,p_MinET2)) && (parType_t((*tob2)->Et()) <= std::max(p_MinET1,p_MinET2))) continue;
                    // DeltaPhi cuts
                    unsigned int deltaPhi = calcDeltaPhi( *tob1, *tob2 );
                    // DeltaEta cuts
                    unsigned int deltaEta = calcDeltaEta( *tob1, *tob2 );
                    //
                    // to-do change message output
                    std::stringstream msgss;
                    msgss << "    Combination : " << distance( input[0]->begin(), tob1)
                          << " x " << distance( input[0]->begin(), tob2)
                          << "  phi1=" << (*tob1)->phi()
                          << " , phi2=" << (*tob2)->phi()
                          << ", DeltaPhi = " << deltaPhi
                          << ", DeltaEta = " << deltaEta << " -> ";
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        accept = ( deltaEta >= p_DeltaEtaMin[i] ||  deltaPhi >= p_DeltaPhiMin[i] ) && deltaPhi <= p_DeltaPhiMax[i] && deltaEta <= p_DeltaEtaMax[i];
			const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
			const bool fillReject = fillHistos() and not fillAccept;
			const bool alreadyFilled = decision.bit(i);
                        if( accept ) {
                            decision.setBit(i, true);  
                            output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                        }
			if(fillAccept and not alreadyFilled) {
			  fillHist2D(m_histAccept[i],(float)deltaEta,(float)deltaPhi);
			} else if(fillReject) {
			  fillHist2D(m_histReject[i],(float)deltaEta,(float)deltaPhi);
			}
                        msgss << "DeltaRApproxBoxCutIncl1 alg bit" << i << (accept?" pass":" fail") << "|";
                    }
                    TRG_MSG_DEBUG(msgss.str());
                }
            }
    } else {
        TCS_EXCEPTION("DeltaRApproxBoxCutIncl1 alg must have  1 input, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
