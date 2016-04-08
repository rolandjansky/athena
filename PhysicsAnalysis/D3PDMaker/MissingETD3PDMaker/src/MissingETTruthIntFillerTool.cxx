/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETTruthIntFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Int truth objects.
 */


#include "MissingETD3PDMaker/MissingETTruthIntFillerTool.h"
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
MissingETTruthIntFillerTool::MissingETTruthIntFillerTool (const std::string& type,
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
StatusCode MissingETTruthIntFillerTool::book()
{
  //int
  CHECK( addVariable ("Int_etx",    m_int_etx) );
  CHECK( addVariable ("Int_ety",    m_int_ety) );
  
  
  //central
  CHECK( addVariable ("IntCentral_etx",    m_intcentral_etx) );
  CHECK( addVariable ("IntCentral_ety",    m_intcentral_ety) );
  
  //fwd
  CHECK( addVariable ("IntFwd_etx",    m_intfwd_etx) );
  CHECK( addVariable ("IntFwd_ety",    m_intfwd_ety) );
  
  //outcover
  CHECK( addVariable ("IntOutCover_etx",    m_intoutcover_etx) );
  CHECK( addVariable ("IntOutCover_ety",    m_intoutcover_ety) );
 
  //muons
  CHECK( addVariable ("IntMuons_etx",    m_muons_etx) );
  CHECK( addVariable ("IntMuons_ety",    m_muons_ety) );
 
  return StatusCode::SUCCESS;
}


StatusCode MissingETTruthIntFillerTool::fill (const MissingEtTruth& p)
{
  float tempx = p.exTruth(MissingEtTruth::Int);
  float tempy = p.eyTruth(MissingEtTruth::Int);
  
  *m_int_etx   = tempx;
  *m_int_ety   = tempy;
 
  tempx = p.exTruth(MissingEtTruth::IntCentral);
  tempy = p.eyTruth(MissingEtTruth::IntCentral);
  
  *m_intcentral_etx   = tempx;
  *m_intcentral_ety   = tempy;
 
  tempx = p.exTruth(MissingEtTruth::IntFwd);
  tempy = p.eyTruth(MissingEtTruth::IntFwd);
  
  *m_intfwd_etx   = tempx;
  *m_intfwd_ety   = tempy;
  
 tempx = p.exTruth(MissingEtTruth::IntOutCover);
  tempy = p.eyTruth(MissingEtTruth::IntOutCover);
  
  *m_intoutcover_etx   = tempx;
  *m_intoutcover_ety   = tempy;
 
  tempx = p.exTruth(MissingEtTruth::Muons);
  tempy = p.eyTruth(MissingEtTruth::Muons);
  
  *m_muons_etx   = tempx;
  *m_muons_ety   = tempy;
 
  mLog << MSG::DEBUG << "MissingETD3PDMaker::MissingETTruthIntFillerTool -- Interacting Truth Filled" << endreq;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
