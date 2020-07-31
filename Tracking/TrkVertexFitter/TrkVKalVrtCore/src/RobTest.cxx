/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace Trk {


//extern void digx(double*, double*, double*, long int , long int );
 
extern void vkGetEigVect(double ci[], double d[], double vect[], int n);

void robtest(VKVertex * vk, long int ifl)
{
    long int i, j, k, kk, it;
    double rob, res, c[5], darg, absX, roba[5];
 
/* ------------------------------------------------------------ */
/*  Robustification procedure for weight matrix */
/*  IROB  = 0  - Standard chi^2 */
/*        = 1  - Geman-McClure function */
/*        = 2  - Welch function */
/*        = 3  - Cauchy function */
/*        = 4  - L1-L2 function */
/*        = 5  - Fair function */
/*        = 6  - Huber function */
/*        = 7  - Modified Huber function */
/*   (1,2,3)  - Zero outliers */
/*   (4,5,6,7)  - Downweight outliers */
/* Author: V.Kostyukhin */
/* ------------------------------------------------------------ */

    int NTRK = vk->TrackList.size();

    long int irob = vk->vk_fitterControl->vk_forcft.irob;
    double    Scl = vk->vk_fitterControl->vk_forcft.RobustScale;  //Tuning constant
    double    C;                          // Breakdown constant

    if ( ifl == 0) {                               /* FILLING OF EIGENVALUES AND VECTORS */
	for (it = 0; it < NTRK ; ++it) {               /* RESTORE MATRIX */
            VKTrack *trk=vk->TrackList[it].get();
            if(trk->Id < 0) continue;  // Not a real track
	    //k = 0;   double dest[5][5];
	    //for (i = 0; i < 5; ++i) {
	    //for (j = 0; j <= i; ++j) {
	    //      dest[i][j] = trk->WgtM[k];
	    //      dest[j][i] = trk->WgtM[k];
	    //	  ++k;
	    //	}
	    //}
	    //digx(&dest[0][0], &(vk->TrackList[it]->e[0]), &(vk->TrackList[it]->v[0][0]), 5, 1);
            vkGetEigVect(trk->WgtM, trk->e ,&(trk->v[0][0]), 5);
	}
//std::cout.precision(9); std::cout<<"Rob="<<irob<<'\n';
//std::cout<<" Ini="<<vk->TrackList[0]->WgtM[0]<<", "<<vk->TrackList[0]->WgtM[1]<<", "<<vk->TrackList[0]->WgtM[2]
//            <<", "<<vk->TrackList[0]->WgtM[3]<<", "<<vk->TrackList[0]->WgtM[4]<<", "<<vk->TrackList[0]->WgtM[5]
//            <<", "<<vk->TrackList[0]->WgtM[6]<<", "<<vk->TrackList[0]->WgtM[7]<<", "<<vk->TrackList[0]->WgtM[8]<<'\n';
	return;
    }
/* -- */
    double    halfPi=M_PI/2.;
    for (it = 0; it < NTRK; ++it) {
        VKTrack *trk=vk->TrackList[it].get();
        if(trk->Id < 0) continue;  // Not a real track
        c[0]=c[1]=c[2]=c[3]=c[4]=0.;
        for( k = 0; k < 5; k++){
          c[0] +=   trk->rmnd[k] * trk->v[0][k];
          c[1] +=   trk->rmnd[k] * trk->v[1][k];
          c[2] +=   trk->rmnd[k] * trk->v[2][k];
          c[3] +=   trk->rmnd[k] * trk->v[3][k];
          c[4] +=   trk->rmnd[k] * trk->v[4][k];
        }
//std::cout<<"rm="<<trk->rmnd[0]<<", "<<trk->rmnd[1]<<", "<<trk->rmnd[2]<<
//            ", "<<trk->rmnd[3]<<", "<<trk->rmnd[4]<<", "<<'\n';
//std::cout<<" c="<<c[0]<<", "<<c[1]<<", "<<c[2]<<", "<<c[3]<<", "<<c[4]<<", "<<'\n';
	for (k = 0; k < 5; ++k) {
	  darg = c[k]*c[k]*trk->e[k];
	  if(darg < 1.e-10) darg = 1.e-10;
	  absX = sqrt(darg);                          /* Abs(X) == sqrt(X)*/
	  rob = 1.; C=1;
	  if(irob == 1)C = 1.58*Scl;                  /* Geman-McClure Standard =1 but c^2=2.5 is better(Gem.McC) */
	  if(irob == 2)C = 2.9846*Scl;                /* Welch def c^2=2.9846^2 */
	  if(irob == 3)C = 2.3849*Scl;                /* Cauchy def 2.385^2=5.7 */
	  if(irob == 4)C = 1.;                        /* L1-L2  no tuning*/
	  if(irob == 5)C = 1.3998*Scl;                /* Fair (def=1.3998)*/
	  if(irob == 6)C = 1.345 *Scl;                /* Huber (def=1.345)*/
	  if(irob == 7)C = 1.2107 *Scl;               /* Modified Huber (def=1.2107)*/
/* Functionals here are divided by (X^2/2)!!!*/
          double C2=C*C;
	  if(irob == 1)rob = 1. / (darg/C2 + 1.);                                  /* Geman-McClure */
	  if(irob == 2)rob = C2*(1. - exp(-darg/C2) )/darg;                        /* Welch */
	  if(irob == 3)rob = C2*log(darg/C2 + 1.)/darg;                            /* Cauchy  */
	  if(irob == 4)rob = 4.*(sqrt(darg / 2. + 1.) - 1.)/darg;                  /* L1-L2 */
	  if(irob == 5)rob = 2.*C2*(absX/C - log(absX/C + 1.))/darg;               /* Fair */
	  if(irob == 6)rob = C>absX ? 1. : (2*C/absX - C*C/darg) ;                 /* Huber */
	  if(irob == 7)rob = halfPi>(absX/C) ? 2*C*C*(1-cos(absX/C))/darg : 
                                               2*(C*absX+C*C*(1.-halfPi))/darg;    /* Modified Huber */
          roba[k] = rob;
          if(rob>0.99)roba[k] = 1.; //To improve precision
	}
//std::cout<<"robn="<<roba[0]<<", "<<roba[1]<<", "<<roba[2]<<", "<<roba[3]<<", "<<roba[4]<<", "<<irob<<'\n';
	for (i = 0; i < 5; ++i) if(roba[i]<1.e-3)roba[i]=1.e-3;
	kk = 0;
	for (i = 0; i < 5; ++i) {
	    for (j = 0; j <= i; ++j) {
		res = 0.;
		for (k = 0; k < 5; ++k) {
                    res += trk->v[k][i] * trk->e[k] * trk->v[k][j]*roba[k];
		}
		trk->WgtM[kk]=res;
		kk++;
	    }
	}
	vk->vk_fitterControl->vk_forcft.robres[it] = roba[0] * roba[1] * roba[2] * roba[3] * roba[4];
	if(vk->vk_fitterControl->vk_forcft.robres[it]>1.)vk->vk_fitterControl->vk_forcft.robres[it]=1.;
    }
//std::cout<<" Fin="<<vk->TrackList[0]->WgtM[0]<<", "<<vk->TrackList[0]->WgtM[1]<<", "<<vk->TrackList[0]->WgtM[2]
//            <<", "<<vk->TrackList[0]->WgtM[3]<<", "<<vk->TrackList[0]->WgtM[4]<<", "<<vk->TrackList[0]->WgtM[5]
//            <<", "<<vk->TrackList[0]->WgtM[6]<<", "<<vk->TrackList[0]->WgtM[7]<<", "<<vk->TrackList[0]->WgtM[8]<<'\n';
} 


} /* End of namespace */


