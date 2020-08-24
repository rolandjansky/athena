/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaPhiIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V SOrin 2014
 *
 * @brief algorithm calculates the phi-distance between one  lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include "TH1F.h"

#include "L1TopoAlgorithms/DeltaPhiIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaPhiIncl1)

using namespace std;

TCS::DeltaPhiIncl1::DeltaPhiIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 0);
   defineParameter("MaxTob", 0);
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

TCS::DeltaPhiIncl1::~DeltaPhiIncl1(){}


TCS::StatusCode
TCS::DeltaPhiIncl1::initialize() {

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
   
   // create strings for histogram names
   std::vector<std::ostringstream> MyAcceptHist(numberOutputBits());
   std::vector<std::ostringstream> MyRejectHist(numberOutputBits());
   int dphi_bin=100;
   float dphi_min=0;
   float dphi_max=3.4;
   
   for (unsigned int i=0;i<numberOutputBits();i++) {
     MyAcceptHist[i] << "Accept" << p_DeltaPhiMin[i] << "DPHI" << p_DeltaPhiMax[i];
     MyRejectHist[i] << "Reject" << p_DeltaPhiMin[i] << "DPHI" << p_DeltaPhiMax[i];
   }


   for (unsigned int i=0; i<numberOutputBits();i++) {

     const std::string& MyTitle1 = MyAcceptHist[i].str();
     const std::string& MyTitle2 = MyRejectHist[i].str();
     
     registerHist(m_histAcceptDPhi2[i] = new TH1F(MyTitle1.c_str(),MyTitle1.c_str(),dphi_bin,dphi_min,dphi_max));
     registerHist(m_histRejectDPhi2[i] = new TH1F(MyTitle2.c_str(),MyTitle2.c_str(),dphi_bin,dphi_min,dphi_max));
   }
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaPhiIncl1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
    if(input.size() == 1) {
        TRG_MSG_DEBUG("input size     : " << input[0]->size());
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
                    // DeltaPhi cuts
                    unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhiBW( *tob1, *tob2 );
                    std::stringstream msgss;
                    msgss << "  phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                          << ", DeltaPhi = " << deltaPhi << " -> ";
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                        if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
                        accept = deltaPhi >= p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i];
 			const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
			const bool fillReject = fillHistos() and not fillAccept;
			const bool alreadyFilled = decision.bit(i);
                        if( accept ) {
                            decision.setBit(i, true);  
                            output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                        }
 			if(fillAccept and not alreadyFilled) {
			  fillHist1D(m_histAcceptDPhi2[i]->GetName(),deltaPhi);
			} else if(fillReject) {
			  fillHist1D(m_histRejectDPhi2[i]->GetName(),deltaPhi);
			}
                        TRG_MSG_DEBUG("DeltaPhiIncl1 = " << deltaPhi << " -> " 
                                      << (accept?"pass":"fail"));
                    }
                }
            }
    } else {
        TCS_EXCEPTION("DeltaPhiIncl1 alg must have  1 input, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaPhiIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
    if(input.size() == 1) {
        TRG_MSG_DEBUG("input size     : " << input[0]->size());
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
                    // DeltaPhi cuts
                    unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhi( *tob1, *tob2 );
                    std::stringstream msgss;
                    msgss << "    Combination : " << distance( input[0]->begin(), tob1) 
                          << " x " << distance( input[0]->begin(), tob2)
                          << "  phi1=" << (*tob1)->phi()
                          << " , phi2=" << (*tob2)->phi()
                          << ", DeltaPhi = " << deltaPhi << " -> ";
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
                        bool accept = false;
                        if( parType_t((*tob1)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                        if( parType_t((*tob2)->Et()) <= min(p_MinET1[i],p_MinET2[i])) continue; // ET cut
                        if( (parType_t((*tob1)->Et()) <= max(p_MinET1[i],p_MinET2[i])) && (parType_t((*tob2)->Et()) <= max(p_MinET1[i],p_MinET2[i]))) continue;
                        accept = deltaPhi >= p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i];
 			const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
			const bool fillReject = fillHistos() and not fillAccept;
			const bool alreadyFilled = decision.bit(i);
			if( accept ) {
                            decision.setBit(i, true);  
                            output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                        }
 			if(fillAccept and not alreadyFilled) {
			  fillHist1D(m_histAcceptDPhi2[i]->GetName(),deltaPhi);
			} else if(fillReject) {
			  fillHist1D(m_histRejectDPhi2[i]->GetName(),deltaPhi);
			}
                        TRG_MSG_DEBUG("DeltaPhiIncl1 = " << deltaPhi << " -> " 
                                      << (accept?"pass":"fail"));
                    }
                }
            }
    } else {
        TCS_EXCEPTION("DeltaPhiIncl1 alg must have  1 input, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
