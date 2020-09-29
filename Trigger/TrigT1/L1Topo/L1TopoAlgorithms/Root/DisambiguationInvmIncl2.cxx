/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DisambiguationInvmIncl2.cpp
 * Created by Davide Gerbaudo on 2016-10-10.
 *
 * @brief compute the INVMASS**2 between distinct objects from two lists and applies invmass criteria
 *
 * The 'distinct object' criterion is based on the disambiguation
 * as implemented in DisambiguationDRIncl2, where deltaR is > 0.
 *
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DisambiguationInvmIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"

REGISTER_ALG_TCS(DisambiguationInvmIncl2)


// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << "TCS::DisambiguationInvmIncl2:     "

namespace {
unsigned int
calcInvMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
    double deta = fabs( tob1->etaDouble() - tob2->etaDouble() );
    double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
    if(dphi>M_PI)
        dphi = 2*M_PI - dphi;
    double cosheta = cosh (deta);
    double cosphi = cos (dphi);
    double invmass2 = 2*tob1->Et()*tob2->Et()*(cosheta - cosphi);
    return round( invmass2 );
}

unsigned int
calcInvMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
    auto bit_cosheta = TSU::L1TopoDataTypes<19,7>(TSU::Hyperbolic::Cosh.at(abs(tob1->eta() - tob2->eta())));
    auto bit_cosphi = TSU::L1TopoDataTypes<9,7>(TSU::Trigo::Cos.at(abs(tob1->phi() - tob2->phi())));
    TSU::L1TopoDataTypes<11,0> bit_Et1(tob1->Et());
    TSU::L1TopoDataTypes<11,0> bit_Et2(tob2->Et());
    auto bit_invmass2 = 2*bit_Et1*bit_Et2*(bit_cosheta - bit_cosphi);
    return int(bit_invmass2) ;
}

unsigned int
calcDeltaR2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
    double deta = ( tob1->etaDouble() - tob2->etaDouble() );
    double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
    if(dphi>M_PI)
        dphi = 2*M_PI - dphi;
    return round ( 100 * ((dphi)*(dphi) + (deta)*(deta) )) ;
}

unsigned int
calcDeltaR2BW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
    int detaB = abs( tob1->eta() - tob2->eta() );
    int dphiB = abs( tob1->phi() - tob2->phi() );
    if(dphiB>32)
        dphiB = 64 - dphiB;
    unsigned int bit_dr2 = dphiB*dphiB + detaB*detaB;
    return bit_dr2;
}
} // namespace

TCS::DisambiguationInvmIncl2::DisambiguationInvmIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0);
   defineParameter("MaxTob2", 0);
   defineParameter("NumResultBits", 2);
   defineParameter("MinET1",  1, 0);
   defineParameter("MinET2",  1, 0);
   defineParameter("MinMSqr", 0, 0);
   defineParameter("MaxMSqr", 0, 0);
   defineParameter("MinET1",  1, 1);
   defineParameter("MinET2",  1, 1);
   defineParameter("MinMSqr", 0, 1);
   defineParameter("MaxMSqr", 0, 1);

   setNumberOutputBits(2);
}

TCS::DisambiguationInvmIncl2::~DisambiguationInvmIncl2(){}

TCS::StatusCode
TCS::DisambiguationInvmIncl2::initialize() {
    p_NumberLeading1 = parameter("InputWidth1").value();
    p_NumberLeading2 = parameter("InputWidth2").value();
    if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
    if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

    for(unsigned int i=0; i<numberOutputBits(); ++i) {
        p_MinET1[i] = parameter("MinET1",i).value();
        p_MinET2[i] = parameter("MinET2",i).value();
        p_InvMassMin[i] = parameter("MinMSqr", i).value();
        p_InvMassMax[i] = parameter("MaxMSqr", i).value();
        TRG_MSG_INFO("MinET1  : " << i << " : "<< p_MinET1[i]);
        TRG_MSG_INFO("MinET2  : " << i << " : "<< p_MinET2[i]);
        TRG_MSG_INFO("MinMSqr : " << i << " : "<< p_InvMassMin[i]);
        TRG_MSG_INFO("MaxMSqr : " << i << " : "<< p_InvMassMin[i]);
    }
    TRG_MSG_INFO("number output : " << numberOutputBits());
    return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DisambiguationInvmIncl2::processBitCorrect(const std::vector<TCS::TOBArray const *> & input,
                                                const std::vector<TCS::TOBArray *> & output,
                                                Decision & decision)
{
    if(input.size() == 2) {
        for(TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
            ++tob1) {
            for(TCS::TOBArray::const_iterator tob2 = input[1]->begin();
                tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                ++tob2) {
                unsigned int invmass2 = calcInvMassBW( *tob1, *tob2 );
                unsigned int deltaR2 = calcDeltaR2BW( *tob1, *tob2 );
                for(unsigned int i=0; i<numberOutputBits(); ++i) {
                    bool accept = false;
                    if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                    if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                    accept = (deltaR2 > 0 &&
                              invmass2 >= p_InvMassMin[i] &&
                              invmass2 <= p_InvMassMax[i] );
                    if( accept ) {
                        decision.setBit(i, true);
                        output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                    }
                    TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail")
                                  << " deltaR = " << deltaR2
                                  << " invmass2 = " << invmass2);
                } // for(i)
            } // for(tob2)
        } // for(tob1)
    } else {
        TCS_EXCEPTION("DisambiguationInvmIncl2 alg must have  2 inputs, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DisambiguationInvmIncl2::process(const std::vector<TCS::TOBArray const *> & input,
                                      const std::vector<TCS::TOBArray *> & output,
                                      Decision & decision)
{
   if( input.size() == 2) {
       for( TOBArray::const_iterator tob1 = input[0]->begin();
            tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
            ++tob1) {
           for(TCS::TOBArray::const_iterator tob2 = input[1]->begin();
               tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
               ++tob2) {
               unsigned int invmass2 = calcInvMass( *tob1, *tob2 );
               unsigned int deltaR2 = calcDeltaR2( *tob1, *tob2 );
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   bool accept = false;
                   if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue;
                   if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue;
                   accept = (deltaR2 > 0 &&
                             invmass2 >= p_InvMassMin[i] &&
                             invmass2 <= p_InvMassMax[i] );
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR = " << deltaR2 );
               } // for(i)
           } // for(tob2)
       } // for(tob1)
   } else {
       TCS_EXCEPTION("DisambiguationInvmIncl2 alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
