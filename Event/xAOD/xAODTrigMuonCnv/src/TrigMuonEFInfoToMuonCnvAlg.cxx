/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  TrigMuonEFInfoToMuonCnvAlg.cxx
//
//  Created by Mark Owen on 13/03/2014.
//

#include "TrigMuonEFInfoToMuonCnvAlg.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

using namespace xAODMaker;

TrigMuonEFInfoToMuonCnvAlg::TrigMuonEFInfoToMuonCnvAlg(const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc ),
    m_converterTool("TrigMuonEFInfoMuonCnvTool/TrigMuonEFInfoMuonCnvTool"),
    m_nMuonsCreated(0),
    m_nCombTrksCreated(0),
    m_nExtrapTrksCreated(0),
    m_nproc(0)
{
    declareProperty( "AODContainerName", m_aodContainerName = "HLT_MuonEFInfo" ); // need to fix the name
    declareProperty( "xAODMuonContainerName", m_xaodMuonContainerName = "HLT_Muons" );
    declareProperty( "xAODCombinedTrackParticleContainerName", m_xaodCombTrkPartContainerName = "HLT_MuonCombinedTrackParticles" );
    declareProperty( "xAODExtrapolatedTrackParticleContainerName", m_xaodExtrapTrkPartContainerName = "HLT_MuonExtrapolatedTrackParticles" );
    declareProperty( "ConverterTool", m_converterTool );
}

StatusCode TrigMuonEFInfoToMuonCnvAlg::initialize() {

  // print some settings
  ATH_MSG_INFO( "AODContainerName  = " << m_aodContainerName << ",  xAODMuonContainerName = " << m_xaodMuonContainerName );
  ATH_MSG_INFO( "xAODCombinedTrackParticleContainerName = " << m_xaodCombTrkPartContainerName << ", xAODExtrapolatedTrackParticleContainerName = " << m_xaodExtrapTrkPartContainerName);
  ATH_MSG_INFO( "ConverterTool = " << m_converterTool.typeAndName());
  
  // retrieve converter tool
  ATH_CHECK(m_converterTool.retrieve());
  
  // Return gracefully
  return StatusCode::SUCCESS;
}

StatusCode TrigMuonEFInfoToMuonCnvAlg::finalize() {
  ATH_MSG_INFO("Statistics for conversion:");
  ATH_MSG_INFO("Successfully processed " << m_nproc << " events");
  if(m_nproc>0) {
    ATH_MSG_INFO("Created " << m_nMuonsCreated << " xAOD::Muons, average per event = " << ((double)m_nMuonsCreated)/((double)m_nproc));
    ATH_MSG_INFO("Created " << m_nCombTrksCreated << " combined xAOD::TrackParticles, average per event = " << ((double)m_nCombTrksCreated)/((double)m_nproc));
    ATH_MSG_INFO("Created " << m_nExtrapTrksCreated << " extrapolated xAOD::TrackParticles, average per event = " << ((double)m_nExtrapTrksCreated)/((double)m_nproc));
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigMuonEFInfoToMuonCnvAlg::execute() {
    
    // Retrieve the AOD particles:
    const TrigMuonEFInfoContainer* aod = evtStore()->tryConstRetrieve<TrigMuonEFInfoContainer>(m_aodContainerName);
    if (!aod) {
        ATH_MSG_WARNING("No TrigMuonEFInfoContainer with key " << m_aodContainerName << " found. Do nothing.");
        return StatusCode::SUCCESS;
    }
    
    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerName );
    
    // Create the xAOD muon container and its auxiliary store:
    xAOD::MuonContainer* xaodMuon = new xAOD::MuonContainer();
    xAOD::MuonAuxContainer* auxMuon = new xAOD::MuonAuxContainer();
    ATH_CHECK(recordxAODContainers( m_xaodMuonContainerName, m_xaodMuonContainerName + "Aux.", *xaodMuon, *auxMuon));
    
    // Create the xAOD track particle containers and auxiliary stores:
    xAOD::TrackParticleContainer* xaodCombTrackParticles = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer* auxCombTrack = new xAOD::TrackParticleAuxContainer();
    ATH_CHECK(recordxAODContainers( m_xaodCombTrkPartContainerName, m_xaodCombTrkPartContainerName + "Aux.", *xaodCombTrackParticles, *auxCombTrack));
    xAOD::TrackParticleContainer* xaodExtrapTrackParticles = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer* auxExtrapTrack = new xAOD::TrackParticleAuxContainer();
    ATH_CHECK(recordxAODContainers( m_xaodExtrapTrkPartContainerName, m_xaodExtrapTrkPartContainerName + "Aux.", *xaodExtrapTrackParticles, *auxExtrapTrack));

    // call the converter tool which does the work
    StatusCode sc = m_converterTool->convertTrigMuonEFInfoContainer( *aod, *xaodMuon, xaodCombTrackParticles, xaodExtrapTrackParticles);
    if(sc.isFailure()) {
        ATH_MSG_ERROR("Converter tool failed");
        return sc;
    }
    // update our counters
    ++m_nproc;
    m_nMuonsCreated += xaodMuon->size();
    m_nCombTrksCreated += xaodCombTrackParticles->size();
    m_nExtrapTrksCreated += xaodExtrapTrackParticles->size();

    return StatusCode::SUCCESS;

}//execute
