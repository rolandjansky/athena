/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMbD3PDMaker/src/fillers/BCM_RawDataFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2010
 * @brief Block filler tool for BCM_RawData objects.
 */


#include "TrigMbD3PDMaker/fillers/BCM_RawDataFillerTool.h"
#include "InDetBCM_RawData/BCM_RawData.h"
#include "AthenaKernel/errorcheck.h"


namespace TrigMbD3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
BCM_RawDataFillerTool::BCM_RawDataFillerTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : D3PD::BlockFillerTool<BCM_RawData> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode BCM_RawDataFillerTool::book()
{
  CHECK( addVariable ("channel", m_channel,
                      "BCM channel ID.") );
  CHECK( addVariable ("pulse1pos",   m_pulse1pos,
                      "Position of the first pulse.") );
  CHECK( addVariable ("pulse1width", m_pulse1width,
                      "Width of the first pulse.") );
  CHECK( addVariable ("pulse2pos",   m_pulse2pos,
                      "Position of the first pulse.") );
  CHECK( addVariable ("pulse2width", m_pulse2width,
                      "Width of the second pulse.") );
  CHECK( addVariable ("lvl1a",       m_lvl1a,
                      "Level-1 accept.") );
  CHECK( addVariable ("bcid",        m_bcid,
                      "Bunch crossing ID.") );
  CHECK( addVariable ("lvl1id",      m_lvl1id,
                      "Level 1 ID.") );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param bcm The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode BCM_RawDataFillerTool::fill (const BCM_RawData& bcm)
{
  *m_channel         = bcm.getChannel();
  *m_lvl1a           = bcm.getLVL1A();
  *m_bcid            = bcm.getBCID();
  *m_lvl1id          = bcm.getLVL1ID();
  *m_pulse1pos       = bcm.getPulse1Position();
  *m_pulse1width     = bcm.getPulse1Width();

  if (bcm.getPulse2Width()>0) {
    *m_pulse2pos   = bcm.getPulse2Position();
    *m_pulse2width = bcm.getPulse2Width();
  }
  
  return StatusCode::SUCCESS;
}


} // namespace TrigMbD3PD
