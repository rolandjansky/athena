/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleCaloExtensionAlg.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"

// Constructor with parameters:
TrackParticleCaloExtensionAlg::TrackParticleCaloExtensionAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
  m_caloCellAssociationTool(""),
  m_trackSelector(""),
  m_nevents(0),
  m_totalTracks(0),
  m_processedTracks(0),
  m_extensions(0)
 {
  
  declareProperty("TrackParticleContainerName", m_trackParicleContainerName="InDetTrackParticles");
  declareProperty("ParticleCaloExtensionTool", m_caloExtensionTool);
  declareProperty("ParticleCaloCellAssociationTool", m_caloCellAssociationTool);
  declareProperty("TrackParticleSelector", m_trackSelector);
}

// Initialize method:
StatusCode TrackParticleCaloExtensionAlg::initialize() {
  if( !m_trackSelector.empty() )           ATH_CHECK(m_trackSelector.retrieve());
  if( !m_caloExtensionTool.empty() )       ATH_CHECK(m_caloExtensionTool.retrieve());
  if( !m_caloCellAssociationTool.empty() ) ATH_CHECK(m_caloCellAssociationTool.retrieve());
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode TrackParticleCaloExtensionAlg::execute() {

  ++m_nevents;
  // retrieve track partic;es
  const xAOD::TrackParticleContainer* trackParticles = 0;
  if(evtStore()->contains<xAOD::TrackParticleContainer>(m_trackParicleContainerName)) {
    if(evtStore()->retrieve(trackParticles,m_trackParicleContainerName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_trackParicleContainerName );
      return StatusCode::FAILURE;
    }
  }
	 
  if( !trackParticles ){
    ATH_MSG_WARNING("TrackParticles tracks not found at " << m_trackParicleContainerName );
    return StatusCode::SUCCESS;
  }

  m_totalTracks += trackParticles->size();

  // loop over track particles 
  for( auto tp : *trackParticles ){

    // track selection
    if( !m_trackSelector.empty() && !m_trackSelector->decision(*tp) ) continue;
    ++m_processedTracks;

    // // call calo Extension
    // if( !m_caloExtensionTool.empty() ){
    //   const Trk::CaloExtension* extension = m_caloExtensionTool->caloExtension(const_cast<xAOD::TrackParticle&>(*tp));
    //   if( extension ) ++m_extensions;

    //   if( msgLvl(MSG::DEBUG) && extension && extension->numberOfParameters() != 0 ){
    //     ATH_MSG_DEBUG("Got calo extension " << extension->numberOfParameters() << " cells " << extension->caloCells().size() );
    //     for( unsigned int i=0;i<extension->numberOfParameters();++i){
    //       xAOD::CurvilinearParameters_t pars = extension->trackParameters(i);
    //       unsigned int id = extension->parameterIdentifier(i);
    //       ATH_MSG_DEBUG( " id " << id << " pos: r " << sqrt(pars[0]*pars[0]+pars[1]*pars[1]) << " z " << pars[2]
    //                      << " momentum " << sqrt(pars[3]*pars[3]+pars[4]*pars[4]+pars[5]*pars[5]) );
    //     }
    //   }
    // }
    // // call calo cell association
    // if( !m_caloCellAssociationTool.empty() ){
    //   const xAOD::ParticleCaloExtension* extension = m_caloCellAssociationTool->caloAssociation(const_cast<xAOD::TrackParticle&>(*tp));
    //   if( msgLvl(MSG::DEBUG) && extension && extension->numberOfParameters() != 0 ){
    //     ATH_MSG_DEBUG("Got calo cell extension " << extension->numberOfParameters() << " cells " << extension->caloCells().size() );
    //   }
    // }
  }

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode TrackParticleCaloExtensionAlg::finalize() {
  float scale = m_nevents!=0 ? 1./m_nevents : 1;
  float scaleTrk = m_totalTracks!=0 ? 1./m_totalTracks : 1;
  ATH_MSG_INFO("Processed events " << m_nevents << " average number of tracks " << scale*m_totalTracks
               << " fraction processed tracks " << scaleTrk*m_processedTracks
               << " fraction with extension " << scaleTrk*m_extensions);
  return StatusCode::SUCCESS;
}

