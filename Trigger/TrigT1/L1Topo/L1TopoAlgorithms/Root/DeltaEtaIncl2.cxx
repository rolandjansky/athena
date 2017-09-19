/*********************************
 * DeltaEtaIncl2.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified V Sorin 2014
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
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

using namespace std;

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

   // create strings for histogram names
   std::vector<std::ostringstream> MyAcceptHist(numberOutputBits());
   std::vector<std::ostringstream> MyRejectHist(numberOutputBits());
   
   for (unsigned int i=0;i<numberOutputBits();i++) {
     MyAcceptHist[i] << "Accept" << p_DeltaEtaMin[i] << "DEta"; 
     MyRejectHist[i] << "Reject" << p_DeltaEtaMin[i] << "DEta";
   }

   for (unsigned int i=0; i<numberOutputBits();i++) {

     const std::string& MyTitle1 = MyAcceptHist[i].str();
     const std::string& MyTitle2 = MyRejectHist[i].str();
       
     registerHist(m_histAcceptDEta2[i] = new TH1F(MyTitle1.c_str(),MyTitle1.c_str(),100,0.,4.));
     registerHist(m_histRejectDEta2[i] = new TH1F(MyTitle2.c_str(),MyTitle2.c_str(),100,0.,4.));
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
                            m_histAcceptDEta2[i]->Fill((float)deltaEta*0.10);
                        } else if(fillReject) {
                            m_histRejectDEta2[i]->Fill((float)deltaEta*0.10);
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
                            m_histAcceptDEta2[i]->Fill((float)deltaEta*0.10);
                        } else if(fillReject) {
                            m_histRejectDEta2[i]->Fill((float)deltaEta*0.10);
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

