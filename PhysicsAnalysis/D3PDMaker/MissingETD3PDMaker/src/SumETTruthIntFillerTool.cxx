/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/SumETTruthIntFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Int truth objects.
 */


#include "MissingETD3PDMaker/SumETTruthIntFillerTool.h"
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
SumETTruthIntFillerTool::SumETTruthIntFillerTool (const std::string& type,
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
StatusCode SumETTruthIntFillerTool::book()
{
  //int
 
  CHECK( addVariable ("Int_sumet",  m_int_sumet) );
 
  
  
  //central
  
  CHECK( addVariable ("IntCentral_sumet",  m_intcentral_sumet) );
 
  //fwd
  
  CHECK( addVariable ("IntFwd_sumet",  m_intfwd_sumet) );
 
  //outcover
  
  CHECK( addVariable ("IntOutCover_sumet",  m_intoutcover_sumet) );
  
  //muons
  
  CHECK( addVariable ("IntMuons_sumet",  m_muons_sumet) );
  
  return StatusCode::SUCCESS;
}


StatusCode SumETTruthIntFillerTool::fill (const MissingEtTruth& p)
{
  
  float tempsum = p.etSumTruth(MissingEtTruth::Int);
 
  *m_int_sumet = tempsum;
 
  tempsum = p.etSumTruth(MissingEtTruth::IntCentral);
  
  *m_intcentral_sumet = tempsum;
 
  tempsum = p.etSumTruth(MissingEtTruth::IntFwd);
 
  *m_intfwd_sumet = tempsum;
  
  tempsum = p.etSumTruth(MissingEtTruth::IntOutCover);
  
  *m_intoutcover_sumet = tempsum;
  
  tempsum = p.etSumTruth(MissingEtTruth::Muons);
 
  *m_muons_sumet = tempsum;
  

  mLog << MSG::DEBUG << "MissingETD3PDMaker::SumETTruthIntFillerTool -- Interacting Truth Filled" << endreq;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
