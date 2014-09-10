/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackPIDFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2010
 * @brief Fill in @c getPID items from @c TrackSummary.
 */


#include "TrackPIDFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackPIDFillerTool::TrackPIDFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : BlockFillerTool<Trk::TrackSummary> (type, name, parent)
{
  declareProperty ("PID", m_pid);
}


/**
 * @brief Book variables for this block.
 */
StatusCode TrackPIDFillerTool::book()
{
  m_vars.resize (m_pid.size() / 2);
  for (size_t i = 0; i < m_vars.size(); ++i) {
    m_vars[i].key =
      static_cast<Trk::eProbabilityType>
      (std::atoi (m_pid[2*i].c_str()));

    std::string name = m_pid[2*i+1];
    std::string docname = name;
    std::string::size_type ipos = name.find (':');
    if (ipos != std::string::npos) {
      docname = name.substr (ipos+1, std::string::npos);
      name = name.substr (0, ipos);
    }
    std::string docstring = "TrackSummary::getPID(";
    docstring += docname;
    docstring += ")";
    CHECK( addVariable (name, m_vars[i].x, docstring) );
  }
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
StatusCode TrackPIDFillerTool::fill (const Trk::TrackSummary& p)
{
  for (size_t i = 0; i < m_vars.size(); i++) {
    Var& var = m_vars[i];
    *var.x = p.getPID(var.key);
    
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
