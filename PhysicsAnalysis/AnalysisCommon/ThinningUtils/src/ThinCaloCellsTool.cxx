/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        ThinCaloCellsTool

Author:      Karsten Koeneke
Created:     July 2014
******************************************************************************/

// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS

// This class' header
#include "ThinCaloCellsTool.h"

// STL includes
#include <string>
#include <vector>

// EDM includes
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
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
ThinCaloCellsTool::ThinCaloCellsTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent ) :
  ::AthAlgTool( type, name, parent ),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IThinningTool >(this);
}





//=============================================================================
// Destructor
//=============================================================================
ThinCaloCellsTool::~ThinCaloCellsTool()
{
}


//=============================================================================
// Athena's initialize
//=============================================================================
StatusCode ThinCaloCellsTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  // Print out the used configuration
  ATH_MSG_DEBUG ( " using " << m_streamName );
  ATH_MSG_DEBUG ( " using " << m_caloCellKey.key() );
  ATH_MSG_DEBUG ( " using " << m_inCollKeyList );

  if (m_streamName.empty()) {
    ATH_MSG_ERROR ("StreamName property was not set.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_caloCellKey.initialize (m_streamName) );

  ATH_CHECK( m_inCollKeyList.initialize() );

  // Initialize the counters
  m_nEventsProcessed = 0;

  return StatusCode::SUCCESS;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ThinCaloCellsTool::finalize()
{
  // Print messages
  ATH_MSG_DEBUG ( "==> finalize " << name() << "..." );
  ATH_MSG_DEBUG ( " Number of processed events:  " << m_nEventsProcessed );

  return StatusCode::SUCCESS;
}




//=============================================================================
// The main method from the inherited interface
//=============================================================================
StatusCode ThinCaloCellsTool::doThinning() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event
  ATH_MSG_DEBUG ( "==> doThinning " << name() << " on " << m_nEventsProcessed << ". event..." );

  // retrieve the caloCellContainer

  SG::ThinningHandle<CaloCellContainer> caloCellContainer (m_caloCellKey, ctx);
  ATH_MSG_DEBUG ( "Container '" << m_caloCellKey.key() << "' retrieved from StoreGate" );
  size_t nTotalCaloCells  = caloCellContainer->size();

  // Create the mask to be used for thinning
  std::vector<bool> mask(nTotalCaloCells, false);

  // Now, retrieve the other containers that are holding the information which
  // CaloCells should be kept
  for ( const SG::ReadHandleKey<SG::AuxVectorBase>& inKey : m_inCollKeyList ) {

    SG::ReadHandle<SG::AuxVectorBase> inObj (inKey, ctx);
    if (const auto* inContainer = dynamic_cast<const xAOD::CaloClusterContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* cluster : *inContainer ) {
        ATH_CHECK( this->selectFromCaloCluster( mask, caloCellContainer.cptr(), cluster ) );
      }
    }

    // // Check for an std::vector<ElementLink<CaloCellContainer> >
    // else if ( evtStore()->contains< std::vector<ElementLink<CaloCellContainer> > >( inKey ) ) {
    //   const std::vector<ElementLink<CaloCellContainer> >* inLinkContainer;
    //   ATH_CHECK( evtStore()->retrieve( inLinkContainer , inKey ) );
    //   ATH_MSG_DEBUG ( "Input link collection = '" << inKey
    //                   << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
    //   // Iterate over all given inputs and find their associated CaloClusters
    //   for ( const auto& cellLink : *inLinkContainer ) {
    //     ATH_CHECK( this->selectFromCaloCellLink( mask, caloCellContainer, cellLink ) );
    //   }
    // }

    else if (const auto* inLinkContainer = dynamic_cast<const xAOD::IParticleLinkContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input link collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const xAOD::IParticleLink& partLink : *inLinkContainer ) {
        ATH_CHECK( this->selectFromIParticleLink( mask, caloCellContainer.cptr(), partLink ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::EgammaContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromEgamma( mask, caloCellContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::MuonContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromMuon( mask, caloCellContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::TauJetContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromTauJet( mask, caloCellContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::JetContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromJet( mask, caloCellContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::CompositeParticleContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromCompositeParticle( mask, caloCellContainer.cptr(), part ) );
      }
    }

    else if (const auto* inContainer = dynamic_cast<const xAOD::IParticleContainer*> (inObj.cptr()))
    {
      ATH_MSG_DEBUG ( "Input collection = '" << inKey.key()
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* part : *inContainer ) {
        ATH_CHECK( this->selectFromIParticle( mask, caloCellContainer.cptr(), part ) );
      }
    }

    else {
      if ( m_nEventsProcessed <= 10 ) {
        ATH_MSG_WARNING ( "Input collection  = '" << inKey.key()
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

  // Perform the actual thinning
  caloCellContainer.keep (mask);

  return StatusCode::SUCCESS;
}




// //=============================================================================
// // Thinning method for ElementLink<CaloCellContainer>
// //=============================================================================
// StatusCode
// ThinCaloCellsTool::selectFromCaloCellLink( std::vector<bool>& mask,
//                                            const CaloCellContainer* caloCellContainer,
//                                            const ElementLink<CaloCellContainer>& cellLink ) const
// {
//   ATH_MSG_VERBOSE("In selectFromCaloCellLink");
//   if ( !(cellLink.isValid()) ) {
//     ATH_MSG_WARNING("Got an invalid element link. Continuing...");
//     return StatusCode::SUCCESS;
//   }
//
//   // Now, check that the element link points to the given calo cluster container
//   const auto* ptrCaloCellCont = cellLink.getStorableObjectPointer();
//   if ( ptrCaloCellCont == caloCellContainer ) {
//     ATH_MSG_DEBUG("The current ElementLink points to the caloCellContainer");
//     // Get the index of the current element link
//     std::size_t index = cellLink.index();
//
//     // Fill the thinning mask at this place and increment the counter
//     if ( index >= m_nTotalCaloCells ) {
//       ATH_MSG_WARNING("We got an index " << index << "out of container range " << m_nTotalCaloCells );
//       return StatusCode::SUCCESS;
//     }
//     mask[index] = true;
//     return StatusCode::SUCCESS;
//   }
//
//   return StatusCode::SUCCESS;
// }







//=============================================================================
// Thinning method for xAOD::IParticleLink
//=============================================================================
StatusCode
ThinCaloCellsTool::selectFromIParticleLink( std::vector<bool>& mask,
                                            const CaloCellContainer* caloCellContainer,
                                            const xAOD::IParticleLink& partLink ) const
{
  ATH_MSG_VERBOSE("In selectFromIParticleLink");
  if ( !(partLink.isValid()) ) {
    ATH_MSG_WARNING("Got an invalid element link. Continuing...");
    return StatusCode::SUCCESS;
  }

  // If we don't directly have an element link to the relevant caloCellContainer,
  // delegate this to the generic IParticle method
  ATH_CHECK( this->selectFromIParticle( mask, caloCellContainer, *partLink ) );

  return StatusCode::SUCCESS;
}






//=============================================================================
// Thinning method for xAOD::IParticle
//=============================================================================
StatusCode
ThinCaloCellsTool::selectFromIParticle( std::vector<bool>& mask,
                                        const CaloCellContainer* caloCellContainer,
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
    ATH_CHECK( this->selectFromCaloCluster( mask,  caloCellContainer, static_cast<const xAOD::CaloCluster*>(part)));
  }
  else if ( objType == xAOD::Type::ObjectType::Electron || objType == xAOD::Type::ObjectType::Photon ) {
    ATH_CHECK( this->selectFromEgamma( mask, caloCellContainer, static_cast<const xAOD::Egamma*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Muon ) {
    ATH_CHECK( this->selectFromMuon( mask, caloCellContainer, static_cast<const xAOD::Muon*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Tau ) {
    ATH_CHECK( this->selectFromTauJet( mask, caloCellContainer, static_cast<const xAOD::TauJet*>(part) ));
  }
  else if ( objType == xAOD::Type::ObjectType::Jet ) {
    ATH_CHECK( this->selectFromJet( mask, caloCellContainer, static_cast<const xAOD::Jet*>(part) ));
  }
  else {
    const xAOD::CompositeParticle* compPart = dynamic_cast<const xAOD::CompositeParticle*>(part);
    if ( compPart ) {
      ATH_CHECK( this->selectFromCompositeParticle( mask, caloCellContainer, compPart ));
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Couldn't determine the type of the current xAOD::IParticle with ObjectType = " << objType );
  }

  return StatusCode::SUCCESS;
}







//=============================================================================
// Thinning method for xAOD::CaloCluster
//=============================================================================
StatusCode
ThinCaloCellsTool::selectFromCaloCluster( std::vector<bool>& mask,
                                          const CaloCellContainer* caloCellContainer,
                                          const xAOD::CaloCluster* clus ) const
{
  ATH_MSG_VERBOSE("In selectFromIParticle");
  if ( !clus ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Get the CaloCell links
  const CaloClusterCellLink* cccl = clus->getCellLinks();

  // Check if this cell link points to the right container
  const CaloCellContainer* ccc = cccl->getCellContainer();
  if ( ccc != caloCellContainer ) {
    ATH_MSG_DEBUG("points to wrong cell container");
    return StatusCode::SUCCESS;
  }

  // Loop over all CaloClusterCellLinks
  CaloClusterCellLink::const_iterator cellItr    = cccl->begin();
  CaloClusterCellLink::const_iterator cellItrEnd = cccl->end();
  for ( ; cellItr != cellItrEnd; ++cellItr ) {
    unsigned index = cellItr.index();
    mask[index] = true;
  }

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::Egamma
//=============================================================================
StatusCode
ThinCaloCellsTool::selectFromEgamma( std::vector<bool>& mask,
                                     const CaloCellContainer* caloCellContainer,
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
    const xAOD::CaloCluster* clus = part->caloCluster(i);
    ATH_CHECK( this->selectFromCaloCluster( mask,  caloCellContainer, clus ) );
  } // End: loop over all CaloClusters from this Egamma

  return StatusCode::SUCCESS;
}





//=============================================================================
// Thinning method for xAOD::Muon
//=============================================================================
StatusCode
ThinCaloCellsTool::selectFromMuon( std::vector<bool>& mask,
                                   const CaloCellContainer* caloCellContainer,
                                   const xAOD::Muon* part) const
{
  ATH_MSG_VERBOSE("In selectFromMuon");
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the CaloCluster and check that it points to the given calo cluster container
  const xAOD::CaloCluster* clus = part->cluster();
  ATH_CHECK( this->selectFromCaloCluster( mask,  caloCellContainer, clus ) );

  return StatusCode::SUCCESS;
}






//=============================================================================
// Thinning method for xAOD::TauJet
//=============================================================================
StatusCode
ThinCaloCellsTool::selectFromTauJet( std::vector<bool>& mask,
                                     const CaloCellContainer* caloCellContainer,
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
    ATH_CHECK( this->selectFromJet( mask, caloCellContainer, *jetLink ) );
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
ThinCaloCellsTool::selectFromJet( std::vector<bool>& mask,
                                  const CaloCellContainer* caloCellContainer,
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
    ATH_CHECK( this->selectFromIParticleLink( mask, caloCellContainer, partLink ) );
  } // End: loop over all constituents from this Jet

  return StatusCode::SUCCESS;
}




//=============================================================================
// Thinning method for xAOD::CompositeParticle
//=============================================================================
StatusCode
ThinCaloCellsTool::selectFromCompositeParticle( std::vector<bool>& mask,
                                                const CaloCellContainer* caloCellContainer,
                                                const xAOD::CompositeParticle* part ) const
{
  if ( !part ) {
    ATH_MSG_WARNING("Got a zero pointer. Continuing...");
    return StatusCode::SUCCESS;
  }

  // Now, get the TrackParticle and check that it points to the given track particle container
  const auto& partLinks = part->partLinks();
  for ( const xAOD::IParticleLink& partLink : partLinks ) {
    ATH_CHECK( this->selectFromIParticleLink( mask, caloCellContainer, partLink ) );
  } // End: loop over all constituents from this Jet

  return StatusCode::SUCCESS;
}


#endif
