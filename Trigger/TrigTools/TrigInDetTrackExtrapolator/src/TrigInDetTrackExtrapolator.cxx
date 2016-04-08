/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigInDetTrackExtrapolator tool
// -------------------------------
// ATLAS Collaboration
//
// 15.09.2006 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackExtrapolator.h"
#include "TrigInDetTrackExtrapolator/TrigInDetTrackExtrapolator.h"

TrigInDetTrackExtrapolator::TrigInDetTrackExtrapolator(const std::string& t, 
						       const std::string& n,
						       const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_MagFieldSvc("AtlasFieldSvc",this->name())
{
  declareInterface< ITrigInDetTrackExtrapolator >( this );
  declareProperty( "AthenaFieldService", m_MagFieldSvc, "AtlasFieldSvc");
  declareProperty( "MinPt", m_minPt = 500.0);
}

StatusCode TrigInDetTrackExtrapolator::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());
  m_outputLevel=msgSvc()->outputLevel( name() );
  
	athenaLog << MSG::INFO <<"Using Athena magnetic field service"<<endreq;
	sc = m_MagFieldSvc.retrieve();
	if(sc.isFailure()) 
	{
		athenaLog << MSG::ERROR << "Unable to retrieve Athena MagFieldService" << endreq;
		return StatusCode::FAILURE;
	}
  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
//   add some timers:
//
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("TrigExtrap");
  }
  athenaLog << MSG::INFO << "TrigInDetTrackExtrapolator constructed "<< endreq;
  return sc;
}

StatusCode TrigInDetTrackExtrapolator::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigInDetTrackExtrapolator::~TrigInDetTrackExtrapolator()
{

}

Trk::TrkPlanarSurface* TrigInDetTrackExtrapolator::m_createEndSurfaceForBarrel(double pTS[6],double R)
{
  const double C=0.0299997;
  double Phi,sT,rt,Delta,DeltaPhi,sinf,cosf,Bz,PhiT,
    P[3],B[3],s,cosB,sinB,Beta;
  double Center[3],N[3],M[3][3];

  rt=sqrt(pTS[0]*pTS[0]+pTS[1]*pTS[1]);

  PhiT=atan2(pTS[1],pTS[0]);
  Beta=pTS[3]-PhiT;
  cosB=cos(Beta);sinB=sin(Beta);

  for(int i=0;i<3;i++) P[i]=pTS[i];
  m_getMagneticField(P,&B[0]);
  Bz=B[2];

  sinf=sin(pTS[3]);cosf=cos(pTS[3]);

  double dR=R*R-rt*rt*sinB*sinB;
  if(dR<0.0) return NULL;
  sT = sqrt(dR)-rt*cosB;

  DeltaPhi=-pTS[5]*sT*Bz*C*cos(pTS[4])/sin(pTS[4]);
  Delta=0.5*DeltaPhi*sT*cos(pTS[4]);
  s=sqrt(sT*sT+Delta*Delta);
  pTS[2]+=s*cos(pTS[4])/sin(pTS[4]);
      
  pTS[0]+=sT*cosf-Delta*sinf;
  pTS[1]+=sT*sinf+Delta*cosf;
  pTS[3]+=DeltaPhi;
  Phi=pTS[3];

  Center[0]=pTS[0];
  Center[1]=pTS[1];
  Center[2]=pTS[2];

  N[0]=cos(Phi);
  N[1]=sin(Phi);
  N[2]=0.0;

  M[0][0]=-sin(Phi); M[0][1]= 0.0; M[0][2]=cos(Phi);
  M[1][0]= cos(Phi); M[1][1]= 0.0; M[1][2]=sin(Phi);
  M[2][0]= 0.0;      M[2][1]= 1.0; M[2][2]=0.0;
  
  return (new Trk::TrkPlanarSurface(Center,N,M,0.0));
}


TrigInDetTrackFitPar* TrigInDetTrackExtrapolator::extrapolateToBarrel(const TrigInDetTrack* pT, 
								      double Radius)
{  
  if(m_timers) m_timer[0]->start();

  MsgStream athenaLog(msgSvc(), name());

  Trk::TrkPlanarSurface* pSB=NULL;
  Trk::TrkPlanarSurface* pSE=NULL;
  Trk::TrkTrackState* pInitState=NULL;
  TrigInDetTrackFitPar* pEP=NULL;

  const TrigInDetTrackFitPar* pP=pT->param();

  if(pP==NULL)
    {
	  if (m_outputLevel <= MSG::ERROR) 
	    athenaLog << MSG::ERROR << "TrigInDetTrack has no parameters - extrapolation failed" 
		      << endreq;
	  return NULL;
    }
  if(fabs(pP->pT())<m_minPt)
    {

      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Track pT is too low - skipping extrapolation" << endreq;
      return NULL;
    }

  if(pP->surfaceType()==TrigInDetTrackFitPar::PERIGEE)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Parameters at perigee are retrieved" << endreq;
      double Rk[6],P[5],Theta;

      P[0]=pP->a0();
      P[1]=pP->z0();
      Theta=2.0*atan(exp(-pP->eta()));
      P[2]=pP->phi0();
      P[3]=Theta;
      P[4]=sin(Theta)/pP->pT();

      Rk[0]=-pP->a0()*sin(pP->phi0());
      Rk[1]= pP->a0()*cos(pP->phi0());
      Rk[2]=pP->z0();
      Rk[3]=pP->phi0();
      Rk[4]=Theta;
      Rk[5]=sin(Theta)/pP->pT();

      pInitState = new Trk::TrkTrackState(P);
      pSE = m_createEndSurfaceForBarrel(Rk,Radius);
    }
  else
    {
      if(pP->surfaceType()==TrigInDetTrackFitPar::BARREL)
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << "Parameters at cylinder surface R="<<pP->surfaceCoordinate()<<
	      " are retrieved"<< endreq;
	}
      else 
	{
	  if(pP->surfaceType()==TrigInDetTrackFitPar::ENDCAP)
	    {
	      if (m_outputLevel <= MSG::DEBUG) 
		athenaLog << MSG::DEBUG << "Parameters at disk surface Z="<<pP->surfaceCoordinate()<<
		  " are retrieved"<< endreq;
	    }
	  else 
	    {
	      if (m_outputLevel <= MSG::ERROR) 
		athenaLog << MSG::ERROR << "TrigInDetTrackFitPar type is unknown - extrapolation failed" 
			  << endreq;
	      return NULL;
	    }
	}
    }

  Trk::TrkTrackState* pFinalState=m_integrate(pInitState,pSB,pSE);

  if(pFinalState!=NULL)
    {
      double X[3],Y[3];

      X[0]=pFinalState->m_getTrackState(0);
      X[1]=pFinalState->m_getTrackState(1);
      X[2]=0.0;
      pSE->m_transformPointToGlobal(X,Y);

      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Track coordinates at R="<<Radius<<
	  "  X="<<Y[0]<<"  Y="<<Y[1]<<"  Z="<<Y[2]<<"  r="<<sqrt(Y[0]*Y[0]+Y[1]*Y[1])<<endreq;

      double Phi0=atan2(Y[1],Y[0]);
      double eta=-log(sin(0.5*pFinalState->m_getTrackState(3))/cos(0.5*pFinalState->m_getTrackState(3)));

      double theta=atan(Radius/fabs(Y[2]))/2.0;

      eta=-log(sin(theta)/cos(theta));
      if(Y[2]<0.0) eta=-eta;

      double track_pT=pFinalState->m_getTrackState(4)*sin(pFinalState->m_getTrackState(3));

      pEP=new TrigInDetTrackFitPar(Phi0,pFinalState->m_getTrackState(2),
				   Y[2],eta,track_pT,
				   TrigInDetTrackFitPar::BARREL,Radius);
      delete pFinalState;
    }
  delete pInitState;
  if(pSB!=NULL) delete pSB;
  delete pSE;

  if(m_timers) m_timer[0]->stop();
  return pEP;

}


TrigInDetTrackFitPar* TrigInDetTrackExtrapolator::extrapolateToEndcap(const TrigInDetTrack* pT, 
								      double Zcoordinate)
{  
  if(m_timers) m_timer[0]->start();

  MsgStream athenaLog(msgSvc(), name());
  Trk::TrkPlanarSurface* pSB=NULL;
  Trk::TrkPlanarSurface* pSE=NULL;
  Trk::TrkTrackState* pInitState=NULL;
  TrigInDetTrackFitPar* pEP=NULL;

  double C[3],N[3],M[3][3];

  const TrigInDetTrackFitPar* pP=pT->param();

  if(pP==NULL)
    {
	  if (m_outputLevel <= MSG::ERROR) 
	    athenaLog << MSG::ERROR << "TrigInDetTrack has no parameters - extrapolation failed" 
		      << endreq;
	  return NULL;
    }

  if(fabs(pP->pT())<m_minPt)
    {

      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Track pT is too low - skipping extrapolation" << endreq;
      return NULL;
    }

  if(pP->surfaceType()==TrigInDetTrackFitPar::PERIGEE)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Parameters at perigee point are retrieved" << endreq;
      double Rk[5],Theta;

      Rk[0]=pP->a0();
      Rk[1]=pP->z0();
      Rk[2]=pP->phi0();
      Theta=2.0*atan(exp(-pP->eta()));
      Rk[3]=Theta;
      Rk[4]=sin(Theta)/pP->pT();
      pInitState = new Trk::TrkTrackState(Rk);

      C[0]=0.0;
      C[1]=0.0;
      C[2]=Zcoordinate;

      N[0]=0.0;
      N[1]=0.0;
      N[2]=(Zcoordinate<0.0)?-1.0:1.0;

      M[0][0]=1.0;M[1][1]=1.0;M[2][2]=1.0;
      if(Zcoordinate<0.0) 
	{
	  M[0][0]=-1.0;M[2][2]=-1.0;
	}
      M[0][1]=0.0;M[0][2]=0.0;
      M[1][0]=0.0;M[1][2]=0.0;
      M[2][0]=0.0;M[2][1]=0.0;

      pSE=new Trk::TrkPlanarSurface(C,N,M,0.0);

    }
  else
    {
      if(pP->surfaceType()==TrigInDetTrackFitPar::BARREL)
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << "Parameters at cylinder surface R="<<pP->surfaceCoordinate()<<
	      " are retrieved"<< endreq;
	}
      else 
	{
	  if(pP->surfaceType()==TrigInDetTrackFitPar::ENDCAP)
	    {
	      if (m_outputLevel <= MSG::DEBUG) 
		athenaLog << MSG::DEBUG << "Parameters at disk surface Z="<<pP->surfaceCoordinate()<<
		  " are retrieved"<< endreq;
	    }
	  else 
	    {
	      if (m_outputLevel <= MSG::ERROR) 
		athenaLog << MSG::ERROR << "TrigInDetTrackFitPar type is unknown - extrapolation failed" 
			  << endreq;
	      return NULL;
	    }
	}
    }

  Trk::TrkTrackState* pFinalState=m_integrate(pInitState,pSB,pSE);

  if(pFinalState!=NULL)
    {
      double X[3],Y[3];
      X[0]=pFinalState->m_getTrackState(0);
      X[1]=pFinalState->m_getTrackState(1);
      X[2]=0.0;
      pSE->m_transformPointToGlobal(X,Y);

      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Track coordinates at Z="<<Zcoordinate<<
	  "  X="<<Y[0]<<"  Y="<<Y[1]<<"  Z="<<Y[2]<<"  r="<<sqrt(Y[0]*Y[0]+Y[1]*Y[1])<<endreq;

      double Phi0=atan2(Y[1],Y[0]);
      double R0=sqrt(Y[0]*Y[0]+Y[1]*Y[1]);
      double eta=-log(sin(0.5*pFinalState->m_getTrackState(3))/cos(0.5*pFinalState->m_getTrackState(3)));
      double track_pT=pFinalState->m_getTrackState(4)*sin(pFinalState->m_getTrackState(3));

      double theta=atan(R0/fabs(Zcoordinate))/2.0;
      eta=-log(sin(theta)/cos(theta));
      if(Zcoordinate<0.0) eta=-eta;

      pEP=new TrigInDetTrackFitPar(Phi0,pFinalState->m_getTrackState(2),
				   R0,eta,track_pT,
				   TrigInDetTrackFitPar::ENDCAP,Zcoordinate);

      delete pFinalState;
    }
  delete pInitState;
  if(pSB!=NULL) delete pSB;
  delete pSE;

  if(m_timers) m_timer[0]->stop();
  return pEP;
}



TrigInDetTrackFitPar* TrigInDetTrackExtrapolator::extrapolateBlindly(const TrigInDetTrack* pT,
								     double Radius,
								     double Zcoordinate)
{ 
  const double C=0.02999975;
  const double minStep=30.0;
  const int MaxStepNum=10000;


  if(m_timers) m_timer[0]->start();

  MsgStream athenaLog(msgSvc(), name());

  const TrigInDetTrackFitPar* pP=pT->param();
  bool isBarrel=false, isEndcap=false;
  TrigInDetTrackFitPar* pEP=NULL;
	  
  double sint,cost,sinf,cosf,Theta;
  double gP[3],gV[3],CQ,Ac,Av;
  double V[3],P[3],gB[3],DVx,DVy,DVz;
  int i,nStep;
  double ds,path=0.0;

  Theta=2.0*atan(exp(-pP->eta()));

  sint=sin(Theta);cosf=cos(pP->phi0());
  sinf=sin(pP->phi0());cost=cos(Theta);

  gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*sint/pP->pT();
  gP[2]=pP->z0();gP[0]=-pP->a0()*sinf;gP[1]= pP->a0()*cosf;

  m_getMagneticField(gP,gB);
 
  nStep=0;path=0.0;
  ds=minStep;

  while(nStep<MaxStepNum)
    {
      Av=ds*CQ;
      Ac=0.5*ds*Av;
      DVx=gV[1]*gB[2]-gV[2]*gB[1];
      DVy=gV[2]*gB[0]-gV[0]*gB[2];
      DVz=gV[0]*gB[1]-gV[1]*gB[0];

      P[0]=gP[0]+gV[0]*ds+Ac*DVx;
      P[1]=gP[1]+gV[1]*ds+Ac*DVy;
      P[2]=gP[2]+gV[2]*ds+Ac*DVz;
      V[0]=gV[0]+Av*DVx;
      V[1]=gV[1]+Av*DVy;
      V[2]=gV[2]+Av*DVz;

      double R=sqrt(P[0]*P[0]+P[1]*P[1]);
      if(fabs(R-Radius)<1.0)
	{
	  isBarrel=true;
	  break;
	}
      if(R>Radius)
	{
	  ds=ds/2.0;continue;
	}
      if(fabs(P[2]-Zcoordinate)<1.0)
	{
	  isEndcap=true;
	  break;
	}
      if(fabs(P[2])>fabs(Zcoordinate))
	{
	  ds=ds/2.0;continue;
	}

      for(i=0;i<3;i++) 
	{
	  gV[i]=V[i];gP[i]=P[i];path+=ds;
	}
      m_getMagneticField(gP,gB);
      nStep++;
    }

  for(i=0;i<3;i++) 
    {
      gV[i]=V[i];gP[i]=P[i];
    }

  if(m_timers) m_timer[0]->stop();
  if(nStep==MaxStepNum)
    return NULL;

  if(fabs(gV[2])>1.0) return NULL;

  if(isEndcap)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Track coordinates at Z="<<Zcoordinate<<
	  "  X="<<gP[0]<<"  Y="<<gP[1]<<"  Z="<<gP[2]<<"  r="<<sqrt(gP[0]*gP[0]+gP[1]*gP[1])<<endreq;

      double Phi0=atan2(gP[1],gP[0]);
      double phi=atan2(gV[1],gV[0]);
      double R0=sqrt(gP[0]*gP[0]+gP[1]*gP[1]);
      double theta=acos(gV[2]);
      double eta=-log(sin(0.5*theta)/cos(0.5*theta));
      double track_pT=sin(theta)*pP->pT()/sint;

      theta=atan(R0/fabs(Zcoordinate))/2.0;
      eta=-log(sin(theta)/cos(theta));
      if(Zcoordinate<0.0) eta=-eta;

      pEP=new TrigInDetTrackFitPar(Phi0,phi,
				   R0,eta,track_pT,
				   TrigInDetTrackFitPar::ENDCAP,Zcoordinate);
    }
  else if(isBarrel)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Track coordinates at R="<<Radius<<
	  "  X="<<gP[0]<<"  Y="<<gP[1]<<"  Z="<<gP[2]<<"  r="<<sqrt(gP[0]*gP[0]+gP[1]*gP[1])<<endreq;

      double Phi0=atan2(gP[1],gP[0]);
      double phi=atan2(gV[1],gV[0]);
      double theta=acos(gV[2]);
      double eta=-log(sin(0.5*theta)/cos(0.5*theta));
      double track_pT=sin(theta)*pP->pT()/sint;
      theta=atan(Radius/fabs(gP[2]))/2.0;

      eta=-log(sin(theta)/cos(theta));
      if(gP[2]<0.0) eta=-eta;

      pEP=new TrigInDetTrackFitPar(Phi0,phi,
				   gP[2],eta,track_pT,
				   TrigInDetTrackFitPar::BARREL,Radius);
    }
  
  return pEP;
}


StatusCode TrigInDetTrackExtrapolator::extrapolateToCalo(const TrigInDetTrack* pT,
							 double R,
							 double Z,
							 double& phiC,
							 double& etaC)
{

  double z,pt,eta,theta,d0,z0; 

  MsgStream athenaLog(msgSvc(), name());
  const TrigInDetTrackFitPar* pP=pT->param();

  if(pP==NULL)
    {
      if (m_outputLevel <= MSG::ERROR) 
	athenaLog << MSG::ERROR << "TrigInDetTrack has no parameters - extrapolation to Calo failed" 
		  << endreq;
      return StatusCode::FAILURE;
    }

  if(fabs(pP->pT())<m_minPt)
    {

      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Track pT is too low - skipping extrapolation" << endreq;
      return StatusCode::FAILURE;
    }
  
  pt = pP->pT();
  eta = pP->eta();
 
  theta = 2.*atan(exp(-eta));
  d0 = pP->a0();
  z0 = pP->z0();

  if(fabs(d0)>R)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "d0="<<d0<<" exceeds target surface radius="<<R<<endreq;
      return StatusCode::FAILURE;
    }

  if(fabs(z0)>fabs(Z))
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "z0="<<z0<<" is beyond target z="<<Z<<endreq;
      return StatusCode::FAILURE;
    }

  phiC = 0;
  etaC = 0;
   
  z = z0 + (R - fabs(d0))/tan(theta);

  bool isBarrel=(fabs(z)<fabs(Z));

  if(isBarrel)
    {
      double rad=fabs(pt)/0.6;
      if((rad+d0)<R) isBarrel=false;
    }

  TrigInDetTrackFitPar* pF=NULL;

  if(fabs(pt)>5000.0)
    {
      if(isBarrel) pF=extrapolateToBarrel(pT,R);
      else pF=extrapolateToEndcap(pT,fabs(Z)*((pP->eta()<0.0)?-1.0:1.0));
    }
  else
    {
      pF=extrapolateBlindly(pT,R,fabs(Z)*((pP->eta()<0.0)?-1.0:1.0));
    }

  if(pF==NULL) return StatusCode::FAILURE;

  phiC=pF->a0();etaC=pF->eta();
  
  delete pF;

  return StatusCode::SUCCESS;
}


void TrigInDetTrackExtrapolator::m_getMagneticField(double r[3],double* B)
{
  B[0]=0.0;B[1]=0.0;B[2]=0.0;
	double field[3];
	m_MagFieldSvc->getField(r,field);//field is returned in kT
	for(int i=0;i<3;i++) B[i]=field[i]/Gaudi::Units::kilogauss;//convert to kG
}



Trk::TrkTrackState* TrigInDetTrackExtrapolator::m_integrate(Trk::TrkTrackState* pTS, 
							    Trk::TrkPlanarSurface* pSB,
							    Trk::TrkPlanarSurface* pSE)
{
  const double C=0.02999975;
  const double minStep=30.0;
	  
  double sint,cost,sinf,cosf;
  double gP[3],lP[3],gV[3],a,b,c,Rf[5],descr,CQ,Ac,Av;
  double V[3],P[3],D[4],gB[3],DVx,DVy,DVz;
  int i,nStep,nStepMax;
  double sl,ds,path=0.0;

  sint=sin(pTS->m_getTrackState(3));cosf=cos(pTS->m_getTrackState(2));
  sinf=sin(pTS->m_getTrackState(2));cost=cos(pTS->m_getTrackState(3));
  gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*pTS->m_getTrackState(4);

  if(pSB!=NULL)
    {
      lP[0]=pTS->m_getTrackState(0);lP[1]=pTS->m_getTrackState(1);lP[2]=0.0;
      pSB->m_transformPointToGlobal(lP,gP);
    }
  else
    {
      gP[0]=-pTS->m_getTrackState(0)*sinf;
      gP[1]= pTS->m_getTrackState(0)*cosf;
      gP[2]= pTS->m_getTrackState(1);
    }

  for(i=0;i<4;i++) D[i]=pSE->m_getPar(i);

  m_getMagneticField(gP,gB);
 
  c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
  b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
  a=0.5*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
	    gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
	    gB[2]*(D[0]*gV[1]-D[1]*gV[0]));

  descr=b*b-4.0*a*c;

  if(descr<0.0) 
    {
      // printf("D<0 - extrapolation failed\n");
      return NULL;
    }
  sl=-c/b;
  sl=sl*(1-a*sl/b);

  if(fabs(sl)<minStep) nStepMax=1;
  else
    {
      nStepMax=(int)(fabs(sl)/minStep)+1;
    }
 
  nStep=nStepMax;path=0.0;
  while(nStep>0)
    {
      c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
      b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
      a=0.5*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
		gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
		gB[2]*(D[0]*gV[1]-D[1]*gV[0]));
      sl=-c/b;
      sl=sl*(1-a*sl/b);
      ds=sl/nStep;path+=ds;

      Av=ds*CQ;
      Ac=0.5*ds*Av;
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
      m_getMagneticField(gP,gB);
      nStep--;
    }
  pSE->m_transformPointToLocal(gP,lP);

  Rf[0]=lP[0];Rf[1]=lP[1];
  Rf[2]=atan2(V[1],V[0]);

  if(fabs(V[2])>1.0)
    return NULL;

  Rf[3]=acos(V[2]);
  Rf[4]=pTS->m_getTrackState(4);
  
  Trk::TrkTrackState* pTE=new Trk::TrkTrackState(pTS);
  pTE->m_setTrackState(Rf);
  pTE->m_attachToSurface(pSE);

  return pTE;
}


