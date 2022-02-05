/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackPRD_Association/InDetTrackPRD_Association.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::InDetTrackPRD_Association::InDetTrackPRD_Association
(const std::string& name,ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode InDet::InDetTrackPRD_Association::initialize()
{


  ATH_CHECK(m_tracksName.initialize());
  ATH_CHECK(m_assoMapName.initialize());
  ATH_CHECK(m_assoTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetTrackPRD_Association::execute(const EventContext& ctx) const
{
  // Assosiate tracks with PRDs
  //
  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map(m_assoTool->createPRDtoTrackMap());

  for (const SG::ReadHandleKey<TrackCollection>& collKey : m_tracksName) {
    SG::ReadHandle<TrackCollection> tracks(collKey,ctx);
    assert (tracks.isValid());

    unsigned tracksPRD=0;
    unsigned tracksPRDn=0;

    for  (const Trk::Track* t : *tracks) {
      if((m_assoTool->addPRDs(*prd_to_track_map,*t)).isFailure())
        ++tracksPRDn;
      else
        ++tracksPRD;

    }
    ATH_MSG_DEBUG("Collection " << collKey.key() << ": tracks with PRD "<< tracksPRD
                  << ",  without PRD " << tracksPRDn);
  }
  SG::WriteHandle<Trk::PRDtoTrackMap> write_handle(m_assoMapName, ctx);
  if (write_handle.record( m_assoTool->reduceToStorableMap(std::move(prd_to_track_map))).isFailure()) {
    ATH_MSG_FATAL("Failed to add PRD to track association map.");
  }
  return StatusCode::SUCCESS;
}

StatusCode InDet::InDetTrackPRD_Association::finalize() {return StatusCode::SUCCESS;}
