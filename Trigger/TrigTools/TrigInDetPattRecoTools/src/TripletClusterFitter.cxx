/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>
#include <memory>
#include <algorithm>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigInDetTracklet.h"
#include "TrigInDetPattRecoTools/LayeredSpacePointStorage.h"
#include "TrigInDetPattRecoTools/TripletClusterFitter.h"

int TripletClusterFitter::fit(const std::vector<std::shared_ptr<TrigInDetTriplet>>& input, float& eta, float& z0, float& pT) {
  //double chi2=0.0;
  int nHits=0;

  /*
  int nGoodTriplets=0;
  int barCode = 10001;

  for(auto tr1 : input) {
    bool found1 = (barCode == tr1->s1().barCode()) && (barCode == tr1->s2().barCode()) && (barCode == tr1->s3().barCode());
    if(found1) {
      nGoodTriplets++;
      std::cout<<"Triplet pT="<<tr1->pt()<<std::endl;
    }
  }

  bool goodTrack = (nGoodTriplets>1);
  */

  std::vector<const TrigSiSpacePointBase*> spVec;
  spVec.reserve(input.size()+2);
  spVec.push_back(&(*input.begin())->s1());
  spVec.push_back(&(*input.begin())->s2());
  for(auto t : input) {
    spVec.push_back(&(t->s3()));
  }

  newTrack(spVec, eta, z0, pT);

  std::sort(spVec.begin() + 2, spVec.end(), TripletClusterFitter::SpComparator(m_cosA, m_sinA));
  int nProcessed=0;
  for(std::vector<const TrigSiSpacePointBase *>::const_iterator spIt=spVec.begin();spIt!=spVec.end();spIt++, nProcessed++) {
    //    if (goodTrack) std::cout<<"Estimates : d0="<<m_Rk[0]<<" z0="<<m_Rk[1]<<" phi0="<<m_Rk[2]<<" pT="<<1.0/m_Rk[4]<<std::endl;
    const TrigSiSpacePointBase* pSP=(*spIt);
    //if (goodTrack) std::cout<<"r="<<pSP->r()<<" z="<<pSP->z()<<" barcode="<<pSP->barCode()<<" : ";
    double dchi2 = getChi2(pSP);
    if(dchi2<m_maxChi2) {
      //if (goodTrack) std::cout<<"accepted, chi2= "<<dchi2<<std::endl;
      //chi2+=dchi2;
      update();
      nHits++;
    }
    else {
      // if (goodTrack) std::cout<<"rejected, chi2="<<dchi2<<std::endl;
    }
    if(nProcessed>2) applyMaterialEffects();
  }

  //  if (goodTrack) std::cout<<"Total chi2="<<chi2<<" total "<<spVec.size()<<" hits, "<<nHits<<" accepted"<<std::endl;

  /*
  printf("Estimates before amb.resolving : d0=%f z0=%f phi0=%f pT=%f\n",
	 KF.m_Rk[0],KF.m_Rk[1],KF.m_Rk[2],1.0/KF.m_Rk[4]);
  printf("Chi2 = %f Total %d ambiguities\n",
	 chi2,nAmbiLayers);
  */
  z0 = m_Rk[1];
  eta = std::asinh(m_Rk[3]);//==eta
  pT = 1.0/m_Rk[4];

  int nEffHits = nProcessed < 5 ? nProcessed-2 : nHits-2;//2-triplet clusters are always accepted

  return nEffHits;//effective number of triplets

}
void TripletClusterFitter::newTrack(const std::vector<const TrigSiSpacePointBase*>& vsp, float eta, float z0, float pT) {


  m_Rk[0]=0.0;
  m_Rk[1]=z0;
  m_Rk[2]=0.0;
  m_Rk[3]= std::sinh(eta);//==cotan(theta)
  m_Rk[4]=1.0/pT;

  memset(&m_Gk[0][0],0,sizeof(m_Gk));
  m_Gk[0][0]=100.0;
  m_Gk[1][1]=100.0;
  m_Gk[2][2]=1.0;
  m_Gk[3][3]=1.0;
  m_Gk[4][4]=0.01; 

  //rotation to the seed-related c.s.

  double dx = vsp[1]->x() - vsp[0]->x();
  double dy = vsp[1]->y() - vsp[0]->y();
  double L = 1.0/sqrt(dx*dx+dy*dy);
  m_cosA = dx*L;
  m_sinA = dy*L;

  double sMS = 13.6*m_X0*m_Rk[4]/std::sqrt(1+m_Rk[3]*m_Rk[3]);
  m_sigmaMS2 += sMS*sMS;


  m_deltaCov = m_Rk[4]*m_Rk[4]*m_X0*(0.415-0.744*m_X0);
}

double TripletClusterFitter::getChi2(const TrigSiSpacePointBase* pSP) {

  const double sp_z = pSP->z();
  const double sp_r = pSP->r();
  const double sp_dr = pSP->dr();
  const double sp_dz = pSP->dz();
  const double sp_phi = pSP->phi();
  const double sp_dphi = pSP->dphi();
  const double x_sp =  pSP->x()*m_cosA + pSP->y()*m_sinA;
  const double y_sp =  pSP->y()*m_cosA - pSP->x()*m_sinA;

  //double Hk[2][5];

  double Hk[2][5] = {{1.0, 0.0, x_sp, 0.0, -0.5*x_sp*x_sp*m_fConst},
                     {0.0, 1.0,  0.0, sp_r,                    0.0}};
  //Hk[0][0]=1.0;Hk[0][1]=0.0;
  //Hk[0][2]=x_sp;
  //Hk[0][3]=0.0;Hk[0][4]=-0.5*x_sp*x_sp*m_fConst;
  //Hk[1][0]=0.0;Hk[1][1]=1.0;
  //Hk[1][2]=0.0;Hk[1][3]=x_sp;Hk[1][4]=0.0;
  
  for(int i=0;i<5;i++) {
    for(int j=0;j<2;j++) {
      m_Bk[i][j]=0.0;
      for(int m=0;m<5;m++) {
        m_Bk[i][j]+=m_Gk[i][m]*Hk[j][m];
      }
    }
  }
  for(int i=0;i<2;i++) {
    for(int j=0;j<2;j++) {
      m_Vk[i][j]=0.0;
      for(int m=0;m<5;m++) {
        m_Vk[i][j]+=Hk[i][m]*m_Bk[m][j];
      }
    }
  }
  
  double sigma[2][2];
  if(m_layerHelper.isBarrel(pSP->layer())) {

    sigma[0][0]=x_sp*sp_dphi;
    sigma[1][1]=sp_dz;
    for(int i=0;i<2;i++) {
      m_Vk[i][i]+=sigma[i][i]*sigma[i][i];
    }
  }
  else {
    sigma[1][1]=sp_dr*m_Rk[3]/cos(m_Rk[2]);
    sigma[1][1]=sigma[1][1]*sigma[1][1];

    double sin_dphi = m_sinA*cos(sp_phi) - m_cosA*sin(sp_phi);

    sigma[0][0]=x_sp*x_sp*sp_dphi*sp_dphi + sin_dphi*sin_dphi*sp_dr*sp_dr;
    sigma[0][1]=sigma[1][0]=sin_dphi*m_Rk[3]*sp_dr*sp_dr;
    for(int i=0;i<2;i++) {
      for(int j=0;j<2;j++) {
        m_Vk[i][j]+=sigma[i][j];
      }
    }
  }

  double detr=1.0/(m_Vk[0][0]*m_Vk[1][1]-m_Vk[0][1]*m_Vk[1][0]);
  m_Dk[0][0]=m_Vk[1][1]*detr;
  m_Dk[0][1]=m_Dk[1][0]=-m_Vk[0][1]*detr;
  m_Dk[1][1]=m_Vk[0][0]*detr;

  for(int i=0;i<2;i++) {
    m_resid[i]=0.0;
    for(int j=0;j<5;j++) {
      m_resid[i]+=Hk[i][j]*m_Rk[j];
    }
  }
  m_resid[0]=y_sp-m_resid[0];
  m_resid[1]=sp_z-m_resid[1];
  float dchi2=m_resid[0]*m_resid[0]*m_Dk[0][0]+2.0*m_Dk[0][1]*m_resid[0]*m_resid[1]+
	    m_resid[1]*m_resid[1]*m_Dk[1][1];

  //	  printf("r1=%f r2=%f dchi2=%f\n",m_resid[0],m_resid[1],dchi2);
  //printf("dchi2phi=%f dchi2z=%f\n",
  //	 m_resid[0]*resid[0]*m_Dk[0][0],m_resid[1]*m_resid[1]*m_Dk[1][1]);

  return dchi2;
}

void TripletClusterFitter::update() {

  double Kk[5][2];
  for(int i=0;i<5;i++) {
    for(int j=0;j<2;j++) {
      Kk[i][j]=0.0;
      for(int m=0;m<2;m++) {
        Kk[i][j]+=m_Bk[i][m]*m_Dk[m][j];
      }
    }
  }
  for(int i=0;i<5;i++) {
    m_Rk[i]+=Kk[i][0]*m_resid[0]+Kk[i][1]*m_resid[1];
  }
  for(int i=0;i<5;i++) {
    for(int j=i;j<5;j++) {
      m_Gk[i][j]-=Kk[i][0]*m_Bk[j][0]+Kk[i][1]*m_Bk[j][1];
      m_Gk[j][i] =m_Gk[i][j];
    }
  }
}

void TripletClusterFitter::applyMaterialEffects() {
  m_Gk[2][2] += m_sigmaMS2;
  m_Gk[3][3] += m_sigmaMS2;
  if(m_doEloss) {
    m_Gk[4][4] += m_deltaCov;
    m_Rk[4] += m_fracLoss*m_Rk[4];
  }
}
