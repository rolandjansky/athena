/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
* @file MissingETD3PDMaker/src/MissingETTruthNonIntFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET NonInt Truth objects.
 */


#include "MissingETD3PDMaker/MissingETTruthNonIntFillerTool.h"
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "AthenaKernel/errorcheck.h"
#include "MissingETEvent/MissingEtRegions.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETTruthNonIntFillerTool::MissingETTruthNonIntFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<MissingEtTruth> (type, name, parent)
{
  book().ignore(); // Avoid coverity warning.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETTruthNonIntFillerTool::book()
{
  CHECK( addVariable ("NonInt_etx",    m_etx) );
  CHECK( addVariable ("NonInt_ety",    m_ety) );
 

  return StatusCode::SUCCESS;
}


StatusCode MissingETTruthNonIntFillerTool::fill (const MissingEtTruth& p)
{
  *m_etx   = p.exTruth(MissingEtTruth::NonInt);
  *m_ety   = p.eyTruth(MissingEtTruth::NonInt);
  return StatusCode::SUCCESS;
}


} // namespace D3PD
