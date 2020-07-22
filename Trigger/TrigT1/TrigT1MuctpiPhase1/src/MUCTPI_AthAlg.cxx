/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/MUCTPI_AthAlg.h"
#include "TrigT1Interfaces/IMuctpiSimTool.h"

namespace LVL1MUCTPIPHASE1 {
  MUCTPI_AthAlg::MUCTPI_AthAlg( const std::string& name, ISvcLocator* pSvcLocator )
    :
    AthAlgorithm(name, pSvcLocator),
    m_muctpiTool("LVL1MUCTPIPHASE1__MUCTPI_AthTool/MUCTPI_AthTool")
  {
    declareProperty( "MUCTPI_AthTool", m_muctpiTool, "Tool to steer the MUCTPI simulation");
  }
  
  MUCTPI_AthAlg::~MUCTPI_AthAlg()
  {
    
  }



  /// Regular Gaudi algorithm initialization function
  StatusCode MUCTPI_AthAlg::initialize()
  {
    // Init message
    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Initialisation for MUCTPI_AthAlg"  );
    ATH_MSG_INFO( "Package version: " << PACKAGE_VERSION    );
    ATH_MSG_INFO( "=======================================" );

    ATH_MSG_DEBUG("retrieving " << m_muctpiTool);
    CHECK( m_muctpiTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  /// Regular Gaudi algorithm finalization function
  StatusCode MUCTPI_AthAlg::finalize()
  {
    ATH_MSG_DEBUG( "===============================" );
    ATH_MSG_DEBUG( "Finalizing MUCTPI_AthAlg" );
    ATH_MSG_DEBUG( "===============================" );

    return StatusCode::SUCCESS;
  }

  /// Regular Gaudi algorithm execute function
  StatusCode MUCTPI_AthAlg::execute()
  {
    ATH_MSG_DEBUG( "===============================" );
    ATH_MSG_DEBUG( "Executing MUCTPI_AthAlg" );
    ATH_MSG_DEBUG( "===============================" );

    CHECK( m_muctpiTool->execute() );

    return StatusCode::SUCCESS;
  }

}


