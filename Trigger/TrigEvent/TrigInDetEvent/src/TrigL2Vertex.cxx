/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstring>
#include "TrigInDetEvent/TrigL2Vertex.h"
#include "TrkParameters/TrackParameters.h"

TrigVertexFitInputTrack::TrigVertexFitInputTrack(const TrigInDetTrack* pT, double mass=0.0) : m_mass(mass)
{
  m_nTrackType=1;m_active=true;
  m_pTrigTrack=pT;m_pTrkTrack=NULL;
  double Ck[5][5];
  int i{0},j{0};

  const TrigInDetTrackFitPar* p=pT->param();

  double a=-2.0*exp(-p->eta())/(1.0+exp(-2.0*p->eta()));
  m_u[0]=p->a0();
  m_u[1]=p->z0();
  m_q[0]=p->phi0();
  m_q[1]=2.0*atan(exp(-p->eta()));
  m_q[2]=p->pT()/1000.0;      


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
      Ck[4][i]=Ck[4][i]/1000.0;Ck[i][4]=Ck[4][i];
    }
  Ck[4][4]/=1000.0; 


  for(i=0;i<2;i++) for(j=0;j<2;j++) m_Vuu[i][j]=Ck[i][j];
  for(i=0;i<2;i++) for(j=0;j<3;j++) m_Vuq[i][j]=Ck[i][j+2];
  for(i=0;i<3;i++) for(j=0;j<3;j++) m_Vqq[i][j]=Ck[i+2][j+2];

  /*
  m_Vuu[0][0]=(*p->cov())[0];
  m_Vuu[0][1]=m_Vuu[1][0]=(*p->cov())[2];
  m_Vuu[1][1]=(*p->cov())[9];

  m_Vuq[0][0]=(*p->cov())[1];m_Vuq[0][1]=(*p->cov())[3]*a;
  m_Vuq[0][2]=(*p->cov())[4]*1e-3;m_Vuq[1][0]=(*p->cov())[6];m_Vuq[1][1]=(*p->cov())[10]*a;
  m_Vuq[1][2]=(*p->cov())[11]*1e-3;

  m_Vqq[0][0]=(*p->cov())[5];m_Vqq[0][1]=m_Vqq[1][0]=(*p->cov())[7]*a;
  m_Vqq[0][2]=m_Vqq[2][0]=(*p->cov())[8]*1e-3;
  m_Vqq[1][1]=(*p->cov())[12]*a*a;m_Vqq[1][2]=m_Vqq[2][1]=(*p->cov())[13]*a*1e-3;
  m_Vqq[2][2]=(*p->cov())[14]*1e-6;
  */
  m_Perigee[0]=m_u[0];m_Perigee[1]=m_u[1];m_Perigee[2]=m_q[0];m_Perigee[3]=m_q[1];m_Perigee[4]=m_q[2];
}

TrigVertexFitInputTrack::TrigVertexFitInputTrack(const Trk::Track* pT,double mass=0.0) : m_mass(mass)
{
  double Ck[5][5];

  m_nTrackType=2;m_active=true;
  m_pTrkTrack=pT;m_pTrigTrack=NULL;

  const Trk::Perigee* pP= pT->perigeeParameters();

  m_u[0]=pP->parameters()[Trk::d0];
  m_u[1]=pP->parameters()[Trk::z0];
  m_q[0]=pP->parameters()[Trk::phi0];
  m_q[1]=pP->parameters()[Trk::theta];

  double ptC=1000.0*pP->parameters()[Trk::qOverP];

  m_q[2]=sin(pP->parameters()[Trk::theta])/ptC;

	if( pP->covariance() ){
		// has covariance matrix
		const AmgSymMatrix(5)* TC = pP->covariance();

		Ck[0][0]=(*TC)(Trk::d0,Trk::d0);
		Ck[0][1]=Ck[1][0]=(*TC)(Trk::d0,Trk::z0);
		Ck[0][2]=Ck[2][0]=(*TC)(Trk::d0,Trk::phi0);
		Ck[0][3]=Ck[3][0]=(*TC)(Trk::d0,Trk::theta);
		Ck[0][4]=Ck[4][0]=(*TC)(Trk::d0,Trk::qOverP);
		Ck[1][1]=(*TC)(Trk::z0,Trk::z0);
		Ck[1][2]=Ck[2][1]=(*TC)(Trk::z0,Trk::phi0);
		Ck[1][3]=Ck[3][1]=(*TC)(Trk::z0,Trk::theta);
		Ck[1][4]=Ck[4][1]=(*TC)(Trk::z0,Trk::qOverP);
		Ck[2][2]=(*TC)(Trk::phi0,Trk::phi0);
		Ck[2][3]=Ck[3][2]=(*TC)(Trk::phi0,Trk::theta);
		Ck[2][4]=Ck[4][2]=(*TC)(Trk::phi0,Trk::qOverP);
		Ck[3][3]=(*TC)(Trk::theta,Trk::theta);
		Ck[3][4]=Ck[4][3]=(*TC)(Trk::theta,Trk::qOverP);
		Ck[4][4]=(*TC)(Trk::qOverP,Trk::qOverP);

		const double a = cos(pP->parameters()[Trk::theta])/ptC;
		const double b = -sin(pP->parameters()[Trk::theta])/(pP->parameters()[Trk::qOverP]*ptC);

		Ck[3][3]=Ck[3][3]+2.0*a*Ck[3][4]+a*a*Ck[4][4];
		Ck[3][4]=Ck[4][3]=b*Ck[3][4]+a*b*Ck[4][4];
		Ck[4][4]=b*b*Ck[4][4];
		Ck[0][3]=Ck[3][0]=Ck[0][3]+a*Ck[0][4];Ck[0][4]*=b;Ck[4][0]=Ck[0][4];
		Ck[1][3]=Ck[3][1]=Ck[1][3]+a*Ck[1][4];Ck[1][4]*=b;Ck[4][1]=Ck[1][4];
		Ck[2][3]=Ck[3][2]=Ck[2][3]+a*Ck[2][4];Ck[2][4]*=b;Ck[4][2]=Ck[2][4];
		for(int i=0;i<2;i++) for(int j=0;j<2;j++) m_Vuu[i][j]=Ck[i][j];
		for(int i=0;i<3;i++) for(int j=0;j<3;j++) m_Vqq[i][j]=Ck[i+2][j+2];
		for(int i=0;i<2;i++) for(int j=0;j<3;j++) m_Vuq[i][j]=Ck[i][j+2];

		m_Perigee[0]=m_u[0];m_Perigee[1]=m_u[1];m_Perigee[2]=m_q[0];m_Perigee[3]=m_q[1];m_Perigee[4]=m_q[2];
	}
	else{
		m_active=false;
		// no covariance
	}

}

TrigVertexFitInputTrack::~TrigVertexFitInputTrack()
{

}

void TrigVertexFitInputTrack::setMass(double m)
{
  m_mass=m;
}

void TrigVertexFitInputTrack::mask()
{
  m_active=false;
}

void TrigVertexFitInputTrack::activate()
{
  m_active=true;
}

bool TrigVertexFitInputTrack::isActive()
{
  return m_active;
}

int TrigVertexFitInputTrack::getTrackType()
{
  return m_nTrackType;
}


const TrigInDetTrack* TrigVertexFitInputTrack::getTrigTrack()
{
  return m_pTrigTrack;
}

const Trk::Track* TrigVertexFitInputTrack::getTrkTrack()
{
	return m_pTrkTrack;
}

void TrigVertexFitInputTrack::setIndex(int i)
{
  m_index=i;
}
 
int TrigVertexFitInputTrack::getIndex() const
{
  return m_index;
}

const double* TrigVertexFitInputTrack::Perigee() const
{
  return &m_Perigee[0];
}

double TrigVertexFitInputTrack::PerigeeCovariance(int i, int j) const
{
  return m_PerigeeCovariance[i][j];
}

void TrigVertexFitInputTrack::initializeVertex(class TrigL2Vertex* pV)
{
  const int i = 3+m_index*3;
  for(int j=0;j<3;j++) 
    pV->getParametersVector()[i+j]=m_q[j];  
}

double TrigVertexFitInputTrack::getChi2Distance(class TrigL2Vertex* pV)
{
  const double C=0.02997;
  const double B=20.84;
  const double alpha=C*B/1000.0;
	
  double Sk[2][2],detr,chi2;
  double AC[2][3],BV[2][3];
  int i{0},j{0},k{0};

  const int shift=m_index*3;

  k=3+shift;
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      pV->m_Gk[i+k][j+k]=m_Vqq[i][j];	

  initializeVertex(pV);
  const double xv=pV->getParametersVector()[0];
  const double yv=pV->getParametersVector()[1];
  const double zv=pV->getParametersVector()[2];
  const double phi0=pV->getParametersVector()[3+shift];
  const double theta0=pV->getParametersVector()[4+shift];
  const double P0=pV->getParametersVector()[5+shift];

  const double cosPhi0=cos(phi0);
  const double sinPhi0=sin(phi0);
  const double sinPsi=-alpha*(xv*cosPhi0+yv*sinPhi0)/P0;
  if(fabs(sinPsi)>1.0) return -999.9;
  const double cosPsi=sqrt(1.0-sinPsi*sinPsi);
  const double psi=asin(sinPsi);
  const double sint=sin(theta0);
  const double ctt=cos(theta0)/sint;

  m_A[0][0]=-sin(phi0+psi)/cosPsi;
  m_A[0][1]= cos(phi0+psi)/cosPsi;
  m_A[0][2]=0.0;

  m_A[1][0]=-ctt*cosPhi0/cosPsi;
  m_A[1][1]=-ctt*sinPhi0/cosPsi;
  m_A[1][2]=1.0;

  m_B[0][0]=-xv*m_A[0][1]+yv*m_A[0][0];
  m_B[0][1]=0.0;
  m_B[0][2]=(1.0-1.0/cosPsi)/alpha;

  m_B[1][0]=-xv*m_A[1][1]+yv*m_A[1][0];
  m_B[1][1]=-P0*psi/(alpha*sint*sint);
  m_B[1][2]=ctt*(psi-sinPsi/cosPsi)/alpha;

  m_h[0]=yv*cosPhi0-xv*sinPhi0+P0*(1-cosPsi)/alpha;
  m_h[1]=zv+P0*ctt*psi/alpha;

  m_resid[0]=m_u[0]-m_h[0];
  m_resid[1]=m_u[1]-m_h[1];

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
    {
      for(j=0;j<3;j++) m_D[i][j]=AC[i][j];
      for(j=3;j<3+shift;j++) 
	{
	  m_D[i][j]=0.0;
	  for(k=0;k<3;k++)
	    m_D[i][j]+=m_A[i][k]*pV->m_Gk[k][j];
	}
      for(j=0;j<3;j++) m_D[i][j+3+shift]=BV[i][j]-m_Vuq[i][j];
    }
    
  return chi2;
}


void TrigVertexFitInputTrack::updateVertex(class TrigL2Vertex* pV)
{
  int i{0},j{0},k{0};
  const int nSize=6+3*m_index;

  double K[2][MAX_SIZE_VERT_COVM];

  for(i=0;i<2;i++)
    {
      for(j=0;j<nSize;j++)
	{
	  K[i][j]=0.0;
	  for(k=0;k<2;k++) K[i][j]+=m_D[k][j]*m_V[k][i];
	}
    }
  for(i=0;i<nSize;i++)
    {
      pV->getParametersVector()[i]+=K[0][i]*m_resid[0]+K[1][i]*m_resid[1];
      for(j=i;j<nSize;j++)
	{
	  pV->m_Gk[i][j]-=K[0][i]*m_D[0][j]+K[1][i]*m_D[1][j];
	  pV->m_Gk[j][i]=pV->m_Gk[i][j];
	}
    }
}

MsgStream& TrigVertexFitInputTrack::report( MsgStream& out ) const
{
  out<<"Track "<<m_index<<" : mass="<<m_mass<<endmsg;
  for(int i=0;i<2;i++)
    {
      out<<"  u"<<i<<" = "<<m_u[i]<<"      "<<m_Vuu[i][0]<<"   "<<m_Vuu[i][1]<<endmsg;
    }
  for(int i=0;i<3;i++)
    {
      out<<"  q"<<i<<" = "<<m_q[i]<<"      "<<m_Vqq[i][0]<<"   "<<m_Vqq[i][1]<<"   "<<m_Vqq[i][2]<<endmsg;
    }
  return out;
}

TrigVertexFitConstraint::TrigVertexFitConstraint(double m, 
						 const TrigVertexFitInputTrack* pT1, 
						 const TrigVertexFitInputTrack* pT2) : m_value(m)
{
  m_trackList.clear();
  m_trackList.push_back(pT1);
  m_trackList.push_back(pT2);
}

TrigVertexFitConstraint::TrigVertexFitConstraint(double m, 
						 const TrigVertexFitInputTrack* pT1, 
						 const TrigVertexFitInputTrack* pT2,
						 const TrigVertexFitInputTrack* pT3) : m_value(m)
{
  m_trackList.clear();
  m_trackList.push_back(pT1);m_trackList.push_back(pT2);m_trackList.push_back(pT3);
}


TrigVertexFitConstraint::~TrigVertexFitConstraint()
{
  m_trackList.clear();
}

double TrigVertexFitConstraint::calculateInvariantMass(TrigL2Vertex* pV)
{
  const double C=0.029997;
  const double B=20.84;

  double invMass=0.0,alpha=C*B/1000.0;
  double P[3];double E=0.0;
  
  double* Rk = pV->getParametersVector();

  int offset=0;
  for(int i=0;i<3;i++) P[i]=0.0;

  for(std::list<const TrigVertexFitInputTrack*>::iterator it=m_trackList.begin();it!=m_trackList.end();++it)
    {
      offset=3+3*(*it)->getIndex();
      double mass=(*it)->getMass()/1000.0;
      double pT=fabs(Rk[offset+2]);
      double p=pT/sin(Rk[offset+1]);

      double psi=-asin(alpha*(Rk[0]*cos(Rk[offset])+Rk[1]*sin(Rk[offset]))/Rk[offset+2]);
      double phiV=Rk[offset]+psi;
      P[0]+=pT*cos(phiV);
      P[1]+=pT*sin(phiV);
      P[2]+=pT*cos(Rk[offset+1])/sin(Rk[offset+1]);
      E+=sqrt(mass*mass+p*p);
    }
  invMass=sqrt(E*E-P[0]*P[0]-P[1]*P[1]-P[2]*P[2]);

  return invMass;
}


double TrigVertexFitConstraint::getChi2Distance(TrigL2Vertex* pV)
{
  const double C=0.029997;
  const double B=20.84;

  double invMass=0.0,alpha=C*B/1000.0;
  double P[3];
  double E=0.0;
  int i{0},j{0};
  bool linFailed=false;
  double* Rk = pV->getParametersVector();

  int offset=0;
  for(i=0;i<3;i++) P[i]=0.0;

  int nSize=3+3*pV->getTracks()->size();

  for(std::list<const TrigVertexFitInputTrack*>::iterator it=m_trackList.begin();it!=m_trackList.end();++it)
    {
      offset=3+3*(*it)->getIndex();
      double mass=(*it)->getMass()/1000.0;
      double pT=fabs(Rk[offset+2]);
      double p=pT/sin(Rk[offset+1]);

      double psi=-asin(alpha*(Rk[0]*cos(Rk[offset])+Rk[1]*sin(Rk[offset]))/Rk[offset+2]);
      double phiV=Rk[offset]+psi;
      P[0]+=pT*cos(phiV);
      P[1]+=pT*sin(phiV);
      P[2]+=pT*cos(Rk[offset+1])/sin(Rk[offset+1]);
      E+=sqrt(mass*mass+p*p);
    }
  invMass=sqrt(E*E-P[0]*P[0]-P[1]*P[1]-P[2]*P[2]);

  m_resid[0]=m_value/1000.0-invMass;
  m_D[0][0]=0.0;m_D[0][1]=0.0;m_D[0][2]=0.0;

  for(std::list<const TrigVertexFitInputTrack*>::iterator it=m_trackList.begin();it!=m_trackList.end();++it)
    {
      offset=3+3*(*it)->getIndex();

      double mass=(*it)->getMass()/1000.0;
      double Ck=(Rk[offset+2]<0.0)?-1.0:1.0;
      double sinT=sin(Rk[offset+1]);
      double cosT=cos(Rk[offset+1]);
      double pT=fabs(Rk[offset+2]);
      double p=pT/sinT;
      double e=sqrt(p*p+mass*mass);
      double sinF=sin(Rk[offset]);
      double cosF=cos(Rk[offset]);

      double sinPsi=-alpha*(Rk[0]*cosF+Rk[1]*sinF)/Rk[offset+2];
      if(fabs(sinPsi)>1.0)
	{
	  linFailed=true;
	  break;
	}
      double psi=asin(sinPsi);
      double cosPsi=sqrt(1.0-sinPsi*sinPsi);
      double phiV=Rk[offset]+psi;
      
      double aCos=alpha*Ck/cosPsi;
      double dP=P[1]*cos(phiV)-P[0]*sin(phiV);
      double eE=E*Rk[offset+2]/(e*sinT);

      m_D[0][0]+=dP*cosF*aCos;
      m_D[0][1]+=dP*sinF*aCos;
      m_D[0][offset]=-dP*Ck*(Rk[offset+2]-Ck*aCos*(Rk[1]*cosF-Rk[0]*sinF));
      m_D[0][offset+1]=(Rk[offset+2]/(sinT*sinT))*(P[2]*Ck-eE*cosT);
      m_D[0][offset+2]=eE/sinT-Ck*(P[0]*cos(phiV)+P[1]*sin(phiV)+P[2]*cosT/sinT)+dP*Ck*sinPsi/cosPsi;     
    }
  for(i=0;i<nSize;i++) m_D[0][i]/=invMass;
  if(linFailed) return -999.9;
  /*
  for(i=0;i<nSize;i++) printf("%E ",m_D[0][i]);
  printf("\n");
  printf("----- numerical Jacobian -----\n");

  double oldPar, newPar, Delta, newMass, der;
  for(i=0;i<nSize;i++)
    {
      oldPar=Rk[i];
      Delta=0.00001*oldPar;
      newPar=oldPar+Delta;
      Rk[i]=newPar;
      newMass=calculateInvariantMass(pV);
      der=(newMass-invMass)/Delta;
      m_D[1][i]=der;
      Rk[i]=oldPar;
    }
  for(i=0;i<nSize;i++) printf("%E ",m_D[1][i]);
  printf("\n");
  printf("------------------------------\n");
  */

  double covM=1e-12;

  for(i=0;i<nSize;i++)
    {
      for(j=i;j<nSize;j++) 
	{
	  double dCov=pV->m_Gk[i][j]*m_D[0][i]*m_D[0][j];
	  if(i!=j) dCov*=2.0;
	  covM+=dCov;
	}
    }
  m_V[0][0]=1.0/covM;
  return ((m_resid[0]*m_resid[0])*m_V[0][0]);
}
 
void TrigVertexFitConstraint::updateVertex(TrigL2Vertex* pV)
{
  double K[MAX_SIZE_VERT_COVM];
  int i{0},j{0};
  const int nSize=3+3*pV->getTracks()->size();
  double gain{0};

  for(i=0;i<nSize;i++)
    {
      gain=0.0;
      for(j=0;j<nSize;j++) gain+=pV->m_Gk[i][j]*m_D[0][j];
      m_D[1][i]=gain;
      K[i]=gain*m_V[0][0];
    }
  for(i=0;i<nSize;i++)
    {
      pV->getParametersVector()[i]+=K[i]*m_resid[0];
      for(j=i;j<nSize;j++)
	{
	  pV->m_Gk[i][j]-=K[i]*m_D[1][j];
	  pV->m_Gk[j][i]=pV->m_Gk[i][j];
	}
    }
}

MsgStream& TrigVertexFitConstraint::report( MsgStream& out) const
{
  out<<"Mass constraint with "<<m_trackList.size()<<" : mass="<<m_value<<endmsg;
  return out;
}

double TrigVertexFitConstraint::getValue()
{
  return m_value;
}

TrigL2Vertex::TrigL2Vertex()
{
  m_pvConstraints=new std::list<TrigVertexFitConstraint*>;
  m_pvTracks=new std::list<TrigVertexFitInputTrack*>;
  m_pvConstraints->clear();m_pvTracks->clear();
  m_nStatus=0;
  m_nTracks=0;
  m_P=NULL;
  m_mass=0.0;
  m_massVar=0.0;
  m_chiSquared=0.0;
  m_nDOF=0;
  m_ready=false;
}

TrigL2Vertex::~TrigL2Vertex()
{
  for(std::list<TrigVertexFitInputTrack*>::iterator tIt=m_pvTracks->begin();
      tIt!=m_pvTracks->end();++tIt)
    {
      delete (*tIt);
    }
  for(std::list<TrigVertexFitConstraint*>::iterator tIt=m_pvConstraints->begin();
      tIt!=m_pvConstraints->end();++tIt)
    {
      delete (*tIt);
    }
  m_pvConstraints->clear();m_pvTracks->clear();
  delete m_pvTracks;delete m_pvConstraints;
  if(m_P!=NULL) delete m_P;
}

int TrigL2Vertex::getNumberOfTracks()
{
  m_nTracks=m_pvTracks->size();
  return m_nTracks;
}

bool TrigL2Vertex::prepareForFit()
{
  m_nTracks=m_pvTracks->size();
  m_nDOF=-3;
  m_chiSquared=0.0;
  m_ready=true;
  return true;
}

void TrigL2Vertex::reset()
{
  memset(&m_Gk[0][0],0,sizeof(m_Gk));
  m_nDOF=-3;
  m_chiSquared=0.0;
}

bool TrigL2Vertex::isVertexFitted()
{
  return (m_nStatus>0);
}

bool TrigL2Vertex::isMassEstimated()
{
  return (m_nStatus>1);
}

const TrigInDetTrackFitPar* TrigL2Vertex::getMotherTrack()
{
  return m_P;
}

void TrigL2Vertex::setMotherTrack(TrigInDetTrackFitPar* P)
{
  m_P=P;
}

double TrigL2Vertex::chi2()
{
  return m_chiSquared;
}

int TrigL2Vertex::ndof()
{
  return m_nDOF;
}

void TrigL2Vertex::addChi2(double chi2)
{
  m_chiSquared+=chi2;
}

void TrigL2Vertex::addNdof(int n)
{
  m_nDOF+=n;
}

double TrigL2Vertex::mass()
{
  if(isMassEstimated()) return m_mass;
  else return 0.0;}

  
double TrigL2Vertex::massVariance()
{
  if(isMassEstimated()) return m_massVar;
  else return 0.0;
}

double* TrigL2Vertex::getParametersVector()
{
  return &m_Rk[0];
}

std::list<TrigVertexFitInputTrack*>* TrigL2Vertex::getTracks()
{
  return m_pvTracks;
}

std::list<TrigVertexFitConstraint*>* TrigL2Vertex::getConstraints()
{
  return m_pvConstraints;
}

void TrigL2Vertex::setStatus(int s)
{
  m_nStatus=s;
}

void TrigL2Vertex::setMass(double m)
{
  m_mass=m;
}
 
void TrigL2Vertex::setMassVariance(double v)
{
  m_massVar=v;
}

int TrigL2Vertex::getStatus()
{
  return m_nStatus;
}

bool TrigL2Vertex::isReadyForFit()
{
  return m_ready;
}

MsgStream& TrigL2Vertex::report( MsgStream& msg) const
{
  const int nSize=3+3*m_pvTracks->size();

  for(int i=0;i<nSize;i++)
    {
      msg<<m_Rk[i]<<"   ";
      for(int j=0;j<nSize;j++) msg<<m_Gk[i][j]<<"  ";
      msg<<endmsg;
    }
  return msg;
}

const TrigVertexFitInputTrack* TrigL2Vertex::contains(const TrigInDetTrack* pT)
{
  TrigVertexFitInputTrack* pI=NULL;
  for(std::list<TrigVertexFitInputTrack*>::iterator it=m_pvTracks->begin();
      it!=m_pvTracks->end();++it)
    {
      if((*it)->getTrackType()!=1) continue;
      if(pT==(*it)->getTrigTrack())
	{
	  pI=(*it);break;
	}
    }
  return pI;
}


const TrigVertexFitInputTrack* TrigL2Vertex::contains(const Trk::Track* pT)
{
  TrigVertexFitInputTrack* pI=NULL;
  for(std::list<TrigVertexFitInputTrack*>::iterator it=m_pvTracks->begin();
      it!=m_pvTracks->end();++it)
    {
      if((*it)->getTrackType()!=2) continue;
      if(pT==(*it)->getTrkTrack())
	{
	  pI=(*it);break;
	}
    }
  return pI;
}
