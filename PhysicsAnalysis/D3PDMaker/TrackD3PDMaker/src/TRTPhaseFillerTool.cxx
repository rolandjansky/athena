/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTPhaseFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


TRTPhaseFillerTool::TRTPhaseFillerTool (const std::string& type,
					const std::string& name,
					const IInterface* parent)
  : D3PD::BlockFillerTool<ComTime> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}

StatusCode TRTPhaseFillerTool::initialize(){

  CHECK( Base::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TRTPhaseFillerTool::book(){

  CHECK( addVariable ("Phase", m_TRTPhase) );

  return StatusCode::SUCCESS;
}

StatusCode TRTPhaseFillerTool::fill (const ComTime& theComTime)
{

  this->clearData();

  *m_TRTPhase =  float(theComTime.getTime());

  return StatusCode::SUCCESS;
}

void TRTPhaseFillerTool::clearData(){

  *m_TRTPhase = 0;

}

} // namespace D3PD
