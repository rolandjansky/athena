/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
* @file MissingETD3PDMaker/src/MissingETTruthNonIntPhiFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET NonInt Truth objects.
 */


#include "MissingETD3PDMaker/MissingETTruthNonIntPhiFillerTool.h"
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
MissingETTruthNonIntPhiFillerTool::MissingETTruthNonIntPhiFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<MissingEtTruth> (type, name, parent)
  ,   mLog(msgSvc(), name )
{
  book().ignore(); // Avoid coverity warning.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETTruthNonIntPhiFillerTool::book()
{
 
  CHECK( addVariable ("NonInt_phi",    m_phi) );


  return StatusCode::SUCCESS;
}


StatusCode MissingETTruthNonIntPhiFillerTool::fill (const MissingEtTruth& p)
{

  float tempx = p.exTruth(MissingEtTruth::NonInt);
  float tempy = p.eyTruth(MissingEtTruth::NonInt);
 
  *m_phi   = atan2(tempy,tempx);

  mLog << MSG::DEBUG << "MissingETD3PDMaker::MissingETTruthNonIntPhiFillerTool - Non-Interacting Truth written to D3PD" << endreq;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
