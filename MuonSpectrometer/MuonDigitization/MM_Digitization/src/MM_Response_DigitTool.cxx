/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_Response_DigitTool.h"

#include "MM_Digitization/MM_DigitToolInput.h"

#include <iostream>
#include <vector>

/*******************************************************************************/
MM_Response_DigitTool::MM_Response_DigitTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_rndmEngine(0),
  m_rndmEngineName("MuonDigitization"),
  m_rndmSvc("AtRndmGenSvc", name )
{
  declareInterface<IMM_DigitizationTool>(this);
  declareProperty("RndmSvc",    m_rndmSvc,         "Random Number Service used in Muon digitization" );
  declareProperty("RndmEngine", m_rndmEngineName,  "Random engine name");
}
/*******************************************************************************/
MM_DigitToolOutput MM_Response_DigitTool::digitize( /*const MmDigitToolInput& input*/ )
{
  std::vector<float> a, b;
  std::vector<int> c;
  MM_DigitToolOutput output(false, c, b, a, 1, 1);
  return output;
}
/*******************************************************************************/
StatusCode MM_Response_DigitTool::initialize()
{
  ATH_CHECK( m_rndmSvc.retrieve() );

  // getting our random numbers stream
  ATH_MSG_DEBUG("Getting random number engine : <" << m_rndmEngineName << ">");
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_FATAL("Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  }

  initializeStrip();

  return StatusCode::SUCCESS;
}
/*******************************************************************************/
bool MM_Response_DigitTool::initializeStrip(){
  return true;
}
/*******************************************************************************/

