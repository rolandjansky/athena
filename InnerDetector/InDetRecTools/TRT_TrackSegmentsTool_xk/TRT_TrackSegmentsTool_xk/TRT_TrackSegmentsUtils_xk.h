/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrackSegmentsUtils_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for straight line finder
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackSegmentsUtils_xk_H
#define TRT_TrackSegmentsUtils_xk_H

#include <new>
#include <cmath>
#include <algorithm>

namespace InDet{

  class TRT_TrackSegmentsUtils_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      TRT_TrackSegmentsUtils_xk();
      ~TRT_TrackSegmentsUtils_xk();

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const int& size () const {return m_size;}
      void  initiate(int);
      template<class T> void findStraightLine(int,T*,const T*,const T*);
      template<class T> void sort(T*,int*,int n);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      int          m_size;
      int*         m_NA  ;
      int*         m_MA  ;
      double*      m_F   ;

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline TRT_TrackSegmentsUtils_xk::TRT_TrackSegmentsUtils_xk()
    {
      m_size = 0;
      m_NA   = 0;
      m_MA   = 0;
      m_F    = 0;
    }

  inline void TRT_TrackSegmentsUtils_xk::initiate(int n)
    {
      if(n<=0) return;

      if(m_size) {

	delete[] m_NA;
	delete[] m_MA;
	delete[] m_F ;
      }
      m_size = n;
      m_NA   = new int   [n];
      m_MA   = new int   [n];
      m_F    = new double[n];
    }

  inline TRT_TrackSegmentsUtils_xk::~TRT_TrackSegmentsUtils_xk() 
    {
      if(!m_size) return;
      delete[] m_NA;
      delete[] m_MA;
      delete[] m_F ;
    }
  
  ///////////////////////////////////////////////////////////////////
  // Search a stright line Y = A*X+B crossed max number segments                 
  // Input parameters:  X        - X-coordinates of the segment boundaries       
  //                    Y        - Y-coordinates   ------
  //                    par[0]   - initial A
  //                    par[1]   - initial B                        
  //                    par[2]   - boundary of A   abs(A)<DA                     
  //                                                                             
  // Output parameters : A,B - stright line parameters Y = A*X+B                 
  // Program try to find a stright line which cross a segments a  such a way that
  // total number of all crossed segments to get max value                          
  ///////////////////////////////////////////////////////////////////
  
  template<class Tem> void TRT_TrackSegmentsUtils_xk::findStraightLine
    (int Np, Tem* par, const Tem* X, const Tem* Y)
    {
      if(m_size<2 || Np<2 ) return;
      if(Np>m_size) Np = m_size;
 
      Tem A    =      par[0];
      Tem B    =      par[1];
      Tem Amax = fabs(par[2]);
      
      int    i=0; 
      while(i!=Np) {
	m_NA[i]=i; m_F[i]=Y[i]; ++i; 
	m_NA[i]=i; m_F[i]=Y[i]; ++i;
      }
      
      sort(m_F,m_NA,Np);
      
      // Search B with A = par[0];
      //
      i=0; int sm=0, s=0, l=0;
      while(i!=Np) {
	if((m_NA[i  ]&1)==(m_MA[i  ]=0)) {if(++s>sm) {sm=s; l=  i;}} else --s; 
	if((m_NA[i+1]&1)==(m_MA[i+1]=0)) {if(++s>sm) {sm=s; l=1+i;}} else --s; i+=2;
      } 
      B = .5*(Y[m_NA[l]]+Y[m_NA[l+1]]);
      
      // Search A and B
      //
      while(1) {
	
	i=(l=m_NA[l]); Tem u0=X[l], v0=Y[l]; 
	
	// Vector slopes preparation
	//
	
	while(++i!=Np)  {if(X[i]!=u0) break;} Tem U1=-1000., d=0.; int m=0;
	while(i<Np-1) {
	  
	  if (X[i]==U1) {
	    if(fabs(m_F[m]=(Y[i  ]-v0)*d)<Amax) m_NA[m++]=i;
	    if(fabs(m_F[m]=(Y[i+1]-v0)*d)<Amax) m_NA[m++]=i+1;
	  }
	  else          {
	    d=1./((U1=X[i])-u0); 
	    if(fabs(m_F[m]=(Y[i  ]-v0)*d)<Amax) m_NA[m++]=i;
	    if(fabs(m_F[m]=(Y[i+1]-v0)*d)<Amax) m_NA[m++]=i+1;
	  }
	  i+=2;
	} 
	(i=l);  while(--i>0)  {if(X[i]!=u0) break;} U1=-1000.;
	while(i>0) {
	  if (X[i]==U1) {
	    if(fabs(m_F[m]=(Y[i  ]-v0)*d)<Amax) m_NA[m++]=i;
	    if(fabs(m_F[m]=(Y[i-1]-v0)*d)<Amax) m_NA[m++]=i-1;
	    
	  }
	  else          {
	    d=1./((U1=X[i])-u0); 
	    if(fabs(m_F[m]=(Y[i  ]-v0)*d)<Amax) m_NA[m++]=i;
	    if(fabs(m_F[m]=(Y[i-1]-v0)*d)<Amax) m_NA[m++]=i-1;
	  }
	  i-=2;
	} 
    
	if(m<=4) break;
        sort(m_F,m_NA,m);

	int   nm = 0; s = 0; sm=-1000;
	for(int i=0; i!=m-1; ++i) {
	  int na = m_NA[i];
	  if((na&1)==0) {if(na>l) {if(++s>sm) {sm=s; nm=i;}} else --s;}
	  else          {if(na<l) {if(++s>sm) {sm=s; nm=i;}} else --s;}
	}
	if(nm==0) break;
	B = v0-(A=.5*(m_F[nm]+m_F[nm+1]))*u0;  m_MA[l]=1;
	if     (m_MA[m_NA[nm  ]]==0) l = nm;
	else if(m_MA[m_NA[nm+1]]==0) l = nm+1; 
	else    break;
      }
      par[0] = A;
      par[1] = B;
    }

  ///////////////////////////////////////////////////////////////////
  // The procedure sorts the elements of the array A(1:N)                          
  // into ascending order. Algorithm 271, Comm., ACM(1965) p.669                   
  //                                                                               
  // Input parameters  : a    - one-dimension array of elements                    
  //                            to be sorted                                       
  //                     b    - one-dimension array of elements                    
  //                            to be sorted together with a                       
  //                     n    - number of words to be sorted                       
  ///////////////////////////////////////////////////////////////////
  
  template<class Tem> void TRT_TrackSegmentsUtils_xk::sort(Tem* a,int* b,int n) {

    if(n<=1) return; 
    int mt[50],lt[50], j=n-1, i=0, m=0;
    while (1) {
      if(j-i>1) {
	int ip=(j+i)>>1, q=j; Tem ta=a[ip]; a[ip]=a[i]; int tb=b[ip]; b[ip]=b[i];
	for(int k =i+1; k<=q; ++k) {
	  if(a[k]<=ta) continue;
          int l=q;
	  while(1) {
	    if(a[l]<ta) {
	      Tem x=a[k]; a[k]=a[l]; a[l]=x;
	      int y=b[k]; b[k]=b[l]; b[l]=y; q=l-1; break;
	    }
	    if(--l< k) {q=k-1; break;}
	  }
	}
	a[i]=a[q]; a[q]=ta; b[i]=b[q]; b[q]=tb;
	if((q<<1)>i+j) {lt[m]=i;   mt[m++]=q-1; i=q+1;}
	else           {lt[m]=q+1; mt[m++]=j;   j=q-1;} 
      }
      else      {
	if(i<j && a[i]>a[j]) {
	  Tem  x=a[i]; a[i]=a[j]; a[j]=x;
	  int  y=b[i]; b[i]=b[j]; b[j]=y; 
	}
	if(--m<0) return;
        i=lt[m]; j=mt[m];
      }
    }
  }
} // end of name space

#endif // TRT_TrackSegmentsUtils_xk
