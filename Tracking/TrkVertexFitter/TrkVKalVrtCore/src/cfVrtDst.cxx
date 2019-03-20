/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include "TrkVKalVrtCore/Propagator.h"
#include <iostream>

namespace Trk {

extern vkalPropagator  myPropagator;
extern vkalMagFld      myMagFld;

// Function calculates distance between summary track after fit and vertex for constraint
// Flag UseTrkErr tells if SummaryTrack errors+VertexErrors are used or only VertexErrors
//   Algorithm:
//       V0 is first propagated to (0,0,0) point and then is propagated back to reference
//       point. Error matrix is slightly different from one obtained with direct propagation
//       as in cfVrtDstSig
//---------------------------------------------------------------------------------------
//
// Function calculates a significance of summary track - constraint vertex 3D distance
//   As V0 origin vk->cnstV[] is used!!!
//
//  V0 covariance matrix is not calculated and taken from vk->fitCovXYZMom saved on previous fit
//     Algorithm:
//         V0 track is obtained at fitted point and propagated to reference point
//--------------------------------------------------------------------------------------------------
double cfVrtDstSig( VKVertex * vk, bool UseTrkErr)
{
    long int it,IERR; int i,j,ij;
    double parV0[5], covParV0[15];
    double Signif;

    extern void combinedTrack(long int ICH, double *pv0, double *covi, double BMAG, double *paro, double *covo);
    extern std::array<double, 4> getCnstParticleMom( VKTrack * , VKVertex *);
    extern int cfdinv(double *, double *, long int); 
 /* ------------------------------------------------------------------- */

    double ptot[3]= {0.,0.,0.};
    int NTRK = vk->TrackList.size();
    for ( it=0; it<NTRK; it++) {
        std::array<double, 4>  pp=getCnstParticleMom( vk->TrackList[it], vk );
	ptot[0] += pp[0];
	ptot[1] += pp[1];
	ptot[2] += pp[2];
    }
    double fittedVrt[3]={vk->refIterV[0] + vk->cnstV[0],
                         vk->refIterV[1] + vk->cnstV[1],
			 vk->refIterV[2] + vk->cnstV[2]};
    long int Charge = 0; for ( it=0; it<NTRK; it++) Charge += vk->TrackList[it]->Charge; 
//std::cout<<" cfVrtDst ntrk="<<NTRK<<" chg="<<Charge<<'\n';
    double localField=myMagFld.getMagFld(fittedVrt,(vk->vk_fitterControl).get());
    if ( UseTrkErr){
      combinedTrack( Charge, ptot, vk->fitCovXYZMom, localField, parV0, covParV0);
    }else{
      double DummyErr[21] = { 1.e-20,
                                  0., 1.e-20,
				  0.,     0., 1.e-20,
				  0.,     0.,     0., 1.e-18,
				  0.,     0.,     0.,     0., 1.e-18,
				  0.,     0.,     0.,     0.,     0., 1.e-18};
      combinedTrack( Charge, ptot, DummyErr, localField, parV0, covParV0);
    }
//
// Propagation to constraint vertex
//
    double nPar[5],nCov[15];
    long int TrkID = -999;   // Abstract track propagation
    myPropagator.Propagate( TrkID, Charge, parV0, covParV0, fittedVrt, vk->FVC.vrt, nPar, nCov, (vk->vk_fitterControl).get());
//
//
    double cnv[2][3];
    cnv[0][0] = -sin(nPar[3]);
    cnv[0][1] =  cos(nPar[3]);
    cnv[0][2] =  0.;
    cnv[1][0] =  sin(nPar[3])/tan(nPar[2]);
    cnv[1][1] =  cos(nPar[3])/tan(nPar[2]);
    cnv[1][2] =  -1.;
//--
    double covImp[3]={0.,0.,0.};
    for( i=0; i<3; i++){
       for( j=0; j<3; j++){
	   if(i>=j){ ij=i*(i+1)/2+j; }else{ ij=j*(j+1)/2+i;}
           covImp[0] += cnv[0][i]*vk->FVC.covvrt[ij]*cnv[0][j];
           covImp[1] += cnv[0][i]*vk->FVC.covvrt[ij]*cnv[1][j];
           covImp[2] += cnv[1][i]*vk->FVC.covvrt[ij]*cnv[1][j];
       }
    }
    if ( UseTrkErr){ covImp[0] += nCov[0]; covImp[1] += nCov[1]; covImp[2] += nCov[2];}

    double dwgt[3];
    IERR=cfdinv(covImp, dwgt, -2); if(IERR){ IERR=cfdinv(covImp, dwgt, 2); if(IERR){dwgt[0]=dwgt[2]=1.e6; dwgt[1]=0.; IERR=0; }}
    Signif = sqrt(dwgt[0] * nPar[0] * nPar[0] +  2. * dwgt[1] * nPar[0] * nPar[1] + 
                  dwgt[2] * nPar[1] * nPar[1]);
 
//std::cout<<" fittedcov="<<nCov[0]<<", "<<nCov[2]<<'\n';
//std::cout<<"  cfVrtDstSig="<<Signif<<", "<<nPar[0]<<", "<<nPar[1]<<'\n';
    return Signif;
} 

} /* End of namespace */

