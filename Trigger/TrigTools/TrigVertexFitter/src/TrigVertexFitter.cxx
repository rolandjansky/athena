/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigVertexFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 01.02.2006 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <iostream>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigVertexFitter.h"


TrigVertexFitter::TrigVertexFitter(const std::string& t, 
				   const std::string& n,
				   const IInterface*  p ): AthAlgTool(t,n,p)
{
  declareInterface< ITrigVertexFitter >( this );
  declareProperty( "numberOfIterations", m_numIter=5);  
  m_Rk=NULL;
  m_Gk=NULL;
  m_dChi2=NULL;
  m_vpTracks.clear();
}

StatusCode TrigVertexFitter::initialize()
{
  ATH_CHECK( AthAlgTool::initialize() );

  ATH_MSG_INFO( "Number of iterations is set to " << m_numIter  );
  ServiceHandle<ITrigTimerSvc> timerSvc ("TrigTimerSvc", name());
  if( timerSvc.retrieve().isFailure() )
    {
      ATH_MSG_INFO( "Unable to locate Service TrigTimerSvc "  );
      m_timers = false;
    } 
  else m_timers = true;  
  if(m_timers) 
    {
      m_timer[0] = timerSvc->addItem("VertexFit");
    }
  ATH_MSG_INFO( "TrigVertexFitter constructed " );
  return StatusCode::SUCCESS;
}

StatusCode TrigVertexFitter::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

TrigVertexFitter::~TrigVertexFitter()
{
  if(m_Rk!=NULL)
    {
      delete[] m_Rk;m_Rk=NULL;
    }
  if(m_Gk!=NULL)
    {
      delete[] m_Gk;m_Gk=NULL;
    }
  if(m_dChi2!=NULL)
    {
      delete[] m_dChi2;m_dChi2=NULL;
    }
}

TrigVertex* TrigVertexFitter::fit(TrackInVertexList* pTL)
{
  const double maxDChi2=1000.0;
  const double C=0.02997;
  const double B=20.84;

  TrigVertex* pV=NULL;
  double chi2=0.0,detr;
  double Ek[2][2],h[2],F[6],Hk[2][6],Sk[2][2],
    zeta[2],resid[2],dchi2;
  const double alpha=C*B*1e-3;
  const double inv_alpha = 1. / alpha;

  bool fit_failure=false;

  if(m_timers) m_timer[0]->start();

  int i,j,idx;

  bool goodSet=checkTracks(pTL);

  if(!goodSet)
    {
      ATH_MSG_WARNING( "Bad track set - vertex cannot be fit" );
      return NULL;
    }
  for(TrackInVertexList::iterator ptIt=pTL->begin();ptIt!=pTL->end();++ptIt)
  {
    bdTrack* pT=biDiagonalization((*ptIt));
    if(pT!=NULL) m_vpTracks.push_back(pT);
  }

  int nSize=3+3*m_vpTracks.size();

  if(m_Rk!=NULL)
    {
      delete[] m_Rk;m_Rk=NULL;
    }
  if(m_Gk!=NULL)
    {
      delete[] m_Gk;m_Gk=NULL;
    }
  if(m_dChi2!=NULL)
    {
      delete[] m_dChi2;m_dChi2=NULL;
    }
  
  m_Rk=new double[nSize];
  memset(m_Rk,0,sizeof(double)*nSize);
  m_Gk=new double[nSize*(nSize+1)/2];
  memset(m_Gk,0,sizeof(double)*nSize*(nSize+1)/2);
  m_dChi2=new double[pTL->size()];

  ATH_MSG_DEBUG ("Number of iterations set to " << m_numIter <<
                 " size of vector state " << nSize );
  m_Rk[1]=(*pTL->begin())->param()->phi0();
  m_Rk[2]=(*pTL->begin())->param()->z0();

  if(!initFit(pTL)) return pV;

  ATH_MSG_DEBUG ("Initialization: xV=" << m_Rk[0] << 
                 " yV=" << m_Rk[1] <<
                 " zV=" << m_Rk[2] );

  matrixRow* Kk=new matrixRow[nSize];
  matrixRow* Ak=new matrixRow[nSize];

  for(int iter=0;iter<m_numIter;iter++)
    {
      int trackId=0;
      int offset=0;
      
      memset(m_Gk,0,sizeof(double)*nSize*(nSize+1)/2);
      idx=0;j=nSize;
      m_Gk[0]=100.0;
      m_Gk[nSize]=100.0;
      m_Gk[2*nSize-1]=400.0;
      chi2=0.0;
      for(i=0;i<nSize;i++)
	for(j=0;j<2;j++) 
	  {
	    Ak[i][j]=0.0;Kk[i][j]=0.0;
	  }
      int matrixOffset=3*nSize-3;
      int matrixShift=nSize-3;

      for(std::vector<bdTrack*>::iterator ptIt=m_vpTracks.begin();ptIt!=m_vpTracks.end();++ptIt)
	  {
	    offset+=3;trackId++;
	    int currOffset=offset+3;

            ATH_MSG_DEBUG ("Iteration " << iter << " TrackID=" << trackId);
	    
	    m_Gk[matrixOffset]=(*ptIt)->paramCov(0,0);
	    m_Gk[matrixOffset+1]=(*ptIt)->paramCov(0,1);
	    m_Gk[matrixOffset+2]=(*ptIt)->paramCov(0,2);
	    matrixOffset+=matrixShift;
	    matrixShift--;
	    m_Gk[matrixOffset]=(*ptIt)->paramCov(1,1);
	    m_Gk[matrixOffset+1]=(*ptIt)->paramCov(1,2);
	    matrixOffset+=matrixShift;
	    matrixShift--;
	    m_Gk[matrixOffset]=(*ptIt)->paramCov(2,2);
	    matrixOffset+=matrixShift;
	    matrixShift--;

	    if(iter==0) 
	      {
		for(i=0;i<3;i++)
		  m_Rk[offset+i]=(*ptIt)->params(i);		
	      }

	    memset(&F[0],0,sizeof(F));

	    double sinF=sin(m_Rk[offset]);
	    double cosF=cos(m_Rk[offset]);
	    const double aP=alpha/m_Rk[offset+2];
            const double inv_aP = 1. / aP;
	    double sinPsi=-aP*(m_Rk[0]*cosF+m_Rk[1]*sinF);

	    if(fabs(sinPsi)>1.0)
	      {
		fit_failure=true;
		break;
	      }

	    const double cosPsi=sqrt(1.0-sinPsi*sinPsi);
            const double inv_cosPsi = 1. / cosPsi;
	    double ctt=cos(m_Rk[offset+1])/sin(m_Rk[offset+1]);
	    double psi=asin(sinPsi);
	    double sinT=sin(m_Rk[offset+1]);
	    double aCP=ctt*inv_aP;
	    double sinAP=sinPsi*inv_aP;
	    
	    F[0]=-aP*cosF;F[1]=-aP*sinF;F[3]=-aP*(m_Rk[1]*cosF-m_Rk[0]*sinF);
	    F[5]=-sinPsi/m_Rk[offset+2];
	    for(i=0;i<5;i++) F[i]*=inv_cosPsi;
		  
	    Hk[0][0]=-sinF+F[0]*sinAP;
	    Hk[0][1]= cosF+F[1]*sinAP;
	    Hk[0][2]=0.0;
	    Hk[0][3]=(*ptIt)->lambda(0)-m_Rk[1]*sinF-m_Rk[0]*cosF+F[3]*inv_aP;
	    Hk[0][4]=(*ptIt)->lambda(1);
	    Hk[0][5]=(*ptIt)->lambda(2)+(1.0-cosPsi)*inv_alpha+F[5]*sinPsi*inv_aP;
	    
	    Hk[1][0]=F[0]*aCP;Hk[1][1]=F[1]*aCP;Hk[1][2]=1.0;
	    Hk[1][3]=(*ptIt)->lambda(3)+F[3]*aCP;
	    Hk[1][4]=(*ptIt)->lambda(4)-psi/(aP*sinT*sinT);
	    Hk[1][5]=(*ptIt)->lambda(5)+psi*ctt*inv_alpha+F[5]*aCP;

	    double h00g0=Hk[0][0]*m_Gk[0];
	    double h01g1=Hk[0][1]*m_Gk[1];
	    double h11g1=Hk[1][1]*m_Gk[1];
	    double h11gN=Hk[1][1]*m_Gk[nSize];
	    double h11gN1=Hk[1][1]*m_Gk[nSize+1];
	    double h00g2=Hk[0][0]*m_Gk[2];
	    double h01gN=Hk[0][1]*m_Gk[nSize];
	    double h01gN1=Hk[0][1]*m_Gk[nSize+1];
	    double h03p00=Hk[0][3]*(*ptIt)->paramCov(0,0);
	    double h04p01=Hk[0][4]*(*ptIt)->paramCov(0,1);
	    double h04p11=Hk[0][4]*(*ptIt)->paramCov(1,1);
	    double h04p12=Hk[0][4]*(*ptIt)->paramCov(1,2);
	    double h05p02=Hk[0][5]*(*ptIt)->paramCov(0,2);
	    double h05p12=Hk[0][5]*(*ptIt)->paramCov(1,2);
	    double h05p22=Hk[0][5]*(*ptIt)->paramCov(2,2);
	    double h03p02=Hk[0][3]*(*ptIt)->paramCov(0,2);
	    double h10g0=Hk[1][0]*m_Gk[0];
	    double h10g2=Hk[1][0]*m_Gk[2];
	    double h13p00=Hk[1][3]*(*ptIt)->paramCov(0,0);
	    double h13p02=Hk[1][3]*(*ptIt)->paramCov(0,2);
	    double h14p01=Hk[1][4]*(*ptIt)->paramCov(0,1);
	    double h14p11=Hk[1][4]*(*ptIt)->paramCov(1,1);
	    double h15p12=Hk[1][5]*(*ptIt)->paramCov(1,2);
	    double h15p22=Hk[1][5]*(*ptIt)->paramCov(2,2);

	    Ek[0][0]=Hk[0][0]*(h00g0+2.0*h01g1)+Hk[0][1]*h01gN+Hk[0][3]*(h03p00+2.0*h04p01)+
	      Hk[0][4]*(h04p11+2.0*h05p12)+Hk[0][5]*(h05p22+2.0*h03p02);
	    
	    Ek[0][1]=(h00g0+h01g1)*Hk[1][0]+Hk[0][0]*h11g1+
	      h01gN*Hk[1][1]+h00g2+h01gN1+(h03p00+h04p01+h05p02)*Hk[1][3]+
	      Hk[0][3]*h14p01+(h03p02+h04p12+h05p22)*Hk[1][5]+
	      (h05p12+h04p11)*Hk[1][4];

	    Ek[1][1]=Hk[1][0]*(h10g0+2.0*h11g1)+2.0*h10g2+Hk[1][1]*h11gN+2.0*h11gN1+
	      m_Gk[2*nSize-1]+Hk[1][3]*(h13p00+2.0*h14p01)+
	      Hk[1][4]*(h14p11+2.0*h15p12)+Hk[1][5]*(h15p22+2.0*h13p02);
	    
	    Ek[0][0]+=(*ptIt)->measCov(0,0);
	    Ek[0][1]+=(*ptIt)->measCov(0,1);
	    Ek[1][1]+=(*ptIt)->measCov(1,1);
	    
	    detr=Ek[0][0]*Ek[1][1]-Ek[0][1]*Ek[0][1];
	    detr=1.0/detr;
	    Sk[0][0]=Ek[1][1]*detr;
	    Sk[1][1]=Ek[0][0]*detr;
	    Sk[0][1]=-Ek[0][1]*detr;Sk[1][0]=Sk[0][1];

	    Ak[0][0]=h00g0+h01g1;
	    Ak[0][1]=h10g0+h11g1+m_Gk[2];

	    Ak[1][0]=Hk[0][0]*m_Gk[1]+h01gN;
	    Ak[1][1]=Hk[1][0]*m_Gk[1]+h11gN+m_Gk[nSize+1];
	    
	    Ak[2][0]=h00g2+h01gN1;
	    Ak[2][1]=h10g2+h11gN1+m_Gk[2*nSize-1];

	    for(i=0;i<offset-3;i++)
	      {
		Ak[3+i][0]=Hk[0][0]*m_Gk[3+i]+Hk[0][1]*m_Gk[nSize+2+i];
		Ak[3+i][1]=Hk[1][0]*m_Gk[3+i]+m_Gk[2*nSize+i]+Hk[1][1]*m_Gk[nSize+2+i];
	      }
	    Ak[offset][0]=h03p00+h04p01+h05p02;
	    Ak[offset][1]=h13p00+h14p01+Hk[1][5]*(*ptIt)->paramCov(0,2);

	    Ak[offset+1][0]=Hk[0][3]*(*ptIt)->paramCov(0,1)+h04p11+h05p12;
	    Ak[offset+1][1]=Hk[1][3]*(*ptIt)->paramCov(0,1)+h14p11+h15p12;

	    Ak[offset+2][0]=h03p02+h04p12+h05p22;
	    Ak[offset+2][1]=h13p02+Hk[1][4]*(*ptIt)->paramCov(1,2)+h15p22;
	    for(i=0;i<currOffset;i++)
	      {
		Kk[i][0]=Ak[i][0]*Sk[0][0]+Ak[i][1]*Sk[1][0];
		Kk[i][1]=Ak[i][0]*Sk[0][1]+Ak[i][1]*Sk[1][1];
	      }
	    for(i=0;i<3;i++) m_Rk[offset+i]=(*ptIt)->params(i);

	    h[0]=(1.0-cosPsi)*inv_aP+m_Rk[1]*cosF-m_Rk[0]*sinF;
	    h[1]=m_Rk[2]+ctt*psi*inv_aP;

            ATH_MSG_DEBUG ("Expected measurements: m1=" << h[0] <<
                           " m2=" << h[1]);
	    zeta[0]=(*ptIt)->measurements(0);
	    zeta[1]=(*ptIt)->measurements(1);
            ATH_MSG_DEBUG( "Actual measurements: z1=" << zeta[0] <<
                           "z2=" << zeta[1] );

	    for(i=0;i<2;i++) resid[i]=zeta[i]-h[i];
	    dchi2=resid[0]*resid[0]*Sk[0][0]+2.0*resid[0]*resid[1]*Sk[0][1]+
	      resid[1]*resid[1]*Sk[1][1];

            ATH_MSG_DEBUG( "Resids: " << resid[0] << " " << resid[1] );
            ATH_MSG_DEBUG( "Inv cov " << Sk[0][0] << " " << Sk[0][1] <<
                           " " << Sk[1][0] << " " << Sk[1][1] );

            ATH_MSG_DEBUG( "dchi2 contrib.=" << dchi2 );
	    chi2+=dchi2;
	    m_dChi2[trackId-1]=dchi2;

	    if(dchi2>maxDChi2)
	      {
		fit_failure=true;
		break;
	      }

	    for(i=0;i<currOffset;i++) m_Rk[i]+=Kk[i][0]*resid[0]+Kk[i][1]*resid[1];
	      
	    idx=0;
	    for(i=0;i<currOffset;i++)
	      {
		for(j=i;j<currOffset;j++) 
		  {
		    m_Gk[idx]-=(Kk[i][0]*Ak[j][0]+Kk[i][1]*Ak[j][1]);
		    idx++;
		  }
		idx+=nSize-currOffset;
	      }
	    if (msgLvl(MSG::VERBOSE))
	      {
		showVec(nSize);
		showCov(nSize);
	      }
	  }
      if (msgLvl(MSG::DEBUG))
	{
	  showVec(nSize);
          ATH_MSG_DEBUG( "Total chi2=" << chi2 <<
                         " Ndof=" << (int)(pTL->size()*2-3) );
	}
    }

  for(std::vector<bdTrack*>::iterator btIt=m_vpTracks.begin();
      btIt!=m_vpTracks.end();++btIt)
    {
      delete (*btIt);
    }
  m_vpTracks.clear();
  delete[] Kk;
  delete[] Ak;

  if(m_timers) m_timer[0]->stop();
  
  if(fit_failure) return NULL;

  int ndof=pTL->size()*2-3;
  double CV[6];

  CV[0]=m_Gk[0];
  CV[1]=m_Gk[1];
  CV[2]=m_Gk[nSize];
  CV[3]=m_Gk[2];
  CV[4]=m_Gk[nSize+1];
  CV[5]=m_Gk[2*nSize-1];

  pV = new TrigVertex(m_Rk[0],m_Rk[1],m_Rk[2],CV,chi2,ndof,pTL);
  
  return pV;
}


TrigVertex* TrigVertexFitter::fit(TrigVertex* pV)
{
  return fit(pV->tracks());
}

void TrigVertexFitter::showCov(int nSize)
{
  int idx,i,j,k;

  idx=0;j=0;
  printf("--- covariance ---\n");
  for(i=0;i<nSize;i++)
    {
      for(k=0;k<j;k++) printf("             ");
      for(k=j;k<nSize;k++)
	{
	  printf("%E ",m_Gk[idx++]);
	}
      printf("\n");
      j++;
    }
  printf("--------------------\n");
}

void TrigVertexFitter::showVec(int nSize)
{
  int i=0,idx=0;
  printf("--- state vector ---\n");
  while(idx<nSize)
    {
      printf("%E ",m_Rk[idx]);
      i++;
      if(i==3)
	{
	  i=0;printf("\n");
	}
      idx++;
    }
  printf("---------------------\n");
}

bool TrigVertexFitter::checkTracks(TrackInVertexList* pTL)
{
  bool rc=true;
  
  if(pTL->size()<2)
    {
      ATH_MSG_WARNING( "Track set contains less than 2 tracks"  );
      return false;
    }

  for(TrackInVertexList::iterator ptIt=pTL->begin();ptIt!=pTL->end();++ptIt)
    {
      const TrigInDetTrackFitPar* p=(*ptIt)->param();
      if(p==NULL) 
	{
          ATH_MSG_WARNING( "TrigInDetTrack has no parameters"  );
	  rc=false;break;
	}
      if(p->cov()==NULL)
	{
          ATH_MSG_WARNING( "TrigInDetTrack covariance is not set"  );
	  rc=false;break;
	}
    }
  return rc;
}

bool TrigVertexFitter::initFit(TrackInVertexList* pTL)
{
  const double C=0.029997;
  const double B=20.84;

  if(pTL->size()<2) return false;

  int i,j;
  double d0[2],z0[2],ctt[2],xc[2],yc[2],R[2],theta,
    a,h,x[2],y[2],psi[2][2],rv[2],
    dz[2],delta,r[2],xa,ya;
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  
  const double alpha = C*B;
  const double inv_alpha = 1. / alpha;
  
  TrackInVertexList::iterator ptIt=pTL->begin();
  for(int nTracks=0;nTracks<2;nTracks++) {

    const TrigInDetTrackFitPar* p=(*ptIt)->param();
    d0[nTracks]=p->a0();
    z0[nTracks]=p->z0();
    theta=2.0*atan(exp(-p->eta()));
    ctt[nTracks]=cos(theta)/sin(theta);
    R[nTracks]=-p->pT()*inv_alpha;
    r[nTracks]=fabs(R[nTracks]);
    xc[nTracks]=-(d0[nTracks]+R[nTracks])*sin(p->phi0());
    yc[nTracks]=(d0[nTracks]+R[nTracks])*cos(p->phi0());
    ATH_MSG_DEBUG( "xc=" << xc[nTracks] <<
                   " yc=" << yc[nTracks] <<
                   " d0=" << d0[nTracks] <<
                   " R=" << R[nTracks] <<
                   " z0=" << z0[nTracks] );
    ++ptIt;
  }

  double dxc=xc[1]-xc[0];
  double dyc=yc[1]-yc[0];
  dz[0]=z0[1]-z0[0];dz[1]=dz[0];
  const double d=sqrt(dxc*dxc+dyc*dyc);
  const double inv_d = 1. /d;

  int nCase=0;
  if(d>r[0]+r[1]) nCase=1;
  if(d<fabs(r[0]-r[1])) nCase=2;

  switch(nCase)
    {
    case 0:
      {
        ATH_MSG_DEBUG( "circles intersect" );
	a=0.5*(r[0]*r[0]-r[1]*r[1]+d*d)*inv_d;
	xa=xc[0]+dxc*a*inv_d;
	ya=yc[0]+dyc*a*inv_d;
	h=sqrt(r[0]*r[0]-a*a);
	x[0]=xa-dyc*h*inv_d;
	y[0]=ya+dxc*h*inv_d;
	x[1]=xa+dyc*h*inv_d;
	y[1]=ya-dxc*h*inv_d;
	for(i=0;i<2;i++)
	  {
	    rv[i]=sqrt(x[i]*x[i]+y[i]*y[i]);
	    for(j=0;j<2;j++)
	      {
		double cosa=0.5*(R[j]/(R[j]+d0[j])+(R[j]+d0[j])/R[j]-
				 rv[i]*rv[i]/(R[j]*(R[j]+d0[j])));
                ATH_MSG_DEBUG( "cosPsi=" << cosa );
		psi[i][j]=acos(cosa);
	      }
	    dz[i]+=R[1]*psi[i][1]*ctt[1]-R[0]*psi[i][0]*ctt[0];
	  }
        ATH_MSG_DEBUG( "xv1=" << x[0] <<
                       " yv1=" << y[0] <<
                       " xv2=" << x[1] <<
                       " yv2=" << y[1] );
        ATH_MSG_DEBUG( "psi11=" << psi[0][0] <<
                       " psi12=" << psi[0][1] <<
                       " psi21=" << psi[1][0] <<
                       " psi22=" << psi[1][1] );
        ATH_MSG_DEBUG( "dz1=" << dz[0] << " dz2=" << dz[1] );
	i=(fabs(dz[0])<fabs(dz[1]))?0:1;
	m_Rk[0]=x[i];m_Rk[1]=y[i];
	m_Rk[2]=0.5*(z0[0]+R[0]*psi[i][0]*ctt[0]+z0[1]+R[1]*psi[i][1]*ctt[1]);
	break;
      }
    case 1:
      {
        ATH_MSG_DEBUG( "circles are too far" );
	delta=0.5*(d-r[0]-r[1]);
	x[0]=xc[0]+dxc*(r[0]+delta)*inv_d;
	y[0]=yc[0]+dyc*(r[0]+delta)*inv_d;
	rv[0]=sqrt(x[0]*x[0]+y[0]*y[0]);
	for(j=0;j<2;j++)
	  {
	    double cosa=0.5*(fabs(fabs(R[j])+delta)/fabs(R[j]+d0[j])+
			     fabs(R[j]+d0[j])/fabs(fabs(R[j])+delta)-
			     rv[0]*rv[0]/(fabs(fabs(R[j])+delta)*fabs(R[j]+d0[j])));
            ATH_MSG_DEBUG( "cosPsi=" << cosa );
	    psi[0][j]=acos(cosa);
	  }
	dz[0]+=R[1]*psi[0][1]*ctt[1]-R[0]*psi[0][0]*ctt[0];
        ATH_MSG_DEBUG( "xv1=" << x[0] << " yv1=" << y[0] );
        ATH_MSG_DEBUG( "psi11=" << psi[0][0] << " psi12=" << psi[0][1] );
        ATH_MSG_DEBUG( "dz1=" << dz[0] );
	m_Rk[0]=x[0];m_Rk[1]=y[0];
	m_Rk[2]=0.5*(z0[0]+R[0]*psi[0][0]*ctt[0]+z0[1]+R[1]*psi[0][1]*ctt[1]);
	break;
      }
    case 2:
      {
        ATH_MSG_DEBUG( "circle is inside the other" );

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
            ATH_MSG_DEBUG( "sinPsi=" << sina );
	    psi[0][j]=asin(sina);
	  }
	dz[0]+=R[1]*psi[0][1]*ctt[1]-R[0]*psi[0][0]*ctt[0];
	x[0]=0.5*(x[smallJ]+x[bigJ]);y[0]=0.5*(y[smallJ]+y[bigJ]);
        rv[0]=sqrt(x[0]*x[0]+y[0]*y[0]);
	if (outputLevel <= MSG::DEBUG) 
	  {
            ATH_MSG_DEBUG( "xv1=" << x[0] << " yv1=" << y[0] );
            ATH_MSG_DEBUG( "psi11="<< psi[0][0] << " psi12=" << psi[0][1] );
            ATH_MSG_DEBUG( "dz1=" << dz[0] );
	  }
	m_Rk[0]=x[0];m_Rk[1]=y[0];
	m_Rk[2]=0.5*(z0[0]+R[0]*psi[0][0]*ctt[0]+z0[1]+R[1]*psi[0][1]*ctt[1]);
	break;
      }
    } 
  return true;
}

TrigVertex* TrigVertexFitter::fit(TrackInVertexList* pTL, std::vector<TrigVtx::TrigParticleName> vPH)
{
  //  if(m_timers) m_timer[0]->start();
  TrigVertex* pV=fit(pTL);
  if(pV!=NULL)
    calculateInvariantMass(pV,vPH); 
  // if(m_timers) m_timer[0]->stop();
  return pV;
}

void TrigVertexFitter::calculateInvariantMass(TrigVertex* pV, std::vector<TrigVtx::TrigParticleName>& vPH)
{
  const double C=0.029997;
  const double B=20.84;

  int nSize=pV->tracks()->size();
  double invMass=0.0,alpha=C*B*1e-3;
  double P[3];
  double E=0.0;
  int i,j,idx;

  TrigVtx::TrigParticleMasses massVec;

  if(pV->tracks()->size()!=vPH.size())
    {
      ATH_MSG_WARNING( "Inconsistency between sizes of track list and particle hypotheses !" );
      ATH_MSG_WARNING( "Skipping invariant mass calculation" );
      pV->setMass(invMass);
      pV->setMassVariance(1e8);
      return;
    }

  if(pV->tracks()->size()==0)
    {
      ATH_MSG_WARNING( "Vertex with 0 tracks !" );
      ATH_MSG_WARNING( "Skipping invariant mass calculation" );
      pV->setMass(invMass);
      pV->setMassVariance(1e8);
      return;
    }

  double* Hk=new double[3*nSize+3];
  memset(&Hk[0],0,sizeof(double)*(3*nSize+3));

  int trackId=0;
  int offset=0;
  for(i=0;i<3;i++) P[i]=0.0;
  for(std::vector<TrigVtx::TrigParticleName>::iterator phIt=vPH.begin();phIt!=vPH.end();++phIt)
    {
      offset+=3;trackId++;
      
      double mass=massVec.mass[(*phIt)]*1e-3;
      double pT=fabs(m_Rk[offset+2]);
      double p=pT/sin(m_Rk[offset+1]);

      double psi=-asin(alpha*(m_Rk[0]*cos(m_Rk[offset])+m_Rk[1]*sin(m_Rk[offset]))/pT);
      double phiV=m_Rk[offset]+psi;
      ATH_MSG_DEBUG( "Track " << trackId << " psi=" << psi <<
                     " phi0=" << m_Rk[offset] << "  phiV=" << phiV <<
                     " theta=" << m_Rk[offset+1] <<
                     " pT=" << m_Rk[offset+2] <<
                     " mass=" << massVec.mass[(*phIt)] );
      P[0]+=pT*cos(phiV);
      P[1]+=pT*sin(phiV);
      P[2]+=pT*cos(m_Rk[offset+1])/sin(m_Rk[offset+1]);
      E+=sqrt(mass*mass+p*p);
    }
  invMass=sqrt(E*E-P[0]*P[0]-P[1]*P[1]-P[2]*P[2])*1000.0;
  pV->setMass(invMass);

  offset=0;trackId=0;
  for(std::vector<TrigVtx::TrigParticleName>::iterator phIt=vPH.begin();phIt!=vPH.end();++phIt)
    {
      offset+=3;trackId++;

      double mass=massVec.mass[(*phIt)]*1e-3;
      double Ck=(m_Rk[offset+2]<0.0)?-1.0:1.0;
      const double sinT=sin(m_Rk[offset+1]);
      const double inv_sinT = 1. / sinT;
      double cosT=cos(m_Rk[offset+1]);
      double pT=fabs(m_Rk[offset+2]);
      double p=pT*inv_sinT;
      double e=sqrt(p*p+mass*mass);
      double sinF=sin(m_Rk[offset]);
      double cosF=cos(m_Rk[offset]);

      double sinPsi=-alpha*(m_Rk[0]*cosF+m_Rk[1]*sinF)/m_Rk[offset+2];
      double psi=asin(sinPsi);
      const double cosPsi=sqrt(1.0-sinPsi*sinPsi);
      const double inv_cosPsi = 1. / cosPsi;
      double phiV=m_Rk[offset]+psi;
      
      double aCos=alpha*Ck*inv_cosPsi;
      double dP=P[1]*cos(phiV)-P[0]*sin(phiV);
      double eE=E*m_Rk[offset+2]/(e*sinT);

      Hk[0]+=dP*cosF*aCos;
      Hk[1]+=dP*sinF*aCos;
      Hk[offset]=-dP*Ck*(m_Rk[offset+2]-Ck*aCos*(m_Rk[1]*cosF-m_Rk[0]*sinF));
      Hk[offset+1]=(m_Rk[offset+2]*inv_sinT*inv_sinT)*(P[2]*Ck-eE*cosT);
      Hk[offset+2]=eE*inv_sinT-Ck*(P[0]*cos(phiV)+P[1]*sin(phiV)+P[2]*cosT*inv_sinT)+dP*Ck*sinPsi*inv_cosPsi;

      ATH_MSG_INFO( "offset" << offset  );
    }
  
  double covM=0.0;
  idx=0;
  nSize=3+3*nSize;
  for(i=0;i<nSize;i++)
    {
      for(j=i;j<nSize;j++) 
	{
	  double dCov=m_Gk[idx]*Hk[i]*Hk[j];
	  if(i!=j) dCov*=2.0;
	  covM+=dCov;
	  idx++;
	}
    }
  covM=sqrt(covM)*1e6/invMass;
  pV->setMassVariance(covM);
  ATH_MSG_DEBUG( "Inv mass = " << invMass << " +/- " << covM );
  delete[] Hk;
}


bdTrack* TrigVertexFitter::biDiagonalization(const TrigInDetTrack* pT)
{
  double Ck[5][5];
  double A[3][3],B[2][3];
  double L[6],detr,det[3];
  double C[2][2],eta[2],par[3];
  int i,j;
	
  const TrigInDetTrackFitPar* p=pT->param();
  double a=-2.0*exp(-p->eta())/(1.0+exp(-2.0*p->eta()));

  par[0]=p->phi0();
  par[1]=2.0*atan(exp(-p->eta()));
  par[2]=p->pT()*1e-3;

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
      Ck[4][i]=Ck[4][i]*1e-3;  Ck[i][4]=Ck[4][i];
    }
  Ck[4][4]*=1e-3;
  for(i=0;i<3;i++) 
    {
      for(j=0;j<3;j++) A[i][j]=Ck[i+2][j+2];
      B[0][i]=-Ck[0][i+2];B[1][i]=-Ck[1][i+2];
    }
  double prodA=A[1][1]*A[2][2]-A[1][2]*A[2][1];
  double prodB=A[1][0]*A[2][2]-A[1][2]*A[2][0];
  double prodC=A[1][0]*A[2][1]-A[1][1]*A[2][0];

  double coeff[6];

  coeff[0]=A[0][2]*A[2][1]-A[0][1]*A[2][2];
  coeff[1]=A[0][1]*A[1][2]-A[1][1]*A[0][2];
  coeff[2]=A[0][0]*A[2][2]-A[0][2]*A[2][0];
  coeff[3]=A[0][2]*A[1][0]-A[1][2]*A[0][0];
  coeff[4]=A[0][1]*A[2][0]-A[0][0]*A[2][1];
  coeff[5]=A[0][0]*A[1][1]-A[0][1]*A[1][0];
  detr=A[0][0]*prodA-A[0][1]*prodB+A[0][2]*prodC;
  detr=1.0/detr;

  for(i=0;i<2;i++)
    {
      det[0]=B[i][0]*prodA+B[i][1]*coeff[0]+B[i][2]*coeff[1];
      det[1]=-B[i][0]*prodB+B[i][1]*coeff[2]+B[i][2]*coeff[3];
      det[2]=B[i][0]*prodC+B[i][1]*coeff[4]+B[i][2]*coeff[5];
      for(j=0;j<3;j++) L[i*3+j]=det[j]*detr;
    }
  
  C[0][0]=Ck[0][0]+Ck[0][2]*L[0]+Ck[0][3]*L[1]+Ck[0][4]*L[2];
  C[0][1]=Ck[0][1]+Ck[1][2]*L[0]+Ck[1][3]*L[1]+Ck[1][4]*L[2];
  C[1][1]=Ck[1][1]+Ck[1][2]*L[3]+Ck[1][3]*L[4]+Ck[1][4]*L[5];
  C[1][0]=C[0][1];
  
  eta[0]=p->a0();eta[1]=p->z0();
  return new bdTrack(pT,L,C,A,eta,par);
}
