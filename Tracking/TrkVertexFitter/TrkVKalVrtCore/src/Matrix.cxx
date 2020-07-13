/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/TrkVKalUtils.h"
#include <cmath>
#include <exception>
#include <iostream>

namespace Trk {

#define max(a,b) ((a) >= (b) ? (a) : (b))

void vkGetEigVal(double ci[], double d[], int n);

double cfSmallEigenvalue(double *cov,long int n )
{
//    double dest[400], eig[20], eigv[400];
//    long int  i, j, k, ib;
/* -------------------------------*/
/*  Smallest eigenvalue of matrix */
/* -------------------------------*/
//    if (n > 20 || n <= 1) return -1;
//    k = 0;
//    for (i = 1; i <= n; ++i) {
//	ib = (i - 1) * n;
//	for (j = 1; j <= i; ++j) {
//	    dest[ib + j - 1] = cov[k];
//	    dest[(j - 1) * n + i - 1] = cov[k];
//            k++;
//	}
//    }
//    digx(dest, eig, eigv, n, 0);
    
    if (n > 100 || n <= 1) return -1;
    double eig[100];
    vkGetEigVal(cov, eig, n);

    return eig[0];
} 


int cfInv5(double *cov, double *wgt )
{
    extern void dsinv(long int *, double *, long int, long int *);
    double dest[25];
    long int  i, j, k, N;
/* -----------------------------------------*/
/*  Track covariance matrix inversion       */
/*   stable to small value of mom. error    */
/*   Author: V.Kostyukhin                   */
/* -----------------------------------------*/

/* -- RESTORE MATRIX 4x4 -- */
    N = 4;
    k = 0;
    for (i=0; i<N; ++i)  for (j=0; j<=i; ++j) {
	    dest[ i*N + j ] = dest[ j*N + i ] = cov[k]; wgt[k]=0.; k++; }

    double X[4]={cov[10], cov[11], cov[12], cov[13]};

    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) dest[i*N+j] -= X[i]*X[j]/cov[14];
    }
    long int jerr;
    dsinv(&N, dest, N, &jerr);
    if(jerr) return jerr;

    double L[4]={0.,0.,0.,0.};
    for (i = 0; i < N; ++i) {
      for (j = 0; j < N; ++j) L[i] -= dest[i*N+j]*X[j]/cov[14];	
    }
    double W=1.;  for (i = 0; i < N; ++i)  W -= X[i]*L[i]; 
    wgt[10]=L[0]; wgt[11]=L[1]; wgt[12]=L[2]; wgt[13]=L[3]; wgt[14] = W/cov[14];

    k=0;  for (i=0; i<N; i++) for (j=0; j<=i; j++) wgt[k++]=dest[i*N+j];

    return 0;
}



int cfdinv(double *cov, double *wgt, long int NI )
{
    extern void dsinv(long int *, double *, long int, long int *);

    double dest[100]              /* was [10][10] */;
    long int  i, j, k, n, ib;
     
/* ------------------------*/
/*  Matrix inversion       */
/* Author: V.Kostioukhine  */
/* ------------------------*/
    /* Parameter adjustments */
    --wgt;
    --cov;

    /* Function Body */
    n = NI;
    n = n >= 0 ? n : -n;
    if (n > 10 || n <= 1) return -1;
/* -- RESTORE MATRIX */
    k = 0;
    for (i = 1; i <= n; ++i) {
	ib = (i - 1) * n;
	for (j = 1; j <= i; ++j) {
	    ++k;
	    dest[ib + j - 1] = cov[k];
	    dest[(j - 1) * n + i - 1] = cov[k];
	}
    }
/* -- Check */
    if (NI > 0) {
        double add, eig[10] /*,eigv[100]*/;
	//digx(dest, eig, eigv, n, 0);
        vkGetEigVal(cov, eig, n);
	if (eig[0] <= eig[n - 1] * 1.e-12) {
	    add = eig[n - 1] * 1.e-12;
	    if(eig[0]<0) add += fabs(eig[0]) ;
	    for (i = 1; i <= n; ++i) {
		ib = (i - 1) * n + i;
		dest[ib - 1] += add;
	    }
          //std::cout<<" in dsinv="<<n<<", "<<eig[0]<<", "<<eig[n-1]<<'\n';
          //std::cout<<" cov="; for(int ik=1; ik<=n*(n+1)/2; ik++) std::cout<<cov[ik]<<", ";  std::cout<<'\n';
          //if(eig[0]<-1.)std::terminate();
	}
    }
/* -- INVERT */
/* ccc      CALL DINV(N,DEST,N,TMP,JERR) */
    long int jerr;
    dsinv(&n, dest, n, &jerr);
    //if (eig[0]<=eig[n-1]*1.e-12 && NI>0)std::cout<<" in dsinv="<<jerr<<'\n'; 
    if(jerr)return jerr;
/* -- PACK MATRIX */
    k = 0;
    for (i = 1; i <= n; ++i) {
	ib = (i - 1) * n;
	for (j = 1; j <= i; ++j) {
	    ++k;
	    wgt[k] = dest[ib + j - 1];
	}
    }
    return 0;
} 


void  dsinv(long int *n, double *a, long int DIM, long int *ifail)
{
    long int a_dim1, a_offset, i__1, i__2, i__3;
    long int i__, j, k, l;
    double s1, s31, s32, s33;
    long int jm1, jm2, jp1;


    /* Parameter adjustments */
    a_dim1 = DIM;
    a_offset = a_dim1 + 1;
    a -= a_offset;

    /* Function Body */
    jp1 = 0;
    *ifail = 0;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (a[j + j * a_dim1] <= 0.) {
	    goto L150;
	}
	a[j + j * a_dim1] = 1. / a[j + j * a_dim1];
	if (j == *n) {
	    goto L199;
	}
	jp1 = j + 1;
	i__2 = *n;
	for (l = jp1; l <= i__2; ++l) {
	    a[j + l * a_dim1] = a[j + j * a_dim1] * a[l + j * a_dim1];
	    s1 = -a[l + (j + 1) * a_dim1];
	    i__3 = j;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		s1 = a[l + i__ * a_dim1] * a[i__ + (j + 1) * a_dim1] + s1;
	    }
	    a[l + (j + 1) * a_dim1] = -s1;
	}
    }
L150:
    *ifail = -1;
    return;
L199:

    if (*n == 1) {
	goto L399;
    }
    a[(a_dim1 << 1) + 1] = -a[(a_dim1 << 1) + 1];
    a[a_dim1 + 2] = a[(a_dim1 << 1) + 1] * a[(a_dim1 << 1) + 2];
    if (*n == 2) {
	goto L320;
    }
    i__1 = *n;
    for (j = 3; j <= i__1; ++j) {
	jm2 = j - 2;
	i__2 = jm2;
	for (k = 1; k <= i__2; ++k) {
	    s31 = a[k + j * a_dim1];
	    i__3 = jm2;
	    for (i__ = k; i__ <= i__3; ++i__) {
		s31 = a[k + (i__ + 1) * a_dim1] * a[i__ + 1 + j * a_dim1] + 
			s31;
	    }
	    a[k + j * a_dim1] = -s31;
	    a[j + k * a_dim1] = -s31 * a[j + j * a_dim1];
	}
	a[j - 1 + j * a_dim1] = -a[j - 1 + j * a_dim1];
	a[j + (j - 1) * a_dim1] = a[j - 1 + j * a_dim1] * a[j + j * a_dim1];
    }
L320:
    j = 1;
L323:
    s33 = a[j + j * a_dim1];
    if (j == *n)	goto L325;
    jp1 = j + 1;
    i__1 = *n;
    for (i__ = jp1; i__ <= i__1; ++i__) {
	s33 = a[j + i__ * a_dim1] * a[i__ + j * a_dim1] + s33;
    }
L325:
    a[j + j * a_dim1] = s33;
    jm1 = j;
    j = jp1;
    i__1 = jm1;
    for (k = 1; k <= i__1; ++k) {
	s32 = 0.;
	i__2 = *n;
	for (i__ = j; i__ <= i__2; ++i__) {
	    s32 = a[k + i__ * a_dim1] * a[i__ + j * a_dim1] + s32;
	}
	a[k + j * a_dim1] = s32;
	a[j + k * a_dim1] = s32;
    }
    if (j < *n)	goto L323;
L399:
    return;
} 



void scaleg(double *g, double *scale, long int N, long int mfirst)
{

   long int g_dim1, g_offset, i__, j;


#define g_ref(a_1,a_2) g[(a_2)*(g_dim1) + (a_1)]


/* SCALE G MATRIX SO THAT IT'S ELEMENTS ARE ABOUT 1. OF THE ORDER OF */
/* MAGNITUDE. IN THE EQUATION A*X=B CHANGE THE UNITS SO */
/* A(I,J) -> A(I,J)*C(I)*C(J), B(I) -> B(I)*C(I), X(I) -> X(I)/C(I) */
/* MFIRST >= N */
/* Author: V.Kostyukhin */
/*-----------------------------------------------------*/

    /* Parameter adjustments */
    --scale;
    g_dim1 = mfirst;
    g_offset = g_dim1 + 1;
    g -= g_offset;

    /* Function Body */
    double tmp=0.;
    for (i__ = 1; i__ <= N; ++i__) {
	scale[i__] = 1.;
	if (g_ref(i__, i__) == 0.)  			continue;
	tmp = std::sqrt(fabs(g_ref(i__, i__)));
	//scale[i__] = 1./tmp;           g_ref(i__, i__) = d_sign( 1., g_ref(i__, i__)); //VK old version -> diag==1
	scale[i__] = 1./std::sqrt(tmp);   g_ref(i__, i__) = d_sign( tmp, g_ref(i__, i__));  //VK new version -> diag=sqrt(diag_old)
    }

    if (N <= 1) return;
    for (j = 1; j <= N; ++j) {
	for (i__ = 1; i__ <= N; ++i__) {
	    if(j==i__) continue;
	    g_ref(i__, j) *= scale[i__] * scale[j];
	}
    }
} 

#undef g_ref
#undef max

//
//  Solution of A*x = B via LU decomposition
//    A is DESTROYED!!!!
//   Solution X is returned in B
//-------------------------------------------------------------
int vkLUdcmp(double *a, long int n, int *indx)
{
	int i,imax=0,j,k;
	double big,dum,sum,temp;
	double dint=1.0;
        double TINY=1.e-30;
//make indeces start from 1....
        a    -= (n+1);
        indx -= 1;

	double * vv = new double[n+1];


	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if ((temp=fabs(a[i*n+j])) > big) big=temp;
		if (big == 0.0) {delete[] vv; return -1;}
		vv[i]=1.0/big;
	}
	for (j=1;j<=n;j++) {
		for (i=1;i<j;i++) {
			sum=a[i*n+j];
			for (k=1;k<i;k++) sum -= a[i*n+k]*a[k*n+j];
			a[i*n+j]=sum;
		}
		big=0.0;
		for (i=j;i<=n;i++) {
			sum=a[i*n+j];
			for (k=1;k<j;k++)
				sum -= a[i*n+k]*a[k*n+j];
			a[i*n+j]=sum;
			if ( (dum=vv[i]*fabs(sum)) >= big) {
				big=dum;
				imax=i;
			}
		}
		if (j != imax) {
			for (k=1;k<=n;k++) {
				dum=a[imax*n+k];
				a[imax*n+k]=a[j*n+k];
				a[j*n+k]=dum;
			}
			dint = -dint;
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if (a[j*n+j] == 0.0) a[j*n+j]=TINY;
		if (j != n) {
			dum=1.0/(a[j*n+j]);
			for (i=j+1;i<=n;i++) a[i*n+j] *= dum;
		}
	}
	delete[] vv;
        return 0;
}

void vkLUbksb(double *a, long int n, int *indx, double *b)
{
        int i,ii=0,ip,j;
        double sum;
//make indeces start from 1....
        a    -= (n+1);
        b    -= 1;
        indx -= 1;

        for (i=1;i<=n;i++) {
                ip=indx[i];
                sum=b[ip];
                b[ip]=b[i];
                if (ii)
                        for (j=ii;j<=i-1;j++) sum -= a[i*n+j]*b[j];
                else if (sum) ii=i;
                b[i]=sum;
        }
        for (i=n;i>=1;i--) {
                sum=b[i];
                for (j=i+1;j<=n;j++) sum -= a[i*n+j]*b[j];
                b[i]=sum/a[i*n+i];
        }
}
//
//  Solve linear equation with LU decomposition.
//  Matrix (*a) left decomposed
//
int vkMSolve(double *a, double *b, long int n, double *ainv=nullptr)
{   
   int *indx=new int[n+1]; double * Scale=new double[n]; 
   scaleg(a,Scale,n,n);
   int ierr = vkLUdcmp( a, n, indx);
   if( ierr ){delete[] indx; delete[] Scale; return -1;}
   for(int i=0;i<n;i++)b[i]*=Scale[i];
   vkLUbksb(a, n, indx, b);
   for(int i=0;i<n;i++)b[i]*=Scale[i];
   if(ainv){  //Also invert matrix on request
     int i,j;
     double *tmp =new double[n];
     for(j=0; j<n; j++){ 
       for(i=0;i<n;i++)tmp[i]=0.;
       tmp[j]=1.;
       vkLUbksb(a, n, indx, tmp);
       for(i=0;i<n;i++)ainv[i+j*n]=tmp[i];
     }
     delete[] tmp;
     for(i=0;i<n;i++) for(j=0;j<n;j++) ainv[i+j*n] *= Scale[i]*Scale[j];
   }
   delete[] indx; delete[] Scale;
   return 0;
}


//
// SVD method 
//

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

double vkPythag(double a, double b)
{
	double absa,absb;
	absa=fabs(a);
	absb=fabs(b);
	if (absa > absb) return absa*std::sqrt(1.0+(absb/absa)*absb/absa);
	return (absb == 0.0 ? 0.0 : absb*std::sqrt(1.0+(absa/absb)*absa/absb));
}

void vkSVDCmp(double **a, int m, int n, double w[], double **v)
{
	int flag,i,its,j,jj,k,l=0, nm=0;
	double anorm,c,f,g,h,s,scale,x,y,z,*rv1;

	rv1=new double[n+1]; 
	g=scale=anorm=0.0;
	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += fabs(a[k][i]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f=a[i][i];
				g = -SIGN(std::sqrt(s),f);
				h=f*g-s;
				a[i][i]=f-g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
					f=s/h;
					for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
				}
				for (k=i;k<=m;k++) a[k][i] *= scale;
			}
		}
		w[i]=scale *g;
		g=s=scale=0.0;
		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += fabs(a[i][k]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g = -SIGN(std::sqrt(s),f);
				h=f*g-s;
				a[i][l]=f-g;
				for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
					for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
				}
				for (k=l;k<=n;k++) a[i][k] *= scale;
			}
		}
	        double nanorm=fabs(w[i])+fabs(rv1[i]);
		if(anorm < nanorm) anorm = nanorm;
	}
	for (i=n;i>=1;i--) {
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++)
					v[j][i]=(a[i][j]/a[i][l])/g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
					for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
				}
			}
			for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}
	for (i=(m<n?m:n);i>=1;i--) {
		l=i+1;
		g=w[i];
		for (j=l;j<=n;j++) a[i][j]=0.0;
		if (g) {
			g=1.0/g;
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
				f=(s/a[i][i])*g;
				for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
			}
			for (j=i;j<=m;j++) a[j][i] *= g;
		} else for (j=i;j<=m;j++) a[j][i]=0.0;
		++a[i][i];
	}
	for (k=n;k>=1;k--) {
		for (its=1;its<=300;its++) {
			flag=1;
			for (l=k;l>=1;l--) {
				nm=l-1;
				if ( (fabs(rv1[l])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ( (fabs(w[nm])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if ( (fabs(f)+anorm) == anorm) break;
					g=w[i];
					h=vkPythag(f,g);
					w[i]=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=1;j<=m;j++) {
						y=a[j][nm];
						z=a[j][i];
						a[j][nm]=y*c+z*s;
						a[j][i]=z*c-y*s;
					}
				}
			}
			z=w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j=1;j<=n;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			x=w[l];
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=vkPythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=vkPythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=1;jj<=n;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=vkPythag(f,h);
				w[j]=z;
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=1;jj<=m;jj++) {
					y=a[jj][j];
					z=a[jj][i];
					a[jj][j]=y*c+z*s;
					a[jj][i]=z*c-y*s;
				}
			}
			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
	delete[] rv1;
}


//Invert square matrix using SVD + solve linear equation if needed
// Doesn't work for the moment!!!
//
int vkInvSVD(double *ci,long int DIM, double *co, double Chk, double *bvect=nullptr)
{
int i,j,k;
double **a   = new double*[DIM+1]; double  *ab   = new double[(DIM+1)*(DIM+1)];
double **v   = new double*[DIM+1]; double  *vb   = new double[(DIM+1)*(DIM+1)];
double **res = new double*[DIM+1]; double  *resb = new double[(DIM+1)*(DIM+1)];
for(i=0; i<DIM+1; i++){   a[i] = &ab[i*(DIM+1)];
    			  v[i] = &vb[i*(DIM+1)];
  			res[i] = &resb[i*(DIM+1)];}
double  *w = new double[DIM+1];

for( i=1; i<=DIM; i++) for( j=i; j<=DIM; j++)  a[i][j]=ci[(j-1)*DIM+i-1];

  vkSVDCmp ( a, DIM, DIM, w, v);

// Singular value limitation
double svMax=0.; for(k=1; k<=DIM; k++) if( svMax < w[k] ) svMax=w[k];
for(k=1; k<=DIM; k++){  /*std::cout<<w[k]<<'\n';*/  if( w[k]<0. || fabs(w[k]/svMax)<Chk) w[k]=0.; }
// Get inverse matrix
for(i=1; i<=DIM; i++){
  for(j=1; j<=DIM; j++){
    double t=0.;
    for(k=1; k<=DIM; k++) {
      if( w[k] != 0 ) t += a[i][k]*v[j][k]/w[k];
    }
    res[i][j]=t;
  }
} 
//
// Get output matrix 
for( i=1; i<=DIM; i++)  for( j=1; j<=DIM; j++) { co[(j-1)*DIM+i-1]= res[i][j]; }

if(bvect){
  double  *rside = new double[DIM+1];
  for( i=1; i<=DIM; i++) rside[i]=bvect[i-1]; 
  //----
  for( i=1; i<=DIM; i++)  {
    double sum=0.;
    for( j=1; j<=DIM; j++) sum += res[i][j]*rside[j];
    bvect[i-1]=sum;
  }
  delete[] rside;
}

//Cleaning
delete[] w; delete[] a; delete[] ab; delete[] v; delete[] vb; delete[] res; delete[] resb;

return 0;
}


//Invert packed symmetric matrix using SVD
//
int vkSInvSVD(double *ci,long int DIM, double *co, double Chk)
{
int i,j,k;
double **a   = new double*[DIM+1]; double  *ab   = new double[(DIM+1)*(DIM+1)];
double **v   = new double*[DIM+1]; double  *vb   = new double[(DIM+1)*(DIM+1)];
double **res = new double*[DIM+1]; double  *resb = new double[(DIM+1)*(DIM+1)];
for(i=0; i<DIM+1; i++){   a[i] = &ab[i*(DIM+1)];
    			  v[i] = &vb[i*(DIM+1)];
  			res[i] = &resb[i*(DIM+1)];}
double  *w = new double[DIM+1];

for( i=1; i<=DIM; i++) {
   for( j=i; j<=DIM; j++) { k=(j-1)*j/2 + i; a[i][j]=a[j][i]=ci[k-1];}
}

  vkSVDCmp ( a, DIM, DIM, w, v);

// Singular value limitation
double svMax=0.; for(k=1; k<=DIM; k++) if( svMax < w[k] ) svMax=w[k];
for(k=1; k<=DIM; k++){  /*std::cout<<w[k]<<'\n';*/  if( w[k]<0. || fabs(w[k]/svMax)<Chk) w[k]=0.; }
// Get inverse matrix
for(i=1; i<=DIM; i++){
  for(j=1; j<=DIM; j++){
    double t=0.;
    for(k=1; k<=DIM; k++) {
      if( w[k] != 0 ) t += a[i][k]*v[j][k]/w[k];
    }
    res[i][j]=t;
  }
} 
//
// Get output matrix in symmetric way
k=0;
for( i=1; i<=DIM; i++)  for( j=1; j<=i; j++) { co[k]= res[j][i]; k++; }
//Cleaning
delete[] w; delete[] a; delete[] ab; delete[] v; delete[] vb; delete[] res; delete[] resb;

return 0;
}

#define ROTATE(a,i,j,k,l) g=(a)[i][j];h=(a)[k][l];(a)[i][j]=g-s*(h+g*tau);\
	(a)[k][l]=h+s*(g-h*tau);

int vkjacobi(double **a, int n, double d[], double **v)
{
        bool getEVect= true; if( v==nullptr )getEVect=false;
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c;

	double *b=new double[n+1];
	double *z=new double[n+1];
	if(getEVect){
	  for (ip=1;ip<=n;ip++) {
		for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
		v[ip][ip]=1.0;
	  }
        }
	for (ip=1;ip<=n;ip++) {
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0;
	}
	int nrot=0;
	for (i=1;i<=1000;i++) {
		sm=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++)
				sm += fabs(a[ip][iq]);
		}
		if (sm == 0.0) {
			delete[] z;
			delete[] b;
			return nrot;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n);
		else
			tresh=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++) {
				g=100.0*fabs(a[ip][iq]);
				if (i > 4 && (double)(fabs(d[ip])+g) == (double)fabs(d[ip])
					&& (double)(fabs(d[iq])+g) == (double)fabs(d[iq]))
					a[ip][iq]=0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if ((double)(fabs(h)+g) == (double)fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5*h/(a[ip][iq]);
						t=1.0/(fabs(theta)+std::sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/std::sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0;
					for (j=1;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<=n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
	                                if(getEVect){
					  for (j=1;j<=n;j++) {
					  	ROTATE(v,j,ip,j,iq)
					  }
	                                }
					++nrot;
				}
			}
		}
		for (ip=1;ip<=n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0.0;
		}
	}
	delete[] z;
	delete[] b;
        return 0;   //not enough sweeps
}
#undef ROTATE
//-------------------------------------------
// ci - symmetric matrix in compact form
//-------------------------------------------
void vkGetEigVal(double ci[], double d[], int n)
{
  int i,j,k; d--;
  double **a = new double*[n+1]; double  *ab = new double[(n+1)*(n+1)]; 
  for(i=0; i<n+1; i++) a[i] =&ab[i*(n+1)];

  for( i=1; i<=n; i++) {
    for( j=i; j<=n; j++) { k=(j-1)*j/2 + i; a[i][j]=a[j][i]=ci[k-1];}
  }

  vkjacobi(a,n,d,nullptr);

  for (i=1;i<n;i++) {
     double p=d[k=i]; for (j=i+1;j<=n;j++) if (d[j] < p) p=d[k=j];
     if (k != i) { d[k]=d[i]; d[i]=p; }
  }

  delete[] a; delete[] ab;
}

void vkGetEigVect(double ci[], double d[], double vect[], int n)
{
  int i,j,k; d--;
  double **a = new double*[n+1]; double  *ab = new double[(n+1)*(n+1)]; 
  double **v = new double*[n+1]; double  *vb = new double[(n+1)*(n+1)]; 
  for(i=0; i<n+1; i++) a[i] =&ab[i*(n+1)];
  for(i=0; i<n+1; i++) v[i] =&vb[i*(n+1)];

  for( i=1; i<=n; i++) {
    for( j=i; j<=n; j++) { k=(j-1)*j/2 + i; a[i][j]=a[j][i]=ci[k-1];}
  }

  vkjacobi(a,n,d,v);

  for (i=1;i<n;i++) {
     double p=d[k=i]; for (j=i+1;j<=n;j++) if (d[j] < p) p=d[k=j];
     if (k != i) { d[k]=d[i]; d[i]=p;
   	           for (j=1;j<=n;j++) { p=v[j][i]; v[j][i]=v[j][k]; v[j][k]=p;}
     }
  }

  k=0; for (i=1;i<=n;i++) for(j=1;j<=n;j++) vect[k++]=v[j][i];

  delete[] a; delete[] ab;
  delete[] v; delete[] vb;
}

}  /* end of VKalVrtCore namespace */
