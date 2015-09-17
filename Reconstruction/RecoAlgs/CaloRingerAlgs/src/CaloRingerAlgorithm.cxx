/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerAlgorithm.cxx 667872 2015-05-18 17:15:32Z wsfreund $
#include "CaloRingerAlgorithm.h"

// Framework includes:
#include "AthenaKernel/errorcheck.h"
#include "AthContainers/AuxElement.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloRingerTools/ICaloRingerInputReader.h"

// xAOD includes:
#include "xAODBase/IParticle.h"
#include "xAODCaloRings/CaloRingsContainer.h"

namespace Ringer {

// =================================================================================
CaloRingerAlgorithm::CaloRingerAlgorithm( const std::string& name,
			  ::ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  //

  // the input reader tools collection
  declareProperty("inputReaderTools", m_inputReaders ,
      "Sequence of reader tools to execute." );

}

// =================================================================================
CaloRingerAlgorithm::~CaloRingerAlgorithm() {}

// =================================================================================
StatusCode CaloRingerAlgorithm::initialize()
{

  CHECK( retrieveReaders() );

  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode CaloRingerAlgorithm::retrieveReaders()
{
  ATH_MSG_INFO("Retrieving " << m_inputReaders.size() <<
    " reader tools for " << name() );

  for (const auto& tool : m_inputReaders)
  {
    if ( tool.retrieve().isFailure() )
    {
      ATH_MSG_FATAL( "Could not get tool: " << tool );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode CaloRingerAlgorithm::finalize()
{
  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode CaloRingerAlgorithm::execute()
{

  ATH_MSG_INFO ("Executing " << name() << ".");

  // Loop over input readers
  for (const auto& readerTool : m_inputReaders)
  {
    ATH_MSG_DEBUG ("Executing Reader tool " << readerTool.name() << ".");

    // Execute reader 
    CHECK( readerTool->execute() );

  }

  return StatusCode::SUCCESS;
}

} // namespace Ringer
