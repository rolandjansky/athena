/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerElectronsReader.cxx 786306 2016-11-24 13:40:42Z wsfreund $
// =============================================================================
#include "CaloRingerElectronsReader.h"

#include <algorithm>

#include "PATCore/TAccept.h"
#include "StoreGate/ReadHandle.h"

namespace Ringer {

// =============================================================================
CaloRingerElectronsReader::CaloRingerElectronsReader(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent) :
  CaloRingerInputReader(type, name, parent),
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

  ATH_CHECK(m_inputElectronContainerKey.initialize());


  m_selectorDecorHandleKeys.setContName(m_inputElectronContainerKey.key());

  if ( m_selectorsAvailable ) {
    CHECK( retrieveSelectors() );
  }

  if ( m_builderAvailable ) {
    // Initialize our fctor
    m_clRingsBuilderElectronFctor =
      new BuildCaloRingsFctor<const xAOD::Electron>(
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

    ATH_CHECK(m_selectorDecorHandleKeys.addSelector(tool->name()));

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

   // Retrieve photons 
  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_inputElectronContainerKey);
  // check is only used for serial running; remove when MT scheduler used
  if(!electrons.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_inputElectronContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Check if requested to run CaloRings Builder:
  if ( m_builderAvailable ) {
    // Set current container size:
    m_clRingsBuilderElectronFctor->prepareToLoopFor(electrons->size());

    // loop over our particles:
    std::for_each( 
        electrons->begin(), 
        electrons->end(), 
        *m_clRingsBuilderElectronFctor );
  }

  StatusCode sc(SUCCESS);

  writeDecorHandles<xAOD::ElectronContainer> decoHandles(m_selectorDecorHandleKeys);

  // Run selectors, if available:
  for ( size_t i = 0; i < m_ringerSelectors.size(); i++ ) {
    for ( const xAOD::Electron *el : *electrons ) {

      const auto& selector = m_ringerSelectors[i];

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
      decoHandles.sel(i)(*el) = static_cast<char>(accept);

      //// Save the resulting bitmask
      decoHandles.isEM(i)(*el) = static_cast<unsigned int>(accept.getCutResultInverted());

      // Check if output space is empty, if so, use error code
      float outputToSave(std::numeric_limits<float>::min());
      if ( !outputSpace.empty() ){
        outputToSave = outputSpace[0];
      }

      // Save chain output
      decoHandles.lhood(i)(*el) = outputToSave;
    }
  }

  return sc;

}

} // namespace Ringer


