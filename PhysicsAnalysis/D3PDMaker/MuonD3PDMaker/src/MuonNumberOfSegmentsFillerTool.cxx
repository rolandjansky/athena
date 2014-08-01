/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonD3PDMaker/src/MuonNumberOfSegmentsFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Fill number of muon segments.
 */


#include "MuonNumberOfSegmentsFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MuonNumberOfSegmentsFillerTool::MuonNumberOfSegmentsFillerTool (const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent)
  : BlockFillerTool<xAOD::Muon> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MuonNumberOfSegmentsFillerTool::book()
{
  CHECK( addVariable ("numberOfSegments",  m_numberOfSegments, 
                      "Number of segments.") );
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
StatusCode MuonNumberOfSegmentsFillerTool::fill (const xAOD::Muon& p)
{
  *m_numberOfSegments = p.nMuonSegments();
  return StatusCode::SUCCESS;
}


} // namespace D3PD
