/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2HighPtTrackFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 16.06.2010 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "GaudiKernel/ToolFactory.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "TrigInDetTrackFitter/TrigL2HighPtTrackFitter.h"

TrigL2HighPtTrackFitter::TrigL2HighPtTrackFitter(const std::string& t, 
						 const std::string& n,
						 const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_recalibrate(false),
  m_fastExtrapolator("TrigL2FastExtrapolationTool"),
  m_ROTcreator("Trk::RIO_OnTrackCreator/InDetTrigBroadInDetRotCreator")
{
  declareInterface< ITrigL2TrackFittingTool >( this );  
  declareProperty( "fastExtrapolator", m_fastExtrapolator, "TrigL2FastExtrapolationTool");
  declareProperty( "useROTs",m_recalibrate = true);
  declareProperty( "ROTcreator", m_ROTcreator, "Trk::RIO_OnTrackCreator/InDetTrigBroadInDetRotCreator");
}

StatusCode TrigL2HighPtTrackFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  if(m_recalibrate)
    {
      sc=m_ROTcreator.retrieve();
      if(sc.isFailure())
	{
	  ATH_MSG_ERROR("Could not retrieve RIO_OnTrack creator tool "<<m_ROTcreator);
	  return StatusCode::FAILURE;
	}
    }
  sc=m_fastExtrapolator.retrieve();
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve extrapolation tool"<<m_fastExtrapolator);
      return sc;
    }
  ATH_MSG_INFO("TrigL2HighPtTrackFitter initialized ");
  return sc;
}

StatusCode TrigL2HighPtTrackFitter::finalize()
{
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

void TrigL2HighPtTrackFitter::m_recalibrateFilteringNode(Trk::TrkBaseNode* pN, Trk::TrkTrackState* pTS)
{
  if(pTS->m_getSurface()==NULL) return;
  AmgSymMatrix(5)* pM = new AmgSymMatrix(5);

  for(int i=0;i<5;i++) 
    for(int j=0;j<5;j++)
      (*pM)(i,j)=pTS->m_getTrackCovariance(i,j);
  
  const Trk::PlaneSurface& pTrkSB = dynamic_cast<const Trk::PlaneSurface&>(pN->m_getPrepRawData()->detectorElement()->surface());
  Trk::TrackParameters* pTP=new Trk::AtaPlane(pTS->m_getTrackState(0),pTS->m_getTrackState(1),
					      pTS->m_getTrackState(2),pTS->m_getTrackState(3),
					      pTS->m_getTrackState(4),pTrkSB,
					      pM);
  
  const Trk::RIO_OnTrack* pRIO = m_ROTcreator->correct(*(pN->m_getPrepRawData()),*pTP);
  pN->m_updateWithRIO(pRIO);
  
  delete pTP;
  delete pRIO;
}

Trk::TrkTrackState* TrigL2HighPtTrackFitter::fit(Trk::TrkTrackState* pTS, std::vector<Trk::TrkBaseNode*>& vpTrkNodes,
						 bool runSmoother)
{
  std::vector<Trk::TrkTrackState*> vpTrackStates;
  
  vpTrackStates.clear();
  vpTrackStates.push_back(pTS);

  std::vector<Trk::TrkBaseNode*>::iterator pnIt(vpTrkNodes.begin()),
    pnEnd(vpTrkNodes.end());

  bool OK=true;
  Trk::TrkPlanarSurface *pSB=NULL,*pSE;

  for(;pnIt!=pnEnd;++pnIt)
    {
      pSE=(*pnIt)->m_getSurface();
      Trk::TrkTrackState* pNS=m_fastExtrapolator->extrapolate(pTS,pSB,pSE,runSmoother);	  
      pSB=pSE;
      if(pNS!=NULL)
	{
	  vpTrackStates.push_back(pNS);

	  if(m_recalibrate)
	    {
	      m_recalibrateFilteringNode((*pnIt), pNS);
	    }
	  
	  (*pnIt)->m_validateMeasurement(pNS);
		ATH_MSG_DEBUG("dChi2="<<(*pnIt)->m_getChi2());
	  (*pnIt)->m_updateTrackState(pNS);
	  pTS=pNS;
	  double Pt=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
	  if(fabs(Pt)<200.0)
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
      if(runSmoother)
	{
	  std::vector<Trk::TrkTrackState*>::reverse_iterator ptsIt(vpTrackStates.rbegin()),
	    ptsEnd(vpTrackStates.rend());
	  for(;ptsIt!=ptsEnd;++ptsIt)
	    {
	      (*ptsIt)->m_runSmoother();
	    }
	  pTS=new Trk::TrkTrackState( (*vpTrackStates.begin()));
	}
      else
	pTS=new Trk::TrkTrackState( (*vpTrackStates.rbegin()));
    }
  else
    {
      pTS=NULL;
      ATH_MSG_DEBUG("High Pt track fitter: extrapolation failure ");
    }
  for(std::vector<Trk::TrkTrackState*>::iterator ptsIt=vpTrackStates.begin();
      ptsIt!=vpTrackStates.end();++ptsIt) delete (*ptsIt);
  vpTrackStates.clear();
  return pTS;

}
