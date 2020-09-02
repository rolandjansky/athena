/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include "TrkVKalVrtCore/Derivt.h"

namespace Trk {

/* --------------------------------------------------- */
/* ADER is a full matrix of chi2 problem               */
/* It inversion gives the same solution as usual       */
/* Author: V.Kostyukhin                                */
/* --------------------------------------------------- */

#define ader_ref(a_1,a_2) ader[(a_2)*(vkalNTrkM*3+3) + (a_1)]

void FullMTXfill(VKVertex * vk, double * ader)
{
    int i,j,it;

    int NTRK = vk->TrackList.size();
    int NPar=3*NTRK+3;

    for (i=0; i<NPar; i++) { for (j=0; j<NPar; j++) { ader_ref(i, j)=0.;} }

    ader_ref(0, 0) += vk->wa[0];
    ader_ref(0, 1) += vk->wa[1];
    ader_ref(1, 1) += vk->wa[2];
    ader_ref(0, 2) += vk->wa[3];
    ader_ref(1, 2) += vk->wa[4];
    ader_ref(2, 2) += vk->wa[5];
    ader_ref(1, 0)  = ader_ref(0, 1);
    ader_ref(2, 0)  = ader_ref(0, 2);
    ader_ref(2, 1)  = ader_ref(1, 2);


    for (it=0; it<NTRK; ++it) {
	ader_ref(0, it*3 + 3) += vk->tmpArr[it]->wb[0];
	ader_ref(1, it*3 + 3) += vk->tmpArr[it]->wb[1];
	ader_ref(2, it*3 + 3) += vk->tmpArr[it]->wb[2];
	ader_ref(0, it*3 + 4) += vk->tmpArr[it]->wb[3];
	ader_ref(1, it*3 + 4) += vk->tmpArr[it]->wb[4];
	ader_ref(2, it*3 + 4) += vk->tmpArr[it]->wb[5];
	ader_ref(0, it*3 + 5) += vk->tmpArr[it]->wb[6];
	ader_ref(1, it*3 + 5) += vk->tmpArr[it]->wb[7];
	ader_ref(2, it*3 + 5) += vk->tmpArr[it]->wb[8];
    }

    for (it=0; it<NTRK; ++it) {
	ader_ref(it*3 + 3, it*3 + 3) += vk->tmpArr[it]->wc[0];
	ader_ref(it*3 + 3, it*3 + 4) += vk->tmpArr[it]->wc[1];
	ader_ref(it*3 + 4, it*3 + 4) += vk->tmpArr[it]->wc[2];
	ader_ref(it*3 + 3, it*3 + 5) += vk->tmpArr[it]->wc[3];
	ader_ref(it*3 + 4, it*3 + 5) += vk->tmpArr[it]->wc[4];
	ader_ref(it*3 + 5, it*3 + 5) += vk->tmpArr[it]->wc[5];
	ader_ref(it*3 + 4, it*3 + 3) += vk->tmpArr[it]->wc[1];
	ader_ref(it*3 + 5, it*3 + 3) += vk->tmpArr[it]->wc[3];
	ader_ref(it*3 + 5, it*3 + 4) += vk->tmpArr[it]->wc[4];
    }
/* symmetrisation */
    for (i=0; i<NPar-1; i++) {
	for (j=i+1; j<NPar; ++j) {
	    ader_ref(j, i) = ader_ref(i, j);
	}
    }
//std::cout<<" FULLMTX NEW="<<ader_ref(0, 0)<<", "<<ader_ref(1, 1)<<", "<<ader_ref(2, 2)<<", "
//                          <<ader_ref(3, 3)<<", "<<ader_ref(4, 4)<<", "<<ader_ref(5, 5)<<", "
//                          <<ader_ref(0, 3)<<", "<<ader_ref(1, 4)<<", "<<ader_ref(3, 5)<<'\n'; 
} 

#undef ader_ref

//---------------------------------------------------------------------------------------------
//  Fill full matrix of vertex with constraints problem
//  It's used for cascade fit
//
// Total number of parameters  Vertex(3)+NTRK*3+N_constr

int FullMCNSTfill(VKVertex * vk, double * ader, double * LSide)
{
    int i,j,k,l,ii,ic,it,jt;

    int totNC=0;  //total number of constraints
    for(i=0; i<(int)vk->ConstraintList.size();i++)totNC += vk->ConstraintList[i]->NCDim;
 
    int NTRK = vk->TrackList.size();
    int NPar = 3*NTRK+3+totNC;
//    int NPar=3*NTrkM+3;

    for (i=0; i<NPar; i++) { for (j=0; j<NPar; j++) ader[i+j*NPar]=0.; }
    std::vector<std::vector< Vect3DF> > tf0t;  // derivative collectors
    std::vector< Vect3DF >              th0t;  // derivative collectors
    std::vector< double >               taa;   // derivative collectors
    std::vector< Vect3DF > tmpVec;
    for(ii=0; ii<(int)vk->ConstraintList.size();ii++){
       for(ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
         taa.push_back(  vk->ConstraintList[ii]->aa[ic] );
         th0t.push_back( vk->ConstraintList[ii]->h0t[ic] );
         tmpVec.clear();
         for(it=0; it<(int)vk->ConstraintList[ii]->f0t.size(); it++){
	    tmpVec.push_back( vk->ConstraintList[ii]->f0t[it][ic] );
         }
	 tf0t.push_back( tmpVec );
       }
    }
//-----------------------------------------------------------------------------
    ader[0*NPar+ 0] += vk->wa[0];
    ader[0*NPar+ 1] += vk->wa[1];
    ader[1*NPar+ 1] += vk->wa[2];
    ader[0*NPar+ 2] += vk->wa[3];
    ader[1*NPar+ 2] += vk->wa[4];
    ader[2*NPar+ 2] += vk->wa[5];
    ader[1*NPar+ 0]  = ader[0*NPar+ 1];
    ader[2*NPar+ 0]  = ader[0*NPar+ 2];
    ader[2*NPar+ 1]  = ader[1*NPar+ 2];


    for (it=0; it<NTRK; ++it) {
	ader[0 + (it*3 + 3)*NPar] += vk->tmpArr[it]->wb[0];
	ader[1 + (it*3 + 3)*NPar] += vk->tmpArr[it]->wb[1];
	ader[2 + (it*3 + 3)*NPar] += vk->tmpArr[it]->wb[2];
	ader[0 + (it*3 + 4)*NPar] += vk->tmpArr[it]->wb[3];
	ader[1 + (it*3 + 4)*NPar] += vk->tmpArr[it]->wb[4];
	ader[2 + (it*3 + 4)*NPar] += vk->tmpArr[it]->wb[5];
	ader[0 + (it*3 + 5)*NPar] += vk->tmpArr[it]->wb[6];
	ader[1 + (it*3 + 5)*NPar] += vk->tmpArr[it]->wb[7];
	ader[2 + (it*3 + 5)*NPar] += vk->tmpArr[it]->wb[8];
    }

    for (it=0; it<NTRK; ++it) {
	ader[(it*3 + 3) + (it*3 + 3)*NPar] += vk->tmpArr[it]->wc[0];
	ader[(it*3 + 3) + (it*3 + 4)*NPar] += vk->tmpArr[it]->wc[1];
	ader[(it*3 + 4) + (it*3 + 4)*NPar] += vk->tmpArr[it]->wc[2];
	ader[(it*3 + 3) + (it*3 + 5)*NPar] += vk->tmpArr[it]->wc[3];
	ader[(it*3 + 4) + (it*3 + 5)*NPar] += vk->tmpArr[it]->wc[4];
	ader[(it*3 + 5) + (it*3 + 5)*NPar] += vk->tmpArr[it]->wc[5];
	ader[(it*3 + 4) + (it*3 + 3)*NPar] += vk->tmpArr[it]->wc[1];
	ader[(it*3 + 5) + (it*3 + 3)*NPar] += vk->tmpArr[it]->wc[3];
	ader[(it*3 + 5) + (it*3 + 4)*NPar] += vk->tmpArr[it]->wc[4];
    }
/* symmetrisation */
    for (i=0; i<NPar-1; i++) {
	for (j=i+1; j<NPar; ++j) {
	    ader[j + i*NPar] = ader[i + j*NPar];
	}
    }
//
//  For "passing near" constraint
    if (vk->passNearVertex){ 
      double drdpy[2][3],dpipj[3][3];
      for (it = 0; it < NTRK; ++it) {
	drdpy[0][0] = vk->tmpArr[it]->drdp[0][0] * vk->FVC.ywgt[0] + vk->tmpArr[it]->drdp[1][0] * vk->FVC.ywgt[1];
        drdpy[1][0] = vk->tmpArr[it]->drdp[0][0] * vk->FVC.ywgt[1] + vk->tmpArr[it]->drdp[1][0] * vk->FVC.ywgt[2];
	drdpy[0][1] = vk->tmpArr[it]->drdp[0][1] * vk->FVC.ywgt[0] + vk->tmpArr[it]->drdp[1][1] * vk->FVC.ywgt[1];
	drdpy[1][1] = vk->tmpArr[it]->drdp[0][1] * vk->FVC.ywgt[1] + vk->tmpArr[it]->drdp[1][1] * vk->FVC.ywgt[2];
	drdpy[0][2] = vk->tmpArr[it]->drdp[0][2] * vk->FVC.ywgt[0] + vk->tmpArr[it]->drdp[1][2] * vk->FVC.ywgt[1];
	drdpy[1][2] = vk->tmpArr[it]->drdp[0][2] * vk->FVC.ywgt[1] + vk->tmpArr[it]->drdp[1][2] * vk->FVC.ywgt[2];
	for (jt = 0; jt < NTRK; ++jt) {   /* Matrix */
		    for (k=0; k<3; ++k) {
			for (l=0; l<3; ++l) {
			    dpipj[k][l] = 0.;
			    for (j=0; j<2; ++j) {
				dpipj[k][l] +=  vk->tmpArr[jt]->drdp[j][k] * drdpy[j][l];
			    }
			}
		    }
		    for (k=0; k<3; ++k) {
			for (l=0; l<3; ++l) {
			    ader[(it*3+3 +k) +  (jt*3+3 +l)*NPar] += dpipj[l][k];
			}
		    }
	} // jt cycle
      }   // it cycle
    }
//
//   Part for lagrange multipliers
//
    int NTrP=NTRK*3 + 3; // track part of matrix
    for(ic=0; ic<totNC; ic++){
       ader[(0)      + (NTrP+ic)*NPar] = -2.*th0t[ic].X;
       ader[(1)      + (NTrP+ic)*NPar] = -2.*th0t[ic].Y;
       ader[(2)      + (NTrP+ic)*NPar] = -2.*th0t[ic].Z;
       ader[(0)*NPar + (NTrP+ic)     ] = -2.*th0t[ic].X;
       ader[(1)*NPar + (NTrP+ic)     ] = -2.*th0t[ic].Y;
       ader[(2)*NPar + (NTrP+ic)     ] = -2.*th0t[ic].Z;
       for (it=0; it<NTRK; ++it) {
         ader[(it*3+3+0)      + (NTrP+ic)*NPar] = - 2.*tf0t[ic][it].X;
         ader[(it*3+3+1)      + (NTrP+ic)*NPar] = - 2.*tf0t[ic][it].Y;
         ader[(it*3+3+2)      + (NTrP+ic)*NPar] = - 2.*tf0t[ic][it].Z;
         ader[(it*3+3+0)*NPar + (NTrP+ic)]      = - 2.*tf0t[ic][it].X;
         ader[(it*3+3+1)*NPar + (NTrP+ic)]      = - 2.*tf0t[ic][it].Y;
         ader[(it*3+3+2)*NPar + (NTrP+ic)]      = - 2.*tf0t[ic][it].Z;
       }
    }
//
//  Left part
//
//    double *LSide = new double[NPar];
    LSide[0]=vk->T[0]; 
    LSide[1]=vk->T[1]; 
    LSide[2]=vk->T[2]; 
    for (it=0; it<NTRK; ++it) {
       LSide[it*3+3+0]=vk->tmpArr[it]->tt[0];
       LSide[it*3+3+1]=vk->tmpArr[it]->tt[1];
       LSide[it*3+3+2]=vk->tmpArr[it]->tt[2];
    }
    for(ic=0; ic<totNC; ic++){
       LSide[NTRK*3+3+ic]=taa[ic];
    }
//--------------------------------------------------
    //extern int vkMSolve(double *a, double *b, long int n, double* =0);
    //int IERR = vkMSolve(ader, LSide, NPar);
    //std::cout<<"global="<<NPar<<" err="<<IERR<<'\n';
    //std::cout<<" Vrt="<<LSide[0]<<", "<<LSide[1]<<", "<<LSide[2]<<'\n';
    //for(it=0; it<NTRK; it++)std::cout<<" trk="<<LSide[it*3+3+0]<<", "
    //                      <<LSide[it*3+3+1]<<", "<<LSide[it*3+3+2]<<'\n';
    return NPar;
} 


}  /* End of namespace */

