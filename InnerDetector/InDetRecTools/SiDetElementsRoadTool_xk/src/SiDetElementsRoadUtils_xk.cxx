/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SiDetElementsRoadUtils_xk.h"
#include <cmath>

///////////////////////////////////////////////////////////////////
// Output parameters: P[ 0]  - radius          centre of wafer
//                    P[ 1]  - z               centre of wafer
//                    P[ 2]  - azimuthal angle centra of wifer
//                    P[ 3]  - min. distance to surface
//                    P[ 4]  - azimythal angle normal to surface
//                    P[ 5]  - sin(P[4])
//                    P[ 6]  - cos(P[4])
//                    P[ 7]  - sin(Polar angle)
//                    P[ 8]  - cos(Polar abgle)
//                    P[ 9]  - min. radius   
//                    P[10]  - max. radius   
//                    P[11]  - min. z
//                    P[12]  - max  z  
//                    P[13]  - min. azimythal angle  
//                    P[14]  - max. azimuthal angle
//                    P[15]  - tilt angle
//                    P[16]  - min. dist to surface in XY-plane
//                    P[17]  - Tilt angle
//                    P[18]  - first  local coordinate of wafer
//                    P[19]  - second local coordinate of wafer 
//               P[20],P[21] - vector to -F  boundary 
//               P[22],P[23] - vector to +ZR boundary 
//               P[24],P[25] - vector to +F  boundary 
//               P[26],P[27] - vector to -ZR boundary 
//                    P[28]  - thikness of the detector element
//                    P[29]  - Ax of Phi exis
//                    P[30]  - Ay of Phi exis
//                    P[31]  - Ax of Eta exis
//                    P[32]  - Ay of Eta exis
///////////////////////////////////////////////////////////////////

namespace InDet {
  namespace SiDetElementsRoadUtils_xk {
    void detElementInformation(const InDetDD::SiDetectorElement& Si,double* P)
    {
      const double pi2=2.*M_PI, pi= M_PI;
  
      double Sl    = .5*Si.design().length  ();
      double Swmax = .5*Si.design().maxWidth();
      double Swmin = .5*Si.design().minWidth();
  
      // Thickness of the waver
      //
      P[28] = Si.design().thickness();
      Amg::Vector3D C  = Si.center();  double x0=C.x(), y0=C.y(), z0=C.z(); 
      Amg::Vector3D AT = Si.normal ();
      Amg::Vector3D AF = Si.phiAxis();
      Amg::Vector3D AE = Si.etaAxis();

      double      A[3]={AT.x(),AT.y(),AT.z()};   

      P[ 0]    = sqrt(x0*x0+y0*y0);
      P[ 1]    = z0;   
      P[ 2]    = atan2(y0,x0);
      P[ 3]    = A[0]*x0+A[1]*y0+A[2]*z0;
      if(P[3]<0.) {P[3]*=-1.; A[0]*=-1.; A[1]*=-1.; A[2]*=-1.;} 
      P[ 7]    =  sqrt(A[0]*A[0]+A[1]*A[1]);

      if(P[7] >.000001) P[4]=atan2(A[1],A[0]); else P[4]=P[2]; 
      sincos(P[4],&P[5],&P[6]);
      P[ 8]    = A[2];                      

      if( fabs(P[7]) <= .000001) {

        P[3] = fabs(P[1]); 
        if(P[8] > 0.) P[8] = 1.; else P[8] = -1.; 
        P[7] = 0.;
      }

      double x[4],y[4],z[4];

      x[0]     = x0+AF.x()*Swmax+AE.x()*Sl;
      y[0]     = y0+AF.y()*Swmax+AE.y()*Sl;
      z[0]     = z0+AF.z()*Swmax+AE.z()*Sl;

      x[1]     = x0+AF.x()*Swmin-AE.x()*Sl;
      y[1]     = y0+AF.y()*Swmin-AE.y()*Sl;
      z[1]     = z0+AF.z()*Swmin-AE.z()*Sl;

      x[2]     = x0-AF.x()*Swmin-AE.x()*Sl;
      y[2]     = y0-AF.y()*Swmin-AE.y()*Sl;
      z[2]     = z0-AF.z()*Swmin-AE.z()*Sl;

      x[3]     = x0-AF.x()*Swmax+AE.x()*Sl;
      y[3]     = y0-AF.y()*Swmax+AE.y()*Sl;
      z[3]     = z0-AF.z()*Swmax+AE.z()*Sl;

      double rmin= 10000., rmax=-10000.;
      double zmin= 10000., zmax=-10000.;
      double fmin= 10000., fmax=-10000.;
      for(int i=0; i!=4; ++i) {
        double r = sqrt(x[i]*x[i]+y[i]*y[i]);
        double f = atan2(y[i],x[i])-P[2]; if(f<-pi) f+=pi2; else if(f>pi) f-=pi2;
        double zf= z[i];
        if(r <rmin) rmin= r;
        if(r >rmax) rmax= r;
        if(zf<zmin) zmin=zf;
        if(zf>zmax) zmax=zf;
        if(f <fmin) fmin= f;
        if(f >fmax) fmax= f;
      }
      P[ 9]    = rmin;
      P[10]    = rmax;
      P[11]    = zmin;
      P[12]    = zmax;
      P[13]    = P[2]+fmin;
      P[14]    = P[2]+fmax;
      P[15]    = P[4]-P[2]; if(P[15]<-pi) P[15]+=pi2; else if(P[15]>pi) P[15]-=pi2;
      P[16]    = A[0]*x0+A[1]*y0;
      P[17]    = atan2(AE.y(),AE.x());
      P[17]    = P[17]-P[2]; if(P[17]<-pi) P[17]+=pi2; else if(P[17]>pi) P[17]-=pi2;

      // Calculation shape parameters
      //
      P[18]    = y0*P[6]-x0*P[5];                       // F  center
      P[19]    = P[8]*(x0*P[6]+y0*P[5])-P[7]*z0;        // ZR center
      P[20]    = +10000.;                               // -F
      P[21]    = 0.     ;                               //
      P[22]    = 0.     ;                               // +ZR
      P[23]    = -10000.;                               //
      P[24]    = -10000.;                               // +F
      P[25]    = 0.     ;                               //
      P[26]    = 0.     ;                               // -ZR
      P[27]    = +10000.;                               //
      //  
      for(int i=0; i!=4; ++i) {

        int   k1    = i;
        int   k2    = i+1; if(k2>3) k2=0;
        double x1   = y[k1]*P[6]-x[k1]*P[5]                  -P[18];                        
        double y1   = P[8]*(x[k1]*P[6]+y[k1]*P[5])-P[7]*z[k1]-P[19];
        double x2   = y[k2]*P[6]-x[k2]*P[5]                  -P[18];                        
        double y2   = P[8]*(x[k2]*P[6]+y[k2]*P[5])-P[7]*z[k2]-P[19];
        double d    = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
        double ax   =-(y2-y1)*(y1*x2-x1*y2)/d;
        double ay   = (x2-x1)*(y1*x2-x1*y2)/d;
        if(ax<P[20]) {P[20]=ax; P[21]=ay;}
        if(ax>P[24]) {P[24]=ax; P[25]=ay;}
        if(ay<P[27]) {P[26]=ax; P[27]=ay;}
        if(ay>P[23]) {P[22]=ax; P[23]=ay;}
      }

      // Directions Phi and Eta in local system coordinates 
      //
      P[29] = AF.y()*P[6]-AF.x()*P[5];                
      P[30] = P[8]*(AF.x()*P[6]+AF.y()*P[5])-P[7]*AF.z();
      P[31] = AE.y()*P[6]-AE.x()*P[5];                
      P[32] = P[8]*(AE.x()*P[6]+AE.y()*P[5])-P[7]*AE.z();
    }

  }
}
