/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetTrackFitter/PerigeeFilteringNodes.h"

BarrelPerigeeFilteringNode::BarrelPerigeeFilteringNode(const TrigSiSpacePoint* psp)
{
  m_Ri=(*psp).r();
  m_yPhi=(*psp).phi();
  m_yZ=(*psp).z();
  m_sigmaPhi=(*psp).dphi();
  m_sigmaZ=(*psp).dz();
  m_setType(0);
  m_pTrigSp=psp;
  m_acceptIt();
}

DiscPerigeeFilteringNode::DiscPerigeeFilteringNode(const TrigSiSpacePoint* psp)
{
  m_Zi=(*psp).z();
  m_yPhi=(*psp).phi();
  m_yR=(*psp).r();
  m_sigmaPhi=(*psp).dphi();
  m_sigmaR=(*psp).dr();
  m_setType(1);
  m_pTrigSp=psp;
  m_acceptIt();
}

BasePerigeeFilteringNode::BasePerigeeFilteringNode()
{
  m_nStatus=1; m_pTrigSp=NULL;
}

void BasePerigeeFilteringNode::m_rejectIt()
{
  m_nStatus=0;
}

void BasePerigeeFilteringNode::m_acceptIt()
{
  m_nStatus=1;
}

bool BasePerigeeFilteringNode::m_isAccepted()
{
  return (m_nStatus==1);
}

void BasePerigeeFilteringNode::m_runFilter(double* Rk, double* Gk)
{
  double Kk[5][2];
  int i,j,m,ind;

  for(i=0;i<5;i++)
    {
      for(j=0;j<2;j++)
	{
	  Kk[i][j]=0.0;
	  for(m=0;m<2;m++) Kk[i][j]+=m_B[m][i]*m_D[m][j];
	}
    }
  for(i=0;i<5;i++) Rk[i]+=Kk[i][0]*m_resid[0]+Kk[i][1]*m_resid[1];
  ind=0;
  for(j=0;j<5;j++)
    for(i=0;i<=j;i++)
      {
	Gk[ind]-=Kk[i][0]*m_B[0][j]+Kk[i][1]*m_B[1][j];
	ind++;
      }
}

void BasePerigeeFilteringNode::m_applyMultScatt(double* Rk,double* Gk)
{
  double s2,pt,sint,l1;
  double F[5][2];
  double rk;

  pt=-0.3/Rk[0];
  sint=1+Rk[4]*Rk[4];
  pt*=(pt*sint);
  s2=m_EffSigmaMS*m_EffSigmaMS/pt;
  rk=Rk[0]*Rk[0]*m_Rho*m_Rho;
  l1=1.0/(1.0-4*rk);
  F[0][0]=0.0;
  F[0][1]=-Rk[0]*Rk[4];
  F[1][0]=m_Rho*(1-0.5*rk);
  F[1][1]=rk*Rk[4]/Rk[0];
  F[2][0]=(1-2*rk)*l1;
  F[2][1]=-F[0][1]*2*m_Rho*(1-0.5*rk)*l1;
  F[3][0]=-F[0][1]*m_Rho*l1;
  F[3][1]=m_Rho*sint;
  F[4][0]=0.0;
  F[4][1]=-sint;
  int i;
  sint=sqrt(sint);
  for(i=0;i<5;i++) F[i][0]*=sint;
  Gk[0]+=s2*F[0][1]*F[0][1];
  Gk[1]+=s2*F[0][1]*F[1][1];
  Gk[2]+=s2*(F[1][0]*F[1][0]+F[1][1]*F[1][1]);
  Gk[3]+=s2*F[0][1]*F[2][1];
  Gk[4]+=s2*(F[1][0]*F[2][0]+F[1][1]*F[2][1]);
  Gk[5]+=s2*(F[2][0]*F[2][0]+F[2][1]*F[2][1]);
  Gk[6]+=s2*F[0][1]*F[3][1];
  Gk[7]+=s2*(F[1][0]*F[3][0]+F[1][1]*F[3][1]);
  Gk[8]+=s2*(F[2][0]*F[3][0]+F[2][1]*F[3][1]);
  Gk[9]+=s2*(F[3][0]*F[3][0]+F[3][1]*F[3][1]);
  Gk[10]+=s2*F[0][1]*F[4][1];
  Gk[11]+=s2*F[1][1]*F[4][1];
  Gk[12]+=s2*F[2][1]*F[4][1];
  Gk[13]+=s2*F[3][1]*F[4][1];
  Gk[14]+=s2*F[4][1]*F[4][1];
}

void BasePerigeeFilteringNode::m_applyEnergyLoss(double* Rk, double* Gk, int dir)
{
  double alpha,k0,rho2,rho3,rho4,sigma2;

  alpha=m_EffRadLength*(1-0.5*m_EffRadLength);
  sigma2=Rk[0]*Rk[0]*m_EffRadLength*(0.415-0.744*m_EffRadLength);
  rho2=m_Rho*m_Rho;
  rho3=m_Rho*rho2;
  rho4=m_Rho*rho3;
  k0=Rk[0]*dir*alpha;
  Rk[0]+=k0;
  Rk[1]+=-k0*rho2;
  Rk[2]+=-k0*2*m_Rho;
  Gk[0]+=sigma2;
  Gk[1]+=-rho2*sigma2;
  Gk[2]+=rho4*sigma2;
  Gk[3]+=-2*m_Rho*sigma2;
  Gk[4]+=2*rho3*sigma2;
  Gk[5]+=4*rho2*sigma2;
}


double BarrelPerigeeFilteringNode::m_getChi2(double* Rk, double* Gk)
{
  double chi2=0.0,zres,detr;
  double H[2][5];
  double V[2][2];
  
  double h1,h2,h3,sinz,cosz;

  zres=m_yZ-Rk[3];
  h3=zres*2*Rk[0]/Rk[4];
  sinz=sin(h3);cosz=cos(h3);
  h1=1.0/(1.0-2*Rk[1]*Rk[0]);
  h2=(Rk[1]-m_Ri)*(Rk[1]+m_Ri);
  H[0][0]=h1*h1*h2;
  H[0][1]=h1+2*Rk[0]*Rk[0]*H[0][0];
  H[0][2]=-m_Ri*cos(m_yPhi-Rk[2]);
  H[1][0]=0.5*(sinz-h3*cosz)/(Rk[0]*Rk[0]);
  H[1][2]=m_Ri*sin(m_yPhi-Rk[2]);
  H[1][3]=cosz/Rk[4];
  H[1][4]=zres*cosz/(Rk[4]*Rk[4]);

  m_B[0][0]=H[0][0]*Gk[0]+H[0][1]*Gk[1]+H[0][2]*Gk[3];
  m_B[0][1]=H[0][0]*Gk[1]+H[0][1]*Gk[2]+H[0][2]*Gk[4];
  m_B[0][2]=H[0][0]*Gk[3]+H[0][1]*Gk[4]+H[0][2]*Gk[5];
  m_B[0][3]=H[0][0]*Gk[6]+H[0][1]*Gk[7]+H[0][2]*Gk[8];
  m_B[0][4]=H[0][0]*Gk[10]+H[0][1]*Gk[11]+H[0][2]*Gk[12];

  m_B[1][0]=H[1][0]*Gk[0]+H[1][2]*Gk[3]+H[1][3]*Gk[6]+H[1][4]*Gk[10];
  m_B[1][1]=H[1][0]*Gk[1]+H[1][2]*Gk[4]+H[1][3]*Gk[7]+H[1][4]*Gk[11];
  m_B[1][2]=H[1][0]*Gk[3]+H[1][2]*Gk[5]+H[1][3]*Gk[8]+H[1][4]*Gk[12];
  m_B[1][3]=H[1][0]*Gk[6]+H[1][2]*Gk[8]+H[1][3]*Gk[9]+H[1][4]*Gk[13];
  m_B[1][4]=H[1][0]*Gk[10]+H[1][2]*Gk[12]+H[1][3]*Gk[13]+H[1][4]*Gk[14];

  V[0][0]=m_sigmaPhi*H[0][2];
  V[0][0]=V[0][0]*V[0][0];
  V[0][1]=H[0][2]*H[1][2]*m_sigmaPhi*m_sigmaPhi;V[1][0]=V[0][1];
  V[1][1]=m_sigmaZ*m_sigmaZ*H[1][3]*H[1][3]+m_sigmaPhi*m_sigmaPhi*
    H[1][2]*H[1][2];

  V[0][0]+=m_B[0][0]*H[0][0]+m_B[0][1]*H[0][1]+m_B[0][2]*H[0][2];
  V[1][1]+=m_B[1][0]*H[1][0]+m_B[1][2]*H[1][2]+m_B[1][3]*H[1][3]+
    m_B[1][4]*H[1][4];
  V[0][1]+=m_B[0][0]*H[1][0]+m_B[0][2]*H[1][2]+m_B[0][3]*H[1][3]+
    m_B[0][4]*H[1][4];
  V[1][0]+=m_B[1][0]*H[0][0]+m_B[1][1]*H[0][1]+m_B[1][2]*H[0][2];
  detr=V[0][0]*V[1][1]-V[0][1]*V[0][1];
  detr=1.0/detr;
  m_D[0][0]=V[1][1]*detr;m_D[1][1]=V[0][0]*detr;m_D[0][1]=-V[0][1]*detr;
  m_D[1][0]=m_D[0][1];
  m_resid[0]=-H[1][2]-h1*(Rk[1]-Rk[0]*(Rk[1]*Rk[1]+m_Ri*m_Ri));
  m_resid[1]=H[0][2]+0.5*sinz/Rk[0];
  chi2=m_resid[0]*m_resid[0]*m_D[0][0]+2.0*m_resid[0]*m_resid[1]*m_D[0][1]+
    m_resid[1]*m_resid[1]*m_D[1][1];
  return chi2;
}

void BarrelPerigeeFilteringNode::m_applyMultScatt(double* Rk, double* Gk)
{
  m_EffRadLength=m_getEffRadLength(Rk);
  m_EffSigmaMS=0.0136*sqrt(m_EffRadLength)*
    (1.0+0.038*log(m_EffRadLength));
  m_Rho=m_Ri;
  BasePerigeeFilteringNode::m_applyMultScatt(Rk,Gk);
}

void BarrelPerigeeFilteringNode::m_applyEnergyLoss(double* Rk, double* Gk, int dir)
{
  m_EffRadLength=m_getEffRadLength(Rk);
  m_Rho=m_Ri;
  BasePerigeeFilteringNode::m_applyEnergyLoss(Rk,Gk,dir);
}

void BarrelPerigeeFilteringNode::m_runFilter(double* Rk, double* Gk)
{
  BasePerigeeFilteringNode::m_runFilter(Rk,Gk);
}

int BarrelPerigeeFilteringNode::m_getNdof()
{
  return 2;
}

double BarrelPerigeeFilteringNode::m_getEffRadLength(double* Rk)
{
  const double x0=0.022;

  double x0eff=x0*sqrt(1+Rk[4]*Rk[4]);
  return x0eff;
}

double DiscPerigeeFilteringNode::m_getChi2(double* Rk, double* Gk)
{
  double chi2=0.0,zres,detr;
  double H[2][5];
  double V[2][2];  
  double h1,h2,h3,sinz,cosz,dhdR1,dhdR2;

  zres=m_Zi-Rk[3];
  h3=zres*2*Rk[0]/Rk[4];
  sinz=sin(h3);cosz=cos(h3);
  h1=1.0/(1.0-2*Rk[1]*Rk[0]);
  h2=(Rk[1]-m_yR)*(Rk[1]+m_yR);
  dhdR1=sin(m_yPhi-Rk[2]);
  dhdR2=cos(m_yPhi-Rk[2]);
  H[0][0]=h1*h1*h2;
  H[0][1]=h1+2*Rk[0]*Rk[0]*H[0][0];
  H[0][2]=-m_yR*dhdR2;
  H[1][0]=0.5*(sinz-h3*cosz)/(Rk[0]*Rk[0]);
  H[1][2]=m_yR*dhdR1;
  H[1][3]=cosz/Rk[4];
  H[1][4]=zres*cosz/(Rk[4]*Rk[4]);
  dhdR1=sin(m_yPhi-Rk[2]);
  dhdR2=cos(m_yPhi-Rk[2]);

  m_B[0][0]=H[0][0]*Gk[0]+H[0][1]*Gk[1]+H[0][2]*Gk[3];
  m_B[0][1]=H[0][0]*Gk[1]+H[0][1]*Gk[2]+H[0][2]*Gk[4];
  m_B[0][2]=H[0][0]*Gk[3]+H[0][1]*Gk[4]+H[0][2]*Gk[5];
  m_B[0][3]=H[0][0]*Gk[6]+H[0][1]*Gk[7]+H[0][2]*Gk[8];
  m_B[0][4]=H[0][0]*Gk[10]+H[0][1]*Gk[11]+H[0][2]*Gk[12];

  m_B[1][0]=H[1][0]*Gk[0]+H[1][2]*Gk[3]+H[1][3]*Gk[6]+H[1][4]*Gk[10];
  m_B[1][1]=H[1][0]*Gk[1]+H[1][2]*Gk[4]+H[1][3]*Gk[7]+H[1][4]*Gk[11];
  m_B[1][2]=H[1][0]*Gk[3]+H[1][2]*Gk[5]+H[1][3]*Gk[8]+H[1][4]*Gk[12];
  m_B[1][3]=H[1][0]*Gk[6]+H[1][2]*Gk[8]+H[1][3]*Gk[9]+H[1][4]*Gk[13];
  m_B[1][4]=H[1][0]*Gk[10]+H[1][2]*Gk[12]+H[1][3]*Gk[13]+H[1][4]*Gk[14];

  V[0][0]=m_sigmaPhi*m_sigmaPhi*H[0][2]*H[0][2]+
    m_sigmaR*m_sigmaR*dhdR1*dhdR1;
  V[0][1]=H[0][2]*H[1][2]*m_sigmaPhi*m_sigmaPhi+
    dhdR1*dhdR2*m_sigmaR*m_sigmaR;
  V[1][0]=V[0][1];
  V[1][1]=m_sigmaR*m_sigmaR*dhdR2*dhdR2+m_sigmaPhi*m_sigmaPhi*
    H[1][2]*H[1][2];

  V[0][0]+=m_B[0][0]*H[0][0]+m_B[0][1]*H[0][1]+m_B[0][2]*H[0][2];
  V[1][1]+=m_B[1][0]*H[1][0]+m_B[1][2]*H[1][2]+m_B[1][3]*H[1][3]+
    m_B[1][4]*H[1][4];
  V[0][1]+=m_B[0][0]*H[1][0]+m_B[0][2]*H[1][2]+m_B[0][3]*H[1][3]+
    m_B[0][4]*H[1][4];
  V[1][0]+=m_B[1][0]*H[0][0]+m_B[1][1]*H[0][1]+m_B[1][2]*H[0][2];
  detr=V[0][0]*V[1][1]-V[0][1]*V[0][1];
  detr=1.0/detr;
  m_D[0][0]=V[1][1]*detr;m_D[1][1]=V[0][0]*detr;m_D[0][1]=-V[0][1]*detr;
  m_D[1][0]=m_D[0][1];
  m_resid[0]=-H[1][2]-h1*(Rk[1]-Rk[0]*(Rk[1]*Rk[1]+m_yR*m_yR));
  m_resid[1]=H[0][2]+0.5*sinz/Rk[0];
  chi2=m_resid[0]*m_resid[0]*m_D[0][0]+2.0*m_resid[0]*m_resid[1]*m_D[0][1]+
    m_resid[1]*m_resid[1]*m_D[1][1];

  return chi2;
}

void DiscPerigeeFilteringNode::m_applyMultScatt(double* Rk, double* Gk)
{
  m_EffRadLength=m_getEffRadLength(Rk);
  m_EffSigmaMS=0.0136*sqrt(m_EffRadLength)*
    (1.0+0.038*log(m_EffRadLength));
  m_Rho=m_yR;
  BasePerigeeFilteringNode::m_applyMultScatt(Rk,Gk);
}

void DiscPerigeeFilteringNode::m_applyEnergyLoss(double* Rk, double* Gk, int dir)
{
  m_EffRadLength=m_getEffRadLength(Rk);
  m_Rho=m_yR;
  BasePerigeeFilteringNode::m_applyEnergyLoss(Rk,Gk,dir);
}

void DiscPerigeeFilteringNode::m_runFilter(double* Rk, double* Gk)
{
  BasePerigeeFilteringNode::m_runFilter(Rk,Gk);
}

int DiscPerigeeFilteringNode::m_getNdof()
{
  return 2;
}

double DiscPerigeeFilteringNode::m_getEffRadLength(double* Rk)
{
  const double x0=0.022;

  double x0eff=x0/(fabs(Rk[4])*sqrt(1+Rk[4]*Rk[4]));
  return x0eff;
}
