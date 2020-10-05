/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace Trk {



//     Chi2 calculation for 5-par. track 
//     XYZT(3)-vertex , ICH - charge , PART(3) -track pass. XYZT vertex 
//     PAR0(5) - fitted track 
// ------------------------------------------------------------ 

double cfchi2(double *xyzt, long int *ich, double *part, 
	double *par0, double *wgt, double *rmnd)
{

    extern void cfnewp(long int*, double*, double*, double*, double*, double*);
    double phif, epsf, d1, d2, d3, d4, d5, sr, uu, vv, res, zpf;

    /* Parameter adjustments */
    --wgt;
    --part;
    --xyzt;

    uu = xyzt[1] * cos(part[2]) + xyzt[2] * sin(part[2]);
    vv = xyzt[2] * cos(part[2]) - xyzt[1] * sin(part[2]);
    sr = part[3] * std::abs(*ich);
    epsf = -vv - (uu * uu + vv * vv) * sr / 2.;
    zpf = xyzt[3] - uu * (1. - vv * sr) / tan(part[1]);
    phif = part[2] - uu * sr;
    d1 = par0[0] - epsf;
    d2 = par0[1] - zpf;
    d3 = par0[2] - part[1];
    d4 = par0[3] - phif;
    d5 = par0[4] - part[3];
    while(d4 >  M_PI)d4-=2.*M_PI;
    while(d4 < -M_PI)d4+=2.*M_PI;
// -----------------------Check of propagation
//    double paro[5],parn[5],s,ref[3],peri[3];
//    paro[0]=0.; paro[1]=0.; paro[2]=part[1]; paro[3]=part[2]; paro[4]=part[3];
//    ref[0]=-xyzt[1]; ref[1]=-xyzt[2]; ref[2]=-xyzt[3]; 
//    cfnewp(ich, paro, ref, &s, parn, peri);
//    d1 =  par0[0] - parn[0]; d2 =  par0[1] - parn[1]; d3 =  par0[2] - parn[2];
//    d4 =  par0[3] - parn[3]; d5 =  par0[4] - parn[4];
//    std::cout<<" testp0="<<parn[0]<<", "<<parn[1]<<", "<<parn[3]<<", "<<s<<'\n';
//    std::cout<<" testp1="<<(*ich)<<", "<<epsf<<", "<<zpf<<", "<<phif<<", "<<res<<'\n';
//--------------------------------------------------------------------
    res = wgt[1] * (d1 * d1) + wgt[3] * (d2 * d2) + wgt[6] * (d3 * d3) 
       + wgt[10] * (d4 * d4) + wgt[15] * (d5 * d5) + (d2 * 
	    d1 * wgt[2] + d3 * (d1 * wgt[4] + d2 * wgt[5]) + d4 * (d1 * wgt[7]
	    + d2 * wgt[8] + d3 * wgt[9]) + d5 * (d1 * wgt[11] + d2 * wgt[12] 
	    + d3 * wgt[13] + d4 * wgt[14])) * 2;
    rmnd[0] = d1;
    rmnd[1] = d2;
    rmnd[2] = d3;
    rmnd[3] = d4;
    rmnd[4] = d5;
    return  (fabs(res) < 1.e12 ? fabs(res) : 1.e12);
} 


//     Chi2 calculation for 5-par. track 
//     VRTT(3)-vertex ,  PART(3) - parameters of track pass. through VRTT vertex 
// Procedure assumes that VRTT(3) is in system where track reference point is (0,0,0).
// So it transfers fitted parameters to (0,0,0) and compares with reference track parameters
// ------------------------------------------------------------ 

double cfchi2(double *vrtt, double * part, VKTrack * trk)
{

    double d1, d2, d3, d4, d5;
    double theta_f = part[0];
    double phi_f   = part[1];
    double invR_f  = part[2];
    double uu = vrtt[0] * cos(phi_f) + vrtt[1] * sin(phi_f );
    double vv = vrtt[1] * cos(phi_f) - vrtt[0] * sin(phi_f);
    double sr = invR_f * abs(trk->Charge);                // Zero in case of zero charge
    double epsf = -vv - (uu * uu + vv * vv) * sr / 2.;
    double zpf  = vrtt[2] - uu * (1. - vv * sr) / tan(theta_f);
    double phif = phi_f - uu * sr;
    d1 = epsf    - trk->Perig[0];
    d2 = zpf     - trk->Perig[1];
    d3 = theta_f - trk->Perig[2];
    d4 = phif    - trk->Perig[3];
    d5 = invR_f  - trk->Perig[4];
    if(d4 >  M_PI)d4-=2.*M_PI;
    if(d4 < -M_PI)d4+=2.*M_PI;
    double res = trk->WgtM[0] * (d1*d1) 
               + trk->WgtM[2] * (d2*d2) 
               + trk->WgtM[5] * (d3*d3) 
               + trk->WgtM[9] * (d4*d4) 
               + trk->WgtM[14]* (d5*d5) 
            +(d2 *  d1*trk->WgtM[1]
	    + d3 * (d1*trk->WgtM[3]  + d2*trk->WgtM[4])
	    + d4 * (d1*trk->WgtM[6]  + d2*trk->WgtM[7]  + d3*trk->WgtM[8])
	    + d5 * (d1*trk->WgtM[10] + d2*trk->WgtM[11] + d3*trk->WgtM[12] + d4*trk->WgtM[13])) * 2;
    trk->rmnd[0] = d1;
    trk->rmnd[1] = d2;
    trk->rmnd[2] = d3;
    trk->rmnd[3] = d4;
    trk->rmnd[4] = d5;
//std::cout<<__func__<<":"<<d1<<", "<<d2<<", "<<d3<<", "<<d4<<", "<<d5<<'\n';
//std::cout<<trk->Perig[0]<<", "<<trk->Perig[1]<<", "<<trk->Perig[2]<<", "<<trk->Perig[3]<<", "<<trk->Perig[4]<<'\n';
//std::cout<<theta_f<<", "<<phi_f<<", "<<invR_f<<'\n';
//std::cout<<trk->WgtM[0]<<", "<<trk->WgtM[2]<<", "<<trk->WgtM[5]<<", "<<trk->WgtM[9]<<", "<<trk->WgtM[14]<<'\n';
    return  (fabs(res) < 1.e12 ? fabs(res) : 1.e12);
} 




//  
//   Function returns a position of minimum X in parabolic approximation
//         based on 3 measured points (X,Y)
//----------------------------------------------------------------------------
double finter(double y0, double y1, double y2, double x0, double x1, double x2)
{
    double ret_val;
    volatile double b1,b2; // To guarantee AMD==Intel

/* ------------------------*/
/*  Function interpolation */
/* Author: V.Kostyukhin    */
/* ------------------------*/
    b1 = (y1 - y0) / (x1 - x0);
    b2 = (y2 - y0 - b1 * (x2 - x0)) / (x2 - x0) / (x2 - x1);
    if (fabs(b2) < 1e-8) {
	if (y2 <= y0 && y2 < y1) {
	    ret_val = x2;
	} else if (y1 <= y0 && y1 < y2) {
	    ret_val = x1;
	} else if (y0 < y1 && y0 < y2) {
	    ret_val = x0 + (x2 - x0) * .001;
	} else {
	    ret_val = x2;
	}
	return ret_val;
    }
    ret_val = (x1 + x0) - ( b1 / b2 );
//VK 14.08.2007 Check now is after call of this subroutine
//    double NegativeLim = x0 + (x2 - x0) * (-0.3);
//    if (ret_val <= x0) {
//        if (ret_val <= NegativeLim) ret_val = NegativeLim;
//    }
    return ret_val/2.;
} 



//  R=A*S*A'   where A.i.j == A(j,i) -> summation over first index of A 
//   Dimension S(N,N)!!!   OBSOLETE!
//---------------------------------------------------------------------
/*void tdasat(double *a, double *s, double *r__, long int M, long int N)
{

    long int imax, i__, k, ia, mn, ir, is, iaa, ind;
    double sum;
    --r__;
    --s;
    --a;

    imax = (M * M + M) / 2;
    for (i__ = 1; i__ <= imax; ++i__) {r__[i__] = 0.;}
    mn = M * N;
    ind = 0;
    i__ = 0;

L5:
    ind += i__;
    ia = 0;
    ir = 0;

L10:
    is = ind;
    sum = 0.;
    k = 0;

L15:
    if (k > i__)   goto L20;
    ++is;
    goto L30;
L20:
    is += k;
L30:
    ++ia;
    sum += s[is] * a[ia];
    ++k;
    if (k < N)   goto L15;
    iaa = i__ + 1;
L40:
    ++ir;
    r__[ir] += sum * a[iaa];
    iaa += N;
    if (iaa <= ia) goto L40;
    if (ia < mn)   goto L10;

    ++i__;
    if (i__ < N)  goto L5;

    return;
}*/

inline int sIndexVK(int i, int j){ return i>j ? i*(i+1)/2+j : j*(j+1)/2+i;}
//
//  Dimensions  input: CovI(N,N), output: CovF(M,M), input: Der(N,M)-first index runs first
//
void tdasatVK(double *Der, double *CovI, double *CovF, long int M, long int N)
{
   int i,j,k,f;
   for( k=0; k<M; k++) {        // sycle on output index
      for( f=0; f<=k; f++ ) {   //
         double sum=0.;
	 for( i=0; i<N; i++){
           double tmp=0.;
	   for( j=0; j<N; j++) tmp += CovI[sIndexVK(i,j)]*Der[j+f*N];
           sum += Der[i+k*N]*tmp;
         }
	 CovF[sIndexVK(k,f)]=sum;
      }
   }
}

// Zero symmetric matrix and set up diagonal elevents to VALUE
//------------------------------------------------------------
void cfsetdiag(long int n, double *matr, double value)
{
    long int i, j, ij;
    --matr;

    ij = 0;
    for (i = 1; i <= n; ++i) {
	for (j = 1; j <= i; ++j) {
	    ++ij;
	    matr[ij] = 0.;
	    if (i == j) {
		matr[ij] = value;
	    }
	}
    }
} 

// Functions for 2D parabolic fit
//
//   Decomposition of Chi2 function: G(x,y) 
//double Chi2Taylor(double a, double b, double c, double d, double e, double f,
//double x, double y) { return a + b*x + d*y + c*x*x + e*y*y + f*x*y;}


void abcCoef(double g1, double g2, double g3, 
             double &a, double &b, double &c)
/* Function assumes g1(x=0), g2(x=0.5), g3(x=1.) */
{
   a=g1;
   c=2.*(g3-g1) - 4.*(g2-g1);
   b=(g3-g1) - c;
}

void efdCoef(double Ga0, double Gamb, double Gab, double Gw0, double Gwb, 
             double alf, double bet, double w,
	     double &d, double &e, double &f)
/* Function assumes Ga0(x=alf, y=0), Gamb(x=alf, y=-bet), Gab(x=alf, y=+bet),*/
/*                    Gw0(x=w, y=0), Gwb( x=w, y=bet)*/
{ 

   e = (Gab + Gamb - 2.*Ga0)/bet/bet/2.;
   f = (Gwb + Gamb - 2.*e*bet*bet - Gw0 - Ga0)/bet/(w-alf);
   d = (Gab - Ga0 - e*bet*bet - f*alf*bet)/bet;
}


void ParaMin( double b, double c, double d, double e, double f,
              double &xmin, double &ymin)
{
    ymin =  (f*b-2.*c*d)/(4.*c*e - f*f);
    if( fabs(f) > fabs(c) ) {  xmin = - (2.*e*ymin + d)/f;}
    else                    {  xmin = - (f*ymin+b)/(2.*c);}
    }

void cfTrkCovarCorr(double *cov){
//   std::cout<<cov[1]*cov[1]/cov[0]/cov[2]<<'\n';
//   std::cout<<cov[3]*cov[3]/cov[0]/cov[5]<<", "<<cov[4]*cov[4]/cov[2]/cov[5]<<'\n';
//   std::cout<<cov[6]*cov[6]/cov[0]/cov[9]<<", "<<cov[7]*cov[7]/cov[2]/cov[9]<<", "<<cov[8]*cov[8]/cov[5]/cov[9]<<'\n';
//   std::cout<<cov[10]*cov[10]/cov[0]/cov[14]<<", "<<cov[11]*cov[11]/cov[2]/cov[14]<<", "<<cov[12]*cov[12]/cov[5]/cov[14]<<
//        ", "<<cov[13]*cov[13]/cov[9]/cov[14]<<'\n';
   double Lim=0.99; double Lim2=Lim*Lim;
   bool i0,i1,i2,i3,i4; i0=i1=i2=i3=i4=false;
   if( fabs(cov[1]*cov[1])/cov[0]/cov[2] > Lim2 ){ i0=true; i1=true;}
   if( fabs(cov[3]*cov[3])/cov[0]/cov[5] > Lim2 ){ i0=true; i2=true;}
   if( fabs(cov[4]*cov[4])/cov[2]/cov[5] > Lim2 ){ i1=true; i2=true;}
   if( fabs(cov[6]*cov[6])/cov[0]/cov[9] > Lim2 ){ i0=true; i3=true;}
   if( fabs(cov[7]*cov[7])/cov[2]/cov[9] > Lim2 ){ i1=true; i3=true;}
   if( fabs(cov[8]*cov[8])/cov[5]/cov[9] > Lim2 ){ i2=true; i3=true;}
   if( fabs(cov[10]*cov[10])/cov[0]/cov[14] > Lim2 ){ i0=true; i4=true;}
   if( fabs(cov[11]*cov[11])/cov[2]/cov[14] > Lim2 ){ i1=true; i4=true;}
   if( fabs(cov[12]*cov[12])/cov[5]/cov[14] > Lim2 ){ i2=true; i4=true;}
   if( fabs(cov[13]*cov[13])/cov[9]/cov[14] > Lim2 ){ i3=true; i4=true;}

   if(i0){               cov[1]*=Lim;  cov[3]*=Lim;  cov[6]*=Lim;  cov[10]*=Lim; }
   if(i1){ cov[1]*=Lim;                cov[4]*=Lim;  cov[7]*=Lim;  cov[11]*=Lim; }
   if(i2){ cov[3]*=Lim;  cov[4]*=Lim;                cov[8]*=Lim;  cov[12]*=Lim; }
   if(i3){ cov[6]*=Lim;  cov[7]*=Lim;  cov[8]*=Lim;                cov[13]*=Lim; }
   if(i4){ cov[10]*=Lim; cov[11]*=Lim; cov[12]*=Lim; cov[13]*=Lim;               }
}

} /*End of namespace */

