/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkFilteringNodes.cxx
//   Source file for TrkFilteringNodes classes
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/TrapezoidBounds.h"



namespace Trk
{

  double TrkFilteringNode1D::m_calculateChi2()
  {
    return (m_resid*m_resid*m_D);
  }

  void TrkFilteringNode1D::m_runKalmanFilter(TrkTrackState* pTS)
  {
    int i,j,idx=0;
    double updState[5],updCov[15];

    for(i=0;i<5;i++) m_K[i]=m_D*m_B[i];
    for(i=0;i<5;i++) updState[i]=m_K[i]*m_resid;
    pTS->m_updateTrackState(updState);
    for(i=0;i<5;i++) for(j=i;j<5;j++)
      {
	updCov[idx]=-m_K[i]*m_B[j];idx++;
      }
    pTS->m_updateTrackCovariance(updCov);
  }

  int TrkFilteringNode1D::m_getKalmanGain(double K[5][2])
  {
    int i;
    for(i=0;i<5;i++) K[i][0]=m_K[i];
    return 1;
  }

  int TrkFilteringNode1D::m_getResiduals(double r[2])
  {
    r[0]=m_resid;
    return 1;
  }
  
  int TrkFilteringNode1D::m_getInverseResidualVariance(double V[2][2])
  {
    V[0][0]=m_D;
    return 1;
  }

  int TrkFilteringNode1D::m_getMeasurementMatrix(double H[2][5])
  {
    int i;
    for(i=0;i<5;i++) H[0][i]=m_H[i];
    return 1;
  }

  double TrkFilteringNode2D::m_calculateChi2()
  {
    double dchi2;

    dchi2=m_resid[0]*m_resid[0]*m_D[0][0]+2*m_resid[0]*m_resid[1]*m_D[0][1]+
      m_resid[1]*m_resid[1]*m_D[1][1];
    return dchi2;
  }

  void TrkFilteringNode2D::m_runKalmanFilter(TrkTrackState* pTS)
  {
    int i,j,idx=0;
    double updState[5],updCov[15];

    for(i=0;i<5;i++) 
      for(j=0;j<2;j++)
	m_K[i][j]=m_B[i][0]*m_D[0][j]+m_B[i][1]*m_D[1][j];
    for(i=0;i<5;i++) 
      updState[i]=m_K[i][0]*m_resid[0]+m_K[i][1]*m_resid[1]; 
    pTS->m_updateTrackState(updState);
    for(i=0;i<5;i++) for(j=i;j<5;j++)
      {
	updCov[idx]=-m_K[i][0]*m_B[j][0]-m_K[i][1]*m_B[j][1];idx++;
      }
    pTS->m_updateTrackCovariance(updCov);
  }

  int TrkFilteringNode2D::m_getKalmanGain(double K[5][2])
  {
    int i,j;
    for(i=0;i<5;i++) for(j=0;j<2;j++) K[i][j]=m_K[i][j];
    return 2;
  }

  int TrkFilteringNode2D::m_getResiduals(double r[2])
  {
    r[0]=m_resid[0];r[1]=m_resid[1];
    return 2;
  }
  
  int TrkFilteringNode2D::m_getInverseResidualVariance(double V[2][2])
  {
    int i,j;
    for(i=0;i<2;i++) for(j=0;j<2;j++) V[i][j]=m_D[i][j];
    return 2;
  }

  int TrkFilteringNode2D::m_getMeasurementMatrix(double H[2][5])
  {
    int i,j;
    for(j=0;j<2;j++) for(i=0;i<5;i++) H[j][i]=m_H[j][i];
    return 2;
  }


  TrkClusterNode::TrkClusterNode(TrkPlanarSurface* pS, double chi2Cut, const PrepRawData* pPRD)
  {
    m_pSurface=pS;m_chi2Cut=chi2Cut;
    m_m=pPRD->localPosition()[0];m_V=pPRD->localCovariance()(0,0);m_pPRD=pPRD;
    m_nodeType=2;m_ndof=1;
  }
  
  TrkClusterNode::TrkClusterNode(TrkPlanarSurface* pS, double chi2Cut, double pos, double cov)
  {
    m_pSurface=pS;m_chi2Cut=chi2Cut;
    m_m=pos;m_V=cov;m_pPRD=NULL;m_nodeType=2;m_ndof=1;
  }

  void TrkClusterNode::m_serialize(char fileName[])
  {
    FILE* pFile;
    pFile=fopen(fileName,"a");
    fclose(pFile);
  }

  void TrkClusterNode::m_validateMeasurement(TrkTrackState* pTS)
  {
    m_dChi2=m_getChi2Distance(pTS);
    m_nodeState=(m_dChi2<m_chi2Cut)?1:0;
  }

  double TrkClusterNode::m_getChi2Distance(TrkTrackState* pTS)
  {
    for(int i=0;i<5;i++) m_B[i]=pTS->m_getTrackCovariance(0,i);
    m_H[0]=1.0;m_H[1]=0.0;m_H[2]=0.0;m_H[3]=0.0;m_H[4]=0.0;
    m_D=1.0/(pTS->m_getTrackCovariance(0,0)+m_V);
    m_resid=m_m-pTS->m_getTrackState(0);
    return m_calculateChi2();
  }

  void TrkClusterNode::m_updateTrackState(TrkTrackState* pTS)
  {
    if(m_isValidated()) m_runKalmanFilter(pTS);
	m_pTrackState=pTS;
  }

  void TrkClusterNode::m_updateWithRIO(const RIO_OnTrack* pRIO)
  {
    /*
    printf("RIO update, SCT cluster: old m=%f new m=%f old V=%f new V=%f\n",
	   m_m,pRIO->localParameters()[Trk::locX],sqrt(m_V),
	   sqrt(pRIO->localErrorMatrix().localCovariance()[0][0]));
    */
    m_m=pRIO->localParameters()[Trk::locX];
    m_V=pRIO->localCovariance()(0,0);
    

  }

  TrkEndCapClusterNode::TrkEndCapClusterNode(TrkPlanarSurface* pS, double chi2Cut, const PrepRawData* pPRD)
  {
    m_pSurface=pS;m_chi2Cut=chi2Cut;
    m_m=pPRD->localPosition()[0];m_V=pPRD->localCovariance()(0,0);m_pPRD=pPRD;

    const Trk::SurfaceBounds& rBounds=pPRD->detectorElement()->surface().bounds();
    const Trk::TrapezoidBounds& ecBounds=dynamic_cast<const Trk::TrapezoidBounds&>(rBounds);

    m_Rc=(ecBounds.maxHalflengthX()+ecBounds.minHalflengthX())*ecBounds.halflengthY()/
      (ecBounds.maxHalflengthX()-ecBounds.minHalflengthX());
    m_nodeType=2;m_ndof=1;

    double C[2][2];
    C[0][0]=pPRD->localCovariance()(0,0);
    C[0][1]=C[1][0]=pPRD->localCovariance()(0,1);
    C[1][1]=pPRD->localCovariance()(1,1);
    m_V=(C[0][0]*m_Rc*m_Rc-2*m_m*m_Rc*C[0][1]+C[1][1]*m_m*m_m)/(m_Rc*m_Rc+m_m*m_m);
    //printf("EC hit error=%f\n",m_V);
  }
  
  TrkEndCapClusterNode::TrkEndCapClusterNode(TrkPlanarSurface* pS, double chi2Cut, const PrepRawData* pPRD, double Rc)
  {
    m_pSurface=pS;m_chi2Cut=chi2Cut;
    m_m=pPRD->localPosition()[0];m_V=pPRD->localCovariance()(0,0);m_pPRD=pPRD;

    m_Rc=Rc;
    m_nodeType=2;m_ndof=1;
    double C[2][2];
    C[0][0]=pPRD->localCovariance()(0,0);
    C[0][1]=C[1][0]=pPRD->localCovariance()(0,1);
    C[1][1]=pPRD->localCovariance()(1,1);
    m_V=(C[0][0]*m_Rc*m_Rc-2*m_m*m_Rc*C[0][1]+C[1][1]*m_m*m_m)/(m_Rc*m_Rc+m_m*m_m);
    //printf("EC hit error=%f\n",m_V);
  }

  TrkEndCapClusterNode::TrkEndCapClusterNode(TrkPlanarSurface* pS, double chi2Cut, double Rc,
					     double pos, double cov)
  {
    m_pSurface=pS;m_chi2Cut=chi2Cut;
    m_m=pos;m_V=cov;m_pPRD=NULL;m_Rc=Rc;
    m_nodeType=2;m_ndof=1;
  }

  void TrkEndCapClusterNode::m_serialize(char fileName[])
  {
    FILE* pFile;
    pFile=fopen(fileName,"a");
    fclose(pFile);
  }

  void TrkEndCapClusterNode::m_validateMeasurement(TrkTrackState* pTS)
  {
    m_dChi2=m_getChi2Distance(pTS);
    m_nodeState=(m_dChi2<m_chi2Cut)?1:0;
  }

  double TrkEndCapClusterNode::m_getChi2Distance(TrkTrackState* pTS)
  {
    double corr;
    corr=1.0/(pTS->m_getTrackState(1)+m_Rc);
    m_H[0]=m_Rc*corr;
    m_H[1]=-pTS->m_getTrackState(0)*m_Rc*corr*corr;
    m_H[2]=0.0;m_H[3]=0.0;m_H[4]=0.0;
	
    m_B[0]=m_H[0]*pTS->m_getTrackCovariance(0,0)+m_H[1]*pTS->m_getTrackCovariance(0,1);
    m_B[1]=m_H[0]*pTS->m_getTrackCovariance(0,1)+m_H[1]*pTS->m_getTrackCovariance(1,1);
    m_B[2]=m_H[0]*pTS->m_getTrackCovariance(0,2)+m_H[1]*pTS->m_getTrackCovariance(1,2);
    m_B[3]=m_H[0]*pTS->m_getTrackCovariance(0,3)+m_H[1]*pTS->m_getTrackCovariance(1,3);
    m_B[4]=m_H[0]*pTS->m_getTrackCovariance(0,4)+m_H[1]*pTS->m_getTrackCovariance(1,4);
    //corr=1.0+pTS->m_getTrackState(1)/m_Rc;
    //m_D=1.0/(m_B[0]*m_H[0]+m_B[1]*m_H[1]+m_V*corr*corr);
    m_D=1.0/(m_B[0]*m_H[0]+m_B[1]*m_H[1]+m_V);
    m_resid=m_m-pTS->m_getTrackState(0)*m_Rc/(pTS->m_getTrackState(1)+m_Rc);
    return m_calculateChi2();
  }

  void TrkEndCapClusterNode::m_updateTrackState(TrkTrackState* pTS)
  {
    if(m_isValidated()) m_runKalmanFilter(pTS);
    m_pTrackState=pTS;
  }

  void TrkEndCapClusterNode::m_updateWithRIO(const RIO_OnTrack* pRIO)
  {
    /*
    printf("RIO update, EC cluster: old m=%f new m=%f old V=%f ",
	   m_m,pRIO->localParameters()[Trk::locX],sqrt(m_V));
    */
    m_m=pRIO->localParameters()[Trk::locX];
    double C[2][2];
    C[0][0]=pRIO->localCovariance()(0,0);
    C[0][1]=C[1][0]=pRIO->localCovariance()(0,1);
    C[1][1]=pRIO->localCovariance()(1,1);
    m_V=(C[0][0]*m_Rc*m_Rc-2*m_m*m_Rc*C[0][1]+C[1][1]*m_m*m_m)/(m_Rc*m_Rc+m_m*m_m);
    //printf("new V=%f\n",sqrt(m_V));
  }

  TrkPixelNode::TrkPixelNode(TrkPlanarSurface* pS,double chi2Cut, const PrepRawData* pPRD)
  {
    int i,j;
    m_pSurface=pS;m_chi2Cut=chi2Cut;
    for(i=0;i<2;i++) 
      {
	m_m[i]=pPRD->localPosition()[i];
	for(j=0;j<2;j++) m_V[i][j]=pPRD->localCovariance()(i,j);
      }
    m_pPRD=pPRD;m_nodeType=1;m_ndof=2;
  }

  TrkPixelNode::TrkPixelNode(TrkPlanarSurface* pS,double chi2Cut, double pos[2], double cov[4])
  {
    m_pSurface=pS;m_chi2Cut=chi2Cut;
    m_m[0]=pos[0];m_m[1]=pos[1];
    m_V[0][0]=cov[0];m_V[0][1]=cov[1]; m_V[1][0]=cov[2]; m_V[1][1]=cov[3];
    m_pPRD=NULL;m_nodeType=1;m_ndof=2;
  }

  void TrkPixelNode::m_serialize(char fileName[])
  {
    FILE* pFile;
    pFile=fopen(fileName,"a");
    fclose(pFile);
  }

  void TrkPixelNode::m_report()
  {
    /*
    printf("PIXEL NODE x=%f y=%f\n",m_m[0],m_m[1]);
    */
    m_pSurface->m_report();
  }

  void TrkPixelNode::m_validateMeasurement(TrkTrackState* pTS)
  {
    m_dChi2=m_getChi2Distance(pTS);
    m_nodeState=(m_dChi2<m_chi2Cut)?1:0;
  }

  double TrkPixelNode::m_getChi2Distance(TrkTrackState* pTS)
  {
    double det;

    m_H[0][0]=1.0;m_H[1][1]=1.0;
    m_H[0][1]=0.0;m_H[0][2]=0.0;m_H[0][3]=0.0;m_H[0][4]=0.0;
    m_H[1][0]=0.0;m_H[1][2]=0.0;m_H[1][3]=0.0;m_H[1][4]=0.0;

    m_B[0][0]=pTS->m_getTrackCovariance(0,0);m_B[0][1]=pTS->m_getTrackCovariance(0,1);
    m_B[1][0]=pTS->m_getTrackCovariance(0,1);m_B[1][1]=pTS->m_getTrackCovariance(1,1);
    m_B[2][0]=pTS->m_getTrackCovariance(0,2);m_B[2][1]=pTS->m_getTrackCovariance(1,2);
    m_B[3][0]=pTS->m_getTrackCovariance(0,3);m_B[3][1]=pTS->m_getTrackCovariance(1,3);
    m_B[4][0]=pTS->m_getTrackCovariance(0,4);m_B[4][1]=pTS->m_getTrackCovariance(1,4);

    m_resid[0]=m_m[0]-pTS->m_getTrackState(0);
    m_resid[1]=m_m[1]-pTS->m_getTrackState(1);

    double W[2][2];
	
    W[0][0]=pTS->m_getTrackCovariance(0,0)+m_V[0][0];
    W[1][0]=W[0][1]=pTS->m_getTrackCovariance(0,1)+m_V[0][1];
    W[1][1]=pTS->m_getTrackCovariance(1,1)+m_V[1][1];
    det=W[0][0]*W[1][1]-W[0][1]*W[0][1];det=1.0/det;
    m_D[0][0]=W[1][1]*det;m_D[0][1]=-W[0][1]*det;
    m_D[1][0]=m_D[0][1];m_D[1][1]=W[0][0]*det;

    return m_calculateChi2();
  }

  void TrkPixelNode::m_updateTrackState(TrkTrackState* pTS)
  {	
    if(m_isValidated()) m_runKalmanFilter(pTS);
    m_pTrackState=pTS;
  }

  void TrkPixelNode::m_updateWithRIO(const RIO_OnTrack* pRIO)
  {
    int i,j;
    /*
    printf("RIO update, Pixel: old m %f %f new m %f %f\n",
	   m_m[0],m_m[1],pRIO->localParameters()[Trk::locX],pRIO->localParameters()[Trk::locY]);
    */
    m_m[0]=pRIO->localParameters()[Trk::locX];
    m_m[1]=pRIO->localParameters()[Trk::locY];

    // printf("old errors: %f %f %f\n",sqrt(m_V[0][0]),m_V[0][1],sqrt(m_V[1][1]));

    for(i=0;i<2;i++) 
      {
	for(j=0;j<2;j++) m_V[i][j]=pRIO->localCovariance()(i,j);
      }
    // printf("new errors: %f %f %f\n",sqrt(m_V[0][0]),m_V[0][1],sqrt(m_V[1][1]));
  }

  TrkTrtNode::TrkTrtNode(TrkPlanarSurface* pS,double chi2Cut,double lmin, double lmax,
			 const PrepRawData* pPRD)
  {
    m_pSurface=pS;m_chi2Cut=chi2Cut; 
    m_m=pPRD->localPosition()[0];
    m_V=pPRD->localCovariance()(0,0);
    m_minBound=lmin;
    m_maxBound=lmax;
    m_lrSign=0;m_freezeLR=false;
    m_pPRD=pPRD;m_nodeType=3;m_ndof=1;
  }

  void TrkTrtNode::m_serialize(char fileName[])
  {
    FILE* pFile;
    pFile=fopen(fileName,"a");
    fclose(pFile);
  }

  void TrkTrtNode::m_validateMeasurement(TrkTrackState* pTS)
  {
    if((pTS->m_getTrackState(1)>m_minBound)&&(pTS->m_getTrackState(1)<m_maxBound))
      {
	m_dChi2=m_getChi2Distance(pTS);
	m_nodeState=(m_dChi2<m_chi2Cut)?1:0;
      }
    else m_nodeState=0;
  }

  double TrkTrtNode::m_getChi2Distance(TrkTrackState* pTS)
  {
    int i;
    double tx,lV[3],V[3],corr;
    //double lP[3],P[3];

    if(!m_freezeLR) m_lrSign=(pTS->m_getTrackState(0)<0.0)?-1:1;	    
    V[0]=sin(pTS->m_getTrackState(3))*cos(pTS->m_getTrackState(2));
    V[1]=sin(pTS->m_getTrackState(3))*sin(pTS->m_getTrackState(2));
    V[2]=cos(pTS->m_getTrackState(3));
    //lP[0]=pTS->m_getTrackState(0);
    //lP[1]=pTS->m_getTrackState(1);
    //lP[2]=0.0;
    //pTS->m_getSurface()->m_transformPointToGlobal(lP,P);
    pTS->m_getSurface()->m_rotateVectorToLocal(V,lV);
    tx=lV[0]/lV[2];
    corr=1.0/sqrt(1.0+tx*tx);
    for(i=0;i<5;i++) m_B[i]=corr*pTS->m_getTrackCovariance(0,i);
    m_H[0]=corr;m_H[1]=0.0;m_H[2]=0.0;m_H[3]=0.0;m_H[4]=0.0;
    m_D=1.0/(corr*corr*pTS->m_getTrackCovariance(0,0)+m_V);
    m_resid=m_lrSign*m_m-corr*pTS->m_getTrackState(0);
    return m_calculateChi2();
  }

  void TrkTrtNode::m_updateTrackState(TrkTrackState* pTS)
  {	
    if(m_isValidated()) m_runKalmanFilter(pTS);
    m_pTrackState=pTS;
  }

  void TrkTrtNode::m_updateInternal()
  {
    if(m_isValidated()&&(m_pTrackState!=NULL))
      {
	m_freezeLR=true;
	m_lrSign=(m_pTrackState->m_getTrackState(0)<0.0)?-1:1;
      }
  }

  void TrkTrtNode::m_updateWithRIO(const RIO_OnTrack*)
  {

  }

}
