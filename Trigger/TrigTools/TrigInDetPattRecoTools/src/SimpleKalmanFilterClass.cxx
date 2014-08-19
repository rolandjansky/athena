/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/SimpleKalmanFilterClass.h"

#include <iostream>
#include <cmath>
#include <cstring>
#include <list>
#include <algorithm>
#include <map>

void SimpleKalmanFilterClass::newTrack(class TrigL2TrackCandidate* pTrack) {

  double theta=2.0*atan(exp(-pTrack->m_params.m_eta));

  m_Rk[0]=0.0;
  m_Rk[1]=pTrack->m_params.m_z0;
  m_Rk[2]=0.0;
  m_Rk[3]=cos(theta)/sin(theta);
  m_Rk[4]=1/pTrack->m_params.m_pT;

  memset(&m_Gk[0][0],0,sizeof(m_Gk));
  m_Gk[0][0]=100.0;
  m_Gk[1][1]=100.0;
  m_Gk[2][2]=100.0;
  m_Gk[3][3]=100.0;
  m_Gk[4][4]=0.01; 

  double minPhi=(*pTrack->spacePoints().begin())->phi();
  m_averagePhi0=0.0;

  for(std::vector<const TrigSiSpacePointBase *>::const_iterator spIt=pTrack->spacePoints().begin();
      spIt!=pTrack->spacePoints().end();spIt++) {
    if(minPhi>(*spIt)->phi()) 
      minPhi=(*spIt)->phi();
  }
  int nHits=0;
  for(std::vector<const TrigSiSpacePointBase *>::const_iterator spIt=pTrack->spacePoints().begin();
      spIt!=pTrack->spacePoints().end();spIt++) {
    const TrigSiSpacePointBase* pSP=(*spIt);
    nHits++;
    double delta=pSP->phi()-minPhi;
    if(fabs(delta)>M_PI) delta-=2*M_PI;
    m_averagePhi0+=delta;
  }

  m_averagePhi0/=nHits;m_averagePhi0+=minPhi;

#ifdef IDSCAN_DEBUG
  printf("Average PHI0=%f Initial Pt=%f\n",m_averagePhi0,pTrack->m_params.m_pT);
#endif
}


double SimpleKalmanFilterClass::getChi2(const TrigSiSpacePointBase* pSP) {

  int i,j,m;

  m_x=pSP->r()*cos(pSP->phi()-m_averagePhi0);
  m_y=pSP->r()*sin(pSP->phi()-m_averagePhi0);

#ifdef IDSCAN_DEBUG
  printf("x=%f y=%f z=%f\n",m_x,m_y,pSP->z());
#endif

  m_Hk[0][0]=1.0;m_Hk[0][1]=0.0;
  m_Hk[0][2]=m_x;
  m_Hk[0][3]=0.0;m_Hk[0][4]=-0.5*m_x*m_x*m_fConst;
  m_Hk[1][0]=0.0;m_Hk[1][1]=1.0;
  m_Hk[1][2]=0.0;m_Hk[1][3]=m_x;m_Hk[1][4]=0.0;
  
  for(i=0;i<5;i++)
    for(j=0;j<2;j++) {
      m_Bk[i][j]=0.0;for(m=0;m<5;m++) m_Bk[i][j]+=m_Gk[i][m]*m_Hk[j][m];
    }
  for(i=0;i<2;i++)
    for(j=0;j<2;j++) {
      m_Vk[i][j]=0.0;for(m=0;m<5;m++) m_Vk[i][j]+=m_Hk[i][m]*m_Bk[m][j];
    }
	  
  if(pSP->layer()<OffsetEndcapPixels) {

    m_sigma[0][0]=m_x*pSP->dphi();
    m_sigma[1][1]=pSP->dz();
    for(i=0;i<2;i++)
      m_Vk[i][i]+=m_sigma[i][i]*m_sigma[i][i];
  }
  else {
    m_sigma[1][1]=pSP->dr()*m_Rk[3]/cos(m_Rk[2]);
    m_sigma[1][1]=m_sigma[1][1]*m_sigma[1][1];
    double a=m_Rk[2]-m_x*m_fConst*m_Rk[4]-sin(pSP->phi()-m_averagePhi0);
    m_sigma[0][0]=m_x*m_x*pSP->dphi()*pSP->dphi()+
      a*a*pSP->dr()*pSP->dr();
    m_sigma[0][1]=m_sigma[1][0]=a*m_Rk[3]*pSP->dr()*pSP->dr();
    for(i=0;i<2;i++)
      for(j=0;j<2;j++)
	m_Vk[i][j]+=m_sigma[i][j];
  }

  m_detr=m_Vk[0][0]*m_Vk[1][1]-m_Vk[0][1]*m_Vk[1][0];m_detr=1.0/m_detr;
  m_Dk[0][0]=m_Vk[1][1]*m_detr;m_Dk[0][1]=m_Dk[1][0]=-m_Vk[0][1]*m_detr;
  m_Dk[1][1]=m_Vk[0][0]*m_detr;
	  
  for(i=0;i<2;i++) {
    m_resid[i]=0.0;for(j=0;j<5;j++) m_resid[i]+=m_Hk[i][j]*m_Rk[j];
  }
  m_resid[0]=m_y-m_resid[0];
  m_resid[1]=pSP->z()-m_resid[1];
  m_dchi2=m_resid[0]*m_resid[0]*m_Dk[0][0]+2.0*m_Dk[0][1]*m_resid[0]*m_resid[1]+
	    m_resid[1]*m_resid[1]*m_Dk[1][1];

  //	  printf("r1=%f r2=%f dchi2=%f\n",m_resid[0],m_resid[1],m_dchi2);
  //printf("dchi2phi=%f dchi2z=%f\n",
  //	 m_resid[0]*resid[0]*m_Dk[0][0],m_resid[1]*m_resid[1]*m_Dk[1][1]);

  return m_dchi2;
}

void SimpleKalmanFilterClass::update() {

  int i,j,m;

  for(i=0;i<5;i++)
    for(j=0;j<2;j++) {
      m_Kk[i][j]=0.0;for(m=0;m<2;m++) m_Kk[i][j]+=m_Bk[i][m]*m_Dk[m][j];
    }
  for(i=0;i<5;i++)
    m_Rk[i]+=m_Kk[i][0]*m_resid[0]+m_Kk[i][1]*m_resid[1];
  for(i=0;i<5;i++)
    for(j=i;j<5;j++) {
      m_Gk[i][j]-=m_Kk[i][0]*m_Bk[j][0]+m_Kk[i][1]*m_Bk[j][1];
      m_Gk[j][i] =m_Gk[i][j];
    }
}
