/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigToTrkTrackTool
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#include <cmath>
#include <iostream>
#include "StoreGate/StoreGateSvc.h" 
#include "GaudiKernel/ToolFactory.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetTrackUtils/TrigToTrkTrackTool.h"
#include "TrkParameters/TrackParameters.h"
#include "EventPrimitives/SymmetricMatrixHelpers.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "StoreGate/DataHandle.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkTrack/TrackInfo.h"

TrigToTrkTrackTool::TrigToTrkTrackTool(const std::string& t, 
		       const std::string& n,
		       const IInterface*  p ): AlgTool(t,n,p)
{
  declareInterface< TrigToTrkTrackTool >( this );
  declareProperty( "createRIO", m_createRIO = false );
  declareProperty("FitterName", m_FitterName = "Trk::KalmanFilter");
  declareProperty("FitterInstance", m_FitterInstance = "InDetTrackFitter");
  declareProperty("doFit", m_doFit = false);
}

static const InterfaceID IID_ITrigToTrkTrackTool("TrigToTrkTrackTool", 1, 0);

const InterfaceID& TrigToTrkTrackTool::interfaceID()
{
  return IID_ITrigToTrkTrackTool;
}
 
StatusCode TrigToTrkTrackTool::initialize()
{
  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  athenaLog << MSG::INFO << "TrigToTrkTrackTool constructed "                      << endreq;
  athenaLog << MSG::INFO << "Create RIO on track is set to " << m_createRIO   << endreq;

  ITrigTimerSvc* timerSvc;
  StatusCode  scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    athenaLog << MSG::INFO<< "TrigTrackConvertor: Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("CreatePerigee");
    m_timer[0]->propName("CreatePerigee.nTracks");
    m_timer[1] = timerSvc->addItem("CreateTrkRIOs");
    m_timer[1]->propName("CreateTrkRIOs.nTracks");
    m_timer[2] = timerSvc->addItem("CreateTrkTrack");
    m_timer[2]->propName("CreateTrkTrack.nTracks");
    m_timer[3] = timerSvc->addItem("TrkFitter");
    m_timer[3]->propName("TrkFitter.nTracks");
  }


  StoreGateSvc* detStore = 0;
  sc = service( "DetectorStore", detStore); 
  if( sc.isFailure() ) 
    {
      athenaLog << MSG::ERROR << "  TrigToTrkTrackTool unable to locate DetectorStore" << endreq;
      return sc;
    } 

  // Get SCT & pixel Identifier helpers                                                                                                        
                                                                                                                                              
  if (detStore->retrieve(m_pixelId, "PixelID").isFailure()) {
     athenaLog << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;                                                                                                              
  }                                                                                                                                           
  if (detStore->retrieve(m_sctId, "SCT_ID").isFailure()) {
     athenaLog << MSG::FATAL << "Could not get SCT ID helper" << endreq;                                                                      
     return StatusCode::FAILURE;                                                                                                              
  }   

  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    athenaLog << MSG::ERROR << " Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }

  if(m_doFit)
    {
      StatusCode scFit = toolSvc->retrieveTool(m_FitterName, m_FitterInstance, m_ITrkFitter);
      if (scFit.isFailure()) 
	{
	  athenaLog<<MSG::ERROR<<"Could not find refit tool of type "<<m_FitterName<<". Exiting."
		   << endreq;
	  return scFit;
	} 
      else 
	{
	  athenaLog << MSG::INFO << "Refit tool \""<<m_FitterName<<" "
		    <<m_FitterInstance<<"\" booked."
		    << endreq;
	}
    }
  return sc;
}

StatusCode TrigToTrkTrackTool::finalize()
{
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

TrigToTrkTrackTool::~TrigToTrkTrackTool()
{

}


void TrigToTrkTrackTool::execute(const TrigInDetTrackCollection* oldTracks,
			  TrackCollection* fittedTracks) const
{
  constexpr double twoPi=2*M_PI;

  AmgSymMatrix(5)* pCV = new AmgSymMatrix(5);
  const Trk::Perigee* pMP;
  double Gk[5][5];
  double phi0,theta,a,b,c;
  int nTracks=0;

  MsgStream log(msgSvc(), name());

  TrackCollection* newTracks=new TrackCollection;
  
  newTracks->clear(); 
  //printf("Trig to Trk convertor total %d input tracks\n",oldTracks->size());
  TrigInDetTrackCollection::const_iterator trIt = oldTracks->begin();
  TrigInDetTrackCollection::const_iterator lastIt = oldTracks->end();


  if ( m_timers ) 
    {
      m_timer[0]->start();
      m_timer[0]->pause();
      m_timer[1]->start();
      m_timer[1]->pause();
      m_timer[2]->start();
      m_timer[2]->pause();
    }


  for(; trIt !=lastIt; trIt++) 
    {
      nTracks++;
      phi0=(*trIt)->param()->phi0();
      if(phi0>0.5*twoPi) phi0-=twoPi;
      theta=2.0*atan(exp(-(*trIt)->param()->eta())); 

      if((*trIt)->param()->cov()==NULL)
	{
	  (*pCV)(0,0)=(*trIt)->param()->ea0()*(*trIt)->param()->ea0();
	  (*pCV)(1,1)=(*trIt)->param()->ez0()*(*trIt)->param()->ez0();
	  (*pCV)(2,2)=(*trIt)->param()->ephi0()*(*trIt)->param()->ephi0();
	  (*pCV)(3,3)=(*trIt)->param()->eeta()*(*trIt)->param()->eeta()*sin(theta)*sin(theta);
	  (*pCV)(4,4)=(sin(theta)*sin(theta)*(*trIt)->param()->epT()*(*trIt)->param()->epT()/
			  ((*trIt)->param()->pT()*(*trIt)->param()->pT())+cos(theta)*cos(theta)*
			  (*pCV)(3,3))/((*trIt)->param()->pT()*(*trIt)->param()->pT());
	}
      else
	{
	  int idx=0;
	  for(int i=0;i<5;i++) 
	    for(int j=i;j<5;j++)
	      {
		Gk[j][i]=Gk[i][j]=(*(*trIt)->param()->cov())[idx];
		idx++;
	      }
	  a=-sin(theta);
	  b=-sin(theta)*cos(theta)/(*trIt)->param()->pT();
	  c=-sin(theta)/((*trIt)->param()->pT()*(*trIt)->param()->pT());
	  (*pCV)(0,0)=Gk[0][0];
	  (*pCV).fillSymmetric(1,0,Gk[0][2]);
	  (*pCV).fillSymmetric(2,0,Gk[0][1]);
	  (*pCV).fillSymmetric(3,0,a*Gk[0][3]);
	  (*pCV).fillSymmetric(4,0,b*Gk[0][3]+c*Gk[0][4]);
	  (*pCV)(1,1)=Gk[2][2];
	  (*pCV).fillSymmetric(2,1,Gk[1][2]);
	  (*pCV).fillSymmetric(3,1,a*Gk[2][3]);
	  (*pCV).fillSymmetric(4,1,b*Gk[2][3]+b*Gk[2][4]);
	  (*pCV)(2,2)=Gk[1][1];
	  (*pCV).fillSymmetric(3,2,a*Gk[1][3]);
	  (*pCV).fillSymmetric(4,2,b*Gk[1][3]+b*Gk[1][4]);
	  (*pCV)(3,3)=a*a*Gk[3][3];
	  (*pCV).fillSymmetric(4,3,a*b*Gk[3][3]+a*c*Gk[3][4]);
	  (*pCV)(4,4)=b*b*Gk[3][3]+2*b*c*Gk[3][4]+c*c*Gk[4][4];
	}
      if ( m_timers ) m_timer[0]->resume();
			Trk::PerigeeSurface dummySurface;
      pMP=new Trk::Perigee((*trIt)->param()->a0(),
				   (*trIt)->param()->z0(),
				   phi0,theta,
				   sin(theta)/(*trIt)->param()->pT(),
				   dummySurface,
				   pCV);
      

      DataVector<const Trk::TrackStateOnSurface>* pParVec = new DataVector<const Trk::TrackStateOnSurface>; 

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
      typePattern.set(Trk::TrackStateOnSurface::Perigee);

      const Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(0,
											 pMP,
											 0,0,
											 typePattern); 
      pParVec->push_back(trackStateOnSurface); 
      Trk::FitQuality* pFQ=new Trk::FitQuality((*trIt)->chi2(),1);
      if ( m_timers ) m_timer[0]->pause();
      if ( m_timers ) m_timer[1]->resume();

      if(m_createRIO)
	{
	  if ((*trIt)->algorithmId() ==  TrigInDetTrack::IDSCANID || 
	      (*trIt)->algorithmId() ==  TrigInDetTrack::SITRACKID)
	    {
	      if ((*trIt)->siSpacePoints())
		{
		  std::vector<const TrigSiSpacePoint*>::iterator pSP=(*trIt)->siSpacePoints()->begin();
		  std::vector<const TrigSiSpacePoint*>::iterator lastSP=(*trIt)->siSpacePoints()->end();
		  for(int iSP=0; pSP != lastSP; pSP++, iSP++) 
		    {
		      if (m_sctId->is_sct((*pSP)->identify())) 
			{
			  for (int iClusInSP=0; iClusInSP<2; iClusInSP++)
			    {
			      const InDet::SCT_Cluster* pCL;
			      if (iClusInSP == 1) 
				{
				  pCL = dynamic_cast<const InDet::SCT_Cluster*>((*pSP)->clusters().first);
				  
				}
			      else                     
				{
				  pCL = dynamic_cast<const InDet::SCT_Cluster*>((*pSP)->clusters().second);
				}
			      if(!pCL) continue;
			      const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(pCL->identify()));
						InDet::SCT_ClusterOnTrack* pSC=new InDet::SCT_ClusterOnTrack(pCL,pCL->localPosition(),
                                                                       pCL->localCovariance().inverse(),
                                                                       idHash,pCL->globalPosition(),
                                                											 pCL->gangedPixel());
                               
			      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
			      typePattern.set(Trk::TrackStateOnSurface::Measurement);

			      trackStateOnSurface=new Trk::TrackStateOnSurface(pSC, 
									       0, 
									       0,  
									       0, typePattern);
			      pParVec->push_back(trackStateOnSurface);
			    }
			} 
		      else 
			{  // Pixel spacepoint
			  const InDet::PixelCluster* pCL = dynamic_cast<const InDet::PixelCluster*>((*pSP)->clusters().first);
			  if(!pCL) continue;
			  const Amg::Vector3D glo(pCL->globalPosition());
			  const IdentifierHash idHash = m_pixelId->wafer_hash(m_pixelId->wafer_id(pCL->identify()));
			  InDet::PixelClusterOnTrack* pSC=new InDet::PixelClusterOnTrack(pCL,pCL->localPosition(),
                                                                       pCL->localCovariance().inverse(),
                                                                       idHash,pCL->globalPosition(),
                                                											 pCL->gangedPixel());
			  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
			  typePattern.set(Trk::TrackStateOnSurface::Measurement);
			  trackStateOnSurface=new Trk::TrackStateOnSurface(pSC, 
									   0, 
									   0,  
									   0, typePattern);
			  pParVec->push_back(trackStateOnSurface);
			}
		    }
		}
	    }
	}
      if ( m_timers ) m_timer[1]->pause();
      Trk::TrackInfo::TrackPatternRecoInfo pattRecInfo;      
      TrigInDetTrack::AlgoId onlineAuthor=(*trIt)->algorithmId();
      switch(onlineAuthor)
	{
	case TrigInDetTrack::SITRACKID: 
	  {
	    pattRecInfo=Trk::TrackInfo::TrigSiTrack;break;
	  }
	case TrigInDetTrack::IDSCANID:
	  {
	    pattRecInfo=Trk::TrackInfo::TrigIDSCAN;break;
	  }
	case TrigInDetTrack::TRTLUTID:
	  {
	    pattRecInfo=Trk::TrackInfo::TrigTRTLUT;break;
	  }
	case TrigInDetTrack::TRTXKID:
	  {
	    pattRecInfo=Trk::TrackInfo::TrigTRTxK;break;
	  }
	default: pattRecInfo=Trk::TrackInfo::NumberOfTrackRecoInfo;
	};
      if ( m_timers ) m_timer[2]->resume();

      Trk::TrackInfo info(Trk::TrackInfo::KalmanFitter,Trk::pion);
      info.setPatternRecognitionInfo(pattRecInfo);
      Trk::Track* pT = new Trk::Track(info,pParVec,pFQ);
      if ( m_timers ) m_timer[2]->pause();
      if(m_doFit) newTracks->push_back(pT);
      else fittedTracks->push_back(pT);
    }
  if ( m_timers ) 
    {
      m_timer[0]->propVal(nTracks);
      m_timer[0]->stop();
      m_timer[1]->propVal(nTracks);
      m_timer[1]->stop();
      m_timer[2]->propVal(nTracks);
      m_timer[2]->stop();
      m_timer[3]->start();
      m_timer[3]->pause();
    }
  if(!m_doFit) 
    {
      if ( m_timers ) 
	{
	  m_timer[3]->propVal(nTracks);
	  m_timer[3]->stop();
	}
      delete newTracks;
      return;
    }
  else
    {
     
      for(TrackCollection::const_iterator itr=(*newTracks).begin();itr<(*newTracks).end();itr++)
	{
	  if(log.level() <= MSG::VERBOSE)
	    log << MSG::VERBOSE << "input track:" << **itr << endreq;
	  Trk::Track* newtrack;
	  if ( m_timers ) m_timer[3]->resume();
	  newtrack = m_ITrkFitter->fit(**itr,false, Trk::nonInteracting);
	  if ( m_timers ) m_timer[3]->pause();
	  if(newtrack!=NULL)
	    {
	      fittedTracks->push_back(newtrack);
	      const Trk::Perigee *aMeasPer=	newtrack->perigeeParameters();
	      if (aMeasPer==0){
		log << MSG::ERROR << "Could not get Trk::MeasuredPerigee" << endreq;
		continue;
	      }
				if (aMeasPer->covariance()==nullptr) {
					log << MSG::ERROR << "Could not get Trk::MeasuredPerigee" << endreq;
				}
	      double d0 = aMeasPer->parameters()[Trk::d0];
	      double z0 = aMeasPer->parameters()[Trk::z0];
	      double phi0 = aMeasPer->parameters()[Trk::phi0];
	      double theta = aMeasPer->parameters()[Trk::theta];
	      double qOverP = aMeasPer->parameters()[Trk::qOverP];
	      log << MSG::DEBUG << "Refitted parameters " << d0  << " " << z0  << " " << phi0 << " " << theta << " " << qOverP <<  endreq;
	    }
	  else
	    {
	      log << MSG::INFO << "Offline fitter failed - TrkTrack will not be created" << endreq;
	    }
	}
      if ( m_timers ) m_timer[3]->propVal(nTracks);
      if ( m_timers ) m_timer[3]->stop();
      delete newTracks;
      
    }
}
