/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerElectronsReader.cxx 786306 2016-11-24 13:40:42Z wsfreund $
// =============================================================================
#include "CaloRingerElectronsReader.h"

#include <algorithm>

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "PATCore/TAccept.h"

namespace Ringer {

// =============================================================================
CaloRingerElectronsReader::CaloRingerElectronsReader(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent) :
  CaloRingerInputReader(type, name, parent),
  m_container(nullptr),
  m_constContainer(nullptr),
  m_clRingsBuilderElectronFctor(nullptr)
{

  // declare interface
  declareInterface<ICaloRingerElectronsReader>(this);

}

// =============================================================================
CaloRingerElectronsReader::~CaloRingerElectronsReader()
{ 
  delete m_clRingsBuilderElectronFctor;
}

// =============================================================================
StatusCode CaloRingerElectronsReader::initialize()
{

  CHECK( CaloRingerInputReader::initialize() );

  if ( m_selectorsAvailable ) {
    CHECK( retrieveSelectors() );
  }

  if ( m_builderAvailable ) {
    // Initialize our fctor
    m_clRingsBuilderElectronFctor =
      new BuildCaloRingsFctor<xAOD::Electron>(
        m_crBuilder,
        msg()
      );
  }

  return StatusCode::SUCCESS;

}

// =============================================================================
StatusCode CaloRingerElectronsReader::retrieveSelectors()
{
  ATH_MSG_INFO("Retrieving " << m_ringerSelectors.size() <<
    " reader tools for " << name() );

  for (const auto& tool : m_ringerSelectors)
  {
    if ( tool.retrieve().isFailure() )
    {
      ATH_MSG_FATAL( "Could not get tool: " << tool );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode CaloRingerElectronsReader::finalize()
{
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode CaloRingerElectronsReader::execute()
{

  ATH_MSG_DEBUG("Entering " << name() << " execute, m_builderAvailable = " << m_builderAvailable);

  xAOD::ElectronAuxContainer* crAux{nullptr};

  // Retrieve electrons
  if ( m_builderAvailable ) {

    if ( evtStore()->retrieve( m_container, m_inputKey).isFailure() )
    {
      ATH_MSG_ERROR("Cannot retrieve electron container " << m_inputKey );
      return StatusCode::FAILURE;
    }
  } else {

    ATH_MSG_DEBUG("No builder available, will work on copies!");

    if ( evtStore()->retrieve( m_constContainer, m_inputKey).isFailure() ){
      ATH_MSG_ERROR("Cannot retrieve electron container " << m_inputKey );
      return StatusCode::FAILURE;
    }

    // Create container to hold new information
    m_container = new xAOD::ElectronContainer();
    m_container->reserve( m_constContainer->size() );

    // Create its aux container and set it:
    crAux = new xAOD::ElectronAuxContainer();
    crAux->reserve( m_constContainer->size() );
    m_container->setStore( crAux );

    // Copy information to non_const container
    for ( const xAOD::Electron *el : *m_constContainer ) {
      auto elCopy = new xAOD::Electron(*el);
      m_container->push_back( elCopy );
    }
  }

  // Check if requested to run CaloRings Builder:
  if ( m_builderAvailable ) {
    // Set current container size:
    m_clRingsBuilderElectronFctor->prepareToLoopFor(m_container->size());

    // loop over our particles:
    std::for_each( 
        m_container->begin(), 
        m_container->end(), 
        *m_clRingsBuilderElectronFctor );
  }

  StatusCode sc(SUCCESS);

  // Run selectors, if available:
  for ( const auto& selector : m_ringerSelectors ) {
    for ( xAOD::Electron *el : *m_container ) {

      // Execute selector for each electron
      StatusCode lsc = selector->execute(el);
      sc = lsc && sc;

      if ( lsc.isFailure() ){
        ATH_MSG_WARNING("Error while executing selector: " << 
            selector->name());
      }

      // Retrieve results:
      const Root::TAccept& accept = selector->getTAccept(); 
      const std::vector<float> &outputSpace = selector->getOutputSpace(); 

      ATH_MSG_DEBUG( "Result for " << selector->name() << " is: " 
          << std::boolalpha << static_cast<bool>(accept) 
          << " and its outputSpace is: "
          << std::noboolalpha << outputSpace);

      // Save the bool result
      el->setPassSelection(
          static_cast<char>(accept), 
          selector->name()
        );

      //// Save the resulting bitmask
      el->setSelectionisEM(
          static_cast<unsigned int>(accept.getCutResultInverted()), 
          selector->name() + std::string("_isEM")
        );

      // Check if output space is empty, if so, use error code
      float outputToSave(std::numeric_limits<float>::min());
      if ( !outputSpace.empty() ){
        outputToSave = outputSpace[0];
      }

      // Save chain output
      el->setLikelihoodValue(
          outputToSave,
          selector->name() + std::string("_output")
        );
    }
  }

  if ( ! m_builderAvailable ) {
    // In case we worked on copies, place the container on EventStore and set
    // it to be const:
    CHECK( evtStore()->overwrite( crAux, 
          m_inputKey + "Aux.", 
          /*allowMods = */ false) );
    CHECK( evtStore()->overwrite( m_container, 
          m_inputKey, 
          /*allowMods = */ false) );
  }

  return sc;

}

} // namespace Ringer


