/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/Propagator.h"

namespace Trk {

extern vkalPropagator  myPropagator;

void cfpest(long int *ntrk, double *xyz, long int *ich, 
	double *parst, double *parf)
{
    long int i;
    double partmp[5];

/* --------------------------------------------------- */
/*    Subroutine for primary estimation of Theta,Phi   */
/*     and 1/R at point XYZ if they aren't known       */
/* Author: V.Kostyukhin                                */
/* --------------------------------------------------- */

    double Ref0[3]={0.,0.,0.};

    for (i = 0; i < (*ntrk); ++i) {

        myPropagator.Propagate(i, ich[i], &parst[i*5], 0, Ref0, xyz, partmp, 0);

	parf[i*3    ] = partmp[2];
	parf[i*3 + 1] = partmp[3];
	parf[i*3 + 2] = partmp[4];
    }
}

} /* End of namespace */
