/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Kinematics.h
// Pepijn Bakker, Oct 21


#ifndef TSU_Kinematics
#define TSU_Kinematics

//the file 'requirements' in cmt can get you this L1TopoEvent
#include "L1TopoEvent/GenericTOB.h"
#include <iostream>
#include <vector>

namespace TSU { 
   struct Kinematics {
	static unsigned int calcDeltaPhiBWLegacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);		
	static unsigned int calcDeltaEtaBWLegacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);

	static unsigned int calcInvMassBWLegacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcTMassBWLegacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaR2BWLegacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static float calcCosLegacy(unsigned phi);
	static float calcSinLegacy(unsigned phi);
	static unsigned int calcDeltaPhiBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);		
	static unsigned int calcDeltaEtaBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);

	static unsigned int calcInvMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcTMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaR2BW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcXi1(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcXi2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static float calcCos(unsigned phi);
	static float calcSin(unsigned phi);
     /**
        @brief compute the sum in quadrature of two ints

        This function includes an integer square root implementation using a bitwise iterative approach.
        Square root is computed bit-by-bit (highest significance bits first) and the result is
        adjusted up- or downwards in each iteration based on whether or not the current result
        under- or overestimates the value of the sum of squares.

        That is the same way in which the sqrt int is implemented in firmware:
            processor/sources/common/libraries/L1TopoFunctions.vhd
            --> function sqrt(d : UNSIGNED) return UNSIGNED
      */
	static unsigned long quadraticSumBW(int i1, int i2);

	static unsigned int calcDeltaPhiLegacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaEtaLegacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaR2Legacy(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);

	static unsigned int calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcInvMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcTMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaR2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
   };

} // end of namespace TSU

#endif
