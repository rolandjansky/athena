/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SiDetElemenstLauerLxk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiDetElementsRoadTool_xk/SiDetElementsComparison.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayer_xk.h"

///////////////////////////////////////////////////////////////////
// Get barrel detector elements
// Input parameters: P[0] - X     A[0] - Ax 
//                   P[1] - Y     A[1] - Ay
//                   P[2] - Z     A[2] - Az
//                   P[3] - R
//                   P[4] - width
//                   P[5] - step
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsLayer_xk::getBarrelDetElements
(float* P ,float* A,std::vector<InDet::SiDetElementLink_xk*>& lDE)
{
  float a  = (A[0]*P[0]+A[1]*P[1])*2.; 
  float d  = (m_r-P[0]-P[1])*(m_r+P[0]+P[1])+2.*P[0]*P[1];
  float b  = 2.*(A[0]*A[0]+A[1]*A[1]); if(b == 0.) return;
  float sq = a*a+2.*d*b;  sq>0. ? sq=sqrt(sq) : sq=0.;
  float s1 =-(a+sq)/b;
  float s2 =-(a-sq)/b; 
  float s;
  if((s1*s2) > 0.) {fabs(s1) < fabs(s2) ? s = s1 : s = s2;}
  else             {     s1  > 0.       ? s = s1 : s = s2;}  
  float zc   = P[2]+A[2]*s;
  float At   = sqrt(1.-A[2]*A[2]);
  if(At != 0. && fabs(zc-m_z) > (m_dz+(m_dr*fabs(A[2])+P[4])/At)) return;
  float fc   = atan2(P[1]+A[1]*s,P[0]+A[0]*s);
  float dw   = P[4]/m_r;
  getDetElements(P,A,fc,dw,lDE);
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

void InDet::SiDetElementsLayer_xk::getEndcapDetElements
(float* P ,float* A,std::vector<InDet::SiDetElementLink_xk*>& lDE)
{
  float s   =(m_z-P[2])/A[2];
  float xc  = P[0]+A[0]*s;
  float yc  = P[1]+A[1]*s;
  float rc  = sqrt(xc*xc+yc*yc);
  float A23 = A[2]*P[3];
  if(A23 != 0. && fabs(rc-m_r) > m_dr+fabs(2.*(P[0]*A[0]+P[1]*A[1])*m_dz/A23)+P[4]) return;
  float fc  = atan2(yc,xc);
  float dw  = P[4]/rc;
  getDetElements(P,A,fc,dw,lDE);
}

///////////////////////////////////////////////////////////////////
// Get detector elements
// Input parameters: P[0] - X     A[0] - Ax 
//                   P[1] - Y     A[1] - Ay
//                   P[2] - Z     A[2] - Az
//                   P[3] - R
//                   P[4] - width
//                   P[5] - step
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsLayer_xk::getDetElements
(float* P,float* A,float Fc,float dW,std::vector<InDet::SiDetElementLink_xk*>& lDE)
{
  const float pi = M_PI, pi2 = 2.*pi; 
  int im  = int(m_elements.size())-1; if(im<0) return;
  int i0  = 0, i1  = im;

  if (Fc> m_elements[i0].phi() && Fc< m_elements[i1].phi()) {
    while((i1-i0)>1) {int i = ((i0+i1)>>1); m_elements[i].phi() > Fc ? i1=i : i0=i;}
    i0 = i1;
  }
  float O[3];
  int i = i0;
  while(1) {

    if(!m_elements[i].used()) {

      float dF =fabs(m_elements[i].phi()-Fc); if(dF>pi) dF=fabs(dF-pi2);
      if((dF-dW)>m_dfe) break;
      m_elements[i].intersect(P,A,O);

      if((O[0]-P[4])<=0 && (O[1]-P[4])<=0.) {
	lDE.push_back(&m_elements[i]); m_elements[i].setUsed(P[5]+O[2]);
      }
    }
    ++i; if(i>im) i=0; if(i==i0) return;
  }
  i1 = i; i = i0;
  while(1) {

    --i; if(i<0) i=im; if(i==i1) return;

    if(!m_elements[i].used()) {

      float dF =fabs(m_elements[i].phi()-Fc); if(dF>pi) dF=fabs(dF-pi2);
      if((dF-dW)>m_dfe) return;
      m_elements[i].intersect(P,A,O);
      
      if((O[0]-P[4])<=0 && (O[1]-P[4])<=0.) {
	lDE.push_back(&m_elements[i]); m_elements[i].setUsed(P[5]+O[2]);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Sort detector elements in azimuthal angle order
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsLayer_xk::sortDetectorElements()
{
  std::sort(m_elements.begin(),m_elements.end(),InDet::compDetElements_Azimuthal());
}

