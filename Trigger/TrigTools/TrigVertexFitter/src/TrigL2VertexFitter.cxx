/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2VertexFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 04.07.2006 Package created
//
// a new version of LVL2 vertex fitter based on a Kalman filter with
// decorrelating measurement noise transformation
// 
// this tool will supersede TrigVertexFitter
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////


#include <cmath>
#include <iostream>

#include "TrigInDetEvent/TrigL2Vertex.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigInDetToolInterfaces/ITrigVertexingTool.h"
#include "TrigL2VertexFitter.h"

TrigL2VertexFitter::TrigL2VertexFitter(const std::string& t, 
				       const std::string& n,
				       const IInterface*  p ): AthAlgTool(t,n,p), 
							       m_vertexingTool("TrigVertexingTool")
{
  declareInterface< ITrigL2VertexFitter >( this );
  declareProperty( "numberOfIterations", m_numIter=5);
  declareProperty( "maxChi2Increase", m_maxChi2Increase=1000.0);
  declareProperty( "VertexingTool", m_vertexingTool, "TrigVertexingTool");
}

StatusCode TrigL2VertexFitter::initialize()
{
  ATH_CHECK( AthAlgTool::initialize() );
  ATH_CHECK( m_vertexingTool.retrieve() );
  ATH_MSG_INFO( "TrigVertexingTool retrieved" );

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
      m_timer[0] = timerSvc->addItem("TrigL2Vertex");
      m_timer[1] = timerSvc->addItem("TL2VFindDCA");
      m_timer[2] = timerSvc->addItem("TL2VPrep");
      m_timer[3] = timerSvc->addItem("TL2VChi2");
      m_timer[4] = timerSvc->addItem("TL2VUpd");
      m_timer[5] = timerSvc->addItem("TL2VChi2C");
      m_timer[6] = timerSvc->addItem("TL2VUpdC");
    }
  ATH_MSG_INFO( "TrigL2VertexFitter constructed " );
  return StatusCode::SUCCESS;
}

StatusCode TrigL2VertexFitter::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

TrigL2VertexFitter::~TrigL2VertexFitter()
{

}

StatusCode TrigL2VertexFitter::fit(TrigL2Vertex* pV)
{
  int i,iter;
  bool fitFailed=false;

  if(m_timers) m_timer[0]->start();

  ATH_MSG_DEBUG("vertex has "<<pV->getTracks()->size()<<" tracks" );

  if(pV->getTracks()->size()<2)
    {
      ATH_MSG_WARNING("vertex has less than 2 tracks - fit failed" );
      if(m_timers) m_timer[0]->stop();
      return StatusCode::FAILURE;
    }

  std::list<TrigVertexFitInputTrack*>::iterator it,it1,it2;

  if (msgLvl(MSG::VERBOSE) )
    {
      ATH_MSG_VERBOSE("Track list:" );
      for(it=pV->getTracks()->begin();it!=pV->getTracks()->end();++it)
	{
	  ATH_MSG_VERBOSE((*it) );
	}
    }

  double V[3],dist=-999.0;
  it1=pV->getTracks()->begin();it2=it1;++it2;

  if(m_timers) m_timer[1]->start();
  dist=m_vertexingTool->FindClosestApproach((*it1),(*it2),V);
  if(m_timers) m_timer[1]->stop();

  if(dist<-1.0)
    {
      ATH_MSG_DEBUG("FindClosestApproach failed - fit failed" );
      if(m_timers) m_timer[0]->stop();
      return StatusCode::FAILURE;
    }

  if (msgLvl( MSG::DEBUG))
    {
      if ((*it1)->getTrigTrack() && (*it2)->getTrigTrack()) {
        ATH_MSG_DEBUG("Track 1 AlgId="<<(*it1)->getTrigTrack()->algorithmId() );
        ATH_MSG_DEBUG("Track 2 AlgId="<<(*it2)->getTrigTrack()->algorithmId() );
      }
      ATH_MSG_DEBUG("Min dist "<<dist<<" x="<<V[0]<<" y="<<V[1]<<" z="<<V[2] );
    }

  if(m_timers) m_timer[2]->start();
  pV->prepareForFit();
  for(it=pV->getTracks()->begin();it!=pV->getTracks()->end();++it)
    {
      (*it)->initializeVertex(pV);
    }
  for(i=0;i<3;i++) pV->getParametersVector()[i]=V[i];
  if(m_timers) m_timer[2]->stop();

  if(m_timers) 
    {
      m_timer[3]->start();
      m_timer[3]->pause();
      m_timer[4]->start();
      m_timer[4]->pause();
    }

  for(iter=0;iter<m_numIter;iter++)
  {
    pV->reset();
    pV->m_Gk[0][0]=100.0;
    pV->m_Gk[1][1]=100.0;
    pV->m_Gk[2][2]=400.0;
    for(it=pV->getTracks()->begin();it!=pV->getTracks()->end();++it)
      {
	if(m_timers) m_timer[3]->resume();
	double dchi2=(*it)->getChi2Distance(pV);
	if(m_timers) m_timer[3]->pause();

        ATH_MSG_VERBOSE("Track "<<(*it)->getIndex()<<" dchi2="<<dchi2 );
	if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
	  {
	    fitFailed=true;
	    break;
	  }
	pV->addChi2(dchi2);
	pV->addNdof(2);
	if(m_timers) m_timer[4]->resume();
	(*it)->updateVertex(pV);
	if(m_timers) m_timer[4]->pause();
        ATH_MSG_VERBOSE("Updated vertex" << pV);
      }
    if(fitFailed) break;
  }
  if(m_timers) 
    {
      m_timer[0]->stop();
      m_timer[1]->stop();
      m_timer[2]->stop();
      m_timer[3]->stop();
      m_timer[4]->stop();
    }
  if(!fitFailed) pV->setStatus(1);
  else return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

StatusCode TrigL2VertexFitter::fitWithConstraints(TrigL2Vertex* pV)
{
  int i,iter;
  bool fitFailed=false;

  if(m_timers) m_timer[0]->start();
  ATH_MSG_DEBUG("vertex has "<<pV->getTracks()->size()<<" tracks" );

  if(pV->getTracks()->size()<2)
    {
      ATH_MSG_WARNING("vertex has less than 2 tracks - fit failed" );
      if(m_timers) m_timer[0]->stop();
      return StatusCode::FAILURE;
    }

  std::list<TrigVertexFitInputTrack*>::iterator it,it1,it2;

  if (msgLvl(MSG::VERBOSE))
    {
      ATH_MSG_VERBOSE("Track list:" );
      for(it=pV->getTracks()->begin();it!=pV->getTracks()->end();++it)
	{
	  ATH_MSG_VERBOSE((*it) );
	}
    }

  double V[3],dist=-999.0;
  it1=pV->getTracks()->begin();it2=it1;++it2;

  if(m_timers) m_timer[1]->start();
  dist=m_vertexingTool->FindClosestApproach((*it1),(*it2),V);
  if(m_timers) m_timer[1]->stop();

  if(dist<-1.0)
    {
      ATH_MSG_DEBUG("vertex has 2 identical tracks - fit failed" );
      if(m_timers) m_timer[0]->stop();
      return StatusCode::FAILURE;
    }

  ATH_MSG_DEBUG("Min dist "<<dist<<" x="<<V[0]<<" y="<<V[1]<<" z="<<V[2] );
  if(m_timers) m_timer[2]->start();
  pV->prepareForFit();
  for(it=pV->getTracks()->begin();it!=pV->getTracks()->end();++it)
    {
      (*it)->initializeVertex(pV);
    }
  for(i=0;i<3;i++) pV->getParametersVector()[i]=V[i];
  if(m_timers) m_timer[2]->stop();
  if(m_timers) 
    {
      m_timer[3]->start();
      m_timer[3]->pause();
      m_timer[4]->start();
      m_timer[4]->pause();
    }
  for(iter=0;iter<m_numIter;iter++)
    {
      pV->reset();
      pV->m_Gk[0][0]=100.0;
      pV->m_Gk[1][1]=100.0;
      pV->m_Gk[2][2]=400.0;
      
      for(it=pV->getTracks()->begin();it!=pV->getTracks()->end();++it)
	{
	  m_timer[3]->resume();
	  double dchi2=(*it)->getChi2Distance(pV);
	  m_timer[3]->pause();
          ATH_MSG_VERBOSE("Track "<<(*it)->getIndex()<<" dchi2="<<dchi2 );
	  if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
	    {
	      fitFailed=true;
	      break;
	    }
	  pV->addChi2(dchi2);
	  pV->addNdof(2);
	  if(m_timers) m_timer[4]->resume();
	  (*it)->updateVertex(pV);
	  if(m_timers) m_timer[4]->pause();
          ATH_MSG_VERBOSE("Updated vertex" << pV);
	}
      if(fitFailed) break;
      for(std::list<TrigVertexFitConstraint*>::iterator itC=pV->getConstraints()->begin();
	  itC!=pV->getConstraints()->end();++itC)
	{
	  if(m_timers) m_timer[5]->resume();
	  double dchi2=(*itC)->getChi2Distance(pV);
	  if(m_timers) m_timer[5]->pause();
          ATH_MSG_VERBOSE("Constraint "<<(*itC)->getValue()<<" dchi2="<<dchi2 );
	  if((dchi2<0.0)||std::isnan(dchi2))
	    {
	      fitFailed=true;
	      break;
	    }
	  pV->addChi2(dchi2);
	  pV->addNdof(1);
	  if(m_timers) m_timer[6]->resume();
	  (*itC)->updateVertex(pV);
	  if(m_timers) m_timer[6]->pause();
          ATH_MSG_VERBOSE("Updated vertex" << pV);
	}
      if(fitFailed) break;
    }
  if(m_timers) 
    {
      m_timer[0]->stop();
      m_timer[1]->stop();
      m_timer[2]->stop();
      m_timer[3]->stop();
      m_timer[4]->stop();
      m_timer[5]->stop();
      m_timer[6]->stop();
    }
  if(!fitFailed) pV->setStatus(1);
  else return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}


