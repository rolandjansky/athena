/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MdtPrepDataFillerTool.cxx 535262 2013-01-29 13:54:17Z salekd $
#include "MdtPrepDataFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


MdtPrepDataFillerTool::MdtPrepDataFillerTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : BlockFillerTool<Muon::MdtPrepData> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


StatusCode MdtPrepDataFillerTool::book()
{
  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );
  CHECK( addVariable ("tdc", m_tdc) );
  CHECK( addVariable ("adc", m_adc) );
  CHECK( addVariable ("locX", m_locX) );
  CHECK( addVariable ("status", m_status) );
 
  return StatusCode::SUCCESS;
}


StatusCode MdtPrepDataFillerTool::fill (const Muon::MdtPrepData& obj)
{
  *m_x      = obj.globalPosition().x();
  *m_y      = obj.globalPosition().y();
  *m_z      = obj.globalPosition().z();
  *m_tdc    = obj.tdc();
  *m_adc    = obj.adc();
  *m_locX   = obj.localPosition().x();
  *m_status = obj.status();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
