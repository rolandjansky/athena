/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/MbtsContainerFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TileEvent/TileContainer.h"
#include "TileIdentifier/TileTBID.h"

#include "StoreGate/StoreGateSvc.h"

namespace TrigMbD3PD {

MbtsContainerFillerTool::MbtsContainerFillerTool (const std::string& type,
						  const std::string& name,
						  const IInterface* parent)
  : MbtsContainerFillerBase (type, name, parent),
    m_tileTBID(0)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode MbtsContainerFillerTool::initialize(){

  CHECK( MbtsContainerFillerBase::initialize() );

  // Get the test beam identifier for the MBTS
  CHECK( detStore()->retrieve(m_tileTBID) );
  return StatusCode::SUCCESS;
}

StatusCode MbtsContainerFillerTool::book()
{

  CHECK( addVariable ("energy", m_energy) );
  CHECK( addVariable ("time", m_time) );
  CHECK( addVariable ("quality", m_quality) );
  CHECK( addVariable ("type", m_type) );
  CHECK( addVariable ("module", m_module) );
  CHECK( addVariable ("channel", m_channel) );

  return StatusCode::SUCCESS;
}

StatusCode MbtsContainerFillerTool::fill (const MbtsContainerFillerObjectType& cell)
{

  this->clearData();

  *m_energy = cell.energy();
  *m_time = cell.time();
  *m_quality = cell.quality();
 
  Identifier id = cell.ID();
  *m_type = m_tileTBID->type(id);
  *m_channel = m_tileTBID->channel(id);
  *m_module = m_tileTBID->module(id);

  return StatusCode::SUCCESS;
}

void MbtsContainerFillerTool::clearData(){

  *m_energy = 0;
  *m_time = 0;
  *m_quality = 0;
  *m_type = 0;
  *m_module = 0;
  *m_channel = 0;
}

} // namespace TrigMbD3PD
