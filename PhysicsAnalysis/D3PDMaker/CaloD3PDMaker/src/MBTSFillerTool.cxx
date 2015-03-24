/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/MBTSFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for MBTS information.
 */


#include "MBTSFillerTool.h"
#include "TileEvent/TileCell.h"
#include "TileIdentifier/TileTBID.h"
#include "AthenaKernel/errorcheck.h"
#include "math.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MBTSFillerTool::MBTSFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<TileCell> (type, name, parent)
{
  declareProperty("SaveEtaPhiInfo", m_saveEtaPhi = true );
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode MBTSFillerTool::book()
{
  CHECK( addVariable ("E", m_E, "MBTS counter energy.") );
  
  if (m_saveEtaPhi) {
    CHECK( addVariable ("eta", m_eta, "MBTS counter eta.") );
    CHECK( addVariable ("phi", m_phi, "MBTS counter phi.") );
  }

  CHECK( addVariable ("time", m_time, "MBTS counter time.") );
  CHECK( addVariable ("quality", m_quality, "MBTS counter quality.") );
  CHECK( addVariable ("type", m_type, "MBTS counter type.  +-1, depending on side.") );
  CHECK( addVariable ("module", m_module, "MBTS counter module.  0-7, depending on phi.") );
  CHECK( addVariable ("channel", m_channel, "MBTS counter channel.  0-1, depending on eta.  " "Zero is closer to the beam pipe.") );
 
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode MBTSFillerTool::fill (const TileCell& c) {
  const TileTBID* tileid = 0;
  CHECK( detStore()->retrieve (tileid) );

  *m_E = c.energy();
  
  if (m_saveEtaPhi) {
    *m_eta = c.eta();
    *m_phi = c.phi();
  }

  *m_time = c.time();
  *m_quality = c.quality();

  Identifier id = c.ID();
  int module = tileid->module(id);

  *m_type = tileid->type(id);
  *m_module = module;
  *m_channel = tileid->channel(id);

  return StatusCode::SUCCESS;
}


} // namespace D3PD
