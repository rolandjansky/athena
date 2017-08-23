/*********************************
 * DeltaPhiIncl2.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V Sorin
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
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

   // create strings for histogram names
   std::vector<std::ostringstream> MyAcceptHist(numberOutputBits());
   std::vector<std::ostringstream> MyRejectHist(numberOutputBits());
   
   for (unsigned int i=0;i<numberOutputBits();i++) {
     MyAcceptHist[i] << "Accept" << p_DeltaPhiMin[i] << "DPHI" << p_DeltaPhiMax[i];
     MyRejectHist[i] << "Reject" << p_DeltaPhiMin[i] << "DPHI" << p_DeltaPhiMax[i];
   }


   for (unsigned int i=0; i<numberOutputBits();i++) {

     const std::string& MyTitle1 = MyAcceptHist[i].str();
     const std::string& MyTitle2 = MyRejectHist[i].str();
     
     registerHist(m_histAcceptDPhi2[i] = new TH1F(MyTitle1.c_str(),MyTitle1.c_str(),100,0,3.4));
     registerHist(m_histRejectDPhi2[i] = new TH1F(MyTitle2.c_str(),MyTitle2.c_str(),100,0,3.4));
   }
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaPhiIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
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
                        if( accept ) {
                            decison.setBit(i, true);
                            output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                            //if (i == 0) {
                            if (!iaccept[i]) {
                                iaccept[i]=1;
                                m_histAcceptDPhi2[i]->Fill(deltaPhi);
                            }
                            //}
                        }
                        else {
                            //if (i==0)
                            m_histRejectDPhi2[i]->Fill(deltaPhi);
                        }
                        TRG_MSG_DEBUG("DeltaPhi = " << deltaPhi << " -> " 
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
                             Decision & decison )
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
                        if( accept ) {
                            decison.setBit(i, true);
                            output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                            //if (i == 0) {
                            if (!iaccept[i]) {
                                iaccept[i]=1;
                                m_histAcceptDPhi2[i]->Fill(deltaPhi);
                            }
                            //}
                        }
                        else {
                            //if (i==0)
                            m_histRejectDPhi2[i]->Fill(deltaPhi);
                        }
                        TRG_MSG_DEBUG("DeltaPhi = " << deltaPhi << " -> " 
                                      << (accept?"pass":"fail"));
                    }
                }
            }
    } else {
        TCS_EXCEPTION("DeltaPhiIncl2 alg must have  2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
