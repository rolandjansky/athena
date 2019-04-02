/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Idea:
//  Use "pass near" constraint in each cascade vertex with thermodynamical "cooling"
//  Potentially could be more powerful that currently implemented scheme with exact pointing
//  Currently is LESS efficient, so for the moment left for future work
//==========================================================================================
#include <math.h>
#include <iostream>
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include "TrkVKalVrtCore/Derivt.h"


namespace Trk {

extern vkalPropagator  myPropagator;
extern vkalMagFld      myMagFld;

extern int cfdinv(double *, double *, long int );
extern int cfInv5(double *cov, double *wgt );
extern int  vtcfit( VKVertex * vk);
extern void  combinedTrack(long int ICH, double *pv0, double *covi, double BMAG, double *paro, double *covo);
extern int afterFit(VKVertex *, double *, double *, double *, double *, const VKalVrtControlBase* = 0);
extern void applyConstraints(VKVertex * vk);
extern void FullMTXfill(VKVertex * , double * );
extern  int FullMCNSTfill(VKVertex * , double * , double * );

extern int getCascadeNPar(CascadeEvent &, int Type=0);
extern VKTrack * getCombinedVTrack(VKVertex *);
extern void vpderiv(bool, long int , double *, double *, double *, double *, double *, double *, double *,const VKalVrtControl * =0);
extern std::vector<double> transformCovar(int , double **, const std::vector<double>& );
extern double cfVrtDstSig( VKVertex * , bool );
extern long int getVertexCharge( VKVertex *);
extern      int setVTrackMass(VKVertex * );
//
//  Rescale covariance matrices for cascade vertices used as pointing targets
//
void rescaleVrtErrForPointing( double Div, CascadeEvent & cascadeEvent_ )
{
   if(Div<1.)Div=1.;
   cascadeEvent_.setSCALE(cascadeEvent_.getSCALE()/Div);
   for(int iv=0; iv<cascadeEvent_.cascadeNV-1; iv++){        // Last vertex must not be touched
     VKVertex *vk = cascadeEvent_.cascadeVertexList[iv];
     for(int i=0; i<6; i++) vk->FVC.covvrt[i] /=Div;
   }
}
//
//
//
int fitVertexCascadeScale( VKVertex * vk, double & distToVertex )
{     
//-------------------------------------------------------------------------
//  Fits single vertex in cascade (no iterations , just a single fit!!!) 
//  creates summary track after it and fills corresponding track parameters 
//   in next vertex in cascade 
//  
//   "Near to vertex" pointing is used here with decreasing vertex errors
//
//   Procedure assumes that "included tracks"(from vertices) are already 
//  recalculated in predecessor vertices fits.
//
//  Fit itself. First get magnetic field at iteration reference point
//
   distToVertex = 0.;
   vk->vk_fitterControl->vk_forcft.localbmag=myMagFld.getMagFld(vk->refIterV,(vk->vk_fitterControl).get());


//
//-------- Then fit
//
   applyConstraints(vk);                                         //apply all constraints in vertex 
   int IERR = vtcfit( vk );
   if(IERR) return IERR;
//
//
//    Fill fitted combined track in next vertex (if needed)
//
   double dptot[4],VrtMomCov[21];      
   IERR = afterFit( vk, vk->ader, vk->FVC.dcv, dptot, VrtMomCov, (vk->vk_fitterControl).get());
   if (IERR) return -13;                        /* NONINVERTIBLE COV.MATRIX */
   cfdcopy(    dptot, vk->fitMom, 3);          //save Momentum
   cfdcopy(VrtMomCov, vk->fitCovXYZMom, 21);   //save XYZMom covariance
   vk->FVC.Charge=getVertexCharge(vk);     
//
   if(vk->nextCascadeVrt){
      FullMTXfill(vk, vk->ader);
      VKTrack * target_trk = getCombinedVTrack(vk);  // get address of combined track
      if( target_trk == 0 ) return -12;

      long int Charge=getVertexCharge(vk);
      target_trk->Charge=Charge;

      double parV0[5],covParV0[15],tmpCov[15],fittedVrt[3];      
      fittedVrt[0]=vk->refIterV[0]+vk->fitV[0];
      fittedVrt[1]=vk->refIterV[1]+vk->fitV[1];
      fittedVrt[2]=vk->refIterV[2]+vk->fitV[2];
//
//  Particle creation and propagation      
      double localField=myMagFld.getMagFld(fittedVrt,(vk->vk_fitterControl).get());
      combinedTrack( Charge, dptot, VrtMomCov, localField, parV0, covParV0);
      covParV0[0]=fabs(covParV0[0]); covParV0[2]=fabs(covParV0[2]); covParV0[5]=fabs(covParV0[5]);
      covParV0[9]=fabs(covParV0[9]); covParV0[14]=fabs(covParV0[14]);  //VK protection against numerical problems
      myPropagator.Propagate(-999, Charge, parV0, covParV0, fittedVrt, 
                vk->nextCascadeVrt->refIterV, target_trk->Perig, tmpCov, (vk->vk_fitterControl).get());
//std::cout<<"testR,Z="<<target_trk->Perig[0]<<", "<<target_trk->Perig[1]<<'\n';
      distToVertex = sqrt(target_trk->Perig[0]*target_trk->Perig[0]+target_trk->Perig[1]*target_trk->Perig[1]);
      cfdcopy(target_trk->Perig,target_trk->Perig,5);
      target_trk->iniP[0]=target_trk->cnstP[0]=target_trk->Perig[2];   //initial guess
      target_trk->iniP[1]=target_trk->cnstP[1]=target_trk->Perig[3];
      target_trk->iniP[2]=target_trk->cnstP[2]=target_trk->Perig[4];
      IERR=cfInv5(tmpCov, target_trk->WgtM);  if (IERR) IERR=cfdinv(tmpCov, target_trk->WgtM, 5);
      if (IERR) return -15;                        /* NONINVERTIBLE COV.MATRIX */
      for(int im=0; im<15; im++)target_trk->WgtM[im] /= vk->vk_fitterControl->getCascadeEvent()->getSCALE();   // Tighten pseudotrack errors
   }
//
// Save refitted vertex position as target for predecessors
   if(vk->includedVrt.size()){  // Save fitted vertex as target for "pass near" constraint in previous vertex
      for(int pseu=0; pseu<(int)vk->includedVrt.size(); pseu++){
//         vk->includedVrt[pseu]->FVC.vrt[0] = (vk->refIterV[0] + vk->fitV[0] + vk->includedVrt[pseu]->FVC.vrt[0])/2.;
//         vk->includedVrt[pseu]->FVC.vrt[1] = (vk->refIterV[1] + vk->fitV[1] + vk->includedVrt[pseu]->FVC.vrt[1])/2.;
//         vk->includedVrt[pseu]->FVC.vrt[2] = (vk->refIterV[2] + vk->fitV[2] + vk->includedVrt[pseu]->FVC.vrt[2])/2.;
         vk->includedVrt[pseu]->FVC.vrt[0] = vk->refIterV[0] + vk->fitV[0];
         vk->includedVrt[pseu]->FVC.vrt[1] = vk->refIterV[1] + vk->fitV[1];
         vk->includedVrt[pseu]->FVC.vrt[2] = vk->refIterV[2] + vk->fitV[2];
      }
   }
   return 0;
}

//-----------------------------------------------------------------------------------
//
// Iteration over complete cascade  
//
 int processCascadeScale(CascadeEvent & cascadeEvent_ )
{ 
  extern int translateToFittedPos(CascadeEvent &, double Step=1.);

  VKVertex * vk=0;
  long int Iter, IERR, iv;
  int countTrk=0;  // Number of tracks in cascade
  for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     countTrk += cascadeEvent_.cascadeVertexList[iv]->TrackList.size();
  }
  cascadeEvent_.setSCALE(1.);                    // Safety
//============================================================================================
//
// First without pointing to get initial estimations and resolve mass constraints
// This initial step is needed to get initial estimation for "passing near" constraints 
//
  double iv_dstToVrt=0., sum_dstToVrt=0., old_dstToVrt; 
  for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     vk = cascadeEvent_.cascadeVertexList[iv];
     vk->passNearVertex=false;
     vk->passWithTrkCov=false;
     IERR = fitVertexCascadeScale( vk, iv_dstToVrt );    if(IERR)return IERR;   //fit 
     IERR = setVTrackMass(vk);                           if(IERR)return IERR;   //mass of combined particle
  }
  IERR = translateToFittedPos(cascadeEvent_);          if(IERR)return IERR;

//
//  Now fit "close to vertex" pointing and cooling
//
  for(iv=0; iv<cascadeEvent_.cascadeNV-1; iv++) cascadeEvent_.cascadeVertexList[iv]->passNearVertex=true;
  if(cascadeEvent_.nearPrmVertex){  // Primary vertex if needed
     cascadeEvent_.cascadeVertexList[cascadeEvent_.cascadeNV-1]->passNearVertex=true;
     cascadeEvent_.cascadeVertexList[cascadeEvent_.cascadeNV-1]->passWithTrkCov=true;
  }     
//
  double Chi2Old=0.,Chi2Cur=0; 
  double Scale=2.;
  double limDstToVrt=0.001;
  old_dstToVrt=100000.;
  for(Iter=0; Iter<20; Iter++){

    //*******************************************************************************
    for(int Stabil=0; Stabil<2; Stabil++){  // Stabilization cycling for given SCALE
      Chi2Cur=0.;
      sum_dstToVrt=0.;
      for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
        vk = cascadeEvent_.cascadeVertexList[iv];
//
//Calculate derivatives for "passing near" cnst. Initial vertex position is used for derivatives.
        if(vk->passNearVertex){   
          double dparst[6]={vk->refIterV[0]+vk->iniV[0], vk->refIterV[1]+vk->iniV[1], vk->refIterV[2]+vk->iniV[2],
	                    vk->fitMom[0], vk->fitMom[1], vk->fitMom[2] };
	  vpderiv(vk->passWithTrkCov, vk->FVC.Charge, dparst, vk->fitCovXYZMom, vk->FVC.vrt, vk->FVC.covvrt, 
	                                             vk->FVC.cvder, vk->FVC.ywgt, vk->FVC.rv0, (vk->vk_fitterControl).get());
        }
        IERR = fitVertexCascadeScale( vk, iv_dstToVrt);    if(IERR)return IERR;   //fit 
        IERR = setVTrackMass(vk);                          if(IERR)return IERR;   //mass of combined particle
        Chi2Cur += vk->Chi2;
        sum_dstToVrt += iv_dstToVrt;
      }
      IERR = translateToFittedPos(cascadeEvent_,1.0);   if(IERR)return IERR;
      //if(Stabil>0 && sum_dstToVrt<old_dstToVrt) break;
    }
    //********************************************************************************
    if(Iter>5 && Chi2Cur>1.e4) return -1;      //Too bad iteration. Drop fit
    if(cascadeEvent_.getSCALE() < 1.e-6)Scale=1.;   //Enough tightening
    if(sum_dstToVrt<old_dstToVrt){
       rescaleVrtErrForPointing( Scale, cascadeEvent_ );    // Tighten pointing accuracy 
       if(old_dstToVrt/3. < sum_dstToVrt) { old_dstToVrt=sum_dstToVrt;} else {old_dstToVrt/=3.;}
    }
//std::cout<<"Cool iter="<<Iter<<" Scale="<<cascadeEvent_.getSCALE()<<", "<<Chi2Cur<<" NV="<<cascadeEvent_.cascadeNV<<
//           " dst="<<sum_dstToVrt<<'\n';
    if(fabs(Chi2Cur-Chi2Old)<0.01 && Iter>5 && sum_dstToVrt<limDstToVrt*(cascadeEvent_.cascadeNV-1) )break;       //stable cascade position
    Chi2Old=Chi2Cur;
  }
  double cnstRemnants=0. ;
  for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     vk = cascadeEvent_.cascadeVertexList[iv];
     for(int ii=0; ii<(int)vk->ConstraintList.size();ii++){
        for(int ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
           cnstRemnants +=  fabs( vk->ConstraintList[ii]->aa[ic] ); 
     } }
  }
  if( cnstRemnants > 1.e0) return -2;                                        /* Constraints are not resolved. Stop fit */
std::cout<<"================================================="<<sum_dstToVrt<<'\n';
//  if(sum_dstToVrt>limDstToVrt*(cascadeEvent_.cascadeNV-1) )return -2;        //Pointing is not resolved

  long int fullNPar = getCascadeNPar(cascadeEvent_);
  cascadeEvent_.fullCovMatrix = new double[fullNPar*fullNPar]; 
  for(int im=0; im<fullNPar*fullNPar; im++)cascadeEvent_.fullCovMatrix[im]=0.;
  for(int im=0; im<fullNPar; im++)cascadeEvent_.fullCovMatrix[im*fullNPar + im]=1.;
  int NStart=0;
  cascadeEvent_.matrixPnt.resize(cascadeEvent_.cascadeNV);
  double * tmpLSide   = new double[fullNPar];
  for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
    cascadeEvent_.matrixPnt[iv]=NStart;
    NStart  += FullMCNSTfill( vk, vk->ader, tmpLSide);
  }
  delete[] tmpLSide; 

  return 0;
}


} /* End of VKalVrtCore namespace */
