/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "L1TopoSimulationUtils/Kinematics.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoEvent/GenericTOB.h"

//

using namespace std; 

unsigned int TSU::Kinematics::calcDeltaPhiBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2){
      int dphiB = abs( tob1->phi() - tob2->phi() );
      if(dphiB>64)
         dphiB = dphiB - 64;
      if(dphiB>32)
         dphiB = 64 - dphiB;

      return dphiB ;
}

unsigned int TSU::Kinematics::calcDeltaEtaBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = abs( tob1->eta() - tob2->eta() );
	return deta;
}

unsigned int TSU::Kinematics::calcInvMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2){

     auto bit_cosheta = TSU::L1TopoDataTypes<19,7>(TSU::Hyperbolic::Cosh.at(abs(tob1->eta() - tob2->eta())));
      auto bit_cosphi = TSU::L1TopoDataTypes<9,7>(TSU::Trigo::Cos.at(abs(tob1->phi() - tob2->phi())));
      TSU::L1TopoDataTypes<11,0> bit_Et1(tob1->Et());
      TSU::L1TopoDataTypes<11,0> bit_Et2(tob2->Et());
      auto bit_invmass2 = bit_Et1*bit_Et2*(bit_cosheta - bit_cosphi)*2;
      return int(bit_invmass2) ;
}

unsigned int TSU::Kinematics::calcDeltaR2BW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      int detaB = abs( tob1->eta() - tob2->eta() );
      int dphiB = abs( tob1->phi() - tob2->phi() );
      if(dphiB>64) //Probably same error here as in DeltaPhiBW. Check
         dphiB = dphiB - 64;
      if(dphiB>32)
         dphiB = 64 - dphiB;

      unsigned int bit_dr2 = dphiB*dphiB + detaB*detaB;
      return bit_dr2;
}

unsigned long TSU::Kinematics::quadraticSumBW(int i1, int i2){
    unsigned long int iu1 = i1;
    unsigned long int iu2 = i2;
    unsigned long int a = iu1*iu1 + iu2*iu2; // the sum of two int*2 will fit in an unsigned long int

    unsigned long int result=0;
    unsigned long int left=0, right=0, r=0;
    unsigned long int sign=0;
  
    unsigned long int halflength = 16; //max 16
    unsigned long int bitmask = 0b11111111111111111111111111111111; //32 bits
    bitmask >>= (32 - halflength*2);
  
    for(unsigned long int i = 0; i < halflength; i++){ //16-->4
        right = 1 + (sign<<1) + (result<<2);
        left = (r<<2) + (a >> (2*halflength-2));
        a <<= 2;
        a = a & bitmask;
        r = sign ? (left + right) : (left - right);
        sign = ((r & (1 << (halflength+1) ))  > 0) ? 1 : 0;
        result <<= 1;
        if(sign==0) result += 1;
    }
    return result;
}

/*------------------------------------------ NON-BITWISE --------------------------------------------------*/

unsigned int TSU::Kinematics::calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;

      return round( 10 * dphi );
}

unsigned int TSU::Kinematics::calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = fabs( tob1->eta() - tob2->eta() );
	return deta;
}

unsigned int TSU::Kinematics::calcInvMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = fabs( tob1->etaDouble() - tob2->etaDouble() );
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;

      double cosheta = cosh ( deta);
      double cosphi = cos ( dphi);
      double invmass2 = 2*tob1->Et()*tob2->Et()*(cosheta - cosphi);
      return round( invmass2 );
}

unsigned int TSU::Kinematics::calcDeltaR2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = ( tob1->etaDouble() - tob2->etaDouble() );
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;


      return round ( 100 * ((dphi)*(dphi) + (deta)*(deta) )) ;
}

