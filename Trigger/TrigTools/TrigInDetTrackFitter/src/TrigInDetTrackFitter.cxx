/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////////////////////
// TrigInDetTrackFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 01.09.2005 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////


#include <cmath>
#include <iostream>
#include <memory>
#include "GaudiKernel/SystemOfUnits.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetTrackFitter/TrigInDetTrackFitter.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"


#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"

TrigInDetTrackFitter::TrigInDetTrackFitter(const std::string& t, 
					   const std::string& n,
					   const IInterface*  p ): AthAlgTool(t,n,p),
								   m_MagFieldSvc("AtlasFieldSvc",this->name()),
								   m_trackMaker("TrigDkfTrackMakerTool")
{
  declareInterface< ITrigInDetTrackFitter >( this );
  
  declareProperty( "AthenaFieldService", m_MagFieldSvc, "AlasFieldService");
  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremm=false);  
  declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
  declareProperty( "OfflineClusters", m_offlineClusters = true);
  declareProperty( "correctClusterPos", m_correctClusterPos = false);
  declareProperty( "ROTcreator", m_ROTcreator, "ROTcreatorTool" );
}

StatusCode TrigInDetTrackFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  
	ATH_MSG_INFO("Using Athena magnetic field service");
	sc = m_MagFieldSvc.retrieve();
	if(sc.isFailure()) 
	{
		ATH_MSG_ERROR("Unable to retrieve Athena MagFieldService");
		return StatusCode::FAILURE;
	}
  sc=m_trackMaker.retrieve();
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve "<<m_trackMaker);
      return sc;
    }
  if (m_correctClusterPos) {
    sc = m_ROTcreator.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not get ROTcreator "<<m_ROTcreator);
      return sc;
    }
  }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    ATH_MSG_INFO("Unable to locate Service TrigTimerSvc ");
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
//   add some timers:
//
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("CreateNodes");
    m_timer[1] = timerSvc->addItem("ForwardFilter");
    m_timer[2] = timerSvc->addItem("Smoother");
    m_timer[3] = timerSvc->addItem("TrigTrackPars");
    m_timer[4] = timerSvc->addItem("CleanUp");
  }

  m_fitStats.clear();
  for(int i=0;i<13;i++) m_fitStats.push_back(FitStatStruct(i));
  m_algorithmId=0;
  ATH_MSG_INFO("TrigInDetTrackFitter constructed");
  return sc;
}

StatusCode TrigInDetTrackFitter::finalize()
{
  ATH_MSG_INFO("==============================================================");
  ATH_MSG_INFO("TrigInDetTrackFitter::finalize() - LVL2 Track fit Statistics: ");
  for(std::vector<FitStatStruct>::iterator it=m_fitStats.begin();it!=m_fitStats.end();++it) {
    if((*it).m_nTracksTotal==0) continue;
    ATH_MSG_INFO("Algorithm Id="<<(*it).m_algorithmId<<" N tracks = "<<(*it).m_nTracksTotal);
    ATH_MSG_INFO("Problems detected: ");
    ATH_MSG_INFO("Unresolved spacepoints :"<<(*it).m_fitErrors[0]);
    ATH_MSG_INFO("Extrapolator divergence:"<<(*it).m_fitErrors[1]);
    ATH_MSG_INFO("pT falls below 200 MeV :"<<(*it).m_fitErrors[2]);
  }
  ATH_MSG_INFO("==============================================================");
  m_fitStats.clear();
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

void TrigInDetTrackFitter::getMagneticField(double r[3],double* B)
{
  B[0]=0.0;B[1]=0.0;B[2]=0.0;
	double field[3];
	m_MagFieldSvc->getField(r,field);//field is returned in kT
	for(int i=0;i<3;i++) B[i]=field[i]/Gaudi::Units::kilogauss;//convert to kG
}

void TrigInDetTrackFitter::correctScale(Trk::TrkTrackState* pTS) {

  double Rf[5];
  double Gf[5][5];
  int i,j;

  for(i=0;i<4;i++) Rf[i] = pTS->m_getTrackState(i);
  Rf[4] = 0.001*pTS->m_getTrackState(4);

  for(i=0;i<4;i++)
    for(j=0;j<4;j++) Gf[i][j] = pTS->m_getTrackCovariance(i,j);

  Gf[0][4] = Gf[4][0] = pTS->m_getTrackCovariance(0,4)/1000.0;
  Gf[1][4] = Gf[4][1] = pTS->m_getTrackCovariance(1,4)/1000.0;
  Gf[2][4] = Gf[4][2] = pTS->m_getTrackCovariance(2,4)/1000.0;
  Gf[3][4] = Gf[4][3] = pTS->m_getTrackCovariance(3,4)/1000.0;
  Gf[4][4] = pTS->m_getTrackCovariance(4,4)/1000000.0;

  pTS->m_setTrackState(Rf);
  pTS->m_setTrackCovariance(Gf);
}

Trk::TrkTrackState* TrigInDetTrackFitter::extrapolate(Trk::TrkTrackState* pTS, 
                                                      Trk::TrkPlanarSurface* pSB,
                                                      Trk::TrkPlanarSurface* pSE)
{
  const double C=0.02999975/1000.0;//using GeV internally 
  const double minStep=30.0;
	    
  double J[5][5],Rf[5],AG[5][5],Gf[5][5],A[5][5];
  int i,j,m;

  bool samePlane=false;

  if(pSB!=NULL)
    {   
      double diff=0.0;
      for(i=0;i<4;i++) diff+=fabs(pSE->m_getPar(i)-pSB->m_getPar(i));
      if(diff<1e-5) {
	samePlane=true;	
	(m_fitStats[m_algorithmId]).m_fitErrors[0]++;
	//std::cout<<"Starting plane and target plane are the same !"<<std::endl;
      }
    }

  if(!samePlane) {

    double gP[3],gPi[3],lP[3],gV[3],a,b,c,s,J0[7][5],descr,CQ,Ac,Av,Cc;
    double V[3],P[3],M[3][3],D[4],Jm[7][7],
      J1[5][7],gB[3],gBi[3],gBf[3],dBds[3],Buf[5][7],DVx,DVy,DVz;
    int nStep,nStepMax;
    double sl,ds,path=0.0;

    //m_numericalJacobian(pTS,pSB,pSE,J);
    double sint,cost,sinf,cosf;
    sint=sin(pTS->m_getTrackState(3));cosf=cos(pTS->m_getTrackState(2));
    sinf=sin(pTS->m_getTrackState(2));cost=cos(pTS->m_getTrackState(3));
    gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*pTS->m_getTrackState(4);

    memset(&J0[0][0],0,sizeof(J0));

    if(pSB!=NULL)
      {    
	double L[3][3];
	lP[0]=pTS->m_getTrackState(0);lP[1]=pTS->m_getTrackState(1);lP[2]=0.0;
	pSB->m_transformPointToGlobal(lP,gP);
	for(i=0;i<3;i++) for(j=0;j<3;j++) L[i][j]=pSB->m_getInvRotMatrix(i,j);
	
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
	gP[0]=-pTS->m_getTrackState(0)*sinf;
	gP[1]= pTS->m_getTrackState(0)*cosf;
	gP[2]= pTS->m_getTrackState(1);
	J0[0][0]=-sinf;J0[0][2]=-pTS->m_getTrackState(0)*cosf;
	J0[1][0]= cosf;J0[1][2]=-pTS->m_getTrackState(0)*sinf;
	J0[2][1]=1.0;
	J0[3][2]=-sinf*sint;J0[3][3]=cosf*cost;
	J0[4][2]= cosf*sint;J0[4][3]=sinf*cost;
	J0[5][3]=-sint;
	J0[6][4]=1.0;
      }
    for(i=0;i<4;i++) D[i]=pSE->m_getPar(i);
    for(i=0;i<3;i++) gPi[i]=gP[i];
  
    getMagneticField(gP,gB);

    for(i=0;i<3;i++) gBi[i]=gB[i];
    
    c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
    b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
    a=0.5*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
	      gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
	      gB[2]*(D[0]*gV[1]-D[1]*gV[0]));

    descr=b*b-4.0*a*c;
    
    if(descr<0.0) 
      {
	//      printf("D<0 - extrapolation failed\n");
	return NULL;
      }
    
    bool useExpansion=true;
    double ratio = 4*a*c/(b*b);
    
    if(fabs(ratio)>0.1) 
      useExpansion = false;
    
    if(useExpansion) {
      sl=-c/b;
      sl=sl*(1-a*sl/b);
    }
    else {
      int signb = (b<0.0)?-1:1;
      sl = (-b+signb*sqrt(descr))/(2*a);
    }

    if(fabs(sl)<minStep) nStepMax=1;
    else
      {
	nStepMax=(int)(fabs(sl)/minStep)+1;
      }
    if((nStepMax<0)||(nStepMax>1000))
      {
	return NULL;
      } 
    Av=sl*CQ;
    Ac=0.5*sl*Av;
    DVx=gV[1]*gB[2]-gV[2]*gB[1];
    DVy=gV[2]*gB[0]-gV[0]*gB[2];
    DVz=gV[0]*gB[1]-gV[1]*gB[0];
    
    P[0]=gP[0]+gV[0]*sl+Ac*DVx;
    P[1]=gP[1]+gV[1]*sl+Ac*DVy;
    P[2]=gP[2]+gV[2]*sl+Ac*DVz;
    V[0]=gV[0]+Av*DVx;
    V[1]=gV[1]+Av*DVy;
    V[2]=gV[2]+Av*DVz;
    
    getMagneticField(P,gB);
  
    for(i=0;i<3;i++) gBf[i]=gB[i];
    for(i=0;i<3;i++)
      {
	dBds[i]=(gBf[i]-gBi[i])/sl;
	gB[i]=gBi[i];
      }
    nStep=nStepMax;path=0.0;
    while(nStep>0)
      {
	c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
	b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
	a=0.5*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
		  gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
		  gB[2]*(D[0]*gV[1]-D[1]*gV[0]));
	
	ratio = 4*a*c/(b*b);
	if(fabs(ratio)>0.1) 
	  useExpansion = false;
	else useExpansion = true;
	
	if(useExpansion) {
	  sl=-c/b;
	  sl=sl*(1-a*sl/b);
	}
	else {
	  descr=b*b-4.0*a*c;
	  if(descr<0.0) 
	    {
	      // printf("D<0 - extrapolation failed\n");
	      return NULL;
	    }
	  int signb = (b<0.0)?-1:1;
	  sl = (-b+signb*sqrt(descr))/(2*a);
	}

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
	for(i=0;i<3;i++) gB[i]+=dBds[i]*ds;
	nStep--;
      }
    pSE->m_transformPointToLocal(gP,lP);
    Rf[0]=lP[0];Rf[1]=lP[1];
    Rf[2]=atan2(V[1],V[0]);

    if(fabs(V[2])>1.0) 
      {
	return NULL;
      }

    Rf[3]=acos(V[2]);
    Rf[4]=pTS->m_getTrackState(4);
    
    gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;

    for(i=0;i<4;i++) D[i]=pSE->m_getPar(i);
    for(i=0;i<3;i++) gP[i]=gPi[i];

    for(i=0;i<3;i++)
      {
	gB[i]=0.5*(gBi[i]+gBf[i]);
      }
  
    c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
    b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
    a=CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
	  gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
	  gB[2]*(D[0]*gV[1]-D[1]*gV[0]));
    
    ratio = 4*a*c/(b*b);
    if(fabs(ratio)>0.1) 
      useExpansion = false;
    else useExpansion = true;
    
    if(useExpansion) {
      s=-c/b;
      s=s*(1-a*s/b);
    }
    else {
      descr=b*b-4.0*a*c;
      if(descr<0.0) 
	{
	  // printf("D<0 - extrapolation failed\n");
	  return NULL;
	}
      int signb = (b<0.0)?-1:1;
      s = (-b+signb*sqrt(descr))/(2*a);
    }

    Av=s*CQ;
    Ac=0.5*s*Av;
    Cc=0.5*s*s*C;

    DVx=gV[1]*gB[2]-gV[2]*gB[1];
    DVy=gV[2]*gB[0]-gV[0]*gB[2];
    DVz=gV[0]*gB[1]-gV[1]*gB[0];
    
    P[0]=gP[0]+gV[0]*s+Ac*DVx;
    P[1]=gP[1]+gV[1]*s+Ac*DVy;
    P[2]=gP[2]+gV[2]*s+Ac*DVz;

    V[0]=gV[0]+Av*DVx;V[1]=gV[1]+Av*DVy;V[2]=gV[2]+Av*DVz;
    
    pSE->m_transformPointToLocal(P,lP);
  
    memset(&Jm[0][0],0,sizeof(Jm));
    
    for(i=0;i<3;i++) for(j=0;j<3;j++) M[i][j]=pSE->m_getRotMatrix(i,j);
    
    double coeff[3], dadVx,dadVy,dadVz,dadQ,dsdx,dsdy,dsdz,dsdVx,dsdVy,dsdVz,dsdQ;
    coeff[0]=-c*c/(b*b*b);
    coeff[1]=c*(1.0+3.0*c*a/(b*b))/(b*b);
    coeff[2]=-(1.0+2.0*c*a/(b*b))/b;
    
    dadVx=0.5*CQ*(-D[1]*gB[2]+D[2]*gB[1]);
    dadVy=0.5*CQ*( D[0]*gB[2]-D[2]*gB[0]);
    dadVz=0.5*CQ*(-D[0]*gB[1]+D[1]*gB[0]);
    dadQ=0.5*C*(D[0]*DVx+D[1]*DVy+D[2]*DVz);
    
    dsdx=coeff[2]*D[0];
    dsdy=coeff[2]*D[1];
    dsdz=coeff[2]*D[2];
    dsdVx=coeff[0]*dadVx+coeff[1]*D[0];
    dsdVy=coeff[0]*dadVy+coeff[1]*D[1];
    dsdVz=coeff[0]*dadVz+coeff[1]*D[2];
    dsdQ=coeff[0]*dadQ;
    
    Jm[0][0]=1.0+V[0]*dsdx;
    Jm[0][1]=    V[0]*dsdy;
    Jm[0][2]=    V[0]*dsdz;
    
    Jm[0][3]=  s+V[0]*dsdVx;
    Jm[0][4]=    V[0]*dsdVy+Ac*gB[2];
    Jm[0][5]=    V[0]*dsdVz-Ac*gB[1];
    Jm[0][6]=    V[0]*dsdQ+Cc*DVx;

    Jm[1][0]=    V[1]*dsdx;
    Jm[1][1]=1.0+V[1]*dsdy;
    Jm[1][2]=    V[1]*dsdz;

    Jm[1][3]=    V[1]*dsdVx-Ac*gB[2];
    Jm[1][4]=  s+V[1]*dsdVy;
    Jm[1][5]=    V[1]*dsdVz+Ac*gB[0];
    Jm[1][6]=    V[1]*dsdQ+Cc*DVy;
  
    Jm[2][0]=    V[2]*dsdx;
    Jm[2][1]=    V[2]*dsdy;
    Jm[2][2]=1.0+V[2]*dsdz;
    Jm[2][3]=    V[2]*dsdVx+Ac*gB[1];
    Jm[2][4]=    V[2]*dsdVy-Ac*gB[0];
    Jm[2][5]=  s+V[2]*dsdVz;
    Jm[2][6]=    V[2]*dsdQ+Cc*DVz;

    Jm[3][0]=dsdx*CQ*DVx;
    Jm[3][1]=dsdy*CQ*DVx;
    Jm[3][2]=dsdz*CQ*DVx;
  
    Jm[3][3]=1.0+dsdVx*CQ*DVx;
    Jm[3][4]=CQ*(dsdVy*DVx+s*gB[2]);
    Jm[3][5]=CQ*(dsdVz*DVx-s*gB[1]);
  
    Jm[3][6]=(CQ*dsdQ+C*s)*DVx;
  
    Jm[4][0]=dsdx*CQ*DVy;
    Jm[4][1]=dsdy*CQ*DVy;
    Jm[4][2]=dsdz*CQ*DVy;

    Jm[4][3]=CQ*(dsdVx*DVy-s*gB[2]);
    Jm[4][4]=1.0+dsdVy*CQ*DVy;
    Jm[4][5]=CQ*(dsdVz*DVy+s*gB[0]);
  
    Jm[4][6]=(CQ*dsdQ+C*s)*DVy;
  
    Jm[5][0]=dsdx*CQ*DVz;
    Jm[5][1]=dsdy*CQ*DVz;
    Jm[5][2]=dsdz*CQ*DVz;
    Jm[5][3]=CQ*(dsdVx*DVz+s*gB[1]);
    Jm[5][4]=CQ*(dsdVy*DVz-s*gB[0]);
    Jm[5][5]=1.0+dsdVz*CQ*DVz;
    Jm[5][6]=(CQ*dsdQ+C*s)*DVz;
  
    Jm[6][6]=1.0;
  
    memset(&J1[0][0],0,sizeof(J1));

    J1[0][0]=M[0][0];J1[0][1]=M[0][1];J1[0][2]=M[0][2];
    J1[1][0]=M[1][0];J1[1][1]=M[1][1];J1[1][2]=M[1][2];
    J1[2][3]=-V[1]/(V[0]*V[0]+V[1]*V[1]);
    J1[2][4]= V[0]/(V[0]*V[0]+V[1]*V[1]);
    J1[3][5]=-1.0/sqrt(1-V[2]*V[2]);
    J1[4][6]=1.0;

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
  }
  else {
    Rf[0]=pTS->m_getTrackState(0);
    Rf[1]=pTS->m_getTrackState(1);	
    Rf[2]=pTS->m_getTrackState(2);
    Rf[3]=pTS->m_getTrackState(3);
    Rf[4]=pTS->m_getTrackState(4);
    memset(&J[0][0],0,sizeof(J));
    for(i=0;i<5;i++) J[i][i]=1.0;
  }

  for(i=0;i<5;i++) for(j=0;j<5;j++)
    {
      AG[i][j]=0.0;for(m=0;m<5;m++) AG[i][j]+=J[i][m]*pTS->m_getTrackCovariance(m,j);
    }
  for(i=0;i<5;i++) for(j=i;j<5;j++)
    {
      Gf[i][j]=0.0;
      for(m=0;m<5;m++) Gf[i][j]+=AG[i][m]*J[j][m];
      Gf[j][i]=Gf[i][j];
    }

  Trk::TrkTrackState* pTE=new Trk::TrkTrackState(pTS);

  //workaround to keep using existing TrkTrackState code
  double Rtmp[5];

  for(i=0;i<4;i++) Rtmp[i] = Rf[i];
  Rtmp[4] = 0.001*Rf[4];//GeV->MeV

  pTE->m_setTrackState(Rtmp);
  pTE->m_setTrackCovariance(Gf);
  pTE->m_attachToSurface(pSE);

  //  pTE->m_applyMaterialEffects();
  if(m_doMultScatt)
    pTE->m_applyMultipleScattering();

  pTE->m_setTrackState(Rf);//restore

  if(m_doBremm) 
    pTE->m_applyEnergyLoss(1);

  AmgSymMatrix(5) Gi;
  for(i=0;i<5;i++) for(j=i;j<5;j++)
    {
      Gi.fillSymmetric(i, j, pTE->m_getTrackCovariance(i,j));
    }
  Gi = Gi.inverse();
  //m_matrixInversion5x5(Gi);
 
  for(i=0;i<5;i++) for(j=0;j<5;j++)
    {
      A[i][j]=0.0;
      for(m=0;m<5;m++) A[i][j]+=AG[m][i]*Gi(m,j);
    }
  pTE->m_setPreviousState(pTS);
  pTE->m_setSmootherGain(A);

  return pTE;
}

void TrigInDetTrackFitter::matrixInversion5x5(double a[5][5])
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


void TrigInDetTrackFitter::fit(TrigInDetTrackCollection* recoTracks )
{
	if(m_timers)
	{
		m_timer[0]->start();
		m_timer[0]->pause();
		m_timer[1]->start();
		m_timer[1]->pause();
		m_timer[2]->start();
		m_timer[2]->pause();
		m_timer[3]->start();
		m_timer[3]->pause();
		m_timer[4]->start();
		m_timer[4]->pause();
	}

  for(auto trIt = recoTracks->begin(); trIt != recoTracks->end(); ++trIt) {
		fitTrack(**trIt);
	}

	if(m_timers) 
	{
		m_timer[0]->stop();
		m_timer[1]->stop();
		m_timer[2]->stop();
		m_timer[3]->stop();
		m_timer[4]->stop();
	}
}

TrackCollection* TrigInDetTrackFitter::fit(const TrackCollection& recoTracks, const Trk::ParticleHypothesis& matEffects)
{
	if(m_timers)
	{
		m_timer[0]->start();
		m_timer[0]->pause();
		m_timer[1]->start();
		m_timer[1]->pause();
		m_timer[2]->start();
		m_timer[2]->pause();
		m_timer[3]->start();
		m_timer[3]->pause();
		m_timer[4]->start();
		m_timer[4]->pause();
	}
	TrackCollection* fittedTracks = new TrackCollection();	
  for(auto trIt = recoTracks.begin(); trIt != recoTracks.end(); ++trIt) {
		Trk::Track* fittedTrack = fitTrack(**trIt, matEffects);
		if (fittedTrack!=nullptr) {
			fittedTracks->push_back(fittedTrack);
		}
	}
	if(m_timers) 
	{
		m_timer[0]->stop();
		m_timer[1]->stop();
		m_timer[2]->stop();
		m_timer[3]->stop();
		m_timer[4]->stop();
	}
	return fittedTracks;
}

void TrigInDetTrackFitter::fitTrack(TrigInDetTrack& recoTrack ) {

	TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>(recoTrack.param());
	if(param==NULL)
	{
		ATH_MSG_WARNING("Fit Failed -- TrigInDetTrack has no parameters");
		return;
	}

	// 1. Create initial track state:
	double Rk[5];
	Rk[0]=param->a0();
	Rk[1]=param->z0();
	Rk[2]=param->phi0();
	double Theta=2.0*atan(exp(-param->eta()));
	Rk[3]=Theta;
	Rk[4]=1000.0*sin(Theta)/param->pT();//MeV->GeV
	double Pt=param->pT();

	if(fabs(Pt)<100.0)
	{
		ATH_MSG_DEBUG("Estimated Pt is too low "<<Pt<<" - skipping fit");
		return;
	}

	// 2. Create filtering nodes

	if(m_timers) m_timer[0]->resume();
	std::vector<Trk::TrkBaseNode*> vpTrkNodes;
	std::vector<Trk::TrkTrackState*> vpTrackStates;
	bool trackResult = m_trackMaker->createDkfTrack(*(recoTrack.siSpacePoints()),vpTrkNodes, m_DChi2);
	int nHits=vpTrkNodes.size();
	if(m_timers) 
	{
		m_timer[0]->pause();
		m_timer[1]->resume();
	}
	ATH_MSG_VERBOSE(nHits<<" filtering nodes created");

	if(!trackResult) return;

	// 3. Main algorithm: filter and smoother (Rauch-Tung-Striebel)

	m_algorithmId=recoTrack.algorithmId();
	(m_fitStats[m_algorithmId]).m_nTracksTotal++;
	//Trk::TrkTrackState* pTS = new Trk::TrkTrackState(Rk);
	Trk::TrkTrackState* pTS = new Trk::TrkTrackState(Rk);
	double Gk[5][5] = {{100.0,     0, 0,    0,    0},
			   {0,     100.0, 0,    0,    0},
			   {0,         0, 0.01, 0,    0},
			   {0,         0, 0,    0.01, 0},
			   {0,         0, 0,    0,    0.1}};
	pTS->m_setTrackCovariance(Gk);
	if(m_doMultScatt)  
		pTS->m_setScatteringMode(1);
	if(m_doBremm)
		pTS->m_setScatteringMode(2);
	vpTrackStates.push_back(pTS);

	ATH_MSG_VERBOSE("Initial chi2: "<<recoTrack.chi2()<<" track authorId: "<<recoTrack.algorithmId());
	ATH_MSG_VERBOSE("Initial params: locT="<<Rk[0]<<" locL="<<Rk[1]<<" phi="<<Rk[2]
			<<" theta="<<Rk[3]<<" Q="<<Rk[4]<<" pT="<<sin(Rk[3])/Rk[4]<<" GeV");

	std::vector<Trk::TrkBaseNode*>::iterator pnIt(vpTrkNodes.begin()),
		pnEnd(vpTrkNodes.end());

	bool OK=true;

	double chi2tot=0.0;
	int ndoftot=-5;

	Trk::TrkPlanarSurface* pSB=nullptr;
	Trk::TrkPlanarSurface* pSE=nullptr;
	for(;pnIt!=pnEnd;++pnIt)
	{
		pSE=(*pnIt)->m_getSurface();
		Trk::TrkTrackState* pNS=extrapolate(pTS,pSB,pSE);

		pSB=pSE;
		if(pNS!=nullptr)
		{
			// pNS->m_report();
			vpTrackStates.push_back(pNS);

			(*pnIt)->m_validateMeasurement(pNS);
			ATH_MSG_VERBOSE("dChi2="<<(*pnIt)->m_getChi2());
			if((*pnIt)->m_isValidated())
			{
				chi2tot+=(*pnIt)->m_getChi2();
				ndoftot+=(*pnIt)->m_getNdof();
			}
			(*pnIt)->m_updateTrackState(pNS);
			pTS=pNS;
			Pt=1000.0*sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
			if(fabs(Pt)<200.0)
			{
				ATH_MSG_VERBOSE("Estimated Pt is too low "<<Pt<<" - skipping fit");
				(m_fitStats[m_algorithmId]).m_fitErrors[2]++;
				OK=false;break;
			}
		}
		else
		{
			(m_fitStats[m_algorithmId]).m_fitErrors[0]++;
			OK=false;break;
		}
	}
	if(m_timers) m_timer[1]->pause();
	if(OK)
	{
		if(m_timers) m_timer[2]->resume();
		std::vector<Trk::TrkTrackState*>::reverse_iterator ptsIt(vpTrackStates.rbegin()),
			ptsEnd(vpTrackStates.rend());

		for(;ptsIt!=ptsEnd;++ptsIt)
		{
			(*ptsIt)->m_runSmoother();
		}
		if(m_timers) 
		{
			m_timer[2]->pause();
			m_timer[3]->resume();
		}      

		pTS=(*vpTrackStates.begin());
		//correct GeV->MeV

		correctScale(pTS);

		Pt=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
		double Phi0 = pTS->m_getTrackState(2);
		if(Phi0>M_PI) Phi0-=2*M_PI;
		if(Phi0<-M_PI) Phi0+=2*M_PI;
		double Eta = -log(sin(0.5*pTS->m_getTrackState(3))/cos(0.5*pTS->m_getTrackState(3)));
		double Z0 = pTS->m_getTrackState(1);
		double D0 = pTS->m_getTrackState(0);

    bool bad_cov = false;
    for (int i = 0; i < 5; i++) {
      //Check for negative entries along the main diagonal
      double cov_val = pTS->m_getTrackCovariance(i,i);
      if (cov_val < 0) { 
        bad_cov = true;
      }
    }
		if((ndoftot<0) || (fabs(Pt)<100.0) || (std::isnan(Pt)) | bad_cov)
		{
			ATH_MSG_DEBUG("Fit failed - possibly floating point problem");
			recoTrack.chi2(1e8);
		}
    else {
      double errD0 = sqrt(pTS->m_getTrackCovariance(0,0));
      double errZ0 = sqrt(pTS->m_getTrackCovariance(1,1));
      double errPhi0 = sqrt(pTS->m_getTrackCovariance(2,2));
      double errEta = sqrt(pTS->m_getTrackCovariance(3,3))/fabs(sin(pTS->m_getTrackState(3)));
      double b=cos(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
      double c=-Pt/pTS->m_getTrackState(4);
      double a=-1.0/sin(pTS->m_getTrackState(3));
      double errPt = sqrt(b*b*(pTS->m_getTrackCovariance(3,3))+c*c*(pTS->m_getTrackCovariance(4,4))+
          2.0*b*c*(pTS->m_getTrackCovariance(3,4)));

      std::vector<double>* pCov=new std::vector<double>;
      double CV[5][5];
      CV[0][0]=pTS->m_getTrackCovariance(0,0);
      CV[0][1]=pTS->m_getTrackCovariance(0,2);
      CV[0][2]=pTS->m_getTrackCovariance(0,1);
      CV[0][3]=a*(pTS->m_getTrackCovariance(0,3));
      CV[0][4]=b*(pTS->m_getTrackCovariance(0,3))+c*(pTS->m_getTrackCovariance(0,4));
      CV[1][1]=pTS->m_getTrackCovariance(2,2);

      CV[1][2]=pTS->m_getTrackCovariance(1,2);
      CV[1][3]=a*(pTS->m_getTrackCovariance(2,3));
      CV[1][4]=b*(pTS->m_getTrackCovariance(2,3))+c*(pTS->m_getTrackCovariance(2,4));
      CV[2][2]=pTS->m_getTrackCovariance(1,1);
      CV[2][3]=a*(pTS->m_getTrackCovariance(1,3));
      CV[2][4]=b*(pTS->m_getTrackCovariance(1,3))+c*(pTS->m_getTrackCovariance(1,4));
      CV[3][3]=a*a*(pTS->m_getTrackCovariance(3,3));
      CV[3][4]=a*(b*(pTS->m_getTrackCovariance(3,3))+c*(pTS->m_getTrackCovariance(3,4)));
      CV[4][4]=b*b*(pTS->m_getTrackCovariance(3,3))+2.0*b*c*(pTS->m_getTrackCovariance(3,4))+
        c*c*(pTS->m_getTrackCovariance(4,4));

      for(int i=0;i<5;i++) {
        for(int j=i;j<5;j++) {
          pCov->push_back(CV[i][j]);
        }
      }
      const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta, Pt,
          errD0,errPhi0,errZ0,
          errEta,errPt,pCov);
      delete param;
      if(ndoftot>1) chi2tot/=ndoftot;
      recoTrack.param(tidtfp);
      recoTrack.chi2(chi2tot);
    }

		if(m_timers) 
		{
			m_timer[3]->pause();
			m_timer[4]->resume();
		}

		ATH_MSG_VERBOSE("Total chi2 ="<<chi2tot<<" NDOF="<<ndoftot);
		ATH_MSG_VERBOSE("Fitted parameters: d0="<<D0<<" phi0="<<Phi0<<" z0="<<Z0	
				<<" eta0="<<Eta<<" pt="<<Pt);
	}
	else {
		ATH_MSG_DEBUG("Forward Kalman filter: extrapolation failure ");
		recoTrack.chi2(1e8);
	}

	pnIt=vpTrkNodes.begin();pnEnd=vpTrkNodes.end();
	for(;pnIt!=pnEnd;++pnIt) 
	{
		delete((*pnIt)->m_getSurface());
		delete (*pnIt);
	}
	vpTrkNodes.clear();
	for(std::vector<Trk::TrkTrackState*>::iterator ptsIt=vpTrackStates.begin();
			ptsIt!=vpTrackStates.end();++ptsIt) delete (*ptsIt);
	vpTrackStates.clear();

	if(m_timers) 
	{
		m_timer[4]->pause();
	}
}

Trk::Track* TrigInDetTrackFitter::fitTrack(const Trk::Track& recoTrack, const Trk::ParticleHypothesis& matEffects) {

	const Trk::TrackParameters* trackPars = recoTrack.perigeeParameters();
	if(trackPars==nullptr) {
		ATH_MSG_WARNING("Fit Failed -- TrkTrack has no parameters");
		return nullptr;
	}

	// 1. Create initial track state:
	double Rk[5];
	Rk[0] = trackPars->parameters()[Trk::d0]; 
	Rk[1] = trackPars->parameters()[Trk::z0]; 
	Rk[2] = trackPars->parameters()[Trk::phi0]; 
	if(Rk[2]>M_PI) Rk[2]-=2*M_PI; 
	if(Rk[2]<-M_PI) Rk[2]+=2*M_PI;
	double trk_theta = trackPars->parameters()[Trk::theta];
	Rk[3] = trk_theta;
	double trk_qOverP = trackPars->parameters()[Trk::qOverP];
	Rk[4] = 1000.0*trk_qOverP;//MeV->GeV
	double trk_Pt = sin(trk_theta)/trk_qOverP;

	if(fabs(trk_Pt)<100.0)
	{
		ATH_MSG_DEBUG("Estimated Pt is too low "<<trk_Pt<<" - skipping fit");
		return nullptr;
	}

	// 2. Create filtering nodes

	if(m_timers) m_timer[0]->resume();
	std::vector<Trk::TrkBaseNode*> vpTrkNodes;
	std::vector<Trk::TrkTrackState*> vpTrackStates;
  vpTrackStates.reserve(vpTrkNodes.size() + 1);
	bool trackResult = m_trackMaker->createDkfTrack(recoTrack,vpTrkNodes, m_DChi2);
	int nHits=vpTrkNodes.size();
	if(m_timers) 
	{
		m_timer[0]->pause();
		m_timer[1]->resume();
	}
	ATH_MSG_VERBOSE(nHits<<" filtering nodes created");

	if(!trackResult) return nullptr;

	// 3. Main algorithm: filter and smoother (Rauch-Tung-Striebel)

	(m_fitStats[m_algorithmId]).m_nTracksTotal++;
	Trk::TrkTrackState* pTS = new Trk::TrkTrackState(Rk);
	double Gk[5][5] = {{100.0,     0, 0,    0,    0},
			   {0,     100.0, 0,    0,    0},
			   {0,         0, 0.01, 0,    0},
			   {0,         0, 0,    0.01, 0},
			   {0,         0, 0,    0,   0.1}};
	pTS->m_setTrackCovariance(Gk);
	if(m_doMultScatt)  
		pTS->m_setScatteringMode(1);
	if(m_doBremm)
		pTS->m_setScatteringMode(2);
	vpTrackStates.push_back(pTS);

	//ATH_MSG_DEBUG("Initial chi2: "<<recoTrack.chi2()<<" track authorId: "<<recoTrack.algorithmId());
	ATH_MSG_VERBOSE("Initial params: locT="<<Rk[0]<<" locL="<<Rk[1]<<" phi="<<Rk[2]
			<<" theta="<<Rk[3]<<" Q="<<Rk[4]<<" pT="<<sin(Rk[3])/Rk[4]<<" GeV");

	bool OK=true;

	double chi2tot=0.0;
	int ndoftot=-5;

	Trk::TrkPlanarSurface* pSB=nullptr;
	Trk::TrkPlanarSurface* pSE=nullptr;
	for(auto pnIt = vpTrkNodes.begin(); pnIt!=vpTrkNodes.end(); ++pnIt) {
		pSE=(*pnIt)->m_getSurface();
		Trk::TrkTrackState* pNS=extrapolate(pTS,pSB,pSE);

		pSB=pSE;
		if(pNS!=nullptr) {
			vpTrackStates.push_back(pNS);

			(*pnIt)->m_validateMeasurement(pNS);
			ATH_MSG_VERBOSE("dChi2="<<(*pnIt)->m_getChi2());
			if((*pnIt)->m_isValidated())
			{
				chi2tot+=(*pnIt)->m_getChi2();
				ndoftot+=(*pnIt)->m_getNdof();
			}
			(*pnIt)->m_updateTrackState(pNS);
			pTS=pNS;
			double est_Pt = 1000.0*sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
			if(fabs(est_Pt)<200.0)
			{
				ATH_MSG_VERBOSE("Estimated Pt is too low "<<est_Pt<<" - skipping fit");
				(m_fitStats[m_algorithmId]).m_fitErrors[2]++;
				OK=false;break;
			}
		}
		else
		{
			(m_fitStats[m_algorithmId]).m_fitErrors[0]++;
			OK=false;break;
		}
	}
	if(m_timers) m_timer[1]->pause();
	Trk::Track* fittedTrack = nullptr;
	if(OK)
	{
		if(m_timers) m_timer[2]->resume();
		for(auto ptsIt = vpTrackStates.rbegin();ptsIt!=vpTrackStates.rend();++ptsIt)
		{
			(*ptsIt)->m_runSmoother();
		}
		if(m_timers) 
		{
			m_timer[2]->pause();
			m_timer[3]->resume();
		}      

		pTS=(*vpTrackStates.begin());
		//correct GeV->MeV

		correctScale(pTS);
		
    double qOverP = pTS->m_getTrackState(4);
		double pt=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
		double phi0 = pTS->m_getTrackState(2);
		if(phi0>M_PI) phi0-=2*M_PI;
		if(phi0<-M_PI) phi0+=2*M_PI;
    double theta = pTS->m_getTrackState(3);
		double z0 = pTS->m_getTrackState(1);
		double d0 = pTS->m_getTrackState(0);
    bool bad_cov = false;
    auto cov = std::unique_ptr<AmgSymMatrix(5)>(new AmgSymMatrix(5));
    for(int i=0;i<5;i++) {
      double cov_diag = pTS->m_getTrackCovariance(i,i);
      if (cov_diag < 0) {
        bad_cov = true;//Diagonal elements must be positive
        break;
        ATH_MSG_DEBUG("REGTEST: cov(" << i << "," << i << ") =" << cov_diag << " < 0, reject track");
      }
      (*cov)(i, i) = pTS->m_getTrackCovariance(i,i);
      for(int j=i+1;j<5;j++) {
        cov->fillSymmetric(i, j, pTS->m_getTrackCovariance(i,j));
      }
    }

		if((ndoftot<0) || (fabs(pt)<100.0) || (std::isnan(pt)) || bad_cov)
		{
		  ATH_MSG_DEBUG("Fit failed - possibly floating point problem");
		}
		else
		{
      Trk::PerigeeSurface perigeeSurface;
      Trk::Perigee* perigee = new Trk::Perigee(d0, z0, phi0, theta, qOverP, perigeeSurface, cov.release());
      ATH_MSG_VERBOSE("perigee: " << *perigee);

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Perigee);
      DataVector<const Trk::TrackStateOnSurface>* pParVec = new DataVector<const Trk::TrackStateOnSurface>;
      if (m_correctClusterPos) {
        pParVec->reserve(vpTrkNodes.size()+1);
        pParVec->push_back(new Trk::TrackStateOnSurface(0, perigee,0,0, typePattern));
        for(auto pnIt = vpTrkNodes.begin(); pnIt!=vpTrkNodes.end(); ++pnIt) {
          if((*pnIt)->m_isValidated()) {
            Trk::TrackStateOnSurface* pTSS=createTrackStateOnSurface(*pnIt);
            if(pTSS!=nullptr) {
              pParVec->push_back(pTSS);
            }
          }
        }
      }
      else {
        pParVec->reserve(recoTrack.trackStateOnSurfaces()->size());
        pParVec->push_back(new Trk::TrackStateOnSurface(0, perigee,0,0, typePattern));

        for (auto tSOS = recoTrack.trackStateOnSurfaces()->begin(); tSOS != recoTrack.trackStateOnSurfaces()->end(); ++tSOS) {
          //Don't store perigee - new perigee created above
          if ((*tSOS)->type(Trk::TrackStateOnSurface::Perigee) == false) {
            pParVec->push_back((*tSOS)->clone());
          }
        }
        if(m_timers)
        {
          m_timer[3]->pause();
          m_timer[4]->resume();
        }
      }
      ATH_MSG_VERBOSE("Total chi2 ="<<chi2tot<<" NDOF="<<ndoftot);
      if(msgLvl(MSG::VERBOSE)) {
        double eta = -log(tan(0.5*theta));
      ATH_MSG_VERBOSE("Fitted parameters: d0="<<d0<<" phi0="<<phi0<<" z0="<<z0	
          <<" eta0="<<eta<<" pt="<<pt);
      }
      Trk::FitQuality* pFQ=new Trk::FitQuality(chi2tot,ndoftot);
      Trk::TrackInfo info(recoTrack.info());
      info.setParticleHypothesis(matEffects);
      fittedTrack = new Trk::Track(info, pParVec, pFQ);//fittedTrack now owns pParVec and pFQ
    }
	}
	else
	{
		ATH_MSG_DEBUG("Forward Kalman filter: extrapolation failure ");
	}

	for(auto pnIt = vpTrkNodes.begin(); pnIt!=vpTrkNodes.end(); ++pnIt) {
		delete((*pnIt)->m_getSurface());
		delete (*pnIt);
	}
	vpTrkNodes.clear();
	for(auto ptsIt = vpTrackStates.begin();ptsIt!=vpTrackStates.end();++ptsIt) {
		delete (*ptsIt);
	}
	vpTrackStates.clear();

	if(m_timers)
	{
		m_timer[4]->pause();
	}
	return fittedTrack;
}

Trk::TrackStateOnSurface* TrigInDetTrackFitter::createTrackStateOnSurface(Trk::TrkBaseNode* pN) const
{
  Trk::TrackStateOnSurface* pTSS=nullptr;
  char type=pN->m_getNodeType();
  const Trk::TrackParameters* pTP=nullptr;

  if(type==0) return pTSS;


  Trk::TrkTrackState* pTS=pN->m_getTrackState();
  auto pM = std::unique_ptr<AmgSymMatrix(5)>(new AmgSymMatrix(5));
  for(int i=0;i<5;i++) {
    for(int j=i;j<5;j++) {
      (*pM)(i,j)=pTS->m_getTrackCovariance(i,j);
    }
  }
  const Trk::PrepRawData* pPRD=pN->m_getPrepRawData();

  if((type==1)||(type==2))
  {
    const Trk::Surface& rS = pPRD->detectorElement()->surface();
    const Trk::PlaneSurface* pPS = dynamic_cast<const Trk::PlaneSurface*>(&rS);
    if(pPS==nullptr) return pTSS;

    pTP=new Trk::AtaPlane(pTS->m_getTrackState(0),
        pTS->m_getTrackState(1),
        pTS->m_getTrackState(2),
        pTS->m_getTrackState(3),
        pTS->m_getTrackState(4),*pPS,
        pM.release());
  }
  else if(type==3)
  {
    const Trk::Surface& rS = pPRD->detectorElement()->surface(pPRD->identify()); 
    const Trk::StraightLineSurface* pLS=dynamic_cast<const Trk::StraightLineSurface*>(&rS);
    if(pLS==nullptr) return pTSS;

    if((pTS->m_getTrackState(2)<-M_PI) ||(pTS->m_getTrackState(2)>M_PI)) {
      ATH_MSG_WARNING("Phi out of range when correcting Trk::TrackStateOnSurface");
    }


    pTP=new Trk::AtaStraightLine(pTS->m_getTrackState(0),
        pTS->m_getTrackState(1),
        pTS->m_getTrackState(2),
        pTS->m_getTrackState(3),
        pTS->m_getTrackState(4),
        *pLS,
        pM.release());
  }
  if(pTP==nullptr) return nullptr;
  const Trk::RIO_OnTrack* pRIO=m_ROTcreator->correct(*pPRD,*pTP);
  if(pRIO==nullptr) {
    if(pTP!=nullptr) delete pTP;
    return nullptr;
  }
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
  typePattern.set(Trk::TrackStateOnSurface::Measurement);
  typePattern.set(Trk::TrackStateOnSurface::Scatterer);
  Trk::FitQualityOnSurface* pFQ=new Trk::FitQualityOnSurface(pN->m_getChi2(),pN->m_getNdof());
  //pTSS = new Trk::TrackStateOnSurface(pRIO, pTP, pFQ, 0, typePattern);
  delete pTP;
  pTSS = new Trk::TrackStateOnSurface(pRIO, 0, pFQ, 0, typePattern);
  return pTSS;
}
