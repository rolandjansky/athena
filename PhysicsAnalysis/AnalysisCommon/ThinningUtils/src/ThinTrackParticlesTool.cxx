/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        ThinTrackParticlesTool

Author:      Karsten Koeneke
Created:     July 2013
******************************************************************************/

// This class' header
#include "ThinTrackParticlesTool.h"

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "ExpressionEvaluation/StackElement.h"
#include "StoreGate/ThinningHandle.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

// EDM includes
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/ObjectType.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"



//=============================================================================
// Constructor
//=============================================================================
ThinTrackParticlesTool::ThinTrackParticlesTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent ) :
  ThinTrackParticlesToolBase( type, name, parent ),
  m_tauConversion(false),
  m_tauWide(false),
  m_tauOther(false),
  m_nElectronPTMax(-1),
  m_nEventsProcessed(0),
  m_nTrackPartsKept(0),
  m_nTrackPartsProcessed(0)
{
  declareInterface< DerivationFramework::IThinningTool >(this);
}



//=============================================================================
// Destructor
//=============================================================================
ThinTrackParticlesTool::~ThinTrackParticlesTool()
{
}


//=============================================================================
// Athena's initialize
//=============================================================================
StatusCode ThinTrackParticlesTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  // Print out the used configuration
  ATH_MSG_DEBUG ( " using " << m_streamName );
  ATH_MSG_DEBUG ( " using " << m_trackParticleKey );
  ATH_MSG_DEBUG ( " using " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using " << m_selection );
  ATH_MSG_DEBUG ( " using " << m_tauConversion );
  ATH_MSG_DEBUG ( " using " << m_tauWide );
  ATH_MSG_DEBUG ( " using " << m_tauOther );
  ATH_MSG_DEBUG ( " using " << m_nElectronPTMax );

  if (m_streamName.empty()) {
    ATH_MSG_ERROR( "StreamName property has not been initialized" );
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_trackParticleKey.initialize (m_streamName) );
  ATH_CHECK( m_inCollKeyList.initialize() );

  ATH_CHECK( initializeParser( m_selection.value()) );

  // Initialize the counters
  m_nEventsProcessed = 0;
  m_nTrackPartsKept = 0;
  m_nTrackPartsProcessed = 0;

  return StatusCode::SUCCESS;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ThinTrackParticlesTool::finalize()
{
  // Print messages
  ATH_MSG_DEBUG ( "==> finalize " << name() << "..." );
  ATH_MSG_DEBUG ( " Number of processed events:          " << m_nEventsProcessed );
  ATH_MSG_DEBUG ( " Number of processed TrackParticles:  " << m_nTrackPartsProcessed );
  ATH_MSG_DEBUG ( " Number of TrackParticles kept:       " << m_nTrackPartsKept );

  ATH_CHECK( finalizeParser() );

  return StatusCode::SUCCESS;
}




//=============================================================================
// The main method from the inherited interface
//=============================================================================
StatusCode ThinTrackParticlesTool::doThinning() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event
  ATH_MSG_DEBUG ( "==> doThinning " << name() << " on " << m_nEventsProcessed << ". event..." );

  // retrieve the TrackParticle Container
  SG::ThinningHandle<xAOD::TrackParticleContainer> trackParticleContainer (m_trackParticleKey, ctx);
  ATH_MSG_DEBUG ( "Container '" << m_trackParticleKey.key() << "' retrieved from StoreGate" );
  size_t nTotalTrackParts  = trackParticleContainer->size();

  // Create the mask to be used for thinning
  std::vector<bool> mask(nTotalTrackParts, false);

  // Now, retrieve the other containers that are holding the information which
  // TrackParticles should be kept
  for ( const SG::ReadHandleKey<SG::AuxVectorBase>& inKey : m_inCollKeyList ) {

    SG::ReadHandle<SG::AuxVectorBase> inObj (inKey, ctx);

    // First check for an xAOD::IParticleLinkContainer
    if (const auto* inLinkContainer = dynamic_cast<const xAOD::IParticleLinkContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input link collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const xAOD::IParticleLink& partLink : *inLinkContainer ) {
        ATH_CHECK( this->selectFromIParticleLink( mask, trackParticleContainer.cptr(), partLink ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::ElectronContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromElectron( mask, trackParticleContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::PhotonContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromPhoton( mask, trackParticleContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::MuonContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromMuon( mask, trackParticleContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::TauJetContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromTauJet( mask, trackParticleContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::JetContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromJet( mask, trackParticleContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::CompositeParticleContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromCompositeParticle( mask, trackParticleContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::VertexContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* vertex : *inContainer ) {
        ATH_CHECK( this->selectFromVertex( mask, trackParticleContainer.cptr(), vertex ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::IParticleContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated TrackParticles
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromIParticle( mask, trackParticleContainer.cptr(), part ) );
      }
    }

    else {
      if ( m_nEventsProcessed <= 10 ) {
        ATH_MSG_WARNING ( "Input collection  = '" << inKey
                          << "' could not be retrieved from StoreGate! "
                          << " This message will only be repeated 10 times..." );
      }
      else {
        ATH_MSG_DEBUG ( "Input collection  = '" << inKey
                        << "' could not be retrieved from StoreGate! " );
      }
      return StatusCode::SUCCESS;
    } // End: if/elif/else is link container
  } // End: loop over input container names

  // Try to fill the thinning mask based on the selection string, if given
  if ( !(m_selection.value().empty()) ) {
    ATH_CHECK( this->selectFromString( mask, trackParticleContainer.cptr() ) );
  }

  // Some final debug printout
  if ( msgLvl(MSG::DEBUG) || msgLvl(MSG::VERBOSE) ) {
    std::size_t nKept = 0;
    for ( bool isKept : mask ){
      if (isKept) nKept += 1;
    }
    m_nTrackPartsKept += static_cast<unsigned long>(nKept);
    m_nTrackPartsProcessed += static_cast<unsigned long>(mask.size());
    ATH_MSG_DEBUG("Got " << mask.size() << " TrackParticles. Retained " << nKept << " TrackParticles" );
  }

  // Perform the actual thinning
  trackParticleContainer.keep (mask);

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::IParticleLink
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromIParticleLink( std::vector<bool>& mask,
                                                 const xAOD::TrackParticleContainer* trackParticleContainer,
                                                 const xAOD::IParticleLink& partLink ) const
{
  ATH_MSG_VERBOSE("In selectFromIParticleLink");
  if ( !(partLink.isValid()) ) {
    ATH_MSG_WARNING("Got an invalid element link. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, check that the element link points to the given track particle container
  const xAOD::IParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
  // const xAOD::TrackParticleContainer* ptrTrkPartCont =
  //     dynamic_cast< const xAOD::TrackParticleContainer* >( partLink.getStorableObjectPointer() );
  if ( ptrTrkPartCont == trackParticleContainer ) {
    ATH_MSG_DEBUG("The current ElementLink points to the TrackParticleContainer");
    // Get the index of the current element link
    std::size_t index = partLink.index();

    // Fill the thinning mask at this place and increment the counter
    if ( index >= mask.size() ) {
      ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
      return StatusCode::SUCCESS;
    }
    mask[index] = true;
    return StatusCode::SUCCESS;
  }

  // If we don't directly have an element link to the relevant TrackParticleContainer,
  // delegate this to the generic IParticle method
  ATH_CHECK( this->selectFromIParticle( mask, trackParticleContainer, *partLink ) );

  return StatusCode::SUCCESS;
}






//=============================================================================
// Thinning method for xAOD::IParticle
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromIParticle( std::vector<bool>& mask,
                                             const xAOD::TrackParticleContainer* trackParticleContainer,
                                             const xAOD::IParticle* part ) const
{
  ATH_MSG_VERBOSE("In selectFromIParticle");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Try to figure out what type of particle we have and delegate accordingly
  const xAOD::Type::ObjectType objType = part->type();
  if ( objType == xAOD::Type::ObjectType::TrackParticle ) {
    // Now, check that we point to the given track particle container
    if ( part->container() == trackParticleContainer ) {
      ATH_MSG_DEBUG("The current IParticle points to the TrackParticleContainer");
      // Get the index of the current element link
      std::size_t index = part->index();

      // Fill the thinning mask at this place and increment the counter
      if ( index >= mask.size() ) {
        ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
        return StatusCode::SUCCESS;
      }
      mask[index] = true;
      return StatusCode::SUCCESS;
    }
  }
  else if ( objType == xAOD::Type::ObjectType::Electron ) {
    ATH_CHECK( this->selectFromElectron( mask, trackParticleContainer, static_cast<const xAOD::Electron*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Photon ) {
    ATH_CHECK( this->selectFromPhoton( mask, trackParticleContainer, static_cast<const xAOD::Photon*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Muon ) {
    ATH_CHECK( this->selectFromMuon( mask, trackParticleContainer, static_cast<const xAOD::Muon*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Tau ) {
    ATH_CHECK( this->selectFromTauJet( mask, trackParticleContainer, static_cast<const xAOD::TauJet*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Jet ) {
    ATH_CHECK( this->selectFromJet( mask, trackParticleContainer, static_cast<const xAOD::Jet*>(part) ));
  }
  else {
    const xAOD::CompositeParticle* compPart = dynamic_cast<const xAOD::CompositeParticle*>(part);
    if ( compPart ) {
      ATH_CHECK( this->selectFromCompositeParticle( mask, trackParticleContainer, compPart ));
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Couldn't determine the type of the current xAOD::IParticle with ObjectType = " << objType );
  }

  return StatusCode::SUCCESS;
}





//=============================================================================
// Thinning method for xAOD::Electron
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromElectron( std::vector<bool>& mask,
                                            const xAOD::TrackParticleContainer* trackParticleContainer,
                                            const xAOD::Electron* part ) const
{
  ATH_MSG_VERBOSE("In selectFromElectron");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticle and check that it points to the given track particle container
  std::size_t nTP = part->nTrackParticles();
  ATH_MSG_DEBUG("Got " << nTP << " TrackParticle for the current electron");
  if (m_nElectronPTMax.value() >= 0){ nTP = std::min(nTP,static_cast<std::size_t>(m_nElectronPTMax.value())); }
  for ( std::size_t i=0; i<nTP; ++i ) {
    ATH_MSG_VERBOSE("Now looking at TrackParticle number " << i);
    const ElementLink<xAOD::TrackParticleContainer>& partLink = part->trackParticleLink(i);
    if ( !(partLink.isValid()) ) {
      ATH_MSG_WARNING("Got an invalid element link. Continuing...");
      continue;
    }

    // Get the index of the current element link
    std::size_t index = partLink.index();

    const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
    if ( ptrTrkPartCont != trackParticleContainer ) {
      ATH_MSG_DEBUG("The current ElementLink doesn't point to the TrackParticleContainer");
      // Let's assume we have a GSF TrackParticle from the electron, but we want to thin
      // inner detector track particles. Thus, let's try to go from the former to the later
      const xAOD::TrackParticle* trackPart = *partLink;
      bool hasOriginalTP =
        trackPart->isAvailable<ElementLink< xAOD::TrackParticleContainer > >( "originalTrackParticle" );
      if ( !hasOriginalTP ) {
        continue;
      }
      // Let's get the original TrackParticle
      const ElementLink< xAOD::TrackParticleContainer >& origTPLink =
        trackPart->auxdata<ElementLink< xAOD::TrackParticleContainer > >( "originalTrackParticle" );
      if ( !(origTPLink.isValid()) ) {
        ATH_MSG_WARNING("Got an invalid element link to the original TrackParticle. Continuing...");
        continue;
      }
      const xAOD::TrackParticleContainer* ptrOrigTrkPartCont = origTPLink.getStorableObjectPointer();
      if ( ptrOrigTrkPartCont != trackParticleContainer ) {
        ATH_MSG_DEBUG("Even the original ElementLink doesn't point to the TrackParticleContainer");
        continue;
      }
      // Use this new index
      index = origTPLink.index();
    }

    // Fill the thinning mask at this place and increment the counter
    if ( index >= mask.size() ) {
      ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
      continue;
    }
    mask[index] = true;
  } // End: loop over all TrackParticles from this Electron

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::Photon
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromPhoton( std::vector<bool>& mask,
                                          const xAOD::TrackParticleContainer* trackParticleContainer,
                                          const xAOD::Photon* part) const
{
  ATH_MSG_VERBOSE("In selectFromPhoton");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Get the vertex for this photon
  std::size_t nVtx = part->nVertices();
  for ( std::size_t iVtx=0; iVtx<nVtx; ++iVtx ) {
    const xAOD::Vertex* vertex = part->vertex(iVtx);
    if ( !vertex ) {
      ATH_MSG_WARNING("Got a zero pointer to a vertex. Continuing...");
      continue;
    }

    // Now, get the TrackParticle and check that it points to the given track particle container
    const auto& tpLinks = vertex->trackParticleLinks();
    for ( const auto& partLink : tpLinks ) {
      if ( !(partLink.isValid()) ) {
        ATH_MSG_WARNING("Got an invalid element link. Continuing...");
        continue;
      }

      // Get the index of the current element link
      std::size_t index = partLink.index();

      const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
      if ( ptrTrkPartCont != trackParticleContainer ) {
        ATH_MSG_DEBUG("The current ElementLink doesn't point to the TrackParticleContainer");
        // Let's assume we have a GSF TrackParticle from the electron, but we want to thin
        // inner detector track particles. Thus, let's try to go from the former to the later
        const xAOD::TrackParticle* trackPart = *partLink;
        bool hasOriginalTP =
        trackPart->isAvailable<ElementLink< xAOD::TrackParticleContainer > >( "originalTrackParticle" );
        if ( !hasOriginalTP ) {
          continue;
        }
        // Let's get the original TrackParticle
        const ElementLink< xAOD::TrackParticleContainer >& origTPLink =
          trackPart->auxdata<ElementLink< xAOD::TrackParticleContainer > >( "originalTrackParticle" );
        if ( !(origTPLink.isValid()) ) {
          ATH_MSG_WARNING("Got an invalid element link to the original TrackParticle. Continuing...");
          continue;
        }
        const xAOD::TrackParticleContainer* ptrOrigTrkPartCont = origTPLink.getStorableObjectPointer();
        if ( ptrOrigTrkPartCont != trackParticleContainer ) {
          ATH_MSG_DEBUG("Even the original ElementLink doesn't point to the TrackParticleContainer");
          continue;
        }
        // Use this new index
        index = origTPLink.index();
      }

      // Fill the thinning mask at this place and increment the counter
      if ( index >= mask.size() ) {
        ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
        continue;
      }
      mask[index] = true;
    } // End: loop over all TrackParticles from this vertex
  } // End: loop over all vertices from this photon

  return StatusCode::SUCCESS;
}





//=============================================================================
// Thinning method for xAOD::Muon
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromMuon( std::vector<bool>& mask,
                                        const xAOD::TrackParticleContainer* trackParticleContainer,
                                        const xAOD::Muon* part) const
{
  ATH_MSG_VERBOSE("In selectFromMuon");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticle and check that it points to the given track particle container
  int index(-1);
  const auto& partLink = part->combinedTrackParticleLink();
  if ( partLink.isValid() ) {
    const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
    if ( ptrTrkPartCont == trackParticleContainer ) {
      ATH_MSG_DEBUG("Found the right TrackParticleContainer for a combined track particle");
      // Get the index of the current element link
      index = partLink.index();
    }
  }
  if ( index < 0 ) {
    const auto& partLink = part->muonSpectrometerTrackParticleLink();
    if ( partLink.isValid() ) {
      const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
      if ( ptrTrkPartCont == trackParticleContainer ) {
        ATH_MSG_DEBUG("Found the right TrackParticleContainer for a muon spectrometer track particle");
        // Get the index of the current element link
        index = partLink.index();
      }
    }
  }
  if ( index < 0 ) {
    const auto& partLink = part->inDetTrackParticleLink();
    if ( partLink.isValid() ) {
      const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
      if ( ptrTrkPartCont == trackParticleContainer ) {
        ATH_MSG_DEBUG("Found the right TrackParticleContainer for an inner detector track particle");
        // Get the index of the current element link
        index = partLink.index();
      }
    }
  }

  // Fill the thinning mask at this place and increment the counter
  if ( index >= static_cast<int>(mask.size()) ) {
    ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
    return StatusCode::SUCCESS;
  }
  if ( index >= 0 ) {
    mask[index] = true;
  }

  return StatusCode::SUCCESS;
}






//=============================================================================
// Thinning method for xAOD::TauJet
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromTauJet( std::vector<bool>& mask,
                                          const xAOD::TrackParticleContainer* trackParticleContainer,
                                          const xAOD::TauJet* part ) const
{
  ATH_MSG_VERBOSE("In selectFromTauJet");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticle and check that it points to the given track particle container
#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
  const auto& tpLinks = xAOD::TauHelpers::trackParticleLinks(part, xAOD::TauJetParameters::classifiedCharged) ;
#else
  const auto& tpLinks = part->trackLinks();
#endif
  for ( const auto& partLink : tpLinks ) {
    if ( !(partLink.isValid()) ) {
      ATH_MSG_WARNING("Got an invalid element link. Continuing...");
      continue;
    }

    const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
    if ( ptrTrkPartCont != trackParticleContainer ) {
      ATH_MSG_DEBUG("The current ElementLink doesn't point to the TrackParticleContainer");
      continue;
    }

    // Get the index of the current element link
    std::size_t index = partLink.index();

    // Fill the thinning mask at this place and increment the counter
    if ( index >= mask.size() ) {
      ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
      continue;
    }
    mask[index] = true;
  } // End: loop over all TrackParticles from this tau

  // FIXME!!! taus no longer have conversion track link vector after schema evolution to taujet_v2
  // // If requested, also get other types of TrackParticles from the Tau
  // if ( m_tauConversion ) {
  //   const auto& tpLinks = part->conversionTrackLinks();
  //   for ( const auto& partLink : tpLinks ) {
  //     if ( !(partLink.isValid()) ) {
  //       ATH_MSG_WARNING("Got an invalid element link. Continuing...");
  //       continue;
  //     }

  //     const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
  //     if ( ptrTrkPartCont != trackParticleContainer ) {
  //       ATH_MSG_DEBUG("The current ElementLink doesn't point to the TrackParticleContainer");
  //       continue;
  //     }

  //     // Get the index of the current element link
  //     std::size_t index = partLink.index();

  //     // Fill the thinning mask at this place and increment the counter
  //     if ( index >= mask.size() ) {
  //       ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
  //       continue;
  //     }
  //     mask[index] = true;
  //   } // End: loop over all TrackParticles from this tau
  // } // End: if ( m_tauConversion )

  // If requested, also get other types of TrackParticles from the Tau
  if ( m_tauWide ) {
    
#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
    const auto& tpLinks = xAOD::TauHelpers::trackParticleLinks(part,xAOD::TauJetParameters::classifiedIsolation);
#else
    const auto& tpLinks = part->wideTrackLinks();
#endif
    for ( const auto& partLink : tpLinks ) {
      if ( !(partLink.isValid()) ) {
        ATH_MSG_WARNING("Got an invalid element link. Continuing...");
        continue;
      }

      const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
      if ( ptrTrkPartCont != trackParticleContainer ) {
        ATH_MSG_DEBUG("The current ElementLink doesn't point to the TrackParticleContainer");
        continue;
      }

      // Get the index of the current element link
      std::size_t index = partLink.index();

      // Fill the thinning mask at this place and increment the counter
      if ( index >= mask.size() ) {
        ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
        continue;
      }
      mask[index] = true;
    } // End: loop over all TrackParticles from this tau
  } // End: if ( m_tauWide )

  // If requested, also get other types of TrackParticles from the Tau
  if ( m_tauOther ) {
#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
    const auto& tpLinks = xAOD::TauHelpers::allTrackParticleLinks(part);
#else
    const auto& tpLinks = part->otherTrackLinks();
#endif
    for ( const auto& partLink : tpLinks ) {
      if ( !(partLink.isValid()) ) {
        ATH_MSG_WARNING("Got an invalid element link. Continuing...");
        continue;
      }

      const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
      if ( ptrTrkPartCont != trackParticleContainer ) {
        ATH_MSG_DEBUG("The current ElementLink doesn't point to the TrackParticleContainer");
        continue;
      }

      // Get the index of the current element link
      std::size_t index = partLink.index();

      // Fill the thinning mask at this place and increment the counter
      if ( index >= mask.size() ) {
        ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
        continue;
      }
      mask[index] = true;
    } // End: loop over all TrackParticles from this tau
  } // End: if ( m_tauOther )

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::Jet
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromJet( std::vector<bool>& mask,
                                       const xAOD::TrackParticleContainer* trackParticleContainer,
                                       const xAOD::Jet* part ) const
{
  ATH_MSG_VERBOSE("In selectFromJet");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticle and check that it points to the given track particle container
  const auto& partLinks = part->constituentLinks();
  for ( const xAOD::IParticleLink& partLink : partLinks ) {
    ATH_CHECK( this->selectFromIParticleLink( mask, trackParticleContainer, partLink ) );
  } // End: loop over all constituents from this Jet

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::CompositeParticle
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromCompositeParticle( std::vector<bool>& mask,
                                                     const xAOD::TrackParticleContainer* trackParticleContainer,
                                                     const xAOD::CompositeParticle* part ) const
{
  ATH_MSG_VERBOSE("In selectFromCompositeParticle");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticle and check that it points to the given track particle container
  const auto& partLinks = part->partLinks();
  for ( const xAOD::IParticleLink& partLink : partLinks ) {
    ATH_CHECK( this->selectFromIParticleLink( mask, trackParticleContainer, partLink ) );
  } // End: loop over all constituents from this Jet

  return StatusCode::SUCCESS;
}





//=============================================================================
// Thinning method for xAOD::Vertex
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromVertex( std::vector<bool>& mask,
                                          const xAOD::TrackParticleContainer* trackParticleContainer,
                                          const xAOD::Vertex* vertex ) const
{
  ATH_MSG_VERBOSE("In selectFromVertex");
  if ( !vertex ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticles and check that they points to the given track particle container
  const auto& tpLinks = vertex->trackParticleLinks();
  for ( const auto& partLink : tpLinks ) {
    if ( !(partLink.isValid()) ) {
      ATH_MSG_WARNING("Got an invalid element link. Continuing...");
      continue;
    }

    // Check if we have an object from the same container that we want to thin
    const xAOD::TrackParticleContainer* ptrTrkPartCont = partLink.getStorableObjectPointer();
    if ( ptrTrkPartCont != trackParticleContainer ) {
      ATH_MSG_DEBUG("The current ElementLink doesn't point to the TrackParticleContainer");
      continue;
    }

    // Get the index of the current element link
    std::size_t index = partLink.index();

    // Fill the thinning mask at this place and increment the counter
    if ( index >= mask.size() ) {
      ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
      continue;
    }
    mask[index] = true;
  } // End: loop over all TrackParticles from this vertex

  return StatusCode::SUCCESS;
}





//=============================================================================
// Select relevant TrackParticles based on the selection string
//=============================================================================
StatusCode
ThinTrackParticlesTool::selectFromString( std::vector<bool>& mask,
                                          const xAOD::TrackParticleContainer* trackParticleContainer ) const
{
  ATH_MSG_VERBOSE("In selectFromString");

  ExpressionParsing::StackElement selectionResult = m_parser->evaluate();

  if ( selectionResult.isScalar() ) {
    ATH_MSG_ERROR( "We are expecting a vector result such that we can deduct "
                   << "which xAOD::IParticle inside the container we want to keep and which not. "
                   << "For example: 'Muons.pt>10*GeV', but NOT 'count(Muons.pt>10*GeV)>1'; here, "
                   << "the former gives us a boolean answer for every muon while "
                   << "the later gives us a boolean answer for the whole container of muons." );
    return StatusCode::FAILURE;
  }

  if ( selectionResult.isVector() ) {
    // We found a vector. Now, we can go ahead and evaluate which object
    // in the input container we want to kee
    const std::vector<int>& resultVec( selectionResult.vectorValue<int>() );

    // Check that the lengths are the same
    if ( trackParticleContainer && trackParticleContainer->size() != resultVec.size() ) {
      ATH_MSG_ERROR("We got an input container to thin, but its size (" << trackParticleContainer->size()
                    << ") doesn't match the size of the result vector: " << resultVec.size() );
      return StatusCode::FAILURE;
    }

    // Now, loop over the result vector and check which particles to keep
    for ( std::size_t i=0; i<resultVec.size(); ++i ) {
      // If this particle is not accepted, go to the next one
      if ( resultVec[i] ) {
        mask[i] = true;
      }
    }
  }
  else {
    // what we found in the event store is neither a scalar nor a vector
    // it must be of some awkward type that can't be used.
    // Therefore, we fail
    ATH_MSG_ERROR ("Some unexpected format of the expression parser result");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
