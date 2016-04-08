/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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



//=============================================================================
// Constructor
//=============================================================================
ThinCaloClustersTool::ThinCaloClustersTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent ) :
  ::AthAlgTool( type, name, parent ),
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
#endif
  m_parser(0),
  m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
  m_nTotalCaloClusters(0),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IThinningTool >(this);

  // Properties go here
  declareProperty("ThinningSvc",          m_thinningSvc,
                  "The ThinningSvc instance for a particular output stream" );
  declareProperty("CaloClustersToThin", m_caloClusKey = "CaloCalTopoCluster",
                  "The xAOD::CaloClusterContainer to be thinned" );
  declareProperty("InputContainerList",   m_inCollKeyList,
                  "Containers from which to extract the information which xAOD::CaloClusters should be kept" );
  declareProperty("Selection",            m_selection="",
                  "The selection string that defines which xAOD::CaloClusters to select from the container" );
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
  ATH_MSG_DEBUG ( " using " << m_thinningSvc );
  ATH_MSG_DEBUG ( " using " << m_caloClusKey );
  ATH_MSG_DEBUG ( " using " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using " << m_selection );

  // Get pointer to ThinningSvc and cache it :
  // m_thinningSvc is of type IThinningSvc
  ATH_CHECK ( m_thinningSvc.retrieve() );

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

  // Initialize the counters
  m_nTotalCaloClusters = 0;
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

  if (m_parser) {
    delete m_parser;
    m_parser = 0;
  }

  return StatusCode::SUCCESS;
}




//=============================================================================
// The main method from the inherited interface
//=============================================================================
StatusCode ThinCaloClustersTool::doThinning() const
{
  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event
  ATH_MSG_DEBUG ( "==> doThinning " << name() << " on " << m_nEventsProcessed << ". event..." );

  // retrive the CaloClusterContainer
  const xAOD::CaloClusterContainer* caloClusterContainer;
  ATH_CHECK( evtStore()->retrieve(caloClusterContainer, m_caloClusKey.value()) );
  ATH_MSG_DEBUG ( "Container '" << m_caloClusKey.value() << "' retrieved from StoreGate" );
  m_nTotalCaloClusters  = caloClusterContainer->size();

  // Create the mask to be used for thinning
  std::vector<bool> mask(m_nTotalCaloClusters, false);

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
        ATH_CHECK( this->selectFromIParticleLink( mask, caloClusterContainer, partLink ) );
      }
    }

    else if ( evtStore()->contains< xAOD::EgammaContainer >( inKey ) ) {
      // This file holds an xAOD::EgammaContainer
      const xAOD::EgammaContainer* inContainer;
      ATH_CHECK( evtStore()->retrieve( inContainer, inKey ) );
      ATH_MSG_DEBUG ( "Input collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromEgamma( mask, caloClusterContainer, part ) );
      }
    }

    else if ( evtStore()->contains< xAOD::MuonContainer >( inKey ) ) {
      // This file holds an xAOD::MuonContainer
      const xAOD::MuonContainer* inContainer;
      ATH_CHECK( evtStore()->retrieve( inContainer, inKey ) );
      ATH_MSG_DEBUG ( "Input collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromMuon( mask, caloClusterContainer, part ) );
      }
    }

    else if ( evtStore()->contains< xAOD::TauJetContainer >( inKey ) ) {
      // This file holds an xAOD::TauJetContainer
      const xAOD::TauJetContainer* inContainer;
      ATH_CHECK( evtStore()->retrieve( inContainer, inKey ) );
      ATH_MSG_DEBUG ( "Input collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromTauJet( mask, caloClusterContainer, part ) );
      }
    }

    else if ( evtStore()->contains< xAOD::JetContainer >( inKey ) ) {
      // This file holds an xAOD::JetContainer
      const xAOD::JetContainer* inContainer;
      ATH_CHECK( evtStore()->retrieve( inContainer, inKey ) );
      ATH_MSG_DEBUG ( "Input collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromJet( mask, caloClusterContainer, part ) );
      }
    }

    else if ( evtStore()->contains< xAOD::CompositeParticleContainer >( inKey ) ) {
      // This file holds an xAOD::CompositeParticleContainer
      const xAOD::CompositeParticleContainer* inContainer;
      ATH_CHECK( evtStore()->retrieve( inContainer, inKey ) );
      ATH_MSG_DEBUG ( "Input collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromCompositeParticle( mask, caloClusterContainer, part ) );
      }
    }

    else if ( evtStore()->contains< xAOD::IParticleContainer >( inKey ) ) {
      // This container holds an xAOD::IParticleContainer
      const xAOD::IParticleContainer* inContainer;
      ATH_CHECK( evtStore()->retrieve( inContainer, inKey ) );
      ATH_MSG_DEBUG ( "Input collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromIParticle( mask, caloClusterContainer, part ) );
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
    ATH_CHECK( this->selectFromString( mask, caloClusterContainer ) );
  }

  // Perform the actual thinning
  ATH_CHECK ( m_thinningSvc->filter(*caloClusterContainer, mask, IThinningSvc::Operator::Or) );

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::IParticleLink
//=============================================================================
StatusCode
ThinCaloClustersTool::selectFromIParticleLink( std::vector<bool>& mask,
                                               const xAOD::CaloClusterContainer* caloClusterContainer,
                                               const xAOD::IParticleLink& partLink ) const
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
    if ( index >= m_nTotalCaloClusters ) {
      ATH_MSG_WARNING("We got an index " << index << "out of container range " << m_nTotalCaloClusters );
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
      if ( index >= m_nTotalCaloClusters ) {
        ATH_MSG_WARNING("We got an index " << index << "out of container range " << m_nTotalCaloClusters );
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
    if ( index >= m_nTotalCaloClusters ) {
      ATH_MSG_WARNING("We got an index " << index << "out of container range " << m_nTotalCaloClusters );
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
      if ( index >= m_nTotalCaloClusters ) {
        ATH_MSG_WARNING("We got an index " << index << "out of container range " << m_nTotalCaloClusters );
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
