/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigTRT_CombinedExtensionTool tool
// -------------------------------
// ATLAS Collaboration
//
// 31.05.2010 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <algorithm>

#include "GaudiKernel/ToolFactory.h"
//#include "RegionSelector/RegSelSvc.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrigInDetToolInterfaces/ITrigTRT_TrackExtensionTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionGeometry.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoadTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoad.h"
#include "TrigTRT_TrackExtensionTool/TrigMagneticFieldTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_Trajectory.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_CombinedExtensionTool.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 

TrigTRT_CombinedExtensionTool::TrigTRT_CombinedExtensionTool(const std::string& t, 
				 const std::string& n, const IInterface*  p ): 
  AthAlgTool(t,n,p), 
  m_recalibrate(false),
  m_momentumThreshold(10000.0),
  m_regionSelector("RegSelSvc",this->name()),
  m_trtDataProvider("TrigTRT_DriftCircleProviderTool"), 
  m_robDataProvider("ROBDataProviderSvc",this->name()),
  m_trtRoadBuilder("TrigTRT_DetElementRoadTool",this),
  m_trigFieldTool("TrigMagneticFieldTool"),
  m_trackMaker("TrigDkfTrackMakerTool"),
  m_ROTcreator("Trk::RIO_OnTrackCreator/InDetTrigBroadInDetRotCreator"),
  m_extrapolator("Trk::Extrapolator/InDetTrigExtrapolator"),
  m_lowPtFitter("TrigL2LowPtTrackFitter"),
  m_fastExtrapolator("TrigL2FastExtrapolationTool")
{
  declareInterface< ITrigTRT_TrackExtensionTool >( this );
  
  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremm=false);  
  declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
  declareProperty( "RoiPhiHalfWidth", m_roiPhiSize = 0.2);
  declareProperty( "RoiEtaHalfWidth", m_roiEtaSize = 0.1);
  declareProperty( "TRT_ContainerName",m_trtContName="TRT_DriftCircle");
  declareProperty( "TrigFieldTool", m_trigFieldTool, "TrigMagneticFieldTool");
  declareProperty( "TrigTrtRoadBuilderTool", m_trtRoadBuilder, "TrigTRT_DetElementRoadTool");
  declareProperty( "ROT_CreatorTool", m_ROTcreator,"Trk::RIO_OnTrackCreator/InDetTrigBroadInDetRotCreator");
  declareProperty( "TrackExtrapolatorTool",m_extrapolator, "Trk::Extrapolator/InDetTrigExtrapolator");
  declareProperty( "LowPtTrackFitter", m_lowPtFitter, "TrigL2LowPtTrackFitter");
  declareProperty( "fastExtrapolator", m_fastExtrapolator, "TrigL2FastExtrapolationTool"); 
  declareProperty( "useROTs",m_recalibrate = false);
  declareProperty( "MomentumThreshold",m_momentumThreshold=10000.0);  
}

StatusCode TrigTRT_CombinedExtensionTool::initialize()
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

  sc = m_trackMaker.retrieve();
  if(sc.isFailure()) 
    {
      athenaLog << MSG::FATAL << "Unable to locate " <<m_trackMaker<< endmsg;
      return sc;
    } 
  sc=m_ROTcreator.retrieve();
  if(sc.isFailure())
    {
      athenaLog << MSG::ERROR<<"Could not retrieve "<<m_ROTcreator<<endmsg;
      return sc;
    }
  else
    athenaLog << MSG::INFO<<"Successfully retrieved "<<m_extrapolator<<endmsg;

  sc = m_extrapolator.retrieve();
  if( sc.isFailure() )
  {
    athenaLog << MSG::ERROR<<"Could not retrieve "<<m_extrapolator<<endmsg;
    return sc;
  }
  else
    athenaLog << MSG::INFO<<"Successfully retrieved "<<m_extrapolator<<endmsg;

  sc=m_lowPtFitter.retrieve();
  if(sc.isFailure())
    {
      athenaLog << MSG::ERROR<<"Could not retrieve "<<m_lowPtFitter<<endmsg;
      return sc;
    }

  sc=m_fastExtrapolator.retrieve();
  if(sc.isFailure())
    {
      athenaLog << MSG::ERROR<<"Could not retrieve extrapolation tool"<<m_fastExtrapolator<<endmsg;
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
    m_timer[5]->propName("BackwardF.nRoads");
    m_timer[6] = timerSvc->addItem("TrackUpdate");
    m_timer[6]->propName("TrackUpdate.nRoads");
  }
  athenaLog << MSG::INFO << "TrigTRT_CombinedExtensionTool constructed "<< endmsg;
  return sc;
}

StatusCode TrigTRT_CombinedExtensionTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigTRT_CombinedExtensionTool::~TrigTRT_CombinedExtensionTool()
{

}

void TrigTRT_CombinedExtensionTool::getPhiRange(double phi,double d0,double& phiMin,double& phiMax)
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
      std::swap(phi1, phi2);
    }
  phiMin=phi1-m_roiPhiSize;
  phiMax=phi2+m_roiPhiSize;
}

void TrigTRT_CombinedExtensionTool::getEtaRange(double eta,double z0,double& etaMin,double& etaMax)
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
      std::swap(eta1, eta2);
    }
  etaMin=eta1-m_roiEtaSize;
  etaMax=eta2+m_roiEtaSize;

}
int TrigTRT_CombinedExtensionTool::preloadROBs(TrigInDetTrackCollection* recoTracks)
{  

  std::list<uint32_t> idList;
  std::vector<uint32_t> idVec;
  TrigInDetTrackCollection::iterator trIt,lastIt;
  int listSize=0;

  MsgStream athenaLog(msgSvc(), name());

  trIt=recoTracks->begin();lastIt=recoTracks->end();
  m_vROBF.clear();

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
      m_regionSelector->DetROBIDListUint(TRT,idVec);
      for(std::vector<uint32_t>::iterator idIt=idVec.begin();idIt!=idVec.end();++idIt)
	{
	  idList.push_back((*idIt));
	}
    }
  idVec.clear();
  if(!idList.empty())
    {
      idList.sort();
      idList.unique();
      idVec.clear();
      for(std::list<uint32_t>::iterator idIt=idList.begin();idIt!=idList.end();++idIt)
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

void TrigTRT_CombinedExtensionTool::deleteTrajectories()
{
  for(std::vector<TrigTRT_Trajectory*>::iterator ptIt=m_vpTrajectories.begin();
      ptIt!=m_vpTrajectories.end();++ptIt)
    {
      delete (*ptIt);
    }
  m_vpTrajectories.clear();
}

TrigTRT_Trajectory* TrigTRT_CombinedExtensionTool::createTRT_Trajectory(TrigInDetTrack* pTrack)
{
  TrigTRT_Trajectory* pTRAJ=NULL;
  Trk::TrkTrackState *pInitState;
  MsgStream athenaLog(msgSvc(), name());
  double Rk[5],Gk[5][5],tmp;

  TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>(pTrack->param());
  if(param==NULL)
    {
      if (m_outputLevel <= MSG::ERROR) 
	athenaLog << MSG::WARNING << "TRT extension failed -- TrigInDetTrack has no parameters" << endmsg;
      return pTRAJ;
    }
  if(pTrack->siSpacePoints()->size()==0) 
    {
      if (m_outputLevel <= MSG::ERROR) 
	athenaLog << MSG::WARNING << "TRT extension failed -- TrigInDetTrack has no hits" << endmsg;
      return pTRAJ;
    }
  if(fabs(param->pT())<500.0)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "TrigInDetTrack pT < 500 MeV - skipping TRT extension" << endmsg;
      return pTRAJ;
    }
  
  pTRAJ=new TrigTRT_Trajectory(pTrack);
  bool trackResult = m_trackMaker->createDkfTrack(*(pTrack->siSpacePoints()),
						  *(pTRAJ->getFilteringNodes()),m_DChi2);
    
  if(!trackResult) 
    {
      delete pTRAJ;pTRAJ=NULL; 
      return pTRAJ;
    }

  Rk[0]=param->a0();
  Rk[1]=param->z0();
  tmp=param->phi0();
  if(tmp>M_PI) tmp-=2*M_PI;
  Rk[2]=tmp;
  double Theta=2.0*atan(exp(-param->eta()));
  Rk[3]=Theta;
  Rk[4]=sin(Theta)/param->pT();
  pInitState = new Trk::TrkTrackState(Rk);

  memset(&Gk[0][0],0,sizeof(Gk));
  if(m_doBremm)
    {
      Gk[0][0]=1.0;Gk[1][1]=1.0;Gk[2][2]=1e-4;Gk[3][3]=1e-4;Gk[4][4]=1e-8;
    }
  else
    {
      Gk[0][0]=100.0;Gk[1][1]=100.0;Gk[2][2]=0.01;Gk[3][3]=0.01;Gk[4][4]=1e-7;
    }
  pInitState->m_setTrackCovariance(Gk);

  if(m_doMultScatt) 
    pInitState->m_setScatteringMode(1);

  if (m_outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::DEBUG << "Initial params: locT="<<Rk[0]<<" locL="<<Rk[1]<<" phi="<<Rk[2]
	      <<" theta="<<Rk[3]<<" Q="<<Rk[4]<<endmsg;

  pTRAJ->setStartingTrackState(pInitState);
  for(std::vector<Trk::TrkBaseNode*>::iterator it=pTRAJ->getFilteringNodes()->begin();
      it!=pTRAJ->getFilteringNodes()->end();++it)
    {
      pTRAJ->m_vpTrkSurfaces.push_back((*it)->m_getSurface());
    }
    
  return pTRAJ;
}

bool TrigTRT_CombinedExtensionTool::refitTRT_Trajectory(TrigTRT_Trajectory* pTRAJ)
{
  MsgStream athenaLog(msgSvc(), name());

  Trk::TrkTrackState* pTS=pTRAJ->getStartingTrackState();

  double mom = 1.0/pTS->m_getTrackState(4);

  std::vector<Trk::TrkBaseNode*>& nodes = *(pTRAJ->getFilteringNodes());
  if(fabs(mom)<m_momentumThreshold)
    {     
      pTS=m_lowPtFitter->fit(pTS,nodes,false);  
      pTRAJ->setStartingTrackState(pTS); 
      pTRAJ->setFast(false);
    }
  else
    {
      pTRAJ->setFast(true);
      std::vector<Trk::TrkBaseNode*>::iterator pnIt(nodes.begin()),pnEnd(nodes.end());
      Trk::TrkPlanarSurface *pSB,*pSE;
      pSB=NULL;
      pTRAJ->addTrackState(pTS);pTRAJ->setStartingTrackState(NULL);
      bool OK=true;
      for(;pnIt!=pnEnd;++pnIt)
	{
	  pSE=(*pnIt)->m_getSurface();
	  Trk::TrkTrackState* pNS=m_fastExtrapolator->extrapolate(pTS,pSB,pSE,true);
	  pSB=pSE;
	  if(pNS!=NULL)
	    {
	      pTRAJ->addTrackState(pNS);
	      (*pnIt)->m_validateMeasurement(pNS);
	      (*pnIt)->m_updateTrackState(pNS);
	      double Pt=sin(pNS->m_getTrackState(3))/pNS->m_getTrackState(4);
	      if(fabs(Pt)<500.0)
		{
		  if (m_outputLevel <= MSG::DEBUG) 
		    athenaLog << MSG::DEBUG << "Estimated Pt is too low "<<Pt<<" - skipping TRT extension"<< endmsg;
		  OK=false;break;
		}
	      pTS=pNS;
	    }
	  else
	    {
	      OK=false;break;
	    }
	}
      if(!OK) pTS=NULL;
    }
  return (pTS!=NULL);
}

bool TrigTRT_CombinedExtensionTool::runPDAF_Tracking(TrigTRT_Trajectory* pTRAJ)
{
  bool OK=true;

  MsgStream athenaLog(msgSvc(), name());

  std::vector<std::pair<Trk::TrkTrackState*,TrigTRT_DetElementPoint*> > vRT;
   
  Trk::TrkTrackState* pTS=pTRAJ->getStartingTrackState();
  TrigTRT_DetElementRoad* pR=pTRAJ->getRoad(); 
   
  Trk::TrkPlanarSurface *pSB=pTS->m_getSurface();

  TrigTRT_Info* pTI=new TrigTRT_Info();
  pTRAJ->addTRT_SummaryInfo(pTI);
  
  std::vector<TrigTRT_DetElementPoint*>::iterator ppIt(pR->roadPoints()->begin());

  Trk::TrkPlanarSurface* pFirstSurface=NULL;
  
  double pT=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
    
  if(fabs(pT)<500.0)
    {
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "pT < 500 MeV - skipping TRT extension" << endmsg;
      OK=false;
      return OK;
    }
  
  bool first_step=true;
  vRT.clear();
  for(;ppIt!=pR->roadPoints()->end();++ppIt)
    {
      if(!(*ppIt)->hasHits()) continue;
      TrigTRT_DetElementPoint* pRP=(*ppIt);
      Trk::TrkPlanarSurface* pSE=pRP->createSurface();
      pTRAJ->addSurface(pSE);
      Trk::TrkTrackState* pNS=NULL;
      if(!first_step) pNS=m_fastExtrapolator->extrapolate(pTS,pSB,pSE,false);
      else
	{
	  first_step=false;
	  pNS=extrapolateOffline(pTS,pSB,pSE,1,true);
	  pFirstSurface=pSE;
	}
      pSB=pSE;
      if(pNS!=NULL)
	{
	  if(m_outputLevel <= MSG::VERBOSE)
	    pNS->m_report();
	  pTRAJ->addTrackState(pNS);	  
	  std::pair<Trk::TrkTrackState*,TrigTRT_DetElementPoint*> trackPair(pNS,pRP);

	  vRT.push_back(trackPair);
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
	  if(m_outputLevel <= MSG::DEBUG) 
	    {
	      athenaLog << MSG::DEBUG << "Extrapolation failed - TRT extension skipped "<<endmsg;
	    }
	  OK=false;break;
	}
    }

  if(!OK) 
    {
      delete pTRAJ->getStartingTrackState();pTRAJ->setStartingTrackState(NULL); 
      return OK;
    }

  int Nhits   = pTRAJ->getNumberOfTRT_Hits();
  int Nstraw  = pTRAJ->getNumberOfCrossedTRT_Straws();
  //int NTR     = pTRAJ->getNumberOfHighThresholdTRT_Hits();
  //int NTime   = pTRAJ->getNumberOfDriftTimeTRT_Hits();

  if((Nstraw<5)||(Nhits<5))
  {
    delete pTRAJ->getStartingTrackState();pTRAJ->setStartingTrackState(NULL); 
    return false;
  }
  
  pTRAJ->smoothTrajectory();

  pTS=(*(pTRAJ->getTrackStates()->rbegin()));

  double Gk[5][5],Rk[5];
  
  for(int i=0;i<5;i++) Rk[i]=pTS->m_getTrackState(i);
  memset(Gk,0,sizeof(Gk));
  Gk[0][0]=100.0;Gk[1][1]=100.0;Gk[2][2]=0.01;Gk[3][3]=0.0001;Gk[4][4]=1e-7;

  Trk::TrkTrackState* pUS = new Trk::TrkTrackState(Rk);
  pUS->m_setTrackCovariance(Gk);
  pUS->m_attachToSurface(pTS->m_getSurface());
  if(m_doMultScatt) 
    pUS->m_setScatteringMode(1);

  std::vector<std::pair<Trk::TrkTrackState*,TrigTRT_DetElementPoint*> >::reverse_iterator rtIt(vRT.rbegin()),
    rtItEnd(vRT.rend());

  for(;rtIt!=rtItEnd;++rtIt)
    {
      pTS=(*rtIt).first;
      TrigTRT_DetElementPoint* pRP=(*rtIt).second;
      Trk::TrkPlanarSurface *pSB, *pSE;
      pSB=pUS->m_getSurface();
      pSE=pTS->m_getSurface();
      Trk::TrkTrackState* pNS=NULL;
      if(pSB!=pSE) pNS=m_fastExtrapolator->extrapolate(pUS,pSB,pSE,false);	
      else 
	{
	  pNS=new Trk::TrkTrackState(pUS);
	  pNS->m_attachToSurface(pTS->m_getSurface());
	}
      delete pUS;
      if(pNS!=NULL)
	{
	  pRP->updateTrackState(pTS,NULL,pNS);
	  pUS=pNS;
	}
      else
	{
	  OK=false;break;
	}
    }
  if(!OK)
    {
      delete pTRAJ->getStartingTrackState();pTRAJ->setStartingTrackState(NULL); 
      return false;
    }
  pUS->m_attachToSurface(pFirstSurface);
  delete pTRAJ->getStartingTrackState();
  pTRAJ->setStartingTrackState(pUS);
  
  return OK; 
}

bool TrigTRT_CombinedExtensionTool::runBackwardFilter(TrigTRT_Trajectory* pTRAJ)
{
  bool OK=true;
  MsgStream athenaLog(msgSvc(), name());
  Trk::TrkTrackState* pTS = pTRAJ->getStartingTrackState();
  Trk::TrkPlanarSurface* pSB=pTS->m_getSurface();
  pTRAJ->setStartingTrackState(NULL);
  bool first_step=true;

  std::vector<Trk::TrkBaseNode*>::reverse_iterator pnrIt(pTRAJ->getFilteringNodes()->rbegin()),
    pnrEnd(pTRAJ->getFilteringNodes()->rend());

  for(;pnrIt!=pnrEnd;++pnrIt)
    {
      Trk::TrkPlanarSurface* pSE=(*pnrIt)->m_getSurface();
      Trk::TrkTrackState* pNS=NULL;
      double C1,C2,dist=0.0;
      for(int i=0;i<3;i++)
	{
	  C1=pSB->m_getCenter()[i];C2=pSE->m_getCenter()[i];dist+=(C2-C1)*(C2-C1);
	}
      dist=sqrt(dist);
      if(dist>60.0)
	{
	  if(first_step)
	    {
	      pNS=extrapolateOffline(pTS,pSB,pSE,-1,true);
	      /*
	      printf("Extrapolated to SCT\n");
	      pNS->m_report();
	      printf("At\n");
	      pSE->m_report();		      
	      */  
	    }
	  else
	    pNS=extrapolateOffline(pTS,pSB,pSE,-1,false);
	}
      else
	pNS=m_fastExtrapolator->extrapolate(pTS,pSB,pSE,false);	
      pSB=pSE;
      first_step=false;
      if(pNS!=NULL)
	{
	  // m_updateFilteringNode((*pnrIt),pNS);
	  (*pnrIt)->m_validateMeasurement(pNS);
	  (*pnrIt)->m_updateTrackState(pNS);
	  delete pTS;
	  pTS=pNS;
	  double Pt=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
	  if(fabs(Pt)<200.0)
	    {
	      if (m_outputLevel <= MSG::DEBUG) 
		athenaLog << MSG::DEBUG << "pT < 200 MeV - skipping backward filter" << endmsg;
	      delete pTS;
	      OK=false;break;
	    }
	}
      else
	{
	  delete pTS;OK=false;break;
	}
    }
  
  if(OK) pTRAJ->setStartingTrackState(pTS);

  return OK; 
}

bool TrigTRT_CombinedExtensionTool::updatePerigee(TrigTRT_Trajectory* pTRAJ)
{
  bool OK=true;
  Trk::TrkTrackState* pTS = pTRAJ->getStartingTrackState();
  Trk::TrkPlanarSurface* pSB=pTS->m_getSurface();
  Trk::TrkPlanarSurface* pSE=NULL;
  Trk::TrkTrackState* pNS=extrapolateOffline(pTS,pSB,pSE,-1);
  delete pTS;pTRAJ->setStartingTrackState(NULL);
  if(pNS==NULL)
    OK=false; 
  else 
    pTRAJ->setStartingTrackState(pNS);
  return OK; 

}



const std::vector<int>* TrigTRT_CombinedExtensionTool::fillTRT_DataErrors()
{
  return &m_trtDataErrors;
}

Trk::TrkTrackState* TrigTRT_CombinedExtensionTool::extrapolateOffline(Trk::TrkTrackState* pTS, 
									Trk::TrkPlanarSurface* pSB,
									Trk::TrkPlanarSurface* pSE,
									int dir,
									bool createTargetSurface)
{
  //1. create starting parameters

  const Trk::TrackParameters* pTP=NULL;
  Trk::TrkTrackState* pTE=NULL;
  const Trk::PlaneSurface* pTrkSB=NULL;

  bool delete_surf=false;

  if(pSB==NULL)
    {
      // 1a. MeasuredPerigee
      AmgSymMatrix(5)* M = new AmgSymMatrix(5);
      for(int i=0;i<5;i++) for(int j=0;j<5;j++)
		  (*M)(i,j)=pTS->m_getTrackCovariance(i,j);
			Trk::PerigeeSurface dummySurface;
      pTP=new Trk::Perigee(pTS->m_getTrackState(0),pTS->m_getTrackState(1),
				   pTS->m_getTrackState(2),pTS->m_getTrackState(3),
				   pTS->m_getTrackState(4),dummySurface,M);
    }
  else
    {
      if(createTargetSurface && (dir<0))
	{
	  Amg::Vector3D colx(pSB->m_getInvRotMatrix(0,0),pSB->m_getInvRotMatrix(1,0),pSB->m_getInvRotMatrix(2,0));
	  Amg::Vector3D coly(pSB->m_getInvRotMatrix(0,1),pSB->m_getInvRotMatrix(1,1),pSB->m_getInvRotMatrix(2,1));
	  Amg::Vector3D colz(pSB->m_getInvRotMatrix(0,2),pSB->m_getInvRotMatrix(1,2),pSB->m_getInvRotMatrix(2,2));
	  Amg::Vector3D tr(pSB->m_getCenter()[0],pSB->m_getCenter()[1],pSB->m_getCenter()[2]);
		Amg::Transform3D* pTrans = new Amg::Transform3D(colx,coly,colz,tr);
	  pTrkSB = new Trk::PlaneSurface(pTrans);
	  delete_surf=true;
	}
      else
	{
	  pTrkSB = dynamic_cast<const Trk::PlaneSurface*>(pSB->m_getTrkSurface());
	}
			AmgSymMatrix(5)* M = new AmgSymMatrix(5);
      for(int i=0;i<5;i++) for(int j=0;j<5;j++)
	(*M)(i,j)=pTS->m_getTrackCovariance(i,j);
      pTP= pTrkSB->createParameters<5,Trk::Charged>(pTS->m_getTrackState(0),pTS->m_getTrackState(1),
				    pTS->m_getTrackState(2),pTS->m_getTrackState(3),
				    pTS->m_getTrackState(4),M);
    }

  // 2. Extrapolation

  const Trk::TrackParameters* predPar = NULL;

  if(dir>0)
    {
      if(!createTargetSurface)
	predPar = m_extrapolator->extrapolate(*pTP,*pSE->m_getTrkSurface(),
					      Trk::alongMomentum,false,Trk::pion);
      else
	{
	  Amg::Vector3D colx(pSB->m_getInvRotMatrix(0,0),pSB->m_getInvRotMatrix(1,0),pSB->m_getInvRotMatrix(2,0));
	  Amg::Vector3D coly(pSB->m_getInvRotMatrix(0,1),pSB->m_getInvRotMatrix(1,1),pSB->m_getInvRotMatrix(2,1));
	  Amg::Vector3D colz(pSB->m_getInvRotMatrix(0,2),pSB->m_getInvRotMatrix(1,2),pSB->m_getInvRotMatrix(2,2));
	  Amg::Vector3D tr(pSB->m_getCenter()[0],pSB->m_getCenter()[1],pSB->m_getCenter()[2]);
		Amg::Transform3D* pTrans = new Amg::Transform3D(colx,coly,colz,tr);
	  Trk::Surface* pTrkSE = new Trk::PlaneSurface(pTrans);
	  predPar = m_extrapolator->extrapolate(*pTP,*pTrkSE,
						Trk::alongMomentum,false,Trk::pion);
	  delete pTrkSE;
	}
    }
  else
    {
      if(pSE!=NULL)
	{
	  predPar = m_extrapolator->extrapolate(*pTP,*pSE->m_getTrkSurface(),Trk::oppositeMomentum,false,Trk::pion);	  
	}
      else
	{
	  Trk::PerigeeSurface perSurf;
	  predPar = m_extrapolator->extrapolate(*pTP,perSurf,Trk::oppositeMomentum,false,Trk::pion);
	}
    }

  if(predPar!=NULL)
    {
      if(pSE!=NULL)
	{
	  const Trk::AtaPlane* pTPE = dynamic_cast<const Trk::AtaPlane*>(predPar);

	  if(pTPE!=NULL) {
	    // 4. Create new TrackState
	    double Re[5],Ge[5][5];
	    Re[0]=pTPE->parameters()[Trk::locX];Re[1]=pTPE->parameters()[Trk::locY];
	    Re[2]=pTPE->parameters()[Trk::phi];Re[3]=pTPE->parameters()[Trk::theta];
	    Re[4]=pTPE->parameters()[Trk::qOverP];
	  
			const AmgSymMatrix(5)* M = pTPE->covariance();
	    for(int i=0;i<5;i++) for(int j=0;j<5;j++)
	      Ge[i][j]=(*M)(i,j);
	  
	    pTE=new Trk::TrkTrackState(Re);
	    if(m_doMultScatt) 
	      pTE->m_setScatteringMode(1);
	    pTE->m_setTrackCovariance(Ge);
	    pTE->m_attachToSurface(pSE);
	  }
	  else pTE=NULL;
	}
      else
	{

	  const Trk::AtaPlane* pTPE = dynamic_cast<const Trk::AtaPlane*>(predPar);
	  if(predPar->covariance()!=nullptr) {
	    // 4. Create new TrackState
	    double Re[5],Ge[5][5];
	    Re[0]=pTPE->parameters()[Trk::d0];Re[1]=pTPE->parameters()[Trk::z0];
	    Re[2]=pTPE->parameters()[Trk::phi];Re[3]=pTPE->parameters()[Trk::theta];
	    Re[4]=pTPE->parameters()[Trk::qOverP];
			const AmgSymMatrix(5)* M = pTPE->covariance();
	    for(int i=0;i<5;i++) for(int j=0;j<5;j++)
	      Ge[i][j]=(*M)(i,j);
	  
	    pTE=new Trk::TrkTrackState(pTS);
	    pTE->m_setTrackState(Re);
	    pTE->m_setTrackCovariance(Ge);
	  }
	  else pTE=NULL;
	}
      delete predPar;
    }
  if(delete_surf) delete pTrkSB;
  delete pTP;

  return pTE;
}

void TrigTRT_CombinedExtensionTool::updateFilteringNode(Trk::TrkBaseNode* pN, Trk::TrkTrackState* pTS)
{
  if(pTS->m_getSurface()==NULL) return;

  AmgSymMatrix(5)* M = new AmgSymMatrix(5);
  for(int i=0;i<5;i++) for(int j=0;j<5;j++)
    (*M)(i,j)=pTS->m_getTrackCovariance(i,j);

  const Trk::PlaneSurface& pTrkSB = dynamic_cast<const Trk::PlaneSurface&>(pN->m_getPrepRawData()->detectorElement()->surface());

	const Trk::TrackParameters* pTP=pTrkSB.createParameters<5,Trk::Charged>(pTS->m_getTrackState(0),pTS->m_getTrackState(1),
			pTS->m_getTrackState(2),pTS->m_getTrackState(3),
			pTS->m_getTrackState(4),M);


  const Trk::RIO_OnTrack* pRIO = m_ROTcreator->correct(*(pN->m_getPrepRawData()),*pTP);
  pN->m_updateWithRIO(pRIO);

  delete pTP;
  delete pRIO;
}

StatusCode TrigTRT_CombinedExtensionTool::propagate(TrigInDetTrackCollection* recoTracks )
{

  int nHits;
  double Pt, Phi0, D0, Z0, Eta, errD0, errZ0, errEta, errPhi0, errPt;
  double CV[5][5],a,b,c;
  std::vector<double>* pCov;
  TrigInDetTrackCollection::iterator trIt,lastIt;

  m_vpTrajectories.clear();

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
	m_timer[0]->stop();m_trtRobPreloaderTime=m_timer[0]->elapsed();
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
    TrigTRT_Trajectory* pTRAJ = createTRT_Trajectory((*trIt));
    if(pTRAJ==NULL) continue;

    nHits=pTRAJ->getFilteringNodes()->size();      
    if (m_outputLevel <= MSG::DEBUG) 
      athenaLog << MSG::DEBUG << nHits<<" filtering nodes created"<<endmsg;

    nFittedTracks++;
    if(m_timers) m_timer[1]->resume();

    bool OK= refitTRT_Trajectory(pTRAJ);

    if(m_timers) m_timer[1]->pause();
    if(OK)
      m_vpTrajectories.push_back(pTRAJ);
    else
      {
	delete pTRAJ;continue;
      }    
  }
  if(m_timers) 
    {
      m_timer[1]->propVal(nFittedTracks);
      m_timer[1]->stop();
    }
  
  if(m_vpTrajectories.size()==0) return StatusCode::SUCCESS; 

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
      Trk::TrkTrackState* pTS=NULL;
      if(!(*ptrIt)->isFast())
	{
	  pTS=(*ptrIt)->getStartingTrackState();
	}
      else
	{
	  pTS=(*((*ptrIt)->getTrackStates()->rbegin()));
	}
      if(m_outputLevel <= MSG::VERBOSE)
	{
	  athenaLog<<MSG::VERBOSE<<"Starting Track state for TRT road"<<endmsg;
	  pTS->m_report();
	}
      TrigTRT_DetElementRoad* pR=m_trtRoadBuilder->buildTRT_Road(pTS);
      (*ptrIt)->addRoad(pR);
      if(pR!=NULL) 
	{
	  pR->collectDetectorElements(&pDEList);nTRT_Roads++;
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

  // here will be the call for the dc provider tool fillCollections(vIDs,...)
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
	athenaLog<<MSG::VERBOSE<<"TRT Road:"<<endmsg;(*ptrIt)->getRoad()->report();
      }
    nTRT_Roads++;

    if(!(*ptrIt)->isFast())
      {
	if(m_timers) m_timer[4]->resume();

	(*ptrIt)->setStatus(runPDAF_Tracking((*ptrIt)));

	if(m_timers) m_timer[4]->pause();

	if(!(*ptrIt)->isValid()) continue;

	if(m_timers) m_timer[5]->resume();

	(*ptrIt)->setStatus(runBackwardFilter((*ptrIt)));

	if(m_timers) m_timer[5]->pause();

	if(!(*ptrIt)->isValid()) continue;

	if(m_timers) m_timer[5]->resume();

	(*ptrIt)->setStatus(updatePerigee((*ptrIt)));

	if(m_timers) m_timer[5]->pause();
      }
    else
      {
	// TRT tracking for fast tracks 
	if(m_timers) m_timer[4]->resume();
      
	Trk::TrkTrackState* pTS=(*((*ptrIt)->getTrackStates()->rbegin()));
	(*ptrIt)->setStartingTrackState(NULL);
	TrigTRT_DetElementRoad* pR=(*ptrIt)->getRoad();
      
	Trk::TrkPlanarSurface *pSB=pTS->m_getSurface(),*pSE=NULL;
      
	TrigTRT_Info* pTI=new TrigTRT_Info();
	(*ptrIt)->addTRT_SummaryInfo(pTI);
	TrigTRT_DetElementPoint* pRP=NULL;
	std::vector<TrigTRT_DetElementPoint*>::iterator ppIt;
	bool passedOK=true;
	bool firstSurf=true;
	double pT=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);

	if(fabs(pT)<500.0)
	  {
	    if (m_outputLevel <= MSG::DEBUG) 
	      athenaLog << MSG::DEBUG << "pT < 500 MeV - skipping TRT extension" << endmsg;
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
		    Trk::TrkTrackState* pNS=m_fastExtrapolator->extrapolate(pTS,pSB,pSE,true);
		  
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
	if(m_timers) 
	  {
	    m_timer[4]->pause(); 
	    m_timer[5]->resume();
	  }
	if(passedOK)
	  {
	    (*ptrIt)->smoothTrajectory();
	    (*ptrIt)->setStartingTrackState(new Trk::TrkTrackState(*((*ptrIt)->getTrackStates()->begin())));
	  }
	else (*ptrIt)->setStatus(false);
	if(m_timers) m_timer[5]->pause(); 
      }
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
      TrigInDetTrack* pTIDT=(*ptrIt)->getTrigInDetTrack();
      if((*ptrIt)->isValid())
	  {
	    //pTS=(*((*ptrIt)->getTrackStates()->begin()));
	    Trk::TrkTrackState *pTS=(*ptrIt)->getStartingTrackState();
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

	    TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>(pTIDT->param());
	    delete param;
	    pTIDT->param(tidtfp);
	  }
      int nAssociatedHits=0;
      if((*ptrIt)->getTRT_SummaryInfo()!=NULL)
	nAssociatedHits=(*ptrIt)->getTRT_SummaryInfo()->getTRT_Hits().size();
      if(nAssociatedHits!=0)
	{
	  //int Nhits=0,Nstraw=0,NTR=0,NTime=0;
	  int Nhits   = (*ptrIt)->getNumberOfTRT_Hits();
	  int Nstraw  = (*ptrIt)->getNumberOfCrossedTRT_Straws();
	  int NTR     = (*ptrIt)->getNumberOfHighThresholdTRT_Hits();
	  int NTime   = (*ptrIt)->getNumberOfDriftTimeTRT_Hits();
	  pTIDT->StrawHits(Nhits);
	  pTIDT->Straw(Nstraw);
	  pTIDT->TRHits(NTR);
	  pTIDT->StrawTime(NTime);
	  if(m_outputLevel <= MSG::DEBUG) 
	    {
	      athenaLog<<MSG::DEBUG<<"N TR hits="<<NTR<<" N TRT hits="<<Nhits<<" N holes="<<
		(*ptrIt)->getNumberOfMissedDetection()<<endmsg;
	    }
	  std::vector<const InDet::TRT_DriftCircle*>* pDC=new std::vector<const InDet::TRT_DriftCircle*>;
	  for(std::vector<const InDet::TRT_DriftCircle*>::iterator dcIt=(*ptrIt)->getTRT_SummaryInfo()->getTRT_Hits().begin();
	      dcIt!=(*ptrIt)->getTRT_SummaryInfo()->getTRT_Hits().end();++dcIt)
	    {
	      pDC->push_back((*dcIt));
	    }
	  pTIDT->trtDriftCircles(pDC);
	}
      if(m_outputLevel <= MSG::DEBUG) 
	{
	  if((*ptrIt)->getTRT_SummaryInfo()!=NULL)
	    athenaLog<<MSG::DEBUG<<nAssociatedHits<<" TRT hits are associated with the track"<<endmsg;
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

