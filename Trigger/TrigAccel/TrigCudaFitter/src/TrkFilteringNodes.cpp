//////////////////////////////////////////////////////////////////
// TrkFilteringNodes.cxx
//   Source file for TrkFilteringNodes classes
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "TrkBaseNode.h"
#include "TrkFilteringNodes.h"
#include "TrkTrackState.h"
#include "TrkPlanarSurface.h"

double TrkFilteringNode1D::calculateChi2()
{
  return (m_resid*m_resid*m_D);
}

void TrkFilteringNode1D::runKalmanFilter(TrkTrackState* pTS)
{
  int i,j,idx=0;
  double updState[5],updCov[15];

  for(i=0;i<5;i++) m_K[i]=m_D*m_B[i];
  for(i=0;i<5;i++) updState[i]=m_K[i]*m_resid;
  pTS->updateTrackState(updState);
  for(i=0;i<5;i++) for(j=i;j<5;j++)
  {
    updCov[idx]=-m_K[i]*m_B[j];idx++;
  }
  pTS->updateTrackCovariance(updCov);
}

int TrkFilteringNode1D::getKalmanGain(double K[5][2])
{
  int i;
  for(i=0;i<5;i++) K[i][0]=m_K[i];
  return 1;
}

int TrkFilteringNode1D::getResiduals(double r[2])
{
  r[0]=m_resid;
  return 1;
}

int TrkFilteringNode1D::getInverseResidualVariance(double V[2][2])
{
  V[0][0]=m_D;
  return 1;
}

int TrkFilteringNode1D::getMeasurementMatrix(double H[2][5])
{
  int i;
  for(i=0;i<5;i++) H[0][i]=m_H[i];
  return 1;
}

double TrkFilteringNode2D::calculateChi2()
{
  double dchi2;

  dchi2=m_resid[0]*m_resid[0]*m_D[0][0]+2*m_resid[0]*m_resid[1]*m_D[0][1]+m_resid[1]*m_resid[1]*m_D[1][1];
  return dchi2;
}

void TrkFilteringNode2D::runKalmanFilter(TrkTrackState* pTS)
{
  int i,j,idx=0;
  double updState[5],updCov[15];

  for(i=0;i<5;i++) 
    for(j=0;j<2;j++)
      m_K[i][j]=m_B[i][0]*m_D[0][j]+m_B[i][1]*m_D[1][j];
  for(i=0;i<5;i++) 
    updState[i]=m_K[i][0]*m_resid[0]+m_K[i][1]*m_resid[1]; 
  pTS->updateTrackState(updState);
  for(i=0;i<5;i++) for(j=i;j<5;j++)
  {
    updCov[idx]=-m_K[i][0]*m_B[j][0]-m_K[i][1]*m_B[j][1];idx++;
  }
  pTS->updateTrackCovariance(updCov);
}

int TrkFilteringNode2D::getKalmanGain(double K[5][2])
{
  int i,j;
  for(i=0;i<5;i++) for(j=0;j<2;j++) K[i][j]=m_K[i][j];
  return 2;
}

int TrkFilteringNode2D::getResiduals(double r[2])
{
  r[0]=m_resid[0];r[1]=m_resid[1];
  return 2;
}

int TrkFilteringNode2D::getInverseResidualVariance(double V[2][2])
{
  int i,j;
  for(i=0;i<2;i++) for(j=0;j<2;j++) V[i][j]=m_D[i][j];
  return 2;
}

int TrkFilteringNode2D::getMeasurementMatrix(double H[2][5])
{
  int i,j;
  for(j=0;j<2;j++) for(i=0;i<5;i++) H[j][i]=m_H[j][i];
  return 2;
}

TrkClusterNode::TrkClusterNode(TrkPlanarSurface* pS, double chi2Cut, double pos, double cov)
{
  m_pSurface=pS;m_chi2Cut=chi2Cut;
  m_m=pos;m_V=cov;m_nodeType=2;m_ndof=1;
}

void TrkClusterNode::serialize(char fileName[])
{
  FILE* pFile;
  pFile=fopen(fileName,"a");
  fclose(pFile);
}

void TrkClusterNode::validateMeasurement(TrkTrackState* pTS)
{
  m_dChi2=getChi2Distance(pTS);
  m_nodeState=(m_dChi2<m_chi2Cut)?1:0;
}

double TrkClusterNode::getChi2Distance(TrkTrackState* pTS)
{
  for(int i=0;i<5;i++) m_B[i]=pTS->getTrackCovariance(0,i);
  m_H[0]=1.0;m_H[1]=0.0;m_H[2]=0.0;m_H[3]=0.0;m_H[4]=0.0;
  m_D=1.0/(pTS->getTrackCovariance(0,0)+m_V);
  m_resid=m_m-pTS->getTrackState(0);
  return calculateChi2();
}

void TrkClusterNode::updateTrackState(TrkTrackState* pTS)
{
  if(isValidated()) runKalmanFilter(pTS);
    m_pTrackState=pTS;
}

TrkEndCapClusterNode::TrkEndCapClusterNode(TrkPlanarSurface* pS, double chi2Cut, double Rc,
				     double pos, double cov)
{
  m_pSurface=pS;m_chi2Cut=chi2Cut;
  m_m=pos;m_V=cov;m_Rc=Rc;
  m_nodeType=2;m_ndof=1;
}

void TrkEndCapClusterNode::serialize(char fileName[])
{
  FILE* pFile;
  pFile=fopen(fileName,"a");
  fclose(pFile);
}

void TrkEndCapClusterNode::validateMeasurement(TrkTrackState* pTS)
{
  m_dChi2=getChi2Distance(pTS);
  m_nodeState=(m_dChi2<m_chi2Cut)?1:0;
}

double TrkEndCapClusterNode::getChi2Distance(TrkTrackState* pTS)
{
  double corr;
  corr=1.0/(pTS->getTrackState(1)+m_Rc);
  m_H[0]=m_Rc*corr;
  m_H[1]=-pTS->getTrackState(0)*m_Rc*corr*corr;
  m_H[2]=0.0;m_H[3]=0.0;m_H[4]=0.0;

  m_B[0]=m_H[0]*pTS->getTrackCovariance(0,0)+m_H[1]*pTS->getTrackCovariance(0,1);
  m_B[1]=m_H[0]*pTS->getTrackCovariance(0,1)+m_H[1]*pTS->getTrackCovariance(1,1);
  m_B[2]=m_H[0]*pTS->getTrackCovariance(0,2)+m_H[1]*pTS->getTrackCovariance(1,2);
  m_B[3]=m_H[0]*pTS->getTrackCovariance(0,3)+m_H[1]*pTS->getTrackCovariance(1,3);
  m_B[4]=m_H[0]*pTS->getTrackCovariance(0,4)+m_H[1]*pTS->getTrackCovariance(1,4);
  //corr=1.0+pTS->getTrackState(1)/m_Rc;
  //m_D=1.0/(m_B[0]*m_H[0]+m_B[1]*m_H[1]+m_V*corr*corr);
  m_D=1.0/(m_B[0]*m_H[0]+m_B[1]*m_H[1]+m_V);
  m_resid=m_m-pTS->getTrackState(0)*m_Rc/(pTS->getTrackState(1)+m_Rc);
  return calculateChi2();
}

void TrkEndCapClusterNode::updateTrackState(TrkTrackState* pTS)
{
  if(isValidated()) runKalmanFilter(pTS);
  m_pTrackState=pTS;
}

TrkPixelNode::TrkPixelNode(TrkPlanarSurface* pS,double chi2Cut, double pos[2], double cov[4])
{
  m_pSurface=pS;m_chi2Cut=chi2Cut;
  m_m[0]=pos[0];m_m[1]=pos[1];
  m_V[0][0]=cov[0];m_V[0][1]=cov[1]; m_V[1][0]=cov[2]; m_V[1][1]=cov[3];
  m_nodeType=1;m_ndof=2;
}

void TrkPixelNode::serialize(char fileName[])
{
  FILE* pFile;
  pFile=fopen(fileName,"a");
  fclose(pFile);
}

void TrkPixelNode::report()
{
  /*
  printf("PIXEL NODE x=%f y=%f\n",m_m[0],m_m[1]);
  */
  m_pSurface->report();
}

void TrkPixelNode::validateMeasurement(TrkTrackState* pTS)
{
  m_dChi2=getChi2Distance(pTS);
  m_nodeState=(m_dChi2<m_chi2Cut)?1:0;
}

double TrkPixelNode::getChi2Distance(TrkTrackState* pTS)
{
  double det;

  m_H[0][0]=1.0;m_H[1][1]=1.0;
  m_H[0][1]=0.0;m_H[0][2]=0.0;m_H[0][3]=0.0;m_H[0][4]=0.0;
  m_H[1][0]=0.0;m_H[1][2]=0.0;m_H[1][3]=0.0;m_H[1][4]=0.0;

  m_B[0][0]=pTS->getTrackCovariance(0,0);m_B[0][1]=pTS->getTrackCovariance(0,1);
  m_B[1][0]=pTS->getTrackCovariance(0,1);m_B[1][1]=pTS->getTrackCovariance(1,1);
  m_B[2][0]=pTS->getTrackCovariance(0,2);m_B[2][1]=pTS->getTrackCovariance(1,2);
  m_B[3][0]=pTS->getTrackCovariance(0,3);m_B[3][1]=pTS->getTrackCovariance(1,3);
  m_B[4][0]=pTS->getTrackCovariance(0,4);m_B[4][1]=pTS->getTrackCovariance(1,4);

  m_resid[0]=m_m[0]-pTS->getTrackState(0);
  m_resid[1]=m_m[1]-pTS->getTrackState(1);

  double W[2][2];

  W[0][0]=pTS->getTrackCovariance(0,0)+m_V[0][0];
  W[1][0]=W[0][1]=pTS->getTrackCovariance(0,1)+m_V[0][1];
  W[1][1]=pTS->getTrackCovariance(1,1)+m_V[1][1];
  det=W[0][0]*W[1][1]-W[0][1]*W[0][1];det=1.0/det;
  m_D[0][0]=W[1][1]*det;m_D[0][1]=-W[0][1]*det;
  m_D[1][0]=m_D[0][1];m_D[1][1]=W[0][0]*det;

  return calculateChi2();
}

void TrkPixelNode::updateTrackState(TrkTrackState* pTS)
{	
  if(isValidated()) runKalmanFilter(pTS);
  m_pTrackState=pTS;
}
