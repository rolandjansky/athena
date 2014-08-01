/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/EFMuonFillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for TrigMuonEF
 */

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "EFMuonFillerTool.h"
#include "TrigMuonEvent/TrigMuonEF.h"
#include "AthenaKernel/errorcheck.h"



namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
EFMuonFillerTool::EFMuonFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<TrigMuonEF> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode EFMuonFillerTool::book()
{
  CHECK( addVariable ("muoncode",   m_muonCode) );
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
StatusCode EFMuonFillerTool::fill (const TrigMuonEF& p)
{
  *m_muonCode     = p.MuonCode();
  return StatusCode::SUCCESS;
}


} // namespace D3PD
