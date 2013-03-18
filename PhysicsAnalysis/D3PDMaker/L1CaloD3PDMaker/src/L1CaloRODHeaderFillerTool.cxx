/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloRODHeaderFillerTool.cxx
*/

#include "L1CaloRODHeaderFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloRODHeaderFillerTool::L1CaloRODHeaderFillerTool(  const std::string& type,
					     const std::string& name,
					     const IInterface* parent )
  : BlockFillerTool< LVL1::RODHeader >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloRODHeaderFillerTool::book(){

    CHECK( addVariable ("version",m_version) );
    CHECK( addVariable ("sourceId",m_sourceId) );
    CHECK( addVariable ("run",m_run) );
    CHECK( addVariable ("lvl1Id",m_lvl1Id) );
    CHECK( addVariable ("bcId",m_bcId) );
    CHECK( addVariable ("trigType",m_trigType) );
    CHECK( addVariable ("detType",m_detType) );
    CHECK( addVariable ("statusWords",m_statusWords) );
    CHECK( addVariable ("payloadSize",m_payloadSize) );

    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloRODHeaderFillerTool::fill( const LVL1::RODHeader& p ){

    *m_version = p.version();
    *m_sourceId = p.sourceID();
    *m_run = p.run();
    *m_lvl1Id = p.extendedL1ID();
    *m_bcId = p.bunchCrossing();
    *m_trigType = p.l1TriggerType();
    *m_detType = p.detEventType();
    *m_statusWords = p.statusWords();
    *m_payloadSize = p.payloadSize();

    return StatusCode::SUCCESS;
  }

}

    
