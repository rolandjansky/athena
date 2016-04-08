/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/SumETFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for Missing ET objects.
 */


#include "MissingETD3PDMaker/SumETFillerTool.h"
#include "MissingETEvent/MissingET.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
SumETFillerTool::SumETFillerTool (const std::string& type,
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
StatusCode SumETFillerTool::book()
{
  
  CHECK( addVariable ("sumet",  m_sumet) );
  
  return StatusCode::SUCCESS;
}


StatusCode SumETFillerTool::fill (const MissingET& p)
{
  
  *m_sumet = p.sumet();
 

  mLog << MSG::DEBUG << "MissingETD3PDMaker::SumETFillerTool -- MET filled" << endreq;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
