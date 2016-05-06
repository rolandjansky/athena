/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LEGENDREWEIGHTS_H_
#define LEGENDREWEIGHTS_H_

double legendreWeights[21] = {

	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

	// Weights for the 5th order Legendre polynomial

	0.236926885056189, 0.478628670499366, 0.568888888888889, 0.478628670499366,
	0.236926885056189,

	// Weights for the 6th order Legendre polynomial

	0.17132449237917, 0.360761573048139, 0.467913934572691, 0.467913934572691,
	0.360761573048139, 0.17132449237917

};


double legendreRoots[21] = {

	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,

	// Roots of the 5th order Legendre polynomial

	-0.906179845938664, -0.538469310105683, 0.0, 0.538469310105683, 0.906179845938664,

	// Roots of the 6th order Legendre polynomial

	-0.932469514203152, -0.661209386466265, -0.238619186083197, 0.238619186083197,
	0.661209386466265, 0.932469514203152

};

#endif /* LEGENDREWEIGHTS_H_ */
