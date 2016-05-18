/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "ExpressionEvaluation/ExpressionParser.h"
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
#include "TrigDecisionTool/TrigDecisionTool.h"
#endif
#include "ExpressionEvaluation/TriggerDecisionProxyLoader.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/StackElement.h"

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
  ::AthAlgTool( type, name, parent ),
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
	m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
#endif
	m_parser(0),
  m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
  m_nTotalIParticles(0),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IThinningTool >(this);

  // Properties go here
  declareProperty("ThinningSvc",          m_thinningSvc,
                  "The ThinningSvc instance for a particular output stream" );
  declareProperty("IParticlesToThin",     m_ipartKey = "",
                  "The xAOD::IParticleContainer to be thinned" );
  declareProperty("InputContainerList",   m_inCollKeyList,
                  "Containers from which to extract the information which xAOD::IParticles should be kept" );
  declareProperty("Selection",            m_selection="",
								  "The selection string that defines which xAOD::IParticles to select from the container" );

  declareProperty("MaskKey", m_maskStoregate="","If specified, will take the vector of bools from storegate with the given key, and use that as the mask!");
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
  ATH_MSG_DEBUG ( " using " << m_thinningSvc );
  ATH_MSG_DEBUG ( " using " << m_ipartKey );
  ATH_MSG_DEBUG ( " using " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using " << m_selection );

  // Check if we have been given a container to be thinned
  if ( m_ipartKey.value().empty() ) {
    ATH_MSG_ERROR("Got an empty string for the container name that I should thin: " << m_ipartKey);
    return StatusCode::FAILURE;
  }

  // Get pointer to ThinningSvc and cache it :
  // m_thinningSvc is of type IThinningSvc
  ATH_CHECK ( m_thinningSvc.retrieve() );

  if(!m_selection.value().empty()) {

  // initialize proxy loaders for expression parsing
	ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
	proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
#endif
	proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));

	// load the expressions
	m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	m_parser->loadExpression( m_selection.value() );

  }

  // Initialize the counters
  m_nTotalIParticles = 0;
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

	if (m_parser) {
		delete m_parser;
		m_parser = 0;
	}

  return StatusCode::SUCCESS;
}




//=============================================================================
// The main method from the inherited interface
//=============================================================================
StatusCode ThinIParticlesTool::doThinning() const
{
  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event
  ATH_MSG_DEBUG ( "==> doThinning " << name() << " on " << m_nEventsProcessed << ". event..." );

  // retrive the IParticleContainer
  const xAOD::IParticleContainer* iparticleContainer;
  ATH_CHECK( evtStore()->retrieve(iparticleContainer, m_ipartKey.value()) );
  ATH_MSG_DEBUG ( "Container '" << m_ipartKey.value() << "' retrieved from StoreGate" );
  m_nTotalIParticles  = iparticleContainer->size();

  // Create the mask to be used for thinning
  std::vector<bool> mask(m_nTotalIParticles, false);

  // Now, retrieve the other containers that are holding the information which
  // CaloClusters should be kept
  for ( const auto& inKey : m_inCollKeyList.value() ) {

    // First check for an xAOD::IParticleLinkContainer
    if ( evtStore()->contains< xAOD::IParticleLinkContainer >( inKey ) ) {
      const xAOD::IParticleLinkContainer* inLinkContainer;
      ATH_CHECK( evtStore()->retrieve( inLinkContainer , inKey ) );
      ATH_MSG_DEBUG ( "Input link collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const xAOD::IParticleLink& partLink : *inLinkContainer ) {
        ATH_CHECK( this->selectFromIParticleLink( mask, iparticleContainer, partLink ) );
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


  //see if taking the mask from the storegate
  if( !(m_maskStoregate.value().empty()) ) {
    //obtain the mask from the storegate and use that instead 
    const std::vector<bool>* sgMask = 0;
    ATH_CHECK( evtStore()->retrieve(sgMask,m_maskStoregate) );
    ATH_CHECK ( m_thinningSvc->filter(*iparticleContainer, *sgMask, IThinningSvc::Operator::Or) );
    return StatusCode::SUCCESS;
  }

  // Try to fill the thinning mask based on the selection string, if given
  if ( !(m_selection.value().empty()) ) {
    ATH_CHECK( this->selectFromString( mask, iparticleContainer ) );
  }


  // Perform the actual thinning
  ATH_CHECK ( m_thinningSvc->filter(*iparticleContainer, mask, IThinningSvc::Operator::Or) );

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
    if ( index >= m_nTotalIParticles ) {
      ATH_MSG_WARNING("We got an index " << index << "out of container range " << m_nTotalIParticles );
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
