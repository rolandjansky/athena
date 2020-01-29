/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DetElementsRoadUtils_xk.h"
#include <cmath>

#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/PlaneSurface.h"


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
  namespace TRT_DetElementsRoadUtils_xk {
   void detElementInformation(const InDetDD::TRT_BaseElement& E,double* P) 
   {
     const double pi2=2.*M_PI, pi=M_PI;

     const Trk::RectangleBounds* rb = dynamic_cast<const Trk::RectangleBounds*>(&E.bounds());
     const Trk::DiscBounds*      db = dynamic_cast<const Trk::DiscBounds*>     (&E.bounds());

     Amg::Vector3D  C  = E.center();  double x0=C.x(), y0=C.y(), z0=C.z(); 
     Amg::Vector3D  AT = E.normal();  double A[3]={AT.x(),AT.y(),AT.z()};
     const Amg::Transform3D& T = E.surface().transform();
     double Ax[3]            = {T(0,0),T(1,0),T(2,0)};
     double Ay[3]            = {T(0,1),T(1,1),T(2,1)};
     double x[4],y[4],z[4];

     if(rb) {

       P[ 0]    = sqrt(x0*x0+y0*y0);
       P[ 1] = z0;   if(fabs(P[1])<.00004) P[1]=0.; 
       P[ 2] = atan2(y0,x0);
       P[ 3] = A[0]*x0+A[1]*y0+A[2]*z0;
       if(P[3]<0.) {P[3]*=-1.; A[0]*=-1.; A[1]*=-1.; A[2]*=-1.;} 
       double B  = A[0]*A[0]+A[1]*A[1]; 
       if(B>.01) P[4]=atan2(A[1],A[0]); else P[4]=P[2]; 

       sincos(P[4],&P[5],&P[6]);

       if(fabs(P[5])<.00004) P[5]=0.; 
       if(fabs(P[6])<.00004) P[6]=0.; 
       P[ 7] = sqrt(A[0]*A[0]+A[1]*A[1]); if(fabs(P[7])<.00004) P[7]=0.; 
       P[ 8] = A[2];                      if(fabs(P[8])<.00004) P[8]=0.;

       double dx = rb->halflengthX();
       double dy = rb->halflengthY();

       x[ 0] = x0+dx*Ax[0]+dy*Ay[0];
       y[ 0] = y0+dx*Ax[1]+dy*Ay[1];
       z[ 0] = z0+dx*Ax[2]+dy*Ay[2];

       x[ 1] = x0+dx*Ax[0]-dy*Ay[0];
       y[ 1] = y0+dx*Ax[1]-dy*Ay[1];
       z[ 1] = z0+dx*Ax[2]-dy*Ay[2];

       x[ 2] = x0-dx*Ax[0]-dy*Ay[0];
       y[ 2] = y0-dx*Ax[1]-dy*Ay[1];
       z[ 2] = z0-dx*Ax[2]-dy*Ay[2];

       x[ 3] = x0-dx*Ax[0]+dy*Ay[0];
       y[ 3] = y0-dx*Ax[1]+dy*Ay[1];
       z[ 3] = z0-dx*Ax[2]+dy*Ay[2];
     }
     else if (db) {

       Ax[0]=T(0,1); Ax[1]=T(1,1); Ax[2]=T(2,1);
       Ay[0]=T(0,0); Ay[1]=T(1,0); Ay[2]=T(2,0);

       P[ 0] = .5*(db->rMin()+db->rMax()); 
       P[ 1] = z0; 
       P[ 2] = atan2(Ay[1],Ay[0]);
       P[ 3] = A[0]*x0+A[1]*y0+A[2]*z0;
       if(P[3]<0.) {P[3]*=-1.; A[0]*=-1.; A[1]*=-1.; A[2]*=-1.;} 
       P[ 4] = P[2];

       sincos(P[4],&P[5],&P[6]);
       P[ 7] = sqrt(A[0]*A[0]+A[1]*A[1]); if(fabs(P[7])<.00004) P[7]=0.; 
       P[ 8] = A[2];                      if(fabs(P[8])<.00004) P[8]=0.;
       x0    = P[0]*P[6];
       y0    = P[0]*P[5];
       double df = db->halfPhiSector();
       double sn1,sn2,cs1,cs2;

       sincos(P[2]+df,&sn1,&cs1);
       sincos(P[2]-df,&sn2,&cs2);

       x[ 0] = db->rMax()*cs1;
       y[ 0] = db->rMax()*sn1;
       z[ 0] = z0;

       x[ 1] = db->rMin()*cs1;
       y[ 1] = db->rMin()*sn1;
       z[ 1] = z0;

       x[ 2] = db->rMin()*cs2;
       y[ 2] = db->rMin()*sn2;
       z[ 2] = z0;

       x[ 3] = db->rMax()*cs2;
       y[ 3] = db->rMax()*sn2;
       z[ 3] = z0;
     }
     else return;

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
     P[17]    = atan2(Ay[1],Ay[0]);
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
     for(int i=15; i!=28; ++i) {if(fabs(P[i])<.0004 ) P[i]=0.;}

     // Directions Phi and Eta in local system coordinates 
     //
     P[29] = Ax[1]*P[6]-Ax[0]*P[5];                
     P[30] = P[8]*(Ax[0]*P[6]+Ax[1]*P[5])-P[7]*Ax[2];
     P[31] = Ay[1]*P[6]-Ay[0]*P[5];                
     P[32] = P[8]*(Ay[0]*P[6]+Ay[1]*P[5])-P[7]*Ay[2];
   }
  }
}
    
