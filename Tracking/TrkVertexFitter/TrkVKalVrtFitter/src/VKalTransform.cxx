/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include <cmath>
#include <iostream>
//-------------------------------------------------
/* Coversion from ATLAS to VKalVrt parameters
 Output:
        VTrkPar[5]   - track parameters for VKalVrt
        VTrkCov[15]  - track parameters error matrix
        Charge       - track charge

   Convertion is always done for PERIGEE parameters .
   Only Z component of magnetic field is used for momentum calculation
   Magnetic field should be taken at point of conversion ( (0,0,0) usually)
*/
//--------------------------------------------------------------------

namespace Trk{

 bool TrkVKalVrtFitter::convertAmg5SymMtx(const AmgSymMatrix(5)* AmgMtx, double stdSymMtx[15]) const
 {     
       if(!AmgMtx) return false;
       //----- Check perigee covarince matrix for safety
       double DET=AmgMtx->determinant();
       if( DET!=DET ) {
          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" NaN in Perigee covariance is detected! Stop fit."<<endmsg;
	  return false;
       }
       if( fabs(DET) < 1000.*std::numeric_limits<double>::min()) {
          if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<"Zero Perigee covariance DET is detected! Stop fit."<<endmsg;
          return false;
       }
//std::cout.setf(std::ios::scientific); std::cout<<"VKMINNUMB="<<std::numeric_limits<double>::min()<<", "<<DET<<'\n';
       //---------------------------------------------------------
       stdSymMtx[ 0] =(*AmgMtx)(0,0);
       stdSymMtx[ 1] =(*AmgMtx)(1,0);
       stdSymMtx[ 2] =(*AmgMtx)(1,1);
       stdSymMtx[ 3] =(*AmgMtx)(2,0);
       stdSymMtx[ 4] =(*AmgMtx)(2,1);
       stdSymMtx[ 5] =(*AmgMtx)(2,2);
       stdSymMtx[ 6] =(*AmgMtx)(3,0);
       stdSymMtx[ 7] =(*AmgMtx)(3,1);
       stdSymMtx[ 8] =(*AmgMtx)(3,2);
       stdSymMtx[ 9] =(*AmgMtx)(3,3);
       stdSymMtx[10] =(*AmgMtx)(4,0);
       stdSymMtx[11] =(*AmgMtx)(4,1);
       stdSymMtx[12] =(*AmgMtx)(4,2);
       stdSymMtx[13] =(*AmgMtx)(4,3);
       stdSymMtx[14] =(*AmgMtx)(4,4);
       return true;
 }
 
 void TrkVKalVrtFitter::VKalTransform(double BMAG_FIXED,
     double A0V,double ZV,double PhiV,double ThetaV,double  PInv,double CovTrk[15],
     long int & Charge, double VTrkPar[5], double VTrkCov[15]) const
 {
     int i,j,ii,jj;
     double CnvCst=m_CNVMAG*BMAG_FIXED;
     double  sinT = sin(ThetaV);
     double  cosT = cos(ThetaV);
 
     VTrkPar[0] = - A0V                ;
     VTrkPar[1] =   ZV                 ;
     VTrkPar[2] =   ThetaV;
     VTrkPar[3] =   PhiV;
     VTrkPar[4] =  -PInv*CnvCst/sinT   ;
     Charge     =   PInv > 0 ? -1 : 1;
//
//     
     double CovI[5][5];
     double Deriv[5][5] ={{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.},
                                           {0.,0.,0.,0.,0.},{0.,0.,0.,0.,0.}};

     CovI[0][0] = CovTrk[0];

     CovI[1][0] = CovTrk[1];
     CovI[0][1] = CovTrk[1];
     CovI[1][1] = CovTrk[2];

     CovI[0][2] = CovTrk[3];
     CovI[2][0] = CovTrk[3];
     CovI[1][2] = CovTrk[4];
     CovI[2][1] = CovTrk[4];
     CovI[2][2] = CovTrk[5]; 

     CovI[0][3] = CovTrk[6];
     CovI[3][0] = CovTrk[6];
     CovI[1][3] = CovTrk[7];
     CovI[3][1] = CovTrk[7];
     CovI[2][3] = CovTrk[8]; 
     CovI[3][2] = CovTrk[8];
     CovI[3][3] = CovTrk[9]; 

     CovI[0][4] = CovTrk[10]  ;
     CovI[4][0] = CovTrk[10]  ;
     CovI[1][4] = CovTrk[11]  ;
     CovI[4][1] = CovTrk[11]  ;
     CovI[2][4] = CovTrk[12]  ;
     CovI[4][2] = CovTrk[12]  ;
     CovI[3][4] = CovTrk[13]  ;
     CovI[4][3] = CovTrk[13]  ;
     CovI[4][4] = CovTrk[14]  ;


     Deriv[0][0] = -1.;
     Deriv[1][1] =  1.;
     Deriv[2][3] =  1.;
     Deriv[3][2] =  1.;
     Deriv[4][3] =  PInv*CnvCst *(cosT/sinT/sinT) ;
     Deriv[4][4] = -CnvCst/sinT;

     double ct;
     int ipnt=0;
     for(i=0;i<5;i++){ for(j=0;j<=i;j++){
       ct=0.;
       for(ii=4;ii>=0;ii--){
	  if(Deriv[i][ii] == 0.) continue;
          for(jj=4;jj>=0;jj--){ 
	     if(Deriv[j][jj] == 0.) continue;
	     ct += CovI[ii][jj]*Deriv[i][ii]*Deriv[j][jj];};};
       VTrkCov[ipnt++]=ct;
     };}
     
}
} // end of namespace
