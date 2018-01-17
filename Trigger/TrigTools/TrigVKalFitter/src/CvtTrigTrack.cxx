/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigVKalFitter/TrigVKalFitter.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
//-------------------------------------------------
#include <iostream> 
#include <cmath>

//--------------------------------------------------------------------
//  Extract TrigInDetTrack
//

 StatusCode TrigVKalFitter::CvtTrigTrack(const std::vector<const TrigInDetTrack*>& InpTrk,
        long int& ntrk) {

    std::vector<const TrigInDetTrack*>::const_iterator   i_ntrk;
    int i,j,ipnt,ii,jj;
    const TrigInDetTrackFitPar* mPer;
    double CovI[5][5];
    double A0Vert,ZVert,PhiVert,ThetaVert,PtVert,EtaVert,ct;
    const std::vector<double>* TrigTrkErr;
    double Deriv[5][5] ={{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},
                                          {0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};
//std::cout.setf( std::ios::scientific); std::cout.precision(6);

    for (i_ntrk = InpTrk.begin(); i_ntrk < InpTrk.end(); ++i_ntrk) {
//
//-- MeasuredPerigee in TrigInDetTrack
//
       mPer = (*i_ntrk)->param();
       if( mPer == 0 ){ continue; } 
       TrigTrkErr = mPer->cov(); 
//

       A0Vert    = mPer->a0() ;
       if( !std::isfinite(A0Vert) )     return StatusCode::FAILURE;
       ZVert     = mPer->z0() ;
       if( !std::isfinite(ZVert) )      return StatusCode::FAILURE;
       PhiVert   = mPer->phi0();
       if( !std::isfinite(PhiVert) )    return StatusCode::FAILURE;
       if(PhiVert>3.14159265358979) PhiVert-=2.*3.14159265358979;
       EtaVert   = mPer->eta(); 
       if( !std::isfinite(EtaVert) )    return StatusCode::FAILURE;
       ThetaVert = 2.0*atan(exp(-EtaVert)); 
       PtVert    = mPer->pT();
       if( !std::isfinite(PtVert) )     return StatusCode::FAILURE;

//std::cout<<" A0="<<A0Vert<<", ZV="<<ZVert<<", Phi="<<PhiVert<<", Eta="<<EtaVert<<"  Pt="<<PtVert<<'\n';

       m_apar[ntrk][0]= -A0Vert;
       m_apar[ntrk][1]=  ZVert;
       m_apar[ntrk][2]=  ThetaVert;
       m_apar[ntrk][3]=  PhiVert;
       m_apar[ntrk][4]= -m_CNVMAG*m_BMAG/PtVert;
       m_ich[ntrk]    =  PtVert > 0 ? -1 : 1;

                    /* a0,phi,Z,eta,pt - order */
/* Order of matrix element in vector: 00,01,02,03,04,11,12,13,14,22,23,24,33,34,44 */
/*                                     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 */ 
	
       if( TrigTrkErr != 0) {
         CovI[0][0] =              (*TrigTrkErr)[0];

         CovI[1][0] = CovI[0][1] = (*TrigTrkErr)[1];
         CovI[1][1] =              (*TrigTrkErr)[5];

         CovI[0][2] = CovI[2][0] = (*TrigTrkErr)[2];
         CovI[1][2] = CovI[2][1] = (*TrigTrkErr)[6];
         CovI[2][2] =              (*TrigTrkErr)[9]; 

         CovI[0][3] = CovI[3][0] = (*TrigTrkErr)[3];
         CovI[1][3] = CovI[3][1] = (*TrigTrkErr)[7];
         CovI[2][3] = CovI[3][2] = (*TrigTrkErr)[10]; 
         CovI[3][3] =              (*TrigTrkErr)[12]; 

         CovI[0][4] = CovI[4][0] = (*TrigTrkErr)[4];
         CovI[1][4] = CovI[4][1] = (*TrigTrkErr)[8];
         CovI[2][4] = CovI[4][2] = (*TrigTrkErr)[11];
         CovI[3][4] = CovI[4][3] = (*TrigTrkErr)[13];
         CovI[4][4] =              (*TrigTrkErr)[14];
       } else {
         CovI[0][0] =               mPer->ea0()*mPer->ea0();

         CovI[1][0] = CovI[0][1] =  0.;
         CovI[1][1] =               mPer->ephi0()*mPer->ephi0();

         CovI[0][2] = CovI[2][0] =  0.;
         CovI[1][2] = CovI[2][1] =  0.;
         CovI[2][2] =               mPer->ez0()*mPer->ez0();

         CovI[0][3] = CovI[3][0] =  0.;
         CovI[1][3] = CovI[3][1] =  0.;
         CovI[2][3] = CovI[3][2] =  0.; 
         CovI[3][3] =               mPer->eeta()*mPer->eeta(); 

         CovI[0][4] = CovI[4][0] =  0.;
         CovI[1][4] = CovI[4][1] =  0.;
         CovI[2][4] = CovI[4][2] =  0.;
         CovI[3][4] = CovI[4][3] =  0.;
         CovI[4][4] =               mPer->epT()*mPer->epT();
       }

       for(i=0; i<5; i++){ for(j=0; j<=i; j++){  if ( !std::isfinite(CovI[i][j]) ) return StatusCode::FAILURE;}}

//std::cout<<CovI[0][0]<<", "<<CovI[1][0]<<", "<<CovI[1][1]<<", "<<TrigTrkErr<<'\n';
//std::cout<<mPer->ephi0()<<", "<<mPer->ephi0()<<", "<<mPer->ea0()<<", "<<mPer->ez0()<<'\n';
       if(CovI[0][0] < 1.e-30) CovI[0][0] = 1.0e-4;                 // 100mkm   A0
       if(CovI[1][1] < 1.e-30) CovI[2][2] = 3.0e-4;                 // 1deg   phi
       if(CovI[2][2] < 1.e-30) CovI[1][1] = 4.0e-4;                 // 200mkm   Z
       if(CovI[3][3] < 1.e-30) CovI[3][3] = 1.0e-4;                 // 0.01   eta
       if(CovI[4][4] < 1.e-30) CovI[4][4] = 1.0e-4*std::pow(PtVert,4);   // 0.01*Pt²  Pt error

           /* D(new)/D(old) */

       Deriv[0][0] = -1.;
       Deriv[1][2] =  1.;
       Deriv[2][3] = -2.* exp(-EtaVert)/(exp(-2.*EtaVert)+1.);
       Deriv[3][1] =  1.; 
       Deriv[4][4] = -m_CNVMAG*m_BMAG/PtVert/PtVert;

       ipnt=0;
       for(i=0;i<5;i++){ for(j=0;j<=i;j++){
         ct=0.;
         for(ii=0;ii<5;ii++){
	   if(Deriv[i][ii] == 0.) continue;
           for(jj=0;jj<5;jj++){ 
	     if(Deriv[j][jj] == 0.) continue;
	     ct += CovI[ii][jj]*Deriv[i][ii]*Deriv[j][jj];};};
         m_awgt[ntrk][ipnt++]=ct;
       };}

//
       ntrk++; if(ntrk>=NTRMAXTRIG) return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }


  TrigInDetTrack* TrigVKalFitter::CreateTrigTrack( const std::vector<double>& VKPerigee,
                                               const std::vector<double>& VKCov)
   {
     double TrkP0, TrkP1, TrkP2, TrkP3, TrkP4, Theta;
     TrkP0=-VKPerigee[0];           /*!!!! Change of sign !!!!*/
     TrkP1= VKPerigee[1];
     TrkP2= VKPerigee[3];                               /* phi */
     Theta= VKPerigee[2];
     TrkP3=-log(tan(Theta/2));
     TrkP4= (m_CNVMAG*m_BMAG)/ VKPerigee[4];       /* Pt */
     TrkP4=-TrkP4;                      /*!!!! Change of sign of charge!!!!*/

 
     double Deriv[5][5],CovMtxOld[5][5],CovMtx[5][5];
     int i,j,ik,jk;
     for(i=0;i<5;i++){ for(j=0;j<5;j++) {Deriv[i][j]=0.; CovMtxOld[i][j]=0.;}}
     Deriv[0][0]=-1.;
     Deriv[1][3]= 1.;
     Deriv[2][1]= 1.;
     Deriv[3][2]=-1./sin(Theta);
     Deriv[4][4]= (m_CNVMAG*m_BMAG) / (VKPerigee[4]*VKPerigee[4]);
 
     CovMtxOld[0][0]                =VKCov[0];
     CovMtxOld[0][1]=CovMtxOld[1][0]=VKCov[1];
     CovMtxOld[1][1]                =VKCov[2];
     CovMtxOld[0][2]=CovMtxOld[2][0]=VKCov[3];
     CovMtxOld[1][2]=CovMtxOld[2][1]=VKCov[4];
     CovMtxOld[2][2]                =VKCov[5];
     CovMtxOld[0][3]=CovMtxOld[3][0]=VKCov[6];
     CovMtxOld[1][3]=CovMtxOld[3][1]=VKCov[7];
     CovMtxOld[2][3]=CovMtxOld[3][2]=VKCov[8];
     CovMtxOld[3][3]                =VKCov[9];
     CovMtxOld[0][4]=CovMtxOld[4][0]=VKCov[10];
     CovMtxOld[1][4]=CovMtxOld[4][1]=VKCov[11];
     CovMtxOld[2][4]=CovMtxOld[4][2]=VKCov[12];
     CovMtxOld[3][4]=CovMtxOld[4][3]=VKCov[13];
     CovMtxOld[4][4]                =VKCov[14];
 
     for(i=0;i<5;i++){
      for(j=i;j<5;j++){
        CovMtx[i][j]=0.;
        for(ik=0;ik<5;ik++){
         for(jk=0;jk<5;jk++){
            CovMtx[i][j] += Deriv[i][ik]*CovMtxOld[ik][jk]*Deriv[j][jk];
        }}
     }}
 
     std::vector<double>* Ncov = new std::vector<double>;
     (*Ncov).clear();
     (*Ncov).push_back(CovMtx[0][0]);
     (*Ncov).push_back(CovMtx[0][1]);
     (*Ncov).push_back(CovMtx[1][1]);
     (*Ncov).push_back(CovMtx[0][2]);
     (*Ncov).push_back(CovMtx[1][2]);
     (*Ncov).push_back(CovMtx[2][2]);
     (*Ncov).push_back(CovMtx[0][3]);
     (*Ncov).push_back(CovMtx[1][3]);
     (*Ncov).push_back(CovMtx[2][3]);
     (*Ncov).push_back(CovMtx[3][3]);
     (*Ncov).push_back(CovMtx[0][4]);
     (*Ncov).push_back(CovMtx[1][4]);
     (*Ncov).push_back(CovMtx[2][4]);
     (*Ncov).push_back(CovMtx[3][4]);
     (*Ncov).push_back(CovMtx[4][4]);


      TrigInDetTrackFitPar* NewTrackPar =new TrigInDetTrackFitPar(TrkP0, TrkP2, TrkP1, TrkP3, TrkP4,  Ncov);
      TrigInDetTrackFitPar* NewTrackParEnd =0; 

      return new TrigInDetTrack( NewTrackPar, NewTrackParEnd, 10.); 
   }
   
   
   
