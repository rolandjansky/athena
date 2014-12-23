/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DistributedKalmanFilter.cxx
//   Source file for class DistributedKalmanFilter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"

#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkEventUtils/PrepRawDataComparisonFunction.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

//DKF stuff

#include "TrkDistributedKalmanFilter/DistributedKalmanFilter.h"
#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "TrkTrack/TrackInfo.h"


#include <vector>
#include <ext/algorithm>

// constructor

Trk::DistributedKalmanFilter::DistributedKalmanFilter(const std::string& t,const std::string& n,
						      const IInterface* p) :
  AthAlgTool(t,n,p),
  m_ROTcreator("Trk::RIO_OnTrackCreator/RIO_OnTrackCreator"),
  m_extrapolator("Trk::Extrapolator/Extrapolator"), 
  m_MagFieldSvc("AtlasFieldSvc",this->name())
{
  // AlgTool stuff
  declareInterface<ITrackFitter>( this );
  m_pvpNodes=new std::vector<TrkBaseNode*>;
  m_pvpSurfaces=new std::vector<TrkPlanarSurface*>;
  m_pvpTrackStates=new std::vector<TrkTrackState*>;

  declareProperty( "AthenaFieldService", m_MagFieldSvc, "AlasFieldService");
  declareProperty( "ExtrapolatorTool", m_extrapolator, "Extrapolator" );
  declareProperty( "ROTcreator", m_ROTcreator, "ROTcreatorTool" );
  // ME temporary fix
  declareProperty("sortingReferencePoint",m_option_sortingRefPoint);
}

// destructor
Trk::DistributedKalmanFilter::~DistributedKalmanFilter() 
{
  delete m_pvpSurfaces;
  delete m_pvpNodes;
  delete m_pvpTrackStates;
}

// initialize
StatusCode Trk::DistributedKalmanFilter::initialize()
{
  StatusCode s = AthAlgTool::finalize();  

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endreq; 
    return StatusCode::FAILURE;
  }  

  s = m_MagFieldSvc.retrieve();
  if(s.isFailure()) {
    msg(MSG::FATAL)<<"Could not find Athena MagFieldService "<<endreq;
    return s; 
  }

  s=m_ROTcreator.retrieve();
  if (s.isFailure()) 
    {
      msg(MSG::FATAL)<<"Could not get ROTcreator "<<m_ROTcreator<<endreq;
      return s;
    }

  s=m_extrapolator.retrieve();
  if (s.isFailure()) 
    {
      msg(MSG::FATAL)<<"Could not get extrapolator tool:"<<m_extrapolator<<endreq;
      return s;
    }

  msg(MSG::INFO) << "initialize() successful in Trk::DistributedKalmanFilter"
      << endreq;
  return StatusCode::SUCCESS;
}


// finalize
StatusCode Trk::DistributedKalmanFilter::finalize()
{
  // init message stream
  StatusCode sc = AthAlgTool::finalize();
  msg(MSG::INFO) << " finalize() successful in Trk::DistributedKalmanFilter" << endreq;	
  return sc;
}


// helper operator for STL min_element
struct minTrkPar : public std::binary_function<const Trk::TrackParameters*, const Trk::TrackParameters*, bool> {
    bool operator() (const Trk::TrackParameters* one, const Trk::TrackParameters* two) const {
        return (one->position().mag() < two->position().mag());
    };
};

// refit a track
Trk::Track* Trk::DistributedKalmanFilter::fit(const Trk::Track&       inputTrack,
                                              const RunOutlierRemoval runOutlier,
                                              const ParticleHypothesis matEffects) const
{
  msg(MSG::VERBOSE) << "--> enter DistributedKalmanFilter::fit(Track,,)" << endreq;
  
  // protection against track not having any parameters
  if (inputTrack.trackParameters()->empty()) {
    msg(MSG::ERROR) << "need estimated track parameters near "
	<< "origin, reject fit" << endreq;
    return 0;
  }
  // protection against not having RIO_OnTracks
  if (inputTrack.measurementsOnTrack()->empty()) {
    msg(MSG::ERROR) << "try to refit track with empty "
	<< "vec<RIO_OnTrack>, reject fit" << endreq;
    return 0;
  }
	
  // create PrepRawData subset
  PrepRawDataSet prepRDColl;

  // collect PrepRawData pointers from input track ROTs
  msg(MSG::VERBOSE) << "extract PrepRawDataSet from Track" << endreq;
  DataVector<const MeasurementBase>::const_iterator it    = inputTrack.measurementsOnTrack()->begin();
  DataVector<const MeasurementBase>::const_iterator itEnd = inputTrack.measurementsOnTrack()->end(); 
  for ( ; it!=itEnd; ++it) {
    if (!(*it)) 
      {
	msg(MSG::WARNING) << "This track contains empty MeasurementBase "
	    << "objects! Skipped this MB.." << endreq;  
      } 
    else 
      {
	const RIO_OnTrack* testROT = dynamic_cast<const RIO_OnTrack*>(*it);
	const PrepRawData* prepRD = (testROT) ? (testROT)->prepRawData() : 0;
	if (!prepRD) {
	  msg(MSG::WARNING) << "RIO_OnTrack->prepRawRata() "
	      << "returns no object, ignore... " << endreq;
	} else { prepRDColl.push_back( prepRD );  }
      }
  }
  const TrackParameters* minPar = *(std::min_element(inputTrack.trackParameters()->begin(),inputTrack.trackParameters()->end(),
						     minTrkPar()));
							
  // fit set of PrepRawData using main method, start with first Track Parameter in inputTrack
  msg(MSG::VERBOSE) << "call fit(PRDSet,TP,,)" << endreq;
  return fit(prepRDColl,*minPar,runOutlier,matEffects);
}

double Trk::DistributedKalmanFilter::m_integrate(double Rk[5], 
						 TrkPlanarSurface* pSB,
						 TrkPlanarSurface* pSE, double* Rf) const
{
  const double C=0.02999975;
  const int nStepMax=5;
	  
  double sint,cost,sinf,cosf;
  double gP[3],lP[3],gV[3],a,b,c,descr,CQ,Ac,Av;
  double V[3],P[3],D[4],gB[3];
  int i,nStep;
  double sl,ds,path=0.0;

  sint=sin(Rk[3]);cosf=cos(Rk[2]);
  sinf=sin(Rk[2]);cost=cos(Rk[3]);
  gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*Rk[4];

  if(pSB!=NULL)
    {
      lP[0]=Rk[0];lP[1]=Rk[1];lP[2]=0.0;
      pSB->m_transformPointToGlobal(lP,gP);
    }
  else
    {
      gP[0]=-Rk[0]*sinf;
      gP[1]= Rk[0]*cosf;
      gP[2]= Rk[1];
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
      printf("D<0 - extrapolation failed\n");
      return 0;
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

      P[0]=gP[0]+gV[0]*ds+Ac*(gV[1]*gB[2]-gV[2]*gB[1]);
      P[1]=gP[1]+gV[1]*ds+Ac*(gV[2]*gB[0]-gV[0]*gB[2]);
      P[2]=gP[2]+gV[2]*ds+Ac*(gV[0]*gB[1]-gV[1]*gB[0]);
      V[0]=gV[0]+Av*(gV[1]*gB[2]-gV[2]*gB[1]);
      V[1]=gV[1]+Av*(gV[2]*gB[0]-gV[0]*gB[2]);
      V[2]=gV[2]+Av*(gV[0]*gB[1]-gV[1]*gB[0]);
      for(i=0;i<3;i++) 
	{
	  gV[i]=V[i];gP[i]=P[i];
	}
      m_getMagneticField(gP,gB);
      nStep--;
    }
  pSE->m_transformPointToLocal(P,lP);

  Rf[0]=lP[0];Rf[1]=lP[1];
  Rf[2]=atan2(V[1],V[0]);
  Rf[3]=acos(V[2]);
  Rf[4]=Rk[4];

  return path;
}


void Trk::DistributedKalmanFilter::m_numericalJacobian(TrkTrackState* pTS, 
						       TrkPlanarSurface*,// pSB,
						       TrkPlanarSurface*,// pSE,
						       double A[5][5]) const
{
  const double eps=0.005;
  
  double R0[5],delta,Ri[5],Rf[5],J[5][5],Rs[5];
  int i,j;
  //not sure what Rs/Rf should be intialised to - at the moment it is not set at all so setting to zero to solve coverity bugs 14380/14381...
  for(i=0;i<5;i++) {R0[i]=pTS->m_getTrackState(i);Rs[i]=0;Rf[i]=0;}
  
  //s0=m_integrate(R0,pSB,pSE,Rs);
  
  for(i=0;i<5;i++)
    {
      delta=R0[i]*eps;
      for(j=0;j<5;j++) Ri[j]=R0[j];
      Ri[i]+=delta;
      
      //s1=m_integrate(Ri,pSB,pSE,Rf);
      
      // dsdp[i]=(s1-s0)/delta;
      for(j=0;j<5;j++)
	{
	  J[j][i]=(Rf[j]-Rs[j])/delta;
	}
    }
  //printf("------ Numerical Jacobian ------\n");
  for(i=0;i<5;i++)
    {
      for(j=0;j<5;j++) 
	{
	  // printf("%E ",J[i][j]);
	  A[i][j]=J[i][j];
	}
      // printf("\n");
    }
}



Trk::TrkTrackState* Trk::DistributedKalmanFilter::m_extrapolate(Trk::TrkTrackState* pTS, 
								Trk::TrkPlanarSurface* pSB,
								Trk::TrkPlanarSurface* pSE) const
{
  const double C=0.02999975;
  const double minStep=30.0;
	    
  double J[5][5],Rf[5],AG[5][5],Gi[5][5],Gf[5][5],A[5][5];
  int i,j,m;

  bool samePlane=false;

  if(pSB!=NULL)
    {   
      double diff=0.0;
      for(i=0;i<4;i++) diff+=fabs(pSE->m_getPar(i)-pSB->m_getPar(i));
      if(diff<1e-5) {
	samePlane=true;	
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
  
    m_getMagneticField(gP,gB);

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
    
    m_getMagneticField(P,gB);
  
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

  pTE->m_setTrackState(Rf);
  pTE->m_setTrackCovariance(Gf);
  pTE->m_attachToSurface(pSE);
  pTE->m_applyMaterialEffects();

  for(i=0;i<5;i++) for(j=0;j<5;j++)
    {
      Gi[i][j]=pTE->m_getTrackCovariance(i,j);
    }

  m_matrixInversion5x5(Gi);
 
  for(i=0;i<5;i++) for(j=0;j<5;j++)
    {
      A[i][j]=0.0;
      for(m=0;m<5;m++) A[i][j]+=AG[m][i]*Gi[m][j];
    }
  pTE->m_setPreviousState(pTS);
  pTE->m_setSmootherGain(A);

  return pTE;
}


void Trk::DistributedKalmanFilter::m_matrixInversion5x5(double a[5][5]) const 
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

bool Trk::DistributedKalmanFilter::m_runForwardKalmanFilter(TrkTrackState* pInitState) const
{
  std::vector<TrkBaseNode*>::iterator pnIt(m_pvpNodes->begin()),pnEnd(m_pvpNodes->end());
  bool OK=true;
  Trk::TrkPlanarSurface *pSB=NULL,*pSE;

  Trk::TrkTrackState* pTS=new Trk::TrkTrackState(pInitState);
  m_pvpTrackStates->push_back(pTS);
  for(;pnIt!=pnEnd;++pnIt)
    {
      pSE=(*pnIt)->m_getSurface();
      Trk::TrkTrackState* pNS=m_extrapolate(pTS,pSB,pSE);
      pSB=pSE;
      if(pNS!=NULL)
	{
	  m_pvpTrackStates->push_back(pNS);

	  bool isNaN = std::isnan(pTS->m_getTrackState(0)) ||
	               std::isnan(pTS->m_getTrackState(1)) ||
	               std::isnan(pTS->m_getTrackState(2)) ||
	               std::isnan(pTS->m_getTrackState(3)) ||
	    std::isnan(pTS->m_getTrackState(4));
	  if(isNaN) {
	    OK=false;break;
	  }

	  (*pnIt)->m_validateMeasurement(pNS);
	  (*pnIt)->m_updateTrackState(pNS);
	  pTS=pNS;
	}
      else
	{
	  OK=false;break;
	}
    }
  return OK;
}

void Trk::DistributedKalmanFilter::m_runSmoother() const
{
  std::vector<TrkTrackState*>::reverse_iterator ptsIt(m_pvpTrackStates->rbegin()),
    ptsEnd(m_pvpTrackStates->rend());
  for(;ptsIt!=ptsEnd;++ptsIt)
    {
      (*ptsIt)->m_runSmoother();
    }
}

void Trk::DistributedKalmanFilter::m_calculateLRsolution() const
{
  std::vector<TrkBaseNode*>::iterator pnIt(m_pvpNodes->begin()),pnEnd(m_pvpNodes->end());

  for(;pnIt!=pnEnd;++pnIt)
    {
      (*pnIt)->m_updateInternal();
    }
}

int Trk::DistributedKalmanFilter::m_findOutliers(double cut) const
{
  double dchi2;
  int nOutl=0;
  std::vector<TrkBaseNode*>::iterator pnIt(m_pvpNodes->begin()),pnEnd(m_pvpNodes->end());

  for(;pnIt!=pnEnd;++pnIt)
    {
      dchi2=((*pnIt)->m_getChi2Distance((*pnIt)->m_getTrackState()))/(*pnIt)->m_getNdof();
      if(dchi2>cut)
	{
	  if((*pnIt)->m_isValidated()) nOutl++;
	  (*pnIt)->m_setNodeState(0);
	}
      else (*pnIt)->m_setNodeState(1);
    }
  return nOutl;
}


Trk::TrackStateOnSurface* Trk::DistributedKalmanFilter::m_createTrackStateOnSurface(Trk::TrkBaseNode* pN) const
{
  TrackStateOnSurface* pTSS=NULL;
  char type=pN->m_getNodeType();
  const Trk::TrackParameters* pTP=NULL;



  if(type==0) return pTSS;
 
  TrkTrackState* pTS=pN->m_getTrackState();
  const Trk::PrepRawData* pPRD=pN->m_getPrepRawData();

  if((type==1)||(type==2))
    {
      const Trk::Surface& rS = pPRD->detectorElement()->surface();
      const Trk::PlaneSurface* pPS = dynamic_cast<const Trk::PlaneSurface*>(&rS);
      if(pPS==NULL) return pTSS;
      
      AmgSymMatrix(5)* pM = new AmgSymMatrix(5);

      for(int i=0;i<5;i++) 
	for(int j=0;j<5;j++)
	  (*pM)(i,j)=pTS->m_getTrackCovariance(i,j);
  
      pTP=new Trk::AtaPlane(pTS->m_getTrackState(0),
			    pTS->m_getTrackState(1),
			    pTS->m_getTrackState(2),
			    pTS->m_getTrackState(3),
			    pTS->m_getTrackState(4),*pPS,
			    pM);
    }
  else if(type==3)
    {
      const Trk::Surface& rS = pPRD->detectorElement()->surface(pPRD->identify()); 
      const Trk::StraightLineSurface* pLS=dynamic_cast<const Trk::StraightLineSurface*>(&rS);
      if(pLS==NULL) return pTSS;

      AmgSymMatrix(5)* pM = new AmgSymMatrix(5);

      for(int i=0;i<5;i++) 
	for(int j=0;j<5;j++)
	  (*pM)(i,j)=pTS->m_getTrackCovariance(i,j);

      if((pTS->m_getTrackState(2)<-M_PI) ||(pTS->m_getTrackState(2)>M_PI))
	printf("Phi is beyond the range\n");
      
      pTP=new Trk::AtaStraightLine(pTS->m_getTrackState(0),
				   pTS->m_getTrackState(1),
				   pTS->m_getTrackState(2),
				   pTS->m_getTrackState(3),
				   pTS->m_getTrackState(4),
				   *pLS,
				   pM);
    }
  if(pTP==NULL) return NULL;
  const Trk::RIO_OnTrack* pRIO=m_ROTcreator->correct(*pPRD,*pTP);
  if(pRIO==NULL) 
    {
      if(pTP!=NULL) delete pTP;
      return NULL;
    }
  Trk::FitQualityOnSurface* pFQ=new Trk::FitQualityOnSurface(pN->m_getChi2(),pN->m_getNdof());
  pTSS = new Trk::TrackStateOnSurface(pRIO,pTP,pFQ);
  return pTSS;
}

Trk::Perigee* Trk::DistributedKalmanFilter::m_createMeasuredPerigee(TrkTrackState* pTS) const
{


  Trk::Perigee* pMP=NULL;

  AmgSymMatrix(5)* pM = new AmgSymMatrix(5);

  for(int i=0;i<5;i++) 
    for(int j=0;j<5;j++)
      (*pM)(i,j)=pTS->m_getTrackCovariance(i,j);
  const Trk::PerigeeSurface perSurf;
  pMP = new Trk::Perigee(pTS->m_getTrackState(0),
			 pTS->m_getTrackState(1),
			 pTS->m_getTrackState(2),
			 pTS->m_getTrackState(3),
			 pTS->m_getTrackState(4),perSurf,pM);
  return pMP;
}

void Trk::DistributedKalmanFilter::m_getMagneticField(double gP[3], double* pB) const 
{ 

  pB[0]=0.0;pB[1]=0.0;pB[2]=0.0;
  double field[3];
  m_MagFieldSvc->getField(gP,field);//field is returned in kT
  for(int i=0;i<3;i++) pB[i]=field[i]/CLHEP::kilogauss;//convert to kG
}

// fit a set of PrepRawData objects
Trk::Track* Trk::DistributedKalmanFilter::fit(const Trk::PrepRawDataSet&   prepRDColl,
					      const Trk::TrackParameters&  estimatedParametersNearOrigine,
					      const RunOutlierRemoval      runOutlier,
					      const Trk::ParticleHypothesis     matEffects) const
{
  const double outlCut=12.0;
  const double rlSili=0.022;
  const double rlTrt=0.005;

  TrkPlanarSurface* pS;
  TrkTrackState *pTS, *pInitState;
  TrkPixelNode* pPN;
  TrkClusterNode* pCN;
  TrkEndCapClusterNode* pECN;
  TrkTrtNode* pTN;
  double Rk[5],C[3],N[3],M[3][3];
  int i,nAmbHits=0;
  const Perigee* pP;
  Trk::Track* fittedTrack=NULL;
  Eigen::Vector3d mx,my,mz;
  const Trk::PerigeeSurface perSurf;

  bool badTrack=false;

  msg(MSG::VERBOSE) << "--> entering DistributedKalmanFilter::fit"
      << "(PrepRawDataSet,TrackParameters,,)" << endreq;
	
  // protection, if empty PrepRawDataSet
  if (prepRDColl.empty()) {
    msg(MSG::ERROR) << "try to fit empty vec<PrepRawData>, reject fit" << endreq;
    return 0;
  }
  if (runOutlier) msg(MSG::VERBOSE) << "-> Outlier removal switched on" << endreq;
  if (matEffects!=nonInteracting) msg(MSG::VERBOSE) << "-> Material Effects switched on" << endreq;


  // 0. Sort PRD set

  Trk::PrepRawDataComparisonFunction* compareHits = new Trk::PrepRawDataComparisonFunction
    (estimatedParametersNearOrigine.position(),
     estimatedParametersNearOrigine.momentum());
  PrepRawDataSet inputPRDColl=PrepRawDataSet(prepRDColl);
  if(!is_sorted(inputPRDColl.begin(),inputPRDColl.end(),*compareHits)) 
    sort(inputPRDColl.begin(), inputPRDColl.end(), *compareHits);
  delete compareHits;
  msg(MSG::VERBOSE)<<" List of sorted PRDs: "<<endreq;      
  for(PrepRawDataSet::const_iterator prdIt=inputPRDColl.begin();prdIt!=inputPRDColl.end();prdIt++) 
    {
      if(!(*prdIt)->detectorElement()) continue;
      msg(MSG::VERBOSE)<<m_idHelper->print_to_string((*prdIt)->identify())<<" radius= "<<
	(*prdIt)->detectorElement()->surface((*prdIt)->identify()).center().mag()<<endreq;
    }

  // 1. Create initial track state:

  pP=dynamic_cast<const Perigee*>(&estimatedParametersNearOrigine);
  if(pP==NULL)
    {
      msg(MSG::DEBUG) << " fit needs perigee parameters - creating it" << endreq;
      /*
      const Trk::TrackParameters* pTP=m_extrapolator->extrapolate(estimatedParametersNearOrigine, perSurf,
								  Trk::oppositeMomentum, false, matEffects);
      */
      const Trk::TrackParameters* pTP=m_extrapolator->extrapolateDirectly(estimatedParametersNearOrigine, 
									  perSurf,
									  Trk::anyDirection, false, Trk::nonInteracting);

      pP=dynamic_cast<const Perigee*>(pTP);
      if(pP==NULL)
	{
	  msg(MSG::ERROR) << "failed to create perigee parameters, reject fit" << endreq;
	  return 0;
	}
    }
  Rk[0]=pP->parameters()[Trk::d0];
  Rk[1]=pP->parameters()[Trk::z0];
  Rk[2]=pP->parameters()[Trk::phi0];
  Rk[3]=pP->parameters()[Trk::theta];
  Rk[4]=pP->parameters()[Trk::qOverP];
  pTS = new TrkTrackState(Rk);

  if (matEffects==Trk::nonInteracting) 
    {
      pTS->m_setScatteringMode(0);
      msg(MSG::VERBOSE) << "-> Multiple Scattering treatment switched off" << endreq;
    }
  else 
    {
      if ((matEffects==Trk::pion)||(matEffects==Trk::muon))      
	{
	  pTS->m_setScatteringMode(1);
	  msg(MSG::VERBOSE) << "-> Multiple Scattering treatment switched on" << endreq;
	}
      else 
	{
	  if (matEffects==Trk::electron)  
	    {
	      pTS->m_setScatteringMode(2);
	      msg(MSG::VERBOSE) << "-> Multiple Scattering and Bremm treatments switched on" << endreq;
	    }
	  else msg(MSG::WARNING) << "Material setting " << matEffects << "not supported !" << endreq;
	}
    }

  pInitState=pTS;

  // 2. Create filtering nodes:

  m_pvpNodes->clear();
  m_pvpSurfaces->clear();
  m_pvpTrackStates->clear();

  for(PrepRawDataSet::const_iterator prdIt=inputPRDColl.begin();prdIt!=inputPRDColl.end();prdIt++) 
    {
      if((*prdIt)->detectorElement()==NULL)
	{
	  msg(MSG::WARNING) << " PrepRawData has no detector element - drop it" << endreq;
	  continue;
	} 
      Identifier ID=(*prdIt)->identify();
      if(m_idHelper->is_indet(ID) || m_idHelper->is_sct(ID) || m_idHelper->is_pixel(ID) || m_idHelper->is_trt(ID))
	{	
	  if(m_idHelper->is_pixel(ID)|| m_idHelper->is_sct(ID))
	    {
	      const Trk::Surface& rSurf=(*prdIt)->detectorElement()->surface();
	      N[0]=rSurf.normal().x();
	      N[1]=rSurf.normal().y();
	      N[2]=rSurf.normal().z();
	      C[0]=rSurf.center().x();
	      C[1]=rSurf.center().y();
	      C[2]=rSurf.center().z();
	      mx=rSurf.transform().rotation().block(0,0,3,1);
	      my=rSurf.transform().rotation().block(0,1,3,1);
	      mz=rSurf.transform().rotation().block(0,2,3,1);
	      for(i=0;i<3;i++) 
		{
		  M[i][0]=mx[i];M[i][1]=my[i];M[i][2]=mz[i];
		}
	      pS = new TrkPlanarSurface(C,N,M,rlSili);
	      m_pvpSurfaces->push_back(pS);
	      if(m_idHelper->is_pixel(ID))
		{
		  pPN = new TrkPixelNode(pS,200.0,(*prdIt));
		  m_pvpNodes->push_back(pPN);
		}
	      else
		{
		  if(fabs(N[2])<0.1)
		    {
		      pCN = new TrkClusterNode(pS,200.0,(*prdIt));
		      m_pvpNodes->push_back(pCN);
		    }
		  else
		    {
		      pECN = new TrkEndCapClusterNode(pS,200.0,(*prdIt));
		      m_pvpNodes->push_back(pECN);
		    }
		}
	      continue;
	    }
	  if(m_idHelper->is_trt(ID))
	    {
	      nAmbHits++;

	      const Trk::Surface& rSurf=(*prdIt)->detectorElement()->surface(ID);
	      double len=500.0;
	      const Trk::SurfaceBounds& rBounds=(*prdIt)->detectorElement()->surface().bounds();

	      C[0]=rSurf.center().x();C[1]=rSurf.center().y();C[2]=rSurf.center().z();
	      double cosFs=C[0]/sqrt(C[0]*C[0]+C[1]*C[1]);
	      double sinFs=C[1]/sqrt(C[0]*C[0]+C[1]*C[1]);

	      const Amg::Vector3D& rNorm=(*prdIt)->detectorElement()->surface().normal();
	      bool isBarrel=true;
	      if(fabs(rNorm.z())>0.2) isBarrel=false;
	      
	      if(isBarrel)
		{
		  const Trk::RectangleBounds& bBounds=dynamic_cast<const Trk::RectangleBounds&>(rBounds);
		  len=bBounds.halflengthY();
		  N[0]=cosFs;N[1]=sinFs;N[2]=0.0;

		  if(C[2]<0.0)
		    {
		      M[0][0]=-sinFs; M[0][1]=0.0; M[0][2]=cosFs;
		      M[1][0]= cosFs; M[1][1]=0.0; M[1][2]=sinFs;
		      M[2][0]= 0.0;   M[2][1]=1.0; M[2][2]=0.0;
		    }
		  else
		    {
		      M[0][0]= sinFs; M[0][1]=0.0; M[0][2]=cosFs;
		      M[1][0]=-cosFs; M[1][1]=0.0; M[1][2]=sinFs;
		      M[2][0]= 0.0;   M[2][1]=-1.0; M[2][2]=0.0;
		    }
		}
	      else
		{
		  const Trk::DiscBounds& ecBounds=dynamic_cast<const Trk::DiscBounds&>(rBounds);
		  len=0.5*(ecBounds.rMax()-ecBounds.rMin());
		  N[0]=0.0;N[1]=0.0;N[2]=1.0;
		  M[0][0]=-sinFs; M[0][1]=-cosFs; M[0][2]=0.0;
		  M[1][0]= cosFs; M[1][1]=-sinFs; M[1][2]=0.0;
		  M[2][0]= 0.0;   M[2][1]= 0.0;   M[2][2]=1.0;
		}
	      pS = new TrkPlanarSurface(C,N,M,rlTrt);
	      m_pvpSurfaces->push_back(pS);
	      pTN = new TrkTrtNode(pS,200.0,-len,len,(*prdIt));
	      m_pvpNodes->push_back(pTN);
	      
	      continue;
	    }
	}
      msg(MSG::WARNING) << " PrepRawData is neither identified nor supported !" << endreq;
      msg(MSG::WARNING)<<"RIO ID prints as "
	  << m_idHelper->print_to_string(ID)<<endreq;
    }
  msg(MSG::VERBOSE) << " Number of nodes/surfaces created: "<< m_pvpNodes->size()<<"/"<<m_pvpSurfaces->size()<<endreq;

  // 3. Sort vector of filtering nodes according to their distances from the perigee point

  //  m_sortFilteringNodes(pInitState);

  // 4. Main algorithm: filter and smoother (Rauch-Tung-Striebel)

  if(m_runForwardKalmanFilter(pInitState))
  {
    m_runSmoother();
    if(runOutlier)
    {
      int nOutl=m_findOutliers(outlCut);
      msg(MSG::VERBOSE) << nOutl <<" outliers removed "<<endreq;

      if((nOutl*1.0/m_pvpNodes->size())>0.3)
	{
	  msg(MSG::VERBOSE) <<" two many outliers - bad track "<<endreq;
	  badTrack=true;
	}
      if((nOutl!=0)&&(!badTrack))
	{
	  m_deleteTrackStates();
	  m_runForwardKalmanFilter(pInitState);
	  m_runSmoother();
	  nOutl=m_findOutliers(outlCut);
	  msg(MSG::VERBOSE) << nOutl <<" new outliers found "<<endreq;
	}
    }
    if((nAmbHits!=0)&&(!badTrack))
      {
	m_calculateLRsolution();
	m_deleteTrackStates();
	m_runForwardKalmanFilter(pInitState);
	m_runSmoother();
      }
  
    // 5. Create and store back all the stuff
    if(!badTrack)
      {
	pTS=(*m_pvpTrackStates->begin());
	const Trk::Perigee* pMP=m_createMeasuredPerigee(pTS);
	msg(MSG::DEBUG) <<"Fitted perigee: d0="
	    <<pMP->parameters()[Trk::d0]<<
	  " z0="
	    <<pMP->parameters()[Trk::z0]<<
	  " phi="
	    <<pMP->parameters()[Trk::phi0]<<
	  " theta="
	    <<pMP->parameters()[Trk::theta]<<
	  " qOverP="
	    <<pMP->parameters()[Trk::qOverP]<<
	  " pT="<<sin(pMP->parameters()[Trk::theta])/
	  pMP->parameters()[Trk::qOverP]<<endreq;
	
	DataVector<const TrackStateOnSurface>* pvTS = new DataVector<const TrackStateOnSurface>;
	pvTS->clear();

	std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
	typePattern.set(Trk::TrackStateOnSurface::Perigee);
	const TrackStateOnSurface* pTSOS = new TrackStateOnSurface(0,pMP,0,0,typePattern);

	pvTS->push_back(pTSOS);
	std::vector<TrkBaseNode*>::iterator pnIt(m_pvpNodes->begin()),pnEnd(m_pvpNodes->end());
	
	double chi2=0.0;
	int ndof=-5;
	
	for(;pnIt!=pnEnd;++pnIt)
	  {
	    if((*pnIt)->m_isValidated())
	      {
		TrackStateOnSurface* pTSS=m_createTrackStateOnSurface(*pnIt);
		if(pTSS!=NULL) 
		  {
		    pvTS->push_back(pTSS);
		    chi2+=(*pnIt)->m_getChi2();
		    ndof+=(*pnIt)->m_getNdof();
		  }
	      }
	  }
	msg(MSG::DEBUG) <<"Total Chi2: "<<chi2<<" DoF="<<ndof<<endreq;
	Trk::FitQuality* pFQ=new Trk::FitQuality(chi2,ndof);
	msg(MSG::DEBUG) <<pvTS->size()<<" new RIO_OnTrack(s) created"<<endreq; 
	TrackInfo info(TrackInfo::DistributedKalmanFilter,matEffects);
	fittedTrack = new Track(info,pvTS,pFQ);
      }
    else
      {
	fittedTrack=NULL;
      }
  }
  else
    {
      msg(MSG::WARNING) <<"Extrapolation failed "<<endreq;
      fittedTrack=NULL;
    }
  m_deleteNodes();
  m_deleteSurfaces();
  m_deleteTrackStates();
  delete pInitState;
  return fittedTrack;
}

Trk::Track* Trk::DistributedKalmanFilter::fit(const Trk::MeasurementSet&    inputRotColl,
                                              const Trk::TrackParameters&   estimatedParametersNearOrigine,                       
                                              const Trk::RunOutlierRemoval  runOutlier,                                           
                                              const Trk::ParticleHypothesis matEffects) const                                
{
  if (inputRotColl.empty()) {
    msg(MSG::ERROR) << "try to fit empty vec<MeasurementBase>, reject fit" << endreq;
    return NULL;
  }

  PrepRawDataSet prepRDColl;

  DataVector<const MeasurementBase>::const_iterator it    = inputRotColl.begin();
  DataVector<const MeasurementBase>::const_iterator itEnd = inputRotColl.end(); 
  for(;it!=itEnd;++it) 
    {
      if(!(*it))
	{
	  msg(MSG::WARNING) << "This track contains empty MeasurementBase "
	      << "objects! Skipped this MB.." << endreq;
	} 
      else 
	{
	  const RIO_OnTrack* pROT = dynamic_cast<const RIO_OnTrack*>(*it);
	  const PrepRawData* prepRD=(pROT)?(pROT->prepRawData()):NULL;
	  if (!prepRD) 
	    {
	      msg(MSG::WARNING) << "RIO_OnTrack->prepRawRata() "
		  << "returns no object, ignore... " << endreq;
	    } 
	  else 
	    {
	      prepRDColl.push_back( prepRD );
	    }
	}          
    }
  return fit(prepRDColl,estimatedParametersNearOrigine,runOutlier,matEffects);
}

Trk::Track* Trk::DistributedKalmanFilter::fit(const Trk::Track&  inputTrack,
					      const Trk::MeasurementSet& inputRotColl,
					      const Trk::RunOutlierRemoval runOutlier,
					      const Trk::ParticleHypothesis matEffects) const
{

  msg(MSG::VERBOSE) << "--> enter DistributedKalmanFilter::fit(Track,easurementSet,)" << endreq;
  
  // protection against track not having any parameters
  if (inputTrack.trackParameters()->empty()) {
    msg(MSG::ERROR) << "need estimated track parameters near "
	<< "origin, reject fit" << endreq;
    return 0;
  }
  // protection against not having RIO_OnTracks
  if (inputTrack.measurementsOnTrack()->empty()) {
    msg(MSG::ERROR) << "try to refit track with empty "
	<< "vec<RIO_OnTrack>, reject fit" << endreq;
    return 0;
  }
	
  // create PrepRawData subset
  PrepRawDataSet prepRDColl;

  // collect PrepRawData pointers from input track ROTs
  msg(MSG::VERBOSE) << "extract PrepRawDataSet from Track" << endreq;
  DataVector<const MeasurementBase>::const_iterator it    = inputTrack.measurementsOnTrack()->begin();
  DataVector<const MeasurementBase>::const_iterator itEnd = inputTrack.measurementsOnTrack()->end(); 
  for ( ; it!=itEnd; ++it) {
    if (!(*it)) 
      {
	msg(MSG::WARNING) << "This track contains empty MeasurementBase "
	    << "objects! Skipped this MB.." << endreq;  
      } 
    else 
      {
	const RIO_OnTrack* testROT = dynamic_cast<const RIO_OnTrack*>(*it);
	const PrepRawData* prepRD = (testROT) ? (testROT)->prepRawData() : 0;
	if (!prepRD) {
	  msg(MSG::WARNING) << "RIO_OnTrack->prepRawRata() "
	      << "returns no object, ignore... " << endreq;
	} else { prepRDColl.push_back( prepRD );  }
      }
  }
  const Trk::TrackParameters* minPar = *(std::min_element(inputTrack.trackParameters()->begin(),
							  inputTrack.trackParameters()->end(),
							  minTrkPar()));

  it    = inputRotColl.begin();
  itEnd = inputRotColl.end(); 
  for(;it!=itEnd;++it) 
    {
      if(!(*it))
	{
	  msg(MSG::WARNING) << "This track contains empty MeasurementBase "
	      << "objects! Skipped this MB.." << endreq;
	} 
      else 
	{
	  const RIO_OnTrack* pROT = dynamic_cast<const RIO_OnTrack*>(*it);
	  const PrepRawData* prepRD=(pROT)?(pROT->prepRawData()):NULL;
	  if (!prepRD) 
	    {
	      msg(MSG::WARNING) << "RIO_OnTrack->prepRawRata() "
		  << "returns no object, ignore... " << endreq;
	    } 
	  else 
	    {
	      prepRDColl.push_back( prepRD );
	    }
	}          
    }
  return fit(prepRDColl,*minPar,runOutlier,matEffects);
}


// fit a set of RIO_OnTrack objects


Trk::Track* Trk::DistributedKalmanFilter::fit(const Trk::RIO_OnTrackSet&   inputRotColl,
					      const Trk::TrackParameters&  estimatedParametersNearOrigine,
					      const Trk::RunOutlierRemoval runOutlier,
					      const Trk::ParticleHypothesis     matEffects) const
{
    msg(MSG::VERBOSE) << "--> entering DistributedKalmanFilter::fit"
        << "(RIO_OnTrackSet,TrackParameters,,)" << endreq;

    // protection, if empty RIO_OnTrackSet
    if (inputRotColl.empty()) {
        msg(MSG::ERROR) << "try to fit track+vec<ROT> with empty "
            << "extended vec<RIO_OnTrack>, reject fit" << endreq;
        return 0;
    }

    if (runOutlier) msg(MSG::VERBOSE) << "-> Outlier removal switched on" << endreq;
    if (matEffects!=Trk::nonInteracting) msg(MSG::VERBOSE) << "-> Material Effects switched on" << endreq;

    // for the time being, disintegrate ROTSet back to PrepRawData set.
    msg(MSG::VERBOSE) << "copy & downgrade ROTSet into PRDset - "
        << "this is improvised." << endreq;
    PrepRawDataSet prepRDColl;

    RIO_OnTrackSet::const_iterator it    = inputRotColl.begin();
    RIO_OnTrackSet::const_iterator itEnd = inputRotColl.end();

    for ( ; it!=itEnd; ++it) {
        const PrepRawData* prepRD = ((*it)->prepRawData());
        if (!prepRD) {
            msg(MSG::WARNING) << "RIO_OnTrack->prepRawRata() "
                << "returns no object, ignore... " << endreq;
        } else {
            prepRDColl.push_back( prepRD );
        }
    }
							
    // fit set of PrepRawData using main method, start with first Track Parameter in inputTrack
    msg(MSG::VERBOSE) << "call fit(PRDSet,TP,,)" << endreq;
    return fit(prepRDColl,estimatedParametersNearOrigine,runOutlier,matEffects);
}

void Trk::DistributedKalmanFilter::m_deleteNodes() const
{
  for(std::vector<TrkBaseNode*>::iterator pnIt=m_pvpNodes->begin();pnIt!=m_pvpNodes->end();++pnIt) 
    {
      delete (*pnIt);
    }
  m_pvpNodes->clear();
}

void Trk::DistributedKalmanFilter::m_deleteSurfaces() const
{
  std::vector<TrkPlanarSurface*>::iterator psIt(m_pvpSurfaces->begin()),psEnd(m_pvpSurfaces->end());
  for(;psIt!=psEnd;++psIt) delete (*psIt);
  m_pvpSurfaces->clear();
}

void Trk::DistributedKalmanFilter::m_deleteTrackStates() const
{
  std::vector<TrkTrackState*>::iterator ptsIt(m_pvpTrackStates->begin()),ptsEnd(m_pvpTrackStates->end());
  for(;ptsIt!=ptsEnd;++ptsIt) delete (*ptsIt);
  m_pvpTrackStates->clear();
}

