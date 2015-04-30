/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParametersAtPV.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Tomoe Kishimoto (Tomoe.Kishimoto@cern.ch)
// Wrapper around the passSelection() method of xAOD egamma
// Writes result to SG for later selection by string parser

#include "DerivationFrameworkInDet/TrackParametersAtPV.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TrackParametersAtPV::TrackParametersAtPV( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_collTrackName("InDetTrackParticles"),
  m_collVertexName("PrimaryVertices"),
  m_sgKey1("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackParticleContainerName", m_collTrackName);
    declareProperty("VertexContainerName", m_collVertexName);
    declareProperty("Z0SGEntryName", m_sgKey1);
  }
 
// Destructor
DerivationFramework::TrackParametersAtPV::~TrackParametersAtPV() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::TrackParametersAtPV::initialize()
{
  if (m_collTrackName == "" || m_collVertexName == "") {
    ATH_MSG_ERROR("No selection variables for the TrackParametersAtPV tool!");
    return StatusCode::FAILURE;
  }
  
  if (m_sgKey1 == "") {
    ATH_MSG_ERROR("No Store Gate Keys for the TrackParametersAtPV tool!");
    return StatusCode::FAILURE;
  }
  
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
  std::unique_ptr<std::vector<float> > track_z0_PV(new std::vector<float>());  

  // Get Primary vertex
  const xAOD::VertexContainer* vertices = 
    evtStore()->retrieve< const xAOD::VertexContainer >(m_collVertexName);
    
  if(!vertices) {
    ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key: " << m_collVertexName);
    return StatusCode::FAILURE;
  }

  const xAOD::Vertex* pv(0);
  for (const xAOD::Vertex* vx : *vertices) {
    if (vx->vertexType() == xAOD::VxType::PriVtx) {
      pv = vx;
      break;
    }
  }

  // Get the track container
  const xAOD::TrackParticleContainer* tracks = 
    evtStore()->retrieve< const xAOD::TrackParticleContainer >(m_collTrackName);
  
  if(!tracks) {
    ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key: " << m_collTrackName);
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

  // Write decision to SG for access by downstream algs 
  if (evtStore()->contains<std::vector<float> >(m_sgKey1)) {
      ATH_MSG_ERROR("Tool is attempting to write StoreGate keys which already exists. Please use a different key");
      return StatusCode::FAILURE;
  } else {
    CHECK(evtStore()->record(std::move(track_z0_PV), m_sgKey1));       
  }

  return StatusCode::SUCCESS;
}

