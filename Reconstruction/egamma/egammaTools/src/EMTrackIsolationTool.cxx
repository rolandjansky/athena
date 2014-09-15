/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMTrackIsolationTool.h"
#include "EventPrimitives/EventPrimitives.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "IsolationTool/ITrackIsolationTool.h"
#include "xAODPrimitives/IsolationType.h"
// #include "xAODCaloEvent/CaloCluster.h"
// #include "CaloUtils/CaloClusterVertexCorrection.h"


// ==============================================================
EMTrackIsolationTool::EMTrackIsolationTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  //
  // default constructor
  //

  ATH_MSG_DEBUG( "in EMTrackIsolationTool::EMTrackIsolationTool()" );

  // declare interface
  declareInterface<IEMTrackIsolationTool>(this);

  // track isolation tool
  declareProperty( "TrackIsolationTool", m_trackIsolationTool,
		   "the track isolation tool" );

  // primary vertex container name
  declareProperty( "VertexContainerName", 
		   m_vertexContainerName="PrimaryVertices",
		   "primary vertex container name" );

  //To be used for brem associations
  declareProperty("useBremAssoc", 
		  m_useBremAssoc=false,
		  "use track to track assoc after brem");
  




}

// =================================================================
EMTrackIsolationTool::~EMTrackIsolationTool()
{
  //
  // destructor
  //
  //ATH_MSG_DEBUG( " in EMTrackIsolationTool::~EMTrackIsolationTool()" );
}


// ==================================================================
StatusCode EMTrackIsolationTool::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG( " in EMTrackIsolationTool::initialize()" );

  // retrieve TrackIsolationTool
  if ( m_trackIsolationTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Can't get a handle on the TrackIsolationTool" );
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG( m_trackIsolationTool << "retrieved" );
  
  return StatusCode::SUCCESS;
}

// ==================================================================
StatusCode EMTrackIsolationTool::execute(xAOD::Egamma* eg,const xAOD::TrackParticleContainer* aTrackParticleContainer /*= 0*/)
{
  //
  // execute method starting simply from egamma object and EMShower detail
  //

  ATH_MSG_DEBUG( " in EMTrackIsolationTool::execute()" );

  // protection against bad pointers
  if (eg==0) return StatusCode::SUCCESS;
  
  xAOD::TrackIsolation result;
  const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc);
  
  
  const std::vector<xAOD::Iso::IsolationType> isolationTypes{
    xAOD::Iso::ptcone40, xAOD::Iso::ptcone30, xAOD::Iso::ptcone20
  };
    
  //This is needed due to the order of the arguments in the underlying tool
  xAOD::Vertex *vx = 0;

  

  if (!m_trackIsolationTool->trackIsolation(result, *eg, isolationTypes,
					    xAOD::ITrackIsolationTool::UsePointer, vx, 
					    &tracksToExclude,aTrackParticleContainer ) 
      || result.ptcones.size() != 3 || result.nucones.size() != 3) {
    ATH_MSG_WARNING("TrackIsolationTool failed");
    return StatusCode::SUCCESS;
  }
  
  //The m_trackIsolationTool always returns from the larger to the smaller cone

  m_ptcone20 = result.ptcones[2];
  m_ptcone30 = result.ptcones[1];
  m_ptcone40 = result.ptcones[0];
  m_nucone20 = result.nucones[2];
  m_nucone30 = result.nucones[1];
  m_nucone40 = result.nucones[0];
    
  return StatusCode::SUCCESS;
}


