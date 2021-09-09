/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParametersAtPV.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Tomoe Kishimoto (Tomoe.Kishimoto@cern.ch)
// Wrapper around the passSelection() method of xAOD egamma
// Writes result to SG for later selection by string parser

#include "DerivationFrameworkInDet/TrackParametersAtPV.h"
#include <string>

// Constructor
DerivationFramework::TrackParametersAtPV::TrackParametersAtPV( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) :
  AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

// Destructor
DerivationFramework::TrackParametersAtPV::~TrackParametersAtPV() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TrackParametersAtPV::initialize()
{
   if (m_collTrackKey.key().empty() || m_collVertexKey.key().empty()) {
    ATH_MSG_ERROR("No selection variables for the TrackParametersAtPV tool!");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_collTrackKey.initialize() );
  ATH_CHECK( m_collVertexKey.initialize() );

  if (m_trackZ0PVKey.key().empty()) {
    ATH_MSG_ERROR("No Store Gate Keys for the TrackParametersAtPV tool!");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_trackZ0PVKey.initialize() );

  ATH_MSG_VERBOSE("initialize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TrackParametersAtPV::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::TrackParametersAtPV::addBranches() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::WriteHandle< std::vector<float> >track_z0_PV(m_trackZ0PVKey,ctx);
  ATH_CHECK(track_z0_PV.record(std::make_unique< std::vector<float> >()));

  // Get Primary vertex
  SG::ReadHandle<xAOD::VertexContainer> vertices(m_collVertexKey,ctx);
  if(!vertices.isValid()) {
    ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key: " << m_collVertexKey.key());
    return StatusCode::FAILURE;
  }

  const xAOD::Vertex* pv(nullptr);
  for (const xAOD::Vertex* vx : *vertices) {
    if (vx->vertexType() == xAOD::VxType::PriVtx) {
      pv = vx;
      break;
    }
  }

  // Get the track container
  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_collTrackKey,ctx);
  if(!tracks.isValid()) {
    ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key: " << m_collTrackKey.key());
    return StatusCode::FAILURE;
  }

  // Get track z0 w.r.t PV
  for (xAOD::TrackParticleContainer::const_iterator trackIt=tracks->begin(); trackIt!=tracks->end(); ++trackIt) {
    if (pv) {
      float z0wrtPV = (*trackIt)->z0() + (*trackIt)->vz() - pv->z();
      track_z0_PV->push_back(z0wrtPV);

    } else {
      track_z0_PV->push_back(999.);
    }
  }

  return StatusCode::SUCCESS;
}

