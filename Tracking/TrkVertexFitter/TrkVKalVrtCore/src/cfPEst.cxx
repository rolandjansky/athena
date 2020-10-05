/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/Propagator.h"

namespace Trk {

extern const vkalPropagator  myPropagator;

void cfpest(int ntrk, double *xyz, long int *ich, double (*parst)[5], double (*parf)[3])
{
/* --------------------------------------------------- */
/*    Subroutine for primary estimation of Theta,Phi   */
/*     and 1/R at point XYZ if they aren't known       */
/* Author: V.Kostyukhin                                */
/* --------------------------------------------------- */

    double partmp[5];
    double Ref0[3]={0.,0.,0.};

    for (int i = 0; i < ntrk; ++i) {

        long int TrkID=i;
        myPropagator.Propagate( TrkID, ich[i], &parst[i][0], nullptr, Ref0, xyz, partmp, nullptr);

	parf[i][0] = partmp[2];
	parf[i][1] = partmp[3];
	parf[i][2] = partmp[4];
    }
}

} /* End of namespace */
