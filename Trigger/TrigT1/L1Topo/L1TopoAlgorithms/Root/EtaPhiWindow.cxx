/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "L1TopoAlgorithms/EtaPhiWindow.h"

#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

#include <cmath>
#include <iterator>
//
#include "TH1F.h"
#include "TH2F.h"

REGISTER_ALG_TCS(EtaPhiWindow)

using std::distance;

//----------------------------------------------------------
TCS::EtaPhiWindow::EtaPhiWindow(const std::string & name) : DecisionAlg(name)
{
    defineParameter("InputWidth", 3);
    defineParameter("NumResultBits", 1);
    defineParameter("MaxTob", 0);
    defineParameter("MinET",1);
    defineParameter("EtaMin",  0);
    defineParameter("EtaMax",  5);
    defineParameter("PhiMin",  0);
    defineParameter("PhiMax", 63);
    setNumberOutputBits(1);
}
//----------------------------------------------------------
TCS::EtaPhiWindow::~EtaPhiWindow()
{
}
//----------------------------------------------------------
TCS::StatusCode
TCS::EtaPhiWindow::initialize()
{
    parType_t inputWidth = parameter("InputWidth").value();
    p_MaxTob = parameter("MaxTob").value();
    if(p_MaxTob>inputWidth) {
        TRG_MSG_DEBUG("MaxTob ("<<p_MaxTob<<")"
                      <<" is larger than InputWidth ("<<inputWidth<<")"
                      <<" : restricting to InputWidth");
        p_MaxTob = inputWidth;
    }
    p_MinET = parameter("MinET").value();
    p_EtaMin = parameter("EtaMin").value();
    p_EtaMax = parameter("EtaMax").value();
    p_PhiMin = parameter("PhiMin").value();
    p_PhiMax = parameter("PhiMax").value();

    TRG_MSG_INFO("MaxTob : "<<p_MaxTob);
    TRG_MSG_INFO("MinET    : "<<p_MinET);
    TRG_MSG_INFO("EtaMin   : "<<(int)p_EtaMin);
    TRG_MSG_INFO("EtaMax   : "<<(int)p_EtaMax);
    TRG_MSG_INFO("PhiMin   : "<<p_PhiMin);
    TRG_MSG_INFO("PhiMax   : "<<p_PhiMax);
    TRG_MSG_INFO("number of output bits : "<<numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hEtaPhiWindow_accept_bit"+std::to_string((int)i);
       std::string hname_reject = "hEtaPhiWindow_reject_bit"+std::to_string((int)i);
       // deta vs dphi
       bookHist(m_histAccept, hname_accept, "ETA vs PHI", 100, p_EtaMin, p_EtaMax, 100, p_PhiMin, p_PhiMax);
       bookHist(m_histReject, hname_reject, "ETA vs PHI", 100, p_EtaMin, p_EtaMax, 100, p_PhiMin, p_PhiMax);
   }
   
    return StatusCode::SUCCESS;
}
//----------------------------------------------------------
TCS::StatusCode
TCS::EtaPhiWindow::processBitCorrect(const std::vector<TCS::TOBArray const *> &input,
                                     const std::vector<TCS::TOBArray *> &output,
                                     Decision &decision)

{
    if(input.size() == 1) {
        TRG_MSG_DEBUG("input size     : "<<input[0]->size());
        const unsigned int nLeading = p_MaxTob;
        bool accept{false};
        for(TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end();
            ++tob1) {
            const std::ptrdiff_t iTob = distance( input[0]->begin(), tob1);
            if(nLeading!=0 and iTob>=nLeading) continue;
                if( parType_t((*tob1)->Et()) <= p_MinET ) continue;
                if( (int)parType_t((*tob1)->eta()) <  (int)p_EtaMin ) continue;
                if( (int)parType_t((*tob1)->eta()) >= (int)p_EtaMax ) continue;
                if( (int)parType_t((*tob1)->phi()) <  (int)p_PhiMin ) continue;
                if( (int)parType_t((*tob1)->phi()) >= (int)p_PhiMax ) continue;
                accept = true;
		const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(0) : accept);
		const bool fillReject = fillHistos() and not fillAccept;
		const bool alreadyFilled = decision.bit(0);
                output[0]->push_back(TCS::CompositeTOB(*tob1));
		if(fillAccept and not alreadyFilled) {
		  fillHist2D(m_histAccept[0],(float)(*tob1)->eta(),(float)(*tob1)->phi());
		} else if(fillReject) {
		  fillHist2D(m_histReject[0],(float)(*tob1)->eta(),(float)(*tob1)->phi());
		}
                TRG_MSG_DEBUG("TOB "<<iTob
                              <<" ET = "<<(*tob1)->Et()
                              <<" Eta = "<<(*tob1)->eta()
                              <<" phi = "<<(*tob1)->phi());
        }
        decision.setBit( 0, accept );
    } else {
      TCS_EXCEPTION("EtaPhiWindow alg must have  1 input, but got "<<input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
//----------------------------------------------------------
TCS::StatusCode
TCS::EtaPhiWindow::process(const std::vector<TCS::TOBArray const *> &input,
                           const std::vector<TCS::TOBArray *> &output,
                           Decision &decision)
{
    if(input.size() == 1) {
        TRG_MSG_DEBUG("input size     : "<<input[0]->size());
        const unsigned int nLeading = p_MaxTob;
        bool accept{false};
        for(TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end();
            ++tob1) {
            const std::ptrdiff_t iTob = distance( input[0]->begin(), tob1);
            if(nLeading!=0 and iTob>=nLeading) continue;
            if( parType_t((*tob1)->Et()) <= p_MinET ) continue;
            if( (int)parType_t((*tob1)->eta()) <  (int)p_EtaMin ) continue;
            if( (int)parType_t((*tob1)->eta()) >= (int)p_EtaMax ) continue;
            if( (int)parType_t((*tob1)->phi()) <  (int)p_PhiMin ) continue;
            if( (int)parType_t((*tob1)->phi()) >= (int)p_PhiMax ) continue;
            accept = true;
	    const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(0) : accept);
	    const bool fillReject = fillHistos() and not fillAccept;
	    const bool alreadyFilled = decision.bit(0);
            output[0]->push_back(TCS::CompositeTOB(*tob1));
	    if(fillAccept and not alreadyFilled) {
	      fillHist2D(m_histAccept[0],(float)(*tob1)->eta(),(float)(*tob1)->phi());
	    } else if(fillReject) {
	      fillHist2D(m_histReject[0],(float)(*tob1)->eta(),(float)(*tob1)->phi());
	    }
             TRG_MSG_DEBUG("TOB "<<iTob
                          <<" ET = "<<(*tob1)->Et()
                          <<" Eta = "<<(*tob1)->eta()
                          <<" phi = "<<(*tob1)->phi());
        }
        decision.setBit( 0, accept );
    } else {
        TCS_EXCEPTION("EtaPhiWindow alg must have  1 input, but got "<<input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
//----------------------------------------------------------
