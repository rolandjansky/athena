/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
	static unsigned int calcDeltaPhiBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);		
	static unsigned int calcDeltaEtaBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);

	static unsigned int calcInvMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaR2BW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned long quadraticSumBW(int i1, int i2);

	static unsigned int calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcInvMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
	static unsigned int calcDeltaR2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2);
   };

} // end of namespace TSU

#endif
