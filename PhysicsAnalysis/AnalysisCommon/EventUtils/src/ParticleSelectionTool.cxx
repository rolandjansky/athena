///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSelectionTool.cxx
// Implementation file for class ParticleSelectionTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////


// EventUtils includes
#include "ParticleSelectionTool.h"

// STL includes
#include <vector>
#include <string>

// EDM includes
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODTracking/NeutralParticle.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODTracking/NeutralParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleAuxContainer.h"
#include "xAODParticleEvent/Particle.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ParticleSelectionTool::ParticleSelectionTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent ) :
  ExpressionParserUserWithTrigSupport<::AthAlgTool>  ( type, name, parent ),
  m_inCollKey(""),
  m_outCollKey(""),
  m_outCollType(""),
  m_writeSplitAux(false),
  m_outOwnPolicy(SG::VIEW_ELEMENTS),
  m_outLinkCollKey(""),
  m_selection(""),
  m_contID(0),
  m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::IAugmentationTool >(this);

  declareProperty("InputContainer",      m_inCollKey="",   "Input container name" );

  declareProperty("OutputContainer",     m_outCollKey="",
                  "The name of the output container with the copy of the selected xAOD::IParticles" );

  declareProperty("OutputContainerType", m_outCollType="",
                  "The type of the output container, e.g., 'xAOD::JetContainer'" );

  declareProperty("WriteSplitOutputContainer", m_writeSplitAux=false,
                  "Decide if we want to write a fully-split AuxContainer such that we can remove any variables" );

  declareProperty("OutputContainerOwnershipPolicy", m_outOwnPolicyName="OWN_ELEMENTS",
                  "Defines the ownership policy of the output container (default: 'OWN_ELEMENTS'; also allowed: 'VIEW_ELEMENTS')" );

  declareProperty("OutputLinkContainer", m_outLinkCollKey="",
                  "The name of the output container of ElementLinks to the selected xAOD::IParticles" );

  declareProperty("Selection",           m_selection="",
                  "The selection string that defines which xAOD::IParticles to select from the container" );
}

// Destructor
///////////////
ParticleSelectionTool::~ParticleSelectionTool()
{}



// Athena algtool's Hooks
////////////////////////////
StatusCode ParticleSelectionTool::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_inCollKey );
  ATH_MSG_DEBUG ( " using = " << m_outCollKey );
  ATH_MSG_DEBUG ( " using = " << m_outCollType );
  ATH_MSG_DEBUG ( " using = " << m_writeSplitAux );
  ATH_MSG_DEBUG ( " using = " << m_outOwnPolicyName );
  ATH_MSG_DEBUG ( " using = " << m_outLinkCollKey );
  ATH_MSG_DEBUG ( " using = " << m_selection );


  // load the expressions
  ATH_CHECK(initializeParser(m_selection.value()));

  // initialize the counters
  m_contID = 0;
  m_nEventsProcessed = 0;

  // Determine the output container type
  if ( !(m_outCollType.value().empty()) ) {
    if ( m_outCollType.value() == "xAOD::MuonContainer" ) { m_contID = 1; }
    else if ( m_outCollType.value() == "xAOD::JetContainer" ) { m_contID = 2; }
    else if ( m_outCollType.value() == "xAOD::ElectronContainer" ) { m_contID = 3; }
    else if ( m_outCollType.value() == "xAOD::PhotonContainer" ) { m_contID = 4; }
    else if ( m_outCollType.value() == "xAOD::TauJetContainer" ) { m_contID = 5; }
    else if ( m_outCollType.value() == "xAOD::PFOContainer" ) { m_contID = 6; }
    else if ( m_outCollType.value() == "xAOD::NeutralParticleContainer" ) { m_contID = 7; }
    else if ( m_outCollType.value() == "xAOD::TrackParticleContainer" ) { m_contID = 8; }
    else if ( m_outCollType.value() == "xAOD::TruthParticleContainer" ) { m_contID = 9; }
    else if ( m_outCollType.value() == "xAOD::CompositeParticleContainer" ) { m_contID = 10; }
    else if ( m_outCollType.value() == "xAOD::ParticleContainer" ) { m_contID = 11; }
    else if ( m_outCollType.value() == "xAOD::CaloClusterContainer" ) { m_contID = 12; }
    else {
      ATH_MSG_WARNING("I got a non-empty string for the OutputContainerType property "
                      << "with value " << m_outCollType.value()
                      << " which I don't understand. Possible values are: "
                      << "'xAOD::MuonContainer', "
                      << "'xAOD::JetContainer', "
                      << "'xAOD::ElectronContainer', "
                      << "'xAOD::PhotonContainer', "
                      << "'xAOD::TauJetContainer', "
                      << "'xAOD::PFOContainer', "
                      << "'xAOD::NeutralParticleContainer', "
                      << "'xAOD::TrackParticleContainer', "
                      << "'xAOD::TruthParticleContainer', "
                      << "'xAOD::CompositeParticleContainer', "
                      << "'xAOD::ParticleContainer', "
                      << "'xAOD::CaloClusterContainer'" );
    }
  }

  // Determine the ownership policy of the output container
  if ( m_outOwnPolicyName.value() == "OWN_ELEMENTS" ) {
    m_outOwnPolicy = SG::OWN_ELEMENTS;
  }
  else if ( m_outOwnPolicyName.value() == "VIEW_ELEMENTS" ) {
    m_outOwnPolicy = SG::VIEW_ELEMENTS;
  }
  else {
    ATH_MSG_ERROR("Unrecognized ownership policy for the output container: " << m_outOwnPolicyName );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}




StatusCode ParticleSelectionTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  ATH_CHECK( finalizeParser() );
  return StatusCode::SUCCESS;
}




StatusCode ParticleSelectionTool::addBranches() const
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> addBranches " << name() << " on " << m_nEventsProcessed << ". event..." );

  //-----------------------------------------
  // The meat happens here
  //-----------------------------------------
  ExpressionParsing::StackElement selectionResult = m_parser->evaluate();

  if ( selectionResult.isScalar() ) {
    ATH_MSG_ERROR( "We are expecting a vector result such that we can deduct "
                   << "which xAOD::IParticle inside the container passes and which doesn't. "
                   << "For example: 'Muons.pt>10*GeV', but NOT 'count(Muons.pt>10*GeV)>1'; here, "
                   << "the former gives us a boolean answer for every muon while "
                   << "the later gives us a boolean answer for the whole container of muons." );
    return StatusCode::FAILURE;
  }

  if ( selectionResult.isVector() ) {
    // We found a vector. Now, we can go ahead and retrieve the input container
    // and book the output container(s)
    const std::vector<int>& resultVec( selectionResult.vectorValue<int>() );


    // Get the input container and create the output containers
    const xAOD::IParticleLinkContainer* inLinkContainer(0);
    const xAOD::IParticleContainer* inContainer(0);
    xAOD::IParticleLinkContainer* outLinkContainer(0);

    // Prepare all containers as needed
    ATH_CHECK( this->prepareContainers( inLinkContainer,
                                        inContainer,
                                        outLinkContainer ) );


    // Check that the lengths are the same
    if ( inLinkContainer && inLinkContainer->size() != resultVec.size() ) {
      ATH_MSG_ERROR("We got an input link container, but its size (" << inLinkContainer->size()
                    << ") doesn't match the size of the result vector: " << resultVec.size() );
      return StatusCode::FAILURE;
    }
    if ( inContainer && inContainer->size() != resultVec.size() ) {
      ATH_MSG_ERROR("We got an input container, but its size (" << inContainer->size()
                    << ") doesn't match the size of the result vector: " << resultVec.size() );
      return StatusCode::FAILURE;
    }




    // Actually, evaluate the result
    ATH_CHECK( this->evaluate( resultVec, inLinkContainer, inContainer, outLinkContainer ) );


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




StatusCode
ParticleSelectionTool::prepareContainers( const xAOD::IParticleLinkContainer*& inLinkContainer,
                                          const xAOD::IParticleContainer*& inContainer,
                                          xAOD::IParticleLinkContainer*& outLinkContainer ) const
{
  if ( evtStore()->contains< xAOD::IParticleLinkContainer >( m_inCollKey ) ) {
    // Actually retrieve the LinkContainer from StoreGate
    ATH_CHECK( evtStore()->retrieve( inLinkContainer , m_inCollKey ) );
    ATH_MSG_DEBUG ( "Input link collection = '" << m_inCollKey
                    << "' retrieved from StoreGate which has " << inLinkContainer->size() << " entries." );
  }
  else if ( evtStore()->contains< xAOD::IParticleContainer >( m_inCollKey ) ) {
    // This container holds an xAOD::IParticleContainer
    ATH_CHECK( evtStore()->retrieve( inContainer, m_inCollKey ) );
    ATH_MSG_DEBUG ( "Input collection = " << m_inCollKey
                    << " retrieved from StoreGate which has " << inContainer->size() << " entries." );
  }
  else {
    if ( m_nEventsProcessed <= 10 ) {
      ATH_MSG_WARNING ( "Input collection  = " << m_inCollKey
                        << " could not be retrieved from StoreGate! "
                        << " This message will only be repeated 10 times..." );
    }
    else {
      ATH_MSG_DEBUG ( "Input collection  = " << m_inCollKey
                      << " could not be retrieved from StoreGate! " );
    }
    return StatusCode::SUCCESS;
  } // End: if/elif/else is link container


  // Create the IParticleLinkContainer for the output, if requested
  if ( !(m_outLinkCollKey.value().empty()) ) {
    outLinkContainer = new xAOD::IParticleLinkContainer();
    ATH_CHECK( evtStore()->record( outLinkContainer, m_outLinkCollKey.value() ) );
    ATH_MSG_DEBUG( "Recorded xAOD::IParticleLinkContainer with key: "<< m_outLinkCollKey.value() );
  }

  return StatusCode::SUCCESS;
}


// Define a pre-processor macro to easily create the new output containers.
// First, create all needed zero pointers.
// Second, create the output container and record it in StoreGate.
// Finally, try to figure out the most derived type of the input container and
// create the xAOD container and its auxiliary store:
#define CREATE_OUT_CONTAINERS( CONTAINERTYPE, AUXCONTAINERTYPE,                                  \
                               INCONTAINERNAME, OUTCONTAINERNAME, CONTID )                       \
  const CONTAINERTYPE* INCONTAINERNAME(0);                                                       \
  CONTAINERTYPE*  OUTCONTAINERNAME(0);                                                           \
  if ( (m_contID==0 || m_contID==CONTID) && !(m_outCollKey.value().empty())  ) {                 \
    if ( inContainer ) {                                                                         \
      ATH_MSG_VERBOSE("Trying to cast input container to type const "#CONTAINERTYPE"*" );        \
      INCONTAINERNAME = dynamic_cast<const CONTAINERTYPE*>(inContainer);                         \
      if ( INCONTAINERNAME ) {                                                                   \
        ATH_MSG_VERBOSE("Successfully casted input container to type const "#CONTAINERTYPE"*" ); \
        OUTCONTAINERNAME = new CONTAINERTYPE( m_outOwnPolicy );                                  \
        ATH_CHECK( evtStore()->record ( OUTCONTAINERNAME, m_outCollKey.value() ) );              \
        if ( m_outOwnPolicy == SG::OWN_ELEMENTS ) {                                              \
          if ( m_writeSplitAux.value() ) {                                                       \
            xAOD::AuxContainerBase* outAuxContainer = new xAOD::AuxContainerBase();              \
            ATH_CHECK( evtStore()->record( outAuxContainer, m_outCollKey.value() + "Aux." ) );   \
            OUTCONTAINERNAME->setStore( outAuxContainer );                                       \
          }                                                                                      \
          else {                                                                                 \
            AUXCONTAINERTYPE* outAuxContainer = new AUXCONTAINERTYPE();                          \
            ATH_CHECK( evtStore()->record( outAuxContainer, m_outCollKey.value() + "Aux." ) );   \
            OUTCONTAINERNAME->setStore( outAuxContainer );                                       \
          }                                                                                      \
        }                                                                                        \
        m_contID=CONTID;                                                                         \
        ATH_MSG_DEBUG( "Recorded "#CONTAINERTYPE" with key: " << m_outCollKey.value()            \
                       << ", container identifier " << m_contID                                  \
                       << " and address " << OUTCONTAINERNAME );                                 \
      }                                                                                          \
      else {                                                                                     \
        ATH_MSG_DEBUG("Couldn't cast input container to type const "#CONTAINERTYPE"*" );         \
      }                                                                                          \
    }                                                                                            \
    else if ( inLinkContainer && m_contID==CONTID ) {                                            \
      OUTCONTAINERNAME = new CONTAINERTYPE( m_outOwnPolicy );                                    \
      ATH_CHECK( evtStore()->record ( OUTCONTAINERNAME, m_outCollKey.value() ) );                \
      if ( m_outOwnPolicy == SG::OWN_ELEMENTS ) {                                                \
        AUXCONTAINERTYPE* outAuxContainer = new AUXCONTAINERTYPE();                              \
        ATH_CHECK( evtStore()->record( outAuxContainer, m_outCollKey.value() + "Aux." ) );       \
        OUTCONTAINERNAME->setStore( outAuxContainer );                                           \
      }                                                                                          \
      m_contID=CONTID;                                                                           \
      ATH_MSG_DEBUG( "Recorded "#CONTAINERTYPE" with key: " << m_outCollKey.value()              \
                     << ", container identifier " << m_contID                                    \
                     << " and address " << OUTCONTAINERNAME );                                   \
    }                                                                                            \
  }



// Fill the output container, if requested
#define FILL_OUT_CONTAINER( CONTAINERTYPE, PARTICLETYPE,                                       \
                            INCONTAINERNAME, OUTCONTAINERNAME )                                \
  if ( OUTCONTAINERNAME ) {                                                                    \
    if ( inLinkContainer ) {                                                                   \
      ATH_MSG_VERBOSE(" Going to fill output container of type "#CONTAINERTYPE" from link input" ); \
      const xAOD::IParticleLink& partLink = inLinkContainer->at(i);                            \
      if ( partLink.isValid() ) {                                                              \
        const PARTICLETYPE* partPtr = dynamic_cast<const PARTICLETYPE*>(*partLink);            \
        if (partPtr) {                                                                         \
          if ( m_outOwnPolicy == SG::OWN_ELEMENTS ) {                                          \
            PARTICLETYPE* newPart = new PARTICLETYPE();                                        \
            newPart->makePrivateStore(*partPtr);                                               \
            OUTCONTAINERNAME->push_back( newPart );                                            \
          }                                                                                    \
          else {                                                                               \
            OUTCONTAINERNAME->push_back( const_cast<PARTICLETYPE*>(partPtr) );                 \
          }                                                                                    \
          ATH_MSG_VERBOSE("Adding an "#PARTICLETYPE" to the output "                           \
                          "from the input xAOD::IParticleLinkContainer");                      \
        }                                                                                      \
        else {                                                                                 \
          ATH_MSG_WARNING("Couldn't cast to a "#PARTICLETYPE". This should NOT happen!");      \
        }                                                                                      \
      }                                                                                        \
      else {                                                                                   \
        ATH_MSG_WARNING("Couldn't add an xAOD::IParticle to the output "                       \
                        << "because the input ElementLink is not valid!");                     \
      }                                                                                        \
    }                                                                                          \
    if ( INCONTAINERNAME ) {                                                                   \
      ATH_MSG_VERBOSE(" Going to fill output container of type "#CONTAINERTYPE" from input" ); \
      const PARTICLETYPE* partPtr = INCONTAINERNAME->at(i);                                    \
      if ( m_outOwnPolicy == SG::OWN_ELEMENTS ) {                                              \
        PARTICLETYPE* newPart = new PARTICLETYPE();                                            \
        newPart->makePrivateStore(*partPtr);                                                   \
        OUTCONTAINERNAME->push_back( newPart );                                                \
      }                                                                                        \
      else {                                                                                   \
        OUTCONTAINERNAME->push_back( const_cast<PARTICLETYPE*>(partPtr) );                     \
      }                                                                                        \
      ATH_MSG_VERBOSE("Adding an "#PARTICLETYPE" to the output "                               \
                      "from the input "#CONTAINERTYPE );                                       \
    }                                                                                          \
  } /* End: if ( outContainer ) */





StatusCode
ParticleSelectionTool::evaluate( const std::vector<int>& resultVec,
                                 const xAOD::IParticleLinkContainer* inLinkContainer,
                                 const xAOD::IParticleContainer* inContainer,
                                 xAOD::IParticleLinkContainer* outLinkContainer  ) const
{
  // See if we already found the right container type
  CREATE_OUT_CONTAINERS( xAOD::MuonContainer, xAOD::MuonAuxContainer,
                         muonInCont, muonOutContainer, 1 )
  CREATE_OUT_CONTAINERS( xAOD::JetContainer, xAOD::JetAuxContainer,
                         jetInCont, jetOutContainer, 2 )
  CREATE_OUT_CONTAINERS( xAOD::ElectronContainer, xAOD::ElectronAuxContainer,
                         electronInCont, electronOutContainer, 3 )
  CREATE_OUT_CONTAINERS( xAOD::PhotonContainer, xAOD::PhotonAuxContainer,
                         photonInCont, photonOutContainer, 4 )
  CREATE_OUT_CONTAINERS( xAOD::TauJetContainer, xAOD::TauJetAuxContainer,
                         tauJetInCont, tauJetOutContainer, 5 )
  CREATE_OUT_CONTAINERS( xAOD::PFOContainer, xAOD::PFOAuxContainer,
                         pfoInCont, pfoOutContainer, 6 )
  CREATE_OUT_CONTAINERS( xAOD::NeutralParticleContainer, xAOD::NeutralParticleAuxContainer,
                         neutralParticleInCont, neutralParticleOutContainer, 7 )
  CREATE_OUT_CONTAINERS( xAOD::TrackParticleContainer, xAOD::TrackParticleAuxContainer,
                         trackParticleInCont, trackParticleOutContainer, 8 )
  CREATE_OUT_CONTAINERS( xAOD::TruthParticleContainer, xAOD::TruthParticleAuxContainer,
                         truthParticleInCont, truthParticleOutContainer, 9 )
  CREATE_OUT_CONTAINERS( xAOD::CompositeParticleContainer, xAOD::CompositeParticleAuxContainer,
                         compositeParticleInCont, compositeParticleOutContainer, 10 )
  CREATE_OUT_CONTAINERS( xAOD::ParticleContainer, xAOD::ParticleAuxContainer,
                         particleInCont, particleOutContainer, 11 )
  CREATE_OUT_CONTAINERS( xAOD::CaloClusterContainer, xAOD::CaloClusterAuxContainer,
                         caloClusterInCont, caloClusterOutContainer, 12 )


  // Now, loop over the result vector and check which particles to write out
  for ( std::size_t i=0; i<resultVec.size(); ++i ) {
    // If this particle is not accepted, go to the next one
    if ( !(resultVec[i]) ) { continue; }

    // Fill the output IParticleLinkContainer, if requested
    if ( outLinkContainer ) {
      ATH_MSG_VERBOSE("Going to fill output link container");
      if ( inLinkContainer ) {
        outLinkContainer->push_back( inLinkContainer->at(i) );
        ATH_MSG_VERBOSE("Adding an xAOD::IParticleLink to the output "
                        "from the input xAOD::IParticleLinkContainer");
      }
      if ( inContainer ) {
        xAOD::IParticleLink partLink = xAOD::IParticleLink( *inContainer, i );
        outLinkContainer->push_back( partLink );
        ATH_MSG_VERBOSE("Adding an xAOD::IParticleLink to the output "
                        "from the input xAOD::IParticleContainer");
      }
    } // End: if ( outLinkContainer )

    if ( !(m_outCollKey.value().empty()) ) {
      ATH_MSG_VERBOSE("Going to fill output container");
      FILL_OUT_CONTAINER( xAOD::MuonContainer, xAOD::Muon,
                          muonInCont, muonOutContainer )
      FILL_OUT_CONTAINER( xAOD::JetContainer, xAOD::Jet,
                          jetInCont, jetOutContainer )
      FILL_OUT_CONTAINER( xAOD::ElectronContainer, xAOD::Electron,
                          electronInCont, electronOutContainer )
      FILL_OUT_CONTAINER( xAOD::PhotonContainer, xAOD::Photon,
                          photonInCont, photonOutContainer )
      FILL_OUT_CONTAINER( xAOD::TauJetContainer, xAOD::TauJet,
                          tauJetInCont, tauJetOutContainer )
      FILL_OUT_CONTAINER( xAOD::PFOContainer, xAOD::PFO,
                          pfoInCont, pfoOutContainer )
      FILL_OUT_CONTAINER( xAOD::NeutralParticleContainer, xAOD::NeutralParticle,
                          neutralParticleInCont, neutralParticleOutContainer )
      FILL_OUT_CONTAINER( xAOD::TrackParticleContainer, xAOD::TrackParticle,
                          trackParticleInCont, trackParticleOutContainer )
      FILL_OUT_CONTAINER( xAOD::TruthParticleContainer, xAOD::TruthParticle,
                          truthParticleInCont, truthParticleOutContainer )
      FILL_OUT_CONTAINER( xAOD::CompositeParticleContainer, xAOD::CompositeParticle,
                          compositeParticleInCont, compositeParticleOutContainer )
      FILL_OUT_CONTAINER( xAOD::ParticleContainer, xAOD::Particle,
                          particleInCont, particleOutContainer )
      FILL_OUT_CONTAINER( xAOD::CaloClusterContainer, xAOD::CaloCluster,
                          caloClusterInCont, caloClusterOutContainer )
    }

  } // End: Loop over result vector (and input particle(link) vector)

  // Some final debug messages
  if ( !(m_outLinkCollKey.value().empty()) ) {
    ATH_MSG_DEBUG("Output link container with name " << m_outLinkCollKey.value()
                  << " has " << outLinkContainer->size() << " entries." );
  }
  if ( !(m_outCollKey.value().empty()) && msgLvl(MSG::DEBUG) ) {
    if (muonOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << muonOutContainer->size() << " entries." );
    }
    else if (jetOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << jetOutContainer->size() << " entries." );
    }
    else if (electronOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << electronOutContainer->size() << " entries." );
    }
    else if (photonOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << photonOutContainer->size() << " entries." );
    }
    else if (tauJetOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << tauJetOutContainer->size() << " entries." );
    }
    else if (pfoOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << pfoOutContainer->size() << " entries." );
    }
    else if (neutralParticleOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << neutralParticleOutContainer->size() << " entries." );
    }
    else if (trackParticleOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << trackParticleOutContainer->size() << " entries." );
    }
    else if (truthParticleOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << truthParticleOutContainer->size() << " entries." );
    }
    else if (compositeParticleOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << compositeParticleOutContainer->size() << " entries." );
    }
    else if (particleOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << particleOutContainer->size() << " entries." );
    }
    else if (caloClusterOutContainer) {
      ATH_MSG_DEBUG("Output container with name " << m_outCollKey.value()
                    << " has " << caloClusterOutContainer->size() << " entries." );
    }
  }

  return StatusCode::SUCCESS;
}
