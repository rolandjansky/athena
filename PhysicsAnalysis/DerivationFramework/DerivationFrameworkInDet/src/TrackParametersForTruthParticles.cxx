/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParametersForTruthParticles.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Tomoe Kishimoto (Tomoe.Kishimoto@cern.ch)
// Wrapper around the passSelection() method of xAOD egamma
// Writes result to SG for later selection by string parser

#include "DerivationFrameworkInDet/TrackParametersForTruthParticles.h"
#include "xAODTruth/TruthParticleContainer.h"
#include <vector>
#include <string>

#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"

// Constructor
DerivationFramework::TrackParametersForTruthParticles::TrackParametersForTruthParticles( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_collTruthName("TruthParticles"),
  m_sgKey1(""),
  m_decoTool("InDetPhysValTruthDecoratorTool")
	{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TruthParticleContainerName", m_collTruthName);
    declareProperty("DecorationPrefix",m_sgKey1);
    declareProperty("PtCut",m_ptCut = 400);
    declareProperty("OnlyDressPrimaryTracks", m_doPrimary = true);
  }
 
// Destructor
DerivationFramework::TrackParametersForTruthParticles::~TrackParametersForTruthParticles() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::TrackParametersForTruthParticles::initialize()
{
  
  ATH_MSG_VERBOSE("initialize() ...");
  
	ATH_CHECK(m_decoTool.retrieve());

	return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TrackParametersForTruthParticles::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::TrackParametersForTruthParticles::addBranches() const
{

  // Get the track container
  const xAOD::TruthParticleContainer* tracks = 
    evtStore()->retrieve< const xAOD::TruthParticleContainer >(m_collTruthName);
  
  if(!tracks) {
    ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key: " << m_collTruthName);
    return StatusCode::FAILURE;
  }

  for ( const auto truth: *tracks) {
		if( truth->status() != 1 || truth->charge() == 0 ) continue;   
		if( m_doPrimary && truth->barcode() > 200e3 ) continue;
    if( truth->pt() < m_ptCut ) continue;
    m_decoTool->decorateTruth(*truth, m_sgKey1);
	}


  return StatusCode::SUCCESS;
}

