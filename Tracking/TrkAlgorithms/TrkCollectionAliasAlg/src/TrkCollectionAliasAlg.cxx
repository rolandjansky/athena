/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkCollectionAliasAlg.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkCollectionAliasAlg/TrkCollectionAliasAlg.h"

//================ Constructor =================================================

Trk::TrkCollectionAliasAlg::TrkCollectionAliasAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator)
{
  //  template for property decalration
  declareProperty("CollectionName", m_collectionName = "ResolvedTracks");
  declareProperty("AliasName"     , m_aliasName      = "Tracks");
}

//================ Destructor =================================================

Trk::TrkCollectionAliasAlg::~TrkCollectionAliasAlg()
= default;


//================ Initialisation =================================================

StatusCode Trk::TrkCollectionAliasAlg::initialize()
{
  ATH_CHECK( m_collectionName.initialize() );
  ATH_CHECK( m_aliasName.initialize() );

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::TrkCollectionAliasAlg::finalize()
{
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::TrkCollectionAliasAlg::execute(const EventContext& ctx) const
{
  SG::ReadHandle<TrackCollection> tracks(m_collectionName, ctx );
  return tracks.alias(m_aliasName);
}

//============================================================================================

