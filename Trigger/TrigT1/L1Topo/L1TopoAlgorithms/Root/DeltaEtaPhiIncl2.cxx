/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaEtaPhiIncl2.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V SOrin 2014
 * @brief algorithm calculates the eta and  phi-distance between  two lists and applies box cut  criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include "TH1F.h"
#include "TH2F.h"

#include "L1TopoAlgorithms/DeltaEtaPhiIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"


REGISTER_ALG_TCS(DeltaEtaPhiIncl2)


TCS::DeltaEtaPhiIncl2::DeltaEtaPhiIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 5);
   defineParameter("InputWidth2", 3); 
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 3);
   defineParameter("MinET1",1,0);
   defineParameter("MinET2",1,0);
   defineParameter("DeltaEtaMin",  0, 0);
   defineParameter("DeltaEtaMax", 63, 0);
   defineParameter("DeltaPhiMin",  0, 0);
   defineParameter("DeltaPhiMax", 63, 0);
   defineParameter("MinET1",1,1);
   defineParameter("MinET2",1,1);
   defineParameter("DeltaEtaMin",  0, 1);
   defineParameter("DeltaEtaMax",  5, 1);
   defineParameter("DeltaPhiMin",  0, 1);
   defineParameter("DeltaPhiMax",  5, 1);
   defineParameter("MinET1",1,2);
   defineParameter("MinET2",1,2);
   defineParameter("DeltaEtaMin", 25, 2);
   defineParameter("DeltaEtaMax", 32, 2);
   defineParameter("DeltaPhiMin", 25, 2);
   defineParameter("DeltaPhiMax", 32, 2);
   setNumberOutputBits(3);
}

TCS::DeltaEtaPhiIncl2::~DeltaEtaPhiIncl2(){}


TCS::StatusCode
TCS::DeltaEtaPhiIncl2::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_DeltaPhiMin[i] = parameter("DeltaPhiMin", i).value();
      p_DeltaPhiMax[i] = parameter("DeltaPhiMax", i).value();
      p_DeltaEtaMin[i] = parameter("DeltaEtaMin", i).value();
      p_DeltaEtaMax[i] = parameter("DeltaEtaMax", i).value();
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();

      TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
      TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);
      TRG_MSG_INFO("DeltaEtaMin   : " << p_DeltaEtaMin[i]);
      TRG_MSG_INFO("DeltaEtaMax   : " << p_DeltaEtaMax[i]);
      TRG_MSG_INFO("DeltaPhiMin   : " << p_DeltaPhiMin[i]);
      TRG_MSG_INFO("DeltaPhiMax   : " << p_DeltaPhiMax[i]);

   }


   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);

   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hDeltaEtaPhiIncl2_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hDeltaEtaPhiIncl2_reject_bit"+std::to_string((int)i);
       // deta vs dphi
       bookHist(m_histAccept, hname_accept, "DETA vs DPHI", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i], 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
       bookHist(m_histReject, hname_reject, "DETA vs DPHI", 100, p_DeltaEtaMin[i], p_DeltaEtaMax[i], 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
   }
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaEtaPhiIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
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
                for( TOBArray::const_iterator tob2 = input[1]->begin();
                     tob2 != input[1]->end() && distance( input[1]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    // DeltaPhi cuts
                    unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhiBW( *tob1, *tob2 );
                    // DeltaEta cuts
                    unsigned int deltaEta = TSU::Kinematics::calcDeltaEtaBW( *tob1, *tob2 );
                    //
                    // to-do change message output
                    std::stringstream msgss;
                    msgss << "    phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                          << ", DeltaPhi = " << deltaPhi << ", DeltaEta = " << deltaEta <<" -> ";
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= p_MinET1[i] ) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
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
                        msgss << "DeltaRApproxBoxCutIncl2 alg bit" << i << (accept?" pass":" fail") << "|";
                    }
                    TRG_MSG_DEBUG(msgss.str());
                }
            }
    } else {
        TCS_EXCEPTION("DeltaEtaPhiIncl2 alg must have  2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::DeltaEtaPhiIncl2::process( const std::vector<TCS::TOBArray const *> & input,
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
                for( TOBArray::const_iterator tob2 = input[1]->begin();
                     tob2 != input[1]->end() && distance( input[1]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    // DeltaPhi cuts
                    unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhi( *tob1, *tob2 );
                    // DeltaEta cuts
                    unsigned int deltaEta = TSU::Kinematics::calcDeltaEta( *tob1, *tob2 );
                    //
                    // to-do change message output
                    std::stringstream msgss;
                    msgss << "    Combination : " << distance( input[0]->begin(), tob1) << " x " << distance( input[1]->begin(), tob2) << "  phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                          << ", DeltaPhi = " << deltaPhi << ", DeltaEta = " << deltaEta <<" -> ";
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= p_MinET1[i] ) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
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
                        msgss << "DeltaRApproxBoxCutIncl2 alg bit" << i << (accept?" pass":" fail") << "|";
                    }
                    TRG_MSG_DEBUG(msgss.str());
                }
            }
    } else {
        TCS_EXCEPTION("DeltaEtaPhiIncl2 alg must have  2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
