/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// First the corresponding header.
#include "MUCTPI_AthAlg.h"
#include "TrigT1Interfaces/IMuctpiSimTool.h"

namespace LVL1MUCTPIPHASE1 {
  MUCTPI_AthAlg::MUCTPI_AthAlg( const std::string& name, ISvcLocator* pSvcLocator )
    :
    AthReentrantAlgorithm(name, pSvcLocator)
  {
  }
  
  /// Regular Gaudi algorithm initialization function
  StatusCode MUCTPI_AthAlg::initialize()
  {
    ATH_MSG_INFO( "Initialisation for MUCTPI_AthAlg"  );

    ATH_MSG_DEBUG("retrieving " << m_muctpiTool);
    CHECK( m_muctpiTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  /// Regular Gaudi algorithm execute function
  StatusCode MUCTPI_AthAlg::execute(const EventContext& /*eventContext*/) const
  {
    ATH_MSG_DEBUG( "Executing MUCTPI_AthAlg" );

    CHECK( m_muctpiTool->execute() );

    return StatusCode::SUCCESS;
  }

}


