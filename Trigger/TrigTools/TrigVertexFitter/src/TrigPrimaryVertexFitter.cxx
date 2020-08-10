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

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigInDetEvent/TrigL2Vertex.h"
#include "TrigInDetEvent/TrigVertex.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrigInDetToolInterfaces/ITrigPrimaryVertexFitter.h"
#include "TrigPrimaryVertexFitter.h"
#include "TrigPrimaryVertexTrack.h"


TrigPrimaryVertexFitter::TrigPrimaryVertexFitter(const std::string& t, 
						 const std::string& n,
						 const IInterface*  p ): AthAlgTool(t,n,p), 
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
  ATH_CHECK( AthAlgTool::initialize() );

  ATH_MSG_INFO( "Number of iterations is set to " << m_numIter  );
  ATH_MSG_INFO( "Accuracy of starting z-position is " << m_zVariance<<" mm"  );
  ATH_MSG_INFO( "Track lists created ? " << std::boolalpha<<m_createTrackLists );
  return StatusCode::SUCCESS;
}

StatusCode TrigPrimaryVertexFitter::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

TrigPrimaryVertexFitter::~TrigPrimaryVertexFitter()
{

}

//TrigVertex has associated TrigInDetTracks, but we want Trk::Tracks
TrigVertex* TrigPrimaryVertexFitter::fit(const TrackCollection* tc, TrackCollection& output_tc, double z0) const
{

  TrigVertex* pVertex=NULL;

  double chi2;
  int ndof;

  std::vector<TrigPrimaryVertexTrack*> tracks;
  tracks.clear();
  int idx=1;
  for(TrackCollection::const_iterator ptIt=tc->begin();ptIt!=tc->end();++ptIt)
  {
    TrigPrimaryVertexTrack* pT= new TrigPrimaryVertexTrack(*ptIt);
    if(pT!=NULL) {
      pT->setIndex(idx++);
      tracks.push_back(pT);
    }
  }

  TrigL2Vertex* pV = new TrigL2Vertex();

  pV->getParametersVector()[0]=0.0;
  pV->getParametersVector()[1]=0.0;
  pV->getParametersVector()[2]=z0;
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
      if(!(*it)->isActive()) 
        (*it)->activate();

      double dchi2=(*it)->getChi2Distance(pV);
      ATH_MSG_VERBOSE("Track "<<(*it)->getIndex()<<" dchi2="<<dchi2 );
      if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
      {
        fitFailed=true;
        break;
      }
      if(dchi2<m_chi2cut)
      {
        chi2+=dchi2;ndof+=2;
        (*it)->updateVertex(pV);
      }
      else
      {
        (*it)->mask();
        ATH_MSG_DEBUG("Skipping track "<<(*it)->getIndex()<<"due to large dchi2="<<dchi2 );
      }
    }
    if(fitFailed) break;
  }


  if(!fitFailed && (ndof>0))
  {
    ATH_MSG_DEBUG("Primary vertex fit OK, chi2 = "<<chi2<<" NDOF = "<<ndof );
    ATH_MSG_DEBUG("x= "<<pV->getParametersVector()[0]<<
        " y= "<<pV->getParametersVector()[1]<<" z= "<<pV->getParametersVector()[2] );
    double cv[6];
    cv[0]=pV->m_Gk[0][0];cv[1]=pV->m_Gk[0][1];cv[2]=pV->m_Gk[1][1];
    cv[3]=pV->m_Gk[0][2];cv[4]=pV->m_Gk[1][2];cv[5]=pV->m_Gk[2][2];		
    if(m_createTrackLists) {
      output_tc.clear();
      output_tc.reserve(tracks.size());

      for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it) {
        if ((*it)->isActive()) 
          output_tc.push_back(new Trk::Track(*(*it)->getTrkTrack()));
      }
    }
    pVertex = new TrigVertex(pV->getParametersVector()[0],
        pV->getParametersVector()[1],
        pV->getParametersVector()[2],cv,chi2,ndof, nullptr);
  }
  delete pV;
  for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
  {
    delete (*it);
  }
  return pVertex;
}

struct TrigPrimaryTrackSortPredicate
{
  bool operator()(TrigPrimaryVertexTrack* pA, TrigPrimaryVertexTrack* pB)
  {
    return (pA->getChi2Contribution() < pB->getChi2Contribution());
  }
};

TrigVertex* TrigPrimaryVertexFitter::fit(const TrackCollection* tc, std::vector<double>& Chi2V, double z0) const
{

  TrigVertex* pVertex=NULL;

  double chi2;
  int ndof;

  std::vector<TrigPrimaryVertexTrack*> tracks;
  tracks.clear();
  Chi2V.clear();
  int idx=1;
  for(TrackCollection::const_iterator ptIt=tc->begin();ptIt!=tc->end();++ptIt)
  {
    TrigPrimaryVertexTrack* pT= new TrigPrimaryVertexTrack(*ptIt);
    if(pT!=NULL)
      {
	pT->setIndex(idx++);
	tracks.push_back(pT);
	Chi2V.push_back(-100.0);
      }
  }

  TrigL2Vertex* pV = new TrigL2Vertex();

  // 1. preliminary fit

  pV->getParametersVector()[0]=0.0;
  pV->getParametersVector()[1]=0.0;
  pV->getParametersVector()[2]=z0;

  memset(&pV->m_Gk[0][0],0,sizeof(pV->m_Gk));
  pV->m_Gk[0][0]=m_xyVariance*m_xyVariance;
  pV->m_Gk[1][1]=m_xyVariance*m_xyVariance;
  pV->m_Gk[2][2]=m_zVariance*m_zVariance;

  bool fitFailed=false;
  for(std::vector<TrigPrimaryVertexTrack*>::iterator it=tracks.begin();it!=tracks.end();++it)
    {
      double dchi2=(*it)->getChi2Distance(pV);
      if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
	{
	  fitFailed=true;
	  break;
	}
      if(dchi2<m_chi2cut_loose) (*it)->updateVertex(pV);
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
      double dchi2=(*it)->getChi2Distance(pV);
      ATH_MSG_DEBUG("Track "<<(*it)->getIndex()<<" dchi2="<<dchi2 );
    }  

  // 3. sort tracks in accordance with their chi2s

  std::sort(tracks.begin(),tracks.end(),TrigPrimaryTrackSortPredicate());

  // 4. Final track fit

  pV->getParametersVector()[0]=0.0;
  pV->getParametersVector()[1]=0.0;
  pV->getParametersVector()[2]=z0;
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
	  double dchi2=(*it)->getChi2Distance(pV);
	  Chi2V[(*it)->getIndex()-1]=dchi2;
          ATH_MSG_VERBOSE("Track "<<(*it)->getIndex()<<" dchi2="<<dchi2 );
	  if(std::isnan(dchi2)||(dchi2<0.0)||(dchi2>m_maxChi2Increase))
	    {
	      fitFailed=true;
	      break;
	    }
	  if(dchi2<m_chi2cut)
	    {
	      chi2+=dchi2;ndof+=2;
	      (*it)->updateVertex(pV);
	    }
	  else
	    {
              ATH_MSG_DEBUG("Skipping track "<<(*it)->getIndex()<<"due to large dchi2="<<dchi2 );
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
      ATH_MSG_DEBUG("Primary vertex fit OK, chi2 = "<<chi2<<" NDOF = "<<ndof );
      ATH_MSG_DEBUG("x= "<<pV->getParametersVector()[0]<<
                    " y= "<<pV->getParametersVector()[1]<<" z= "<<pV->getParametersVector()[2] );
      double cv[6];
      cv[0]=pV->m_Gk[0][0];cv[1]=pV->m_Gk[0][1];cv[2]=pV->m_Gk[1][1];
      cv[3]=pV->m_Gk[0][2];cv[4]=pV->m_Gk[1][2];cv[5]=pV->m_Gk[2][2];		
      pVertex = new TrigVertex(pV->getParametersVector()[0],
			       pV->getParametersVector()[1],
			       pV->getParametersVector()[2],cv,chi2,ndof,NULL);
    }
  delete pV;

  return pVertex;
}

