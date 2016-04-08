/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETTruthIntPhiFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Int truth objects.
 */


#include "MissingETD3PDMaker/MissingETTruthIntPhiFillerTool.h"
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
MissingETTruthIntPhiFillerTool::MissingETTruthIntPhiFillerTool (const std::string& type,
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
StatusCode MissingETTruthIntPhiFillerTool::book()
{
  //int
 
  CHECK( addVariable ("Int_phi",    m_int_phi) );
  
  
  //central
  
  CHECK( addVariable ("IntCentral_phi",    m_intcentral_phi) );
  
  //fwd
 
  CHECK( addVariable ("IntFwd_phi",    m_intfwd_phi) );
  
  //outcover
 
  CHECK( addVariable ("IntOutCover_phi",    m_intoutcover_phi) );

  //muons
  
  CHECK( addVariable ("IntMuons_phi",    m_muons_phi) );

  return StatusCode::SUCCESS;
}


StatusCode MissingETTruthIntPhiFillerTool::fill (const MissingEtTruth& p)
{
  float tempx = p.exTruth(MissingEtTruth::Int);
  float tempy = p.eyTruth(MissingEtTruth::Int);
 
  *m_int_phi   = atan2(tempy,tempx);

  tempx = p.exTruth(MissingEtTruth::IntCentral);
  tempy = p.eyTruth(MissingEtTruth::IntCentral);
  
  *m_intcentral_phi   = atan2(tempy,tempx);

  tempx = p.exTruth(MissingEtTruth::IntFwd);
  tempy = p.eyTruth(MissingEtTruth::IntFwd);

  *m_intfwd_phi   = atan2(tempy,tempx);
  
 tempx = p.exTruth(MissingEtTruth::IntOutCover);
  tempy = p.eyTruth(MissingEtTruth::IntOutCover);
 
  *m_intoutcover_phi   = atan2(tempy,tempx);

  tempx = p.exTruth(MissingEtTruth::Muons);
  tempy = p.eyTruth(MissingEtTruth::Muons);
 
  *m_muons_phi   = atan2(tempy,tempx);

  mLog << MSG::DEBUG << "MissingETD3PDMaker::MissingETTruthIntPhiFillerTool -- Interacting Truth Filled" << endreq;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
