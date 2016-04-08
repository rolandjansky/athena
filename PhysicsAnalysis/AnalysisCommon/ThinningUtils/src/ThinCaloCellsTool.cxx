/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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



//=============================================================================
// Constructor
//=============================================================================
ThinCaloCellsTool::ThinCaloCellsTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent ) :
  ::AthAlgTool( type, name, parent ),
  m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
  m_nTotalCaloCells(0),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IThinningTool >(this);

  // Properties go here
  declareProperty("ThinningSvc",          m_thinningSvc,
                  "The ThinningSvc instance for a particular output stream" );
  declareProperty("CaloCellsToThin",      m_caloCellKey = "AllCells",
                  "The CaloCellContainer to be thinned" );
  declareProperty("InputContainerList",   m_inCollKeyList,
                  "Containers from which to extract the information which CaloCells should be kept" );
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
  ATH_MSG_DEBUG ( " using " << m_thinningSvc );
  ATH_MSG_DEBUG ( " using " << m_caloCellKey );
  ATH_MSG_DEBUG ( " using " << m_inCollKeyList );

  // Get pointer to ThinningSvc and cache it :
  // m_thinningSvc is of type IThinningSvc
  ATH_CHECK ( m_thinningSvc.retrieve() );

  // Initialize the counters
  m_nTotalCaloCells  = 0;
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
  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event
  ATH_MSG_DEBUG ( "==> doThinning " << name() << " on " << m_nEventsProcessed << ". event..." );

  // retrive the caloCellContainer
  const CaloCellContainer* caloCellContainer;
  ATH_CHECK( evtStore()->retrieve(caloCellContainer, m_caloCellKey.value()) );
  ATH_MSG_DEBUG ( "Container '" << m_caloCellKey.value() << "' retrieved from StoreGate" );
  m_nTotalCaloCells  = caloCellContainer->size();

  // Create the mask to be used for thinning
  std::vector<bool> mask(m_nTotalCaloCells, false);

  // Now, retrieve the other containers that are holding the information which
  // CaloCells should be kept
  for ( const auto& inKey : m_inCollKeyList.value() ) {

    if ( evtStore()->contains< xAOD::CaloClusterContainer >( inKey ) ) {
      // This file holds an xAOD::CaloClusterContainer
      const xAOD::CaloClusterContainer* inContainer;
      ATH_CHECK( evtStore()->retrieve( inContainer, inKey ) );
      ATH_MSG_DEBUG ( "Input collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const auto* cluster : *inContainer ) {
        ATH_CHECK( this->selectFromCaloCluster( mask, caloCellContainer, cluster ) );
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

    else if ( evtStore()->contains< xAOD::IParticleLinkContainer >( inKey ) ) {
      const xAOD::IParticleLinkContainer* inLinkContainer;
      ATH_CHECK( evtStore()->retrieve( inLinkContainer , inKey ) );
      ATH_MSG_DEBUG ( "Input link collection = '" << inKey
                      << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
      // Iterate over all given inputs and find their associated CaloClusters
      for ( const xAOD::IParticleLink& partLink : *inLinkContainer ) {
        ATH_CHECK( this->selectFromIParticleLink( mask, caloCellContainer, partLink ) );
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
        ATH_CHECK( this->selectFromEgamma( mask, caloCellContainer, part ) );
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
        ATH_CHECK( this->selectFromMuon( mask, caloCellContainer, part ) );
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
        ATH_CHECK( this->selectFromTauJet( mask, caloCellContainer, part ) );
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
        ATH_CHECK( this->selectFromJet( mask, caloCellContainer, part ) );
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
        ATH_CHECK( this->selectFromCompositeParticle( mask, caloCellContainer, part ) );
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
        ATH_CHECK( this->selectFromIParticle( mask, caloCellContainer, part ) );
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

  // Perform the actual thinning
  ATH_CHECK ( m_thinningSvc->filter(*caloCellContainer, mask, IThinningSvc::Operator::Or) );

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
