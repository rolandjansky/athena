#include "TrkPlanarSurface.h"
#include "TrkTrackState.h"
#include "TrkBaseNode.h"
#include "BFieldPoint.h"
#include "SiCluster.h"
#include "SpacePoint.h"
#include "RecTrack.h"
#include "DkfTrack.h"
#include "TrackFitter.h"
#include <iostream>
#include <math.h>
#include <memory.h>
#include <algorithm>

TrackFitter::TrackFitter(void)
{
}

TrackFitter::~TrackFitter(void)
{
}

DkfTrack* TrackFitter::fit(const RecTrack* inputTrack)
{
  double path,J[5][5];
  int iFirst=0;
  std::vector<TrkTrackState*> vpTrackStates;
  bool OK=true;

  vpTrackStates.clear();

  DkfTrack* pT = createDkfTrack(inputTrack);
  if(pT==NULL) return pT;

  TrkTrackState* pTS = new TrkTrackState(pT->m_pTrackState);
  double Gk[5][5];
  memset(&Gk[0][0],0,sizeof(Gk));
  Gk[0][0]=(double)100.0;Gk[1][1]=(double)100.0;Gk[2][2]=(double)0.01;Gk[3][3]=(double)0.01;Gk[4][4]=(double)1e-6;
  pTS->setTrackCovariance(Gk);
  pTS->setScatteringMode(1);

  TrkPlanarSurface *pSB=NULL;
  iFirst=0;
  vpTrackStates.push_back(pTS);
  int ndof=-5;

  for(std::vector<TrkBaseNode*>::iterator pnIt=pT->m_vpNodes.begin();
      pnIt!=pT->m_vpNodes.end();++pnIt)
  {
    TrkPlanarSurface* pSE = (*pnIt)->getSurface();
    for(int idx=0;idx<3;idx++)
      m_startB[idx] = inputTrack->getField(m_reMapVector[iFirst])->m_field[idx];
    for(int idx=0;idx<3;idx++)
      m_endB[idx] = inputTrack->getField(m_reMapVector[iFirst+1])->m_field[idx];

    TrkTrackState* pNS = extrapolate(pTS,pSB,pSE,J,path);
    pSB=pSE;
    if(pNS!=NULL)
    {
      vpTrackStates.push_back(pNS);
      (*pnIt)->validateMeasurement(pNS);
      if((*pnIt)->isValidated())
      {
        ndof+=(*pnIt)->getNdof();
      }
      //double dChi2=(*pnIt)->getChi2();
      //std::cout<<"dChi2="<<dChi2<<" Ndof="<<(*pnIt)->getNdof()<<std::endl;
      (*pnIt)->updateTrackState(pNS);

      pTS=pNS;
    }
    else
    {
      OK=false;break;
    }
    iFirst++;
  }
  if((OK)&&(ndof>0))
  {
    std::vector<TrkTrackState*>::reverse_iterator ptsIt(vpTrackStates.rbegin()),ptsEnd(vpTrackStates.rend());
    for(;ptsIt!=ptsEnd;++ptsIt)
    {
      (*ptsIt)->runSmoother();
    }
    pTS=(*vpTrackStates.begin());
    pT->m_vpTrackStates.resize(vpTrackStates.size());
    std::copy(vpTrackStates.begin(),vpTrackStates.end(),pT->m_vpTrackStates.begin());
    for(std::vector<TrkBaseNode*>::iterator pnIt=pT->m_vpNodes.begin();pnIt!=pT->m_vpNodes.end();++pnIt)
    {
      if((*pnIt)->isValidated())
        pT->addChi2((*pnIt)->getChi2(),(*pnIt)->getNdof());
    }
  }
  else
  {
    delete pT;pT=NULL;
    for(std::vector<TrkTrackState*>::iterator it = vpTrackStates.begin();it!=vpTrackStates.end();++it)
      delete (*it);
  }
  vpTrackStates.clear();
  return pT;
}

void TrackFitter::matrixInversion5x5(double a[5][5])
{
  /**** 5x5 matrix inversion by Gaussian elimination ****/
  int i,j,k,l;
  double factor;
  double temp[5];
  double b[5][5];
  // Set b to I

  memset(&b[0][0],0,sizeof(b));
  b[0][0]=1.0;b[1][1]=1.0;b[2][2]=1.0;b[3][3]=1.0;b[4][4]=1.0;

  for(i=0;i<5;i++)
  {
    for(j=i+1;j<5;j++)
      if (fabs(a[i][i])<fabs(a[j][i]))
      {
        for(l=0;l<5;l++) temp[l]=a[i][l];
        for(l=0;l<5;l++) a[i][l]=a[j][l];
        for(l=0;l<5;l++) a[j][l]=temp[l];
        for(l=0;l<5;l++) temp[l]=b[i][l];
        for(l=0;l<5;l++) b[i][l]=b[j][l];
        for(l=0;l<5;l++) b[j][l]=temp[l];
      }
    factor=a[i][i];
    for(j=4;j>-1;j--)
    {
      b[i][j]/=factor;a[i][j]/=factor;
    }
    for(j=i+1;j<5;j++)
    {
      factor=-a[j][i];
      for(k=0;k<5;k++)
      {
        a[j][k]+=a[i][k]*factor;b[j][k]+=b[i][k]*factor;
      }
    }
  }
  for(i=4;i>0;i--)
  {
    for(j=i-1;j>-1;j--)
    {
      factor=-a[j][i];
      for(k=0;k<5;k++)
      {
        a[j][k]+=a[i][k]*factor;b[j][k]+=b[i][k]*factor;
      }
    }
  }
  for(i=0;i<5;i++) for(j=0;j<5;j++) a[i][j]=b[i][j];
}

TrkTrackState* TrackFitter::extrapolate(TrkTrackState* pTS,
										  TrkPlanarSurface* pSB,
										  TrkPlanarSurface* pSE,
										  double J[5][5],
										  double& path)
{
  const double C=0.02999975f;
  const double minStep=30.0;

  double sint,cost,sinf,cosf;
  double gP[3],gPi[3],lP[3],gV[3],a,b,c,s,J0[7][5],Rf[5],descr,CQ,Ac,Av,Cc;
  double V[3],P[3],M[3][3],AG[5][5],D[4],Jm[7][7],Gi[5][5],Gf[5][5],
         J1[5][7],A[5][5],gB[3],Buf[5][7],DVx,DVy,DVz;
  int i,j,m,nStep,nStepMax;
  double sl,ds;

  path=0.0;

  sint=sin(pTS->getTrackState(3));cosf=cos(pTS->getTrackState(2));
  sinf=sin(pTS->getTrackState(2));cost=cos(pTS->getTrackState(3));
  gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*pTS->getTrackState(4);

  memset(&J0[0][0],0,sizeof(J0));

  bool doPrint=false;

  gB[0]=0.0f;gB[1]=0.0f;gB[2]=20.84f;

  if(pSB!=NULL)
  {
    double L[3][3];
    lP[0]=pTS->getTrackState(0);lP[1]=pTS->getTrackState(1);lP[2]=0.0;
    pSB->transformPointToGlobal(lP,gP);
    for(i=0;i<3;i++) for(j=0;j<3;j++) L[i][j]=pSB->getInvRotMatrix(i,j);

    J0[0][0]=L[0][0];J0[0][1]=L[0][1];
    J0[1][0]=L[1][0];J0[1][1]=L[1][1];
    J0[2][0]=L[2][0];J0[2][1]=L[2][1];
    J0[3][2]=-sinf*sint;J0[3][3]=cosf*cost;
    J0[4][2]= cosf*sint;J0[4][3]=sinf*cost;
    J0[5][3]=-sint;
    J0[6][4]=1.0;

  }
  else
  {
    gP[0]=-pTS->getTrackState(0)*sinf;
    gP[1]= pTS->getTrackState(0)*cosf;
    gP[2]= pTS->getTrackState(1);

    J0[0][0]=-sinf;J0[0][2]=-pTS->getTrackState(0)*cosf;
    J0[1][0]= cosf;J0[1][2]=-pTS->getTrackState(0)*sinf;
    J0[2][1]=1.0;
    J0[3][2]=-sinf*sint;J0[3][3]=cosf*cost;
    J0[4][2]= cosf*sint;J0[4][3]=sinf*cost;
    J0[5][3]=-sint;
    J0[6][4]=1.0;
    //doPrint=true;
  }
  for(i=0;i<4;i++) D[i]=pSE->getPar(i);
  for(i=0;i<3;i++) gPi[i]=gP[i];

  for(i=0;i<3;i++) gB[i]=0.5f*(m_startB[i]+m_endB[i]);

  c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
  b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
  a=0.5f*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
      gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
      gB[2]*(D[0]*gV[1]-D[1]*gV[0]));

  descr=b*b-4*a*c;

  if(doPrint) {
    std::cout<<"CQ="<<CQ<<" b="<<b<<" c="<<c<<" descr="<<descr<<std::endl;
    //std::cout<<"cx="<<pSE->m_getCenter()[0]<<" cy="<<pSE->m_getCenter()[1]<<" cz="<<pSE->m_getCenter()[2]<<std::endl;
    //std::cout<<"x="<<gV[0]<<" y="<<gV[1]<<" z="<<gV[2]<<" D="<<D[3]<<std::endl;
  }

  if(descr<0.0)
  {
    //      printf("D<0 - extrapolation failed\n");
    return NULL;
  }
  sl=-c/b;
  sl=sl*(1-a*sl/b);

  if(fabs(sl)<minStep) nStepMax=1;
  else
  {
    nStepMax=(int)(fabs(sl)/minStep)+1;
  }
  if((nStepMax<0)||(nStepMax>1000))
  {
    return NULL;
  }
  nStep=nStepMax;path=0.0;
  while(nStep>0)
  {
    c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
    b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
    a=0.5f*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
        gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
        gB[2]*(D[0]*gV[1]-D[1]*gV[0]));
    sl=-c/b;
    sl=sl*(1-a*sl/b);
    ds=sl/nStep;path+=ds;
    Av=ds*CQ;
    Ac=0.5f*ds*Av;
    DVx=gV[1]*gB[2]-gV[2]*gB[1];
    DVy=gV[2]*gB[0]-gV[0]*gB[2];
    DVz=gV[0]*gB[1]-gV[1]*gB[0];

    P[0]=gP[0]+gV[0]*ds+Ac*DVx;
    P[1]=gP[1]+gV[1]*ds+Ac*DVy;
    P[2]=gP[2]+gV[2]*ds+Ac*DVz;
    V[0]=gV[0]+Av*DVx;
    V[1]=gV[1]+Av*DVy;
    V[2]=gV[2]+Av*DVz;
    for(i=0;i<3;i++)
    {
      gV[i]=V[i];gP[i]=P[i];
    }
    nStep--;
  }
  pSE->transformPointToLocal(gP,lP);

  Rf[0]=lP[0];Rf[1]=lP[1];
  Rf[2]=atan2(gV[1],gV[0]);

  if(fabs(gV[2])>1.0)
    return NULL;

  Rf[3]=acos(gV[2]);
  Rf[4]=pTS->getTrackState(4);

  gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;

  for(i=0;i<4;i++) D[i]=pSE->getPar(i);
  for(i=0;i<3;i++) gP[i]=gPi[i];

  c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
  b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
  a=CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
      gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
      gB[2]*(D[0]*gV[1]-D[1]*gV[0]));
  s=-c/b;
  s=s*(1-a*s/b);
  Av=s*CQ;
  Ac=0.5f*s*Av;
  Cc=0.5f*s*s*C;

  DVx=gV[1]*gB[2]-gV[2]*gB[1];
  DVy=gV[2]*gB[0]-gV[0]*gB[2];
  DVz=gV[0]*gB[1]-gV[1]*gB[0];

  P[0]=gP[0]+gV[0]*s+Ac*DVx;
  P[1]=gP[1]+gV[1]*s+Ac*DVy;
  P[2]=gP[2]+gV[2]*s+Ac*DVz;

  V[0]=gV[0]+Av*DVx;V[1]=gV[1]+Av*DVy;V[2]=gV[2]+Av*DVz;

  pSE->transformPointToLocal(P,lP);

  memset(&Jm[0][0],0,sizeof(Jm));

  for(i=0;i<3;i++) for(j=0;j<3;j++) M[i][j]=pSE->getRotMatrix(i,j);

  double coeff[3], dadVx,dadVy,dadVz,dadQ,dsdx,dsdy,dsdz,dsdVx,dsdVy,dsdVz,dsdQ;
  coeff[0]=-c*c/(b*b*b);
  coeff[1]=c*(1+3*c*a/(b*b))/(b*b);
  coeff[2]=-(1+2*c*a/(b*b))/b;

  dadVx=0.5f*CQ*(-D[1]*gB[2]+D[2]*gB[1]);
  dadVy=0.5f*CQ*( D[0]*gB[2]-D[2]*gB[0]);
  dadVz=0.5f*CQ*(-D[0]*gB[1]+D[1]*gB[0]);
  dadQ=0.5f*C*(D[0]*DVx+D[1]*DVy+D[2]*DVz);

  dsdx=coeff[2]*D[0];
  dsdy=coeff[2]*D[1];
  dsdz=coeff[2]*D[2];
  dsdVx=coeff[0]*dadVx+coeff[1]*D[0];
  dsdVy=coeff[0]*dadVy+coeff[1]*D[1];
  dsdVz=coeff[0]*dadVz+coeff[1]*D[2];
  dsdQ=coeff[0]*dadQ;

  Jm[0][0]=1  +V[0]*dsdx;
  Jm[0][1]=    V[0]*dsdy;
  Jm[0][2]=    V[0]*dsdz;

  Jm[0][3]=  s+V[0]*dsdVx;
  Jm[0][4]=    V[0]*dsdVy+Ac*gB[2];
  Jm[0][5]=    V[0]*dsdVz-Ac*gB[1];
  Jm[0][6]=    V[0]*dsdQ+Cc*DVx;

  Jm[1][0]=    V[1]*dsdx;
  Jm[1][1]=1  +V[1]*dsdy;
  Jm[1][2]=    V[1]*dsdz;

  Jm[1][3]=    V[1]*dsdVx-Ac*gB[2];
  Jm[1][4]=  s+V[1]*dsdVy;
  Jm[1][5]=    V[1]*dsdVz+Ac*gB[0];
  Jm[1][6]=    V[1]*dsdQ+Cc*DVy;

  Jm[2][0]=    V[2]*dsdx;
  Jm[2][1]=    V[2]*dsdy;
  Jm[2][2]=1  +V[2]*dsdz;
  Jm[2][3]=    V[2]*dsdVx+Ac*gB[1];
  Jm[2][4]=    V[2]*dsdVy-Ac*gB[0];
  Jm[2][5]=  s+V[2]*dsdVz;
  Jm[2][6]=    V[2]*dsdQ+Cc*DVz;

  Jm[3][0]=dsdx*CQ*DVx;
  Jm[3][1]=dsdy*CQ*DVx;
  Jm[3][2]=dsdz*CQ*DVx;

  Jm[3][3]=1  +dsdVx*CQ*DVx;
  Jm[3][4]=CQ*(dsdVy*DVx+s*gB[2]);
  Jm[3][5]=CQ*(dsdVz*DVx-s*gB[1]);

  Jm[3][6]=(CQ*dsdQ+C*s)*DVx;

  Jm[4][0]=dsdx*CQ*DVy;
  Jm[4][1]=dsdy*CQ*DVy;
  Jm[4][2]=dsdz*CQ*DVy;

  Jm[4][3]=CQ*(dsdVx*DVy-s*gB[2]);
  Jm[4][4]=1  +dsdVy*CQ*DVy;
  Jm[4][5]=CQ*(dsdVz*DVy+s*gB[0]);

  Jm[4][6]=(CQ*dsdQ+C*s)*DVy;

  Jm[5][0]=dsdx*CQ*DVz;
  Jm[5][1]=dsdy*CQ*DVz;
  Jm[5][2]=dsdz*CQ*DVz;
  Jm[5][3]=CQ*(dsdVx*DVz+s*gB[1]);
  Jm[5][4]=CQ*(dsdVy*DVz-s*gB[0]);
  Jm[5][5]=1  +dsdVz*CQ*DVz;
  Jm[5][6]=(CQ*dsdQ+C*s)*DVz;

  Jm[6][6]=1.0f;

  memset(&J1[0][0],0,sizeof(J1));

  J1[0][0]=M[0][0];J1[0][1]=M[0][1];J1[0][2]=M[0][2];
  J1[1][0]=M[1][0];J1[1][1]=M[1][1];J1[1][2]=M[1][2];
  J1[2][3]=-V[1]/(V[0]*V[0]+V[1]*V[1]);
  J1[2][4]= V[0]/(V[0]*V[0]+V[1]*V[1]);
  J1[3][5]=-1/sqrt(1-V[2]*V[2]);
  J1[4][6]=1.0f;


  for(i=0;i<7;i++)
  {
    for(j=0;j<2;j++)
      Buf[j][i]=J1[j][0]*Jm[0][i]+J1[j][1]*Jm[1][i]+J1[j][2]*Jm[2][i];
    Buf[2][i]=J1[2][3]*Jm[3][i]+J1[2][4]*Jm[4][i];
    Buf[3][i]=J1[3][5]*Jm[5][i];
    Buf[4][i]=Jm[6][i];
  }

  if(pSB!=NULL)
  {
    for(i=0;i<5;i++)
    {
      J[i][0]=Buf[i][0]*J0[0][0]+Buf[i][1]*J0[1][0]+Buf[i][2]*J0[2][0];
      J[i][1]=Buf[i][0]*J0[0][1]+Buf[i][1]*J0[1][1]+Buf[i][2]*J0[2][1];
      J[i][2]=Buf[i][3]*J0[3][2]+Buf[i][4]*J0[4][2];
      J[i][3]=Buf[i][3]*J0[3][3]+Buf[i][4]*J0[4][3]+Buf[i][5]*J0[5][3];
      J[i][4]=Buf[i][6];
    }
  }
  else
  {
    for(i=0;i<5;i++)
    {
      J[i][0]=Buf[i][0]*J0[0][0]+Buf[i][1]*J0[1][0];
      J[i][1]=Buf[i][2];
      J[i][2]=Buf[i][0]*J0[0][2]+Buf[i][1]*J0[1][2]+Buf[i][3]*J0[3][2]+Buf[i][4]*J0[4][2];
      J[i][3]=Buf[i][3]*J0[3][3]+Buf[i][4]*J0[4][3]+Buf[i][5]*J0[5][3];
      J[i][4]=Buf[i][6];
    }
  }
  for(i=0;i<5;i++) for(j=0;j<5;j++)
  {
    AG[i][j]=0.0;for(m=0;m<5;m++) AG[i][j]+=J[i][m]*pTS->getTrackCovariance(m,j);
  }
  for(i=0;i<5;i++) for(j=i;j<5;j++)
  {
    Gf[i][j]=0.0;
    for(m=0;m<5;m++) Gf[i][j]+=AG[i][m]*J[j][m];
    Gf[j][i]=Gf[i][j];
  }

  TrkTrackState* pTE=new TrkTrackState(pTS);
  pTE->setTrackState(Rf);
  pTE->setTrackCovariance(Gf);
  pTE->attachToSurface(pSE);
  pTE->applyMaterialEffects();

  for(i=0;i<5;i++) for(j=0;j<5;j++)
  {
    Gi[i][j]=pTE->getTrackCovariance(i,j);
  }
  matrixInversion5x5(Gi);
  for(i=0;i<5;i++) for(j=0;j<5;j++)
  {
    A[i][j]=0.0;
    for(m=0;m<5;m++) A[i][j]+=AG[m][i]*Gi[m][j];
  }
  pTE->setPreviousState(pTS);
  pTE->setSmootherGain(A);

  return pTE;
}

DkfTrack* TrackFitter::createDkfTrack(const RecTrack* inputTrack)
{
  int fieldIndex=1;

  if(inputTrack->getSpacePoints().size()<3)
    return NULL;

  m_reMapVector.clear();

  TrkTrackState* pTS = new TrkTrackState(inputTrack->getParameters());
  DkfTrack* pT = new DkfTrack(pTS,inputTrack);
  m_reMapVector.push_back(0);
  const std::vector<const SpacePoint*> spv = inputTrack->getSpacePoints();
  for(std::vector<const SpacePoint*>::const_iterator spIt=spv.begin();spIt!=spv.end();++spIt)
  {
    for(std::vector<const SiCluster*>::const_iterator clIt=(*spIt)->m_clusters.begin();clIt!=(*spIt)->m_clusters.end();++clIt)
    {
      const SiCluster *pCL=(*clIt);
      TrkBaseNode *pN = pCL->createDkfNode();
      if(pN!=NULL)
      {
        pT->m_vpNodes.push_back(pN);
        pT->m_vpSurfaces.push_back(pN->getSurface());
        m_reMapVector.push_back(fieldIndex);
      }
    }
    fieldIndex++;
  }
  return pT;
}
