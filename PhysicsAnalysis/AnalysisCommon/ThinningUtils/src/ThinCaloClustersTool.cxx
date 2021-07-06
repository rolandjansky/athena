/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        ThinCaloClustersTool

Author:      Karsten Koeneke
Created:     July 2014
******************************************************************************/

// This class' header
#include "ThinCaloClustersTool.h"

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "ExpressionEvaluation/StackElement.h"

// EDM includes
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/ObjectType.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

//=============================================================================
// Constructor
//=============================================================================
ThinCaloClustersTool::ThinCaloClustersTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent ) :
  ThinCaloClustersToolBase( type, name, parent ),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IThinningTool >(this);
}





//=============================================================================
// Destructor
//=============================================================================
ThinCaloClustersTool::~ThinCaloClustersTool()
{
}


//=============================================================================
// Athena's initialize
//=============================================================================
StatusCode ThinCaloClustersTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  // Print out the used configuration
  ATH_MSG_DEBUG ( " using " << m_streamName );
  ATH_MSG_DEBUG ( " using " << m_caloClusKey.key() );
  ATH_MSG_DEBUG ( " using " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using " << m_selection );

  if (m_streamName.empty()) {
    ATH_MSG_ERROR ("StreamName property was not set.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_caloClusKey.initialize (m_streamName) );
  ATH_CHECK( m_inCollKeyList.initialize() );

  ATH_CHECK( initializeParser(m_selection.value() ) );

  // Initialize the counters
  m_nEventsProcessed   = 0;

  return StatusCode::SUCCESS;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ThinCaloClustersTool::finalize()
{
  // Print messages
  ATH_MSG_DEBUG ( "==> finalize " << name() << "..." );
  ATH_MSG_DEBUG ( " Number of processed events:  " << m_nEventsProcessed );

  ATH_CHECK(finalizeParser());

  return StatusCode::SUCCESS;
}




//=============================================================================
// The main method from the inherited interface
//=============================================================================
StatusCode ThinCaloClustersTool::doThinning() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event
  ATH_MSG_DEBUG ( "==> doThinning " << name() << " on " << m_nEventsProcessed << ". event..." );

  // retrieve the CaloClusterContainer
  SG::ThinningHandle<xAOD::CaloClusterContainer> caloClusterContainer (m_caloClusKey, ctx);
   ATH_MSG_DEBUG ( "Container '" << m_caloClusKey.key() << "' retrieved from StoreGate" );
   size_t nTotalCaloClusters  = caloClusterContainer->size();

  // Create the mask to be used for thinning
  std::vector<bool> mask(nTotalCaloClusters, false);

  // Now, retrieve the other containers that are holding the information which
  // CaloClusters should be kept
  for ( const SG::ReadHandleKey<SG::AuxVectorBase>& inKey : m_inCollKeyList ) {

    SG::ReadHandle<SG::AuxVectorBase> inObj (inKey, ctx);

    // First check for an xAOD::IParticleLinkContainer
    if (const auto* inLinkContainer = dynamic_cast<const xAOD::IParticleLinkContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input link collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const xAOD::IParticleLink& partLink : *inLinkContainer ) {
        ATH_CHECK( this->selectFromIParticleLink( mask, caloClusterContainer.cptr(), partLink ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::EgammaContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromEgamma( mask, caloClusterContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::MuonContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromMuon( mask, caloClusterContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::TauJetContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromTauJet( mask, caloClusterContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::JetContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromJet( mask, caloClusterContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::CompositeParticleContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromCompositeParticle( mask, caloClusterContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::IParticleContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromIParticle( mask, caloClusterContainer.cptr(), part ) );
      }
    }

    else {
      if ( m_nEventsProcessed <= 10 ) {
        ATH_MSG_WARNING ( "Input collection  = '" << inKey.key()
                          << "' could not be retrieved from StoreGate! "
                          << " This message will only be repeated 10 times..." );
      }
      else {
        ATH_MSG_DEBUG ( "Input collection  = '" << inKey.key()
                        << "' could not be retrieved from StoreGate! " );
      }
      return StatusCode::SUCCESS;
    } // End: if/elif/else is link container
  } // End: loop over input container names

  // Try to fill the thinning mask based on the selection string, if given
  if ( !(m_selection.value().empty()) ) {
    ATH_CHECK( this->selectFromString( mask, caloClusterContainer.cptr() ) );
  }

  // Perform the actual thinning
  caloClusterContainer.keep (mask);

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::IParticleLink
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromIParticleLink( std::vector<bool>& mask,
                                               const xAOD::CaloClusterContainer* caloClusterContainer,
                                               const xAOD::IParticleLink& partLink) const
{
  ATH_MSG_VERBOSE("In selectFromIParticleLink");
  if ( !(partLink.isValid()) ) {
    ATH_MSG_WARNING("Got an invalid element link. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, check that the element link points to the given calo cluster container
  const auto* ptrCaloClusCont = partLink.getStorableObjectPointer();
  if ( ptrCaloClusCont == caloClusterContainer ) {
    ATH_MSG_DEBUG("The current ElementLink points to the CaloClusterContainer");
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

  // If we don't directly have an element link to the relevant CaloClusterContainer,
  // delegate this to the generic IParticle method
  ATH_CHECK( this->selectFromIParticle( mask, caloClusterContainer, *partLink ) );

  return StatusCode::SUCCESS;
}






//=============================================================================
// Thinning method for xAOD::IParticle
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromIParticle( std::vector<bool>& mask,
                                           const xAOD::CaloClusterContainer* caloClusterContainer,
                                           const xAOD::IParticle* part ) const
{
  ATH_MSG_VERBOSE("In selectFromIParticle");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Try to figure out what type of particle we have and delegate accordingly
  const xAOD::Type::ObjectType objType = part->type();
  if ( objType == xAOD::Type::ObjectType::CaloCluster ) {
    // Now, check that we point to the given calo cluster container
    if ( part->container() == caloClusterContainer ) {
      ATH_MSG_DEBUG("The current IParticle points to the caloClusterContainer");
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
  else if ( objType == xAOD::Type::ObjectType::Electron || objType == xAOD::Type::ObjectType::Photon ) {
    ATH_CHECK( this->selectFromEgamma( mask, caloClusterContainer, static_cast<const xAOD::Egamma*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Muon ) {
    ATH_CHECK( this->selectFromMuon( mask, caloClusterContainer, static_cast<const xAOD::Muon*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Tau ) {
    ATH_CHECK( this->selectFromTauJet( mask, caloClusterContainer, static_cast<const xAOD::TauJet*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Jet ) {
    ATH_CHECK( this->selectFromJet( mask, caloClusterContainer, static_cast<const xAOD::Jet*>(part) ));
  }
  else {
    const xAOD::CompositeParticle* compPart = dynamic_cast<const xAOD::CompositeParticle*>(part);
    if ( compPart ) {
      ATH_CHECK( this->selectFromCompositeParticle( mask, caloClusterContainer, compPart ));
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Couldn't determine the type of the current xAOD::IParticle with ObjectType = " << objType );
  }

  return StatusCode::SUCCESS;
}





//=============================================================================
// Thinning method for xAOD::Egamma
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromEgamma( std::vector<bool>& mask,
                                        const xAOD::CaloClusterContainer* caloClusterContainer,
                                        const xAOD::Egamma* part ) const
{
  ATH_MSG_VERBOSE("In selectFromEgamma");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the CaloClusters and check that it points to the given calo cluster container
  const std::size_t nCC = part->nCaloClusters();
  for ( std::size_t i=0; i<nCC; ++i ) {
    const ElementLink<xAOD::CaloClusterContainer>& partLink = part->caloClusterLink(i);
    if ( !(partLink.isValid()) ) {
      ATH_MSG_WARNING("Got an invalid element link. Continuing...");
      continue;
    }

    const xAOD::CaloClusterContainer* ptrCaloClusCont = partLink.getStorableObjectPointer();
    if ( ptrCaloClusCont != caloClusterContainer ) {
      ATH_MSG_DEBUG("The current ElementLink doesn't point to the caloClusterContainer");
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
  } // End: loop over all CaloClusters from this Egamma

  return StatusCode::SUCCESS;
}





//=============================================================================
// Thinning method for xAOD::Muon
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromMuon( std::vector<bool>& mask,
                                      const xAOD::CaloClusterContainer* caloClusterContainer,
                                      const xAOD::Muon* part) const
{
  ATH_MSG_VERBOSE("In selectFromMuon");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the CaloCluster and check that it points to the given calo cluster container
  const auto& partLink = part->clusterLink();
  if ( partLink.isValid() ) {
    const auto* ptrCaloClusCont = partLink.getStorableObjectPointer();
    if ( ptrCaloClusCont == caloClusterContainer ) {
      ATH_MSG_DEBUG("Found the right caloClusterContainer for a CaloCluster");

      // Get the index of the current element link
      std::size_t index = partLink.index();

      // Fill the thinning mask at this place and increment the counter
      if ( index >= mask.size() ) {
        ATH_MSG_WARNING("We got an index " << index << "out of container range " << mask.size() );
        return StatusCode::SUCCESS;
      }
      mask[index] = true;
    }
  }

  return StatusCode::SUCCESS;
}






//=============================================================================
// Thinning method for xAOD::TauJet
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromTauJet( std::vector<bool>& mask,
                                        const xAOD::CaloClusterContainer* caloClusterContainer,
                                        const xAOD::TauJet* part ) const
{
  ATH_MSG_VERBOSE("In selectFromTauJet");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the constituents that could have CaloClusters
  const auto& jetLink = part->jetLink();
  if ( jetLink.isValid() ) {
    ATH_CHECK( this->selectFromJet( mask, caloClusterContainer, *jetLink ) );
  }

  // Would also need to get eflow/pflow object...
  // however, the PFO doesn't seem to give me access to its CaloClusters, i.e.,
  // there is no method std::size_t nCaloClusters().

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::Jet
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromJet( std::vector<bool>& mask,
                                     const xAOD::CaloClusterContainer* caloClusterContainer,
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
    ATH_CHECK( this->selectFromIParticleLink( mask, caloClusterContainer, partLink ) );
  } // End: loop over all constituents from this Jet

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::CompositeParticle
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromCompositeParticle( std::vector<bool>& mask,
                                                   const xAOD::CaloClusterContainer* caloClusterContainer,
                                                   const xAOD::CompositeParticle* part ) const
{
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticle and check that it points to the given track particle container
  const auto& partLinks = part->partLinks();
  for ( const xAOD::IParticleLink& partLink : partLinks ) {
    ATH_CHECK( this->selectFromIParticleLink( mask, caloClusterContainer, partLink ) );
  } // End: loop over all constituents from this Jet

  return StatusCode::SUCCESS;
}





//=============================================================================
// Select relevant CaloClusters based on the selection string
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromString( std::vector<bool>& mask,
                                        const xAOD::CaloClusterContainer* caloClusterContainer ) const
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
    if ( caloClusterContainer && caloClusterContainer->size() != resultVec.size() ) {
      ATH_MSG_ERROR("We got an input container to thin, but its size (" << caloClusterContainer->size()
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
