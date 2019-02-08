/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerElectronsReader.cxx 786306 2016-11-24 13:40:42Z wsfreund $
// =============================================================================
#include "CaloRingerElectronsReader.h"

#include <algorithm>

#include "PATCore/AcceptData.h"
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

  ATH_CHECK( CaloRingerInputReader::initialize() );

  ATH_CHECK(m_inputElectronContainerKey.initialize());

  if ( m_selectorsAvailable ) {
    ATH_CHECK( retrieveSelectors() );
  }

  // initialize the selectors
  ATH_CHECK(m_selKeys.initialize());
  ATH_CHECK(m_isEMKeys.initialize());
  ATH_CHECK(m_lhoodKeys.initialize());

  if ( m_builderAvailable ) {
    // Initialize our fctor
    m_clRingsBuilderElectronFctor =
      new BuildCaloRingsFctor<xAOD::ElectronContainer>(
        m_inputElectronContainerKey.key(),
        m_crBuilder,
        msg(),
	this
      );
    ATH_CHECK( m_clRingsBuilderElectronFctor->initialize() );
  }

  return StatusCode::SUCCESS;

}

// =============================================================================
StatusCode CaloRingerElectronsReader::retrieveSelectors()
{
  ATH_MSG_INFO("Retrieving " << m_ringerSelectors.size() <<
    " reader tools for " << name() );

  ATH_CHECK(m_ringerSelectors.retrieve());

  const std::string& contName = m_inputElectronContainerKey.key();

  for (const auto& tool : m_ringerSelectors)
  {
    ATH_CHECK(addSelectorDeco(contName, tool->name()));
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode CaloRingerElectronsReader::addSelectorDeco(const std::string &contName,
						      const std::string &selName)
{
  m_selKeys.emplace_back(contName + "." + selName);
  m_isEMKeys.emplace_back(contName + "." + selName + "_isEM");
  m_lhoodKeys.emplace_back(contName + "." + selName + "_output");
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

   // Retrieve electrons
  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_inputElectronContainerKey);
  // check is only used for serial running; remove when MT scheduler used
  if(!electrons.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_inputElectronContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Check if requested to run CaloRings Builder:
  if ( m_builderAvailable ) {
    ATH_CHECK( m_clRingsBuilderElectronFctor->prepareToLoopFor(electrons->size()) );

    // loop over our particles:
    for ( const auto& electron : *electrons ){
      m_clRingsBuilderElectronFctor->operator()( electron );
    }

  }

  StatusCode sc;

  auto selHandles = m_selKeys.makeHandles();
  auto isEMHandles = m_isEMKeys.makeHandles();
  auto lhoodHandles = m_selKeys.makeHandles();

  // Run selectors, if available:
  for ( size_t i = 0; i < m_ringerSelectors.size(); i++ ) {
    for ( const xAOD::Electron *el : *electrons ) {

      const auto& selector = m_ringerSelectors[i];

      // Execute selector for each electron
      asg::AcceptData acceptData (&selector->getAcceptInfo());
      StatusCode lsc = selector->execute(el, acceptData);
      sc &= lsc;

      if ( lsc.isFailure() ){
        ATH_MSG_WARNING("Error while executing selector: " <<
            selector->name());
      }

      // Retrieve results:
       const std::vector<float> &outputSpace = selector->getOutputSpace();

      ATH_MSG_DEBUG( "Result for " << selector->name() << " is: "
          << std::boolalpha << static_cast<bool>(acceptData)
          << " and its outputSpace is: "
          << std::noboolalpha << outputSpace);

      // Save the bool result
      selHandles[i](*el) = static_cast<bool>(acceptData);

      //// Save the resulting bitmask
      isEMHandles[i](*el) = static_cast<unsigned int>(acceptData.getCutResultInverted());

      // Check if output space is empty, if so, use error code
      float outputToSave(std::numeric_limits<float>::min());
      if ( !outputSpace.empty() ){
        outputToSave = outputSpace[0];
      }

      // Save chain output
      lhoodHandles[i](*el) = outputToSave;
    }
  }

  return sc;

}

} // namespace Ringer


