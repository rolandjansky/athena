/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaEtaPhiIncl1.cpp
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

#include "L1TopoAlgorithms/DeltaEtaPhiIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaEtaPhiIncl1)

using namespace std;

TCS::DeltaEtaPhiIncl1::DeltaEtaPhiIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0);
   defineParameter("NumResultBits", 3);
   defineParameter("MinET1",1,0);
   defineParameter("MinET2",1,0);
   defineParameter("MinDeltaEta",  0, 0);
   defineParameter("MaxDeltaEta", 49, 0);
   defineParameter("MinDeltaPhi",  0, 0);
   defineParameter("MaxDeltaPhi", 63, 0);
   defineParameter("MinET1",1,1);
   defineParameter("MinET2",1,1);
   defineParameter("MinDeltaEta",  0, 1);
   defineParameter("MaxDeltaEta", 49, 1);
   defineParameter("MinDeltaPhi",  0, 1);
   defineParameter("MaxDeltaPhi", 63, 1);
   defineParameter("MinET1",1,2);
   defineParameter("MinET2",1,2);
   defineParameter("MinDeltaEta",  0, 2);
   defineParameter("MaxDeltaEta", 49, 2);
   defineParameter("MinDeltaPhi",  0, 2);
   defineParameter("MaxDeltaPhi", 63, 2);
   setNumberOutputBits(3);
}

TCS::DeltaEtaPhiIncl1::~DeltaEtaPhiIncl1(){}


TCS::StatusCode
TCS::DeltaEtaPhiIncl1::initialize() {

   if(parameter("MaxTob").value() > 0) {
    p_NumberLeading1 = parameter("MaxTob").value();
    p_NumberLeading2 = parameter("MaxTob").value();
   } else {
    p_NumberLeading1 = parameter("InputWidth").value();
    p_NumberLeading2 = parameter("InputWidth").value();
   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_DeltaPhiMin[i] = parameter("MinDeltaPhi", i).value();
      p_DeltaPhiMax[i] = parameter("MaxDeltaPhi", i).value();
      p_DeltaEtaMin[i] = parameter("MinDeltaEta", i).value();
      p_DeltaEtaMax[i] = parameter("MaxDeltaEta", i).value();
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
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int EtaPhi_bin=100;
       float EtaPhi_min=0;
       float EtaPhi_max=70;
       // eta2 vs. eta1
       snprintf(hname_accept, buf_len, "Accept_DeltaEtaPhiIncl1_bit%d", i);
       snprintf(hname_reject, buf_len, "Reject_DeltaEtaPhiIncl1_bit%d", i);
       registerHist(m_histAccept[i] = new TH2F(hname_accept, hname_accept, EtaPhi_bin, EtaPhi_min, EtaPhi_max, EtaPhi_bin, EtaPhi_min, EtaPhi_max));
       registerHist(m_histReject[i] = new TH2F(hname_reject, hname_reject, EtaPhi_bin, EtaPhi_min, EtaPhi_max, EtaPhi_bin, EtaPhi_min, EtaPhi_max));
   }   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaEtaPhiIncl1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
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
               TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
               for( ;
                    tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                    ++tob2) {
                   // DeltaPhi cuts
                   unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhiBW( *tob1, *tob2 );
                   // DeltaEta cuts
                   unsigned int deltaEta = TSU::Kinematics::calcDeltaEtaBW( *tob1, *tob2 );
                   //
                   // to-do change message output
                   std::stringstream msgss;
                   msgss << "  phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                         << ", DeltaPhi = " << deltaPhi << ", DeltaEta = " << deltaEta << " -> ";
                   for(unsigned int i=0; i<numberOutputBits(); ++i) {
                       bool accept = false;
                       if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                       if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                       if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
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
                        msgss << "DeltaEtaPhiIncl1 alg bit" << i << (accept?" pass":" fail") << "|";
                   }
                   TRG_MSG_DEBUG(msgss.str());
               }
           }
    } else {
        TCS_EXCEPTION("DeltaEtaPhiIncl1 alg must have  1 input, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaEtaPhiIncl1::process( const std::vector<TCS::TOBArray const *> & input,
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
                TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
                for( ;
                     tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    // DeltaPhi cuts
                    unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhi( *tob1, *tob2 );
                    // DeltaEta cuts
                    unsigned int deltaEta = TSU::Kinematics::calcDeltaEta( *tob1, *tob2 );
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
                        if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                        if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
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
                        msgss << "DeltaEtaPhiIncl1 alg bit" << i << (accept?" pass":" fail") << "|";
                     }
                    TRG_MSG_DEBUG(msgss.str());
                }
            }
    } else {
        TCS_EXCEPTION("DeltaEtaPhiIncl1 alg must have  1 input, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
