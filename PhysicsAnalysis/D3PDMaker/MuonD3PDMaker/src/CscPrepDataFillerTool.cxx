/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CscPrepDataFillerTool.cxx 535262 2013-01-29 13:54:17Z salekd $
#include "CscPrepDataFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


CscPrepDataFillerTool::CscPrepDataFillerTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : BlockFillerTool<Muon::CscPrepData> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


StatusCode CscPrepDataFillerTool::book()
{
  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );
  CHECK( addVariable ("time", m_time) );
  CHECK( addVariable ("locX", m_locX) );
  CHECK( addVariable ("status", m_status) );
  CHECK( addVariable ("timeStatus", m_timeStatus) );
  CHECK( addVariable ("charge", m_charge) );
 
  return StatusCode::SUCCESS;
}


StatusCode CscPrepDataFillerTool::fill (const Muon::CscPrepData& obj)
{
  *m_x      = obj.globalPosition().x();
  *m_y      = obj.globalPosition().y();
  *m_z      = obj.globalPosition().z();
  *m_time   = obj.time();
  *m_locX   = obj.localPosition().x();
  *m_status = obj.status();
  *m_timeStatus = obj.timeStatus();
  *m_charge = obj.charge();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
