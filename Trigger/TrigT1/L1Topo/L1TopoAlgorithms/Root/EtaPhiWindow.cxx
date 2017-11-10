/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "L1TopoAlgorithms/EtaPhiWindow.h"

#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

#include <cmath>
#include <iterator>

REGISTER_ALG_TCS(EtaPhiWindow)

using std::distance;

//----------------------------------------------------------
TCS::EtaPhiWindow::EtaPhiWindow(const std::string & name) : DecisionAlg(name)
{
    defineParameter("InputWidth", 3);
    defineParameter("NumResultBits", 1);
    defineParameter("NumberLeading", 0);
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
    p_NumberLeading = parameter("NumberLeading").value();
    if(p_NumberLeading>inputWidth) {
        TRG_MSG_DEBUG("NumberLeading ("<<p_NumberLeading<<")"
                      <<" is larger than InputWidth ("<<inputWidth<<")"
                      <<" : restricting to InputWidth");
        p_NumberLeading = inputWidth;
    }
    p_MinET = parameter("MinET").value();
    p_EtaMin = parameter("EtaMin").value();
    p_EtaMax = parameter("EtaMax").value();
    p_PhiMin = parameter("PhiMin").value();
    p_PhiMax = parameter("PhiMax").value();

    TRG_MSG_INFO("NumberLeading : "<<p_NumberLeading);
    TRG_MSG_INFO("MinET    : "<<p_MinET);
    TRG_MSG_INFO("EtaMin   : "<<p_EtaMin);
    TRG_MSG_INFO("EtaMax   : "<<p_EtaMax);
    TRG_MSG_INFO("PhiMin   : "<<p_PhiMin);
    TRG_MSG_INFO("PhiMax   : "<<p_PhiMax);
    TRG_MSG_INFO("number of output bits : "<<numberOutputBits());

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
        const unsigned int nLeading = p_NumberLeading;
        bool accept{false};
        for(TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end();
            ++tob1) {
            const std::ptrdiff_t iTob = distance( input[0]->begin(), tob1);
            if(nLeading!=0 and iTob>=nLeading) continue;
                if( parType_t((*tob1)->Et()) <= p_MinET ) continue;
                if( parType_t((*tob1)->eta()) <= p_EtaMin ) continue;
                if( parType_t((*tob1)->eta()) >= p_EtaMax ) continue;
                if( parType_t(abs((*tob1)->phi())) <= p_PhiMin ) continue;
                if( parType_t(abs((*tob1)->phi())) >= p_PhiMax ) continue;
                accept = true;
                output[0]->push_back(TCS::CompositeTOB(*tob1));

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
        const unsigned int nLeading = p_NumberLeading;
        bool accept{false};
        for(TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end();
            ++tob1) {
            const std::ptrdiff_t iTob = distance( input[0]->begin(), tob1);
            if(nLeading!=0 and iTob>=nLeading) continue;
            if( parType_t((*tob1)->Et()) <= p_MinET ) continue;
            if( parType_t((*tob1)->eta()) <= p_EtaMin ) continue;
            if( parType_t((*tob1)->eta()) >= p_EtaMax ) continue;
            if( parType_t(abs((*tob1)->phi())) <= p_PhiMin ) continue;
            if( parType_t(abs((*tob1)->phi())) >= p_PhiMax ) continue;
            accept = true;
            output[0]->push_back(TCS::CompositeTOB(*tob1));
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
