/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iostream>
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include "TrkVKalVrtCore/Derivt.h"

namespace Trk {


//These two objects either use pointers to external handlers
//provided on input or returns constant magentic field and
//propagation in it.
// No writable data members ==> thread-safe
//=========================================================
vkalMagFld      myMagFld;
vkalPropagator  myPropagator;


#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))

void cfdcopy(double *source, double *target, int n)
{   for(int i=0; i<n; i++) target[i]=source[i]; }

void printT(double p[], double e[], std::string name){
  std::cout<<name<<p[0]<<", "<<p[1]<<", "<<p[2]<<", "<<p[3]<<", "<<p[4]<<'\n';
  std::cout<<e[0]<<'\n';
  std::cout<<e[1]<<", "<<e[2]<<'\n';
  std::cout<<e[3]<<", "<<e[4]<<", "<<e[5]<<'\n';
  std::cout<<e[6]<<", "<<e[7]<<", "<<e[8]<<", "<<e[9]<<'\n';
  std::cout<<e[10]<<", "<<e[11]<<", "<<e[12]<<", "<<e[13]<<", "<<e[14]<<'\n';
}

void protectCurvatureSign(double ini,double cur,double * Wgt){ //VK 15.12.2009 Protection against sign change
   double x=cur/ini;   if(x>=0.7) return;
   if(x<=0.) return;
   Wgt[14]*=(0.7*0.7)/(x*x);
}   

extern int cfdinv(double *, double *, long int);
extern double cfSmallEigenvalue( double*, long int );


/*----------------------------------------------------------------------------*/
/*        CONSTRAINT VERTEX FIT:                                              */
/*        INPUT :                                                             */
/*                          IFLAG       : what to do                          */
/*                        0 - simple vertex fit                               */
/*                        1 - fit with mass constraint                        */
/*                        2 - fit with constraint on passing through vertex VRT*/
/*                        3 - fit with Z-constraint                           */
/*                        4 - 1+2                                             */
/*                        5 - 1+3                                             */
/*                        6 - fit with primary vertex constraint              */
/*                        7 - fit with summary vector close to given vertex   */
/*                        8 - 1+7                                             */
/*                        9 - like 7 but summary track error is not used      */
/*                       10 - like 8 but summary track error is not used      */
/*                       11 - Phi angle between track is 0                    */
/*                       12 - Phi and Theta angles between track are 0        */
/*                       13 - 11+7                                            */
/*                       14 - 12+7                                            */
/* 	            IFCOVV0     : =0 only vertex error matrix(6)  is returned */
/* 	                          >0 full V0 x,p error matrix(21) is returned */
/*                              (calculations are CPU consuming in case of    */
/*                                constraints 1:5 with big track number)      */
/* 			   NTRK        : number of tracks                     */
/* 			   ICH(JTK)    : charge of tracks                     */
/*                         XYZ0(3)     : starting point                       */
/*                         PAR0(3,JTK) : tracks parameters in XYZ             */
/*                         APAR(5,JTK) : measured tracks parameters           */
/*                         AWGT(15,JTK): tracks parameter errors              */
/*        OUTPUT:          XYZFIT(3)   : fitted sec. vertex                   */
/*                         PARFS(3,JTK): th,phi,1/r of tracks at vertex       */
/*                         PTOT(3)     : total momentum in vertex             */
/*                         COVF(21)    : covariance matrix (x,y,z,ptot(3))    */
/* 			   CHI2        : total chi-squared                    */
/* 			   CHI2TR(JTK) : chi-squared per track                */
/*                          IERR        : error flag                          */
/*                         -1     bad fit chi2>1000000 ( no fitted output)    */
/*                         -2     fit is ready but without covariance matrix  */
/*                         -3     no fit at all                               */
/*                         -4     noninvertible vert.error matrix for IFLAG=6 */
/*                         -5     Divergence of iterations                    */
/* Author: V.Kostyukhin ( 1996 )                                            */
/*----------------------------------------------------------------------------*/


//
//  New data model for cascade fit
//



VKVertex createVertex(long int NTRK, long int *ich, double *xyz0, double *par0,
	              double *inp_Trk5, double *inp_CovTrk5, VKalVrtControl * currFitControl)
{
    long int  tk;

    VKVertex MainVRT(*currFitControl);           
    ForCFT & vrtForCFT=currFitControl->m_forcft;

    double xyz[3]={0.,0.,0.};              // Perigee point
//
//VK 13.01.2009 New strategy with ref.point left at initial position
    MainVRT.setRefV(xyz);      //ref point
    MainVRT.setRefIterV(xyz0);  //iteration ref. point
    for (tk=0; tk<NTRK ; tk++) {
//     printT(&inp_Trk5[tk*5], &inp_CovTrk5[tk*15] , "Input track:");
       MainVRT.TrackList.push_back(new VKTrack(tk, &inp_Trk5[tk*5], &inp_CovTrk5[tk*15], &MainVRT, vrtForCFT.wm[tk]));
       MainVRT.tmpArr.push_back(new TWRK());
       MainVRT.TrackList[tk]->Charge = ich[tk];           // Charge coinsides with sign of curvature
    }

    MainVRT.FVC.Charge = 0; for (tk=0; tk<NTRK; ++tk)MainVRT.FVC.Charge += ich[tk];  // summary charge
    cfdcopy(vrtForCFT.vrt,    MainVRT.FVC.vrt ,    3);
    cfdcopy(vrtForCFT.covvrt, MainVRT.FVC.covvrt , 6);

/* --------------------------------------------------------*/
/*           Initial value                                 */
/* --------------------------------------------------------*/
    VKTrack *trk; 
    for( tk=0; tk<NTRK; tk++){
      trk = MainVRT.TrackList[tk]; 
      trk->iniP[0]=trk->cnstP[0]=trk->fitP[0]=par0[tk*3+0];   //initial guess
      trk->iniP[1]=trk->cnstP[1]=trk->fitP[1]=par0[tk*3+1];
      trk->iniP[2]=trk->cnstP[2]=trk->fitP[2]=par0[tk*3+2];
    }
    MainVRT.setIniV(xyz); MainVRT.setCnstV(xyz);           // initial guess. 0 of course.

    return MainVRT;
}


extern int afterFit(VKVertex *, double *, double *, double *, double *, const VKalVrtControlBase* = 0);
extern void vpderiv(bool, long int , double *, double *, double *, double *, double *, double *, double *, const VKalVrtControl * =0);
extern void cfmasserr(VKVertex* , int*, double, double*, double*);

int fitVertex(VKVertex * vk) 
{
    long int  i, jerr, tk, it=0;

    double chi2df, dparst[6];
    double chi2min, dxyzst[3],  chi21s=11., chi22s=10., vShift;
    double aermd[30],tmpd[30];  // temporary array
    double tmpPer[5],tmpCov[15], tmpWgt[15];
    double VrtMomCov[21],PartMom[4];
    double cnstRemnants=0., iniCnstRem=0.;

    const double ConstraintAccuracy=1.e-4;

    extern int  vtcfit( VKVertex * vk);
    extern void cfsetdiag(long int , double *, double );
    extern int cfInv5(double *cov, double *wgt );
    extern void cfTrkCovarCorr(double *cov);
    extern void applyConstraints(VKVertex * vk);
    extern void robtest(VKVertex * , long int );

//
//    New datastructure

    long int IERR = 0;
                                /* Type of the constraint for the fit */
    long int NTRK = vk->TrackList.size();
    ForCFT & vrtForCFT=vk->m_fitterControl->m_forcft;
    vk->passNearVertex=false;  //explicit setting - not to use
    vk->passWithTrkCov=false;  //explicit setting - not to use
    if ( vrtForCFT.usePassNear    )  vk->passNearVertex=true;
    if ( vrtForCFT.usePassNear==2 )  vk->passWithTrkCov=true;
    double zeroV[3]={0.,0.,0.};
    VKTrack * trk=0;

    chi2min = 1e15;
    chi2df = 0.;

    if( vrtForCFT.nmcnst && vrtForCFT.useMassCnst )  {       //mass constraints are present
      std::vector<int> index;
      for( int ic=0; ic<8; ic++){
        if(vrtForCFT.wmfit[ic]>0){    // new mass constraint
           index.clear();
           for(tk=0; tk<NTRK; tk++){ if( vrtForCFT.indtrkmc[ic*vkalNTrkM + tk] )index.push_back(tk);}
           vk->ConstraintList.push_back(new VKMassConstraint( NTRK, vrtForCFT.wmfit[ic], index, vk));
        }
      }
      //VKMassConstraint *ctmp=dynamic_cast<VKMassConstraint*>( vk->ConstraintList[0]); std::cout<<(*ctmp)<<'\n';
    }
    if( vrtForCFT.usePointingCnst==1 ){  //3Dpointing
      vk->ConstraintList.push_back(new VKPointConstraint( NTRK, vrtForCFT.vrt, vk));
      //VKPointConstraint *ptmp=dynamic_cast<VKPointConstraint*>( vk->ConstraintList[1]); std::cout<<(*ptmp)<<'\n';
    }
    if( vrtForCFT.usePointingCnst==2 ){  //Z pointing
      VKPointConstraint *temp = new VKPointConstraint( NTRK, vrtForCFT.vrt, vk);
      temp->onlyZ=true;
      vk->ConstraintList.push_back(temp);
      //VKPointConstraint *ptmp=dynamic_cast<VKPointConstraint*>( vk->ConstraintList[1]); std::cout<<(*ptmp)<<'\n';
    }
    if ( vrtForCFT.useAprioriVrt ) {
        cfdcopy(vrtForCFT.covvrt, tmpd,   6);
	IERR=cfdinv(tmpd, aermd, -3); if (IERR) {  IERR = -4; return IERR; }
        vk->useApriorVertex = 1;
        cfdcopy(vrtForCFT.vrt, vk->apriorV,   3);
        cfdcopy(      aermd, vk->apriorVWGT,6);
    }
    if ( vrtForCFT.usePhiCnst )  vk->ConstraintList.push_back(new VKPhiConstraint( NTRK, vk));
    if ( vrtForCFT.useThetaCnst )vk->ConstraintList.push_back(new VKThetaConstraint( NTRK, vk));
    if ( vrtForCFT.usePlaneCnst ){
      if( vrtForCFT.Ap+vrtForCFT.Bp+vrtForCFT.Cp != 0.){
        vk->ConstraintList.push_back(new VKPlaneConstraint( NTRK, vrtForCFT.Ap, vrtForCFT.Bp, vrtForCFT.Cp, vrtForCFT.Dp, vk));
      }
    }
//
// Needed for track close to vertex constraint
    for(i=0; i<2*6; i++)vk->FVC.cvder[i]=0.;
    for(i=0; i<21; i++)vk->FVC.dcovf[i]=0.;
    vk->FVC.dcovf[0]=10.;vk->FVC.dcovf[2] =10.;vk->FVC.dcovf[5] =10.;
    vk->FVC.dcovf[9]=10.;vk->FVC.dcovf[14]=10.;vk->FVC.dcovf[20]=10.;

/* --------------------------------------------------------*/
/*           Initial value                                 */
/* --------------------------------------------------------*/
    cfdcopy(vk->refIterV,dxyzst,3);                  // copy initial data to start values
    vk->setIniV(zeroV);vk->setCnstV(zeroV);          // initial guess. 0 of course.
/* ------------------------------------------------------------*/
/*           MAIN FITTING CYCLE                                */
/* ------------------------------------------------------------*/
     bool extrapolationDone=false;
     bool forcedExtrapolation=false;  //To force explicit extrapolation
     std::vector< Vect3DF > savedExtrapVertices(vrtForCFT.IterationNumber);
     double cnstRemnantsPrev=1.e20, Chi2Prev=0.; int countBadStep=0; // For consecutive bad steps
     for (it = 1; it <= vrtForCFT.IterationNumber; ++it) {
        vShift = sqrt( (vk->refIterV[0]-dxyzst[0])*(vk->refIterV[0]-dxyzst[0])
	              +(vk->refIterV[1]-dxyzst[1])*(vk->refIterV[1]-dxyzst[1])
	              +(vk->refIterV[2]-dxyzst[2])*(vk->refIterV[2]-dxyzst[2]) );
       savedExtrapVertices[it-1].X=dxyzst[0];   //it starts from 1 !!!
       savedExtrapVertices[it-1].Y=dxyzst[1];
       savedExtrapVertices[it-1].Z=dxyzst[2];
       if(it==1){
	 bool insideGoodVolume=false;
         if(vk->m_fitterControl && vk->m_fitterControl->m_objProp) { insideGoodVolume = vk->m_fitterControl->m_objProp->checkTarget(dxyzst);}
         else                                                      { insideGoodVolume = myPropagator.checkTarget(dxyzst); }
         if(!insideGoodVolume) { IERR=-10; return IERR; }       // First guess is definitely outside working volume
       }
/* ---------------------------------------------------------------- */
/*  Propagate parameters and errors at point dXYZST                 */
/*   Also set up localBMAG in dXYZST if nonuniform field is used    */
/* ---------------------------------------------------------------- */
        extrapolationDone=false;
        if( vShift>vkalShiftToTrigExtrapolation || it==1 || forcedExtrapolation){
          extrapolationDone=true;
          forcedExtrapolation=false;
          double oldX=0., oldY=0., oldZ=0.;
          if(it>=2){ oldX=savedExtrapVertices[it-2].X; oldY=savedExtrapVertices[it-2].Y; oldZ=savedExtrapVertices[it-2].Z;}
	  bool insideGoodVolume=false;
          while( !insideGoodVolume ){      //check extrapolation and limit step if needed
             dxyzst[0]=(savedExtrapVertices[it-1].X + oldX)/2.;
             dxyzst[1]=(savedExtrapVertices[it-1].Y + oldY)/2.;
             dxyzst[2]=(savedExtrapVertices[it-1].Z + oldZ)/2.;
             savedExtrapVertices[it-1].X=dxyzst[0];
             savedExtrapVertices[it-1].Y=dxyzst[1];
             savedExtrapVertices[it-1].Z=dxyzst[2];
             double ddx=savedExtrapVertices[it-1].X - oldX;
             double ddy=savedExtrapVertices[it-1].Y - oldY;
             double ddz=savedExtrapVertices[it-1].Z - oldZ;
             if( sqrt(ddx*ddx+ddy*ddy+ddz*ddz)<5.) { IERR=-11; return IERR; }       // Impossible to extrapolate
             if(vk->m_fitterControl && vk->m_fitterControl->m_objProp) { insideGoodVolume = vk->m_fitterControl->m_objProp->checkTarget(dxyzst);}
             else                                                      { insideGoodVolume = myPropagator.checkTarget(dxyzst); }
          }
	  double targV[3]={dxyzst[0],dxyzst[1],dxyzst[2]};
	  for (tk = 0; tk < NTRK; ++tk) {
            myPropagator.Propagate(vk->TrackList[tk], vk->refV,  targV, tmpPer, tmpCov, (vk->m_fitterControl).get());
            cfTrkCovarCorr(tmpCov);
            double eig5=cfSmallEigenvalue(tmpCov,5 );
 	    if(eig5<1.e-15 ){ 
                tmpCov[0]+=1.e-15; tmpCov[2]+=1.e-15; tmpCov[5]+=1.e-15;  tmpCov[9]+=1.e-15;  tmpCov[14]+=1.e-15; 
	    }else if(tmpCov[0]>1.e9) {  //Bad propagation with material. Try without it.
               myPropagator.Propagate(-999, vk->TrackList[tk]->Charge, vk->TrackList[tk]->refPerig,vk->TrackList[tk]->refCovar,
	              	                vk->refV,  targV, tmpPer, tmpCov, (vk->m_fitterControl).get()); 

	       if(cfSmallEigenvalue(tmpCov,5 )<1.e-15){    //Final protection
 	           tmpCov[1]=0.;tmpCov[3]=0.;tmpCov[6]=0.;tmpCov[10]=0.;
		                tmpCov[4]=0.;tmpCov[7]=0.;tmpCov[11]=0.;
		                             tmpCov[8]=0.;tmpCov[12]=0.;
		                                          tmpCov[13]=0.;
                   tmpCov[0]=fabs(tmpCov[0]); tmpCov[2]=fabs(tmpCov[2]);tmpCov[5]=fabs(tmpCov[5]);
                   tmpCov[9]=fabs(tmpCov[9]); tmpCov[14]=fabs(tmpCov[14]);
               }
            }
            if(tmpCov[0]>1.e9){ IERR=-7; return IERR; }     //extremely big A0 error !!!
            jerr=cfInv5(tmpCov, tmpWgt);
	    if(jerr)jerr=cfdinv(tmpCov, tmpWgt,5);
            if(jerr){ IERR=-7; return IERR; }       //Nothing helps. Break fit.
            vk->TrackList[tk]->setCurrent(tmpPer,tmpWgt);
//VK	    if ( vShift > 200. ) vk->TrackList[tk]->setReference( tmpPer, tmpCov ); //Change reference for big shift
          }
//VK          if ( vShift > 200. )vk->setRefV(dxyzst);  // Change reference vertex for big shift
//
/* Magnetic field in fitting point setting */
          vk->setRefIterV(dxyzst);
          vrtForCFT.localbmag = myMagFld.getMagFld(vk->refIterV,(vk->m_fitterControl).get());
          vk->setIniV(zeroV);vk->setCnstV(zeroV);    // initial guess with respect to refIterV[]. 0 after extrap.
        }else{
          vk->setIniV( vk->fitV ); vk->setCnstV( vk->fitV );  // use previous fitV[] as start position
          for( tk=0; tk<NTRK; tk++) vk->TrackList[tk]->restoreCurrentWgt(); // restore matrix changed by ROBTEST!!!
        }
/*-----------------------------------------------------------------------*/
/*   All parameters are set in new vertex position. We are ready for fit */
/*  Track WGT matrix is completely clean now                             */
/*                    - apply protection against charge sign change      */
/*-----------------------------------------------------------------------*/
	for (tk = 0; tk < NTRK; ++tk){
	    trk = vk->TrackList[tk]; protectCurvatureSign( trk->refPerig[4], trk->fitP[2] , trk->WgtM);
        }
/*--------------------------------  Now the fit itself -----------------*/
	if (vrtForCFT.irob != 0) {robtest(vk, 0);}  // ROBUSTIFICATION new data structure
	if (vrtForCFT.irob != 0) {robtest(vk, 1);}  // ROBUSTIFICATION new data structure
        for( tk=0; tk<NTRK; tk++){
	  trk = vk->TrackList[tk]; 
	  trk->iniP[0]=trk->cnstP[0]=trk->fitP[0];   //use fitted track parameters as initial guess
	  trk->iniP[1]=trk->cnstP[1]=trk->fitP[1];
	  trk->iniP[2]=trk->cnstP[2]=trk->fitP[2]; 
	  if(extrapolationDone)trk->iniP[2]=trk->cnstP[2]=trk->Perig[4]; //Use exact value here to take into account change of mag.field
	}
        applyConstraints( vk );
        if(it==1) { bool bsave = vk->passNearVertex; vk->passNearVertex=false; IERR = vtcfit( vk );  vk->passNearVertex=bsave;}
	else IERR = vtcfit( vk );    // In first iteration data for PassNearVertex constraint don't exist!!!
//std::cout<<" test1 IERR="<<IERR<<", "<<vk->Chi2<<'\n';
        if (IERR)  return IERR;

	vShift = sqrt( (vk->fitV[0]- vk->iniV[0])*(vk->fitV[0]-vk->iniV[0])
	              +(vk->fitV[1]- vk->iniV[1])*(vk->fitV[1]-vk->iniV[1])
	              +(vk->fitV[2]- vk->iniV[2])*(vk->fitV[2]-vk->iniV[2]) );
//----
	if(extrapolationDone && chi21s*10.<vk->Chi2 && it>2){  //Extrapolation produced huge degradation
          double ddx=savedExtrapVertices[it-1].X-savedExtrapVertices[it-2].X;
          double ddy=savedExtrapVertices[it-1].Y-savedExtrapVertices[it-2].Y;
          double ddz=savedExtrapVertices[it-1].Z-savedExtrapVertices[it-2].Z;
          double ddstep=sqrt(ddx*ddx+ddy*ddy+ddz*ddz);
//std::cout<<" Huge degradation due to extrapolation. Limit step! it="<<it<<" step="<<ddstep<<'\n';
          if( ddstep > 10.*vkalShiftToTrigExtrapolation) { 
            dxyzst[0]=(savedExtrapVertices[it-1].X + 2.*savedExtrapVertices[it-2].X)/3.;
            dxyzst[1]=(savedExtrapVertices[it-1].Y + 2.*savedExtrapVertices[it-2].Y)/3.;
            dxyzst[2]=(savedExtrapVertices[it-1].Z + 2.*savedExtrapVertices[it-2].Z)/3.;
	    forcedExtrapolation=true;
	    continue;
	  }
        }
        chi21s = vk->Chi2;
	chi22s = chi21s * 1.01 + 10.; //for safety 
	if ( vShift < 10.*vkalShiftToTrigExtrapolation) {              // REASONABLE DISPLACEMENT - RECALCULATE
/* ROBUSTIFICATION */
	  if (vrtForCFT.irob != 0) {robtest(vk, 1);}  // ROBUSTIFICATION new data structure
//Reset mag.field
          for( i=0; i<3; i++) dparst[i]=vk->refIterV[i]+vk->fitV[i]; // fitted vertex at global frame
          vrtForCFT.localbmag=myMagFld.getMagFld(dparst,(vk->m_fitterControl).get());
	  if ( vk->passNearVertex ) {
            jerr = afterFit(vk, vk->ader, vk->FVC.dcv, PartMom, VrtMomCov, (vk->m_fitterControl).get());
	    cfdcopy( PartMom, &dparst[3], 3);  //vertex part of it is filled above
            cfdcopy(VrtMomCov,vk->FVC.dcovf,21);  //Used in chi2 caclulation later...
	    cfdcopy(  PartMom, vk->fitMom, 3);          //save Momentum
            cfdcopy(VrtMomCov, vk->fitCovXYZMom, 21);   //save XYZMom covariance
	    vpderiv(vk->passWithTrkCov, vk->FVC.Charge, dparst, VrtMomCov, 
	                       vk->FVC.vrt, vk->FVC.covvrt, 
		               vk->FVC.cvder, vk->FVC.ywgt, vk->FVC.rv0, (vk->m_fitterControl).get());
	  }

          for( tk=0; tk<NTRK; tk++){
	    trk = vk->TrackList[tk]; 
	    trk->iniP[0]=trk->cnstP[0]=trk->fitP[0];   //use fitted track parameters as initial guess
	    trk->iniP[1]=trk->cnstP[1]=trk->fitP[1];
	    trk->iniP[2]=trk->cnstP[2]=trk->fitP[2];
          }
          vk->setIniV( vk->fitV ); vk->setCnstV( vk->fitV );  // use previous fitV[] as start position
          applyConstraints( vk);
          if(it==1) { bool bsave = vk->passNearVertex; vk->passNearVertex=false; IERR = vtcfit( vk );  vk->passNearVertex=bsave;}
	  else IERR = vtcfit( vk );    // In first iteration data for PassNearVertex constraint don't exist!!!
//std::cout<<" test2 IERR="<<IERR<<", "<<vk->Chi2<<'\n';
 	  if ( IERR ) return IERR;
	  vShift = sqrt( (vk->fitV[0]- vk->iniV[0])*(vk->fitV[0]-vk->iniV[0])
	                +(vk->fitV[1]- vk->iniV[1])*(vk->fitV[1]-vk->iniV[1])
	                +(vk->fitV[2]- vk->iniV[2])*(vk->fitV[2]-vk->iniV[2]) );
          chi22s = vk->Chi2;
        }
//
// 
	if (chi22s > 1e8) {  IERR = -1; return IERR; }      // TOO HIGH CHI2 - BAD FIT
	for( i=0; i<3; i++) dxyzst[i] = vk->refIterV[i]+vk->fitV[i];  //   fitted vertex in global frame
	//std::cout.precision(11);
	//std::cout<<"NNFIT Iter="<<it<<" Chi2ss="<< chi21s <<", "<<chi22s<<", "<<vShift<<'\n';
	//std::cout<<"NNVertex="<<dxyzst[0]<<", "<<dxyzst[1]<<", "<<dxyzst[2]<<'\n';
	//std::cout<<"-----------------------------------------------"<<'\n';
/*  Test of convergence */
	chi2df = fabs(chi21s - chi22s);
  /*---------------------Normal convergence--------------------*/
        double PrecLimit = min(chi22s*1.e-4, vrtForCFT.IterationPrecision);
	//std::cout<<"Convergence="<< chi2df <<"<"<<PrecLimit<<" cnst="<<cnstRemnants<<"<"<<ConstraintAccuracy<<'\n';
	if ((chi2df < PrecLimit) && (vShift < 0.001) && it>1 && (cnstRemnants<ConstraintAccuracy)){
	   double dstFromExtrapPnt=sqrt(vk->fitV[0]*vk->fitV[0] + vk->fitV[1]*vk->fitV[1]+ vk->fitV[2]*vk->fitV[2]);
	   if( dstFromExtrapPnt>vkalShiftToTrigExtrapolation/2. && it < vrtForCFT.IterationNumber-15){
	     forcedExtrapolation=true;
	     continue;          // Make another extrapolation exactly to found vertex position
           }
	   break;
        }
	chi2min = min(chi2min,chi22s);
	if ((chi2min*100. < chi22s) && (chi22s>max( (2*NTRK-3)*10., 100.)) && (it>5)){
	   //std::cout<<" DIVERGENCE="<<chi22s<<" Ratio="<<chi22s/chi2min<<'\n';
           IERR = -5; return IERR;           /* Divergence. Stop fit */
        }

// Track near vertex constraint recalculation for next fit
	if ( vk->passNearVertex ) {
            jerr = afterFit(vk, vk->ader, vk->FVC.dcv, PartMom, VrtMomCov, (vk->m_fitterControl).get());
            for( i=0; i<3; i++) dparst[i] = vk->refIterV[i]+vk->fitV[i]; // fitted vertex at global frame
            cfdcopy( PartMom, &dparst[3], 3);
            cfdcopy(VrtMomCov,vk->FVC.dcovf,21);  //Used in chi2 caclulation later...
	    cfdcopy(  PartMom, vk->fitMom, 3);          //save Momentum
            cfdcopy(VrtMomCov, vk->fitCovXYZMom, 21);   //save XYZMom covariance
	    vpderiv(vk->passWithTrkCov, vk->FVC.Charge, dparst, VrtMomCov, 
	                       vk->FVC.vrt, vk->FVC.covvrt, 
		               vk->FVC.cvder, vk->FVC.ywgt, vk->FVC.rv0, (vk->m_fitterControl).get());
	}
//
// Check constraints status
//
        cnstRemnants=0. ;
        for(int ii=0; ii<(int)vk->ConstraintList.size();ii++){
          for(int ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
            cnstRemnants +=  fabs( vk->ConstraintList[ii]->aa[ic] ); } }
        if(it==1){ if(cnstRemnants>0.){iniCnstRem=cnstRemnants;}else{iniCnstRem=1.e-12;} }
        if(it==10){   
	  if( cnstRemnants/iniCnstRem > 0.1 && cnstRemnants > 0.1){ 
            IERR = -6; return IERR;           /* Constraints are not resolved. Stop fit */
          }
        }         
//
// Check consecutive bad steps in fitting
//
        if(it>5 && ((cnstRemnants>cnstRemnantsPrev)||cnstRemnants==0.) && chi22s>(Chi2Prev*1.0001)){
	   if(++countBadStep>5) break;     // Apparently further improvement is impossible
        }else{countBadStep=0;}
	Chi2Prev=chi22s; cnstRemnantsPrev=cnstRemnants;

    }
/*--------------------------------------*/
/*  End of cycling                      */
/*  Now we have a secondary vertex xyzs */
/*--------------------------------------*/
    if( cnstRemnants > ConstraintAccuracy ){
       IERR = -6; return IERR;           /* Constraints are not resolved. Bad fit */
    }
    return 0;

}


     int CFit(VKalVrtControl &FitCONTROL, long int ifCovV0, long int NTRK, 
	      long int *ich, double *xyz0, double *par0,
	      double *inp_Trk5, double *inp_CovTrk5, 
	      double *xyzfit, double *parfs, double *ptot,
              double *covf, double *chi2, double *chi2tr)
{
    int IERR,tk;
    double dptot[5], VrtMomCov[21];

    ptot[0]=0.; ptot[0]=0.; ptot[0]=0.;     
    cfdcopy(xyz0,xyzfit,3);

//
//   Create vertex
//
    VKVertex MainVRT = createVertex(NTRK, ich, xyz0, par0, inp_Trk5, inp_CovTrk5, &FitCONTROL);
//
//  Fit vertex
//
    IERR = fitVertex( &MainVRT); 
    if(IERR)   return IERR;
//
//  if successful - save results
//
    (*chi2) = 0.;
    for (tk = 0; tk < NTRK; tk++) {
	(*chi2)   += MainVRT.TrackList[tk]->Chi2;
	chi2tr[tk] = MainVRT.TrackList[tk]->Chi2;
        cfdcopy( MainVRT.TrackList[tk]->fitP, &parfs[tk*3], 3);
    }
    for(int i=0; i<3; i++) xyzfit[i] = MainVRT.refIterV[i]+MainVRT.fitV[i];  //   fitted vertex in global frame
    cfdcopy(MainVRT.fitVcov, covf  , 6);  //fitted vertex covariance
//     
//  If required - get full covariance matrix
//
    if(ifCovV0){
      MainVRT.m_fitterControl->m_forcft.localbmag = myMagFld.getMagFld(xyzfit,(MainVRT.m_fitterControl).get());
      IERR = afterFit(&MainVRT, MainVRT.ader, MainVRT.FVC.dcv, dptot, VrtMomCov, (MainVRT.m_fitterControl).get());
      if (IERR) return -2;   /* NONINVERTIBLE COV.MATRIX */
      if(MainVRT.existFullCov){
        if(FitCONTROL.m_fullCovariance) delete[] FitCONTROL.m_fullCovariance;
        FitCONTROL.m_fullCovariance=new double( (3*NTRK+3)*(3*NTRK+3+1)/2 );
        int out=0;
        for(int ti=0; ti<3+3*NTRK; ti++){
          for(int tj=ti; tj<3+3*NTRK; tj++){
            FitCONTROL.m_fullCovariance[out] = ARR_2D(MainVRT.ader, 3*vkalNTrkM+3, ti, tj);
            out++;
        } }
        int activeTrk[vkalNTrkM]={1};
        cfmasserr(&MainVRT, activeTrk,MainVRT.m_fitterControl->m_forcft.localbmag, 
                      &FitCONTROL.m_vrtMassTot, &FitCONTROL.m_vrtMassError);
      }
      cfdcopy( VrtMomCov,  covf, 21);  //XvYvZvPxPyPz covariance
      cfdcopy( dptot,      ptot,  3);  //summary mom
      cfdcopy( MainVRT.m_fitterControl->m_forcft.robres, FitCONTROL.m_forcft.robres, NTRK); //Track weights from robust fit
      
//std::cout<<" newcov="<<VrtMomCov[0]<<", "<<VrtMomCov[2]<<", "<<VrtMomCov[4]<<", "
//                     <<VrtMomCov[9]<<", "<<VrtMomCov[14]<<", "<<VrtMomCov[13]<<'\n';
    }

    return 0;

}


} /* End of VKalVrtCore namespace */
