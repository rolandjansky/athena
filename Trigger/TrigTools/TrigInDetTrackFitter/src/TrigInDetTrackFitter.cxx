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

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetTrackFitter.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "TrigInDetToolInterfaces/TrigL2HitResidual.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

TrigInDetTrackFitter::TrigInDetTrackFitter(const std::string& t, 
					   const std::string& n,
					   const IInterface*  p ): AthAlgTool(t,n,p),
								   m_trackMaker("TrigDkfTrackMakerTool")
{
  declareInterface< ITrigInDetTrackFitter >( this );
  
  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremm=false);  
  declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
  declareProperty( "correctClusterPos", m_correctClusterPos = false);
  declareProperty( "ROTcreator", m_ROTcreator, "ROTcreatorTool" );
  m_nTracksTotal = 0;
  m_fitErrorsUnresolved=0;
  m_fitErrorsDivergence=0;
  m_fitErrorsLowPt=0;
}

StatusCode TrigInDetTrackFitter::initialize()
{
  ATH_CHECK(m_trackMaker.retrieve());
  if (m_correctClusterPos) {
    ATH_CHECK(m_ROTcreator.retrieve());
  }
  ATH_CHECK( m_fieldCondObjInputKey.initialize());
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
  ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID")); 
  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));  

  return StatusCode::SUCCESS;

}

StatusCode TrigInDetTrackFitter::finalize()
{
  ATH_MSG_INFO("==============================================================");
  ATH_MSG_INFO("TrigInDetTrackFitter::finalize() - LVL2 Track fit Statistics: ");
  ATH_MSG_INFO(" N tracks = "<<m_nTracksTotal);
  ATH_MSG_INFO("Problems detected: ");
  ATH_MSG_INFO("Unresolved spacepoints :"<< m_fitErrorsUnresolved);
  ATH_MSG_INFO("Extrapolator divergence:"<< m_fitErrorsDivergence);
  ATH_MSG_INFO("pT falls below 200 MeV :"<< m_fitErrorsLowPt);
  ATH_MSG_INFO("==============================================================");
  return StatusCode::SUCCESS;
}

void TrigInDetTrackFitter::getMagneticField(double r[3],double* B, MagField::AtlasFieldCache& fieldCache) const {
  B[0]=0.0;B[1]=0.0;B[2]=0.0;
	double field[3];
	fieldCache.getField(r,field);//field is returned in kT
	for(int i=0;i<3;i++) B[i]=field[i]/Gaudi::Units::kilogauss;//convert to kG
}

void TrigInDetTrackFitter::correctScale(Trk::TrkTrackState* pTS) const {

  double Rf[5];
  double Gf[5][5];
  int i,j;

  for(i=0;i<4;i++) Rf[i] = pTS->getTrackState(i);
  Rf[4] = 0.001*pTS->getTrackState(4);

  for(i=0;i<4;i++)
    for(j=0;j<4;j++) Gf[i][j] = pTS->getTrackCovariance(i,j);

  Gf[0][4] = Gf[4][0] = pTS->getTrackCovariance(0,4)/1000.0;
  Gf[1][4] = Gf[4][1] = pTS->getTrackCovariance(1,4)/1000.0;
  Gf[2][4] = Gf[4][2] = pTS->getTrackCovariance(2,4)/1000.0;
  Gf[3][4] = Gf[4][3] = pTS->getTrackCovariance(3,4)/1000.0;
  Gf[4][4] = pTS->getTrackCovariance(4,4)/1000000.0;

  pTS->setTrackState(Rf);
  pTS->setTrackCovariance(Gf);
}

Trk::TrkTrackState* TrigInDetTrackFitter::extrapolate(Trk::TrkTrackState* pTS, 
                                                      Trk::TrkPlanarSurface* pSB,
                                                      Trk::TrkPlanarSurface* pSE,
                                                      MagField::AtlasFieldCache& fieldCache) const
{
  const double C=0.02999975/1000.0;//using GeV internally 
  const double minStep=30.0;
	    
  double J[5][5],Rf[5],AG[5][5],Gf[5][5],A[5][5];
  int i,j,m;

  bool samePlane=false;

  if(pSB!=NULL)
    {   
      double diff=0.0;
      for(i=0;i<4;i++) diff+=fabs(pSE->getPar(i)-pSB->getPar(i));
      if(diff<1e-5) {
        samePlane=true;	
        m_fitErrorsUnresolved++;
      }
    }

  if(!samePlane) {

    double gP[3],gPi[3],lP[3],gV[3],a,b,c,s,J0[7][5],descr,CQ,Ac,Av,Cc;
    double V[3],P[3],M[3][3],D[4],Jm[7][7],
      J1[5][7],gB[3],gBi[3],gBf[3],dBds[3],Buf[5][7],DVx,DVy,DVz;
    int nStep,nStepMax;
    double sl,ds,path=0.0;

    double sint,cost,sinf,cosf;
    sint=sin(pTS->getTrackState(3));cosf=cos(pTS->getTrackState(2));
    sinf=sin(pTS->getTrackState(2));cost=cos(pTS->getTrackState(3));
    gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*pTS->getTrackState(4);

    memset(&J0[0][0],0,sizeof(J0));

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
      }
    for(i=0;i<4;i++) D[i]=pSE->getPar(i);
    for(i=0;i<3;i++) gPi[i]=gP[i];
  
    getMagneticField(gP,gB, fieldCache);

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
    
    getMagneticField(P,gB,fieldCache);
  
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
    pSE->transformPointToLocal(gP,lP);
    Rf[0]=lP[0];Rf[1]=lP[1];
    Rf[2]=atan2(V[1],V[0]);

    if(fabs(V[2])>1.0) 
      {
	return NULL;
      }

    Rf[3]=acos(V[2]);
    Rf[4]=pTS->getTrackState(4);
    
    gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;

    for(i=0;i<4;i++) D[i]=pSE->getPar(i);
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
    if (std::abs(V[2]) > 1.0) {
      return nullptr;
    }
    
    pSE->transformPointToLocal(P,lP);
  
    memset(&Jm[0][0],0,sizeof(Jm));
    
    for(i=0;i<3;i++) for(j=0;j<3;j++) M[i][j]=pSE->getRotMatrix(i,j);
    
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
    Rf[0]=pTS->getTrackState(0);
    Rf[1]=pTS->getTrackState(1);	
    Rf[2]=pTS->getTrackState(2);
    Rf[3]=pTS->getTrackState(3);
    Rf[4]=pTS->getTrackState(4);
    memset(&J[0][0],0,sizeof(J));
    for(i=0;i<5;i++) J[i][i]=1.0;
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

  Trk::TrkTrackState* pTE=new Trk::TrkTrackState(pTS);

  //workaround to keep using existing TrkTrackState code
  double Rtmp[5];

  for(i=0;i<4;i++) Rtmp[i] = Rf[i];
  Rtmp[4] = 0.001*Rf[4];//GeV->MeV

  pTE->setTrackState(Rtmp);
  pTE->setTrackCovariance(Gf);
  pTE->attachToSurface(pSE);

  //  pTE->applyMaterialEffects();
  if(m_doMultScatt)
    pTE->applyMultipleScattering();

  pTE->setTrackState(Rf);//restore

  if(m_doBremm) 
    pTE->applyEnergyLoss(1);

  AmgSymMatrix(5) Gi;
  for(i=0;i<5;i++) for(j=i;j<5;j++)
    {
      Gi.fillSymmetric(i, j, pTE->getTrackCovariance(i,j));
    }
  Gi = Gi.inverse();
 
  for(i=0;i<5;i++) for(j=0;j<5;j++)
    {
      A[i][j]=0.0;
      for(m=0;m<5;m++) A[i][j]+=AG[m][i]*Gi(m,j);
    }
  pTE->setPreviousState(pTS);
  pTE->setSmootherGain(A);

  return pTE;
}

void TrigInDetTrackFitter::fit(const TrackCollection& inputTracks, TrackCollection& fittedTracks, const EventContext& ctx, const Trk::ParticleHypothesis& matEffects) const
{
  MagField::AtlasFieldCache fieldCache;

  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

  fieldCondObj->getInitializedCache (fieldCache);
  fittedTracks.reserve(inputTracks.size());
  for(auto trIt = inputTracks.begin(); trIt != inputTracks.end(); ++trIt) {
		Trk::Track* fittedTrack = fitTrack(**trIt, fieldCache, matEffects);
		if (fittedTrack!=nullptr) {
			fittedTracks.push_back(fittedTrack);
		}
	}
}

Trk::Track* TrigInDetTrackFitter::fitTrack(const Trk::Track& recoTrack, MagField::AtlasFieldCache& fieldCache, const Trk::ParticleHypothesis& matEffects) const {

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

	std::vector<Trk::TrkBaseNode*> vpTrkNodes;
	std::vector<Trk::TrkTrackState*> vpTrackStates;
  vpTrackStates.reserve(vpTrkNodes.size() + 1);
	bool trackResult = m_trackMaker->createDkfTrack(recoTrack,vpTrkNodes, m_DChi2);
	int nHits=vpTrkNodes.size();
	ATH_MSG_VERBOSE(nHits<<" filtering nodes created");

	if(!trackResult) return nullptr;

	// 3. Main algorithm: filter and smoother (Rauch-Tung-Striebel)
  m_nTracksTotal++;
	Trk::TrkTrackState* pTS = new Trk::TrkTrackState(Rk);
	double Gk[5][5] = {{100.0,     0, 0,    0,    0},
			   {0,     100.0, 0,    0,    0},
			   {0,         0, 0.01, 0,    0},
			   {0,         0, 0,    0.01, 0},
			   {0,         0, 0,    0,   0.1}};
	pTS->setTrackCovariance(Gk);
	if(m_doMultScatt)  
		pTS->setScatteringMode(1);
	if(m_doBremm)
		pTS->setScatteringMode(2);
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
		pSE=(*pnIt)->getSurface();
		Trk::TrkTrackState* pNS=extrapolate(pTS,pSB,pSE,fieldCache);

		pSB=pSE;
		if(pNS!=nullptr) {
			vpTrackStates.push_back(pNS);

			(*pnIt)->validateMeasurement(pNS);
			ATH_MSG_VERBOSE("dChi2="<<(*pnIt)->getChi2());
			if((*pnIt)->isValidated())
			{
				chi2tot+=(*pnIt)->getChi2();
				ndoftot+=(*pnIt)->getNdof();
			}
			(*pnIt)->updateTrackState(pNS);
			pTS=pNS;
			double est_Pt = 1000.0*sin(pTS->getTrackState(3))/pTS->getTrackState(4);
			if(fabs(est_Pt)<200.0)
			{
				ATH_MSG_VERBOSE("Estimated Pt is too low "<<est_Pt<<" - skipping fit");
        m_fitErrorsLowPt++;
				OK=false;break;
			}
		}
		else
		{
      m_fitErrorsDivergence++;
			OK=false;break;
		}
	}
	Trk::Track* fittedTrack = nullptr;
	if(OK)
	{
		for(auto ptsIt = vpTrackStates.rbegin();ptsIt!=vpTrackStates.rend();++ptsIt)
		{
			(*ptsIt)->runSmoother();
		}
		pTS=(*vpTrackStates.begin());
		//correct GeV->MeV

		correctScale(pTS);
		
    double qOverP = pTS->getTrackState(4);
		double pt=sin(pTS->getTrackState(3))/pTS->getTrackState(4);
		double phi0 = pTS->getTrackState(2);
		if(phi0>M_PI) phi0-=2*M_PI;
		if(phi0<-M_PI) phi0+=2*M_PI;
    double theta = pTS->getTrackState(3);
		double z0 = pTS->getTrackState(1);
		double d0 = pTS->getTrackState(0);
    bool bad_cov = false;
    auto cov = std::unique_ptr<AmgSymMatrix(5)>(new AmgSymMatrix(5));
    for(int i=0;i<5;i++) {
      double cov_diag = pTS->getTrackCovariance(i,i);
      if (cov_diag < 0) {
        bad_cov = true;//Diagonal elements must be positive
        break;
        ATH_MSG_DEBUG("REGTEST: cov(" << i << "," << i << ") =" << cov_diag << " < 0, reject track");
      }
      (*cov)(i, i) = pTS->getTrackCovariance(i,i);
      for(int j=i+1;j<5;j++) {
        cov->fillSymmetric(i, j, pTS->getTrackCovariance(i,j));
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
          if((*pnIt)->isValidated()) {
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
		delete((*pnIt)->getSurface());
		delete (*pnIt);
	}
	vpTrkNodes.clear();
	for(auto ptsIt = vpTrackStates.begin();ptsIt!=vpTrackStates.end();++ptsIt) {
		delete (*ptsIt);
	}
	vpTrackStates.clear();

	return fittedTrack;
}

Trk::TrackStateOnSurface* TrigInDetTrackFitter::createTrackStateOnSurface(Trk::TrkBaseNode* pN) const
{
  Trk::TrackStateOnSurface* pTSS=nullptr;
  char type=pN->getNodeType();
  const Trk::TrackParameters* pTP=nullptr;

  if(type==0) return pTSS;


  Trk::TrkTrackState* pTS=pN->getTrackState();
  auto pM = std::unique_ptr<AmgSymMatrix(5)>(new AmgSymMatrix(5));
  for(int i=0;i<5;i++) {
    for(int j=i;j<5;j++) {
      (*pM)(i,j)=pTS->getTrackCovariance(i,j);
    }
  }
  const Trk::PrepRawData* pPRD=pN->getPrepRawData();

  if((type==1)||(type==2))
  {
    const Trk::Surface& rS = pPRD->detectorElement()->surface();
    const Trk::PlaneSurface* pPS = dynamic_cast<const Trk::PlaneSurface*>(&rS);
    if(pPS==nullptr) return pTSS;

    pTP=new Trk::AtaPlane(pTS->getTrackState(0),
        pTS->getTrackState(1),
        pTS->getTrackState(2),
        pTS->getTrackState(3),
        pTS->getTrackState(4),*pPS,
        pM.release());
  }
  else if(type==3)
  {
    const Trk::Surface& rS = pPRD->detectorElement()->surface(pPRD->identify()); 
    const Trk::StraightLineSurface* pLS=dynamic_cast<const Trk::StraightLineSurface*>(&rS);
    if(pLS==nullptr) return pTSS;

    if((pTS->getTrackState(2)<-M_PI) ||(pTS->getTrackState(2)>M_PI)) {
      ATH_MSG_WARNING("Phi out of range when correcting Trk::TrackStateOnSurface");
    }


    pTP=new Trk::AtaStraightLine(pTS->getTrackState(0),
        pTS->getTrackState(1),
        pTS->getTrackState(2),
        pTS->getTrackState(3),
        pTS->getTrackState(4),
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
  Trk::FitQualityOnSurface* pFQ=new Trk::FitQualityOnSurface(pN->getChi2(),pN->getNdof());
  //pTSS = new Trk::TrackStateOnSurface(pRIO, pTP, pFQ, 0, typePattern);
  delete pTP;
  pTSS = new Trk::TrackStateOnSurface(pRIO, 0, pFQ, 0, typePattern);
  return pTSS;
}

StatusCode TrigInDetTrackFitter::getUnbiasedResiduals(const Trk::Track& pT, 
    std::vector<TrigL2HitResidual>& vResid, const EventContext& ctx) const {

	const Trk::TrackParameters* trackPars = pT.perigeeParameters();
  if(trackPars==nullptr) {
    ATH_MSG_WARNING("Fit Failed -- TrkTrack has no parameters");
    return StatusCode::FAILURE;
  }

  MagField::AtlasFieldCache fieldCache;

  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

  fieldCondObj->getInitializedCache (fieldCache);
  std::vector<Trk::TrkBaseNode*> vpTrkNodes;
  std::vector<Trk::TrkTrackState*> vpTrackStates;
  vResid.clear();
  double trk_theta = trackPars->parameters()[Trk::theta];
  double trk_qOverP = trackPars->parameters()[Trk::qOverP];
  double Pt = sin(trk_theta)/trk_qOverP;
  if(fabs(Pt)<100.0)
  {
    ATH_MSG_DEBUG("TrigL2ResidualCalculator failed -- Estimated Pt is too low "<<Pt);
    return StatusCode::FAILURE;
  }

  // 1. Create filtering nodes

  bool trackResult = m_trackMaker->createDkfTrack(pT,vpTrkNodes, m_DChi2);
  if(!trackResult)
  {
    ATH_MSG_DEBUG("TrigDkfTrackMaker failed");
    return StatusCode::FAILURE;
  }

  int nNodeIndex=-1;
  bool OK=true;
  std::vector<Trk::TrkBaseNode*>::iterator pnIt,pnEnd(vpTrkNodes.end());

  for(std::vector<Trk::TrkBaseNode*>::iterator pNodeIt=vpTrkNodes.begin();pNodeIt!=vpTrkNodes.end();
      ++pNodeIt)
  {
    nNodeIndex++;
    Trk::TrkBaseNode* pMaskedNode=(*pNodeIt);
    Trk::TrkTrackState* pMaskedState=NULL;

    // 2. Create initial track state:

    double Rk[5];
    Rk[0] = trackPars->parameters()[Trk::d0]; 
    Rk[1] = trackPars->parameters()[Trk::z0]; 
    Rk[2] = trackPars->parameters()[Trk::phi0]; 
    if(Rk[2]>M_PI) Rk[2]-=2*M_PI; 
    if(Rk[2]<-M_PI) Rk[2]+=2*M_PI;
    trk_theta = trackPars->parameters()[Trk::theta];
    Rk[3] = trk_theta;
    Rk[4] = 1000.0*trackPars->parameters()[Trk::qOverP];//MeV->GeV
    //No need to correct scale back - not returning track

    // 3. Main algorithm: filter and smoother (Rauch-Tung-Striebel)

    Trk::TrkTrackState* pTS = new Trk::TrkTrackState(Rk);
    double Gk[5][5] = {{100.0,     0, 0,    0,    0},
                       {0,     100.0, 0,    0,    0},
                       {0,         0, 0.01, 0,    0},
                       {0,         0, 0,    0.01, 0},
                       {0,         0, 0,    0,   0.1}};
    pTS->setTrackCovariance(Gk);
    if(m_doMultScatt)  
      pTS->setScatteringMode(1);
    if(m_doBremm)
      pTS->setScatteringMode(2);
    vpTrackStates.push_back(pTS);

    ATH_MSG_DEBUG("Initial params: locT="<<Rk[0]<<" locL="<<Rk[1]<<" phi="<<Rk[2]
        <<" theta="<<Rk[3]<<" Q="<<Rk[4]<<" pT="<<sin(Rk[3])/Rk[4]);

    OK=true;
    Trk::TrkPlanarSurface *pSB=NULL,*pSE;

    for(pnIt=vpTrkNodes.begin();pnIt!=pnEnd;++pnIt)
    {
      pSE=(*pnIt)->getSurface();
      Trk::TrkTrackState* pNS=extrapolate(pTS,pSB,pSE,fieldCache);

      pSB=pSE;
      if(pNS!=NULL)
      {
        vpTrackStates.push_back(pNS);

        (*pnIt)->validateMeasurement(pNS);
        ATH_MSG_DEBUG("dChi2="<<(*pnIt)->getChi2());
        if((*pnIt)!=pMaskedNode)
        {
          (*pnIt)->updateTrackState(pNS);
        }
        else
        {
          pMaskedState=pNS;
        }
        pTS=pNS;
        Pt=sin(pTS->getTrackState(3))/pTS->getTrackState(4);
        if(fabs(Pt)<0.2)
        {
          ATH_MSG_DEBUG("Estimated Pt is too low "<<Pt<<" - skipping fit");
          OK=false;break;
        }
      }
      else
      {
        OK=false;break;
      }
    }
    if(OK)
    {
      std::vector<Trk::TrkTrackState*>::reverse_iterator ptsrIt(vpTrackStates.rbegin()),
        ptsrEnd(vpTrackStates.rend());

      for(;ptsrIt!=ptsrEnd;++ptsrIt)
      {
        (*ptsrIt)->runSmoother();
      }

      pMaskedNode->validateMeasurement(pMaskedState);

      double r[2],V[2][2];

      int nSize=pMaskedNode->getResiduals(r);
      nSize=pMaskedNode->getInverseResidualVariance(V);
      const Trk::PrepRawData* pPRD = pMaskedNode->getPrepRawData();

      Identifier id = pPRD->identify();

      Region region = Region::Undefined;
      if(m_idHelper->is_pixel(id))
      {
        region=(m_pixelId->is_barrel(id)) ? Region::PixBarrel: Region::PixEndcap;
        if (m_pixelId->is_blayer(id)) {
          region = Region::IBL;
        }
      }
      if(m_idHelper->is_sct(id))
      {
        region=(m_sctId->is_barrel(id)) ? Region::SctBarrel : Region::SctEndcap;
      }
      if(nSize==1) {
        if(V[0][0]>0.0) {
          vResid.push_back(TrigL2HitResidual(id,region,r[0],r[0]*sqrt(V[0][0])));
        }
        else {
          OK=false;
          break;
        }
      }
      else {
        if((V[0][0]>0.0) && (V[1][1]>0.0)) {
          vResid.push_back(TrigL2HitResidual(id,region,r[0],r[0]*sqrt(V[0][0]),
                r[1],r[1]*sqrt(V[1][1])));
        }
        else {
          OK=false;
          break;
        }
      }
    }
    else
    {
      ATH_MSG_DEBUG("Forward Kalman filter: extrapolation failure ");
      vResid.clear();
    }
    for(std::vector<Trk::TrkTrackState*>::iterator ptsIt=vpTrackStates.begin();
        ptsIt!=vpTrackStates.end();++ptsIt) delete (*ptsIt);
    vpTrackStates.clear();
    if(!OK) break;
  }
  pnIt=vpTrkNodes.begin();pnEnd=vpTrkNodes.end();
  for(;pnIt!=pnEnd;++pnIt) 
  {
    delete((*pnIt)->getSurface());
    delete (*pnIt);
  }
  vpTrkNodes.clear();

  if(OK) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;

}
