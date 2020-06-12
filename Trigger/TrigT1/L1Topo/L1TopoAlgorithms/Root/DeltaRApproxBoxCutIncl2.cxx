/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaRApproxBoxCutIncl2.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V SOrin 2014
 * @brief algorithm calculates the eta and  phi-distance between  two lists and applies box cut  criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include "TH1F.h"
#include "TH2F.h"

#include "L1TopoAlgorithms/DeltaRApproxBoxCutIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaRApproxBoxCutIncl2)

using namespace std;

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


TCS::DeltaRApproxBoxCutIncl2::DeltaRApproxBoxCutIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 5);
   defineParameter("InputWidth2", 3); 
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

TCS::DeltaRApproxBoxCutIncl2::~DeltaRApproxBoxCutIncl2(){}


TCS::StatusCode
TCS::DeltaRApproxBoxCutIncl2::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
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
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int EtaPhi_bin=100;
       float EtaPhi_min=0;
       float EtaPhi_max=70;
       // eta2 vs. eta1
       snprintf(hname_accept, buf_len, "Accept_DeltaRApproxBoxCutIncl2_bit%d", i);
       snprintf(hname_reject, buf_len, "Reject_DeltaRApproxBoxCutIncl2_bit%d", i);
       registerHist(m_histAccept[i] = new TH2F(hname_accept, hname_accept, EtaPhi_bin, EtaPhi_min, EtaPhi_max, EtaPhi_bin, EtaPhi_min, EtaPhi_max));
       registerHist(m_histReject[i] = new TH2F(hname_reject, hname_reject, EtaPhi_bin, EtaPhi_min, EtaPhi_max, EtaPhi_bin, EtaPhi_min, EtaPhi_max));
   }      
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaRApproxBoxCutIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}



TCS::StatusCode
TCS::DeltaRApproxBoxCutIncl2::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
    if(input.size() == 2) {
        TRG_MSG_DEBUG("input size     : " << input[0]->size());
        unsigned int nLeading = p_NumberLeading1;
        for( TOBArray::const_iterator tob1 = input[0]->begin(); 
             tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
             ++tob1) 
            {
                if( parType_t((*tob1)->Et()) <= p_MinET1 ) continue; // ET cut
                for( TOBArray::const_iterator tob2 = input[1]->begin();
                     tob2 != input[1]->end() && distance( input[1]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut
                    // DeltaPhi cuts
                    unsigned int deltaPhi = calcDeltaPhi( *tob1, *tob2 );
                    // DeltaEta cuts
                    unsigned int deltaEta = calcDeltaEta( *tob1, *tob2 );
                    //
                    // to-do change message output
                    std::stringstream msgss;
                    msgss << "    Combination : " << distance( input[0]->begin(), tob1)
                          << " x " << distance( input[1]->begin(), tob2)
                          << "  phi1=" << (*tob1)->phi()
                          << " , phi2=" << (*tob2)->phi()
                          << ", DeltaPhi = " << deltaPhi
                          << ", DeltaEta = " << deltaEta <<" -> ";
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
			  fillHist2D(m_histAccept[i]->GetName(),(float)deltaEta,(float)deltaPhi);
			} else if(fillReject) {
			  fillHist2D(m_histReject[i]->GetName(),(float)deltaEta,(float)deltaPhi);
			}
                        msgss << "DeltaRApproxBoxCutIncl2 alg bit" << i << (accept?" pass":" fail") << "|";
                    }
                    TRG_MSG_DEBUG(msgss.str());
                }
            }
    } else {
      TCS_EXCEPTION("DeltaRApproxBoxCutIncl2 alg must have  2 inputs, but got " << input.size());
    }
   return TCS::StatusCode::SUCCESS;
}
