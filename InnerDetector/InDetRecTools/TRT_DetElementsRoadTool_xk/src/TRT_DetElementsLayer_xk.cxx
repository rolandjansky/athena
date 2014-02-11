/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DetElemenstLayerLxk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <utility>
#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsLayer_xk.h"

///////////////////////////////////////////////////////////////////
// Set detector elements layer
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsLayer_xk::set
(double r,double dr,double z,double dz,double df,double wf,double wz)
{
  m_r   = float(r ) ;
  m_dr  = float(dr) ;
  m_z   = float(z ) ;
  m_dz  = float(dz) ;
  m_dfe = float(df) ;
  m_wf  = float(wf) ;
  m_wz  = float(wz) ;
  m_f0  = m_elements[0].phi();
  m_sfi = 0.        ;
  if(m_elements.size()<=1) return;
  m_sfi = m_elements[1].phi()-m_elements[0].phi();
  if(m_sfi < .001) m_sfi = m_elements[2].phi()-m_elements[0].phi();
  m_sfi = 1./m_sfi;
} 

///////////////////////////////////////////////////////////////////
// Get barrel detector elements for Atlas geometry 
// Input parameters: P[0] - X     A[0] - Ax 
//                   P[1] - Y     A[1] - Ay
//                   P[2] - Z     A[2] - Az
//                   P[3] - R
//                   P[4] - width
//                   P[5] - step
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsLayer_xk::getBarrelDetElementsATL
(float* P ,float* A,std::list<InDet::TRT_DetElementLink_xk*>& lDE)
{
  float a    = (A[0]*P[0]+A[1]*P[1])*2.; 
  float s    = 0. ;
  if(a!=0.) {
    float d  = (m_r-P[0]-P[1])*(m_r+P[0]+P[1])+2.*P[0]*P[1];
    float b  = 2.*(A[0]*A[0]+A[1]*A[1]);
    float sq = a*a+2.*d*b;  sq>0. ? sq=sqrt(sq) : sq=0.;
    float s1 =-(a+sq)/b;
    float s2 =-(a-sq)/b; 

    if((s1*s2) > 0.) {fabs(s1) < fabs(s2) ? s = s1 : s = s2;}
    else             {     s1  > 0.       ? s = s1 : s = s2;}  

  }
  float zc  = P[2]+A[2]*s ;  if(fabs(zc-m_z)>(m_dz+P[4])) return;
  float zc0 = zc-P[4]-m_z ;
  float zc1 = zc+P[4]-m_z ;
  float yc  = P[1]+A[1]*s ;
  float xc  = P[0]+A[0]*s ; s+=P[5];
  float fc  = atan2(yc,xc);
  int m     = m_elements.size()-2;
  int n     = int((fc-m_f0)*m_sfi)*2; if(n<0) n=0; else if(n>m) n=m;

  float dF = fc-m_elements[n].phi();
  float dx = fabs(yc*m_elements[n].cos()-xc*m_elements[n].sin()-m_elements[n].centerf());
  if((dx-m_wf-P[4]) <= 0.) {

    if(zc0 <= 0 && !m_elements[n].used()) {
      lDE.push_back(&m_elements[n]); m_elements[n].setUsed(s);
    }

    int k = n+1;
    if(zc1 >= 0. && !m_elements[k].used() ) {
      lDE.push_back(&m_elements[k]); m_elements[k].setUsed(s);
    }
  }
  if(dF>0.) { n!=62 ? n+=2 : n=0 ;}
  else      { n!= 0 ? n-=2 : n=62;}

  dx = fabs(yc*m_elements[n].cos()-xc*m_elements[n].sin()-m_elements[n].centerf());
  if((dx-m_wf-P[4]) <= 0.) {

    if(zc0 <= 0. && !m_elements[n].used()) {
      lDE.push_back(&m_elements[n]); m_elements[n].setUsed(s);
    }

    int k = n+1;
    if(zc1 >= 0. && !m_elements[k].used()) {
      lDE.push_back(&m_elements[k]); m_elements[k].setUsed(s);
    }
  }
}

///////////////////////////////////////////////////////////////////
// Get barrel detector elements for CTB geometry 
// Input parameters: P[0] - X     A[0] - Ax 
//                   P[1] - Y     A[1] - Ay
//                   P[2] - Z     A[2] - Az
//                   P[3] - R
//                   P[4] - width
//                   P[5] - step
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsLayer_xk::getBarrelDetElementsCTB
(float* P ,float* A,std::list<InDet::TRT_DetElementLink_xk*>& lDE)
{
  float a    = (A[0]*P[0]+A[1]*P[1])*2.; 
  float s    = 0. ;
  if(a!=0.) {
    float d  = (m_r-P[0]-P[1])*(m_r+P[0]+P[1])+2.*P[0]*P[1];
    float b  = 2.*(A[0]*A[0]+A[1]*A[1]);
    float sq = a*a+2.*d*b;  sq>0. ? sq=sqrt(sq) : sq=0.;
    float s1 =-(a+sq)/b;
    float s2 =-(a-sq)/b; 

    if((s1*s2) > 0.) {fabs(s1) < fabs(s2) ? s = s1 : s = s2;}
    else             {     s1  > 0.       ? s = s1 : s = s2;}  

  }
  float zc  = P[2]+A[2]*s ;  if(fabs(zc-m_z)>(m_dz+P[4])) return;
  float zc0 = zc-P[4]-m_z ;
  float zc1 = zc+P[4]-m_z ;
  float yc  = P[1]+A[1]*s ;
  float xc  = P[0]+A[0]*s ; s+=P[5];
  
  for(size_t n = 0; n < m_elements.size(); n+=2) {

    float dx = fabs(yc*m_elements[n].cos()-xc*m_elements[n].sin()-m_elements[n].centerf());
    if((dx-m_wf-P[4]) <= 0.) {

      if(zc0 <= 0. &&  !m_elements[n].used()) {
	lDE.push_back(&m_elements[n]); m_elements[n].setUsed(s);
      }

      if(zc1 >= 0. && !m_elements[n+1].used()) {

	lDE.push_back(&m_elements[n+1]); m_elements[n+1].setUsed(s);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Get endcap detector elements
// Input parameters: P[0] - X     A[0] - Ax 
//                   P[1] - Y     A[1] - Ay
//                   P[2] - Z     A[2] - Az
//                   P[3] - R
//                   P[4] - width
//                   P[5] - step
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsLayer_xk::getEndcapDetElements
(float* P ,float* A,std::list<InDet::TRT_DetElementLink_xk*>& lDE)
{
  const float pi2=2.*M_PI;

  float s   = (m_z-P[2])/A[2];
  float xc  = P[0]+A[0]*s;
  float yc  = P[1]+A[1]*s; s+=P[5]; 
  float rc  = sqrt(xc*xc+yc*yc); if(fabs(rc-m_r)>m_dr+P[4]) return;
  float fc  = atan2(yc,xc);
  float sf  = 0.09817477+P[4]/rc;

  int m     = m_elements.size()-1;
  int n     = int((fc-m_f0)*m_sfi); if(n<0) n=0; else if(n>m) n=m;
  float dF  = fc-m_elements[n].phi();

  if(fabs(dF)                     < sf && !m_elements[n].used()) {
    lDE.push_back(&m_elements[n]); m_elements[n].setUsed(s);
  }

  //  if(dF>0.) {if(n!=63) ++n; else {n=0 ; fc-=pi2;}}
  //  else      {if(n!=0 ) --n; else {n=63; fc+=pi2;}}
  if(dF>0.) {if(n!=m) ++n; else {n=0 ; fc-=pi2;}}
  else      {if(n!=0) --n; else {n=m ; fc+=pi2;}}
  
  if(fabs(fc-m_elements[n].phi()) < sf && !m_elements[n].used()) {
    lDE.push_back(&m_elements[n]); m_elements[n].setUsed(s);
  }
}
