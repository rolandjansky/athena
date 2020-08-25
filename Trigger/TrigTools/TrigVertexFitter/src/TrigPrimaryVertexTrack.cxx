/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigL2Vertex.h"
#include "TrigPrimaryVertexTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

TrigPrimaryVertexTrack::TrigPrimaryVertexTrack(const TrigInDetTrack* pT)
{
  m_nTrackType=1;m_active=true;
  m_pTrigTrack=pT;m_pTrkTrack=NULL;m_dChi2=-100.0;
  double Ck[5][5];
  int i,j;

  const TrigInDetTrackFitPar* p=pT->param();

  double a=-2.0*exp(-p->eta())/(1.0+exp(-2.0*p->eta()));
  m_u[0]=p->a0();
  m_u[1]=p->z0();
  m_q[0]=p->phi0();
  m_q[1]=2.0*atan(exp(-p->eta()));
  m_q[2]=p->pT()*1e-3;


  Ck[0][0]=(*p->cov())[0];Ck[0][1]=Ck[1][0]=(*p->cov())[2];
  Ck[0][2]=Ck[2][0]=(*p->cov())[1];Ck[0][3]=Ck[3][0]=(*p->cov())[3];
  Ck[0][4]=Ck[4][0]=(*p->cov())[4];Ck[1][1]=(*p->cov())[9];
  Ck[1][2]=Ck[2][1]=(*p->cov())[6];Ck[1][3]=Ck[3][1]=(*p->cov())[10];
  Ck[1][4]=Ck[4][1]=(*p->cov())[11];Ck[2][2]=(*p->cov())[5];
  Ck[2][3]=Ck[3][2]=(*p->cov())[7];Ck[2][4]=Ck[4][2]=(*p->cov())[8];
  Ck[3][3]=(*p->cov())[12];Ck[3][4]=Ck[4][3]=(*p->cov())[13];
  Ck[4][4]=(*p->cov())[14];
  for(i=0;i<5;i++)
    {
      Ck[3][i]=a*Ck[3][i];Ck[i][3]=Ck[3][i];
    }
  Ck[3][3]*=a;
  for(i=0;i<5;i++)                    
    {
      Ck[4][i]=Ck[4][i]*1e-3;   Ck[i][4]=Ck[4][i];
    }
  Ck[4][4]*=1e-3;


  for(i=0;i<2;i++) for(j=0;j<2;j++) m_Vuu[i][j]=Ck[i][j];
  for(i=0;i<2;i++) for(j=0;j<3;j++) m_Vuq[i][j]=Ck[i][j+2];
  for(i=0;i<3;i++) for(j=0;j<3;j++) m_Vqq[i][j]=Ck[i+2][j+2];

  m_Perigee[0]=m_u[0];m_Perigee[1]=m_u[1];m_Perigee[2]=m_q[0];m_Perigee[3]=m_q[1];m_Perigee[4]=m_q[2];
}

TrigPrimaryVertexTrack::TrigPrimaryVertexTrack(const Trk::Track* pT)
{
  m_nTrackType=2;m_active=true;
  m_pTrkTrack=pT;m_pTrigTrack=NULL;m_dChi2=-100.0;

  const Trk::Perigee* pP= pT->perigeeParameters();

  if(pP!=NULL) {
    m_u[0]=pP->parameters()[Trk::d0];
    m_u[1]=pP->parameters()[Trk::z0];
    m_q[0]=pP->parameters()[Trk::phi0];
    m_q[1]=pP->parameters()[Trk::theta];

    const double ptC=1000.0*pP->parameters()[Trk::qOverP];
    const double inv_ptC = 1. / ptC;

    m_q[2]=sin(pP->parameters()[Trk::theta])*inv_ptC;

    const AmgSymMatrix(5)& TC= (*pP->covariance());

    const double a=cos(pP->parameters()[Trk::theta])*inv_ptC;
    const double b=-sin(pP->parameters()[Trk::theta])/(pP->parameters()[Trk::qOverP]*ptC);

         
    const double Ck[5][5] = {{TC(0,0), TC(1,0), TC(2,0), TC(3,0), a*TC(3,0) + b*TC(4,0)},
                             {TC(1,0), TC(1,1), TC(2,1), TC(3,1), a*TC(3,1) + b*TC(4,1)},
                             {TC(2,0), TC(2,1), TC(2,2), TC(3,2), a*TC(3,2) + b*TC(4,2)},
                             {TC(3,0), TC(3,1), TC(3,2), TC(3,3), a*TC(3,3) + b*TC(4,3)},
                             {a*TC(3,0) + b*TC(4,0), a*TC(3,1) + b*TC(4,1), a*TC(3,2) + b*TC(4,2), a*TC(3,3) + b*TC(4,3), a*a*TC(3,3) + 2*a*b*TC(4,3) + b*b*TC(4,4)}};

    for(int i=0;i<2;i++) for(int j=0;j<2;j++) m_Vuu[i][j]=Ck[i][j];
    for(int i=0;i<3;i++) for(int j=0;j<3;j++) m_Vqq[i][j]=Ck[i+2][j+2];
    for(int i=0;i<2;i++) for(int j=0;j<3;j++) m_Vuq[i][j]=Ck[i][j+2];

    m_Perigee[0]=m_u[0];m_Perigee[1]=m_u[1];m_Perigee[2]=m_q[0];m_Perigee[3]=m_q[1];m_Perigee[4]=m_q[2];
  }
}

TrigPrimaryVertexTrack::~TrigPrimaryVertexTrack()
{

}

void TrigPrimaryVertexTrack::mask()
{
  m_active=false;
}

void TrigPrimaryVertexTrack::activate()
{
  m_active=true;
}

bool TrigPrimaryVertexTrack::isActive()
{
  return m_active;
}

int TrigPrimaryVertexTrack::getTrackType()
{
  return m_nTrackType;
}


const TrigInDetTrack* TrigPrimaryVertexTrack::getTrigTrack()
{
  return m_pTrigTrack;
}

const Trk::Track* TrigPrimaryVertexTrack::getTrkTrack()
{
	return m_pTrkTrack;
}

void TrigPrimaryVertexTrack::setIndex(int i)
{
  m_index=i;
}
 
int TrigPrimaryVertexTrack::getIndex() const
{
  return m_index;
}

const double* TrigPrimaryVertexTrack::Perigee() const
{
  return &m_Perigee[0];
}

double TrigPrimaryVertexTrack::PerigeeCovariance(int i, int j) const
{
  return m_PerigeeCovariance[i][j];
}

double TrigPrimaryVertexTrack::getChi2Distance(class TrigL2Vertex* pV)
{
  const double C=0.02997;
  const double B=20.84;
	
  double Sk[2][2],detr,chi2;
  double AC[2][3],BV[2][3],h[2];
  int i,j,k;
  double psi,sinPsi,xv,yv,zv,
    cosPhi0,sinPhi0,ctt,sint,phi0,theta0,P0;
  const double alpha = C*B*1e-3;
  const double inv_alpha = 1. / alpha;

  xv=pV->getParametersVector()[0];
  yv=pV->getParametersVector()[1];
  zv=pV->getParametersVector()[2];

  phi0=m_q[0];
  theta0=m_q[1];
  P0=m_q[2];

  cosPhi0=cos(phi0);sinPhi0=sin(phi0);
  sinPsi=-alpha*(xv*cosPhi0+yv*sinPhi0)/P0;
  if(fabs(sinPsi)>1.0) return -999.9;
  const double cosPsi=sqrt(1.0-sinPsi*sinPsi);
  const double inv_cosPsi = 1. / cosPsi;
  psi=asin(sinPsi);
  sint=sin(theta0);
  ctt=cos(theta0)/sint;

  m_A[0][0]=-sin(phi0+psi)*inv_cosPsi;
  m_A[0][1]= cos(phi0+psi)*inv_cosPsi;
  m_A[0][2]=0.0;

  m_A[1][0]=-ctt*cosPhi0*inv_cosPsi;
  m_A[1][1]=-ctt*sinPhi0*inv_cosPsi;
  m_A[1][2]=1.0;

  m_B[0][0]=-xv*m_A[0][1]+yv*m_A[0][0];
  m_B[0][1]=0.0;
  m_B[0][2]=(1.0-inv_cosPsi)*inv_alpha;

  m_B[1][0]=-xv*m_A[1][1]+yv*m_A[1][0];
  m_B[1][1]=-P0*psi/(alpha*sint*sint);
  m_B[1][2]=ctt*(psi-sinPsi*inv_cosPsi)*inv_alpha;

  h[0]=yv*cosPhi0-xv*sinPhi0+P0*(1-cosPsi)*inv_alpha;
  h[1]=zv+P0*ctt*psi*inv_alpha;

  m_resid[0]=m_u[0]-h[0];
  m_resid[1]=m_u[1]-h[1];

  for(i=0;i<2;i++) for(j=0;j<2;j++) Sk[i][j]=m_Vuu[i][j];
  for(i=0;i<2;i++) for(j=0;j<3;j++)
    {
      AC[i][j]=0.0;
      for(k=0;k<3;k++) AC[i][j]+=m_A[i][k]*pV->m_Gk[k][j];
    }
  for(i=0;i<2;i++) for(j=0;j<2;j++)
    {
      for(k=0;k<3;k++) Sk[i][j]+=AC[i][k]*m_A[j][k];
    }
  for(i=0;i<2;i++)
    for(j=0;j<3;j++)
      {
	BV[i][j]=0.0;
	for(k=0;k<3;k++) BV[i][j]+=m_B[i][k]*m_Vqq[k][j];
      }
  for(i=0;i<2;i++)
    for(j=0;j<2;j++)
      {
	for(k=0;k<3;k++) Sk[i][j]+=BV[i][k]*m_B[j][k];
      }
  Sk[0][0]-=2.0*(m_Vuq[0][0]*m_B[0][0]+m_Vuq[0][1]*m_B[0][1]+m_Vuq[0][2]*m_B[0][2]);
  Sk[1][1]-=2.0*(m_Vuq[1][0]*m_B[1][0]+m_Vuq[1][1]*m_B[1][1]+m_Vuq[1][2]*m_B[1][2]);
  Sk[0][1]-=m_Vuq[1][0]*m_B[0][0]+m_Vuq[1][1]*m_B[0][1]+m_Vuq[1][2]*m_B[0][2]+
    m_Vuq[0][0]*m_B[1][0]+m_Vuq[0][1]*m_B[1][1]+m_Vuq[0][2]*m_B[1][2];
  Sk[1][0]=Sk[0][1];

  detr=1.0/(Sk[0][0]*Sk[1][1]-Sk[0][1]*Sk[1][0]);
  m_V[0][0]=Sk[1][1]*detr;
  m_V[1][1]=Sk[0][0]*detr;
  m_V[0][1]=m_V[1][0]=-Sk[0][1]*detr;
  
  chi2=m_V[0][0]*m_resid[0]*m_resid[0]+m_V[1][1]*m_resid[1]*m_resid[1]+
    2.0*m_V[0][1]*m_resid[1]*m_resid[0];

  for(i=0;i<2;i++) 
    for(j=0;j<3;j++) m_D[i][j]=AC[i][j];
    
  m_dChi2=chi2;
  return chi2;
}

double TrigPrimaryVertexTrack::getChi2Contribution()
{
  return m_dChi2;
}

void TrigPrimaryVertexTrack::updateVertex(class TrigL2Vertex* pV)
{
  int i,j,k;

  double K[2][3];

  for(i=0;i<2;i++)
    {
      for(j=0;j<3;j++)
	{
	  K[i][j]=0.0;
	  for(k=0;k<2;k++) K[i][j]+=m_D[k][j]*m_V[k][i];
	}
    }
  for(i=0;i<3;i++)
    {
      pV->getParametersVector()[i]+=K[0][i]*m_resid[0]+K[1][i]*m_resid[1];
      for(j=i;j<3;j++)
	{
	  pV->m_Gk[i][j]-=K[0][i]*m_D[0][j]+K[1][i]*m_D[1][j];
	  pV->m_Gk[j][i]=pV->m_Gk[i][j];
	}
    }
}

MsgStream& TrigPrimaryVertexTrack::report( MsgStream& out ) const
{
  int i;

  out<<"Primary track "<<m_index<<endmsg;
  for(i=0;i<2;i++)
    {
      out<<"  u"<<i<<" = "<<m_u[i]<<"      "<<m_Vuu[i][0]<<"   "<<m_Vuu[i][1]<<endmsg;
    }
  for(i=0;i<3;i++)
    {
      out<<"  q"<<i<<" = "<<m_q[i]<<"      "<<m_Vqq[i][0]<<"   "<<m_Vqq[i][1]<<"   "<<m_Vqq[i][2]<<endmsg;
    }
  return out;
}



