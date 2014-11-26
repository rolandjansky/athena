/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigPrimaryVertexFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 04.05.2008 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <iostream>
#include <algorithm>
#include "GaudiKernel/ToolFactory.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigVertexFitter/ITrigPrimaryVertexFitter.h"
#include "TrigVertexFitter/TrigPrimaryVertexFitter.h"
#include "TrigVertexFitter/TrigPrimaryVertexTrack.h"


TrigPrimaryVertexFitter::TrigPrimaryVertexFitter(const std::string& t, 
						 const std::string& n,
						 const IInterface*  p ): AlgTool(t,n,p), 
									 m_createTrackLists(false),
									 m_xyVariance(10.0)
{
  declareInterface< ITrigPrimaryVertexFitter >( this );
  declareProperty( "numberOfIterations", m_numIter=3);
  declareProperty( "maxChi2Increase", m_maxChi2Increase=1000.0);
  declareProperty( "zVariance", m_zVariance=1.0);
  declareProperty( "Chi2Cut", m_chi2cut=15.0);
  declareProperty( "Chi2CutLoose", m_chi2cut_loose=100.0);
  declareProperty( "CreateTrackLists", m_createTrackLists=false);
  declareProperty( "xyVariance", m_xyVariance=10.0);
}

StatusCode TrigPrimaryVertexFitter::initialize()
{

  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  athenaLog << MSG::INFO << "Number of iterations is set to " << m_numIter << endreq;
  athenaLog << MSG::INFO << "Accuracy of starting z-position is " << m_zVariance<<" mm" << endreq;
  athenaLog << MSG::INFO << "Track lists created ? " << std::boolalpha<<m_createTrackLists<< endreq;
  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if(scTime.isFailure()) 
    {
      athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
      m_timers = false;
    } 
  else m_timers = true;  
  if(m_timers) 
    {
      m_timer[0] = timerSvc->addItem("PrimaryVertexFit");
    }
  athenaLog << MSG::INFO << "TrigPrimaryVertexFitter constructed "<< endreq;
  return sc;
}

StatusCode TrigPrimaryVertexFitter::finalize()
{
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

TrigPrimaryVertexFitter::~TrigPrimaryVertexFitter()
{

}

TrigVertex* TrigPrimaryVertexFitter::fit(const TrigInDetTrackCollection* tc, double z0)
{

  TrigVertex* pVertex=NULL;

  double chi2;
  int ndof;

  if(m_timers) m_timer[0]->start();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  bool goodSet=m_checkTracks(tc);

  if(!goodSet)
    {
      athenaLog << MSG::WARNING << "Bad track set - vertex cannot be fit"<< endreq;
      return NULL;
    }

  std::vector<TrigPrimaryVertexTrack*> tracks;
  tracks.clear();
  int idx=1;
  for(TrigInDetTrackCollection::const_iterator ptIt=tc->begin();ptIt!=tc->end();++ptIt)
  {
    TrigPrimaryVertexTrack* pT= new TrigPrimaryVertexTrack(*ptIt);
    if(pT!=NULL) {
      pT->m_setIndex(idx++);
      tracks.push_back(pT);
    }
  }

  TrigL2Vertex* pV = new TrigL2Vertex();

  pV->m_getParametersVector()[0]=0.0;
  pV->m_getParametersVector()[1]=0.0;
  pV->m_getParametersVector()[2]=z0;
  bool fitFailed=false;
  chi2=0.0;ndof=-3;
  for(int nIter=0;nIter<m_numIter;nIter++)
    {
      memset(&pV->m_Gk[0][0],0,sizeof(pV->m_Gk));
      pV->m_Gk[0][0]=m_xyVariance*m_xyVariance;
      pV->m_Gk[1][1]=m_xyVariance*m_xyVariance;
      pV->m_Gk[2][2]=m_zVariance*m_zVariance;
      chi2=0.0;ndof=-3;
      fitFailed=false;

      for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
	{
	  if(!(*it)->m_isActive()) 
	    (*it)->m_activate();

	  double dchi2=(*it)->m_getChi2Distance(pV);
	  if (outputLevel <= MSG::VERBOSE) 
	    {
	      athenaLog<<MSG::VERBOSE<<"Track "<<(*it)->m_getIndex()<<" dchi2="<<dchi2<<endreq;
	    }
	  if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
	    {
	      fitFailed=true;
	      break;
	    }
	  if(dchi2<m_chi2cut)
	    {
	      chi2+=dchi2;ndof+=2;
	      (*it)->m_updateVertex(pV);
	    }
	  else
	    {
	      (*it)->m_mask();
	      if (outputLevel <= MSG::DEBUG) 
		{
		  athenaLog<<MSG::DEBUG<<"Skipping track "<<(*it)->m_getIndex()<<"due to large dchi2="<<dchi2<<endreq;
		}
	    }
	}
      if(fitFailed) break;
    }
  

  if(!fitFailed && (ndof>0))
    {
      if (outputLevel <= MSG::DEBUG) 
	{
	  athenaLog<<MSG::DEBUG<<"Primary vertex fit OK, chi2 = "<<chi2<<" NDOF = "<<ndof<<endreq;
	  athenaLog<<MSG::DEBUG<<"x= "<<pV->m_getParametersVector()[0]<<
		   " y= "<<pV->m_getParametersVector()[1]<<" z= "<<pV->m_getParametersVector()[2]<<endreq;
	}
      double cv[6];
      cv[0]=pV->m_Gk[0][0];cv[1]=pV->m_Gk[0][1];cv[2]=pV->m_Gk[1][1];
      cv[3]=pV->m_Gk[0][2];cv[4]=pV->m_Gk[1][2];cv[5]=pV->m_Gk[2][2];		
      TrackInVertexList* pTL = NULL;
      if(m_createTrackLists) {

	pTL = new TrackInVertexList;
	for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it) {
	    if ((*it)->m_isActive()) 
	      pTL->push_back((*it)->m_getTrigTrack());
	  }
      }
      pVertex = new TrigVertex(pV->m_getParametersVector()[0],
			       pV->m_getParametersVector()[1],
			       pV->m_getParametersVector()[2],cv,chi2,ndof,pTL);
    }
  delete pV;
  for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
    {
      delete (*it);
    }
  if(m_timers) m_timer[0]->stop();

  return pVertex;
}

struct TrigPrimaryTrackSortPredicate
{
  bool operator()(TrigPrimaryVertexTrack* pA, TrigPrimaryVertexTrack* pB)
  {
    return (pA->m_getChi2Contribution() < pB->m_getChi2Contribution());
  }
};

TrigVertex* TrigPrimaryVertexFitter::fit(const TrigInDetTrackCollection* tc, std::vector<double>& Chi2V, double z0)
{

  TrigVertex* pVertex=NULL;

  double chi2;
  int ndof;

  if(m_timers) m_timer[0]->start();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  bool goodSet=m_checkTracks(tc);

  if(!goodSet)
    {
      athenaLog << MSG::WARNING << "Bad track set - vertex cannot be fit"<< endreq;
      return NULL;
    }

  std::vector<TrigPrimaryVertexTrack*> tracks;
  tracks.clear();
  Chi2V.clear();
  int idx=1;
  for(TrigInDetTrackCollection::const_iterator ptIt=tc->begin();ptIt!=tc->end();++ptIt)
  {
    TrigPrimaryVertexTrack* pT= new TrigPrimaryVertexTrack(*ptIt);
    if(pT!=NULL)
      {
	pT->m_setIndex(idx++);
	tracks.push_back(pT);
	Chi2V.push_back(-100.0);
      }
  }

  TrigL2Vertex* pV = new TrigL2Vertex();

  // 1. preliminary fit

  pV->m_getParametersVector()[0]=0.0;
  pV->m_getParametersVector()[1]=0.0;
  pV->m_getParametersVector()[2]=z0;

  memset(&pV->m_Gk[0][0],0,sizeof(pV->m_Gk));
  pV->m_Gk[0][0]=m_xyVariance*m_xyVariance;
  pV->m_Gk[1][1]=m_xyVariance*m_xyVariance;
  pV->m_Gk[2][2]=m_zVariance*m_zVariance;

  bool fitFailed=false;
  for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
    {
      double dchi2=(*it)->m_getChi2Distance(pV);
      if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
	{
	  fitFailed=true;
	  break;
	}
      if(dchi2<m_chi2cut_loose) (*it)->m_updateVertex(pV);
    }
  if(fitFailed) 
    {
      delete pV;
      for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();
	  it!=tracks.end();++it) delete (*it);
      return pVertex;
    }

  // 2. calculation of "smoothed" chi2s

  memset(&pV->m_Gk[0][0],0,sizeof(pV->m_Gk));

  for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
    {
      double dchi2=(*it)->m_getChi2Distance(pV);
      if (outputLevel <= MSG::DEBUG) 
	{
	  athenaLog<<MSG::DEBUG<<"Track "<<(*it)->m_getIndex()<<" dchi2="<<dchi2<<endreq;
	}
    }  

  // 3. sort tracks in accordance with their chi2s

  std::sort(tracks.begin(),tracks.end(),TrigPrimaryTrackSortPredicate());

  // 4. Final track fit

  pV->m_getParametersVector()[0]=0.0;
  pV->m_getParametersVector()[1]=0.0;
  pV->m_getParametersVector()[2]=z0;
  fitFailed=false;
  chi2=0.0;ndof=-3;
  for(int nIter=0;nIter<m_numIter;nIter++)
    {
      memset(&pV->m_Gk[0][0],0,sizeof(pV->m_Gk));
      pV->m_Gk[0][0]=100.0;
      pV->m_Gk[1][1]=100.0;
      pV->m_Gk[2][2]=m_zVariance*m_zVariance;
      chi2=0.0;ndof=-3;
      fitFailed=false;

      for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
	{
	  double dchi2=(*it)->m_getChi2Distance(pV);
	  Chi2V[(*it)->m_getIndex()-1]=dchi2;
	  if (outputLevel <= MSG::VERBOSE) 
	    {
	      athenaLog<<MSG::VERBOSE<<"Track "<<(*it)->m_getIndex()<<" dchi2="<<dchi2<<endreq;
	    }
	  if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
	    {
	      fitFailed=true;
	      break;
	    }
	  if(dchi2<m_chi2cut)
	    {
	      chi2+=dchi2;ndof+=2;
	      (*it)->m_updateVertex(pV);
	    }
	  else
	    {
	      if (outputLevel <= MSG::DEBUG) 
		{
		  athenaLog<<MSG::DEBUG<<"Skipping track "<<(*it)->m_getIndex()<<"due to large dchi2="<<dchi2<<endreq;
		}
	    }
	}
      if(fitFailed) break;
    }
  for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
    {
      delete (*it);
    }

  if(!fitFailed && (ndof>0))
    {
      if (outputLevel <= MSG::DEBUG) 
	{
	  athenaLog<<MSG::DEBUG<<"Primary vertex fit OK, chi2 = "<<chi2<<" NDOF = "<<ndof<<endreq;
	  athenaLog<<MSG::DEBUG<<"x= "<<pV->m_getParametersVector()[0]<<
		   " y= "<<pV->m_getParametersVector()[1]<<" z= "<<pV->m_getParametersVector()[2]<<endreq;
	}
      double cv[6];
      cv[0]=pV->m_Gk[0][0];cv[1]=pV->m_Gk[0][1];cv[2]=pV->m_Gk[1][1];
      cv[3]=pV->m_Gk[0][2];cv[4]=pV->m_Gk[1][2];cv[5]=pV->m_Gk[2][2];		
      pVertex = new TrigVertex(pV->m_getParametersVector()[0],
			       pV->m_getParametersVector()[1],
			       pV->m_getParametersVector()[2],cv,chi2,ndof,NULL);
    }
  delete pV;

  if(m_timers) m_timer[0]->stop();

  return pVertex;
}


bool TrigPrimaryVertexFitter::m_checkTracks(const TrigInDetTrackCollection* tc)
{
  bool rc=true;
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  
  if(tc->size()<2)
    {
      if (outputLevel <= MSG::WARNING) 
	athenaLog << MSG::WARNING << "Track set contains less than 2 tracks" << endreq;
      return false;
    }
  for(TrigInDetTrackCollection::const_iterator ptIt=tc->begin();ptIt!=tc->end();++ptIt)
    {
      const TrigInDetTrackFitPar* p=(*ptIt)->param();
      if(p==NULL) 
	{
	  if (outputLevel <= MSG::WARNING) 
	    athenaLog << MSG::WARNING << "TrigInDetTrack has no parameters" << endreq;
	  rc=false;break;
	}
      if(p->cov()==NULL)
	{
	  if (outputLevel <= MSG::WARNING) 
	    athenaLog << MSG::WARNING << "TrigInDetTrack covariance is not set" << endreq;
	  rc=false;break;
	}
    }  
  return rc;
}
