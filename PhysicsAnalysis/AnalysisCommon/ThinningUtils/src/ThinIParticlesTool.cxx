/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        ThinIParticlesTool

Author:      Karsten Koeneke
Created:     July 2014
******************************************************************************/

// This class' header
#include "ThinIParticlesTool.h"

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "ExpressionEvaluation/StackElement.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

// EDM includes
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"


//=============================================================================
// Constructor
//=============================================================================
ThinIParticlesTool::ThinIParticlesTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  ThinIParticlesToolBase( type, name, parent ),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IThinningTool >(this);
}





//=============================================================================
// Destructor
//=============================================================================
ThinIParticlesTool::~ThinIParticlesTool()
{
}


//=============================================================================
// Athena's initialize
//=============================================================================
StatusCode ThinIParticlesTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  // Print out the used configuration
  ATH_MSG_DEBUG ( " using " << m_streamName );
  ATH_MSG_DEBUG ( " using " << m_ipartKey.key() );
  ATH_MSG_DEBUG ( " using " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using " << m_selection );

  if (m_streamName.empty()) {
    ATH_MSG_ERROR( "StreamName property has not been initialized." );
    return StatusCode::FAILURE;
  }

  // Check if we have been given a container to be thinned
  ATH_CHECK( m_ipartKey.initialize (m_streamName) );

  ATH_CHECK( m_inCollKeyList.initialize() );

  if(!m_selection.value().empty()) {
     ATH_CHECK( initializeParser( m_selection.value() ) );
  }

  // Initialize the counters
  m_nEventsProcessed = 0;

  return StatusCode::SUCCESS;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ThinIParticlesTool::finalize()
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
StatusCode ThinIParticlesTool::doThinning() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event
  ATH_MSG_DEBUG ( "==> doThinning " << name() << " on " << m_nEventsProcessed << ". event..." );

  // retrive the IParticleContainer
  SG::ThinningHandle<xAOD::IParticleContainer> iparticleContainer (m_ipartKey, ctx);
  ATH_MSG_DEBUG ( "Container '" << m_ipartKey.key() << "' retrieved from StoreGate" );
  size_t nTotalIParticles  = iparticleContainer->size();

  // Create the mask to be used for thinning
  std::vector<bool> mask(nTotalIParticles, false);

  // Now, retrieve the other containers that are holding the information which
  // CaloClusters should be kept
  for (const SG::ReadHandleKey<xAOD::IParticleLinkContainer>& inKey : m_inCollKeyList)
  {
      SG::ReadHandle<xAOD::IParticleLinkContainer> inLinkContainer (inKey, ctx);
      ATH_MSG_DEBUG ( "Input link collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const xAOD::IParticleLink& partLink : *inLinkContainer ) {
        ATH_CHECK( this->selectFromIParticleLink( mask, iparticleContainer.cptr(), partLink ) );
      }

  } // End: loop over input container names


  // Try to fill the thinning mask based on the selection string, if given
  if ( !(m_selection.value().empty()) ) {
    ATH_CHECK( this->selectFromString( mask, iparticleContainer.cptr() ) );
  }


  // Perform the actual thinning
  iparticleContainer.keep (mask);
  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::IParticleLink
//=============================================================================
StatusCode
ThinIParticlesTool::selectFromIParticleLink( std::vector<bool>& mask,
                                             const xAOD::IParticleContainer* iparticleContainer,
                                             const xAOD::IParticleLink& partLink ) const
{
  ATH_MSG_VERBOSE("In selectFromIParticleLink");
  if ( !(partLink.isValid()) ) {
    ATH_MSG_WARNING("Got an invalid element link. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, check that the element link points to the given IParticleContainer
  const auto* ptrIPartCont = partLink.getStorableObjectPointer();
  if ( ptrIPartCont == iparticleContainer ) {
    ATH_MSG_DEBUG("The current ElementLink points to the IParticleContainer");
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

  return StatusCode::SUCCESS;
}




//=============================================================================
// Select relevant IParticles based on the selection string
//=============================================================================
StatusCode
ThinIParticlesTool::selectFromString( std::vector<bool>& mask,
                                      const xAOD::IParticleContainer* iParticleContainer ) const
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
		if ( iParticleContainer && iParticleContainer->size() != resultVec.size() ) {
			ATH_MSG_ERROR("We got an input container to thin, but its size (" << iParticleContainer->size()
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
