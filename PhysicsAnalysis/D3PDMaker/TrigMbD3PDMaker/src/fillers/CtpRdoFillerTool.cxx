/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/CtpRdoFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TrigT1Result/CTP_RDO.h"

namespace TrigMbD3PD {

CtpRdoFillerTool::CtpRdoFillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : CtpRdoFillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode CtpRdoFillerTool::initialize()
{

  CHECK( CtpRdoFillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode CtpRdoFillerTool::book()
{

  CHECK( addVariable ("nBC", m_nBC) );
  CHECK( addVariable ("lvl1aBC", m_L1A_BC) );
  CHECK( addVariable ("time_s" ,m_timeSec) );
  CHECK( addVariable ("time_ns", m_timeNanoSec) );
  CHECK( addVariable ("pit", m_pit) );
  CHECK( addVariable ("tbp", m_tbp) );
  CHECK( addVariable ("tap", m_tap) );
  CHECK( addVariable ("tav", m_tav) );

  return StatusCode::SUCCESS;
}

StatusCode CtpRdoFillerTool::fill (const CtpRdoFillerObjectType& ctp)
{

  this->clearData();

  *m_nBC = ctp.getNumberOfBunches();
  *m_L1A_BC = ctp.getL1AcceptBunchPosition();
  *m_timeSec = ctp.getTimeSec();
  *m_timeNanoSec = ctp.getTimeNanoSec();   
  
  *m_pit = ctp.getPITWords();
  *m_tbp = ctp.getTBPWords();
  *m_tap = ctp.getTAPWords();
  *m_tav = ctp.getTAVWords();

  return StatusCode::SUCCESS;
}

void CtpRdoFillerTool::clearData(){

  *m_nBC = 999;
  *m_L1A_BC = 999;
  *m_timeSec = 999;
  *m_timeNanoSec = 999;

  m_pit->clear();
  m_tbp->clear();
  m_tap->clear();
  m_tav->clear();
}


} // namespace TrigMbD3PD
