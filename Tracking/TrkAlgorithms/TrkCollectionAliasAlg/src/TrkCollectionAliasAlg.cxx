/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkCollectionAliasAlg.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkCollectionAliasAlg/TrkCollectionAliasAlg.h"
#include "TrkTrack/TrackCollection.h"

//================ Constructor =================================================

Trk::TrkCollectionAliasAlg::TrkCollectionAliasAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
  //  template for property decalration
  declareProperty("CollectionName", m_collectionName = "ResolvedTracks");
  declareProperty("AliasName"     , m_aliasName      = "Tracks");
}

//================ Destructor =================================================

Trk::TrkCollectionAliasAlg::~TrkCollectionAliasAlg()
{}


//================ Initialisation =================================================

StatusCode Trk::TrkCollectionAliasAlg::initialize()
{
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::TrkCollectionAliasAlg::finalize()
{
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::TrkCollectionAliasAlg::execute()
{
  const TrackCollection* trkColPtr=0;
  StatusCode sc;
    
  sc = evtStore()->retrieve(trkColPtr,m_collectionName);
  if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not retrieve TrackCollection " << m_collectionName << endreq;
      return StatusCode::FAILURE;
    }

  sc = evtStore()->setAlias(trkColPtr,m_aliasName);
  if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not alias TrackCollection " << m_collectionName << " to " << m_aliasName << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

//============================================================================================

