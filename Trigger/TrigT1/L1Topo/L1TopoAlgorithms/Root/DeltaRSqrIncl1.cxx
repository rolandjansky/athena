/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaRSqrIncl1.cpp
 * Created by Joerg Stelzer / V Sorin on 9/16/14.
 *
 * @brief algorithm calculates the R2-distance between objects in one  list and applies dR criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include "TH1F.h"
#include "TH2F.h"

#include "L1TopoAlgorithms/DeltaRSqrIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaRSqrIncl1)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::DeltaRSqrIncl1:     "

TCS::DeltaRSqrIncl1::DeltaRSqrIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 9);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 3);
   defineParameter("RequireOneBarrel", 0);
   defineParameter("MinET1",1);
   defineParameter("MinET2",1);
   defineParameter("DeltaRMin",  0, 0);
   defineParameter("DeltaRMax",  0, 0);
   defineParameter("DeltaRMin",  0, 1);
   defineParameter("DeltaRMax",  0, 1);
   defineParameter("DeltaRMin",  0, 2);
   defineParameter("DeltaRMax",  0, 2);
   setNumberOutputBits(3);
}

TCS::DeltaRSqrIncl1::~DeltaRSqrIncl1(){}


TCS::StatusCode
TCS::DeltaRSqrIncl1::initialize() {
   if(parameter("MaxTob").value() > 0) {
    p_NumberLeading1 = parameter("MaxTob").value();
    p_NumberLeading2 = parameter("MaxTob").value();
   } else {
    p_NumberLeading1 = parameter("InputWidth").value();
    p_NumberLeading2 = parameter("InputWidth").value();
   }
   p_OneBarrel = parameter("RequireOneBarrel").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_DeltaRMin[i] = parameter("DeltaRMin", i).value();
      p_DeltaRMax[i] = parameter("DeltaRMax", i).value();
   }
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("RequireOneBarrel : " << p_OneBarrel);

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    TRG_MSG_INFO("DeltaRMin   : " << p_DeltaRMin[i]);
    TRG_MSG_INFO("DeltaRMax   : " << p_DeltaRMax[i]);
   }
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);

   TRG_MSG_INFO("number output : " << numberOutputBits());
   for (unsigned int i=0; i<numberOutputBits();i++) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int deltaR_max = sqrt(p_DeltaRMax[i]);
       // mass
       snprintf(hname_accept, buf_len, "Accept_DeltaRSqrIncl1_bit%d", i);
       snprintf(hname_reject, buf_len, "Reject_DeltaRSqrIncl1_bit%d", i);
       registerHist(m_histAccept[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*deltaR_max));
       registerHist(m_histReject[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*deltaR_max));
   }
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaRSqrIncl1::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 1) {
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
          {
              if( parType_t((*tob1)->Et()) <= min(p_MinET1,p_MinET2)) continue; // ET cut
              TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
              for( ;
                   tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                   ++tob2) {
                  if( parType_t((*tob2)->Et()) <= min(p_MinET1,p_MinET2)) continue; // ET cut
                  if( (parType_t((*tob1)->Et()) <= max(p_MinET1,p_MinET2)) && (parType_t((*tob2)->Et()) <= max(p_MinET1,p_MinET2))) continue;
                  // OneBarrel
                  if (p_OneBarrel && parType_t(abs((*tob1)->eta())) > 10 && parType_t(abs((*tob2)->eta())) > 10 ) continue;
                  // DeltaR2 cuts
                  unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob2 );
                  for(unsigned int i=0; i<numberOutputBits(); ++i) {
		    bool accept = false;
		    accept = deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i];
		    const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		    const bool fillReject = fillHistos() and not fillAccept;
		    const bool alreadyFilled = decision.bit(i);
		    if( accept ) {
		      decision.setBit(i, true);  
		      output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		    }
		    if(fillAccept and not alreadyFilled) {
		      fillHist1D(m_histAccept[i]->GetName(),sqrt((float)deltaR2));
		    } else if(fillReject) {
		      fillHist1D(m_histReject[i]->GetName(),sqrt((float)deltaR2));
		    }
		    TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
                  }
              }
          }
   } else {
      TCS_EXCEPTION("DeltaRSqrIncl1 alg must have either 1 input, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaRSqrIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
    if(input.size() == 1) {
        for( TOBArray::const_iterator tob1 = input[0]->begin(); 
             tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
             ++tob1) 
            {
                if( parType_t((*tob1)->Et()) <= min(p_MinET1,p_MinET2)) continue; // ET cut
                TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
                for( ;
                     tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    if( parType_t((*tob2)->Et()) <= min(p_MinET1,p_MinET2)) continue; // ET cut
                    if( (parType_t((*tob1)->Et()) <= max(p_MinET1,p_MinET2)) && (parType_t((*tob2)->Et()) <= max(p_MinET1,p_MinET2))) continue;
                    // OneBarrel
                    if (p_OneBarrel && parType_t(abs((*tob1)->eta())) > 10 && parType_t(abs((*tob2)->eta())) > 10 ) continue;
                    // DeltaR2 cuts
                    unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2( *tob1, *tob2 );
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
		      bool accept = false;
		      accept = deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i];
		      const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		      const bool fillReject = fillHistos() and not fillAccept;
		      const bool alreadyFilled = decision.bit(i);
		      if( accept ) {
                        decision.setBit(i, true);  
                        output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		      }
		      if(fillAccept and not alreadyFilled) {
			fillHist1D(m_histAccept[i]->GetName(),sqrt((float)deltaR2));
		      } else if(fillReject) {
			fillHist1D(m_histReject[i]->GetName(),sqrt((float)deltaR2));
		      }
		      TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
                    }
                }
            }
    } else {
      TCS_EXCEPTION("DeltaRSqrIncl1 alg must have either 1 input, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
