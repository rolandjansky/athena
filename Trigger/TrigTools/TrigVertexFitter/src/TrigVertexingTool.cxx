/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigVertexingTool tool
// -------------------------------
// ATLAS Collaboration
//
// 04.07.2006 Tool created
//
// This tool implements some basic operations needed for
// vertex reconstruction:
//  - finding a point of the closest approach between two tracks
//  - min. distance between two tracks, track and vertex
//  - chi2 distances calculation
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigL2Vertex.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigVertexingTool.h"

TrigVertexingTool::TrigVertexingTool(const std::string& t, 
				     const std::string& n,
				     const IInterface*  p ): AthAlgTool(t,n,p)
{
  declareInterface< ITrigVertexingTool >( this );  
}


StatusCode TrigVertexingTool::initialize()
{
  ATH_CHECK( AthAlgTool::initialize() );

  ServiceHandle<ITrigTimerSvc> timerSvc ("TrigTimerSvc", name());
  if( timerSvc.retrieve().isFailure() )
    {
      ATH_MSG_INFO( "Unable to locate Service TrigTimerSvc "  );
      m_timers = false;
    } 
  else m_timers = true;  
  if(m_timers) 
    {
      m_timer[0] = timerSvc->addItem("LVTAddTrack");
      m_timer[1] = timerSvc->addItem("LVTMass");
      m_timer[2] = timerSvc->addItem("LVTCreateV");
    }
  ATH_MSG_INFO( "TrigVertexingTool constructed " );
  return StatusCode::SUCCESS;
}

StatusCode TrigVertexingTool::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

TrigVertexingTool::~TrigVertexingTool()
{

}
/*
double TrigVertexingTool::FindClosestApproach(const Trk::Track* pT1, const Trk::Track* pT2, double* V)
{
  V[0]=0.0;V[1]=0.0;V[2]=0.0;
  return -999.0;
}

double TrigVertexingTool::FindClosestApproach(const TrigInDetTrack* pT1, const TrigInDetTrack* pT2, double* V)
{
  V[0]=0.0;V[1]=0.0;V[2]=0.0;
  return -999.0;
}


double TrigVertexingTool::TrackToTrackDistance(const Trk::Track* pT1, const Trk::Track* pT2)
{
  return -999.0;
}

double TrigVertexingTool::TrackToTrackDistance(const TrigInDetTrack* pT1, const TrigInDetTrack* pT2)
{
  return -999.0;
}

double TrigVertexingTool::TrackToVertexDistance(const Trk::Track* pT, const TrigL2Vertex* pV)
{
  return -999.0;
}

double TrigVertexingTool::TrackToVertexDistance(const TrigInDetTrack* pT, const TrigL2Vertex* pV)
{
  return -999.0;
}

double TrigVertexingTool::TrackToTrackChi2Distance(const Trk::Track* pT1, const Trk::Track* pT2)
{
  return -999.0;
}

double TrigVertexingTool::TrackToTrackChi2Distance(const TrigInDetTrack* pT1, const TrigInDetTrack* pT2)
{
  return -999.0;
}

double TrigVertexingTool::TrackToVertexChi2Distance(const Trk::Track* pT, const TrigL2Vertex* pV)
{
  return -999.0;
}

double TrigVertexingTool::TrackToVertexChi2Distance(const TrigInDetTrack* pT, const TrigL2Vertex* pV)
{
  return -999.0;
}
*/
double TrigVertexingTool::FindClosestApproach(const TrigVertexFitInputTrack* pT1,
					      const TrigVertexFitInputTrack* pT2, 
					      double V[3])
{
  return twoCircleClosestApproach(pT1->Perigee(),pT2->Perigee(),V);
}


TrigVertex* TrigVertexingTool::createTrigVertex(TrigL2Vertex* pV)
{
  if(!pV->isVertexFitted())
    {
      ATH_MSG_WARNING( "Cannot create TrigVertex: input TrigL2Vertex is not fitted" );
      return NULL;
    }

  if(m_timers) m_timer[2]->start();

  TrackInVertexList* pTVL=new TrackInVertexList;
  pTVL->clear();

  for(std::list<TrigVertexFitInputTrack*>::iterator it=pV->getTracks()->begin();
      it!=pV->getTracks()->end();++it)
    {
      if((*it)->getTrackType()!=1) continue;
      pTVL->push_back((*it)->getTrigTrack());
    }
  double* Rk=pV->getParametersVector();
  //  TrigVertexCovariance& Gk=(*pV->getCovariance());
  double cv[6];

  cv[0]=pV->m_Gk[0][0];cv[1]=pV->m_Gk[0][1];cv[2]=pV->m_Gk[1][1];
  cv[3]=pV->m_Gk[0][2];cv[4]=pV->m_Gk[1][2];cv[5]=pV->m_Gk[2][2];

  TrigVertex* pTV=new TrigVertex(Rk[0],Rk[1],Rk[2],cv,pV->chi2(),pV->ndof(),pTVL);

  pTV->setMass(pV->mass());
  pTV->setMassVariance(pV->massVariance());
  if(pV->getMotherTrack()!=NULL) 
  {
    pTV->setMotherTrack(pV->getMotherTrack());
    pV->setMotherTrack(NULL);
  }

  if(m_timers) m_timer[2]->stop();

  return pTV;
}


StatusCode TrigVertexingTool::setMassConstraint(TrigL2Vertex* pV, const TrigInDetTrack* pT1, 
						const TrigInDetTrack* pT2, double m)
{
  int nFound=0;
  MsgStream athenaLog(msgSvc(), name());  
  const TrigVertexFitInputTrack *pI1;
  const TrigVertexFitInputTrack *pI2;

  if(m<0.0)
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - mass is negative " );
      return StatusCode::FAILURE;
    }

  pI1=pV->contains(pT1);pI2=pV->contains(pT2);

  if(pI1!=NULL) nFound++;
  if(pI2!=NULL) nFound++;
  if(nFound!=2) 
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - no such tracks in vertex " );
      return StatusCode::FAILURE;
    }
  pV->getConstraints()->push_back(new TrigVertexFitConstraint(m,pI1,pI2));
  return StatusCode::SUCCESS;
}


StatusCode TrigVertexingTool::setMassConstraint(TrigL2Vertex* pV, const TrigInDetTrack* pT1, 
						const TrigInDetTrack* pT2, 
						const TrigInDetTrack* pT3, double m)
{
  int nFound=0;

  MsgStream athenaLog(msgSvc(), name());
  const TrigVertexFitInputTrack *pI1;
  const TrigVertexFitInputTrack *pI2;
  const TrigVertexFitInputTrack *pI3;

  if(m<0.0)
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - mass is negative " );
      return StatusCode::FAILURE;
    }

  pI1=pV->contains(pT1);pI2=pV->contains(pT2);pI3=pV->contains(pT3);

  if(pI1!=NULL) nFound++;
  if(pI2!=NULL) nFound++;
  if(pI3!=NULL) nFound++;
  if(nFound!=3) 
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - no such tracks in vertex " );
      return StatusCode::FAILURE;
    }
  pV->getConstraints()->push_back(new TrigVertexFitConstraint(m,pI1,pI2,pI3));
  return StatusCode::SUCCESS;
}

StatusCode TrigVertexingTool::setMassConstraint(TrigL2Vertex* pV, const Trk::Track* pT1, const Trk::Track* pT2, double m)
{
  int nFound=0;

  const TrigVertexFitInputTrack *pI1;
  const TrigVertexFitInputTrack *pI2;

  if(m<0.0)
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - mass is negative " );
      return StatusCode::FAILURE;
    }

  pI1=pV->contains(pT1);pI2=pV->contains(pT2);

  if(pI1!=NULL) nFound++;
  if(pI2!=NULL) nFound++;
  if(nFound!=2) 
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - no such tracks in vertex " );
      return StatusCode::FAILURE;
    }
  pV->getConstraints()->push_back(new TrigVertexFitConstraint(m,pI1,pI2));
  return StatusCode::SUCCESS;
}

StatusCode TrigVertexingTool::setMassConstraint(TrigL2Vertex* pV, const Trk::Track* pT1, const Trk::Track* pT2, 
			     const Trk::Track* pT3, double m)
{
  int nFound=0;

  const TrigVertexFitInputTrack *pI1;
  const TrigVertexFitInputTrack *pI2;
  const TrigVertexFitInputTrack *pI3;
  if(m<0.0)
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - mass is negative " );
      return StatusCode::FAILURE;
    }

  pI1=pV->contains(pT1);pI2=pV->contains(pT2);pI3=pV->contains(pT3);

  if(pI1!=NULL) nFound++;
  if(pI2!=NULL) nFound++;
  if(pI3!=NULL) nFound++;
  if(nFound!=3) 
    {
      ATH_MSG_WARNING( "Cannot setup mass constraint - no such tracks in vertex " );
      return StatusCode::FAILURE;
    }
  pV->getConstraints()->push_back(new TrigVertexFitConstraint(m,pI1,pI2,pI3));
  return StatusCode::SUCCESS;
}

/*
StatusCode removeTrack(const TrigInDetTrack*)
{
  return StatusCode::SUCCESS;
}
*/

StatusCode TrigVertexingTool::addTrack(const TrigInDetTrack* pT, TrigL2Vertex* pV, const Trk::ParticleHypothesis h)
{
  Trk::ParticleMasses M;
  return addTrack(pT,pV,M.mass[h]);
}

StatusCode TrigVertexingTool::addTrack(const Trk::Track* pT, TrigL2Vertex* pV, const Trk::ParticleHypothesis h)
{
  Trk::ParticleMasses M;
  return addTrack(pT,pV,M.mass[h]);
}

StatusCode TrigVertexingTool::addTrack(const TrigInDetTrack* pT, TrigL2Vertex* pV, double mass=0.0)
{
  //1. check perigee and cov matrix

  const TrigInDetTrackFitPar* p=pT->param();
  if(p==NULL)
    {
      ATH_MSG_WARNING( "TrigInDetTrack has no perigee - cannot be added to a vertex " );
      return StatusCode::FAILURE;
    }
  if(p->cov()==NULL)
    {
      ATH_MSG_WARNING( "TrigInDetTrackFitPar has no covariance - track cannot be added to a vertex " );
      return StatusCode::FAILURE;
    }  
  //2. new vertex fitting node

  if(m_timers) m_timer[0]->start();

  TrigVertexFitInputTrack* pFN=new TrigVertexFitInputTrack(pT,mass);

  //3. adding track to vertex


  pFN->setIndex(pV->getTracks()->size());
  pV->getTracks()->push_back(pFN);

  if(m_timers) m_timer[0]->stop();

  return StatusCode::SUCCESS;
}

StatusCode TrigVertexingTool::addTrack(const Trk::Track* pT,  TrigL2Vertex* pV, double mass=0.0)
{
  //1. check perigee and cov matrix

  if(pT->trackParameters()->empty()) 
    {
      ATH_MSG_WARNING( "Trk::Track has no parameters - cannot be added to a vertex " );
      return StatusCode::FAILURE;
    }

  //2. new vertex fitting node

  if(m_timers) m_timer[0]->start();

  TrigVertexFitInputTrack* pFN=new TrigVertexFitInputTrack(pT,mass);

  //3. adding track to vertex
  pFN->setIndex(pV->getTracks()->size());
  pV->getTracks()->push_back(pFN);

  if(m_timers) m_timer[0]->stop();
  return StatusCode::SUCCESS;
}

double TrigVertexingTool::twoCircleClosestApproach(const double *P1, const double *P2, double *V)
{
  const double C=0.029997;
  const double B=20.84;

  int i,j,nCase;
  double d0[2],z0[2],ctt[2],xc[2],yc[2],R[2],
    alpha=C*B,a,dxc,dyc,h,x[2],y[2],psi[2][2],rv[2],
    dz[2],delta,r[2],xa,ya,minDist=-999.0,coeff=1000.0/alpha;

  d0[0]=P1[0];z0[0]=P1[1];
  ctt[0]=cos(P1[3])/sin(P1[3]);
  R[0]=-P1[4]*coeff;
  r[0]=fabs(R[0]);
  xc[0]=-(d0[0]+R[0])*sin(P1[2]);
  yc[0]=(d0[0]+R[0])*cos(P1[2]);
  ATH_MSG_DEBUG("Helix 1: xc="<<xc[0]<<" yc="<<yc[0]<<" R="<<R[0]<<" z0="<<z0[0] );
  d0[1]=P2[0];z0[1]=P2[1];
  ctt[1]=cos(P2[3])/sin(P2[3]);
  R[1]=-P2[4]*coeff;
  r[1]=fabs(R[1]);
  xc[1]=-(d0[1]+R[1])*sin(P2[2]);
  yc[1]=(d0[1]+R[1])*cos(P2[2]);
  ATH_MSG_DEBUG("Helix 2: xc="<<xc[1]<<" yc="<<yc[1]<<" R="<<R[1]<<" z0="<<z0[1] );
  dxc=xc[1]-xc[0];dyc=yc[1]-yc[0];
  dz[0]=z0[1]-z0[0];dz[1]=dz[0];
  const double d=sqrt(dxc*dxc+dyc*dyc);
  const double inv_d = 1. / d;

  if(d==0.0)
    {
      ATH_MSG_DEBUG("twoCircleClosestApproach failed - the circles have the same center" );
      return -999.0;
    }
  nCase=0;
  if(d>r[0]+r[1]) nCase=1;
  if(d<fabs(r[0]-r[1])) nCase=2;

  switch(nCase)
    {
    case 0:
      {
        ATH_MSG_DEBUG("Case 0: intersection" );
	a=0.5*(r[0]*r[0]-r[1]*r[1]+d*d)*inv_d;
	double ad=a*inv_d;
	xa=xc[0]+dxc*ad;ya=yc[0]+dyc*ad;
	h=sqrt(r[0]*r[0]-a*a);
	double hd=h*inv_d;
	x[0]=xa-dyc*hd;y[0]=ya+dxc*hd;
	x[1]=xa+dyc*hd;y[1]=ya-dxc*hd;
	for(i=0;i<2;i++)
	  {
	    rv[i]=sqrt(x[i]*x[i]+y[i]*y[i]);
	    for(j=0;j<2;j++)
	      {
		double Rdj=R[j]+d0[j];
		double cosa=0.5*(R[j]/Rdj+Rdj/R[j]-
				 rv[i]*rv[i]/(R[j]*Rdj));
                ATH_MSG_DEBUG("cosPsi="<<cosa );
		if(fabs(cosa)>1.0) {
		  ATH_MSG_DEBUG("twoCircleClosestApproach failed - cosPsi > 1" );
		  return -999.0;
		}
		psi[i][j]=acos(cosa);
	      }
	    dz[i]+=R[1]*psi[i][1]*ctt[1]-R[0]*psi[i][0]*ctt[0];
	  }
        ATH_MSG_DEBUG("Solution I: dz="<<dz[0]<<" xv="<<x[0]<<" yv="<<y[0]<<" psi1="<<psi[0][0]
                      <<" psi2="<<psi[0][1] );
        ATH_MSG_DEBUG("Solution II: dz="<<dz[1]<<" xv="<<x[1]<<" yv="<<y[1]<<" psi1="<<psi[1][0]
                      <<" psi2="<<psi[1][1] );
	i=(fabs(dz[0])<fabs(dz[1]))?0:1;
	if(rv[i]<500.0) {
          ATH_MSG_DEBUG("Selected Solution is "<<i+1 );
	  V[0]=x[i];V[1]=y[i];
	  V[2]=0.5*(z0[0]+R[0]*psi[i][0]*ctt[0]+z0[1]+R[1]*psi[i][1]*ctt[1]);
	  minDist=fabs(dz[i]);
	}
	else {
          ATH_MSG_DEBUG("Both Solutions rejected " );
	}
	break;
      }
    case 1:
      {
        ATH_MSG_DEBUG("Case 1: Circles are separated" );
	delta=0.5*(d-r[0]-r[1]);
	double rd=(r[0]+delta)*inv_d;
	x[0]=xc[0]+dxc*rd;y[0]=yc[0]+dyc*rd;
	rv[0]=sqrt(x[0]*x[0]+y[0]*y[0]);
	for(j=0;j<2;j++)
	  {
	    double Rdj=fabs(R[j]+d0[j]);
	    double fRd=fabs(fabs(R[j])+delta);
	    double cosa=0.5*(fRd/Rdj+Rdj/fRd-rv[0]*rv[0]/(fRd*Rdj));
            ATH_MSG_DEBUG("cosPsi="<<cosa );
	    if(fabs(cosa)>1.0) {
	      ATH_MSG_DEBUG("twoCircleClosestApproach failed - cosPsi > 1" );
	      return -999.0;
	    }
	    psi[0][j]=acos(cosa);
	  }
	dz[0]+=R[1]*psi[0][1]*ctt[1]-R[0]*psi[0][0]*ctt[0];
        ATH_MSG_DEBUG("Solution : dz="<<dz[0]<<" xv="<<x[0]<<" yv="<<y[0]<<" psi1="<<psi[0][0]
                      <<" psi2="<<psi[0][1] );
	V[0]=x[0];V[1]=y[0];
	V[2]=0.5*(z0[0]+R[0]*psi[0][0]*ctt[0]+z0[1]+R[1]*psi[0][1]*ctt[1]);
	minDist=sqrt(dz[0]*dz[0]+4.0*delta*delta);
	break;
      }
    case 2:
      {
        ATH_MSG_DEBUG("Case 2: One circle is inside the other" );

	int bigJ=(r[0]>r[1])?0:1;
	int smallJ=(r[0]<r[1])?0:1;
	dxc=xc[smallJ]-xc[bigJ];dyc=yc[smallJ]-yc[bigJ];
	dz[0]=z0[smallJ]-z0[bigJ];dz[1]=dz[0];
	
	x[smallJ]=xc[smallJ]+dxc*r[smallJ]*inv_d;
	y[smallJ]=yc[smallJ]+dyc*r[smallJ]*inv_d;
        x[bigJ]=xc[bigJ]+dxc*r[bigJ]*inv_d;
	y[bigJ]=yc[bigJ]+dyc*r[bigJ]*inv_d;
	
	for(j=0;j<2;j++)
	  {
	    double Phi=atan2(yc[j],xc[j]);
	    double sina=(x[j]*sin(Phi)-y[j]*cos(Phi))/R[j];
            ATH_MSG_DEBUG("sinPsi="<<sina );
	    if(fabs(sina)>1.0) {
	      ATH_MSG_DEBUG("twoCircleClosestApproach failed - sinPsi > 1" );
	      return -999.0;
	    }
	    psi[0][j]=asin(sina);
	  }
	dz[0]+=R[1]*psi[0][1]*ctt[1]-R[0]*psi[0][0]*ctt[0];
	x[0]=0.5*(x[smallJ]+x[bigJ]);y[0]=0.5*(y[smallJ]+y[bigJ]);
        rv[0]=sqrt(x[0]*x[0]+y[0]*y[0]);
        ATH_MSG_DEBUG("Solution : dz="<<dz[0]<<" xv="<<x[0]<<" yv="<<y[0]<<" psi1="<<psi[0][0]
                      <<" psi2="<<psi[0][1] );
	V[0]=x[0];V[1]=y[0];
	V[2]=0.5*(z0[0]+R[0]*psi[0][0]*ctt[0]+z0[1]+R[1]*psi[0][1]*ctt[1]);
	minDist=sqrt(dz[0]*dz[0]+(x[smallJ]-x[bigJ])*(x[smallJ]-x[bigJ])+(y[smallJ]-y[bigJ])*(y[smallJ]-y[bigJ]));
	break;
      }
    } 
  return minDist;
}


StatusCode TrigVertexingTool::calculateInvariantMass(TrigL2Vertex* pV)
{
  const double C=0.029997;
  const double B=20.84;

  if(!pV->isVertexFitted())
    {
      ATH_MSG_WARNING("Mass estimation: vertex is not fitted - run fit first" );
      return StatusCode::FAILURE;
    }

  if(m_timers) m_timer[1]->start();

  int nSize=pV->getTracks()->size();
  double invMass=0.0,alpha=C*B*1e-3;
  double P[3];
  double E=0.0;
  int i,j;
  
  //  double* Hk=new double[3*nSize+3];
  //memset(&Hk[0],0,sizeof(double)*(3*nSize+3));

  double Hk[200];

  double* Rk = pV->getParametersVector();

  int trackId=0;
  int offset=0;
  for(i=0;i<3;i++) P[i]=0.0;
  for(std::list<TrigVertexFitInputTrack*>::iterator it=pV->getTracks()->begin();
      it!=pV->getTracks()->end();++it)
    {
      offset+=3;trackId++;
      double mass=(*it)->getMass()*1e-3;
      double pT=fabs(Rk[offset+2]);
      double p=pT/sin(Rk[offset+1]);

      double sinPsi = alpha*(Rk[0]*cos(Rk[offset])+Rk[1]*sin(Rk[offset]))/Rk[offset+2];
      if(fabs(sinPsi) > 1.0) {
        return StatusCode::FAILURE;
      }

      double psi=-asin(sinPsi);
      double phiV=Rk[offset]+psi;
      ATH_MSG_DEBUG("Track "<<trackId<<" psi="<<psi<<"  phi0="<<Rk[offset]<<" phiV="<<
                    phiV<<" theta="<<Rk[offset+1]<<" pT="<<Rk[offset+2]<<" mass="<<mass );
      P[0]+=pT*cos(phiV);
      P[1]+=pT*sin(phiV);
      P[2]+=pT*cos(Rk[offset+1])/sin(Rk[offset+1]);
      E+=sqrt(mass*mass+p*p);
    }
  invMass=sqrt(E*E-P[0]*P[0]-P[1]*P[1]-P[2]*P[2])*1000.0;
  pV->setMass(invMass);

  offset=0;trackId=0;Hk[0]=0.0;Hk[1]=0.0;Hk[2]=0.0;
  for(std::list<TrigVertexFitInputTrack*>::iterator it=pV->getTracks()->begin();
      it!=pV->getTracks()->end();++it)
    {
      offset+=3;trackId++;

      double mass=(*it)->getMass()*1e-3;
      double Ck=(Rk[offset+2]<0.0)?-1.0:1.0;
      const double sinT=sin(Rk[offset+1]);
      const double inv_sinT = 1. / sinT;
      double cosT=cos(Rk[offset+1]);
      double pT=fabs(Rk[offset+2]);
      double p=pT*inv_sinT;
      double e=sqrt(p*p+mass*mass);
      double sinF=sin(Rk[offset]);
      double cosF=cos(Rk[offset]);

      double sinPsi=-alpha*(Rk[0]*cosF+Rk[1]*sinF)/Rk[offset+2];
      if(fabs(sinPsi) > 1.0) {
	return StatusCode::FAILURE;
      }
      double psi=asin(sinPsi);
      const double cosPsi=sqrt(1.0-sinPsi*sinPsi);
      const double inv_cosPsi = 1. / cosPsi;
      double phiV=Rk[offset]+psi;
      
      double aCos=alpha*Ck*inv_cosPsi;
      double dP=P[1]*cos(phiV)-P[0]*sin(phiV);
      double eE=E*Rk[offset+2]/(e*sinT);

      Hk[0]+=dP*cosF*aCos;
      Hk[1]+=dP*sinF*aCos;
      Hk[offset]=-dP*Ck*(Rk[offset+2]-Ck*aCos*(Rk[1]*cosF-Rk[0]*sinF));
      Hk[offset+1]=(Rk[offset+2]*inv_sinT*inv_sinT)*(P[2]*Ck-eE*cosT);
      Hk[offset+2]=eE*inv_sinT-Ck*(P[0]*cos(phiV)+P[1]*sin(phiV)+P[2]*cosT*inv_sinT)+dP*Ck*sinPsi*inv_cosPsi;
    }
  //  TrigVertexCovariance& Gk=(*pV->getCovariance());
  double covM=0.0;

  nSize=3+3*nSize;
  for(i=0;i<nSize;i++)
    {
      for(j=i;j<nSize;j++) 
	{
	  double dCov=pV->m_Gk[i][j]*Hk[i]*Hk[j];
	  if(i!=j) dCov*=2.0;
	  covM+=dCov;
	}
    }
  if (covM<0.0)
    {
      ATH_MSG_DEBUG("Negative mass covariance = "<<covM<<" probably fit divergence " );
      covM=fabs(covM);
    }
  covM=sqrt(covM)*1e6/invMass;
  pV->setMassVariance(covM);
  ATH_MSG_DEBUG("Inv mass = "<<invMass<<"  +/- "<<covM );
  pV->setStatus(2);
  //  delete[] Hk;

  if(m_timers) m_timer[1]->stop();

  return StatusCode::SUCCESS;
}

StatusCode TrigVertexingTool::createMotherParticle(TrigL2Vertex* pV)
{
  const double C=0.029997;
  const double B=20.84;

  if(!pV->isVertexFitted())
    {
      ATH_MSG_WARNING("Cannot form mother particle: vertex is not fitted - run fit first" );
      return StatusCode::FAILURE;
    }

  int Charge;
  double alpha=C*B*1e-3;
  double P[3];
  int i;

  double mPer[5];

  double* Rk = pV->getParametersVector();

  int trackId=0;
  int offset=0;
  for(i=0;i<3;i++) P[i]=0.0;
    
  Charge=0;
  for(std::list<TrigVertexFitInputTrack*>::iterator it=pV->getTracks()->begin();
      it!=pV->getTracks()->end();++it)
    {
      offset+=3;trackId++;      
      Charge+=(Rk[offset+2]<0.0)?-1:1;
      double pT=fabs(Rk[offset+2]);
      double psi=-asin(alpha*(Rk[0]*cos(Rk[offset])+Rk[1]*sin(Rk[offset]))/Rk[offset+2]);
      double phiV=Rk[offset]+psi;
      P[0]+=pT*cos(phiV);
      P[1]+=pT*sin(phiV);
      P[2]+=pT*cos(Rk[offset+1])/sin(Rk[offset+1]);
    }
  ATH_MSG_DEBUG("Mother particle charge = "<<Charge );
  double Ptot=sqrt(P[0]*P[0]+P[1]*P[1]+P[2]*P[2]);
  const double PTM=sqrt(P[0]*P[0]+P[1]*P[1]);
  const double inv_PTM = 1. / PTM;
  double ThetaM=acos(P[2]/Ptot);
  
  double PhiMV=atan2(P[1],P[0]);
  double sM=Rk[0]*cos(PhiMV)+Rk[1]*sin(PhiMV);
  double PsiM=-alpha*Charge*sM*inv_PTM;
  double PhiM0=PhiMV-PsiM;
  double D0=-0.5*alpha*Charge*(Rk[1]*Rk[1]+Rk[0]*Rk[0])*inv_PTM+
    Rk[1]*cos(PhiMV)-Rk[0]*sin(PhiMV);

  mPer[0]=D0;
  mPer[1]=PhiM0;
  mPer[2]=Rk[2]-sM*cos(ThetaM)/sin(ThetaM);
  mPer[3]=-log(sin(0.5*ThetaM)/cos(0.5*ThetaM));
  mPer[4]=PTM*1000.0;

  TrigInDetTrackFitPar* pMPar=new TrigInDetTrackFitPar(mPer[0],mPer[1],mPer[2],
						       mPer[3],mPer[4],
						       0.05,0.001,0.1,0.0001,100.0);
  pV->setMotherTrack(pMPar);

  pV->setStatus(3);
  return StatusCode::SUCCESS;
}


/*
StatusCode removeTrack(const Trk::Track*){}
StatusCode addMassToTrack(const Trk::Track*,double, TrigL2Vertex* ){}
StatusCode addMassToTrack(const TrigInDetTrack*,double, TrigL2Vertex* ){}

TrigL2Vertex* createVertex(std::list<const TrigInDetTrack*>&){}
TrigL2Vertex* createVertex(std::list<const Trk::Track*>&){}
TrigL2Vertex* createVertex(std::list<std::pair<const TrigInDetTrack*, 
			   const Trk::ParticleHypothesis> >&){}
TrigL2Vertex* createVertex(std::list<std::pair<const Trk::Track*, 
			   const Trk::ParticleHypothesis> >&){}
TrigL2Vertex* createVertex(std::list<std::pair<const TrigInDetTrack*, 
			   double> >&){}
TrigL2Vertex* createVertex(std::list<std::pair<const Trk::Track*, 
			   double> >&){}

TrigVertex* generateTrigVertex(const TrigL2Vertex*){}
*/

