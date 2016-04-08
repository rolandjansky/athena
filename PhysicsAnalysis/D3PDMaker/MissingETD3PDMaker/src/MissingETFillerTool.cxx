/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for Missing ET objects.
 */


#include "MissingETD3PDMaker/MissingETFillerTool.h"
#include "MissingETEvent/MissingET.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETFillerTool::MissingETFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<MissingET> (type, name, parent)
  ,   mLog(msgSvc(), name )
{
  book().ignore(); // Avoid coverity warning.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETFillerTool::book()
{
  CHECK( addVariable ("etx",    m_etx) );
  CHECK( addVariable ("ety",    m_ety) );
  
  return StatusCode::SUCCESS;
}


StatusCode MissingETFillerTool::fill (const MissingET& p)
{
  *m_etx   = p.etx();
  *m_ety   = p.ety();
 

  mLog << MSG::DEBUG << "MissingETD3PDMaker::MissingETFillerTool -- MET filled" << endreq;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
