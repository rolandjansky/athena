/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iostream>
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/ForCFT.h"
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"


namespace Trk {

extern vkalPropagator  myPropagator;
extern vkalMagFld      myMagFld;

extern int cfdinv(double *, double *, long int );
extern int cfInv5(double *cov, double *wgt );
extern double cfSmallEigenvalue( double*, long int );
extern int  vtcfit( VKVertex * vk);
extern void  combinedTrack(long int ICH, double *pv0, double *covi, double BMAG, double *paro, double *covo);
extern int afterFit(VKVertex *, double *, double *, double *, double *, const VKalVrtControlBase * =0 );
extern int afterFitWithIniPar(VKVertex *vk, double *, double *, double *, double *, const VKalVrtControlBase * =0 );
extern void applyConstraints(VKVertex * vk);
extern void FullMTXfill(VKVertex * , double * );
extern  int FullMCNSTfill(VKVertex * , double * , double * );
extern  int vkMSolve(double *, double*, long int, double* =0);

extern void copyFullMtx(double *Input, long int IPar, long int IDIM,
                 double *Target, long int TStart, long int TDIM);
extern void addCrossVertexDeriv(CascadeEvent &, double * , long int , const std::vector<int> & );
extern void setFittedParameters(double * , std::vector<int> &, CascadeEvent &);
extern int getCascadeNPar(CascadeEvent &, int Type=0);
extern VKTrack * getCombinedVTrack(VKVertex *);
extern void vpderiv(bool, long int , double *, double *, double *, double *, double *, double *, double *, const VKalVrtControl * =0);
extern std::array<double, 4> getFitParticleMom( VKTrack *, double);

extern void setFittedMatrices(double * , long int , std::vector<int> &, std::vector< std::vector<double> > &, CascadeEvent& );
extern std::vector<double> transformCovar(int , double **, const std::vector<double>& );
extern double cfVrtDstSig( VKVertex * , bool );
extern void robtest(VKVertex * , long int );

extern int fixPseudoTrackPt(long int NPar, double * fullMtx, double * LSide, CascadeEvent&);
extern void getNewCov(double *OldCov, double* Der, double* Cov, long int DIM);

//--------------------------------------------------------------------
//
//  Set up masses of combined particles
//
int setVTrackMass(VKVertex * vk)
{
   double vBx,vBy,vBz;
   int it, NTRK;
   VectMOM totP;

   if(!vk->nextCascadeVrt)return 0;               // nonpointing vertex

   myMagFld.getMagFld(vk->refIterV[0]+vk->fitV[0], vk->refIterV[1]+vk->fitV[1], vk->refIterV[2]+vk->fitV[2],
                                        vBx,vBy,vBz,(vk->vk_fitterControl).get());

   NTRK = vk->TrackList.size();                   // Number of tracks at vertex
   totP.Px=0.;totP.Py=0.;totP.Pz=0.;totP.E=0.;
   for(it=0; it<NTRK; it++){
       std::array<double, 4> pp = getFitParticleMom( vk->TrackList[it], vBz );
       totP.Px += pp[0];     
       totP.Py += pp[1];     
       totP.Pz += pp[2];     
       totP.E  += pp[3];     
   }
   VKTrack * target_trk = getCombinedVTrack(vk);
   if( target_trk == 0 ) {
       std::cout<<" ERROR in CASCADE STRUCTURE"<<'\n';
       return -1;
   }else{
       double Pt=sqrt(totP.Px*totP.Px + totP.Py*totP.Py);
       double Mass;
       if(Pt > fabs(totP.Pz)){
         Mass = sqrt( (totP.E-Pt)*(totP.E+Pt) - totP.Pz*totP.Pz);
       }else{
         Mass = sqrt( (totP.E-totP.Pz)*(totP.E+totP.Pz) - Pt*Pt);
       }
       target_trk->setMass(Mass);
   }
   return 0;
}

long int getVertexCharge( VKVertex * vk){
  long int tCharge=0;
  for(int it=0; it<(int)vk->TrackList.size(); it++) tCharge += vk->TrackList[it]->Charge;     
  return tCharge;
}

double cascadeCnstRemnants(CascadeEvent & cascadeEvent_){
  double cnstRemnants=0.;
  for(int iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     VKVertex * vk = cascadeEvent_.cascadeVertexList[iv];
     for(int ii=0; ii<(int)vk->ConstraintList.size();ii++){
        for(int ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
           cnstRemnants +=  fabs( vk->ConstraintList[ii]->aa[ic] ); 
     } }
  }
  return cnstRemnants;
}

int fitVertexCascade( VKVertex * vk, int Pointing)
{     
//-------------------------------------------------------------------------
//  Fits single vertex in cascade (no iterations , just a single fit!!!) 
//  creates summary track after it and fills corresponding track parameters 
//   in next vertex in cascade 
//  
//   Procedure assumes that "included tracks"(from vertices) are already 
//  recalculated in predecessor vertices fits.
//    
//   Pointing=0 (false) - for combined track FITTED at vertex parameters are used
//                                           ^^^^^^ better convergence
//   Pointing=1 (true)  - for combined track GUESSED(ini) at GUESSED(ini) vertex
//                        parameters are used together with afterfit error matrix.
//                        It's needed for correct treatment of pointing
//-------------------------------------------------------------------------
//
//  Fit itself. First get magnetic field at iteration reference point
//
   vk->vk_fitterControl->vk_forcft.localbmag=myMagFld.getMagFld(vk->refIterV,(vk->vk_fitterControl).get());


//
//-------- Check if pointing constraint exists
//
  bool existPointingCnst=false;
  for(int ic=0; ic<(int)vk->ConstraintList.size(); ic++){
    VKPointConstraint * p_cnst=dynamic_cast<VKPointConstraint *>(vk->ConstraintList[ic]);
    if(p_cnst) existPointingCnst=true;
  }
//
//-------- Then fit
//
   applyConstraints(vk);                                         //apply all constraints in vertex 
   int IERR = vtcfit( vk );
   if(IERR) return IERR;
//
//fit vertex once more with resolved constraints to prevent oscillations
//   if(Pointing){ 
//      double targVrt[3]={vk->refIterV[0] + vk->fitV[0],vk->refIterV[1] + vk->fitV[1],vk->refIterV[2] + vk->fitV[2]};
//      vk->vk_fitterControl->vk_forcft.localbmag=myMagFld.getMagFld(vk->targVrt,(vk->vk_fitterControl).get());
//      vk->setRefIterV(targVrt);
//      vk->iniV[0]=vk->fitV[0]=vk->cnstV[0]=vk->iniV[1]=vk->fitV[1]=vk->cnstV[1]=vk->iniV[2]=vk->fitV[2]=vk->cnstV[2]=0.;
//      for(int it=0; it<(int)vk->TrackList.size(); it++){
//         VKTrack * trk = vk->TrackList[it];
//         trk->cnstP[0] = trk->iniP[0] = trk->fitP[0];   // for constraint calculation on next step
//         trk->cnstP[1] = trk->iniP[1] = trk->fitP[1];
//         trk->cnstP[2] = trk->iniP[2] = trk->fitP[2];
//      }      
//      applyConstraints(vk);                                         //apply all constraints in vertex 
//      int IERR = vtcfit( vk );
//      if(IERR) return IERR;
//   }
//
//    Fill fitted combined track in next vertex (if needed)
//
   if(vk->nextCascadeVrt){
      FullMTXfill(vk, vk->ader);
      VKTrack * target_trk = getCombinedVTrack(vk);  // get address of combined track
      if( target_trk == 0 ) return -12;

      long int Charge=getVertexCharge(vk);
      target_trk->Charge=Charge;

      double dptot[4],VrtMomCov[21];      
      double parV0[5],covParV0[15],tmpCov[15],fittedVrt[3];      
      if(Pointing){
         IERR = afterFitWithIniPar( vk, vk->ader, vk->FVC.dcv, dptot, VrtMomCov, (vk->vk_fitterControl).get());
         fittedVrt[0]=vk->refIterV[0]+vk->iniV[0];
         fittedVrt[1]=vk->refIterV[1]+vk->iniV[1];
         fittedVrt[2]=vk->refIterV[2]+vk->iniV[2];
      }
      else{
         IERR = afterFit( vk, vk->ader, vk->FVC.dcv, dptot, VrtMomCov, (vk->vk_fitterControl).get());
         fittedVrt[0]=vk->refIterV[0]+vk->fitV[0];
         fittedVrt[1]=vk->refIterV[1]+vk->fitV[1];
         fittedVrt[2]=vk->refIterV[2]+vk->fitV[2];
      }
      if (IERR) return -13;                        /* NONINVERTIBLE COV.MATRIX */
      cfdcopy(    dptot, vk->fitMom, 3);          //save Momentum
      cfdcopy(VrtMomCov, vk->fitCovXYZMom, 21);   //save XYZMom covariance
//
//-- Solution (not ideal!) of cascade covariance problem. 
//-- Covariance of pseudo-particle must be calculated WITHOUT pointing constraint ALWAYS!.
// 
//      if(existPointingCnst){   // use saved matrix without pointing constraint
//        cfdcopy( vk->savedVrtMomCov, VrtMomCov, 21);
//      }else{                   // save computed matrix for further use
//        cfdcopy( VrtMomCov, vk->savedVrtMomCov, 21);
//      }
      if(existPointingCnst){   // add pointed vertex covariance to track
        for(int km=0; km<6; km++) VrtMomCov[km] += vk->nextCascadeVrt->fitCovXYZMom[km];
        VrtMomCov[0]  *= 1000.;
        VrtMomCov[2]  *= 1000.;
        VrtMomCov[5]  *= 1000.;
        VrtMomCov[9]  *= 1000.;
        VrtMomCov[14] *= 1000.;
        VrtMomCov[20] *= 1000.;
      }
//
//  Particle creation and propagation      
      double localField=myMagFld.getMagFld(fittedVrt,(vk->vk_fitterControl).get());
      combinedTrack( Charge, dptot, VrtMomCov, localField, parV0, covParV0);
      covParV0[0]=fabs(covParV0[0]); covParV0[2]=fabs(covParV0[2]); covParV0[5]=fabs(covParV0[5]);
      covParV0[9]=fabs(covParV0[9]); covParV0[14]=fabs(covParV0[14]);  //VK protection against numerical problems
      myPropagator.Propagate(-999, Charge, parV0, covParV0, fittedVrt, 
                vk->nextCascadeVrt->refIterV, target_trk->Perig, tmpCov, (vk->vk_fitterControl).get());
//    IERR=cfInv5(tmpCov, target_trk->WgtM);  if (IERR) IERR=cfdinv(tmpCov, target_trk->WgtM, 5);
//      target_trk->iniP[0]=target_trk->cnstP[0]=target_trk->fitP[0]=target_trk->Perig[2];   //initial guess
//      target_trk->iniP[1]=target_trk->cnstP[1]=target_trk->fitP[1]=target_trk->Perig[3];
//      target_trk->iniP[2]=target_trk->cnstP[2]=target_trk->fitP[2]=target_trk->Perig[4];
      if( target_trk->fitP[2] == 0.){     
        target_trk->iniP[0]=target_trk->cnstP[0]=target_trk->Perig[2];   //initial guess
        target_trk->iniP[1]=target_trk->cnstP[1]=target_trk->Perig[3];
        target_trk->iniP[2]=target_trk->cnstP[2]=target_trk->Perig[4];
      }else if(Pointing){
        target_trk->iniP[0]=target_trk->cnstP[0]=target_trk->fitP[0];   //initial guess
        target_trk->iniP[1]=target_trk->cnstP[1]=target_trk->fitP[1];
        target_trk->iniP[2]=target_trk->cnstP[2]=target_trk->fitP[2];
      }else{
        target_trk->iniP[0]=target_trk->cnstP[0]=(target_trk->fitP[0]+target_trk->Perig[2])/2.;   //initial guess
        target_trk->iniP[1]=target_trk->cnstP[1]=(target_trk->fitP[1]+target_trk->Perig[3])/2.;
        target_trk->iniP[2]=target_trk->cnstP[2]=(target_trk->fitP[2]+target_trk->Perig[4])/2.;
      }
      if(Pointing){tmpCov[0] += target_trk->Perig[0]*target_trk->Perig[0]; tmpCov[2] += target_trk->Perig[1]*target_trk->Perig[1];}
      tmpCov[0] += 0.0001*0.0001; tmpCov[2] += 0.0002*0.0002;  //numerical accuracy protection

      IERR=cfInv5(tmpCov, target_trk->WgtM);  if (IERR) IERR=cfdinv(tmpCov, target_trk->WgtM, 5);
      if (IERR) return -15;                        /* NONINVERTIBLE COV.MATRIX */
   }
//
// For passing near vertex constraint. Normally first(mother) vertex of cascade
//   Now used also for "close to vertex" mode for cascade
//   For last in structure (mother in cascade...) always calculate it - needed for pointing
//
   CascadeEvent & cascadeEvent_ = *(vk->vk_fitterControl->getCascadeEvent());
   if( vk->passNearVertex || vk==cascadeEvent_.cascadeVertexList[cascadeEvent_.cascadeNV-1]){
      double dptot[4],VrtMomCov[21];      
      IERR = afterFit( vk, vk->ader, vk->FVC.dcv, dptot, VrtMomCov, (vk->vk_fitterControl).get());
      if (IERR) return -13;                        /* NONINVERTIBLE COV.MATRIX */
      cfdcopy(    dptot, vk->fitMom, 3);          //save Momentum
      cfdcopy(VrtMomCov, vk->fitCovXYZMom, 21);   //save XYZMom covariance
   }
   return 0;
}
//==============================
// int processCascadeScale( );
// int processCascade( ) { int IERR=processCascadeScale(); std::cout<<IERR<<'\n'; return IERR;}
//-----------------------------------------------------------------------------------
//
// Iteration over complete cascade  
//
 int processCascade(CascadeEvent & cascadeEvent_ )
{ 
  int translateToFittedPos(CascadeEvent &, double Step=1.);
  int restorePreviousPos(CascadeEvent &,  std::vector<VKVertex> & );

  VKVertex * vk;
  long int Iter, IERR, iv, i;
  int countTrk=0;  // Number of tracks in cascade
  for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     countTrk += cascadeEvent_.cascadeVertexList[iv]->TrackList.size();
  }
//============================================================================================
//
// First without pointing to get initial estimations and resolve mass constraints
// This initial step is needed to get initial estimation for "passing near constraint 
//
  for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     vk = cascadeEvent_.cascadeVertexList[iv];
     IERR = fitVertexCascade( vk, 0);     if(IERR)return IERR;   //fit 
     IERR = setVTrackMass(vk);            if(IERR)return IERR;   //mass of combined particle
  }
  IERR = translateToFittedPos(cascadeEvent_);          if(IERR)return IERR;
//
//  Now standard fit without pointing in cascade but WITH pointing to PV if present
//

  double Chi2Old=0.,Chi2Cur=0.; 
  for(Iter=0; Iter<100; Iter++){
     Chi2Cur=0.;
     for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
       vk = cascadeEvent_.cascadeVertexList[iv];
//
//Calculate derivatives for "passing near" cnst. Initial vertex position is used for derivatives.
//Results are saved in ForVRTClose structure
       if(cascadeEvent_.nearPrmVertex && iv==(cascadeEvent_.cascadeNV-1)){   //only last vertex in cascade may have it
          double dparst[6]={vk->refIterV[0]+vk->iniV[0], vk->refIterV[1]+vk->iniV[1], vk->refIterV[2]+vk->iniV[2],
	                    vk->fitMom[0], vk->fitMom[1], vk->fitMom[2] };
          vk->FVC.Charge=getVertexCharge(vk);     
	  vpderiv(true, vk->FVC.Charge, dparst, vk->fitCovXYZMom, vk->FVC.vrt, vk->FVC.covvrt, 
	                      vk->FVC.cvder, vk->FVC.ywgt, vk->FVC.rv0, (vk->vk_fitterControl).get());
       }
       IERR = fitVertexCascade( vk, 0);     if(IERR)return IERR;   //fit 
       IERR = setVTrackMass(vk);            if(IERR)return IERR;   //mass of combined particle
       Chi2Cur += vk->Chi2;
//std::cout<<iv<<"---Chi2="<<vk->Chi2<<", "<<vk->refIterV[0]+vk->fitV[0]<<", "<<vk->refIterV[1]+vk->fitV[1]<<", "<<vk->refIterV[2]+vk->fitV[2]<<'\n';
     }
     if( Chi2Cur-Chi2Old > 1. && Iter){ IERR = translateToFittedPos(cascadeEvent_,0.8);}   //step limitation is case of divergence
     else                             { IERR = translateToFittedPos(cascadeEvent_,1.0);}   if(IERR)return IERR;
 
//std::cout<<" free iter="<<Iter<<", "<<Chi2Cur<<", "<<Chi2Old-Chi2Cur<<"; "<<cascadeEvent_.cascadeNV<<'\n';
     if(fabs(Chi2Cur-Chi2Old)<0.01)break;       //stable cascade position
     if(Iter>10 && (Chi2Cur-Chi2Old)>0.)break;  //oscillations. Stop preliminary fit
     Chi2Old=Chi2Cur;
     if(Chi2Cur>1.e6 && Iter>0) return -1;   //Too bad iteration. Drop fit
  }
//  if(Chi2Cur>3000.) return -1;   // Initial solution is too bad!!!
//-------------------------------- Check constraints status
  double cnstRemnants=cascadeCnstRemnants(cascadeEvent_);
  if( cnstRemnants > 10000.e0)           return -2;   /* Constraints are not resolved. Stop fit */
  if(cnstRemnants==0. && Chi2Cur>3000. ) return -1;   // Initial solution is too bad!!!
  if(                    Chi2Cur>50000.) return -1;   // Initial solution is too bad!!!
//-------------------------------------------------------------------------
//  Attach pointing constraint to affected vertices and create working arrays
//     of correct size
//
  for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     vk = cascadeEvent_.cascadeVertexList[iv];
     if(vk->nextCascadeVrt){
       long int NTRK = vk->TrackList.size();  
       vk->ConstraintList.push_back(new VKPointConstraint( NTRK, vk->nextCascadeVrt->refIterV , vk, false));
     }
  }
  long int fullNPar = getCascadeNPar(cascadeEvent_);
  double * fullMatrix   = new double[fullNPar*fullNPar];
  double * iniCovMatrix = new double[fullNPar*fullNPar];for(int ss=0; ss<fullNPar*fullNPar; ss++) iniCovMatrix[ss]=0.;
  double * fullLSide  = new double[fullNPar];
  double * tmpLSide   = new double[fullNPar];
  //std::unique_ptr<double[]> tmpLSide    = std::unique_ptr<double[]>(new double[fullNPar])
  std::vector<VKVertex> listVCopy(cascadeEvent_.cascadeNV); 
//-------------------------------------------------------
//  Now fit with pointing constraint
//
  cascadeEvent_.matrixPnt.resize(cascadeEvent_.cascadeNV);
  long int NParCur, NStart;
  int getBack=0;  // counter of oscillations
  int fullSTOP=0; // immediate stop iteration flag
  double Chi2Diff=0.;

  int NFitIterationMax=100;        // Maximal amount of iterations
  int badStepCountMax=5;           // Maximal amount of bad steps

  cnstRemnants=100000.; double cnstProgr=1.; double minCnstRemnants=100000.;
  int badStepCount=0;

  for(Iter=0; Iter<=NFitIterationMax; Iter++){
     Chi2Cur=0.;
     NStart=0;
     for( iv=0; iv<fullNPar*fullNPar; iv++)fullMatrix[iv]=0.;  // zero full matrix
     for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
       vk = cascadeEvent_.cascadeVertexList[iv];
//
//Calculate derivatives for "passing near" cnst. Initial vertex position is used for derivatives.
//Results are saved in ForVRTClose structure
       if(cascadeEvent_.nearPrmVertex && iv==(cascadeEvent_.cascadeNV-1)){   //only last vertex in cascade may have it
          double dparst[6]={vk->refIterV[0]+vk->iniV[0], vk->refIterV[1]+vk->iniV[1], vk->refIterV[2]+vk->iniV[2],
	                    vk->fitMom[0], vk->fitMom[1], vk->fitMom[2] };
          vk->FVC.Charge=getVertexCharge(vk);
	  vpderiv(vk->passWithTrkCov, vk->FVC.Charge, dparst, vk->fitCovXYZMom, 
	     vk->FVC.vrt, vk->FVC.covvrt, vk->FVC.cvder, vk->FVC.ywgt, vk->FVC.rv0, (vk->vk_fitterControl).get());
       }
       if (vk->vk_fitterControl->vk_forcft.irob != 0) {robtest(vk, 0);}  // ROBUSTIFICATION new data structure
       if (vk->vk_fitterControl->vk_forcft.irob != 0) {robtest(vk, 1);}  // ROBUSTIFICATION new data structure
       IERR = fitVertexCascade( vk, 1 );   if(IERR) break;              //with passNear for last vertex in cascade if needed
       IERR = setVTrackMass(vk);           if(IERR) break;               //mass of combined particle
//
//Get full constraint matrix and left side for vertex
       cascadeEvent_.matrixPnt[iv]=NStart;
       NParCur = FullMCNSTfill( vk, vk->ader, tmpLSide);
//
//Move them to cascade full matrix and left side
       copyFullMtx( vk->ader, NParCur, NParCur, fullMatrix, NStart, fullNPar); 
//
//Copy error matrix for left side vector (measured part T,U1,..,Un - see Billoir)
//  to correct place in iniCovMatrix matrix. Only at last step and without constraint part!!! CHECK IT!!!
       if(Iter==NFitIterationMax || fullSTOP || badStepCount>=badStepCountMax) 
       copyFullMtx( vk->ader, 3+3*vk->TrackList.size(), NParCur, iniCovMatrix, NStart, fullNPar); 
//
// Fill left part of the system
       for(i=0; i<NParCur; i++) fullLSide[i+NStart] = tmpLSide[i];
       NStart += NParCur;
       Chi2Cur += vk->Chi2;
//std::cout<<iv<<"--------Chi2="<<vk->Chi2<<", "<<cascadeEvent_.nearPrmVertex<<" nstart="<<NStart<<'\n';
     }
     if( (Chi2Cur>1.e8 && Iter>0) || IERR){
       delete[] fullMatrix; delete[] fullLSide; delete[] tmpLSide; delete[] iniCovMatrix;
       if(Chi2Cur>1.e8) return -1;
       return IERR;
     }
//
//--- Attempt to dump oscillations by returning to good position
     cnstProgr=cascadeCnstRemnants(cascadeEvent_)/cnstRemnants;
     if( Iter>3 && cnstProgr>4. && getBack<5 && fullSTOP==0 ) {  //------ Bad previous step discovered. 
       //std::cout<<" oscillation discovered="<<Iter<<" progr="<<cnstProgr<<'\n';
       IERR=restorePreviousPos(cascadeEvent_, listVCopy );
       if(IERR){ delete[] fullMatrix; delete[] fullLSide; delete[] tmpLSide; delete[] iniCovMatrix; return IERR;}
       cnstRemnants=100000.; Chi2Old+=10000;                // to skip restoring on next iteration
       if(Iter==NFitIterationMax  )Iter-=2;      // to give time to converge after this operation
       if(Iter==NFitIterationMax-1)Iter-=1;      // to give time to converge after this operation
       getBack++; badStepCount=0;
       continue;
     }                    //--------- Make some copy of vertices for safety
     cnstRemnants*=cnstProgr; cnstRemnants += 1.e-6*cascadeEvent_.getAccuracyConstraint(); //VK Protection against 0
     if(cnstRemnants<minCnstRemnants)minCnstRemnants=cnstRemnants;
     if(minCnstRemnants==cnstRemnants){        //--------- Make copy of vertices for safety at best point
       for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
         listVCopy.at(iv)=*(cascadeEvent_.cascadeVertexList[iv]);     // vertex list copy
       }
     }
//--------------------------------------------------------------------------
//for( iv=0; iv<fullNPar; iv++)std::cout<<fullLSide[iv]<<", ";std::cout<<'\n';
//    std::cout<<fullLSide[3+3*2]<<", "<<fullLSide[3+3*2+1]<<", "<<
//               fullLSide[20]<<", "<<fullLSide[21]<<", "<<fullLSide[22]<<'\n';
//---------------------------------------------------------------------------
//
//  Add cross vertices derivatives
     addCrossVertexDeriv(cascadeEvent_, fullMatrix, fullNPar, cascadeEvent_.matrixPnt);
//
//   Add pseudotrack momentum fix
//
     int tstRet=fixPseudoTrackPt(fullNPar, fullMatrix, fullLSide, cascadeEvent_ );
     if( tstRet ){ delete[] fullMatrix; delete[] fullLSide; delete[] tmpLSide; delete[] iniCovMatrix; return -1; }
//
//  Resolve full cascade with constraints system
//
     if( Iter<NFitIterationMax && fullSTOP==0 && badStepCount<badStepCountMax){    //Normal step. Solution of system only
       IERR = vkMSolve(fullMatrix, fullLSide, fullNPar);
       if(IERR){ delete[] fullMatrix; delete[] fullLSide; delete[] tmpLSide; delete[] iniCovMatrix;return IERR;}
     }else{                                               //Last step. Solution+full error matrix
       if(cascadeEvent_.fullCovMatrix)delete[] cascadeEvent_.fullCovMatrix;
       cascadeEvent_.fullCovMatrix = new double[fullNPar*fullNPar];     //Create fresh matrix
       IERR = vkMSolve(fullMatrix, fullLSide, fullNPar, cascadeEvent_.fullCovMatrix);
       if(IERR){ delete[] fullMatrix; delete[] fullLSide; delete[] tmpLSide; delete[] iniCovMatrix;
                 delete[] cascadeEvent_.fullCovMatrix; cascadeEvent_.fullCovMatrix=0; return IERR;}
//std::cout<<"fulcov="; for(int tt=0; tt<fullNPar; tt++)std::cout<<cascadeEvent_.fullCovMatrix[tt*fullNPar+tt]<<", "; std::cout<<'\n';
       double *  newCov = new double[fullNPar*fullNPar];  //Check via error transfer from left side (measured values). Gives exactly the same errors - correct!!! 
       getNewCov(iniCovMatrix, cascadeEvent_.fullCovMatrix, newCov, fullNPar);
       delete[] cascadeEvent_.fullCovMatrix; cascadeEvent_.fullCovMatrix=newCov;
     }
//
// Set fitted parameters
//
     setFittedParameters( fullLSide, cascadeEvent_.matrixPnt, cascadeEvent_ );
//
// Update pointing constraints and calculate real cascade Chi2 
//
     double Chi2Full=0.;
     for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
        vk = cascadeEvent_.cascadeVertexList[iv];
        for(int it=0; it<(int)vk->TrackList.size(); it++){
           VKTrack * trk=vk->TrackList[it]; if(trk->Id >= 0)Chi2Full+=trk->Chi2;    // real track in cascade
        }
        if(cascadeEvent_.nearPrmVertex && iv==(cascadeEvent_.cascadeNV-1)){   //only last vertex in cascade may have it
           double signift = cfVrtDstSig( vk, vk->passWithTrkCov);
           Chi2Full += signift*signift;
       }
     }
     Chi2Diff=Chi2Old-Chi2Full;
     if(Chi2Diff<0. && cnstProgr>0.99)  badStepCount++;
     if(cnstRemnants==minCnstRemnants || cnstProgr<0.9)  badStepCount=0;
//std::cout.precision(5); std::cout<<" iter="<<Iter<<" separ="<<Chi2Cur<<" full="<<Chi2Full<<" iter diff="<<Chi2Full/Chi2Cur<<
//" cnstrem="<<cnstRemnants<<" progr="<<cnstProgr<<" lim="<<cnstRemnants/minCnstRemnants<<" bstep="<<badStepCount<<'\n';
     bool badFullStep=false;
     if( Iter>5 && Chi2Full/Chi2Cur>1.2)          badFullStep=true;   //VK 16.04.2013 New strategy
     if( Iter==NFitIterationMax || fullSTOP )     badFullStep=false;  //VK 16.04.2013 No step limitation on last iteration
//
//  Prepare next iteration
//
     if(badFullStep) IERR = translateToFittedPos(cascadeEvent_,0.3);
     else            IERR = translateToFittedPos(cascadeEvent_);
     if(IERR){ delete[] fullMatrix; delete[] fullLSide; delete[] tmpLSide; delete[] iniCovMatrix; return IERR;}

     for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
        vk = cascadeEvent_.cascadeVertexList[iv];
        if(vk->nextCascadeVrt){
          int pntCnst = vk->ConstraintList.size() - 1;  // pointing constraint is always last in the list
          VKPointConstraint * pcnst = dynamic_cast<VKPointConstraint*>(vk->ConstraintList[pntCnst]);
          pcnst->setTargetVertex(vk->nextCascadeVrt->refIterV);
        }
     }
//

     if(fullSTOP) break;  
//     
     if( Iter<NFitIterationMax && Iter>2){
       if(fabs(Chi2Diff)<0.001 && cnstRemnants/minCnstRemnants<10.){   //stable cascade position before end of cycling
         NFitIterationMax=Iter+1;                           //then makes additional iteration to get the full error matrix
       } 
       if(cnstRemnants<cascadeEvent_.getAccuracyConstraint()) fullSTOP++;   //Good constraint accuracy is reached
     }
     if ( Iter>NFitIterationMax+50 ) break;     //Protection - too many iterations due to whatever reason.                   
     if(badStepCount>badStepCountMax)break;     //Divergence discovered (in both cases the full error matrix is ready!!!)

     Chi2Old=Chi2Full;
  }

  //if(tmpc0)std::cout<<(*tmpc0)<<'\n';
  //if(tmpc1)std::cout<<(*tmpc1)<<'\n';

  delete[] fullMatrix; delete[] fullLSide; delete[] tmpLSide; delete[] iniCovMatrix;
//-------------------------------- Check constraints status
  cnstRemnants=cascadeCnstRemnants(cascadeEvent_);
//std::cout<<"fullcnst="<<cnstRemnants<<" lim="<<cnstRemnants/minCnstRemnants<<'\n';
  if( cnstRemnants > 1.e0) return -2;           /* Constraints are not resolved. Stop fit */
  return 0;
}

//-----------------------------------------------------------------------------------
//
// Iteration over complete cascade  with first cascade vertex in Primary Vertex
//
 int processCascadePV(CascadeEvent & cascadeEvent_, double *primVrt, double *primVrtCov )
{
    double aermd[6],tmpd[6];  // temporary arrays
    VKVertex * vk = cascadeEvent_.cascadeVertexList[cascadeEvent_.cascadeNV-1]; //Main vertex

    vk->vk_fitterControl->vk_forcft.vrt[0] = primVrt[0];
    vk->vk_fitterControl->vk_forcft.vrt[1] = primVrt[1];
    vk->vk_fitterControl->vk_forcft.vrt[2] = primVrt[2];
    vk->vk_fitterControl->vk_forcft.covvrt[0] = primVrtCov[0];
    vk->vk_fitterControl->vk_forcft.covvrt[1] = primVrtCov[1];
    vk->vk_fitterControl->vk_forcft.covvrt[2] = primVrtCov[2];
    vk->vk_fitterControl->vk_forcft.covvrt[3] = primVrtCov[3];
    vk->vk_fitterControl->vk_forcft.covvrt[4] = primVrtCov[4];
    vk->vk_fitterControl->vk_forcft.covvrt[5] = primVrtCov[5];

    vk->useApriorVertex = 1;
    cfdcopy(vk->vk_fitterControl->vk_forcft.covvrt, tmpd,   6);
    int IERR=cfdinv(tmpd, aermd, -3); if (IERR) {  IERR = -4; return IERR; }
    cfdcopy(vk->vk_fitterControl->vk_forcft.vrt, vk->apriorV,   3);
    cfdcopy(      aermd, vk->apriorVWGT,6);

    return processCascade(cascadeEvent_);
}

//-----------------------------------------------------------------------------------
//
// Iteration over complete cascade  with "close to primary vertex" Chi2 term
//
 int processCascade(CascadeEvent & cascadeEvent_, double *primVrt, double *primVrtCov )
{ 
    cascadeEvent_.nearPrmVertex=1;                                             //setting up additional Chi2 term
    VKVertex * vk = cascadeEvent_.cascadeVertexList[cascadeEvent_.cascadeNV-1]; //Main vertex
    vk->passNearVertex  =true;                        // For fitting machinery
    vk->passWithTrkCov  =true;                        // For fitting machinery
    vk->FVC.vrt[0] = primVrt[0];
    vk->FVC.vrt[1] = primVrt[1];
    vk->FVC.vrt[2] = primVrt[2];
    vk->FVC.covvrt[0] = primVrtCov[0];
    vk->FVC.covvrt[1] = primVrtCov[1];
    vk->FVC.covvrt[2] = primVrtCov[2];
    vk->FVC.covvrt[3] = primVrtCov[3];
    vk->FVC.covvrt[4] = primVrtCov[4];
    vk->FVC.covvrt[5] = primVrtCov[5];
    return processCascade(cascadeEvent_);
}
//-----------------------------------------------------------------------------------
//
// Iteration over complete cascade  with exact pointing to primary vertex
//
 int processCascade(CascadeEvent & cascadeEvent_, double *primVrt )
{ 
    VKVertex * vk = cascadeEvent_.cascadeVertexList[cascadeEvent_.cascadeNV-1]; //Main vertex
    long int NTRK = vk->TrackList.size();  
    vk->ConstraintList.push_back(new VKPointConstraint( NTRK, primVrt, vk, false));
    cascadeEvent_.nearPrmVertex=0;                 //explicitly turn off additional Chi2 term
    return processCascade(cascadeEvent_);
}


//----------------------------------------------------------------------
// Translate all external (physics) tracks to fitted vertices positions
//   Step limitation is needed to break positive feedback
//
 int translateToFittedPos(CascadeEvent & cascadeEvent_, double Step )
{ 
  int iv,it;
  long int IERR,NTRK;
  double vShift, tmpPer[5], tmpWgt[15], tmpCov[15], targetVertex[3];
  VKTrack * trk; VKVertex * vk;
  if(Step>1.)Step=1.;
  
  for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     vk=cascadeEvent_.cascadeVertexList[iv];
     NTRK = vk->TrackList.size();            // Number of tracks at vertex
     targetVertex[0]=vk->refIterV[0] + vk->fitV[0]*Step;  // target vertex for extrapolation
     targetVertex[1]=vk->refIterV[1] + vk->fitV[1]*Step;
     targetVertex[2]=vk->refIterV[2] + vk->fitV[2]*Step;
     vShift = sqrt( (vk->refV[0]-targetVertex[0])*(vk->refV[0]-targetVertex[0])
	           +(vk->refV[1]-targetVertex[1])*(vk->refV[1]-targetVertex[1])
                   +(vk->refV[2]-targetVertex[2])*(vk->refV[2]-targetVertex[2]) );
//std::cout<<"target="<<targetVertex[0]<<", "<<targetVertex[1]<<", "<<targetVertex[2]<<" vsht="<<vShift<<'\n';
     bool insideGoodVolume=false;
     if(vk->vk_fitterControl && vk->vk_fitterControl->vk_objProp) { insideGoodVolume = vk->vk_fitterControl->vk_objProp->checkTarget(targetVertex);}
     else                                                      { insideGoodVolume = myPropagator.checkTarget(targetVertex); }
     if(!insideGoodVolume) { return -16; }       //Vertex is definitely outside working volume
     for(it=0; it<NTRK; it++){
       trk=vk->TrackList[it];
       if(trk->Id < 0){                // pseudo-track from cascade vertex
          trk->fitP[0] =trk->iniP[0]+ Step*(trk->fitP[0]-trk->iniP[0]);
          trk->fitP[1] =trk->iniP[1]+ Step*(trk->fitP[1]-trk->iniP[1]);
          trk->fitP[2] =trk->iniP[2]+ Step*(trk->fitP[2]-trk->iniP[2]);
          continue;
       }
       myPropagator.Propagate(trk, vk->refV,  targetVertex, tmpPer, tmpCov, (vk->vk_fitterControl).get());
 //Check!!!And protection if needed.
       double eig5=cfSmallEigenvalue(tmpCov,5 );
       if(eig5<1.e-15 || tmpCov[0]>1.e7) {  //Bad propagation with material. Try without it.
          myPropagator.Propagate(-999, trk->Charge,trk->refPerig,trk->refCovar, vk->refV, targetVertex, tmpPer, tmpCov, (vk->vk_fitterControl).get());
	  if(cfSmallEigenvalue(tmpCov,5 )<1.e-15){    //Final protection
	        tmpCov[1]=0.;tmpCov[3]=0.;tmpCov[6]=0.;tmpCov[10]=0.;
		             tmpCov[4]=0.;tmpCov[7]=0.;tmpCov[11]=0.;
		                          tmpCov[8]=0.;tmpCov[12]=0.;
		                                       tmpCov[13]=0.;
                tmpCov[0]=fabs(tmpCov[0]); tmpCov[2]=fabs(tmpCov[2]);tmpCov[5]=fabs(tmpCov[5]);
                tmpCov[9]=fabs(tmpCov[9]); tmpCov[14]=fabs(tmpCov[14]);
          }
       }
       if(tmpCov[0]>1.e7){ IERR=-7; return IERR; }     //extremely big A0 error !!!
       IERR=cfInv5(tmpCov, tmpWgt);  if (IERR) IERR=cfdinv(tmpCov, tmpWgt, 5); if(IERR)return -8; 
       trk->setCurrent(tmpPer,tmpWgt);
       if ( vShift > 100. ) trk->setReference( tmpPer, tmpCov ); //Change reference for big shift
     }      
     vk->setRefIterV(targetVertex);
     if ( vShift > 100. )vk->setRefV(targetVertex);  // Change reference vertex for big shift
     vk->iniV[0] = vk->fitV[0] = vk->cnstV[0]=0.;
     vk->iniV[1] = vk->fitV[1] = vk->cnstV[1]=0.;
     vk->iniV[2] = vk->fitV[2] = vk->cnstV[2]=0.;
     for(it=0; it<NTRK; it++){
       trk=vk->TrackList[it];
       if(Step<1.){                          // Step limitation for constraint calculation on next step
         trk->cnstP[0] = trk->iniP[0] =trk->iniP[0]+ Step*(trk->fitP[0]-trk->iniP[0]);
         trk->cnstP[1] = trk->iniP[1] =trk->iniP[1]+ Step*(trk->fitP[1]-trk->iniP[1]);
         trk->cnstP[2] = trk->iniP[2] =trk->iniP[2]+ Step*(trk->fitP[2]-trk->iniP[2]);
       }else{
         trk->cnstP[0] = trk->iniP[0] = trk->fitP[0];   // for constraint calculation on next step
         trk->cnstP[1] = trk->iniP[1] = trk->fitP[1];
         trk->cnstP[2] = trk->iniP[2] = trk->fitP[2];
       }
     }      
  }
  return 0;
}

//----------------------------------------------------------------------
// Translate all external (physics) tracks to fitted vertices positions
//   Step limitation is needed to break positive feedback
//
int restorePreviousPos(CascadeEvent & cascadeEvent_, std::vector<VKVertex> & SV )
{ 
  int iv,it;
  long int NTRK;
  VKTrack * trk; VKTrack * trks; VKVertex * vk; VKVertex * vks;
  
  for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     vk=cascadeEvent_.cascadeVertexList[iv];
     vks=&SV[iv];
     NTRK = vk->TrackList.size();            // Number of tracks at vertex
     vk->refIterV[0]=vks->refIterV[0];
     vk->refIterV[1]=vks->refIterV[1];
     vk->refIterV[2]=vks->refIterV[2];
     vk->iniV[0] = vk->fitV[0] = vk->cnstV[0]=0.;
     vk->iniV[1] = vk->fitV[1] = vk->cnstV[1]=0.;
     vk->iniV[2] = vk->fitV[2] = vk->cnstV[2]=0.;
     cfdcopy(vks->fitMom,       vk->fitMom, 3);        //saved Momentum
     cfdcopy(vks->fitCovXYZMom, vk->fitCovXYZMom, 21); //saved XYZMom covariance
     for(it=0; it<NTRK; it++){
       trk =vk ->TrackList[it];                  //  working track
       trks=vks->TrackList[it];                // track from saved copy
       if(trk->Id < 0) {     // pseudo-track from cascade vertex
          //trk->fitP[2]=0.;        // reset the pseudo-track parameters
          trk->fitP[0] =(trks->iniP[0]+trks->fitP[0])/2.;
          trk->fitP[1] = trks->iniP[1];
          trk->fitP[2] =(trks->iniP[2]+trks->fitP[2])/2.;
	  continue;
       }
       cfdcopy(trks->Perig,  trk->Perig, 5);       
       cfdcopy(trks->WgtM,   trk->WgtM, 15);       
       cfdcopy(trks->fitP,   trk->fitP,  3);
       cfdcopy(trks->iniP,   trk->iniP,  3);
       cfdcopy(trks->cnstP,  trk->cnstP, 3);
     }      
  }
  for(iv=0; iv<cascadeEvent_.cascadeNV; iv++){
    vk = cascadeEvent_.cascadeVertexList[iv];
    if(vk->nextCascadeVrt){
       int pntCnst = vk->ConstraintList.size() - 1;  // pointing constraint is always last in the list
       VKPointConstraint * pcnst = dynamic_cast<VKPointConstraint*>(vk->ConstraintList[pntCnst]);
       if(pcnst==0) return -1;
       pcnst->setTargetVertex(vk->nextCascadeVrt->refIterV);
    }
  }
  return 0;
}

void getFittedCascade( CascadeEvent & cascadeEvent_,
                       std::vector< Vect3DF > & cVertices, 
	               std::vector< std::vector<double> > & covVertices,
                       std::vector< std::vector< VectMOM> > & fittedParticles,
		       std::vector< std::vector<double> > & cascadeCovar,
		       std::vector<double> & particleChi2,
		       std::vector<double> & fullCovar)
{
   cVertices.clear();
   covVertices.clear();
   fittedParticles.clear();
   particleChi2.clear();
//
//   Split common covariance matrix into pieces
//
   std::vector< std::vector<double> > cascadeCovarFit;
   setFittedMatrices(cascadeEvent_.fullCovMatrix, getCascadeNPar(cascadeEvent_), cascadeEvent_.matrixPnt, cascadeCovarFit, cascadeEvent_);
//
   double vBx,vBy,vBz,pp2,pt,invR;
   int iv,it,jt, NTRK, pnt;
//
   int PDIM=getCascadeNPar(cascadeEvent_, 1); // number of physics parametrs
   int ADIM=getCascadeNPar(cascadeEvent_, 0); // number of ALL parametrs
   double **DPhys     = new double*[PDIM]; for(it=0; it<PDIM; it++) DPhys[it]     = new double[ADIM];
   for(it=0;it<PDIM;it++) for(jt=0;jt<ADIM;jt++)DPhys[it][jt]=0.;
//
//
   Vect3DF vrtPos;
   VectMOM prtMom;
   VKVertex * vk;
   std::vector<VectMOM> momCollector;
   std::vector<double>  tmpCov(6),Deriv(3);
//
   int pntPhys=0;  // Counter for physics cascade parameters 
   for( iv=0; iv<cascadeEvent_.cascadeNV; iv++){
     vk=cascadeEvent_.cascadeVertexList[iv];
     vrtPos.X=vk->refIterV[0] + vk->fitV[0];  // target vertex for extrapolation
     vrtPos.Y=vk->refIterV[1] + vk->fitV[1];
     vrtPos.Z=vk->refIterV[2] + vk->fitV[2];
     cVertices.push_back(vrtPos);
     NTRK = vk->TrackList.size();            // Number of tracks at vertex
     momCollector.clear();
     int DIM=3*(NTRK+1);
     double **Deriv     = new double*[DIM]; for(it=0; it<DIM; it++) Deriv[it]     = new double[DIM];
     for(it=0;it<DIM;it++) for(jt=0;jt<DIM;jt++)Deriv[it][jt]=0.;
     Deriv[0][0]=1.;Deriv[1][1]=1.;Deriv[2][2]=1.;
     DPhys[pntPhys+0][cascadeEvent_.matrixPnt[iv]+0]=1.;
     DPhys[pntPhys+1][cascadeEvent_.matrixPnt[iv]+1]=1.;
     DPhys[pntPhys+2][cascadeEvent_.matrixPnt[iv]+2]=1.;
     myMagFld.getMagFld(vk->refIterV[0]+vk->fitV[0], vk->refIterV[1]+vk->fitV[1], vk->refIterV[2]+vk->fitV[2],
                                                      vBx,vBy,vBz,(vk->vk_fitterControl).get());
     for(it=0; it<NTRK; it++){
       std::array<double, 4> pp = getFitParticleMom( vk->TrackList[it], vBz );
       prtMom.Px=pp[0]; prtMom.Py=pp[1]; prtMom.Pz=pp[2]; prtMom.E=pp[3]; 
       momCollector.push_back( prtMom );
       if(vk->TrackList[it]->Id >= 0) particleChi2.push_back( vk->TrackList[it]->Chi2 ); //Only real tracks
       pp2  = pp[0]*pp[0] + pp[1]*pp[1] +pp[2]*pp[2];
       pt   = sqrt(pp[0]*pp[0] + pp[1]*pp[1]);
       invR = vk->TrackList[it]->fitP[2];

       pnt  = it*3+3;
       DPhys[pntPhys+pnt+0][cascadeEvent_.matrixPnt[iv]+pnt+0]= Deriv[pnt+0][pnt+0]=   0   ;                           //dPx/dTheta
       DPhys[pntPhys+pnt+0][cascadeEvent_.matrixPnt[iv]+pnt+1]= Deriv[pnt+0][pnt+1]= -pp[1];                           //dPx/dPhi
       DPhys[pntPhys+pnt+0][cascadeEvent_.matrixPnt[iv]+pnt+2]= Deriv[pnt+0][pnt+2]= -pp[0]/invR;                      //dPx/dinvR

       DPhys[pntPhys+pnt+1][cascadeEvent_.matrixPnt[iv]+pnt+0]= Deriv[pnt+1][pnt+0]=   0   ;                           //dPy/dTheta
       DPhys[pntPhys+pnt+1][cascadeEvent_.matrixPnt[iv]+pnt+1]= Deriv[pnt+1][pnt+1]=  pp[0];                           //dPy/dPhi
       DPhys[pntPhys+pnt+1][cascadeEvent_.matrixPnt[iv]+pnt+2]= Deriv[pnt+1][pnt+2]= -pp[1]/invR;                      //dPy/dinvR

       DPhys[pntPhys+pnt+2][cascadeEvent_.matrixPnt[iv]+pnt+0]= Deriv[pnt+2][pnt+0]= -pp2/pt;                          //dPz/dTheta
       DPhys[pntPhys+pnt+2][cascadeEvent_.matrixPnt[iv]+pnt+1]= Deriv[pnt+2][pnt+1]=   0    ;                          //dPz/dPhi
       DPhys[pntPhys+pnt+2][cascadeEvent_.matrixPnt[iv]+pnt+2]= Deriv[pnt+2][pnt+2]= -pp[2]/invR;                      //dPz/dinvR
     }
     pntPhys += DIM;
     fittedParticles.push_back(momCollector);
     cascadeCovar.push_back( transformCovar(DIM, Deriv, cascadeCovarFit[iv] ) );  //Transform covariance and save it
     for(int kk=0; kk<6; kk++) tmpCov[kk]=cascadeCovar[iv][kk];
     covVertices.push_back(tmpCov);
     for(it=0; it<DIM; it++) delete[]Deriv[it];
     delete[]Deriv;
   }
//
// Get full physics covariance 
   int itn,jtn;
   fullCovar.resize(PDIM*(PDIM+1)/2);
   for(it=0; it<PDIM; it++){
     for(jt=0; jt<=it; jt++){
        double tmp=0.;
	for(itn=0; itn<ADIM; itn++) for(jtn=0; jtn<ADIM; jtn++) tmp += DPhys[it][itn]*DPhys[jt][jtn]*cascadeEvent_.fullCovMatrix[itn*ADIM+jtn];
        fullCovar[it*(it+1)/2+jt]=tmp; // symmetrical index formula works ONLY if it>=jt!!!
     }
   }
   for(it=0; it<PDIM; it++) delete[]DPhys[it];
   delete[]DPhys;
//
   return;
}



} /* End of VKalVrtCore namespace */
