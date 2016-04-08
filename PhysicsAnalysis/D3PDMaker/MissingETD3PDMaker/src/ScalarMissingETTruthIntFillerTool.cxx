/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/ScalarMissingETTruthIntFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Int truth objects.
 */


#include "MissingETD3PDMaker/ScalarMissingETTruthIntFillerTool.h"
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
ScalarMissingETTruthIntFillerTool::ScalarMissingETTruthIntFillerTool (const std::string& type,
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
StatusCode ScalarMissingETTruthIntFillerTool::book()
{
  //int
 
  CHECK( addVariable ("Int_et",     m_int_et) );
 
  
  //central
  
  CHECK( addVariable ("IntCentral_et",     m_intcentral_et) );
 
  //fwd
 
  CHECK( addVariable ("IntFwd_et",     m_intfwd_et) );
  
  //outcover
 
  CHECK( addVariable ("IntOutCover_et",     m_intoutcover_et) );
 
  //muons
 
  CHECK( addVariable ("IntMuons_et",     m_muons_et) );
 
  return StatusCode::SUCCESS;
}


StatusCode ScalarMissingETTruthIntFillerTool::fill (const MissingEtTruth& p)
{
  float tempx = p.exTruth(MissingEtTruth::Int);
  float tempy = p.eyTruth(MissingEtTruth::Int);
 
  *m_int_et    = sqrt(pow(tempx, 2) + pow(tempy, 2));
  

  tempx = p.exTruth(MissingEtTruth::IntCentral);
  tempy = p.eyTruth(MissingEtTruth::IntCentral);
 
  *m_intcentral_et    = sqrt(pow(tempx, 2) + pow(tempy, 2));
  
  tempx = p.exTruth(MissingEtTruth::IntFwd);
  tempy = p.eyTruth(MissingEtTruth::IntFwd);
  
  *m_intfwd_et    = sqrt(pow(tempx, 2) + pow(tempy, 2));
  
  
 tempx = p.exTruth(MissingEtTruth::IntOutCover);
  tempy = p.eyTruth(MissingEtTruth::IntOutCover);
  
  *m_intoutcover_et    = sqrt(pow(tempx, 2) + pow(tempy, 2));
 

  tempx = p.exTruth(MissingEtTruth::Muons);
  tempy = p.eyTruth(MissingEtTruth::Muons);
 
  *m_muons_et    = sqrt(pow(tempx, 2) + pow(tempy, 2));
  
  mLog << MSG::DEBUG << "MissingETD3PDMaker::ScalarMissingETTruthIntFillerTool -- Interacting Truth Filled" << endreq;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
