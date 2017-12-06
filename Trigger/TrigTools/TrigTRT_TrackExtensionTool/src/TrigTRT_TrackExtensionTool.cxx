/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigTRT_TrackExtensionTool tool
// -------------------------------
// ATLAS Collaboration
//
// 01.11.2005 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "GaudiKernel/ToolFactory.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/TrapezoidBounds.h"

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"


#include "TrigInDetToolInterfaces/ITrigTRT_TrackExtensionTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionGeometry.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoadTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoad.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_Trajectory.h"
#include "TrigTRT_TrackExtensionTool/TrigMagneticFieldTool.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 

#include "TrigInDetToolInterfaces/ITrigInDetBremDetectionTool.h"

#include <set>

TrigTRT_TrackExtensionTool::TrigTRT_TrackExtensionTool(const std::string& t, 
				 const std::string& n, const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_regionSelector("RegSelSvc",this->name()),
  m_trtDataProvider("TrigTRT_DriftCircleProviderTool"), 
  m_robDataProvider("ROBDataProviderSvc",this->name()),
  m_trtRoadBuilder("TrigTRT_DetElementRoadTool",this),
  m_trigFieldTool("TrigMagneticFieldTool"),
  m_trigBremTool("TrigInDetBremDetectionTool"),
  m_trackMaker("TrigDkfTrackMakerTool")
  
{
  declareInterface< ITrigTRT_TrackExtensionTool >( this );
  
  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremm=false);  
  declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
  declareProperty( "PtCut", m_minPt = 500.0);
  declareProperty( "RoiPhiHalfWidth", m_roiPhiSize = 0.2);
  declareProperty( "RoiEtaHalfWidth", m_roiEtaSize = 0.1);
  declareProperty( "OfflineClusters", m_offlineClusters = true);
  declareProperty( "TRT_ContainerName",m_trtContName="TRT_DriftCircle");
  declareProperty( "TrigFieldTool", m_trigFieldTool, "TrigMagneticFieldTool");
  declareProperty( "TrigBremTool", m_trigBremTool, "TrigInDetBremDetectionTool");
  declareProperty( "TrigTrtRoadBuilderTool", m_trtRoadBuilder, "TrigTRT_DetElementRoadTool");
}

StatusCode TrigTRT_TrackExtensionTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  sc = m_regionSelector.retrieve();
  if(sc.isFailure()) 
    {
      athenaLog<<MSG::FATAL<< "Unable to retrieve RegionSelector Service " << m_regionSelector<< endmsg;
      return sc;
    }

  if (detStore()->retrieve(m_trtId, "TRT_ID").isFailure()) {
     athenaLog << MSG::FATAL << "Could not get TRT ID helper" << endmsg;
     return StatusCode::FAILURE;
  }


  sc = m_robDataProvider.retrieve();
  if(sc.isFailure()) 
    {
      athenaLog << MSG::ERROR << "Unable to retrieve ROBDataProviderSvc" <<m_robDataProvider <<endmsg;
      return StatusCode::FAILURE;
    }

  sc = m_trtDataProvider.retrieve();
  if(sc.isFailure()) 
    {
      athenaLog << MSG::FATAL << "Unable to locate TRT data provider tool " 
		<<m_trtDataProvider<< endmsg;
      return sc;
    } 

  sc = m_trtRoadBuilder.retrieve();
  if(sc.isFailure()) 
    {
      athenaLog << MSG::FATAL << "Unable to locate TRT Road Tool" <<m_trtRoadBuilder<< endmsg;
      return sc;
    } 

  sc = m_trigFieldTool.retrieve();
  if(sc.isFailure()) 
    {
      athenaLog << MSG::FATAL << "Unable to locate TrigMagneticField Tool" <<m_trigFieldTool<<endmsg;
      return sc;
    } 

  if(m_doBremm)
    {
      sc = m_trigBremTool.retrieve();
      if(sc.isFailure()) 
	{
	  athenaLog << MSG::FATAL << "Unable to locate TrigBremTool " <<m_trigBremTool<< endmsg;
	  return sc;
	} 
    }

  sc = m_trackMaker.retrieve();
  if(sc.isFailure()) 
    {
      athenaLog << MSG::FATAL << "Unable to locate " <<m_trackMaker<< endmsg;
      return sc;
    } 

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endmsg;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
//   add some timers:
//
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("RobPreloader");
    m_timer[1] = timerSvc->addItem("SiRefit");
    m_timer[1]->propName("SiRefit.nTracks");
    m_timer[2] = timerSvc->addItem("TRT_Roads");
    m_timer[2]->propName("TRT_Roads.nTracks");
    m_timer[3] = timerSvc->addItem("TRT_Data");
    m_timer[4] = timerSvc->addItem("ForwardPDAF");
    m_timer[4]->propName("ForwardPDAF.nRoads");
    m_timer[5] = timerSvc->addItem("Smoother");
    m_timer[5]->propName("Smoother.nRoads");
    m_timer[6] = timerSvc->addItem("TrackUpdate");
    m_timer[6]->propName("TrackUpdate.nRoads");
  }
  athenaLog << MSG::INFO << "TrigTRT_TrackExtensionTool constructed "<< endmsg;
  return sc;
}

StatusCode TrigTRT_TrackExtensionTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigTRT_TrackExtensionTool::~TrigTRT_TrackExtensionTool()
{

}

void TrigTRT_TrackExtensionTool::getPhiRange(double phi,double d0,double& phiMin,double& phiMax)
{
  const double r1=600.0;
  const double r2=1200.0;

  double phi1,phi2,Delta;

  Delta=d0/r1;
  phi1=phi+Delta;
  Delta=d0/r2;
  phi2=phi+Delta;
  if(phi1>phi2)
    {
     std::swap(phi1,phi2);
    }
  phiMin=phi1-m_roiPhiSize;
  phiMax=phi2+m_roiPhiSize;
}

void TrigTRT_TrackExtensionTool::getEtaRange(double eta,double z0,double& etaMin,double& etaMax)
{
  const double r1=600.0;
  const double r2=1200.0;

  double th,Delta,cth,eta1,eta2;

  th=2.0*atan(exp(-eta));
  cth=cos(th)/sin(th);
  
  Delta=z0/r1;
  th=cth+Delta;
  th=atan(1.0/th);
  eta1=-log(sin(0.5*th)/cos(0.5*th));

  Delta=z0/r2;
  th=cth+Delta;
  th=atan(1.0/th);
  eta2=-log(sin(0.5*th)/cos(0.5*th));
  if(eta1>eta2)
    {
			std::swap(eta1,eta2);
    }
  etaMin=eta1-m_roiEtaSize;
  etaMax=eta2+m_roiEtaSize;

}
int TrigTRT_TrackExtensionTool::preloadROBs(TrigInDetTrackCollection* recoTracks)
{  
  const double maxD0=10.0;
  const double maxZ0=200.0;

  std::set<uint32_t> idSet;
  std::vector<uint32_t> idVec;
  double phi,eta,phiMax,phiMin,etaMin,etaMax;
  TrigInDetTrackCollection::iterator trIt,lastIt;
  int listSize=0;

  MsgStream athenaLog(msgSvc(), name());

  trIt=recoTracks->begin();lastIt=recoTracks->end();
  m_vROBF.clear();
  idSet.clear();

  for(; trIt !=lastIt; trIt++) 
    {
      TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>((*trIt)->param());
      if(param==NULL)
	{
	  if (m_outputLevel <= MSG::ERROR) 
	    athenaLog << MSG::ERROR << "ROB preloading failed -- TrigInDetTrack has no parameters" 
		      << endmsg;
	  continue;
	}

      if(fabs(param->pT())<m_minPt) continue;

      double tmp=param->phi0();
      if(tmp>M_PI) tmp-=2*M_PI;
      phi=tmp;
      eta=param->eta();

      if(fabs(param->a0())>maxD0)
	{
	  getPhiRange(phi,param->a0(),phiMin,phiMax);	  
	}
      else
	{
	  phiMin=phi-m_roiPhiSize;phiMax=phi+m_roiPhiSize;
	}
      if(fabs(param->z0())>maxZ0)
	{
	  etaMin=-3.0;etaMax=3.0;
	}
      else
	{
	  etaMin=eta-m_roiEtaSize;etaMax=eta+m_roiEtaSize;
	}
			TrigRoiDescriptor roi(eta, etaMin, etaMax, phi, phiMin, phiMax);
      idVec.clear();
      m_regionSelector->DetROBIDListUint(TRT,roi,idVec);
      for(std::vector<uint32_t>::iterator idIt=idVec.begin();idIt!=idVec.end();++idIt)
	{
	  idSet.insert((*idIt));
	}
    }
  idVec.clear();
  if(!idSet.empty())
    {
      idVec.clear();
      for(std::set<uint32_t>::iterator idIt=idSet.begin();idIt!=idSet.end();++idIt)
	{
	  idVec.push_back((*idIt));
	  listSize++;
	}
      if(m_outputLevel<=MSG::DEBUG)
	{	  
	  for(unsigned int i=0;i<idVec.size();i++)
	    {
	      athenaLog << MSG::DEBUG << " Requesting ROB Id : 0x " << MSG::hex << 
		idVec[i] << MSG::dec << endmsg;
	    }
	}
      m_robDataProvider->addROBData(idVec);
      m_robDataProvider->getROBData(idVec,m_vROBF);
    }
  else
    {
      if(m_outputLevel<=MSG::DEBUG)
	athenaLog << MSG::DEBUG << " ROB id vector is empty - track is beyond TRT acceptance"<<endmsg;
    }
  return listSize;
}


void TrigTRT_TrackExtensionTool::deleteNodes()
{
  std::vector<Trk::TrkBaseNode*>::iterator pnIt=m_vpTrkNodes.begin(),
    pnEnd=m_vpTrkNodes.end();
  for(;pnIt!=pnEnd;++pnIt) 
    {
      delete (*pnIt);
    }
  m_vpTrkNodes.clear();
}

void TrigTRT_TrackExtensionTool::deleteTrajectories()
{
  for(std::vector<TrigTRT_Trajectory*>::iterator ptIt=m_vpTrajectories.begin();
      ptIt!=m_vpTrajectories.end();++ptIt)
    {
      delete (*ptIt);
    }
  m_vpTrajectories.clear();
}

Trk::TrkTrackState* TrigTRT_TrackExtensionTool::extrapolate(Trk::TrkTrackState* pTS, 
                                                            Trk::TrkPlanarSurface* pSB,
                                                            Trk::TrkPlanarSurface* pSE,
                                                            double J[5][5],
                                                            double& path)
{
  const double C=0.02999975;
  const double minStep=30.0;
	  
  double sint,cost,sinf,cosf;
  double gP[3],gPi[3],lP[3],gV[3],a,b,c,s,J0[7][5],Rf[5],descr,CQ,Ac,Av,Cc;
  double P[3],M[3][3],AG[5][5],D[4],Jm[7][7],Gi[5][5],Gf[5][5],
    J1[5][7],A[5][5],gB[3],Buf[5][7],DVx,DVy,DVz;
  int i,j,m,nStep,nStepMax;
  double sl,ds;
  double V[3] = {0};

  path=0.0;

  //m_numericalJacobian(pTS,pSB,pSE,J);

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

  m_trigFieldTool->getMagneticField(gP,gB);
 
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
      m_trigFieldTool->getMagneticField(gP,gB);
      nStep--;
    }
  pSE->m_transformPointToLocal(gP,lP);

  Rf[0]=lP[0];Rf[1]=lP[1];
  Rf[2]=atan2(V[1],V[0]);

  if(fabs(V[2])>1.0)
    return NULL;

  Rf[3]=acos(V[2]);
  Rf[4]=pTS->m_getTrackState(4);

  gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;

  for(i=0;i<4;i++) D[i]=pSE->m_getPar(i);
  for(i=0;i<3;i++) gP[i]=gPi[i];

  double mP[3];

  for(i=0;i<3;i++) 
    {
      mP[i]=0.5*(gP[i]+P[i]);
    }

  m_trigFieldTool->getMagneticField(mP,gB);

  c=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
  b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
  a=CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+
	gB[1]*(D[2]*gV[0]-D[0]*gV[2])+
	gB[2]*(D[0]*gV[1]-D[1]*gV[0]));


  if(fabs(b)<0.001) {
    // printf("Extrapolation failure: track is parallel to the target surface\n");
    return NULL;
  }

  ratio = 4*a*c/(b*b);
  if(fabs(ratio)>0.1) 
    useExpansion = false;
  else useExpansion = true;

  if(useExpansion) {
    s=-c/b;
    s=s*(1-a*sl/b);
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
  matrixInversion5x5(Gi);
  for(i=0;i<5;i++) for(j=0;j<5;j++)
    {
      A[i][j]=0.0;
      for(m=0;m<5;m++) A[i][j]+=AG[m][i]*Gi[m][j];
    }
  pTE->m_setPreviousState(pTS);
  pTE->m_setSmootherGain(A);

  return pTE;
}


void TrigTRT_TrackExtensionTool::matrixInversion5x5(double a[5][5])
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

const std::vector<int>* TrigTRT_TrackExtensionTool::fillTRT_DataErrors()
{
  return &m_trtDataErrors;
}


StatusCode TrigTRT_TrackExtensionTool::propagate(TrigInDetTrackCollection* recoTracks )
{

  //  StatusCode scData(StatusCode::SUCCESS);
  int nHits;
  double Pt, Phi0, D0, Z0, Eta, errD0, errZ0, errEta, errPhi0, errPt;
  double CV[5][5],a,b,c,tmp,Theta;
  std::vector<double>* pCov;
  TrigInDetTrackCollection::iterator trIt,lastIt;

  Trk::TrkTrackState *pTS, *pInitState;

  m_vpTrajectories.clear();
  m_vpTrkNodes.clear();

  double Rk[5],J[5][5],path;
  int nFittedTracks=0,nTRT_Roads=0;

  MsgStream athenaLog(msgSvc(), name());
  m_outputLevel = msgSvc()->outputLevel( name() );

  m_trtDataProviderTime = 0.0;//3
  m_trtRobPreloaderTime = 0.0;//0
  m_trtRoadBuilderTime = 0.0;//1+2
  m_trtPDAF_TrackingTime = 0.0;//4+5+6


  if(m_timers) m_timer[0]->start();
  if(preloadROBs(recoTracks)==0)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "skipping TRT track extension ..." << endmsg;
      if(m_timers) 
	{
	  m_timer[0]->stop();
	  m_trtRobPreloaderTime=m_timer[0]->elapsed();
	}
      return StatusCode::SUCCESS;
    }
  if(m_timers) 
    {
      m_timer[0]->stop();
      m_trtRobPreloaderTime=m_timer[0]->elapsed();
    }
  if(m_timers)
    {
      m_timer[1]->start();
      m_timer[1]->pause();      
    }
  trIt = recoTracks->begin();lastIt = recoTracks->end();
  for(; trIt !=lastIt; trIt++) 
    {
      TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>((*trIt)->param());
      if(param==NULL)
	{
	  if (m_outputLevel <= MSG::ERROR) 
	    athenaLog << MSG::WARNING << "TRT extension failed -- TrigInDetTrack has no parameters" 
		      << endmsg;
	  continue;
	}
      if((*trIt)->siSpacePoints()->size()==0) 
	{
	  if (m_outputLevel <= MSG::ERROR) 
	    athenaLog << MSG::WARNING << "Fit Failed -- TrigInDetTrack has no hits" 
		      << endmsg;
	  continue;
	}

      if(fabs(param->pT())<m_minPt)
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << "TrigInDetTrack pT < "<<m_minPt<<" MeV - skipping TRT extension" << endmsg;

	  (*trIt)->StrawHits(-2);
	  (*trIt)->Straw(-2);
	  (*trIt)->TRHits(-2);
	  (*trIt)->StrawTime(-2);

	  continue;
	}

      // 1. Create initial track state:

      nFittedTracks++;
      if(m_timers) m_timer[1]->resume();

      Rk[0]=param->a0();
      Rk[1]=param->z0();
      tmp=param->phi0();
      if(tmp>M_PI) tmp-=2*M_PI;
      Rk[2]=tmp;
      Theta=2.0*atan(exp(-param->eta()));
      Rk[3]=Theta;
      Rk[4]=sin(Theta)/param->pT();
      pTS = new Trk::TrkTrackState(Rk);

      double Gk[5][5];
      memset(&Gk[0][0],0,sizeof(Gk));
      Gk[0][0]=100.0;Gk[1][1]=100.0;Gk[2][2]=0.01;Gk[3][3]=0.01;Gk[4][4]=1e-7;
      pTS->m_setTrackCovariance(Gk);


      if(m_doMultScatt)  
	pTS->m_setScatteringMode(1);

      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Initial params: locT="<<Rk[0]<<" locL="<<Rk[1]<<" phi="<<Rk[2]
		  <<" theta="<<Rk[3]<<" Q="<<Rk[4]<<endmsg;

      if(m_doBremm)
	{
	  memset(&J[0][0],0,sizeof(J));
	  J[0][0]=1.0;
	  J[1][1]=1.0;
	  J[2][2]=1e-4;
	  J[3][3]=1e-4;
	  J[4][4]=1e-8;
	  pTS->m_setTrackCovariance(J);
	}
 
      pInitState=pTS;

      // 2. Create filtering nodes

      m_vpTrkNodes.clear();

      TrigTRT_Trajectory* pTRAJ=new TrigTRT_Trajectory((*trIt));

      bool trackResult = m_trackMaker->createDkfTrack(*(*trIt)->siSpacePoints(),m_vpTrkNodes,m_DChi2);

      if(!trackResult) 
	{
	  delete pTRAJ;
	  continue;
	}
      nHits=m_vpTrkNodes.size();      

      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << nHits<<" filtering nodes created"<<endmsg;

      // 3. Main algorithm: filter and smoother (Rauch-Tung-Striebel)
      
      std::vector<Trk::TrkBaseNode*>::iterator pnIt(m_vpTrkNodes.begin()),
	pnEnd(m_vpTrkNodes.end());
      for(std::vector<Trk::TrkBaseNode*>::iterator it=m_vpTrkNodes.begin();it!=m_vpTrkNodes.end();++it)
	pTRAJ->m_vpTrkSurfaces.push_back((*it)->m_getSurface());
      bool OK=true;
      Trk::TrkPlanarSurface *pSB,*pSE;
      if(m_doBremm)
	{
          pTS=new Trk::TrkTrackState(pInitState); 
	  pSB=NULL;
	  m_trigBremTool->reset();
	  for(;pnIt!=pnEnd;++pnIt)
	    {
	      pSE=(*pnIt)->m_getSurface();
	      Trk::TrkTrackState* pNS=extrapolate(pTS,pSB,pSE,J,path);
	  
	      pSB=pSE;
	      if(pNS!=NULL)
		{
		  (*pnIt)->m_validateMeasurement(pNS);
		  (*pnIt)->m_updateTrackState(pNS);
		  if((*pnIt)->m_getNodeType()!=3)
		    {
		      m_trigBremTool->addNewPoint(pNS,(*pnIt),pSB,J,path);
		    }
		  delete pTS;
		  pTS=pNS;
		}
	      else
		{
		  delete pTS;OK=false;break;
		}
	    }
	  if(OK)
	    {
	      int Sign=(pTS->m_getTrackState(4)<0.0)?-1:1;
	      if (m_outputLevel <= MSG::DEBUG) 
		 m_trigBremTool->report(2);
	      bool ideResult=m_trigBremTool->solve(Sign);	
	      if(ideResult) m_trigBremTool->modifySurfaces(1);
	      delete pTS;
	    }
	}
      pTS=new Trk::TrkTrackState(pInitState);
      pSB=NULL;OK=true;
      pTRAJ->addTrackState(pTS);pnIt=m_vpTrkNodes.begin();
      for(;pnIt!=pnEnd;++pnIt)
	{
	  pSE=(*pnIt)->m_getSurface();
	  // pSE->m_report();
	  Trk::TrkTrackState* pNS=extrapolate(pTS,pSB,pSE,J,path);
	  pSB=pSE;
	  if(pNS!=NULL)
	    {
	      pTRAJ->addTrackState(pNS);
	      (*pnIt)->m_validateMeasurement(pNS);
	      (*pnIt)->m_updateTrackState(pNS);
	      Pt=sin(pNS->m_getTrackState(3))/pNS->m_getTrackState(4);
	      if(fabs(Pt)<m_minPt)
		{
		  if (m_outputLevel <= MSG::DEBUG) 
		    athenaLog << MSG::DEBUG << "Estimated Pt is too low "<<Pt<<" - skipping TRT extension"<< endmsg;
		  (*trIt)->StrawHits(-2);
		  (*trIt)->Straw(-2);
		  (*trIt)->TRHits(-2);
		  (*trIt)->StrawTime(-2);
		  OK=false;break;
		}

	      //	      pNS->m_report();
	      pTS=pNS;
	    }
	  else
	    {
	      OK=false;break;
	    }
	}
      pTRAJ->setStatus(OK);
      delete pInitState;
      deleteNodes();
      m_vpTrajectories.push_back(pTRAJ);
      if(m_timers) m_timer[1]->pause();
    }  
  if(m_timers) 
    {
      m_timer[1]->propVal(nFittedTracks);
      m_timer[1]->stop();
    }

  if(m_vpTrajectories.size()==0) return StatusCode::SUCCESS; 

  // TRT tracking sequence:
  
  // 1. Collect list of DetElIDs 

  std::list<int> deIdList;
  std::list<TrigTRT_DetElement*> pDEList;
  pDEList.clear();
  deIdList.clear();

  if(m_timers) m_timer[2]->start();

  for(std::vector<TrigTRT_Trajectory*>::iterator ptrIt=m_vpTrajectories.begin();
      ptrIt!=m_vpTrajectories.end();++ptrIt)
    {
      if(!(*ptrIt)->isValid()) continue;
      Trk::TrkTrackState* pTS=(*((*ptrIt)->getTrackStates()->rbegin()));
      if(m_outputLevel <= MSG::VERBOSE)
	{
	  athenaLog<<MSG::VERBOSE<<"Starting Track state for TRT road"<<endmsg;
	  pTS->m_report();
	}
      TrigTRT_DetElementRoad* pR=m_trtRoadBuilder->buildTRT_Road(pTS);
      (*ptrIt)->addRoad(pR);
      if(pR!=NULL) 
	{
	  pR->collectDetectorElements(&pDEList);
	  nTRT_Roads++;
	}
      else (*ptrIt)->setStatus(false);
    }
  if(m_outputLevel <= MSG::DEBUG)
    athenaLog<<MSG::DEBUG<<"Total "<<nTRT_Roads<<" roads created with "<<pDEList.size()<<
      " DEs"<<endmsg;

  pDEList.sort();
  pDEList.unique();

  std::vector<IdentifierHash> vIDs;
  vIDs.clear();

  for(std::list<TrigTRT_DetElement*>::iterator ldeIt=pDEList.begin();
      ldeIt!=pDEList.end();++ldeIt)
    {
      vIDs.push_back((*ldeIt)->getHashId());
    }

  if(m_timers) 
    {
      m_timer[2]->propVal(nTRT_Roads);
      m_timer[2]->stop();
      m_trtRoadBuilderTime=m_timer[2]->elapsed()+m_timer[1]->elapsed();
      m_timer[3]->start();
    }

  // 2. Request data

  // here will be the call for the dc provider tool m_getDC(vIDs)
  m_trtDataErrors.clear();
  StatusCode scData = m_trtDataProvider->fillCollections(vIDs,m_vROBF);
  if(scData.isRecoverable())
    {
      if(m_outputLevel <= MSG::DEBUG)
	athenaLog<<MSG::DEBUG<<"Recoverable errors during TRT BS conversion  "<<endmsg;
      const std::vector<int>* errVect = m_trtDataProvider->fillTRT_DataErrors();
      std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_trtDataErrors));

    }
  else if(scData.isFailure())
    {
      athenaLog << MSG::WARNING << " TRT Data provider failed" << endmsg;
      deleteTrajectories();
      if(m_timers) m_timer[3]->stop();
      return scData;
    }

  // 3. Retrieve IDC

  const InDet::TRT_DriftCircleContainer* trtContainer;
  StatusCode sc=evtStore()->retrieve(trtContainer,m_trtDataProvider->trtContainerName());
  if(sc.isFailure())
    {
      athenaLog<<MSG::WARNING<<"TRT DriftCircle container is not found: name "
	       <<m_trtDataProvider->trtContainerName()<<
	" pointer="<<trtContainer<< endmsg;
      return sc;
    }
  if(m_outputLevel <= MSG::DEBUG)
    athenaLog<<MSG::DEBUG<<"TRT DriftCircle container retrieved"<<endmsg;
  // 4. Add pointers to DC collections to DEs from the list
  for(std::list<TrigTRT_DetElement*>::iterator ldeIt=pDEList.begin();
      ldeIt!=pDEList.end();++ldeIt)
    {
      unsigned int id=(unsigned int)((*ldeIt)->getHashId());
      InDet::TRT_DriftCircleContainer::const_iterator collIt(trtContainer->indexFind(id));
      if(collIt==trtContainer->end()) 
	{
	  (*ldeIt)->addDC_Collection(NULL);
	  continue;
	}
      const InDet::TRT_DriftCircleCollection* coll=&(**collIt);
      if(coll->begin()==coll->end()) 
	{
	  (*ldeIt)->addDC_Collection(NULL);
	  continue;
	}
      (*ldeIt)->addDC_Collection(coll);
      if(m_outputLevel <= MSG::VERBOSE)
	athenaLog<<MSG::VERBOSE<<"Collection with size="<<coll->size()<<" attached to element "<<id<<
	  endmsg;
    }

  if(m_timers) 
    {
      m_timer[3]->stop();
      m_trtDataProviderTime=m_timer[3]->elapsed();
    }
  nTRT_Roads=0;

  // 5. Loop over trajectories:
  if(m_timers)
    {
      m_timer[4]->start();
      m_timer[4]->pause();
      m_timer[5]->start();
      m_timer[5]->pause();
    }

  for(std::vector<TrigTRT_Trajectory*>::iterator ptrIt=m_vpTrajectories.begin();
      ptrIt!=m_vpTrajectories.end();++ptrIt)
    {
      if(!(*ptrIt)->isValid()) continue;
      if(m_outputLevel <= MSG::VERBOSE) 
	{
	  athenaLog<<MSG::VERBOSE<<"TRT Road:"<<endmsg;
	  (*ptrIt)->getRoad()->report();
	}
      nTRT_Roads++;
      if(m_timers) m_timer[4]->resume();
      
      Trk::TrkTrackState* pTS=(*((*ptrIt)->getTrackStates()->rbegin()));
      TrigTRT_DetElementRoad* pR=(*ptrIt)->getRoad();
      
      Trk::TrkPlanarSurface *pSB=pTS->m_getSurface(),*pSE=NULL;
      
      TrigTRT_Info* pTI=new TrigTRT_Info();
      (*ptrIt)->addTRT_SummaryInfo(pTI);
      TrigTRT_DetElementPoint* pRP=NULL;
      std::vector<TrigTRT_DetElementPoint*>::iterator ppIt;
      bool passedOK=true;
      bool firstSurf=true;

      double pT=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);

      if(fabs(pT)<m_minPt)
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << "pT < "<<m_minPt<<" MeV - skipping TRT extension" << endmsg;
	  passedOK=false;
	}
      else
	{
	  for(ppIt=pR->roadPoints()->begin();ppIt!=pR->roadPoints()->end();++ppIt)
	    {
	      if((*ppIt)->hasHits())
		{
		  pRP=(*ppIt);
		  pSE=pRP->createSurface();
		  Trk::TrkTrackState* pNS=extrapolate(pTS,pSB,pSE,J,path);
		  
		  if(pNS!=NULL)
		    {
		      if(m_outputLevel <= MSG::VERBOSE)
			pNS->m_report();
		      (*ptrIt)->addTrackState(pNS);
		      pRP->updateTrackState(pNS,pTI);
		      if(m_outputLevel <= MSG::VERBOSE) 
			{
			  athenaLog<<MSG::VERBOSE<<"Updated tracks state:"<<endmsg;
			  pNS->m_report();
			}
		      pTS=pNS;
		    }
		  else
		    {
		      athenaLog << MSG::DEBUG << "Extrapolation failed - TRT extension skipped "<<endmsg;
		      passedOK=false;
		    }
		  if(!firstSurf) delete pSB;
                  firstSurf=false;
		  pSB=pSE;
		}
	      if(!passedOK) break; 
	    }
	  delete pSE;
	}
      //    c) run smoother
      if(m_timers) 
	{
	  m_timer[4]->pause(); 
	  m_timer[5]->resume();
	}
      if(passedOK)
	{
	  (*ptrIt)->smoothTrajectory();
	}
      else (*ptrIt)->setStatus(false);
      if(m_timers) m_timer[5]->pause(); 
    }
  if(m_timers) 
    {
      m_timer[4]->propVal(nTRT_Roads);
      m_timer[5]->propVal(nTRT_Roads);
      m_timer[4]->stop();
      m_timer[5]->stop();
    }

  if(m_timers) m_timer[6]->start();

  for(std::vector<TrigTRT_Trajectory*>::iterator ptrIt=m_vpTrajectories.begin();
      ptrIt!=m_vpTrajectories.end();++ptrIt)
    {
      if(!(*ptrIt)->isValid()) continue;
      pTS=(*((*ptrIt)->getTrackStates()->begin()));
      Pt=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
      Phi0 = pTS->m_getTrackState(2);
      if(Phi0>M_PI) Phi0-=2*M_PI;
      if(Phi0<-M_PI) Phi0+=2*M_PI;
      Eta = -log(sin(0.5*pTS->m_getTrackState(3))/cos(0.5*pTS->m_getTrackState(3)));
      Z0 = pTS->m_getTrackState(1);
      D0 = pTS->m_getTrackState(0);
	  
      errD0 = sqrt(pTS->m_getTrackCovariance(0,0));
      errZ0 = sqrt(pTS->m_getTrackCovariance(1,1));
      errPhi0 = sqrt(pTS->m_getTrackCovariance(2,2));
      errEta = sqrt(pTS->m_getTrackCovariance(3,3))/fabs(sin(pTS->m_getTrackState(3)));
      b=cos(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
      c=-Pt/pTS->m_getTrackState(4);
      a=-1.0/sin(pTS->m_getTrackState(3));
      errPt = sqrt(b*b*(pTS->m_getTrackCovariance(3,3))+c*c*(pTS->m_getTrackCovariance(4,4))+
		   2.0*b*c*(pTS->m_getTrackCovariance(3,4)));
      
      pCov=new std::vector<double>;
	  
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
      
      for(int i=0;i<5;i++)
	for(int j=i;j<5;j++) pCov->push_back(CV[i][j]);
      const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta, Pt,
								    errD0,errPhi0,errZ0,
								    errEta,errPt,pCov);
      
            
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "Updated parameters: d0="<<D0<<" phi0="<<Phi0<<" z0="<<Z0	
		  <<" eta0="<<Eta<<" pt="<<Pt << endmsg;

      TrigInDetTrack* pTIDT=(*ptrIt)->getTrigInDetTrack();

      TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>(pTIDT->param());
      delete param;
      pTIDT->param(tidtfp);

      int Nhits=0,Nstraw=0,NTR=0,NTime=0;
      Nhits   = (*ptrIt)->getNumberOfTRT_Hits();
      Nstraw  = (*ptrIt)->getNumberOfCrossedTRT_Straws();
      NTR     = (*ptrIt)->getNumberOfHighThresholdTRT_Hits();
      NTime   = (*ptrIt)->getNumberOfDriftTimeTRT_Hits();
      pTIDT->StrawHits(Nhits);
      pTIDT->Straw(Nstraw);
      pTIDT->TRHits(NTR);
      pTIDT->StrawTime(NTime);
      std::vector<const InDet::TRT_DriftCircle*>* pDC=new std::vector<const InDet::TRT_DriftCircle*>;
      for(std::vector<const InDet::TRT_DriftCircle*>::iterator dcIt=
	    (*ptrIt)->getTRT_SummaryInfo()->getTRT_Hits().begin();
	  dcIt!=(*ptrIt)->getTRT_SummaryInfo()->getTRT_Hits().end();++dcIt)
	{
	  pDC->push_back((*dcIt));
	}
      pTIDT->trtDriftCircles(pDC);
      if(m_outputLevel <= MSG::DEBUG) 
	{
	  athenaLog<<MSG::DEBUG<<"N TR hits="<<NTR<<" N TRT hits="<<Nhits<<" N holes="<<
	    (*ptrIt)->getNumberOfMissedDetection()<<endmsg;
	  athenaLog<<MSG::DEBUG<<(*ptrIt)->getTRT_SummaryInfo()->getTRT_Hits().size()
		   <<" TRT hits are associated with the track"<<endmsg;
	}
    }
  if(m_timers) 
    {
      m_timer[6]->stop();
      m_trtPDAF_TrackingTime=m_timer[6]->elapsed()+m_timer[5]->elapsed()+m_timer[4]->elapsed();
    }
  deleteTrajectories();

  return scData;
}

