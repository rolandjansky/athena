/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackPRD_Association/InDetTrackPRD_Association.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::InDetTrackPRD_Association::InDetTrackPRD_Association
(const std::string& name,ISvcLocator* pSvcLocator) : 

  AthAlgorithm(name, pSvcLocator)                           ,
  m_assoTool  ("InDet::InDetPRD_AssociationToolGangedPixels")
{
  // InDetTrackPRD_Association steering parameters
  //
  declareProperty("AssociationTool", m_assoTool  );
  declareProperty("TracksName"     , m_tracksName);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::InDetTrackPRD_Association::initialize() 
{
 

  ATH_CHECK(m_tracksName.initialize());
  // Get association tool
  //
  if( m_assoTool.retrieve().isFailure()) {

    msg(MSG::FATAL)<< "Failed to retrieve tool " << m_assoTool << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endmsg;
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::InDetTrackPRD_Association::execute() 
{ 


  // Assosiate tracks with PRDs
  //
  m_assoTool->reset();

  for (const SG::ReadHandleKey<TrackCollection>& collKey : m_tracksName) {
    SG::ReadHandle<TrackCollection> tracks(collKey);
    
    unsigned tracksPRD=0;
    unsigned tracksPRDn=0;

    for  (const Trk::Track* t : *tracks) {  
      if((m_assoTool->addPRDs(*t)).isFailure()) 
	++tracksPRDn; 
      else 
	++tracksPRD;
    }
    ATH_MSG_DEBUG("Collection " << collKey.key() << ": tracks with PRD " << tracksPRD << ",  without PRD " << tracksPRD);
  } 

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::InDetTrackPRD_Association::finalize() {return StatusCode::SUCCESS;}
