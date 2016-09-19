/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <math.h>
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

 bool TrkVKalVrtFitter::convertAmg5SymMtx(const AmgSymMatrix(5)* AmgMtx, double stdSymMtx[15])
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
     long int & Charge, double VTrkPar[5], double VTrkCov[15])
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
     return;

}
//--------------------------------------------------------------------------
   Amg::RotationMatrix3D TrkVKalVrtFitter::getMagFldRotation(double bx,double by, double bz, double vX,double vY,double vZ)
   {
     
     Amg::Vector3D field(bx,by,bz); Amg::Vector3D addon(vX,vY,vZ);
     Amg::Vector3D axis1=field.unit();
//-------------------------------------------------
//     CLHEP::Hep3Vector axis2=field.orthogonal().unit();
//
     if(addon.mag2()==0.) addon[0]=100.;
     Amg::Vector3D axis2=field.cross(addon);
     if(axis2.mag2() < 1.e-10){addon[1]+=100.; axis2=field.cross(addon);}
     axis2.unit();
//-------------------------------------------------
     Amg::Vector3D axis3=axis2.cross(axis1).unit();
     //CLHEP::HepRotation ROT(axis3,axis2,axis1);
     Amg::RotationMatrix3D ROT; ROT.col(0)=axis3; ROT.col(1)=axis2; ROT.col(2)=axis1;
//std::cout<<"test="<<ROT.inverse()*field<<'\n';
//CLHEP::Hep3Vector fieldC(0.,0.,(ROT.inverse()*field).z());
//std::cout<<"inversion test="<<ROT*fieldC<<'\n';
     return ROT.inverse();
   }
//-----------------------------------------------------------------
//  Rotation of fit results back to global ATLAS ref.system
//    vi[] and pi[] are in LOCAL(CORE) rotated fitter frame
//    v0[] and po[] are in LOCAL ATLAS nonrotated frame
//
   void TrkVKalVrtFitter::rotateBack(double vi[],double pi[], double covi[],
                                     double vo[],double po[], double covo[])
   {
       CLHEP::HepMatrix CNV(6,6,1);
       CLHEP::HepMatrix tmp(3,3,0); 
       Amg::RotationMatrix3D amgtmp=m_trkControl[0].trkRotation.inverse();
       tmp[0][0]=amgtmp(0,0);  tmp[0][1]=amgtmp(0,1);  tmp[0][2]=amgtmp(0,2);
       tmp[1][0]=amgtmp(1,0);  tmp[1][1]=amgtmp(1,1);  tmp[1][2]=amgtmp(1,2);
       tmp[2][0]=amgtmp(2,0);  tmp[2][1]=amgtmp(2,1);  tmp[2][2]=amgtmp(2,2);
       CNV.sub(1,1,tmp); CNV.sub(4,4,tmp);
//-------------------------------------------------
       CLHEP::HepMatrix COV(6,6,1);
       int cnt=0;  //counter
       for(int i=0; i<6; i++) for(int j=0; j<=i; j++){ COV[i][j]=COV[j][i]=covi[cnt]; cnt++;}	
       CLHEP::HepMatrix newCOV = CNV*COV*(CNV.T());
//----------Vector rotation
       CLHEP::HepVector param_rot(6);
       param_rot[0] = vi[0] - m_trkControl[0].trkSavedLocalVertex.x();
       param_rot[1] = vi[1] - m_trkControl[0].trkSavedLocalVertex.y();
       param_rot[2] = vi[2] - m_trkControl[0].trkSavedLocalVertex.z();
       param_rot[3] = pi[0]; param_rot[4] = pi[1]; param_rot[5] = pi[2];
       CLHEP::HepVector param_atl=CNV*param_rot;
//------------------------------------------------------------------------------------------
       vo[0]=param_atl[0] + m_trkControl[0].trkRotationVertex.x() - m_refFrameX;
       vo[1]=param_atl[1] + m_trkControl[0].trkRotationVertex.y() - m_refFrameY;
       vo[2]=param_atl[2] + m_trkControl[0].trkRotationVertex.z() - m_refFrameZ;
       po[0]=param_atl[4];  po[1]=param_atl[5]; po[2]=param_atl[6];
       cnt=0; for(int i=0; i<6; i++) for(int j=0; j<=i; j++){ covo[cnt]=newCOV[i][j]; cnt++;}	
   }
//--
   Amg::Vector3D TrkVKalVrtFitter::rotateBack(double px,double py, double pz)
   {
       return (m_trkControl[0].trkRotation.inverse())*Amg::Vector3D(px,py,pz);
   }
} // end of namespace
