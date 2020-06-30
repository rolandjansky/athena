/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkTrackState.cxx
//   Source file for TrkTrackState class
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkParameters/TrackParameters.h"

#include "memory.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace Trk
{
  TrkTrackState::TrkTrackState()
  {
    memset(m_Rk,0,sizeof(m_Rk));
    memset(m_Gk,0,sizeof(m_Gk));
    m_pSurface=nullptr;m_scattMode=0;
    m_isScattered=false;m_pPrevState=nullptr;
  }

  TrkTrackState::TrkTrackState(double Rk[5])
  {
    int i;
    for(i=0;i<5;i++) m_Rk[i]=Rk[i];
    if(m_Rk[2]>M_PI) m_Rk[2]-=2*M_PI;
    if(m_Rk[2]<-M_PI) m_Rk[2]+=2*M_PI;
    if(m_Rk[3]<0.0) m_Rk[3]+=M_PI;
    if(m_Rk[3]>M_PI) m_Rk[3]-=M_PI;
    m_pSurface=nullptr;m_scattMode=0;
    m_isScattered=false;m_pPrevState=nullptr;
    resetCovariance();
  }

  void TrkTrackState::resetCovariance()
  {
    memset(m_Gk,0,sizeof(m_Gk));
    m_Gk[0][0]=100.0;m_Gk[1][1]=100.0;m_Gk[2][2]=0.01;m_Gk[3][3]=0.01;m_Gk[4][4]=1e-5;
  }

  TrkTrackState::TrkTrackState(const TrkTrackState* pTS)
  {
    for(int i=0;i<5;i++)
      {
	m_Rk[i]=pTS->m_Rk[i];
      }
    for(int i=0;i<5;i++)
      for(int j=0;j<5;j++) m_Gk[i][j]=pTS->m_Gk[i][j];
    m_scattMode=pTS->m_scattMode;
    m_pSurface=nullptr;
    m_isScattered=false;m_pPrevState=nullptr;
  }

  void TrkTrackState::serialize(char fileName[])
  {
    FILE *pFile;
    
    pFile=fopen(fileName,"a");
    fprintf(pFile,"%f %f %f %f %f\n",
	   m_Rk[0],m_Rk[1],m_Rk[2],m_Rk[3],m_Rk[4]);
    for(int i=0;i<5;i++)
      {
	for(int j=0;j<5;j++)
	  {
	    if(j<i) fprintf(pFile,"          ");
	    else fprintf(pFile,"%f ",getTrackCovariance(i,j));
	  }
	fprintf(pFile,"\n");
      }
    fclose(pFile);
  }

  void TrkTrackState::report()
  {
    printf("STATE x0=%f y0=%f phi=%f theta=%f qOverP=%f pT=%f\n",
	   m_Rk[0],m_Rk[1],m_Rk[2],m_Rk[3],m_Rk[4],sin(m_Rk[3])/m_Rk[4]);    
    printf("COVARIANCE \n");
    for(int i=0;i<5;i++)
      {
	for(int j=0;j<5;j++)
	  {
	    printf("%E ",m_Gk[i][j]);
	  }
	printf("\n");
      }
  }

  void TrkTrackState::attachToSurface(TrkPlanarSurface* pS)
  {
    m_pSurface=pS;m_isScattered=false;
  }

  TrkPlanarSurface* TrkTrackState::getSurface()
  {
    return m_pSurface;
  }

  void TrkTrackState::updateTrackState(double* pUpd)
  {
    for(int i=0;i<5;i++) m_Rk[i]+=pUpd[i];
    if(m_Rk[2]>M_PI) m_Rk[2]-=2*M_PI;
    if(m_Rk[2]<-M_PI) m_Rk[2]+=2*M_PI;
    if(m_Rk[3]<0.0) m_Rk[3]+=M_PI;
    if(m_Rk[3]>M_PI) m_Rk[3]-=M_PI; 
  }

  void TrkTrackState::updateTrackCovariance(double* pUpd)
  {
    int idx=0;
    for(int i=0;i<5;i++)
      {
	for(int j=i;j<5;j++) 
	  {
	    m_Gk[i][j]+=pUpd[idx];idx++;
	    m_Gk[j][i]=m_Gk[i][j];
	  }
      }
  }

  void TrkTrackState::setScatteringMode(int mode)
  {
    m_scattMode=mode;
  }

  int TrkTrackState::getScatteringMode()
  {
    return m_scattMode;
  }
  void TrkTrackState::setTrackState(double R[5])
  {
    for(int i=0;i<5;i++) m_Re[i]=m_Rk[i]=R[i];
  }
  
  void TrkTrackState::setTrackCovariance(double G[5][5])
  {
    for(int i=0;i<5;i++) 
      for(int j=0;j<5;j++)
	m_Ge[i][j]=m_Gk[i][j]=G[i][j];
  }
  
  void TrkTrackState::setPreviousState(TrkTrackState* pTS)
  {
    m_pPrevState=pTS;
  }

  void TrkTrackState::setSmootherGain(double A[5][5])
  {
    for(int i=0;i<5;i++) 
      for(int j=0;j<5;j++)
	m_A[i][j]=A[i][j];
  }
  void TrkTrackState::applyMultipleScattering()
  {
    double lenCorr,sigmaMS,s2,a2,radLength,lV[3],gV[3],a;
    TrkPlanarSurface* pS=m_pSurface;
    if(pS==nullptr) return;
    gV[0]=sin(m_Re[3])*cos(m_Re[2]);
    gV[1]=sin(m_Re[3])*sin(m_Re[2]);
    gV[2]=cos(m_Re[3]);
    pS->rotateVectorToLocal(gV,lV);
    lenCorr=1.0/fabs(lV[2]);
    radLength=m_pSurface->getRadLength()*lenCorr;
    sigmaMS=13.6*fabs(m_Re[4])*sqrt(radLength)*(1.0+0.038*log(radLength));
    s2=sigmaMS*sigmaMS;a=1.0/sin(m_Rk[3]);a2=a*a;
    m_Ge[2][2]+=s2*a2;m_Ge[3][3]+=s2;m_Ge[2][3]+=s2*a;
    m_Ge[3][2]=m_Ge[2][3]; 
    m_Gk[2][2]=m_Ge[2][2];m_Gk[2][3]=m_Ge[2][3];
    m_Gk[3][2]=m_Ge[3][2];m_Gk[3][3]=m_Ge[3][3];
  }
  
  void TrkTrackState::applyEnergyLoss(int dir)
  {
    double lenCorr,effLength,lV[3],gV[3];
    TrkPlanarSurface* pS=m_pSurface;
    if(pS==nullptr) return;
    gV[0]=sin(m_Re[3])*cos(m_Re[2]);
    gV[1]=sin(m_Re[3])*sin(m_Re[2]);
    gV[2]=cos(m_Re[3]);
    pS->rotateVectorToLocal(gV,lV);
    lenCorr=1.0/fabs(lV[2]);
    effLength=m_pSurface->getRadLength()*lenCorr;

    if(abs(dir)==1)
      {
	m_Re[4]+=dir*(m_Re[4]*effLength*(1.0-0.5*effLength));
	m_Ge[4][4]+=m_Re[4]*m_Re[4]*effLength*(0.415-0.744*effLength);
	m_Rk[4]=m_Re[4];m_Gk[4][4]=m_Ge[4][4];
      }
    else if(abs(dir)==2)
      {
	m_Ge[4][4]+=1e-5;
	m_Rk[4]=m_Re[4];m_Gk[4][4]=m_Ge[4][4];
      }
  }

  void TrkTrackState::applyMaterialEffects()
  {
    if(m_scattMode!=0) applyMultipleScattering();
    if(m_scattMode==-2) applyEnergyLoss(-1);
    else if(m_scattMode==2) applyEnergyLoss(1);
    if(m_pSurface!=nullptr)
      {
	if(m_pSurface->isBreakPoint()) applyEnergyLoss(2);
      }
  }

  void TrkTrackState::runSmoother()
  {
    double dR[5],dG[5][5],B[5][5];
    int i,j,m;

    if(m_pPrevState==nullptr) return;
    
    for(i=0;i<5;i++) 
      {
	dR[i]=m_Rk[i]-m_Re[i];
	for(j=0;j<5;j++) dG[i][j]=m_Gk[i][j]-m_Ge[i][j];
      }

    if(dR[2]>M_PI) dR[2]-=2*M_PI; 
    if(dR[2]<-M_PI) dR[2]+=2*M_PI;
    for(i=0;i<5;i++)
      {
	for(j=0;j<5;j++) m_pPrevState->m_Rk[i]+=m_A[i][j]*dR[j];
      }

    if(m_pPrevState->m_Rk[2]>M_PI) m_pPrevState->m_Rk[2]-=2*M_PI; 
    if(m_pPrevState->m_Rk[2]<-M_PI) m_pPrevState->m_Rk[2]+=2*M_PI;                                        
    if(m_pPrevState->m_Rk[3]<0.0) m_pPrevState->m_Rk[3]+=M_PI;                                         
    if(m_pPrevState->m_Rk[3]>M_PI) m_pPrevState->m_Rk[3]-=M_PI;
    for(i=0;i<5;i++)
      for(j=0;j<5;j++)
	{
	  B[i][j]=0.0;for(m=0;m<5;m++) B[i][j]+=m_A[i][m]*dG[m][j];
	}
    for(i=0;i<5;i++)
      for(j=0;j<5;j++)
	{
	  for(m=0;m<5;m++)
	    m_pPrevState->m_Gk[i][j]+=B[i][m]*m_A[j][m];
	}
  }

}
